#ifndef 	_asaxh
#define		_asaxh
#include <stdlib.h> 
#include <stdio.h>

#include <string.h>
#ifndef bcmp
#define bcmp(s1, s2, n)	memcmp ((s1), (s2), (n))
#endif
#ifndef bcopy
#define bcopy(s, d, n)	memcpy ((d), (s), (n))
#endif
#ifndef bzero
#define bzero(s, n)	memset ((s), 0, (n))
#endif

extern char *Heap[];
extern int Heap_lg;
#define tmalloc(n, t) Heap[Heap_lg++] = (t *)malloc((unsigned)((n)*sizeof(t)))
#define MAXS 100         /* C or X string max length */
#define ident_lg 20

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif


#ifdef __ProtoGlarp__
#undef __ProtoGlarp__
#endif
#ifdef __STDC__
#define __ProtoGlarp__(x) x
#else
#define __ProtoGlarp__(x) ()
#endif

typedef void *hole, **holePtr;	/* generic pointer types */

extern int LineNbre;
extern int ErrorFound;
extern int NoRun;
extern char *rule_name;
extern char *CurrMod_name;
enum index  { andx, at_completionx, beginx, divx, dox, endx, externalx,
			   falsex, fix, for_currentx, for_nextx, globalx,
		    ifx, init_actionx, integerx, internalx, modx, notx, odx,
			   offx, orx, presentx, rulex, skipx, stringx,
			   triggerx, truex, usesx, varx,
			   /* signs symbols */
			   neq, lparenthesis, rparenthesis, mult, add, comma,
			  minus, larrow, colon, assign, semicolon, less, leq,
			   equal, greater, geq, period, eqx,
			   /* other symbols */
			   identifierx, integerxx, C_literalx, X_literalx,
			   booleanx, errsymbx, nosymbx};

enum tbsymb  {integer_t, C_literal, X_literal, identifier, specsymb, boolean,
	      nosymb, errsymb, eq};
typedef struct {
	enum index ibsymb;
	int        X_lg;     /* length of X literals */
	union {
		char svalue[MAXS]; /* identifier, C_literal, X_literal,
				       specsymb */
		int  ivalue;        /* integer, boolean */
	} bsymbval;
} tsymb;
tsymb symb;

enum var_par {par, var, intGlobal, extGlobal};
enum errortype {fatal, lexical, syntax, semantics};

extern error();
extern error1();

int	symbolClass	__ProtoGlarp__((int index));
int	printsymb	__ProtoGlarp__(());
int	get_char	__ProtoGlarp__((FILE *fp));
int 	put_type	__ProtoGlarp__((int type, int pv, int rank));
int 	print_tdescr	__ProtoGlarp__(());
int 	insert_stop	__ProtoGlarp__((int s, int Stop[], int lg, int Stop_new[], int *lg_new));
int 	merge		__ProtoGlarp__((int a[], int lga, int b[], int lgb, int abmerge[]));
int 	VariableGroup	__ProtoGlarp__((int Stop[], int lg, int pv, int *rank));
int 	VarDefPart	__ProtoGlarp__((int Stop[], int lg, int *rank));
int 	VarDefPart1	__ProtoGlarp__((int Stop[], int lg, int *rank));
int 	ParameterGroup	__ProtoGlarp__((int Stop[], int lg, int *rank));
int 	FormalParList	__ProtoGlarp__((int Stop[], int lg, int *rank));
int 	ParameterList	__ProtoGlarp__((int Stop[], int lg, int *rank));
int 	ForParamPart	__ProtoGlarp__((int Stop[], int lg, int *rank));
int 	RuleHeading	__ProtoGlarp__((int Stop[], int lg));
#define Mlg 53
#define lgStop1 1
extern Stop1[lgStop1];
#define lgAdd 2
extern int AddStop[lgAdd];
#define lgMult 3
extern int MultStop[lgMult];
#define lgTerm 7
extern int TermStop[lgTerm];
#define lgFact 5
extern int FactorStop[lgFact];
#define lgActPar 6
extern int ActParStop[lgActPar];
#define lgTrMode 3
extern int TrModeStop[lgTrMode];

#define lgParStop 1
extern int ParStop[lgParStop];
#define lgParamStop 1
extern int ParameterStop[lgParamStop];
#define lgVarStop 1
extern int VariableStop[lgVarStop];

#define lgRel 7
extern int relStop[lgRel];
#define lgCond 9
extern int CondStop[lgCond];
#define lgAction 6
extern int ActionStop[lgAction];
#define lgRule 4
extern int RuleStop[lgRule];
enum truthValue {true_exit, false_exit, any_exit};

typedef struct ami *BoxPtr;

