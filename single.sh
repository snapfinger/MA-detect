#!/bin/sh
vcc lmr.c -o lmr
vcc peak.c -o peak
vcc output.c -o output

#10 places 

echo start pre-processing
#convert from .jpg to .vx
vfmt if=image11_training.jpg of=image11.vx
echo conversion complete

#invert to green channel
    vchan c=2 if=image11.vx of=temp1
    vpix -neg if=temp1 of=temp2
    vgfilt if=temp2 xs=1 ys=1 zs=0 of=image11_pre.vx
    rm temp1
    rm temp2

echo  preprocessing complete and output image11_pre file

lmr if=image11_pre.vx of=image11_lmr.vx #will output lmrout.csv file containing all lmr pixel coordinates
vxto png image11_lmr.vx
peak if=image11_pre.vx
echo end pre-lmr-peak operations on image11
echo output peakout.csv file


