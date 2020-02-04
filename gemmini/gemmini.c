#include "gemmini.h"
#include "mcall.h"

extern void gemmini_get_args(void);
extern void gemmini_finish(void);
extern void mcall_puts(const char *);

static void trap_stub(trapframe_t* tf) {
  // Exit quietly
  return;
}

void vprintk(const char* s, va_list vl)
{
  char buf[256];
  vsnprintf(buf, sizeof buf, s, vl);
  mcall_puts(buf);
}

void printk(const char* s, ...)
{
  va_list vl;

  va_start(vl, s);
  vprintk(s, vl);
  va_end(vl);
}

static void handle_interrupt(trapframe_t* tf)
{
  long satp, fp;
  register uintptr_t a0 asm("a0");
  register uintptr_t a1 asm("a1");

  clear_csr(sip, SIP_SSIP);
  
  printk("Girl, interrupted\n");

  /* Magically populates a0 and a1 */
  gemmini_get_args();

  satp = a0, fp = a1;
  if (!(fp)) {
    printk("Interrupted without HLS fields set\n");
    return;
  }

  printk("satp: %lx\n", satp);
  printk("fp: %lx\n", fp);

  /* Return to sender */
  gemmini_finish();
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
    return;
  }

  printk("Exception cause value: %d\n", tf->cause);

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
