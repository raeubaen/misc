#!/bin/tcsh

g++ CRT.C -g -Wall `root-config --cflags --glibs` -o CRT.x
./CRT.x run$1_ana.root run$1_hist.root
