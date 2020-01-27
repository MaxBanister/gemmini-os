// See LICENSE for license details.

#include "mcall.h"
#include "encoding.h"
#include "bits.h"

  .macro save_tf
  # save gprs
  STORE  x1,1*REGBYTES(x2)
  STORE  x3,3*REGBYTES(x2)
  STORE  x4,4*REGBYTES(x2)
  STORE  x5,5*REGBYTES(x2)
  STORE  x6,6*REGBYTES(x2)
  STORE  x7,7*REGBYTES(x2)
  STORE  x8,8*REGBYTES(x2)
  STORE  x9,9*REGBYTES(x2)
  STORE  x10,10*REGBYTES(x2)
  STORE  x11,11*REGBYTES(x2)
  STORE  x12,12*REGBYTES(x2)
  STORE  x13,13*REGBYTES(x2)
  STORE  x14,14*REGBYTES(x2)
  STORE  x15,15*REGBYTES(x2)
  STORE  x16,16*REGBYTES(x2)
  STORE  x17,17*REGBYTES(x2)
  STORE  x18,18*REGBYTES(x2)
  STORE  x19,19*REGBYTES(x2)
  STORE  x20,20*REGBYTES(x2)
  STORE  x21,21*REGBYTES(x2)
  STORE  x22,22*REGBYTES(x2)
  STORE  x23,23*REGBYTES(x2)
  STORE  x24,24*REGBYTES(x2)
  STORE  x25,25*REGBYTES(x2)
  STORE  x26,26*REGBYTES(x2)
  STORE  x27,27*REGBYTES(x2)
  STORE  x28,28*REGBYTES(x2)
  STORE  x29,29*REGBYTES(x2)
  STORE  x30,30*REGBYTES(x2)
  STORE  x31,31*REGBYTES(x2)

  # get sr, epc, badvaddr, cause
  csrrw  t0,sscratch,x0
  csrr   s0,sstatus
  csrr   t1,sepc
  csrr   t2,sbadaddr
  csrr   t3,scause
  STORE  t0,2*REGBYTES(x2)
  STORE  s0,32*REGBYTES(x2)
  STORE  t1,33*REGBYTES(x2)
  STORE  t2,34*REGBYTES(x2)
  STORE  t3,35*REGBYTES(x2)

  # get faulting insn, if it wasn't a fetch-related trap
  li x5,-1
  STORE x5,36*REGBYTES(x2)
1:
  .endm

  .section ".text.init"
  .globl _start
_start:
1:
  # write stvec
  la t0, trap_entry
  csrw stvec, t0
  csrw sscratch, x0
  csrwi sie, MIP_SSIP
  li t0, SSTATUS_SUM | SSTATUS_FS | SSTATUS_SIE
  csrw sstatus, t0
  la sp, kstack_end

  //la a0, str
  //jal ra, janky_print

1:wfi
  #la a0, str2
  #jal janky_print
  j 1b

  # just in case
  li a7, SBI_SHUTDOWN
  ecall

  .align 2
  .global  trap_entry
trap_entry:
  li t3, 0x1337
  la a0, str3
  jal ra, janky_print

  csrrw sp, sscratch, sp
  bnez sp, 1f
  # Trap came from S-mode
  csrrw sp, sscratch, sp
1:addi sp,sp,-320
  save_tf
  mv  a0,sp

  jal ra, handle_trap
  //ebreak

  la a0, str4
  jal ra, janky_print

  mv a0,sp
  # don't restore sscratch if trap came from kernel
  #andi s0,s0,SSTATUS_SPP
  #bnez s0,start_user
  addi sp,sp,320
  csrw sscratch,sp

  li a7, SBI_CLEAR_IPI
  ecall

  # wait for the next interrupt
1:wfi
  j 1b
  
  .global gemmini_finish
gemmini_finish:
  li a7, SBI_GEMMINI_FIN
  ecall
  ret

  .global janky_print
janky_print:
  mv a1, a0
1:lbu a0, (a1)
  beqz a0, 1f
  li a7, SBI_CONSOLE_PUTCHAR
  ecall
  add a1, a1, 1
  j 1b
1:ret
  

  .data
str:
  .asciz "11111111\n"
str2:
  .asciz "GGGGGGGGG"
str3:
  .asciz "+"
str4:
  .asciz "After handle_trap\n"

  .bss
  .align RISCV_PGSHIFT
kstack:
  .skip RISCV_PGSIZE * 2
kstack_end:
