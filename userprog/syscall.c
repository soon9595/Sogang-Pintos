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
#include "filesys/filesys.h"
#include "filesys/file.h"
#include "filesys/inode.h"
#include "threads/synch.h"
//#include "examples/sum.c"
//#include "lib/string.c"
//int debug = 0;
//static struct lock open_lock;
static void syscall_handler (struct intr_frame *);

int syscall_halt(void);
int syscall_read(int fd,void *buffer,unsigned size);
int syscall_write(int fd,const void *buffer,unsigned size);
void syscall_exit(int status);
pid_t syscall_exec(const char *cmd_line);// pid_t is integer type
int syscall_wait(tid_t child_tid);

bool syscall_create(const char *file, unsigned initial_size);

bool syscall_remove(const char *file);
int syscall_open(const char *file);
int syscall_filesize(int fd);
int syscall_read(int fd, void *buffer, unsigned size);
int syscall_write(int fd, const void *buffer, unsigned size);
void syscall_seek(int fd, unsigned position);
unsigned syscall_tell(int fd);
void syscall_close(int fd);




/*this is homework funcitoin*/

int pibonacci(const char *argv);
int sum(const char *argv1, const char *argv2, const char *argv3, const char *argv4);


    void
syscall_init (void) 
{
    intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
    lock_init(&sys_lock);
     
}

    static void
syscall_handler (struct intr_frame *f UNUSED) 
{

    // In here, we get system call, and put system call to f-> eax
    //1. by USING ESP, we can divide systemcall name, ( esp[0])


    int *syscall_esp = f->esp;// temporarily save the f->esp, so we can get file name.
    int syscallnum = 0;
    int ptrflag=0;
    bool success;
 //   struct thread *cur = thread_current();// current thread .
    ptrflag = check_valid_ptr(f->esp);
    if(ptrflag==-1){// not valid ptr;
      syscall_exit(-1);
    }

    syscallnum = *syscall_esp;
    //check the valid esp pointer
    if(check_valid_ptr(syscall_esp) == -1){
            syscall_exit(-1);
    }
    if( syscallnum == SYS_HALT){

        syscall_halt();
    }

    else if( syscallnum == SYS_EXIT){
        // function call 
        if(check_valid_ptr(f->esp+4) == -1){
            syscall_exit(-1);
        }
        syscall_exit(syscall_esp[1]);
    }
    else if(syscallnum ==SYS_EXEC){
        //first we have to check the bit.
        if(check_valid_ptr(f->esp+4) == -1){
            syscall_exit(-1);
        }
        // function call
        f->eax =(uint32_t)syscall_exec((const char*)syscall_esp[1]);//file name으로 부모 자식간 프로세스관계를 형성한다.

    }

    else if(syscallnum == SYS_WAIT){

            f->eax = (uint32_t)process_wait((syscall_esp[1]));
    }
    else if(syscallnum == SYS_READ){
        f->eax = syscall_read((int)get_arg(f->esp+4),(void*)get_arg(f->esp+8),get_arg(f->esp+12));

    }
    else if(syscallnum == SYS_WRITE){
        f->eax =syscall_write((int)get_arg(f->esp+4),(void*)get_arg(f->esp+8),get_arg(f->esp+12));
    }
    else if(syscallnum == SYS_SUM_4){
        f->eax = sum((const char*)get_arg(f->esp+4),(const char*)get_arg(f->esp+8),(const char*)get_arg(f->esp+12),(const char*)get_arg(f->esp+16));
    }

    else if(syscallnum == SYS_FIBO){
        // function call
        f->eax = pibonacci((const char*)get_arg(f->esp+4));

    }
    else if(syscallnum == SYS_CREATE){
        f->eax = syscall_create((char*)get_arg(f->esp+4), (unsigned)get_arg(f->esp+8));
    }
    else if(syscallnum == SYS_REMOVE){
        f->eax = syscall_remove((char*)get_arg(f->esp+4));
    }
    else if(syscallnum == SYS_OPEN){
        f->eax = syscall_open((char *)get_arg(f->esp+4));

    }   
    else if(syscallnum == SYS_CLOSE){
        syscall_close((int)get_arg(f->esp+4));
    }
    else if(syscallnum == SYS_FILESIZE){
        f->eax = syscall_filesize((int)get_arg(f->esp+4));
    }

    else if(syscallnum == SYS_SEEK){
        syscall_seek((int)get_arg(f->esp+4), get_arg(f->esp+8));
    }
    else if(syscallnum == SYS_TELL){
        f->eax = syscall_tell((int)get_arg(f->esp+4));

    }



    else{

        syscall_exit(-1);
    }

}

