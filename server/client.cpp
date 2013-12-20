#include "client.h"

CClient::CClient()
{

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
	return m_iDataLen;
}


