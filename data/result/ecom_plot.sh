#!/bin/bash

tteps="e_comsize.eps"
ttfile="e_comsize.dat"
ttpng="e_comsize.png"

gnuplot -p << EOF
#set terminal postscript png enhanced 'GothicBBB-Medium-EUC-H' color
set encoding utf8
set size 0.6,0.6
set xrange [0:110]
set yrange [0:20]
#set key box
set nokey
#set key right bottom
set xl "Threshold Value {/Symbol e}"
set yl "Communication Size [MB]"
plot "$ttfile" title "proposed" ps 1.5 pt 4 "Arias" 30
set terminal postscript eps enhanced color 
set output "$tteps"
replot
set output
#set output "$ttpng"
#set output
EOF

evince $tteps
