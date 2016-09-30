/* ASAX.C - 07/23/97 by Hooman Vassef
            07/23/97: last modification

  This file was originally standard.c in the asaxw (ASAX adapted to Windows by Bill/Riad) source.
  I replaced main() and Run_Code() by asax_init( ), asax( ), asax_complete( ).
  This way, the asaxw files and the bochs files can be unified in one program.

  asax_init initializes the expert system by loading the asax module and the descriptor file

  asax runs the expert system on one given record and returns

  asax_complete is to be run at the end to execute the completion rules
  
*/



/*
*	List of updates by 
* 	Mounji Abdelaziz
* 	Institut d'Informatique 
*	Rue Grangagnage, 21
*	5000 Namur
*
*	rdescr.c 
*
* Wed Jan 26 15:59:21 WET 1994  Mounji
* in function main_asax (line: 875)
* the audit data descriprtion file was not closed !!!
* added fclose(descr_fp) 
*/
/*
 :******************************************************************
 :* MODULE-DESCRIPTION
 :*
 :* TITLE         (/ the main program of the evaluator in the standard mode /)
 :*
 :* NAME          standard.c
 :*
 :* COPYRIGHT     (C)   SIEMENS NIXDORF INFORMATIONSSYSTEME AG  1992
 :*                     ALL RIGHTS RESERVED
 :*
 :* MEMORY-CLASS  6
 :*
 :* PROG-CONVENTS (/ <internal conventions (instance, names,
 :*                  external data, ...). None -> empty text>     /)
 :*
 :* PROCEDURE     main
 :* REMARKS       (/ the main function             /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     compile_module
 :* REMARKS       (/ compilation of rule module   /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     runCode
 :* REMARKS       (/ running the abstract machine   /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     pushInitActions
 :* REMARKS       (/ push all init action code in the active rules list /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     getInitDescr
 :* REMARKS       (/ initialize a descriptor for init action   /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     process
 :* REMARKS       (/ processing a list of active rules               /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     process_rule
 :* REMARKS       (/ processing an active rule    /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     emulate_code
 :* REMARKS       (/ emulate a rule internal code /)
 :* ENTRY         EXTERNAL
 :*
 :*-----------------------------------------------------------------
 :*
 :* VERSION       001
 :*   CRDATE      1992-11-03
 :*   AUTHOR      (/ Mounji Abdelaziz    , FUNDP /)
 :*   UPDATE      (/ original issue /)
 :*
 :* END-MODULE-DESCRIPTION standard.c
 :******************************************************************
*/

#include <string.h>

#ifdef BS2000
	#include <sys.time.h>
#endif
#ifdef VMS
	#include <time.h>
#else
//#include <sys/time.h>
#endif

#include "NADF_flag.h"
#include "asax.h"  /* all data structure declarations and prototypes */

#define mode standard	/* this file is to run in the standard mode */
char *Heap[3000];
int Heap_lg = 0;

extern int intConstPtr, strConstPtr;
extern hole *tabStrConst[];
extern int tabIntConst[];

int CurrModAddr = -1;
int tModDescr_lg = 0;
char exten[5] = ".asa";	 /* default extension for rule modules */

extern int CR_table_lg;
extern CR_table_entry CR_table[Max_ad_id]; /* from stack.c */

char *rule_name;	/* name of the rule been parsed */
char *CurrMod_name ;	/* name of the module been parsed */
int LineNbre = 1;	/* line number of the line been parsed */

int ErrorFound = FALSE;	/* TRUE if an error was found in the rule module */

/* initialize stops */

int Stop1[lgStop1]       = {nosymbx};
int AddStop[lgAdd]       = {add, minus};
int ParameterStop[lgParamStop] = { lparenthesis};
int VariableStop[lgVarStop] = { varx};
int ParStop[lgParStop] = { identifierx};
int TermStop[lgTerm]     = {lparenthesis, add, minus, identifierx, integerxx,
	C_literalx, X_literalx};
int FactorStop[lgFact]   = {lparenthesis, identifierx, integerxx,
	C_literalx, X_literalx};
int MultStop[lgMult]     = {divx, modx, mult};
int ActParStop[lgActPar] = {lparenthesis, minus, identifierx, integerxx,
	C_literalx, X_literalx};
int TrModeStop[lgTrMode] = {at_completionx, for_currentx, for_nextx};
int CondStop[lgCond] = {falsex, notx, presentx, truex, lparenthesis,
	add, minus, identifierx, integerxx};
