#!/bin/bash

echo "" > fuzz.err && echo "" > fuzz.out

N=0
while [ $N -ne $1 ]
do
    echo "Round $N/$1 of fuzzing configuration"
    python3 test/config_gen.py 2>fuzz.tini 1>/dev/null
    ../bin/debug fuzz.tini 2>>fuzz.err 1>>fuzz.out
    N=$((N+1))
done

N=0
LIMIT=$(($1*100))
../bin/debug ../config.tini 1>>fuzz.out 2>>fuzz.err &
WEBSERV=$!
while [ $N -ne $LIMIT ]
do
    echo "Round $N/$LIMIT of fuzzing server"
    head -c 1000 /dev/urandom | nc 127.0.0.1 8080 2>>fuzz.err 1>>fuzz.out
    ps -p $WEBSERV 1>/dev/null 2>/dev/null
    if [ $? -ne 0 ]
    then
        echo "FAIL: Server died during fuzzing"
        exit 1
    fi
    N=$((N+1))
done

kill $WEBSERV 1>/dev/null 2>/dev/null

grep -a "fuzz.out" fuzz.err fuzz.out 1>/dev/null 2>/dev/null

if [ $? -ne 1 ]
then
    echo "FAIL: Found bad matches in files:"
    grep -a "fuzz.out" fuzz.err fuzz.out
    exit 2
fi
echo "SUCCESS: Didn't find bad matches"
