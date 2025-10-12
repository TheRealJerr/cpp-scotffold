all : server1 server2 rigister

server1 : server1.cc caculate.pb.cc
	g++ -o $@ $^ -lprotobuf -lbrpc -lpthread -lleveldb -ldl -std=c++17 -lssl -lcrypto -lgflags -lspdlog -lfmt

server2 : server2.cc caculate.pb.cc
	g++ -o $@ $^ -lprotobuf -lbrpc -lpthread -lleveldb -ldl -std=c++17 -lssl -lcrypto -lgflags -lspdlog -lfmt

rigister : rigistryServer.cc caculate.pb.cc rigistry.cpp log.cpp
	g++ -o $@ $^ -lprotobuf -lbrpc -lpthread -lleveldb -ldl -std=c++17 -lssl -lcrypto -lgflags -lspdlog -lfmt

.PHONY : clean
clean :
	rm -f server1 server2 rigister