#pragma GCC optimize("Os",  "-finline-functions-called-once", "-ffast-math", "-falign-functions=32")

#ifndef MEMSETSTUFF
#define MEMSETSTUFF
extern void* memset(void *s, int c, size_t len);
extern void *memmove(void * restrict dst, const void *src, size_t n);
#endif