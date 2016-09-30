/*
*	List of updates by 
* 	Mounji Abdelaziz
* 	Institut d'Informatique 
*	Rue Grangagnage, 21
*	5000 Namur
*
*	rdescr.c 
*
* Wed Jan 26 12:01:27 WET 199  Mounji
* in function getRuleAddr (line: 875)
* in the first while
* i must be < trdescr_lg 
*/

/*
 :******************************************************************
 :* MODULE-DESCRIPTION
 :*
 :* TITLE         (/ management routines for objects descriptor tables /)
 :* NAME          rdescr.c
 :*
 :* COPYRIGHT     (C)   SIEMENS NIXDORF INFORMATIONSSYSTEME AG  1992
 :*                     ALL RIGHTS RESERVED
 :*
 :* MEMORY-CLASS  6
 :*
 :* PROG-CONVENTS (/ <internal conventions (instance, names,
 :*                  external data, ...). None -> empty text>     /)
 :*
 :* PROCEDURE     add_rule_decl
 :* REMARKS       (/ add a descriptor for a rule declaration           /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     add_rule_trig
 :* REMARKS       (/ add a descriptor for a rule triggering            /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     add_rule
 :* REMARKS       (/ initialize a rule descriptor                      /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     add_name
 :* REMARKS       (/ add a parameter or a variable descriptor    /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     put_type
 :* REMARKS       (/ put the type of a variable or parameter group /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     size
 :* REMARKS       (/ get the storage size of a type                /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     getRuleCode
 :* REMARKS       (/ get the code of a given rule                  /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     getRuleAddr
 :* REMARKS       (/ get the address of a given rule               /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     getNameAddr
 :* REMARKS       (/ get characteristics of a parameter or a variable /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     getVarAddr
 :* REMARKS       (/ get characteristics of a variable /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     getGlobNameAddr
 :* REMARKS       (/ get characteristics of a global variable /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     getParType
 :* REMARKS       (/ get the type of a rule parameter              /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     getParType1
 :* REMARKS       (/ get the type of a rule parameter /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     add_hole
 :* REMARKS       (/ add a hole related to a rule code              /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     getFieldNameAddr
 :* REMARKS       (/ get a field name address /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     getAuditDataId
 :* REMARKS       (/ get audit data identifier /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     print_trdescr
 :* REMARKS       (/ output the rule descriptor table content           /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     print_tdescr
 :* REMARKS       (/ output the content of the table of name descriptors /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     print_targ_types
 :* REMARKS       (/ output of the content of the table of argument types /)
 :* ENTRY         EXTERNAL
 :*
 :*-----------------------------------------------------------------
 :*
 :* VERSION       001
 :*   CRDATE      1992-23-10
 :*   AUTHOR      (/ Mounji Abdelaziz    , FUNDP /)
 :*   UPDATE      (/ original issue /)
 :*
 :* END-MODULE-DESCRIPTION rdescr.c
 :******************************************************************
*/
#include <ctype.h>          /* definition of some types */
#include "asax.h"           /* shared declarations      */


int trdescr_lg = 0;        /* next free position in trdescr[] */
int trdescr_ptr = -1;      /* current position in trdescr[]   */
rdescr trdescr[MaxRuleNr]; /* descriptors of internal rules   */

int tExtRule_lg = 0;             /* next free position in trdescr[]       */
int tExtRule_ptr = -1;           /* current position in tExtRuleDescr[]   */
rdescr tExtRuleDescr[MaxRuleNr]; /* descriptors of external rules         */

int targ_types_lg = 0;             /* next free position in targ_types[]  */
enum type targ_types[MaxVarNr];    /* variable or parameter types         */

namedescr tdescr[MaxVarNr]; /* table of var. or par. descriptors   */
int tdescr_lg = 0;          /* next free position in tdescr[]      */

static int untyped = 0;            /* number of names awaiting for a type */

