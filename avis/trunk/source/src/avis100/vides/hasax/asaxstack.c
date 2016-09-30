/*
 :******************************************************************
 :* MODULE-DESCRIPTION
 :*
 :* TITLE         (/ management of the current record indirection table and
 :*                  the three lists of active rules       /)
 :* NAME          stack.c
 :*
 :* COPYRIGHT     (C)   SIEMENS NIXDORF INFORMATIONSSYSTEME AG  1992
 :*                     ALL RIGHTS RESERVED
 :*
 :* MEMORY-CLASS  6
 :*
 :* PROG-CONVENTS (/ <internal conventions (instance, names,
 :*                  external data, ...). None -> empty text>     /)
 :*
 :* PROCEDURE     push
 :* REMARKS       (/ push a triggered rule descriptor /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     getAddr
 :* REMARKS       (/ get an active rule descriptors list address /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     pop
 :* REMARKS       (/ pop a triggered rule descriptor /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     scanCR
 :* REMARKS       (/ update of the current record table pointers /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     fill_CR_holes
 :* REMARKS       (/ filling holes related to the current audit record /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     install_ad
 :* REMARKS       (/ install an audit data id in the current record table /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     insert_hole
 :* REMARKS       (/ insert a new hole related to a field name /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     get_location
 :* REMARKS       (/ find insertion point in the current record table /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     print_CR_table
 :* REMARKS       (/ Current Record Table output /)
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

#include "asax.h"
#ifdef SunOS
#define ALIGN_REC 4
#else
#define ALIGN_REC 2
#endif

int CR_table_lg = 0;   /* current length of the current record table */
CR_table_entry CR_table[Max_ad_id];/* the current reocrd table */

extern hole *NOTFOUND; 	/* the null string */
/* 
static unsigned short Nl = 0;
static hole *NullStr = (hole *) &Nl;  */  /* the null string in NADF format */


/*
 :******************************************************************
 :* UNIT-BODY stack.push
 :*
 :* TITLE         (/ push a triggred rule descriptor/)
 :*
 :* PURPOSE       (/ push a triggered rule descriptor on top of a specified
 :*                  list of active rules   /)
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
 :*   Pre    (/ - 'eff_ptr' is the address of a triggered rule descriptor
 :*             - 'tr_m' identifies one of the three lists of active rules:
 :*               'tr_m' is in {for_current1, for_next1, at_completion}
 :*
 :*              'eff_ptr'
 :*                 |
 :*                ____________
 :*               | rule_descr |
 :*                ------------
 :*
 :*           let ptr be the address of the list specified by 'tr_m'
 :*
 :*              ptr
 :*                |
 :*                _____________               _____________
 :*               | rule_descr_1|---> ... --->| rule_descr_n|
 :*                -------------               -------------
 :*
 :*            where n>=0 (list may be empty)
 :*                                                                  /)
 :*                                                             END-Pre
 :*   Post   (/
 :*             ptr
 :*                |
 :*               ___________      _____________               _____________
 :*              | rule_descr|--->| rule_descr_1|---> ... --->| rule_descr_n|
 :*               -----------      -------------               -------------
 :*
 :*                                                                  /)
 :*                                                             END-Post
 :*   RETURNS  NONE
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY stack.push
 :******************************************************************
*/
push(eff_ptr, tr_m)
trigD *eff_ptr;
enum operation tr_m;
{
    trigD **head_addr, **getAddr();

