/*
 :******************************************************************
 :* MODULE-DESCRIPTION
 :*
 :* TITLE         (/ management of strings /)
 :*
 :* NAME          string.c
 :*
 :* COPYRIGHT     (C)   SIEMENS NIXDORF INFORMATIONSSYSTEME AG  1992
 :*                     ALL RIGHTS RESERVED
 :*
 :* MEMORY-CLASS  6
 :*
 :* PROG-CONVENTS (/ <internal conventions (instance, names,
 :*                  external data, ...). None -> empty text>     /)
 :*
 :* PROCEDURE     saveIntConst
 :* REMARKS       (/ save an integer constant /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     saveStrConst
 :* REMARKS       (/ save a string constant/)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     strcpy1
 :* REMARKS       (/ string copy  /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     printStr
 :* REMARKS       (/ print a string  /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     printIntConst
 :* REMARKS       (/ print the integer constants table /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     printStrConst
 :* REMARKS       (/ print the string constants table/)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     cpy_str
 :* REMARKS       (/ string copy /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     lenstr
 :* REMARKS       (/ get a string length /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     cmp_str
 :* REMARKS       (/ string comparison/)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     getExtStrRepr
 :* REMARKS       (/ get the external string representation /)
 :* ENTRY         EXTERNAL
 :*
 :*-----------------------------------------------------------------
 :*
 :* VERSION       001
 :*   CRDATE      1992-12-04
 :*   AUTHOR      (/ Mounji Abdelaziz    , FUNDP /)
 :*   UPDATE      (/ original issue /)
 :*
 :* END-MODULE-DESCRIPTION string.c
 :******************************************************************
*/
#include <stdio.h>
#include "asax.h"

hole *tabStrConst[lgStrConst];  /* table of strings constants */
int strConstPtr = 0;   /* next free position in tabStrConst[] */