int ActionStop[lgAction] = {beginx, dox, ifx, skipx, triggerx, identifierx};
int RuleStop[lgRule] = {externalx, init_actionx, internalx, rulex};
int relStop[lgRel] = {neq, less, leq, equal, greater, geq, eqx};


/* File descriptors */

FILE *descr_fp;					/* the audit data description file           */
FILE *rfp;							/* the file containing the text of the rules */
char *Curr_Rec_Addr;				/* Current Audit Record Address              */

/* stacks of triggered rules descriptors lists heads */

trigD *current, *next, *compl;

unsigned short ZERO = 0;
hole *NOTFOUND = (hole *) &ZERO;	/* riad */

/* Hooman: here, asax_init, asax, asax_complete */

int rep_fp;


int numactiverules;		// to count the number of active rules run during one asax() call (memory usage debugging)
int lastnumactiverules=0;

char rulespath[256];

int asax_init(char *descrfile, char *module_name, char *reportfile)
{
	hole *getInitRuleCode();
	static int NotfirstCall;
	char *bslash;

	strncpy(rulespath, module_name, 255);
	rulespath[255] = '\0';
	bslash = strrchr(rulespath, '\\');

	if (bslash) {
		*bslash++ = '\0';
		module_name = bslash;
	}
	else {
		strcpy(rulespath, ".");
	}

	if(NotfirstCall)
	{
		init_parse();
		init_standard();
		init_globalVar();
		init_printbox();
		init_rdescr();
		init_stack();
		init_rule_decl();
	}

	NotfirstCall = 1;
	/* parse options if any */
	if((descr_fp = fopen(descrfile, "r")) == NULL)
	{/* data description file */
		printf("cannot open %s for read\n", descrfile);
		return(1);
	}
	if((rep_fp = fopen(reportfile, "w")) == NULL)
	{/* report file */
		printf("cannot open %s for write\n", reportfile);
		return(1);
	}
	printf("begin parsing description file ... \n");
	parse_descrip(descr_fp);
	fclose(descr_fp);
	compile_module(TRUE, module_name);
	while(Heap_lg)
		free(Heap[--Heap_lg]);
		
	/*		if (!ErrorFound) {
				  printf("\n    ***** Some Useful Addresses ****\n\n");
				  printf("      Working Area: ____________ %x\n", WA_ptr);
				  printf("      Parameter Area: __________ %x\n", PA_ptr);
				  printf("      Variable Area: ___________ %x\n", VA_ptr);
				  printf("      Global Variable Area: ____ %x\n", GV);
				  printf("      Current Record table: ____ %x\n", CR_table);
				  printf("      lgIntConst = %d Ptr = %d\n", lgIntConst, intConstPtr);
				  printf("      lgStrConst = %d Ptr = %d\n", lgStrConst, strConstPtr);
				  if (intConstPtr)
						printf("      Integer Constants Area: __ %x\n", tabIntConst);
				  if (strConstPtr)
						printf("      String  Constants Area: __ %x\n", tabStrConst);
				  if (intConstPtr) {
						printf("\n    ***** Integer Constants Area *****\n");
						printIntConst();
				  }
				  if (strConstPtr) {
						printf("\n    ***** String Constants Area *****\n");
						printStrConst();
				  }
				  print_CR_table();
				  printCodes();
			} */
		
	current = next = compl = NULL;
	pushInitActions();
	if(current)
	{
		process(for_current1);
		current = next;
		next = NULL;
	}
	return(ErrorFound);
}

int asax(char *record_buffer)
{
	if(current)
	{
	numactiverules = 0;						// Reset rules counter

	Curr_Rec_Addr = record_buffer;
	resetIsWritten();
	scanCR(CR_table, CR_table_lg, Curr_Rec_Addr);
	process(for_current1);					// Count the rules while we run them
	current = next;
	next = NULL;

	if (numactiverules > lastnumactiverules) {
		lastnumactiverules = numactiverules;
	}

	lastnumactiverules = numactiverules;	// Keep the number of rules having been processed so that a rule
											// can show it during the next asax() call

	return(0);
	}
	else return(1);
}

int asax_complete(char *record_buffer)
{
	Curr_Rec_Addr = record_buffer;
	resetIsWritten();
	process(at_completion1);
	return(0); /* Successful completion */
}


