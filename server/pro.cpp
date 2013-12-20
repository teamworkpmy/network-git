#include "pro.h"

CPro::CPro() :
	m_pQueue(0), 
	m_pData(0)
{

}

CPro::~CPro()
{
	if (m_pQueue) {
		delete m_pQueue;
	}

	if (m_pData) {
		delete[] m_pData;
	}
}

int CPro::PushData(u_char *pData, u_int uDataLen)
{
	if (!pData || uDataLen < 1) {
		DOLOG("[ERROR]%s(%d): DATA ERROR, len: %d", __FUNCTION__, __LINE__, uDataLen);
		return -1;
	}

	m_pQueue->Push(pData, uDataLen);
	m_sem.Post();
	return 0;
}

int CPro::Init(CServer *pServer, u_short hdThread)
{
	m_pQueue = new CQueue; 
	if (!m_pQueue) {
		DOLOG("[ERROR]%s(%d): can't allocate for m_pQueue", __FUNCTION__, __LINE__);
		return -1;
	}

	if (!m_pQueue->Alloc()) {
		DOLOG("[ERROR]%s(%d): can't init for m_pQueue", __FUNCTION__, __LINE__);
		return -1;
	}

	m_pData = new u_char[10240];
	if (!m_pData) {
		DOLOG("[ERROR]%s(%d): can't allocate for m_pData", __FUNCTION__, __LINE__);
		return -1;
	}


	return 0;
}

int CPro::Loop()
{
	u_char *pData = m_pData;
	u_int uLen = 0;
	u_int uDataLen = 0;
	u_char buf[10240] = {0};

	for ( ; ; ) {
		if (m_sem.Wait(5, 0) == 0) {
			memset(m_pData, 0, 10240);
			memset(buf, 0, 10240);
			uLen = m_pQueue->Peek(pData, 4);
			if (uLen != 4) {
				//DOLOG("[ERROR]%s(%d): PEEK ERROR, len: %u", __FUNCTION__, __LINE__, uLen);
				continue;
			}

			uLen = *(u_int *)pData;
			if (uLen < 1 || uLen > 4294967294) {
				DOLOG("[ERROR]%s(%d): DATALEN ERROR, len: %u", __FUNCTION__, __LINE__, uLen);
				continue;
			}

			uDataLen = m_pQueue->Pop(pData, uLen);
			if (uDataLen != uLen) {
				DOLOG("[ERROR]%s(%d): POP ERROR, len: %u, data: %d", __FUNCTION__, __LINE__, uLen, uDataLen);
				continue;
			}

			tagData *p = (tagData *)pData;
			memcpy(buf, p->pData, p->iSize);
			DOLOG("[INFO]%s: RECV, len: %u, data: %s", __FUNCTION__, p->iSize, buf);
		}
	}
	return 0;
}

