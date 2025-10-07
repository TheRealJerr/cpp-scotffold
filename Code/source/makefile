src=main.cpp log.cpp
obj=$(src:.cpp=.o)

all: $(obj)
	g++ -o main $(obj) -lspdlog -lpthread -lgflags -lfmt 

%.o: %.cpp
	g++ -c $< -o $@ -lspdlog -lpthread -lgflags -lfmt -std=c++17

.PHONY: clean
clean:
	rm -f $(obj) main