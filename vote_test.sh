#!/bin/bash
# Function: do dpos vote and unVote test
# Usage: bash vote_test.sh &

# Set voter account number
config_file=./test.cfg

while true; do
	num=$(head -n 1 $config_file | awk -F'=' '{print $2}')
	python ./dpos_test.py -c testVote -p voter_num=$num &>>/tmp/stress.log;
	[[ $? -ne 0 ]] && sleep 30 || sleep 11
	python ./dpos_test.py -c testUnVote -p "voter_num=5,candidate=1" &>>/tmp/stress.log;
	[[ $? -ne 0 ]] && sleep 30 || sleep 11
	hour=$(date +%H)
	if [[ "$hour" == "23" ]]; then
		df -h &>> /tmp/stress.log
	fi
done
