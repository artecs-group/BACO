#!/bin/bash


cd cholesky-1
NX_ARGS="--schedule=ss --instrumentation=extrae" ./cholesky-i 16384 1024 0 > LOG.txt &
sleep 2

cd ../cholesky-2
NX_ARGS="--schedule=ss --instrumentation=extrae" ./cholesky-i 8129 1024 0 > LOG.txt &
cd ..




