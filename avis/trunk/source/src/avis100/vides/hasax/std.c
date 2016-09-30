/*
 :******************************************************************
 :* MODULE-DESCRIPTION
 :*
 :* TITLE         (/ management routines for predefined function and
 :*                  procedure descriptors table     /)
 :* NAME          std.c
 :*
 :* COPYRIGHT     (C)   SIEMENS NIXDORF INFORMATIONSSYSTEME AG  1992
 :*                     ALL RIGHTS RESERVED
 :*
 :* MEMORY-CLASS  6
 :*
 :* PROG-CONVENTS (/ <internal conventions (instance, names,
 :*                  external data, ...). None -> empty text>     /)
 :*
 :* PROCEDURE     getfpAddr
 :* REMARKS       (/ get a predefined function or procedure descriptor
 :*                  address /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     getfpPtr
 :* REMARKS       (/ get a predefined function or procedure address    /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     getArgPass
 :* REMARKS       (/ get the passing type of an argument               /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     getArgType
 :* REMARKS       (/ get the type of an argument                       /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     getArgType
 :* REMARKS       (/ get the type of an argument                       /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     getArity
 :* REMARKS       (/ get the a predefined routine arity			/)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     isSpecial						
 :* REMARKS       (/ is this a variable argument length routine		/)
 :* ENTRY         EXTERNAL
 :*
 :*-----------------------------------------------------------------
 :*
 :* VERSION       001
 :*   CRDATE      1992-12-02
 :*   AUTHOR      (/ Mounji Abdelaziz    , FUNDP /)
 :*   UPDATE      (/ original issue /)
 :*
 :* END-MODULE-DESCRIPTION std.c
 :******************************************************************
*/

#include "predef.h"
#ifdef BS2000
#include "BS_lib.h"
#include "BS_proto.h"
#else
#include "_PROTO__.h"
#include "_ASAX__LIB.h"
#endif

/*
 :******************************************************************
 :* UNIT-BODY std.getfpAddr
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
 :* END-BODY std.getfpAddr
 :******************************************************************
*/

getfpAddr(s)    /* return the entry int the standard library table corresponding
		   to the function or procedure name s; (-1) otherwise */