/*
 :******************************************************************
 :* UNIT-BODY  standard.compile_module
 :*
 :* TITLE         (/ compilation of a rule module       /)
 :*
 :* PURPOSE       (/ compile a rule module and all modules it includes
 :*                  	                                              /)
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
  :*                (Only the main global data structures are listed here )
 :*
 :*                  - descr_fp       : file pointer of the Audit Data
 :*                                     Description file
 :*                  - rfp            : file pointer of the rule file
 :*                  - dfp            : NADF file descriptor
 :*                  - mapping_table[]: table that maps field names to
 :*                                     their corresponding audit data id
 :*                  - CR_table[]     : the current record table
 :*                  - Curr_Rec_Addr  : pointer to the current audit record
 :*                  - trdescr[]      : internal rule descriptors table
 :*                  - tExtRuleDescr[]: external rule descriptors table
 :*                  - tdescr[]       : names (par; or var) descriptors
 :*				        table
 :*                  - targ_type[]    : parameter types for undeclared rules
 :*                  - tfp_descr[]    : table of descriptors for functions
 :*                                     or procedures
 :*                  - targ_descr[]   : table of descriptors for function
 :*                                     procedure arguments
 :*                  - all tables of stops (see stops.h )
 :*                  - symb           : holds the internal representation
 :*                                     of the last read symbol.
 :*                  - ErrorFound     : = TRUE if an error was found
 :*                                     = FALSE otherwise
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
 :*   Pre    (/ - 
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/ if compileAll = TRUE, the module 'module_name and all
 :*                it includes are compiled and internal code generated
 :* 		   for them. Any error in these modules is reported 
 :*             if compileAll = FALSE, only the module 'module_name' is  
 :*                compiled and internal code generated for it
 :* 		     	         				        /)
 :*                                                             END-Post
 :*   RETURNS NONE
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY  standard.compile_module
 :******************************************************************
*/
compile_module(compileAll, module_name)
int compileAll;
char *module_name;
{
	int i;

	allocateAreas();
	i=0;
	add_module(module_name);
	while(i++ < tModDescr_lg && !ErrorFound)
	{
		parse_module();
		if(!compileAll)
			break;
	}
	fill_CR_holes();
	if(!ErrorFound)
		checkAllDecl();		  /* check if all rules have been declared */
}
/*
 :******************************************************************
 :* UNIT-BODY standard.pushInitActions
 :*
 :* TITLE         (/ push all init action codes in the active rules list /)
 :*
 :* PURPOSE       (/ Scan the table of rule descriptors, build a descriptor
 :* 		     for any found init action and push it in the list of rules
 :* 		     to be active for the next (first) NADF record.   /)
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
 :*			- trdescr[]: table of internal rules descriptors 
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
 :*   Pre    (/ - Let init_action_1, ..., init_action_N (N>=0) be the 
 :*		  be the set of init actions found in trdescr[];
 :*   	         - Let init_descr_1, ..., init_descr_N (N>=0) be the 
 :*		   be the set of init actions descriptors corresponding to these
 :*		    rules; 
																				 /)
 :*                                                             END-Pre
 :*   Post   (/
 :*   current
 :*      |
 :*       _____________      _____________               _____________
 :*      | init_descr_1|--->| init_descr_2|---> ... --->| init_descr_n|
 :*       -------------      -------------               -------------
 :*
 :*                                                                  /)
 :*                                                             END-Post
 :*   RETURNS  NONE
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY standard.pushInitActions
 :******************************************************************
*/
pushInitActions()
{
	int i;
	trigD *getInitDescr();

	for(i=0; i<trdescr_lg; i++)
	{
		if(strcmp(trdescr[i].rule_name, "init_action") == 0)
			push(getInitDescr(trdescr[i].Code), for_current1);
	}
}
/*
 :******************************************************************
 :* UNIT-BODY  standard.getInitDescr
 :*
 :* TITLE         (/ initialize a descriptor for the init action /)
 :*
 :* PURPOSE       (/ Build a descriptor with the received rule code;
 :*                  return the address of this descriptor /)
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
 :*   Pre    (/ - 
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/ - A memory area of size:
 :*                  sizeof(trigD *)
 :*               is allocated and initialized as follows:
 :*
 :*             p0
 :*             |
 :*             |
 :*            ____________________
 :*           | size | code | next |
 :*            --------------------
 :*
 :*          where
 :*             - size = 0
 :*             - code 'code'
 :*             - next = NULL
 :*             - param = NULL
 :*                                                                   /)
 :*                                                             END-Post
 :*   RETURNS  "trigD *" (/ the built descriptor address /)
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY  standard.getInitDescr
 :******************************************************************
*/
trigD *getInitDescr(code)
BoxPtr code;
{

	trigD *trD;

	if((trD = (trigD *) malloc(sizeof(trigD))) == NULL)
	{;
		error(fatal, 25); /* allocation problem */
	}
	trD->size = 0;
	trD->code = code;
	trD->next = NULL;
	trD->param = NULL;
	return (trD);
}
/*
 :******************************************************************
 :* UNIT-BODY  standard.process
 :*
 :* TITLE         (/ processing a list of active rules       /)
 :*
 :* PURPOSE       (/ receives a pointers to a list of active rules. Execute
 :*                  each cell (a rule) of this list with respect to the
 :*                  language semantics                           /)
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
 :*                  - CR_table[]     : the current record table
 :*                  - Curr_Rec_Addr  : pointer to the current audit record
 :*                  - dfp            : NADF file descriptor
 :*                  - current        : list of rule active for the current
 :*                                     record
 :*                  - next           : list of rule active for the next
 :*                                     record
 :*                  - compl          : list of rule active at completion
 :*                                                                /)
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
 :*   Pre    (/ 'tr_m' is in {for_current1, for_next1, at_completion1}
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/- let 'p' be the list of active rules corresponding to 'tr_m'.
 :*              the rules in this list are executed against the current
 :*              record in the order they occur in this list and according
 :*              to the language semantics
 :*                                                         /)
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY  standard.process
 :******************************************************************
*/
process(tr_m)
enum operation tr_m;
{
	trigD *pop(), *p;

