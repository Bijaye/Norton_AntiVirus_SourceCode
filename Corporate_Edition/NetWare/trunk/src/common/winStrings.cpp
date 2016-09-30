// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation. All rights reserved.
// Author: Bertil Askelid <Bertil_Askelid@Symantec.com>
// Intent: NetWare implementation of Windows string handling functions

#include <stdarg.h>

#include <new>

#include "winStrings.h"
#include "SymSaferStrings.h"
#include "SavAssert.h"

// ================== libc ==============================================================

#ifdef SYM_LIBC_NLM

// --------------------- Search ---------------------------------------------------------

char* _mbschr(const char* string,char character) {
	return (char*) utf8chr((const utf8_t*) string,(const utf8_t*) &character);
}
char* _mbsrchr(const char* string,char character) {
	return (char*) utf8rchr((const utf8_t*) string,(const utf8_t*) &character);
}

// --------------------- Compare --------------------------------------------------------

int _mbsnbcmp(const char* s1,const char* s2,size_t n) {
	char* t1 = new(std::nothrow) char[n + 1]; if (! t1) return 0;
	char* t2 = new(std::nothrow) char[n + 1]; if (! t2) {delete[] t1; return 0;}

	strncpy(t1,s1,n); t1[n] = '\0';
	strncpy(t2,s2,n); t2[n] = '\0';

	int rc = utf8cmp((const utf8_t*) t1,
						  (const utf8_t*) t2); delete[] t1; delete[] t2; return rc;
}
int _mbsnbicmp(const char* s1,const char* s2,size_t n) {
	char* t1 = new(std::nothrow) char[n + 1]; if (! t1) return 0;
	char* t2 = new(std::nothrow) char[n + 1]; if (! t2) {delete[] t1; return 0;}

	strncpy(t1,s1,n); t1[n] = '\0';
	strncpy(t2,s2,n); t2[n] = '\0';

	int rc = utf8cmp(utf8lwr((utf8_t*) t1),
						  utf8lwr((utf8_t*) t2)); delete[] t1; delete[] t2; return rc;
}

// ================== Clib ==============================================================

#elif defined NLM

#include <time.h>
#include <nunicode.h>  

#include <fcntl.h>
#include <sys/stat.h>
#include <nlm/nwfinfo.h>
#include <unistd.h>

#ifndef P_tmpdir
	#define P_tmpdir "sys:/tmp"
#endif

// --------------------- Compare --------------------------------------------------------

int _mbsnbcmp(const char* s1,const char* s2,size_t n) {
	char* t1 = new(std::nothrow) char[n + 1]; if (! t1) return 0;
	char* t2 = new(std::nothrow) char[n + 1]; if (! t2) {delete[] t1; return 0;}

	strncpy(t1,s1,n); t1[n] = '\0';
	strncpy(t2,s2,n); t2[n] = '\0';

	int rc = NWLstrcoll(t1,t2); delete[] t1; delete[] t2; return rc;
}
int _mbsnbicmp(const char* s1,const char* s2,size_t n) {
	char* t1 = new(std::nothrow) char[n + 1]; if (! t1) return 0;
	char* t2 = new(std::nothrow) char[n + 1]; if (! t2) {delete[] t1; return 0;}

	strncpy(t1,s1,n); t1[n] = '\0';
	strncpy(t2,s2,n); t2[n] = '\0';

	int rc = NWLstricmp(t1,t2); delete[] t1; delete[] t2; return rc;
}

// --------------------- Case Conversion ------------------------------------------------

static inline unsigned char* _mbsinc(unsigned char* mbString) {
	return (unsigned char*) NWNextChar((const char*) mbString);
}
char* _mbslwr(char* mbString) {
	const unsigned char* mbMixed = (const unsigned char*) mbString;
	char* mbResult = mbString;

	unicode *wcMixed = NULL,*wcLower = NULL; unsigned char* mbLower = NULL;
	
	try {
		size_t nrMixed; if (! NWLUTF8ToUnicodeSize(mbMixed,&nrMixed)) throw;
		if (! (wcMixed = new(std::nothrow) unicode[nrMixed])) throw;
		NWLUTF8ToUnicode(mbMixed,nrMixed,wcMixed,NULL,NULL);

		size_t nrLower; NWUSUnicodeToLowerCase(NULL,0,wcMixed,&nrLower);
		if (! (wcLower = new(std::nothrow) unicode[++nrLower])) throw;
		NWUSUnicodeToLowerCase(wcLower,nrLower,wcMixed,NULL);
	
		size_t nrBytes = NWLUnicodeToUTF8Size(wcLower);
		if (! (mbLower = new(std::nothrow) unsigned char[nrBytes])) throw;
		NWLUnicodeToUTF8(wcLower,nrBytes,mbLower,NULL); 
	
		// There is a risk here that the number of bytes needed to hold the UTF8 conversion
		// of `wcLower' is larger than what is allocated for `mbString' -- the result will
		// be truncated. We have to make sure that the last character copied from `mbLower'
		// to `mbString' is not cut in half.
		
		unsigned char* p; char* eos = mbString + strlen(mbString);
		while (*mbLower && mbString + (size_t) ((p = _mbsinc(mbLower)) - mbLower) <= eos)
			while (mbLower < p) *mbString++ = *mbLower++;
		*mbString = '\0';

	} catch (...) {mbResult = NULL;}

	if (wcMixed) delete[] wcMixed; if (wcLower) delete[] wcLower;
	if (mbLower) delete[] mbLower;

	return mbResult;
}


