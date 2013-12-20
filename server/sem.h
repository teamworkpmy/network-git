#ifndef _UTILITY_H_2013_12_20_
#define _UTILITY_H_2013_12_20_

#include <semaphore.h>
#include "log.h"
#include "types.h"

class CSem 
{
	public:
		CSem();
		~CSem();

		int Wait(time_t sec, long nsec);
		void Post();

	private:
		sem_t m_sem;
};

#endif