#define MaxRuleNr 222
#define MaxVarNr 222
typedef struct {
	char rule_name[ident_lg];
	int modulePtr;
	BoxPtr Code;
	int ParNre;         /* nbre of formal pararmeters */
	int VarNre;         /* nbre of local variables */
	int APASize;        /* Size in byte of the Actual Parameter Area */
	int status;         /* unknown, known or declared rule           */
	int nameDescr;      /* an index to the names descriptor table tdescr */
	int targDescr;      /* an index to the names descriptor table targ_types */
} rdescr;
extern rdescr trdescr[MaxRuleNr];
extern int trdescr_ptr;   /* pointer to the first free element of trdescr[] */
extern int trdescr_lg;    /* length of trdescr[] */
/* descriptor of a name i.e, a formal parameter or a local variable   */
enum type {integer, bytestring, fieldstring, undef};
typedef struct {
	enum var_par iname; /* either a parameter or a variable */
	char name[ident_lg]; /* the name */
	enum type tname;     /* the type of a name */
} namedescr;

extern rdescr tExtRuleDescr[MaxRuleNr];
extern int tExtRule_lg;
extern int tExtRule_ptr;

extern enum type targ_types[MaxVarNr];

	     /* pointer to the first free element of targ_types[] */

extern int targ_types_lg;

/* flags for indicating a rule status */

#define unknown  1
#define known    2
#define declared 3

extern char *VA_ptr, *PA_ptr;
extern char *WA_ptr;
extern char *GV, *GV_ptr;
#define MaxGV 4*(MaxIntGlobal+MaxExtGlobal)   /*  4 * max. of globals vars.*/
#define Byte sizeof(char)
int	getVarAddr	__ProtoGlarp__((char *name, char *rule_name, enum var_par *pv, enum type *type));
BoxPtr 	getRuleCode	__ProtoGlarp__((char *rule_name));
int 	add_hole	__ProtoGlarp__((hole *addr, char *rule_name));

/* table of module descriptors  */

# define MaxModDescr 50 /* maximum number of modules in a source program */
extern char exten[5];
typedef struct {
    char module_name[1024];
    int intVarNbre;
    int ruleDeclNbre;
    int VarPtr;
    int RulePtr;
} modDescr;

modDescr tModDescr[MaxModDescr];

extern int tModDescr_lg;
extern int CurrModAddr;

typedef struct {
    char varName[ident_lg];
    enum type type;
    char *addr;
} varDescr;

# define MaxIntGlobal 222 /* maximum number of internal global in a module  */
# define MaxExtGlobal 222 /* maximum number of external in a source program */

varDescr tIntGlobal[MaxIntGlobal];
varDescr tExtGlobal[MaxExtGlobal];
#define MaxExtAux 222     /* maximum number of externals per a source file */

extern int tIntGlobal_lg;
extern int tExtGlobal_lg;

extern varDescr tIntGlobal[MaxIntGlobal];
extern varDescr tExtGlobal[MaxExtGlobal];
char *tExtAux[MaxExtAux];
extern int tExtAux_lg;

extern char *tExtAux[MaxExtAux];

#define internal 0
#define external 1

extern char *Curr_Rec_Addr;          /* Current Audit Record Address              */
#define MaxIdent 30
#define Max_ad_id 1200
#ifndef WIN32 /* riad */
#define NULL 0
#endif


typedef struct {
	unsigned short ad_id;
	hole *ad_value;
} CR_table_entry;


/* When  the current record must be written to an NADF file                */
/* the following structures are used by creatNADF() and writeNADF()        */

#define MAX_files 20    /* max. number of simultaneously opened NADF files */

/* Functions handling the current record table */

int  	getFieldNameAddr	__ProtoGlarp__((char *s, hole *gamma));
int 	getAuditDataId		__ProtoGlarp__((char *s));
int 	insert_hole		__ProtoGlarp__((hole *gamma, unsigned short i, unsigned short found));
int 	get_location		__ProtoGlarp__((unsigned short id));
int 	print_CR_table		__ProtoGlarp__(());
int 	scanCR			__ProtoGlarp__((CR_table_entry tab[], int lg, char *p));

/* Abstract Machine Instructions */

enum operation {and1, or1, trigger1, div1 = divx, mod1 = modx, not1,
		neq1 = neq, mult1 = mult, add1, minus1 = minus, un_minus1,
		assign1, assign_str1, less1 = less, leq1, equal1,
		greater1, geq1, assign_addr1, present1,
		for_next1, for_current1, at_completion1,
		call_p1, call_f1, move_int1,
		move_str1, setLen1, addLen1,
		cpy_intAddr1, cpy_strAddr1, cpy_str1, cpy_int1,
		StrLess1, StrLeq1, StrEqual1, StrGreater1, StrGeq1, StrNeq1,
		StrEq1, mv_int1, compile1};


typedef struct {
	int val;
	char *deltaIn;
} mv_int;

typedef struct {
	int *gamma1, *gamma2;
} assign_int;

typedef struct {
	hole *gamma1, *gamma2;
} assign_str;

