#!/bin/bash
# Function: do dpos vote and unVote test
# Usage: bash vote_test.sh &

# Set voter account number
config_file=./test.cfg
log_file=./vote_test.log

while true; do
	v_num=$(grep '^VOTER_NUM=' $config_file | awk -F'=' '{print $2}')
	uv_num=$(grep '^UNVOTE_NUM=' $config_file | awk -F'=' '{print $2}')
	uv_cand=$(grep '^UNVOTE_CANDIDATES=' $config_file | awk -F'=' '{print $2}')

	python ./dpos_test.py -c testVote -p voter_num=$v_num &>>$log_file
	[[ $? -ne 0 ]] && sleep 30 || sleep 11
	python ./dpos_test.py -c testUnVote -p "voter_num=$uv_num,candidate=$uv_cand" &>>$log_file
	[[ $? -ne 0 ]] && sleep 30 || sleep 11

	hour=$(date +%H)
	if [[ "$hour" == "23" ]]; then
		df -h &>> /tmp/stress.log
	fi
done
