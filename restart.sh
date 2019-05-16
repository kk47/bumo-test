#!/bin/bash

cmd="$1"

if [[ "$cmd" == "stop" ]]; then
    pid=$(ps aux |grep dpos.py |grep -v grep |awk '{print $2}')
    kill -9 $pid
elif [[ "$cmd" == "start" ]]; then
    python ./dpos.py
else
    pid=$(ps aux |grep dpos.py |grep -v grep |awk '{print $2}')
    kill -9 $pid
    python ./dpos.py
fi
