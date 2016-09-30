/*
 :******************************************************************
 :* MODULE-DESCRIPTION
 :*
 :* TITLE         (/ Parsing a rule heading   /)
 :*
 :* NAME             rule_head.c
 :*
 :* COPYRIGHT     (C)   SIEMENS NIXDORF INFORMATIONSSYSTEME AG  1992
 :*                     ALL RIGHTS RESERVED
 :*
 :* MEMORY-CLASS  6
 :*
 :* PROG-CONVENTS (/ <internal conventions (instance, names,
 :*                  external data, ...). None -> empty text>     /)
 :*
 :* PROCEDURE     merge
 :* REMARKS       (/ type validation /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     shift
 :* REMARKS       (/ type validation /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     InTypes
 :* REMARKS       (/ type validation /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     TypeName
 :* REMARKS       (/ parsing a type name    /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     VariableGroup
 :* REMARKS       (/ parsing a group of variables           /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     VariableDef
 :* REMARKS       (/ parsing a group of variables           /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     VarDefPart1
 :* REMARKS       (/ parsing a variable declaration /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     VarDefPart
 :* REMARKS       (/ parsing an optional variable declaration /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     ParameterGroup
 :* REMARKS       (/ parsing a group of parameters    /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     FormalParList
 :* REMARKS       (/ parsing a list of group of parameters   /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     ParameterList
 :* REMARKS       (/ parsing a list of group of parameters /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     ForParamPart
 :* REMARKS       (/ parsing an optional list of group of parameters  /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     RuleHeading
 :* REMARKS       (/ parsing a rule heading /)
 :* ENTRY         EXTERNAL
 :*
 :*-----------------------------------------------------------------
 :*
 :* VERSION       001
 :*   CRDATE      1992-03-09
 :*   AUTHOR      (/ Mounji Abdelaziz       , FUNDP /)
 :*   UPDATE      (/ original issue /)
 :*
 :* END-MODULE-DESCRIPTION rule_head.c
 :******************************************************************
*/
#include <stdio.h>
#include "asax.h"

#define NbreTypes 2
static char *Types[NbreTypes] = { "integer", "string" };

