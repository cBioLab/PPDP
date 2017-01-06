#!/bin/bash

path="../data/client/"
port=23456
core=2
sigma=(10 20 30 40 50 60 70 80 90 100)
host="localhost"
result="../data/ct_result.dat"

if [ $# != 0 ]; then
	host=$1
	echo $host
fi

for i in ${sigma[@]}
do
	filename="$path$i.dat"
	echo ${filename}"---------------------------------"
	./client -p $port -f $filename -c $core -h $host >> $result
	port=`expr $port + 1`
	sleep 2s
done
