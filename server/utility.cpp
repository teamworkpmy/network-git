#include "utility.h"

int ThreadCreate(const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg)
{
	if (!start_routine) {
		LOG("[ERROR]%s(%d): func is a null ptr", __FUNCTION__, __LINE__);
		return -1;
	}
	
	int ret = 0;
	pthread_t thread;

	ret = pthread_create(&thread, NULL, start_routine, arg);
	if (ret != 0) {
		LOG("[ERROR]%s(%d): thread create failed, error: %s", __FUNCTION__, __LINE__, strerror(errno));
		return -1;
	}

	return 0;
}