/*
 :******************************************************************
 :* UNIT-BODY rdescr.add_rule_decl
 :*
 :* TITLE         (/ add a descriptor for a rule declaration     /)
 :*
 :* PURPOSE       (/ Update the tables trdesc[] and tExtRuleDescr[]
 :*                  to contain a descriptor for an internal or an external
 :*                  rule declaration                            /)
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
 :*                - trdescr[]      : rule descriptors table
 :*                - trdescr_ptr    : current index in trdescr[]
 :*                - trdescr_lg     : length of trdescr[]
 :*                - tdescr_ptr     : current index in tdescr[]
 :*                - tExtRuleDescr[]: rule descriptors table
 :*                - tExtRule_lg    : length of tExtRuleDescr[]
 :*                - tExtRule_ptr   : current index in tExtRuleDescr[]
 :*                - tdescr_ptr     : current index in tdescr[]
 :*                - targ_types_ptr : current index in targ_types[]
 :*
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
 :*   Pre    (/  - 'ruleclass' = internal if this is an internal rule decl.
 :*                            = external if this is an external rule decl.
 :*              - 'rule_name' a string representing a rule name
 :*                                                                /)
 :*                                                             END-Pre
 :*
 :*   Post   (/ if this is an unknown internal (respec. external)  rule
 :*             (never declared or triggered before) a new descriptor is
 :*             added for it to 'trdescr[]' (respec. to tExtRuleDescr[]).
 :*             This entry is initialized as follows:
 :*                 - ParNre = VarNre = APASize = Code = NULL
 :*                 - nameDescr = tdescr_ptr
 :*                 - targDescr = targ_types_ptr
 :*             If this is a declaration for an external rule and a
 :*             descriptor for it is found in 'trdescr[]', this descriptor is
 :*             copied in tExtRuleDescr[] and the name of this rule is
 :*             replaced by the '$$NowExernal$$'.
 :*
 :*             In case the maximum size of 'trdescr[]' or tExtyRuleDescr[]
 :*             is reached a message is sent to stdout to signal this.
 :*             If this is a declaration of an already declared rule an
 :*             error message is sent to stdout
 :*                                                                   /)
 :*                                                             END-Post
 :*   RETURNS  NONE
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY  rdescr.add_rule
 :******************************************************************
*/
add_rule_decl(ruleclass, rule_name)
int ruleclass;
char *rule_name;
{
    int i;  /* a module address */
    int k;  /* a rule address   */
    rdescr *rdescrTab;  /* base address of trdescr[] or tExtRuleDescr[] */

    k = getRuleAddr(rule_name, &rdescrTab);
    if (k < 0) {   /* the rule was never encountered before */
	add_rule(ruleclass, rule_name);
    } else if (rdescrTab == trdescr && ruleclass == external) {
	if (trdescr[k].status != declared) {

	    /* the rule was triggered before but considered internal */

	    tExtRuleDescr[tExtRule_lg] = trdescr[k];
	    tExtRuleDescr[tExtRule_lg].Code = (BoxPtr) &trdescr[k].Code;
	    tExtRule_lg++;
	    tExtRule_ptr++;

	    /*  hide the name of this rule to avoid name calshes */

	    strcpy(trdescr[k].rule_name, "$$NowExternal$$");
	    trdescr[k].status = declared;
	} else {

	    /* the rule once was internal and now it is external */

	    error1(semantics, 16, rule_name);   /*   redeclared rule */
	}
    } else if (rdescrTab == tExtRuleDescr && ruleclass == internal) {
	i = rdescrTab[k].modulePtr;
	if (strcmp(tModDescr[i].module_name, CurrMod_name) == 0) {

	    /* the rule once was external and now it is internal */

	    error1(semantics, 16, rule_name);   /*   redeclared rule */
	} else {
	    add_rule(ruleclass, rule_name);
	}
    } else if (rdescrTab[k].status != declared) {
	rdescrTab[k].nameDescr = tdescr_lg;
    } else {
	error1(semantics, 16, rule_name);   /*   redeclared rule */
    }
}
/*
 :******************************************************************
 :* UNIT-BODY rdescr.add_rule_trig
 :*
 :* TITLE         (/ add a descriptor for a rule triggering /)
 :*
 :* PURPOSE       (/ A rule has been triggered. The table
 :*                 'trdescr[]' of rule descriptors must be updated according
 :*                 to this rule declaration triggering            /)
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
 :*                       - trdescr[]     : rule descriptors table
 :*                       - trdescr_ptr   : current index in trdescr[]
 :*                       - trdescr_lg    : length of trdescr[]
 :*                       - tdescr_ptr    : current index in tdescr[]
 :*                       - targ_types_ptr: current index in targ_types[]
 :*
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
 :*   Pre    (/  - 'rule_name' a string representing a rule name
 :*                                                                /)
 :*                                                             END-Pre
 :*   Post   (/ if this is an unknown rule (never declared or triggered
 :*             before) a new entry for it is added to 'trdescr[]'. This
 :*             entry is initialized as follows:
 :*                 - ParNre = VarNre = APASize = Code = NULL
 :*                 - status = unknown if 'rule_name' is first encountered
 :*                 - status = known if 'rule_name' was triggered only once
 :*                 - nameDescr = tdescr_ptr
 :*                 - targDescr = targ_types_ptr
 :*             In case the maximum size of 'trdescr[]' or tExtRuleDescr[]
 :*             is reached a message is sent to stdout to signal this.
 :*                                                                   /)
 :*                                                             END-Post
 :*   RETURNS  NONE
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY  rdescr.add_rule_trig
 :******************************************************************
*/
add_rule_trig(rule_name)
char *rule_name;
{
    int k, i;
    rdescr *rdescrTab;

    if ((k = getRuleAddr(rule_name, &rdescrTab))< 0) {
	i = add_rule(internal, rule_name);
	trdescr[i].status = unknown;
    } else if (rdescrTab[k].status == unknown) {
	    rdescrTab[k].status = known;
    }
}
/*
 :******************************************************************
 :* UNIT-BODY rdescr.add_rule
 :*
 :* TITLE         (/ add a descriptor for a rule triggering /)
 :*
 :* PURPOSE       (/ A rule has been triggered. The table
 :*                 'trdescr[]' of rule descriptors must be updated according
 :*                 to this rule declaration triggering            /)
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
 :*                       - trdescr[]     : rule descriptors table
 :*                       - trdescr_ptr   : current index in trdescr[]
 :*                       - trdescr_lg    : length of trdescr[]
 :*                       - tdescr_ptr    : current index in tdescr[]
 :*                       - targ_types_ptr: current index in targ_types[]
 :*
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
 :*   Pre    (/  - 'rule_name' a string representing a rule name
 :*              - 'ruleclass' = internal if 'rule_name' is an internal rule
 :*                              external if 'rule_name' is an external rule
 :*                                                                /)
 :*                                                             END-Pre
 :*
 :*   Post   (/ if 'ruleclass ' = internal, a rule descriptor is added for
 :*             'rule_name' to 'trdescr[]'
 :*             if 'ruleclass ' = 'external, a rule descriptor is added for
 :*             'rule_name' to 'tExtRuleDescr[]'.  This entry is initialized
 :*             as follows:
 :*                 - ParNre = VarNre = APASize = Code = NULL
 :*                 - nameDescr = tdescr_ptr
 :*                 - targDescr = targ_types_ptr
 :*
 :*             In case the maximum size of 'trdescr[]' or tExtRuleDescr[]
 :*             is reached a message is sent to stdout to signal this.
 :*                                                                   /)
 :*                                                             END-Post
 :*   RETURNS  (/ if ruleclass = internal, tdescr_lg is returned
 :*               if ruleclass = external, tExtRule_lg is returned
 :*                                                                   /)
 :*                                                         END-RETURNS
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY  rdescr.add_rule
 :******************************************************************
*/
add_rule(ruleclass, rule_name)
int ruleclass;
char *rule_name;
{
    int *prlg;         /* = &tExtRule_lg  or &trdescr_lg  */
    int *pptr;         /* = &tExtRule_ptr or &trdescr_ptr */
    rdescr *rdescrTab; /* = tExtRuleDescr or trdescr      */

    if (ruleclass == internal) {
	tModDescr[CurrModAddr].ruleDeclNbre++;
	rdescrTab = trdescr;
	prlg = &trdescr_lg;
	pptr = &trdescr_ptr;
    } else { /* ruleclass == external */
	rdescrTab = tExtRuleDescr;
	prlg = &tExtRule_lg;
	pptr = &tExtRule_ptr;
    }
    if (*pptr < MaxRuleNr) {
	(*pptr)++;    /* increment current and next positions */
	(*prlg)++;
		      /* initialize descriptor */

	strcpy(rdescrTab[*pptr].rule_name, rule_name);
	rdescrTab[*pptr].ParNre = 0;
	rdescrTab[*pptr].VarNre = 0;
	rdescrTab[*pptr].APASize = 0;
	rdescrTab[*pptr].modulePtr = 0;
	rdescrTab[*pptr].status = 0;
	rdescrTab[*pptr].Code = NULL;
	rdescrTab[*pptr].nameDescr = tdescr_lg;
	rdescrTab[*pptr].targDescr = targ_types_lg;
    } else {
	error(fatal, 26);  /* too much rule declarations */
    }
    return (*pptr);
}

