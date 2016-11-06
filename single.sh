#!/bin/sh
echo start pre-processing
#convert from .jpg to .vx
vfmt if=image10_training.jpg of=image10.vx
echo conversion complete

#invert to green channel
    vchan c=2 if=image10.vx of=image10_temp1
    vpix -neg if=image10_temp1 of=image10_temp2
    vgfilt if=image10_temp2 xs=1 ys=1 zs=0 of=image10_pre.vx
    rm image10_temp1
    rm image10_temp2
echo green channel complete
echo gaussian filter complete

#start main.c
echo compile main.c
vcc main.c -o main
main if=image10_pre.vx of=image10_out.vx

