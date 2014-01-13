#include "log.h"

CMyLog::CMyLog() : 
	m_buf(0), 
	m_pBufLen(0), 
	m_timebuf(0), 
	m_timelen(0),
	m_uMaxLogSize(0),
	m_pLogData(NULL)
{
	time_t tTimeNow = time(NULL);
	struct tm *timeinfo = localtime(&tTimeNow);
	char tmp[256] = {0};
	snprintf(tmp, 256, "./log/pmy_%04d_%02d_%02d.log",
			timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday);
	g_Log.LogInit(tmp);
}

CMyLog::~CMyLog()
{
	if (m_buf) {
		delete[] m_buf;
	}

	if (m_timebuf) {
		delete[] m_timebuf;
	}

	if (m_pLogData) {
		delete m_pLogData;
	}
}

int CMyLog::LogInit(const char *szFileName)
{
	m_timelen = 128;
	m_pBufLen = 1024 * 1024 + 1;
	m_uMaxLogSize = 1024 * 1024;

	m_buf = (char *)new char[m_pBufLen];
	if (!m_buf) {
		printf("[ERROR]%s: CAN'T ALLOCATE FOR m_buf", __FUNCTION__);
		return -1;
	}

	m_timebuf = (char *)new char[m_timelen];
	if (!m_timebuf) {
		printf("[ERROR]%s: CAN'T ALLOCATE FOR m_timebuf", __FUNCTION__);
		return -1;
	}

	m_pLogData = (tagLogData *)new tagLogData;
	if (!m_pLogData) {
		printf("[ERROR]%s: CAN'T ALLOCATE FOR m_pLogData", __FUNCTION__);
		return -1;
	}

	m_pLogData->uPerSize = 512;
	m_pLogData->hdCount = 1;

	m_pLogData->fp = fopen(szFileName, "a");
	if (m_pLogData->fp == NULL) {
		printf("[ERROR]:%s: FOPEN FAILED\n", __FUNCTION__);
		return -1;
	}	

	memcpy(m_pLogData->szLogPath, szFileName, 256);
	memcpy(m_pLogData->szOldPath, szFileName, 256);

	stat(m_pLogData->szLogPath, &m_pLogData->s_FileStat);

	return 0;
}

int CMyLog::LogAppend(const char *fmt,...)
{
	int ret;   

	memset(m_buf, 0, m_pBufLen);
	tTimeNow = time(NULL);
	localtime_r(&tTimeNow, &timeinfo);
	localtime_r(&m_pLogData->tLastTime, &LastLogInfo);

	if (timeinfo.tm_mday - LastLogInfo.tm_mday == 1 || 
			(LastLogInfo.tm_mday == 1 && timeinfo.tm_mday > 27 && 
			 LastLogInfo.tm_year != 70)) {

		ret = snprintf(
				m_buf, 256, "./log/pmy_%04d_%02d_%02d.log", 
				timeinfo.tm_year + 1900, 
				timeinfo.tm_mon + 1, 
				timeinfo.tm_mday);

		memset(m_pLogData->szLogPath, 0, 256);
		memcpy(m_pLogData->szLogPath, m_buf, ret);
		memset(m_pLogData->szOldPath, 0, 256);
		memcpy(m_pLogData->szOldPath, m_buf, ret);
		memset(m_buf, 0, m_pBufLen);

		m_pLogData->uLogSize = 0;
		m_pLogData->hdCount = 1;
		m_pLogData->tLastTime = tTimeNow;
		m_pLogData->bInit = true;

		m_pLogData->fp = fopen(m_pLogData->szLogPath, "a");
		if (m_pLogData->fp == NULL) {
			printf("[ERROR]%s(%d): FOPEN FAILED\n", __FUNCTION__, __LINE__);
			return -1;
		}

		stat(m_pLogData->szLogPath, &m_pLogData->s_FileStat);
	}

	if (!m_pLogData->bInit) {
		if (m_pLogData->s_FileStat.st_size > m_uMaxLogSize) {
			ret = RecusiveQuery();
			if (ret == -1)
				return -1;
		}
	}
	else {
		m_pLogData->bInit = false;
	}

	ret = strftime(m_timebuf, 128, "%F %T", &timeinfo);

	ret = snprintf(m_buf, 512, "%.*s %s\n", ret, m_timebuf, fmt);
	m_pLogData->uLogSize += ret;
	m_pLogData->tLastTime = tTimeNow;

	va_list ap1, ap2;         
	va_start(ap1, fmt);
	vprintf(m_buf, ap1);

	va_start(ap2, fmt);           
	vfprintf(m_pLogData->fp, m_buf, ap2);
	fflush(m_pLogData->fp);
	va_end(ap2);   
	va_end(ap1);

	return 0;
}

void CMyLog::LogClose()
{
	fclose(m_pLogData->fp);
	m_pLogData->fp = NULL;
}

int CMyLog::RecusiveQuery()
{
	memset(m_buf, 0, 512);
	int ret = snprintf(   
			m_buf, 512, "%s%hu", 
			m_pLogData->szOldPath,
			m_pLogData->hdCount);

	LogClose();
	memset(m_pLogData->szLogPath, 0, 256);
	memcpy(m_pLogData->szLogPath, m_buf, ret);

	m_pLogData->uLogSize = 0;
	m_pLogData->hdCount++;

	m_pLogData->fp = fopen(m_pLogData->szLogPath, "a");
	if (m_pLogData->fp == NULL) {
		printf("[ERROR]%s(%d): FOPEN FAILED\n", __FUNCTION__, __LINE__);
		return -1; 
	}

	stat(m_pLogData->szLogPath, &m_pLogData->s_FileStat);

	if (m_pLogData->s_FileStat.st_size > m_uMaxLogSize) {
		RecusiveQuery();
	}

	return 0;
}