int tabIntConst[lgIntConst];  /* table of integer constants */
int intConstPtr = 0; /* next free position in tabIntConst[] */
/*
 :******************************************************************
 :* UNIT-BODY string.saveIntConst
 :*
 :* TITLE         (/ save an integer constant  /)
 :*
 :* PURPOSE       (/ save an integer constant (encountered during
 :*                  compilation of the rules) in the integer constants
 :*                  table.                                          /)
 :*                                                      END-PURPOSE
 :* VERSION       001
 :*   CRDATE      1992-12-02
 :*   AUTHOR      (/ Mounji Abdelaziz       , FUNDP              /)
 :*   UPDATE      (/ original issue /)
 :*
 :* REALIZATION   (/ <note, reference to other proc. (optional)> /)
 :*
 :*   CONTROL    (/
 :*                                                               /)
 :*   DATA       (/
 :*                     - tabIntConst[]: table of integer constants
 :*                     - intConstPtr  : current length of 'tabIntConst[]'
 :*                     - lgIntConst   : maximum length of 'tabIntConst[]'
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
 :*   Pre    (/ - 'v' is an integer value
 :*             - 'intConstPtr' = lg_0
 :*                                                                     /)
 :*                                                             END-Pre
 :*   Post   (/ - if 'intConstPtr' < 'lgIntConst'
 :*                 - tabIntConst[lg_0] = v
 :*                 - 'intConstPtr' = lg_0 + 1
 :*                 - the returned value is ('tabIntConst' + 'intConstPtr')
 :*             - otherwise the following error message:
 :*                       "table of integer constants overflow"
 :*               is output on stdout
 :*                                                                 /)
 :*
 :*                                                             END-Post
 :*   RETURNS  "int *" (/ address of this constant  /)
 :*
 :*   REMARKS          (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY string.saveIntConst
 :******************************************************************
*/
int *saveIntConst(v)   /* saves an integer constant and return its address */
int v;
{
    int i, found;

    found = FALSE;
    i = 0;
    while (i<intConstPtr && !found) {
	if (tabIntConst[i++] == v)
	    found = TRUE;
    }
    if (found)
	return (tabIntConst + i - 1);
    else
	if (intConstPtr < lgIntConst) {
	    tabIntConst[intConstPtr] = v;
	    return (tabIntConst + intConstPtr++);
	} else {
	    error(fatal, 22);  /* too much integer constants */
	    return(NULL);
	}
}
/*
 :******************************************************************
 :* UNIT-BODY string.saveStrConst
 :*
 :* TITLE         (/ save a string constant  /)
 :*
 :* PURPOSE       (/ save a string constant (encountered during compilation)
 :*                  of the rules) in the string constants table.
 :*                                                               /)
 :*                                                      END-PURPOSE
 :* VERSION       001
 :*   CRDATE      1992-12-02
 :*   AUTHOR      (/ Mounji Abdelaziz       , FUNDP              /)
 :*   UPDATE      (/ original issue /)
 :*
 :* REALIZATION   (/ <note, reference to other proc. (optional)> /)
 :*
 :*   CONTROL    (/
 :*                                                               /)
 :*   DATA       (/
 :*                     - tabStrConst[]: table of integer constants
 :*                     - strConstPtr  : current length of 'tabStrConst[]'
 :*                     - lgStrConst   : maximum length of 'tabStrConst[]'
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
 :*   Pre    (/ - 's' is a string
 :*             - 'strlg' is the length of 's'
 :*             - 'strConstPtr' = lg_0
 :*                                                                     /)
 :*                                                             END-Pre
 :*   Post   (/ - if 'strConstPtr' < 'lgStrConst'
 :*                 - tabStrConst[lg_0] points to the area:
 :*
 :*                     | strlg |    string    |
 :*                      ----------------------
 :*                   where 'string' is a copy of the string 's'
 :*
 :*                 - 'strConstPtr' = lg_0 + 1
 :*                 - the returned value is ('tabStrConst' + 'strConstPtr')
 :*             - otherwise the following error message:
 :*                       "too much string constants"
 :*               is output on stdout
 :*                                                                 /)
 :*
 :*                                                             END-Post
 :*   RETURNS  "hole **" (/ address of this constant  /)
 :*
 :*   REMARKS          (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY string.saveStrConst
 :******************************************************************
*/
hole **saveStrConst(s, strlg) /* save a string constant and return its address */
char *s;
int strlg;
{
    char *cp, *p;
    hole *h_ptr;
    unsigned short *usp;
    int i, found;

    if ((cp = (char *) malloc(usz + strlg)) == 0) {
	error(fatal, 25);    /* memory allocation problem */
    } else {
	p = cp;
	usp = (unsigned short *) cp;
	*usp = (unsigned short) strlg;
	cp += usz;
	strcpy1(cp, strlg, s);
    }
    found = FALSE;
    i = 0;
    while (i<strConstPtr && !found) {
	if (cmp_str((hole *) p, tabStrConst[i++]) == 0)
	    found = TRUE;
    }
    if (found) {
	free(p);
	return (tabStrConst + i - 1);
    } else if (strConstPtr < lgStrConst) {
	tabStrConst[strConstPtr] = (hole *) p;
	return (tabStrConst + strConstPtr++);
    } else {
	  error(fatal, 23);  /* too much string constants */
	  return(NULL);
    }
}
/*
 :******************************************************************
 :* UNIT-BODY string.strcpy1
 :*
 :* TITLE         (/ string copy /)
 :*
 :* PURPOSE       (/ copy a string to another area, the null ending character
 :*                  being excluded.
 :*                                                               /)
 :*                                                      END-PURPOSE
 :* VERSION       001
 :*   CRDATE      1992-12-02
 :*   AUTHOR      (/ Mounji Abdelaziz       , FUNDP              /)
 :*   UPDATE      (/ original issue /)
 :*
 :* REALIZATION   (/ <note, reference to other proc. (optional)> /)
 :*
 :*   CONTROL    (/
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
 :*   Pre    (/ - 's1' is a string
 :*             - 'lg1' is the length of the string 's2'
 :*             - 's2' is the address of an allocated area of at least 'lg1'
 :*               bytes in size                                           /)
 :*                                                             END-Pre
 :*   Post   (/ - 'lg1' bytes are copied from 's1' to 's2'
 :*                                                                 /)
 :*
 :*                                                             END-Post
 :*   RETURNS NONE
 :*
 :*   REMARKS          (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY string.strcpy1
 :******************************************************************
*/
strcpy1(s1, lg1, s2) /* copy s2 to s1; the ending null character excluded */
char *s1, *s2;
int lg1;       /* length of the string s2 */
{
    int i = 1;

    while (i++ <= lg1)
	*s1++ = *s2++;
}
/*
 :******************************************************************
 :* UNIT-BODY lib.printStr
 :*
 :* TITLE         (/ print a string   /)
 :*
 :* PURPOSE       (/ print the value of a string on stdout /)
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
 :*   Pre    (/ - 'm' is a string in the form length-value:
 :*
 :*                        'm'
 :*                          |
 :*                         \|/
 :*                          |lg| string |
 :*                           -----------
 :*
 :*             - lg is the length of 'string'
 :*                                                                     /)
 :*                                                             END-Pre
 :*   Post   (/ - the string 'string' is output on stdout
 :*                                                                 /)
 :*
 :*                                                             END-Post
 :*   RETURNS NONE
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY lib.printStr
 :******************************************************************
*/
printStr(s)
hole *s;
{
    int i = 1;
    unsigned short lg, lenstr();
    char *cp;

    lg = lenstr(s);
    cp = (char *) s;
    cp += usz;
    while (i++ <= lg)
	printf("%c", *cp++);

}
/*
 :******************************************************************
 :* UNIT-BODY string.printIntConst
 :*
 :* TITLE         (/ print the integer constants table /)
 :*
 :* PURPOSE       (/ print the content of the table of integer constants
 :*                  (encountered during compilation of the rules).
 :*                                                               /)
 :*                                                      END-PURPOSE
 :* VERSION       001
 :*   CRDATE      1992-12-02
 :*   AUTHOR      (/ Mounji Abdelaziz       , FUNDP              /)
 :*   UPDATE      (/ original issue /)
 :*
 :* REALIZATION   (/ <note, reference to other proc. (optional)> /)
 :*
 :*   CONTROL    (/
 :*                                                               /)
 :*   DATA       (/
 :*                     - tabIntConst[]: table of integer constants
 :*                     - intConstPtr  : current length of 'tabIntConst[]'
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
 :*                                                                     /)
 :*                                                             END-Pre
 :*   Post   (/ - the values tabIntConst[0], ..., tabIntConst[intConstPtr-1]
 :*               are printed on stdout
 :*                                                                 /)
 :*
 :*                                                             END-Post
 :*   RETURNS NONE
 :*
 :*   REMARKS          (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY string.printIntConst
 :******************************************************************
*/
printIntConst()
{
	int i;

	printf("\n");
	for (i = 0; i < intConstPtr; i++)
		printf("%2d: %4d ", i, tabIntConst[i]);
	printf("\n");
}
/*
 :******************************************************************
 :* UNIT-BODY string.printStrConst
 :*
 :* TITLE         (/ print the string constants table /)
 :*
 :* PURPOSE       (/ print the content of the table of string constants
 :*                  (encountered during compilation of the rules).
 :*                                                               /)
 :*                                                      END-PURPOSE
 :* VERSION       001
 :*   CRDATE      1992-12-02
 :*   AUTHOR      (/ Mounji Abdelaziz       , FUNDP              /)
 :*   UPDATE      (/ original issue /)
 :*
 :* REALIZATION   (/ <note, reference to other proc. (optional)> /)
 :*
 :*   CONTROL    (/
 :*                                                               /)
 :*   DATA       (/
 :*                     - tabStrConst[]: table of integer constants
 :*                     - strConstPtr  : current length of 'tabStrConst[]'
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
 :*                                                                     /)
 :*                                                             END-Pre
 :*   Post   (/ - the strings tabStrConst[0], ..., tabStrConst[strConstPtr-1]
 :*               are printed on stdout
 :*                                                                 /)
 :*
 :*                                                             END-Post
 :*   RETURNS NONE
 :*
 :*   REMARKS          (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY string.printStrConst
 :******************************************************************
*/
printStrConst()
{
	int i;

	printf("\n");
	for (i = 0; i < strConstPtr; i++) {
		printStr(tabStrConst[i]);
	}
	printf("\n");
}
/*
 :******************************************************************
 :* UNIT-BODY string.cpy_str
 :*
 :* TITLE         (/ string copy /)
 :*
 :* PURPOSE       (/ copy a sequence of bytes from one location to another /)
 :*                                                      END-PURPOSE
 :* VERSION       001
 :*   CRDATE      1992-12-02
 :*   AUTHOR      (/ Mounji Abdelaziz       , FUNDP              /)
 :*   UPDATE      (/ original issue /)
 :*
 :* REALIZATION   (/ <note, reference to other proc. (optional)> /)
 :*
 :*   CONTROL    (/
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
 :*   DOCUMENTS       (/                                          /)
 :*
 :*                                                  END-REALIZATION
 :* PARAMETERS
 :*
 :*   Pre    (/ - 'from' is a pointer to a string
 :*
 :*                         'from'
 :*                          |
 :*                         \|/
 :*                          |lg| string |
 :*                           -----------
 :*
 :*             - lg is the length of 'string'
 :*             - 'to' is a pointer to an allocated area  of at least
 :*               ('lg' + size of "short int") bytes in length
 :*                                                          /)
 :*                                                             END-Pre
 :*   Post   (/ - the ('lg' + size of "short int")  bytes starting from
 :*               'from' are copied to the area pointed by 'to':
 :*
 :*                         'to'
 :*                          |
 :*                         \|/
 :*                          |lg| string |
 :*                           -----------
 :*
 :*                                                                 /)
 :*                                                             END-Post
 :*   RETURNS   NONE
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY string.cpy_str
 :******************************************************************
*/
cpy_str(to, from)
hole *to, *from;
{
    unsigned short *usp, *usp1;
    char *c0, *c1, *c;

    usp = (unsigned short *) from;
    usp1 = (unsigned short *) to;
    *usp1 = *usp;
    c0 = c1 = (char *) from;
    c = (char *) to;
    c += usz;
    c0 += usz;
    c1 += usz;
    while (c1 < c0 + (*usp)) {
	*c++ = *c1++;
    }
}
/*
 :******************************************************************
 :* UNIT-BODY string.lenstr
 :*
 :* TITLE         (/ get a string length /)
 :*
 :* PURPOSE       (/ get the length of a string /)
 :*                                                      END-PURPOSE
 :* VERSION       001
 :*   CRDATE      1992-12-02
 :*   AUTHOR      (/ Mounji Abdelaziz       , FUNDP              /)
 :*   UPDATE      (/ original issue /)
 :*
 :* REALIZATION   (/ <note, reference to other proc. (optional)> /)
 :*
 :*   CONTROL    (/
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
 :*   DOCUMENTS       (/                                          /)
 :*
 :*                                                  END-REALIZATION
 :* PARAMETERS
 :*
 :*   Pre    (/ - 's' is a pointer to a string
 :*
 :*                         's'
 :*                          |
 :*                         \|/
 :*                          |lg| string |
 :*                           -----------
 :*
 :*             - lg is the length of 'string'
 :*                                                          /)
 :*                                                             END-Pre
 :*   Post   (/ - the value 'lg' is returned
 :*                                                                 /)
 :*                                                             END-Post
 :*   RETURNS   "unsigned short" (/ the string length /)
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY string.lenstr
 :******************************************************************
*/
unsigned short lenstr(s)
hole *s;
{
    unsigned short *usp;

    usp = (unsigned short *) s;
    return (*usp);
}
/*
 :******************************************************************
 :* UNIT-BODY string.cmp_str
 :*
 :* TITLE         (/ string comparaison /)
 :*
 :* PURPOSE       (/ compare two strings represented in the form length-value
 :*                  using the lexicographic order for characters.
 :*                                                                /)
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
 :*   Pre    (/ - 's' and 't' are strings:
 :*
 :*                         's'
 :*                          |
 :*                         \|/
 :*                          | lgs |  str_s    |
 :*                           ------------------
 :*
 :*                         't'
 :*                          |
 :*                         \|/
 :*                          | lgt |  str_t    |
 :*                           ------------------
 :*
 :*             - lgs is the length of 'str_s'
 :*             - lgt is the length of 'str_t'
 :*                                                                     /)
 :*                                                             END-Pre
 :*   Post   (/ - let s_1 s_2 ... s_lgs and t_1 t_2 ... t_lgt be the sequence
 :*               of characters respectively in 'str_s' and 'str_t'
 :*             - let c_1 ... c_j0 be the longuest prefix common to 'str_s'
 :*               and 'str_t'  (0<=j0<=lgs, lgt)
 :*             - if j0 = lgs or j0 = lgt the returned value is (lgs - lgt)
 :*               otherwise the returned value is (s_j0+1 - t_j0+1)
 :*                                                                 /)
 :*                                                             END-Post
 :*   RETURNS "int" (/ the result of the lexicographic comparaison /)
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY string.cmp_str
 :******************************************************************
*/
cmp_str(s, t)
hole *s, *t;
{
    unsigned short i, *usps, *uspt;  /* to get the length of each string */
    char *cps, *cpt;   /* character pointers to scan the two strings */

    cps = (char *) s;
    cpt = (char *) t;
    usps = (unsigned short *) s;
    uspt = (unsigned short *) t;

    cps += usz;
    cpt += usz;
    i = 0;
    while (i != *usps && i != *uspt && *cps == *cpt) {
	i++;
	cps++;
	cpt++;
    }
    if (i == *usps || i == *uspt) {
	return(*usps - *uspt);
    } else {
	return(*cps - *cpt);
    }
}
/*
 :******************************************************************
 :* UNIT-BODY string.getExtStrRepr
 :*
 :* TITLE         (/ get the external string representation/)
 :*
 :* PURPOSE       (/ This function receives a string represented in the form
 :*                  length-value and returns the corresponding C
 :*                  representation of this string                /)
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
 :*   Pre    (/ - 'h' is a string represented in the form:
 :*
 :*                         'h'
 :*                          |
 :*                         \|/
 :*                          | lgh |  str_h    |
 :*                           ------------------
 :*
 :*             - lgh is the length of 'str_h'
 :*                                                                     /)
 :*                                                             END-Pre
 :*   Post   (/ - A memory area of size ('lgh' + 1) is allocated and the
 :*               string 'str_h' is copied in it ended by the null character
 :*               '\0'. A pointer to this area is returned
 :*                                                                 /)
 :*                                                             END-Post
 :*   RETURNS "char *" (/ the created external string representation /)
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY string.getExtStrRepr
 :******************************************************************
*/
char *getExtStrRepr(h)
hole *h;
{
    unsigned short ui = 0, lg, *psh;
    char *m, *m1, *cp;

    m1 = (char *) h;
    m1 += usz;
    lg = lenstr(h);
    //if ((m = tmalloc(lg + 1, char)) == NULL)

// WARNING: This is a quick and DIRTY fix, because I don't know how to correct a bug that triggers here for the moment
// It __must__ be changed asap.
// The bug comes from higher levels of abstraction, perhaps even from the NADF file, and its effect is hole pointing to
// a very long string of garbage (unitialized memory)

    if (lg>1000 || (m = tmalloc(lg + 1, char)) == NULL)

// WARNING

	error(fatal, 25);  /* memory allocation problem */
    else {
	cp = m;
	while (ui++ != lg) {
	    *cp++ = *m1++;
	}
	*cp ='\0';
    }
    return(m);
}
char *getExtStrRepr1(h)
hole *h;
{
    unsigned short ui = 0, lg, *psh;
    char *m, *m1, *cp;

    m = m1 = (char *) h;
    psh = (unsigned short *) m1;
    m1 += usz + *psh;
    *m1 = '\0';
    return(m+usz);
/*     lg = lenstr(h);
    if ((m = (char *) tmalloc(lg + 1, char)) == NULL)
	error(fatal, 25); */ /* memory allocation problem */
/*    else {
	cp = m;
	while (ui++ != lg) {
	    *cp++ = *m1++;
	}
	*cp ='\0';
    }
    return(m);
*/
}
init_string()
{

    /* strConstPtr:  next free position in tabStrConst[] */
    intConstPtr = 0; /* next free position in tabIntConst[] */
    while (strConstPtr > 0) {
	free(tabStrConst[--strConstPtr]);
    }
}


