#!/bin/tcsh

#check paths
cd /home/mu2e/mod0/root
g++ CRT.C -g -Wall `root-config --cflags --glibs` -o CRT.x
if ($? == 0) then
	./CRT.x ../CRT/analysis/run$1_ana.root ../CRT/analysis/run$1_hist.root
endif
