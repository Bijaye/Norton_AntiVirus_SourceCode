/*
 :******************************************************************
 :* MODULE-DESCRIPTION
 :*
 :* TITLE         (/ Parser of the Audit Data Description file /)
 :*
 :* NAME          parse.c
 :*
 :* COPYRIGHT     (C)   SIEMENS NIXDORF INFORMATIONSSYSTEME AG  1992
 :*                     ALL RIGHTS RESERVED
 :*
 :* MEMORY-CLASS  6
 :*
 :* PROG-CONVENTS (/ <internal conventions (instance, names,
 :*                  external data, ...). None -> empty text>     /)
 :*
 :* PROCEDURE     add_field_name
 :* REMARKS       (/ install a new field name             /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     getAuditDataId
 :* REMARKS       (/ get audit data identifier /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     print_map_tab
 :* REMARKS       (/ print mapping table             /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     parse_descrip
 :* REMARKS       (/ parser of the Audit Data Description file         /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     header_rec
 :* REMARKS       (/ parsing the header lines             /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     description_rec
 :* REMARKS       (/ parsing an audit data descriptor record /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     line_1
 :* REMARKS       (/ parsing a '1' line  /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     skipblanks
 :* REMARKS       (/ skiping blanks                          /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     skiplines
 :* REMARKS       (/ skip empty lines   /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     X_lines
 :* REMARKS       (/ parsing of lines '2' '3' and '4' /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     line_5
 :* REMARKS       (/ parsing a '5' line  /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     alpha_lines
 :* REMARKS       (/ parsing lines 'A' ... 'F' /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     alpha_line
 :* REMARKS       (/ parsing a line 'A' or ... or 'F' /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     get_identif
 :* REMARKS       (/ reading of an identifier /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     takeOfChar
 :* REMARKS       (/ skip alphanumeric characters /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     get_int
 :* REMARKS       (/ reading of an integer /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     get_any_seq
 :* REMARKS       (/ skiping current line /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     get_c
 :* REMARKS       (/ reading of a character in the input stream /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     error_msg
 :* REMARKS       (/ error message output /)
 :* ENTRY         EXTERNAL
 :*
 :*-----------------------------------------------------------------
 :*
 :* VERSION       001
 :*   CRDATE      1992-11-03
 :*   AUTHOR      (/ Mounji Abdelaziz    , FUNDP /)
 :*   UPDATE      (/ original issue /)
 :*
 :* END-MODULE-DESCRIPTION parse.c
 :******************************************************************
*/
#include <stdio.h>
#include "asax.h"
#include <ctype.h>

#define Max_us 65536   /* maximum value of an unsigned short */

typedef struct {
	unsigned short ad_id;
	char field_name[MaxIdent];
} map_slot;

int map_tab_lg = 0;  /* length of the mapping table */
static map_slot mapping_table[Max_ad_id];

static char ad_name[MaxIdent];
static unsigned short ad_id;
int line = 1, NbreChar = 0;
static short int more_char;
static int nextchar;


/*
 :******************************************************************
 :* UNIT-BODY parse.add_field_name
 :*
 :* TITLE         (/ install a new field name  /)
 :*
 :* PURPOSE       (/ add a new entry to the table mapping field names to
 :*                  their audit data id /)
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
 :*                     - mapping_table[]: table that maps field names to
 :*                                        their corresponding audit data id
 :*                     - map_tab_lg     : current length of 'mapping_table[]'
 :*                     - Max_ad_id      : maximum length of 'mapping_table[]'
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
 :*   Pre    (/ - 'ad_name' is a string representing a field name
 :*             - 'ad_id' is its corresponding audit data id
 :*             - the set of entries in 'mapping_table[]' is:
 :*               S = {(name_0, id_0), ..., (name_n, id_n)}  (n=map_tab_lg-1)
 :*             - 'mapping_table[]' is sorted in ascending alphabetic order
 :*               on field names:
 :*      mapping_table[0].ad_name < ... < mapping_table[map_tab_lg-1].ad_name
 :*             - 'map_tab_lg' = lg_0
 :*                                                                     /)
 :*                                                             END-Pre
 :*
 :*   Post  (/ - if 'map_tab_lg'<'Max_ad_id'-1 and (ad_name, ad_id) not in S:
 :*                 - 'map_tab_lg' = lg_0 + 1
 :*                 - the set of entries in 'mapping_table[]' is:
 :*                              S U {(ad_name, id)}
 :*                 - 'mapping_table[]' is sorted in ascending alphabetic
 :*                   order on field names:
 :*                       mapping_table[0].ad_name < ...
 :*                       < mapping_table[map_tab_lg-1].ad_name
 :*                 - the returned value is 'i' such that:
 :*                       mapping_table[i].ad_name = ad_name
 :*            - if 'map_tab_lg'<'Max_ad_id'-1 and (ad_name, ad_id) in S:
 :*                 the following error message:
 :*                   "Severe error: duplicate audit data: ad_name"
 :*                 is output on stdout
 :*            - otherwise if 'map_tab_lg'>='Max_ad_id'-1 the error message:
 :*                       "mapping table overflow!"
 :*               is output on stdout
 :*                                                                 /)
 :*
 :*                                                             END-Post
 :*   RETURNS  "int"   (/ index of the installed audit data /)
 :*
 :*   REMARKS          (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY parse.add_field_name
 :******************************************************************
*/
add_field_name(ad_name, ad_id)
char ad_name[];
unsigned short ad_id;
{
    int i;  /* loop counter */
    int k;  /* to compare two strings */

    i = ++map_tab_lg;
    if (i >= Max_ad_id) {
	printf("mapping_table overflow!\n");
	exit(1);
    }
    while (i-- > 1) {
	if ((k = strcmp(mapping_table[i-1].field_name, ad_name)) > 0) {
	    mapping_table[i].ad_id = mapping_table[i-1].ad_id;
	    strcpy(mapping_table[i].field_name,
		   mapping_table[i-1].field_name);
	} else if (k < 0) {
	    break;
	} else { /* k = 0 */
	    printf("Severe error: duplicate audit data: %s !\n",
		    ad_name);
	    exit(1);
	}
    }
    mapping_table[i].ad_id = ad_id;
    strcpy(mapping_table[i].field_name, ad_name);
    return(i);
}
/*
 :******************************************************************
 :* UNIT-BODY rdescr.getAuditDataId
 :*
 :* TITLE         (/ get audit data identifier      /)
 :*
 :* PURPOSE       (/ get the audit data identifier of a given field name /)
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
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/  if 's' is a valid field name (i.e, there exist j such that
 :*                 mapping_table[j].fieldname = 's')
 :*              then the returned value is:
 :*                      mapping_table[j].ad_id
 :*              otherwise -1 is returned
 :*                                                                 /)
 :*
 :*                                                             END-Post
 :*   RETURNS  "int" (/ -1 if invalid field name or its audit data id
 :*                     otherwise                           /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY rdescr.getAuditDataId
 :******************************************************************
*/
getAuditDataId(s)
char *s;
{
	short int low, high, mid, k;

	low = 0;
	high = map_tab_lg - 1;
	while (low <= high) {
		mid = (low + high) / 2;
		if ((k = strcmp(s, mapping_table[mid].field_name)) < 0)
			high = mid - 1;
		else if (k > 0)
			low = mid + 1;
		else /* external name found */
			return (mapping_table[mid].ad_id);
	}
	return (-1);
}

