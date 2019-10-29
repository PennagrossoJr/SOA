#include <asm.h>


ENTRY(task_switch)

  pushl %ebp
  movl %esp,%ebp

  #no hace falta????
  #movl 0(%ebp),%ebx
  #movl 12(%ebp),%esi
  #movl 16(%ebp),%edi

  # save ESI, EDI and EBX

  pushl %esi
  pushl %edi
  pushl %ebx

  call inner_task_switch

  popl %ebx
  popl %edi
  popl %esi
  
  popl %ebp

  ret
  
ENTRY(inner_task_switch2)
  
  #DINAMY LINK
  pushl %ebp
  movl %esp,%ebp
  
  #switch_context (kernel_esp <- ebp)
  movl 4(%esp), %eax
  movl %ebp, (%eax) #cambia el stack de la pila del current con el nuevo
  movl 8(%esp), %esp
  
  popl %ebp  
  
  ret
