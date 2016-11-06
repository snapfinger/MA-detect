#!/bin/sh
# The first script

for i in $(find /data/home/yl2658/CV/exp/ -name '*.vx');
do
rm $i
done
echo vx file deleted
