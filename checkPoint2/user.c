#include <libc.h>
#include <stats.h>

#define POLITICA 1
#define TYPE 0
char buff[24];


/*void workCPU() {
    
    int aux = 1;
    int i;
    for(i=0 ; i<1000; i++) {
        aux = aux*aux*aux + 1; //work for CPU
    }
    writeSta(getpid(), 0);
}*/

/*void workIO() {
    int i;
    int buff[2];
    for(i = 0; i < 200 ; i++ ) {
        read(0, buff, 3); //emulates console reading, blocking 
    }
    writeSta(getpid(), 1);
}*/

void writeSta(int pid, int type) {
      struct stats aux;
      int p = 0;
      char buff[200];
      if(get_stats(pid, &aux) == -1) perror();
      else {
        char buffer3[64] = "Pintenando los valores del stat!!!";
        write(1,buffer3,64);
        
        itoa(type, buff);
        p = strlen(buff);
        buff[p] = ' ';
        itoa(aux.user_ticks, &(buff[p+1]));//user_ticks
        p = strlen(buff);
        buff[p] = ' ';
        itoa(aux.blocked_ticks, &(buff[p+1])); //blocked_ticks
        p = strlen(buff);
        buff[p] = ' ';
        itoa(aux.ready_ticks, &(buff[p+1])); //ready_ticks
        p = strlen(buff);
        buff[p] = ' ';
        itoa(aux.system_ticks, &(buff[p+1]));//system_ticks
        p = strlen(buff);
        buff[p] = '\n';
        write(1, buff, p+1);
        
        char buffer4[64] = "Acabanado de printear los valores del stat!!!";
        write(1,buffer4,64);
      }
}


/*void select(int i) {
    if(i == 0) workCPU();
    else {
        workIO();
    }
    char buffer57[64] = "acabado el select ";
    write(1,buffer57,64);
}*/

int __attribute__ ((__section__(".text.main")))
  main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */
    
     
     
    if(POLITICA == 0) set_sched_policy(0); //RR
    else set_sched_policy(1); //FCFS
    
    int pid;
    pid = fork();
    
    if(pid) { //padre
        pid = fork();
        if(pid) {
            //select(TYPE);
            
            /* Workload 1
            int aux = 1;
            int i;
            for(i=0 ; i<1000; i++) {
                aux = aux*aux*aux + 1; //work for CPU
            }
            writeSta(getpid(), 0);*/
            
            /* Workload 2
            int i;
            int buff[2];
            for(i = 0; i < 200 ; i++ ) {
                read(0, buff, 3); //emulates console reading, blocking 
            }
            writeSta(getpid(), 1);*/
            
            
            //Workload 3
            int aux = 1;
            int ix;
            for(ix=0 ; ix<10000; ix++) {
                aux = aux*aux*aux + 1; //work for CPU
            }
                    
                    int i;
            int buff[2];
            for(i = 0; i < 2 ; i++ ) {
                read(0, buff, 3); //emulates console reading, blocking 
            }
            writeSta(getpid(), 0);
            
            
            
            
            
            
            
            
            
        }   
        else{  //hijo2
            
             /* Workload 1
            int aux = 1;
            int i;
            for(i=0 ; i<1000; i++) {
                aux = aux*aux*aux + 1; //work for CPU
            }
            writeSta(getpid(), 0);*/
            
            /* Workload 2
            int i;
            int buff[2];
            for(i = 0; i < 200 ; i++ ) {
                read(0, buff, 3); //emulates console reading, blocking 
            }
            writeSta(getpid(), 1);*/

            //Workload 3
            int aux = 1;
            int ix;
            for(ix=0 ; ix<10000; ix++) {
                aux = aux*aux*aux + 1; //work for CPU
            }


            int i;
            int buff[2];
            for(i = 0; i < 2 ; i++ ) {
                read(0, buff, 3); //emulates console reading, blocking 
            }
            
            
            writeSta(getpid(), 0);
       
        }
    }
    
    
    else { //hijo
        //select(TYPE);
        
         /* Workload 1
            int aux = 1;
            int i;
            for(i=0 ; i<1000; i++) {
                aux = aux*aux*aux + 1; //work for CPU
            }
            writeSta(getpid(), 0);*/
            
            /* Workload 2
            int i;
            int buff[2];
            for(i = 0; i < 200 ; i++ ) {
                read(0, buff, 3); //emulates console reading, blocking 
            }
            writeSta(getpid(), 1);*/
        
        //Workload 3
        int i;
        int buff[2];
        for(i = 0; i < 2 ; i++ ) {
            read(0, buff, 3); //emulates console reading, blocking 
        }
        int aux = 1;
        int ix;
        for(ix=0 ; ix<1000; ix++) {
            aux = aux*aux*aux + 1; //work for CPU
        }   
        
    writeSta(getpid(), 1);
        
    }
    
	while (1) {}
	
}
