#ifdef __cplusplus
extern "C" {
#endif
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#ifdef __cplusplus
}
#endif

#include "./algutil/algutil.h"

static int
not_here(s)
char *s;
{
    croak("%s not implemented on this architecture", s);
    return -1;
}

static double
constant(name, arg)
char *name;
int arg;
{
    errno = 0;
    switch (*name) {
    }
    errno = EINVAL;
    return 0;

not_there:
    errno = ENOENT;
    return 0;
}


MODULE = EntTrac		PACKAGE = EntTrac		


double
constant(name,arg)
	char *		name
	int		arg


int 
alg_inslen(buf, ip)
	unsigned char *buf
	int ip
	ALIAS:
	EntTrac::i86OpcLen = 1
	OUTPUT:
	RETVAL


