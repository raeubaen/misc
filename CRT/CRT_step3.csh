#!/bin/tcsh

g++ plots.C -g `root-config --cflags --glibs` -o plots.x
if ($? == 0) then
  g++ preliminars.C -g `root-config --cflags --glibs` -o preliminars.x

  if ($? == 0) then
    ./preliminars.x run$1_ana.root hists/run$1_hist.root
    ./plots.x run$1_ana.root hists/run$1_hist.root
  endif
endif
