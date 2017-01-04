#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include "nonstdio.h"

long nwrite_tail(noFILE *np, const char *buf, size_t size)
{
    while (np->fill + size >= BUFSIZ) {
	if (np->fill == 0) {
	    /* writing user buf directly */
	    ssize_t ret = write(1, buf, BUFSIZ);
	    if (ret < 0) {
		if (errno == EINTR)
		    continue;
		return -1;
	    }
	    buf += ret;
	    size -= ret;
	    continue;
	}
	/* fill np->buf to its full size */
	size_t more = BUFSIZ - np->fill;
	memcpy(np->buf + np->fill, buf, more);
	np->fill = BUFSIZ;
	buf += more;
	size -= more;
	/* need to flush it all */
	if (nflush_tail(np) < 0)
	    return -1;
    }
    memcpy(np->buf + np->fill, buf, size);
    np->fill += size;
    return 1;
}

int nflush_tail(noFILE *np)
{
    char *outbuf = np->buf;
    size_t outsize = np->fill;
    do {
	ssize_t ret = write(1, outbuf, outsize);
	if (ret < 0) {
	    if (errno == EINTR)
		continue;
	    return -1;
	}
	outbuf += ret;
	outsize -= ret;
    } while (outsize);
    np->fill = 0;
    return 0;
}

#undef nonstdout
struct noFILE nonstdout = { .fd = 1 };
