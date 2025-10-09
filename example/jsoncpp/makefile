src=json.cpp test_jsoncpp.cpp
obj=$(src:.cpp=.o)

all: $(obj)
	g++ -o test_jsoncpp $(obj) -ljsoncpp 
%.o: %.cpp
	g++ -c -o $@ $< -std=c++17

.PHONY: clean
clean: 
	rm -f $(obj) test_jsoncpp