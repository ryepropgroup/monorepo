package rpcService

import (
	"context"
	"encoding/csv"
	"fmt"
	"io"
	"log"
	"net"
	"os"
	"sort"
	"strings"
	"sync"
	"time"

	service "github.com/ryepropgroup/protoServer/protos"
	messages "github.com/ryepropgroup/protoServer/protos/messages"
	"github.com/ryepropgroup/protoServer/state"
	"google.golang.org/grpc"
	"google.golang.org/grpc/reflection"
)

type server struct {
	service.UnimplementedEngineComputerServer
	commandChan     chan *messages.SequenceCommand
	statusChan      chan *messages.SensorData
	deviceInfo      *messages.DeviceInformation
	currentFileName string
	sensorOrder     []string
	mu              sync.Mutex
	isFileReady     bool
	file            *os.File
	csvWriter       *csv.Writer
	state           *state.SharedState
}

func (s *server) writeData(file *os.File, data *messages.SensorData) error {
	writer := csv.NewWriter(file)
	row := []string{fmt.Sprintf("%d", data.Timestamp)}
	for _, name := range s.sensorOrder {
		row = append(row, fmt.Sprintf("%f", data.Values[name]))
	}
	if err := writer.Write(row); err != nil {
		return err
	}
	writer.Flush()
	return nil
}

func NewServer(commandChan chan *messages.SequenceCommand, statusChan chan *messages.SensorData,
	state *state.SharedState) *server {
	return &server{
		commandChan: commandChan,
		statusChan:  statusChan,
		isFileReady: false,
		state:       state,
	}
}

func (s *server) updateSensorOrder() {
	s.sensorOrder = make([]string, 0, len(s.deviceInfo.Devices))
	for name, device := range s.deviceInfo.Devices {
		if device.Type == messages.Device_SENSOR {
			s.sensorOrder = append(s.sensorOrder, name)
		}
	}
	sort.Strings(s.sensorOrder)
}

func (s *server) createCSVFile() error {
	s.closeCSVFile()
	headers := []string{"timestamp"}
	headers = append(headers, s.sensorOrder...)

	cTime := time.Now()
	fName := fmt.Sprintf("../../data/sensor_data_%s.csv", cTime.Format("2006-01-02_15-04-05"))

	// Create data directory if it doesn't exist.
	if _, err := os.Stat("../../data"); os.IsNotExist(err) {
		os.Mkdir("../../data", 0755)
	}

	fmt.Println("MACH: Creating new file:", fName)
	file, err := os.OpenFile(fName, os.O_APPEND|os.O_CREATE|os.O_WRONLY, 0644)
	if err != nil {
		return err
	}

	s.csvWriter = csv.NewWriter(file)
	s.currentFileName = fName
	s.mu.Lock()
	defer s.mu.Unlock()
	s.file = file
	s.isFileReady = true
	s.state.StateMutex.Lock()
	s.state.WritingData = true
	s.state.StateMutex.Unlock()
	err = s.csvWriter.Write(headers)
	if err != nil {
		return err
	}
	s.csvWriter.Flush()
	return nil
}

func (s *server) closeCSVFile() error {
	s.mu.Lock()
	defer s.mu.Unlock()
	if !s.isFileReady {
		return nil
	}
	println("MACH: Closing file:", s.currentFileName)
	s.isFileReady = false
	s.state.StateMutex.Lock()
	s.state.WritingData = true
	s.state.StateMutex.Unlock()
	s.currentFileName = ""
	s.file.Close()
	return nil
}

func (s *server) SensorDataStream(stream service.EngineComputer_SensorDataStreamServer) error {
	for {
		data, err := stream.Recv()
		if err == io.EOF {
			log.Println("SensorDataStream: client disconnected")
			s.currentFileName = ""
			return nil
		}
		if err != nil {
			if err == context.Canceled {
				s.currentFileName = ""
				return nil
			}
			log.Printf("Error: %v", err)
			s.currentFileName = ""
			return err
		}
		// fmt.Println(data)
		// write Data

		// Forward sensor data to TCP clients
		s.statusChan <- data

		s.mu.Lock()
		if !s.isFileReady {
			s.mu.Unlock()
			continue
		}
		s.mu.Unlock()

		if err := s.writeData(s.file, data); err != nil {
			fmt.Println("MACH: Error writing data to file, closing file!")
			s.closeCSVFile()
		}
	}
}

func (s *server) CommandStream(info *messages.DeviceInformation, stream service.EngineComputer_CommandStreamServer) error {
	s.deviceInfo = info
	s.updateSensorOrder()

	// TODO: Add different types of commands?
	// i.e. if cmd.Command == messages.Command_STOP...
	for cmd := range s.commandChan {
		if strings.HasPrefix(cmd.Sequence, "go:") {
			s.HandleGoCommand(cmd.Sequence)
			continue
		}

		if err := stream.Send(cmd); err != nil {
			log.Printf("Error sending command: %v", err)
			return err
		}
	}
	return nil
}

func (s *server) HandleGoCommand(command string) {
	if command == "go:start_data" {
		fmt.Println("MACH: Starting data collection.")
		s.createCSVFile()
	} else if command == "go:stop_data" {
		fmt.Println("MACH: Stopping data collection.")
		s.closeCSVFile()
	} else {
		log.Printf("Unknown go command: %s", command)
	}
}

func StartGRPCServer(commandChan chan *messages.SequenceCommand, statusChan chan *messages.SensorData,
	state *state.SharedState) {
	lis, err := net.Listen("tcp", ":50051")
	if err != nil {
		log.Fatalf("Failed to listen: %v", err)
	}
	grpcServer := grpc.NewServer()
	service.RegisterEngineComputerServer(grpcServer, NewServer(commandChan, statusChan, state))
	reflection.Register(grpcServer)

	log.Println("gRPC server listening on :50051")
	if err := grpcServer.Serve(lis); err != nil {
		log.Fatalf("Failed to serve: %v", err)
	}
}
