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

int PIDGLOBAL = 50;

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
int ret_from_fork () {
    return 0;
}
int sys_getpid()
{
	return current()->PID;
}

int sys_fork()
{

  
  if (list_empty(&free_queue)) return -11; //Get a free task_struct for the process. If there is no space for a new process, an  error will be returned. EAGAIN
      
    struct list_head *hijo = list_first(&free_queue); //primer elemento de la queue!!!
    list_del(hijo);
    
    struct task_struct *task_hijo = list_entry(hijo,struct task_struct, anchor); //   list_head_to_task_struct(hijo); //struct of the idle
    union task_union *hijo_task_union = (union task_union*)task_hijo; //COMO CONSTRUYO EL TASK_UNION??
    
    
    //task del padre
    struct task_struct *task_padre = current();
    union task_union *padre_task_union = (union task_union *)task_padre;
    //pasamos info
    copy_data(padre_task_union, hijo_task_union,sizeof(union task_union));
    //copy_from_user(&current(),&task_hijo, 4096);
    
    allocate_DIR(task_hijo);
    
    
    
    
    //Returns the frame number or -1 if there isn't any frame available. */
    //we need NUM_PAG_DATA==20 paginas para datos,por lo que reservamos tantas paginas
    int framesData[NUM_PAG_DATA];
    for (i = 0; i < NUM_PAG_DATA; i++){
        framesData[i] = alloc_frame();
        if (framesData[i] == -1) {//not available and whe should free space
            while(i != 0 ) {
                    free_frame(framesData[--i]);
            
            }
            return -11; //error
                    
        }
    }
    //hacer cosas con la TP 
    page_table_entry *hijo_pt, *padre_pt;
    hijo_pt = get_PT(task_hijo);
    padre_pt = get_PT(task_padre);
    //KERNEL - CODIGO - DATOS - PILA , KERNEL Y CODIGO SE COMPARTEN
    for (int i = 0; i < NUM_PAG_KERNEL + NUM_PAG_CODE; i++) {
            int frame = get_frame(padre_pt,i);
            //asignar frame al hijo
            set_ss_pag(hijo_pt,i,frame)
    }
    int Data_Start = NUM_PAG_KERNEL + NUM_PAG_CODE;
    //ahora asociamos las paginas logicas (i) con los frames que reservamos (framesData) y no son compartidas
    for (int i=0; i < NUM_PAG_DATA; i++ ) {
            set_ss_pag(hijo_pt, Data_Start + i, framesData[i]);
    }
    
    // COPIAR DATOS+PILA DEL PADRE AL HIJO
    // ASIGNAR TEMPORALMENTE MEMORIA FISICA AL PADRE QUE SERA LUEGO PARA EL HIJO
    int TEMP_DATA = NUM_PAG_DATA + Data_Start;
    for (int i = 0 ; i < NUM_PAG_DATA; i++) {
        set_ss_pag(padre_pt, i + TEMP_DATA, framesData[i]);
        //conversion (int -> void *)
        copy_data((void *)((Data_Start + i)<<12), (void *)((TEMP_DATA + i)<<12), PAGE_SIZE);
        del_ss_pag(padre_pt, i + TEMP_DATA);
    }
    //Writes on CR3 register producing a TLB flush 
    set_cr3(get_DIR(task_padre)); //disable the parent process to access the child pages.
    //PID
    task_hijo->PID = ++PIDGLOBAL;
    task_hijo->task.estado=ST_READY;
    //PREPARE CHILD STACK
    // oldss ... ebx 16 posiciones 17 -> @reth , 18 -> ret_from_fork, 19 -> 0 ebp task_switch
    hijo_task_union->stack[KERNEL_STACK_SIZE-19] =  0;
    hijo_task_union->stack[KERNEL_STACK_SIZE-18] = (unsigned int)ret_from_fork;
    task_hijo->kernel_esp = (unsigned long *)&hijo_task_union->stack[KERNEL_STACK_SIZE-19];
        

    
    // meter hijo en la cola de ready
   list_add_tail(&task_hijo->anchor, &readyqueue);	
	
    return task_hijo->PID;
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
    //Free  the  data  structures  and  resources  of  this  process  (physical  memory,  task_struct, and so). It uses the free_frame function to free physical pages
     
   page_table_entry *PT_actual = get_PT(current());
   for (int i = 0; i < NUM_PAG_DATA; ++i) {
      //void free_frame(unsigned int frame) --> unsigned int get_frame(PT,unsigned int page)
       
      free_frame(get_frame(PT_actual,PAGE_LOG_INIT_DATA+i)); 
      del_ss_pag(PT_actual,PAGE_LOG_INIT_DATA+i);
      
   }
   list_add_tail(&(current()->anchor),&free_queue) //enviarlo a la free_queue free!!!!
   
   update_sched_data_rr(); //Use the scheduler interface to select a new process to be executed and make a context switch
}
