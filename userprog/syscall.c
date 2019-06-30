#include "userprog/syscall.h"
#include "lib/user/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "devices/shutdown.h"
#include "process.h"
#include "threads/vaddr.h"
#include "threads/init.h"
#include "userprog/pagedir.h"
#include "userprog/process.h"
#include "threads/thread.h"
//#include "examples/sum.c"
//#include "lib/string.c"

static void syscall_handler (struct intr_frame *);

int syscall_halt(void);
int syscall_read(int fd,void *buffer,unsigned size);
int syscall_write(int fd,const void *buffer,unsigned size);
int syscall_exit(int status);
pid_t syscall_exec(const char *cmd_line);// pid_t is integer type
int syscall_wait(tid_t child_tid);


/*this is homework funcitoin*/

int pibonacci(const char *argv);
int sum(const char *argv1, const char *argv2, const char *argv3, const char *argv4);


    void
syscall_init (void) 
{
    intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

    static void
syscall_handler (struct intr_frame *f UNUSED) 
{

    // In here, we get system call, and put system call to f-> eax
    //1. by USING ESP, we can divide systemcall name, ( esp[0])


    int *syscall_esp = f->esp;// temporarily save the f->esp, so we can get file name.
	int syscallnum = 0;
    int ptrflag=0;

    
    struct thread *cur = thread_current();// current thread .
    ptrflag = check_valid_ptr(f->esp);
    if(ptrflag==-1){// not valid ptr;
       return  syscall_exit(-1);
    }

	syscallnum = *syscall_esp;
    //check the valid esp pointer
    if( syscallnum == SYS_HALT){

        syscall_halt();
    }

    else if( syscallnum == SYS_EXIT){
        // function call 
        if(check_valid_ptr(f->esp+4) == -1){
            syscall_exit(-1);
        }
        f->eax = (uint32_t)syscall_exit(get_arg(f->esp+4));
    }
    else if(syscallnum ==SYS_EXEC){
        //first we have to check the bit.
        if(check_valid_ptr(f->esp+4) == -1)
            syscall_exit(-1);
        // function call
        f->eax =(uint32_t)syscall_exec((const char*)syscall_esp[1]);//file name으로 부모 자식간 프로세스관계를 형성한다.
    }

    else if(syscallnum == SYS_WAIT){


        f->eax = (uint32_t)process_wait(get_arg(f->esp+4));

    }
    else if(syscallnum == SYS_READ){
        // fucntion call
        f->eax = syscall_read((int)get_arg(f->esp+4),(void*)get_arg(f->esp+8),get_arg(f->esp+12));


    }
    else if(syscallnum == SYS_WRITE){
        f->eax =syscall_write((int)get_arg(f->esp+4),(void*)get_arg(f->esp+8),get_arg(f->esp+12));
    }
    else if(syscallnum == SYS_SUM_4){
        // function call
        //printf("here\n");
        f->eax = sum((const char*)get_arg(f->esp+4),(const char*)get_arg(f->esp+8),(const char*)get_arg(f->esp+12),(const char*)get_arg(f->esp+16));
    }

    else if(syscallnum == SYS_FIBO){
        // function call
        f->eax = pibonacci((const char*)get_arg(f->esp+4));

    }
    else{

       return syscall_exit(-1);
     }

}

int syscall_halt(void)
{
    // declared in devices/shutdown.h
    shutdown_power_off();
}
int syscall_read(int fd,void *buffer, unsigned size)
{
    int i =0;
    uint8_t tmp;
    if(buffer > PHYS_BASE)
       syscall_exit(-1);
    if(fd==0)
    {
        for(i=0;i<size;i++){
            tmp = input_getc();
            memcpy(buffer++,&tmp,1);
        }
    }
    else
        syscall_exit(-1);
}
int syscall_write(int fd,const void *buffer,unsigned size)
{
    int ptrflag=1;
//    printf("write session\n");
    ptrflag = check_valid_ptr(buffer);
    if(ptrflag==-1)
    {
        syscall_exit(-1);
    }

      if(fd==1)
    {
        putbuf(buffer,size);
        return size;
    }
    else{
        return size;
    }
}

int syscall_exit(int status)// typedef int pid_t
{
    
	struct thread *thread_now;
    struct thread *thread_dead; // which is dead thread, it has to be checking to be deadstatus
	char *save_ptr, *exit_name, tmp[40];
    thread_dead= thread_current();
	thread_now = thread_current();
	strlcpy(tmp, thread_dead->name, 40);
	exit_name = strtok_r(tmp, " ", &save_ptr);
	printf("%s: exit(%d)\n",exit_name , status);
    thread_dead->dead_status = thread_dead->parent->child_dead_status = status;
	thread_exit();
    return status;

}
pid_t syscall_exec(const char *cmd_line)//cmd_line== 자식 프로세스 이름.
{


    if(check_valid_ptr(cmd_line)==-1){
            syscall_exit(-1);
    }
    return process_execute(cmd_line);//자식프로세스 새성!!!

}
int pibonacci(const char *argv) {
	int n;
	int res = 1, i;
	int val1, val2;
    //int *fibo;
	n = atoi(argv);
    //fibo = malloc(n);
    //printf("%d\n",n);
	if (n == 1 || n == 2)
		return res;
	else {
		//fibo[0] = fibo[1] = 1;
        val1 = 1;
        val2 = 1;
		for (i = 2; i < n; i++) {
			res = val1 + val2;
			val2 = val1;
			val1 = res;
            //fibo[i] = fibo[i-1] + fibo[i-2];
            //res = fibo[i];
            //printf("%dth res is %d\n",i,res);
		}
        return res;
	}
}




int sum(const char *argv1, const char *argv2, const char *argv3, const char *argv4) {
	int a, b, c, d;
	a = atoi(argv1);
	b = atoi(argv2);
	c = atoi(argv3);
	d = atoi(argv4);
	return a + b + c + d;
}
