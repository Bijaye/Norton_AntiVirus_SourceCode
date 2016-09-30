/*
 :******************************************************************
 :* MODULE-DESCRIPTION
 :*
 :* TITLE         (/ Parsing a /)
 :*
 :* NAME             usage.c
 :*
 :* COPYRIGHT     (C)   SIEMENS NIXDORF INFORMATIONSSYSTEME AG  1992
 :*                     ALL RIGHTS RESERVED
 :*
 :* MEMORY-CLASS  6
 :*
 :* PROG-CONVENTS (/ <internal conventions (instance, names,
 :*                  external data, ...). None -> empty text>     /)
 :*
 :* PROCEDURE     usage
 :* REMARKS       (/ parsing of a list of module usage group /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     module_usage
 :* REMARKS       (/ parsing of a module usage group /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     add_module
 :* REMARKS       (/ add a module descriptor /)
 :* ENTRY         EXTERNAL
 :*
 :*-----------------------------------------------------------------
 :*
 :* VERSION       001
 :*   CRDATE      1992-03-09
 :*   AUTHOR      (/ Mounji Abdelaziz       , FUNDP /)
 :*   UPDATE      (/ original issue /)
 :*
 :* END-MODULE-DESCRIPTION usage.c
 :******************************************************************
*/
#include <stdio.h>             /* standard IO definition */
#include "asax.h"            /* declares all shared data types */

/*
 :******************************************************************
 :* UNIT-BODY usage.usage
 :*
 :* TITLE         (/ parsing a list of a module usage group /)
 :*
 :* PURPOSE       (/ parse a sequence of symbols representing a declaration
 :*                  of a list of module usage group; initialize
 :*                  descriptors for these module usages if no error
 :*                  is found or recover from this error otherwise /)
 :*                                                      END-PURPOSE
 :* VERSION       001
 :*   CRDATE      1993-07-03
 :*   AUTHOR      (/ Mounji Abdelaziz       , FUNDP              /)
 :*   UPDATE      (/ original issue /)
 :*
 :* REALIZATION   (/ <note, reference to other proc. (optional)> /)
 :*
 :*   CONTROL    (/
 :*
 :*                                                               /)
 :*   DATA       (/
 :*                   - tModDescr[]   : table of module descriptors
 :*                   - tModDescr_lg  : length of tModDescr[]
 :*                   - MaxModDescr   : maximum length of tModDescr[]
 :*                                                                 /)
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
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/   if the CSS is of the form:
 :*                   <ModuleGroup_1>";" ... ";"<ModulesGroup_N><rest>
 :*               where
 :*                 - N>=1
 :*                 - <ModulesGroup_i> is a sequence of symbols of the form:
 :*                  "uses" <name_i_1> "," ... "," <name_i_ni> ";"
 :*                    where
 :*                       - 1<=i<=N
 :*                       - ni>=1
 :*                       - <name_i_j> is a module name  (j=1, ..., ni)
 :*                 - <rest> is any character sequence
 :*               the effect is the following:
 :*                - the table of module descriptors 'tModDescr[]' is updated
 :*                  as follows:
 :*                    - let {mod_1, ..., mod_m} be a subset of
 :*                          {name_1_1, ... name_N_Nn} so that
 :*                        mod_i != mod_j for all i, j (0<=i<m, 0<=j<m, i!=j)
 :*                    - for all i: (0<=i<=m) we have:
 :*                          tModDescr[i].module_name = mod_i
 :*                          tModDescr[i].intVarNbre = 0
 :*                          tModDescr[i].ruleDeclNbre = 0
 :*                          tModDescr[i].VarPtr = 0
 :*                          tModDescr[i].RulePtr = 0
 :*                - tModdescr_lg = m;
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
 :* END-BODY usage.usage
 :******************************************************************
*/
usage(Stop, lg) /* <usage> ::= <module_usage> { ";" <module_usage> }  */
int Stop[], lg;
{
	int S1[Mlg], lg1, S2[Mlg], lg2;

	insert_stop(semicolon, Stop, lg, S1, &lg1);
	insert_stop(usesx, S1, lg1, S2, &lg2);
	while (symb.ibsymb == usesx) { /* loop into module usages */
	    module_usage(S1, lg1);     /* parse a group of mdule usage */
	    Expect(semicolon, S2, lg2);
	}
}

