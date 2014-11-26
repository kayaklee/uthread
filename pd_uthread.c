#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <setjmp.h>
#include "pd_uthread.h"
#include "pd_log.h"
#include "pd_list.h"

struct PdUThreadMgr;

struct PdUThreadNode
{
  ucontext_t context;
  int inited;
  jmp_buf jmpbuf;
  pd_uthread_func_pt uthread_func;
  void *uthread_arg;
  struct PdUThreadMgr *host;
  struct PdUThreadNode *prev;
  struct PdUThreadNode *next;
};

struct PdUThreadMgr
{
  ucontext_t context;
  jmp_buf jmpbuf;
  struct PdUThreadNode *waiting_list_head;
  struct PdUThreadNode *waiting_list_tail;
  struct PdUThreadNode *ready_list_head;
  struct PdUThreadNode *ready_list_tail;
};

static void pd_uthread_wrapper(struct PdUThreadMgr *uthread_mgr, struct PdUThreadNode *uthread_node)
{
  uthread_node->uthread_func(uthread_node, uthread_node->uthread_arg);
  pd_uthread_exit(uthread_node);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

struct PdUThreadMgr *pd_uthread_init_mgr()
{
  struct PdUThreadMgr *uthread_mgr = (struct PdUThreadMgr*)malloc(sizeof(struct PdUThreadMgr));
  if (NULL != uthread_mgr)
  {
    uthread_mgr->context.uc_stack.ss_sp = malloc(PD_UTHREAD_STACK_SIZE);
    uthread_mgr->context.uc_stack.ss_size = PD_UTHREAD_STACK_SIZE;
    uthread_mgr->waiting_list_head = NULL;
    uthread_mgr->waiting_list_tail = NULL;
    uthread_mgr->ready_list_head = NULL;
    uthread_mgr->ready_list_tail = NULL;
  }
  return uthread_mgr;
}

extern void pd_uthread_destroy_mgr(struct PdUThreadMgr *uthread_mgr)
{
  if (NULL != uthread_mgr)
  {
    if (NULL != uthread_mgr->context.uc_stack.ss_sp)
    {
      free(uthread_mgr->context.uc_stack.ss_sp);
    }
    free(uthread_mgr);
  }
}

struct PdUThreadNode *pd_uthread_init_node(struct PdUThreadMgr *uthread_mgr, pd_uthread_func_pt uthread_func, void *uthread_arg)
{
  struct PdUThreadNode *uthread_node = (struct PdUThreadNode*)malloc(sizeof(struct PdUThreadNode));
  if (NULL != uthread_node)
  {
    uthread_node->context.uc_stack.ss_sp = malloc(PD_UTHREAD_STACK_SIZE);
    uthread_node->context.uc_stack.ss_size = PD_UTHREAD_STACK_SIZE;
    uthread_node->uthread_func = uthread_func;
    uthread_node->uthread_arg = uthread_arg;
    uthread_node->host = uthread_mgr;
    uthread_node->prev = NULL;
    uthread_node->next = NULL;

    uthread_node->inited = 0;

    LIST_PUSH_BACK(uthread_mgr->ready_list_head, uthread_mgr->ready_list_tail, uthread_node);

    getcontext(&uthread_node->context);
    makecontext(&uthread_node->context, (void(*)(void))pd_uthread_wrapper, 2, uthread_mgr, uthread_node);
  }
  return uthread_node;
}

void pd_uthread_destroy_node(struct PdUThreadNode *uthread_node)
{
  if (NULL != uthread_node)
  {
    if (NULL != uthread_node->context.uc_stack.ss_sp)
    {
      free(uthread_node->context.uc_stack.ss_sp);
    }
    free(uthread_node);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void pd_uthread_schedule(struct PdUThreadMgr *uthread_mgr)
{
  while (NULL != uthread_mgr->ready_list_head)
  {
    struct PdUThreadNode *curr_node = NULL;
    LIST_POP_FRONT(uthread_mgr->ready_list_head, uthread_mgr->ready_list_tail, curr_node);

    PD_LOG(DEBUG, "schedule node=%p", curr_node);
    if (0 == _setjmp(uthread_mgr->jmpbuf))
    {
      if (!curr_node->inited)
      {
        curr_node->inited = 1;
        swapcontext(&uthread_mgr->context, &curr_node->context);
      }
      else
      {
        _longjmp(curr_node->jmpbuf, 1);
      }
    }
  }
}

void pd_uthread_yield(struct PdUThreadNode *uthread_node, const int ready)
{
  PD_LOG(DEBUG, "swith to scheduler");
  struct PdUThreadMgr *uthread_mgr = uthread_node->host;
  if (ready)
  {
    LIST_PUSH_BACK(uthread_mgr->ready_list_head, uthread_mgr->ready_list_tail, uthread_node);
  }
  else
  {
    //LIST_PUSH_BACK(uthread_mgr->waiting_list_head, uthread_mgr->waiting_list_tail, uthread_node);
  }
  //swapcontext(&uthread_node->context, &uthread_mgr->context);
  if (0 == _setjmp(uthread_node->jmpbuf))
  {
    _longjmp(uthread_mgr->jmpbuf, 1);
  }
}

void pd_uthread_wakeup(struct PdUThreadNode *uthread_node)
{
  struct PdUThreadMgr *uthread_mgr = uthread_node->host;
  //LIST_ERASE_NODE(uthread_mgr->waiting_list_head, uthread_mgr->waiting_list_tail, uthread_node);
  LIST_PUSH_FRONT(uthread_mgr->ready_list_head, uthread_mgr->ready_list_tail, uthread_node);
}

void pd_uthread_exit(struct PdUThreadNode *uthread_node)
{
  struct PdUThreadMgr *uthread_mgr = uthread_node->host;
  //swapcontext(&uthread_node->context, &uthread_mgr->context);
  if (0 == _setjmp(uthread_node->jmpbuf))
  {
    _longjmp(uthread_mgr->jmpbuf, 1);
  }
}

