#include "queue.h"

CQueue::CQueue() :
	m_pQueue(NULL)
{
	m_uHead = 0;
	m_uTail = 0;
	m_uQueueLen = 0;
}

CQueue::~CQueue()
{
	if (m_pQueue) {
		delete m_pQueue;
	}
}

void CQueue::Reset()
{
	m_uHead = 0;
	m_uTail = 0;
}

void CQueue::Clear()
{
	m_uHead = 0;
	m_uTail = 0;
	m_uQueueLen = 0;
	m_pQueue = NULL;
}

u_int CQueue::UsedSize()
{
	if (m_uQueueLen < 1)
		return 0;

	if (m_uTail < m_uHead) {
		return m_uQueueLen - (m_uHead - m_uTail);
	}
	else {
		return m_uTail - m_uHead;
	}
}

u_int CQueue::FreeSize()
{
	if (m_uQueueLen < 1) {
		return 0;
	}

	if (m_uTail == 0 && m_uHead == 0) {
		return m_uQueueLen;
	}

	if (m_uTail < m_uHead) {
		return m_uHead - m_uTail;
	}
	else {
		return m_uQueueLen - (m_uTail - m_uHead);
	}
}

bool CQueue::Alloc(const u_int uAllocLen)
{
	m_pQueue = new u_char[uAllocLen];
	if (!m_pQueue) {
		return false;
	}

	m_uHead = 0;
	m_uTail = 0;
	m_uQueueLen = uAllocLen;
	return true;
}

u_int CQueue::Pop(u_char *pBuf, const u_int uLen)
{
	if (m_uQueueLen < 1 || uLen < 1)
		return 0;

	if (UsedSize() < uLen) {
		return 0;
	}

	if (m_uTail < m_uHead) {
		if (m_uQueueLen - m_uHead >= uLen)
		{
			memcpy(pBuf, &m_pQueue[m_uHead], uLen);
			m_uHead += uLen;
		}
		else {
			memcpy(pBuf, &m_pQueue[m_uHead], m_uQueueLen - m_uHead);
			memcpy(&pBuf[m_uQueueLen - m_uHead], m_pQueue, uLen - (m_uQueueLen - m_uHead));
			m_uHead = uLen - (m_uQueueLen - m_uHead);
		}
	}
	else {
		memcpy(pBuf, &m_pQueue[m_uHead], uLen);
		m_uHead += uLen;
	}

	return uLen;
}

u_int CQueue::Push(const u_char *pBuf, const u_int uLen)
{
	if (m_uQueueLen < 1 || uLen < 1 || uLen > FreeSize())
		return 0;

	if (m_uTail < m_uHead) {
		memcpy(&m_pQueue[m_uTail], pBuf, uLen);
		m_uTail += uLen;
	}
	else {
		if (m_uQueueLen - m_uTail >= uLen) {
			memcpy(&m_pQueue[m_uTail], pBuf, uLen);
			m_uTail += uLen;
		}
		else {
			memcpy(&m_pQueue[m_uTail], pBuf, m_uQueueLen - m_uTail);
			memcpy(m_pQueue, &pBuf[m_uQueueLen - m_uTail], uLen - (m_uQueueLen - m_uTail));
			m_uTail = uLen - (m_uQueueLen - m_uTail);
		}
	}

	return uLen;
}

u_int CQueue::Peek(u_char *pBuf, const u_int uLen)
{
	if (m_uQueueLen < 1 || uLen < 1)
		return 0;

	if (UsedSize() < uLen) {
		return 0;
	}

	if (m_uTail < m_uHead) {
		if (m_uQueueLen - m_uHead >= uLen)
		{
			memcpy(pBuf, &m_pQueue[m_uHead], uLen);
		}
		else {
			memcpy(pBuf, &m_pQueue[m_uHead], m_uQueueLen - m_uHead);
			memcpy(&pBuf[m_uQueueLen - m_uHead], m_pQueue, uLen - (m_uQueueLen - m_uHead));
		}
	}
	else {
		memcpy(pBuf, &m_pQueue[m_uHead], uLen);
	}

	return uLen;
}

