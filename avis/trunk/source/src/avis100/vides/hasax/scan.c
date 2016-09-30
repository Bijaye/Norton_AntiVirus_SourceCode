#pragma warning(disable:4035) // 'no return value'


/*
 :******************************************************************
 :* MODULE-DESCRIPTION
 :*
 :* TITLE         (/ Lexical Analyser /)
 :*
 :* NAME          scan.c
 :*
 :* COPYRIGHT     (C)   SIEMENS NIXDORF INFORMATIONSSYSTEME AG  1992
 :*                     ALL RIGHTS RESERVED
 :*
 :* MEMORY-CLASS  6
 :*
 :* PROG-CONVENTS (/ <internal conventions (instance, names,
 :*                  external data, ...). None -> empty text>     /)
 :*
 :* PROCEDURE     getsymb
 :* REMARKS       (/ reading of a token in the input stream              /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     isRcomment
 :* REMARKS       (/ check if this is the end of a comment              /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     getComment
 :* REMARKS       (/ skip a comment and return the next token           /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     issymbol
 :* REMARKS       (/ check if this is a specified symbol                /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     get_spec
 :* REMARKS       (/ reading of a special symbol in the input stream     /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     get_leq
 :* REMARKS       (/ reading of a relational symbol in the input stream  /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     get_identifier
 :* REMARKS       (/ reading of an identifier                           /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     takeofchar
 :* REMARKS       (/ skip alphanumeric characters                       /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     internRepr
 :* REMARKS       (/ build the internal repres. of an identifier        /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     get_Xliteral
 :* REMARKS       (/ reading of an X-literal in the input stream  /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     hexToDec
 :* REMARKS       (/ hexa to decimal conversion                  /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     isspecsymb
 :* REMARKS       (/ check if this is a special symbol                  /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     get_integer
 :* REMARKS       (/ reading of an integer in the input stream /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     get_Cliteral
 :* REMARKS       (/ reading of a C-literal in the input stream /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     skipspaces
 :* REMARKS       (/ skip spaces in the input stream            /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     get_char
 :* REMARKS       (/ reading of a legal character in the input steam /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     get_char1
 :* REMARKS       (/ reading of a character in the input stream /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     isBadChar
 :* REMARKS       (/ check if this is a legal character /)
 :* ENTRY         EXTERNAL
 :*
 :* PROCEDURE     symbolClass
 :* REMARKS       (/ return the class of a symbol/)
 :* ENTRY         EXTERNAL
 :*
 :*-----------------------------------------------------------------
 :*
 :* VERSION       001
 :*   CRDATE      1992-11-03
 :*   AUTHOR      (/ Mounji Abdelaziz    , FUNDP /)
 :*   UPDATE      (/ original issue /)
 :*
 :* END-MODULE-DESCRIPTION scan.c
 :******************************************************************
*/
#include <stdio.h>
#include <ctype.h>
#include "asax.h"

#define specsymbnbre  29 /* nbre of special symbols */

static char *specsymbtab[] = {"and", "at_completion", "begin", "div", "do",
		       "end", "external", "false", "fi", "for_current",
	    "for_next", "global", "if", "init_action", "integer", "internal",
		       "mod", "not", "od", "off", "or", "present", "rule",
		       "skip", "string", "trigger", "true", "uses", "var",
		       "!=", "(", ")", "*", "+", ",", "-", "-->", ":", ":=",
		       ";", "<", "<=", "=", ">", ">=", ".", "%="};

#define tabchar '\t'
#define eoline  '\n'
#define space  ' '
#define quote  '\''
int nextchar;           /* last read character  */
short int more_char;      /* = EOF */
static char resymb[MAXS];

// 07/11/97 Added handling of hexadecimal constants in Russel (in get_integer), Fred
#define ISWITHINabcdef(c) ((c) >= 'a' && (c) <= 'f')
#define ISWITHINABCDEF(c) ((c) >= 'A' && (c) <= 'F')
#define ISHEXDIGIT(c) (isdigit(c) || ISWITHINabcdef(c) || ISWITHINABCDEF(c))

#define abcdefVALUE(c) ((c)-'a'+10)
#define ABCDEFVALUE(c) ((c)-'A'+10)
#define LETTERVALUE(c) (ISWITHINabcdef(c)?abcdefVALUE(c):ABCDEFVALUE(c))
#define HEXDIGITVALUE(c) (isdigit(c)?((c)-'0'):LETTERVALUE(c))