/*
 :******************************************************************
 :* UNIT-BODY parse.print_map_tab
 :*
 :* TITLE         (/ print mapping table  /)
 :*
 :* PURPOSE       (/ print the content of the table that maps field names to
 :*                  their corresponding audit data id
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
 :*                     - mapping_table[]: table that maps field names to
 :*                                        their corresponding audit data id
 :*                     - map_tab_lg     : current length of 'mapping_table[]'
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
 :*   Post   (/ - the two header lines:
 :*
 :*                   *******  mapping table ********
 :*               audit data name             |   audit data id
 :*
 :*               is output to stdout
 :*             - 'map_tab_lg' lines having the format:
 :*
 :*                  mapping_table[i].ad_name "|" mapping_table[i].ad_id
 :*
 :*               are then output to stdout in ascending order of 'i'
 :*                                                                 /)
 :*
 :*                                                             END-Post
 :*   RETURNS NONE
 :*
 :*   REMARKS          (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY parse.print_map_tab
 :******************************************************************
*/
print_map_tab()
{
    int i;

    printf("\n   *******  mapping table ********\n\n");
    printf("  audit data name             |   audit data id\n");
    printf("  ---------------------------------------------\n");
    for (i=0; i< map_tab_lg; i++) {
	printf("  %-20s        |  %8d \n", mapping_table[i].field_name,
					  mapping_table[i].ad_id);
    }
}
/*
 :******************************************************************
 :* UNIT-BODY parse.parse_descrip
 :*
 :* TITLE         (/ parser of the Audit Data Description file /)
 :*
 :* PURPOSE       (/ this function receives a file pointer of an Audit Data
 :*                  Description file open in "READ" mode. In absence of
 :*                  errors, it initialize the table 'mapping_table[]' with
 :*                  all pairs (field_name, audit_data_id) in this file /)
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
 :*                                                               /)
 :*   DATA       (/
 :*                     - mapping_table[]: table that maps field names to
 :*                                        their corresponding audit data id
 :*                     - map_tab_lg     : current length of 'mapping_table[]'
 :*                     - Max_ad_id      : maximum length of 'mapping_table[]'
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
 :*   Pre    (/ - 'fp' is the file pointer of the Audit Data Description file
 :*               open in "READ" mode
 :*             - 'map_tab_lg' = 0                                  /)
 :*                                                             END-Pre
 :*
 :*   Post   (/ - let the list of entries '1' and '4' of this file be:
 :*                       (name_0, id_0), ..., (name_n, id_n)
 :*               sorted in alphabetic order on name_i's
 :*             - if
 :*               (A) the sequence name_0, ..., name_n contains no duplicates
 :*               and if
 :*               (B) n+1 < 'Max_ad_id' then we have the following:
 :*                 - mapping_table[i].ad_name = name_i and
 :*                 - mapping_table[i].ad_id   = id_i  (i=0, ..., n)
 :*                 - 'map_tab_lg' = n+1
 :*               and if
 :*                (C) the BNF syntax of Audit Data Description file was
 :*                    respected
 :*             - otherwise
 :*                 - if (A) is false, the error message:
 :*                     "Severe error: duplicate audit data: ad_name"
 :*                   is output on stdout (ad_name is the first duplicate)
 :*                 - if (B) is false the error message:
 :*                       "mapping table overflow!"
 :*                   is output on stdout
 :*                 - if (C) is false the error message:
 :*
 :*                     Line  : 'lineNbre' invalid line
 :*
 :*               where 'lineNbre' is the line number is sent to stdout /)
 :*
 :*                                                             END-Post
 :*   RETURNS NONE
 :*
 :*   REMARKS          (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY parse.parse_descrip
 :******************************************************************
*/
parse_descrip(fp)
FILE *fp;
{
    get_c(fp);                               /* get the first character   */
    header_rec(fp);                          /* parse lines 'A', ..., 'F' */
    while (more_char) {
	description_rec(fp);                 /* parse lines '1', ... '5'  */
	add_field_name(ad_name, ad_id);      /* install in mapp_table[]   */
    }
}
/*
 :******************************************************************
 :* UNIT-BODY parse.header_rec
 :*
 :* TITLE         (/ parsing the header lines /)
 :*
 :* PURPOSE       (/ parse the header lines 'A', ..., 'F' and report errors
 :*                  in case the syntax of these lines was not observed/)
 :*                                                      END-PURPOSE
 :* VERSION       001
 :*   CRDATE      1992-16-06
 :*   AUTHOR      (/ Mounji Abdelaziz       , FUNDP              /)
 :*   UPDATE      (/ original issue /)
 :*
 :* REALIZATION   (/ <note, reference to other proc. (optional)> /)
 :*
 :*   CONTROL    (/
 :*                                                                 /)
 :*
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
 :*   DOCUMENTS       (/ Advanced Security Audit trail analysis on uniX
 :*                             (ASAX also called SAT-X)
 :*                      Implementation design of the NADF Evaluator   /)
 :*                                                  END-REALIZATION
 :* PARAMETERS
 :*
 :*   Pre    (/ - fp    a valid file pointer.
 :*             - the input stream is <stream>
 :*                                                                      /)
 :*                                                             END-Pre
 :*
 :*   Post   (/ - if <stream>=<spaces><alpha_lines><rest>
 :*                where:
 :*                 - <spaces> is any sequence of blanks tabs and new lines
 :*                 - <alpha_lines> is a valid sequence of 'A', ..., 'F'
 :*                   lines (see BNF syntax of Audit Data Description file)
 :*                 - <rest> is any character sequence
 :*             - then the current input stream is <rest>
 :*             - otherwise the error message:
 :*
 :*                     Line  : 'lineNbre' invalid line
 :*
 :*               where 'lineNbre' is the line number is output to stdout /)
 :*                                                             END-Post
 :*
 :*   RETURNS  NONE
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY parse.header_rec
 :******************************************************************
*/
header_rec(fp)
FILE *fp;
{
    char c;

    for (c = 'A'; c <= 'F'; c++) {
	alpha_lines(c, fp);
    }
}
/*
 :******************************************************************
 :* UNIT-BODY parse.description_rec
 :*
 :* TITLE         (/ parsing an audit data descriptor record /)
 :*
 :* PURPOSE       (/ parse the next sequence of lines '1', ..., '5'
 :*                  describing an audit data. In case the syntax of these
 :*                  lines was observed, install a new entry in
 :*                  'mapping_table[]' corresponding to this audit data.
 :*                  Otherwise report the error.                 /)
 :*
 :*                                                      END-PURPOSE
 :* VERSION       001
 :*   CRDATE      1992-16-06
 :*   AUTHOR      (/ Mounji Abdelaziz       , FUNDP              /)
 :*   UPDATE      (/ original issue /)
 :*
 :* REALIZATION   (/ <note, reference to other proc. (optional)> /)
 :*
 :*   CONTROL    (/
 :*                                                                 /)
 :*
 :*   DATA       (/
 :*                     - mapping_table[]: table that maps field names to
 :*                                        their corresponding audit data id
 :*                     - map_tab_lg     : current length of 'mapping_table[]'
 :*                     - Max_ad_id      : maximum length of 'mapping_table[]'
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
 :*   Pre    (/ - fp    a valid file pointer.
 :*             - the input stream is <stream>
 :*             - the set of entries in 'mapping_table[]' is:
 :*               S = {(name_0, id_0), ..., (name_n, id_n)}  (n=map_tab_lg-1)
 :*             - 'mapping_table[]' is sorted in ascending alphabetic order
 :*               on field names:
 :*      mapping_table[0].ad_name < ... < mapping_table[map_tab_lg-1].ad_name
 :*             - 'map_tab_lg' = lg_0
 :*                                                                      /)
 :*                                                             END-Pre
 :*
 :*   Post   (/ - if <stream>=<spaces><X_lines><rest>
 :*                where:
 :*                 - <spaces> is any sequence of blanks tabs and new lines
 :*                 - <alpha_lines> is a valid sequence of '1', ..., '5'
 :*                   lines (see BNF syntax of Audit Data Description file)
 :*                 - <rest> is any character sequence
 :*                 - let (ad_name, ad-id) the pair audit data name, audit
 :*                   data id contained in lines '1' and '4'
 :*             - and if 'map_tab_lg'<'Max_ad_id'-1 and (ad_name, ad_id) not
 :*                      in S
 :*                 - 'map_tab_lg' = lg_0 + 1
 :*                 - the set of entries in 'mapping_table[]' is:
 :*                              S U {(ad_name, id)}
 :*                 - 'mapping_table[]' is sorted in ascending alphabetic
 :*                   order on field names:
 :*                       mapping_table[0].ad_name < ...
 :*                       < mapping_table[map_tab_lg-1].ad_name
 :*            - if 'map_tab_lg'<'Max_ad_id'-1 and (ad_name, ad_id) in S:
 :*                 the following error message:
 :*                   "Severe error: duplicate audit data: ad_name"
 :*                 is output on stdout
 :*            - otherwise if 'map_tab_lg'>='Max_ad_id'-1 the error message:
 :*                       "mapping table overflow!"
 :*               is output on stdout
 :*             - then the current input stream is <rest>
 :*            -  if the syntax was not respected the error message:
 :*
 :*                     Line  : 'lineNbre' invalid line
 :*
 :*               where 'lineNbre' is the line number is output to stdout /)
 :*                                                             END-Post
 :*
 :*   RETURNS  NONE
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY parse.description_rec
 :******************************************************************
*/
description_rec(fp)
FILE *fp;
{
    char c;

    line_1(fp);
    for(c = '2'; c <= '4'; c++) {
	X_lines(c, fp);
    }
    lines_5(fp);    /* line 5 are optional */
}
/*
 :******************************************************************
 :* UNIT-BODY parse.line_1
 :*
 :* TITLE         (/ parsing a '1' line /)
 :*
 :* PURPOSE       (/ parse a line of type '1' and report errors in case the
 :*                  syntax of this type of line was not observed   /)
 :*                                                      END-PURPOSE
 :* VERSION       001
 :*   CRDATE      1992-16-06
 :*   AUTHOR      (/ Mounji Abdelaziz       , FUNDP              /)
 :*   UPDATE      (/ original issue /)
 :*
 :* REALIZATION   (/ <note, reference to other proc. (optional)> /)
 :*
 :*   CONTROL    (/
 :*                                                                 /)
 :*
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
 :*   DOCUMENTS       (/ Advanced Security Audit trail analysis on uniX
 :*                             (ASAX also called SAT-X)
 :*                      Implementation design of the NADF Evaluator   /)
 :*                                                  END-REALIZATION
 :* PARAMETERS
 :*
 :*   Pre    (/ - fp    a valid file pointer.
 :*             - the input stream is <stream>
 :*                                                                      /)
 :*                                                             END-Pre
 :*
 :*   Post   (/ - if <stream>=<spaces><lines_1><rest>
 :*                where:
 :*                 - <spaces> is any sequence of blanks tabs and new lines
 :*                 - <lines_1> is a valid line of type '1' (see BNF syntax
 :*                             of Audit Data Description file)
 :*                 - <rest> is any character sequence
 :*             - then the current input stream is <rest>
 :*             - otherwise the error message:
 :*
 :*                     Line  : 'lineNbre' invalid line
 :*
 :*               where 'lineNbre' is the line number is output to stdout /)
 :*                                                             END-Post
 :*
 :*   RETURNS  NONE
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY parse.line_1
 :******************************************************************
*/
line_1(fp)
FILE *fp;
{
    skiplines(fp);
    if (nextchar == '1') {
	get_c(fp);
	skipblanks(fp);
	if (isdigit(nextchar)) {
	    ad_id = get_int(fp);
	} else {
	    error_msg("invalid line");
	}
    } else {
	    error_msg("invalid line");
    }
}

