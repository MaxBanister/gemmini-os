// See LICENSE for license details.

#include "syscall_tr.h"
#include "pk.h"
#include "frontend_tr.h"
#include <string.h>
#include <errno.h>

typedef long (*syscall_t)(long, long, long, long, long, long, long);

#define CLOCK_FREQ 1000000000

void sys_exit(int code)
{
  /*if (current.cycle0) {
    size_t dt = rdtime() - current.time0;
    size_t dc = rdcycle() - current.cycle0;
    size_t di = rdinstret() - current.instret0;

    printk("%ld ticks\n", dt);
    printk("%ld cycles\n", dc);
    printk("%ld instructions\n", di);
    printk("%d.%d%d CPI\n", dc/di, 10ULL*dc/di % 10, (100ULL*dc + di/2)/di % 10);
  }*/
  shutdown(code);
}

static int sys_stub_success()
{
  return 0;
}

static int sys_stub_nosys()
{
  return -ENOSYS;
}

long do_syscall(long a0, long a1, long a2, long a3, long a4, long a5, unsigned long n)
{
  const static void* syscall_table[] = {
    [SYS_exit] = sys_exit,
    [SYS_exit_group] = sys_stub_nosys,
    [SYS_read] = sys_stub_nosys,
    [SYS_pread] = sys_stub_nosys,
    [SYS_write] = sys_stub_nosys,
    [SYS_openat] = sys_stub_nosys,
    [SYS_close] = sys_stub_nosys,
    [SYS_fstat] = sys_stub_nosys,
    [SYS_lseek] = sys_stub_nosys,
    [SYS_fstatat] = sys_stub_nosys,
    [SYS_linkat] = sys_stub_nosys,
    [SYS_unlinkat] = sys_stub_nosys,
    [SYS_mkdirat] = sys_stub_nosys,
    [SYS_renameat] = sys_stub_nosys,
    [SYS_getcwd] = sys_stub_nosys,
    [SYS_brk] = sys_stub_nosys,
    [SYS_uname] = sys_stub_nosys,
    [SYS_getpid] = sys_stub_nosys,
    [SYS_getuid] = sys_stub_nosys,
    [SYS_geteuid] = sys_stub_nosys,
    [SYS_getgid] = sys_stub_nosys,
    [SYS_getegid] = sys_stub_nosys,
    [SYS_mmap] = sys_stub_nosys,
    [SYS_munmap] = sys_stub_nosys,
    [SYS_mremap] = sys_stub_nosys,
    [SYS_mprotect] = sys_stub_nosys,
    [SYS_prlimit64] = sys_stub_nosys,
    [SYS_rt_sigaction] = sys_stub_nosys,
    [SYS_gettimeofday] = sys_stub_nosys,
    [SYS_times] = sys_stub_nosys,
    [SYS_writev] = sys_stub_nosys,
    [SYS_faccessat] = sys_stub_nosys,
    [SYS_fcntl] = sys_stub_nosys,
    [SYS_ftruncate] = sys_stub_nosys,
    [SYS_getdents] = sys_stub_nosys,
    [SYS_dup] = sys_stub_nosys,
    [SYS_readlinkat] = sys_stub_nosys,
    [SYS_rt_sigprocmask] = sys_stub_nosys,
    [SYS_ioctl] = sys_stub_nosys,
    [SYS_clock_gettime] = sys_stub_nosys,
    [SYS_getrusage] = sys_stub_nosys,
    [SYS_getrlimit] = sys_stub_nosys,
    [SYS_setrlimit] = sys_stub_nosys,
    [SYS_chdir] = sys_stub_nosys,
    [SYS_set_tid_address] = sys_stub_nosys,
    [SYS_set_robust_list] = sys_stub_nosys,
    [SYS_madvise] = sys_stub_nosys,
  };

  const static void* old_syscall_table[] = {
    [-OLD_SYSCALL_THRESHOLD + SYS_open] = sys_stub_nosys,
    [-OLD_SYSCALL_THRESHOLD + SYS_link] = sys_stub_nosys,
    [-OLD_SYSCALL_THRESHOLD + SYS_unlink] = sys_stub_nosys,
    [-OLD_SYSCALL_THRESHOLD + SYS_mkdir] = sys_stub_nosys,
    [-OLD_SYSCALL_THRESHOLD + SYS_access] = sys_stub_nosys,
    [-OLD_SYSCALL_THRESHOLD + SYS_stat] = sys_stub_nosys,
    [-OLD_SYSCALL_THRESHOLD + SYS_lstat] = sys_stub_nosys,
    [-OLD_SYSCALL_THRESHOLD + SYS_time] = sys_stub_nosys,
  };

  syscall_t f = 0;

  if (n < ARRAY_SIZE(syscall_table))
    f = syscall_table[n];
  else if (n - OLD_SYSCALL_THRESHOLD < ARRAY_SIZE(old_syscall_table))
    f = old_syscall_table[n - OLD_SYSCALL_THRESHOLD];

  if (!f)
    sys_exit(1);

  return f(a0, a1, a2, a3, a4, a5, n);
}
