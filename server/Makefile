all: server.exe

CC = g++
RM = rm -rf
CFLAGS = -g -Wall -I/usr/include/mysql/ -lmysqlclient -lpthread

server.exe: epoll.o log.o mysql.o client.o queue.o pro.o server.o utility.o sem.o
	$(CC) -o server.exe epoll.o log.o mysql.o client.o queue.o pro.o server.o utility.o sem.o $(CFLAGS)

log.o: log.cpp log.h types.h
	$(CC) -c -g log.cpp $(CFLAGS)

mysql.o: mysql.cpp mysql.h log.h
	$(CC) -c -g mysql.cpp $(CFLAGS) 

epoll.o: epoll.cpp epoll.h log.h mysql.h
	$(CC) -c -g epoll.cpp $(CFLAGS)

client.o: client.cpp client.h log.h
	$(CC) -c -g client.cpp $(CFLAGS)

queue.o: queue.cpp queue.h log.h
	$(CC) -c -g queue.cpp $(CFLAGS)

server.o: server.cpp server.h epoll.h log.h pro.h
	$(CC) -c -g server.cpp $(CFLAGS)

pro.o: pro.cpp pro.h log.h sem.h
	$(CC) -c -g pro.cpp $(CFLAGS)

utility.o: utility.cpp utility.h log.h types.h
	$(CC) -c -g utility.cpp $(CFLAGS)

sem.o: sem.cpp sem.h log.h types.h
	$(CC) -c -g sem.cpp $(CFLAGS)



clean:
	$(RM) *.o
	$(RM) server.exe
