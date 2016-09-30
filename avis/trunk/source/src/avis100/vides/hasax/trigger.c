/*
 :******************************************************************
 :* MODULE-DESCRIPTION
 :*
 :* TITLE         (/ Parsing a rule triggering /)
 :*
 :* NAME             trigger.c
 :*
 :* COPYRIGHT     (C)   SIEMENS NIXDORF INFORMATIONSSYSTEME AG  1992
 :*                     ALL RIGHTS RESERVED
 :*
 :* MEMORY-CLASS  6
 :*
 :* PROG-CONVENTS (/ <internal conventions (instance, names,
 :*                  external data, ...). None -> empty text>     /)
 :*
 :* PROCEDURE     sxtrigger
 :* REMARKS       (/ parsing a rule triggering /)
 :* ENTRY         EXTERNAL
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
 :* END-MODULE-DESCRIPTION trigger.c
 :******************************************************************
*/
#include <stdio.h>
#include "asax.h"
/*
 :******************************************************************
 :* UNIT-BODY trigger.sxtrigger
 :*
 :* TITLE         (/ parsing a rule triggering                   /)
 :*
 :* PURPOSE       (/ parsing a sequence of symbols representing a rule
 :*                  triggering; generate the corresponding internal code if
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
 :*                 - trdescr[]   : rule descriptors table
 :*                 - tdescr[]    : names descriptors table (param. & var.)
 :*                 - targ_types[]: parameter types for undeclared rules
 :*                 - symb        : holds the internal representation of the
 :*                                 last read symbol.
 :*                 - ErrorFound  : = TRUE if an error was found
 :*                                 = FALSE otherwise
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
 :*    DEFINITION (/
 :*
 :*     - the CSS is s1 s2 ... sn  (see REMARKS below)
 :*     - the sequence of symbols already parsed is t0 ... tp (so as the
 :*       whole file is t0, ...,tp, s1, ..., sn)
 :*     - let rule_0, ..., rule_m be the sequence of rule declarations or
 :*       triggerings in t0 ... tp (p>=m)
 :*     - let r_0, ..., r_s  (s<=m<=p) be the sequence of the first
 :*       occurrences in  rule_0, ..., rule_m
 :*     - let rep_i be the number of times r_i is repeated in
 :*               rule_1, ..., rule_m             (rep_i>=1)
 :*     - the occurrences of r_i in rule_1, ..., rule_m are noted:
 :*               rule_i_1, ..., rule_i_rep_i     (rule_i_1 = r_i)
 :*     - under the above definitions we say that the three tables
 :*
 :*                     'trdescr[]', 'tdescr[]' and 'targ_types[]'
 :*
 :*       describe t0, ..., tp if the following holds (0<=i<=s):
 :*
 :*         - 'trdescr[i].rule_name' is the rule name of r_i
 :*         - 'trdescr[i].status' = 'unknown'  if rep_i = 1 and r_i is a
 :*                                            rule triggering
 :*
 :*                               = 'known'    if rep_i>1 and neither of
 :*                                             rule_i_1, ..., rule_i_rep_i
 :*                                            is a rule declaration
 :*
 :*                               = 'declared' if there exist j such
 :*                                               1<=j<=rep_i and rule_i_j
 :*                                            is a rule declaration
 :*
 :*         - if 'trdescr[i].status' = 'declared'
 :*
 :*
 :*             - 'trdescr[i].Code' = a pointer to the internal code of the
 :*                                   action part corresponding to r_i
 :*
 :*             -'trdescr[i].ParNre' = number of parameters in r_i (= ParNre)
 :*             -'trdescr[i].VarNre' = number of variables in r_i  (= VarNre)
 :*             - 'trdescr[i].APASize' = the size (in bytes) of the Actual
 :*                                      Parameter Area
 :*
 :*             - 'trdescr[i].nameDescr' = (=base) index in 'tdescr[]' table
 :*                 such that 'tdescr[k]' is the descriptor of the k-th
 :*                 parameter or local variable i.e:
 :*                     'tdescr[k].var_par' = 'var' if this is a variable
 :*                                         = 'par' if it is a parameter
 :*                     'tdescr[k].name'    = its name
 :*                     'tdescr[k].tname'   = its type
 :*                  where base<=k< ParNre+VarNre
 :*
 :*         - if 'trdescr[i].status' = 'unknown'  or 'known'
 :*
 :*             - 'trdescr[i].Code'       contains a list of holes related to
 :*                                       the internal code of r_i
 :*             - 'trdescr[i].ParNre'     = number of arguments in r_i
 :*                                         (= ParNre)
 :*             - 'trdescr[i].targDescr'  = (=base) index in 'targ_types[]'
 :*                                         such that 'targ_types[j]' is the
 :*                                         type of the j-th argument in the
 :*                                         rule triggering for base<=j<ParNre
 :*
 :*                                                                    /)
 :*                                                           END-DEFINITION
 :*
 :*   Pre    (/ - the CSS is s1 s2 ... sn     (see REMARKS below)
 :*             - the sequence of symbols already parsed t0, ..., tp is
 :*               described by 'trdescr[]', 'tdescr[]' and 'targ_types[]'
 :*               (see DEFINITION above)
 :*             - 'Stop' is an array of stops (see REMARKS below)
 :*             - 'lg' is the length of 'Stop'
 :*                                                                      /)
 :*
 :*                                                             END-Pre
 :*   Post   (/   if the CSS is of the following form:
 :*
 :*                           <r><rest>
 :*
 :*               where
 :*                  - <r> = "trigger"<"off"><tr_mode><rule_name><arg_list>
 :*                  - <"off"> is the keyword "off" with possibly leading
 :*                            and / or ending spaces (blanks, tabs, or eols)
 :*                  - <tr_mode> is one of the keywords "for_current",
 :*                              "for_next" or "at_completion" with possibly
 :*                              leading and/or ending spaces
 :*                  - <rule_name> is an identifier representing a rule name
 :*                  - <arg_list> is a sequence of symbols representing a list
 :*                               of arguments
 :*                  - <rest> is any character sequence
 :*               the effect is the following:
 :*                  - the CSS is <rest>
 :*                  - 'trdescr[]', 'tdescr[]' and 'targ_types[]' describe
 :*                    the sequence t0, ..., tp, <r>   (see DEFINITION above)
 :*                  - a pointer to the list of boxes corresponding to this
 :*                    rule triggering is returned in 'p'
 :*                  - 'head' and 'tail' are respectively the head and tail
 :*                    of the list of holes in this list of boxes
 :*                otherwise
 :*                  - the CSS is of the form:
 :*                          <first_Stop><rest>
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
 :* END-BODY trigger.sxtrigger
 :******************************************************************
*/