/*
 :******************************************************************
 :* UNIT-BODY rdescr.add_name
 :*
 :* TITLE         (/ add a parameter or a variable descriptor      /)
 :*
 :* PURPOSE       (/ add a new desccriptor for this parameter or local
 :*                  variable to the table 'tdescr[]' and initialize its
 :*                  fields                                         /)
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
 :*                    - trdescr[]   : rule descriptors table
 :*                    - tdescr[]    : names (par; or var) descriptors table
 :*                    - tdescr_lg   : length of 'tdescr[]'
 :*                    - tdescr_ptr  : current index in 'tdescr[]'
 :*                    - rule_name   : name of the rule this name declaration
 :*                                    belongs to
 :*                    - untyped     : the rank of this name (par. or var.)
 :*                                    in the parameter or variable group
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
 :*   Pre    (/ - 'name' is the parameter or local variable name
 :*             - 'pv' = 0 if this is a parameter, 1 if a local variable
 :*                                                                      /)
 :*                                                             END-Pre
 :*
 :*   Post   (/ A new entry is added to 'tdescr[]'. This entry is initialized
 :*             with the characeristics of this name:
 :*                 - name = 'name'
 :*                 - iname = 'pv'
 :*                 - If 'name' is a parameter, the field ParNre (number of
 :*                   parameters) corresponding to the rule been declared is
 :*                   incremented by 1 unless this rule was already been
 :*                   triggered before (status = 'unkonwn' or 'known')
 :*                 - If 'name' is a variable, the field VarNre (number of
 :*                   variables) corresponding to the rule been declared is
 :*                   incremented by 1.
 :*             'untyped' = current length of the parameter or variable group
 :*             'tdescr_lg' is the current index and length of 'tdescr[]'
 :*
 :*             In case the maximum size of 'tdescr[]' is reached a message
 :*             is sent to stdout to signal this.
 :*                                                                 /)
 :*                                                             END-Post
 :*   RETURNS  NONE
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY rdescr.add_name
 :******************************************************************
*/
add_name(name, pv)
char name[];
int pv;
{
    int k;             /* rule address */
    int status;        /* rule status = knonw, unknown or declared */
    rdescr *rdescrTab; /* tExtRuleDescr or trdescr */

	/* get the address of the rule this name declaration comes from */

    k = getRuleAddr(rule_name, &rdescrTab);
    if (tdescr_lg < MaxVarNr) {
			      /* initialize name descriptor */

	strcpy(tdescr[tdescr_lg].name, name);
	tdescr[tdescr_lg].iname = (enum var_par) pv;
	status = rdescrTab[k].status;
	if (pv == (int) par && status != unknown && status != known) {
	    rdescrTab[k].ParNre++;
	}
	if (pv == (int) var)
	    rdescrTab[k].VarNre++;
	untyped++;          /* count the names to put the type afterwards */
	tdescr_lg++;
    } else {
       error(fatal, 27);  /* too much local variables and parameters */
    }
}
/*
 :******************************************************************
 :* UNIT-BODY rdescr.put_type
 :*
 :* TITLE         (/ put the type of a variable or parameter group      /)
 :*
 :* PURPOSE       (/ this function receives a type and install it in the
 :*                  descriptors corresponding to this variables or
 :*                  parameters in case  no error is found or recover from
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
 :*                    - trdescr[]     : rule descriptors table
 :*                    - tdescr[]      : names (par; or var) descriptors table
 :*                    - tdescr_lg     : length of 'tdescr[]'
 :*                    - tdescr_ptr    : current index in 'tdescr[]'
 :*                    - targ_types[]  : argument types table
 :*                    - targ_types_ptr: current index in 'targ_types[]'
 :*                    - rule_name     : name of the rule this name
 :*                                      declaration belongs to
 :*                    - untyped       : the rank of this name (par. or var.)
 :*                                      in the parameter or variable group
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
 :*             - 'pv' is 0 if this is a parameter group 1 if a variable group
 :*             - 'rank' is the length of this group
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/ let
 :*              - k be the index in 'trdescr[]' or 'tExtRuledescr[]' of
 :*                the rule this variable or parameter declaration belongs to;
 :*              - l, m be respectively the values of the fields 'nameDescr'
 :*                and 'targDescr' corresponding to this rule
 :*           then we have:
 :*              - for all i:
 :*                0<=i<'untyped' ==> tdescr[nameDescr + i].tname = 'type'
 :*              - if status = 'known' or 'unknown' an error is generated
 :*                if there exist a j such that:
 :*                  0<=j<rank and targ_types[m+j] != 'type'
 :*              - trdescr[k].APASize = the storage size (in bytes) of the
 :*                the list of parameters so far parsed.
 :*              - 'untyped' is reset to 0
 :*                                                             END-Post
 :*   RETURNS  NONE
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY rdescr.put_type
 :******************************************************************
*/
put_type(type, pv, rank)
int type, pv, rank;
{
	int i, k, index, status;
	rdescr *rdescrTab;

	k = getRuleAddr(rule_name, &rdescrTab);
	status = rdescrTab[k].status;
	index = rdescrTab[k].targDescr + rank;
	for (i=untyped; i>0; i--)
	    tdescr[tdescr_lg - i].tname = (enum type) type;
	if (pv == (int) par && (status == unknown || status == known)) {
	    for (i=index; i>index-untyped; i--) {
		checkTypes(&targ_types[i], type);
	    }
	}
	rdescrTab[k].APASize += untyped * size(type);
	untyped = 0;
}
/*
 :******************************************************************
 :* UNIT-BODY rdescr.size
 :*
 :* TITLE         (/ get the storage size of a type      /)
 :*
 :* PURPOSE       (/ get the storage size (in bytes) of a given type among
 :*                  integer, bytestring, fieldstring and undef   /)
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
 :*   Pre    (/ - 'type' is in {integer, bytestring, fieldstring, undef}
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/ - the storage size (in byte) of 'type' is returned
 :*                                                                 /)
 :*
 :*                                                             END-Post
 :*   RETURNS  "int" (/ the storage size (in byte)          /)
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY rdescr.size
 :******************************************************************
*/
size(type)
int type;
{
	switch (type) {
		case integer    : return (sizeof(int));
		case bytestring : return (sizeof(char *));
		case fieldstring: return (sizeof(char *));
		case undef      : return (sizeof(char *));
		default         : error1(fatal, 24, "size() of rdescr.c");
				  return (0);
	}
}
/*
 :******************************************************************
 :* UNIT-BODY rdescr.getRuleCode
 :*
 :* TITLE         (/ get the code of a given rule       /)
 :*
 :* PURPOSE       (/ get the internal code address of a given rule /)
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
 :*                 - trdescr[]        : descriptors for internal rules
 :*                 - tExtRuleDescr[]  : descriptors for external rules
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
 :*   Pre    (/ - 'rule_name' a string representing a rule name
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/ if there exist a i such that:
 :*                    trdescr[i].rule_name = 'rule_name'
 :*            then trdescr[i].Code is returned
 :*            else if there exist a i such that:
 :*                    tExtRuleDescr[i].rule_name = 'rule_name'
 :*            then tExtRuleDescr[i].Code is returned
 :*             -1 is returned otherwise
 :*                                                                 /)
 :*
 :*                                                             END-Post
 :*   RETURNS  "int" (/ if >= 0 the address of the internal code
 :*                     -1 if unknown rule              /)
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY rdescr.getRuleCode
 :******************************************************************
*/
BoxPtr getRuleCode(rule_name)
char *rule_name;
{
    int k;
    rdescr *rdescrTab;

    if ((k = getRuleAddr(rule_name, &rdescrTab)) < 0)  {
	return ((BoxPtr) -1);
    }
    return(rdescrTab[k].Code);
}
/*
 :******************************************************************
 :* UNIT-BODY rdescr.getRuleAddr
 :*
 :* TITLE         (/ get the address of a given rule    /)
 :*
 :* PURPOSE       (/ get the address of an internal or external rule /)
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
 :*                       - trdescr[]        : internal rule descriptors
 :*                                            table
 :*                       - trdescr_lg       : length of 'trdescr[]'
 :*                       - tExtRuledescr[]  : external rule descriptors
 :*                                            table
 :*                       - tExtRule_lg      : length of 'tExtRuleDescr[]'
 :*                       - tModDescr[]      : module descriptors table
 :*                       - CurrModAddr      : index in tModDescr of the
 :*                                            module descriptor of the
 :*                                            module been parsed
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
 :*   Pre    (/ 's' a string representing a rule name
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/ Let i0 = tModDescr[CurrModAddr].RulePtr
 :*                 m0 = i0 + tModDescr[CurrModAddr].ruleDeclNbre;
 :*
 :*             if there exist a i such that:
 :*                (i0 <= i < m) and trdescr[i].rule_name = 's'
 :*             then i is returned and *rdescr_pp = trdescr
 :*             else if there exist a i such that:
 :*                (0<= i < tExtRule_lg) and tExtRuleDescr[i].rule_name = 's'
 :*             then i is returned and *rdescr_pp = tExtRuleDescr
 :*             -1 is returned otherwise
 :*                                                                 /)
 :*
 :*                                                             END-Post
 :*   RETURNS  "int" (/ if >= 0 index of 'rule_name' in 'trdescr[]'
 :*                     -1 if unknown rule              /)
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY rdescr.getRuleAddr
 :******************************************************************
*/
getRuleAddr(s, rdescr_pp)
char *s;
rdescr **rdescr_pp;
{
    int i, k=1, m;

    i = tModDescr[CurrModAddr].RulePtr;
    m = i + tModDescr[CurrModAddr].ruleDeclNbre;
    while (i < trdescr_lg && i <= m &&
	   (k = strcmp(s, trdescr[i++].rule_name)) != 0);
    if (k == 0) {
	*rdescr_pp = trdescr;
	return (i - 1);
    }
    i = 0;
    while (i<tExtRule_lg && (k=strcmp(s, tExtRuleDescr[i++].rule_name))!=0);
    if (k == 0) {
	*rdescr_pp = tExtRuleDescr;
	return (i - 1);
    }
    return (-1);
}
/*
 :******************************************************************
 :* UNIT-BODY rdescr.getNameAddr
 :*
 :* TITLE         (/ get characteristics of a parameter or a variable /)
 :*
 :* PURPOSE       (/ get the type and offset of a given name and indicate if
 :*                  this is a parameter or a local variable
 :*                                                                   /)
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
 :*                    - trdescr[]   : internal rule descriptors table
 :*                    - tExtRuleDescr[] : external rule descriptors table
 :*                    - tdescr[]    : names (par; or var) descriptors table
 :*                    - tdescr_lg   : length of 'tdescr[]'
 :*                    - tdescr_ptr  : current index in 'tdescr[]'
 :*                    - rule_name   : name of the rule this name declaration
 :*                                    belongs to
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
 :*   Pre   (/ - 'name' is a parameter or a local variable name
 :*            - 'rule_name' is a rule name
 :*                                                                    /)
 :*
 :*                                                             END-Pre
 :*   Post  (/ Let rdescrTab = trdescr if 'rule_name' is internal
 :*                          = tExtRuleDescr if 'rule_name' is external
 :*            if there exist a i0 such that
 :*             (A): 0<=i0<trdescr_lg and rdescrTab[i0].rule_name = 'rule_name'
 :*             (B): there exist a j0 such that
 :*                 - 0<=j0<rdescrTab[i0].ParNre + rdescrTab[i0].VarNre
 :*                 - tdescr[j0].name = 'name'
 :*            then
 :*               - 'pv'    = tdescr[j0].iname  ('par' or 'var')
 :*               - '*type' = tdescr[j0].tname  (its type)
 :*               - let
 :*                    - index  be rdescrTab[i0].nameDescr
 :*                    - size_i be size(tdescr[index+i].tname) (see size())
 :*                      for i: 0<=i<rdescrTab[i0].ParNre + rdescrTab[i0].VarNre
 :*                 the returned value is
 :*                      size_0 + ... + size_j0-1
 :*             otherwise, if condition (A) failed -2 is returned
 :*                        if condition (B) failed -1 is returned
 :*                                                                 /)
 :*
 :*                                                             END-Post
 :*  RETURNS  "int" (/ return -2 if unknown rule, -1 if unknown variable or
 :*                    parameter, otherwise, it returns an offset  /)
 :*
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY rdescr.getNameAddr
 :******************************************************************
*/
getNameAddr(name, rule_name, pv, type)
char *name, *rule_name;
enum var_par *pv;
enum type *type;
{
    int offset, base, i0, RAZ, max, j, j0, cmp, lgTab;
    rdescr rl_descr;
    namedescr nm_descr, *descrTab;
    rdescr *rdescrTab;

