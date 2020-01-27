#include "gemmini.h"
#include "mtrap.h"

extern void janky_print(char *string);
extern void gemmini_finish(void);

static void trap_stub(trapframe_t* tf) {
  // Exit quietly
  //do_syscall(1, 0, 0, 0, 0, 0, 0);
}

static void handle_interrupt(trapframe_t* tf)
{
  long satp, fp, from_hart;
  clear_csr(sip, SIP_SSIP);
  
  janky_print("Girl, interrupted\r\n");

  satp = HLS()->satp, fp = HLS()->fp, from_hart = HLS()->from_hart;

  if (!(satp && fp)) {
    janky_print("Interrupted without HLS fields set\r\n");
    return;
  }

  janky_print("Satp first byte:\r\n");
  uint64_t x = 0;
  do {
    x = ((satp % 10) + 48);
    janky_print((char *)&x); 
    satp /= 10;
  } while (satp > 0);
  janky_print("\r\nFp first byte:\r\n");
  x = 0;
  do {
    x = ((fp % 10) + 48);
    janky_print((char *)&x); 
    fp /= 10;
  } while (fp > 0);
  janky_print("\r\nFrom hart first byte:\r\n");
  x = 0;
  do {
    x = ((from_hart % 10) + 48);
    janky_print((char *)&x); 
    from_hart /= 10;
  } while (from_hart > 0);
  janky_print("\r\n");

  /* Return to sender */
  asm("li t0, 0x1337");
  gemmini_finish();
  janky_print("After gemmini_finish\n");
}

static void handle_syscall(trapframe_t* tf)
{
  /*tf->gpr[10] = do_syscall(tf->gpr[10], tf->gpr[11], tf->gpr[12], tf->gpr[13],
                           tf->gpr[14], tf->gpr[15], tf->gpr[17]);
  */
  if (tf->gpr[10] == 0)
    // User program exited
    gemmini_finish(); 

  tf->epc += 4; // Skip faulting instruction
}

void handle_trap(trapframe_t* tf)
{
  if ((intptr_t)tf->cause < 0) {
    handle_interrupt(tf);
    janky_print("After handle_interrupt\n");
    return;
  }

  int x = tf->cause + 48;
  janky_print((char *)&x);
  janky_print("\nExc\n");

  typedef void (*trap_handler)(trapframe_t*);

  const static trap_handler trap_handlers[] = {
    [CAUSE_MISALIGNED_FETCH] = trap_stub,
    [CAUSE_FETCH_ACCESS] = trap_stub,
    [CAUSE_LOAD_ACCESS] = trap_stub,
    [CAUSE_STORE_ACCESS] = trap_stub,
    [CAUSE_FETCH_PAGE_FAULT] = trap_stub,
    [CAUSE_ILLEGAL_INSTRUCTION] = trap_stub,
    [CAUSE_USER_ECALL] = handle_syscall,
    [CAUSE_BREAKPOINT] = trap_stub,
    [CAUSE_MISALIGNED_LOAD] = trap_stub,
    [CAUSE_MISALIGNED_STORE] = trap_stub,
    [CAUSE_LOAD_PAGE_FAULT] = trap_stub,
    [CAUSE_STORE_PAGE_FAULT] = trap_stub,
  };

  //kassert(tf->cause < ARRAY_SIZE(trap_handlers) && trap_handlers[tf->cause]);

  trap_handlers[tf->cause](tf);
}
