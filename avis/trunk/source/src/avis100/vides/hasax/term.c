/*
 :******************************************************************
 :* MODULE-DESCRIPTION
 :*
 :* TITLE         (/ Parsing of an expression   /)
 :*
 :* NAME             term.c
 :*
 :* COPYRIGHT     (C)   SIEMENS NIXDORF INFORMATIONSSYSTEME AG  1992
 :*                     ALL RIGHTS RESERVED
 :*
 :* MEMORY-CLASS  6
 :*
 :* PROG-CONVENTS (/ <internal conventions (instance, names,
 :*                  external data, ...). None -> empty text>     /)
 :*
 :* PROCEDURE     sxfactor
 :* REMARKS       (/ parsing of a factor  /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     sxterm
 :* REMARKS       (/ parsing of a term    /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     simpExpr
 :* REMARKS       (/ parsing of a simple expression               /)
 :* ENTRY         EXTERNAL
 :*
 :*-----------------------------------------------------------------
 :*
 :* VERSION       001
 :*   CRDATE      1992-03-09
 :*   AUTHOR      (/ Mounji Abdelaziz       , FUNDP /)
 :*   UPDATE      (/ original issue /)
 :*
 :* END-MODULE-DESCRIPTION term.c
 :*****************************************************************
*/
#include <stdio.h>
#include "asax.h"

