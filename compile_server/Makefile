testserver: compile_server.cc ../comm/message.pb.cc
	g++ -o $@ $^ -std=c++17  -letcd-cpp-api -lpthread -ljsoncpp -lcpprest -lgflags \
	-lspdlog -lfmt -lbrpc -lprotobuf -lssl -lcrypto -lpthread -lleveldb
.PHONY:clean
clean:
	rm -rf testserver