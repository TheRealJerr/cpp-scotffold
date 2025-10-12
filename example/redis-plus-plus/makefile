redis_client : test_redis.cc
	g++ -o $@ $^ -lpthread -lredis++ -std=c++17 -Wall -lgflags
.PHONY : clean
clean : 
	rm -rf redis_client