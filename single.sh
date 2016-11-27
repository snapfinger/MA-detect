#!/bin/sh
echo start pre-processing
#convert from .jpg to .vx
vfmt if=image10_training.jpg of=image10.vx
echo conversion complete

#invert to green channel
    vchan c=2 if=image10.vx of=temp1
    vpix -neg if=temp1 of=temp2
    vgfilt if=temp2 xs=1 ys=1 zs=0 of=img_pre.vx
    rm temp1
    rm temp2
echo green channel complete
echo gaussian filter complete


vdraw if=image10.vx C 256 16 20 200 of=image_out.vx
#start main.c
#echo compile main.c
#vcc main.c -o main
#main if=image10_pre.vx of=image10_out.vx

