#!/bin/bash

trap killgroup SIGINT
trap killgroup SIGTSTP

killgroup() {
    echo "MACH: Killing all gui processes!"
    kill 0
}

cd gui
if [ "$(uname)" == "Linux" ]; then
    if [ ! -d ".venv" ]; then
        python3 -m venv .venv
    fi
    source .venv/bin/activate
    pip install -r requirements.txt
fi
python main.py "$@" &
wait
