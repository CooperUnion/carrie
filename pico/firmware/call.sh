#!/bin/bash
tty=/dev/ttyACM0
call_cmd="cu -l $tty -s 115200"
if [ "$1" == "-n" ]; then
	$call_cmd
else
	rlwrap -a $call_cmd
fi