    head_addr = (trigD **) getAddr(tr_m);
    eff_ptr->next = *head_addr;
    *head_addr = eff_ptr;
}
/*
 :******************************************************************
 :* UNIT-BODY stack.getAddr
 :*
 :* TITLE         (/get an active rule descriptors list address /)
 :*
 :* PURPOSE       (/ get the address of a list of active rule descriptors
 :*                  pointer given an identifier of such a list      /)
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
 :*   Pre    (/ - 'tr_m' identifies one of the three lists of active rules:
 :*               'tr_m' is in {for_current1, for_next1, at_completion}
 :*                                                                  /)
 :*                                                             END-Pre
 :*   Post   (/ the returned value is the pointer to pointer to the list
 :*             specified by 'tr_m'
 :*                                                                  /)
 :*                                                             END-Post
 :*   RETURNS "hole **" (/ address of the specified list pointer  /)
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY stack.getAddr
 :******************************************************************
*/
trigD **getAddr(tr_m)
enum operation tr_m;
{
    switch(tr_m) {
	case for_current1  : return(&current);
	case for_next1     : return(&next);
	case at_completion1: return(&compl);
	default            : error1(fatal, 24, "getAddr() of stack.c");
			     return (NULL);
    }
}
/*
 :******************************************************************
 :* UNIT-BODY stack.pop
 :*
 :* TITLE         (/ pop a triggred rule descriptor/)
 :*
 :* PURPOSE       (/ pop a triggered rule descriptor from top of a specified
 :*                  list of active rules   /)
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
 :*   Pre    (/ - 'tr_m' identifies one of the three lists of active rules:
 :*               'tr_m' is in {for_current1, for_next1, at_completion}
 :*
 :*           let ptr be the address of the list specified by 'tr_m'
 :*               ptr_0 = ptr
 :*       ptr
 :*        |
 :*        _____________      _____________               _____________
 :*       | rule_descr_1|--->| rule_descr_2|---> ... --->| rule_descr_n|
 :*        -------------      -------------               -------------
 :*
 :*            where n>=0 (list may be empty)
 :*                                                                  /)
 :*                                                             END-Pre
 :*   Post   (/
 :*       p1                 ptr_1
 :*        |                  |
 :*        _____________      _____________               _____________
 :*       | rule_descr_1|--->| rule_descr_2|---> ... --->| rule_descr_n|
 :*        -------------      -------------               -------------
 :*
 :*            - ptr = ptr_1
 :*            - the returned value is p1 which is NULL if ptr_0 = NULL
 :*
 :*                                                                  /)
 :*                                                             END-Post
 :*   RETURNS  "hole" (/ the address of the descriptor on top of the list.
 :*                      = NULL if this list is empty              /)
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY stack.pop
 :******************************************************************
*/
trigD *pop(tr_m)
enum operation tr_m;
{
    trigD **p, *p1, **rr, **getAddr();

    rr = getAddr(tr_m);
    if (*rr != NULL) {
	p1 = *rr;
	*rr = (*rr)->next;
	return(p1);
    } else {
	return(NULL);
    }
}
/*
 :******************************************************************
 :* UNIT-BODY stack.scanCR
 :*
 :* TITLE         (/ update of the current record table pointers  /)
 :*
 :* PURPOSE       (/ Scan the indirection table to update the pointers inside
 :*                  the newly read (NADF) audit record                 /)
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
 :*   DATA       (/
 :*                   - nullStr: pointer to the null string
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
 :*   Pre    (/ - 'tab' is current record table address
 :*             - 'lgtab' is the length of the current record table
 :*             - let the set of audit data ids corresponding to the set of
 :*               all field names in the text of the rules be
 :*                   ID1 = {id_0, ... id_N}
 :*             - let id_i = tab[i].ad_id (0<=i<lgtab)
 :*             - id_0 < ... < id_N  (N>=-1, lgtab = N+1)
 :*       tab
 :*        |
 :*       _________________________________________________________
 :*      | id_0 |       ...      | id_i |       ...         | id_N |
 :*       _________________________________________________________
 :*      |  ?   |       ...      |  ?   |       ...         |  ?   |
 :*       ---------------------------------------------------------
 :*
 :*                     Fig1: the current record table
 :*
 :*
 :*             - 'p' is the address of the current NADF audit record:
 :*             - let the set of all audit data ids in the current record be:
 :*                   ID2 = {ID_0, ..., ID_n}
 :*             - ID_0 < ... < ID_n    (n>=-1)
 :*      p
 :*      |
 :*       ______________________________________________________________
 :*      |RecLen|ID_0|lg_0|val_0|...|ID_i|lg_i|val_i|...|ID_n|lg_n|val_n|
 :*       --------------------------------------------------------------
 :*
 :*                     Fig2: the current record
 :*
 :*             - RecLen is the length of the whole record (in bytes)
 :*             - ID_i is the identifier of a field name having the value
 :*               val_i whose length (in bytes) is lg_i (i=0, ..., n)
 :*
 :*                                                                  /)
 :*                                                             END-Pre
 :*   Post   (/ - let PRESENT be {id in ID1 | id is in ID2}
 :*             - let I be {i | tab[i].ad_id = id and id in PRESENT}
 :*             - let J = {0, 1, ..., N}\I
 :*             - for i in I tab[i].ad_value is the address of lg_i
 :*             - for j in J tab[j].ad_value = NOTFOUND
 :*                                                                  /)
 :*                                                             END-Post
 :*   RETURNS NONE
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY stack.scanCR
 :******************************************************************
*/
scanCR(tab, lgtab, p) /* Scan the indirection table to update the pointers */
		      /* inside the newly read audit record. Must undoubt- */
		      /* edly be changed according to the format of the    */
		      /* read audit record from Siemens AG.                 */
