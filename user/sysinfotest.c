#include "kernel/types.h"
#include "kernel/riscv.h"
#include "user/user.h"
#include "kernel/sysinfo.h"

void
sinfo(struct sysinfo *info_buf) {
  if (info((uint64*)info_buf) < 0) {
    printf("FAIL: sysinfo failed");
    exit(1);
  }
}

//
// use sbrk() to count how many free physical memory pages there are.
//
int
countfree()
{
  uint64 sz0 = (uint64)sbrk(0);
  struct sysinfo info_buf;
  int n = 0;

  while(1){
    if((uint64)sbrk(PGSIZE) == 0xffffffffffffffff){
      break;
    }
    n += PGSIZE;
  }
  sinfo(&info_buf);
  if (info_buf.freemem != 0) {
    printf("FAIL: there is no free mem, but sysinfo.freemem=%d\n",
      info_buf.freemem);
    exit(1);
  }
  sbrk(-((uint64)sbrk(0) - sz0));
  return n;
}

void
testmem() {
  //printf("testmem\n");
  struct sysinfo info_buf;
  uint64 n = countfree();
  
  sinfo(&info_buf);

  if (info_buf.freemem!= n) {
    printf("FAIL: free mem %d (bytes) instead of %d\n", info_buf.freemem, n);
    exit(1);
  }
  
  if((uint64)sbrk(PGSIZE) == 0xffffffffffffffff){
    printf("sbrk failed");
    exit(1);
  }

  sinfo(&info_buf);
    
  if (info_buf.freemem != n-PGSIZE) {
    printf("FAIL: free mem %d (bytes) instead of %d\n", n-PGSIZE, info_buf.freemem);
    exit(1);
  }
  
  if((uint64)sbrk(-PGSIZE) == 0xffffffffffffffff){
    printf("sbrk failed");
    exit(1);
  }

  sinfo(&info_buf);
    
  if (info_buf.freemem != n) {
    printf("FAIL: free mem %d (bytes) instead of %d\n", n, info_buf.freemem);
    exit(1);
  }
}

void
testcall() {
  //printf("testcall\n");
  struct sysinfo info_buf;
  
  if (info((uint64*)&info_buf) < 0) {
    printf("FAIL: sysinfo failed\n");
    exit(1);
  }

  if (info((uint64*) 0xeaeb0b5b00002f5e) !=  0xffffffffffffffff) {
    printf("FAIL: sysinfo succeeded with bad argument\n");
    exit(1);
  }
}

void testproc() {
  //printf("testproc\n");
  struct sysinfo info_buf;
  uint64 nproc;
  int status;
  int pid;
  
  sinfo(&info_buf);
  nproc = info_buf.nproc;

  pid = fork();
  if(pid < 0){
    printf("sysinfotest: fork failed\n");
    exit(1);
  }
  if(pid == 0){
    sinfo(&info_buf);
    if(info_buf.nproc != nproc+1) {
      printf("sysinfotest: FAIL nproc is %d instead of %d\n", info_buf.nproc, nproc+1);
      exit(1);
    }
    exit(0);
  }
  wait(&status);
  sinfo(&info_buf);
  if(info_buf.nproc != nproc) {
      printf("sysinfotest: FAIL nproc is %d instead of %d\n", info_buf.nproc, nproc);
      exit(1);
  }
}

void testbad() {
  printf("testbad\n");
  int pid = fork();
  int xstatus;
  
  if(pid < 0){
    printf("sysinfotest: fork failed\n");
    exit(1);
  }
  if(pid == 0){
      sinfo(0x0);
      exit(0);
  }
  wait(&xstatus);
  if(xstatus == -1)  // kernel killed child?
    exit(0);
  else {
    printf("sysinfotest: testbad succeeded %d\n", xstatus);
    exit(xstatus);
  }
}

int
main(int argc, char *argv[])
{
  printf("sysinfotest: start\n");
  testcall();
  testmem();
  testproc();
  printf("sysinfotest: OK\n");
  exit(0);
}
