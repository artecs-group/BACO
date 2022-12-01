#!/bin/bash

SERVER_PREFIX=./server

function usage {
    echo "$0 <mode> <action>"
}

function prepareQueues {

    echo "Removing all the previous queues...." >&2

    ipcrm --all=msg
    #ipcs --human -q
}

function runServer {
    # runServer <server_name> <action>
    if [[ -z $1 || ! -f $1 ]]; then
	echo "ERROR: server app $1 does not exist" >&2
	return
    fi


    echo "Running server ... [$1] " >&2
    $1 $2

}


function main {
    # if [ $# -ne 2 ]; then
    # 	usage $1
    # 	exit 1
    # fi

    prepareQueues

    mode=${1:-"release"}
    case $mode in
	"release" )
	    echo "Mode: release" >&2
	    runServer $SERVER_PREFIX.x $2
	    ;;
	"debug" )
	    echo "Mode: debug" >&2
	    runServer $SERVER_PREFIX-d.x $2
	    ;;
	"instr" )
	    echo "Mode: instrumentation" >&2
	    runServer $SERVER_PREFIX-i.x $2
	    ;;
	*)
	    echo -e "\nModo no soportado. Comprueba el valor de $1" >&2 ;;
    esac
}



main $1 $2
