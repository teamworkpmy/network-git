#include "mysql.h"

CMySQL_Lite::CMySQL_Lite() :
	m_pQueryBuf(0)
{

}

CMySQL_Lite::~CMySQL_Lite()
{
	if (m_pQueryBuf) {
		delete[] m_pQueryBuf;
		m_pQueryBuf = NULL;
	}

	mysql_close(m_pConn);
}

int CMySQL_Lite::InitMySQL()
{
	MYSQL *conn_ptr = mysql_init(NULL);

	if (!conn_ptr) {
		DOLOG("error: %s, line: %d\n", strerror(errno), __LINE__);
	}

	conn_ptr = mysql_real_connect(conn_ptr, 
			"192.168.1.111", "pmy", "test", "", 3306, NULL, 0);
	if (conn_ptr) {
		DOLOG("connecting\n");
	}
	else {
		DOLOG("error: %s, line: %d\n", strerror(errno), __LINE__);
	}

	m_pConn = conn_ptr;

	m_uQueryLen = 1024;

	m_pQueryBuf = (char *)new char[m_uQueryLen + 1];
	if (!m_pQueryBuf) {
		DOLOG("error: %s, line: %d\n", strerror(errno), __LINE__);
		return -1;
	}

	m_uDataLen = 1024 * 1024;

	m_pData = (char *)new char[m_uDataLen + 1];
	if (!m_pData) {
		DOLOG("error: %s, line: %d\n", strerror(errno), __LINE__);
		return -1;
	}

	return 0;
}

int CMySQL_Lite::UpdateSQL(const char *pQuery, ...)
{
	int ret = -1;

	va_list argptr;
	va_start(argptr, pQuery);
	vsnprintf(m_pQueryBuf, m_uQueryLen, pQuery, argptr);

	ret = mysql_query(this->m_pConn, m_pQueryBuf);
	if (ret != 0) {
		DOLOG("error: %s, line: %d\n", 
				mysql_error(this->m_pConn), __LINE__);
		return -1;
	}

	return 0;
}

int CMySQL_Lite::QuerySQLPure(const char *pQuery, ...)
{
	int ret = -1;

	va_list argptr;
	va_start(argptr, pQuery);
	vsnprintf(m_pQueryBuf, m_uQueryLen, pQuery, argptr);

	ret = mysql_query(this->m_pConn, m_pQueryBuf);
	if (ret != 0) {
		DOLOG("error: %s, line: %d\n", 
				mysql_error(this->m_pConn), __LINE__);
		return -1;
	}

	do {
		this->m_pRes = mysql_use_result(this->m_pConn);
		if (!this->m_pRes) {
			DOLOG("error: %s, line: %d\n", 
					mysql_error(this->m_pConn), __LINE__);
			return -1;
		}

		uint32_t num_fields = mysql_num_fields(this->m_pRes);

		MYSQL_FIELD *m_pField = mysql_fetch_fields(this->m_pRes);

		MYSQL_ROW row;

		uint32_t i;

		char *p = m_pData;

		while ((row = mysql_fetch_row(this->m_pRes))) {

			for (i = 0; i < num_fields; i++) {
				if (m_pField[i].type == MYSQL_TYPE_LONG) {
					*(int *)p = atol(row[i]);
					p += 8;
				}
				else if (m_pField[i].type == MYSQL_TYPE_TINY) {
					*p = atoi(row[i]);
					p += 1;
				}
				else if (m_pField[i].type == MYSQL_TYPE_SHORT) {
					*(short *)p = atoi(row[i]);
					p += 2;
				}
				else if (m_pField[i].type == MYSQL_TYPE_LONGLONG) {
					*(long *)p = atol(row[i]);
					p += 8;
				}
				else if (m_pField[i].type == MYSQL_TYPE_DATETIME) {
					struct tm tm_s;
					char *res = strptime(row[i], "%Y-%m-%d %H:%M:%S", &tm_s);
					if (!res) {
						DOLOG("error: %s, line: %d\n", 
								mysql_error(this->m_pConn), __LINE__);
						break;
					}

					ret = mktime(&tm_s);
					if (ret == -1) {
						DOLOG("error: %s, line: %d\n", 
								mysql_error(this->m_pConn), __LINE__);
						break;
					}

					*(long *)p = ret;
					p += 8;
				}
				else {
					memcpy(p, row[i], m_pField[i].length);
					p += m_pField[i].length;
					*p = '\0';
				}
			}
		}
	} while(false);

	mysql_free_result(this->m_pRes);

	return 0;
}

int CMySQL_Lite::QueryCount(const char *pQuery, ...)
{
	int ret = -1;

	va_list argptr;
	va_start(argptr, pQuery);
	vsnprintf(m_pQueryBuf, m_uQueryLen, pQuery, argptr);

	ret = mysql_query(this->m_pConn, m_pQueryBuf);
	if (ret != 0) {
		DOLOG("error: %s, line: %d\n", 
				mysql_error(this->m_pConn), __LINE__);
		return -1;
	}

	this->m_pRes = mysql_use_result(this->m_pConn);
	if (!this->m_pRes) {
		DOLOG("error: %s, line: %d\n", 
				mysql_error(this->m_pConn), __LINE__);
		return -1;
	}

	uint32_t num_fields = mysql_num_fields(this->m_pRes);
	if (num_fields != 1) {
		DOLOG("error: %s, line: %d\n", 
				mysql_error(this->m_pConn), __LINE__);
		return -1;
	}

	MYSQL_FIELD *m_pField = mysql_fetch_fields(this->m_pRes);

	MYSQL_ROW row;

	unsigned short hdNum;

	row = mysql_fetch_row(this->m_pRes);

	if (IS_NUM(m_pField->type)) {
		hdNum = atoi(*row);
	}
	else {
		DOLOG("error: %s, line: %d\n", 
				mysql_error(this->m_pConn), __LINE__);
		return -1;
	}

	mysql_free_result(this->m_pRes);

	return hdNum;
}

int CMySQL_Lite::GetRecordCount() 
{
	return mysql_num_rows(this->m_pRes);
}

