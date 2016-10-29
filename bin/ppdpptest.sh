#!/bin/bash

path="../data/test2/"

for i in $(seq 1 99)
do
	filename="$path$i.dat"
	$filename
	./ppdpp_main $filename
done
