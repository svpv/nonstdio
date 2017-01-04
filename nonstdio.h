/*
 * nonstdio - the non-standard input/output library
 */
#ifndef NONSTDIO_H
#define NONSTDIO_H

/* We use memcpy(3); this should also get us size_t. */
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Symbols defined in this library are not exported. */
#pragma GCC visibility push(hidden)

/* The non-standard output file */
struct noFILE {
    int fd;
    size_t fill;
    char buf[BUFSIZ];
};

typedef struct noFILE noFILE;
extern noFILE nonstdout;
#define nonstdout (&nonstdout)

/* These "tail" functions are called by the inline functions
 * when the fast case condition does not hold. */
long nwrite_tail(noFILE *np, const char *buf, size_t size);
int nflush_tail(noFILE *np);

/* Replacement for fwrite(3): returns 1, which is not dissimilar
 * to fwrite(buf, size, 1, fp) returning 1, or -1 on error. */
static inline long nwrite(noFILE *np, const void *buf, size_t size)
{
    if (np->fill + size >= BUFSIZ)
	return nwrite_tail(np, (const char *) buf, size);
    /* memcpy call will be inlined if the size is constant */
    memcpy(np->buf + np->fill, buf, size);
    np->fill += size;
    return 1;
}

/* Replacement for fflush(3): returns 0 (just like fflush),
 * or -1 on error. */
static inline int nflush(noFILE *np)
{
    if (np->fill)
	return nflush_tail(np);
    return 0;
}

#pragma GCC visibility pop

#ifdef __cplusplus
}
#endif

#endif
