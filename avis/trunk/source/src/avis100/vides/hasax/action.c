/*
 :******************************************************************
 :* MODULE-DESCRIPTION
 :*
 :* TITLE         (/ parsing an action /)
 :*
 :* NAME          action.c
 :*
 :* COPYRIGHT     (C)   SIEMENS NIXDORF INFORMATIONSSYSTEME AG  1992
 :*                     ALL RIGHTS RESERVED
 :*
 :* MEMORY-CLASS  6
 :*
 :* PROG-CONVENTS (/ <internal conventions (instance, names,
 :*                  external data, ...). None -> empty text>     /)
 :*
 :* PROCEDURE     assignt
 :* REMARKS       (/ parsing an assignment action             /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     action
 :* REMARKS       (/ parsing an action             /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     subAction
 :* REMARKS       (/ parsing an assignment or a call to a predefined    /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     iffi
 :* REMARKS       (/ parsing a conditional action               /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     dood
 :* REMARKS       (/ parsing a repetitive action     /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     compoundAction
 :* REMARKS       (/ parsing a compound action /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     compAction
 :* REMARKS       (/ parsing a sequence of actions             /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     compndAction
 :* REMARKS       (/ parsing a non empty sequence of actions         /)
 :* ENTRY         EXTERNAL
 :*
 :*-----------------------------------------------------------------
 :*
 :* VERSION       001
 :*   CRDATE      1992-11-03
 :*   AUTHOR      (/ Mounji Abdelaziz    , FUNDP /)
 :*   UPDATE      (/ original issue /)
 :*
 :* END-MODULE-DESCRIPTION action.c
 :******************************************************************
*/

#include <stdio.h>
#include "asax.h"

/*
 :******************************************************************
 :* UNIT-BODY action.assignt
 :*
 :* TITLE         (/ parsing an assignment action      /)
 :*
 :* PURPOSE       (/ parsing a sequence of symbols representing an assignment;
 :*                  generate the corresponding internal code if no error is
 :*                  found or recover from this error otherwise/)
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
 :*             - 'name' a string representing an identifier
 :*             - 'deltaIn' is the current address in the WA
 :*             - 'Stop' is an array stops (see REMARKS below)
 :*             - 'lg' is the length of 'Stop'
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/   if the CSS is of the following form:
 :*                   <identifier> <":="> <expression> <rest>
 :*               where
 :*                  - <identifier> is a valid identifier with possible
 :*                                 ending spaces
 :*                  - <":="> is the assignment operator ":=" with possible
 :*                           ending spaces
 :*                  - <expression> is a sequence of symbols representing a
 :*                                 valid expression
 :*                  - <rest> is any character sequence
 :*                  - the identifier and expression are of the same type
 :*               the effect is the following:
 :*                  - the CSS is <rest>
 :*                  - a pointer to the corresponding list of boxes is
 :*                    returned in 'p'
 :*                  - 'head' and 'tail' are respectively the head and tail
 :*                    of the list of holes in this list of boxes
 :*
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
 :* END-BODY action.assignt
 :******************************************************************
*/

