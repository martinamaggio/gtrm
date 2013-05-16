#!/bin/bash

# this is a test stub and shows how to write scripts
# to test the game theoretic resource manager behavior
# it assumes that you have installed the jobsignaler library
# with the test application

# set the location of the resource manager and of the test
# application
APP="/path/to/application/binary"
RMG="/path/to/resource/manager/binary"

# export jobsignaler directory: it is the directory where
# shared memory files are stored, make sure it is empty
export JOBSIGNALER_DIR=./jobsignaler
rm -rf $JOBSIGNALER_DIR
mkdir -p $JOBSIGNALER_DIR

# remove log files that are in the current directory, to
# replace them with the results of the current test
rm -f *.log

# set no admission control in sched deadline
echo -1 > /proc/sys/kernel/sched_dl_runtime_us

# if you want to change something in the online cpus
# you might want to do it throught the proc file system
# echo 0 > /sys/devices/system/cpu/cpu1/online

# start resource manager
# the two parameters are constants to express how much
# the action of the resource manager would decrease over
# time if no event happens
# default values for single cpu C1 = 0.5, C2 = 5.0
# default values for single cpu C1 = 0.1, C2 = 10.0
$RMG 0.5 5.0 &
RMGPID=$!

# start applications
# parameters:
# initial service level
# a_cpu
# b_cpu
# a_mem
# b_mem
# epsilon (service level adaptation rate)
# weight = (1 - lambda)
# deadline in seconds (0.1 means 1/10 second)
$APP 1.0 0.0 30000.0 0.0 0.0 0.0 0.9 0.1 &
APPPID=$!

# waiting for some time
sleep 1

# stopping application
kill -2 $APPPID

# stopping resource manager
kill -2 $RMGPID

# if you changed something in the number of online cpus
# restart them
# echo 1 > /sys/devices/system/cpu/cpu1/online

# save results
mkdir results
mv *.log results/