	while((p = pop(tr_m)) != NULL)
	{
		process_rule(p);
		numactiverules++;
	}
}
/*
 :******************************************************************
 :* UNIT-BODY   standard.process_rule
 :*
 :* TITLE         (/ processing an active rule   /)
 :*
 :* PURPOSE       (/ Given an active rule descriptor, execute its internal
 :*                  code with respect to the actual parameter values and the
 :*                  current record in accordance with the language semantics
 :*                                                               /)
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
 :*                  - CR_table[]     : the current record table
 :*                  - Curr_Rec_Addr  : pointer to the current audit record
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
 :*   Pre    (/ 'ruleDescr' is a pointer to a valid active rule descriptor
 :*            'ruleDescr'
 :*             |
 :*             |
 :*            __________________________
 :*           | lg | A_P_V | code | next |
 :*            --------------------------
 :*
 :*          where
 :*             - lg = is the length of the descriptor (length field exculded)
 :*             - A_P_V is a possibly empty list of actual parameter values
 :*             - code is the internal code of "init_rule"
 :*             - next = address of the next descriptor in the list
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/ - the area A_P_V is copied to the area pointed by 'PA_tr'
 :*               which is the parameter area;
 :*             - the code pointed by 'code' is executed with respect to the
 :*               actual parameter area, the current record and in accordance
 :*               with the language semantics;
 :*             - the memory allocated for this descriptor is released.
 :*                                                                   /)
 :*                                                             END-Post
 :*   RETURNS   NONE
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY  standard.process_rule
 :******************************************************************
*/
process_rule(ruleDescr)
trigD *ruleDescr;
{
	/*char *c, *from;
	hole *p;
	int *ip, lg, size;*/

	if(ruleDescr->size)
//		copy_bytes((char *) ruleDescr->param, PA_ptr, ruleDescr->size);
		memcpy(PA_ptr, (char *) ruleDescr->param, ruleDescr->size);
	if(ruleDescr->code != NULL)
		emulate_code(ruleDescr->code);
	if(ruleDescr->size)
		free(ruleDescr->param);
	free(ruleDescr);
}
/*
 :******************************************************************
 :* UNIT-BODY  standard.emulate_code
 :*
 :* TITLE         (/ emulate a rule internal code      /)
 :*
 :* PURPOSE       (/ execute a rule internal
 :*                  code with respect to the actual parameter values and the
 :*                  current record in accordance with the language semantics
 :*                                                               /)
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
 :*                  - CR_table[]     : the current record table
 :*                  - Curr_Rec_Addr  : pointer to the current audit record
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
 :*   Pre    (/ 'p' is a valid box address
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/ - the code pointed by 'p' is executed with respect to the
 :*               actual parameter area, the current record and in accordance
 :*               with the language semantics;
 :*                                                                   /)
 :*                                                             END-Post
 :*   RETURNS   NONE
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY  standard.emulate_code
 :******************************************************************
*/
emulate_code(p)
BoxPtr p;
{
	BoxPtr emulate();

	while((p = emulate(p)) != NULL);
	while(Heap_lg)
		free(Heap[--Heap_lg]); 
}
init_standard()
{

	CurrModAddr = -1;
	tModDescr_lg = 0;

	LineNbre = 1;		  /* line number of the line been parsed */

	ErrorFound = FALSE; /* TRUE if an error was found in the rule module */


}




