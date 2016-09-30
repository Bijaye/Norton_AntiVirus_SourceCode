/*
 :******************************************************************
 :* MODULE-DESCRIPTION
 :*
 :* TITLE         (/ the internal code emulator /)
 :*
 :* NAME             emulate.c
 :*
 :* COPYRIGHT     (C)   SIEMENS NIXDORF INFORMATIONSSYSTEME AG  1992
 :*                     ALL RIGHTS RESERVED
 :*
 :* MEMORY-CLASS  6
 :*
 :* PROG-CONVENTS (/ <internal conventions (instance, names,
 :*                  external data, ...). None -> empty text>     /)
 :*
 :* PROCEDURE     emulate
 :* REMARKS       (/ the internal code emulator /)
 :* ENTRY         EXTERNAL
 :*
 :*-----------------------------------------------------------------
 :*
 :* VERSION       001
 :*   CRDATE      1992-03-09
 :*   AUTHOR      (/ Mounji Abdelaziz       , FUNDP /)
 :*   UPDATE      (/ original issue /)
 :*
 :* END-MODULE-DESCRIPTION rule_head.c
 :******************************************************************
*/
#include "asax.h"

#if defined(POSIXREGEX) || defined(GNUREGEX) || defined(BSDREGEX)
#include "regex.h"
#define PATBUFSZ	sizeof(struct re_pattern_buffer)
#define LGSZ		sizeof(long)
#elif !defined(STDREGEX)
#define STDREGEX
#endif

#define isize sizeof(int)

/* 
 * 	from standard.c
 *	All entries in the indirection table corresponding to 
 *	audit data not present in the current record must point to the
 *	area NOTFOUND
*/
extern hole *NOTFOUND;	/* from standard.c represents the null string */

/*
 :******************************************************************
 :* UNIT-BODY emulate.emulate
 :*
 :* TITLE         (/ the internal code emulator/)
 :*
 :* PURPOSE       (/ This function receives a pointer to an abstract machine
 :*                  instruction and execute it according to its semantic /)
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
 :*   Pre    (/ - 'p' is the address of an abstract machine instruction
 :*                |
 :*                |
 :*                ________________________________
 :*               | ai_id | operandeList | nextBox |
 :*                --------------------------------
 :*
 :*             where
 :*               - ai_id   is the abstract instruction identifier
 :*               - nextBox is the address of the next box
 :*               - operandeList is the list of operandes of the instruction
 :*                 which is abstract instruction identifier dependent
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/ - the abstract instruction is executed according to its
 :*               semantics and the address of the next instruction to be
 :*               executed is returned. (see REMARKS)                 /)
 :*                                                             END-Post
 :*   RETURNS  "BoxPtr" (/ address of the next instruction /)
 :*
 :*   REMARKS    (/   refer to <DOCUMENTS> for a detailed description of
 :*                   the internal representation of boxes and their
 :*                   semantics                                     /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY emulate.emulate
 :******************************************************************
*/

