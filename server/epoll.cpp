#include "epoll.h"
int size = 0;

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
#define EPOLLFD_NUM 8192

CMyLog g_Log;

CEpoll::CEpoll() 
{

}

CEpoll::~CEpoll()
{

}

int CEpoll::CreateSocket(const char *szIP, const short hdPort)
{
	m_svraddr.sin_family = AF_INET;
	m_svraddr.sin_port = htons(hdPort);
	m_svraddr.sin_addr.s_addr = inet_addr(szIP);

	m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	return m_sockfd;
}

int CEpoll::SetNonBlocking(int sockfd)
{
	int flag = fcntl(sockfd, F_GETFL);
	if (flag < 0) {
		DOLOG("[ERROR]%s(%d), fcntl failed, error: %s", __FUNCTION__, __LINE__, strerror(errno));
		return -1;
	}

	return (fcntl(sockfd, F_SETFL, flag | O_NONBLOCK) != -1);
}

int CEpoll::SetLinger(int sockfd)
{
	struct linger so_linger;
	so_linger.l_onoff = 1;
	so_linger.l_linger = 0; 

	return setsockopt( sockfd, SOL_SOCKET, SO_LINGER, &so_linger, sizeof(so_linger));
}

int CEpoll::SetReuseaddr(int sockfd)
{
	int iREUSEADDR = 1;
	return setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &iREUSEADDR, sizeof(iREUSEADDR));
}

int CEpoll::BindAndListen()
{
	if (m_sockfd < 0) {
		DOLOG("[ERROR]%s(%d), socket: %d error", __FUNCTION__, __LINE__, 
				m_sockfd);
		return -1;
	}

	if (bind(m_sockfd, (struct sockaddr *)&m_svraddr, sizeof(m_svraddr)) == -1) {
		DOLOG("[ERROR]%s(%d), bind socket failed, error: %s", __FUNCTION__, __LINE__, strerror(errno));
		return -1;
	}

	return listen(m_sockfd, 10);
}

int CEpoll::Accept()
{
	if (m_sockfd < 0) {
		DOLOG("[ERROR]%s(%d), socket: %d error", __FUNCTION__, __LINE__, 
				m_sockfd);
		return -1;
	}

	return accept(m_sockfd, (struct sockaddr *)&m_cltaddr, &m_cltaddr_len);
}

int CEpoll::EpollCreate()
{
	m_epollfd = epoll_create(EPOLLFD_NUM);

	return m_epollfd;
}

int CEpoll::EpollAdd(int sockfd)
{
	if (m_epollfd < 1) {
		DOLOG("[ERROR]%s(%d): m_epollfd: %d error", __FUNCTION__, __LINE__, m_epollfd);
		return -1;
	}

	struct epoll_event ev;
	ev.data.fd = sockfd;
	ev.events = EPOLLIN;

	return epoll_ctl(m_epollfd, EPOLL_CTL_ADD, sockfd, &ev);
}

int CEpoll::EpollWait(struct epoll_event *events, int maxevents, int timeout)
{
	if (m_epollfd < 1) {
		DOLOG("[ERROR]%s(%d): m_epollfd: %d error", __FUNCTION__, __LINE__, m_epollfd);
		return -1;
	}

	return epoll_wait(m_epollfd, events, maxevents, timeout);
}

int CEpoll::EpollDel(struct epoll_event *event)
{
	if (m_epollfd < 1) {
		DOLOG("[ERROR]%s(%d): m_epollfd: %d error", __FUNCTION__, __LINE__, m_epollfd);
		return -1;
	}

	epoll_ctl(m_epollfd, EPOLL_CTL_DEL, event->data.fd, NULL);
	close(event->data.fd);
	event->data.fd = -1;

	return 0;
}

int CEpoll::OnReadData(CClient *pClient)
{
	DOLOG("[INFO]%s(%d): len: %d", __FUNCTION__, __LINE__, pClient->GetDataSize());
	return 0;
}


