#include "log.h"
#define DEFAULT_RECV_DATA_LEN 1024 * 10

class CClient
{
	public:
		u_char m_pRecvData[DEFAULT_RECV_DATA_LEN];
		u_int m_iDataLen;
		
	public:
		u_char* GetData();
		u_int GetDataSize();

	public:
		CClient();
		~CClient();

	private:
		int m_sockfd;
		time_t m_tCreateTime;
		short hdPort;


};


