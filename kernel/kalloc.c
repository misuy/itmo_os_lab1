// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

extern char end[];  // first address after kernel.
                    // defined by kernel.ld.

struct spinlock page_refs_lock;
uint64 pages_count;
char *page_refs;

uint64 ptr_to_page_refs_idx(void *p)
{
  //printf("P: %p\n", p);
  return ((uint64) p - PGROUNDUP((uint64)end)) / PGSIZE;
}

void kinit() {
  char *p = (char *)PGROUNDUP((uint64)end);
  bd_init(p, (void *)PHYSTOP);
  pages_count = (PHYSTOP - PGROUNDUP((uint64)end)) / PGSIZE + 1;
  page_refs = bd_malloc(sizeof(char) * pages_count);
  memset(page_refs, 0, pages_count);
  initlock(&page_refs_lock, "page_refs_lock");
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void kfree(void *pa)
{
  acquire(&page_refs_lock);
  page_refs[ptr_to_page_refs_idx(pa)]--;
  if (page_refs[ptr_to_page_refs_idx(pa)] == 0)
  {
    bd_free(pa);
  }
  release(&page_refs_lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *kalloc(void)
{
  acquire(&page_refs_lock);
  void *p = bd_malloc(PGSIZE);
  if (p != 0)
    page_refs[ptr_to_page_refs_idx(p)] = 1;
  release(&page_refs_lock);
  return p;
}

void page_refs_inc(void *pa)
{
  acquire(&page_refs_lock);
  page_refs[ptr_to_page_refs_idx(pa)]++;
  release(&page_refs_lock);
}

char page_refs_get(void *pa)
{
  char refs_count;
  acquire(&page_refs_lock);
  refs_count = page_refs[ptr_to_page_refs_idx(pa)];
  release(&page_refs_lock);
  return refs_count;
}