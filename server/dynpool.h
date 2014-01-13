#ifndef _DYNPOOL_H_2013_12_26
#define _DYNPOOL_H_2013_12_26

#include "log.h"

template<class C, typename K> class CDynPool
{
	private:
		C** m_ppPool;
		map<K, u_int> m_mapKeyPool;
		set<u_int> m_setFreeIndex;
		u_int m_uPoolSize;
		u_int m_uPoolCount;
		u_int m_uCurPoolCount;
		u_int m_uRow;
		u_int m_uColumn;

	public:
		CDynPool(const u_int uPoolSize = 32, const u_int uPoolCount = 64)
		{
			m_uPoolSize = uPoolSize;
			m_uPoolCount = uPoolCount;
			m_uCurPoolCount = 0;
			m_mapKeyPool.clear();
			m_setFreeIndex.clear();
			m_ppPool = new C*[m_uPoolCount];
			m_uRow = 0;
			m_uColumn = 0;

			LOG("[INFO]%s(%d): construct, poolsize: %u, poolcount: %u, curpoolcount: %u", 
					__FUNCTION__, __LINE__, m_uPoolSize, m_uPoolCount, m_uCurPoolCount);
		}

		~CDynPool()
		{
			for (u_int i = 0; i < m_uCurPoolCount; i++) {
				if (m_ppPool[i]) {
					delete[] m_ppPool[i];
					m_ppPool[i] = NULL;
				}
			}

			delete[] m_ppPool;
			m_ppPool = NULL;

			m_mapKeyPool.clear();
			m_setFreeIndex.clear();
		}

		inline int AllocPool()
		{
			if (!m_ppPool) {
				LOG("[ERROR]%s(%d): m_ppPool is a null ptr", __FUNCTION__, __LINE__);
				return -1;
			}

			m_ppPool[m_uCurPoolCount] = new C[m_uPoolSize];
			if (!m_ppPool[m_uCurPoolCount]) {
				LOG("[ERROR]%s(%d): alloc pool failed", __FUNCTION__, __LINE__);
				return -1;
			}

			memset(m_ppPool[m_uCurPoolCount], 0, sizeof(C) * m_uPoolSize);

			for (u_int i = m_uCurPoolCount * m_uPoolSize; i < (m_uCurPoolCount + 1) * m_uPoolSize; i++) {
				m_setFreeIndex.insert(i);
			}

			LOG("[INFO]%s(%d): alloc succ, poolsize: %u, poolcount: %u, curpoolcount: %u", 
					__FUNCTION__, __LINE__, m_uPoolSize, m_uPoolCount, m_uCurPoolCount + 1);

			return ++m_uCurPoolCount;
		}

		inline C* GetUserdMem(const K key) 
		{
			typename map<K, u_int>::iterator ite = m_mapKeyPool.find(key);
			if (ite != m_mapKeyPool.end()) {
				if (ite->second < m_uCurPoolCount * m_uPoolSize)
				{
					m_uRow = ite->second / m_uPoolSize;
					m_uColumn = ite->second % m_uPoolSize;

					if (m_ppPool[m_uRow]) {
						return m_ppPool[m_uRow] + m_uColumn;
					}
					else {
						m_mapKeyPool.erase(ite);
						LOG("[ERROR]%s(%d): null ptr, index: %d, poolsize: %d, curalloc: %d", 
								__FUNCTION__, __LINE__, ite->second, m_uPoolSize, m_uCurPoolCount);
						return NULL;
					}
				}
			}

			if (m_setFreeIndex.empty()) {
				if (AllocPool() == -1) {
					LOG("[ERROR]%s(%d): alloc failed, index: %d, poolsize: %d, curalloc: %d", 
							__FUNCTION__, __LINE__, ite->second, m_uPoolSize, m_uCurPoolCount);
					return NULL;
				}
			}

			set<u_int>::iterator ite_index = m_setFreeIndex.begin();
			u_int index = *ite_index;
			m_setFreeIndex.erase(ite_index);
			m_mapKeyPool.insert(make_pair(key, index));

			m_uRow = index / m_uPoolSize;
			m_uColumn = index % m_uPoolSize;

			if (!m_ppPool[m_uRow]) {
				LOG("[ERROR]%s(%d): null ptr, index: %d, poolsize: %d, curalloc: %d", 
						__FUNCTION__, __LINE__, index, m_uPoolSize, m_uCurPoolCount);
			}

			return m_ppPool[m_uRow] + m_uColumn;
		}

		inline int FreeMem(const K key) 
		{
			typename map<K, u_int>::iterator ite = m_mapKeyPool.find(key);
			if (ite != m_mapKeyPool.end()) {
				if (ite->second < m_uCurPoolCount * m_uPoolSize)
				{
					m_uRow = ite->second / m_uPoolSize;
					m_uColumn = ite->second % m_uPoolSize;

					if (m_ppPool[m_uRow]) {
						m_setFreeIndex.insert(ite->second);
						m_mapKeyPool.erase(ite);
						memset(m_ppPool[m_uRow] + m_uColumn, 0, sizeof(C));
						return 0;
					}
					else {
						m_mapKeyPool.erase(ite);
						LOG("[ERROR]%s(%d): null ptr, index: %d, poolsize: %d, curalloc: %d", 
								__FUNCTION__, __LINE__, ite->second, m_uPoolSize, m_uCurPoolCount);
						return -1;
					}
				}
			}
			else {
				LOG("[INFO]%s(%d): cannot find key, key: %d, poolsize: %d, curalloc: %d", 
						__FUNCTION__, __LINE__, (int)key, m_uPoolSize, m_uCurPoolCount);
			}

			return 0;
		}
};



#endif
