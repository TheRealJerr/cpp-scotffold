all: server client

client: async_client.cc caculate.pb.cc
	g++ -o client $^ -lprotobuf -lbrpc -lpthread -lleveldb -ldl -std=c++17 -lssl -lcrypto -lgflags
server: server.cc caculate.pb.cc
	g++ -o server $^ -lprotobuf -lbrpc -lpthread -lleveldb -ldl -std=c++17 -lssl -lcrypto -lgflags 
.PHONY: clean
clean:
	rm -rf server client