/*
 :******************************************************************
 :* UNIT-BODY term.sxfactor
 :*
 :* TITLE         (/ parsing of a factor        /)
 :*
 :* PURPOSE       (/ parse a sequence of symbols representing a factor;
 :*                  generate the corresponding internal code for it if no
 :*                  error is found or recover from this error otherwise /)
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
 :*      <sExpr_1><w_1>...<w_i-1><sExpr_i> <w_i>...<w_n-1><sExpr_n><c><beta>
 :*      <--------------------------------><------------------------------->
 :*               treated  (=alpha)                  to be treated
 :*
 :*      p                    h
 :*       |                    |
 :*       |                    | ...
 :*        ______________                  -------------
 :*       |ir(sExpr_1) |-|--->   ... ---> |ir(sExpr_i)|/|
 :*        --------------                  -------------
 :*
 :*               where
 :*
 :*                  <sExpr_i> is a valid simple expression i=1..N
 :*                  <w_i>     in {'*', 'div', 'mod'} i=1..N-1
 :*                  <c>   not in {'*', 'div', 'mod'}
 :*                  <beta> is any character sequence
 :*                  - ir(sExpr_j) denotes the internal representation of
 :*                    sExpr_j for i=1..j
 :*                  - the CSS is <w_i>...<w_n-1><sExpr_n><c><beta>
 :*                  - the list of boxes corresponding to alpha is pointed by
 :*                    'p'
 :*                  - the list of holes corresponding to this list of boxes
 :*                    is pointed by 'h'
 :*                  - 'type' contains the type of the sub factor 'alpha'
 :*                  - 'isConst' = TRUE if this sub factor is neither a
 :*                                fieldname, a variable nor a parameter
 :*                              = FALSE otherwise
 :*                  - 'isSimp'  = FALSE if at least one box was generated
 :*                              = TRUE otherwise
 :*
 :*                                                               /)
 :*   DATA       (/
 :*                       - symb       : holds the internal representation
 :*                                      of the last read symbol.
 :*                       - nextchar   : the last character being read.
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
 :*   Pre    (/ - the CSS is s1 s2 ... sn   (see REMARKS below)
 :*             - 'Stop' is an array stops  (see REMARKS below)
 :*             - 'lg' is the length of 'Stop'
 :*             - 'deltaIn' is the current address in the Working Area
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/   if the CSS is of the form:
 :*                  <simpExpr_1><w_1>...<w_N-1><simpExpr_N><c><beta>
 :*               where
 :*                  <simpExpr_i> is a valid simple expression i=1..N
 :*                  <w_i>     in {'*', 'div', 'mod'} i=1..N-1
 :*                  <c>   not in {'*', 'div', 'mod'}
 :*                  <beta> is any character sequence
 :*               the effect is the following:
 :*                  - the CSS is <c><beta>
 :*                  - the pointer to the corresponding list of boxes is
 :*                    returned in 'p'
 :*                  - 'h' is the pointer to the list of holes in the list of
 :*                    boxes
 :*                  - 'type' contains the type of the factor
 :*                  - 'isConst' = TRUE if this sub factor is neither a
 :*                                fieldname, a variable nor a parameter
 :*                              = FALSE otherwise
 :*                  - 'isSimp'  = FALSE if at least one box was generated
 :*                              = TRUE otherwise
 :*                otherwise
 :*                  - the CSS is of the form:
 :*                         <first_Stop><rest>
 :*                    where <first_Stop> is the first symbol in
 :*                                 s1 s2 ... sN
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
 :*                                                         /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY term.sxfactor
 :******************************************************************
*/
BoxPtr sxfactor(isSimp, isConst, type, Stop, lg, deltaIn, h)
int *isSimp, *isConst, Stop[], lg;
enum type *type;
char *deltaIn;
BoxPtr **h;
{
	BoxPtr p, p1, boxp, *h1, simpExpr();
	int isSimp1, isConst1, S1[Mlg], lg1, S2[Mlg], lg2;
	char *deltaIn1;
	enum type type1;
	enum operation op;

	lg1 = merge(Stop, lg, MultStop, lgMult, S1);
	lg2 = merge(S1, lg1, FactorStop, lgFact, S2);
	p = simpExpr(isSimp, isConst, type, S1, lg1, deltaIn, h);
	while (InStop(symb.ibsymb, MultStop, lgMult) >= 0) {
		*isConst = FALSE;
		op = (enum operation) symb.ibsymb;
		Expect(op, S2, lg2);
		deltaIn1 = deltaIn + sizeof(int);
		p1 = simpExpr(&isSimp1, &isConst1, &type1, S1, lg1, deltaIn1, &h1);
		checkTypes(type, integer);
		checkTypes(&type1, integer);
		if ((boxp = (BoxPtr) malloc(sizeof(Box))) == NULL)
			error(fatal, 25);   /* allocation problem */
		storeBox(boxp);
		boxp->op = op;
		boxp->arg.arithmbox.deltaIn = (int *) deltaIn;
		boxp->arg.arithmbox.gamma1 =
		(*isSimp)? (int *) p: (int *) deltaIn;
		boxp->arg.arithmbox.gamma2 =
		(isSimp1)? (int *) p1: (int *) deltaIn1;
		boxp->alpha = (BoxPtr) NULL;
		if (*isSimp && isSimp1)            {
			p = boxp;
		} else if (*isSimp && !isSimp1)    {
			p = p1;
			*h1 = boxp;
		} else if (!(*isSimp) && isSimp1)   {
			**h = boxp;
		} else if (!(*isSimp) && !isSimp1) {
			**h = p1;
			*h1 = boxp;
		}
		*h = &boxp->alpha;
		*isSimp = FALSE;
	}
	return ((BoxPtr) p);
}
/*
 :******************************************************************
 :* UNIT-BODY term.sxterm
 :*
 :* TITLE         (/ parsing of a term       /)
 :*
 :* PURPOSE       (/ parse a sequence of symbols representing a term;
 :*                  generate the corresponding internal code for it if no
 :*                  error is found or recover from this error otherwise /)
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
 :*    <w_0><fact_1><w_1>...<w_i-1><fact_i> <w_i>...<w_n-1><fact_n><c><beta>
 :*    <----------------------------------> <------------------------------>
 :*               treated  (= alpha)                  to be treated
 :*
 :*      p                    h
 :*       |                    |
 :*       |                    | ...
 :*        ______________                  ------------
 :*       |ir(fact_1)  |-|--->   ... ---> |ir(fact_i)|/|
 :*        --------------                  ------------
 :*
 :*               where
 :*
 :*                  <fact_i> is a valid factor i=1..N
 :*                  <w_0> is either in {'+', '-'} or is an empty sequence
 :*                  <w_i>     in {'+', '-'} i=1..N-1
 :*                  <c>   not in {'+', '-'}
 :*                  <beta> is any character sequence
 :*                  - ir(fact_j) denotes the internal representation of
 :*                    fact_j for i=1..j
 :*                  - the CSS is <w_i>...<w_n-1><fact_n><c><beta>
 :*                  - the list of boxes corresponding to alpha is pointed by
 :*                    'p'
 :*                  - the list of holes corresponding to this list of boxes
 :*                    is pointed by 'h'
 :*                  - 'type' contains the type of the sub term 'alpha'
 :*                  - 'isConst' = TRUE if this sub factor is neither a
 :*                                fieldname, a variable nor a parameter
 :*                              = FALSE otherwise
 :*                  - 'isSimp'  = FALSE if at least one box was generated
 :*                              = TRUE otherwise
 :*
 :*                                                               /)
 :*   DATA       (/
 :*                       - symb       : holds the internal representation
 :*                                      of the last read symbol.
 :*                       - nextchar   : the last character being read.
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
 :*   Pre    (/ - the CSS is s1 s2 ... sN   (see REMARKS below)
 :*             - 'Stop' is an array stops  (see REMARKS below)
 :*             - 'lg' is the length of 'Stop'
 :*             - 'deltaIn' is the current address in the Working Area
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/   if the CSS is of the form:
 :*                  <w_0><fact_1><w_1>...<w_N-1><fact_N><c><beta>
 :*               where
 :*                  <fact_i> is a valid simple expression i=1..N
 :*                  <w_0> is either in {'+', '-'} or is the empty sequence
 :*                  <w_i>     in {'+', '-'} i=1..N-1
 :*                  <c>   not in {'+', '-'}
 :*                  <beta> is any character sequence
 :*               the effect is the following:
 :*                  - the CSS is <c><beta>
 :*                  - the pointer to the corresponding list of boxes is
 :*                    returned in 'p'
 :*                  - 'h' is the pointer to the list of holes in the list of
 :*                    boxes
 :*                  - 'type' contains the type of the term
 :*                  - 'isConst' = TRUE if this sub factor is neither a
 :*                                fieldname, a variable nor a parameter
 :*                              = FALSE otherwise
 :*                  - 'isSimp'  = FALSE if at least one box was generated
 :*                              = TRUE otherwise
 :*                otherwise
 :*                  - the CSS is of the form:
 :*                         <first_Stop><rest>
 :*                    where <first_Stop> is the first symbol in
 :*                                 s1 s2 ... sN
 :*                          such that <first_Stop> in 'Stop[]'
 :*                  - an undefined partial list of boxes is pointed by 'p'
 :*                  - an undefined partial list of holes is pointed by 'h'
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
 :*                     in this table.
 :*                   - refer to <DOCUMENTS> for a detailed description of
 :*                     the internal representation (boxes).
 :*
 :*                                                         /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY term.sxterm
 :******************************************************************
*/
BoxPtr sxterm(isSimp, isConst, type, Stop, lg, deltaIn, h)
int *isSimp, *isConst, Stop[], lg;
enum type *type;
char *deltaIn;
BoxPtr **h;
{
	BoxPtr p, p1, boxp, *h1, sxfactor();
	int isSimp1, isConst1, S1[Mlg], lg1, S2[Mlg], lg2;
	enum type type1;
	char *deltaIn1;
	enum operation op;

	lg1 = merge(Stop, lg, AddStop, lgAdd, S1);
	lg2 = merge(S1, lg1, FactorStop, lgFact, S2);
	if (InStop(symb.ibsymb, AddStop, lgAdd) >= 0) {
		*isConst = FALSE;
		op = (enum operation) symb.ibsymb;
		Expect(op, S2, lg2);
		p = sxfactor(isSimp, &isConst1, type, S1, lg1, deltaIn, h);
		checkTypes(type, integer);
		if ((boxp = (BoxPtr) malloc(sizeof(Box))) == NULL)
			error(fatal, 25); /* allocation problem */
		storeBox(boxp);
		boxp->op = un_minus1;
		boxp->arg.un_minusbox.deltaIn = (int *) deltaIn;
		boxp->arg.un_minusbox.gamma = (*isSimp)? (int *) p :
							 (int *) deltaIn;
		boxp->alpha = NULL;
		if (*isSimp) {
			p = boxp;
		} else             /* !(*isSimp) */
			**h = boxp;
		*h = &boxp->alpha;
		*isSimp = FALSE;
	} else p = sxfactor(isSimp, isConst, type, S1, lg1, deltaIn, h);
	while (InStop(symb.ibsymb, AddStop, lgAdd) >= 0) {
		*isConst = FALSE;
		op = (enum operation) symb.ibsymb;
		Expect(op, S2, lg2);
		deltaIn1 = deltaIn + sizeof(int);
		p1 = sxfactor(&isSimp1, &isConst1, &type1,
			      S1, lg1, deltaIn1, &h1);
		checkTypes(type, integer);
		checkTypes(&type1, integer);
		if ((boxp = (BoxPtr) malloc(sizeof(Box))) == NULL)
			error(fatal, 25); /* allocation problem */
		storeBox(boxp);
		boxp->op = op;
		boxp->arg.arithmbox.deltaIn = (int *) deltaIn;
		boxp->arg.arithmbox.gamma1 =
		(*isSimp)? (int *) p: (int *) deltaIn;
		boxp->arg.arithmbox.gamma2 =
		(isSimp1)? (int *) p1: (int *) deltaIn1;
		boxp->alpha = NULL;

		if (*isSimp && isSimp1)            {
			p = boxp;
		} else if (*isSimp && !isSimp1)    {
			p = p1;
			*h1 = boxp;
		} else if (!(*isSimp) && isSimp1)  {
			**h = boxp;
		} else if (!(*isSimp) && !isSimp1) {
			**h = p1;
			*h1 = boxp;
		}
		*h = &boxp->alpha;
		*isSimp = FALSE;
	}
	return (p);
}
/*
 :******************************************************************
 :* UNIT-BODY term.simpExpr
 :*
 :* TITLE         (/ parsing of a simple expression       /)
 :*
 :* PURPOSE       (/ parse a sequence of symbols representing a simple
 :*                  expression; generate the corresponding internal code for
 :*                  it if no error is found or recover from this error
 :*                  otherwise                                          /)
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
 :*                       - nextchar   : the last character being read.
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
 :*             - 'Stop' is an array stops  (see REMARKS below)
 :*             - 'lg' is the length of 'Stop'
 :*             - 'deltaIn' is the current address in the Working Area
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/   if the CSS is of the form:
 :*                      <simpExpr><rest>
 :*               where
 :*                  <simpExpr> is a valid simple expression
 :*                  <rest> is any character sequence
 :*               the effect is the following:
 :*                  - the CSS is <rest>
 :*                  - the pointer to the corresponding list of boxes is
 :*                    returned in 'p'
 :*                  - 'h' is the pointer to the list of holes in the list of
 :*                    boxes
 :*                  - 'type' contains the type of the simple expression
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
 :* END-BODY term.simpExpr
 :******************************************************************
*/
BoxPtr simpExpr(isSimp, isConst, type, Stop, lg, deltaIn, h)
int *isSimp, *isConst, Stop[], lg;
enum type *type;
char *deltaIn;
BoxPtr **h;
{
	BoxPtr p, p0, sxterm(), ActualParList(), callBox();
	int isConst1, lgStr;
	int S1[Mlg], lg1, S2[Mlg], lg2, S3[Mlg], lg3, S4[Mlg], lg4, d;
	enum index s;
	char name[ident_lg], *base, *str;
	enum type type1;
	enum var_par pv;
	unsigned isFunc;
	hole **saveStrConst();
	char *str_add;
	int k; 		/* routine address */

	insert_stop(rparenthesis, Stop, lg, S1, &lg1);
	lg4 = merge(S1, lg1, TermStop, lgTerm, S4);
	insert_stop(lparenthesis, S1, lg1, S2, &lg2);
	s = symb.ibsymb;
	if (s == lparenthesis) {
		*isConst = FALSE;
		Expect(lparenthesis, S4, lg4);
		p = sxterm(isSimp, &isConst1, type, S1, lg1, deltaIn, h);
		Expect(rparenthesis, Stop, lg);
	} else if (s == integerxx) {
		*isConst = TRUE;
		p = (BoxPtr) saveIntConst(symb.bsymbval.ivalue);
		*isSimp = TRUE;
		*type = integer;
		Expect(integerxx, Stop, lg);
	} else if (s == C_literalx || s == X_literalx) {
		*isConst = TRUE;
		str_add = symb.bsymbval.svalue;
		lgStr = (s == C_literalx)? strlen(str_add): symb.X_lg;
		p = (BoxPtr) saveStrConst(str_add, lgStr);
		*isSimp = TRUE;
		*type = bytestring;
		Expect(s, Stop, lg);
	} else if (s == identifierx) {
		ExpectName(name, S2, lg2);
		if (symb.ibsymb == lparenthesis) {
			*isConst = TRUE;
			lg3 = merge(ActParStop, lgActPar, S1, lg1, S3);
			Expect(lparenthesis, S3, lg3);
			*type = (enum type) getfpType(name);
			isFunc = TRUE;
			p = (BoxPtr) ActualParList(isFunc, name, &type1,
						   S1, lg1, deltaIn, h);
			Expect(rparenthesis, Stop, lg);
			*isSimp = FALSE;
		} else if ((k = getfpAddr(name)) >= 0) {
			*isConst = TRUE;
			isFunc = TRUE;
			*type = (enum type) getfpType(name);
			p = callBox(isFunc, name, deltaIn);
			if (isSpecial(k)) { /* a variable arg. routine */
			    if ((p0 = (BoxPtr) malloc(sizeof(Box))) == NULL)
				error(fatal, 25);
			    storeBox(p0);
			    p0->op = mv_int1;
			    p0->arg.mv_intbox.val = 0;
			    p0->arg.mv_intbox.deltaIn = deltaIn + sizeof(int);
			    p0->alpha = p;
			    *h = &p->alpha;
			    p = p0;
			} else 
			    *h = &p->alpha;
			*isSimp = FALSE;
		} else if ((d = getVarAddr(name, rule_name, &pv, type)) >= 0) {
				*isConst = FALSE;
				*isSimp = TRUE;
				switch (pv) {
				   case par      : base = PA_ptr;
						   break;
				   case var      : base = VA_ptr;
						   break;
				   case intGlobal:
				   case extGlobal: base = GV;
						   break;
				}
				p = (BoxPtr) (base + d);
		} else if ((d = getAuditDataId(name)) >= 0) {
				*isConst = FALSE;
				*isSimp = TRUE;
				*type = fieldstring;
				str = tmalloc(ident_lg, char);
				strcpy(str, name);
				p = (BoxPtr) str;
		} else  {
				*isConst = TRUE;
				*isSimp = TRUE;
				*type = undef;
				error1(semantics, 6, name);/* unknown identif */
				p = NULL;
		}
	} else {
		*isConst = TRUE;
		*isSimp = TRUE;
		*type = undef;
		SyntaxError(Stop, lg, 3);
		p = NULL;
	}
	return (p);
}
