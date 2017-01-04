#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>

#ifdef NONSTDIO
#include "nonstdio.h"
#else 
/* nonstdio calls are mapped to stdio */
#define nwrite(np, buf, size) fwrite(buf, size, 1, np)
#define nflush(np) fflush(np)
#define nonstdout stdout
#endif

#ifdef UNLOCKED
#ifdef __GLIBC__
#define fwrite fwrite_unlocked
#define fflush fflush_unlocked
#else
#warning "unlocked stdio not supported"
#endif
#endif

static void *dummy(void *arg)
{
    return arg;
}

int main(int argc, char **argv)
{
    /* Usage: ./cat niter maxsize </dev/zero >/dev/null */
    assert(argc == 3);
    int niter = atoi(argv[1]);
    int maxsize = atoi(argv[2]);
    assert(niter > 0);
    assert(maxsize > 0);
    /* Launch a thread to disable glibc's optimization and expose
     * the difference between stdio(3) and unlocked_stdio(3). */
    pthread_t thread;
    int err = pthread_create(&thread, NULL, dummy, NULL);
    if (err) {
	fprintf(stderr, "pthread_create failed\n");
	return 1;
    }
    for (int i = 0; i < niter; i++) {
	char buf[BUFSIZ];
	size_t size = 1 + rand() % maxsize;
	if (fread(buf, size, 1, stdin) == 0)
	    break;
	if (nwrite(nonstdout, buf, size) != 1) {
	    fprintf(stderr, "write failed\n");
	    return 1;
	}
    }
    if (nflush(nonstdout)) {
	fprintf(stderr, "flush failed\n");
	return 1;
    }
    err = pthread_join(thread, NULL);
    if (err) {
	fprintf(stderr, "pthread_join failed\n");
	return 1;
    }
    return 0;
}