BoxPtr assignt(name, deltaIn, Stop, lg, head, tail)
char *name;                             /* the identifier */
char *deltaIn;                          /* current address in WA */
int Stop[], lg;                         /* the stops */
holePtr *head, *tail;                   /* the list of holes */
{
    BoxPtr *h1, p, boxp, sxterm();      /* box pointers */
    int S1[Mlg], lg1, S2[Mlg], lg2;     /* temporary stops */
    char *addr;                         /* temporary */
    char *leftExpr();                   /* handle a left expression */
    unsigned isSimp;                    /* at least one box generated */
    unsigned isConst;                   /* must this be saved */
    enum type type1, type2;             /* temporary types */

    /* build stops */

    lg1 = merge(Stop, lg, TermStop, lgTerm, S1);
    addr = leftExpr(name, &type1);        /* return local variable address */
    Expect(assign, S1, lg1); /* read ":=" keyword */
    p = sxterm(&isSimp, &isConst, &type2, Stop, lg, deltaIn, &h1);
    /* do type checking */
    if (type2 == fieldstring) checkTypes(&type1, bytestring);
    else checkTypes(&type1, type2);
    if ((boxp = (BoxPtr) malloc(sizeof(Box))) == 0)
	error(fatal, 25); /* allocation problem */
    storeBox(boxp);		/* for tests only */
    if (type1 == integer) {
	 boxp->op = assign1;
	 boxp->arg.assign_intbox.gamma1 = (int *) addr;/* variable address */
	 boxp->arg.assign_intbox.gamma2 = (isSimp)? (int *) p:
						    (int *) deltaIn;
    } else {
	 boxp->op = assign_str1;
	 boxp->arg.assign_strbox.gamma1 = (hole *) addr;
	 if (type2 == fieldstring) {  /* this is a field name */
	   /* install a new field name in the indirection table */
	   /* or insert a new hole for this field name */
	    getFieldNameAddr((char *) p,
			     (holePtr) &boxp->arg.assign_strbox.gamma2);
	 } else {  /* type2 == bytestring */
	    boxp->arg.assign_strbox.gamma2 =
	    (isSimp)? (hole *) p: (hole *) deltaIn;
	 }
      }
    boxp->alpha = (BoxPtr) NULL;
    *head = *tail = (holePtr) &boxp->alpha;
    if (isSimp) {
	return(boxp);
    } else {
	*h1 = boxp;
	return(p);
    }
}
/*
 :******************************************************************
 :* UNIT-BODY  action.action
 :*
 :* TITLE         (/ parsing an action       /)
 :*
 :* PURPOSE       (/ parsing a sequence of symbols representing an action;
 :*                  generate the corresponding internal code if no error is
 :*                  found or recover from this error otherwise/)
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
 :*             - 'deltaIn' is the current address in the WA
 :*             - 'Stop' is an array stops (see REMARKS below)
 :*             - 'lg' is the length of 'Stop'
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/   if the CSS is of the following form:
 :*                   <action><rest>
 :*               where
 :*                  - <action> is a sequence of symbols representing a valid
 :*                             action i.e, either of:
 :*
 :*                         <compound_action>  (a compound action)
 :*                         <skip>             (the empty action)
 :*                         <if_fi>            (conditional action)
 :*                         <do_od>            (repetitive action)
 :*                         <rule_triggering>  (rule triggering)
 :*                         <assignment>       (assignment)
 :*                         <predef_call>      (predefined call)
 :*
 :*                  - <rest> is any character sequence
 :*               the effect is the following:
 :*                  - the CSS is <rest>
 :*                  - a pointer to the corresponding list of boxes is
 :*                    is returned in 'p'
 :*                  - 'head' and 'tail' are respectively the head and tail
 :*                    of the list of holes in this list of boxes
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
 :* END-BODY  action.action
 :******************************************************************
*/
/* action ::= compoundAction
	| iffi
	| dood
	| sxtrigger
	| assignt
	| predefined procedure call
*/
BoxPtr action(deltaIn, Stop, lg, head, tail)
char *deltaIn;                      /* current address in WA */
int Stop[], lg;                     /* the stops */
holePtr *head, *tail;               /* list of holes */
{
    int S1[Mlg], lg1, S2[Mlg], lg2; /* temporary stops */
    BoxPtr compoundAction(), iffi(), dood(), sxtrigger(), subAction();
    char *name;                     /* local variable name */

    switch(symb.ibsymb) {
	case beginx:      return(compoundAction(deltaIn, Stop, lg,
						head, tail));
	case skipx:       Expect(skipx, Stop, lg);
			  return(NULL);
	case ifx:         return(iffi(deltaIn, Stop, lg, head, tail));
	case dox:         return(dood(deltaIn, Stop, lg, head, tail));
	case triggerx:    return(sxtrigger(Stop, lg, head, tail));
	case identifierx:
			  /* either an assignment or a predefined call */
			  insert_stop(lparenthesis, Stop, lg, S2, &lg2);
			  insert_stop(assign, S2, lg2, S1, &lg1);
			  name = tmalloc(ident_lg, char);
			  ExpectName(name, S1, lg1);
			  return(subAction(name, deltaIn, Stop, lg, head,
					   tail));
	default:          error(syntax, 13); /* action expected */
			  return(NULL);
    }
}
/*
 :******************************************************************
 :* UNIT-BODY  action.subAction
 :*
 :* TITLE         (/ parsing an assignment or predefined call      /)
 :*
 :* PURPOSE       (/ parsing a sequence of symbols representing either an
 :*                  assignment or a predefined call; generate the
 :*                  corresponding internal code if no error is found or
 :*                  recover from this error otherwise /)
 :*                                                          END-PURPOSE
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
 :*             - 'name' a string representing an identifier
 :*             - 'deltaIn' is the current address in the WA
 :*             - 'Stop' is an array stops (see REMARKS below)
 :*             - 'lg' is the length of 'Stop'
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/   if the CSS is of the following form:
 :*                  <assingt_or_predef_call><rest>
 :*               where
 :*                  - <assingt_or_predef_call> is a sequence of symbols
 :*                             representing either an assignment or a call
 :*                             to predefined procedure or function
 :*                  - <rest> is any character sequence
 :*               the effect is the following:
 :*                  - the CSS is <rest>
 :*                  - a pointer to the corresponding list of boxes is
 :*                    returned in 'p'
 :*                  - 'head' and 'tail' are respectively the head and tail
 :*                    of the list of holes in this list of boxes
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
 :* END-BODY action.subAction
 :******************************************************************
*/
BoxPtr subAction(name, deltaIn, Stop, lg, head, tail)
char *name;                         /* identifier name */
char *deltaIn;                      /* current address in WA */
int Stop[], lg;                     /* the stops */
holePtr *head, *tail;               /* list of holes */
{
    int S1[Mlg], lg1, S2[Mlg], lg2;
    enum index s;
    enum type type;
    BoxPtr *h, p, p0, assignt(), ActualParList(), callBox();
    unsigned isFunc; /* is this a function or a procedure */
    int k; 		/* routine address */

    s = symb.ibsymb;
    if (s == assign) {	/* this is an assignment */
	p = assignt(name, deltaIn, Stop, lg, head, tail);
    } else if (s == lparenthesis) { /* this is a predefined call */
	isFunc = FALSE;             /* this is a procedure */
	insert_stop(rparenthesis, Stop, lg, S1, &lg1);
	lg2 = merge(ActParStop, lgActPar, S1, lg1, S2);
	Expect(lparenthesis, S2, lg2);    /* read left parenthesis */
	p = ActualParList(isFunc, name, &type, S1, lg1, deltaIn, &h);
	*head = *tail = (holePtr) h;      /* listof holes */
	Expect(rparenthesis, Stop, lg);   /* read right parenthesis */
    } else if ((k = getfpAddr(name)) >= 0) {
	isFunc = FALSE;              /* this is a procedure */
	p = callBox(isFunc, name, deltaIn);

	if (isSpecial(k)) { /* a variable arg. routine */
		if ((p0 = (BoxPtr) malloc(sizeof(Box))) == NULL)
			error(fatal, 25);
		storeBox(p0);
		p0->op = mv_int1;
		p0->arg.mv_intbox.val = 0;
		p0->arg.mv_intbox.deltaIn = deltaIn;
		p0->alpha = p;
		*head = *tail = (holePtr) &p->alpha;
		p = p0;
	} else 
		*head = *tail = (holePtr) &p->alpha;
    } else {
        printf("(action.c:)");
	error1(semantics, 7, name);
    }
    return(p);
}
/*
 :******************************************************************
 :* UNIT-BODY action.iffi
 :*
 :* TITLE         (/ parsing a conditional action       /)
 :*
 :* PURPOSE       (/ parsing a sequence of symbols representing a conditional
 :*                  action; generate the corresponding internal code if
 :*                  no error is found or recover from this error otherwise/)
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
 :*             - 'deltaIn' is the current address in WA area
 :*             - 'Stop' is an array stops (see REMARKS below)
 :*             - 'lg' is the length of 'Stop'
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/   if the CSS is of the following form:
 :*                   <"if"> <guardedAction> <"fi"><rest>
 :*               where
 :*                  - <"if"> is the keyword "if" with possible ending spaces
 :*                  - <"fi"> is the keyword "fi" with possible ending spaces
 :*                  - <guardedAction> is a comma separated sequence of
 :*                                    guarded actions
 :*                  - <rest> is any character sequence
 :*               the effect is the following:
 :*                  - the CSS is <rest>
 :*                  - a pointer to the list of boxes corresponding to this
 :*                    conditional action is returned in 'p'
 :*                  - 'head' and 'tail' are respectively the head and tail
 :*                    of the list of holes in this list of boxes
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
 :* END-BODY action.iffi
 :******************************************************************
*/

