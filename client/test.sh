#!/bin/bash

# i=3
# wget www.google.com:$(($i+3000))

NUM_TESTS=1
NUM_RUNS=1

# for i in $(seq 2 $NUM_TESTS)
# 	do sudo iptables -t nat -A OUTPUT -p tcp --dport $(($i+3000)) -j DNAT --to-destination 127.0.0.1:$(($i+2000))
# done

# do wgets
echo "Doing $NUM_TESTS wgets."

for i in $(seq 1 $NUM_TESTS)
do
	for j in $(seq 1 $NUM_RUNS)
	do
		wget -qO- "www.google.com:$(($i+3000))" > "resp$i$j" & #wget www.google.com:$(($i+3000))
	done
done

echo "Done. Waiting for completion."

time wait


