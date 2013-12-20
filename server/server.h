#include "epoll.h"
#include "mysql.h"
#include "pro.h"

class CServer : public CEpoll
{
	public:
		CServer();
		~CServer();
		int Init();
		int OnReadData(CClient *pClient);

	private:
		u_short GetThread();

	private:
		u_short m_hdThreadNum;
		CPro *m_pPro;
};
