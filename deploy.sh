#!/bin/bash
# Function: deploy bumo program on multual-trust node like bm01-bm03

workdir=/root/dpos
build_dir=$workdir/bumo
bumo=$workdir/bchain
keypair_prog=$workdir/model/dpos_test.py
hosts="bm01 bm02 bm03 bm04 bm05 bm06 bm07 bm08 bm09 bm10 bm11 bm12 bm13 bm14 bm15"
instance_per_node=2 # max number is 20, listen port from 36 01 1 - 36 20 1
start_idx=1
init_validator_num=4
dposd=$workdir/dposd

function usage
{
	cat << EOF
Usage:
	Options:
		-h	show help message
		-c	command mode
			  deploy:	Deploy bumo to all nodes
			  mod:		Modify and update configuration
			  init:		Re-init and start bumo program
			  clean:	Clean test environment
			  stop:		stop all bumo instance 	
			  start:	start all bumo instance 	
			  clean:	clean all log and coredump files 	
			  drop:		drop db	for bumo instance
			  remove:	remove bumo instance	
		-d	bumo install directory, new instance will copy from here
		-i	bumo instance per node
		-l	node list to deploy
		-g	keypairs generation program, dpos_test.py
		-b	bumo build directory, binary file bin/bumo will update from here

	Prepare:
		Build ssh mutual-trust for all nodes and then download and compile bumo program.
	
	Getting Started:
	1. $0 -c deploy -d /root/kk/bchain -i 5 -l "bm01 bm02 bm03"
	2. $0 -c mod -d /root/kk/bchain -i 5 -l "bm01 bm02 bm03" -g "/root/kk/dpos_test.py"
	3. $0 -c init -d /root/kk/bchain -i 5 -l "bm01 bm02 bm03" -b "/root/kk/bumo-develop"

	
EOF
	exit 0
}

# Prepare bumo program
function deploy_prog
{
	for h in $hosts; do
		for i in `seq -f %02g ${start_idx} $instance_per_node`; do
			ssh ${h} "test -d ${bumo}${i}"
			if [[ $? -ne 0 ]]; then
				scp -r $bumo ${h}:${bumo}${i}
				
				# update init scripts
				cp $bumo/scripts/bumo /tmp/bumo
				sed -i "/install_dir=/c install_dir=${bumo}${i}" /tmp/bumo
				sed -i "/script_dir=/c script_dir=${bumo}${i}/scripts" /tmp/bumo
				scp /tmp/bumo ${h}:${bumo}${i}/scripts/bumo
				
				cp $bumo/scripts/bumod /tmp/bumod
				sed -i "/install_dir=/c install_dir=${bumo}${i}" /tmp/bumod
				scp /tmp/bumod ${h}:${bumo}${i}/scripts/bumod
			else
				scp $bumo/bin/bumo ${h}:${bumo}${i}/bin/bumo
			fi
		done
	done
}

