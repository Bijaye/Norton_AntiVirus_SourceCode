// Copyright 2004-2005 Symantec Corporation. All rights reserved.
// Author: Bertil Askelid <Bertil_Askelid@Symantec.com>
// Intent: Linux implementation of NDK String Handling functions

// ================== ndkStrings ========================================================

#include <new>

extern "C" {
	#include <wctype.h>
	#include <ctype.h>

	#include <ndkStrings.h>
}

// When something is wrong, we return a NULL pointer. There is no sense in continuing,
// and the system will crash pretty soon, making it easy to locate the problem with a
// debugger.

// ================== Internationalization ==============================================

// A unified handling of wide characters, multi-byte characters, and single-byte
// characters.

// --------------------- Utilities ------------------------------------------------------

static inline bool wfCharacter(size_t lCharacter) {
	return lCharacter && lCharacter != (size_t)(-1) && lCharacter != (size_t)(-2);
}
static inline bool wfBytes(size_t lCharacter) {
	return lCharacter != (size_t)(-1);
}

// --------------------- Language -------------------------------------------------------

// Returns the language ID for the language currently running on the server.

int GetCurrentOSLanguageID() {return 15;}			// dummy number for now

// Returns the name associated with an OS language ID.

int ReturnLanguageName(int languageID,BYTE* languageName) {
	if (languageID == 15) {
		strcpy((char*) languageName,"English"); return 0;
	}
	else return 1;
}
// --------------------- Locale ---------------------------------------------------------

// Pointer to structure of locale environment information.

struct LCONV* NWLlocaleconv(struct LCONV* pLconv) {
	*pLconv = *localeconv(); return pLconv;
}

// --------------------- String Size ----------------------------------------------------

// Number of characters in a multi-byte string.

size_t _mbslen(const char* mbString) {
	const char* mbs = mbString; mbstate_t state; memset(&state,0,sizeof(mbstate_t));
	return mbsrtowcs(NULL,&mbs,0,&state);
}

// --------------------- Next Character -------------------------------------------------

/** Returns a pointer to the character after `mbCurrent'. If `mbCurrent' points to the
 * last character, a pointer to the terminating '\0' will be returned. If `mbCurrent'
 * points to the terminating '\0', the return value is equal to mbCurrent.
 */
wchar_t* _wcsinc(const wchar_t* wcCurrent) {
    wchar_t* retVal = const_cast<wchar_t*>(wcCurrent);
    if ( wcCurrent && *wcCurrent ) {
        retVal += 1;
    }
    return retVal;
}

char* _mbsinc(const char* mbCurrent) {
    char* retVal = const_cast<char*>(mbCurrent);

    if ( mbCurrent && *mbCurrent) {
        mbstate_t state;
        memset(&state, 0, sizeof(mbstate_t));
        size_t lCharacter = mbrlen(mbCurrent, MB_CUR_MAX, &state);
        retVal += wfCharacter(lCharacter) ? lCharacter : 1;
    }

    return retVal;
}

char* _strinc(const char* sbCurrent) {
    char* retVal = const_cast<char*>(sbCurrent);
    if ( sbCurrent && *sbCurrent ) {
        retVal++;
    }
    return retVal;
}

// --------------------- Previous Character ---------------------------------------------

// Returns a pointer to the character before `mbCurrent'. If `mbCurrent' points to the
// terminating '\0', the last character is returned. It is an error for `mbCurrent' to be
// NULL, less than or equal to `mbStart', or pointing after the terminating '\0'.

wchar_t* _wcsdec(const wchar_t* wcStart,const wchar_t* wcCurrent) {
	return
		wcStart && wcCurrent && wcStart < wcCurrent &&
		wcCurrent <= (wchar_t*) wcStart + wcslen(wcStart)
		? (wchar_t*) wcCurrent - 1 : NULL;
}

// As different multi-byte characters have different lengths (number of bytes), we have
// to start at the beginning and traverse until we get back to the current character, at
// each step making note of the position of the character we just left behind.

char* _mbsdec(const char* mbStart,const char* mbCurrent) {
	if (! (mbStart && mbCurrent && mbStart < mbCurrent)) return NULL;

	mbstate_t state; memset(&state,0,sizeof(mbstate_t));
	char *r,*s; size_t lCharacter;
	for (s = (char*) mbStart,r = s; s < mbCurrent; r = s,s += lCharacter)
		if (! wfCharacter(lCharacter = mbrlen(s,MB_CUR_MAX,&state))) return NULL;
	return r;
}

