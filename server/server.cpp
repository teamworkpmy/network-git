#include "server.h"

CServer::CServer() :
	m_pPro(0)
{
	m_hdThreadNum = 0;
}

CServer::~CServer()
{
	if (m_pPro) {
		delete[] m_pPro;
		m_pPro = NULL;
	}
}

void* thr_fn(void *p) 
{
	CPro *pPro = (CPro *)p;
	if (!pPro) {
		LOG("[ERROR]%s(%d): pPor is a null ptr", __FUNCTION__, __LINE__);
		return 0;
	}

	pPro->Loop();
	return 0;
}

int CServer::Init()
{
	CEpoll::Init();

	m_hdThreadNum = 10;

	m_pPro = new CPro[m_hdThreadNum]; 
	if (!m_pPro) {
		LOG("[ERROR]%s(%d): can't allocate for m_pPro", __FUNCTION__, __LINE__);
		return -1;
	}

	pthread_t thread[m_hdThreadNum];

	struct timespec req;
	req.tv_sec = 0;
	req.tv_nsec = 300;

	for (u_short i = 0; i < m_hdThreadNum; i++) {
		m_pPro[i].Init(this, i);

		if (pthread_create(&thread[i], NULL, thr_fn, &m_pPro[i]) != 0) {
			LOG("[ERROR]%s(%d): pthread_create error, n: %d", __FUNCTION__, __LINE__, i);
			return -1;
		}

		nanosleep(&req, NULL);
	}

	return 0;
}

u_short CServer::GetThread()
{
	static u_short hdThread = 0;

	if (hdThread >= m_hdThreadNum) {
		hdThread = 0;
	}

	return hdThread++;
}

int CServer::OnReadData(CClient *pClient) 
{
	struct sockaddr_in addr = pClient->m_addr;
	//LOG("%s: len: %d, IP %s:%hd", __FUNCTION__, pClient->GetDataSize(), inet_ntoa(addr.sin_addr), addr.sin_port);

	u_short hdThread = GetThread();

	if (hdThread >= m_hdThreadNum) {
		LOG("[ERROR]%s(%d): thread: %d error", __FUNCTION__, __LINE__, hdThread);
		return -1;
	}

	m_pPro[hdThread].PushData(pClient->GetData(), pClient->GetDataSize());
	return 0;
}

int main()
{
	CServer *p = new CServer;
	if (p && p->Init() == 0) {
		p->Loop();
	}

	if (p) {
		delete p;
		p = NULL;
	}

	return 0;
}



