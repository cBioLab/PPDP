#!/bin/bash

path="../data/server_d/"
port=23456
core=8
sigma=(10 20 30 40 50 60 70 80 90 100)
result="../data/st_result.dat"

for i in ${sigma[@]}
do
	filename="$path$i.dat"
	echo ${filename}"---------------------------------"
	./server -p $port -f $filename -c $core -s 2 >> $result
	port=`expr $port + 1`
done
