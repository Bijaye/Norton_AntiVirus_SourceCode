/*
 :******************************************************************
 :* MODULE-DESCRIPTION
 :*
 :* TITLE         (/ Parsing conditional and repetitive actions    /)
 :*
 :* NAME             guardAct.c
 :*
 :* COPYRIGHT     (C)   SIEMENS NIXDORF INFORMATIONSSYSTEME AG  1992
 :*                     ALL RIGHTS RESERVED
 :*
 :* MEMORY-CLASS  6
 :*
 :* PROG-CONVENTS (/ <internal conventions (instance, names,
 :*                  external data, ...). None -> empty text>     /)
 :*
 :* PROCEDURE     guardedAction
 :* REMARKS       (/ parsing a conditional action/)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     guardAction
 :* REMARKS       (/ parsing a non empty conditional action     /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     grdedAction
 :* REMARKS       (/ parsing a guarded action                        /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     doGuardedAction
 :* REMARKS       (/ parsing a repetitive action            /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     doGuardAction
 :* REMARKS       (/ parsing a non empty repetitive action  /)
 :* ENTRY         EXTERNAL
 :*
 :*-----------------------------------------------------------------
 :*
 :* VERSION       001
 :*   CRDATE      1992-03-09
 :*   AUTHOR      (/ Mounji Abdelaziz       , FUNDP /)
 :*   UPDATE      (/ original issue /)
 :*
 :* END-MODULE-DESCRIPTION guardActions.c
 :******************************************************************
*/
#include <stdio.h>
#include "asax.h"

/*
 :******************************************************************
 :* UNIT-BODY guardAct.guardedAction
 :*
 :* TITLE         (/ parsing a conditional action                    /)
 :*
 :* PURPOSE       (/ parse a sequence of symbols representing a conditional
 :*                  action; generate the corresponding internal code if no
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
 :*             - 'Stop' is an array stops (see REMARKS below)
 :*             - 'lg' is the length of 'Stop'
 :*             - 'deltaIn' is the current address in the Working Area
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/   if the CSS is of the following form:
 :*                 <guardedAction_1>";" ... ";"<guardedAction_N>"fi"<rest>
 :*               where
 :*                  - N >= 0 (the list may be empty)
 :*                  - <guardedAction_i> is a sequence of symbols representing
 :*                                      a valid guarded action i=1..N
 :*                  <rest> is any character sequence
 :*               the effect is the following:
 :*                  - the CSS is "fi"<rest>
 :*                  - a pointer to the list of boxes corresponding to the
 :*                    conditional action:
 :*
 :*                    "if"<guardedAction_1>";" ... ";"<guardedAction_N>"fi"
 :*
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
 :* END-BODY expr.leftExpr
 :******************************************************************
*/
/* guardedAction ::= [guardAction] */

BoxPtr guardedAction(deltaIn, Stop, lg, head, tail)
char *deltaIn;
int Stop[], lg;
holePtr *head, *tail;
{
    int S1[Mlg], lg1;
    BoxPtr guardAction();

    lg1 = merge(Stop, lg, CondStop, lgCond, S1);
    SyntaxCheck(S1, lg1);
    if (InStop(symb.ibsymb, CondStop, lgCond) >= 0) {
	return(guardAction(deltaIn, Stop, lg, head, tail));
    } else {
	*head = *tail = NULL;
	return (NULL);
    }
}
/*
 :******************************************************************
 :* UNIT-BODY guardAct.guardAction
 :*
 :* TITLE         (/ parsing a non empty conditional action              /)
 :*
 :* PURPOSE       (/ parse a sequence of symbols representing a conditional
 :*                  action; generate the corresponding internal code if no
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
 :*             - 'Stop' is an array stops (see REMARKS below)
 :*             - 'lg' is the length of 'Stop'
 :*             - 'deltaIn' is the current address in the Working Area
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/   if the CSS is of the following form:
 :*                 <guardedAction_1>";" ... ";"<guardedAction_N>"fi"<rest>
 :*               where
 :*                  - N >= 1 (the list may not be empty)
 :*                  - <guardedAction_i> is a sequence of symbols representing
 :*                                      a valid guarded action i=1..N
 :*                  <rest> is any character sequence
 :*               the effect is the following:
 :*                  - the CSS is "fi"<rest>
 :*                  - a pointer to the list of boxes corresponding to the
 :*                    repetitive action:
 :*
 :*                    "if"<guardedAction_1>";" ... ";"<guardedAction_N>"fi"
 :*
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
 :* END-BODY expr.leftExpr
 :******************************************************************
*/
/* guardAction ::= grdedAction { ";" grdedAction } */

