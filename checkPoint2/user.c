#include <libc.h>
#include <stats.h>

#define POLITICA 0
#define TYPE 0
char buff[24];


void workCPU() {
    int aux = 1;
    int i;
    for(i=0 ; i<1000; i++) {
        aux = aux*aux*aux + 1; //work for CPU
    }
}

void workIO() {
    int i;
    int buff[2];
    for(i = 0; i < 200 ; i++ ) {
        read(0, buff, 3); //emulates console reading, blocking 
    }
}

void writeSta(int pid, int type) {
      struct stats aux;
      int p = 0;
      int buff[200];
      if(get_stats(pid, &aux) == -1) perror();
      else {
        itoa(type, buff);
        p = strlen(buff);
        buff[p] = ' ';
        itoa(aux.user_ticks, &(buff[p+1]));
        p = strlen(buff);
        buff[p] = ' ';
        itoa(aux.blocked_ticks, &(buff[p+1]));
        p = strlen(buff);
        buff[p] = ' ';
        itoa(aux.ready_ticks, &(buff[p+1]));
        p = strlen(buff);
        buff[p] = ' ';
        itoa(aux.system_ticks, &(buff[p+1]));
        p = strlen(buff);
        buff[p] = '\n';
        write(1, buff, p+1);
      }
}


void select(int i) {
    if(i == 0) workCPU();
    else {
        workIO();
    }
}
int __attribute__ ((__section__(".text.main")))
  main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */


    if(POLITICA == 0) set_sched_policy(0); //RR
    else set_sched_policy(1);
    
    int pid;
    pid = fork();
    if(pid) { //padre
        pid = fork();
        if(pid) {
            select(TYPE);
        }   
        else{  //hijo2
            select(TYPE);
        }
    }
	else { //hijo
        select(TYPE);
    }

	
	
}
