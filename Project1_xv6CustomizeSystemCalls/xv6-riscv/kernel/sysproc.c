#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "shm.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

// System call: shmget
uint64 
sys_shmget(void) 
{
    int key, size, flags;
    argint(0, &key);
    argint(1, &size);
    argint(2, &flags);
    return shmget(key, size, flags);
}

// System call: shmat
uint64 
sys_shmat(void) 
{
    int shmid;
    argint(0, &shmid);
    return shmat(shmid);
}

// System call: shmdt
uint64 
sys_shmdt(void) 
{
    int shmid;
    argint(0, &shmid);

    return shmdt(shmid);
}

// System call: shmctl
uint64 
sys_shmctl(void) 
{
    int shmid, cmd;
    argint(0, &shmid);
    argint(1, &cmd);

    return shmctl(shmid, cmd);
}

// System call : getprocinfo
uint64 
sys_procinfo(void) {
  struct proc *p = myproc();  // Get the current process
  
  // Extract PID, state, and priority
  int pid = p->pid;
  int ppid = p->parent ? p->parent->pid : -1;
  int state = p->state;

  static const char *state_names[] = {
    "UNUSED",    // 0
    "USED",       // 1
    "SLEEPING",   // 2
    "RUNNABLE",   // 3
    "RUNNING",    // 4
    "ZOMBIE"      // 5
  };

  // Print the process info
  printf("PID: %d, Parent PID: %d, State: %s\n", pid, ppid, state_names[state]);
  
  return 0;
}

// System call : raise
uint64
sys_raise(void) {
    int signum;
    argint(0, &signum);

    if (signum < 0 || signum >= 32)
        return -1;

    struct proc *p = myproc();
    p->signals[signum].pending = 1; // Mark signal as pending
    yield();
    printf("Signal %d raised\n", signum);
    return 0;
}