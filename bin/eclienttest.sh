#!/bin/bash

path="../data/client/"
port=23456
core=2
epsilon=(10 20 30 40 50 60 70 80 90 100)
host="localhost"
result="../data/ect_result.dat"

if [ $# != 0 ]; then
	host=$1
	echo $host
fi

for i in ${epsilon[@]}
do
	filename="${path}ex_c1.dat"
	echo ${i}"---------------------------------"
	./client -p $port -f $filename -c $core -h $host -e $i >> $result
	port=`expr $port + 1`
	sleep 2s
done
