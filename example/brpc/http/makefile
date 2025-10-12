all: server client

client: http_client.cc ../caculate.pb.cc
	g++ -o client $^ -lprotobuf -lbrpc -lpthread -lleveldb -ldl -std=c++17 -lssl -lcrypto -lgflags
server: http_server.cc ../caculate.pb.cc
	g++ -o server $^ -lprotobuf -lbrpc -lpthread -lleveldb -ldl -std=c++17 -lssl -lcrypto -lgflags 
.PHONY: clean
clean:
	rm -rf server client