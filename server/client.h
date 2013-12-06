#include "log.h"
#define DEFAULT_RECV_DATA_LEN 1024 * 10

class CClient
{
	public:
		char m_pRecvData[DEFAULT_RECV_DATA_LEN];
		int m_iDataLen;
		
	public:
		char* GetData();
		int GetDataSize();

	public:
		CClient();
		~CClient();
};


