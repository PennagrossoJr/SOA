#include <asm.h>

ENTRY(write)

  pushl %ebp
  movl %esp,%ebp

  # Parameter passing: 3 parameters

  pushl %ebx //fb
  pushl %ecx //buffer
  pushl %edx //size
  movl 0(%ebp),%ebx
  movl 4(%ebp),%ecx
  movl 8(%ebp),%edx

  # Put the identifier of the system call in the EAX register (number 4 for write)

  movl $0x04,%eax

  # Generate the trap: int $0x80

  int $0x80

  # error si es numero negativo!

  cmpl $0, %eax
  jl error
  popl %edx
  popl %ecx
  popl %ebx
  popl %ebp

error:
  # will save the absolute value of the return in an error variable defined ( errno )
  neg  %eax
  movl %eax,errno

  # return -1

  movl $-1, %eax

  ret


ENTRY(gettime)

  pushl %ebp
  movl %esp,%ebp

  movl $0x0A,%eax
  int $0x80

  cmpl $0, %eax
  jl error_gettime

error_gettime:

  neg  %eax
  movl %eax,errno
  movl $-1, %eax

  popl %ebp
  ret
