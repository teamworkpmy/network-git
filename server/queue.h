#ifndef _QUEUE_H_2013_12_14_
#define _QUEUE_H_2013_12_14_

#include "log.h"
#include "types.h"

class CQueue
{
	public:
		CQueue();
		~CQueue();

		void Reset();
		void Clear();

		u_int UsedSize();
		u_int FreeSize();

		bool Alloc(const u_int uAllocLen = 1024 * 1024);
		u_int Pop(u_char *pBuf, const u_int uLen);
		u_int Push(const u_char *pBuf, const u_int uLen);
		u_int Peek(u_char *pBuf, const u_int uLen);

	private:
		u_int m_uHead;
		u_int m_uTail;
		u_int m_uQueueLen;
		u_char *m_pQueue;
};

#endif