    if ((i0 = getRuleAddr(rule_name, &rdescrTab)) < 0)
	return (-2);
    rl_descr = rdescrTab[i0];
    base = rl_descr.nameDescr;
    max = rl_descr.ParNre + rl_descr.VarNre + base;
    if (max == 0) /* rule with no parameters or variables */
	return(-1);
    *pv = par;
    offset = -size(tdescr[base].tname);
    j = base - 1;
    RAZ = 0;
    do {
	j++;
	nm_descr = tdescr[j];
	if (nm_descr.iname == var && RAZ == 0) {
	    RAZ = 1;
	    *pv = var;
	    offset = 0;
	} else
	    offset += size(nm_descr.tname);
    } while (   (cmp = strcmp(nm_descr.name, name)) != 0
	     && j < max && j < tdescr_lg);
    if (cmp == 0) {
	*type = nm_descr.tname;
	return (offset);
    }
    return (-1);
}
/*
 :******************************************************************
 :* UNIT-BODY rdescr.getVarAddr
 :*
 :* TITLE         (/ get characteristics of a variable /)
 :*
 :* PURPOSE       (/ get the type and offset of a given name and indicate if
 :*                  this is a parameter, a local or a global variable
 :*                                                                   /)
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
 :*                    - trdescr[]   : internal rule descriptors table
 :*                    - tExtRuleDescr[] : external rule descriptors table
 :*                    - tdescr[]    : names (par; or var) descriptors table
 :*                    - tdescr_lg   : length of 'tdescr[]'
 :*                    - tdescr_ptr  : current index in 'tdescr[]'
 :*                    - rule_name   : name of the rule this name declaration
 :*                                    belongs to
 :*                    - tIntGlobal[]: internal global variable descriptors
 :*                    - tExtGlobal[]: external global variable descriptors
 :*                    - GV          : base address for global variables area
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
 :*   Pre   (/ - 'name' is a parameter or a local variable name
 :*            - 'rule_name' is a rule name
 :*                                                                    /)
 :*
 :*                                                             END-Pre
 :*   Post  (/ if 'rule_name' is internal let rdescrTab = 'trdescr' and
 :*               'lg' = trdescr_lg'
 :*            if 'rule_name' is external let rdescrTab = 'tExtRuleDescr' and
 :*               'lg' = tExtRule_lg'
 :*            if there exist a i0 such that
 :*             (A): 0<=i0<lg and rdescrTab[i0].rule_name = 'rule_name'
 :*             (B): there exist a j0 such that
 :*                 - 0<=j0<rdescrTab[i0].ParNre + rdescrTab[i0].VarNre
 :*                 - tdescr[j0].name = 'name'
 :*            then
 :*               - '*pv'    = tdescr[j0].iname  ('par' or 'var')
 :*               - '*type' = tdescr[j0].tname  (its type)
 :*               - let
 :*                    - index  be rdescrTab[i0].nameDescr
 :*                    - size_i be size(tdescr[index+i].tname) (see size())
 :*                      for i: 0<=i<rdescrTab[i0].ParNre + rdescrTab[i0].VarNre
 :*                 the returned value is
 :*                      size_0 + ... + size_j0-1
 :*            else let
 :*
 :*                     from = tModDescr[CurrModAddr].VarPtr;
 :*                     to = from + tModDescr[CurrModAddr].intVarNbre;
 :*               if there exist a k0 such that:
 :*               (C): from<= k0 < to and tIntGlobal[k0].varName = 'name'
 :*               then
 :*                 - '*pv' = intGlobal
 :*                 - '*type' = tIntGlobal[k0].type
 :*                 - the returned value is tIntGlobal[k0].addr - GV
 :*            else if there exist a k0 such that:
 :*               (D): 0<= k0 < tExtGlobal and tExtGlobal[k0].varName = 'name'
 :*               then
 :*                 - '*pv' = extGlobal
 :*                 - '*type' = tExtGlobal[k0].type
 :*                 - the returned value is tExtGlobal[k0].addr - GV
 :*
 :*            otherwise, if condition (A) failed -2 is returned
 :*                       if condition (B), (C) and (D) failed -1 is
 :*                       returned                                      /)
 :*
 :*                                                             END-Post
 :*  RETURNS  "int" (/ return -2 if unknown rule, -1 if unknown variable or
 :*                    parameter, otherwise, it returns an offset  /)
 :*
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY rdescr.getNameAddr
 :******************************************************************
*/
getVarAddr(name, rule_name, pv, type)
char *name, *rule_name;
enum var_par *pv;
enum type *type;
{
    int k, from, to;

