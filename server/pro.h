#include "mysql.h"
#include "queue.h"
#include "sem.h"

class CServer;

class CPro
{
	public:
		CPro();
		~CPro();

		int Init(CServer *p, const u_short hdThread);
		int Loop();
		int PushData(u_char *pData, u_int uDataLen);
	private:
		CQueue *m_pQueue;
		u_char *m_pData;

	public:
		CSem m_sem;
};
