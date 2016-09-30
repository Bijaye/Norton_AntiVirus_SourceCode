/*
 :******************************************************************
 :* MODULE-DESCRIPTION
 :*
 :* TITLE         (/ error output    /)
 :*
 :* NAME          error.c
 :*
 :* COPYRIGHT     (C)   SIEMENS NIXDORF INFORMATIONSSYSTEME AG  1992
 :*                     ALL RIGHTS RESERVED
 :*
 :* MEMORY-CLASS  6
 :*
 :* PROG-CONVENTS (/ <internal conventions (instance, names,
 :*                  external data, ...). None -> empty text>     /)
 :*
 :* PROCEDURE     error
 :* REMARKS       (/ output error message  /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     error1
 :* REMARKS       (/ output extended error message  /)
 :* ENTRY         EXTERNAL
 :*
 :*-----------------------------------------------------------------
 :*
 :* VERSION       001
 :*   CRDATE      1992-12-02
 :*   AUTHOR      (/ Mounji Abdelaziz    , FUNDP /)
 :*   UPDATE      (/ original issue /)
 :*
 :* END-MODULE-DESCRIPTION error.c
 :******************************************************************
*/
#include <stdio.h>
#include "asax.h"
#define ErrTypes 4

	/* in case an error is encountered in the RE to be parsed, this macro */
	/* is used to report the corresponding error */


struct  code_map {
	unsigned int RE_code;
	unsigned int ASAX_code;
};

#define Maxlg 12
static struct code_map code_mapping[Maxlg] = {
			{11, 29},
			{16, 30},
			{25, 31},
			{36, 32},
			{41, 33},
			{42, 34},
			{43, 35},
			{44, 36},
			{45, 37},
			{46, 38},
			{49, 39},
			{50, 40}
				              };

static char *tabErrType[ErrTypes] = {
		 "Internal", "Lexical", "Syntax", "Semantics"};

#define ErrMsgs 44
static char *errmsgtab[ErrMsgs] =  {

/* 0  */		"type name expected", 
/* 1  */		"unknown type",
/* 2  */		"identifier expected",
/* 3  */		"unexpected symbol",
/* 4  */		"semicolon expected",
/* 5  */		"undefined rule",
/* 6  */		"unknown identifier",
/* 7  */		"undefined function or procedure",
/* 8  */		"check arity",
/* 9  */		"error in expression",
/* 10 */        	"type mismatch",
/* 11 */		"not a left value",
/* 12 */		"not a field name",
/* 13 */		"action expected",
/* 14 */		"function not a procedure!",
/* 15 */		"procedure not a function!",
/* 16 */		"redeclared rule",
/* 17 */		"too much global variables",
/* 18 */		"redeclaration of ",
/* 19 */		"requiring no parenthesis for ",
/* 20 */		"too much external variables per a file",
/* 21 */		"too much internal variables",
/* 22 */		"too much integer constants ",
/* 23 */		"too much string constants ",
/* 24 */		"bad data ",
/* 25 */		"out of memory",
/* 26 */		"too much rule declarations",
/* 27 */		"too much local variables and parameters",
/* 28 */		"too much module inclusions",
/* 29 */		"Range endpoint too large",
/* 30 */		"Bad number",
/* 31 */		"``\\ digit'' out of range",
/* 32 */		"Illegal or missing delimiter",
/* 33 */		"No remembered search string",
/* 34 */		"\\( \\) imbalance",
/* 35 */		"Too many \\(",
/* 36 */		"More than 2 numbers given in \\{ \\}", 
/* 37 */		"} expected after \\",
/* 38 */		"First number exceeds second in \\{ \\}",
/* 39 */		"[] imbalance",
/* 40 */		"Regular expression too long",
/* 41 */ 		"warning: insuffisant size of the buffer",
/* 42 */		"illegal option: ",
/* 43 */		"string constant too long"
				   };