/*
 :******************************************************************
 :* UNIT-BODY usage.module_usage
 :*
 :* TITLE         (/ parsing of a module usage group /)
 :*
 :* PURPOSE       (/ parse a sequence of symbols representing a declaration
 :*                  of module usage group; initialize
 :*                  descriptors for these module usages if no error
 :*                  is found or recover from this error otherwise /)
 :*                                                      END-PURPOSE
 :* VERSION       001
 :*   CRDATE      1993-07-03
 :*   AUTHOR      (/ Mounji Abdelaziz       , FUNDP              /)
 :*   UPDATE      (/ original issue /)
 :*
 :* REALIZATION   (/ <note, reference to other proc. (optional)> /)
 :*
 :*   CONTROL    (/
 :*
 :*                                                               /)
 :*   DATA       (/
 :*                   - tModDescr[]   : table of module descriptors
 :*                   - tModDescr_lg  : length of tModDescr[]
 :*                   - MaxModDescr   : maximum length of tModDescr[]
 :*                                                                 /)
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
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/   if the CSS is of the form:
 :*                 - "uses" <module_name_1> "," ... "," <module_name_N> ";"
 :*                    where
 :*                       - 0<=N
 :*                       - <module_name_i> is a module name (j=1, ..., N)
 :*                 - <rest> is any character sequence
 :*               the effect is the following:
 :*                - the table of module descriptors 'tModDescr[]' is updated
 :*                  as follows:
 :*                    - let {mod_1, ..., mod_m} be a subset of
 :*                          {module_name_1, ... module_name_N} so that
 :*                        mod_i != mod_j for all i, j (0<=i<m, 0<=j<m, i!=j)
 :*                    - for all i: (0<=i<=m) we have:
 :*                          tModDescr[i].module_name = mod_i
 :*                          tModDescr[i].intVarNbre = 0
 :*                          tModDescr[i].ruleDeclNbre = 0
 :*                          tModDescr[i].VarPtr = 0
 :*                          tModDescr[i].RulePtr = 0
 :*                - tModdescr_lg = m;
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
 :* END-BODY usage.module_usage
 :******************************************************************
*/

/* module_usage ::= uses <module_name> { "," <module_name> }              */

module_usage(Stop, lg)
int Stop[], lg;
{
    int S1[Mlg], lg1, S2[Mlg], lg2; /* auxiliary stops   */
                     /* memory allocation */
    char *name;                     /* module name       */

    if ((name = (char *) malloc(ident_lg)) == NULL) {
	error(fatal, 25);   /* out of memory */
    }
    insert_stop(comma, Stop, lg, S2, &lg2);
    insert_stop(identifierx, S2, lg2, S1, &lg1);
    Expect(usesx, S1, lg1);
    ExpectName(name, S2, lg2);
    add_module(name);
    while(symb.ibsymb == comma) {
	Expect(comma, S1, lg1);
	ExpectName(name, S2, lg2);
	add_module(name);
    }
    free(name);        /* release memory area */
}
/*
 :******************************************************************
 :* UNIT-BODY usage.add_module
 :*
 :* TITLE         (/ add a module descriptor/)
 :*
 :* PURPOSE       (/ add a module descriptor to the table 'tModDescr[]'
 :*                                                                   /)
 :*                                                      END-PURPOSE
 :* VERSION       001
 :*   CRDATE      1993-07-03
 :*   AUTHOR      (/ Mounji Abdelaziz       , FUNDP              /)
 :*   UPDATE      (/ original issue /)
 :*
 :* REALIZATION   (/ <note, reference to other proc. (optional)> /)
 :*
 :*   CONTROL    (/
 :*
 :*                                                               /)
 :*   DATA       (/
 :*                   - tModDescr[]   : table of module descriptors
 :*                   - tModDescr_lg  : length of tModDescr[]
 :*                   - MaxModDescr   : maximum length of tModDescr[]
 :*                                                                 /)
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
 :*   Pre    (/ - 'name' a module name
 :*             - 'tModDescr_lg' = lg0
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/   if there exists a i (0<=i<lg0) and:
 :*                     tModDescr[i].module_name = name
 :*                  nothing is changed otherwise:
 :*               if (lg0 >= MaxModDescr) an error message is sent to stdout
 :*               otherwise:
 :*                  - tModDescr[lg0].module_name = name
 :*                  - tModDescr[lg0].RulePtr = 0
 :*                  - tModDescr[lg0].VarPtr = 0
 :*                  - tModDescr[lg0].intVarNbre = 0
 :*                  - tModDescr[lg0].ruleNbre = 0
 :*                  - tModdescr_lg = lg0 + 1;
 :*                                                                 /)
 :*
 :*                                                             END-Post
 :*   RETURNS  NONE
 :*
 :*   REMARKS     (/
 :*                                                         /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY usage.add_module
 :******************************************************************
*/
add_module(name) /* adds a new entry in the module descriptor table */
char *name;
{
    int i;     /* loop counter */
    int found; /* = TRUE if name is in name_0, ..., name_i-1 */

    i=0;
    found = FALSE;
    while (!found && i < tModDescr_lg) {
	if (strcmp(tModDescr[i++].module_name, name) == 0)
	    found = TRUE;
    }
    if (!found) {
	if (tModDescr_lg < MaxModDescr) {/* no overflow of tModDescr table */
	    strcpy(tModDescr[tModDescr_lg].module_name, name);
	    tModDescr[tModDescr_lg].intVarNbre = 0;
	    tModDescr[tModDescr_lg].ruleDeclNbre = 0;
	    tModDescr_lg++;
	} else {
	   error(fatal, 29); /* too much module inclusions */
	}
    }
}