/* iffi ::= if guardedAction fi */

BoxPtr iffi(deltaIn, Stop, lg, head, tail)
char *deltaIn;                      /* current address in WA */
int Stop[], lg;                     /* the stops */
holePtr *head, *tail;               /* list of holes */
{
    int S1[Mlg], lg1, S2[Mlg], lg2; /* temporary stops */
    BoxPtr p, guardedAction();

    /* build stops */
    insert_stop(fix, Stop, lg, S2, &lg2);
    lg1 = merge(S2, lg2, CondStop, lgCond, S1);
    Expect(ifx, S1, lg1); /* read keyword "if" */
    /* parse the sequence of guarded actions and generate code */
    p = guardedAction(deltaIn, S2, lg2, head, tail);
    Expect(fix, Stop, lg); /* read keyword "fi" */
    return(p);
}
/*
 :******************************************************************
 :* UNIT-BODY action.dood
 :*
 :* TITLE         (/ parsing a repetitive action       /)
 :*
 :* PURPOSE       (/ parsing a sequence of symbols representing a repetitive
 :*                  action; generate the corresponding internal code if
 :*                  no error is found or recover from this error otherwise/)
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
 :*             - 'deltaIn' is the current address in WA area
 :*             - 'Stop' is an array stops (see REMARKS below)
 :*             - 'lg' is the length of 'Stop'
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/   if the CSS is of the following form:
 :*                   <"do"> <guardedAction> <"od"><rest>
 :*               where
 :*                  - <"do"> is the keyword "do" with possible ending spaces
 :*                  - <"od"> is the keyword "od" with possible ending spaces
 :*                  - <guardedAction> is a comma separated sequence of
 :*                                    guarded actions
 :*                  - <rest> is any character sequence
 :*               the effect is the following:
 :*                  - the CSS is <rest>
 :*                  - a pointer to the list of boxes corresponding to this
 :*                    repetitive action is returned in 'p'
 :*                  - 'head' and 'tail' are respectively the head and tail
 :*                    of the list of holes in this list of boxes
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
 :* END-BODY action.dood
 :******************************************************************
*/
/* dood ::= do doGuardedAction od */