merge(a, lga, b, lgb, abmerge)
int a[], lga, b[], lgb, abmerge[];
{
	int  i, j, k;

	i=j=k= 0;
	while ( i < lga  && j < lgb) {
		if (a[i] == b[j]) {
			abmerge[k++] = a[i++];
			j++;
		} else if (a[i] < b[j])
			abmerge[k++] = a[i++];
		  else abmerge[k++] = b[j++];
	}
	if (i == lga)
		    while (j < lgb)
			abmerge[k++] = b[j++];
	else /* j == lgb */
		    while (i < lga)
			abmerge[k++] = a[i++];
	return (k); /* length of the resulting table */
}
shift(i, Stop, lg)
int i, Stop[], *lg;
{
	int k;

	for (k= (*lg) - 1; k>= i; k--)
		Stop[k+1] = Stop[k];
	(*lg)++;
}
/*
 :******************************************************************
 :* UNIT-BODY rule_head.InTypes
 :*
 :* TITLE         (/ type validation   /)
 :*
 :* PURPOSE       (/ search for the given type name in the table of type
 :*                  names , return its index if found -1 otherwise /)
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
 :*   DATA       (/
 :*                       - 'Types[]'  : Table of type names
 :*                       - NbreTypes  : length of 'Types[]'
 :*                                                                /)
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
 :*   Pre    (/ - s is a string
 :*                                                            /)
 :*                                                             END-Pre
 :*
 :*   Post   (/ - if there exist a i (0<=i<NbreTypes) such that the string
 :*               Types[i] is equal to s return i otherwise return -1
 :*                                                                   /)
 :*                                                             END-Post
 :*
 :*   RETURNS  "int" (/ if s is in 'Types[]' the returned value is the index
 :*                     of s in 'Types[]' otherwise, the returned value is -1
 :*                                                               /)
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY rule_head.InTypes
 :******************************************************************
*/
InTypes(s)
char s[];
{
	int low, high, mid, k;

	low = 0;
	high = NbreTypes - 1 ;
	while (low <= high) {
		mid = (low + high) / 2;
		if ((k = strcmp(s, Types[mid])) < 0)
			high = mid - 1;
		else if (k > 0)
			low = mid + 1;
		else /* s is a special symbol */
			return (mid);
	}
	return (-1);
}
/*
 :******************************************************************
 :* UNIT-BODY rule_head.TypeName
 :*
 :* TITLE         (/ parsing a type name                   /)
 :*
 :* PURPOSE       (/ parse a symbol representing a type name; initialize
 :*                  'tdescr[]' to contain this type if no error is found or
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
 :*                   - trdescr[]     : rule descriptors table
 :*                   - tdescr[]      : names (par; or var) descriptors table
 :*                   - targ_types[]  : argument types table
 :*                   - rule_name     : name of the rule this object belongs
 :*                                     to
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
 :*             - 'pv' is in {par, var} a type for a parameter or a variable
 :*             - 'rank'>=0
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/   if the CSS is of the form:
 :*                      <type_name><rest>
 :*               where
 :*                  <type_name> is a type name
 :*                  <rest> is any character sequence
 :*               the effect is the following:
 :*                - the table of parameter and variable descriptors 'tdescr[]'
 :*                  is updated and any type mismatch is reported (see file
 :*                  rdescr.c function put_type() for extensive details about
 :*                  this update)
 :*                - the CSS is <rest>
 :*               otherwise
 :*                  - the CSS is of the form:
 :*                         <first_Stop><rest>
 :*                    where <first_Stop> is the first symbol in
 :*                                 s1 s2 ... sn
 :*                          such that <first_Stop> is in 'Stop[]'
 :*                  - one or more error messages are output to stdout
 :*                  - 'ErrorFound' is set to TRUE
 :*                                                                 /)
 :*
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
 :* END-BODY rule_head.TypeName
 :******************************************************************
*/
TypeName(Stop, lg, pv, rank)
int Stop[], lg, pv, rank;
{
	int k;

	if ((k = InTypes(symb.bsymbval.svalue)) >= 0) {
			put_type(k, pv, rank);
	} else {
		error(semantics, 1);
	}
	Expect(symb.ibsymb, Stop, lg);
}
/*
 :******************************************************************
 :* UNIT-BODY rule_head.VariableGroup
 :*
 :* TITLE         (/ parsing a group of variables                  /)
 :*
 :* PURPOSE       (/ parse a sequence of symbols representing the declaration
 :*                  of a group of variables or parameters; update 'tdescr[]'
 :*                  to contain descriptors for these names if no error is
 :*                  found or recover from this error otherwise /)
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
 :*                   - trdescr[]     : rule descriptors table
 :*                   - tdescr[]      : names (par; or var) descriptors table
 :*                   - targ_types[]  : argument types table
 :*                   - rule_name     : name of the rule this object belongs
 :*                                     to
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
 :*             - 'pv' is in {par, var} a type for a parameter or a variable
 :*             - 'rank'>=0
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/   if the CSS is of the form:
 :*                   <name_1> "," ... "," <name_N> ":" <type_name><rest>
 :*               where
 :*                  - N>=1
 :*                  - <name_i> is an identifier (parameter or a variable)
 :*                  -          (i=1, ..., N)
 :*                  - <type_name> is a valid type name (integer or string)
 :*                  - <rest> is any character sequence
 :*               the effect is the following:
 :*                - if 'pv' = var <name_i> is considered as a variable name
 :*                - if 'pv' = par <name_i> is considered as a parameter name
 :*                - the table of parameter and variable descriptors 'tdescr[]'
 :*                  is updated (see functions add_name() and put_type() in
 :*                  file rdescr.c for extensive details about this update)
 :*                - the CSS is <rest>
 :*               otherwise
 :*                  - the CSS is of the form:
 :*                         <first_Stop><rest>
 :*                    where <first_Stop> is the first symbol in
 :*                                 s1 s2 ... sn
 :*                          such that <first_Stop> is in 'Stop[]'
 :*                  - one or more error messages are output to stdout
 :*                  - 'ErrorFound' is set to TRUE
 :*                                                                 /)
 :*
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
 :* END-BODY rule_head.VariableGroup
 :******************************************************************
*/
VariableGroup(Stop, lg, pv, rank)
int lg, Stop[], pv, *rank;
{
	int lg1,lg2, S1[Mlg], S2[Mlg];
	char name[MAXS];

	insert_stop(colon, Stop, lg, S1, &lg1);
	insert_stop(comma, S1, lg1, S2, &lg2);
	ExpectName(name, S2, lg2);
	(*rank)++;
	add_name(name, pv);
	if (symb.ibsymb == comma) {
		insert_stop(identifierx, Stop, lg, S1, &lg1);
		Expect(comma, S1, lg1);
		VariableGroup(Stop, lg, pv, rank);
	} else {
		insert_stop(integerx, Stop, lg, S1, &lg1);
		insert_stop(stringx, S1, lg1, S2, &lg2);
		Expect(colon, S2, lg2);
		TypeName(Stop, lg, pv, *rank-1);
	  }
}
/*
 :******************************************************************
 :* UNIT-BODY rule_head.VariableDef
 :*
 :* TITLE         (/ parsing a group of variables                  /)
 :*
 :* PURPOSE       (/ parse a sequence of symbols representing the declaration
 :*                  of a group of variables or parameters; update 'tdescr[]'
 :*                  to contain descriptors for these names if no error is
 :*                  found or recover from this error otherwise /)
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
 :*                   - trdescr[]     : rule descriptors table
 :*                   - tdescr[]      : names (par; or var) descriptors table
 :*                   - targ_types[]  : argument types table
 :*                   - rule_name     : name of the rule this object belongs
 :*                                     to
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
 :*             - 'rank'>=0
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/   if the CSS is of the form:
 :*                   <name_1> "," ... "," <name_N> ":" <type_name>";"<rest>
 :*               where
 :*                  - N>=1
 :*                  - <name_i> is an identifier (parameter or a variable)
 :*                  -          (i=1, ..., N)
 :*                  - <type_name> is a valid type name (integer or string)
 :*                  - <rest> is any character sequence
 :*               the effect is the following:
 :*                - if 'pv' = var <name_i> is considered as a variable name
 :*                - if 'pv' = par <name_i> is considered as a parameter name
 :*                - the table of parameter and variable descriptors 'tdescr[]'
 :*                  is updated (see functions add_name() and put_type() in
 :*                  file rdescr.c for extensive details about this update)
 :*                - the CSS is <rest>
 :*               otherwise
 :*                  - the CSS is of the form:
 :*                         <first_Stop><rest>
 :*                    where <first_Stop> is the first symbol in
 :*                                 s1 s2 ... sn
 :*                          such that <first_Stop> is in 'Stop[]'
 :*                  - one or more error messages are output to stdout
 :*                  - 'ErrorFound' is set to TRUE
 :*                                                                 /)
 :*
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
 :* END-BODY rule_head.VariableDef
 :******************************************************************
*/
VariableDef(Stop, lg, rank)
int Stop[], lg, *rank;
{
	int S1[Mlg], lg1;

	insert_stop(semicolon, Stop, lg, S1, &lg1);
	VariableGroup(S1, lg1, var, rank);
	Expect(semicolon, Stop, lg);
}
/*
 :******************************************************************
 :* UNIT-BODY rule_head.VarDefPart1
 :*
 :* TITLE         (/ parsing a variable declaration                  /)
 :*
 :* PURPOSE       (/ parse a sequence of symbols representing a declaration
 :*                  of variables; update 'tdescr[]' to contain descriptors
 :*                  for these variables if no error is found or recover from
 :*                  this error otherwise /)
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
 :*                   - trdescr[]     : rule descriptors table
 :*                   - tdescr[]      : names (par; or var) descriptors table
 :*                   - targ_types[]  : argument types table
 :*                   - rule_name     : name of the rule this object belongs
 :*                                     to
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
 :*             - 'rank'>=0
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/   if the CSS is of the form:
 :*                   "var" <VariableDef_1> ... <VariableDef_N> <rest>
 :*               where
 :*                  - N>=1
 :*                  - <VariableDef_i> is a sequence of symbols of the form:
 :*                       <name_1> "," ... "," <name_n> ":" <type_name_i>";"
 :*                    where
 :*                       - n>=1
 :*                       - <name_j> is a variable name  (j=1, ..., n)
 :*                       - <type_name_i> is a valid type name (integer or
 :*                                       string)        (i=1, ..., N)
 :*                  - <rest> is any character sequence
 :*               the effect is the following:
 :*                - the table of parameter and variable descriptors 'tdescr[]'
 :*                  is updated (see functions add_name() and put_type() in
 :*                  file rdescr.c for extensive details about this update)
 :*                - the CSS is <rest>
 :*               otherwise
 :*                  - the CSS is of the form:
 :*                         <first_Stop><rest>
 :*                    where <first_Stop> is the first symbol in
 :*                                 s1 s2 ... sn
 :*                          such that <first_Stop> is in 'Stop[]'
 :*                  - one or more error messages are output to stdout
 :*                  - 'ErrorFound' is set to TRUE
 :*                                                                 /)
 :*
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
 :* END-BODY rule_head.VarDefPart1
 :******************************************************************
*/
VarDefPart1(Stop, lg, rank)
int Stop[], lg, *rank;
{
	int S1[Mlg], lg1;

	insert_stop(identifierx, Stop, lg, S1, &lg1);
	Expect(varx, S1, lg1);
	VariableDef(S1, lg1, rank);
	while (symb.ibsymb == identifierx) {
		VariableDef(S1, lg1, rank);

	}
}
/*
 :******************************************************************
 :* UNIT-BODY rule_head.VarDefPart
 :*
 :* TITLE         (/ parsing an optional variable declaration  /)
 :*
 :* PURPOSE       (/ parse a sequence of symbols representing a possibly
 :*                  empty declaration of variables; update 'tdescr[]' to
 :*                  contain descriptors for these variables if no error is
 :*                  found or recover from this error otherwise /)
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
 :*                   - trdescr[]     : rule descriptors table
 :*                   - tdescr[]      : names (par; or var) descriptors table
 :*                   - targ_types[]  : argument types table
 :*                   - rule_name     : name of the rule this object belongs
 :*                                     to
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
 :*             - 'rank'>=0
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/   if the CSS is of the form:
 :*                   "var" <VariableDef_1> ... <VariableDef_N> <rest>
 :*               where
 :*                  - N>=1
 :*                  - <VariableDef_i> is a sequence of symbols of the form:
 :*                       <name_1> "," ... "," <name_n> ":" <type_name_i>";"
 :*                    where
 :*                       - n>=1
 :*                       - <name_j> is a variable name  (j=1, ..., n)
 :*                       - <type_name_i> is a valid type name (integer or
 :*                                       string)        (i=1, ..., N)
 :*                  - <rest> is any character sequence
 :*               the effect is the following:
 :*                - the table of parameter and variable descriptors 'tdescr[]'
 :*                  is updated (see functions add_name() and put_type() in
 :*                  file rdescr.c for extensive details about this update)
 :*                - the CSS is <rest>
 :*               otherwise nothing is done
 :*                                                                 /)
 :*
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
 :* END-BODY rule_head.VarDefPart
 :******************************************************************
*/
VarDefPart(Stop, lg, rank)
int Stop[], lg, *rank;
{
	int S1[Mlg], lg1;

	insert_stop(varx, Stop, lg, S1, &lg1);
	SyntaxCheck(S1, lg1);
	if (symb.ibsymb == varx)
		VarDefPart1(Stop, lg, rank);
}
/*
 :******************************************************************
 :* UNIT-BODY rule_head.ParameterGroup
 :*
 :* TITLE         (/ parsing a group of parameters                 /)
 :*
 :* PURPOSE       (/ parse a sequence of symbols representing the declaration
 :*                  of a group of parameters; update 'tdescr[]'
 :*                  to contain descriptors for these parameters if no error
 :*                  is found or recover from this error otherwise /)
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
 :*                   - trdescr[]     : rule descriptors table
 :*                   - tdescr[]      : names (par; or var) descriptors table
 :*                   - targ_types[]  : argument types table
 :*                   - rule_name     : name of the rule this object belongs
 :*                                     to
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
 :*             - 'rank'>=0
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/   if the CSS is of the form:
 :*                   <name_1> "," ... "," <name_N> ":" <type_name><rest>
 :*               where
 :*                  - N>=1
 :*                  - <name_i> is a parameter name  (i=1, ..., N)
 :*                  - <type_name> is a valid type name (integer or string)
 :*                  - <rest> is any character sequence
 :*               the effect is the following:
 :*                - the table of parameter and variable descriptors 'tdescr[]'
 :*                  is updated (see functions add_name() and put_type() in
 :*                  file rdescr.c for extensive details about this update)
 :*                - the CSS is <rest>
 :*               otherwise
 :*                  - the CSS is of the form:
 :*                         <first_Stop><rest>
 :*                    where <first_Stop> is the first symbol in
 :*                                 s1 s2 ... sn
 :*                          such that <first_Stop> is in 'Stop[]'
 :*                  - one or more error messages are output to stdout
 :*                  - 'ErrorFound' is set to TRUE
 :*                                                                 /)
 :*
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
 :* END-BODY rule_head.ParameterGroup
 :******************************************************************
*/
ParameterGroup(Stop, lg, rank)
int Stop[], lg, *rank;
{
	VariableGroup(Stop, lg, par, rank);
}
/*
 :******************************************************************
 :* UNIT-BODY rule_head.FormalParList
 :*
 :* TITLE         (/ parsing a list of group of parameters   /)
 :*
 :* PURPOSE       (/ parse a sequence of symbols representing a declaration
 :*                  of groups of parameters; update 'tdescr[]' to contain
 :*                  descriptors for these parameters if no error
 :*                  is found or recover from this error otherwise /)
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
 :*                   - trdescr[]     : rule descriptors table
 :*                   - tdescr[]      : names (par; or var) descriptors table
 :*                   - targ_types[]  : argument types table
 :*                   - rule_name     : name of the rule this object belongs
 :*                                     to
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
 :*             - 'rank'>=0
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/   if the CSS is of the form:
 :*                   <ParameterGroup_1>";" ... ";"<ParameterGroup_N><rest>
 :*               where
 :*                 - N>=1
 :*                 - <ParameterGroup_i> is a sequence of symbols of the form:
 :*                       <name_1> "," ... "," <name_n> ":" <type_name_i>
 :*                    where
 :*                       - n>=1
 :*                       - <name_j> is a parameter name  (j=1, ..., n)
 :*                       - <type_name_i> is a valid type name (integer or
 :*                                       string)        (i=1, ..., N)
 :*                 - <rest> is any character sequence
 :*               the effect is the following:
 :*                - the table of parameter and variable descriptors 'tdescr[]'
 :*                  is updated (see functions add_name() and put_type() in
 :*                  file rdescr.c for extensive details about this update)
 :*                - the CSS is <rest>
 :*               otherwise
 :*                  - the CSS is of the form:
 :*                         <first_Stop><rest>
 :*                    where <first_Stop> is the first symbol in
 :*                                 s1 s2 ... sn
 :*                          such that <first_Stop> is in 'Stop[]'
 :*                  - one or more error messages are output to stdout
 :*                  - 'ErrorFound' is set to TRUE
 :*                                                                 /)
 :*
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
 :* END-BODY rule_head.FormalParList
 :******************************************************************
*/
FormalParList(Stop, lg, rank)
int Stop[], lg, *rank;
{
	int S1[Mlg], lg1, S2[Mlg], lg2, rule_addr;
	rdescr *rdescrTab;

	insert_stop(semicolon, Stop, lg, S1, &lg1);
	ParameterGroup(S1, lg1, rank);
	while (symb.ibsymb == semicolon) {
		lg2 = merge(ParStop, lgParStop, S1, lg1, S2);
		Expect(semicolon, S2, lg2);
		ParameterGroup(S1, lg1, rank);
	}
	rule_addr = getRuleAddr(rule_name, &rdescrTab);
	if (rdescrTab[rule_addr].status)
	    if (*rank != rdescrTab[rule_addr].ParNre) {
		error(semantics, 8);        /* check arity */
	    }
}
/*
 :******************************************************************
 :* UNIT-BODY rule_head.ParameterList
 :*
 :* TITLE         (/ parsing a list of group of parameters   /)
 :*
 :* PURPOSE       (/ parse a sequence of symbols representing a declaration
 :*                  of groups of parameters; update 'tdescr[]' to contain
 :*                  descriptors for these parameters if no error
 :*                  is found or recover from this error otherwise /)
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
 :*                   - trdescr[]     : rule descriptors table
 :*                   - tdescr[]      : names (par; or var) descriptors table
 :*                   - targ_types[]  : argument types table
 :*                   - rule_name     : name of the rule this object belongs
 :*                                     to
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
 :*             - 'rank'>=0
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/   if the CSS is of the form:
 :*               "("<ParameterGroup_1>";" ... ";"<ParameterGroup_N>")"<rest>
 :*               where
 :*                 - N>=1
 :*                 - <ParameterGroup_i> is a sequence of symbols of the form:
 :*                       <name_1> "," ... "," <name_n> ":" <type_name_i>
 :*                    where
 :*                       - n>=1
 :*                       - <name_j> is a parameter name  (j=1, ..., n)
 :*                       - <type_name_i> is a valid type name (integer or
 :*                                       string)        (i=1, ..., N)
 :*                 - <rest> is any character sequence
 :*               the effect is the following:
 :*                - the table of parameter and variable descriptors 'tdescr[]'
 :*                  is updated (see functions add_name() and put_type() in
 :*                  file rdescr.c for extensive details about this update)
 :*                - the CSS is <rest>
 :*               otherwise
 :*                  - the CSS is of the form:
 :*                         <first_Stop><rest>
 :*                    where <first_Stop> is the first symbol in
 :*                                 s1 s2 ... sn
 :*                          such that <first_Stop> is in 'Stop[]'
 :*                  - one or more error messages are output to stdout
 :*                  - 'ErrorFound' is set to TRUE
 :*                                                                 /)
 :*
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
 :* END-BODY rule_head.ParameterList
 :******************************************************************
*/
ParameterList(Stop, lg, rank)
int Stop[], lg, *rank;
{
	int S1[Mlg], lg1, S2[Mlg], lg2;

	lg1 = merge(ParStop, lgParStop, Stop, lg, S1);
	insert_stop(rparenthesis, S1, lg1, S2, &lg2);
	Expect(lparenthesis, S2, lg2);
	insert_stop(rparenthesis, Stop, lg, S1, &lg1);
	FormalParList(S1, lg1, rank);
	Expect(rparenthesis, Stop, lg);
}
/*
 :******************************************************************
 :* UNIT-BODY rule_head.ForParamPart
 :*
 :* TITLE         (/ parsing an optional list of group of parameters   /)
 :*
 :* PURPOSE       (/ parse a sequence of symbols representing a possibly
 :*                  empty declaration of groups of parameters; update
 :*                  'tdescr[]' to contain descriptors for these parameters
 :*                  if no error is found or recover from this
 :*                  error otherwise /)
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
 :*                   - trdescr[]     : rule descriptors table
 :*                   - tdescr[]      : names (par; or var) descriptors table
 :*                   - targ_types[]  : argument types table
 :*                   - rule_name     : name of the rule this object belongs
 :*                                     to
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
 :*             - 'rank'>=0
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/   if the CSS is of the form:
 :*               "("<ParameterGroup_1>";" ... ";"<ParameterGroup_N>")"<rest>
 :*               where
 :*                 - N>=1
 :*                 - <ParameterGroup_i> is a sequence of symbols of the form:
 :*                       <name_1> "," ... "," <name_n> ":" <type_name_i>
 :*                    where
 :*                       - n>=1
 :*                       - <name_j> is a parameter name  (j=1, ..., n)
 :*                       - <type_name_i> is a valid type name (integer or
 :*                                       string)        (i=1, ..., N)
 :*                 - <rest> is any character sequence
 :*               the effect is the following:
 :*                - the table of parameter and variable descriptors 'tdescr[]'
 :*                  is updated (see functions add_name() and put_type() in
 :*                  file rdescr.c for extensive details about this update)
 :*                - the CSS is <rest>
 :*               otherwise nothing is done
 :*                                                                 /)
 :*
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
 :* END-BODY rule_head.ForParamPart
 :******************************************************************
*/
ForParamPart(Stop, lg, rank)
int Stop[], lg, *rank;
{
	int S1[Mlg], lg1;

	insert_stop(lparenthesis, Stop, lg, S1, &lg1);
	SyntaxCheck(S1, lg1);
	if (symb.ibsymb == lparenthesis)
		ParameterList(Stop, lg, rank);
}
/*
 :******************************************************************
 :* UNIT-BODY rule_head.RuleHeading
 :*
 :* TITLE         (/ parsing a rule heading /)
 :*
 :* PURPOSE       (/ parse a sequence of symbols representing a rule heading;
 :*                  update 'tdescr[]' to contain descriptors for this
 :*                  declaration if no error is found or recover from this
 :*                  error otherwise /)
 :*                                                      END-PURPOSE
 :* VERSION       001
 :*   CRDATE      1992-12-07
 :*   AUTHOR      (/ Mounji Abdelaziz      , FUNDP              /)
 :*   UPDATE      (/ original issue /)
 :*
 :* REALIZATION   (/ <note, reference to other proc. (optional)> /)
 :*
 :*   CONTROL    (/
 :*
 :*                                                               /)
 :*   DATA       (/
 :*                   - trdescr[]     : rule descriptors table
 :*                   - tdescr[]      : names (par; or var) descriptors table
 :*                   - targ_types[]  : argument types table
 :*                   - rule_name     : name of the rule this object belongs
 :*                                     to
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
 :*             - 'rank'>=0
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/   if the CSS is of the form:
 :*                 <rule_heading> <rest>
 :*               where
 :*                 - <rule_heading> is a valid rule heading (see BNF syntax
 :*                                  of rule heading in DOCUMENTS)
 :*                 - <rest> is any character sequence
 :*               the effect is the following:
 :*                - the table of rule descriptors 'trdescr[]' is added a
 :*                  new rule descriptor corresponding to this rule
 :*                - the table of parameter and variable descriptors 'tdescr[]'
 :*                  is updated (see functions add_name() and put_type() in
 :*                  file rdescr.c for extensive details about this update)
 :*                - the CSS is <rest>
 :*               otherwise
 :*                  - the CSS is of the form:
 :*                         <first_Stop><rest>
 :*                    where <first_Stop> is the first symbol in
 :*                                 s1 s2 ... sn
 :*                          such that <first_Stop> is in 'Stop[]'
 :*                  - one or more error messages are output to stdout
 :*                  - 'ErrorFound' is set to TRUE
 :*                                                                 /)
 :*
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
 :* END-BODY rule_head.RuleHeading
 :******************************************************************
*/
RuleHeading(Stop, lg)
int Stop[], lg;
{
    int S0[Mlg], lg0, S1[Mlg], lg1, S2[Mlg], lg2, S3[Mlg], lg3, S4[Mlg], lg4;
    
    int rank = 0;

	lg4 = merge(VariableStop, lgVarStop, Stop, lg, S4);
	insert_stop(semicolon, S4, lg4, S3, &lg3);
	lg2 = merge(ParameterStop, lgParamStop, S3, lg3, S2);
	switch (symb.ibsymb) {
	    case rulex:
			       insert_stop(identifierx, S2, lg2, S1, &lg1);
			       Expect(rulex, S1, lg1);
			       ExpectName(rule_name, S2, lg2);
			       add_rule_decl(internal, rule_name);
			       break;

	    case internalx:
			       insert_stop(rulex, S2, lg2, S0, &lg0);
			       insert_stop(identifierx, S0, lg0, S1, &lg1);
			       Expect(internalx, S0, lg0);
			       Expect(rulex, S1, lg1);
			       ExpectName(rule_name, S2, lg2);
			       add_rule_decl(internal, rule_name);
			       break;

	    case externalx:
			       insert_stop(rulex, S2, lg2, S0, &lg0);
			       insert_stop(identifierx, S0, lg0, S1, &lg1);
			       Expect(externalx, S0, lg0);
			       Expect(rulex, S1, lg1);
			       ExpectName(rule_name, S2, lg2);
			       add_rule_decl(external, rule_name);
			       break;

	    case init_actionx:
			       strcpy(rule_name, symb.bsymbval.svalue);
			       Expect(init_actionx, S2, lg2);
			       add_rule_decl(internal, rule_name);
			       break;

	    default          : error(syntax, 3);
	}
	ForParamPart(S3, lg3, &rank);
	Expect(semicolon, S4, lg4);
	VarDefPart(Stop, lg, &rank);
}

