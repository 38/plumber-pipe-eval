#!/usr/bin/zsh
echo > test.in
for ((i=0;i<512;i++))
do
	echo ".TEXT case_${i}" >> test.in
	echo $(($RANDOM/100.0)) $(($RANDOM/100.0)) $(($RANDOM/100.0)) $(($RANDOM/100.0)) $(($RANDOM/100.0)) $(($RANDOM/100.0)) >> test.in
	echo ".END" >> test.in
done
echo ".STOP" >> test.in