BoxPtr dood(deltaIn, Stop, lg, head, tail)
char *deltaIn;                      /* current address in WA */
int Stop[], lg;                     /* the stops */
holePtr *head, *tail;               /* list of holes */
{
    int S1[Mlg], lg1, S2[Mlg], lg2;/* temporary stops */
    BoxPtr p, doGuardedAction();

    insert_stop(odx, Stop, lg, S2, &lg2);
    lg1 = merge(S2, lg2, CondStop, lgCond, S1);
    Expect(dox, S1, lg1);
    p = doGuardedAction(deltaIn, S2, lg2, head, tail);
    Expect(odx, Stop, lg);
    return(p);
}
/*
 :******************************************************************
 :* UNIT-BODY action.compoundAction
 :*
 :* TITLE         (/ parsing compoundAction      /)
 :*
 :* PURPOSE       (/ parsing a sequence of symbols representing a compound
 :*                  action; generate the corresponding internal code if
 :*                  no error is found or recover from this error otherwise/)
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
 :*             - 'deltaIn' is the current address in WA area
 :*             - 'Stop' is an array stops (see REMARKS below)
 :*             - 'lg' is the length of 'Stop'
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/   if the CSS is of the following form:
 :*                <compoundAction><rest>
 :*               where
 :*                  - <compoundAction> is a sequence of symbols representing
 :*                            a valid compound action
 :*                  - <rest> is any character sequence
 :*               the effect is the following:
 :*                  - the CSS is ")"<rest>
 :*                  - a pointer to the list of boxes corresponding to this
 :*                    compound action is returned in 'p'
 :*                  - 'head' and 'tail' are respectively the head and tail
 :*                    of the list of holes in this list of boxes
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
 :* END-BODY action.compoundAction
 :******************************************************************
*/
/* compoundAction ::= begin compAction end */

