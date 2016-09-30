/*
 :******************************************************************
 :* MODULE-DESCRIPTION
 :*
 :* TITLE         (/ error recovery management /)
 :*
 :* NAME          recov.c
 :*
 :* COPYRIGHT     (C)   SIEMENS NIXDORF INFORMATIONSSYSTEME AG  1992
 :*                     ALL RIGHTS RESERVED
 :*
 :* MEMORY-CLASS  6
 :*
 :* PROG-CONVENTS (/ <internal conventions (instance, names,
 :*                  external data, ...). None -> empty text>     /)
 :*
 :* PROCEDURE     Expect
 :* REMARKS       (/ expecting a symbol             /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     ExpectName
 :* REMARKS       (/ expecting a name             /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     SyntaxCheck
 :* REMARKS       (/ checking of the current symbol          /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     SyntaxError
 :* REMARKS       (/ error message output               /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     InStop
 :* REMARKS       (/ dichotomic search for a stop   /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     get_position
 :* REMARKS       (/ find position in a set of stops/)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     insert_stop
 :* REMARKS       (/ insertion of a stop                          /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     checkTypes
 :* REMARKS       (/ type checking       /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     printsymb
 :* REMARKS       (/ output current symbol /)
 :* ENTRY         EXTERNAL
 :*
 :*-----------------------------------------------------------------
 :*
 :* VERSION       001
 :*   CRDATE      1992-11-27
 :*   AUTHOR      (/ Mounji Abdelaziz    , FUNDP /)
 :*   UPDATE      (/ original issue /)
 :*
 :* END-MODULE-DESCRIPTION recov.c
 :******************************************************************
*/

#include "asax.h"

