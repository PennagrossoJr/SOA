/*
 * sys.c - Syscalls implementation
 */
#include <devices.h>

#include <utils.h>

#include <io.h>

#include <mm.h>

#include <mm_address.h>

#include <sched.h>

#define LECTURA 0
#define ESCRIPTURA 1

int check_fd(int fd, int permissions)
{
  if (fd!=1) return -9; /*EBADF*/
  if (permissions!=ESCRIPTURA) return -13; /*EACCES*/
  return 0;
}

int sys_ni_syscall()
{
	return -38; /*ENOSYS*/
}

int sys_getpid()
{
	return current()->PID;
}

int sys_fork()
{
  int PID=-1;

  struct list_head *hijo = list_first(&free_queue); //primer elemento de la queue!!!
  list_del(list_first(&freequeue));
  
  task_struct *task_hijo = current(); //   list_head_to_task_struct(hijo); //struct of the idle
  union task_union *hijo_task_union = (union task_union*)task_hijo; //COMO CONSTRUYO EL TASK_UNION??
  
  copy_from_user(&current(),&task_hijo, 4096);
  allocate_DIR(task_hijo);
  //Returns the frame number or -1 if there isn't any frame available. */
  int alloc_frame_aux = alloc_frame();
  if (alloc_frame_aux == -1) return error;
  else  {
   	  task_hijo->PT = current()->PT;
   	  
  }
  	
	
  return PID;
}

int sys_gettime() {
 // return zeos_ticks;
 return get_ticks();
}

int sys_write(int fd, char *buffer, int size) {
  printk("interrupcion de teclado dentro del teclado sys, escritura");	
  //Check the fd
  int c_fd = check_fd(fd,ESCRIPTURA);
  /*if (c_fd < 0) return c_fd;

  //Check buffer
  if (buffer == NULL) //hace algo!!!
  else //hace otra cosa

  //check the size
  if (size > 0) {

  }
  else*/
  if (c_fd >= 0 && buffer != NULL && size > 0) {
    //printk("No hay fallo!!!");
    //que tenemos que escribir!!!!
    
    char aux_buffer[4096];
    int size_aux = size;
    while (size_aux > 0) {
        copy_from_user(buffer,aux_buffer,size_aux)
        sys_write_console (aux_buffer,size);
        size_aux -= size_aux;
    }
    return sys_write_console (buffer,size);
  }
  else if (c_fd < 0) return c_fd; //errores de los demas¿?
  else if (buffer == NULL) return -6;//valor del error --> EFAULT
  else return -22; //EINVAL
      

}

void sys_task_switch(struct task_union *new) {
    
    set_cr3(new->task->*dir_pages_baseAddr);
    tss.esp0 = & (*new).stack;
    // luego se vuelve al wrapper para cambiar el kernel ebp al del new proceso
}


void sys_exit()
{
}