BoxPtr compoundAction(deltaIn, Stop, lg, head, tail)
char *deltaIn;                      /* current address in WA */
int Stop[], lg;                     /* the stops */
holePtr *head, *tail;               /* list of holes */
{
    int S1[Mlg], lg1, S2[Mlg], lg2; /* temporary stops */
    BoxPtr p, compAction();

   /* build stops */
    insert_stop(endx, Stop, lg, S2, &lg2);
    lg1 = merge(S2, lg2, ActionStop, lgAction, S1);
    Expect(beginx, S1, lg1);        /* read keyword "begin" */
    /* parse a sequence of actions and generate code */
    p = compAction(deltaIn, S2, lg2, head, tail);
    Expect(endx, Stop, lg);         /* read keyword "end" */
    return(p);
}

/* compAction ::= [compndAction] */

/*
 :******************************************************************
 :* UNIT-BODY action.compAction
 :*
 :* TITLE         (/ parsing a sequence of actions      /)
 :*
 :* PURPOSE       (/ parsing a sequence of symbols representing a possibly
 :*                  empty, semicolon separated sequence of actions; generate
 :*                  the corresponding internal code if no error is found or
 :*                  recover from this error otherwise /)
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
 :*             - 'deltaIn' is the current address in WA area
 :*             - 'Stop' is an array stops (see REMARKS below)
 :*             - 'lg' is the length of 'Stop'
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/   if the CSS is of the following form:
 :*                   <action_1> ";" ... ";" <action_N> ";" <rest>
 :*               where
 :*                  - N >= 0 (the sequence may be empty)
 :*                  - <action_i> is a sequence of symbols representing a
 :*                            valid action
 :*                  - <rest> is any character sequence
 :*               the effect is the following:
 :*                  - the CSS is <rest>
 :*                  - a pointer to the corresponding list of boxes is
 :*                    returned in 'p'
 :*                  - 'head' and 'tail' are respectively the head and tail
 :*                    of the list of holes in this list of boxes
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
 :* END-BODY action.compAction
 :******************************************************************
*/

BoxPtr compAction(deltaIn, Stop, lg, head, tail)
char *deltaIn;                      /* current address in WA */
int Stop[], lg;                     /* the stops */
holePtr *head, *tail;               /* list of holes */
{
    int S1[Mlg], lg1;               /* temporary stops */
    BoxPtr p, compndAction();

   /* build stops */
    lg1 = merge(Stop, lg, ActionStop, lgAction, S1);
    SyntaxCheck(S1, lg1);   /* check that current symb. is in S1[] */
    if (InStop(symb.ibsymb, ActionStop, lgAction) >= 0) { /* non empty seq.*/
	/* parse a non empty seq. of action and return code */
	return(compndAction(deltaIn, Stop, lg, head, tail));
    } else {   /* empty seq. of actions */
	*head = *tail = NULL;
	return (NULL);
    }
}

