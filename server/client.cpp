#include "client.h"

CClient::CClient() :
	m_uDataLen(0),
	m_uExceptLen(0),
	m_uRecvedLen(0),
	m_tCreateTime(0),
	m_sockfd(0)
{
	//Init();
}

CClient::~CClient()
{
}

u_char* CClient::GetData()
{
	return m_pRecvData;
}

u_int CClient::GetDataSize()
{
	return m_uRecvedLen;
}

/*
   int CClient::Init()
   {
   m_pRecvData = new u_char[1024 * 1024 + 1];
   if (!m_pRecvData) {
   LOG("[ERROR]%s(%d): can't allocate for m_pRecvData, error: %s", __FUNCTION__, __LINE__, strerror(errno));
   return -1;
   }

   return 0;
   }*/