/*
 :******************************************************************
 :* UNIT-BODY recov.Expect
 :*
 :* TITLE         (/ expecting a symbol     /)
 :*
 :* PURPOSE       (/ check if the current symbol is the one expected. If this
 :*                  is not, skip to any of a specified set of symbols /)
 :*                                                      END-PURPOSE
 :* VERSION       001
 :*   CRDATE      1992-11-03
 :*   AUTHOR      (/ Mounji Abdelaziz       , FUNDP              /)
 :*   UPDATE      (/ original issue /)
 :*
 :* REALIZATION   (/ <note, reference to other proc. (optional)> /)
 :*
 :*   CONTROL    (/
 :*                                                               /)
 :*   DATA
 :*       Global Data:  (/
 :*                       - specsymbtab: Table of special symbol of the
 :*                                      rule based language.
 :*                       - symb       : holds the internal representation
 :*                                      of the read symbol.
 :*                       - rfp        : (FILE *) file pointer of the text
 :*                                      file containing the rules.
 :*                                                                  /)
 :*
 :*                                                          END-DATA
 :*   ASSUMPTIONS     (/ <external conditions> /)
 :*   PERFORMANCE     (/ <measures taken or not: when, why, how> /)
 :*   VALIDATION      (/ <for which param. or other data ? How ?> /)
 :*   SYNCHRONIZATION (/ <serialization & locks: purpose, method> /)
 :*   INIT&TERM       (/ <what must already/still be available> /)
 :*   ERROR-HANDLING  (/ <reaction to errors, r.c., state of data>/)
 :*   DOCUMENTS       (/ Advanced Security Audit trail analysis on uniX
 :*                             (ASAX also called SAT-X)
 :*                      Implementation design of the NADF Evaluator   /)
 :*                                                  END-REALIZATION
 :* PARAMETERS
 :*
 :*   Pre (/ - the CSS is s1 s2 ... sn  (see REMARKS below)
 :*          - s is in 'specsymbtab[]'
 :*          - 'Stop' is an array stops (see REMARKS below)
 :*          - 'lg' is the length of 'Stop'
 :*       /)
 :*                                                             END-Pre
 :*   Post   (/ if symb.ibsymb = s and s2.ibsymb is in 'Stop[]'
 :*                the CSS is s2 ... sn
 :*             otherwise
 :*               - the CSS is of the form:
 :*                         <first_Stop><rest>
 :*                    where <first_Stop> is the first symbol in
 :*                                 s1 s2 ... sn
 :*                          such that <first_Stop> is in 'Stop[]'
 :*
 :*               - the error message:
 :*
 :*                     Syntax             'lineNbre': unexpected symbol
 :*
 :*                 where 'lineNbre' is the line number of s1 or s2
 :*                 is output to stdout
 :*               - 'ErrorFound' is set to TRUE
 :*                                                             END-Post
 :*   RETURNS  NONE
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
 :*                                                         /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY recov.Expect
 :******************************************************************
*/
Expect(s, Stop, lg)
int s, Stop[], lg;
{
	if (s == (int) symb.ibsymb) {
		getsymb(rfp);
	} else {
		SyntaxError(Stop, lg, 3);  /* unexpected symbol */
	}
	SyntaxCheck(Stop, lg);
}
/*
 :******************************************************************
 :* UNIT-BODY recov.ExpectName
 :*
 :* TITLE         (/ expecting a name    /)
 :*
 :* PURPOSE       (/ check if the current symbol is identifier. If this
 :*                  is not, skip to any of a specified set of symbols /)
 :*                                                      END-PURPOSE
 :* VERSION       001
 :*   CRDATE      1992-11-03
 :*   AUTHOR      (/ Mounji Abdelaziz       , FUNDP              /)
 :*   UPDATE      (/ original issue /)
 :*
 :* REALIZATION   (/ <note, reference to other proc. (optional)> /)
 :*
 :*   CONTROL    (/
 :*                                                               /)
 :*   DATA
 :*       Global Data:  (/
 :*                       - specsymbtab: Table of special symbol of the
 :*                                      rule based language.
 :*                       - symb       : holds the internal representation
 :*                                      of the read symbol.
 :*                       - rfp        : (FILE *) file pointer of the text
 :*                                      file containing the rules.
 :*                                                                  /)
 :*
 :*                                                          END-DATA
 :*   ASSUMPTIONS     (/ <external conditions> /)
 :*   PERFORMANCE     (/ <measures taken or not: when, why, how> /)
 :*   VALIDATION      (/ <for which param. or other data ? How ?> /)
 :*   SYNCHRONIZATION (/ <serialization & locks: purpose, method> /)
 :*   INIT&TERM       (/ <what must already/still be available> /)
 :*   ERROR-HANDLING  (/ <reaction to errors, r.c., state of data>/)
 :*   DOCUMENTS       (/ Advanced Security Audit trail analysis on uniX
 :*                             (ASAX also called SAT-X)
 :*                      Implementation design of the NADF Evaluator   /)
 :*                                                  END-REALIZATION
 :* PARAMETERS
 :*
 :*   Pre (/ - the CSS is s1 s2 ... sn  (see REMARKS below)
 :*          - 'Stop' is an array stops (see REMARKS below)
 :*          - 'lg' is the length of 'Stop'
 :*       /)
 :*                                                             END-Pre
 :*   Post   (/ if symb.ibsymb = identifierx and s2.ibsymb is in 'Stop[]'
 :*               - the CSS is s2 ... sn
 :*               - name is the identifier s1
 :*             otherwise
 :*               - the CSS is of the form:
 :*                         <first_Stop><rest>
 :*                    where <first_Stop> is the first symbol in
 :*                                 s1 s2 ... sn
 :*                          such that <first_Stop> is in 'Stop[]'
 :*
 :*               - the error message:
 :*
 :*                     Syntax             'lineNbre': identifier expected
 :*
 :*                 where 'lineNbre' is the line number of s1 or s2
 :*                 is output to stdout
 :*               - 'ErrorFound' is set to TRUE
 :*                                                             END-Post
 :*   RETURNS  NONE
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
 :*                                                         /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY recov.ExpectName
 :******************************************************************
*/
ExpectName(name, Stop, lg)
char name[];
int Stop[], lg;
{
	int i;

	if (identifierx == symb.ibsymb)  {
		i=0;
		do  name[i] = symb.bsymbval.svalue[i];
		    while (name[i++] != '\0');
		getsymb(rfp);
	}
	else {
		name[0] = '\0';
		SyntaxError(Stop, lg, 2);   /* identifier expected */
	}
	SyntaxCheck(Stop, lg);
}
/*
 :******************************************************************
 :* UNIT-BODY recov.SyntaxCheck
 :*
 :* TITLE         (/ checking of the current symbol   /)
 :*
 :* PURPOSE       (/ check if the current symbol is the one expected. If this
 :*                  is not, skip to any of a specified set of symbols /)
 :*                                                      END-PURPOSE
 :* VERSION       001
 :*   CRDATE      1992-11-03
 :*   AUTHOR      (/ Mounji Abdelaziz       , FUNDP              /)
 :*   UPDATE      (/ original issue /)
 :*
 :* REALIZATION   (/ <note, reference to other proc. (optional)> /)
 :*
 :*   CONTROL    (/
 :*                                                               /)
 :*   DATA
 :*       Global Data:  (/
 :*                       - specsymbtab: Table of special symbol of the
 :*                                      rule based language.
 :*                       - symb       : holds the internal representation
 :*                                      of the read symbol.
 :*                                                                  /)
 :*
 :*                                                          END-DATA
 :*   ASSUMPTIONS     (/ <external conditions> /)
 :*   PERFORMANCE     (/ <measures taken or not: when, why, how> /)
 :*   VALIDATION      (/ <for which param. or other data ? How ?> /)
 :*   SYNCHRONIZATION (/ <serialization & locks: purpose, method> /)
 :*   INIT&TERM       (/ <what must already/still be available> /)
 :*   ERROR-HANDLING  (/ <reaction to errors, r.c., state of data>/)
 :*   DOCUMENTS       (/ Advanced Security Audit trail analysis on uniX
 :*                             (ASAX also called SAT-X)
 :*                      Implementation design of the NADF Evaluator   /)
 :*                                                  END-REALIZATION
 :* PARAMETERS
 :*
 :*   Pre (/ - the CSS is s1 s2 ... sn  (see REMARKS below)
 :*          - 'Stop' is an array stops (see REMARKS below)
 :*          - 'lg' is the length of 'Stop'
 :*       /)
 :*                                                             END-Pre
 :*   Post   (/ if s1.ibsymb is in 'Stop[]' nothing is done
 :*             otherwise
 :*               - the CSS is of the form:
 :*                         <first_Stop><rest>
 :*                    where <first_Stop> is the first symbol in
 :*                                 s1 s2 ... sn
 :*                          such that <first_Stop> is in 'Stop[]'
 :*
 :*               - the error message:
 :*
 :*                     Syntax             'lineNbre': unexpected symbol
 :*
 :*                 where 'lineNbre' is the line number of s1 or s2
 :*                 is output to stdout
 :*               - 'ErrorFound' is set to TRUE                 /)
 :*                                                             END-Post
 :*   RETURNS  NONE
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
 :*                                                         /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY recov.SyntaxCheck
 :******************************************************************
*/
SyntaxCheck(Stop, lg)
int Stop[], lg;
{
	if (InStop(symb.ibsymb, Stop, lg)< 0) {
		SyntaxError(Stop, lg, 3);
	}
}
/*
 :******************************************************************
 :* UNIT-BODY recov.SyntaxError
 :*
 :* TITLE         (/ error message output   /)
 :*
 :* PURPOSE       (/ output an error message and skip to any of a specified
 :*                  set of symbols /)
 :*                                                      END-PURPOSE
 :* VERSION       001
 :*   CRDATE      1992-11-03
 :*   AUTHOR      (/ Mounji Abdelaziz       , FUNDP              /)
 :*   UPDATE      (/ original issue /)
 :*
 :* REALIZATION   (/ <note, reference to other proc. (optional)> /)
 :*
 :*   CONTROL    (/
 :*                                                               /)
 :*   DATA
 :*       Global Data:  (/
 :*                       - specsymbtab: Table of special symbol of the
 :*                                      rule based language.
 :*                       - symb       : holds the internal representation
 :*                                      of the read symbol.
 :*                       - rfp        : (FILE *) file pointer of the text
 :*                                      file containing the rules.
 :*                                                                  /)
 :*
 :*                                                          END-DATA
 :*   ASSUMPTIONS     (/ <external conditions> /)
 :*   PERFORMANCE     (/ <measures taken or not: when, why, how> /)
 :*   VALIDATION      (/ <for which param. or other data ? How ?> /)
 :*   SYNCHRONIZATION (/ <serialization & locks: purpose, method> /)
 :*   INIT&TERM       (/ <what must already/still be available> /)
 :*   ERROR-HANDLING  (/ <reaction to errors, r.c., state of data>/)
 :*   DOCUMENTS       (/ Advanced Security Audit trail analysis on uniX
 :*                             (ASAX also called SAT-X)
 :*                      Implementation design of the NADF Evaluator   /)
 :*                                                  END-REALIZATION
 :* PARAMETERS
 :*
 :*   Pre (/ - the CSS is s1 s2 ... sn  (see REMARKS below)
 :*          - 'errorcode' is an integer identifying an error message
 :*          - 'Stop' is an array stops (see REMARKS below)
 :*          - 'lg' is the length of 'Stop'
 :*       /)
 :*                                                             END-Pre
 :*   Post   (/   - the CSS is of the form:
 :*                          <first_Stop><rest>
 :*                    where <first_Stop> is the first symbol in
 :*                                 s1 s2 ... sn
 :*                          such that <first_Stop> is in 'Stop[]'
 :*               - the error message identified by 'errorcode' is output to
 :*                 stdout
 :*               - 'ErrorFound' is set to TRUE
 :*                                                             END-Post
 :*   RETURNS  NONE
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
 :*                                                         /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY recov.SyntaxError
 :******************************************************************
*/
SyntaxError(Stop, lg, errorcode)
int Stop[], lg;
short errorcode;
{
    char *str;

	error(syntax, errorcode);
	while (InStop(symb.ibsymb, Stop, lg) < 0)  {
		getsymb(rfp);
	}
}
/*
 :******************************************************************
 :* UNIT-BODY recov.InStop
 :*
 :* TITLE         (/ dichotomic search for a stop  /)
 :*
 :* PURPOSE       (/ return index of 's' in 'Stop[]' or -1 if not found /)
 :*                                                      END-PURPOSE
 :* VERSION       001
 :*   CRDATE      1992-11-03
 :*   AUTHOR      (/ Mounji Abdelaziz       , FUNDP              /)
 :*   UPDATE      (/ original issue /)
 :*
 :* REALIZATION   (/ <note, reference to other proc. (optional)> /)
 :*
 :*   CONTROL    (/
 :*                                                               /)
 :*   DATA       (/
 :*                                                                  /)
 :*
 :*                                                          END-DATA
 :*   ASSUMPTIONS     (/ <external conditions> /)
 :*   PERFORMANCE     (/ <measures taken or not: when, why, how> /)
 :*   VALIDATION      (/ <for which param. or other data ? How ?> /)
 :*   SYNCHRONIZATION (/ <serialization & locks: purpose, method> /)
 :*   INIT&TERM       (/ <what must already/still be available> /)
 :*   ERROR-HANDLING  (/ <reaction to errors, r.c., state of data>/)
 :*   DOCUMENTS       (/ Advanced Security Audit trail analysis on uniX
 :*                             (ASAX also called SAT-X)
 :*                      Implementation design of the NADF Evaluator   /)
 :*                                                  END-REALIZATION
 :* PARAMETERS
 :*
 :*   Pre (/ - 's' is a stop               (see REMARKS below)
 :*          - 'Stop' is an array of stops (see REMARKS below)
 :*          - 'lg' is the length of 'Stop'
 :*       /)
 :*                                                             END-Pre
 :*   Post   (/   - if there exist 'i' such that Stop[i] = s return 'i'
 :*                 otherwise return -1
 :*                                                                 /)
 :*                                                             END-Post
 :*   RETURNS  "int" (/ if >= 0 this is the index of 's' in 'Stop[];
 :*                     otherwise -1 signals that 's' is not found /)
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
 :*                                                         /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY recov.InStop
 :******************************************************************
*/
InStop(s, Stop, lg)
enum index s;
int Stop[], lg;
{
	int low, high, mid;

	low = 0;
	high = lg - 1;
	while (low <= high) {
		mid = (low + high) / 2;
		if ((int ) s < Stop[mid])
			high = mid - 1;
		else if ((int ) s > Stop[mid])
			low = mid + 1;
		else {/* s is a special symbol */
			return (mid);
		}
	}
	return (-1);
}
/*
 :******************************************************************
 :* UNIT-BODY recov.checkTypes
 :*
 :* TITLE         (/ type checking       /)
 :*
 :* PURPOSE       (/ output error message if two given types do not match /)
 :*                                                      END-PURPOSE
 :* VERSION       001
 :*   CRDATE      1992-11-03
 :*   AUTHOR      (/ Mounji Abdelaziz       , FUNDP              /)
 :*   UPDATE      (/ original issue /)
 :*
 :* REALIZATION   (/ <note, reference to other proc. (optional)> /)
 :*
 :*   CONTROL    (/
 :*                                                               /)
 :*   DATA  (/
 :*                                                                  /)
 :*
 :*                                                          END-DATA
 :*   ASSUMPTIONS     (/ <external conditions> /)
 :*   PERFORMANCE     (/ <measures taken or not: when, why, how> /)
 :*   VALIDATION      (/ <for which param. or other data ? How ?> /)
 :*   SYNCHRONIZATION (/ <serialization & locks: purpose, method> /)
 :*   INIT&TERM       (/ <what must already/still be available> /)
 :*   ERROR-HANDLING  (/ <reaction to errors, r.c., state of data>/)
 :*   DOCUMENTS       (/ Advanced Security Audit trail analysis on uniX
 :*                             (ASAX also called SAT-X)
 :*                      Implementation design of the NADF Evaluator   /)
 :*                                                  END-REALIZATION
 :* PARAMETERS
 :*
 :*   Pre (/ - 't1' is a pointer to a variable of the enumerated type:
 :*                 {integer, bytestring, fieldstring, undef}
 :*          - 't2' is a variable of this same enumerated type
 :*       /)
 :*                                                             END-Pre
 :*
 :*   Post (/ - if neither of the variables '*t1' and 't2' is 'undef'
 :*             and '*t1' != 't2
 :*               - the error mesage:
 :*                      Semantics        'lineNbre': type mismatch
 :*                  where 'lineNbre' indicates the line number of the error
 :*                  is output to stdout
 :*              - '*t1' is set to the value 'undef'
 :*                                                              /)
 :*
 :*                                                             END-Post
 :*   RETURNS  NONE
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY recov.checkTypes
 :******************************************************************
*/
checkTypes(t1, t2)
enum type *t1, t2;
{
	if (*t1 != t2) {
		if (*t1 != undef && t2 != undef)
			error(semantics, 10);
		*t1 = undef;
	}
}
/*
 :******************************************************************
 :* UNIT-BODY recov.printsymb
 :*
 :* TITLE         (/ output the current symbol     /)
 :*
 :* PURPOSE       (/ output the current symbol characteristics    /)
 :*                                                      END-PURPOSE
 :* VERSION       001
 :*   CRDATE      1992-11-03
 :*   AUTHOR      (/ Mounji Abdelaziz       , FUNDP              /)
 :*   UPDATE      (/ original issue /)
 :*
 :* REALIZATION   (/ <note, reference to other proc. (optional)> /)
 :*
 :*   CONTROL    (/
 :*                                                               /)
 :*   DATA  (/
 :*       Global Data:  (/
 :*                       - specsymbtab: Table of special symbol of the
 :*                                      rule based language.
 :*                       - symb       : holds the internal representation
 :*                                      of the read symbol.
 :*                                                                   /)
 :*
 :*                                                          END-DATA
 :*   ASSUMPTIONS     (/ <external conditions> /)
 :*   PERFORMANCE     (/ <measures taken or not: when, why, how> /)
 :*   VALIDATION      (/ <for which param. or other data ? How ?> /)
 :*   SYNCHRONIZATION (/ <serialization & locks: purpose, method> /)
 :*   INIT&TERM       (/ <what must already/still be available> /)
 :*   ERROR-HANDLING  (/ <reaction to errors, r.c., state of data>/)
 :*   DOCUMENTS       (/ Advanced Security Audit trail analysis on uniX
 :*                             (ASAX also called SAT-X)
 :*                      Implementation design of the NADF Evaluator   /)
 :*                                                  END-REALIZATION
 :* PARAMETERS
 :*
 :*   Pre (/
 :*                                                                /)
 :*                                                             END-Pre
 :*
 :*   Post (/ the characteristics of the current symbol 'symb' are output
 :*           to stdout. These characteristics are:
 :*                    - the symbol class i.e boolean, integer, special symbol
 :*                      identifier, C literal, X literal, erro symbol, or
 :*                      no symbol for the end of file;                                        /)
 :*                    - the index of the current symbol in 'specsymbtab[]'
 :*                    - the symbol itself
 :*
 :*                                                             END-Post
 :*   RETURNS  NONE
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY recov.printsymb
 :******************************************************************
*/
printsymb()
{
	int i, ib;
	char *s;

	ib = symb.ibsymb;
	i = symb.bsymbval.ivalue;
	s = symb.bsymbval.svalue;
	switch(symbolClass(symb.ibsymb)) {
	  case boolean   : printf("boolean: %d %d\n", i, ib);
			   break;
	  case integer   : printf("integer: %d %d\n", i, ib);
			   break;
	  case specsymb  : printf("special symbol: %s %d\n", s, ib);
			   break;
	  case identifier: printf("identifier: %s %d\n", s, ib);
			   break;
	  case X_literal : printf("X_literal: %s %d\n", s, ib);
			   break;
	  case C_literal : printf("C_literal: %s %d\n", s, ib);
			   break;
	  case errsymb   : printf("errsymb %d\n", ib);
			   break;
	  case eq        : printf("eq %d %d \n", ib, LineNbre);
			   break;
	  case nosymb    : printf("nosymb  %d\n", ib);
			   break;
	}
}
/*
 :******************************************************************
 :* UNIT-BODY recov.insert_stop
 :*
 :* TITLE         (/ insertion of a stop          /)
 :*
 :* PURPOSE       (/ insert a stop in a set of stops /)
 :*                                                      END-PURPOSE
 :* VERSION       001
 :*   CRDATE      1992-11-03
 :*   AUTHOR      (/ Mounji Abdelaziz       , FUNDP              /)
 :*   UPDATE      (/ original issue /)
 :*
 :* REALIZATION   (/ <note, reference to other proc. (optional)> /)
 :*
 :*   CONTROL    (/
 :*                                                               /)
 :*   DATA  (/
 :*                                                                  /)
 :*
 :*                                                          END-DATA
 :*   ASSUMPTIONS     (/ <external conditions> /)
 :*   PERFORMANCE     (/ <measures taken or not: when, why, how> /)
 :*   VALIDATION      (/ <for which param. or other data ? How ?> /)
 :*   SYNCHRONIZATION (/ <serialization & locks: purpose, method> /)
 :*   INIT&TERM       (/ <what must already/still be available> /)
 :*   ERROR-HANDLING  (/ <reaction to errors, r.c., state of data>/)
 :*   DOCUMENTS       (/ Advanced Security Audit trail analysis on uniX
 :*                             (ASAX also called SAT-X)
 :*                      Implementation design of the NADF Evaluator   /)
 :*                                                  END-REALIZATION
 :* PARAMETERS
 :*
 :*   Pre (/ - 's' is a stop                         (see REMARKS below)
 :*          - 'Stop' is an array of ascending stops (see REMARKS below)
 :*          - 'lg' is the length of 'Stop'
 :*          - {s_1, ..., s_lg} is the set of stops in 'Stop[]'
 :*       /)
 :*                                                             END-Pre
 :*
 :*   Post (/ - 'Stop_new' is an array of ascending stops (see REMARKS below)
 :*           - 'lg_new' is the length of 'Stop_new[]'
 :*           - {s_1, ..., s_lg} U {s}  is the set of stops in 'Stop_new[]'
 :*                                                              /)
 :*
 :*                                                             END-Post
 :*   RETURNS  NONE
 :*
 :*   REMARKS     (/ Definitions:
 :*                   - the values of the enumerated type 'enum index' are
 :*                     called 'stops'. They are used as indexes to the table
 :*                     'specsymbtab[]' so as a 'stop' identifies an element
 :*                     this table.
 :*                                                         /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY recov.insert_stop
 :******************************************************************
*/
insert_stop(s, Stop, lg, Stop_new, lg_new)
int s, Stop[], lg, Stop_new[], *lg_new;
{
	int pos, i;

	pos = get_position(s, Stop, lg);
	if (Stop[pos] == s) {
		for (i=0; i<lg; i++) Stop_new[i] = Stop[i];
		*lg_new = lg;
	} else {
		for (i=0; i<pos; i++) Stop_new[i] = Stop[i];
		Stop_new[pos] = s;
		for (i=pos+1; i<=lg; i++) Stop_new[i] = Stop[i-1];
		*lg_new = lg+1;
	  }
}
/*
 :******************************************************************
 :* UNIT-BODY recov.get_position
 :*
 :* TITLE         (/ find position in a set of stops  /)
 :*
 :* PURPOSE       (/ return an insertion point of a stop in a set of stops /)
 :*                                                      END-PURPOSE
 :* VERSION       001
 :*   CRDATE      1992-11-03
 :*   AUTHOR      (/ Mounji Abdelaziz       , FUNDP              /)
 :*   UPDATE      (/ original issue /)
 :*
 :* REALIZATION   (/ <note, reference to other proc. (optional)> /)
 :*
 :*   CONTROL    (/
 :*                                                               /)
 :*   DATA  (/
 :*                                                                  /)
 :*
 :*                                                          END-DATA
 :*   ASSUMPTIONS     (/ <external conditions> /)
 :*   PERFORMANCE     (/ <measures taken or not: when, why, how> /)
 :*   VALIDATION      (/ <for which param. or other data ? How ?> /)
 :*   SYNCHRONIZATION (/ <serialization & locks: purpose, method> /)
 :*   INIT&TERM       (/ <what must already/still be available> /)
 :*   ERROR-HANDLING  (/ <reaction to errors, r.c., state of data>/)
 :*   DOCUMENTS       (/ Advanced Security Audit trail analysis on uniX
 :*                             (ASAX also called SAT-X)
 :*                      Implementation design of the NADF Evaluator   /)
 :*                                                  END-REALIZATION
 :* PARAMETERS
 :*
 :*
 :*   Pre (/ - 'v' is a stop                         (see REMARKS below)
 :*          - 'Stop' is an array of ascending stops (see REMARKS below)
 :*          - 'lg' is the length of 'Stop'
 :*       /)
 :*                                                             END-Pre
 :*   Post   (/   - the returned value is 'i' such that:
 :*                     - 0 <= i <= lg
 :*                     - for all j:      0<=j<i    ==> Stop[j] < v
 :*                     - for all j:      i<=j<lg   ==> Stop[j] >= v
 :*                                                                   /)
 :*
 :*                                                             END-Post
 :*   RETURNS  "int" (/ the returned value is >=0 and satisfies Post  /)
 :*
 :*   REMARKS     (/ Definitions:
 :*                   - the values of the enumerated type 'enum index' are
 :*                     called 'stops'. They are used as indexes to the table
 :*                     'specsymbtab[]' so as a 'stop' identifies an element
 :*                     in this table.
 :*                                                         /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY recov.get_position
 :******************************************************************
*/

get_position(v, Stop, lg)
int v, Stop[], lg;
{
	int low, high, mid;

	low = 0;
	high = lg - 1;
	while (high != low - 1) {
		mid = (low + high) / 2;
		if (Stop[mid] == v)
			return (mid);
		else if (Stop[mid] < v)
			low = mid + 1;
		else /* Stop[mid] > v) */
			high = mid - 1;
	}
	return (low);
}
