#include "epoll.h"

#if 0
typedef struct epoll_event {
	uint32_t events;  /* Epoll events */
	epoll_data_t data;  /* User data variable */
};

typedef union epoll_data {
	void *ptr;
	int fd;
	uint32_t u32;
	uint64_t u64;
} epoll_data_t;
#endif
#define EPOLLFD_NUM 1024

CMyLog g_Log;

int size = 0;

int main()
{
	time_t tTimeNow = time(NULL);
	struct tm *timeinfo = localtime(&tTimeNow);
	char tmp[256] = {0};
	snprintf(tmp, 256, "./log/pmy_%04d_%02d_%02d.log", timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday);

	g_Log.LogInit(tmp);

	int ret = -1;
	int listen_sock;
	struct sockaddr_in svraddress;

	svraddress.sin_family = AF_INET;
	svraddress.sin_port = htons(6666);
	svraddress.sin_addr.s_addr = inet_addr("192.168.1.222");

	listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == listen_sock) {
		DOLOG("[error]%s", strerror(errno));
		return 0;
	}

	int flag = fcntl(listen_sock, F_GETFL);
	if (flag < 0) {
		DOLOG("error:%s,%d", strerror(errno), __LINE__);
		return 0;
	}

	ret = fcntl(listen_sock, F_SETFL, flag | O_NONBLOCK);
	if (ret < 0) {
		DOLOG("error:%s,%d", strerror(errno), __LINE__);
		return 0;
	}

	struct linger so_linger;
	so_linger.l_onoff = 1;
	so_linger.l_linger = 1; 
	ret = setsockopt(listen_sock, SOL_SOCKET, 
			SO_LINGER, &so_linger, sizeof(so_linger));
	if (ret == -1) {
		DOLOG("error:%s,%d", strerror(errno), __LINE__);
		return 0;
	}

	int32_t iREUSEADDR = 1;
	ret = setsockopt(listen_sock, SOL_SOCKET, 
			SO_REUSEADDR, &iREUSEADDR, sizeof(iREUSEADDR));
	if (ret == -1) {
		DOLOG("error:%s,%d", strerror(errno), __LINE__);
		return 0;
	}

	ret = bind(listen_sock, 
			(struct sockaddr *)&svraddress, sizeof(svraddress));
	if (ret == -1) {
		DOLOG("error:%s,%d", strerror(errno), __LINE__);
		return 0;
	}

	ret = listen(listen_sock, 10);
	if (ret == -1) {
		DOLOG("error:%s,%d", strerror(errno), __LINE__);
		return 0;
	}
	else {
		DOLOG("connecting..%d", listen_sock);
	}

	int nfds;
	int epollfd;
	int conn_sock;
	struct epoll_event ev;
	struct epoll_event events[EPOLLFD_NUM];
	struct sockaddr_in cltaddress;
	unsigned int addrlen = sizeof(cltaddress);
	int timeout = 30;
	char buf[4096] = {0};

	epollfd = epoll_create(EPOLLFD_NUM);
	if (-1 == epollfd) {
		return 0;
	}

	ev.data.fd = listen_sock;
	ev.events = EPOLLIN;
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listen_sock, &ev) == -1) {
		return 0;
	}

	int i = 0;
	for ( ; ; ) {
		nfds = epoll_wait(epollfd, events, EPOLLFD_NUM, timeout);
		if (nfds == 0) {
			continue;
		}
		else if (nfds == -1) {
			DOLOG("error:%s", strerror(errno));
			return 0;
		}

		for (int n = 0; n < nfds; ++n) {
			if (events[n].data.fd == listen_sock) {
				conn_sock = accept(listen_sock, 
						(struct sockaddr *)&cltaddress, &addrlen);
				if (conn_sock == -1) {
					DOLOG("error:%s,%d", strerror(errno), __LINE__);
					continue;                                       //accept之前，client断开连接，继续
				}

				int flag = fcntl(conn_sock, F_GETFL);
				if (flag < 0) {
					DOLOG("error:%s,%d", strerror(errno), __LINE__);
					continue;
				}

				ret = fcntl(conn_sock, F_SETFL, flag | O_NONBLOCK);
				if (ret < 0) {
					DOLOG("error:%s,%d", strerror(errno), __LINE__);
					continue;
				}

				struct linger so_linger;
				so_linger.l_onoff = 1;
				so_linger.l_linger = 1; 
				ret = setsockopt(conn_sock, SOL_SOCKET, 
						SO_LINGER, &so_linger, sizeof(so_linger));
				if (ret == -1) {
					DOLOG("error:%s,%d", strerror(errno), __LINE__);
					continue;
				}

				int32_t iREUSEADDR = 1;
				ret = setsockopt(conn_sock, SOL_SOCKET, 
						SO_REUSEADDR, &iREUSEADDR, sizeof(iREUSEADDR));
				if (ret == -1) {
					DOLOG("error:%s,%d", strerror(errno), __LINE__);
					continue;
				}

				ev.events = EPOLLIN;
				ev.data.fd = conn_sock;

				if (epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock, &ev) == -1) {
					DOLOG("error:%s,%d", strerror(errno), __LINE__);
					continue;
				}
			} 
			else {
				if (events[n].events == EPOLLIN) {
					DOLOG("[INFO]%s(%d): EPOLLIN: %d, fd: %d ", 
							__FUNCTION__, __LINE__, EPOLLIN, events[n].data.fd);

					memset(buf, 0, 4096);

					ret = recv(events[n].data.fd, buf, 4096, 0);
					if (ret == -1) {
						DOLOG("error:%s,%d", strerror(errno), __LINE__);
						continue;
					}
					else if (ret == 0) {
						DOLOG("[INFO]%s(%d): EPOLL_CTL_DEL: %d, fd: %d", 
								__FUNCTION__, __LINE__, EPOLL_CTL_DEL, events[n].data.fd);
						epoll_ctl(epollfd, EPOLL_CTL_DEL, events[n].data.fd, NULL);
						close(events[n].data.fd);
						events[n].data.fd = -1;
						continue;
					}
					size += ret;
					i++;
					DOLOG("%d: info %d ret: %d, total: %d", i, events[n].data.fd, ret, size);
				}
				else {
					DOLOG("NO EVENT");
				}
			}
		}
	}
	return 0;
}