register CR_table_entry tab[];
int lgtab;
char *p;
{
    char *base_addr, *rec_end, *rec_ptr;
    int i, data_id, *rec_len, len_rec;
    unsigned short *usp;



    i = 0;
    rec_len = (int *) p;
    base_addr = rec_ptr = (char *) p;
    len_rec = *rec_len;
    rec_ptr += sizeof(int);                     /* skip record lgth */
    usp = (unsigned short *) rec_ptr;
    rec_end = base_addr + len_rec - sizeof(int);
    while (i < lgtab && rec_ptr <= rec_end) {
	if (*usp == tab[i].ad_id) {
	    rec_ptr += usz;                     /* skip data id */
	    tab[i].ad_value = (hole *) rec_ptr;
	    usp = (unsigned short *) rec_ptr;
            rec_ptr += usz + *usp + ((int) (*usp)*(ALIGN_REC -1)%ALIGN_REC);
	    usp = (unsigned short *) rec_ptr;
	    i++;
	} else if (*usp < tab[i].ad_id) {
	    rec_ptr += usz;
	    usp = (unsigned short *) rec_ptr;
            rec_ptr += usz + *usp + ((int) (*usp)*(ALIGN_REC -1)%ALIGN_REC);
	    usp = (unsigned short *) rec_ptr;
	} else {   /* *p > tab[i].ad_id */
	    tab[i].ad_value = NOTFOUND;
	    i++;
	}
    }
    while (i < lgtab)
	tab[i++].ad_value = NOTFOUND;
}
/*
 :******************************************************************
 :* UNIT-BODY rdescr.fill_CR_holes
 :*
 :* TITLE         (/ filling holes related to the current audit record /)
 :*
 :* PURPOSE       (/ At the end of parsing, each entry in the current record
 :*                  table is assigned a list of holes related to references
 :*                  to the corresponding field name. This function scans
 :*                  this table and fills each of these lists of holes  /)
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
 :*                     - CR_table[]   : the current record table
 :*                     - CR_table_lg  : length of 'CR_table[]'
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
 :*   Pre    (/ - CR_table[i].ad_value are list of holes
 :*               (i=0, ..., N=CR_table_lg-1)
 :*      'CR_table'
 :*        |
 :*       _______________________________________
 :*      | id_0 |  ...   | id_i |   ...   | id_N |
 :*       _______________________________________
 :*      |  |   |  ...   |      |   ...   |  |   |
 :*       --|--------------------------------|---
 :*         |                                |
 :*         _____               ______       |
 :*        | h1_1|---> ... --->| h1_m1|       _____               ______
 :*         -----               ------       | hN_1|---> ... --->| hN_mN|
 :*                                           -----               ------
 :*
 :*
 :*               where - mi>=0 for i=1, ... N (lists may be empty)
 :*                     - hi_j is the address of the next hole in the list
 :*                       (i=1, ..., N) and (j=1, ..., mj)
 :*            - let value_i be the address of 'CR_table[i].ad_value'
 :*              (i=0, ..., N-1)                                     \)
 :*                                                             END-Pre
 :*   Post   (/ - for i=0, ..., N:  the list CR_table[i].ad_value is filled
 :*               with the value value_i:
 :*      'CR_table'
 :*        |
 :*       ___________________________
 :*      |   id_0  |  ...   |   id_N |
 :*       ___________________________
 :*      | value_0 |  ...   |value_N |
 :*       ---------------------------
 :*
 :*         ________          ________
 :*        | value_0|  ...   | value_0|       ________       ________
 :*         --------          --------       | value_N| ... | value_N|
 :*                                           --------       --------
 :*                                                                     /)
 :*                                                             END-Post
 :*   RETURNS   NONE
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY  rule_decl.fill_CR_holes
 :******************************************************************
*/
fill_CR_holes()
{
    int i;

    i = 0;
    while (i < CR_table_lg) {
/* ### cast of second arg of fill_holes to char ** ### */
	fill_holes((holePtr) CR_table[i].ad_value, 
		   (char **) &CR_table[i].ad_value);
	i++;
    }
}
/*
 :******************************************************************
 :* UNIT-BODY stack.install_ad
 :*
 :* TITLE         (/ install an audit data id in the current record table /)
 :*
 :* PURPOSE       (/ install an audit data id in the current record table /)
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
 :*                     - CR_table[]  : the current record table
 :*                     - CR_table_lg : length of 'CR_table[]'
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
 :*   Pre    (/ - 'ad_id' is an audit data identifier
 :*             - let ad_id_i = CR_table[i].ad_id (0<=i<CR_table_lg)
 :*             - let 'CR_table_lg' = CR_table_lg_0
 :*             - ad_id_0 < ... < ad_id_CR_table_lg_0
 :*             - the set of audit data ids is
 :*                   ID = {ad_id_0, ... ad_id_CR_table_lg_0}
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/     - the set of audit data ids is
 :*                      ID U {id_0}
 :*                 - if id_0 is in ID
 :*                     - 'CR_table_lg' = CR_table_lg_0
 :*                     - '*found' = 1
 :*                   otherwise
 :*                     -'CR_table_lg' = CR_table_lg_0 + 1
 :*                     - '*found' = 0
 :*                 - ad_id_0 < ... < ad_id_CR_table_lg
 :*                 - the returned value is k such that
 :*                         CR_table[k].ad_id = id_0
 :*                                                                 /)
 :*                                                             END-Post
 :*   RETURNS  "int" (/ index of this audit data id in 'CR_table[]' /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY stack.install_ad
 :******************************************************************
*/
install_ad(ad_id, found)
short int ad_id;
unsigned short *found;
{
	int pos, i;

	pos = get_location(ad_id);
	if (pos == CR_table_lg) {
		*found = 0;
		CR_table[pos].ad_id = ad_id;
		CR_table_lg++;
	} else if (CR_table[pos].ad_id == ad_id) {
		*found = 1;
	} else {
		*found = 0;
		for (i=CR_table_lg; i>pos; i--)
			CR_table[i] = CR_table[i-1];
		CR_table[pos].ad_id = ad_id;    /* i == pos */
		CR_table_lg++;
	  }
	return (pos);
}
/*
 :******************************************************************
 :* UNIT-BODY stack.insert_hole
 :*
 :* TITLE         (/ insert a new hole related to a field name       /)
 :*
 :* PURPOSE       (/
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
 :*   Pre    (/ - 'i' is an index in 'CR_table[]'
 :*             - 'gamma' is the address of a hole to be inserted:
 :*
 :*              'gamma'
 :*                 |
 :*                ___
 :*               | h |
 :*                ---
 :*             - the list of holes related to the entry 'i' in 'CR_table[]'
 :*               is of the following form:
 :*
 :*             CR_table[i].ad_value
 :*                |
 :*                ___               ___
 :*               | h1|---> ... --->| hn|   (n>=0)
 :*                ---               ---
 :*             - 'found' = 0 if this list is empty (n=0)
 :*                       = 1 otherwise             (n>0)
 :*                                                                  /)
 :*                                                             END-Pre
 :*   Post (/   CR_table[i].ad_value
 :*                |
 :*                ___      ___               ___
 :*               | h |--->| h1|---> ... --->| hn|
 :*                ---      ---               ---
 :*                                                                   /)
 :*   RETURNS  BoxPtr (/ A pointer to the generated internal code /)
 :*
 :*   REMARKS     (/ Definitions:
 :*                   - refer to <DOCUMENTS> for a detailed description of
 :*                     the internal representation (boxes).
 :*                                                         /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY stack.insert_hole
 :******************************************************************
*/
int insert_hole(gamma, i, found)
hole *gamma;
unsigned short i;
unsigned short found;
{
	if (!found)     {
		*gamma = (hole) NULL;
	} else {
		*gamma = (hole) CR_table[i].ad_value;
	}
	CR_table[i].ad_value = (hole *) gamma;
}
/*
 :******************************************************************
 :* UNIT-BODY rdescr.get_location
 :*
 :* TITLE         (/ find an insertion point in the current record table /)
 :*
 :* PURPOSE       (/ find an index in 'CR_table[]' such that all audit data
 :*                  id's below this index are less than the given audit data
 :*                  id and all those higher than this index are greater  /)
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
 :*                       - CR_table[]  : the current record table
 :*                       - CR_table_lg : length of 'CR_table[]'
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
 :*   Pre    (/ - CR_table[0].ad_id < ... < CR_table[CR_table_lg - 1].ad_id
 :*             - CR_table >= 0
 :*             - 'id' >= 0
 :*                                                                 /)
 :*   Post   (/ the returned value is i such that:
 :*                - for j: 0<=j<i            CR_table[j].ad_id < 'id'
 :*                - for j: i<=j<CR_table_lg  CR_table[j].ad_id >= 'id'
 :*                                                                   /)
 :*                                                             END-Post
 :*   RETURNS  "int" (/ >= 0 an insertion point in 'CR_table[]'       /)
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY rdescr.get_location
 :******************************************************************
*/
get_location(id)
unsigned short id;
{
	int low, high, mid;

	low = 0;
	high = CR_table_lg - 1;
	while (high != low - 1) {
		mid = (low + high) / 2;
		if (CR_table[mid].ad_id == id) {
			return (mid);
		} else if (CR_table[mid].ad_id < id) {
			low = mid + 1;
		} else {/* (CR_table[mid].ad_id > id) */
			high = mid - 1;
		}
	}
	return (low);
}
/*
 :******************************************************************
 :* UNIT-BODY rdescr.print_CR_table
 :*
 :* TITLE         (/  Current Record Table output /)
 :*
 :* PURPOSE       (/ the content of the table 'CR_table[]' is output to
 :*                  stdout                                       /)
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
 :*                       - CR_table[]  : Current Record table
 :*                       - CR_table_lg : length of 'CR_table[]'
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
 :*   Post   (/ the content of 'CR_table[]' is output to stdout in the
 :*             following format:
 :*                - a header:
 :*
 :*            i  |  data id      |  data address | data value
 :*            -----------------------------------------------
 :*                - a number (= 'CR_table_lg') of lines are output each
 :*                  one been related to a field name and containing the
 :*                  fields (self explanatory) in the header above
 :*                                                                     /)
 :*                                                             END-Post
 :*   RETURNS   NONE
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY  rule_decl.print_CR_table
 :******************************************************************
*/
print_CR_table()
{
	int i, ad_id;
	holePtr p;

   printf("  i  |  data id      |  data address | data value\n");
   printf("  -----------------------------------------------\n");
   for (i=0; i<CR_table_lg; i++) {
	printf(" %2d  |     %5d     |     %x     |     %6x     \n",
	i, CR_table[i].ad_id, &CR_table[i].ad_value, CR_table[i].ad_value);

   }
}
init_stack()
{
CR_table_lg = 0;   /* current length of the current record table */
}
