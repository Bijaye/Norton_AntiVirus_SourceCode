/*
 :******************************************************************
 :* MODULE-DESCRIPTION
 :*
 :* TITLE         (/ library predefined procedures and functions   /)
 :*
 :* NAME          lib.c
 :*
 :* COPYRIGHT     (C)   SIEMENS NIXDORF INFORMATIONSSYSTEME AG  1992
 :*                     ALL RIGHTS RESERVED
 :*
 :* MEMORY-CLASS  6
 :*
 :* PROG-CONVENTS (/ <internal conventions (instance, names,
 :*                  external data, ...). None -> empty text>     /)
 :*
 :* PROCEDURE     strToInt
 :* REMARKS       (/ conversion of a string to an integer  /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     println
 :* REMARKS       (/ output a list of arguments and a new line /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     print
 :* REMARKS       (/ output a list of arguments /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     printI
 :* REMARKS       (/ output of an integer /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     printIln
 :* REMARKS       (/ output of an integer and a new line  /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     printS
 :* REMARKS       (/ output of a string /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     printSln
 :* REMARKS       (/ output of a string and a new line /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     bytesToInt
 :* REMARKS       (/ conversion of a string of digits to an integer /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     getUid
 :* REMARKS       (/ get the user id of a BS 2000 file name /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     eq_str
 :* REMARKS       (/ a weaker string equality /)
 :* ENTRY         EXTERNAL
 :*
 :*-----------------------------------------------------------------
 :*
 :* VERSION       001
 :*   CRDATE      1992-12-02
 :*   AUTHOR      (/ Mounji Abdelaziz    , FUNDP /)
 :*   UPDATE      (/ original issue /)
 :*
 :* END-MODULE-DESCRIPTION lib.c
 :******************************************************************
*/
#include <stdio.h>
#include "predef.h"
#include "NADF_flag.h"
#include "SATX_error.h"

#if defined(POSIXREGEX) || defined(GNUREGEX) || defined(BSDREGEX)
#include "regex.h"
#elif !defined(STDREGEX)
#define	STDREGEX
#endif 	


#define uid_max 8

static int var_int;    /* area where to move an int to insure alignment */
/*   declaration for match routine */

#ifdef 	STDREGEX

#define INIT register char *sp = instring; /* instring is the base address   */
                                           /* of the buffer containing the   */
                                           /* regular expression to compiled */

                                           /* sp is the current position in  */
                                           /* this buffer. sp is used as an  */
                                          /* arguments in the folowing macro */   
#define GETC() (*sp++)   /* returns the current character in instring buffer */
                         /* abd advances the pointer                         */

#define PEEKC() (*sp) /* returns the current character in the instring buffer */
#define UNGETC(c) (--sp) /* pushes sp backward in the instring buffer */
#define RETURN(c) return(c);  

        /* in case an error is encountered in the RE to be parsed, this macro */
        /* is used to report the corresponding error */

#define ERROR(c) printf("code = %d\n", c);

#define BUFSIZE 256  /* Max. size of the buffer holding the RE to be compiled */
                     /* in case the RE is greater than BUFSIZE an error is    */
                     /* reported using ERROR(c)                               */

#define ESIZE 256    /* Max. size of the buffer holding the compiled RE       */
                     /* in case the compiled RE is greater than ESIZE an      */
                     /* error is reported using ERROR(c)                      */
                        
#include <regexp.h>  /* public domain source modules of REs  */

#endif /* STDREGEX */


