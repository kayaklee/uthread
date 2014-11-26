#ifndef PD_LOG_H_
#define PD_LOG_H_

#include <sys/time.h>
#include <stdio.h>
#include <stdint.h>
#include <execinfo.h>
#include "pd_define.h"

#define FATAL 0
#define WARN  1
#define INFO  2
#define TRACE 3
#define DEBUG 4

#define PD_FATAL    "FATAL"
#define PD_WARN     "WARN"
#define PD_INFO     "INFO"
#define PD_TRACE    "TRACE"
#define PD_DEBUG    "DEBUG"

#define PD_LOG(_loglevel_, _fmt_, args...) \
  do \
  { \
    if (pd_log_get_glevel() >= _loglevel_) \
    { \
      struct timeval log_tm;\
      gettimeofday(&log_tm, NULL);\
      fprintf(stderr, \
          "[%s] [%s][%s][%ld] %d [%s][%d][%s] " _fmt_ "\n", \
          PD_##_loglevel_, \
          __DATE__, __TIME__, log_tm.tv_usec, \
          (int)GETTID(), __FILE__, __LINE__, __FUNCTION__, \
          ##args); \
    } \
  } while (0) \

#define PD_BACKTRACE(_fmt_, args...) \
  do \
  { \
    PD_LOG(INFO, _fmt_ " BackTrace Start: %s", ##args, pd_log_print_bt()); \
  } while (0)

extern const char* pd_log_print_array(char* buf, int64_t len, int64_t* array, int size);

extern const char* pd_log_print_bt();

extern void pd_log_set_glevel(const int level);

extern int pd_log_get_glevel();

#endif

