AM_CFLAGS = -Wall -Wextra

lib_LTLIBRARIES = libnonstdio.la
libnonstdio_la_SOURCES = nonstdio.c

noinst_PROGRAMS = stdio-cat unlocked-cat nonstdio-cat
stdio_cat_SOURCES = cat.c
unlocked_cat_SOURCES = cat.c
nonstdio_cat_SOURCES = cat.c
unlocked_cat_CPPFLAGS = -DUNLOCKED
nonstdio_cat_CPPFLAGS = -DNONSTDIO
nonstdio_cat_LDADD = libnonstdio.la
nonstdio_cat_LDFLAGS = -static
