// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;

struct page_ref{
  struct spinlock lock;
  uint32 ref_count;
}page_ref[(PHYSTOP - KERNBASE) / PGSIZE];


void
kinit()
{
  initlock(&kmem.lock, "kmem");

//COW: For init the spinlock of the page reference
  for(int i = 0; i < (PHYSTOP - KERNBASE) / PGSIZE ; i++){//
      initlock(&page_ref[i] , "page_ref");
      page_ref[i].ref_count = 0;
  }

  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by pa,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.

  if(page_ref[(uint64)pa / PGSIZE].ref_count == 0 ){
    panic("kfree: <cow> free the unused page");
  }

  acquire(&page_ref[(uint64)pa / PGSIZE].lock);

  page_ref[(uint64)pa / PGSIZE].ref_count -= 1;

  release(&page_ref[(uint64)pa / PGSIZE].lock);

  if(page_ref[(uint64)pa / PGSIZE].ref_count > 0){
   return;
  }
  else{
    memset(pa, 1, PGSIZE);

    r = (struct run*)pa;
    acquire(&kmem.lock);
    r->next = kmem.freelist;
    kmem.freelist = r;
    release(&kmem.lock);
  }
  
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  if(r)
    kmem.freelist = r->next;
  release(&kmem.lock);

  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk
  return (void*)r;
}

unsigned int kfreemem(void){ //-------add by Lee 20240718
  acquire(&kmem.lock); //
  unsigned int sum = 0;
  struct run *r = kmem.freelist;

    while(r){
      sum += 1;
      r = r->next;
    }

  release(&kmem.lock);

  return sum*PGSIZE;  
}
