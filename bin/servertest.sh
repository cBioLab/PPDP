#!/bin/bash

path="../data/server/"
port=23456
core=8
sigma=(10 20 30 40 50 60 70 80 90 100)

for i in ${sigma[@]}
do
	filename="$path$i.dat"
	echo ${filename}"---------------------------------"
	./server -p $port -f $filename -c $core
	port=`expr $port + 1`
done
