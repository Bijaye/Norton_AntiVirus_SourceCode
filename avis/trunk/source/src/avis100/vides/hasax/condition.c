/*
 :******************************************************************
 :* MODULE-DESCRIPTION
 :*
 :* TITLE         (/ Parsing of a condition   /)
 :*
 :* NAME             condition.c
 :*
 :* COPYRIGHT     (C)   SIEMENS NIXDORF INFORMATIONSSYSTEME AG  1992
 :*                     ALL RIGHTS RESERVED
 :*
 :* MEMORY-CLASS  6
 :*
 :* PROG-CONVENTS (/ <internal conventions (instance, names,
 :*                  external data, ...). None -> empty text>     /)
 :*
 :* PROCEDURE     relation
 :* REMARKS       (/ parsing of a relational expression /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     present
 :* REMARKS       (/ parsing of a "present" condition    /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     simpCond
 :* REMARKS       (/ parsing of a simple condition              /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     sxconj
 :* REMARKS       (/ parsing of a conjunction             /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     sxcond
 :* REMARKS       (/ parsing of a disjunction             /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     connectBoxes
 :* REMARKS       (/ connecting generated boxes             /)
 :* ENTRY         EXTERNAL
 :*
 :*-----------------------------------------------------------------
 :*
 :* VERSION       001
 :*   CRDATE      1992-03-09
 :*   AUTHOR      (/ Mounji Abdelaziz       , FUNDP /)
 :*   UPDATE      (/ original issue /)
 :*
 :* END-MODULE-DESCRIPTION condition.c
 :******************************************************************
*/

/*
$Log: condition.c,v $
 * Revision 1.0  1995/04/05  11:48:16  Aziz Mounji
 * cast second arg. of `getFieldNameAddr' to (hole *) (line 408)
 *
*/

#include <stdio.h>
#include "asax.h"                           /* almost all data structures */

#define cpy_intToLess1 20  /* In order to keep the code simpler, relational
			      operators for strings are obtained from
			      the relational operators for integers by adding
			      the constant 'cpy_intToless1' which is :
				 cpy_int1 - less1 + 1
			   */

/*
 :******************************************************************
 :* UNIT-BODY condition.relation
 :*
 :* TITLE         (/ parsing of a relational expression       /)
 :*
 :* PURPOSE       (/ parse a sequence of symbols representing a relational
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
 :*             - 'Stop' is an array of stops  (see REMARKS below)
 :*             - 'lg' is the length of 'Stop'
 :*             - isNeg = TRUE if the relational expression represented by
 :*                            a prefix of the CSS s1 s2 ... sn is to be
 :*                            negated
 :*                     = FALSE otherwise
 :*             - 'deltaIn' is the current address in the Working Area
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/   if the CSS is of the form:
 :*                      <term1><relOp><term2><rest>
 :*               where
 :*                  <term1> and <term2> are valid expressions of the same
 :*                                      type (integer or string)
 :*                  <relOp> is a relational operator
 :*                  <rest> is any character sequence
 :*               the effect is the following:
 :*                  - the pointer to the corresponding list of boxes is
 :*                    returned in 'p'
 :*                 if isNeg = FALSE (resp. TRUE)
 :*                  - two lists of holes correspond to this internal code
 :*                    one related to the branch address 'true' and the other
 :*                    to the branch address 'false'
 :*                  - *true_head and *true_tail are respectively the pointer
 :*                    to the first and last hole in the list of holes
 :*                    associated with the branch address 'true' (resp.
 :*                    'false')
 :*                  - *false_head and *false_tail are respectively the
 :*                     pointer to the first and last hole in the list of
 :*                     holes associated with the branch address 'false'
 :*                     (resp. 'true')
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
 :* END-BODY term.relation
 :******************************************************************
*/
BoxPtr relation(isNeg, deltaIn, Stop, lg,
		false_head, false_tail, true_head, true_tail)