BoxPtr guardAction(deltaIn, Stop, lg, head, tail)
char *deltaIn;
int Stop[], lg;
holePtr *head, *tail;
{
    unsigned isTrue;
    enum truthValue exit_value;
    int S1[Mlg], lg1, S2[Mlg], lg2;
    holePtr fh, ft, fh1, ft1, head1, tail1;
    BoxPtr p, p1, grdedAction();

    insert_stop(semicolon, Stop, lg, S2, &lg2);
    lg1 = merge(S2, lg2, CondStop, lgCond, S1);
    p = grdedAction(&exit_value, deltaIn, S2, lg2, &fh, &ft, head, tail);
    if (exit_value == true_exit) isTrue = TRUE;
    else isTrue = FALSE;
    while (symb.ibsymb == semicolon) {
	Expect(semicolon, S1, lg1);
	p1 = grdedAction(&exit_value, deltaIn, S2, lg2,
			 &fh1, &ft1, &head1, &tail1);
	if (!isTrue && !ErrorFound) {
	    if (p != NULL && p1 != NULL) {
		fill_holesb(fh, p1);
		*tail1 = (hole) *head;
		*head = head1;
		if (fh1 != NULL && ft1 != NULL) {
		    fh = fh1;
		    ft = ft1;
		}
	    } else if (p == NULL && p1 != NULL) {
		p = p1;
		*head = head1;
		*tail = tail1;
		if (fh1 != NULL && ft1 != NULL) {
		    fh =  fh1;
		    ft =  ft1;
		} else {
		    fh = ft = NULL;
		}
	    }
	    isTrue = (exit_value == true_exit)? TRUE: FALSE;
	}
    }
    if (!ErrorFound && p != NULL && !isTrue) {
	   *ft = (hole) *head;
	   *head = fh;
    }
    return(p);
}

/*
 :******************************************************************
 :* UNIT-BODY guardAct.grdedAction
 :*
 :* TITLE         (/ parsing a guarded action                    /)
 :*
 :* PURPOSE       (/ parse a sequence of symbols representing a guarded
 :*                  action; generate the corresponding internal code if no
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
 :*             - 'Stop' is an array stops (see REMARKS below)
 :*             - 'lg' is the length of 'Stop'
 :*             - 'deltaIn' is the current address in the Working Area
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/   if the CSS is of the following form:
 :*                 <condition><rightArrow><action><rest>
 :*               where
 :*                  - <condition> is a sequence of symbols representing
 :*                                a valid condition
 :*                  - <rightArrow> is the keyword "-->" with possibly leading
 :*                                 and ending white spaces (blanks, tabs or
 :*                                 end of lines)
 :*                  <rest> is any character sequence
 :*               the effect is the following:
 :*                  - the CSS is <rest>
 :*                  - a pointer to the list of boxes corresponding to the
 :*                    guarded action:
 :*
 :*                     condition  --> action
 :*
 :*                    is returned in 'p'
 :*                  - *false_head and *false_tail are respectively the head
 :*                    and tail of the list of holes related to the address
 :*                    of the box to be executed next if the condition is
 :*                    evaluated to 'false'
 :*
 :*                  - *head and *tail are respectively the head and tail of
 :*                    the list of holes related to the address of the box to
 :*                    be executed next when the whole guarded action will be
 :*                    executed;
 :*                  - if <condition> is
 :*                           <true>
 :*                        or <not> <false>
 :*                       'exit_value' = true_exit
 :*                    else if the expression is
 :*                           <false>
 :*                        or <not> <true>
 :*                       'exit_value' = false_exit
 :*                    in all other cases
 :*                       'exit_value' = any_exit
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
 :* END-BODY expr.leftExpr
 :******************************************************************
*/
/* grdedAction ::= condition "-->" action */

BoxPtr grdedAction(exit_value, deltaIn, Stop, lg,
		  false_head, false_tail, head, tail)
