#/bin/bash

LATSIZE=1
LATCOUNT=10000000
THRSIZE=1000000
THRCOUNT=10000

echo LATENCY same core same thread

#echo common memory space
#taskset -c 0 ./common_lat $LATSIZE $LATCOUNT
echo pipe
taskset -c 0 ./pipe_lat $LATSIZE $LATCOUNT
echo unix socket
taskset -c 0 ./unix_lat $LATSIZE $LATCOUNT
echo tcp socket
taskset -c 0 ./tcp_lat $LATSIZE $LATCOUNT

echo LATENCY same core diff thread

echo common memory space
taskset -c 0,1 ./common_lat $LATSIZE $LATCOUNT
echo pipe
taskset -c 0,1 ./pipe_lat $LATSIZE $LATCOUNT
echo unix socket
taskset -c 0,1 ./unix_lat $LATSIZE $LATCOUNT
echo tcp socket
taskset -c 0,1 ./tcp_lat $LATSIZE $LATCOUNT

echo LATENCY diff core

echo common memory space
taskset -c 0,2 ./common_lat $LATSIZE $LATCOUNT
echo pipe
taskset -c 0,2 ./pipe_lat $LATSIZE $LATCOUNT
echo unix socket
taskset -c 0,2 ./unix_lat $LATSIZE $LATCOUNT
echo tcp socket
taskset -c 0,2 ./tcp_lat $LATSIZE $LATCOUNT

echo THROUGHPUT same core same thread

echo common memory space
#taskset -c 0 ./common_thr $THRSIZE $THRCOUNT
echo pipe
taskset -c 0 ./pipe_thr $THRSIZE $THRCOUNT
echo unix socket
taskset -c 0 ./unix_thr $THRSIZE $THRCOUNT
echo tcp socket
taskset -c 0 ./tcp_thr $THRSIZE $THRCOUNT

echo THROUGHPUT same core diff thread

echo common memory space
taskset -c 0,1 ./common_thr $THRSIZE $THRCOUNT
echo pipe
taskset -c 0,1 ./pipe_thr $THRSIZE $THRCOUNT
echo unix socket
taskset -c 0,1 ./unix_thr $THRSIZE $THRCOUNT
echo tcp socket
taskset -c 0,1 ./tcp_thr $THRSIZE $THRCOUNT

echo THROUGHPUT diff core
echo common memory space
taskset -c 0,2 ./common_thr $THRSIZE $THRCOUNT
echo pipe
taskset -c 0,2 ./pipe_thr $THRSIZE $THRCOUNT
echo unix socket
taskset -c 0,2 ./unix_thr $THRSIZE $THRCOUNT
echo tcp socket
taskset -c 0,2 ./tcp_thr $THRSIZE $THRCOUNT
