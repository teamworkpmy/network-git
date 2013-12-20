#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include "log.h"
#include "client.h"

class CEpoll 
{
	public:
		CEpoll();
		~CEpoll();
		int Init();
		int Loop();
		virtual int OnReadData(CClient *pClient);

	private: 
		int CreateSocket(const char *szIP, const short hdPort);
		int SetNonBlocking(int sockfd);
		int SetLinger(int sockfd);
		int SetReuseaddr(int sockfd);
		int BindAndListen();
		int Accept();

		int EpollCreate();
		int EpollAdd(int sockfd);
		int EpollWait(struct epoll_event *events, int maxevents, int timeout);
		int EpollDel(struct epoll_event *event);

	private:
		struct sockaddr_in m_svraddr;
		struct sockaddr_in m_cltaddr;
		uint32_t m_cltaddr_len;
		int m_sockfd;

		int m_epollfd;

	private:
		CClient *m_pClient;
};
