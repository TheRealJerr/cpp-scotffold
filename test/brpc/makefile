all: server client

client: test.cc caculate.pb.cc log.cpp rigistry.cpp 
	g++ -o client $^ -lprotobuf -lbrpc -lpthread -lleveldb -ldl -std=c++17 -lssl -lcrypto -lgflags -lspdlog -lfmt
server: server.cc caculate.pb.cc log.cpp rigistry.cpp 
	g++ -o server $^ -lprotobuf -lbrpc -lpthread -lleveldb -ldl -std=c++17 -lssl -lcrypto -lgflags -lspdlog -lfmt
.PHONY: clean
clean:
	rm -rf server client