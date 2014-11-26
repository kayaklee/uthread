#ifndef PD_DEFINE_H_
#define PD_DEFINE_H_

#define _GNU_SOURCE
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>

#ifdef __cplusplus
# define PD_CPP_START extern "C" {
# define PD_CPP_END }
#else
# define PD_CPP_START
# define PD_CPP_END
#endif

#define GETTID() syscall(__NR_gettid)

#define PD_UTHREAD_STACK_SIZE 65536

#endif

