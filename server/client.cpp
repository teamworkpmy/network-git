#include "client.h"

CClient::CClient()
{

}

CClient::~CClient()
{

}

char* CClient::GetData()
{
	return m_pRecvData;
}

int CClient::GetDataSize()
{
	return m_iDataLen;
}


