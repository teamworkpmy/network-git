#include "server.h"

CServer::CServer()
{

}

CServer::~CServer()
{

}

int CServer::Init()
{
	CEpoll::Init();
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



