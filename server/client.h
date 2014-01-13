#include "log.h"
#include "types.h"
#define DEFAULT_RECV_DATA_LEN 1024 * 1024 + 1

class CClient
{
	public:
		u_char m_pRecvData[DEFAULT_RECV_DATA_LEN];
		u_char m_pBuffer[DEFAULT_RECV_DATA_LEN];
		u_int m_uDataLen;                       
		u_int m_uExceptLen;                  // the except data length
		u_int m_uRecvedLen;                  // the received data length
		u_int m_uRemainLen;                  // when m_uRecvedLen > m_uExceptLen
		struct sockaddr_in m_addr;
		time_t m_tCreateTime;
		int m_sockfd;
		
	public:
		u_char* GetData();
		u_int GetDataSize();

	public:
		CClient();
		~CClient();
	//	int Init();


	private:
};