int syscall_halt(void)
{
    shutdown_power_off();
}
int syscall_read(int fd,void *buffer, unsigned size)
{
    int i =0;
    uint8_t tmp;
    bool suc;
    //if(buffer > PHYS_BASE)
    if(check_valid_ptr(buffer)==-1)
        syscall_exit(-1);
      if(size <0){
        syscall_exit(-1);
    }

    suc = lock_try_acquire(&sys_lock);
    if(fd==0)
    {
        for(i=0;i<size;i++){
            tmp = input_getc();
            memcpy(buffer++,&tmp,1);
        }
        if(suc)
            lock_release(&sys_lock);
        return size;
    }
    else if(fd == 1){
        if(suc)
            lock_release(&sys_lock);
        syscall_exit(-1);

    }



    else{
        struct thread *cur = thread_current ();
        struct file_desc *cur_fd;
        struct list_elem *e;
        int byte;
        struct file *fp = NULL;

        if (!is_user_vaddr(buffer) 
                || check_valid_ptr(buffer)==-1){
            syscall_exit (-1);
        }
        for (e = list_begin(&cur->fd_list); 
                e != list_end (&cur->fd_list);
                e = list_next(e)){
            cur_fd = list_entry (e, struct file_desc, fd_elem);
            if (cur_fd->fd == fd){
                fp = cur_fd->f_ptr;
                break;
            }
        }
        if (!fp){
            if(suc)
                lock_release(&sys_lock);
            return -1;
        }
        else {
        //    lock_acquire (&sys_lock);
            byte = file_read (fp, buffer, size);
            if(suc)
                lock_release (&sys_lock);

            return byte;
        }
    }
}
int syscall_write(int fd,const void *buffer,unsigned size)
{
    int ptrflag=1,result;
    bool suc;
    struct thread *current= thread_current();
    struct file_desc *desc;
    struct list_elem *e;
    ptrflag = check_valid_ptr(buffer);
    if(ptrflag==-1)
    {
        syscall_exit(-1);
    }
    //debug for oom problem;and copycheck.
    if(is_kernel_vaddr(buffer) || !check_valid_ptr(buffer)||buffer == NULL){

        syscall_exit(-1);
    }
    suc = lock_try_acquire(&sys_lock);
    if(fd<0){
        if(suc)
            lock_release(&sys_lock);
        syscall_exit(-1);
    }

    if(fd==1)
    {
        putbuf(buffer,size);
        if(suc)
            lock_release(&sys_lock);
        return size;
    }
    else if( fd ==0)
    {

        if(suc)
            lock_release(&sys_lock);
        syscall_exit(-1);
    }
  

         else {
             struct thread *cur = thread_current ();
             struct file_desc *cur_fd;
             struct list_elem *e;
             int byte;
             struct file *fp = NULL;
             if (is_kernel_vaddr(buffer) 
                     || check_valid_ptr(buffer)==-1){
                 syscall_exit (-1);
             }
             for (e = list_begin(&cur->fd_list); 
                     e != list_end (&cur->fd_list);
                     e = list_next(e)){
                 cur_fd = list_entry (e, struct file_desc, fd_elem);
                 if (cur_fd->fd == fd){
                     fp = cur_fd->f_ptr;
                     break;
                 }
             }
             if (!fp){
                 if(suc)
                    lock_release(&sys_lock);
                 return -1;
             }
             else {
        //         lock_acquire (&sys_lock);
                 byte = file_write (fp, buffer, size);
                 if(suc)
                    lock_release (&sys_lock);
                 return byte;
             }
         }

}
void syscall_exit(int status)// typedef int pid_t
{
    struct file_desc *fd;
    struct thread *thread_now;
    struct list_elem *e;
    struct thread *thread_dead; // which is dead thread, it has to be checking to be deadstatus
    char *save, *exit_name, tmp[40];
    bool suc;
    thread_dead= thread_current();
    strlcpy(tmp, thread_dead->name, 40);
    exit_name = strtok_r(tmp, " ", &save);
    printf("%s: exit(%d)\n",exit_name , status);
    thread_current()->exit_status = status;
    //   this is just copying part

    if(thread_dead->elf_ptr){
        suc = lock_try_acquire(&sys_lock);
        file_close(thread_dead->elf_ptr);
        if(suc)
            lock_release(&sys_lock);
    }
    while(!list_empty(&thread_dead->fd_list)){
        e = list_pop_back (&thread_dead->fd_list);
        fd = list_entry(e, struct file_desc, fd_elem);
        if(fd){
            if(!fd->f_ptr){
                file_allow_write(fd->f_ptr);
                file_close(fd->f_ptr);
            }
            else
            {
                file_deny_write(fd->f_ptr);
                file_close(fd->f_ptr);
            }

        }

        free(fd);
    }

    if(list_empty(&thread_dead->fd_list))
        thread_dead->exist_fdlist = false;
    sema_down(&thread_dead->parent_sema);
   // sema_up(&thread_dead->parent->child_sema);
    thread_dead->exit_status = status;
    thread_dead->parent->deadchild_status = status;
    thread_dead->parent->exist_childlist = false;
    thread_dead->parent->dead = true;
    sema_up(&thread_dead->parent->child_sema);
    //sema_down(&thread_dead->parent_sema);

    thread_exit();
}
pid_t syscall_exec(const char *cmd_line)//cmd_line== 자식 프로세스 이름.
{
    if(check_valid_ptr(cmd_line)==-1){
        //syscall_exit(-1);
        return -1;
    }
    if(!pagedir_get_page(thread_current()->pagedir, cmd_line)){
        //syscall_exit(-1);
        return -1;
    }
    
    

    return process_execute(cmd_line);//자식프로세스 새성!!!

}
int pibonacci(const char *argv) {
    int n;
    int res = 1, i;
    int val1, val2;
    n = atoi(argv);
    if (n == 1 || n == 2)
        return res;
    else {
        val1 = 1;
        val2 = 1;
        for (i = 2; i < n; i++) {
            res = val1 + val2;
            val2 = val1;
            val1 = res;
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
bool syscall_create(const char *file, unsigned initial_size){
    // create file is not open,,. open is different order.....

    bool suc;
    bool result = false;
    int ptrflag=0;

    if(!file) syscall_exit(-1);
    if(check_valid_ptr(file)==-1)
        syscall_exit(-1);
    suc = lock_try_acquire(&sys_lock);
    result = filesys_create(file, initial_size);
    if(suc)
        lock_release(&sys_lock);
    return result;


}
void syscall_close(int fd)
{

    struct thread *current = thread_current();
    struct list_elem *e;
    struct file_desc *current_fd;

    if(fd == 0) return ;

    else if(fd == 1) return ;


  current_fd = find_filedesc(fd);
    if(current_fd){
        lock_acquire(&sys_lock);
        file_close(current_fd->f_ptr);
        lock_release(&sys_lock);
        list_remove(&current_fd->fd_elem);
        free(current_fd);
        return;
    }
    return;

}
int syscall_open(const char *file){// the important part of this homework

    struct file *fp;
    struct file_desc *fd, *insert_new_fd;
    struct thread *current = thread_current();
    bool suc;
    if(!file){
        syscall_exit(-1);
    }
    if(check_valid_ptr(file) == -1){
        syscall_exit(-1);
    }
    if(!pagedir_get_page(current->pagedir,file)){

        syscall_exit(-1);        
    }
    if(list_size(&current->fd_list)>35){
            return -1;
    }
    
    
    suc = lock_try_acquire(&sys_lock);
    fp = filesys_open(file);

    if(suc)
        lock_release(&sys_lock);
    
    if(fp == NULL) 
        return -1;


    fd = malloc(sizeof(fp));
    
    

    fd->f_ptr = fp;
    
    if(list_empty(&current->fd_list)){// if there is no file descripitor i.e. filepointer

        fd->fd = 2; // to distinguish the file descriptor ID.
    }
    else{// not empty, then +1 then insert it
        insert_new_fd = list_entry(list_back(&current->fd_list), struct file_desc, fd_elem);
        fd->fd = insert_new_fd ->fd + 1;
    }


    list_push_back(&current->fd_list, &fd->fd_elem);

    return fd->fd;

}
int syscall_filesize(int fd){

    struct thread *current;

    struct file_desc *current_fd;
    struct list_elem *e;
    struct file *fp = NULL;
    int size;
    current = thread_current();

    e=list_begin(&current->fd_list);
    
    if(list_empty(&current->fd_list)){
        
        return 0;

    }
    while(e != list_end(&current->fd_list)){

        current_fd = list_entry(e, struct file_desc, fd_elem);
        if(current_fd ->fd == fd){
            fp = current_fd->f_ptr;
            break;
        }
        e= list_next(e);
    }
    if(current_fd == NULL){
            syscall_exit(-1);
    }
    if(fp == NULL){
        syscall_exit(-1);
    }
    lock_acquire(&sys_lock);
    size = file_length(fp);
    lock_release(&sys_lock);
    return size;

}
void syscall_seek(int fd, unsigned position)
{
    struct thread *current;
    struct file_desc *current_fd;
    struct list_elem *e;


    current = thread_current();

    e=list_begin(&current->fd_list);



    while(e != list_end(&current->fd_list)){

        current_fd = list_entry(e, struct file_desc, fd_elem);
        if(current_fd ->fd == fd){
            lock_acquire(&sys_lock);
            file_seek(current_fd -> f_ptr,position);
            lock_release(&sys_lock);
           // break;
            return;
        }

        
        e= list_next(e);
    }
    if(current_fd ->f_ptr ==NULL){
            syscall_exit(-1);
    }
    syscall_exit(-1);
    

}
bool syscall_remove(const char *file)
{
    bool result;
    bool suc;
    if(file == NULL){
        syscall_exit(-1);
    }

    suc = lock_try_acquire(&sys_lock);
    result = filesys_remove(file);
    if(suc)
        lock_release(&sys_lock);
    return result;

}
unsigned syscall_tell(int fd)
{
    struct thread *current;
    struct file_desc *current_fd;
    struct list_elem *e;
    //    struct file *fp =NULL;


    current = thread_current();

    e=list_begin(&current->fd_list);



    while(e != list_end(&current->fd_list)){

        current_fd = list_entry(e, struct file_desc, fd_elem);
        if(current_fd ->fd == fd){
           // lock_acquire(&sys_lock);
             return file_tell(current_fd -> f_ptr);
          //  lock_release(&sys_lock);
           //   break;
        }
        e= list_next(e);
    }

    return -1;

}