/*
 :******************************************************************
 :* UNIT-BODY error.error
 :*
 :* TITLE         (/ output error message /)
 :*
 :*
 :* PURPOSE       (/ output the type, the line and the message of an error
 :*                  identified by its error code    /)
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
 :*                 - tabErrType[]: table of error types
 :*                 - errmsgtab[] : table of error messages
 :*                 - LineNbre    : the number of the current line
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
 :*   Pre    (/ - 'errType' an integer value identifying the error type
 :*             - 'errorcode' an integer value identifying the error message
 :*                                                                  /)
 :*                                                             END-Pre
 :*   Post   (/ - let type    = tabErrType[errType] be the error type
 :*                   line    = 'LineNbre'
 :*                   message = errmsgtab[errorcode]
 :*              - the following line is output to stdout:
 :*             type              line: message
 :*                                                                 /)
 :*
 :*                                                             END-Post
 :*   RETURNS  NONE
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY error.error
 :******************************************************************
*/

error(errType, errorcode)
int errType, errorcode;
{
	ErrorFound = TRUE;
	printf("%-20s%-9s%3d: %s\n", CurrMod_name,
		 tabErrType[errType], LineNbre,
		 errmsgtab[errorcode]);
	if (errType == fatal)
	    exit(1);
}
/*
 :******************************************************************
 :* UNIT-BODY error.error1
 :*
 :* TITLE         (/ output extended error message /)
 :*
 :*
 :* PURPOSE       (/ output the type, the line and the message of an error
 :*                  identified by its error code then output the erronuous
 :*                  token                                              /)
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
 :*                 - tabErrType[]: table of error types
 :*                 - errmsgtab[] : table of error messages
 :*                 - LineNbre    : the number of the current line
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
 :*   Pre    (/ - 'errType' an integer value identifying the error type
 :*             - 'errorcode' an integer value identifying the error message
 :*             - 'errStr' is the erronuous token
 :*                                                                  /)
 :*                                                             END-Pre
 :*   Post   (/ - let type    = tabErrType[errType] be the error type
 :*                   line    = 'LineNbre'
 :*                   message = errmsgtab[errorcode]
 :*              - the following line is output to stdout:
 :*             type              line: message  'errStr'
 :*                                                                 /)
 :*
 :*                                                             END-Post
 :*   RETURNS  NONE
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY error.error1
 :******************************************************************
*/
error1(errType, errorcode, errStr)
int errType, errorcode;
char *errStr;
{

	ErrorFound = TRUE;
	if (errorcode != 5) {
		if (CurrMod_name) 
			printf("%-20s", CurrMod_name);
		printf("%-9s%3d: %s: %s\n",
		tabErrType[errType],
		LineNbre,
		errmsgtab[errorcode], errStr);
	} else {
		printf("%-20s%-9s: %s: %s\n", "ld: ",
		tabErrType[errType],
		errmsgtab[errorcode], errStr);
	}
	if (errType == fatal)
	    exit(1);
}

/*
 :******************************************************************
 :* UNIT-BODY regular.getErrCode
 :*
 :* TITLE         (/ get a predefined function or procedure descriptor
 :*                  address                                        /)
 :*
 :* PURPOSE       (/ get the index in 'tfp_descr[]' corresponding to
 :*                  to a given predefined function or procedure    /)
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
 :*                 - tfp_descr[]  : table of descriptors for predefined
 :*                                  functions and procedures
 :*                 - tfp_descr_lg : length of 'tfp_descr[]'
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
 :*   Pre    (/ 's' a string representing a predefined function or procedure
 :*             name                                                     /)
 :*                                                             END-Pre
 :*   Post   (/ if there exist a i such that:
 :*                    tfp_descr[i].name = 's'
 :*             then i is returned
 :*             -1 is returned otherwise
 :*                                                                 /)
 :*
 :*                                                             END-Post
 :*   RETURNS  "int" (/ if >= 0 index of 's' in 'tfp_descr[]'
 :*                     -1 if unknown function or procedure         /)
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY regular.getErrCode
 :******************************************************************
*/

getErrCode(c)    
int c;
{
	int low, high, mid, k;

	low = 0;
	high = Maxlg - 1;
	while (low <= high) {
		mid = (low + high) / 2;
		if (c < code_mapping[mid].RE_code)
			high = mid - 1;
		else if (c > code_mapping[mid].RE_code)
			low = mid + 1;
		else /* std function or procedure is found */
			return (code_mapping[mid].ASAX_code);
	}
	return (-1);
}
