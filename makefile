client: client.cpp utcp/utcp_manager.o utcp/checksums.o server
	g++ -o client utcp/checksums.o utcp/utcp_manager.o client.cpp 

server: server.cpp utcp/utcp_manager.o utcp/checksums.o
	g++ -o server utcp/checksums.o utcp/utcp_manager.o server.cpp 

utcp/utcp_manager.o: utcp/utcp_manager.cpp utcp/utcp_manager.h utcp/checksums.o
	g++ -o utcp/utcp_manager.o -c utcp/utcp_manager.cpp 

utcp/checksums.o: utcp/checksums.cpp utcp/checksums.h
	g++ -o utcp/checksums.o -c utcp/checksums.cpp 

clean:
	rm utcp/*.o
	rm *.o