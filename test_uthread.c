#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "pd_uthread.h"
#include "pd_log.h"

void *uthread_func(struct PdUThreadNode *uthread_node, void *arg)
{
  int64_t ut_num = (int64_t)arg;

  PD_LOG(INFO, "[%ld] uthread_func start", ut_num);

  PD_LOG(INFO, "[%ld] uthread_func step1", ut_num);
  pd_uthread_yield(uthread_node, 1);

  PD_LOG(INFO, "[%ld] uthread_func step2", ut_num);
  pd_uthread_yield(uthread_node, 1);

  PD_LOG(INFO, "[%ld] uthread_func step3", ut_num);
  pd_uthread_yield(uthread_node, 1);

  PD_LOG(INFO, "[%ld] uthread_func end", ut_num);
}

int main()
{
  const int64_t uthread_num = 4;
  struct PdUThreadMgr *uthread_mgr;
  struct PdUThreadNode *uthread_node[uthread_num];

  int64_t i = 0;
  uthread_mgr = pd_uthread_init_mgr();
  for (i = 0; i < uthread_num; i++)
  {
    uthread_node[i] = pd_uthread_init_node(uthread_mgr, uthread_func, (void*)i);
  }

  pd_uthread_schedule(uthread_mgr);

  for (i = 0; i < uthread_num; i++)
  {
    pd_uthread_destroy_node(uthread_node[i]);
  }
  pd_uthread_destroy_mgr(uthread_mgr);
}

