/* *
 * * *************************************
 * * module name: Mysql interface header file
 * * author: PuMaoyang
 * * e-mail: pumaoyang1988@163.com
 * * create date: 2013-09-18
 * * modify date: 
 * * modify log: 
 * * *************************************
 * */

#ifndef _MYSQL_H_2013_09_17
#define _MYSQL_H_2013_09_17

#include <mysql.h>
#include "log.h"

class CMySQL_Lite
{
	private:
		MYSQL *m_pConn;
		MYSQL_RES *m_pRes;

	public:
		CMySQL_Lite();
		~CMySQL_Lite();
		int InitMySQL();
		int CloseMySQL();

		int UpdateSQL(const char *pQuery, ...);
		int QuerySQLPure(const char *pQuery, ...);
		int QueryCount(const char *pQuery, ...);

		int GetRecordCount();
		int CustomBigIntFields(const char *pBuf, const uint16_t hdNum);

		uint32_t m_uQueryLen;
		char *m_pQueryBuf;
		uint32_t m_uDataLen;
		char *m_pData;

};

#endif