char* _strdec(const char* sbStart,const char* sbCurrent) {
	return
		sbStart && sbCurrent && sbStart < sbCurrent &&
		sbCurrent <= (char*) sbStart + strlen(sbStart)
		? (char*) sbCurrent - 1 : NULL;
}

// --------------------- Search ---------------------------------------------------------

// If the wide character is in the ASCII range, we look for it using the regular
// single-byte character lookup knowing that UTF-8 guarantees that ASCII characters are
// unique in a multi-byte string. Otherwise, the wide character is converted to its
// multi-byte representation and the single-byte substring lookup function completes the
// job.

char* mbschr(const char* mbString,wchar_t wcCharacter) {
	if (wcCharacter == 0x00) return NULL;
	if (wcCharacter <	 0x80) return strchr(mbString,wcCharacter);

	char mbCharacter[MB_CUR_MAX + 1]; size_t n;
	mbstate_t state; memset(&state,0,sizeof(mbstate_t));
	if (! (n = wfBytes(wcrtomb(mbCharacter,wcCharacter,&state)))) return NULL;

	mbCharacter[n + 1] = '\0'; return strstr(mbString,mbCharacter);
}

// When the wide character is in the ASCII range, the single-byte character lookup
// function looks for the right-most occurrence. Otherwise, it is converted to its
// multi-byte representation and the string is traversed in reverse comparing the
// multi-byte character substring at each step until a match is found. I none is found,
// NULL is returned.

char* mbsrchr(const char* mbString,wchar_t wcCharacter) {
	if (wcCharacter == 0x00) return NULL;
	if (wcCharacter <	 0x80) return strrchr(mbString,wcCharacter);

	char mbCharacter[MB_CUR_MAX + 1]; size_t n;
	mbstate_t state; memset(&state,0,sizeof(mbstate_t));
	if (! (n = wfBytes(wcrtomb(mbCharacter,wcCharacter,&state)))) return NULL;

	mbCharacter[n + 1] = '\0';

	char *p = (char*) mbString + strlen(mbString) - strlen(mbCharacter),*q,*r;
	for (; p >= mbString; p--) {
		for (q = p,r = mbCharacter; *r; q++,r++) if (*q != *r) goto next;
		break;
next:;
	}
	return p >= mbString ? p : NULL;
}

// --------------------- Compare --------------------------------------------------------

// Comparison of multibyte strings has to been done on their wide character
// representation to ensure that the correct lexicographical order is preserved.

int _mbscmp(const char* s1,const char* s2) {
	wchar_t* w1 = mbs2wcs(s1); if (! w1) return 0;
	wchar_t* w2 = mbs2wcs(s2); if (! w2) {delete[] w1; return 0;}

	int rc = wcscmp(w1,w2); delete[] w1; delete[] w2; return rc;
}
int _mbsicmp(const char* s1,const char* s2) {
	wchar_t* w1 = mbs2wcs(s1); if (! w1) return 0;
	wchar_t* w2 = mbs2wcs(s2); if (! w2) {delete[] w1; return 0;}

	int rc = wcscasecmp(w1,w2); delete[] w1; delete[] w2; return rc;
}
int _mbsnbcmp(const char* s1,const char* s2,size_t n) {
	char* t1 = new(std::nothrow) char[n + 1]; if (! t1) return 0;
	char* t2 = new(std::nothrow) char[n + 1]; if (! t2) {delete[] t1; return 0;}

	strncpy(t1,s1,n); t1[n] = '\0';
	strncpy(t2,s2,n); t2[n] = '\0';

	int rc = _mbscmp(t1,t2); delete[] t1; delete[] t2; return rc;
}
int _mbsnbicmp(const char* s1,const char* s2,size_t n) {
	char* t1 = new(std::nothrow) char[n + 1]; if (! t1) return 0;
	char* t2 = new(std::nothrow) char[n + 1]; if (! t2) {delete[] t1; return 0;}

	strncpy(t1,s1,n); t1[n] = '\0';
	strncpy(t2,s2,n); t2[n] = '\0';

	int rc = _mbsicmp(t1,t2); delete[] t1; delete[] t2; return rc;
}

