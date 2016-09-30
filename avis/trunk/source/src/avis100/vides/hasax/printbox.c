/*
 :******************************************************************
 :* MODULE-DESCRIPTION
 :*
 :* TITLE         (/ internal code output    /)
 :*
 :* NAME          printbox.c
 :*
 :* COPYRIGHT     (C)   SIEMENS NIXDORF INFORMATIONSSYSTEME AG  1992
 :*                     ALL RIGHTS RESERVED
 :*
 :* MEMORY-CLASS  6
 :*
 :* PROG-CONVENTS (/ <internal conventions (instance, names,
 :*                  external data, ...). None -> empty text>     /)
 :*
 :* PROCEDURE     storeBox
 :* REMARKS       (/ store a box in a global array /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     prcode
 :* REMARKS       (/ output the internal code of a rule /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     printCodes
 :* REMARKS       (/ output the internal code for all rules  /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     printBox
 :* REMARKS       (/ output the content of a box /)
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
#include "predef.h"

#define NbreBoxes 4000
int lgBPArray = 0;
BoxPtr BoxPtrArray[NbreBoxes];


/*
 :******************************************************************
 :* UNIT-BODY printbox.storeBox
 :*
 :* TITLE         (/ store a box in a global array/)
 :*
 :* PURPOSE       (/ when a box is generated and initialized, it is stored
 :*                  in a global array for a later output   /)
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
 :*                 - BoxPtrArray[]: table of pointers to generated boxes
 :*                 - lgBPArray    : current length of 'BoxPtrArray[]'
 :*                 - NbreBoxes    : maximum length of 'BoxPtrArray[]'
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
 :*   Pre    (/ - 'p' is pointer to a box
 :*             - 'BoxPtrArray[]' is an array of pointers to boxes
 :*             - let 'lgBPArray' = lg_0
 :*                                                                  /)
 :*                                                             END-Pre
 :*   Post   (/ if lgBPArray < NbreBoxes:
 :*                - BoxPtrArray[0], ..., BoxPtrArray[lg_0 - 1] are unchanged
 :*                - BoxPtrArray[lg_0] = 'p'
 :*                - 'lgBPArray' = lg_0 + 1
 :*             otherwise the message:
 :*                "BoxPtrArray table overflow"
 :*             is sent to stdout
 :*
 :*                                                             END-Post
 :*   RETURNS  NONE
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY printbox.storeBox
 :******************************************************************
*/
storeBox(p)
BoxPtr p;
{
    if (lgBPArray == NbreBoxes - 1) {
	printf("BoxPtrArray table overflow\n");
	exit(1);
    } else
	BoxPtrArray[lgBPArray++] = p;
}
/*
 :******************************************************************
 :* UNIT-BODY printbox.prcode
 :*
 :* TITLE         (/ output the internal code of a rule /)
 :*
 :* PURPOSE       (/ output the sequence of boxes generated for a rule /)
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
 :*                 - BoxPtrArray[]: table of pointers to generated boxes
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
 :*   Pre    (/ - 'BoxPtrArray[]' is an array of pointers to boxes
 :*             - 0<=i<lgBPArray-1
 :*                                                                  /)
 :*                                                             END-Pre
 :*   Post   (/ let k = min{j | i<=j<lgBPArray and BoxPtrArray[j] = NULL}
 :*             the content of the following sequence of boxes:
 :*                 BoxPtrArray[i], ..., BoxPtrArray[k-1]
 :*             is output to stdout                                  /)
 :*                                                             END-Post
 :*   RETURNS  "int" ( the number of boxes that were output /)
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY printbox.prcode
 :******************************************************************
*/
prcode(i)
int i;
{
    int j;

    j = i;
    while (BoxPtrArray[j] != NULL) {
	printBox(BoxPtrArray[j++]);
    }
    return(j - i);
}
/*
 :******************************************************************
 :* UNIT-BODY printbox.printCodes
 :*
 :* TITLE         (/ output the internal code of all rules /)
 :*
 :* PURPOSE       (/ output the sequence of boxes generated for all rules
 :*                  that were compiled                              /)
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
 :*                 - BoxPtrArray[]: table of pointers to generated boxes
 :*                 - lgBPArray    : length of 'BoxPtrArray[]'
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
 :*   Pre    (/                                                      /)
 :*                                                             END-Pre
 :*   Post   (/ the content of the following sequence of boxes:
 :*                 BoxPtrArray[0], ..., BoxPtrArray[lgBPArray - 1]
 :*             is output to stdout                                  /)
 :*                                                             END-Post
 :*   RETURNS NONE
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY printbox.printCodes
 :******************************************************************
*/
printCodes()
{
    int i, k;

    i = 0;
    while (i != lgBPArray) {
	k = prcode(i);
	i += k + 1;
	if (i != lgBPArray)
	    printf("\n     ****** next rule *****  \n\n");
    }
}
/*
 :******************************************************************
 :* UNIT-BODY printbox.printBox
 :*
 :* TITLE         (/ output the content of a box   /)
 :*
 :* PURPOSE       (/ the address of the box as well as the value of each of
 :*                  its fields are output to stdout               /)
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
 :*   Pre    (/ 'p' is a pointer to a generated box      /)
 :*                                                             END-Pre
 :*   Post   (/ the values of each of the fields of the box pointed by 'p' are
 :*             sent to stdout. This includes at least the following:
 :*               - box address
 :*               - the abstract instruction identifier
 :*               - the address of the next box
 :*             The other fields are abstract instruction identifier dependent
 :*                                                                     /)
 :*                                                             END-Post
 :*   RETURNS NONE
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY printbox.printBox
 :******************************************************************
*/
printBox(p)
BoxPtr p;
{
  switch(p->op) {

    case add1:
			   printf(" %x ", p);
			   printf("|   +   | ");
			   printf("delta = %x | ", p->arg.arithmbox.deltaIn);
			   printf("gma1 = %x | ", p->arg.arithmbox.gamma1);
			   printf("gma2 = %x | ", p->arg.arithmbox.gamma2);
			   printf("alpha = %x |\n", p->alpha);
			   break;

    case minus1:
			   printf(" %x ", p);
			   printf("|   -   | ");
			   printf("delta = %x | ", p->arg.arithmbox.deltaIn);
			   printf("gma1 = %x | ", p->arg.arithmbox.gamma1);
			   printf("gma2 = %x | ", p->arg.arithmbox.gamma2);
			   printf("alpha = %x |\n", p->alpha);
			   break;

    case un_minus1:
			   printf(" %x ", p);
			   printf("|  --   | ");
			   printf("delta = %x | ", p->arg.arithmbox.deltaIn);
			   printf("gma = %x | ", p->arg.un_minusbox.gamma);
			   printf("alpha = %x |\n", p->alpha);
			   break;

    case mult1:
			   printf(" %x ", p);
			   printf("|   *   | ");
			   printf("delta = %x | ", p->arg.arithmbox.deltaIn);
			   printf("gma1 = %x | ", p->arg.arithmbox.gamma1);
			   printf("gma2 = %x | ", p->arg.arithmbox.gamma2);
			   printf("alpha = %x |\n", p->alpha);
			   break;

    case mod1:
			   printf(" %x ", p);
			   printf("|  mod  | ");
			   printf("delta = %x | ", p->arg.arithmbox.deltaIn);
			   printf("gma1 = %x | ", p->arg.arithmbox.gamma1);
			   printf("gma2 = %x | ", p->arg.arithmbox.gamma2);
			   printf("alpha = %x |\n", p->alpha);
			   break;

    case div1:
			   printf(" %x ", p);
			   printf("|  div  | ");
			   printf("delta = %x | ", p->arg.arithmbox.deltaIn);
			   printf("gma1 = %x | ", p->arg.arithmbox.gamma1);
			   printf("gma2 = %x | ", p->arg.arithmbox.gamma2);
			   printf("alpha = %x |\n", p->alpha);
			   break;

    case move_int1:
			   printf(" %x ", p);
			   printf("| mv_int| ");
			   printf("dest = %x | ", p->arg.movebox.dest);
			   printf("source = %x | ", p->arg.movebox.source);
			   printf("alpha = %x |\n", p->alpha);
			   break;

    case move_str1:
			   printf(" %x ", p);
			   printf("|mv_str | ");
			   printf("dest = %x | ", p->arg.movebox.dest);
			   printf("source = %x | ", p->arg.movebox.source);
			   printf("alpha = %x |\n", p->alpha);
			   break;

    case mv_int1:
			   printf(" %x ", p);
			   printf("|MV_int | ");
			   printf("val = %d | ", p->arg.mv_intbox.val);
			   printf("deltaIn = %x | ", p->arg.mv_intbox.deltaIn);
			   printf("alpha = %x |\n", p->alpha);
			   break;

    case call_f1:
			   printf(" %x ", p);
			   printf("| call_f| ");
			   printf("fptr = %x | ", p->arg.callbox.fpptr);
			   printf("delta = %x | ", p->arg.callbox.deltaIn);
			   printf("alpha = %x |\n", p->alpha);
			   break;

    case call_p1:
			   printf(" %x ", p);
			   printf("| call_p| ");
			   printf("fptr = %x | ", p->arg.callbox.fpptr);
			   printf("delta = %x | ", p->arg.callbox.deltaIn);
			   printf("alpha = %x\n", p->alpha);
			   break;


    case assign_addr1:
			   printf(" %x ", p);
			   printf("|:=_addr| ");
			   printf("gma1 = %x | ", p->arg.assign_addrbox.gamma1);
			   printf("gma2 = %x | ", p->arg.assign_addrbox.gamma2);
			   printf("alpha = %x |\n", p->alpha);
			   break;

    case assign1:
			   printf(" %x ", p);
			   printf("| :=_int| ");
			   printf("gma1 = %x | ", p->arg.assign_intbox.gamma1);
			   printf("gma2 = %x | ", p->arg.assign_intbox.gamma2);
			   printf("alpha = %x |\n", p->alpha);
			   break;

    case assign_str1:
			   printf(" %x ", p);
			   printf("| :=_str| ");
			   printf("gma1 = %x | ", p->arg.assign_intbox.gamma1);
			   printf("gma2 = %x | ", p->arg.assign_intbox.gamma2);
			   printf("alpha = %x |\n", p->alpha);
			   break;


    case cpy_intAddr1:
			   printf(" %x ", p);
			   printf("|cpy_intAddr| ");
			   printf("gma = %x | ", p->arg.gamma);
			   printf("alpha = %x |\n", p->alpha);
			   break;

    case cpy_strAddr1:
			   printf(" %x ", p);
			   printf("|cpy_strAddr| ");
			   printf("gma = %x | ", p->arg.gamma);
			   printf("alpha = %x |\n", p->alpha);
			   break;

    case cpy_str1:
			   printf(" %x ", p);
			   printf("|  cpy_str  | ");
			   printf("gma = %x | ", p->arg.gamma);
			   printf("alpha = %x |\n", p->alpha);
			   break;

    case setLen1:
			   printf(" %x ", p);
			   printf("|  setLen   | ");
			   printf("size = %d | ", p->arg.size);
			   printf("alpha = %x |\n", p->alpha);
			   break;

    case addLen1:
			   printf(" %x ", p);
			   printf("|  addLen   | ");
			   printf("gma = %x | ", p->arg.gamma);
			   printf("alpha = %x |\n", p->alpha);
			   break;

    case for_current1:
			   printf(" %x ", p);
			   printf("|for_current| ");
			   printf("ruleCode = %x |",
				   p->arg.triggerbox.ruleCode);
			   printf(" alpha = %x | \n", p->alpha);
			   break;

    case for_next1:
			   printf(" %x ", p);
			   printf("| for_next  | ");
			   printf("ruleCode = %x |",
				   p->arg.triggerbox.ruleCode);
			   printf(" alpha = %x |\n", p->alpha);
			   break;

    case at_completion1:
			   printf(" %x ", p);
			   printf("| at_compl  | ");
			   printf("ruleCode = %x |",
				   p->arg.triggerbox.ruleCode);
			   printf(" alpha = %x | \n", p->alpha);
			   break;

    case less :
			   printf(" %x ", p);
			   printf("|   <   | ");
			   printf("gma1 = %x | ",
				   p->arg.relationalbox.gamma1);
			   printf("gma1 = %x | ",
				   p->arg.relationalbox.gamma2);
			   printf("True = %x | ",
				   p->arg.relationalbox.alphaTrue);
			   printf("False = %x | \n", p->alpha);
/*                         printCode(p->arg.relationalbox.alphaTrue); */
			   break;

    case leq :
			   printf(" %x ", p);
			   printf("|   <=  | ");
			   printf("gma1 = %x | ",
				   p->arg.relationalbox.gamma1);
			   printf("gma1 = %x | ",
				   p->arg.relationalbox.gamma2);
			   printf("True = %x | ",
				   p->arg.relationalbox.alphaTrue);
			   printf("False = %x | \n", p->alpha);
/*                         printCode(p->arg.relationalbox.alphaTrue);     */
			   break;

    case neq :
			   printf(" %x ", p);
			   printf("|  neq  | ");
			   printf("gma1 = %x | ",
				   p->arg.relationalbox.gamma1);
			   printf("gma1 = %x | ",
				   p->arg.relationalbox.gamma2);
			   printf("True = %x | ",
				   p->arg.relationalbox.alphaTrue);
			   printf("False = %x | \n", p->alpha);
  /*                       printCode(p->arg.relationalbox.alphaTrue); */
			   break;

    case equal :
			   printf(" %x ", p);
			   printf("|   =   | ");
			   printf("gma1 = %x | ",
				   p->arg.relationalbox.gamma1);
			   printf("gma1 = %x | ",
				   p->arg.relationalbox.gamma2);
			   printf("True = %x | ",
				   p->arg.relationalbox.alphaTrue);
			   printf("False = %x | \n", p->alpha);
   /*                      printCode(p->arg.relationalbox.alphaTrue); */
			   break;

    case geq :
			   printf(" %x ", p);
			   printf("|   >=  | ");
			   printf("gma1 = %x | ",
				   p->arg.relationalbox.gamma1);
			   printf("gma1 = %x | ",
				   p->arg.relationalbox.gamma2);
			   printf("True = %x | ",
				   p->arg.relationalbox.alphaTrue);
			   printf("False = %x | \n", p->alpha);
  /*                       printCode(p->arg.relationalbox.alphaTrue); */
			   break;

    case greater :
			   printf(" %x ", p);
			   printf("|   >   | ");
			   printf("gma1 = %x | ",
				   p->arg.relationalbox.gamma1);
			   printf("gma1 = %x | ",
				   p->arg.relationalbox.gamma2);
			   printf("True = %x | ",
				   p->arg.relationalbox.alphaTrue);
			   printf("False = %x | \n", p->alpha);
/*                         printCode(p->arg.relationalbox.alphaTrue);     */
			   break;

    case StrLess1 :
			   printf(" %x ", p);
			   printf("| Str<  | ");
			   printf("gma1 = %x | ",
				   p->arg.relationalbox.gamma1);
			   printf("gma1 = %x | ",
				   p->arg.relationalbox.gamma2);
			   printf("True = %x | ",
				   p->arg.relationalbox.alphaTrue);
			   printf("False = %x | \n", p->alpha);
/*                         printCode(p->arg.relationalbox.alphaTrue); */
			   break;

    case StrLeq1 :
			   printf(" %x ", p);
			   printf("| Str<= | ");
			   printf("gma1 = %x | ",
				   p->arg.relationalbox.gamma1);
			   printf("gma1 = %x | ",
				   p->arg.relationalbox.gamma2);
			   printf("True = %x | ",
				   p->arg.relationalbox.alphaTrue);
			   printf("False = %x | \n", p->alpha);
/*                         printCode(p->arg.relationalbox.alphaTrue);     */
			   break;

    case StrEqual1 :
			   printf(" %x ", p);
			   printf("| Str=  | ");
			   printf("gma1 = %x | ",
				   p->arg.relationalbox.gamma1);
			   printf("gma1 = %x | ",
				   p->arg.relationalbox.gamma2);
			   printf("True = %x | ",
				   p->arg.relationalbox.alphaTrue);
			   printf("False = %x | \n", p->alpha);
   /*                      printCode(p->arg.relationalbox.alphaTrue); */
			   break;

    case StrGeq1 :
			   printf(" %x ", p);
			   printf("| Str>= | ");
			   printf("gma1 = %x | ",
				   p->arg.relationalbox.gamma1);
			   printf("gma1 = %x | ",
				   p->arg.relationalbox.gamma2);
			   printf("True = %x | ",
				   p->arg.relationalbox.alphaTrue);
			   printf("False = %x | \n", p->alpha);
  /*                       printCode(p->arg.relationalbox.alphaTrue); */
			   break;

    case StrNeq1 :
			   printf(" %x ", p);
			   printf("| Str != | ");
			   printf("gma1 = %x | ",
				   p->arg.relationalbox.gamma1);
			   printf("gma1 = %x | ",
				   p->arg.relationalbox.gamma2);
			   printf("True = %x | ",
				   p->arg.relationalbox.alphaTrue);
			   printf("False = %x | \n", p->alpha);
  /*                       printCode(p->arg.relationalbox.alphaTrue); */
			   break;

    case StrGreater1 :
			   printf(" %x ", p);
			   printf("| Str>  | ");
			   printf("gma1 = %x | ",
				   p->arg.relationalbox.gamma1);
			   printf("gma1 = %x | ",
				   p->arg.relationalbox.gamma2);
			   printf("True = %x | ",
				   p->arg.relationalbox.alphaTrue);
			   printf("False = %x | \n", p->alpha);
/*                         printCode(p->arg.relationalbox.alphaTrue);     */
			   break;

    case present1 :
			   printf(" %x ", p);
			   printf("|present| ");
			   printf("gma = %x | ",
				   p->arg.presentbox.str_addr);
			   printf("True = %x | ",
				   p->arg.presentbox.alphaTrue);
			   printf("False = %x | \n", p->alpha);
  /*                       printCode(p->arg.presentbox.alphaTrue); */
			   break;

	}

}

init_printbox()
{
    	while (lgBPArray > 0) 
		free(BoxPtrArray[--lgBPArray]);
}
