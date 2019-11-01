/*
 * sched.c - initializes struct for task 0 anda task 1
 */

#include <sched.h>
#include <mm.h>
#include <io.h>

union task_union task[NR_TASKS]
  __attribute__((__section__(".data.task")));

#if 0
struct task_struct *list_head_to_task_struct(struct list_head *l)
{
  return list_entry( l, struct task_struct, list);
}
#endif

extern struct list_head blocked;


/* get_DIR - Returns the Page Directory address for task 't' */
page_table_entry * get_DIR (struct task_struct *t)
{
	return t->dir_pages_baseAddr;
}

/* get_PT - Returns the Page Table address for task 't' */
page_table_entry * get_PT (struct task_struct *t)
{
	return (page_table_entry *)(((unsigned int)(t->dir_pages_baseAddr->bits.pbase_addr))<<12);
}


page_table_entry * allocate_DIR(struct task_struct *t)
{
	int pos;

	pos = ((int)t-(int)task)/sizeof(union task_union);

	return (page_table_entry*) &dir_pages[pos];
}

void cpu_idle(void)
{
	__asm__ __volatile__("sti": : :"memory");

	while(1)
	{
	;
	}
}

void init_idle (void)
{


    struct list_head *e = list_first(&free_queue); //primer elemento de la queue!!!
    list_del(list_first(&free_queue));
    //list_del(e); //eliminamos de free_queue
    idle_task = list_head_to_task_struct(e); //struct of the idle
    union task_union *idle_task_union = (union task_union*)idle_task; //COMO CONSTRUYO EL TASK_UNION??

    idle_task->PID = 0;
    idle_task->estado = ST_READY;
    idle_task->quantum = defaultQuantum;

    allocate_DIR(idle_task); //inicializo con nuevo directory

    idle_task_union->stack[KERNEL_STACK_SIZE-1] = &cpu_idle; //stack vacia??
    idle_task_union->stack[KERNEL_STACK_SIZE-2] = 0; //este es el ebp creo!!!!
    idle_task->kernel_esp =(int)&(idle_task_union->stack[KERNEL_STACK_SIZE-2]);

    //Initialize the global variable idle_task, which will help to get easily the task_struct of the idle process.

}

void init_task1(void)
{

    struct list_head *e = list_first(&free_queue);
    struct task_struct *e1 = list_head_to_task_struct(e);
    union task_union *union_task1 = (union task_union*)e1; //o task struct
    list_del(list_first(&free_queue));

    e1->PID = 1;
    allocate_DIR(e1);
    set_user_pages(e1);
    e1->PT = get_PT(e1);
    e1->quantum = defaultQuantum;
    e1->estado=ST_RUN;

    tss.esp0 = (int)&(union_task1 -> stack[KERNEL_STACK_SIZE-2]);

    set_cr3(e1->dir_pages_baseAddr); // == get_DIR(&e1->task)


    list_add_tail(&(e1->anchor),&free_queue); //añadimos el proceso a la free_queue // implementing queues

}

void inner_task_switch(union task_union*t) {

    tss.esp0 = (int)&(new->stack[KERNEL_STACK_SIZE-2]); //1024

    //cambio cr3
    set_cr3(get_DIR(&new->task))//set_cr3(new->task->*dir_pages_baseAddr);


    // luego se vuelve al wrapper para cambiar el kernel ebp al del new proceso
    inner_task_switch2(&(current()->kernel_esp), new->task.kernel_esp); //cambio de las stacks!!!
}


void init_sched()
{

}

void schedule() {
   update_sched_data_rr();
   if (needs_sched_rr() == 1) { //Function to decide if it is necessary to change the current process.
      //returns: 1 if it is necessary to change the current process and 0 otherwise
      update_process_state_rr(current(), &ready_queue); //struct task_struct *t, struct list_head *dst_queue);
      sched_next_rr();
   }
}

void update_sched_data_rr () {
    current()->quantum = (current()->quantum) - 1;
}

int needs_sched_rr () {
  //returns: 1 if it is necessary to change the current process and 0 otherwise
  if (current()->quantum == 0) {
    current()->quantum = defaultQuantum;
    if (list_empty(&ready_queue)) return 1; //necessary Change:  A context switch is required when the quantum is over and there are some candidate process to use the CPU
  }
  else  return 0;
}

void update_process_state_rr (struct task_struct *t, struct list_head *dst_queue) {

  //If the current state of the process is running, then there is no need to delete it from any queue
  if (t->estado != ST_RUN) {
    list_del(&(t->anchor)); //eliminarlo de la actual queue
    list_add_tail(&(t->anchor),dst_queue); //la añadimos en la cola dst!!!
  }

  // CAMBIO LOS ESTADOS DE LA ESTADO!!!!!
  if (dst_queue == &ready_queue)  t->estado = ST_READY;
  else if (dst_queue == NULL) {
      if (t->estado == ST_READY) ++((t->stadistical)->total_trans); //Total transitions ready --> run
      t->estado = ST_RUN;
  }
  else t->estado = ST_BLOCKED; //we have not implemented any blocking system call
  //else running!!! If the current state of the process is running, then there is no need to delete it from any queue.
}

void init_queues()
{
  INIT_LIST_HEAD(&free_queue); //inicializar la struct!!!
  for(int i =0; i < NR_TASKS ; i++ ) {
      list_add_tail( &task[i].task.anchor, &free_queue)
  }
  INIT_LIST_HEAD(&ready_queue); //inicializar la struct!!!

  defaultQuantum = 100; //valor???
}

void sched_next_rr() {

  current()->quantum = defaultQuantum;//solamete para cuando hago el cambio en el process destruccion!!!!!
  if (!list_empty(&ready_queue)) { // to extract it from the ready queue
    struct list_head *next = list_first(&ready_queue); //primer elemento de la queue --> el sguiente!!!
    list_del(list_first(&ready_queue));
    struct task_struct *next_task = list_head_to_task_struct(&next);
    union task_union *next_task_union = (union task_union*)next_task;
    task_switch(next_task_union); //invoke the context switch process.
  }
  else task_switch((union task_union*)idle_task); //sino no hay mas en la ready_queue, psasr al idle or free_queue??

}


void set_quantum (struct task_struct *t, int new_quantum){

    t->quantum = new_quantum;
}


int get_quantum (struct task_struct *t) {

    return t->quantum;

}

struct task_struct* current()
{
  int ret_value;

  __asm__ __volatile__(
  	"movl %%esp, %0"
	: "=g" (ret_value)
  );
  return (struct task_struct*)(ret_value&0xfffff000);
}