/*
 :******************************************************************
 :* UNIT-BODY lib.strToInt
 :*
 :* TITLE         (/ conversion of a string to an integer /)
 :*
 :* PURPOSE       (/ convert the internal representation (binary) of an
 :*                  integer to this integer                        /)
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
 :*   Pre    (/ - 'm' is the address of a string representing an integer
 :*                         ___
 :*                'm' --->|_|_|
 :*                          |
 :*                         \|/
 :*                          |lg|integer|
 :*                           -----------
 :*
 :* 		- lg = 1 if this is a single byte 
 :*             - lg = 2 if this is a "short int"
 :*                  = 4 if this is a "int"
 :*             - integer is the binary representation of a short int or int
 :*
 :*                                                                     /)
 :*                                                             END-Pre
 :*   Post   (/ - the returned value is the value of this integer
 :*                                                                 /)
 :*                                                             END-Post
 :*   RETURNS  "void *"   (/ the value of the integer            /)
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY lib.strToInt
 :******************************************************************
*/

// This is the signed version
void *strToInt(m)
char *m;
{
    hole **h;
    unsigned short lg, lenstr();

    h = (hole **) m;
    lg = lenstr(*h);
    m = (char *) *h;
    m += 2;
    switch(lg) {
	
	case 0:
		    return (void *) 0;
	case 1:
		    return (void *) *m;
	case 2:
		    return (void *) *(short *) m;
	case 4:
//		    copy_bytes(m, &var_int, sizeof(int)); // Why using this global variable ??? Fred
		    memcpy(&var_int, m, sizeof(int));
		    return((void *) var_int);
	default:
		    error1(fatal, 24, "strToInt() of lib.c");
		    fprintf(stderr, "len : %d\n", lg);
		    return((void *) -1);
    }
}

// This is the unsigned version, much more useful
void *strToU(m)
char *m;
{
    hole **h;
    unsigned short lg, lenstr();

    h = (hole **) m;
    lg = lenstr(*h);
    m = (char *) *h;
    m += 2;
    switch(lg) {
	
	case 0:
		    return (void *) 0;
	case 1:
		    return (void *) *(unsigned char *) m;
	case 2:
		    return (void *) *(unsigned short *) m;
	case 4:
		    return (void *) *(unsigned int *) m;
	default:
		    error1(fatal, 24, "strToU() of lib.c");
		    fprintf(stderr, "len : %d\n", lg);
		    return (void *) -1;
    }
}

// These are functions returning partial registers
void *rX(m)
char *m;
{
	return (void *) *((*(unsigned short **) m)+1);
}

void *rH(m)
char *m;
{
	return (void *) *((*(unsigned char **) m)+3);
}

void *rL(m)
char *m;
{
	return (void *) *((*(unsigned char **) m)+2);
}

