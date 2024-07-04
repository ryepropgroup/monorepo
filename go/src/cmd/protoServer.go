package main

import (
	"github.com/ryepropgroup/protoServer/protos/messages"
	"github.com/ryepropgroup/protoServer/rpcService"
	"github.com/ryepropgroup/protoServer/tcp"
)

func main() {
	commandChan := make(chan *messages.SequenceCommand)
	statusChan := make(chan *messages.SensorData)

	go tcp.StartTCPServer(commandChan, statusChan)

	rpcService.StartGRPCServer(commandChan, statusChan)

}