// --------------------- Case Conversion ------------------------------------------------

// For fixed width characters, the case conversion is done character by character in a
// simple traversal.

static wchar_t* wcConvert(wchar_t* wcString,wint_t (*fun)(wint_t)) {
	wchar_t* wcResult = wcString;
	for (; *wcString; wcString++) *wcString = fun(*wcString);
	return wcResult;
}

// A multi-byte character string is converted to wide characters, transformed using a
// wide character string transformation function, and then converted back to a multi-byte
// character string representation. Note that for the case when the transformed string
// contains more bytes than the original string, a new is allocated while the original is
// deleted. Also note that the `nr*' variables indicate number of bytes/characters
// written to the destination string, i.e. including the terminating '\0'.

static char* mbConvert(char* mbString,wchar_t* (*fun)(wchar_t*)) {
	mbstate_t state; size_t nrChars,nrBytes;

	const char* mbs = mbString; memset(&state,0,sizeof(mbstate_t));
	if (! wfBytes(nrChars = mbsrtowcs(NULL,&mbs,0,&state))) return NULL; nrChars++;

	wchar_t wcString[nrChars]; mbs = mbString; memset(&state,0,sizeof(mbstate_t));
	mbsrtowcs(wcString,&mbs,nrChars,&state);

	fun(wcString);

	const wchar_t* wcs = wcString; memset(&state,0,sizeof(mbstate_t));
	if (! wfBytes(nrBytes = wcsrtombs(NULL,&wcs,0,&state))) return NULL; nrBytes++;

	char mbResult[nrBytes]; wcs = wcString; memset(&state,0,sizeof(mbstate_t));
	wcsrtombs(mbResult,&wcs,nrBytes,&state);

	// There is a risk here that the number of bytes needed to hold the UTF8 conversion of
	// `wcString' is larger than what is allocated for `mbString' -- the result will be
	// truncated. We have to make sure that the last character copied from `mbResult' to
	// `mbString' is not cut in half.

	char *s = mbString,*r = mbResult,*p,*eos = s + strlen(s);
	while (*r && s + (size_t) ((p = _mbsinc(r)) - r) <= eos)
		while (r < p) *s++ = *r++;
	*s = '\0';

	return mbString;
}

static char* sbConvert(char* sbString,int (*fun)(int)) {
	char* sbResult = sbString;
	for (; *sbString; sbString++) *sbString = fun(*sbString);
	return sbResult;
}

wchar_t* _wcsupr(wchar_t* wcString) {return wcConvert(wcString,towupper);}
char*		_mbsupr(char*	  mbString) {return mbConvert(mbString,_wcsupr);}
char*		_sbsupr(char*	  sbString) {return sbConvert(sbString,toupper);}

wchar_t* _wcslwr(wchar_t* wcString) {return wcConvert(wcString,towlower);}
char*		_mbslwr(char*	  mbString) {return mbConvert(mbString,_wcslwr);}
char*		_sbslwr(char*	  sbString) {return sbConvert(sbString,tolower);}

// --------------------- Character Set Conversion ---------------------------------------

char* wcs2mbs(const wchar_t* wcString) {
	const wchar_t* wcs; mbstate_t state; size_t n; char* mbString;

	wcs = wcString; memset(&state,0,sizeof(mbstate_t)); n = wcsrtombs(NULL,&wcs,0,&state);
	n++;	mbString = new(std::nothrow) char[n]; if (! mbString) return NULL;
	wcs = wcString; memset(&state,0,sizeof(mbstate_t)); wcsrtombs(mbString,&wcs,n,&state);

	return mbString;
}
wchar_t* mbs2wcs(const char* mbString) {
	const char* mbs; mbstate_t state; size_t n; wchar_t* wcString;

	mbs = mbString; memset(&state,0,sizeof(mbstate_t)); n = mbsrtowcs(NULL,&mbs,0,&state);
	n++;	wcString = new(std::nothrow) wchar_t[n]; if (! wcString) return NULL;
	mbs = mbString; memset(&state,0,sizeof(mbstate_t)); mbsrtowcs(wcString,&mbs,n,&state);

	return wcString;
}
