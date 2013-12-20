#ifndef _UTILITY_H_2013_12_18_
#define _UTILITY_H_2013_12_18_

#include "types.h"
#include "log.h"

int ThreadCreate(const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg);

#endif
