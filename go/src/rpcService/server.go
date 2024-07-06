package rpcService

import (
	"context"
	"encoding/csv"
	"fmt"
	"io"
	"log"
	"net"
	"os"
	"path/filepath"
	"sort"
	"time"

	service "github.com/ryepropgroup/protoServer/protos"
	messages "github.com/ryepropgroup/protoServer/protos/messages"
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
}

func getMostRecentSensorData(prefix string) (string, error) {
	files, err := filepath.Glob(prefix + "*.csv")
	if err != nil {
		return "", err
	}

	if len(files) == 0 {
		return "", nil
	}

	var mostRecentFile string
	var mostRecentTime time.Time

	for _, file := range files {
		finfo, err := os.Stat(file)
		if err != nil {
			return "", err
		}

		if mostRecentFile == "" || finfo.ModTime().After(mostRecentTime) {
			mostRecentFile = file
			mostRecentTime = finfo.ModTime()
		}
	}

	return mostRecentFile, nil
}

func NewServer(commandChan chan *messages.SequenceCommand, statusChan chan *messages.SensorData) *server {
	return &server{
		commandChan: commandChan,
		statusChan:  statusChan,
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
	headers := []string{"ts"}
	headers = append(headers, s.sensorOrder...)

	cTime := time.Now()
	fName := fmt.Sprintf("sensor_data_%s.csv", cTime.Format("2006-01-02_15-04-05"))

	file, err := os.Create(fName)
	if err != nil {
		return err
	}
	defer file.Close()
	writer := csv.NewWriter(file)
	s.currentFileName = fName
	return writer.Write(headers)
}

func (s *server) SensorDataStream(stream service.EngineComputer_SensorDataStreamServer) error {
	if s.currentFileName == "" {
		fName, err := getMostRecentSensorData("sensor_data_")
		if err != nil {
			return err
		}
		if fName == "" {
			return fmt.Errorf("no sensor data location")
		}
		s.currentFileName = fName
	}
	file, err := os.OpenFile(s.currentFileName, os.O_APPEND|os.O_WRONLY, 0644)
	if err != nil {
		return err
	}
	defer file.Close()

	writer := csv.NewWriter(file)

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
		fmt.Println(data)
		// write Data
		row := []string{fmt.Sprintf("%d", data.Timestamp)}
		for _, name := range s.sensorOrder {
			row = append(row, fmt.Sprintf("%f", data.Values[name]))
		}

		if err := writer.Write(row); err != nil {
			return err
		}
		writer.Flush()
		// Forward sensor data to TCP clients
		s.statusChan <- data
	}
}

func (s *server) CommandStream(info *messages.DeviceInformation, stream service.EngineComputer_CommandStreamServer) error {
	s.deviceInfo = info
	s.updateSensorOrder()
	s.createCSVFile()

	for cmd := range s.commandChan {
		if err := stream.Send(cmd); err != nil {
			log.Printf("Error sending command: %v", err)
			return err
		}
	}
	return nil
}

func StartGRPCServer(commandChan chan *messages.SequenceCommand, statusChan chan *messages.SensorData) {
	lis, err := net.Listen("tcp", ":50051")
	if err != nil {
		log.Fatalf("Failed to listen: %v", err)
	}
	grpcServer := grpc.NewServer()
	service.RegisterEngineComputerServer(grpcServer, NewServer(commandChan, statusChan))
	reflection.Register(grpcServer)

	log.Println("gRPC server listening on :50051")
	if err := grpcServer.Serve(lis); err != nil {
		log.Fatalf("Failed to serve: %v", err)
	}
}
