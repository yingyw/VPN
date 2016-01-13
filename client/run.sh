NUM_TESTS=1
# start 1 bunch of code
for i in $(seq 1 $NUM_TESTS); do
	./run -s 69.91.153.28 -f -l $(($i+2000)) &> /dev/null &
	echo "ran $i"
done
