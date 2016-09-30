/*
 :******************************************************************
 :* MODULE-DESCRIPTION
 :*
 :* TITLE         (/ the asax rule compiler /)
 :*
 :* NAME          rule_decl.c
 :*
 :* COPYRIGHT     (C)   SIEMENS NIXDORF INFORMATIONSSYSTEME AG  1992
 :*                     ALL RIGHTS RESERVED
 :*
 :* MEMORY-CLASS  6
 :*
 :* PROG-CONVENTS (/ <internal conventions (instance, names,
 :*                  external data, ...). None -> empty text>     /)
 :*
 :* PROCEDURE     rule_declaration
 :* REMARKS       (/ parsing a rule declaration /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     cpExtRules
 :* REMARKS       (/ copy external rule descriptors /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     parse_module
 :* REMARKS       (/ parse a rule module /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     allocateAreas
 :* REMARKS       (/ allocate area for global objects /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     parse_rules
 :* REMARKS       (/ parsing a sequence of rule declarations /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     fill_holes
 :* REMARKS       (/ filling a list of holes /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     fill_holesb
 :* REMARKS       (/ filling holes related to boxes /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     checkAllDecl
 :* REMARKS       (/ check if all rules were declared /)
 :* ENTRY         EXTERNAL
 :*
 :*-----------------------------------------------------------------
 :*
 :* VERSION       001
 :*   CRDATE      1992-11-03
 :*   AUTHOR      (/ Mounji Abdelaziz    , FUNDP /)
 :*   UPDATE      (/ original issue /)
 :*
 :* END-MODULE-DESCRIPTION
 :******************************************************************
*/
#include <stdio.h>              /* standard IO definitions */
#include "asax.h"             /* all shared declarations */

char *VA_ptr, *PA_ptr;   /* variable and parameter areas */
char *WA_ptr;		 /* the working arra 		 */
char *GV, *GV_ptr;	 /* global variable area base and current address */

