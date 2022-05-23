#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "date.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
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
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  
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


  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}


#ifdef LAB_PGTBL
int
sys_pgaccess(void)
{
  // lab pgtbl: your code here.
  uint64 base;
  int page_num;
  uint64 user_bitmask_addr;
  if (argaddr(0, &base) < 0 ||  argint(1, &page_num) < 0 || argaddr(2, &user_bitmask_addr) < 0){
    printf("Get paramemters error for sys_pgaccess function.\n");
    return -1;
  }
  if(page_num < 0 || page_num > 32){
    printf("Illegal page number should be [0, 32], but actually: %d.\n", page_num);
    return -1;
  }
  
  uint32 mask = 0;
  pte_t *pte;
  struct proc* p = myproc();
  for (int i = 0; i < page_num; i++){
    if(base >= MAXVA)
      return -1;
    
    pte = walk(p->pagetable, base, 0);
    if(pte == 0){
      panic("pgaccess: pte should exist");
    }

    if(*pte & PTE_A){
      mask |= (1 << i);
      // Be sure to clear PTE_A after checking if it is set.
      *pte &= (~PTE_A);
    }
    base += PGSIZE;
  }
  
  if(copyout(p->pagetable, user_bitmask_addr, (char*)&mask, sizeof(mask)) < 0){
    return -1;
  }  
  return 0;
}
#endif

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
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
