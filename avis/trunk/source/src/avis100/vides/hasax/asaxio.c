/*
 :******************************************************************
 :* MODULE-DESCRIPTION
 :*
 :* TITLE         (/ library predefined procedures and functions   /)
 :*
 :* NAME          IO.c
 :*
 :* COPYRIGHT     (C)   SIEMENS NIXDORF INFORMATIONSSYSTEME AG  1992
 :*                     ALL RIGHTS RESERVED
 :*
 :* MEMORY-CLASS  6
 :*
 :* PROG-CONVENTS (/ <internal conventions (instance, names,
 :*                  external data, ...). None -> empty text>     /)
 :*
 :* PROCEDURE     creatNADF
 :* REMARKS       (/ creation of a NADF file /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     closeNADF
 :* REMARKS       (/ closing of a NADF file /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     writeNADF
 :* REMARKS       (/ writing of the current NADF record   /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     resetIsWritten
 :* REMARKS       (/ zerowing table 'IsWritten[]' /)
 :* ENTRY         EXTERNAL
 :*
 :*-----------------------------------------------------------------
 :*
 :* VERSION       001
 :*   CRDATE      1992-12-02
 :*   AUTHOR      (/ Mounji Abdelaziz    , FUNDP /)
 :*   UPDATE      (/ original issue /)
 :*
 :* END-MODULE-DESCRIPTION IO.c
 :******************************************************************
*/
#include <stdio.h>
#include "asax.h"
#include "NADF_flag.h"
#include "SATX_error.h"

unsigned short IsWritten[MAX_files];

/* for 0 <= i <= MAX_files :                                               */
/* IsWritten[i] = TRUE  if the current record has not yet been written to  */
/*                      NADF file with file descriptor i                   */
/*              = FALSE otherwise.                                         */

/*
 :******************************************************************
 :* UNIT-BODY IO.writeNADF
 :*
 :* TITLE         (/ writing of the current NADF record  /)
 :*
 :* PURPOSE       (/ writing of the current NADF record in the buffer
 :*                  corresponding to the file.
 :*                  When the buffer is full, it is really written /)
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
 :*                  - IsWritten[]  : table of boolean values.
 :*                                   IsWritten[i] <==> the current NADF
 :*                                   record is already written to file
 :*                                   with file descriptor 'i'
 :*                  - Curr_Rec_Addr: current record address
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
 :*   Post   (/ - if '*m' is a valid NADF file descriptor, the current record
 :*                  is written to the buffer allocated for this file unless
 :*                  IsWritten[*m] is TRUE (record was alraedy written);
 :*               otherwise, an error code is returned.
 :*                                                                 /)
 :*                                                             END-Post
 :*   RETURNS  "int"      (/ if the value is >=0, it signals the success
 :*                          of the operation else it is the error code /)
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY IO.writeNADF
 :******************************************************************
*/

void *writeNADF(m)
char *m;
{
    int *i;      /* *i is a file descriptor  (if a valid one) */
    int rc;

    i = (int *) m;
    if (*i > -1 && *i < MAX_files) {
	if (!IsWritten[*i]) {  /* current record not already written */
	    IsWritten[*i] = TRUE;
	    rc = write_NADF(*i, Curr_Rec_Addr, NO_FLUSH);
	    return((void *) rc);
	}
	return ((void *) 0);
    }
    return ((void *) ESBADFID);
}

/*
 :******************************************************************
 :* UNIT-BODY IO.creatNADF
 :*
 :* TITLE         (/ creation of a NADF file    /)
 :*
 :* PURPOSE       (/ creation of a NADF file of a specified name.
 :*                  This file is open with write mode            /)
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
 :*                          |lg|filename|
 :*                           -----------
 :*
 :*             - lg is the length of 'filename
 :*                                                                     /)
 :*                                                             END-Pre
 :*   Post   (/ - a file with file name 'filename' is created and open with
 :*               the "WRITE" mode. If a file with name 'filename' already
 :*               exist an error code is returned
 :*                                                                 /)
 :*
 :*                                                             END-Post
 :*   RETURNS  "int"      (/ if the value is >=0, it is the file descriptor
 :*                          else it is the error code /)
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY IO.creatNADF
 :******************************************************************
*/
void *creatNADF(m)
char *m;
{
    hole **h;
    char *s;                            /* an NADF file name               */
    char *getExtStrRepr();
    int rc;                             /* returned code from creat_NADF() */

    h = (hole **) m;
    s = getExtStrRepr(*h);
    rc = creat_NADF(s);
    free(s);
    if (rc > -1) {   /* creation succeeded */
	IsWritten[rc] = FALSE;
    }
    return((void *) rc);
}
/*
 :******************************************************************
 :* UNIT-BODY IO.closeNADF
 :*
 :* TITLE         (/ closing of a NADF file    /)
 :*
 :* PURPOSE       (/ this function closes the NADF file open by 'creatNADF'.
 :*                  The buffer is flushed and released.            /)
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
 :*   Post   (/ - if '*m' is a valid NADF file descriptor, this file is
 :*               closed, the buffer allocated for this file is flushed to
 :*               disk then released, otherwise, an error code is returned.
 :*                                                                 /)
 :*                                                             END-Post
 :*   RETURNS  "int"      (/ if the value is >=0, it signals the success
 :*                          of the operation else it is the error code /)
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY IO.closeNADF
 :******************************************************************
*/

void *closeNADF(m)
char *m;
{
    int *i;      /* *i is a file descriptor  (if a valid one) */
    int rc;

    i = (int *) m;
    if (*i > -1 && *i < MAX_files) {
	rc = close_NADF(*i);
	return((void *) rc);
    } else {
	return ((void *) ESBADFID);
    }
}
/*
 :******************************************************************
 :* UNIT-BODY  IO.resetIsWritten
 :*
 :* TITLE         (/ zerowing table 'IsWritten[]' /)
 :*
 :* PURPOSE       (/ set all elements of 'IsWritten[]' to FALSE /)
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
 :*                  - IsWritten[]  : table of boolean values.
 :*                                   IsWritten[i] <==> the current NADF
 :*                                   record is already written to file
 :*                                   with file descriptor 'i'
 :*                  - MAX_files    : length of 'IsWritten[]'
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
 :*   Post   (/ - IsWitten[i] = FALSE (i=0, ..., MAX_files - 1)
 :*                                                                   /)
 :*                                                             END-Post
 :*   RETURNS   NONE
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY  IO.resetIsWritten
 :******************************************************************
*/

resetIsWritten()  /* (when a new record is read,) reset all members of     */
		  /* IsWritten table to be FALSE                           */
{
    int i;

    for (i=0; i<MAX_files; i++)
	IsWritten[i] = FALSE;
}
