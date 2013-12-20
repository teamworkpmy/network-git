#include "sem.h"

CSem::CSem()
{
	sem_init(&m_sem, 0, 0);
}

CSem::~CSem()
{
	sem_destroy(&m_sem);
}

void CSem::Post()
{
	sem_post(&m_sem);
}

int CSem::Wait(time_t sec, long nsec)
{
	if (sec < 0 || nsec < 0) {
		DOLOG("[ERROR]%s(%d): param error, sec: %lu, nsec: %lu", __FUNCTION__, __LINE__, sec, nsec);
		return -1;
	}

	if (sec == 0 && nsec == 0) {
		sem_wait(&m_sem);
	}
	else {
		struct timespec timeout;
		timeout.tv_sec = time(NULL) + sec;
		timeout.tv_nsec = nsec;

		if (sem_timedwait(&m_sem, &timeout) != 0) {
			if (errno != ETIMEDOUT) { 
				DOLOG("[ERROR]%s(%d): timewait failed, sec: %lu, nsec: %lu, error: %s", 
						__FUNCTION__, __LINE__, sec, nsec, strerror(errno));
			}
			return -1;
		}
	}

	return 0;
}