/*
 :******************************************************************
 :* UNIT-BODY scan.getsymb
 :*
 :* TITLE         (/ reading of a token in the input stream      /)
 :*
 :* PURPOSE       reading of the next token in the input stream and
 :*               storing its internal representation.
 :*                                                      END-PURPOSE
 :* VERSION       001
 :*   CRDATE      1992-11-03
 :*   AUTHOR      (/ Mounji Abdelaziz       , FUNDP              /)
 :*   UPDATE      (/ original issue /)
 :*
 :* REALIZATION   (/ <note, reference to other proc. (optional)> /)
 :*
 :*   CONTROL    (/
 :*                                                               /)
 :*   DATA
 :*       Global Data:  (/
 :*                       - specsymbtab: Table of special symbol of the
 :*                                      rule based language.
 :*                       - symb       : holds the internal representation
 :*                                      of the read symbol.
 :*                       - nextchar   : the last character being read.
 :*                       - more_char  : is TRUE when end of file is.
 :*                       - LineNbre   : the line number.
 :*                                                                  /)
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
 :*   Pre (/ - fp      a valid file pointer
 :*          - Let s be the current input stream.
 :*                s = s1 t
 :*            where
 :*                  - s1 is a possibly empty sequence of spaces (blanks, eol
 :*                    and tabs);
 :*                  - there exist a greatest prefixe t1 of t such that:
 :*                      t = t1 u
 :*                    where t1 is a symbol and u is any character sequenece
 :*       /)
 :*                                                             END-Pre
 :*   Post   (/ - the input stream is u;
 :*             - nextchar contains the first character in u;
 :*             - symb contains the internal representation of t1.  /)
 :*             - LineNbre contains the current line number
 :*             - more_char is TRUE if end of file is reached.
 :*                                                             END-Post
 :*   RETURNS  NONE
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY scan.getsymb
 :******************************************************************
*/
getsymb(fp)
FILE *fp;                  /* file pointer */
{
	int index;         /* index in the special symbol table specymbtab */

	skipspaces(fp);                               /* skip white spaces */
	if (more_char == 0)                     /* this is the end of file */
	   symb.ibsymb = nosymbx;
				      /* switch to the right routine given */
				      /* the first character of the symbol */
	else if (isalpha(nextchar))
		get_identifier(fp);             /* expecting an identifier */
	else if (isdigit(nextchar))
		get_integer(fp);                   /* expecting an integer */
	else if (nextchar == quote)
		get_Cliteral(fp);                 /* expecting a C-literal */
	else if ((index = issymbol(nextchar)) >= 0)
		get_spec(fp, index);          /* expecting a reserved word */
	else if (nextchar == '>' || nextchar == '<' ||
		 nextchar == '%' || nextchar == '!')
		get_leq(fp);              /* expecting a relational symbol */
	else if (nextchar == '/')                   /* expecting a comment */
		getComment(fp);
	else {
		symb.ibsymb = errsymbx;                   /* lexical error */
		get_char(fp);
	}
}
/*
 :******************************************************************
 :* UNIT-BODY scan.isRcomment
 :*
 :* TITLE         (/ checking the end of comment /)
 :*
 :* PURPOSE       check if the input stream starts with the sequence
 :*               of two characters "star slash" i.e. the end of a comment.
 :*                                                      END-PURPOSE
 :* VERSION       001
 :*   CRDATE      1992-11-03
 :*   AUTHOR      (/ Mounji Abdelaziz       , FUNDP              /)
 :*   UPDATE      (/ original issue /)
 :*
 :* REALIZATION   (/ <note, reference to other proc. (optional)> /)
 :*
 :*   CONTROL    (/
 :*                                                               /)
 :*   DATA
 :*       Global Data:  (/
 :*                        nextchar   : the last character being read.
 :*                                                                  /)
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
 :*   Pre    (/ - fp      a valid file pointer          /)
 :*                                                             END-Pre
 :*   Post   (/ - if the input stream is of the form:
 :*                    <star slash><rest>
 :*                 where
 :*                 - star is '*' and slash is '/'
 :*                 - <rest> is the rest of the input stream.
 :*                the effect is
 :*                 - nextchar contains the first character in <rest>;
 :*                 - LineNbre contains the current line number;
 :*                 - the input stream is <rest>
 :*                 - the returned value = 1
 :*                otherwise the returned value = 0 and the input stream is
 :*                 unchanged.
 :*                                                                   /)
 :*                                                             END-Post
 :*   RETURNS  "int" (/ = 1 if "star slash" is a prefixe of the input stream
 :*                     = 0 otherwise                              /)
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY scan.isRcomment
 :******************************************************************
*/
isRcomment(fp)  /* returns 1 if star slash is a prefixe of
		   the remaining char. seq. return 0 otherwise */
