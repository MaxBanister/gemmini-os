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
  # send_ipi_many takes a pointer to a bit mask
  # of harts to send interrupts to; 2 is hart 1
  la a0, tinyrocket_hart_no
  # a1 doesn't matter in this case
  li a7, SBI_SEND_IPI
  ecall

1:wfi
  j 1b

  li a7, SBI_SHUTDOWN
  ecall

  .data
tinyrocket_hart_no:
  .align 3
  .word 2
str:
  .asciz "00000000000\n"

