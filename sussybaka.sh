#!/bin/bash

trap killgroup SIGINT

killgroup() {
    echo "MACH: Killing all engine computer processes!"
    kill 0
}

cd go/src/ && go run cmd/protoServer.go &
sleep 1
./engine_computer.exe &
sleep 1
#cd gui && source .venv/bin/activate && python main.py &
wait
