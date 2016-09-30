#ifdef WIN32 /* riad */
#pragma warning(disable:4035)
#include <string.h>
#include <io.h>
#endif
		   /* Standard Library Table */

/* #include "asax.h"*/
#ifndef 	_asaxh
#define 	ident_lg 	20
typedef void * (*retfp)();
enum type {integer, bytestring, fieldstring, undef};
typedef void *hole, **holePtr;	/* generic pointer types */
enum errortype {fatal, lexical, syntax, semantics};
#define usz sizeof(unsigned short)
extern char *Heap[];
extern int Heap_lg;
#define tmalloc(n, t) Heap[Heap_lg++] = (t *)malloc((unsigned)((n)*sizeof(t)))
#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif
#endif /* _asaxh 	*/


#define undefArity -1 /* the arity of a variable arity predefined fun or proc */

typedef struct {
	char name[ident_lg];
	retfp fp_ptr;
	int narg;                /* = undefArity in case of variable arity */
	enum type rettype;
	int argp;          /* = passing type if fp_descr.narg = undefArity */
} fp_descr;

void *printI(), *printIln(), *printS(), *printSln();
#define Ext_Int_Nre 50
extern globalInt[Ext_Int_Nre];

int tfp_descr_lg;
extern int tfp_descr_lg;

/* #define MaxFp 200
fp_descr tfp_descr[MaxFp];
extern fp_descr tfp_descr[MaxFp];
   */
enum passtype {value, ref};
typedef struct {
	enum passtype tpass;
	enum type targ;
} arg_descr;

int targ_descr_lg;
extern int targ_descr_lg;

/* #define MaxFpArg 300
arg_descr targ_descr[MaxFpArg];
extern arg_descr targ_descr[MaxFpArg];

   */
