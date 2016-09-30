/*
 :******************************************************************
 :* MODULE-DESCRIPTION
 :*
 :* TITLE         (/ Parsing an actual parameter list    /)
 :*
 :* NAME             expr.c
 :*
 :* COPYRIGHT     (C)   SIEMENS NIXDORF INFORMATIONSSYSTEME AG  1992
 :*                     ALL RIGHTS RESERVED
 :*
 :* MEMORY-CLASS  6
 :*
 :* PROG-CONVENTS (/ <internal conventions (instance, names,
 :*                  external data, ...). None -> empty text>     /)
 :*
 :* PROCEDURE     ActualParList
 :* REMARKS       (/ parsing an actual parameter list /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     ActualPar
 :* REMARKS       (/ parsing an actual parameter (evaluate + move)  /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     ActualP
 :* REMARKS       (/ parsing an actual parameter (evaluate)         /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     ActualP1
 :* REMARKS       (/ parsing a value parameter               /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     leftExpr
 :* REMARKS       (/ parsing a reference parameter               /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     callBox
 :* REMARKS       (/ building a call box              /)
 :* ENTRY         EXTERNAL
 :*
 :*-----------------------------------------------------------------
 :*
 :* VERSION       001
 :*   CRDATE      1992-03-09
 :*   AUTHOR      (/ Mounji Abdelaziz       , FUNDP /)
 :*   UPDATE      (/ original issue /)
 :*
 :* END-MODULE-DESCRIPTION expr.c
 :******************************************************************
*/
#include <stdio.h>
#include "asax.h"
#include "predef.h"

static unsigned int preCompile = FALSE;
static unsigned int isMatch    = FALSE;
#ifdef 	STDREGEX
extern char *compile(); /* from /usr/include/regexp.h */
#endif

#if defined(POSIXREGEX) || defined(GNUREGEX) || defined(BSDREGEX)
#include "regex.h"
#define PATBUFSZ	sizeof(struct re_pattern_buffer)
#define LGSZ		sizeof(long)
extern 	reg_syntax_t 	re_syntax_options;
#elif !defined(STDREGEX) 
#define	STDREGEX
#endif


/*
 :******************************************************************
 :* UNIT-BODY expr.ActualParList
 :*
 :* TITLE         (/ parsing an actual parameter list       /)
 :*
 :* PURPOSE       (/ parse a sequence of symbols representing a list of
 :*                  actual parameters generate the corresponding internal
 :*                  code for it if no error is found or recover from this
 :*                  error otherwise                        /)
 :*                                                      END-PURPOSE
 :* VERSION       001
 :*   CRDATE      1992-11-03
 :*   AUTHOR      (/ Mounji Abdelaziz       , FUNDP              /)
 :*   UPDATE      (/ original issue /)
 :*
 :* REALIZATION   (/ <note, reference to other proc. (optional)> /)
 :*
 :*   CONTROL    (/
 :*
 :*       INVARIANT:
 :*
 :*  <param_1><",">...<","><param_i> <",">...<","><param_N><")"><beta>
 :*  <---------------------------------> <------------------------------->
 :*               treated  (=alpha)                  to be treated
 :*
 :*               where
 :*                  - N >= 0       (argument list may be empty)
 :*                  - 1 <= i <= N
 :*                  - 1 <= j <= i
 :*                  - rank = i-1
 :*                  - <param_i> is an actual parameter  i=1 .. N
 :*                  - <beta> is any character sequence
 :*                  - the CSS is <",">...<","><param_n><")"><beta>
 :*                  - the list of boxes corresponding to alpha is pointed by
 :*                    'p'
 :*                  - the list of holes corresponding to this list of boxes
 :*                    is pointed by 'h'
 :*                  - the list of boxes corresponding to <param_i> is
 :*                    pointed by 'p1'
 :*                  - the list of holes corresponding to <param_i> is
 :*                    pointed by 'h1'
 :*                  - 'deltaIn' is the current location the 'WA' zone
 :*                  - 'name' is the procedure or function name
 :*                  - 'isFunc'= TRUE if 'name' is a predefined function name
 :*                            = FALSE if corresponds to a procedure name
 :*                  - 'isSpec' = TRUE if 'name' can have a variable number
 :*                                       of arguments
 :*                             = FALSE otherwise
 :*
 :*                                                               /)
 :*   DATA       (/
 :*                       - symb       : holds the internal representation
 :*                                      of the last read symbol.
 :*                       - tfp_descr  : table of predefined functions and
 :*                                      procedures descriptors
 :*                       - ErrorFound : = TRUE if an error was found
 :*                                      = FALSE otherwise
 :*                                                                /)
 :*
 :*                                                          END-DATA
 :*   ASSUMPTIONS     (/ <external conditions> /)
 :*   PERFORMANCE     (/ <measures taken or not: when, why, how> /)
 :*   VALIDATION      (/ <for which param. or other data ? How ?> /)
 :*   SYNCHRONIZATION (/ <serialization & locks: purpose, method> /)
 :*   INIT&TERM       (/ <what must already/still be available> /)
 :*   ERROR-HANDLING  (/ <reaction to errors, r.c., state of data>/)
 :*   DOCUMENTS       (/
 :*                                Preliminary report on
 :*                      Advanced Security Audit trail Analysis on uniX
 :*                             (ASAX also called SAT-X)
 :*
 :*                      Advanced Security Audit trail analysis on uniX
 :*                             (ASAX also called SAT-X)
 :*                      Implementation design of the NADF Evaluator   /)
 :*                                                  END-REALIZATION
 :* PARAMETERS
 :*
 :*   Pre    (/ - the CSS is s1 s2 ... sn   (see REMARKS below)
 :*             - 'Stop' is an array stops  (see REMARKS below)
 :*             - 'lg' is the length of 'Stop'
 :*             - 'deltaIn' is the current address in the Working Area
 :*             - 'name' is the procedure or function name
 :*             - 'isFunc'= TRUE if 'name' is a predefined function name
 :*                       = FALSE if corresponds to a procedure name
 :*
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/   if the CSS is of the form:
 :*                  <param_1><",">...<","><param_N><")"><beta>
 :*               where
 :*                  <param_i> is a valid actual parameter i=1..N
 :*                  <beta> is any character sequence
 :*               the effect is the following:
 :*                  - the CSS is <")"><beta>
 :*                  - the pointer to the corresponding list of boxes is
 :*                    returned in 'p'
 :*                  - 'h' is the pointer to the list of holes in the list of
 :*                    boxes
 :*                otherwise
 :*                  - the CSS is of the form:
 :*                         <first_Stop><rest>
 :*                    where <first_Stop> is the first symbol in
 :*                                 s1 s2 ... sn
 :*                          such that <first_Stop> in 'Stop[]'
 :*                  - an undefined partial list of boxes is pointed by 'p'
 :*                  - one or more error messages are output to stdout
 :*                  - 'ErrorFound' is set to TRUE
 :*                                                                 /)
 :*
 :*                                                             END-Post
 :*   RETURNS  BoxPtr (/ A pointer to the generated internal code /)
 :*
 :*   REMARKS     (/ Definitions:
 :*                   - let the input stream be:
 :*                                 <t2><t3>...<tn>
 :*                     the symbol sequence:
 :*                             <t1><t2><t3>...<tn>
 :*                     where <t1> is the last symbol been read is called the
 :*                     'current symbol sequence' or CSS for short
 :*                   - the values of the enumerated type 'enum index' are
 :*                     called 'stops'. They are used as indexes to the table
 :*                     'specsymbtab[]' so as a 'stop' identifies an element
 :*                     this table.
 :*                   - refer to <DOCUMENTS> for a detailed description of
 :*                     the internal representation (boxes).
 :*
 :*
 :*                                                         /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY expr.ActualParList
 :******************************************************************
*/