# Modify bumo.json
function modify_cfg
{
	# generate keypairs for validator address
	if [[ ! -f ${keypair_prog} ]]; then
		echo "No such file $keypair_prog"
		exit 1
	fi
	
	init_validators=""
	seed_host=""
	let cnt=0
	[ -f ./keypairs ] && rm -f ./keypairs
	for h in $hosts; do
	    tmp_keypairs="/tmp/keypairs-${h}"
	    if [[ ! -f /tmp/keypairs-${h} ]]; then
		python $keypair_prog -u seed1.bumo.io:16002 -c genKeyPairs -n $instance_per_node
		mv ./keypairs /tmp/keypairs-${h}
	    fi
	    if [[ -z "$seed_host" ]]; then
		head -n 1 /tmp/keypairs-${h} > /tmp/init_validator
		seed_host="\"${h}:36011\""
	    else
		head -n 1 /tmp/keypairs-${h} >> /tmp/init_validator
		seed_host="${seed_host}, \"${h}:36011\""
	    fi
	    let cnt+=1
	done
	init_validators=$(awk '{print $6}' /tmp/init_validator | head -n $init_validator_num | tr -d "}" | tr "\n" ",")
	init_validators="${init_validators%,}"
	for h in $hosts; do
		tmp_keypairs="/tmp/keypairs-${h}"

		for i in `seq -f %02g 1 $instance_per_node`; do
			tmp_cfg="/tmp/bumo-${h}-${i}.json"
			cp $bumo/config/bumo.json $tmp_cfg
			listen_port="36${i}1"
			web_addr="0.0.0.0:36${i}2"
			ws_addr="0.0.0.0:36${i}3"
			sed -i "/\"listen_port\":/c \"listen_port\": ${listen_port}," $tmp_cfg
			sed -i "/\"listen_addresses\":/c \"listen_addresses\": \"${web_addr}\"," $tmp_cfg 
			sed -i "/\"listen_address\":/c \"listen_address\": \"${ws_addr}\"," $tmp_cfg
			#peers=$(seq -f "\"${seed_host}:36%02g1\"" 1 $instance_per_node | tr "\n" ",")
			sed -i "/\"known_peers\":/c \"known_peers\": [${seed_host}]" $tmp_cfg

			addr=$(sed -n "${i}p" $tmp_keypairs |awk -F: '{print $4}'|cut -d\" -f2)
			aes_priv=$(sed -n "${i}p" $tmp_keypairs |awk -F: '{print $3}'|cut -d\" -f2)
			sed -i "/\"validation_address\":/c \"validation_address\": \"${addr}\"," $tmp_cfg
			sed -i "/\"validation_private_key\":/c \"validation_private_key\": \"${aes_priv}\"," $tmp_cfg
			sed -i "/\"validators\":/c \"validators\": [${init_validators}]" $tmp_cfg
			scp $tmp_cfg ${h}:${bumo}${i}/config/bumo.json
		done
	done
}

# Re-deploy and then restart bumo program

function kill_prog
{
	# Stop
	for h in $hosts; do
		for i in `seq -f %02g 1 $instance_per_node`; do
			ssh $h "bash ${bumo}${i}/scripts/bumod stop"
			[ $? -ne 0 ] && echo "Stop failed" || echo "Stopped"
		done
		sleep 2
		ssh $h "ps aux |grep -v grep |grep bumo &>/dev/null"
		if [ $? -eq 0 ]; then
		    bumod_pids=$(ssh $h "pidof bumod")
		    ssh $h "kill -9 $bumod_pids"
		    bumo_pids=$(ssh $h "pidof bumo")
		    ssh $h "kill -9 $bumo_pids"
		fi
	done
}

function init
{
	# Stop
	stop_prog

	# Update bumo if the program has been modified
	if [ -f "$build_dir/bin/bumo" ]; then 
		cp $build_dir/bin/bumo $bumo/bin/bumo 
	else
		echo "No build directory found, use $bumo/bin/bumo"
	fi

	for h in $hosts; do
		for i in `seq -f %02g 1 $instance_per_node`; do
			ssh $h "${bumo}${i}/bin/bumo --dropdb"
			scp $bumo/bin/bumo ${h}:${bumo}${i}/bin/bumo		
		done
	done
	
	# Start
	sleep 2
	
	start_prog
}

function start_prog
{
	for h in $hosts; do
	    echo "+++ $h +++"
	    ssh $h "bash $dposd start $instance_per_node"
	done
}

function stop_prog
{
	for h in $hosts; do
	    echo "+++ $h +++"
	    ssh $h "bash $dposd stop $instance_per_node"
	done
}

function remove
{
	for h in $hosts; do
	    echo "+++ $h +++"
	    ssh $h "bash $dposd remove $instance_per_node"
	done
}

function clean
{
	for h in $hosts; do
	    echo "+++ $h +++"
	    ssh $h "bash $dposd clean $instance_per_node"
	done
}

function drop
{
	for h in $hosts; do
	    echo "+++ $h +++"
	    ssh $h "bash $dposd drop $instance_per_node"
	done
}

function status_prog 
{
	for h in $hosts; do
	    echo "+++ $h +++"
	    ssh $h "bash $dposd status $instance_per_node"
	done
}
cmds=""

while getopts 'hc:b:d:i:l:g:' arg ; do
	case $arg in
		h)
			usage
			;;
		c)
			cmds="$OPTARG"
			;;
		b)
			build_dir=$OPTARG
			;;
		d)
			bumo=$OPTARG
			;;
		i)
			instance_per_node=$OPTARG
			;;
		l)
			hosts="$OPTARG"
			;;
		g)
			keypair_prog=$OPTARG
			;;
		?)
			echo "Unknown options" && exit 1
			;;
	esac
done

for c in $cmds; do
	if [[ $c == "deploy" ]]; then
		deploy_prog
	elif [[ $c == "mod" ]]; then
		modify_cfg
	elif [[ $c == "init" ]]; then
		init
	elif [[ $c == "remove" ]]; then
		remove
	elif [[ $c == "clean" ]]; then
		clean
	elif [[ $c == "drop" ]]; then
		drop
	elif [[ $c == "stop" ]]; then
		stop_prog	
	elif [[ $c == "start" ]]; then
		start_prog
	elif [[ $c == "status" ]]; then
		status_prog
	else
		echo "Unknown command $c"
	fi
done
