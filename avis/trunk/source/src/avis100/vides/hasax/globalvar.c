/*
 :******************************************************************
 :* MODULE-DESCRIPTION
 :*
 :* TITLE         (/ Parsing a group of global variable declaration /)
 :*
 :* NAME             globalVar.c
 :*
 :* COPYRIGHT     (C)   SIEMENS NIXDORF INFORMATIONSSYSTEME AG  1992
 :*                     ALL RIGHTS RESERVED
 :*
 :* MEMORY-CLASS  6
 :*
 :* PROG-CONVENTS (/ <internal conventions (instance, names,
 :*                  external data, ...). None -> empty text>     /)
 :*
 :* PROCEDURE     GlTypeName
 :* REMARKS       (/ parsing a global variable type name    /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     globalVarGr
 :* REMARKS       (/ parsing a group of global variables           /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     addGlobalVar
 :* REMARKS       (/ add a global variable descriptor              /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     globalVariables
 :* REMARKS       (/ parsing a list of groups of global variable
 :*                  declarations                                  /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     globalVariableGroup
 :* REMARKS       (/ parsing a group of global variable declarations /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     put_Gl_types
 :* REMARKS       (/ install the type of a group of global variables /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     checkGroupTypes
 :* REMARKS       (/ check the type of a group of global variables /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     look_up
 :* REMARKS       (/ look up variable name                      /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     look_up1
 :* REMARKS       (/ look up variable name                      /)
 :* ENTRY         EXTERNAL
 :*
 :*-----------------------------------------------------------------
 :*
 :* VERSION       001
 :*   CRDATE      1993-07-08
 :*   AUTHOR      (/ Mounji Abdelaziz       , FUNDP /)
 :*   UPDATE      (/ original issue /)
 :*
 :* END-MODULE-DESCRIPTION rule_head.c
 :******************************************************************
*/
#include "asax.h"

int tExtGlobal_lg = 0;    /* internal global variable descriptors length   */
int tIntGlobal_lg = 0;    /* external global variable descriptors length   */
int tExtAux_lg = 0;       /* length of the table for auxiliary variables   */
static int untypedGl = 0; /* contain the number of global variables so far */
			  /* parsed in a group of global variables         */
			  /*  declaration                                  */



/*
 :******************************************************************
 :* UNIT-BODY globalVar.GlTypeName
 :*
 :* TITLE         (/ parsing a type name                   /)
 :*
 :* PURPOSE       (/ parse a symbol representing a type name; initialize
 :*                  the involved table of global variables to contain this
 :*                  type if no error is found or recover from this error
 :*                  otherwise /)
 :*                                                      END-PURPOSE
 :* VERSION       001
 :*   CRDATE      1993-07-08
 :*   AUTHOR      (/ Mounji Abdelaziz       , FUNDP              /)
 :*   UPDATE      (/ original issue /)
 :*
 :* REALIZATION   (/ <note, reference to other proc. (optional)> /)
 :*
 :*   CONTROL    (/
 :*
 :*                                                               /)
 :*   DATA       (/
 :*                   - tIntGlobal[]  : internal variable descriptors table
 :*                   - tExtGlobal[]  : external variable descriptors table
 :*                   - tExtAux[]     : auxiliary table for external variable
 :*                                     names
 :*
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
 :*             - 'varclass' = internal or = external
 :*             - 'Stop' is an array stops (see REMARKS below)
 :*             - 'lg' is the length of 'Stop'
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/ Let varDescr = tIntGlobal if 'varclass' = internal
 :*                          = tExtGlobal if 'varclass' = external
 :*               if the CSS is of the form:
 :*                      <type_name><rest>
 :*               where
 :*                  <type_name> is a type name
 :*                  <rest> is any character sequence
 :*               the effect is the following:
 :*                - the table of global variable descriptors
 :*                   'varDescr[]' is updated and any type
 :*                   mismatch is reported (see file rdescr.c function
 :*                   put_type() for extensive details about this update)
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
 :* END-BODY globalVar.GlTypeName
 :******************************************************************
*/

