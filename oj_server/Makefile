oj_server: oj_server.cc ../comm/message.pb.cc
	g++ -o $@ $^ -std=c++17 \
	-I../muduo/include/http \
	-I../muduo/include/tcp \
	-I../muduo/include/comm \
	-L../muduo/lib -ljsoncpp -lctemplate -lmysqlcppconn -letcd-cpp-api \
	-lcpprest -lgflags -lspdlog -lfmt -lbrpc -lprotobuf -lssl -lcrypto -lpthread -lleveldb \
	-lcpp_redis -ltacopie \
	-Wl,-rpath,../muduo/lib -lhttp -ltcp
	

.PHONY: clean
clean:
	rm -rf oj_server