char *s;
{
	int low, high, mid, k;

	low = 0;
	high = tfp_descr_lg - 1;
	while (low <= high) {
		mid = (low + high) / 2;
		if ((k = strcmp(s, tfp_descr[mid].name)) < 0)
			high = mid - 1;
		else if (k > 0)
			low = mid + 1;
		else /* std function or procedure is found */
			return (mid);
	}
	return (-1);
}
/*
 :******************************************************************
 :* UNIT-BODY std.getfpPtr
 :*
 :* TITLE         (/ get a predefined function or procedure address /)
 :*
 :* PURPOSE       (/ get the index of a given function or procedure in
 :*                  'tfp_descr[]'                                  /)
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
 :*                  - tfp_descr[]  : table of descriptors for predefined
 :*                                   functions and procedures
 :*                  - tfp_descr_lg : length of 'tfp_descr[]'
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
 :*             name                                                    /)
 :*                                                             END-Pre
 :*   Post   (/ if there exist a i such that:
 :*                    tfp_descr[i].name = 's'
 :*             then the returned value is tfp_descr[i].fp_ptr
 :*             -1 is returned otherwise
 :*                                                                 /)
 :*
 :*                                                             END-Post
 :*   RETURNS  "retfp" (/ if >= 0 this is a function or procedure address;
 :*                       -1 if unknown function or procedure             /)
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY std.getfpPtr
 :******************************************************************
*/
retfp getfpPtr(s)
char *s;
{
	int i;

	if ((i = getfpAddr(s)) >= 0)
		return (tfp_descr[i].fp_ptr);
	else  return((retfp) -1);   /* unknown function or procedure */
}
/*
 :******************************************************************
 :* UNIT-BODY std.getArgPass
 :*
 :* TITLE         (/ get the passing type of an argument               /)
 :*
 :* PURPOSE       (/ get the passing type (by value or by name) of a given
 :*                  argument in a given function or procedure         /)
 :*
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
 :*                    - tfp_descr[]   : table of descriptors for functions
 :*                                      or procedures
 :*                    - tfp_descr_lg  : length of 'tfp_descr[]'
 :*                    - targ_descr[]  : table of descriptors for function
 :*                                      procedure arguments
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
 :*   Pre    (/ - 'fpname' is a predefined function or procedure name
 :*             - 'rank' is the rank of an argument in 'fpname' argument list
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post  (/ if there exist a i0 such that
 :*             (A): 0<=i0<tfp_descr_lg and tfp_descr[i0].name = 'fpname'
 :*                 if this is a variable arity function or procedure (i.e.,
 :*                 tfp_descr[i0].narg = 'undefArity'), the returned value is
 :*                                  tfp_descr[i0].argp
 :*                 which is the common passing type for all arguments.
 :*                 Otherwise if (B): rank >= tfp_descr[i0].narg the returned
 :*                 value is targ_descr[tfp_descr[i0].argp + rank].tpass
 :*            otherwise, if condition (A) failed -1 is returned
 :*                       if condition (B) failed -2 is returned
 :*                                                                 /)
 :*
 :*                                                             END-Post
 :*  RETURNS  "int" (/ -1 if unknown function or procedure , -2 if too many
 :*                    arguments, or the passing type of the rank-th argument
 :*                    otherwise                                    /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY std.getArgPass
 :******************************************************************
*/
getArgPass(fpname, rank)   /* returns fpname rank-th argument passing type */
char *fpname;   /* a procedure or a function name */
int rank;       /* rank of the argument in the argument list */
{
	int i0;

	if ((i0 = getfpAddr(fpname)) < 0)
		return (-1);    /* unknown function or procedure */
	else if (tfp_descr[i0].narg == undefArity)
		return(tfp_descr[i0].argp);
	else if (rank < tfp_descr[i0].narg)
		return (targ_descr[tfp_descr[i0].argp + rank].tpass);
	else return (-2);       /* too many arguments */
}
/*
 :******************************************************************
 :* UNIT-BODY std.getArgType
 :*
 :* TITLE         (/ get the type of an argument               /)
 :*
 :* PURPOSE       (/ get the type of a given argument in a given function or
 :*                  procedure                                 /)
 :*
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
 :*                    - tfp_descr[]   : table of descriptors for functions
 :*                                      or procedures
 :*                    - tfp_descr_lg  : length of 'tfp_descr[]'
 :*                    - targ_descr[]  : table of descriptors for function
 :*                                      procedure arguments
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
 :*   Pre    (/ - 'fpname' is a predefined function or procedure name
 :*             - 'rank' is the rank of an argument in 'fpname' argument list
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post  (/ if there exist a i0 such that
 :*             (A): 0<=i0<tfp_descr_lg and tfp_descr[i0].name = 'fpname'
 :*                 if this is a variable arity function or procedure (i.e.,
 :*                 tfp_descr[i0].narg = 'undefArity'), the returned value is
 :*                                 'undef'
 :*                 because for such functions this cannt be known at compile
 :*                 time;
 :*                 Otherwise if (B): rank >= tfp_descr[i0].narg the returned
 :*                 value is targ_descr[tfp_descr[i0].argp + rank].targ
 :*            otherwise, if condition (A) failed -1 is returned
 :*                       if condition (B) failed 'undef' is returned
 :*                                                                 /)
 :*
 :*                                                             END-Post
 :*  RETURNS  "rettype" (/ -1 if unknown function or procedure, 'undef' if
 :*                    too many arguments, or the type of the rank-th argument
 :*                    otherwise                                    /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY std.getArgType
 :******************************************************************
*/
getArgType(fpname, rank)
char *fpname;               /* a procedure or a function name */
int rank;                   /* rank of the argument in the argument list */
{
	int i0;

	if ((i0 = getfpAddr(fpname)) < 0)
		return (-1);    /* unknown function or procedure */
	else if (tfp_descr[i0].narg == undefArity)
		return(undef);
	else if (rank < tfp_descr[i0].narg) {
		return (targ_descr[tfp_descr[i0].argp + rank].targ);
	} else {
		return (undef); /* too many arguments */
	}
}
/*
 :******************************************************************
 :* UNIT-BODY rdescr.getfpType
 :*
 :* TITLE         (/ get the returned type of a predefined function or
 :*                  procedure                                         /)
 :*
 :* PURPOSE       (/ get the type of the returned value of a given function
 :*                  or procedure                                      /)
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
 :*                    - tfp_descr[]   : table of descriptors for functions
 :*                                      or procedures
 :*                    - tfp_descr_lg  : length of 'tfp_descr[]'
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
 :*   Pre    (/ - 'fp_name' is a predefined function or procedure name
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post  (/ if there exist a i0 such that
 :*             (A): 0<=i0<tfp_descr_lg and tfp_descr[i0].name = 'fpname'
 :*            then the returned value is
 :*                   tfp_descr[i0].].rettype (the returned type)
 :*            otherwise, if condition (A) failed 'undef' is returned
 :*                                                                 /)
 :*
 :*                                                             END-Post
 :*  RETURNS  "int" (/ 'undef' if unknown function or procedure or the type
 :*                    of the returned value otherwise  /)
 :*
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY std.getfpType
 :******************************************************************
*/
getfpType(fpname)  /* returns the returned type of fpname */
char *fpname;   /* a procedure or a function name */
{
	int i0;

	if ((i0 = getfpAddr(fpname)) < 0)
		return (undef);    /* unknown function or procedure */
	else         {
		return (tfp_descr[i0].rettype);
	}
}
/*
 :******************************************************************
 :* UNIT-BODY rdescr.getfpType
 :*
 :* TITLE         (/ get the arity of a predefined routine           /)
 :*
 :* PURPOSE       (/ get the arity of the  given function
 :*                  or procedure                                      /)
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
 :*                    - tfp_descr[]   : table of descriptors for functions
 :*                                      or procedures
 :*                    - tfp_descr_lg  : length of 'tfp_descr[]'
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
 :*   Pre    (/ - 'fp_name' is a predefined function or procedure name
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post  (/ The returned value is
 :*                   tfp_descr[getfpAddr(fpname)].narg
 :*                                                                 /)
 :*
 :*                                                             END-Post
 :*  RETURNS  "int" (/ the predefined routine arity   /)
 :*
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY std.getArity
 :******************************************************************
*/

getArity(fpname)
char *fpname;     /* function or procedure name */
{
    return (tfp_descr[getfpAddr(fpname)].narg);
}
/*
 :******************************************************************
 :* UNIT-BODY rdescr.isSpecial
 :*
 :* TITLE         (/ is this a variable argument length routine  /)
 :*
 :* PURPOSE       (/ return TRUE if this is a variable argument length  
 :* 		     routine; FALSE otherwise                   /)
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
 :*                    - tfp_descr[]   : table of descriptors for functions
 :*                                      or procedures
 :*                    - tfp_descr_lg  : length of 'tfp_descr[]'
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
 :*   Pre    (/ - 'address' is a predefined function or procedure address
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post  (/ The returned value is TRUE if 
 :*                   (tfp_descr[address].narg = undefArity
 :*            otherwise, the returned value is FALSE
 :*                                                                 /)
 :*
 :*                                                             END-Post
 :*  RETURNS  "int" (/ TRUE or FALSE  				  /)
 :*
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY std.getfpType
 :******************************************************************
*/
isSpecial(address)
int address;      /* function or procedure address */
{
     return ((tfp_descr[address].narg == undefArity)? TRUE : FALSE);
}
