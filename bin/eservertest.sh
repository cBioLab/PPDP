#!/bin/bash

path="../data/server/"
port=23456
core=8
result="../data/est_result.dat"

for i in {0..9}
do
	filename="${path}ex_s1.dat"
	echo "${i}---------------------------------"
	./server -p $port -f $filename -c $core >> $result
	port=`expr $port + 1`
done
