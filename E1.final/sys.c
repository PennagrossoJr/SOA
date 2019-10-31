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

  if (list_empty(&free_queue)) { //Get a free task_struct for the process. If there is no space for a new process, an  error will be returned.

    struct list_head *hijo = list_first(&free_queue); //primer elemento de la queue!!!
    list_del(list_first(&free_queue));

    task_struct *task_hijo = current(); //   list_head_to_task_struct(hijo); //struct of the idle
    union task_union *hijo_task_union = (union task_union*)task_hijo; //COMO CONSTRUYO EL TASK_UNION??

    //Inherit system data: copy the parent’s task_union to the child.
    copy_from_user(&current(),&task_hijo, 4096);
    //Initialize field dir_pages_baseAddr
    allocate_DIR(task_hijo);

    //Search physical pages in which to map logical pages for data+stack of the child process (using the alloc_frames function).
    //Returns the frame number or -1 if there isn't any frame available.


    if (alloc_frame_aux == -1) return -1; //-1???

    for(int i=0; i < NUM_PAG_DATA; ++i) {

      int alloc_frame_aux = alloc_frame();
      if (alloc_frame_aux == -1) {
        list_add(hijo,&free_queue); //se le añade a la free queue ya que no tiene frame suficientes!!!!
        return -EAGAIN;
      }
      else set_ss_pag(new_PT, PAG_LOG_INIT_DATA + i, i); //void set_ss_pag(page_table_entry *PT, unsigned page,unsigned frame);

    }

    //Create new address space
    page_table_entry *new__PT = get_PT(task_hijo); //Returns the Page Table address
    page_table_entry *parent_PT = get_PT(current());   //Returns the Page Table address

    //Page table entries for the system code and data and for the user code can be a copy of the page table entries of the parent process

    for (int i=0; i<NUM_PAG_KERNEL; ++i) {
      set_ss_pag(new_PT, i, get_frame(parent_PT, i));
    }
    for (int i=0; i<NUM_PAG_CODE; ++i) {
      set_ss_pag(new_PT, PAG_LOG_INIT_CODE+i, get_frame(parent_PT, PAG_LOG_INIT_CODE+i));
    }

    


  return PID;
}

return error; //buscar que error es!!!


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

/*void sys_task_switch(struct task_union *new) { //no se si esto se tiene que hacer aqui!!!!

    //cambio de tss
    tss.esp0 = (int)&(new->stack[KERNEL_STACK_SIZE]); //1024

    //cambio cr3
    set_cr3(get_DIR(&new->task))//set_cr3(new->task->*dir_pages_baseAddr);+


    // luego se vuelve al wrapper para cambiar el kernel ebp al del new proceso
    inner_task_switch();

}*/


void sys_exit()
{
}