#define vsnprintf vsnprintf
#define _vsnprintf _vsnprintf
// --------------------- Print Format to String -----------------------------------------
int _vsnprintf( char* mbString, size_t bufferSize, const char* mbFormat, va_list args )
{
    // We must pass *args instead of args because libc's va_list is a char*, but clib's
    // is a char*[1].  So just dereference the pointer to fix it up to what libc needs.
    return vsnprintf(mbString, bufferSize, mbFormat, (va_listLIBC) *args);
}

// --------------------- RPC ------------------------------------------------------------

RPC_STATUS UuidToString( UUID __RPC_FAR* Uuid, unsigned char __RPC_FAR * __RPC_FAR* StringUuid )
// Sets *StringUuid equal to a string representing the value of Uuid
{
    LPTSTR          tempStringUuid      = NULL;
    RPC_STATUS      returnValRPC                    = RPC_S_OUT_OF_MEMORY;

    // Validate parameters
    if (Uuid == NULL)
        return RPC_S_INVALID_ARG;
    if (StringUuid == NULL)
        return RPC_S_INVALID_ARG;
    if (*StringUuid != NULL)
        return RPC_S_INVALID_ARG;

    // Allocate string, re-pack UUID into values for sprintf, then create the string via
	 // sprintf
    tempStringUuid = new(std::nothrow) TCHAR[37];
    if (tempStringUuid != NULL)
    {
        // NOTE:  values must be built byte-by-byte because Intel processors are little
		  // endian, so a simple cast will result in byte swapped and reversed data in the
		  // sprintf
        DWORD component4 = Uuid->Data4[1] | (Uuid->Data4[0] << 8);
        DWORD component5 = Uuid->Data4[5] | (Uuid->Data4[4] << 8) |
			  (Uuid->Data4[3] << 16) | (Uuid->Data4[2] << 24);
        DWORD component6 = Uuid->Data4[7] | (Uuid->Data4[6] << 8);
        sssnprintf(tempStringUuid, sizeof(tempStringUuid),
						 "%08x-%04x-%04x-%04x-%08x%04x",
						 Uuid->Data1, Uuid->Data2, Uuid->Data3,
						 component4, component5, component6);
        returnValRPC = RPC_S_OK;
    }
    else
    {
        returnValRPC = RPC_S_OUT_OF_MEMORY;
    }

    // Return result
    *StringUuid = (unsigned char*) tempStringUuid;
    return returnValRPC;
}

RPC_STATUS UuidFromString( unsigned char __RPC_FAR* StringUuid, UUID __RPC_FAR* Uuid )
// Sets *Uuid equal to the UUID value represented by StringUuid
{
    DWORD componentA = 0;
    DWORD componentB = 0;
    DWORD componentC = 0;
    DWORD componentD = 0;
    DWORD componentE = 0;
    DWORD componentF = 0;
    DWORD noItems = 0;

    // Validate parameters
    if (Uuid == NULL)
        return RPC_S_INVALID_ARG;
    if (StringUuid == NULL)
        return RPC_S_INVALID_ARG;

    // Scan string components into separate DWORDs, then store in the UUID
    noItems = _stscanf((LPCTSTR) StringUuid, "%08x-%04x-%04x-%04x-%08x%04x",
							  &componentA, &componentB, &componentC, &componentD, &componentE,
							  &componentF);
    if (noItems == 6)
    {
        Uuid->Data1 = componentA;
        Uuid->Data2 = (WORD) componentB;
        Uuid->Data3 = (WORD) componentC;
        Uuid->Data4[0] = HIBYTE(componentD);
        Uuid->Data4[1] = LOBYTE(componentD);
        Uuid->Data4[2] = HIBYTE(HIWORD(componentE));
        Uuid->Data4[3] = LOBYTE(HIWORD(componentE));
        Uuid->Data4[4] = HIBYTE(LOWORD(componentE));
        Uuid->Data4[5] = LOBYTE(LOWORD(componentE));
        Uuid->Data4[6] = HIBYTE(componentF);
        Uuid->Data4[7] = LOBYTE(componentF);

        return RPC_S_OK;
    }
    else
    {
        return RPC_S_INVALID_ARG;
    }
}

RPC_STATUS RpcStringFree( IN OUT unsigned char __RPC_FAR * __RPC_FAR * String )
{
    // Validate parameters
    if (String == NULL)
        return RPC_S_INVALID_ARG;
    if (*String == NULL)
        return RPC_S_INVALID_ARG;

    // Release the memory
    delete [] *String;
    *String = NULL;
    return RPC_S_OK;
}

#endif // NLM

// ================== Common ============================================================

char* PathAddBackslash(char* lpszPath) {
   size_t n = strlen(lpszPath); lpszPath += n - 1;
   if (*(lpszPath++) != '\\') {
      if (n >= MAX_PATH - 1) {
			SetLastError(ERROR_BUFFER_OVERFLOW); return NULL;
		}
      *(lpszPath++) = '\\'; *lpszPath = '\0';
   }
   return lpszPath;
}

char* mkWindowsPath(char* path) {
   for (char* p = path; *p; p++) if (*p == '/') *p = '\\';
   return path;
}
