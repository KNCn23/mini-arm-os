#ifndef MEM_H
#define MEM_H

#include "types.h"

void  *memset(void *dst, int v, size_t n);
void  *memcpy(void *dst, const void *src, size_t n);
int    memcmp(const void *a, const void *b, size_t n);

/* Bump allocator over a fixed 1 MiB heap. Never freed. */
void  *kmalloc(size_t bytes);
size_t kheap_used(void);
size_t kheap_total(void);

#endif