/*
 :******************************************************************
 :* UNIT-BODY rule_decl.rule_declaration
 :*
 :* TITLE         (/ parsing a rule declaration  /)
 :*
 :* PURPOSE       (/ parse a sequence of symbols representing a rule
 :*                  declaration update global tables to
 :*                  contain descriptors for this declaration if no error
 :*                  is found or recover from this error otherwise /)
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
 :*                   - CurrModAddr   : index in tModDescr[] of the module
 :*                                     been parsed
 :*                   - tModDescr[]   : table of module descriptors
 :*                   - tIntGlobal[]  : internal variable descriptors table
 :*                   - tExtGlobal[]  : external variable descriptors table
 :*                   - tExtAux[]     : auxiliary table for external variable
 :*                                     names
 :*                   - trdescr[]     : internal rule descriptors table
 :*                   - tExtRuleDescr[]: external rule descriptors table
 :*                   - tdescr[]      : names (par; or var) descriptors table
 :*                   - targ_types[]  : argument types table
 :*                   - rule_name     : name of the rule been declared
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
 :*                 <rule_declaration><rest>
 :*               where
 :*                 - <rule_declaration> is a valid rule declaration (see BNF
 :*                                  syntax of rule declaration in DOCUMENTS)
 :*                 - <rest> is any character sequence
 :*               the effect is the following:
 :*                - the table of rule descriptors 'trdescr[]' or
 :*                 'tExtRuleDescr[]' is added a  new rule descriptor
 :*                  corresponding to this rule
 :*              - the table of parameter and variable descriptors 'tdescr[]'
 :*                is updated (see functions addGlobalVar() and put_type() in
 :*                  file rdescr.c for extensive details about this update)
 :*                - the address of the generated internal code is assigned
 :*                  to the field 'Code' of this rule descriptor
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
 :*   RETURNS  (/ the address of the rule internal code /)
 :*                                                       END-RETURN
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
 :*
 :*                                                         /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY rule_decl.rule_declaration
 :******************************************************************
*/
BoxPtr rule_declaration(Stop, lg)
int Stop[], lg;             /* the stops */
{
	int S1[Mlg], lg1;       /* auxiliary stops */
	int rule_addr;          /* rule descriptor address */
	BoxPtr p;               /* address of the returned internal code */
	holePtr tr_head;      /* list of holes related to this rule code   */
	holePtr head, tail;   /* list of holes inside this rule code       */
	rdescr *rdescrTab;    /* either 'trdescr' or 'tExRuleDescr'        */

	/* build stops */
	lg1 = merge(Stop, lg, ActionStop, lgAction, S1);
	RuleHeading(S1, lg1);                 /* parse rule heading */
	p = action(WA_ptr, Stop, lg, &head, &tail); /* parse action part */
	if (!ErrorFound && p != NULL) {
	    /* the remaining holes in the code must be filled with 0       */
	    fill_holesb(head, (BoxPtr) 0);
	}
	rule_addr = getRuleAddr(rule_name, &rdescrTab);
	tr_head = (holePtr) &rdescrTab[rule_addr].Code;
	if (!ErrorFound)
	    /* If previous rules triggered this one, fill their holes */
	    fill_holesb(tr_head, p);
	rdescrTab[rule_addr].status = declared; /* mark this rule declared */
	rdescrTab[rule_addr].modulePtr = CurrModAddr;
	return(p);
}
/*
 :******************************************************************
 :* UNIT-BODY rule_decl.cpExtRules
 :*
 :* TITLE         (/ copy external rules descriptors /)
 :*
 :* PURPOSE       (/ scan the table of internal rule descriptors 'trdescr'
 :*                  and copy all undeclared rules in the table of external
 :*                  rule descriptors                               /)
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
 :*                   - CurrModAddr   : index in tModDescr[] of the module
 :*                                     been parsed
 :*                   - tModDescr[]   : table of module descriptors
 :*                   - trdescr[]     : internal rule descriptors table
 :*                   - tExtRuleDescr[]: external rule descriptors table
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
 :*   Pre    (/ - 0<=CurrModDescr<tModDescr_lg
 :*             - tExtRule_lg = lg0
 :*             - tExtRule_ptr = ptr0
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/ Let l = tModDescr[CurrModDescr].rulePtr
 :*                 m = tModDescr[CurrModDescr].rulePtr + l
 :*             Let I = {ri1, ..., riN} be the set of entries in trdescr such
 :*             that:    l <= i1, ..., iN < m
 :*                      trdescr[i_j].status != declared
 :*             then we have:
 :*                 if tExtRule_lg < MaxRulez!Nr - #I
 :*                      tExtRuleDescr[lg0 + i] = trdescr[i_j]
 :*                      tExtRuleDescr[lg0 + i].Code = &trdescr[i_j].Code
 :*                      tExtRule_lg = lg0 + 1
 :*                      tExtRule_ptr = ptr0 + 1
 :*                 otherwise an error message (26) is sent to stdout
 :*                                                                 /)
 :*
 :*                                                             END-Post
 :*   RETURNS  NONE
 :*
 :*   REMARKS     (/
 :*                                                         /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY rule_decl.cpExtRules
 :******************************************************************
*/
cpExtRules()
{
    int i, l, m;

    l=tModDescr[CurrModAddr].RulePtr;
    m=l+tModDescr[CurrModAddr].ruleDeclNbre;
    for (i=l; i<m; i++) {
	if (trdescr[i].status != declared) {
	    if (tExtRule_lg < MaxRuleNr) {
		tExtRuleDescr[tExtRule_lg] = trdescr[i];
		tExtRuleDescr[tExtRule_lg].Code = (BoxPtr) &trdescr[i].Code;
		tExtRule_lg++;
		tExtRule_ptr++;
	    } else {
		error(fatal, 26); /* too much rule declarations */
	    }
       }
    }
}
/*
 :******************************************************************
 :* UNIT-BODY rule_decl.parse_module
 :*
 :* TITLE         (/ parse a rule module /)
 :*
 :* PURPOSE       (/ parse a sequence of symbols representing a list of module
 :*                  usage, global variable declaration and rule
 :*                  declaration. Update object descriptors
 :*                  these declarations and generate internal code if no
 :*                  error is found or recover from this error otherwise /)
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
 :*                   - CurrModAddr   : index in tModDescr[] of this module
 :*                   - CurrMod_name  : name of this module
 :*                   - exten         : ='asa' the default extension of
 :*                                     ASAX module names
 :*                   - rfp           : file descriptor of the file
 :*                                     containing this module
 :*                   - LineNbre      : the number of the current line of
 :*                                     the module been parsed
 :*                   - tModDescr[]   : table of module descriptors
 :*                   - tIntGlobal[]  : internal variable descriptors table
 :*                   - tExtGlobal[]  : external variable descriptors table
 :*                   - tExtAux[]     : auxiliary table for external variable
 :*                                     names
 :*                   - trdescr[]     : internal rule descriptors table
 :*                   - tExtRuleDescr[]: external rule descriptors table
 :*                   - tdescr[]      : names (par; or var) descriptors table
 :*                   - targ_types[]  : argument types table
 :*                   - rule_name     : name of the rule been declared
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
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/   if the CSS is of the form:
 :*                <module usage> ";"
 :*                <global variable declarations> ";"
 :*                <rule_decl_1> ";" ... ";" <rule_decl_N> "." <rest>
 :*               where
 :*                 - <module usage> is a possibly empty module usage
 :*                 - <global variable declarations> is a valid global
 :*                   variable declaration
 :*                 - N>=1  (N = m + n; n internal and m external)
 :*                 - <rule_decl_i> is a valid rule declaration (see BNF
 :*                                  syntax of rule declaration in DOCUMENTS)
 :*                                 (i=1, ..., N)
 :*                 - <rest> is any character sequence
 :*               the effect is the following:
 :*                - the table of rule descriptors 'trdescr[]' is added n
 :*                  new rule descriptors corresponding to this list of rules
 :*                - the table of rule descriptors 'trdescr[]' is added m
 :*                  new rule descriptors corresponding to this list of rules
 :*                - the table of parameter and variable descriptors 'tdescr[]'
 :*                  is updated (see functions add_name() and put_type() in
 :*                  file rdescr.c for extensive details about this update)
 :*                - the address of the generated internal code for each of
 :*                  these rules is assigned to the field 'Code' of this rule
 :*                  descriptor
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
 :*                   - refer to <DOCUMENTS> for a detailed description of
 :*                     the internal representation (boxes).
 :*
 :*                                                         /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY rule_decl.parse_module
 :******************************************************************
*/
parse_module()
{
    int S1[Mlg], lg1, S2[Mlg], lg2, S3[Mlg], lg3, S4[Mlg], lg4, S5[Mlg], lg5;
    int S6[Mlg], lg6;
	char *strcpy();
    char *strcat();
	extern char rulespath[];

	strcpy(CurrMod_name, rulespath);
	strcat(CurrMod_name, "\\");
    strcat(CurrMod_name, tModDescr[++CurrModAddr].module_name);
    strcat(CurrMod_name, exten);
    tModDescr[CurrModAddr].VarPtr = tIntGlobal_lg;
    tModDescr[CurrModAddr].RulePtr = trdescr_lg;
    if ((rfp = fopen(CurrMod_name, "r")) == NULL) { /* rule file */
	printf("cannot open %s\n", CurrMod_name);
	exit(1);
    }
    LineNbre = 1;
    get_char(rfp);
    getsymb(rfp);
    printf("         asax :     %s\n", CurrMod_name);
    insert_stop(period, Stop1, lgStop1, S6, &lg6);
    insert_stop(init_actionx, S6, lg6, S5, &lg5);
    insert_stop(rulex, S5, lg5, S4, &lg4);
    insert_stop(internalx, S4, lg4, S3, &lg3);
    insert_stop(externalx, S3, lg3, S2, &lg2);
    insert_stop(globalx, S2, lg2, S1, &lg1);
    usage(S1, lg1);
    globalVariables(S2, lg2);
    parse_rules(Stop1, lgStop1);
    fclose(rfp);
}
/*
 :******************************************************************
 :* UNIT-BODY rule_decl.allocateAreas
 :*
 :* TITLE         (/ allocate areas for global objects /)
 :*
 :* PURPOSE       (/ allocate areas for global objects used throughout the
 :*                  parsing and execution of rule modules
 :*                                                             /)
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
 :*                   - CurrMod_name  : name of the module currently parsed
 :*                   - PA_ptr        : base address of the parameter area
 :*                   - VA_ptr        : base address of the variable area
 :*                   - WA_ptr        : base address of the work area
 :*                   - rule_name     : name of the rule been parsed
 :*                   - GV            : base address of the global variable
 :*                                     area
 :*                   - GV_ptr        : next free position in the global
 :*                                     variable area
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
 :*   Pre    (/
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/ - a memory area is allocated to each of the variables with
 :*               the indicated size:
 :*                   - CurrMod_name  :
 :*                   - PA_ptr        : 150 bytes
 :*                   - VA_ptr        : 150 bytes
 :*                   - WA_ptr        : 150 bytes
 :*                   - rule_name     : ident_lg
 :*                   - GV            : MaxGV * sizeof(byte)
 :*             - GV_ptr = GV
 :*             - in case of allocation problem, an error message is sent to
 :*               and the whole program is aborted                  /)
 :*
 :*                                                             END-Post
 :*   RETURNS  NONE
 :*
 :*   REMARKS     (/
 :*                                                         /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY rule_decl.allocateAreas
 :******************************************************************
*/
allocateAreas()
{
    

		      /* 150 must be an adequate val */

    if ((PA_ptr = (char *) malloc(150*Byte)) == NULL)
	error(fatal, 25);
    if ((VA_ptr = (char *) malloc(150*Byte)) == NULL)
	error(fatal, 25);
    if ((WA_ptr = (char *) malloc(150*Byte)) == NULL)
	error(fatal, 25);
    if ((rule_name = (char *) malloc(ident_lg)) == NULL)
	error(fatal, 25);
    if ((CurrMod_name = (char *) malloc(1024)) == NULL)
	error(fatal, 25);
    if ((GV = (char *) malloc(MaxGV * Byte)) == NULL) {
	error(fatal, 25);
    } else {
	GV_ptr = GV;
    }
}
/*
 :******************************************************************
 :* UNIT-BODY rule_decl.parse_rules
 :*
 :* TITLE         (/ parsing a sequence of rule declarations /)
 :*
 :* PURPOSE       (/ parse a sequence of symbols representing a list of rule
 :*                  declaration update 'tExtRuleDescr[]', 'trdescr[]' ,
 :*                  'tdescr[]' and 'tModDescr[]' to contain descriptors
 :*                  for these declarations if no error is found or
 :*                  recover from this error otherwise /)
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
 :*                   - CurrModAddr   : index in tModDescr[] of this module
 :*                   - CurrMod_name  : name of this module
 :*                   - exten         : ='asa' the default extension of
 :*                                     ASAX module names
 :*                   - rfp           : file descriptor of the file
 :*                                     containing this module
 :*                   - LineNbre      : the number of the current line of
 :*                                     the module been parsed
 :*                   - tModDescr[]   : table of module descriptors
 :*                   - tIntGlobal[]  : internal variable descriptors table
 :*                   - tExtGlobal[]  : external variable descriptors table
 :*                   - tExtAux[]     : auxiliary table for external variable
 :*                                     names
 :*                   - trdescr[]     : internal rule descriptors table
 :*                   - tExtRuleDescr[]: external rule descriptors table
 :*                   - tdescr[]      : names (par; or var) descriptors table
 :*                   - targ_types[]  : argument types table
 :*                   - rule_name     : name of the rule been declared
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
 :*                <rule_decl_1> ";" ... ";" <rule_decl_N> <rest>
 :*               where
 :*                 - N>=1 (n internal rules and m external rules: N=n+m)
 :*                 - <rule_decl_i> is a valid rule declaration (see BNF
 :*                                  syntax of rule declaration in DOCUMENTS)
 :*                                 (i=1, ..., N)
 :*                 - <rest> is any character sequence
 :*               the effect is the following:
 :*              - the table of rule descriptors 'trdescr[]' is added n
 :*                new rule descriptors corresponding to this list of rules
 :*              - the table of rule descriptors 'tExtRulrDescr[]' is added m
 :*                new rule descriptors corresponding to this list of rules
 :*              - the table of parameter and variable descriptors 'tdescr[]'
 :*                is updated (see functions add_name() and put_type() in
 :*                file rdescr.c for extensive details about this update)
 :*              - the address of the generated internal code for each of
 :*                these rules is assigned to the field 'Code' of this rule
 :*                descriptor
 :*              - the CSS is <rest>
 :*           otherwise
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
 :*                   - refer to <DOCUMENTS> for a detailed description of
 :*                     the internal representation (boxes).
 :*
 :*                                                         /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY rule_decl.parse_rules
 :******************************************************************
*/
parse_rules(Stop, lg)
int Stop[], lg;
{
    int lg1, lg2, lg3, lg4, lg5, S1[Mlg], S2[Mlg], S3[Mlg], S4[Mlg], S5[Mlg];
    
    int NoRule;

    insert_stop(semicolon, Stop, lg, S1, &lg1);
    insert_stop(period, S1, lg1, S3, &lg3);
    lg2 = merge(S3, lg3, RuleStop, lgRule, S2);
    NoRule = TRUE;
    while (InStop(symb.ibsymb, RuleStop, lgRule) >= 0) {
	NoRule = FALSE;         /* there is at least one rule declaration */
	rule_declaration(S3, lg3);   /* parse a rule decl */
	storeBox(NULL);
	Expect(symb.ibsymb, S2, lg2);
    }
    if (NoRule)
	Expect(period, Stop, lg);
    SyntaxCheck(Stop, lg);
    cpExtRules();   /* all undeclared rules are moved to tExtRuleDescr[] */
}
/*
 :******************************************************************
 :* UNIT-BODY rdescr.fill_holes
 :*
 :* TITLE         (/ filling a list of holes /)
 :*
 :* PURPOSE       (/ given a list of holes and a value, fill these holes with
 :*                  this given value                             /)
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
 :*   Pre    (/- 'head' is the address of a list of holes:
 :*
 :*              'head'
 :*                |
 :*                _______               _______
 :*               | hole_1|---> ... --->| hole_n|
 :*                -------               -------
 :*
 :*               where - n>=0 (list may be empty)
 :*                     - hole_i is the address of the next hole in the list
 :*                       (i=1, ..., n)
 :*            - 'value' is the value to fill the holes with
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/ - for i=1, ..., n:  hole_i = 'value':
 :*
 :*              'head'
 :*                |
 :*                _______               _______
 :*               | value |     ...     | value |
 :*                -------               -------
 :*                                                                     /)
 :*                                                             END-Post
 :*   RETURNS   NONE
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY  rule_decl.fill_holes
 :******************************************************************
*/
fill_holes(head, value)
hole *head;
char **value;
{
    hole *Hole, nextHole;

    Hole = head;
    if (Hole == NULL)
	return;
    do {
	nextHole = *Hole;
	*Hole = (hole) value;
	Hole = (holePtr) nextHole;
    } while (Hole != (holePtr) NULL) ;
}
/*
 :******************************************************************
 :* UNIT-BODY rdescr.fill_holesb
 :*
 :* TITLE         (/ filling holes related to boxes /)
 :*
 :* PURPOSE       (/ given a list of holes and a box address, fill these
 :*                  holes with this given value                  /)
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
 :*   Pre    (/- 'head' is the address of a list of holes:
 :*
 :*              'head'
 :*                |
 :*                _______               _______
 :*               | hole_1|---> ... --->| hole_n|
 :*                -------               -------
 :*
 :*               where - n>=0 (list may be empty)
 :*                     - hole_i is the address of the next hole in the list
 :*                       (i=1, ..., n)
 :*            - 'value' is the address of a box to fill the holes with
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/ - for i=1, ..., n:  hole_i = 'value':
 :*
 :*              'head'
 :*                |
 :*                _______               _______
 :*               | value |     ...     | value |
 :*                -------               -------
 :*                                                                     /)
 :*                                                             END-Post
 :*   RETURNS   NONE
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY  rule_decl.fill_holesb
 :******************************************************************
*/
fill_holesb(head, value)
hole *head;
BoxPtr value;
{
	hole *Hole, nextHole;

	Hole = head;
	if (Hole == NULL) return;
	do {
		nextHole = *Hole;
		*Hole = (hole) value;
		Hole = (holePtr) nextHole;
	} while (Hole != (holePtr) NULL) ;
}
/*
 :******************************************************************
 :* UNIT-BODY rule_decl.checkAllDecl
 :*
 :* TITLE         (/ check if all rules were declared /)
 :*
 :* PURPOSE       (/ scan the table of rule descriptors 'trdescr[]' to check
 :*                  that all triggered rules were declared. Output error
 :*                  message otherwise                            /)
 :*                                                      END-PURPOSE
 :* VERSION       001
 :*   CRDATE      1992-12-02
 :*   AUTHOR      (/ Mounji Abdelaziz       , FUNDP              /)
 :*   UPDATE      (/ original issue /)
 :*
 :* REALIZATION   (/ <note, reference to other proc. (optional)> /)
 :*
 :*   CONTROL    (/
 :*
 :*                                                               /)
 :*   DATA       (/
 :*                 - tExtRuleDescr[]   : rule descriptors table
 :*                 - tExtRule_lg       : length of 'tExtRuleDescr[]'
 :*                 - tabErrType[]      : table of error types
 :*                 - errmsgtab[]       : table of error messages
 :*                 - LineNbre          : the number of the current line
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
 :*                                                                  /)
 :*                                                             END-Pre
 :*   Post   (/ - let Undeclared be the set of i (0<=i<tExtRule_lg) such that
 :*                   tExtRuleDescr[i].status != declared
 :*
 :*             - for j in Undeclared, the following message
 :*   "Semantics"       line: "undeclared rule": 'tExtRuleDescr[j].rule_name'
 :*               is output to stdout. where line is the line number
 :*                                                                 /)
 :*
 :*                                                             END-Post
 :*   RETURNS  NONE
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY rule_decl.checkAllDecl
 :******************************************************************
*/
checkAllDecl()
{
    int i;

    for(i = 0; i < tExtRule_lg; i++) {
	if (tExtRuleDescr[i].status != declared)
	    error1(semantics, 5, tExtRuleDescr[i].rule_name);
    }
}
init_rule_decl()
{
    		free(PA_ptr);
    		free(VA_ptr);
    		free(WA_ptr);
    		free(rule_name);
    		free(CurrMod_name);
    		free(GV);
}
