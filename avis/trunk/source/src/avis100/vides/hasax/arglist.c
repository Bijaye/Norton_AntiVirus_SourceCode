/*
 :******************************************************************
 :* MODULE-DESCRIPTION
 :*
 :* TITLE         (/ Parsing a rule triggering /)
 :*
 :* NAME             arglist.c
 :*
 :* COPYRIGHT     (C)   SIEMENS NIXDORF INFORMATIONSSYSTEME AG  1992
 :*                     ALL RIGHTS RESERVED
 :*
 :* MEMORY-CLASS  6
 :*
 :* PROG-CONVENTS (/ <internal conventions (instance, names,
 :*                  external data, ...). None -> empty text>     /)
 :*
 :* PROCEDURE     RuleActParList
 :* REMARKS       (/ parsing a rule triggering actual parameter list     /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     RuleActPar
 :* REMARKS       (/ parsing an actual parameter in a rule triggering.   /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     RuleActP
 :* REMARKS       (/ parsing an actual parameter in a rule triggering    /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     setLenBox
 :* REMARKS       (/ Building a set length box      /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     addLenBox
 :* REMARKS       (/ Building an add length box     /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     cpyBox
 :* REMARKS       (/ Building a copy Box /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     cpyStrBox
 :* REMARKS       (/ Build a box for string copy    /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     allocateBox
 :* REMARKS       (/ Building a box for memory allocation /)
 :* ENTRY         EXTERNAL
 :*
 :*-----------------------------------------------------------------
 :*
 :* VERSION       001
 :*   CRDATE      1992-03-09
 :*   AUTHOR      (/ Mounji Abdelaziz       , FUNDP /)
 :*   UPDATE      (/ original issue /)
 :*
 :* END-MODULE-DESCRIPTION arglist.c
 :******************************************************************
*/
#include <stdio.h>
#include "asax.h"