GlTypeName(varclass, Stop, lg)
int varclass, Stop[], lg;
{
	int k;

	if ((k = InTypes(symb.bsymbval.svalue)) >= 0) {
	    put_Gl_type(varclass, k);
	} else {
		error(semantics, 1);
	}
	Expect(symb.ibsymb, Stop, lg);
}
/*
 :******************************************************************
 :* UNIT-BODY globalVar.globalVarGr
 :*
 :* TITLE         (/ parsing a group of variables                  /)
 :*
 :* PURPOSE       (/ parse a sequence of symbols representing the declaration
 :*                  of a group of global variables; update the involved table
 :*                  to contain descriptors for these names if no error is
 :*                  found or recover from this error otherwise /)
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
 :*                   - tIntGlobal[]  : internal variable descriptors table
 :*                   - tExtGlobal[]  : external variable descriptors table
 :*                   - tExtAux[]     : auxiliary table for external variable
 :*                                     names
 :*
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
 :*             - 'varclass' = internal or = external
 :*             - 'Stop' is an array stops (see REMARKS below)
 :*             - 'lg' is the length of 'Stop'
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/ Let varDescr = tIntGlobal if 'varclass' = internal
 :*                          = tExtGlobal if 'varclass' = external
 :*             if the CSS is of the form:
 :*                   <name_1> "," ... "," <name_N> ":" <type_name><rest>
 :*               where
 :*                  - N>=1
 :*                  - <name_i> is an identifier (an internal or external )
 :*                  -          (i=1, ..., N)
 :*                  - <type_name> is a valid type name (integer or string)
 :*                  - <rest> is any character sequence
 :*               the effect is the following:
 :*                - the table of global variable descriptors 'varDescr[]'
 :*                  is updated (see functions addGlobalVar() and put_type() in
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
 :* END-BODY globalVar.globalVarGr
 :******************************************************************
*/
globalVarGr(varclass, Stop, lg)
int varclass, Stop[], lg;
{
	int lg1,lg2, S1[Mlg], S2[Mlg];   /* auxiliary stops      */
	                  /* memory allocation    */
	char *module_name;               /* current module name  */
	char *name;                      /* global variable name */

	insert_stop(colon, Stop, lg, S1, &lg1);
	insert_stop(comma, S1, lg1, S2, &lg2);
	if ((name = tmalloc(ident_lg, char)) == NULL) {
	    error(fatal, 25);  /* allocation problem */
	}
	ExpectName(name, S2, lg2);
	addGlobalVar(varclass, name);
	if (symb.ibsymb == comma) {
		insert_stop(identifierx, Stop, lg, S1, &lg1);
		Expect(comma, S1, lg1);
		globalVarGr(varclass, Stop, lg);
	} else {
		insert_stop(integerx, Stop, lg, S1, &lg1);
		insert_stop(stringx, S1, lg1, S2, &lg2);
		Expect(colon, S2, lg2);
		GlTypeName(varclass, Stop, lg);
	  }
}
/*
 :******************************************************************
 :* UNIT-BODY globalVar.addGlobalVar
 :*
 :* TITLE         (/ add a global variable descriptor      /)
 :*
 :* PURPOSE       (/ add a new desccriptor for this global
 :*                  variable to the table 'tIntGlobal[]' or 'tExtGlobal[]'
 :*                  and initialize its fields                   /)
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
 :*                   - CurrModAddr   : index in tModDescr[] of the module
 :*                                     been parsed
 :*                   - tModDescr[]   : table of module descriptors
 :*                   - tIntGlobal[]  : internal variable descriptors table
 :*                   - tExtGlobal[]  : external variable descriptors table
 :*                   - tExtAux[]     : auxiliary table for external variable
 :*                                     names
 :*                   - untypedGl     : number of global variables parsed
 :*                                     so far in the global variables group
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
 :*   Pre    (/ - 'var_name' is the internal or external variable name
 :*             - 'varclass' = internal or external
 :*                                                                      /)
 :*                                                             END-Pre
 :*
 :*   Post   (/ Let varDescr = tIntGlobal if 'varclass' = internal
 :*                          = tExtAux    if 'varclass' = external
 :*             A new entry is added to 'varDescr[]'. This entry
 :*             is initialized with this name:
 :*                 - varName = 'name'
 :*                 - If 'name' is an internal variable name
 :*                     tModDescr[CurrModAddr].intVarNbre is added 1
 :*             'untypedGl' = number of global variables parsed so far
 :*             'tExtAux_lg' and tIntGlobal_lg' are respectively the current
 :*             length of 'tExtAux[]' and 'tIntGlobal[]'
 :*             In case the maximum size of 'tExtAux[]' or 'tIntGlobal[]' is
 :*             reached a message is sent to stdout to signal this.
 :*                                                                 /)
 :*                                                             END-Post
 :*   RETURNS  NONE
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY globalVar.addGlobalVar
 :******************************************************************
*/
addGlobalVar(varclass, var_name)
int varclass;
char var_name[];
{
    int i0;
    char *to;

    if (varclass == internal) {
	i0 = tModDescr[CurrModAddr].VarPtr;
	if (look_up(tIntGlobal, i0, tIntGlobal_lg, var_name)<0) {
	    if (GV_ptr < GV + MaxGV) { /* below global var. area max size */
		if (tIntGlobal_lg < MaxIntGlobal) {
		    to = tIntGlobal[tIntGlobal_lg++].varName;
		    strcpy(to , var_name);
		    tModDescr[CurrModAddr].intVarNbre++;
		    untypedGl++;
		} else {
		    error(fatal, 21); /* too much internals */
		}
	    } else {
		error(fatal, 17); /* max globals exceeded */
	    }
	} else {
	    error1(semantics, 18, var_name);
	}
    } else { /* varclass == external */
	if (look_up1(tExtAux, 0, tExtAux_lg, var_name)<0) {
	    if (tExtAux_lg < MaxExtAux) {
	       if ((to = (char *) malloc(ident_lg)) == NULL) {
			error(fatal, 25);  /* out of memory */
		}
		strcpy(to, var_name);
		tExtAux[tExtAux_lg] = to;
		tExtAux_lg++;
		untypedGl++;
	    } else {
		error(fatal, 20); /* external gl per a file exceeded */
	    }
	} else {
	    error1(semantics, 18, var_name);
	}
    }
}
/*
 :******************************************************************
 :* UNIT-BODY globalVar.globalVariables
 :*
 :* TITLE         (/ parsing a list of group of global variables /)
 :*
 :* PURPOSE       (/ parse a sequence of symbols representing a declaration
 :*                  of a list of groups of global variables; initialize
 :*                  descriptors for these variables if no error
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
 :*                   - CurrModAddr   : index in tModDescr[] of the module
 :*                                     been parsed
 :*                   - tModDescr[]   : table of module descriptors
 :*                   - tIntGlobal[]  : internal variable descriptors table
 :*                   - tExtGlobal[]  : external variable descriptors table
 :*                   - tExtAux[]     : auxiliary table for external variable
 :*                                     names
 :*
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
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/   if the CSS is of the form:
 :*                   <VariableGroup_1>";" ... ";"<VariablesGroup_N><rest>
 :*               where
 :*                 - N>=1
 :*                 - <VariablesGroup_i> is a sequence of symbols of the form:
 :*                  "global" <name_1> "," ... "," <name_n> ":" <type_name_i>
 :*                    where
 :*                       - n>=1
 :*                       - <name_j> is a variable name  (j=1, ..., n)
 :*                       - <type_name_i> is a valid type name (integer or
 :*                                       string)        (i=1, ..., N)
 :*                 - <rest> is any character sequence
 :*               the effect is the following:
 :*                - the table of variable descriptors 'tExtGlobal[]' and
 :*                  'tIntGlobal[]' are updated
 :*                  (see functions addGlobalVar() and put_type() in
 :*                  file rdescr.c for extensive details about this update)
 :*                - tExtAux_lg = 0
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
 :* END-BODY globalVar.globalVariables
 :******************************************************************
*/
globalVariables(Stop, lg)
int Stop[], lg;
{
    int i, S1[Mlg], lg1, S2[Mlg], lg2;

    insert_stop(semicolon, Stop, lg, S1, &lg1);
    insert_stop(globalx, S1, lg1, S2, &lg2);
    while (symb.ibsymb == globalx) {/* loop in group of global variables */
	globalVariableGroup(S1, lg1); /* parse a group of global variables */
	Expect(semicolon, S2, lg2);
    }
    for (i=0; i<tExtAux_lg; i++)
	free(tExtAux[i]);
    tExtAux_lg = 0; /* clear the table of auxiliary externals */
}
/*
 :******************************************************************
 :* UNIT-BODY globalVar.globalVariableGroup
 :*
 :* TITLE         (/ parsing a group of variables                  /)
 :*
 :* PURPOSE       (/ parse a sequence of symbols representing the declaration
 :*                  of a group of global variables; initialize descriptors
 :*                  this group if no error is
 :*                  found or recover from this error otherwise /)
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
 :*                   - CurrModAddr   : index in tModDescr[] of the module
 :*                                     been parsed
 :*                   - tModDescr[]   : table of module descriptors
 :*                   - tIntGlobal[]  : internal variable descriptors table
 :*                   - tExtGlobal[]  : external variable descriptors table
 :*                   - tExtAux[]     : auxiliary table for external variable
 :*                                     names
 :*
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
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/
 :*          if the CSS is of the form:
 :*                   <name_1> "," ... "," <name_N> ":" <type_name><rest>
 :*               where
 :*                  - N>=1
 :*                  - <name_i> is an identifier (a global variable)
 :*                  -          (i=1, ..., N)
 :*                  - <type_name> is a valid type name (integer or string)
 :*                  - <rest> is any character sequence
 :*               the effect is the following:
 :*                - the table of global variable descriptors 'tIntGloba[]'
 :*                  or 'tExtGlobal[]' is updated (see functions
 :*                  addGlobalVar() and put_type() in file rdescr.c
 :*                  for extensive details about this update)
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
 :* END-BODY globalVar.globalVariableGroup
 :******************************************************************
*/
globalVariableGroup(Stop, lg)
int Stop[], lg;
{
    int S1[Mlg], lg1, S2[Mlg], lg2, S3[Mlg], lg3;

    insert_stop(identifierx, Stop, lg, S1, &lg1);
    insert_stop(externalx, S1, lg1, S3, &lg3);
    insert_stop(internalx, S3, lg3, S2, &lg2);
    Expect(globalx, S2, lg2);
    if (symb.ibsymb == internalx) {
	Expect(internalx, S1, lg1);
	globalVarGr(internal, Stop, lg);
    } else if (symb.ibsymb == externalx) {
	Expect(externalx, S1, lg1);
	globalVarGr(external, Stop, lg);
    } else { /* the default is an internal variable group declaration */
	globalVarGr(internal, Stop, lg);
    }
}

