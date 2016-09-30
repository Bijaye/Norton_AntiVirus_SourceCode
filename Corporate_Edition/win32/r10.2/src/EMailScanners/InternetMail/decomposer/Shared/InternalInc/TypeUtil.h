/*
 * Installmaker
 * COPYRIGHT FUJITSU DEVICES INC. 1994
 *
 */
#ifndef __TYPEUTIL_H__

	#define __TYPEUTIL_H__

	typedef unsigned char FAR *LPUSTR;

	#if !defined(UNIX)
		typedef unsigned char UCHAR;
	#endif

	typedef unsigned char byte;
	typedef unsigned short word;
	typedef unsigned long dword;
	typedef short int ARCNO;
	typedef char *PTR;


	#if defined(MSDOS) || defined(UNIX)
		# ifndef __
			#  define __(args) args
		# endif
	#else
		# ifndef __
			#  define __(args) ()
		# endif
	#endif

	#if defined(_WINDOWS)
		#define ASCII_DIR_SEP "\\"
		#define DIR_SEP "\\"
		#define DIR_SEP_CHAR '\\'
		#define ALL_FILES "*.*"
	#endif

	#if defined(UNIX)
		#define ASCII_DIR_SEP "\x2f"
		#define DIR_SEP "/"
		#define DIR_SEP_CHAR '/'
		#define ALL_FILES "*"
	#endif

	#if defined(SYM_NLM)
		#pragma message ("I don't know if this is correct - check it!")
		#define ASCII_DIR_SEP "\\"
		#define DIR_SEP "\\"
		#define DIR_SEP_CHAR '\\'
		#define ALL_FILES "*.*"
	#endif

	#if !defined(SYM_NLM) && !defined(UNIX) && !defined(_WINDOWS) /* Default separator */
		#define ASCII_DIR_SEP "\x2f"
		#define DIR_SEP "/"
		#define DIR_SEP_CHAR '/'
		#define ALL_FILES "*"
	#endif

#endif /* __TYPEUTIL_H__ */
/*
 * End of `typeutil.h'
 */