unsigned isNeg;                                    /* must this be negated */
char *deltaIn;                                    /* current address in WA */
holePtr *false_head, *false_tail, *true_head, *true_tail; /* list of holes */
int Stop[], lg;                                           /* the stops */
{
	int S1[Mlg], lg1, S2[Mlg], lg2;      /* temporary stops */
	int isSimp1, isSimp2;                /* at least one box generated */
	int s, isConst1, isConst2, **intAddr1, **intAddr2;
	enum type type1, type2;                     /* type of each member */
	BoxPtr p, p1, p2, p3, *h1, *h2, sxterm();
	char *deltaIn1, **addr1, **addr2;
	holePtr t_addr, f_addr;                   /* temporary addresses */

      /* build stops */
	lg2 = merge(Stop, lg, TermStop, lgTerm, S2);
	lg1 = merge(S2, lg2, relStop, lgRel, S1);
      /* parse the first member of the relation and generate code */
	p1 = sxterm(&isSimp1, &isConst1, &type1, S1, lg1, deltaIn, &h1);
	s = symb.ibsymb;
	Expect(s, S2, lg2);      /* read the relational symbol */
	deltaIn1 = deltaIn + sizeof(int);
      /* parse the second member of the relation and generate code */
	p2 = sxterm(&isSimp2, &isConst2, &type2, Stop, lg, deltaIn1, &h2);
      /* generate code for the relational operator */
	if ((p3 = (BoxPtr) malloc(sizeof(Box))) == NULL)
		error(fatal, 25);   /* allocation error */
	else {
		storeBox(p3);         /* for tests only */
		if ((type1 == bytestring || type1 == fieldstring) &&
		    (type2 == bytestring || type2 == fieldstring)) {
		 /* relational operator for string expressions */
			if ((enum index) s == neq)
			    p3->op = StrNeq1;
			else if ((enum index) s == eqx)
			    p3->op = StrEq1;
			else
			    p3->op = (enum operation) (s + cpy_intToLess1);
		    addr1 = (char **) &p3->arg.StrRelatbox.gamma1;
		    addr2 = (char **) &p3->arg.StrRelatbox.gamma2;
		    if (type1 == fieldstring)  /* this is a field name */
			getFieldNameAddr((char *) p1, (holePtr) addr1);
		    else
			*addr1 = (isSimp1)? (char *) p1: deltaIn;
		    if (type2 == fieldstring)
			getFieldNameAddr((char *) p2, (holePtr) addr2);
		    else
			*addr2 = (isSimp2)? (char *) p2: deltaIn1;
		    p3->arg.StrRelatbox.alphaTrue = NULL;
		    p3->alpha = NULL;
		} else if (type1 == integer && type2 == integer) {
		  /* relational operator for integer expressions */
		    intAddr1 = &p3->arg.relationalbox.gamma1;
		    intAddr2 = &p3->arg.relationalbox.gamma2;
		    p3->op = (enum operation) s;
		    *intAddr1 = (isSimp1)? (int *) p1: (int *) deltaIn;
		    *intAddr2 = (isSimp2)? (int *) p2: (int *) deltaIn1;
		    p3->arg.relationalbox.alphaTrue = NULL;
		    p3->alpha = NULL;
	       } else
		    error(semantics, 10);   /* type mismatch */
	}
	t_addr =  (holePtr) &p3->arg.relationalbox.alphaTrue;
	f_addr =  (holePtr) &p3->alpha;
	*true_head = *true_tail = (isNeg)? f_addr: t_addr;
	*false_head = *false_tail = (isNeg)? t_addr: f_addr;
     /* connect the three components lists together */
	if (!ErrorFound) {
	if (isSimp1 && isSimp2) {   /* no boxes for both two members */
		p = p3;
	} else if (isSimp1 && !isSimp2) {  /* at least one box for the */
		*h2 = p3;                  /* second member             */
		p = p2;
	} else if (!isSimp1 && isSimp2) {
		*h1 = p3;
		p = p1;
	} else if (!isSimp1 && !isSimp2) { /* at least one box for each */
		*h1 = p2;                  /* member                    */
		*h2 = p3;
		p = p1;
	}
	}
	return(p);
}
/*
 :******************************************************************
 :* UNIT-BODY condition.present
 :*
 :* TITLE         (/ parsing of a "present" condition       /)
 :*
 :* PURPOSE       (/ parse a sequence of symbols representing the condition
 :*                  <present><field_name>; generate the corresponding
 :*                  internal code for it if no error is found or recover
 :*                  from this error otherwise                                          /)
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
 :*                 - CR_table     : the indirection table
 :*                 - mapping_table: maps external audit data names to
 :*                                  their identifiers
 :*                 - symb         : holds the internal representation
 :*                                  of the last read symbol.
 :*                 - nextchar     : the last character being read.
 :*                 - ErrorFound   : = TRUE if an error was found
 :*                                  = FALSE otherwise
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
 :*             - 'Stop' is an array of stops  (see REMARKS below)
 :*             - 'lg' is the length of 'Stop'
 :*             - isNeg = TRUE if the conditional expression represented by
 :*                            a prefix of the CSS s1 s2 ... sn is to be
 :*                            negated
 :*                     = FALSE otherwise
 :*             - 'deltaIn' is the current address in the Working Area
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/   if the CSS is of the form:
 :*                     present <field_name><rest>
 :*               where
 :*                  <field_name> is a valid audit data name
 :*                  <rest> is any character sequence
 :*               the effect is the following:
 :*                  - the pointer to the box corresponding to this
 :*                    expression (see <DOCUMENTS>) is returned in 'p'
 :*                 if isNeg = FALSE (resp. TRUE)
 :*                  - two lists of holes correspond to this internal code
 :*                    one related to the branch address 'true' and the other
 :*                    to the branch address 'false'
 :*                  - *true_head and *true_tail are respectively the pointer
 :*                    to the first and last hole in the list of holes
 :*                    associated with the branch address 'true' (resp.
 :*                    'false')
 :*                  - *false_head and *false_tail are respectively the
 :*                     pointer to the first and last hole in the list of
 :*                     holes associated with the branch address 'false'
 :*                     (resp. 'true')
 :*                  - the indirection table contains an entry corresponding
 :*                    to this field name
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
 :* END-BODY term.present
 :******************************************************************
*/
BoxPtr present(isNeg, Stop, lg, false_head, false_tail, true_head, true_tail)
unsigned isNeg;                                    /* must this be negated */
holePtr *false_head, *false_tail, *true_head, *true_tail; /* list of holes */
int Stop[], lg;                                           /* the stops */
{
	int S1[Mlg], lg1, d;
	BoxPtr p;
	char *name;
	holePtr t_addr, f_addr;

      /* build stops */
	insert_stop(identifierx, Stop, lg, S1, &lg1); /* build stops */
	Expect(presentx, S1, lg1);           /* read the keyword "present" */
	name = tmalloc(ident_lg, char);
	ExpectName(name, Stop, lg);             /* read field name */
	if ((d = getAuditDataId(name)) < 0) {
		error1(semantics, 12, name);    /* not a field name */
		p = NULL;
	} else {
		if (( p = (BoxPtr) malloc(sizeof(Box))) == NULL)
			error(fatal, 25);   	/* allocation error*/
		else {
		    /* build internal code */
			storeBox(p);              /* for tests only */
			p->op = present1;
		    /* install a new field name in the indirection table */
		    /* or insert a new hole for this field name */
			getFieldNameAddr(name, (hole *) &p->arg.presentbox.str_addr);
			p->arg.presentbox.alphaTrue = NULL;
			p->alpha = NULL;
			t_addr = (holePtr) &p->arg.presentbox.alphaTrue;
			f_addr = (holePtr) &p->alpha;
		    /* build the two lists of holes */
			*true_head = *true_tail = (isNeg)? f_addr: t_addr;
			*false_head = *false_tail = (isNeg)? t_addr: f_addr;
	       }
	}
	return (p);
}
/*
 :******************************************************************
 :* UNIT-BODY condition.simpCond
 :*
 :* TITLE         (/ parsing of a simple condition       /)
 :*
 :* PURPOSE       (/ parse a sequence of symbols representing a simple
 :*                  condition; generate the corresponding internal code for
 :*                  it if no error is found or recover from this error
 :*                  otherwise                                        /)
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
 :*                 - CR_table     : the indirection table
 :*                 - mapping_table: maps external audit data names to
 :*                                  their identifiers
 :*                 - symb         : holds the internal representation
 :*                                  of the last read symbol.
 :*                 - nextchar     : the last character being read.
 :*                 - ErrorFound   : = TRUE if an error was found
 :*                                    = FALSE otherwise
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
 :*             - 'Stop' is an array of stops  (see REMARKS below)
 :*             - 'lg' is the length of 'Stop'
 :*             - isNeg = TRUE if the conditional expression represented by
 :*                            a prefix of the CSS s1 s2 ... sn is to be
 :*                            negated
 :*                     = FALSE otherwise
 :*             - 'deltaIn' is the current address in the Working Area
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/   if the CSS is of the form:
 :*                     <simple_condition><rest>
 :*               where
 :*                  <simple_condition> is a valid simple condition
 :*                  <rest> is any character sequence
 :*               the effect is the following:
 :*                  - the pointer to the box corresponding to this
 :*                    expression (see <DOCUMENTS>) is returned in 'p'
 :*                 if isNeg = FALSE (resp. TRUE)
 :*                  - two lists of holes correspond to this internal code
 :*                    one related to the branch address 'true' and the other
 :*                    to the branch address 'false'
 :*                  - *true_head and *true_tail are respectively the pointer
 :*                    to the first and last hole in the list of holes
 :*                    associated with the branch address 'true' (resp.
 :*                    'false')
 :*                  - *false_head and *false_tail are respectively the
 :*                     pointer to the first and last hole in the list of
 :*                     holes associated with the branch address 'false'
 :*                     (resp. 'true')
 :*                  - if the expression is
 :*                           <true>
 :*                        or <not> <false>
 :*                       'exit_value' = true_exit
 :*                    else if the expression is
 :*                           <false>
 :*                        or <not> <true>
 :*                       'exit_value' = false_exit
 :*                    in all other cases
 :*                       'exit_value' = any_exit
 :*                  - the indirection table contains an entry corresponding
 :*                    to this field name
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
 :* END-BODY term.present
 :******************************************************************
*/
BoxPtr simpCond(isNeg, exit_value, deltaIn, Stop, lg,
		false_head, false_tail, true_head, true_tail)