BoxPtr ActualParList(isFunc, name, type, Stop, lg, deltaIn, h)
unsigned isFunc;
char *name, *deltaIn;
int Stop[], lg;
BoxPtr **h;
enum type *type;
{
	BoxPtr p0, p, p1, boxp, *h1, ActualPar(), callBox();
	int S1[Mlg], lg1, S2[Mlg], lg2;
	int k, rank, isSpec;
	enum type type1;
	char *dlta;
	extern int circf; /* from regexp.h value tp restored */

	if (strcmp(name, "match") == 0) {
		isMatch = TRUE;
		preCompile = FALSE;
	}
	dlta = deltaIn;
	if ((k = getfpAddr(name)) >= 0)
	    isSpec = isSpecial(k);
	else {
            printf("(expr_reg.c:)");
	    error1(semantics, 7, name);
        }
	if (symb.ibsymb != rparenthesis) {  /* arg. list not empty */
		rank = 0;
		if (isFunc) deltaIn += sizeof(int);
		if (isSpec) {
		    if ((p0 = (BoxPtr) malloc(sizeof(Box))) == NULL)
			error(fatal, 25);
		    storeBox(p0);
		    p0->op = mv_int1;
		    p0->arg.mv_intbox.deltaIn = deltaIn;
		    deltaIn += sizeof(int);
		}
		insert_stop(comma, Stop, lg, S1, &lg1);
		p = ActualPar(isSpec, name, rank, &type1, S1, lg1, deltaIn, h);
		if (isSpec && p!= NULL) {
		    deltaIn += sizeof(int);
		    p0->alpha = p;
		} else {
		    p0 = p;
		}
		if (isMatch && !preCompile) {
		    if ((p1 = (BoxPtr) malloc(sizeof(Box))) == NULL)
			error(fatal, 25);
		    storeBox(p1);
		    p1->op = compile1;
		    p1->arg.gamma = (hole *) deltaIn;
		    p1->alpha = NULL;

/* ### Mon Apr 10 15:25:19 1995  ###  */
/* the test was not done before */ 
		    if (!ErrorFound && p1 != NULL) {
		    	**h = p1;
		    	*h = &p1->alpha;
		    } 
		    preCompile = TRUE;
		}	
		while (symb.ibsymb == comma) {
     		     rank++;
		     lg2 = merge(S1, lg1, ActParStop, lgActPar, S2);
		     Expect(comma, S2, lg2);
		     deltaIn += sizeof(int); /* was size(type1) till Nov 2 93 */
		     p1 = ActualPar(isSpec, name, rank, &type1, S1, lg1,
				    deltaIn, &h1);
		     if (isSpec)
			   deltaIn += sizeof(int);
		     if (!ErrorFound && p1 != NULL) {
			   **h = p1;
			   *h = h1;
		     }
		}
		if (!isSpec) {/* no arity check for a variable arity funct */
		    if (rank < getArity(name) - 1)
			     /*  tfp_descr[getfpAddr(name)].narg - 1) */
			   error(semantics, 8);
		}

 
#ifdef STDREGEX
/*
*	standard <regexp.h> set a global variable circf during compilation
*	of a regular expression. This value must be reset before matching
* 	is done (ie, before calling step(). We add a virtual machine
*	instruction for this
*/
		if (isMatch && preCompile) {
		    if ((boxp = (BoxPtr) malloc(sizeof(Box))) == NULL)
			error(fatal, 25);
		    storeBox(boxp);
		    boxp->op = mv_int1;
		    boxp->arg.mv_intbox.deltaIn = (char *) &circf;
		    boxp->arg.mv_intbox.val = circf;
			if (!ErrorFound && p0 != NULL) {
		     		**h = boxp;
		     		*h = &boxp->alpha;
			}
		}
#endif
		boxp = callBox(isFunc, name, dlta);
		if (isSpec) p0->arg.mv_intbox.val = rank + 1;
		if (!ErrorFound && p0 != NULL) {
		     **h = boxp;
		     *h = &boxp->alpha;
		     return (p0);
		} else  {
		     *h = &boxp->alpha;
		     return (boxp);
		}
	} else {        /* no arguments */
		error1(semantics, 19, name);
		boxp = callBox(isFunc, name, dlta);
		*h = &boxp->alpha;
		return (boxp);
	}
}
/*
 :******************************************************************
 :* UNIT-BODY expr.ActualPar
 :*
 :* TITLE         (/ parsing an actual parameter (evaluate + move)     /)
 :*
 :* PURPOSE       (/ parse a sequence of symbols representing an actual
 :*                  parameter; generate the corresponding internal code for
 :*                  its evaluation and moving to the WA area if no error is
 :*                  found or recover from this error otherwise                                          /)
 :*                                                      END-PURPOSE
 :* VERSION       001
 :*   CRDATE      1992-11-03
 :*   AUTHOR      (/ Mounji Abdelaziz       , FUNDP              /)
 :*   UPDATE      (/ original issue /)
 :*
 :* REALIZATION   (/ <note, reference to other proc. (optional)> /)
 :*
 :*   CONTROL    (/
 :*
 :*                                                               /)
 :*   DATA       (/
 :*                       - symb       : holds the internal representation
 :*                                      of the last read symbol.
 :*                       - tfp_descr  : table of predefined functions and
 :*                                      procedures descriptors
			  - ErrorFound : = TRUE if an error was found
 :*                                      = FALSE otherwise
 :*                                                                /)
 :*
 :*                                                          END-DATA
 :*   ASSUMPTIONS     (/ <external conditions> /)
 :*   PERFORMANCE     (/ <measures taken or not: when, why, how> /)
 :*   VALIDATION      (/ <for which param. or other data ? How ?> /)
 :*   SYNCHRONIZATION (/ <serialization & locks: purpose, method> /)
 :*   INIT&TERM       (/ <what must already/still be available> /)
 :*   ERROR-HANDLING  (/ <reaction to errors, r.c., state of data>/)
 :*   DOCUMENTS       (/
 :*                                Preliminary report on
 :*                      Advanced Security Audit trail Analysis on uniX
 :*                             (ASAX also called SAT-X)
 :*
 :*                      Advanced Security Audit trail analysis on uniX
 :*                             (ASAX also called SAT-X)
 :*                      Implementation design of the NADF Evaluator   /)
 :*                                                  END-REALIZATION
 :* PARAMETERS
 :*
 :*   Pre    (/ - the CSS is s1 s2 ... sn  (see REMARKS below)
 :*             - 'name' is the procedure or function name
 :*             - 'isSpec' = TRUE if 'name' takes a variable number
 :*                               of arguments
 :*                        = FALSE otherwise
 :*             - 0 <= rank (the current argument rank)
 :*             - 'Stop' is an array stops  (see REMARKS below)
 :*             - 'lg' is the length of 'Stop'
 :*             - 'deltaIn' is the current address in the Working Area
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/   if the CSS is of the form:
 :*                      <actualParam><c><rest>
 :*               where
 :*                  <actualParam> is a valid actual parameter
 :*                  <c> is either a comma or a right parenthesis
 :*                  <rest> is any character sequence
 :*               the effect is the following:
 :*                  - the CSS is <c><rest>
 :*                  - the list of boxes corresponding to an actual parameter
 :*                    (a sequence of boxes to evaluate and move its value to
 :*                    the Working Area) is returned in 'p' (see <DOCUMENTS>)
 :*                  - 'h' is the pointer to the list of holes in this list
 :*                    of boxes
 :*                  - 'type' contains the type of the actual parameter
 :*                otherwise
 :*                  - the CSS is of the form:
 :*                         <first_Stop><rest>
 :*                    where <first_Stop> is the first symbol in
 :*                                 s1 s2 ... sn
 :*                          such that <first_Stop> is in 'Stop[]'
 :*                  - an undefined partial list of boxes is pointed by 'p'
 :*                  - one or more error messages are output to stdout
 :*                  - 'ErrorFound' is set to TRUE
 :*                                                                 /)
 :*
 :*                                                             END-Post
 :*   RETURNS  BoxPtr (/ A pointer to the generated internal code /)
 :*
 :*   REMARKS     (/ Definitions:
 :*                   - let the input stream be:
 :*                                 <t2><t3>...<tn>
 :*                     the symbol sequence:
 :*                             <t1><t2><t3>...<tn>
 :*                     where <t1> is the last symbol been read is called the
 :*                     'current symbol sequence' or CSS for short
 :*                   - the values of the enumerated type 'enum index' are
 :*                     called 'stops'. They are used as indexes to the table
 :*                     'specsymbtab[]' so as a 'stop' identifies an element
 :*                     this table.
 :*                   - refer to <DOCUMENTS> for a detailed description of
 :*                     the internal representation (boxes).
 :*                                                         /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY expr.ActualPar
 :******************************************************************
*/

