all : subscriber publisher

subscriber : subscriber.cc
	g++ -o subscriber subscriber.cc -lamqpcpp -lpthread -lev -std=c++17

publisher : publisher.cc
	g++ -o publisher publisher.cc -lamqpcpp -lpthread -lev -std=c++17

.PHONY : clean
clean :
	rm -f subscriber publisher