unsigned isNeg;
enum truthValue *exit_value; /* to detect truth value at compile time */
char *deltaIn;
int Stop[], lg;                                           /* the stops */
holePtr *false_head, *false_tail, *true_head, *true_tail; /* list of holes */
{
    int S1[Mlg], lg1, S2[Mlg], lg2;                  /* temporary stops */
    enum index s;
    BoxPtr p, relation(), present(), sxcond();

    s = symb.ibsymb;
    switch (s) {
	case falsex:       p = NULL;                           /* no boxes */
			   *exit_value = false_exit;     /* false detected */
			   *false_head = *false_tail = NULL;   /* no holes */
			   *true_head = *true_tail = NULL;
			   Expect(falsex, Stop, lg);
			   break;

	case truex:        p = NULL;
			   *exit_value = true_exit;       /* true detected */
			   *false_head = *false_tail = NULL;
			   *true_head = *true_tail = NULL;
			   Expect(truex, Stop, lg);
			   break;

	case presentx:     p = present(isNeg, Stop, lg, false_head,
			       false_tail, true_head, true_tail);
			 /* truth value connot be detected at compile time */
			   *exit_value = any_exit;
			   break;

	case lparenthesis:
			 /* build stops */
			   insert_stop(rparenthesis, Stop, lg, S2, &lg2);
			   lg1 = merge(S2, lg2, CondStop, lgCond, S1);
			 /* read left parenthesis */
			   Expect(lparenthesis, S1, lg1);
			 /* parse and generate code */
			   p = sxcond(isNeg, exit_value, deltaIn, S2, lg2,
			       false_head, false_tail, true_head, true_tail);
			 /* read right  parenthesis */
			   Expect(rparenthesis, Stop, lg);
			   break;

	case notx:
			 /* build stops */
			   lg1 = merge(Stop, lg, CondStop, lgCond, S1);
			 /* read the word "not" */
			   Expect(notx, S1, lg1);
		       /* parse the simple condition which must be negated */
			   p = simpCond(!isNeg, exit_value, deltaIn, Stop, lg
			      ,false_head, false_tail, true_head, true_tail);
		       /* take the reverse of the truth value */
			   if (*exit_value == true_exit)
				*exit_value = false_exit;
			   else if (*exit_value == false_exit)
				*exit_value = true_exit;
			   break;

	default:
			/* this must be a relational expression */
			   p = relation(isNeg, deltaIn, Stop, lg,
			       false_head, false_tail, true_head, true_tail);
			 /* truth value connot be detected at compile time */
			   *exit_value = any_exit;
			   break;

    }
    return (p);
}
/*
 :******************************************************************
 :* UNIT-BODY condition.sxconj
 :*
 :* TITLE         (/ parsing of a conjunction       /)
 :*
 :* PURPOSE       (/ parse a sequence of symbols representing a conjunction;
 :*                  generate the corresponding internal code for it if no
 :*                  error is found or recover from this error otherwise  /)
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
 :*       INVARIANT:
 :*
 :*    <sCond_1><and>...<and><sCond_i><and>...<and><sCond_n><c><beta>
 :*    <-----------------------------><----------------------------->
 :*           treated  (= alpha)            to be treated
 :*
 :*               where
 :*
 :*                  <sCond_j> is a valid simple condition j=1..i
 :*                  <c>   not in {'+', '-'}
 :*                  <beta> is any character sequence
 :*                  - ir(sCond_j) denotes the internal representation of
 :*                    <sCond_j> for i=1..j
 :*                  - the CSS is <and>...<and><sCond_n><c><beta>
 :*                  - a list of boxes corresponding to alpha is pointed by
 :*                    'p'
 :*                  - two lists of holes correspond to this internal code
 :*                    one related to the branch address 'true' and the other
 :*                    to the branch address 'false'
 :*                 if isNeg = FALSE (resp. TRUE)
 :*                  - *true_head and *true_tail are respectively the pointer
 :*                    to the first and last hole in the list of holes
 :*                    associated with the branch address 'true' (resp.
 :*                    'false')
 :*                  - *false_head and *false_tail are respectively the
 :*                     pointer to the first and last hole in the list of
 :*                     holes associated with the branch address 'false'
 :*                     (resp. 'true')
 :*                  - if there exist j0:
 :*                        1 <= j0 <= i   such that <sCond_j0> = false
 :*                    then 'conj_ext_value' = false_exit
 :*                  - if <sCond_j> = true for j=1 .. j
 :*                    then 'conj_exit_value' = true_exit
 :*                  - otherwise
 :*                       'conj_exit_value' = any_exit
 :*                  - the indirection table contains an entry corresponding
 :*                    to each reference to a field name in alpha
 :*
 :*                                                               /)
 :*   DATA       (/
 :*                 - CR_table     : the indirection table
 :*                 - mapping_table: maps external audit data names to
 :*                                  their identifiers
 :*                 - symb         : holds the internal representation
 :*                                  of the last read symbol.
 :*                 - nextchar     : the last character being read.
 :*                 - ErrorFound   : = TRUE if an error was found
 :*                                  = FALSE otherwise
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
 :*             - 'Stop' is an array of stops  (see REMARKS below)
 :*             - 'lg' is the length of 'Stop'
 :*             - isNeg = TRUE if the conjunction represented by a prefix
 :*                            (the greatest) of the CSS s1 s2 .. sn is to be
 :*                            negated
 :*                     = FALSE otherwise
 :*             - 'deltaIn' is the current address in the Working Area
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/   if the CSS is of the form:
 :*                  <conj_1><and>...<and><conj_N><c><beta>
 :*               where
 :*                  <conj_i> is a valid conjunction for j=1 ... N
 :*                  <c> is not <and>
 :*                  <beta> is any character sequence
 :*               the effect is the following:
 :*                  - the pointer to the box corresponding to this
 :*                    expression (see <DOCUMENTS>) is returned in 'p'
 :*                 if isNeg = FALSE (resp. TRUE)
 :*                  - two lists of holes correspond to this internal code
 :*                    one related to the branch address 'true' and the other
 :*                    to the branch address 'false'
 :*                  - *true_head and *true_tail are respectively the pointer
 :*                    to the first and last hole in the list of holes
 :*                    associated with the branch address 'true' (resp.
 :*                    'false')
 :*                  - *false_head and *false_tail are respectively the
 :*                     pointer to the first and last hole in the list of
 :*                     holes associated with the branch address 'false'
 :*                     (resp. 'true')
 :*                  - the indirection table contains an entry corresponding
 :*                    to this field name
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
 :* END-BODY term.sxconj
 :******************************************************************
*/
BoxPtr sxconj(isNeg, conj_exit_value, deltaIn, Stop, lg,
	      false_head, false_tail, true_head, true_tail)