BoxPtr ActualPar(isSpec, name, rank, type, Stop, lg, deltaIn, h)
char *name, *deltaIn;
int isSpec, rank, Stop[], lg;
enum type *type;
BoxPtr **h;
{
	BoxPtr p, boxp, ActualP();
	int isSimp;
	enum passtype tpass;
	holePtr sp;
	char *dlta;

	if (isSpec) {
	    dlta = deltaIn;
	    deltaIn += sizeof(int);
	}
	p = ActualP(&isSimp, name, rank, type, &tpass, Stop, lg, deltaIn, h);
	if (isSimp && tpass == value && p != NULL) {
		if ((boxp = (BoxPtr) malloc(sizeof(Box))) == NULL)
			error(fatal, 25);
		storeBox(boxp);
		if (*type == integer) boxp->op = move_int1;
		else boxp->op = move_str1;
		boxp->arg.movebox.dest = (hole *) deltaIn;
		if (*type == fieldstring) {
			sp = (holePtr) &(boxp->arg.movebox.source);
			getFieldNameAddr(symb.bsymbval.svalue, sp);
			Expect(symb.ibsymb, Stop, lg);
		} else boxp->arg.movebox.source = (hole *) p;
		boxp->alpha = NULL;
		p = boxp;
		*h = &boxp->alpha;
	}
	if (isSpec) {
	    if ((boxp = (BoxPtr) malloc(sizeof(Box))) == NULL)
		error(fatal, 25);
	    storeBox(boxp);
	    boxp->op = mv_int1;
	    boxp->arg.mv_intbox.val = *type;
	    boxp->arg.mv_intbox.deltaIn = dlta;
	    boxp->alpha = p;
	    p = boxp;
	}
	return (p);
}
/*
 :******************************************************************
 :* UNIT-BODY expr.ActualP
 :*
 :* TITLE         (/ parsing an actual parameter (evaluate)       /)
 :*
 :* PURPOSE       (/ parse a sequence of symbols representing an actual
 :*                  parameter; generate the corresponding internal code for
 :*                  its evaluation if no error is found or recover from this
 :*                  error otherwise                              /)
 :*                                                      END-PURPOSE
 :*
 :* VERSION       001
 :*   CRDATE      1992-11-03
 :*   AUTHOR      (/ Mounji Abdelaziz       , FUNDP               /)
 :*   UPDATE      (/ original issue /)
 :*
 :* REALIZATION   (/ <note, reference to other proc. (optional)> /)
 :*
 :*   CONTROL    (/
 :*
 :*                                                               /)
 :*   DATA       (/
 :*                       - symb       : holds the internal representation
 :*                                      of the last read symbol.
 :*                       - tfp_descr  : table of predefined functions and
 :*                                      procedures descriptors
 :*                       - ErrorFound : = TRUE if an error was found
 :*                                      = FALSE otherwise
 :*                                                                /)
 :*
 :*                                                          END-DATA
 :*   ASSUMPTIONS     (/ <external conditions> /)
 :*   PERFORMANCE     (/ <measures taken or not: when, why, how> /)
 :*   VALIDATION      (/ <for which param. or other data ? How ?> /)
 :*   SYNCHRONIZATION (/ <serialization & locks: purpose, method> /)
 :*   INIT&TERM       (/ <what must already/still be available> /)
 :*   ERROR-HANDLING  (/ <reaction to errors, r.c., state of data>/)
 :*   DOCUMENTS       (/
 :*                                Preliminary report on
 :*                      Advanced Security Audit trail Analysis on uniX
 :*                             (ASAX also called SAT-X)
 :*
 :*                      Advanced Security Audit trail analysis on uniX
 :*                             (ASAX also called SAT-X)
 :*                      Implementation design of the NADF Evaluator   /)
 :*                                                  END-REALIZATION
 :* PARAMETERS
 :*
 :*   Pre    (/ - the CSS is s1 s2 ... sn  (see REMARKS below)
 :*             - 'name' is the procedure or function name
 :*             - 'isSpec' = TRUE if 'name' takes a variable number
 :*                               of arguments
 :*                        = FALSE otherwise
 :*             - 0 <= rank (rank of the current argument)
 :*             - 'Stop' is an array of stops  (see REMARKS below)
 :*             - 'lg' is the length of 'Stop'
 :*             - 'deltaIn' is the current address in the Working Area
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/   if the CSS is of the form:
 :*                      <actualParam><c><rest>
 :*               where
 :*                  <actualParam> is a valid actual parameter
 :*                  <c> is either a comma or a right parenthesis
 :*                  <rest> is any character sequence
 :*               the effect is the following:
 :*                  - the CSS is <c><rest>
 :*                  - the list of boxes corresponding to an actual parameter
 :*                    (a sequence of boxes for its evaluation) is returned
 :*                    in 'p'  (see <DOCUMENTS>)
 :*                  - 'h' is the pointer to the list of holes in this list
 :*                    of boxes
 :*                  - 'isSimp'  = FALSE if at least one box was generated
 :*                              = TRUE otherwise
 :*                  - 'type' contains the type of the actual parameter
 :*                  - 'tpass' = 'value' if the corresponding formal parameter
 :*                                      is a value parameter
 :*                            = 'ref' if the corresponding formal parameter
 :*                                    is a reference parameter
 :*                otherwise
 :*                  - the CSS is of the form:
 :*                         <first_Stop><rest>
 :*                    where <first_Stop> is the first symbol in
 :*                                 s1 s2 ... sn
 :*                          such that <first_Stop> is in 'Stop[]'
 :*                  - an undefined partial list of boxes is pointed by 'p'
 :*                  - one or more error messages are output to stdout
 :*                  - 'ErrorFound' is set to TRUE
 :*                                                                 /)
 :*
 :*                                                             END-Post
 :*   RETURNS  BoxPtr (/ A pointer to the generated internal code /)
 :*
 :*   REMARKS     (/ Definitions:
 :*                   - let the input stream be:
 :*                                 <t2><t3>...<tn>
 :*                     the symbol sequence:
 :*                     <Box        <t1><t2><t3>...<tn>
 :*                     where <t1> is the last symbol been read is called the
 :*                     'current symbol sequence' or CSS for short
 :*                   - the values of the enumerated type 'enum index' are
 :*                     called 'stops'. They are used as indexes to the table
 :*                     'specsymbtab[]' so as a 'stop' identifies an element
 :*                     this table.
 :*                   - refer to <DOCUMENTS> for a detailed description of
 :*                     the internal representation (boxes).
 :*                                                         /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY expr.ActualP
 :******************************************************************
*/
BoxPtr ActualP(isSimp, name, rank, type, tpass, Stop, lg, deltaIn, h)
char *name, *deltaIn;
int *isSimp, rank, Stop[], lg;
enum type *type;
enum passtype *tpass;
BoxPtr **h;
{
	char *addr, *leftExpr();
	BoxPtr p, /*boxp,*/ ActualP1();
	int pass, isConst;
	enum type type1;

	pass = getArgPass(name, rank);
	if (pass == -1) {
		p = NULL;
		*isSimp = TRUE;
		Expect(symb.ibsymb, Stop, lg);
	} else if (pass == -2) {
		error(semantics, 8);
		p = NULL;
		*isSimp = TRUE;
		Expect(symb.ibsymb, Stop, lg);
	} else if (pass == (int) value) {
		p = ActualP1(isSimp, &isConst, type, Stop, lg, deltaIn, h);
		*tpass = value;
	} else { /* pass == ref */
		*isSimp = TRUE;
		if (symb.ibsymb == identifierx) {
		    addr = leftExpr(symb.bsymbval.svalue, type);
		    if ((p = (BoxPtr) malloc(sizeof(Box))) == NULL)
			error(fatal, 25);
		    storeBox(p);
		    p->op = assign_addr1;
		    p->arg.assign_addrbox.gamma1 = deltaIn;
		    p->arg.assign_addrbox.gamma2 = addr;
		    p->alpha = NULL;
		    *h = &p->alpha;
		    *tpass = ref;
		    Expect(symb.ibsymb, Stop, lg);
		} else { /* reference parameter --> identifier is expected */
		    *type = undef;
		    error(syntax, 2);
		    p = NULL;
		}
	}
	if (pass != -1)
	    type1 = (enum type) getArgType(name, rank);
	else
	    type1 = undef;
	if (*type == fieldstring)
	    checkTypes(&type1, bytestring);
	else
	    checkTypes(&type1, *type);
	return (p);
}
/*
 :******************************************************************
 :* UNIT-BODY expr.ActualP1
 :*
 :* TITLE         (/ parsing a value parameter            /)
 :*
 :* PURPOSE       (/ parse a sequence of symbols representing a value
 :*                  parameter; generate the corresponding internal code (for
 :*                  it its evaluation) if no error is found or recover from
 :*                  this error otherwise                                          /)
 :*                                                      END-PURPOSE
 :* VERSION       001
 :*   CRDATE      1992-11-03
 :*   AUTHOR      (/ Mounji Abdelaziz       , FUNDP              /)
 :*   UPDATE      (/ original issue /)
 :*
 :* REALIZATION   (/ <note, reference to other proc. (optional)> /)
 :*
 :*   CONTROL    (/
 :*
 :*                                                               /)
 :*   DATA       (/
 :*                       - symb       : holds the internal representation
 :*                                      of the last read symbol.
			  - ErrorFound : = TRUE if an error was found
 :*                                      = FALSE otherwise
 :*                                                                /)
 :*
 :*                                                          END-DATA
 :*   ASSUMPTIONS     (/ <external conditions> /)
 :*   PERFORMANCE     (/ <measures taken or not: when, why, how> /)
 :*   VALIDATION      (/ <for which param. or other data ? How ?> /)
 :*   SYNCHRONIZATION (/ <serialization & locks: purpose, method> /)
 :*   INIT&TERM       (/ <what must already/still be available> /)
 :*   ERROR-HANDLING  (/ <reaction to errors, r.c., state of data>/)
 :*   DOCUMENTS       (/
 :*                                Preliminary report on
 :*                      Advanced Security Audit trail Analysis on uniX
 :*                             (ASAX also called SAT-X)
 :*
 :*                      Advanced Security Audit trail analysis on uniX
 :*                             (ASAX also called SAT-X)
 :*                      Implementation design of the NADF Evaluator   /)
 :*                                                  END-REALIZATION
 :* PARAMETERS
 :*
 :*   Pre    (/ - the CSS is s1 s2 ... sn  (see REMARKS below)
 :*             - 'Stop' is an array stops (see REMARKS below)
 :*             - 'lg' is the length of 'Stop'
 :*             - 'deltaIn' is the current address in the Working Area
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/   if the CSS is of the form:
 :*                      <ActualParam><c><rest>
 :*               where
 :*                  <actualParam> is a valid value parameter
 :*                  <c> is either a comma or a right parenthesis
 :*                  <rest> is any character sequence
 :*               the effect is the following:
 :*                  - the CSS is <c><rest>
 :*                  - the list of boxes corresponding to a value parameter
 :*                    (a sequence of boxes for its evaluation) is returned
 :*                    in 'p'  (see <DOCUMENTS>)
 :*                  - 'h' is the pointer to the list of holes in this list
 :*                    of boxes
 :*                  - 'type' contains the type of the actual parameter
 :*                  - 'isConst' = TRUE if this expression is neither a
 :*                                fieldname, a variable nor a parameter
 :*                              = FALSE otherwise
 :*                  - 'isSimp'  = FALSE if at least one box was generated
 :*                              = TRUE otherwise
 :*                otherwise
 :*                  - the CSS is of the form:
 :*                         <first_Stop><rest>
 :*                    where <first_Stop> is the first symbol in
 :*                                 s1 s2 ... sn
 :*                          such that <first_Stop> is in 'Stop[]'
 :*                  - an undefined partial list of boxes is pointed by 'p'
 :*                  - one or more error messages are output to stdout
 :*                  - 'ErrorFound' is set to TRUE
 :*                                                                 /)
 :*
 :*                                                             END-Post
 :*   RETURNS  BoxPtr (/ A pointer to the generated internal code /)
 :*
 :*   REMARKS     (/ Definitions:
 :*                   - let the input stream be:
 :*                                 <t2><t3>...<tn>
 :*                     the symbol sequence:
 :*                             <t1><t2><t3>...<tn>
 :*                     where <t1> is the last symbol been read is called the
 :*                     'current symbol sequence' or CSS for short
 :*                   - the values of the enumerated type 'enum index' are
 :*                     called 'stops'. They are used as indexes to the table
 :*                     'specsymbtab[]' so as a 'stop' identifies an element
 :*                     this table.
 :*                   - refer to <DOCUMENTS> for a detailed description of
 :*                     the internal representation (boxes).
 :*                                                         /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY expr.ActualP1
 :******************************************************************
*/
BoxPtr ActualP1(isSimp, isConst, type, Stop, lg, deltaIn, h)
int *isSimp, *isConst, Stop[], lg;
char *deltaIn;
enum type *type;
BoxPtr **h;
{
	BoxPtr p, sxterm();
	hole gamma;
	enum  index s;
	hole **saveStrConst();
	int i = -1, lgStr;
	char *str_add, *ret;
	char *expbuf;

	s = symb.ibsymb;
	if (s == C_literalx || s == X_literalx) {
	    *isConst = TRUE;
	    str_add = symb.bsymbval.svalue;
	    lgStr = (s == C_literalx)? strlen(str_add): symb.X_lg;
	    if (isMatch && !preCompile) { 

#ifdef 	STDREGEX
  		if ((expbuf  = (char *) malloc(BUFSIZE)) == NULL) 
			error(fatal, 25);  /* report allocation problem  */

		/* compile the RE */

  	        ret = compile(str_add, expbuf, &expbuf[ESIZE], '\0');
	        p = (BoxPtr) saveStrConst(expbuf, ret-expbuf);
		free(expbuf);
		preCompile = TRUE;
#endif
#ifdef 	GNUREGEX


		struct re_pattern_buffer pattern_buff;
		const char 		*re_compile_pattern();
		char  			*r, *str;
		hole 			**savePatternBuffer();
		
#if defined(REGEX_USED_SYNTAX)
		re_syntax_options = REGEX_USED_SYNTAX;
#else
		re_syntax_options = RE_SYNTAX_POSIX_EXTENDED;
#endif

  		if ((expbuf  = (char *) malloc(BUFSIZE)) == NULL) 
			error(fatal, 25);  /* report allocation problem  */

		pattern_buff.allocated = BUFSIZE;
		pattern_buff.buffer    = expbuf;
		pattern_buff.fastmap   = 0;
		pattern_buff.translate = 0;
				
		r = re_compile_pattern(str_add, strlen(str_add), &pattern_buff);
		if (r) { 
			fprintf(stderr, "%s: %s\n", str_add, r);
			exit(1);
		}
		p = (BoxPtr) savePatternBuffer((char *) &pattern_buff, 
						sizeof(pattern_buff));
		preCompile = TRUE;
#endif

#ifdef 	POSIXREGEX
		regex_t 	preg;
		char  		*str;
		char 		errMsg[128];
		int 		cflags = REG_EXTENDED;
		int 		errcode;
		hole 		**savePatternBuffer();
		
		preg.allocated = 0;
		preg.buffer    = 0;
		preg.fastmap   = 0;
		preg.translate = 0;
				
		errcode= regcomp(&preg, str_add, cflags);
		if (errcode) { 
			regerror(errcode, &preg, errMsg, 128);
			fprintf(stderr, "%s: %s\n", str_add, errMsg);
			exit(1);
		}
		p = (BoxPtr) savePatternBuffer((char *) &preg, sizeof(preg));
		preCompile = TRUE;
#endif

#ifdef	BSDREGEX
		p = (BoxPtr) saveStrConst(str_add, lgStr);
#endif
	    } else {
	    	p = (BoxPtr) saveStrConst(str_add, lgStr);
	    }
	    *isSimp = TRUE;
	    *type = bytestring;
	    Expect(s, Stop, lg);
	} else if (InStop(s, TermStop, lgTerm) >= 0) {
		if (s == identifierx) {
		i = getAuditDataId(symb.bsymbval.svalue);
		}
		if (i >= 0) {  /* this is a field name */
			*isConst = FALSE;
			*type = fieldstring;
			p = (BoxPtr) TRUE;
			*isSimp = TRUE;
		      /*  Expect(symb.ibsymb, Stop, lg);     */
		} else { /* arithmetic expression */
		    p = sxterm(isSimp, isConst, type, Stop, lg, deltaIn, h);
		}
	} else {
		*isConst = TRUE;
		*type = undef;
		p = NULL;
		SyntaxError(Stop, lg, 3);
	}
	return (p);
}

