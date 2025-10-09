src=main.cpp

server : $(src)
	g++ -o server $(src) -lpthread -lboost_system -std=c++17

.PHONY: clean
clean:
	rm -f server

