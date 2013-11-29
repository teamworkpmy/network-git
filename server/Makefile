all: server.exe

CC = g++
RM = rm -rf
CFLAGS = -g -Wall -lmysqlclient 
CFLAGS2 = -I/usr/include/mysql/

server.exe: epoll.o log.o mysql.o
	$(CC) -o server.exe epoll.o log.o mysql.o $(CFLAGS)

log.o: log.cpp log.h types.h
	$(CC) -c -g log.cpp 

mysql.o: mysql.cpp mysql.h log.h
	$(CC) -c -g mysql.cpp $(CFLAGS2) 

epoll.o: epoll.cpp epoll.h log.h mysql.h
	$(CC) -c -g epoll.cpp $(CFLAGS2)

clean:
	$(RM) *.o
	$(RM) server.exe
