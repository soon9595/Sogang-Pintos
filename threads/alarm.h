#ifndef THREADS_ALARM_H
#define THREADS_ALARM_H
#include "devices/timer.h"
#include <list.h>
#include "threads/interrupt.h"
struct alarm
  {
    int64_t ticks;
    struct thread *td;   
    struct list_elem elem;     
    unsigned magic; 
  };
void alarm_init (void); 
void set_alarm (int64_t); 
void alarm_check_and_wake (void);
#endif /* THREADS_ALARM_H */
