// the non-standard input/output
struct NILE {
    int fd;
    size_t fill;
    char buf[BUFSIZ];
};

typedef struct NILE NILE;
static struct NILE nonstdout = { .fd = 1 };
#define nonstdout (&nonstdout)

static void nwrite_(const char *func, NILE *np, const void *buf_, size_t size)
{
    const char *buf = buf_;
    while (np->fill + size >= BUFSIZ) {
	if (np->fill == 0) {
	    // writing user buf directly
	    ssize_t ret = write(1, buf, BUFSIZ);
	    if (ret < 0) {
		if (errno == EINTR)
		    continue;
		die_(func, "%s: %m", "write");
	    }
	    buf += ret;
	    size -= ret;
	    continue;
	}
	// fill np->buf to its full size
	size_t more = BUFSIZ - np->fill;
	memcpy(np->buf + np->fill, buf, more);
	buf += more;
	size -= more;
	// need to flush it all
	char *outbuf = np->buf;
	size_t outsize = BUFSIZ;
	do {
	    ssize_t ret = write(1, outbuf, outsize);
	    if (ret < 0) {
		if (errno == EINTR)
		    continue;
		die_(func, "%s: %m", "write");
	    }
	    outbuf += ret;
	    outsize -= ret;
	} while (outsize);
	np->fill = 0;
    }
    memcpy(np->buf + np->fill, buf, size);
    np->fill += size;
}

static void nflush_(const char *func, NILE *np)
{
    if (np->fill == 0)
	return;
    char *outbuf = np->buf;
    size_t outsize = np->fill;
    do {
	ssize_t ret = write(1, outbuf, outsize);
	if (ret < 0) {
	    if (errno == EINTR)
		continue;
	    die_(func, "%s: %m", "write");
	}
	outbuf += ret;
	outsize -= ret;
    } while (outsize);
    np->fill = 0;
}

#define nwrite(np, buf, size)	nwrite_(__func__, np, buf, size)
#define nflush(np)		nflush_(__func__, np)