enum truthValue *exit_value;
char *deltaIn;
int Stop[], lg;
holePtr *false_head, *false_tail, *head, *tail;
{
    int S1[Mlg], lg1, S2[Mlg], lg2;
    holePtr th, tt;
    BoxPtr p1, p2, sxcond(), action();

    lg2 = merge(Stop, lg, ActionStop, lgAction, S2);
    insert_stop(larrow, S2, lg2, S1, &lg1);
    p1 = sxcond(FALSE, exit_value, deltaIn, S1, lg1, false_head, false_tail,
		&th, &tt);
    Expect(larrow, S2, lg2);
    p2 = action(deltaIn, Stop, lg, head, tail);
    if (!ErrorFound) {
    if (*exit_value == false_exit) {
	return(NULL);
    } else if (*exit_value == true_exit && p2 != NULL) { /* != skip */
	*false_head = *head;
	*false_tail = *tail;
	return(p2);
    } else if (p2 == NULL) {
	*head = th ;
	*tail = tt;
	return(p1);
    } else {
	fill_holesb(th, p2);
	return(p1);
    }
    } else
	return(NULL);
}
/*
 :******************************************************************
 :* UNIT-BODY guardAct.doGuardedAction
 :*
 :* TITLE         (/ parsing a repetitive action                    /)
 :*
 :* PURPOSE       (/ parse a sequence of symbols representing a repetitive
 :*                  action; generate the corresponding internal code if no
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
 :*             - 'Stop' is an array stops (see REMARKS below)
 :*             - 'lg' is the length of 'Stop'
 :*             - 'deltaIn' is the current address in the Working Area
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/   if the CSS is of the following form:
 :*                 <guardedAction_1>";" ... ";"<guardedAction_N>"od"<rest>
 :*               where
 :*                  - N >= 0 (the list may be empty)
 :*                  - <guardedAction_i> is a sequence of symbols representing
 :*                                      a valid guarded action i=1..N
 :*                  <rest> is any character sequence
 :*               the effect is the following:
 :*                  - the CSS is "od"<rest>
 :*                  - a pointer to the list of boxes corresponding to the
 :*                    repetitive action:
 :*
 :*                    "do"<guardedAction_1>";" ... ";"<guardedAction_N>"od"
 :*
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
 :* END-BODY guardAct.doguardedAction
 :******************************************************************
*/

/* doGuardedAction ::= [doGuardAction] */

BoxPtr doGuardedAction(deltaIn, Stop, lg, head, tail)
char *deltaIn;
int Stop[], lg;
holePtr *head, *tail;
{
    int S1[Mlg], lg1;
    BoxPtr doGuardAction();

    lg1 = merge(Stop, lg, CondStop, lgCond, S1);
    SyntaxCheck(S1, lg1);
    if (InStop(symb.ibsymb, CondStop, lgCond) >= 0) {
	return(doGuardAction(deltaIn, Stop, lg, head, tail));
    } else {
	*head = *tail = NULL;
	return (NULL);
    }
}
/*
 :******************************************************************
 :* UNIT-BODY guardAct.doGuardAction
 :*
 :* TITLE         (/ parsing a non empty repetitive action                    /)
 :*
 :* PURPOSE       (/ parse a sequence of symbols representing a non empty
 :*                  repetitive action; generate the corresponding internal
 :*                  code if no error is found or recover from this error
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
 :*             - 'Stop' is an array stops (see REMARKS below)
 :*             - 'lg' is the length of 'Stop'
 :*             - 'deltaIn' is the current address in the Working Area
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/   if the CSS is of the following form:
 :*                 <guardedAction_1>";" ... ";"<guardedAction_N>"od"<rest>
 :*               where
 :*                  - N >= 1 (the list may be empty)
 :*                  - <guardedAction_i> is a sequence of symbols representing
 :*                                      a valid guarded action i=1..N
 :*                  <rest> is any character sequence
 :*               the effect is the following:
 :*                  - the CSS is "od"<rest>
 :*                  - a pointer to the list of boxes corresponding to the
 :*                    repetitive action:
 :*
 :*                    "do"<guardedAction_1>";" ... ";"<guardedAction_N>"od"
 :*
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
 :* END-BODY guardAct.doguardedAction
 :******************************************************************
*/
/* doGuardAction ::= grdedAction { ";" grdedAction } */

BoxPtr doGuardAction(deltaIn, Stop, lg, head, tail)
char *deltaIn;
int Stop[], lg;
holePtr *head, *tail;
{
    unsigned isTrue;
    enum truthValue exit_value;
    int S1[Mlg], lg1, S2[Mlg], lg2;
    holePtr fh, ft, fh1, ft1, head1, tail1;
    BoxPtr p, p1, grdedAction();

    insert_stop(semicolon, Stop, lg, S2, &lg2);
    lg1 = merge(S2, lg2, CondStop, lgCond, S1);
    p = grdedAction(&exit_value, deltaIn, S2, lg2,
		    head, tail, &head1, &tail1);
    if (!ErrorFound && p != NULL) {
	fill_holesb(head1, p);
    }
    isTrue = (exit_value == true_exit)? TRUE: FALSE;
    while (symb.ibsymb == semicolon) {
	Expect(semicolon, S1, lg1);
	p1 = grdedAction(&exit_value, deltaIn, S2, lg2,
			 &fh1, &ft1, &head1, &tail1);
	if (!ErrorFound && !isTrue) {
	    if (p != NULL && p1 != NULL) {
		fill_holesb(*head, p1);
		*head = fh1;
		*tail = ft1;
		fill_holesb(head1, p);
	    } else if (p == NULL && p1 != NULL) {
		p = p1;
		*head = fh1;
		*tail = ft1;
		fill_holesb(head1, p);
	    }
	    isTrue = (exit_value == true_exit)? TRUE: FALSE;
	}
    }
    if (isTrue) *head = *tail = NULL;
    return(p);
}




