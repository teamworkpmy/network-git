all: server.exe

CC = g++
RM = rm -rf
CFLAGS = -g -Wall -I/usr/include/mysql/ -lmysqlclient 

server.exe: epoll.o log.o mysql.o server.o client.o
	$(CC) -o server.exe server.o epoll.o log.o mysql.o client.o $(CFLAGS)

log.o: log.cpp log.h types.h
	$(CC) -c -g log.cpp $(CFLAGS)

mysql.o: mysql.cpp mysql.h log.h
	$(CC) -c -g mysql.cpp $(CFLAGS) 

epoll.o: epoll.cpp epoll.h log.h mysql.h
	$(CC) -c -g epoll.cpp $(CFLAGS)

server.o: server.cpp server.h epoll.h log.h
	$(CC) -c -g server.cpp $(CFLAGS)

client.o: client.cpp client.h log.h
	$(CC) -c -g client.cpp $(CFLAGS)

clean:
	$(RM) *.o
	$(RM) server.exe
