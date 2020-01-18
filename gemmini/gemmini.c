#include "gemmini.h"

extern void janky_print(char *string);

static void trap_stub(trapframe_t* tf) {
  // Exit quietly
  //do_syscall(1, 0, 0, 0, 0, 0, 0);
}

static void handle_interrupt(trapframe_t* tf)
{
  clear_csr(sip, SIP_SSIP);
  
  janky_print("Girl, interrupted\n");
}

static void handle_syscall(trapframe_t* tf)
{
  /*tf->gpr[10] = do_syscall(tf->gpr[10], tf->gpr[11], tf->gpr[12], tf->gpr[13],
                           tf->gpr[14], tf->gpr[15], tf->gpr[17]);
  */
  if (tf->gpr[10] == 0)
    ; // User program exited

  tf->epc += 4; // Skip faulting instruction

}

void handle_trap(trapframe_t* tf)
{
  if ((intptr_t)tf->cause < 0)
    return handle_interrupt(tf);

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