/*
 :******************************************************************
 :* UNIT-BODY globalVar.put_Gl_type
 :*
 :* TITLE         (/ initialize descriptors for gloabal variables /)
 :*
 :* PURPOSE       (/ this function receives a type and a variable class and
 :*                  the descriptors corresponding to this variables
 :*                  in case  no error is found or recover from
 :*                  this error otherwise /)
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
 :*                   - tIntGlobal[]  : internal variable descriptors table
 :*                   - tExtGlobal[]  : external variable descriptors table
 :*                   - tExtAux[]     : auxiliary table for external variable
 :*                                     names
 :*                   - untypedGl       : the rank of this name (par. or var.)
 :*                                      in the parameter or variable group
 :*
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
 :*   Pre    (/ - 'type' is in {integer, bytestring, fieldstring, undef} and
 :*               is the type of the variable or parameter group
 :*             - 'varclass' = internal or = external
 :*             - tIntGlobal_lg = lg0
 :*             - GV_ptr = ptr0
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/ if 'varclass' = internal
 :*             then we have:
 :*                - for all i:
 :*                   0<=i<'untypedGl' ==> tIntGlobal[lg0 + i].type = 'type'
 :*                          tIntGlobal[lg0 + i].addr = ptr0 + 4 * i
 :*                - GV_ptr = ptr0 + 4 * untypedGl
 :*                - 'untypedGl' is reset to 0
 :*             otherwise (varclass = external) we have the POST of the
 :*             routine checkGroupTypes() in this same file            /)
 :*                                                             END-Post
 :*   RETURNS  NONE
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY globalVar.put_Gl_type
 :******************************************************************
*/
put_Gl_type(varclass, type)
int varclass, type;
{
    int i;

    if (varclass == external) {
	checkGroupType(type);
    } else {
	for (i=untypedGl; i>0; i--) {
	    tIntGlobal[tIntGlobal_lg - i].type = (enum type) type;
	    tIntGlobal[tIntGlobal_lg - i].addr = GV_ptr;
	    GV_ptr += size(type);
	}
    }
    untypedGl = 0;
}
/*
 :******************************************************************
 :* UNIT-BODY globalVar.checkGroupType
 :*
 :* TITLE         (/ type checking for external global variables  /)
 :*
 :* PURPOSE       (/ output error message if two given types do not match /)
 :*                                                      END-PURPOSE
 :* VERSION       001
 :*   CRDATE      1992-07-03
 :*   AUTHOR      (/ Mounji Abdelaziz       , FUNDP              /)
 :*   UPDATE      (/ original issue /)
 :*
 :* REALIZATION   (/ <note, reference to other proc. (optional)> /)
 :*
 :*   CONTROL    (/
 :*                                                               /)
 :*   DATA  (/
 :*                   - tExtGlobal[]  : external variable descriptors table
 :*                   - tExtGlobal_lg : length of tExtGlobal[]
 :*                   - tExtAux[]     : auxiliary table for external variable
 :*                                     names
 :*                   - tExtAux_lg    : length of tExtAux[]
 :*                   - GV_ptr        : next free position in the global
 :*                                     variables area
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
 :*   Pre (/ - 'type' is a variable of the enumerated type:
 :*                 {integer, bytestring, fieldstring, undef}
 :*          - Let
 :*                - tExtAux_lg = lg0
 :*                - tExtGlobal_lg = lg1
 :*                - GV_ptr = ptr0
 :*                - Inter = {(i, j)  | 0<=i<lg0, 0<=j<lg1 and
 :*                         tExtAux[i] = tExtGlobal[j].varName};
 :*                - n = #Inter
 :*       /)
 :*                                                             END-Pre
 :*
 :*   Post (/ - for each (i, j) in Inter if
 :*                         tExtAux[i] != tExtGlobal[j].type
 :*               - the error mesage:
 :*                      Semantics        'lineNbre': type mismatch
 :*                  where 'lineNbre' indicates the line number of the error
 :*                  is output to stdout and ErrorFound is set to True
 :*           - the entries 0 ... lg1 - 1 of tExtGlobal[] are unchanged
 :*           - for all i such that (i, j) not in Inter (for all y)
 :*             there exist j (lg1<=j<(lg0 - n) such that
 :*                 - tExtGlobal[j].type = 'type'
 :*                 - tExtGlobal[j].addr = ptr0 + (j - lg0)
 :*           - GV_ptr = ptr0 + lg0 - n
 :*           - tExtGlobal_lg = lg1 + lg0 - n
 :*                                                              /)
 :*
 :*                                                             END-Post
 :*   RETURNS  NONE
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY globalVar.checkGroupType
 :******************************************************************
*/
checkGroupType(type)
int type;
{
    int i, j, found;

    for (i=0; i<tExtAux_lg; i++) {
	found = FALSE;
	for(j=0; j<tExtGlobal_lg; j++) {
	    if (strcmp(tExtGlobal[j].varName, tExtAux[i]) == 0) {
		found = TRUE;
		checkTypes(&tExtGlobal[j].type, type);
		break;
	    }
	}
	if (!found) {
	    strcpy(tExtGlobal[tExtGlobal_lg].varName, tExtAux[i]);
	    tExtGlobal[tExtGlobal_lg].type = (enum type) type;
	    tExtGlobal[tExtGlobal_lg].addr = GV_ptr;
	    GV_ptr += size(type);
	    tExtGlobal_lg++;
	}
    }
}
/*
 :******************************************************************
 :* UNIT-BODY globalVar.look_up
 :*
 :* TITLE         (/ look up a variable name /)
 :*
 :* PURPOSE       (/ search a table of global variable descriptors
 :*                  for a global variable name, return its index if found
 :*                  -1 otherwise /)
 :*                                                           /)
 :*                                                      END-PURPOSE
 :* VERSION       001
 :*   CRDATE      1993-07-03
 :*   AUTHOR      (/ Mounji Abdelaziz       , FUNDP              /)
 :*   UPDATE      (/ original issue /)
 :*
 :* REALIZATION   (/ <note, reference to other proc. (optional)> /)
 :*
 :*   CONTROL    (/
 :*   DATA       (/
 :*                   - tExtGlobal[]  : external variable descriptors table
 :*                   - tExtGlobal_lg : length of tExtGlobal[]
 :*                   - tExtAux[]     : auxiliary table for external variable
 :*                                     names
 :*                   - tExtAux_lg    : length of tExtAux[]
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
 :*   Pre    (/ -
 :*                                                            /)
 :*                                                             END-Pre
 :*
 :*   Post   (/ - if there exist a i (low<=i<high) such that the string
 :*               tabDescr[i] is equal to 'name' return i otherwise return -1
 :*                                                                   /)
 :*                                                             END-Post
 :*
 :*   RETURNS  "int" (/ index of 'name' in 'tabDescr[]' if found
 :*                     otherwise, the returned value is -1
 :*                                                               /)
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY globalVar.look_up
 :******************************************************************
*/
look_up(tabDescr, low, high, name)
varDescr tabDescr[];
int low, high;
char name[];
{
    int i, found;

    i = low;
    found = FALSE;
    while (i < high && !found) {
	if (!strcmp(tabDescr[i++].varName, name))
	    found = TRUE;
    }
    if (found)
	return (--i);
    else
	return (-1);
}
/*
 :******************************************************************
 :* UNIT-BODY globalVar.look_up1
 :*
 :* TITLE         (/ look up a variable name /)
 :*
 :* PURPOSE       (/ search a table of strings
 :*                  for a global variable name, return its index if found
 :*                  -1 otherwise /)
 :*                                                           /)
 :*                                                      END-PURPOSE
 :* VERSION       001
 :*   CRDATE      1993-07-03
 :*   AUTHOR      (/ Mounji Abdelaziz       , FUNDP              /)
 :*   UPDATE      (/ original issue /)
 :*
 :* REALIZATION   (/ <note, reference to other proc. (optional)> /)
 :*
 :*   CONTROL    (/
 :*   DATA       (/
 :*                   - tExtAux[]     : auxiliary table for external variable
 :*                                     names
 :*                   - tExtAux_lg    : length of tExtAux[]
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
 :*   Pre    (/ -
 :*                                                            /)
 :*                                                             END-Pre
 :*
 :*   Post   (/ - if there exist a i (low<=i<high) such that the string
 :*               tabStr[i] is equal to 'name' return i otherwise return -1
 :*                                                                   /)
 :*                                                             END-Post
 :*
 :*   RETURNS  "int" (/ index of 'name' in 'tabDescr[]' if found
 :*                     otherwise, the returned value is -1
 :*                                                               /)
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY globalVar.look_up1
 :******************************************************************
*/
look_up1(tabStr, low, high, name)
char *tabStr[];
int low, high;
char name[];
{
    int i, found;

    i = low;
    found = FALSE;
    while (i < high && !found) {
	if (strcmp(tabStr[i++], name) == 0)
	    found = TRUE;
    }
    if (found)
	return (--i);
    else
	return (-1);
}
init_globalVar()
{

tExtGlobal_lg = 0;    /* internal global variable descriptors length   */
tIntGlobal_lg = 0;    /* external global variable descriptors length   */
untypedGl = 0; 		/* contain the number of global variables so far */
                          /* parsed in a group of global variables         */
                          /*  declaration                                  */
	
}
