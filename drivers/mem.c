#include "mem.h"

#define HEAP_SIZE (1024 * 1024)

static u8     heap[HEAP_SIZE] __attribute__((aligned(16)));
static size_t heap_off = 0;

void *memset(void *dst, int v, size_t n) {
    u8 *d = (u8 *)dst;
    for (size_t i = 0; i < n; i++) d[i] = (u8)v;
    return dst;
}

void *memcpy(void *dst, const void *src, size_t n) {
    u8 *d = (u8 *)dst; const u8 *s = (const u8 *)src;
    for (size_t i = 0; i < n; i++) d[i] = s[i];
    return dst;
}

int memcmp(const void *a, const void *b, size_t n) {
    const u8 *x = a, *y = b;
    for (size_t i = 0; i < n; i++)
        if (x[i] != y[i]) return x[i] - y[i];
    return 0;
}

void *kmalloc(size_t bytes) {
    bytes = (bytes + 15) & ~(size_t)15;       /* 16-byte align */
    if (heap_off + bytes > HEAP_SIZE) return NULL;
    void *p = &heap[heap_off];
    heap_off += bytes;
    return p;
}

size_t kheap_used(void)  { return heap_off; }
size_t kheap_total(void) { return HEAP_SIZE; }