FILE *fp;       /* file pointer */
{
    if (nextchar == '*') {
	get_char(fp);
	if (nextchar == '/') {
	    get_char(fp);
	    return(1);         /* success */
	} else {
	    return (0);        /* not a comment end */
	}
    } else {
	return(0);             /* not a comment end */
    }
}
/*
 :******************************************************************
 :* UNIT-BODY scan.getComment
 :*
 :* TITLE         (/ reading of a comment in the input stream      /)
 :*
 :* PURPOSE       skip the entire current comment and then read the
 :*               the symbol after this comment.
 :*                                                      END-PURPOSE
 :* VERSION       001
 :*   CRDATE      1992-11-03
 :*   AUTHOR      (/ Mounji Abdelaziz       , FUNDP              /)
 :*   UPDATE      (/ original issue /)
 :*
 :* REALIZATION   (/ <note, reference to other proc. (optional)> /)
 :*
 :*   CONTROL    (/
 :*                                                               /)
 :*   DATA
 :*       Global Data:  (/
 :*                       - symb       : holds the internal representation
 :*                                      of the read symbol.
 :*                       - nextchar   : the last character being read.
 :*                       - more_char  : is TRUE when end of file is.
 :*                       - LineNbre   : the line number.
 :*                                                                  /)
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
 :*   Pre (/ - fp      a valid file pointer
 :*          - nextchar = '/'
 :*       /)
 :*                                                             END-Pre
 :*   Post   (/ - if the input stream is of the form:
 :*                    <star><comment_body><star slash><whites><token><rest>
 :*                 where
 :*                 - star is '*' and slash is '/'
 :*                 - comment_body is the character sequence of the comment;
 :*                 - whites is any sequence of spaces (blanks, eol and tabs)
 :*                 - token is a symbol;
 :*                 - rest is the rest of the input stream.
 :*                the effect is
 :*                 - nextchar contains the first character in <rest>;
 :*                 - symb contains the internal representation of <token>;
 :*                 - LineNbre contains the current line number;
 :*                 - more_char is TRUE if end of file is reached;
 :*                 - the input stream is <rest>
 :*                otherwise 'symb' contains the internal repres.
 :*                of the error symbol.
 :*                                                                   /)
 :*                                                             END-Post
 :*   RETURNS  "int" (/ if this is a legal comment = 1 else = 0 /)
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY scan.getComment
 :******************************************************************
*/
getComment(fp)  /* skip a comment and get the next token */
FILE *fp;       /* file pointer */
{
    get_char(fp);                              /* skip the slash character */
    if (nextchar == '*') {
	get_char1(fp);
	while (more_char && !isRcomment(fp)) {    /* skip the comment body */
	    get_char1(fp);
	}
	if (more_char) {
	    getsymb(fp);
	} else {
	    symb.ibsymb = errsymbx;                    /* unclosed comment */
	}
    } else {
	symb.ibsymb = errsymbx;                     /* bad character found */
    }
}
/*
 :******************************************************************
 :* UNIT-BODY scan.issymbol
 :*
 :* TITLE         (/ checking if the current character is a member
 :*                  of a given list.   /)
 :*
 :* PURPOSE       checking if the current character is a member of a
 :*               subset of the special symbol table. If this is the
 :*               case return its index; otherwise return -1.
 :*                                                      END-PURPOSE
 :* VERSION       001
 :*   CRDATE      1992-11-03
 :*   AUTHOR      (/ Mounji Abdelaziz       , FUNDP              /)
 :*   UPDATE      (/ original issue /)
 :*
 :* REALIZATION   (/ <note, reference to other proc. (optional)> /)
 :*
 :*   CONTROL    (/
 :*                                                               /)
 :*   DATA
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
 :*   Pre    (/ - c    a given character  /)
 :*                                                             END-Pre
 :*   Post   (/ - if c in {'+', '-', '*', '(', ')', '=', ':', ';', ',', '.'}
 :*                 return index
 :*                   where specsymbtab[index] = c;
 :*               otherwise returned -1.
 :*                                                                   /)
 :*                                                             END-Post
 :*   RETURNS  "int" (/ = index of the character in 'specsymbtab' if a member
 :*                     = -1 otherwise                              /)
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY scan.issymbol
 :******************************************************************
*/
issymbol(c)
char c;
{
	switch (c) {
		case '+': return(add);
		case '-': return(minus);
		case '*': return(mult);
		case '(': return(lparenthesis);
		case ')': return(rparenthesis);
		case '=': return(equal);
		case ':': return(colon);
		case ';': return(semicolon);
		case ',': return(comma);
		case '.': return(period);
		default : return (-1);
	}
}
/*
 :******************************************************************
 :* UNIT-BODY scan.get_spec
 :*
 :* TITLE         (/ reading of a special symbol in the input stream /)
 :*
 :* PURPOSE       (/ having read the first character of a special symbol,
 :*                  read further characters to identify this symbol and
 :*                  create its internal representation              /)
 :*                                                      END-PURPOSE
 :* VERSION       001
 :*   CRDATE      1992-11-03
 :*   AUTHOR      (/ Mounji Abdelaziz       , FUNDP              /)
 :*   UPDATE      (/ original issue /)
 :*
 :* REALIZATION   (/ <note, reference to other proc. (optional)> /)
 :*
 :*   CONTROL    (/
 :*                                                               /)
 :*   DATA       (/
 :*                       - specsymbtab: Table of special symbol of the
 :*                                      rule based language.
 :*                       - symb       : holds the internal representation
 :*                                      of the read symbol.
 :*                       - nextchar   : the last character being read.
 :*                       - more_char  : is TRUE when end of file is.
 :*                       - LineNbre   : the line number.
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
 :*             - index an "int" value
 :*             - nextchar is a member of
 :*                     {'+', '-', '*', '(', ')', '=', ':', ';', ',', '.'}
 :*             - the input stream is of the form:
 :*                      <token><rest>
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/ - symb contains the internal repres. of <token>
 :*             - the input stream is <rest>                          /)
 :*                                                             END-Post
 :*
 :*
 :*   RETURNS  NONE
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY scan.get_spec
 :******************************************************************
*/
get_spec(fp, index)
FILE *fp;
int index;
{
	symb.bsymbval.svalue[0] = nextchar;
	if (nextchar == ':') { /* expecting the assignment symbol ':=' */
		get_char(fp);
		if (nextchar == '=') {
			symb.ibsymb = assign;
			symb.bsymbval.svalue[1] = nextchar;
			symb.bsymbval.svalue[2] = '\0';
			get_char(fp);
		} else {
			symb.ibsymb = (enum index) index;
			symb.bsymbval.svalue[1] = '\0';
		  }

	} else if (nextchar != '-') {
		symb.ibsymb = (enum index) index;
		symb.bsymbval.svalue[1] = '\0';
		get_char(fp);
	} else { /* expecting right arrow '-->' or a signed integer '-i' */
		get_char(fp);
		if (nextchar == '-') {
		    symb.bsymbval.svalue[1] = nextchar;
		    get_char(fp);
		    if (nextchar == '>') {
			   symb.ibsymb = larrow;
			   symb.bsymbval.svalue[2] = nextchar;
			   symb.bsymbval.svalue[3] = '\0';
			   get_char(fp);
		    } else { /* error detected '--' is an illegal token */
			   symb.ibsymb = errsymbx;
			   get_char(fp);
		      }
		} else { /* '-' is the read token */
			   symb.ibsymb = minus;
			   symb.bsymbval.svalue[1] = '\0';
		    }
	  }
}
/*
 :******************************************************************
 :* UNIT-BODY scan.get_leq
 :*
 :* TITLE         (/ reading of a relational symbol in the input stream /)
 :*
 :* PURPOSE       (/ having read the first character of a relational symbol,
 :*                  read further characters to identify this symbol and
 :*                  create its internal representation              /)
 :*                                                      END-PURPOSE
 :* VERSION       001
 :*   CRDATE      1992-11-03
 :*   AUTHOR      (/ Mounji Abdelaziz       , FUNDP              /)
 :*   UPDATE      (/ original issue /)
 :*
 :* REALIZATION   (/ <note, reference to other proc. (optional)> /)
 :*
 :*   CONTROL    (/
 :*                                                               /)
 :*   DATA       (/
 :*                       - specsymbtab: Table of special symbol of the
 :*                                      rule based language.
 :*                       - symb       : holds the internal representation
 :*                                      of the read symbol.
 :*                       - nextchar   : the last character being read.
 :*                       - more_char  : is TRUE when end of file is.
 :*                       - LineNbre   : the line number.
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
 :*             - nextchar is a member of
 :*                     {'>', '<', '%', '!'}
 :*             - the input stream is of the form:
 :*                      <token><rest>
 :*                                                                      /)
 :*                                                             END-Pre
 :*   Post   (/ - symb contains the internal repres. of <token>
 :*             - the input stream is <rest>                          /)
 :*
 :*                                                             END-Post
 :*
 :*   RETURNS  NONE
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY scan.get_leq
 :******************************************************************
*/
get_leq(fp)
FILE *fp;
{
	char c;

	c = symb.bsymbval.svalue[0] = nextchar;
	get_char(fp);
	if (nextchar == '=') {
		symb.bsymbval.svalue[1] = nextchar;
		symb.bsymbval.svalue[2] = '\0';
		switch (c) {
			case '<':   symb.ibsymb = leq; break;
			case '>':   symb.ibsymb = geq; break;
			case '!':   symb.ibsymb = neq; break;
			case '%':   symb.ibsymb = eqx; break;
		}
		get_char(fp);
	} else {
		symb.bsymbval.svalue[1] = '\0';
		switch (c) {
			case '<':   symb.ibsymb = less;    break;
			case '>':   symb.ibsymb = greater;  break;
			case '!':   symb.ibsymb = errsymbx; break;
		}
	  }
}
/*
 :******************************************************************
 :* UNIT-BODY scan.get_identifier
 :*
 :* TITLE         (/ reading of an identifier    /)
 :*
 :* PURPOSE       (/ read of the identifier that begins the input
 :*                  stream and store its internal repres.
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
 :*               - 0 <= i < ident_lg
 :*               - <stream> is the current input stream
 :*               - for j=0 .. i
 :*                               cj is a letter, a digit or '_'
 :*                               resymb[j] = cj
 :*                                                                 /)
 :*
 :*   DATA       (/
 :*                       - specsymbtab: Table of special symbol of the
 :*                                      rule based language.
 :*                       - symb       : holds the internal representation
 :*                                      of the read symbol.
 :*                       - resymb     : last read symbol.
 :*                       - nextchar   : the last character being read.
 :*                       - more_char  : is TRUE when end of file is.
 :*                       - LineNbre   : the line number.
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
 :*   Post   (/ - symb contains the internal repres. of <identifier1>
 :*               where
 :*                     <identifier1> is <identifier> truncated to its
 :*                     'ident_lg' first characters;
 :*             - the input stream is <rest>                          /)
 :*
 :*                                                             END-Post
 :*
 :*   RETURNS  NONE
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY scan.get_identifier
 :******************************************************************
*/
get_identifier(fp)
FILE *fp;
{
	int i;

	i = 0;
	while ((isalpha(nextchar) || isdigit(nextchar) || nextchar == '_') &&
		  i < ident_lg - 1   ) {
		resymb[i] = nextchar;
		i++;
		get_char(fp);
	}
	resymb[i] = '\0';
	takeofchar(fp);
	internRepr(fp);
}
/*
 :******************************************************************
 :* UNIT-BODY scan.takeofchar
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
 :*             - nextchar is alphanumeric or an '_'
 :*             - the input stream is of the form:
 :*                      <alphanum><rest>
 :*               where <alphanum> is a possibly empty sequence of                                                       /)
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
 :* END-BODY scan.takeofchar
 :******************************************************************
*/
takeofchar(fp)		 /* skip the rest of the identifier */
FILE *fp;		/* file pointer */
{
	while ( isalpha(nextchar) || isdigit(nextchar) || nextchar == '_')
		get_char(fp);
}
/*
 :******************************************************************
 :* UNIT-BODY scan.internRepr
 :*
 :* TITLE         (/ build the internal repres. of an identifier      /)
 :*
 :* PURPOSE       (/ given the external represenation of an identifier,
 :*                  generate the internal repres.
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
 :*             - resymb is a string representing the identifier.
 :*                                                            /)
 :*                                                             END-Pre
 :*
 :*   Post   (/ - - symb contains the internal representation of resymb. /)
 :*                                                             END-Post
 :*
 :*   RETURNS  NONE
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY scan.internRepr
 :******************************************************************
*/
internRepr(fp)
FILE *fp;
{
	char *str;
	int index;

	if (strcmp(resymb, (str = "true")) == 0)    {
		   symb.ibsymb = truex;
		   symb.bsymbval.ivalue = 1;
	} else if (strcmp(resymb, (str = "false")) == 0) {
		   symb.ibsymb = falsex;
		   symb.bsymbval.ivalue = 0;
	} else if ( (*resymb == 'X') && (nextchar == quote) ) {
		   get_Xliteral(fp);
	}  else if ((index = isspecsymb(resymb)) >= 0) {
		   symb.ibsymb = (enum index) index;
		   strcpy(symb.bsymbval.svalue, resymb);
	} else /* identifier */ {
		   symb.ibsymb = identifierx;
		   strcpy(symb.bsymbval.svalue, resymb);
	}
}
/*
 :******************************************************************
 :* UNIT-BODY scan.get_Xliteral
 :*
 :* TITLE         (/ reading of an X-literal in the input stream     /)
 :*
 :* PURPOSE       (/ reading of an X-literal in the input stream and
 :*                  return its internal repres.
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
 :*                     - morechar : = EOF
 :*                     - nextchar : last read character
 :*                     - symb     : internal repres. of the last read
 :*                                  symbol                           /)
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
 :*                     quote <restStream>
 :*               where quote is "'" and <restStream> is any sequence /)                                             /)
 :*
 :*                                                             END-Pre
 :*
 :*   Post   (/ if <restStream> matches the form:
 :*                        <X-characters>quote<rest>
 :*                  where <X-characters> is a possibly empty sequence of
 :*                        characters in {0, ..., 9, a, ..., f, A, ...,F}
 :*               - the current input stream is <rest>
 :*               - the internal representation of the X-literal
 :*                 corresponding to <X-characters> is stored in 'symb'
 :*             otherwise the internal repres. of the error symbol is
 :*                 stored in 'symb'                            /)
 :*                                                             END-Post
 :*
 :*   RETURNS  NONE
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY scan.get_Xliteral
 :******************************************************************
*/
get_Xliteral(fp)
FILE *fp;
{
	int c1, c2, i;

	get_char(fp);
	i = 0;
	while  ( more_char == 1 && (c1 = hexToDec(nextchar)) >= 0) {
	    get_char(fp);
	    if ( !more_char || (c2 = hexToDec(nextchar)) < 0) {
		symb.ibsymb = errsymbx;
		break;
	    } else {
		symb.bsymbval.svalue[i] = c1 * 16 + c2;
	    }
	    i++;
	    get_char(fp);
	}
	if (nextchar == quote && symb.ibsymb != errsymbx) {
	    symb.bsymbval.svalue[i] = '\0';
	    symb.X_lg = i;             /* length of the X literal */
	    symb.ibsymb = X_literalx;
	    get_char(fp);
	} else
	    symb.ibsymb = errsymbx;
}
/*
 :******************************************************************
 :* UNIT-BODY scan.hexToDec
 :*
 :* TITLE         (/ hexa to decimal conversion.   /)
 :*
 :* PURPOSE       (/ return the decimal number if an hexadecimal digit 
 :* 		     return -1 otehrwise.			/)
 :*                                                      END-PURPOSE
 :* VERSION       001
 :*   CRDATE      1992-11-03
 :*   AUTHOR      (/ Mounji Abdelaziz       , FUNDP              /)
 :*   UPDATE      (/ original issue /)
 :*
 :* REALIZATION   (/ <note, reference to other proc. (optional)> /)
 :*
 :*   CONTROL    (/
 :*                                                               /)
 :*   DATA
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
 :*   Pre    (/ - c    a given character  /)
 :*                                                             END-Pre
 :*   Post   (/ - if c in {'0', ..., '9', 'a', ..., 'f', 'A', ..., 'F' }
 :*                 return the decimal number corresponding to this 
 :*                 hexadecimal digit;
 :*               otherwise returned -1.
 :*                                                                   /)
 :*                                                             END-Post
 :*   RETURNS  "int" (/ = the decimal number or -1 		  /)
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY scan.hexToDec
 :******************************************************************
*/
hexToDec(c)
int c;
{
    switch (c) {
	case '0' : return 0;
	case '1' : return 1;
	case '2' : return 2;
	case '3' : return 3;
	case '4' : return 4;
	case '5' : return 5;
	case '6' : return 6;
	case '7' : return 7;
	case '8' : return 8;
	case '9' : return 9;
	case 'a' :
	case 'A' :
		   return 10;
	case 'b' :
	case 'B' :
		   return 11;
	case 'c' :
	case 'C' :
		   return 12;
	case 'd' :
	case 'D' :
		   return 13;
	case 'e' :
	case 'E' :
		   return 14;
	case 'f' :
	case 'F' :
		   return 15;
	default  : return -1;
    }
}
/*
 :******************************************************************
 :* UNIT-BODY scan.issymbol
 :*
 :* TITLE         (/ checking if the current character is a member
 :*                  of a given list.   /)
 :*
 :* PURPOSE       checking if the current character is a member of a
 :*               subset of the special symbol table. If this is the
 :*               case return its index; otherwise return -1.
 :*                                                      END-PURPOSE
 :* VERSION       001
 :*   CRDATE      1992-11-03
 :*   AUTHOR      (/ Mounji Abdelaziz       , FUNDP              /)
 :*   UPDATE      (/ original issue /)
 :*
 :* REALIZATION   (/ <note, reference to other proc. (optional)> /)
 :*
 :*   CONTROL    (/
 :*                                                               /)
 :*   DATA
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
 :*   Pre    (/ - c    a given character  /)
 :*                                                             END-Pre
 :*   Post   (/ - if c in {'+', '-', '*', '(', ')', '=', ':', ';', ',', '.'}
 :*                 return index
 :*                   where specsymbtab[index] = c;
 :*               otherwise returned -1.
 :*                                                                   /)
 :*                                                             END-Post
 :*   RETURNS  "int" (/ = index of the character in 'specsymbtab' if a member
 :*                     = -1 otherwise                              /)
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY scan.issymbol
 :******************************************************************
*/
isspecsymb(s)
char s[MAXS];
{
	int low, high, mid, k;

	low = 0;
	high = specsymbnbre - 1 ;
	while (low <= high) {
		mid = (low + high) / 2;
		if ((k = strcmp(s, specsymbtab[mid])) < 0)
			high = mid - 1;
		else if (k > 0)
			low = mid + 1;
		else /* s is a special symbol */
			return (mid);
	}
	return (-1);
}
/*
 :******************************************************************
 :* UNIT-BODY scan.get_integer
 :*
 :* TITLE         (/ reading of an integer in the input stream  /)
 :*
 :* PURPOSE       (/ reading of an integer in the input stream and
 :*                  return its internal repres.
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
 :*                     - morechar : = EOF
 :*                     - nextchar : last read character
 :*                     - symb     : internal repres. of the last read
 :*                                  symbol                      /)
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
 :*             - the input stream is in the form:
 :*                   <integer><rest>
 :*               where <integer> is a sequence of digits
 :*                                                            /)
 :*                                                             END-Pre
 :*
 :*   Post   (/ - the input stream is <rest>
 :*             - the internal repres. of the integer represented by
 :*               <integer> is stored in 'symb'
 :*                                                             END-Post
 :*
 :*   RETURNS  NONE
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY scan.get_integer
 :******************************************************************
*/
get_integer(fp)
FILE *fp;
{
	int v = 0;
	int firstchar;

// Old version. Deals only with decimal numbers
/*
	v = 0;
	while (more_char == 1 && isdigit(nextchar)) {
		v = 10 * v + nextchar - '0';
		get_char(fp);
	}
*/
// New version. Reads C-style hex numbers too

	v = nextchar - '0';

	// Test if there is more than one char
	if (more_char == 1) {
		// Keep the first char to compare beginning of string to '0x'
		firstchar = nextchar;
		get_char(fp);
		// Test if the second character may be part of a number (is it a digit or an 'x' after a '0')
		if (isdigit(nextchar) || (firstchar == '0' && nextchar == 'x')) {
			// nextchar is part of a number, keep on decoding
			// Do we decode a decimal or an hex number ?
			if (firstchar == '0' && nextchar == 'x') {
				// Decode an hex number
				// Are there any hex digits after the '0x' ?
				if (more_char == 1) {
					// Decode the hex digits after the '0x'
					get_char(fp);
					while (more_char == 1 && ISHEXDIGIT(nextchar)) {
						v = 16 * v + HEXDIGITVALUE(nextchar);
						get_char(fp);
					}
				} // end if something after '0x'
			}
			else {
				// Decode a decimal number
				while (more_char == 1 && isdigit(nextchar)) {
					v = 10 * v + nextchar - '0';
					get_char(fp);
				}
			}
		} // if second char OK
	} // end if more than 1 char
	symb.ibsymb = integerxx;
	symb.bsymbval.ivalue = v;
}
/*
 :******************************************************************
 :* UNIT-BODY scan.get_Cliteral
 :*
 :* TITLE         (/ reading of a C-literal in the input stream /)
 :*
 :* PURPOSE       (/ reading of a C-literal in the input stream; truncate
 :*                  if characters exceed MAXS then return internal repres.
 :*                                                           /)
 :*                                                      END-PURPOSE
 :* VERSION       001
 :*   CRDATE      1992-11-03
 :*   AUTHOR      (/ Mounji Abdelaziz       , FUNDP              /)
 :*   UPDATE      (/ original issue /)
 :*
 :* REALIZATION   (/ <note, reference to other proc. (optional)> /)
 :*
 :*   CONTROL    (/                        /)
 :*   DATA       (/
 :*                     - morechar : = EOF
 :*                     - nextchar : last read character
 :*                     - symb     : internal repres. of the last read
 :*                                  symbol
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
 :*             - the input stream is in the form:
 :*                   quote<restStream>
 :*               where quote is "'" and <restStream> is any sequence
 :*                                                            /)
 :*                                                             END-Pre
 :*
 :*   Post   (/ - if <restStream> matches the form:
 :*                        <printable>quote<rest>
 :*                  where <printable> is a possibly empty sequence of
 :*                        printable characters excluding quote
 :*               - the current input stream is <rest>
 :*               - the internal representation of <printable1> is stored
 :*                 in 'symb' where <printable1> is <printable> truncated
 :*                   to its  'MAXS' first characters. 
 :* 		  - In case <printable> is more than MAXS characters long,
 :*		    an error message is sent to stdout; 
 :*               otherwise the internal repres. of the error symbol is
 :*                 stored in 'symb'                            /)
 :*
 :*                                                             END-Post
 :*
 :*   RETURNS  NONE
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY scan.get_Cliteral
 :******************************************************************
*/
get_Cliteral(fp)
FILE *fp;
{
	int i;

	get_char1(fp);
	i = 0;
	while (more_char == 1 && isprint(nextchar) && nextchar != quote
	       && i < MAXS)  {
		symb.bsymbval.svalue[i] = nextchar;
		i++;
		get_char1(fp);
	}
	if (i >= MAXS)
		error(syntax, 43);
	while (more_char && nextchar != quote)
		get_char1(fp);
	if (nextchar == quote) {
		symb.bsymbval.svalue[i] = '\0';
		symb.ibsymb = C_literalx;
		get_char(fp);
	} else
	       symb.ibsymb = errsymbx;
}
/*
 :******************************************************************
 :* UNIT-BODY scan.skipspaces
 :*
 :* TITLE         (/ skip spaces in the input stream       /)
 :*
 :* PURPOSE       (/ skipping all white characters (blank, tab and eol)
 :*                  in the input stream
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
 :*                      - nextchar : the last read character
 :*                      - morechar : = EOF
 :*                      - LineNbre : the current line number
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
 :*             - the input stream is in the form:
 :*                    <spaces><rest>
 :*               where <spaces> is a possibly empty sequence of blanks,
 :*                              tabs and end of lines
 :*                     <rest> is a sequence not beginning with a blank,
 :*                            a tab or an end of line
 :*                                                            /)
 :*                                                             END-Pre
 :*
 :*   Post   (/ - the input stream is <rest>
 :*                                                             END-Post
 :*
 :*   RETURNS  NONE
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY scan.skipspaces
 :******************************************************************
*/
skipspaces(fp)
FILE *fp;
{
    while (more_char == 1 &&
	  (nextchar == space || nextchar == eoline || nextchar == tabchar)) {
		get_char(fp);
    }
}
/*
 :******************************************************************
 :* UNIT-BODY scan.get_char
 :*
 :* TITLE         (/ reading of a legal character in the input stream /)
 :*
 :* PURPOSE       (/ reading of the next legal character in the input
 :*                  stream ignoring bad ones.
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
 :*                      - nextchar : the last read character
 :*                      - LineNbre : the current line number
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
 :*                   <badCharSeq> c <rest>
 :*               where <badCharSeq> is a possibly empty sequence of
 :*               illegal characters.
 :*                                                            /)
 :*                                                             END-Pre
 :*
 :*   Post   (/ - the input stream is <rest>
 :*             - nextchar = c
 :*             - n error messages are output to stdout where n is the
 :*               length of <badCharSeq>
 :*             - morechar and LineNbre are updated accordingly    /)
 :*                                                             END-Post
 :*
 :*   RETURNS  NONE
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY scan.get_char
 :******************************************************************
*/
get_char(fp)
FILE *fp;       /* file pointer */
{
    get_char1(fp);
    while (isBadChar()) {    /* ignore bad characters */
	printf("lexical      %d: bad character: %x\n", LineNbre, nextchar);
	get_char1(fp);
    }
}
/*
 :******************************************************************
 :* UNIT-BODY scan.get_char1
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
 :*                      - nextchar : the last read character
 :*                      - morechar : = EOF
 :*                      - LineNbre : the current line number
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
 :*               if c is end of line LineNbre is incremented by 1.
 :*             - the input stream is <rest>
 :*
 :*                                                             END-Post
 :*
 :*   RETURNS  NONE
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY scan.get_char1
 :******************************************************************
*/
get_char1(fp)
FILE *fp;       /* file pointer */
{
	if ((nextchar = getc(fp)) == EOF) {
	    more_char = 0;
	} else {
	    more_char = 1;
	    if (nextchar == eoline) LineNbre++;
	}
}
/*
 :******************************************************************
 :* UNIT-BODY scan.isBadChar
 :*
 :* TITLE         (/ check if this is a legal character    /)
 :*
 :* PURPOSE       (/ checking if the last read character is a legal one /)
 :*
 :*                                                      END-PURPOSE
 :* VERSION       001
 :*   CRDATE      1992-11-03
 :*   AUTHOR      (/ Mounji Abdelaziz       , FUNDP              /)
 :*   UPDATE      (/ original issue /)
 :*
 :* REALIZATION   (/ <note, reference to other proc. (optional)> /)
 :*
 :*   CONTROL    (/  /)
 :*   DATA       (/
 :*                  - nextchar   : the last character being read.
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
 :*   Pre    (/
 :*                                                            /)
 :*                                                             END-Pre
 :*
 :*   Post   (/ - if nextchar in
 :*                    A = {'+', '-', '*', '(', ')', '>', '<', '!', '=',
 :*                         ':', ';', ',', '/', '.', '\', '_', ''', '%' ' '}
 :*                 or nextchar is a letter, a digit, a tab, an end of line
 :*                 or an end of file
 :*               return 0
 :*               return 1 otherwise
 :*
 :*                                                             END-Post
 :*   RETURNS  "int" (/ =0 if nextchar is among the above list
 :*                     =1 otherwise                              /)
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY scan.isBadChar
 :******************************************************************
*/
isBadChar()
{
    if ( isalpha(nextchar) || isdigit(nextchar) ||
	 nextchar == '+' || nextchar == '-' || nextchar == '*' ||
	 nextchar == '(' || nextchar == ')' || nextchar == '>' ||
	 nextchar == '<' || nextchar == '!' || nextchar == '=' ||
	 nextchar == ':' || nextchar == ';' || nextchar == ',' ||
	 nextchar == '/' || nextchar == '.' || nextchar == '\\' ||
	 nextchar == '_' || nextchar == '^' || nextchar == '[' ||
	 nextchar == ']' || nextchar == '$' || nextchar == '|' ||
	 nextchar == '{' || nextchar == '}' || 
	 nextchar == quote || nextchar == tabchar || nextchar == eoline ||
	 nextchar == space || nextchar == EOF || nextchar == '%')

	 return(0);
    else
	 return(1);
}
/*
 :******************************************************************
 :* UNIT-BODY recov.symbolClass
 :*
 :* TITLE         (/ return the class of a token      /)
 :*
 :* PURPOSE       (/ given a token, return the corresponding class /)
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
 :*   Pre    (/ - andx <= index < nosymbx
 :*                                                            /)
 :*                                                             END-Pre
 :*
 :*   Post   (/ - return the symbol class correspending to this index /)
 :*                                                             END-Post
 :*
 :*   RETURNS  "int" (/ represents the symbol class of index /)
 :*
 :*   REMARKS             (/ <general notes (none -> no REMARKS)> /)
 :*                                                   END-PARAMETERS
 :*
 :* END-BODY recov.symbolClass
 :******************************************************************
*/
symbolClass(index)
int index;
{
	int aux1, aux2, aux3, aux4;

	aux1 = andx;
	aux2 = period;
	aux3 = falsex;
	aux4 = truex;
	if (index == aux3 || index == aux4) return(boolean);
	else if (index >= aux1 && index <= aux2) return(specsymb);
	else switch (index) {
		case integerxx  : return(integer_t);
		case C_literalx : return(C_literal);
		case X_literalx : return(X_literal);
		case identifierx: return(identifier);
		case booleanx   : return(boolean);
		case nosymbx    : return(nosymb);
		case errsymbx   : return(errsymb);
		case eqx        : return(eq);
		default         : return(integer_t);
	     }
}

#pragma warning(default:4035) // 'no return value'
