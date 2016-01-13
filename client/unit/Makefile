CC=g++
CFLAGS=-g --std=c++11

suite: *.cpp *.h
	$(CC) $(CFLAGS) -o suite.out suite.cpp ../number.cpp ../rsa.cpp

clean:
	rm ./suite.out