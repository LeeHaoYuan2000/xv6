#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "sysinfo.h"

#include "syscall.h"

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

  backtrace();

  argint(0, &n);
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

//return 1 means set task mask success
uint64 sys_trace(void){
  int n;
  argint(0, &n);
  printf("trace bit is %d\n", n);
  return trace(n);
}

//get the sys_info
uint64 sys_info(void){
  uint64 info_addr;

argaddr(0, &info_addr);

  if(info_addr < 0){
    return -1;
  }

  return info(&info_addr);
}

int
sys_pgaccess(void)
{
  // lab pgtbl: your code here.
  uint64 start_va;
  argaddr(0, &start_va);

  int page_num;
  argint(1, &page_num);

  uint64 result_va;
  argaddr(2, &result_va) ;


  struct proc *p = myproc();
  if(pgaccess(p->pagetable,start_va,page_num,result_va) < 0)
    return -1;

  return 0;
}

uint64 sys_sigalarm(void){
  int alarm_time;
  uint64 handler;

  argint(0, &alarm_time);
  argaddr(1, &handler);

  if(alarm_time < 0){
    return -1;
  }

  struct proc *process = myproc();

  process->alarm_time = alarm_time;
  process->handler    = handler;

  return 0;
}

uint64 sys_sigreturn(void){

  myproc()->tick_count = 0;
  memmove(myproc()->trapframe,myproc()->save_frame, PGSIZE);
  myproc()->trapframe->epc = myproc()->interrupt_ra;

  kfree(myproc()->save_frame);
  return 0;
}