unsigned isNeg;
enum truthValue *conj_exit_value; /* to detect truth value at compile time */
char *deltaIn;
int Stop[], lg;                                           /* the stops */
holePtr *false_head, *false_tail, *true_head, *true_tail; /* list of holes */
{
    int S1[Mlg], lg1, S2[Mlg], lg2;                    /* temporary stops */
    BoxPtr p, p1, simpCond();
    enum truthValue exit_value;  /* truth value for the simple condition */
    holePtr fh, ft, th, tt;      /* list of holes for the simple condition */

    /* build stops */
    insert_stop(andx, Stop, lg, S2, &lg2);
    lg1 = merge(S2, lg2, CondStop, lgCond, S1);
    /* parse the first simple condition and generate code */
    p = simpCond(isNeg, &exit_value, deltaIn, S2, lg2,
		 false_head, false_tail, true_head, true_tail);
    *conj_exit_value = exit_value;
    while (symb.ibsymb == andx) {  /* while we are in a conjunction */
	Expect(andx, S1, lg1);
       /* parse the next simple condition and generate code */
	p1 = simpCond(isNeg, &exit_value, deltaIn, S2, lg2,
		      &fh, &ft, &th, &tt);
       /* try to detect truth value at compile time */
	if ((*conj_exit_value == true_exit) ||
	    ((*conj_exit_value == any_exit) && (exit_value == false_exit))) {
		*conj_exit_value = exit_value;
	}
  /* connect the so far generated list of boxes with */
  /* that of this simple condition */
	connectBoxes(!isNeg, &p, p1, false_head, false_tail, true_head,
		     true_tail, &fh, ft, &th, tt);
    }
    if (*conj_exit_value == any_exit) {
	return (p);
    } else  {
	return (NULL);/* truth value detected at compile time */
    }
}
/*
 :******************************************************************
 :* UNIT-BODY condition.sxcond
 :*
 :* TITLE         (/ parsing of a disjunction       /)
 :*
 :* PURPOSE       (/ parse a sequence of symbols representing a disjunction;
 :*                  generate the corresponding internal code for it if no
 :*                  error is found or recover from this error otherwise  /)
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
 :*       INVARIANT:
 :*
 :*    <conj_1><or>...<or><conj_i><or>...<or><conj_n><c><beta>
 :*    <-------------------------><-------------------------->
 :*         treated  (= alpha)         to be treated
 :*
 :*               where
 :*
 :*                  <conj_j> is a valid conjunction j=1..i
 :*                  <c>   not "or"
 :*                  <beta> is any character sequence
 :*                  - ir(conj_j) denotes the internal representation of
 :*                    fact_j for i=1..j
 :*                  - the CSS is <or>...<or><conj_n><c><beta>
 :*                  - a list of boxes corresponding to alpha is pointed by
 :*                    'p'
 :*                  - two lists of holes correspond to this internal code
 :*                    one related to the branch address 'true' and the other
 :*                    to the branch address 'false'
 :*                  - if there exist j0:
 :*                        1 <= j0 <= i   such that <conj_j0> = true
 :*                    then 'conj_ext_value' = true_exit
 :*                  - if <conj_j> = false for j=1 .. j
 :*                    then 'conj_exit_value' = false_exit
 :*                  - otherwise
 :*                       'conj_exit_value' = any_exit
 :*                 if isNeg = FALSE (resp. TRUE)
 :*                  - *true_head and *true_tail are respectively the pointer
 :*                    to the first and last hole in the list of holes
 :*                    associated with the branch address 'true' (resp.
 :*                    'false')
 :*                  - *false_head and *false_tail are respectively the
 :*                     pointer to the first and last hole in the list of
 :*                     holes associated with the branch address 'false'
 :*                     (resp. 'true')
 :*                  - the indirection table contains an entry corresponding
 :*                    to each reference to a field name in alpha
 :*
 :*                                                               /)
 :*   DATA       (/
 :*                 - CR_table     : the indirection table
 :*                 - mapping_table: maps external audit data names to
 :*                                  their identifiers
 :*                 - symb         : holds the internal representation
 :*                                  of the last read symbol.
 :*                 - nextchar     : the last character being read.
 :*                 - ErrorFound   : = TRUE if an error was found
 :*                                  = FALSE otherwise
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
 :*             - 'Stop' is an array of stops  (see REMARKS below)
 :*             - 'lg' is the length of 'Stop'
 :*             - isNeg = TRUE if the disjunction represented by a prefix
 :*                            (the greatest) of the CSS s1 s2 .. sn is to be
 :*                            negated
 :*                     = FALSE otherwise
 :*             - 'deltaIn' is the current address in the Working Area
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/   if the CSS is of the form:
 :*                  <conj_1><and>...<and><conj_N><c><beta>
 :*               where
 :*                  <conj_i> is a valid disjunction for j=1 ... N
 :*                  <c> is not <and>
 :*                  <beta> is any character sequence
 :*               the effect is the following:
 :*                  - the pointer to the box corresponding to this
 :*                    expression (see <DOCUMENTS>) is returned in 'p'
 :*                 if isNeg = FALSE (resp. TRUE)
 :*                  - two lists of holes correspond to this internal code
 :*                    one related to the branch address 'true' and the other
 :*                    to the branch address 'false'
 :*                  - *true_head and *true_tail are respectively the pointer
 :*                    to the first and last hole in the list of holes
 :*                    associated with the branch address 'true' (resp.
 :*                    'false')
 :*                  - *false_head and *false_tail are respectively the
 :*                     pointer to the first and last hole in the list of
 :*                     holes associated with the branch address 'false'
 :*                     (resp. 'true')
 :*                  - the indirection table contains an entry corresponding
 :*                    to this field name
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
 :* END-BODY term.sxconj
 :******************************************************************
*/
BoxPtr sxcond(isNeg, cond_exit_value, deltaIn, Stop, lg,
	      false_head, false_tail, true_head, true_tail)
