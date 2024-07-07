package tcp

import (
	"bufio"
	"fmt"
	"log"
	"net"
	"strings"
	"sync"

	messages "github.com/ryepropgroup/protoServer/protos/messages"
	"google.golang.org/protobuf/encoding/protojson"
)

type TCPServer struct {
	mu          sync.Mutex
	clients     map[net.Conn]struct{}
	commandChan chan *messages.SequenceCommand
	statusChan  chan *messages.SensorData
}

func NewTCPServer(commandChan chan *messages.SequenceCommand, statusChan chan *messages.SensorData) *TCPServer {
	server := &TCPServer{
		clients:     make(map[net.Conn]struct{}),
		commandChan: commandChan,
		statusChan:  statusChan,
	}
	go server.broadcastStatusUpdates()
	return server
}

func (s *TCPServer) handleConnection(conn net.Conn) {
	defer func() {
		s.mu.Lock()
		delete(s.clients, conn)
		s.mu.Unlock()
		conn.Close()
		log.Println("TCP client disconnected")
	}()
	s.mu.Lock()
	s.clients[conn] = struct{}{}
	s.mu.Unlock()

	reader := bufio.NewReader(conn)
	for {
		msg, err := reader.ReadString('\n')
		if err != nil {
			log.Printf("Failed to read from connection: %v", err)
			break
		}
		msg = strings.TrimSpace(msg)
		fmt.Println(msg)

		cmd := &messages.SequenceCommand{
			Sequence: msg,
		}

		// Send command to gRPC server
		s.commandChan <- cmd
	}

	s.mu.Lock()
	delete(s.clients, conn)
	s.mu.Unlock()
}

func (s *TCPServer) startListening(addr string) {
	ln, err := net.Listen("tcp", addr)
	if err != nil {
		log.Fatalf("Failed to listen on %s: %v", addr, err)
	}
	log.Printf("TCP server listening on %s", addr)

	for {
		conn, err := ln.Accept()
		if err != nil {
			log.Printf("Failed to accept connection: %v", err)
			continue
		}
		go s.handleConnection(conn)
	}
}

func (s *TCPServer) broadcastStatusUpdates() {
	for update := range s.statusChan {
		jsonData, err := protojson.Marshal(update)
		fmt.Println(jsonData)
		if err != nil {
			log.Printf("Failed to marshal status update: %v", err)
			continue
		}

		s.mu.Lock()
		for conn := range s.clients {
			_, err := conn.Write(append(jsonData, '\n'))
			if err != nil {
				log.Printf("Failed to send status update: %v", err)
			}
		}
		s.mu.Unlock()
	}
}

func StartTCPServer(commandChan chan *messages.SequenceCommand, statusChan chan *messages.SensorData) {
	server := NewTCPServer(commandChan, statusChan)
	server.startListening(":6000")
}
