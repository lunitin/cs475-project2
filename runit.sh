#!/bin/bash

IF=p2.cpp
OF=p2
LOG=log.csv

THREADS=(1 2 4 8 12)
ELEMENTS=(10 50 100 500 1000 2500 5000 7500 10000 20000)

g++ ${IF} -o ${OF} -lm -fopenmp

# Clean out log
echo "" > $LOG

# CSV Headers
for s in "${ELEMENTS[@]}"; do
	if [ $s == ${ELEMENTS[0]} ]; then
		echo -n , | tee -a $LOG
	fi
	echo -n $s | tee -a $LOG
	if [ $s != ${ELEMENTS[-1]} ]; then
		echo -n , | tee -a  $LOG
	else
		echo | tee -a $LOG
	fi

done

# number of threads:
for t in "${THREADS[@]}"
do
	echo -n "$t," | tee -a $LOG
	# number of subdivisions:
	for s in "${ELEMENTS[@]}"
	do
		./${OF} $t $s 2>&1 | tee -a  $LOG
	done
	echo | tee -a $LOG
done