BoxPtr sxtrigger(Stop, lg, head, tail)
int Stop[], lg;
holePtr *head, *tail;
{
	int S0[30], lg0, S1[30], lg1, S2[30], lg2, S3[30], lg3, S4[30], lg4;
	BoxPtr p, p1, setLenBox(), allocateBox();
	char *tr_rule_name;     /* name of the triggered rule */
	rdescr *rdescrTab;
	enum index tr_mode;     /* the triggering mode */
	int *h0, status, rule_addr;

        /* build stops */
	insert_stop(rparenthesis, Stop, lg, S4, &lg4);
	lg3 = merge(ActParStop, lgActPar, S4, lg4, S3);
	insert_stop(lparenthesis, S3, lg3, S2, &lg2);
	lg1 = merge(S2, lg2, TrModeStop, lgTrMode, S1);
	insert_stop(offx, S1, lg1, S0, &lg0);
	Expect(triggerx, S0, lg0);
	Expect(offx, S1, lg1);
        /* is the triggering mode valid ? */
	if (InStop(symb.ibsymb, TrModeStop, lgTrMode) >= 0) {
		tr_mode = symb.ibsymb;
		Expect(symb.ibsymb, S2, lg2); /* read this mode */
	} else
		SyntaxError(S2, lg2, 3);
	tr_rule_name = (char *) malloc(ident_lg);
	ExpectName(tr_rule_name, S2, lg2);
	if (symb.ibsymb == lparenthesis) {
	    Expect(lparenthesis, S3, lg3);        /* read left parenthesis */
	    p = RuleActParList(tr_mode, tr_rule_name, S4, lg4, head, tail);
	    Expect(rparenthesis, Stop, lg);       /* read right parenth.   */
	} else {
	    add_rule_trig(tr_rule_name);     /* add this rule to trdescr[] */
	    p = setLenBox(&h0);                 /* build set length box */
	    *h0 = 0;
	    p1 = allocateBox(tr_mode, tr_rule_name); /* build allocate box */
	    p->alpha = p1;
	    *head = *tail = (holePtr) &p1->alpha; /* list of holes */
	    rule_addr = getRuleAddr(tr_rule_name , &rdescrTab);
	    status = trdescr[rule_addr].status;
	    if (status != unknown) {
		if (trdescr[rule_addr].ParNre != 0) {
		    error(semantics, 8);        /* check arity */
		}
	    } else {
		trdescr[rule_addr].ParNre = 0;
	    }
	}
	free(tr_rule_name);
	return (p);
}