BoxPtr emulate(p)
register BoxPtr p;              /* address of the box to be executed       */
{
    unsigned short *lgth;       /* pointer to string length                */
    unsigned short lg;          /* length of an active rule descriptor     */
    unsigned short lenstr();    /* return the length of a string           */
    int *ip;                    /* temporary                               */
    static int size0;           /* initial size of active rule descriptor  */
    static int size;            /* final size of active rule descriptor    */
    static holePtr Eff_ptr;     /* address of the free area in descriptor  */
    static holePtr Eff_aux;     /* address of a triggered rule descriptor  */
    static holePtr str_ptr;     /* where the next string will be saved     */
    char *cp, **cpp1, *cpp2;    /* temporary                               */
    char *getExtStrRepr();      /* convert to null ended string  forma     */
    BoxPtr *boxpp;              /* pointer to a rule internal code         */
    trigD *Eff;                 /* pointer to a triggering descriptor      */

   switch(p->op) {

	case add1           :
			      *p->arg.arithmbox.deltaIn =
			      *p->arg.arithmbox.gamma1 +
			      *p->arg.arithmbox.gamma2;
			      return p->alpha;

	case mult1          : *p->arg.arithmbox.deltaIn =
			      *p->arg.arithmbox.gamma1 *
			      *p->arg.arithmbox.gamma2;
			      return p->alpha;

	case div1           : *p->arg.arithmbox.deltaIn =
			      *p->arg.arithmbox.gamma1 /
			      *p->arg.arithmbox.gamma2;
			      return p->alpha;

	case mod1            : *p->arg.arithmbox.deltaIn =
			       *p->arg.arithmbox.gamma1 %
			       *p->arg.arithmbox.gamma2;
			       return p->alpha;

	case minus1          : *p->arg.arithmbox.deltaIn =
			       *p->arg.arithmbox.gamma1 -
			       *p->arg.arithmbox.gamma2;
			       return p->alpha;

	case un_minus1       :
			       *p->arg.un_minusbox.deltaIn = -
			       *p->arg.un_minusbox.gamma;
			       return p->alpha;

	case assign1         :
			       *p->arg.assign_intbox.gamma1 =
			       *p->arg.assign_intbox.gamma2;
			       return p->alpha;

	case assign_str1     : *p->arg.assign_strbox.gamma1 =
			       *(p->arg.assign_strbox.gamma2);
			       return p->alpha;

	case neq1            : if (*p->arg.relationalbox.gamma1 !=
				   *p->arg.relationalbox.gamma2)
				    return p->arg.relationalbox.alphaTrue;
			       else
				    return p->alpha;

	case less1           : if (*p->arg.relationalbox.gamma1 <
				   *p->arg.relationalbox.gamma2)
				    return p->arg.relationalbox.alphaTrue;
			       else
				    return p->alpha;

	case leq1            : if (*p->arg.relationalbox.gamma1 <=
				  *p->arg.relationalbox.gamma2)
				    return p->arg.relationalbox.alphaTrue;
			       else
				    return p->alpha;

	case equal1          : if (*p->arg.relationalbox.gamma1 ==
				   *p->arg.relationalbox.gamma2)
				   return p->arg.relationalbox.alphaTrue;
			       else
				   return p->alpha;

	case greater1        : if (*p->arg.relationalbox.gamma1 >
				   *p->arg.relationalbox.gamma2)
				   return p->arg.relationalbox.alphaTrue;
			       else
				   return p->alpha;

	case geq1           : if (*p->arg.relationalbox.gamma1 >=
				  *p->arg.relationalbox.gamma2)
				    return p->arg.relationalbox.alphaTrue;
			      else
				    return p->alpha;

	case present1       :
			      cp = *p->arg.presentbox.str_addr;
			      if (cp != (char *) NOTFOUND)
				return p->arg.presentbox.alphaTrue;
			      else
				return p->alpha;

	case move_int1      : *p->arg.movebox.dest =
			      *p->arg.movebox.source;
			      return p->alpha;

	case mv_int1        : ip = (int *) p->arg.mv_intbox.deltaIn;
			      *ip = p->arg.mv_intbox.val;
			      return p->alpha;

	case move_str1      :
			      *p->arg.movebox.dest =
			      *p->arg.movebox.source;
			      return p->alpha;

	case assign_addr1   :
			      cpp1 = (char **) p->arg.assign_addrbox.gamma1;
			      *cpp1 = p->arg.assign_addrbox.gamma2;
			      return p->alpha;

	case cpy_intAddr1   :
			      ip = (int *) p->arg.gamma;
			      *Eff_ptr = (hole) *ip;
			      cp = (char *) Eff_ptr;
			      cp += sizeof(int);
			      Eff_ptr = (hole *) cp;
			      return p->alpha;

	case cpy_str1       :
			      *Eff_ptr = (hole) str_ptr;
			      cp = (char *) Eff_ptr;
			      cp += sizeof(hole *);
			      Eff_ptr = (hole *) cp;
			      cpy_str(str_ptr, *p->arg.gamma);
			      cp = (char *) str_ptr;
			      lg = lenstr(*p->arg.gamma);
/*
 * 	Thu Aug 10 11:35:52 MET DST 1995 (Aziz Mounji)
 *
 * 	for alignment restriction, the str_ptr must be moved to the
 * 	next multiple of unsigned short size (sizeof(unsigned short))
 */
			      cp += sizeof(unsigned short) + lg + lg*(sizeof(unsigned short) -1)%sizeof(unsigned short);
			      str_ptr = (hole *) cp;
			      return p->alpha;

	case cpy_strAddr1   : *Eff_ptr = (hole) *p->arg.gamma;
			      cp = (char *) Eff_ptr;
			      cp += sizeof(hole *);
			      Eff_ptr = (hole *) cp;
			      return p->alpha;

	case setLen1        : 
				size = size0 = p->arg.size;
			      return p->alpha;

	case addLen1        :
			      lg = lenstr(*p->arg.gamma);
			      size += usz + lg + lg*(sizeof(unsigned short) -1)%sizeof(unsigned short);
			      return p->alpha;


	case for_current1  :
	case for_next1     :
	case at_completion1:
				if (size%isize != 0)
				size += (isize - size%isize);
				Eff = (trigD *) malloc(sizeof(trigD));
				if (Eff == NULL) { /* allocation problem */
					error(fatal, 25);
				}
				Eff->size = size0;
				Eff->code = p->arg.triggerbox.ruleCode;
				Eff->next = NULL;
#if 0
				Eff->param = (hole *) malloc(size);
				if (Eff->param == NULL) { /* allocation problem */
					error(fatal, 25);
				}
#endif
#if 1	// Former code (above) caused memory leak when rule with no argument was triggered, Fred
				if (size > 0) {
					Eff->param = (hole *) malloc(size);
					if (Eff->param == NULL) { /* allocation problem */
						error(fatal, 25);
					}
				}
				else {
					Eff->param = (hole *) NULL;
				}
#endif
			     Eff_ptr = Eff->param;
			     cp = (char *) Eff_ptr;
			     cp += size0;
			     str_ptr = (hole *) cp;
			     push(Eff, p->op);
			     return (p->alpha);

	case call_p1       :
			     (*p->arg.callbox.fpptr)
			     (p->arg.callbox.deltaIn);
			     return p->alpha;

	case call_f1       :
			     ip = (int *) p->arg.callbox.deltaIn;
			     *ip = (int) (*p->arg.callbox.fpptr)
			     (p->arg.callbox.deltaIn + sizeof(int));
			     return p->alpha;

	case StrLess1      :
			     if (cmp_str(*p->arg.StrRelatbox.gamma1,
					 *p->arg.StrRelatbox.gamma2) < 0)
				return p->arg.StrRelatbox.alphaTrue;
			     else
				return p->alpha;

	case StrLeq1       :
			     if (cmp_str(*p->arg.StrRelatbox.gamma1,
					 *p->arg.StrRelatbox.gamma2) <= 0)
				return p->arg.StrRelatbox.alphaTrue;
			     else
				return p->alpha;

	case StrEqual1     :
			     if (cmp_str(*p->arg.StrRelatbox.gamma1,
					 *p->arg.StrRelatbox.gamma2) == 0)
				return p->arg.StrRelatbox.alphaTrue;
			     else
				return p->alpha;

	case StrGreater1        :
			     if (cmp_str(*p->arg.StrRelatbox.gamma1,
					 *p->arg.StrRelatbox.gamma2) > 0)
				return p->arg.StrRelatbox.alphaTrue;
			     else
				return p->alpha;

	case StrGeq1           :
			     if (cmp_str(*p->arg.StrRelatbox.gamma1,
					 *p->arg.StrRelatbox.gamma2) >= 0)
				return p->arg.StrRelatbox.alphaTrue;
			     else
				return p->alpha;

	case StrNeq1           :
			     if (cmp_str(*p->arg.StrRelatbox.gamma1,
					 *p->arg.StrRelatbox.gamma2) != 0)
				return p->arg.StrRelatbox.alphaTrue;
			     else
				return p->alpha;

	case StrEq1           :
			     if (eq_str(*p->arg.StrRelatbox.gamma1,
					*p->arg.StrRelatbox.gamma2))
				return p->arg.StrRelatbox.alphaTrue;
			     else
				return p->alpha;
	case compile1         :

#ifdef STDREGEX
		
		/*
		 *  allocate area where to store the compiled RE 
		 */
			     if ((cp = tmalloc(BUFSIZE, char)) == NULL) 
				error(fatal, 25);
		/* 
		 *  convert the RE string from asax repr. to C representaion
		 */
			     cpp2 = getExtStrRepr(*p->arg.gamma);

				/* 	cpp2 is the the regular expression. 	
				*	Compile it depending on the regular 
				*	expressions in use (STD, GNU, POSIX or BSD)
				*/

			     compile(cpp2, cp + usz, &cp[ESIZE], '\0');
		/* 
		 *  store its length (ASAX format of strings)
		 */
			     lgth = (unsigned short *) cp;
			     *lgth = strlen(cp+usz);
		/*
		 *  put the now compiled RE at its original place (in the WA)
		 *  so match will find it 
		 */
			     *p->arg.gamma = cp;
			     return p->alpha;
#endif

#ifdef GNUREGEX
			     { 
				static char		 staticBuff[LGSZ+PATBUFSZ]; 			
				struct re_pattern_buffer *pbuff;
				const char 		 *re_compile_pattern(); 
				char  			 *errMsg;
				extern reg_syntax_t      re_syntax_options;

		
		/*
		 *  allocate area where to store the compiled RE 
		 */
			        if ((cp = tmalloc(BUFSIZE, char)) == NULL) 
					error(fatal, 25);
		/* 
		 *  convert the RE string from asax repr. to C representaion
		 */
			     	cpp2 = getExtStrRepr(*p->arg.gamma);

				/* 	cpp2 is the the regular expression 	
				*	compile it depending on the regular 
				*	expressions in use (STD, GNU, POSIX or BSD) 
				*/

		/* initialize the pattern buffer */

				bzero(staticBuff, sizeof(staticBuff));
				pbuff = (struct re_pattern_buffer *) (staticBuff + LGSZ);

				pbuff->allocated = BUFSIZE;
				pbuff->buffer    = cp;
				pbuff->fastmap   = 0;
				pbuff->translate = 0;



				
#if defined(REGEX_USED_SYNTAX)
				re_syntax_options = REGEX_USED_SYNTAX;
#else
				re_syntax_options = RE_SYNTAX_POSIX_EXTENDED;	/* default */
#endif
				errMsg = re_compile_pattern(cpp2, strlen(cpp2), pbuff);
				if (errMsg) { 
					fprintf(stderr, "%s: %s\n", cpp2, errMsg);
					exit(1);
				}

			        lgth = (unsigned short *) staticBuff;
			        *lgth = PATBUFSZ + LGSZ - usz;
			     	*p->arg.gamma = staticBuff;
			     	return p->alpha;
			     }
#endif 
			
#ifdef POSIXREGEX
			     { 
				static char	staticBuff[LGSZ+PATBUFSZ]; 			
				regex_t		*pbuff;
				char  		*str;
				char 		errMsg[128];
				int 		cflags = REG_EXTENDED;
				int 		errcode;
				
		/*
		 *  allocate area where to store the compiled RE 
		 */
			        if ((cp = tmalloc(BUFSIZE, char)) == NULL) 
					error(fatal, 25);

		/* 
		 *  convert the RE string from asax repr. to C representaion
		 */
			     	cpp2 = getExtStrRepr(*p->arg.gamma);

				/* 	cpp2 is the the regular expression 	
				*	compile it depending on the regular 
				*	expressions in use 
				*/

		/* initialize the pattern buffer */

				bzero(staticBuff, sizeof(staticBuff));
				pbuff = (regex_t *) (staticBuff + LGSZ);

				pbuff->allocated = BUFSIZE;
				pbuff->buffer    = cp;
				pbuff->fastmap   = 0;
				pbuff->translate = 0;
				
				errcode = regcomp(pbuff, cpp2, cflags);
				if (errcode) { 
					regerror(errcode, pbuff, errMsg, 128);
					fprintf(stderr, "%s: %s\n", cpp2, errMsg);
					exit(1);
				}
			        lgth = (unsigned short *) staticBuff;
			        *lgth = PATBUFSZ + LGSZ - usz;
			     	*p->arg.gamma = staticBuff;
			     	return p->alpha;
			     }
#endif 

#ifdef BSDREGEX
			     { 
				char  		    *getExtStrRepr();
				char 		    *re_comp();
				char 		    *errMsg;
				char 		    *str;
				extern reg_syntax_t re_syntax_options;
				
		/* 
		 *  convert the RE string from asax repr. to C representaion
		 */
			     	cpp2 = getExtStrRepr(*p->arg.gamma);

				/* 	cpp2 is the the regular expression 	
				*	compile it depending on the regular 
				*	expressions in use 
				*/
#if defined(REGEX_USED_SYNTAX)
				re_syntax_options = REGEX_USED_SYNTAX;
#else
				re_syntax_options = RE_SYNTAX_POSIX_EXTENDED;
#endif
				errMsg = re_comp(cpp2);
				if (errMsg) { 
					fprintf(stderr, "%s: %s\n", cpp2, errMsg);
					exit(1);
				}
			        if ((str = (char *) tmalloc(1, unsigned short)) == NULL) 
					error(fatal, 25);
			        lgth = (unsigned short *) str;
			        *lgth = 0;
			     	*p->arg.gamma = str;
			     	return p->alpha;
			     }
#endif 	/* BSDREGEX */
	default               :
			     error1(fatal, 24, "emulate() of emulate.c");
			     return(NULL);
    }
}