typedef struct {
	char *gamma1, *gamma2;
} assign_addr;

typedef struct {
	int *deltaIn, *gamma;
} un_minus;

typedef struct {
	int  *deltaIn, *gamma1, *gamma2;
} arithm;

typedef struct {
	void *(*fpptr )();
	char *deltaIn;
} call;


typedef struct {
	char **str_addr;
	BoxPtr alphaTrue;
} present_typ;

typedef struct {
	BoxPtr ruleCode;
} trigger;

typedef struct {
	char **gamma1, **gamma2;
	BoxPtr alphaTrue;
} StrRelat;

typedef struct {
	int *gamma1, *gamma2;
	BoxPtr alphaTrue;
} relational;

typedef struct {
	hole *dest, *source;
} move;

typedef struct ami {
	enum operation op;
	union {
		mv_int          mv_intbox;
		assign_int      assign_intbox;
		assign_str      assign_strbox;
		assign_addr     assign_addrbox;
		un_minus        un_minusbox;
		arithm          arithmbox;
		StrRelat        StrRelatbox;
		relational      relationalbox;
		present_typ     presentbox;
		trigger         triggerbox;
		char            *rule_name;
		call            callbox;
		move            movebox;
		hole            *gamma;
		int             size;
	} arg;
	BoxPtr alpha;
} Box;

extern BoxPtr sxcond();
extern BoxPtr relation();
extern BoxPtr present();
extern BoxPtr sxfactor();
extern BoxPtr sxterm();
extern BoxPtr simpExpr();
extern BoxPtr ActualParList();
extern BoxPtr ActualPar();
extern BoxPtr ActualP();
extern BoxPtr ActualP1();
extern char *leftExpr();
extern checkTypes();
 
/* Rule Triggering */

extern BoxPtr sxtrigger();
extern BoxPtr RuleActParList();

/* actions */

extern BoxPtr assignt();
extern BoxPtr action();
extern BoxPtr subAction();
extern BoxPtr guardedAction();
extern BoxPtr guardAction();
extern BoxPtr grdedAction();
extern BoxPtr compoundAction();
extern BoxPtr compAction();
extern BoxPtr compndAction();
extern BoxPtr dood();
extern BoxPtr iffi();

		/* Functions for accessing the standard library table */
int  	getfpAddr	__ProtoGlarp__((char *s));
int 	getfpType	__ProtoGlarp__((char *s));
typedef void * (*retfp)();
retfp 	getfpPtr	__ProtoGlarp__((char *s));
int 	getArgPass	__ProtoGlarp__((char *fpname, int rank));
int 	getArgType	__ProtoGlarp__((char *s, int rank));

		/* Integer and string contants areas */

extern int *saveIntConst();
extern hole **saveStrConst();
extern unsigned short lenstr();

  /* Internal Code Printing */

int 	fill_holesb	__ProtoGlarp__((hole *head, BoxPtr value));
int 	fill_CR_holes	__ProtoGlarp__(());
int 	fill_holes	__ProtoGlarp__((hole *head, char **value));
int 	printCodes	__ProtoGlarp__(());
int 	printBox	__ProtoGlarp__((BoxPtr p));
int 	storeBox	__ProtoGlarp__((BoxPtr p));

#define usz sizeof(unsigned short)

	 /* File descriptors */

#include <stdio.h>

extern FILE *rfp; /* the file containing the text of the rules */
extern int dfp;   /* the NADF file descriptor */

	    /* function handling the three stacks of triggered rules */

struct trigDescr {
    int size;
    struct trigDescr *next;
    BoxPtr code;
    hole *param;
};
typedef struct trigDescr trigD;

int 	push		__ProtoGlarp__((trigD *eff_ptr, enum operation tr_m));
trigD 	*pop		__ProtoGlarp__((enum operation tr_m));
extern trigD *current, *next, *compl;

/* set of flags indicating the supported mode of operation of asax */

#define standard   1  /* operates in the standard mode                  */
#define conversion 2  /* operates in the conversion/evaluation mode     */
#define mixed      3  /* operates in both standard and conversion modes */


#define BUFSIZE 256  /* Max. size of the buffer holding the RE to be compiled */
		     /* in case the RE is greater than BUFSIZE an error is    */
 		     /* reported using ERROR(c)                               */

#define ESIZE 256    /* Max. size of the buffer holding the compiled RE       */
		     /* in case the compiled RE is greater than ESIZE an      */
 		     /* error is reported using ERROR(c)      		      */

typedef struct {
    unsigned short lgth; /* string length */
    char     *str;       /* the string itself (no ending null char: '\0') */
} StrConst;

#define lgStrConst 400		       /* max size of tabStrConst[]  */
#define lgIntConst 200		     /* max size of tabIntConst[]  */

#ifdef WIN32 /* riad */
//#include "functions.h"
#endif


#endif		/* _asaxh	*/
