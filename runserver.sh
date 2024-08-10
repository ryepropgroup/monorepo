#!/bin/bash

trap killgroup SIGINT
trap killgroup SIGTSTP

killgroup() {
    echo "MACH: Killing all engine computer processes!"
    kill 0
}

cd go/src/ && go run cmd/protoServer.go "$@" &
sleep 1
./engine_computer.exe "$@" &
wait
