#ifndef PD_LIST_H_
#define PD_LIST_H_

#define LIST_PUSH_BACK(head, tail, node) \
  do \
  { \
    if (NULL == tail) \
    { \
      head = node; \
      tail = node; \
    } \
    else \
    { \
      node->prev = tail; \
      node->next = NULL; \
      \
      tail->next = node; \
      tail = node; \
    } \
  } while (0)

#define LIST_PUSH_FRONT(head, tail, node) \
  do \
  { \
    if (NULL == head) \
    { \
      head = node; \
      tail = node; \
    } \
    else \
    { \
      node->next = head; \
      node->prev = NULL; \
      \
      head->prev = node; \
      head = node; \
    } \
  } while (0)

#define LIST_POP_FRONT(head, tail, node) \
  do \
  { \
    node = head; \
    head = head->next; \
    if (NULL != head) \
    { \
      head->prev = NULL; \
    } \
    else \
    { \
      tail = NULL; \
    } \
    node->prev = NULL; \
    node->next = NULL; \
  } while (0)

#define LIST_ERASE_NODE(head, tail, node) \
  do \
  { \
    if (NULL == node->prev) \
    { \
      head = node->next; \
      if (NULL != head) \
      { \
        head->prev = NULL; \
      } \
    } \
    else \
    { \
      node->prev->next = node->next; \
    } \
    if (NULL == node->next) \
    { \
      tail = node->prev; \
      if (NULL != tail) \
      { \
        tail->next = NULL; \
      } \
    } \
    else \
    { \
      node->next->prev = node->prev; \
    } \
    node->prev = NULL; \
    node->next = NULL; \
  } while (0)

#endif

