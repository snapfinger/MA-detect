#!/bin/sh
#proceed this script after managing format of peakout.csv (add header,etc.)
chmod 755 vrclasstt.tcl
#vrclasstt.tcl tr=traindata_106.csv of=trainoutput.csv om=model_106 -v cl=log
vrclasstt.tcl im=model_60 te=peakout.csv of=class_vx.csv -v cl=log