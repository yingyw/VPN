CC=g++
CFLAGS=--std=c++11 -g
CLIBS=-L/usr/lib -lssl -lcrypto

run: *.cpp
	$(CC) $(CFLAGS) -o run ops.cpp vpn.cpp $(CLIBS)

clean:
	rm run

