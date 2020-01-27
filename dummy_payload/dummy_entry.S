// See LICENSE for license details.

#include "mcall.h"
#include "mtrap.h"

  .section ".text.init"
  .globl _start
_start:
  # write stvec
  la t0, trap_entry
  csrw stvec, t0
  csrwi sie, MIP_SSIP
  li t0, SSTATUS_SUM | SSTATUS_FS | SSTATUS_SIE
  csrw sstatus, t0

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
  li a0, 0x11111111
  li a1, 0x22222222
  li a7, SBI_GEMMINI_EXEC
  ecall

/* Wait for gemmini to do its thing */
1:wfi
  j 1b

  li a7, SBI_SHUTDOWN
  ecall

  .align 2
  .global trap_entry
trap_entry:
  la s0, str1
1:
  lbu a0, (s0)
  beqz a0, 1f
  li a7, SBI_CONSOLE_PUTCHAR
  ecall
  add s0, s0, 1
  j 1b
1:
  wfi
  j 1b

  .data
tinyrocket_hart_no:
  .align 3
  .word 2
str:
  .asciz "00000000000\n"
str1:
  .asciz "99999999999\n"