/*
 :******************************************************************
 :* UNIT-BODY  action.compndAction
 :*
 :* TITLE         (/ parsing a non empty sequence of actions      /)
 :*
 :* PURPOSE       (/ parsing a sequence of symbols representing a non empty,
 :*                  semicolon separated sequence of actions; generate the
 :*                  corresponding internal code if no error is found or
 :*                  recover from this error otherwise            /)
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
 :*      <action_1>";"...<";"><action_i> ";" ...";"<action_n>";"<beta>
 :*      <-----------------------------> <------------------------------->
 :*               treated  (=alpha)                  to be treated
 :*
 :*      p                                         head = tail
 :*       |                                            |
 :*       |                                            |
 :*        _______________                 -------------
 :*       |ir(action_1) |-|--->  ... ---> |ir(action_i)|/|
 :*        ---------------                 -------------
 :*
 :*               where
 :*
 :*                  <action_i> is a valid action for i=1..N
 :*                  <beta> is any character sequence
 :*                  - ir(action_j) denotes the internal representation of
 :*                    action_j for i=1..j
 :*                  - the CSS is ";"...";"<action_n>";"<beta>
 :*                  - the list of boxes corresponding to alpha is pointed by
 :*                    'p'
 :*                  - the list of holes corresponding to this list of boxes
 :*                    is pointed by 'head'  (='tail')
 :*
 :*
 :*                                                               /)
 :*   DATA       (/
 :*                       - symb       : holds the internal representation
 :*                                      of the last read symbol.
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
 :*             - 'deltaIn' is the current address in WA area
 :*             - 'Stop' is an array stops (see REMARKS below)
 :*             - 'lg' is the length of 'Stop'
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/   if the CSS is of the following form:
 :*                   <action_1> ";" ... ";" <action_N> ";" <rest>
 :*               where
 :*                  - N >= 1 (the sequence may not be empty)
 :*                  - <action_i> is a sequence of symbols representing a
 :*                            valid action
 :*                  - <rest> is any character sequence
 :*               the effect is the following:
 :*                  - the CSS is <rest>
 :*                  - a pointer to the corresponding list of boxes is
 :*                    returned in 'p'
 :*                  - 'head' and 'tail' are respectively the head and tail
 :*                    of the list of holes in this list of boxes
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
 :* END-BODY action.compndAction
 :******************************************************************
*/
/* compndAction ::= action { ";" action } */

BoxPtr compndAction(deltaIn, Stop, lg, head, tail)
char *deltaIn;                      /* current address in WA */
int Stop[], lg;                     /* the stops */
holePtr *head, *tail;               /* list of holes */
{
    int S1[Mlg], lg1, S2[Mlg], lg2; /* temporary stops */
    holePtr head1, tail1;           /* holes for the current action */
    BoxPtr p, p1, action();         /* box pointers */

    /* build stops */
    insert_stop(semicolon, Stop, lg, S2, &lg2);
    lg1 = merge(S2, lg2, ActionStop, lgAction, S1);
    /* parse the first action and generate code */
    p = action(deltaIn, S2, lg2, head, tail);
    while (symb.ibsymb == semicolon) {  /* loop in the sequence of actions */
	Expect(semicolon, S1, lg1);     /* read keyword ";" */
	/* parse the next action and generate code */
	p1 = action(deltaIn, S2, lg2, &head1, &tail1);
	if (!ErrorFound) {
	    if (p != NULL && p1 != NULL) {
		/* at least one box for this action */
		fill_holesb(*head, p1);  /* fill existing holes with 'p1' */
		/* current hole list is that of the last parsed action */
		*head = head1;
		*tail = tail1;
	    } else if (p == NULL && p1 != NULL) {
		/* no boxes generated so far */
		p = p1;
		*head = head1;
		*tail = tail1;
	    }
	}
    }
    return(p);
}