/*
 :******************************************************************
 :* UNIT-BODY expr.leftExpr
 :*
 :* TITLE         (/ parsing a reference parameter            /)
 :*
 :* PURPOSE       (/ parse a symbol representing a reference parameter;
 :*                  return its address if no error is found or recover from
 :*                  this error otherwise
 :*			                                        /)
 :*                                                      END-PURPOSE
 :* VERSION       001
 :*   CRDATE      1992-11-03
 :*   AUTHOR      (/ Mounji Abdelaziz       , FUNDP              /)
 :*   UPDATE      (/ original issue /)
 :*
 :* REALIZATION   (/ <note, reference to other proc. (optional)> /)
 :*
 :*   CONTROL    (/
 :*
 :*                                                               /)
 :*   DATA       (/
 :*                       - symb       : holds the internal representation
 :*                                      of the last read symbol.
 :*                       - VA_ptr     : address of the Variable Area
 :*                       - ErrorFound : = TRUE if an error was found
 :*                                      = FALSE otherwise
 :*                                                                /)
 :*
 :*                                                          END-DATA
 :*   ASSUMPTIONS     (/ <external conditions> /)
 :*   PERFORMANCE     (/ <measures taken or not: when, why, how> /)
 :*   VALIDATION      (/ <for which param. or other data ? How ?> /)
 :*   SYNCHRONIZATION (/ <serialization & locks: purpose, method> /)
 :*   INIT&TERM       (/ <what must already/still be available> /)
 :*   ERROR-HANDLING  (/ <reaction to errors, r.c., state of data>/)
 :*   DOCUMENTS       (/
 :*                                Preliminary report on
 :*                      Advanced Security Audit trail Analysis on uniX
 :*                             (ASAX also called SAT-X)
 :*
 :*                      Advanced Security Audit trail analysis on uniX
 :*                             (ASAX also called SAT-X)
 :*                      Implementation design of the NADF Evaluator   /)
 :*                                                  END-REALIZATION
 :* PARAMETERS
 :*
 :*   Pre    (/ - 'name' is the procedure or function name
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/ - if 'name' is the name of a known local variable, its
 :*                  absolute address:
 :*                            VA_ptr + offset
 :*                  is returned where offset is the size of this variable.
 :*                otherwise
 :*                  - one or more error messages are output to stdout
 :*                                                                 /)
 :*
 :*                                                             END-Post
 :*   RETURNS  "char *" (/ if > 0 this is the absolute address of a local
 :*                               variable
 :*                        if = NULL an error had occured           /)
 :*   REMARKS     (/
 :*                   - refer to <DOCUMENTS> for a detailed description of
 :*                     the Local Variable Area (LVA).
 :*                                                         /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY expr.leftExpr
 :******************************************************************
*/

