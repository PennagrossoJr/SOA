#include <asm.h>


ENTRY(task_switch)

  pushl %ebp
  movl %esp,%ebp

  #no hace falta????
  movl 0(%ebp),%ebx
  movl 12(%ebp),%esi
  movl 16(%ebp),%edi

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
  
ENTRY(inner_task_switch)
  
  #DINAMY LINK
  pushl %ebp
  movl %esp,%eb
  
  #switch_context
  
  
  
  ret
