package main

import (
	"sync"

	"github.com/ryepropgroup/protoServer/protos/messages"
	"github.com/ryepropgroup/protoServer/rpcService"
	"github.com/ryepropgroup/protoServer/tcp"

	"github.com/ryepropgroup/protoServer/state"
)

func main() {
	commandChan := make(chan *messages.SequenceCommand)
	statusChan := make(chan *messages.SensorData)
	sharedState := &state.SharedState{
		WritingData: false,
		StateMutex:  sync.Mutex{},
	}

	go tcp.StartTCPServer(commandChan, statusChan, sharedState)

	rpcService.StartGRPCServer(commandChan, statusChan, sharedState)

}