/*
 :******************************************************************
 :* UNIT-BODY lib.println
 :*
 :* TITLE         (/ output of a list of arguments and a new line   /)
 :*
 :* PURPOSE       (/ this function takes a pointer to a list of arguments of
 :*                  of type integer or string and output their values to
 :*                  stdout followed by the new line character    /)
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
 :*   Pre    (/ - 'm' is a pointer to a memory area as follows:
 :*               'm'
 :*                |
 :*                |
 :*              |lg|t_1|val_1 ... |t_n|val_n|
 :*               ---------------------------
 :*
 :*              where
 :*
 :*                 - lg is the number of pairs (t_i, val_i); lg = n
 :*                 - t_i is in {integer, bytestring, fieldstring}. This is
 :*                   the type of the i-th argument
 :*                 - val_i is the value of the i-th argument            /)
 :*                                                             END-Pre
 :*   Post   (/ - the values val_i are printed on the standard output
 :*               according to their types and in the order they occurred:
 :*             - if t_i = integer, val_i is interpreted as an integer
 :*               if t_i = bytestring or fieldstring, val_i is interpreted as
 :*                        a string
 :*             - the new line character is then written to stdout
 :*                                                                 /)
 :*                                                             END-Post
 :*   RETURNS NONE
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY lib.println
 :******************************************************************
*/
void *println(m)
char *m;
{
    void *print();
	
    print(m);
    printf("\n");
}
/*
 :******************************************************************
 :* UNIT-BODY lib.print
 :*
 :* TITLE         (/ output of a list of arguments   /)
 :*
 :* PURPOSE       (/ this function takes a pointer to a list of arguments of
 :*                  of type integer or string and output their values to
 :*                  stdout.                                      /)
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
 :*   Pre    (/ - 'm' is a pointer to a memory area as follows:
 :*               'm'
 :*                |
 :*                |
 :*              |lg|t_1|val_1 ... |t_n|val_n|
 :*               ---------------------------
 :*
 :*              where
 :*
 :*                 - lg is the number of pairs (t_i, val_i); lg = n
 :*                 - t_i is in {integer, bytestring, fieldstring}. This is
 :*                   the type of the i-th argument
 :*                 - val_i is the value of the i-th argument            /)
 :*                                                             END-Pre
 :*   Post   (/ - the values val_i are printed on the standard output
 :*               according to their types and in the order they occurred:
 :*             - if t_i = integer, val_i is interpreted as an integer
 :*               if t_i = bytestring or fieldstring, val_i is interpreted as
 :*                        a string
 :*                                                                 /)
 :*                                                             END-Post
 :*   RETURNS NONE
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY lib.print
 :******************************************************************
*/
void *print(m)
char *m;
{
    int arity, *i;
    //char *c;
    //hole **h;

    i = (int *) m;
    arity = *i;
    m += sizeof(int *);
    i = (int *) m;
    while (arity--) {
	if (*i == (int) integer) {
	    m += sizeof(int *);
	    printI(m);
	    m += sizeof(int *);
	    i = (int *) m;
	} else if (*i == (int) bytestring || *i == (int) fieldstring){
	    m += sizeof(int *);
	    printS(m);
	    m += sizeof(hole **);
	    i = (int *) m;
	} else {
		error1(fatal, 24, "in call to print() of lib.c");
	}
    }
}
/*
 :******************************************************************
 :* UNIT-BODY lib.printI
 :*
 :* TITLE         (/ output of an integer   /)
 :*
 :* PURPOSE       (/ this function prints the value of an integer on stdout
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
 :*   Pre    (/ - 'm' is the address of an integer
 :*                                                                     /)
 :*                                                             END-Pre
 :*   Post   (/ - the value of the integer pointed by 'm' is printed on
 :*               stdout.
 :*                                                                 /)
 :*                                                             END-Post
 :*   RETURNS  NONE
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY lib.printI
 :******************************************************************
*/
void *printI(m)
char *m;
{
    int *i;

    i = (int *) m;
    printf("%d", *i);
}
/*
 :******************************************************************
 :* UNIT-BODY lib.printIln
 :*
 :* TITLE         (/ output of an integer   /)
 :*
 :* PURPOSE       (/ this function prints the value of an integer on stdout.
 :*                  The new line characer is then printed on stdout  /)
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
 :*   Pre    (/ - 'm' is the address of an integer
 :*                                                                     /)
 :*                                                             END-Pre
 :*   Post   (/ - the value of the integer pointed by 'm' is printed on
 :*               stdout. The new line character is then printed.
 :*                                                                 /)
 :*                                                             END-Post
 :*   RETURNS  NONE
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY lib.printI
 :******************************************************************
*/
void *printIln(m)
char *m;
{
    printI(m);
    printf("\n");
}
/*
 :******************************************************************
 :* UNIT-BODY lib.printS
 :*
 :* TITLE         (/ output of a string   /)
 :*
 :* PURPOSE       (/ output the value of a string on stdout /)
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
 :*   Pre    (/ - 'm' is the address of a string:
 :*                         ___
 :*                'm' --->|_|_|
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
 :* END-BODY lib.printS
 :******************************************************************
*/
void *printS(m)
char *m;
{
    hole **h;

    h = (hole **) m;
    printStr(*h);
}
/*
 :******************************************************************
 :* UNIT-BODY lib.printSln
 :*
 :* TITLE         (/ output of a string and a new line  /)
 :*
 :* PURPOSE       (/ output the value of a string followed by a new line on
 :*                stdout                                               /)
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
 :*   Pre    (/ - 'm' is the address of a string representing a file name:
 :*                         ___
 :*                'm' --->|_|_|
 :*                          |
 :*                         \|/
 :*                          |lg| string |
 :*                           -----------
 :*
 :*             - lg is the length of 'string'
 :*                                                                     /)
 :*                                                             END-Pre
 :*   Post   (/ - the string 'string' is output on stdout followed by the
 :*               new line character                                    /)
 :*
 :*                                                             END-Post
 :*   RETURNS NONE
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY lib.printSln
 :******************************************************************
*/
void *printSln(m)
char *m;
{
    hole **h;
    unsigned short lg, lenstr();

    h = (hole **) m;
    lg = lenstr(*h);
    printS(m);
    printf("\n");
}
/*
 :******************************************************************
 :* UNIT-BODY lib.bytesToInt
 :*
 :* TITLE         (/ conversion of a string of digits to an integer /)
 :*
 :* PURPOSE       (/ convert a string of digits to the decimal integer value
 :*                  they represent                               /)
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
 :*   Pre    (/ - 'm' is the address of a string representing an integer
 :*                         ___
 :*                'm' --->|_|_|
 :*                          |
 :*                         \|/
 :*                          | n |d_1| ... |d_n|
 :*                           -----------------
 :*
 :*             - for i=1, ..., n: 0<=d_i<=9
 :*                                                                     /)
 :*                                                             END-Pre
 :*   Post   (/ - the returned value is the value of the decimal integer
 :*               represented by the digits d_1, ..., d_n
 :*                                                                 /)
 :*                                                             END-Post
 :*   RETURNS  "void *"   (/ the value of the integer            /)
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY lib.bytesToInt
 :******************************************************************
*/
void *bytesToInt(m)
char *m;
{
    hole **h;
    int n, i = 1;
    unsigned short lg, lenstr();
    char *cp;

    h = (hole **) m;
    lg = lenstr(*h);
    cp = (char *) *h;
    cp += usz;
    n = 0;
    while (i++ <= (int ) lg && *cp >= '0' && *cp <= '9')
	n = 10 * n + (*cp++ - '0');
    return ((void *) n);
}
/*
 :******************************************************************
 :* UNIT-BODY lib.getUid
 :*
 :* TITLE         (/ get the user id of a BS2000 file name /)
 :*
 :* PURPOSE       (/ this function returns a string which is the 'userid'
 :*                  field of a BS2000 path name                 /)
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
 :*   Pre    (/ - 'm' is the address of a string representing a BS2000
 :*               pathname
 :*                         ___
 :*                'm' --->|_|_|
 :*                          |
 :*                         \|/
 :*                          | lg |  string    |
 :*                           -----------------
 :*
 :*             - lg is the length of 'string'
 :*                                                                     /)
 :*                                                             END-Pre
 :*   Post   (/ - If 'string' is a valid BS 2000 path name i.e.:
 :*
 :*                       string = :catid:$userid.filename
 :*
 :*                  the returned value is a pointer to the user id field
 :*               otherwise, the error message:
 :*
 :*                       "invalid filename: 'string'"
 :*
 :*                  is output on stdout
 :*                                                                 /)
 :*                                                             END-Post
 :*   RETURNS  "void *"   (/ the value of the integer            /)
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY lib.getUid
 :******************************************************************
*/
void *getUid(m)
char *m;
{
    hole **h;
    int i, j;
    char *m1, *ad_ptr, *uid_ptr, *uid;
    unsigned short *ad_len, len_ad, *uid_len/*, len_uid*/;


    h = (hole **) m;
    m1 = (char *) *h;
    ad_len = (unsigned short *) m1;
    ad_ptr = m1+2;
    len_ad = *ad_len;
    if (*ad_ptr != ':') {
	error1(syntax, 24, "in call to getUid() of lib.c");
	printStr(m1);
	printf("\n");
	return((void *) 0);
    } else {
	ad_ptr++;
	i = 0;
	while (i<=4 && i <= (int) len_ad-4 && (int) *ad_ptr != ':') {
	    i++;
	    ad_ptr++;
	}
	if ((int) *ad_ptr == ':' && i <= 4 && i >= 1 && i <= (int) len_ad-4) {
	    i++;
	    ad_ptr++;
	    if(*ad_ptr != '$') {
		error1(syntax, 24, "in call to getUid() of lib.c");
		printStr(m1);
		printf("\n");
		return((void *) 0);
	    } else {
		j=0;
		i++;
		ad_ptr++;
		uid = tmalloc(usz + uid_max, char);
		uid_len = (unsigned short *) uid;
		uid_ptr = uid + 2;
	    }
	    while ((int) *ad_ptr != '.' &&  j <= 8 && i < (int) len_ad - 1) {
		j++;
		*uid_ptr++ = *ad_ptr++;
		i++;
	    }
	    *uid_len = (unsigned short) j;
	    if ((int) *ad_ptr != '.' || j > 8 || i > (int) len_ad - 1) {
		error1(syntax, 24, "in call to getUid() of lib.c");
		printStr(m1);
		printf("\n");
		return((void *) 0);
	    } else {
		return((void *) uid);     /* success */
	    }
	} else {
	    error1(syntax, 24, "in call to getUid() of lib.c");
	    printStr(m1);
	    printf("\n");
	    return((void *) 0);
	}
    }
}
/*
 :******************************************************************
 :* UNIT-BODY lib.eq_str
 :*
 :* TITLE         (/ a weaker string equality           /)
 :*
 :* PURPOSE       (/ return the value TRUE if the two strings are equal
 :*                  or if their respective longest prefixes (with no blanks)
 :*                  are equal and followed by a blank or end of string.
 :*                  return FALSE otherwise
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
 :*   Pre    (/ - 's' and 't' are pointers to strings:
 :*                         ___
 :*                's' --->|_|_|
 :*                          |
 :*                         \|/
 :*                          | lgs |  str_s    |
 :*                           ------------------
 :*
 :*                         ___
 :*                't' --->|_|_|
 :*                          |
 :*                         \|/
 :*                          | lgt |  str_t    |
 :*                           ------------------
 :*
 :*             - lgs is the length of 'str_s'
 :*             - lgt is the length of 'str_t'
 :*                                                                     /)
 :*                                                             END-Pre
 :*   Post   (/ - the returned value is TRUE if and only if
 :*                     'str_s' = 'str_t'
 :*                 or  'str_s' = 's1' 's2'  and
 :*                     'str_t' = 't1' 't2'  and
 :*                     's1', 't1' are none empty strings with no blanks and
 :*                     's1' = 't1'
 :*               in all other cases, the returned value is FALSE
 :*                                                                 /)
 :*                                                             END-Post
 :*   RETURNS  "void *"   (/ the result of the weaker comparaison   /)
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY lib.eq_str
 :******************************************************************
*/
eq_str(s, t)
char *s, *t;
{
    unsigned short *s_len, *t_len, /*len_s, len_t,*/ empty_s, empty_t, eq, end;
    unsigned short i = 0, j = 0;
    char /**s1, *t1,*/ *s_ptr, *t_ptr;

    s_len = (unsigned short *) s;
    t_len = (unsigned short *) t;
    s_ptr = s + usz;    /* usz = Unsigned Short siZe */
    t_ptr = t + usz;
    end = FALSE;
    while (!end) {
	if (i >= *s_len)
	    empty_s = TRUE;
	else if (*s_ptr == ' ')
	    empty_s = TRUE;
	else
	    empty_s = FALSE;

	if (j >= *t_len)
	    empty_t = TRUE;
	else if (*t_ptr == ' ')
	    empty_t = TRUE;
	else
	    empty_t = FALSE;

	if (empty_s && empty_t) {
	    eq = TRUE;
	    end = TRUE;
	} else if (!empty_s && !empty_t) {
	    if (*s_ptr == *t_ptr) {
		i++;
		j++;
		s_ptr++;
		t_ptr++;
	    } else {
		end = TRUE;
		eq = FALSE;
	    }
	} else {
	    end = TRUE;
	    eq = FALSE;
	}
    }
    return ( (int) eq);
}
void *match(m)
char *m;
{
#ifdef STDREGEX

char *expbuf;		/* receives the compiled  RE */
char *getExtStrRepr();  /* converts an ASAX string (lg, value) to the null */ 
			/* terminated form   */
char *str;		/* null terminated string repr. of String */
hole **sp1;		/* auxiliary pointers */


  	sp1 = (hole **) m++;
  	expbuf = getExtStrRepr(*sp1);  /* the regular expression */
  	sp1++; 
  	str    = getExtStrRepr(*sp1); /* the string to be matched against. */
  	if (step(str, expbuf)) 
     		return ((void *) 1);
  	return ((void *) 0);

#endif

#ifdef GNUREGEX

struct re_pattern_buffer *pbuff; 
char 			 *str;  /* null terminated string repr. of String */
char *getExtStrRepr();  /* converts an ASAX string (lg, value) to the null */ 
hole 			 **sp1;	/* auxiliary pointer */
int 			 ret;	

	sp1 = (hole **) m;
	str = (char * ) *sp1;

	pbuff = (struct re_pattern_buffer *) (str + sizeof(long));
	sp1++;
 	str  = getExtStrRepr(*sp1);  /* the string to be matched with the RE. */
	if ((ret = re_search(pbuff, str, strlen(str), 0, strlen(str), 0)) >= 0) {
     		return ((void *) 1);
	} else if (ret == -2) {
		fprintf(stderr, "an internal error occurred while matching ");
		fprintf(stderr, "a regexp with %s using GNU REGEX\n", str);
		exit(1);
	}
	/* ret == -1 ie. no match */
  	return ((void *) 0);

#endif

#ifdef POSIXREGEX

regex_t 	*pbuff; 
char 		*str;		/* null terminated string repr. of String */
char *getExtStrRepr();  /* converts an ASAX string (lg, value) to the null */ 
hole 		**sp1;	/* auxiliary pointer */

	sp1 = (hole **) m;
	str = (char * ) *sp1;

	pbuff = (regex_t *) (str + sizeof(long));
	sp1++;
 	str  = getExtStrRepr(*sp1);  /* the string to be matched with the RE. */
	if (regexec(pbuff, str, 0, 0, 0) != REG_NOMATCH) 
     		return ((void *) 1);
  	return ((void *) 0);

#endif

#ifdef 	BSDREGEX

char *s;		
char *getExtStrRepr();  /* converts an ASAX string (lg, value) to the null  
			   terminated form   */
hole **aux;		/* auxiliary pointer */


 	aux = (hole **) m;
  	s   = getExtStrRepr(*++aux); /* the string to be matched against RE. */
  	return ((void *) re_exec(s));

#endif /* BSDREGEX */
}

void *accept(m)
char *m;
{
    int i;

    scanf("%d", &i);
    return ( (void *) i);
}
void *append(m)
char *m;
{
        hole **sp1;
        char *getExtStrRepr(), *s, *s0, *s1, *s2;
        unsigned short *sh;
        int l1, l2;

        sp1 = (hole **) m++; 
        l1 = strlen(s1 = getExtStrRepr(*sp1++));
        l2 = strlen(s2 = getExtStrRepr(*sp1));
        s = s0 = tmalloc(l1 + l2 + 2, char);
        sh = (unsigned short *) s0;
        *sh++ = l1 + l2 + 2;
        s0 = (char *) sh;
        while (*s1 != ' ' && *s1) {
                *s0 = *s1++;
                s0++;
        } 
        strcpy(s0, s2);
        return( (void *) s);
}