unsigned isNeg;
enum truthValue *cond_exit_value; /* truth value detection at compile time */
char *deltaIn;
int Stop[], lg;                                               /* the stops */
holePtr *false_head, *false_tail, *true_head, *true_tail; /* list of holes */
{
    int S1[Mlg], lg1, S2[Mlg], lg2;                     /* temporary stops */
    BoxPtr p, p1, sxconj();
    enum truthValue exit_value;         /* truth value for the conjunction */
    holePtr fh, ft, th, tt;           /* list of holes for the conjunction */

  /* build stops */
    insert_stop(orx, Stop, lg, S2, &lg2);
    lg1 = merge(S2, lg2, CondStop, lgCond, S1);
  /* parse first conjunction */
    p = sxconj(isNeg, &exit_value, deltaIn, S2, lg2, false_head, false_tail,
	       true_head, true_tail);
    *cond_exit_value = exit_value;
    while (symb.ibsymb == orx) {
	Expect(orx, S1, lg1);
  /* parse next conjunction */
	p1 = sxconj(isNeg, &exit_value, deltaIn, S2, lg2,
		    &fh, &ft, &th, &tt);
	if ((*cond_exit_value == false_exit) ||
	    ((*cond_exit_value == any_exit) && (exit_value != false_exit))) {
		*cond_exit_value = exit_value;
	}
  /* connect the so far generated list of boxes with */
  /* that of this conjunction */
	connectBoxes(isNeg, &p, p1, false_head, false_tail,
		     true_head, true_tail, &fh, ft, &th, tt);
    }
    if (*cond_exit_value == any_exit) {
	return (p);
    } else  {
	return (NULL);/* truth value detected at compile time */
    }
}
/*
 :******************************************************************
 :* UNIT-BODY condition.connectBoxes
 :*
 :* TITLE         (/ connecting generated boxes      /)
 :*
 :* PURPOSE       (/ connecting two given lists of boxes (corresponding to
 :*                  two sub expressions of the same conditional expression)
 :*                  along with their respective lists of holes to obtain a
 :*                  single list of boxes with a single list of holes
 :*                                                           /)
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
 :*   Pre    (/
 :*             - 'p' is a pointer to a list of boxes corresponding to the
 :*               conditional expression 'cond1'; the list of holes related
 :*               to the branch address 'true' (resp. false) is identified by
 :*               its head 'th' (resp. 'fh') and tail 'tt' (resp. 'ft').
 :*             - 'p1' is a pointer to a list of boxes corresponding to the
 :*               conditional expression 'cond2'; the list of holes related
 :*               to the branch address 'true' (resp. false) is identified by
 :*               its head 'th1' (resp. 'fh1') and tail 'tt1' (resp. 'ft1').
 :*             - 'connectType' is a flag indicating if the two conditions
 :*               are to be OR-ed or AND-ed
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/   if ErrorFound (i.e. an error was found somewhere) nothing
 :*                  is done.
 :*               else if 'connectType' = TRUE  (AND-ed)
 :*                  the set {p, th, tt, fh, ft} identifies the internal code
 :*                  (along with the related two lists of holes) generated
 :*                  for <cond1> <and> <cond2>
 :*               otherwise (OR-ed)
 :*                  the set {p, th, tt, fh, ft} identifies the internal code
 :*                  (along with the related two lists of holes) generated
 :*                  for <cond1> <or> <cond2>
 :*                                                                 /)
 :*
 :*                                                             END-Post
 :*   RETURNS  BoxPtr (/ A pointer to the resulting list /)
 :*
 :*   REMARKS     (/
 :*                   - refer to <DOCUMENTS> for a detailed description of
 :*                     the internal representation (boxes).
 :*                                                         /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY term.connectBoxes
 :******************************************************************
*/
connectBoxes(connectType, p, p1, fh, ft, th, tt, fh1, ft1, th1, tt1)
unsigned connectType;
BoxPtr *p, p1;
holePtr *fh, *ft, *th, *tt, *fh1, ft1, *th1, tt1;
{
    if (!ErrorFound && *p != NULL && p1 != NULL) {
	if (connectType)  { /* the two conditions are to be AND-ed */
	    fill_holesb(*th, p1);  /* fill 'true' holes with p1 */
	    *ft1 = (hole) *fh;     /* append 'false' lists */
	    *fh = *fh1;
	    *th = *th1;
	    *tt = tt1;
	} else  {           /* to be OR-ed */
	    fill_holesb(*fh, p1);  /* fill 'false' holes with p1 */
	    *tt1 =  (hole) *th;    /* append 'true' lists */
	    *th = *th1;
	    *fh = *fh1;
	    *ft = ft1;
	}
    } else if (*p == NULL && p1 != NULL) {
	   /* the resulting list is identified by p1, th1, tt1, fh1, ft1 */
	    *p = p1;
	    *fh = *fh1;
	    *ft = ft1;
	    *th = *th1;
	    *tt = tt1;
    }
}