char *leftExpr(name, type)
char *name;
enum type *type;
{
	int offset;
	enum var_par pv;
	char *base;

	offset = getVarAddr(name, rule_name, &pv, type);
	switch (pv) {
	    case var      : base = VA_ptr;
			    break;
	    case intGlobal:
	    case extGlobal: base = GV;
			    break;
	}
	if (offset >= 0 && pv > par) { /* This is a variable  */
	    return (base + offset);
	} else if (offset >= 0 && pv == par) { /* This is a parameter */
	    *type = undef;
	    error1(semantics, 11, name); /* not a left value */
	    return (NULL);
	} else if ((offset = getAuditDataId(name)) >= 0) {
	    *type = undef;
	    error1(semantics, 11, name); /* not a left value */
	    return (NULL);
	} else {
	    *type = undef;
	    error1(semantics, 6, name); /* unknown identifier */
	    return (NULL);
	}
}
/*
 :******************************************************************
 :* UNIT-BODY expr.callBox
 :*
 :* TITLE         (/Building a call box                     /)
 :*
 :* PURPOSE       (/Allocate and initialize a memory area representing
 :*                 an abstract machine instruction for a predefined function
 :*                 or procedure call. Output an error message in case of
 :*                 allocation problems                     /)
 :*                                                      END-PURPOSE
 :* VERSION       001
 :*   CRDATE      1992-11-03
 :*   AUTHOR      (/ Mounji Abdelaziz       , FUNDP              /)
 :*   UPDATE      (/ original issue /)
 :*
 :* REALIZATION   (/ <note, reference to other proc. (optional)> /)
 :*
 :*   CONTROL    (/
 :*
 :*                                                               /)
 :*   DATA       (/
 :*                       - symb       : holds the internal representation
 :*                                      of the last read symbol.
 :*                       - tfp_descr  : table of predefined function and
 :*                                      procedure descriptors
 :*                       - ErrorFound : = TRUE if an error was found
 :*                                      = FALSE otherwise
 :*                                                                /)
 :*
 :*                                                          END-DATA
 :*   ASSUMPTIONS     (/ <external conditions> /)
 :*   PERFORMANCE     (/ <measures taken or not: when, why, how> /)
 :*   VALIDATION      (/ <for which param. or other data ? How ?> /)
 :*   SYNCHRONIZATION (/ <serialization & locks: purpose, method> /)
 :*   INIT&TERM       (/ <what must already/still be available> /)
 :*   ERROR-HANDLING  (/ <reaction to errors, r.c., state of data>/)
 :*   DOCUMENTS       (/
 :*                                Preliminary report on
 :*                      Advanced Security Audit trail Analysis on uniX
 :*                             (ASAX also called SAT-X)
 :*
 :*                      Advanced Security Audit trail analysis on uniX
 :*                             (ASAX also called SAT-X)
 :*                      Implementation design of the NADF Evaluator   /)
 :*                                                  END-REALIZATION
 :* PARAMETERS
 :*
 :*   Pre    (/ - 'name' is the name of a known function or procedure name
 :*             - 'isFunc'= TRUE if 'name' is a predefined function name
 :*                       = FALSE if corresponds to a procedure name
 :*             - 'deltaIn' is the current address in the Working Area
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/ - A memory area representing the abstract machine instruction
 :*               for a predefined function or procedure is allocated and
 :*               initialized and a pointer to it is returned.
 :*               In case of allocation problem, an error message is sent to
 :*               stdout                                                 /)
 :*                                                             END-Post
 :*   RETURNS  "BoxPtr" (/ if > 0 this is the address of the internal code
 :*                        if = NULL an allocation problem occured       /)
 :*
 :*   REMARKS  (/  - refer to <DOCUMENTS> for a detailed description of
 :*                     the predefined call internal representation (box).
 :*                                                         /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY expr.leftExpr
 :******************************************************************
*/
BoxPtr callBox(isFunc, name, deltaIn)
unsigned isFunc;
char *name, *deltaIn;
{
	BoxPtr p;
	retfp fptr, getfpPtr();

	if ((p = (BoxPtr) malloc(sizeof(Box))) == NULL)
	    error(fatal, 25);
	storeBox(p);
	if (isFunc) {
	    if (undef == (enum type) getfpType(name))
		error1(semantics, 15, name);
	    p->op = call_f1;
	} else {
	    if (undef != (enum type) getfpType(name))
		error1(semantics, 14, name);
	    p->op = call_p1;
	}
	fptr =  getfpPtr(name);
	if ((int) fptr == -1) {
		p->arg.callbox.fpptr = NULL;
	} else {
		p->arg.callbox.fpptr = fptr;
	}
	p->arg.callbox.deltaIn = deltaIn;
	p->alpha = NULL;
	return (p);
}


/* 
*  	#### 04/04/95 #### for use with regex package
*/

#define lgPatternBuff	200	
hole *tabPatternBuffer[lgPatternBuff];  /* table of pattern buffers */
int patternBuffPtr = 0;   /* next free position in tabPatternBuffer[] */

/* 	savePatternBuffer()
*	
*	save a string constant and return its address
*/

hole **savePatternBuffer(buf, lg) 
char *buf;
int lg;
{
    char *cp, *p;
    unsigned short *usp;

    if ((cp = (char *) malloc(sizeof(long) + lg)) == 0) {
	error(fatal, 25);    /* memory allocation problem */
    } else {
	p = cp;
	usp = (unsigned short *) cp;
	*usp = (unsigned short) (lg + sizeof(long) - usz);
	cp += sizeof(long);
	bcopy(buf, cp, lg);
    }
    if (patternBuffPtr < lgPatternBuff) {
	tabPatternBuffer[patternBuffPtr] = (hole *) p;
	return (tabPatternBuffer + patternBuffPtr++);
    } else {
	  error(fatal, 23);  /* too much string constants */
	  return(NULL);
    }
}