    if ((k = getNameAddr(name, rule_name, pv, type)) == -2 || k >= 0)
	return (k);
    from = tModDescr[CurrModAddr].VarPtr;
    to = from + tModDescr[CurrModAddr].intVarNbre;
    k = getGlobNameAddr(internal, name, from, to); /* is it an internal var*/
    if (k < 0) {
	if ((k=getGlobNameAddr(external, name, 0, tExtGlobal_lg)) < 0) {

			      /* unknown name */
	    *type = undef;
	    *pv = par;        /* it does not matter which value */
	    return (k);
	} else {
			      /* external name */

	    *pv = extGlobal;
	    *type = tExtGlobal[k].type;
	    return (tExtGlobal[k].addr - GV);
	}
    } else {
	*pv = intGlobal;
	*type = tIntGlobal[k].type;
	return (tIntGlobal[k].addr - GV);
    }
}
/*
 :******************************************************************
 :* UNIT-BODY rdescr.getGlobNameAddr
 :*
 :* TITLE         (/ get characteristics of a global variable /)
 :*
 :* PURPOSE       (/ get the offset of a given global variable
 :*                                                                   /)
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
 :*                    - tIntGlobal[]: internal global variable descriptors
 :*                    - tExtGlobal[]: external global variable descriptors
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
 :*   Pre   (/ - 'name' is a parameter or a local variable name
 :*            - 'varclass' = 'internal or = external
 :*            - 0 <= from, to < max(tExtGlobal_lg, tIntGlobal_lg)
 :*                                                                    /)
 :*                                                             END-Pre
 :*
 :*   Post  (/ Let varDescr = tIntGlobal if 'varclass' = internal
 :*                varDescr = tIntGlobal if 'varclass' = internal
 :*               if there exist a k0 such that:
 :*               (C): from<= k0 < to and varDescr[k0].varName = 'name'
 :*               then
 :*                 - the returned value is k0
 :*            otherwise, if condition (C) failed -1 is returned
 :*                                                                /)
 :*                                                             END-Post
 :*
 :*  RETURNS  "int" (/ index in tIntGlobal[] or tExtGlobal[] or -1 /)
 :*
 :*
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY rdescr.getGlobNameAddr
 :******************************************************************
*/
getGlobNameAddr(varclass, name, from, to)
char *name;
int varclass, from, to;
{
    int i, k;
    varDescr *varDescrTab;

    varDescrTab = (varclass == internal)? tIntGlobal: tExtGlobal;
    i = from;
    while (i < to && (k = strcmp(name, varDescrTab[i++].varName)) != 0);
    if (k == 0) {
	return (i - 1);
    }
    return (-1);
}
/*
 :******************************************************************
 :* UNIT-BODY rdescr.getParType
 :*
 :* TITLE         (/ get the type of a rule parameter      /)
 :*
 :* PURPOSE       (/ get the type of a given parameter in a given rule. This
 :*                  rule is supposed to be already declared           /)
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
 :*                    - trdescr[]   : internal rule descriptors table
 :*                    - tExtRuleDescr[] : external rule descriptors table
 :*                    - tdescr[]    : names (par; or var) descriptors table
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
 :*   Pre    (/ - 'rl_name' is a rule name
 :*             - 'rank' is a parameter rank in the declaration of 'rl_name'
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post  (/ Let rdescrTab = trdescr and lg = trdescr_lg if 'rl_name' is
 :*                            internal
 :*                          = tExtRuleDescr and lg = tExtRule_lg if
 :*                            'rl_name' is external
 :*            if there exist a i0 such that
 :*             (A): 0<=i0<lg and rdescrTab[i0].rule_name = 'rl_name'
 :*             (B): rank >= rdescrTab[i0].ParNre
 :*            then the returned value is
 :*                   tdescr[rdescrTab[i0].nameDescr + rank].tname (the type)
 :*            otherwise, if condition (A) failed -2 is returned
 :*                       if condition (B) failed -1 is returned
 :*                                                                 /)
 :*
 :*                                                             END-Post
 :*  RETURNS  "int" (/ -2 if unknown rule, -1 if too many arguments
 :*                    or the type of the rank-th argument otherwise  /)
 :*
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY rdescr.getParType
 :******************************************************************
*/
getParType(rl_name, rank) /* return -2 if unknown rule -1 if too many arg.
			     or the type of the rank-th argument otherwise */