/*
 :******************************************************************
 :* UNIT-BODY parse.skipblanks
 :*
 :* TITLE         (/ skipping blanks /)
 :*
 :* PURPOSE       (/ skip blanks, tabs in the current input stream /)
 :*                                                      END-PURPOSE
 :* VERSION       001
 :*   CRDATE      1992-16-06
 :*   AUTHOR      (/ Mounji Abdelaziz       , FUNDP              /)
 :*   UPDATE      (/ original issue /)
 :*
 :* REALIZATION   (/ <note, reference to other proc. (optional)> /)
 :*
 :*   CONTROL    (/
 :*                                                                 /)
 :*
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
 :*   DOCUMENTS       (/ Advanced Security Audit trail analysis on uniX
 :*                             (ASAX also called SAT-X)
 :*                      Implementation design of the NADF Evaluator   /)
 :*                                                  END-REALIZATION
 :* PARAMETERS
 :*
 :*   Pre    (/ - fp    a valid file pointer.
 :*             - the input stream is <stream>=<spaces><rest>
 :*               where <spaces> is a possibly empty sequence of blanks and
 :*                              tabs
 :*                     <rest>   is any character sequence
 :*                                                                      /)
 :*                                                             END-Pre
 :*
 :*   Post   (/ - <stream>=<rest>                                     /)
 :*                                                             END-Post
 :*
 :*   RETURNS  NONE
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY parse.skipblanks
 :******************************************************************
*/
skipblanks(fp)
FILE *fp;
{
    while (more_char && (nextchar == ' '|| nextchar == '\t')) {
		get_c(fp);
       }
}
/*
 :******************************************************************
 :* UNIT-BODY parse.skiplines
 :*
 :* TITLE         (/ skip empty lines
 :*
 :* PURPOSE       (/ skip empty lines in the current input stream /)
 :*                                                      END-PURPOSE
 :* VERSION       001
 :*   CRDATE      1992-16-06
 :*   AUTHOR      (/ Mounji Abdelaziz       , FUNDP              /)
 :*   UPDATE      (/ original issue /)
 :*
 :* REALIZATION   (/ <note, reference to other proc. (optional)> /)
 :*
 :*   CONTROL    (/
 :*                                                                 /)
 :*
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
 :*   DOCUMENTS       (/ Advanced Security Audit trail analysis on uniX
 :*                             (ASAX also called SAT-X)
 :*                      Implementation design of the NADF Evaluator   /)
 :*                                                  END-REALIZATION
 :* PARAMETERS
 :*
 :*   Pre    (/ - fp    a valid file pointer.
 :*             - the input stream is <stream>=<spaces><rest>
 :*               where <spaces> is a possibly empty sequence of blanks, tabs
 :*                              and new lines
 :*                     <rest>   is any character sequence
 :*                                                                      /)
 :*                                                             END-Pre
 :*
 :*   Post   (/ - <stream>=<rest>                                     /)
 :*                                                             END-Post
 :*
 :*   RETURNS  NONE
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY parse.skiplines
 :******************************************************************
*/
skiplines(fp)
FILE *fp;
{
    while (more_char &&
	  (nextchar == ' '|| nextchar == '\t' || nextchar == '\n')) {
	get_c(fp);
    }
}
/*
 :******************************************************************
 :* UNIT-BODY parse.X_lines
 :*
 :* TITLE         (/ parsing of lines '2', '3' and '4' /)
 :*
 :* PURPOSE       (/ parse a line of type '2', '3' or '4' and report error in
 :*                  case the syntax of this type of line was not observed /)
 :*                                                      END-PURPOSE
 :* VERSION       001
 :*   CRDATE      1992-16-06
 :*   AUTHOR      (/ Mounji Abdelaziz       , FUNDP              /)
 :*   UPDATE      (/ original issue /)
 :*
 :* REALIZATION   (/ <note, reference to other proc. (optional)> /)
 :*
 :*   CONTROL    (/
 :*                                                                 /)
 :*
 :*   DATA       (/
 :*                     - ad_name[] : the last audit data name found
 :*                                   in the Audit Data Description file
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
 :*   Pre    (/ - 'fp'    a valid file pointer.
 :*             - 'leading_digit' is in {2, 3, 4}
 :*             - the input stream is <stream>
 :*                                                                      /)
 :*                                                             END-Pre
 :*
 :*   Post   (/ - if <stream>=<spaces><X_line><rest>
 :*                where:
 :*                 - <spaces> is any sequence of blanks tabs and new lines
 :*                 - <X_lines> is a valid line of type '2', '3' or '4'
 :*                            (see BNF syntaxof Audit Data Description file)
 :*                 - <rest> is any character sequence
 :*             - then
 :*                    - the current input stream is <rest>
 :*                    - 'ad_name[]' contains the identifier in this line
 :*             - otherwise the error message:
 :*
 :*                     Line  : 'lineNbre' invalid line
 :*
 :*               where 'lineNbre' is the line number is output to stdout /)
 :*                                                             END-Post
 :*
 :*   RETURNS  NONE
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY parse.X_lines
 :******************************************************************
*/
X_lines(leading_digit, fp) /* parse lines 2, 3 or 4 */
char leading_digit;
FILE *fp;
{
    skiplines(fp);
    if (nextchar == leading_digit) {
	get_c(fp);
	skipblanks(fp);
	if (isalpha(nextchar) || nextchar == '_') {
	    get_identif(fp);
	} else {
	    error_msg("invalid line");
	}
     } else {
	    error_msg("invalid line");
     }
}
/*
 :******************************************************************
 :* UNIT-BODY parse.line_5
 :*
 :* TITLE         (/ parsing a '5' line /)
 :*
 :* PURPOSE       (/ parse a line of type '5' and report errors in case the
 :*                  syntax of this type of line was not observed   /)
 :*                                                      END-PURPOSE
 :* VERSION       001
 :*   CRDATE      1992-16-06
 :*   AUTHOR      (/ Mounji Abdelaziz       , FUNDP              /)
 :*   UPDATE      (/ original issue /)
 :*
 :* REALIZATION   (/ <note, reference to other proc. (optional)> /)
 :*
 :*   CONTROL    (/
 :*                                                                 /)
 :*
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
 :*   DOCUMENTS       (/ Advanced Security Audit trail analysis on uniX
 :*                             (ASAX also called SAT-X)
 :*                      Implementation design of the NADF Evaluator   /)
 :*                                                  END-REALIZATION
 :* PARAMETERS
 :*
 :*   Pre    (/ - fp    a valid file pointer.
 :*             - the input stream is <stream>
 :*                                                                      /)
 :*                                                             END-Pre
 :*
 :*   Post   (/ - if <stream>=<spaces><lines_5><rest>
 :*                where:
 :*                 - <spaces> is any sequence of blanks tabs and new lines
 :*                 - <lines_5> is a valid line of type '5' (see BNF syntax
 :*                             of Audit Data Description file)
 :*                 - <rest> is any character sequence
 :*             - then the current input stream is <rest>
 :*             - otherwise the error message:
 :*
 :*                     Line  : 'lineNbre' invalid line
 :*
 :*               where 'lineNbre' is the line number is output to stdout /)
 :*                                                             END-Post
 :*
 :*   RETURNS  NONE
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY parse.line_5
 :******************************************************************
*/
lines_5(fp) /* parse lines 5 */
FILE *fp;
{
    alpha_lines('5', fp);
}
/*
 :******************************************************************
 :* UNIT-BODY parse.alpha_lines
 :*
 :* TITLE         (/ parsing of lines 'A', ... 'F'  /)
 :*
 :* PURPOSE       (/ parse a sequence of lines of type 'A' or ... or 'F' and
 :*                  report error in case the syntax of this type of line was
 :*                  not observed /)
 :*                                                      END-PURPOSE
 :* VERSION       001
 :*   CRDATE      1992-16-06
 :*   AUTHOR      (/ Mounji Abdelaziz       , FUNDP              /)
 :*   UPDATE      (/ original issue /)
 :*
 :* REALIZATION   (/ <note, reference to other proc. (optional)> /)
 :*
 :*   CONTROL    (/
 :*                                                                 /)
 :*
 :*   DATA       (/
 :*                     - ad_name[] : the last audit data name found
 :*                                   in the Audit Data Description file
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
 :*   Pre    (/ - 'fp'    a valid file pointer.
 :*             - 'leading_char' is in {'A', ..., 'F'}
 :*             - the input stream is <stream>
 :*                                                                      /)
 :*                                                             END-Pre
 :*
 :*   Post   (/ - if <stream>=<spaces><X_lines><rest>
 :*                where:
 :*                 - <spaces> is any sequence of blanks tabs and new lines
 :*                 - <X_lines> is a valid sequence of lines of the same type
 :*                            'A' or ... or 'F' (see BNF syntax of Audit
 :*                            Data Description file)
 :*                 - <rest> is any character sequence
 :*             - then
 :*                    - the current input stream is <rest>
 :*             - otherwise the error message:
 :*
 :*                     Line  : 'lineNbre' invalid line
 :*
 :*               where 'lineNbre' is the line number is output to stdout /)
 :*                                                             END-Post
 :*
 :*   RETURNS  NONE
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY parse.alpha_lines
 :******************************************************************
*/
alpha_lines(leading_char, fp)
char leading_char;
FILE *fp;
{
    skiplines(fp);
    while (nextchar == leading_char) {
	alpha_line(leading_char, fp);
	skiplines(fp);
    }
}
/*
 :******************************************************************
 :* UNIT-BODY parse.alpha_line
 :*
 :* TITLE         (/ parsing a line 'A' or ... or  'F'  /)
 :*
 :* PURPOSE       (/ parse a line of type 'A' or ... or 'F' and
 :*                  report error in case the syntax of this type of line was
 :*                  not observed /)
 :*                                                      END-PURPOSE
 :* VERSION       001
 :*   CRDATE      1992-16-06
 :*   AUTHOR      (/ Mounji Abdelaziz       , FUNDP              /)
 :*   UPDATE      (/ original issue /)
 :*
 :* REALIZATION   (/ <note, reference to other proc. (optional)> /)
 :*
 :*   CONTROL    (/
 :*                                                                 /)
 :*
 :*   DATA       (/
 :*                     - ad_name[] : the last audit data name found
 :*                                   in the Audit Data Description file
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
 :*   Pre    (/ - 'fp'    a valid file pointer.
 :*             - 'leading_char' is in {'A', ..., 'F'}
 :*             - the input stream is <stream>
 :*                                                                      /)
 :*                                                             END-Pre
 :*
 :*   Post   (/ - if <stream>=<spaces><X_line><rest>
 :*                where:
 :*                 - <spaces> is any sequence of blanks tabs and new lines
 :*                 - <X_line> is a valid line of type
 :*                            'A' or ... or 'F' (see BNF syntax of Audit
 :*                            Data Description file)
 :*                 - <rest> is any character sequence
 :*             - then
 :*                    - the current input stream is <rest>
 :*             - otherwise the error message:
 :*
 :*                     Line  : 'lineNbre' invalid line
 :*
 :*               where 'lineNbre' is the line number is output to stdout /)
 :*                                                             END-Post
 :*
 :*   RETURNS  NONE
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY parse.alpha_line
 :******************************************************************
*/
alpha_line(leading_char, fp)
char leading_char;
FILE *fp;
{
    if (leading_char == nextchar) {
	get_c(fp);
	if (nextchar == ' ') {
	    get_any_seq(fp);
	} else {
	    error_msg("invalid line");
	}
    }
}
/*
 :******************************************************************
 :* UNIT-BODY parse.get_identif
 :*
 :* TITLE         (/ reading of an identifier    /)
 :*
 :* PURPOSE       (/ reading of the identifier that begins the input
 :*                  stream and store it in 'ad_name[]'
 :*                                                           /)
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
 :*      Invariant:
 :*
 :*               - c0 c1 ... ci <stream>
 :*               - 0 <= i < identif_lg
 :*               - <stream> is the current input stream
 :*               - for j=0 .. i
 :*                               cj is a letter, a digit or '_'
 :*                               ad_name[j] = cj
 :*                                                                 /)
 :*
 :*   DATA       (/
 :*                       - ad_name[]    : the last read audit data name.
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
 :*   Pre    (/ - fp    a valid file pointer.
 :*             - nextchar is alphanumeric
 :*             - the input stream is of the form:
 :*                      <identifier><rest>
 :*                                                                      /)
 :*                                                             END-Pre
 :*
 :*   Post   (/ - 'ad_name[]' contains the identifier <identifier1>
 :*               where
 :*                     <identifier1> is <identifier> truncated to its
 :*                     'MaxIdent' first characters;
 :*             - the input stream is <rest>
 :*             - if the identifier is longuer than 'MaxIdent' the message:
 :*                   "warning: too long name"
 :*               is output on stdout                                 /)
 :*                                                             END-Post
 :*
 :*   RETURNS  NONE
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY parse.get_identif
 :******************************************************************
*/
get_identif(fp)
FILE *fp;
{
    int i = 0; /* loop counter */

    while ((isalpha(nextchar) || isdigit(nextchar) || nextchar == '_') &&
	   i < MaxIdent)   {
	ad_name[i] = nextchar;
	i++;
	get_c(fp);
    }
    ad_name[i] = '\0';
    if (i >= MaxIdent) {
	takeOfChar(fp);  /* truncate identifier to its first 30 chars */
	error_msg("warning: too long name");
    }
}
/*
 :******************************************************************
 :* UNIT-BODY parse.takeOfChar
 :*
 :* TITLE         (/ skip alphanumeric characters   /)
 :*
 :* PURPOSE       (/ read all successive alphanumeric characters from
 :*                  the input stream i.e. search for identifier end.
 :*                                                           /)
 :*                                                      END-PURPOSE
 :* VERSION       001
 :*   CRDATE      1992-11-03
 :*   AUTHOR      (/ Mounji Abdelaziz       , FUNDP              /)
 :*   UPDATE      (/ original issue /)
 :*
 :* REALIZATION   (/ <note, reference to other proc. (optional)> /)
 :*
 :*   CONTROL    (/                                              /)
 :*
 :*      Invariant:
 :*
 :*               - c0 c1 ... ci <stream>
 :*               - <stream> is the current input stream
 :*               - for j=0 .. i
 :*                               cj is a letter, a digit or '_'
 :*                                                                 /)
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
 :*   DOCUMENTS       (/ Advanced Security Audit trail analysis on uniX
 :*                             (ASAX also called SAT-X)
 :*                      Implementation design of the NADF Evaluator   /)
 :*                                                  END-REALIZATION
 :* PARAMETERS
 :*
 :*   Pre    (/ - fp    a valid file pointer.
 :*             - nextchar is alphanumeric
 :*             - the input stream is of the form:
 :*                      <alphanum><rest>
 :*               where <alphanum> is a possibly empty sequence of
 :*                                letters digits and '_'
 :*                     <rest>     is a sequence not beginning with a letter
 :*                                a digit or '_'
 :*                                                            /)
 :*
 :*                                                             END-Pre
 :*
 :*   Post   (/ - the input stream is <rest>                          /)
 :*                                                             END-Post
 :*
 :*   RETURNS  NONE
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY parse.takeOfChar
 :******************************************************************
*/
takeOfChar(fp)
FILE *fp;
{
	while ( isalpha(nextchar) || isdigit(nextchar))
		get_c(fp);
}
/*
 :******************************************************************
 :* UNIT-BODY parse.get_int
 :*
 :* TITLE         (/ reading of an integer in the input stream  /)
 :*
 :* PURPOSE       (/ reading of an integer in the input stream and
 :*                  return its value
 :*                                                           /)
 :*                                                      END-PURPOSE
 :* VERSION       001
 :*   CRDATE      1992-11-03
 :*   AUTHOR      (/ Mounji Abdelaziz       , FUNDP              /)
 :*   UPDATE      (/ original issue /)
 :*
 :* REALIZATION   (/ <note, reference to other proc. (optional)> /)
 :*
 :*   CONTROL    (/
 :*   DATA       (/
 :*                                                                /)
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
 :*   Pre    (/ - fp    a valid file pointer.
 :*             - the input stream is in the form:
 :*                   <integer><rest>
 :*               where <integer> is a sequence of digits
 :*                                                            /)
 :*                                                             END-Pre
 :*
 :*   Post   (/ - the input stream is <rest>
 :*             - if the value of this integer is < 65536 it is returned
 :*               otherwise, the error message:
 :*                  "audit data identifiers must be < 65536"
 :*               is sent to stdout                                  /)
 :*
 :*                                                             END-Post
 :*
 :*   RETURNS  "int" (/ the value of the integer /)
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY parse.get_int
 :******************************************************************
*/
get_int(fp)
FILE *fp;
{
    int v = 0;

    while (more_char && isdigit(nextchar) && v < Max_us) {
	v = 10 * v + nextchar - '0';
	get_c(fp);
    }
    if (v >= Max_us) {
	error_msg("audit data identifiers must be < 65536");
	return(-1);
    } else {
	return(v);
    }
}
/*
 :******************************************************************
 :* UNIT-BODY parse.get_any_seq
 :*
 :* TITLE         (/ skip current line /)
 :*
 :* PURPOSE       (/ skip skip all characters up to the new line /)
 :*                                                      END-PURPOSE
 :* VERSION       001
 :*   CRDATE      1992-16-06
 :*   AUTHOR      (/ Mounji Abdelaziz       , FUNDP              /)
 :*   UPDATE      (/ original issue /)
 :*
 :* REALIZATION   (/ <note, reference to other proc. (optional)> /)
 :*
 :*   CONTROL    (/
 :*                                                                 /)
 :*
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
 :*   DOCUMENTS       (/ Advanced Security Audit trail analysis on uniX
 :*                             (ASAX also called SAT-X)
 :*                      Implementation design of the NADF Evaluator   /)
 :*                                                  END-REALIZATION
 :* PARAMETERS
 :*
 :*   Pre    (/ - fp    a valid file pointer.
 :*             - the input stream is <stream>=<any_seq>'\n'<rest>
 :*               where <any_seq> is a possibly empty sequence of any
 :*                              characters other than new line
 :*                     <rest>   is any character sequence
 :*                                                                      /)
 :*                                                             END-Pre
 :*
 :*   Post   (/ - if <stream>=<any_seq>'\n'<rest>   (A)
 :*               where <any_seq> is a possibly empty sequence of any
 :*                              characters other than new line and with a
 :*                              length < 255       (B)
 :*                     <rest>   is any character sequence
 :*               the input stream is <rest>
 :*             - otherwise if (A) is false, the message:
 :*                    "Line : 'lineNbre' invalid line "
 :*
 :*               where 'lineNbre' is the line number is sent to stdout
 :*               if (B) is false, the message
 :*                    "Line : 'lineNbre' too long line"
 :*
 :*               where 'lineNbre' is the line number is sent to stdout /)
 :*                                                              END-Post
 :*   RETURNS  "int" (/ 0 if no error /)
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY parse.get_any_seq
 :******************************************************************
*/
get_any_seq(fp)  /* skips any sequence of characters up to the next line */
FILE *fp;
{
    while (more_char && nextchar != '\n' && NbreChar <= 255) {
	get_c(fp);
    }
    if (NbreChar > 255){
	error_msg("too long line");
	return(-1);
    } else if (nextchar != '\n') {
	error_msg("invalid line ");
	return(-1);
    } else {
	get_c(fp);
	return(0);
    }
}
/*
 :******************************************************************
 :* UNIT-BODY parse.get_c
 :*
 :* TITLE         (/ reading of a character in the input stream /)
 :*
 :* PURPOSE       (/ read the next character in the input stream.
 :*                  increment the line number if end of line.
 :*                                                           /)
 :*                                                      END-PURPOSE
 :* VERSION       001
 :*   CRDATE      1992-11-03
 :*   AUTHOR      (/ Mounji Abdelaziz       , FUNDP              /)
 :*   UPDATE      (/ original issue /)
 :*
 :* REALIZATION   (/ <note, reference to other proc. (optional)> /)
 :*
 :*   CONTROL    (/             /)
 :*   DATA       (/
 :*                      - nextchar  : the last read character
 :*                      - more_char : = EOF
 :*                      - line      : the current line number
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
 :*   Pre    (/ - fp    a valid file pointer.
 :*             - the input stream is of the form:
 :*                      c <rest>
 :*                                                            /)
 :*                                                             END-Pre
 :*
 :*   Post   (/ - if c is end of file
 :*                   morechar = 0
 :*               otherwise morechar = 1
 :*               if c is end of line 'line' is incremented by 1.
 :*             - the input stream is <rest>
 :*
 :*                                                             END-Post
 :*
 :*   RETURNS  NONE
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY parse.get_c
 :******************************************************************
*/
get_c(fp)
FILE *fp;
{

    if ((nextchar = getc(fp)) == EOF) {
	more_char = 0;
    } else {
	more_char = 1;
	NbreChar++;
	if (nextchar == '\n') {
	    NbreChar = 0;
	    line++;
	}
    }
}
/*
 :******************************************************************
 :* UNIT-BODY error.error
 :*
 :* TITLE         (/ output error message /)
 :*
 :*
 :* PURPOSE       (/ output the type, the line and the message of an error.
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
 :*                 - line    : the number of the current line
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
 :*   Pre    (/ - 'msg' is the error message
 :*                                                                  /)
 :*                                                             END-Pre
 :*   Post   (/  - the following line is output to stdout:
 :*                "Line 'line': 'msg'
 :*                                                                 /)
 :*
 :*                                                             END-Post
 :*   RETURNS  NONE
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY parse.error_msg
 :******************************************************************
*/
error_msg(msg)
char *msg;
{
    printf("Line %d: %s\n", line, msg);
    exit(1);
}
init_parse()
{

map_tab_lg = 0;  /* length of the mapping table */
line = 1;
NbreChar = 0;

}