/*
 :******************************************************************
 :* UNIT-BODY arglist.RuleActParList
 :*
 :* TITLE         (/ parsing a rule triggering actual parameter list /)
 :*
 :* PURPOSE       (/ parsing a sequence of symbols representing a list of
 :*                  actual parameters of a rule triggering; generate the
 :*                  corresponding internal code if no error is found or
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
 :*                       - trdescr[]  : rule descriptors table
 :*                       - tdescr[]   : names (par; or var) descriptors table
 :*                       - targ_type[]: parameter types for undeclared rules
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
 :*   Pre    (/ - the CSS is s1 s2 ... sn     (see REMARKS below)
 :*             - the sequence of symbols already parsed t0, ..., tp is
 :*               described by 'trdescr[]', 'tdescr[]' and 'targ_types[]'
 :*               (see REMARKS below)
 :*             - tr_mode an integer identifying the triggering mode
 :*             - name a string representing the name of the triggered rule
 :*             - 'Stop' is an array of stops (see REMARKS below)
 :*             - 'lg' is the length of 'Stop'
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/   if the CSS is of the following form:
 :*                   <arg_1> "," ... "," <arg_N> ")" <rest>
 :*               where
 :*                  - N >= 0 (argument list may be empty)
 :*                  - <arg_i> is a sequence of symbols representing an
 :*                            argument (an expression) i=1 .. N
 :*                  - <rest> is any character sequence
 :*               the effect is the following:
 :*                  - the CSS is ")"<rest>
 :*                  - the sequence t0, ..., tp <r> is described by the three
 :*                    tables where <r> is the rule triggering corresponding
 :*                    to this actual parameter list
 :*                  - a pointer to the list of boxes corresponding to this
 :*                    rule triggering is returned in 'p'
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
 :* END-BODY arglist.RuleActParList
 :******************************************************************
*/
BoxPtr RuleActParList(tr_mode, name, Stop, lg, head, tail)
enum index tr_mode;
char *name;
int Stop[], lg;
holePtr *head, *tail;
{
	int status, rule_addr, isConst, rank, S1[30], lg1, S2[30], lg2;
	char *deltaIn;
	BoxPtr p, *h, p1, *h1, p2, addLen, cpy, cpy_ptr;
	BoxPtr setLenBox(), RuleActPar(), addLenBox(), cpyBox(), cpyStrBox();
	BoxPtr allocateBox();
	enum type type;
	int *h0;
	extern char *WA_ptr;
	rdescr *rdescrTab;

	add_rule_trig(name);
	rank = 0;
	deltaIn = WA_ptr;
	p = setLenBox(&h0);
	cpy = cpy_ptr = NULL;
	h = &p->alpha;
	if (symb.ibsymb != rparenthesis) {
		insert_stop(comma, Stop, lg, S1, &lg1);
		p->alpha = RuleActPar(name, &isConst, rank, &type, S1, lg1,
				      deltaIn, &h);
		if ((type == bytestring || type == fieldstring) && !isConst){
			 cpy = cpy_ptr = cpyStrBox(deltaIn);
			 addLen = addLenBox(deltaIn);
			 if (p->alpha != NULL) {
				*h = addLen;
				h = &addLen->alpha;
			 }
		} else {
			 cpy = cpy_ptr = cpyBox(deltaIn, type);
		}
		rank++;
		deltaIn += (type == integer)? sizeof(int *): sizeof(char **);
		while (symb.ibsymb == comma) {
		    insert_stop(comma, Stop, lg, S1, &lg1);
		    lg2 = merge(S1, lg1, ActParStop, lgActPar, S2);
		    Expect(comma, S2, lg2);
		    p1 = RuleActPar(name, &isConst, rank, &type, S1, lg1,
				    deltaIn, &h1);
		    if (p1 != NULL) {
			*h = p1;
			h = h1;
		    }
		    if ((type==bytestring || type==fieldstring) && !isConst){
			cpy_ptr->alpha = cpyStrBox(deltaIn);
			addLen = addLenBox(deltaIn);
			if (p1 != NULL) {
				*h = addLen;
				h = &addLen->alpha;
			}
		    } else {
			cpy_ptr->alpha = cpyBox(deltaIn, type);
		    }
		    cpy_ptr = cpy_ptr->alpha;
		    rank++;
		    deltaIn += (type == integer)? sizeof(int *): sizeof(char **);
		}  /* while */
		p1 = allocateBox(tr_mode, name);
		*h= p1;
		p1->alpha = cpy; /* connect the list of arg.code to
				    the cpy list */
		cpy_ptr->alpha = NULL;
		*head = *tail = (holePtr) &cpy_ptr->alpha;
		*h0 = deltaIn - WA_ptr;
	} else { /* no arguments */
		error1(semantics, 19, name);
		p1 = allocateBox(tr_mode, name);
		*h= p1;
		*h0 = deltaIn - WA_ptr;
		*head = *tail = (holePtr) &p1->alpha;
	}
	rule_addr = getRuleAddr(name, &rdescrTab);
	status = rdescrTab[rule_addr].status;
	if (status != unknown) {
	    if (rank != rdescrTab[rule_addr].ParNre) {
		error(semantics, 8);        /* check arity */
	    }
	} else {
	     rdescrTab[rule_addr].ParNre = rank;
	}
	return (p);
}
/*
 :******************************************************************
 :* UNIT-BODY  arglist.RuleActPar
 :*
 :* TITLE         (/ parsing an actual parameter in a rule triggering    /)
 :*
 :* PURPOSE       (/ parse a sequence of symbols representing an actual
 :*                  parameter (an expression); generate the corresponding
 :*                  internal code for its evaluation and moving to the WA
 :*                  area if no error is found or recover from this error
 :*                  otherwise /)
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
 :*                       - trdescr[]  : rule descriptors table
 :*                       - tdescr[]   : names (par; or var) descriptors table
 :*                       - targ_type[]: parameter types for undeclared rules
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
 :*             - 'name' is the name of the rule
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
 :*                  - 'isConst' = TRUE if this expression is neither a
 :*                                fieldname, a variable nor a parameter
 :*                              = FALSE otherwise
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
 :* END-BODY  arglist.RuleActPar
 :******************************************************************
*/
BoxPtr RuleActPar(name, isConst, rank, type, Stop, lg, deltaIn, h)
char *name, *deltaIn;
int *isConst, rank, Stop[], lg;
enum type *type;
BoxPtr **h;
{
	BoxPtr p, boxp, RuleActP();
	int isSimp;
	holePtr sp;

	p = RuleActP(&isSimp, isConst, name, rank, type, Stop, lg, deltaIn, h);
	if (isSimp && p != NULL) {
		if (  (boxp = (BoxPtr) malloc(sizeof(Box))) == NULL)
			error(fatal, 25);
		storeBox(boxp);
		if (*type == integer)
		    boxp->op = move_int1;
		else
		    boxp->op = move_str1;
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
	return (p);
}
/*
 :******************************************************************
 :* UNIT-BODY arglist.RuleActP
 :*
 :* TITLE         (/ parsing an actual parameter in a rule triggering    /)
 :*
 :* PURPOSE       (/ parse a sequence of symbols representing an actual
 :*                  parameter (an expression); generate the corresponding
 :*                  internal code for its evaluation to the WA area if no
 :*                  error is found or recover from this error otherwise /)
 :*
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
 :*                       - trdescr[]  : rule descriptors table
 :*                       - tdescr[]   : names (par; or var) descriptors table
 :*                       - targ_type[]: parameter types for undeclared rules
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
 :*             - 'name' is the rule name
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
 :*                  - 'isConst' = TRUE if this expression is neither a
 :*                                fieldname, a variable nor a parameter
 :*                              = FALSE otherwise
 :*                  - 'type' contains the type of the actual parameter
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
 :* END-BODY arglist.RuleActP
 :******************************************************************
*/
BoxPtr RuleActP(isSimp, isConst, name, rank, type, Stop, lg, deltaIn, h)
char *name, *deltaIn;
int *isSimp, *isConst, rank, Stop[], lg;
enum type *type;
BoxPtr **h;
{
	BoxPtr p, boxp, ActualP1();
	enum type type1;
	int rule_addr, status;
	rdescr *rdescrTab;

	p = ActualP1(isSimp, isConst, type, Stop, lg, deltaIn, h);
	rule_addr = getRuleAddr(name, &rdescrTab);
	status = rdescrTab[rule_addr].status;
	if (status == unknown) {
	    targ_types_lg++;
	    if (*type == fieldstring)
		targ_types[rdescrTab[rule_addr].targDescr + rank] = bytestring;
	    else
		targ_types[rdescrTab[rule_addr].targDescr + rank] = *type;
	} else if (status == known) {
	    if ((type1 = (enum type) getParType1(name, rank)) == -1) {
		error(semantics, 8);       /* check arity */
		type1 = undef;
	    } else if (type1 != -2) {
		 if (*type == fieldstring) checkTypes(&type1, bytestring);
		 else checkTypes(&type1, *type);
	    }
	} else /* if (status == declared)  */ {
	    if ((type1 = (enum type) getParType(name, rank)) == -1) {
		error(semantics, 8);       /* check arity */
		type1 = undef;
	    } else if (type1 != -2) {
		 if (*type == fieldstring) checkTypes(&type1, bytestring);
		 else checkTypes(&type1, *type);
	    }
	}
	return (p);
}
/*
 :******************************************************************
 :* UNIT-BODY arglist.setLenBox
 :*
 :* TITLE         (/Building a set length box                     /)
 :*
 :* PURPOSE       (/Allocate and initialize a memory area representing
 :*                 the abstract machine instruction needed to set the length
 :*                 of the memory area to allocated for a rule triggering
 :*                 descriptor. Output an an error message in case of
 :*                 allocation problems /)
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
 :*   Pre    (/
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/ - A memory area representing the abstract machine instruction
 :*               to alloacte a memory area for a rule triggering descriptor
 :*               is allocated and a pointer to it (say 'p') is returned.
 :*               In case of allocation problem, an error message is sent to
 :*               stdout.
 :*             - *h = &p->arg.size where 'p->arg.size' is the size (in bytes)
 :*               of the arguments in the rule triggering.               /)
 :*                                                             END-Post
 :*   RETURNS  "BoxPtr" (/ if > 0 this is the address of the internal code
 :*                        if = NULL an allocation problem occured       /)
 :*
 :*   REMARKS  (/  - refer to <DOCUMENTS> for a detailed description of
 :*                     the predefined call internal representation (box).
 :*                                                         /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY arglist.setLenBox
 :******************************************************************
*/
BoxPtr setLenBox(h)
int **h;
{
	
	BoxPtr p;

	if ((p = (BoxPtr) malloc(sizeof(Box))) == NULL)
		error(fatal, 25);
	storeBox(p);
	p->op = setLen1;
	p->arg.size = 0;        /* to be initialized later */
	p->alpha = NULL;
	*h = &p->arg.size;
	return (p);
}
/*
 :******************************************************************
 :* UNIT-BODY arglist.addLenBox
 :*
 :* TITLE         (/Building an add length box                     /)
 :*
 :* PURPOSE       (/Allocate and initialize a memory area representing
 :*                 the abstract machine instruction needed to increment
 :*                 the size of the memory area to be allocated for a rule
 :*                 triggering descriptor. Output an an error message in
 :*                 case of allocation problems /)
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
 :*   Pre    (/ - 'deltaIn' is the current address in the WA area
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/ - A memory area representing the abstract machine instruction
 :*               used to increment the size of the memory area for a rule
 :*               triggering descriptor is allocated and a pointer to it is
 :*               returned. In case of allocation problem, an error message
 :*               is sent to stdout.                                     /)
 :*                                                             END-Post
 :*   RETURNS  "BoxPtr" (/ if > 0 this is the address of the internal code
 :*                        if = NULL an allocation problem occured       /)
 :*
 :*   REMARKS  (/  - refer to <DOCUMENTS> for a detailed description of
 :*                     the predefined call internal representation (box).
 :*                                                         /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY arglist.addLenBox
 :******************************************************************
*/
BoxPtr addLenBox(deltaIn)
char *deltaIn;
{
	
	BoxPtr p;

	if ((p = (BoxPtr) malloc(sizeof(Box))) == NULL)
		error(fatal, 25);
	storeBox(p);
	p->op = addLen1;
	p->arg.gamma = (hole *) deltaIn;
	p->alpha = NULL;
	return (p);
}
/*
 :******************************************************************
 :* UNIT-BODY arglist.cpyBox
 :*
 :* TITLE         (/Building a copy box                     /)
 :*
 :* PURPOSE       (/Allocate and initialize a memory area representing
 :*                 the abstract machine instruction needed to copy a data
 :*                 from the WA area into the rule triggering descriptor.
 :*                 Output an error message in case of allocation problems
 :*                                                             /)
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
 :*   Pre    (/ - 'deltaIn' is the current address in the WA area
 :*             - type is the type of the data to be copied ("int", "string")
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/ - A memory area representing the abstract machine instruction
 :*               used to copy a data from the address 'deltaIn' into the
 :*               rule triggering descriptor is allocated and a pointer to it
 :*               is returned. In case of allocation problem, an error
 :*               message is sent to stdout.                                     /)
 :*                                                             END-Post
 :*   RETURNS  "BoxPtr" (/ if > 0 this is the address of the internal code
 :*                        if = NULL an allocation problem occured       /)
 :*
 :*   REMARKS  (/  - refer to <DOCUMENTS> for a detailed description of
 :*                     the predefined call internal representation (box).
 :*                                                         /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY arglist.cpyBox
 :******************************************************************
*/
BoxPtr cpyBox(deltaIn, type)
char *deltaIn;
enum type type;
{
	
	BoxPtr p;

	if ((p = (BoxPtr) malloc(sizeof(Box))) == NULL)
		error(fatal, 25);
	storeBox(p);
	if (type != undef) {
		if (type == integer)
			p->op = cpy_intAddr1;
		else
			p->op = cpy_strAddr1;
	}
	p->arg.gamma = (hole *) deltaIn;
	p->alpha = NULL;
	return (p);
}
/*
 :******************************************************************
 :* UNIT-BODY arglist.cpyStrBox
 :*
 :* TITLE         (/Building a box for string copy                     /)
 :*
 :* PURPOSE       (/Allocate and initialize a memory area representing
 :*                 the abstract machine instruction needed to copy a string
 :*                 from the WA area into the rule triggering descriptor.
 :*                 Output an an error message in case of allocation problems
 :*                                                             /)
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
 :*   Pre    (/ - 'deltaIn' is the current address in the WA area
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/ - A memory area representing the abstract machine instruction
 :*               used to copy a string from the address 'deltaIn' into the
 :*               rule triggering descriptor is allocated and a pointer to it
 :*               is returned. In case of allocation problem, an error
 :*               message is sent to stdout.                                     /)
 :*                                                             END-Post
 :*   RETURNS  "BoxPtr" (/ if > 0 this is the address of the internal code
 :*                        if = NULL an allocation problem occured       /)
 :*
 :*   REMARKS  (/  - refer to <DOCUMENTS> for a detailed description of
 :*                     the predefined call internal representation (box).
 :*                                                         /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY arglist.cpyStrBox
 :******************************************************************
*/
BoxPtr cpyStrBox(deltaIn)
char *deltaIn;
{
	
	BoxPtr p;

	if ((p = (BoxPtr) malloc(sizeof(Box))) == NULL)
		error(fatal, 25);
	storeBox(p);
	p->op = cpy_str1;
	p->arg.gamma = (hole *) deltaIn;
	p->alpha = NULL;
	return (p);
}
/*
 :******************************************************************
 :* UNIT-BODY arglist.allocateBox
 :*
 :* TITLE         (/Building a box for memory allocation               /)
 :*
 :* PURPOSE       (/Allocate and initialize a memory area representing
 :*                 the abstract machine instruction needed to allocate
 :*                 a memory area for a rule triggering descriptor.
 :*                 Output an an error message in case of allocation problems
 :*                                                             /)
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
 :*   Pre    (/ - 'tr_mode' is an integer that identifies which list of active
 :*               rules the rule triggering descriptor is to be pushed in;
 :*               'tr_mode' is in {for_currentx, for_nextx, at_completionx}
 :*             - 'name' is the name of the triggered rule              /)
 :*                                                             END-Pre
 :*
 :*   Post   (/ - A memory area representing the abstract machine instruction
 :*               used to allocate a memory area for a rule triggering
 :*               descriptor and a pointer to it is returned.
 :*               In case of allocation problem, an error message is sent to
 :*               stdout.
 :*             - 'p->op' = for_current1   if the rule triggering descriptor
 :*                                        is to be pushed in the list of
 :*                                        rules active for the current record
 :*                       = for_next1      if the rule triggering descriptor
 :*                                        is to be pushed in the list of
 :*                                        rules active for the next record
 :*                       = at_completion1 if the rule triggering descriptor
 :*                                        is to be pushed in the list of
 :*                                        rules active at completion
 :*             - 'p->arg.triggerbox.ruleCode' is either a pointer to the
 :*               internal code of the triggered rule or inserted in a list
 :*               holes related to this internal code                   /)
 :*                                                             END-Post
 :*   RETURNS  "BoxPtr" (/ if > 0 this is the address of the internal code
 :*                        if = NULL an allocation problem occured       /)
 :*
 :*   REMARKS  (/  - refer to <DOCUMENTS> for a detailed description of
 :*                     a rule triggering internal representation (box).
 :*                                                         /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY arglist.allocateBox
 :******************************************************************
*/
BoxPtr allocateBox(tr_mode, name)
enum index tr_mode;
char *name;
{
	
	BoxPtr p, getRuleCode();
	rdescr *rdescrTab;
	int k;

	if ((p = (BoxPtr) malloc(sizeof(Box))) == NULL)
		error(fatal, 25);
	storeBox(p);
	switch(tr_mode) {
	    case for_currentx  : p->op = for_current1;
				 break;
	    case for_nextx     : p->op = for_next1;
				 break;
	    case at_completionx: p->op = at_completion1;
				 break;
	}
	k = getRuleAddr(name, &rdescrTab);
	if (rdescrTab[k].status == declared) {
	    p->arg.triggerbox.ruleCode = getRuleCode(name);
	} else {
	    add_hole((hole *) &p->arg.triggerbox.ruleCode, name);
	}
	p->alpha = NULL;
	return (p);
}
