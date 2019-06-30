#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H
struct lock sys_lock;
void syscall_init (void);
/*****************************/
void user_exit(int status);
/*****************************/
int sum(const char *argv1,const char *argv2,const char *argv3,const char *argv4);
int pibonacci(const char *argv);
#endif /* userprog/syscall.h */
