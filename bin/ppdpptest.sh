#!/bin/bash

path="../data/test/test"

for i in $(seq 1 100)
do
	filename="$path$i.dat"
	$filename
	./ppdpp_main $filename
done
