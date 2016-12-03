#!/bin/sh
vcc output.c -o output
#do the final output with different threshold of response
for i in 0.5 0.6 0.7 0.8
do
output if=image11_pre.vx th=$i of=image11_60_$i.vx
vxto png image11_60_$i.vx
done