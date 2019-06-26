// See LICENSE for license details.

#include "mcall.h"
#include "mtrap.h"

  .section ".text.init"
  .globl _start
_start:
  la s0, str
1:
  lbu a0, (s0)
  beqz a0, 1f
  li a7, SBI_CONSOLE_PUTCHAR
  ecall
  add s0, s0, 1
  j 1b

1:
  li a0, 1
  li a1, MIP_SSIP
  call send_ipi

  #li a7, SBI_SHUTDOWN
  #ecall

  .data
str:
  .asciz "Hart 0: Sending interrupt to other core..."

