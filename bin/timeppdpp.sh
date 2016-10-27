#!/bin/bash

path="../data/sq/sq"

for i in $(seq 1 10)
do
	filename="$path${i}0.dat"
	$filename
	./sdp_main $filename
done
