/*
 *******************************
 * module name: log header file
 * author: PuMaoyang
 * e-mail: pumaoyang1988@163.com
 * create date: 2013-09-18
 * modify date: 
 * modify log: 
 *******************************
 */


#ifndef _L0G_H_2013_09_02_
#define _L0G_H_2013_09_02_

#include "types.h"

#define u_char unsigned char
#define u_int unsigned int
#define u_short unsigned short

typedef struct tagLogData
{
	tagLogData() {
		memset(this, 0, sizeof(*this));
	}
	u_short hdCount;
	FILE *fp;
	u_int uLogSize;
	u_int uPerSize;
	char szLogPath[256];
	char szOldPath[256];
	struct stat s_FileStat;
	time_t tLastTime;
	bool bInit;
}tagLogData;

class CMyLog 
{
	public:
		CMyLog();
		~CMyLog();

		int LogInit(const char *szFileName);
		int LogAppend(const char *szfmt,...);
		void LogClose();

		int RecusiveQuery();

	private:
		char *m_buf;
		int m_buflen;

		char *m_timebuf;
		int m_timelen;

		time_t tTimeNow;
		struct tm timeinfo, LastLogInfo;

		u_int m_uMaxLogSize;
		tagLogData *m_pLogData;
};
#endif

extern CMyLog g_Log;
#define DOLOG g_Log.LogAppend