/*
 :******************************************************************
 :* UNIT-BODY   standard.main
 :*
 :* TITLE         (/ the main function      /)
 :*
 :* PURPOSE       (/ Open files, parse Audit Data Description file and
 :*                  compile rule modules. In absence of errors, execute
 :*                  generated internal code on the NADF file otherwise,
 :*                  report encountered errors. Close all files /)
 :*                                                      END-PURPOSE
 :* VERSION       001
 :*   CRDATE      1992-12-07
 :*   AUTHOR      (/ Mounji Abdelaziz       , FUNDP              /)
 :*   UPDATE      (/ original issue /)
 :*
 :* REALIZATION   (/ <note, reference to other proc. (optional)> /)
 :*
 :*   CONTROL    (/
 :*
 :*                                                               /)
 :*   DATA       (/
 :*                (Only the main global data structures are listed here )
 :*
 :*                  - descr_fp       : file pointer of the Audit Data
 :*                                     Description file
 :*                  - rfp            : file pointer of the rule file
 :*                  - dfp            : NADF file descriptor
 :*                  - mapping_table[]: table that maps field names to
 :*                                     their corresponding audit data id
 :*                  - CR_table[]     : the current record table
 :*                  - Curr_Rec_Addr  : pointer to the current audit record
 :*                  - trdescr[]      : internal rule descriptors table
 :*                  - tExtRuleDescr[]: external rule descriptors table
 :*                  - tdescr[]       : names (par; or var) descriptors
 :*				        table
 :*                  - targ_type[]    : parameter types for undeclared rules
 :*                  - tfp_descr[]    : table of descriptors for functions
 :*                                     or procedures
 :*                  - targ_descr[]   : table of descriptors for function
 :*                                     procedure arguments
 :*                  - all tables of stops (see stops.h )
 :*                  - symb           : holds the internal representation
 :*                                     of the last read symbol.
 :*                  - ErrorFound     : = TRUE if an error was found
 :*                                     = FALSE otherwise
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
 :*   Post   (/
 :*         If argc = 3 and
 :*            - argv[1] = valid Audit Data Description file name
 :*            - argv[2] = valid rule file name
 :*            - argv[3] = valid NADF file name
 :*            - the file argv[2] and all rule modules it includes are
 :*              correct with respect to their syntax
 :*         Then
 :*            the rules are executed according to the language semantics
 :*            and execution performance reported
 :*         If argc = 3 and
 :*            - argv[1] = -l
 :*            - argv[2] = valid Audit Data Description file name
 :*            - argv[3] = valid rule file name
 :*         Then
 :*            - only the file named argv[2] is parsed and checked if 
 :*              correct with respect to the syntax. the  rules are not
 :*		 executed
 :*         If argc = 3 and
 :*            - argv[1] = -g
 :*            - argv[2] = valid Audit Data Description file name
 :*            - argv[3] = valid rule file name
 :*         Then
 :*            - the file named argv[2] and all rule modules it includes
 :* 	 	  are  parsed and checked if 
 :*              correct with respect to the syntax. the  rules are not
 :*		 executed
 :*         Otherwise, a list of errors is reported on stdout         /)
 :*                                                             END-Post
 :*   RETURNS NONE
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY standard.main
 :******************************************************************
*/
/*
 :******************************************************************
 :* UNIT-BODY  standard.runCode
 :*
 :* TITLE         (/ running the abstract machine       /)
 :*
 :* PURPOSE       (/ Execute the generated internal code against the NADF
 :*                  file                                              /)
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
 :*                  - CR_table[]     : the current record table
 :*                  - Curr_Rec_Addr  : pointer to the current audit record
 :*                  - dfp            : NADF file descriptor
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
 :*   Pre    (/ - 'dfp' is the file descriptor of the NADF file open in
 :*               "READ" mode
 :*             - the table of rule descriptors 'trdescr[]' contains at least
 :*               the rule descriptor for the rule named "init_rule"
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/ the internal code corresponding to "init_rule" is executed
 :*             according to the language semantics                   /)
 :*                                                             END-Post
 :*   RETURNS NONE
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY  standard.runCode
 :******************************************************************
*/
