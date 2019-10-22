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
    list_del(list_first(&freequeue));
    //list_del(e); //eliminamos de free_queue
    idle_task = list_head_to_task_struct(e); //struct of the idle
    union task_union *idle_task_union = (union task_union*)idle_task; //COMO CONSTRUYO EL TASK_UNION??

    idle_task->PID = 0;
    allocate_DIR(idle_task); //inicializo con nuevo directory

    idle_task_union->stack[KERNEL_STACK_SIZE] = &cpu_idle; //stack vacia??

    //Initialize the global variable idle_task, which will help to get easily the task_struct of the idle process.

}

void init_task1(void)
{

    struct list_head *e = list_first(&free_queue);
    struct task_struct *e1 = list_head_to_task_struct(e);
    union task_union *union_task1 = (union task_union*)e1; //o task struct
    list_del(list_first(&freequeue));

    e1->PID = 1;
    allocate_DIR(e1);
    set_user_pages(e1);
    e1->PT = get_PT(e1);
    setTSS().tss.esp0 = union_task1 -> stack[KERNEL_STACK_SIZE]; 
   
    set_cr3(e1->dir_pages_baseAddr); 


    list_add(&(el.anchor),&free_queue); //añadimos el proceso a la free_queue

}


void init_sched()
{

}

void init_queues()
{
  INIT_LIST_HEAD(&free_queue); //inicializar la struct!!!
  for(int i =0; i < NR_TASKS ; i++ ) {
      list_add_tail( &task[i].task.anchor, &free_queue)
  }
  INIT_LIST_HEAD(&ready_queue); //inicializar la struct!!!
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
