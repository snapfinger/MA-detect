#!/bin/sh
echo start pre-processing
# This script does pre-processing operations on input images in batches
for i in $(find /data/home/yl2658/CV/data/ROCtraining/ -name '*.jpg'); #may need to adjust the directory here (also below)
do 
    vfmt if=$i of=tmp1 #convert input images from .jpeg to .vx
    vchan c=2 if=tmp1 of=tmp2  #get green channel
    vpix -neg if=tmp2 of=tmp3 #invert pixel value
    vgfilt if=tmp3 xs=1 ys=1 zs=0 of=$i.vx #gaussian filter with variance of 1
    rm tmp1
    rm tmp2
    rm tmp3    
    
done

for o in $(find /data/home/yl2658/CV/data/ROCtraining/ -name '*.vx'); #may need adjust the directory here (also below)
do
  mv $o /data/home/yl2658/CV/data/preprocessed/  #desired directory, may need adjust 
  echo $o preprocess complete
done