char *rl_name;
int rank;
{
	int i0;
	rdescr *rdescrTab;

	if (( i0 = getRuleAddr(rl_name, &rdescrTab)) < 0)
		return (-2);                   /* unknown rule */
	if (rank >= rdescrTab[i0].ParNre)
		return (-1);                   /* too many arguments */
	return (tdescr[rdescrTab[i0].nameDescr + rank].tname);
}
/*
 :******************************************************************
 :* UNIT-BODY rdescr.getParType1
 :*
 :* TITLE         (/ get the type of a rule parameter      /)
 :*
 :* PURPOSE       (/ get the type of a given parameter in a given rule. This
 :*                  rule is supposed to be already declared           /)
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
 :*                    - trdescr[]   : rule descriptors table
 :*                    - tExtRuleDescr[] : external rule descriptors table
 :*                    - tdescr[]    : names (par; or var) descriptors table
 :*                    - targ_types[]  : argument types table
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
 :*   Pre    (/ - 'rl_name' is a rule name
 :*             - 'rank' is a parameter rank in the declaration of 'rl_name'
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post  (/ Let rdescrTab = trdescr and lg = trdescr_lg if 'rl_name' is
 :*                            internal
 :*                          = tExtRuleDescr and lg = tExtRule_lg if
 :*                            'rl_name' is external
 :*            if there exist a i0 such that
 :*             (A): 0<=i0<lg and rdescrTab[i0].rule_name = 'rl_name'
 :*             (B): rank >= rdescrTab[i0].ParNre
 :*            then the returned value is
 :*              targ_types[rdescrTab[i0].nameDescr + rank].tname (the type)
 :*            otherwise, if condition (A) failed -2 is returned
 :*                       if condition (B) failed -1 is returned
 :*                                                                 /)
 :*
 :*                                                             END-Post
 :*  RETURNS  "int" (/ -2 if unknown rule, -1 if too many arguments
 :*                    or the type of the rank-th argument otherwise  /)
 :*
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY rdescr.getParType1
 :******************************************************************
*/
getParType1(rl_name, rank) /* return -2 if unknown rule -1 if too many arg.
			     or the type of the rank-th argument otherwise */
