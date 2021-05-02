LIBFLAGS=-lcrypto -lssl
STD=-std=c++17

pmanager: main.o aes.o hasher.o env.o
	g++ main.o aes.o hasher.o env.o -o pmanager -O2 $(LIBFLAGS)

aes.o: aes.cpp aes.hpp
	g++ aes.cpp -c $(STD)

hasher.o: hasher.cpp hasher.hpp
	g++ hasher.cpp -c $(STD)

env.o: env.cpp env.hpp
	g++ env.cpp -c $(STD)

main.o: main.cpp
	g++ main.cpp -c $(STD)

clean:
	rm *.o
