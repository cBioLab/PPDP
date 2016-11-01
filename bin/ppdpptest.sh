#!/bin/bash

path="../data/test3/"

for i in $(seq 1 99)
do
	filename="$path$i.dat"
	$filename
	./ppdpp_main $filename
done