char *rl_name;
int rank;
{
	int i0;
	rdescr *rdescrTab;

	if (( i0 = getRuleAddr(rl_name, &rdescrTab)) < 0)
		return (-2);                   /* unknown rule */
	if (rank >= rdescrTab[i0].ParNre)
		return (-1);                   /* too many arguments */
	return (targ_types[rdescrTab[i0].targDescr + rank]);
}
/*
 :******************************************************************
 :* UNIT-BODY rdescr.add_hole
 :*
 :* TITLE         (/ add a hole related to a rule code       /)
 :*
 :* PURPOSE       (/ insert a new hole at the head of the list of holes
 :*                  related to a rule code                  /)
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
 :*                       - trdescr[]  : internal rule descriptors table
 :*                       - tExtRuleDescr[]: external rule descriptors table
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
 :*   Post  (/ Let rdescrTab = trdescr and if 'rl_name' is internal
 :*                          = tExtRuleDescr if 'rl_name' is external
 :*             - 'addr' is the address of a hole to be inserted:
 :*
 :*              'addr'
 :*                 |
 :*                ___
 :*               | h |
 :*                ---
 :*             - 'rule_name' is a rule name
 :*
 :*             rdescrTab[rule_addr].Code
 :*                |
 :*                ___               ___
 :*               | h1|---> ... --->| hn|
 :*                ---               ---
 :*                                                                  /)
 :*                                                             END-Pre
 :*   Post   (/
 :*             rdescrTab[rule_addr].Code
 :*                |
 :*                ___      ___               ___
 :*               | h |--->| h1|---> ... --->| hn|
 :*                ---      ---               ---
 :*                                                                   /)
 :*                                                             END-Post
 :*   RETURNS  NONE
 :*
 :*   REMARKS     (/  refer to <DOCUMENTS> for a detailed description of
 :*                   the internal representation (boxes).
 :*                                                         /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY rdescr.add_hole
 :******************************************************************
*/
add_hole(addr, rule_name)
hole *addr;
char *rule_name;
{
	int rule_addr;
	rdescr *rdescrTab;

	rule_addr = getRuleAddr(rule_name, &rdescrTab);
	*addr= (hole) rdescrTab[rule_addr].Code;
	rdescrTab[rule_addr].Code = (BoxPtr) addr;
}
/*
 :******************************************************************
 :* UNIT-BODY rdescr.getFieldNameAddr
 :*
 :* TITLE         (/ get a field name address      /)
 :*
 :* PURPOSE       (/ if the field name is already existent in the current
 :*                  record table add a new entry to this table and install
 :*                  a new list of holes with a single hole otherwise, insert
 :*                  a new hole in the list of holes related to this field
 :*                  name                                         /)
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
 :*                     - CR_table[] : the current record table
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
 :*   Pre    (/ - 's' is an identifier
 :*             - let ad_id_i = CR_table[i].ad_id (0<=i<CR_table_lg)
 :*             - let 'CR_table_lg' = CR_table_lg_0
 :*             - ad_id_0 < ... < ad_id_CR_table_lg_0
 :*             - the set of audit data ids is
 :*                   ID = {ad_id_0, ... ad_id_CR_table_lg_0}
 :*             - 'gamma' is the address of a hole to be inserted:
 :*
 :*              'gamma'
 :*                 |
 :*                ___
 :*               | h |
 :*                ---
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/  if 's' is a valid field name (i.e, there exist j such that
 :*                 mapping_table[j].fieldname = 's')
 :*                 let mapping_table[j].ad_id = id_0
 :*              then we have the following:
 :*                 - the set of audit data ids is
 :*                      ID U {id_0}
 :*                 - 'CR_table_lg' = CR_table_lg_0 if id_0 is in ID
 :*                                 = CR_table_lg_0 + 1 otherwise
 :*                 - ad_id_0 < ... < ad_id_CR_table_lg
 :*                 - the hole pointed by 'gamma' is at the head of the list
 :*                   of holes related to the field name 's'
 :*                 - the returned value is k such that
 :*                         CR_table[k].ad_id = id_0
 :*               otherwise, -1 is returned
 :*                                                                 /)
 :*
 :*                                                             END-Post
 :*   RETURNS  "int" (/ -1 if invalid field name or index of this field
 :*                     name in 'CR_table[]'                /)
 :*
 :*   REMARKS   (/ refer to <DOCUMENTS> for a detailed description of the
 :*                internal representation (boxes).
 :*                                                         /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY rdescr.getFieldNameAddr
 :******************************************************************
*/
getFieldNameAddr(s, gamma)
char *s;
hole *gamma;
{
	short ad_id;
	int i, install_ad(), getAuditDataId();
	unsigned short found;

	if ((ad_id = getAuditDataId(s)) != -1) {
		i = install_ad(ad_id, &found);
		insert_hole(gamma, i, found);
		return(i);
	}
	return (-1);     /* not a field name */
}
/*
 :******************************************************************
 :* UNIT-BODY rdescr.print_trdescr
 :*
 :* TITLE         (/ output the rule descriptors table content      /)
 :*
 :* PURPOSE       (/ the content of the table 'trdescr[]' is output to stdout
 :*                                                                  /)
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
 :*                     - trdescr[]  : internal rule descriptors table
 :*                     - tExtRuleDescr[]  : external rule descriptors table
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
 :*   Pre    (/ 'ruleclass' = internal or = external
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/ Let s = intRuleName if 'ruleclass' = internal
 :*                   = extRuleName if 'ruleclass' = external
 :*             the content of 'trdescr[]' is output to stdout in the
 :*             following format:
 :*                - a header:
 :*
 :* 's' | module | P.Nre| V.Nre| P.Sz| st | Code | targ | names
 :*        -------------------------------------------------------------
 :*                - a number (= 'trdescr_lg') of lines are output each
 :*                  representing a rule descriptor and containing the fields
 :*                  in the header above
 :*                                                                     /)
 :*                                                             END-Post
 :*   RETURNS   NONE
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY  rdescr.print_trdescr
 :******************************************************************
*/
print_trdescr(ruleclass)
int ruleclass;
{
	int i, lgTab;
	rdescr *rdescrTab;
	char *s;

	if (ruleclass == internal) {
	    rdescrTab = trdescr;
	    lgTab = trdescr_lg;
	    s = "IntRuleName";
	} else {
	    rdescrTab = tExtRuleDescr;
	    lgTab = tExtRule_lg;
	    s = "ExtRuleName";
	}
       if (!lgTab)
	    return;
 printf("\n%-16s| module | P.Nre| V.Nre| P.Sz| st | Code | targ | names\n", s);
 printf("---------------------------------------------------------------------\n");
	for (i=0; i<lgTab; i++) {
	   printf("%-16s|%-7d |  %-4d|  %-4d| %-4d| %2d |%4x |%4d  |%4d\n",
		 rdescrTab[i].rule_name,
		 rdescrTab[i].modulePtr, rdescrTab[i].ParNre,
		 rdescrTab[i].VarNre, rdescrTab[i].APASize,
		 rdescrTab[i].status, rdescrTab[i].Code,
		 rdescrTab[i].targDescr, rdescrTab[i].nameDescr);
	}
}
/*
 :******************************************************************
 :* UNIT-BODY rdescr.print_tdescr
 :*
 :* TITLE         (/ output the content of the table of name descriptors /)
 :*
 :* PURPOSE       (/ the content of the table 'tdescr[]' is output to stdout
 :*                                                                  /)
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
 :*                       - tdescr[]  : rule descriptors table
 :*                       - tdescr_ptr: current index in 'tdescr[]'
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
 :*   Post   (/ the content of 'tdescr[]' is output to stdout. This output
 :*             is a sequence of lines each one related to a parameter or a
 :*             local variable descriptor and have the following format:
 :*
 :*              name: 'name' pv: 'pv'  type: 'type'
 :*
 :*              where 'name' is the parameter or local variable name
 :*                    'pv'   = 0 if this is a parameter
 :*                           = 1 if this is a local variable
 :*                    'type' is in {integer, bytestring, fieldstring, undef}
 :*                     where fieldstring means an audit data
 :*                                                                     /)
 :*                                                             END-Post
 :*   RETURNS   NONE
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY rdescr.print_tdescr
 :******************************************************************
*/
print_tdescr()
{
	int i;

	if (!tdescr_lg)
	    return;
	printf("\nIndex |    Name   | par.var | type \n");
	printf(" ------------------------------------\n");
	for (i=0; i<tdescr_lg; i++)
	    printf("%-5d | %-9s | %-7d | %-4d\n", i, tdescr[i].name,
					   tdescr[i].iname, tdescr[i].tname);
}
/*
 :******************************************************************
 :* UNIT-BODY rdescr.print_targ_types
 :*
 :* TITLE         (/ output the content of the table of argument types/)
 :*
 :* PURPOSE       (/ the content of the table 'targ_types[]' is output to
 :*                  stdout                                           /)
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
 :*                       - targ_types[]  : argument types table
 :*                       - targ_types_ptr: current index in 'targ_types[]'
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
 :*   Post   (/ the content of 'targ_types[]' is output to stdout. This output
 :*             is a sequence of lines each one related to an argument type
 :*             and have the following format:
 :*
 :*              'i' type: 'type'
 :*
 :*              where 'i' is a counter
 :*                    'type' is in {integer, bytestring, fieldstring, undef}
 :*                     where fieldstring means an audit data
 :*             These line are preceeded by the folowing header line:
 :*
 :*                  Contents of targ_types table
 :*                                                                     /)
 :*                                                             END-Post
 :*   RETURNS   NONE
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY rdescr.print_targ_types
 :******************************************************************
*/
print_targ_types()
{
	int i;

	if (!targ_types_lg)
	    return;
	printf("\n\n Contents of targ_types table \n");
	for (i=0; i<targ_types_lg; i++)
	printf("\t %3d type: %d\n", i, targ_types[i]);
}
init_rdescr()
{

trdescr_lg = 0;        /* next free position in trdescr[] */
trdescr_ptr = -1;      /* current position in trdescr[]   */

tExtRule_lg = 0;             /* next free position in trdescr[]       */
tExtRule_ptr = -1;           /* current position in tExtRuleDescr[]   */

targ_types_lg = 0;             /* next free position in targ_types[]  */

tdescr_lg = 0;          /* next free position in tdescr[]      */

untyped = 0;            /* number of names awaiting for a type */


}
