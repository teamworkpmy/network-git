#include "epoll.h"
#define DEFAULT_RECV_LEN 10240

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

CEpoll::CEpoll() :
	m_cltaddr_len(0),
	m_sockfd(0),
	m_epollfd(0),
	m_uRecvLen(0),
	m_pClient(0)
{

}

CEpoll::~CEpoll()
{
	if (m_pClient) {
		delete m_pClient;
	}
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
		LOG("[ERROR]%s(%d), fcntl failed, error: %s", __FUNCTION__, __LINE__, strerror(errno));
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
		LOG("[ERROR]%s(%d), socket: %d error", __FUNCTION__, __LINE__, 
				m_sockfd);
		return -1;
	}

	if (bind(m_sockfd, (struct sockaddr *)&m_svraddr, sizeof(m_svraddr)) == -1) {
		LOG("[ERROR]%s(%d), bind socket failed, error: %s", __FUNCTION__, __LINE__, strerror(errno));
		return -1;
	}

	return listen(m_sockfd, 10);
}

int CEpoll::Accept()
{
	if (m_sockfd < 0) {
		LOG("[ERROR]%s(%d), socket: %d error", __FUNCTION__, __LINE__, 
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
		LOG("[ERROR]%s(%d): m_epollfd: %d error", __FUNCTION__, __LINE__, m_epollfd);
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
		LOG("[ERROR]%s(%d): m_epollfd: %d error", __FUNCTION__, __LINE__, m_epollfd);
		return -1;
	}

	return epoll_wait(m_epollfd, events, maxevents, timeout);
}

int CEpoll::EpollDel(struct epoll_event *event)
{
	if (m_epollfd < 1) {
		LOG("[ERROR]%s(%d): m_epollfd: %d error", __FUNCTION__, __LINE__, m_epollfd);
		return -1;
	}

	epoll_ctl(m_epollfd, EPOLL_CTL_DEL, event->data.fd, NULL);
	close(event->data.fd);
	event->data.fd = -1;

	return 0;
}

int CEpoll::OnReadData(CClient *pClient)
{
	LOG("[INFO]%s(%d): len: %d", __FUNCTION__, __LINE__, pClient->GetDataSize());
	return 0;
}


int CEpoll::Init()
{
	int ret = 0;
#if 0
	time_t tTimeNow = time(NULL);
	struct tm *timeinfo = localtime(&tTimeNow);
	char tmp[256] = {0};
	snprintf(tmp, 256, "./log/pmy_%04d_%02d_%02d.log", 
			timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday);

	g_Log.LogInit(tmp);
#endif

	const char *szIP = "192.168.1.222";
	short hdPort = 6666;

	int listen_sock = CreateSocket(szIP, hdPort);
	if (-1 == listen_sock) {
		LOG("[ERROR]%s(%d), create socket failed, error: %s", __FUNCTION__, __LINE__, strerror(errno));
		return -1;
	}

	ret = SetNonBlocking(listen_sock);
	if (ret < 0) {
		LOG("[ERROR]%s(%d), set non-blocking socket failed, error: %s", 
				__FUNCTION__, __LINE__, strerror(errno));
		return -1;
	}

	ret = SetLinger(listen_sock);
	if (ret == -1) {
		LOG("[ERROR]%s(%d), set linger failed, error: %s", __FUNCTION__, __LINE__, strerror(errno));
		return -1;
	}

	ret = SetReuseaddr(listen_sock);
	if (ret == -1) {
		LOG("[ERROR]%s(%d), set reuseaddr failed, error: %s", __FUNCTION__, __LINE__, strerror(errno));
		return -1;
	}

	ret = BindAndListen();
	if (ret == -1) {
		LOG("[ERROR]%s(%d), listen failed, error: %s", __FUNCTION__, __LINE__, strerror(errno));
		return -1;
	}
	else {
		LOG("[CONFIG] Listen IP: %s:%hd, sockfd: %d", szIP, hdPort, listen_sock);
	}

	ret = EpollCreate();
	if (ret == -1) {
		LOG("[ERROR]%s(%d), epoll create failed, error: %s", __FUNCTION__, __LINE__, strerror(errno));
		return -1;
	}

	ret = EpollAdd(listen_sock);
	if (ret == -1) {
		LOG("[ERROR]%s(%d), epoll add failed, error: %s", __FUNCTION__, __LINE__, strerror(errno));
		return -1;
	}

	m_pClient = (CClient *)new CClient;
	if (!m_pClient) {
		LOG("[ERROR]%s(%d): can't allocate for m_pClient, error: %s", __FUNCTION__, __LINE__, strerror(errno));
		return -1;
	}

	m_uRecvLen = DEFAULT_RECV_LEN;

	return 0;
}

int CEpoll::Loop()
{
	int ret;
	int nfds;
	struct epoll_event events[EPOLLFD_NUM];
	int timeout = 30;
	int conn_sock;
	socklen_t len = 0;

	for ( ; ; ) {
		nfds = EpollWait(events, EPOLLFD_NUM, timeout);
		if (nfds == 0) {
			continue;
		}
		else if (nfds == -1) {
			LOG("[ERROR]%s(%d), epoll wait failed, error: %s", __FUNCTION__, __LINE__, strerror(errno));
			return 0;
		}

		for (int n = 0; n < nfds; ++n) {
			if (events[n].data.fd == m_sockfd) {
				conn_sock = Accept();
				if (conn_sock == -1) {
					LOG("[ERROR]%s(%d), accept failed, error: %s", __FUNCTION__, __LINE__, strerror(errno));
					continue;
				}

				ret = SetNonBlocking(conn_sock);
				if (ret < 0) {
					LOG("[ERROR]%s(%d), set non-blocking socket failed, error: %s", __FUNCTION__, __LINE__, strerror(errno));
					return 0;
				}

				ret = SetLinger(conn_sock);
				if (ret == -1) {
					LOG("[ERROR]%s(%d), set linger failed, error: %s", __FUNCTION__, __LINE__, strerror(errno));
					return 0;
				}

				ret = SetReuseaddr(conn_sock);
				if (ret == -1) {
					LOG("[ERROR]%s(%d), set reuseaddr failed, error: %s", __FUNCTION__, __LINE__, strerror(errno));
					return 0;
				}

				ret = EpollAdd(conn_sock);
				if (ret < 0) {
					LOG("[ERROR]%s(%d), epoll add failed, error: %s", __FUNCTION__, __LINE__, strerror(errno));
					return 0;
				}
			}
			else {
				if (events[n].events == EPOLLIN) {
					//LOG("[INFO]%s(%d): EPOLLIN: %d, fd: %d ", __FUNCTION__, __LINE__, EPOLLIN, events[n].data.fd);
					m_pClient = m_MemPool.GetUserdMem(events[n].data.fd);

					if (m_pClient->m_uRecvedLen == 0) {
						ret = recv(events[n].data.fd, m_pClient->m_pRecvData, m_uRecvLen, 0);
					}
					else {
						ret = recv(events[n].data.fd, m_pClient->m_pRecvData + m_pClient->m_uRecvedLen, m_uRecvLen, 0);
					}

					if (ret == -1) {
						LOG("[ERROR]%s(%d): recv error: %s", __FUNCTION__, __LINE__, strerror(errno));
						continue;
					}
					else if (ret == 0) {
						//LOG("[INFO]%s(%d): EPOLL_CTL_DEL: %d, fd: %d", __FUNCTION__, __LINE__, EPOLL_CTL_DEL, events[n].data.fd);
						m_MemPool.FreeMem(events[n].data.fd);
						EpollDel(&events[n]);
						continue;
					}

					if (m_pClient->m_uRecvedLen == 0) {
						m_pClient->m_uExceptLen = *(u_int *)m_pClient->m_pRecvData;
						if (m_pClient->m_uExceptLen > 10240) {
							LOG("[ERROR]%s(%d): RECV ERROR, len: %u, except: %u", 
									__FUNCTION__, __LINE__, m_pClient->m_uDataLen, m_pClient->m_uExceptLen);
							m_MemPool.FreeMem(events[n].data.fd);
							continue;
						}

						m_pClient->m_uDataLen = ret;
						m_pClient->m_uRecvedLen = ret;
						m_pClient->m_tCreateTime = time(NULL);
						m_pClient->m_sockfd = events[n].data.fd;
						len = sizeof(struct sockaddr);
						if (getsockname(events[n].data.fd, (struct sockaddr *)&m_pClient->m_addr, &len) == -1) {
							LOG("[INFO]%s(%d): getsockname error, sock: %d", __FUNCTION__, __LINE__, events[n].data.fd);
						}
					}
					else {
						m_pClient->m_uDataLen = ret;
						m_pClient->m_uRecvedLen += ret;
					}

					//LOG("[RECV]%s: nfds: %d, n: %d, fd: %d, ret: %d, total: %d, except: %d", 
					//		__FUNCTION__, nfds, n, events[n].data.fd, ret, m_pClient->m_uRecvedLen, m_pClient->m_uExceptLen);

					if (m_pClient->m_uRecvedLen < m_pClient->m_uExceptLen) {
						continue;
					}
					else if (m_pClient->m_uRecvedLen == m_pClient->m_uExceptLen) {
						OnReadData(m_pClient);
						m_pClient->m_uRecvedLen = 0;
						m_pClient->m_uExceptLen = 0;
						m_pClient->m_uRemainLen = 0;
					}
					else {
						m_pClient->m_uRemainLen = m_pClient->m_uRecvedLen;

						do {
							m_pClient->m_uRecvedLen = m_pClient->m_uExceptLen;
							OnReadData(m_pClient);
							m_pClient->m_uRemainLen -= m_pClient->m_uExceptLen;

							if (m_pClient->m_uRemainLen == 0)
								break;

							m_pClient->m_uExceptLen = *(u_int *)(m_pClient->m_pRecvData + m_pClient->m_uExceptLen);
							memmove(m_pClient->m_pRecvData, m_pClient->m_pRecvData + m_pClient->m_uRecvedLen, m_pClient->m_uRemainLen);
							//LOG("except: %d, remain: %d", m_pClient->m_uExceptLen, m_pClient->m_uRecvedLen);
						} while(m_pClient->m_uRemainLen >= m_pClient->m_uExceptLen);
					}
				}
				else {
					LOG("NO EVENT");
				}
			}
		}
	}

	delete m_pClient;
	return 0;
}

void CEpoll::DealData()
{
}
