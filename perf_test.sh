#!/bin/bash
# Funciton: run stress test of paycoin
# Usage: bash perf_test.sh

prog=./dpos_test.py
logfile=./stress.log
tps10=20000

while true; do
	#sleep 20
	rm -f ./keypairs
	sleep 10
	res=$($prog -c testCreateAccount -n $tps10 -o 1)
	echo "$prog -c testCreateAccount -n $tps10 -o 1"
	if [[ "$res" =~ "done, $tps10 succeed" ]]; then
		echo "CreateAccount done, $res" >> $logfile
		sleep 10
	else
		echo "CreateAccount failed, $res" >> $logfile
		sleep 10
		continue
	fi
	for i in `seq 1 20`; do
		sleep 10
		res1=$($prog -c testPayCoin -n $tps10 -o 1 -s $i)
		echo "$prog -c testPayCoin -n $tps10 -o 1 -s $i"
		if [[ "$res1" =~ "done, $tps10 tx succeed" ]]; then
			echo "PayCoin done $i, $res1" >> $logfile
		else
			echo "PayCoin failed $i, $res1" >> $logfile
			sleep 20
			break
		fi
	done
done
