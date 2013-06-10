#ifndef _UTILS_H
#define _UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/syscall.h>
#include <unistd.h>

static inline int gettid(void)
{
	return syscall(__NR_gettid);	
}

#ifdef __cplusplus
}
#endif

#endif