int CEpoll::Init()
{
	int ret = 0;
	time_t tTimeNow = time(NULL);
	struct tm *timeinfo = localtime(&tTimeNow);
	char tmp[256] = {0};
	snprintf(tmp, 256, "./log/pmy_%04d_%02d_%02d.log", 
			timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday);

	g_Log.LogInit(tmp);

	const char *szIP = "192.168.1.222";
	short hdPort = 6666;

	int listen_sock = CreateSocket(szIP, hdPort);
	if (-1 == listen_sock) {
		DOLOG("[ERROR]%s(%d), create socket failed, error: %s", __FUNCTION__, __LINE__, strerror(errno));
		return -1;
	}

	ret = SetNonBlocking(listen_sock);
	if (ret < 0) {
		DOLOG("[ERROR]%s(%d), set non-blocking socket failed, error: %s", 
				__FUNCTION__, __LINE__, strerror(errno));
		return -1;
	}

	ret = SetLinger(listen_sock);
	if (ret == -1) {
		DOLOG("[ERROR]%s(%d), set linger failed, error: %s", __FUNCTION__, __LINE__, strerror(errno));
		return -1;
	}

	ret = SetReuseaddr(listen_sock);
	if (ret == -1) {
		DOLOG("[ERROR]%s(%d), set reuseaddr failed, error: %s", __FUNCTION__, __LINE__, strerror(errno));
		return -1;
	}

	ret = BindAndListen();
	if (ret == -1) {
		DOLOG("[ERROR]%s(%d), listen failed, error: %s", __FUNCTION__, __LINE__, strerror(errno));
		return -1;
	}
	else {
		DOLOG("[CONFIG] Listen IP: %s:%hd, sockfd: %d", szIP, hdPort, listen_sock);
	}

	ret = EpollCreate();
	if (ret == -1) {
		DOLOG("[ERROR]%s(%d), epoll create failed, error: %s", __FUNCTION__, __LINE__, strerror(errno));
		return -1;
	}

	ret = EpollAdd(listen_sock);
	if (ret == -1) {
		DOLOG("[ERROR]%s(%d), epoll add failed, error: %s", __FUNCTION__, __LINE__, strerror(errno));
		return -1;
	}

	return 0;
}

int CEpoll::Loop()
{
	int ret;
	int nfds;
	struct epoll_event events[EPOLLFD_NUM];
	m_pClient = new CClient;
	int timeout = 30;
	int conn_sock;

	for ( ; ; ) {
		nfds = EpollWait(events, EPOLLFD_NUM, timeout);
		if (nfds == 0) {
			continue;
		}
		else if (nfds == -1) {
			DOLOG("[ERROR]%s(%d), epoll wait failed, error: %s", __FUNCTION__, __LINE__, strerror(errno));
			return 0;
		}

		for (int n = 0; n < nfds; ++n) {
			if (events[n].data.fd == m_sockfd) {
				conn_sock = Accept();
				if (conn_sock == -1) {
					DOLOG("[ERROR]%s(%d), accept failed, error: %s", __FUNCTION__, __LINE__, strerror(errno));
					continue;
				}

				ret = SetNonBlocking(conn_sock);
				if (ret < 0) {
					DOLOG("[ERROR]%s(%d), set non-blocking socket failed, error: %s", __FUNCTION__, __LINE__, strerror(errno));
					return 0;
				}

				ret = SetLinger(conn_sock);
				if (ret == -1) {
					DOLOG("[ERROR]%s(%d), set linger failed, error: %s", __FUNCTION__, __LINE__, strerror(errno));
					return 0;
				}

				ret = SetReuseaddr(conn_sock);
				if (ret == -1) {
					DOLOG("[ERROR]%s(%d), set reuseaddr failed, error: %s", __FUNCTION__, __LINE__, strerror(errno));
					return 0;
				}

				ret = EpollAdd(conn_sock);
				if (ret < 0) {
					DOLOG("[ERROR]%s(%d), epoll add failed, error: %s", __FUNCTION__, __LINE__, strerror(errno));
					return 0;
				}
			}
			else {
				if (events[n].events == EPOLLIN) {
					//DOLOG("[INFO]%s(%d): EPOLLIN: %d, fd: %d ", __FUNCTION__, __LINE__, EPOLLIN, events[n].data.fd);

					memset(m_pClient->m_pRecvData, 0, 10240);

					ret = recv(events[n].data.fd, m_pClient->m_pRecvData, 
							10240, 0);
					if (ret == -1) {
						DOLOG("error:%s,%d", strerror(errno), __LINE__);
						continue;
					}
					else if (ret == 0) {
					//	DOLOG("[INFO]%s(%d): EPOLL_CTL_DEL: %d, fd: %d", __FUNCTION__, __LINE__, EPOLL_CTL_DEL, events[n].data.fd);
						EpollDel(&events[n]);
						continue;
					}

					m_pClient->m_iDataLen = ret;
					OnReadData(m_pClient);

					size += ret;
			//		DOLOG("%d: info %d ret: %d, total: %d", n, events[n].data.fd, ret, size);
				}
				else {
					DOLOG("NO EVENT");
				}
			}
		}
	}

	delete m_pClient;
	return 0;
}

