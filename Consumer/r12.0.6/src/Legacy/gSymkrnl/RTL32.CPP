// Copyright 1994 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/SYMKRNL/VCS/RTL32.CPv   1.13   21 Jun 1998 21:42:42   RStanev  $
//
// Description:
//      This file contains 32-bit RTL functions.  We need these
//      for VxD & NTK platforms.
//
// Contains:
//      _memccpy()
//      memcpy()
//      memcmp()
//      memmove()
//      memset()
//      strncpy(),    wcsncpy()
//      strcpy(),     wcscpy()
//      strcmp(),     wcscmp()
//      strlen(),     wcslen()
//      strcat(),     wcscat()
//      _strrev(),    _wcsrev()
//      strchr(),     wcschr()
//      strrchr(),    wcsrchr()
//      strncmp(),    wcsncmp()
//      _stricmp(),   _wcsicmp()
//      _strnicmp(),  _wcsnicmp()
//      strcspn(),    wcscspn()
//      strpbrk(),    wcspbrk()
//      strstr(),     wcsstr()
//      toupper(),    towupper()
//      tolower(),    towlower()
//      _strupr(),    _wcsupr()
//      _strlwr(),    _wcslwr()
//      vxd_strupr()                    (SYM_VXD only, local function)
//
//***************************************************************************
// $Log:   S:/SYMKRNL/VCS/RTL32.CPv  $
// 
//    Rev 1.13   21 Jun 1998 21:42:42   RStanev
// vxd_strupr now support MBCS strings.
//
//    Rev 1.12   21 May 1997 18:05:06   BILL
// Removed memmove for Alpha platform (used intrinis version)
//
//    Rev 1.11   20 May 1997 12:34:46   RStanev
// The assembly version of wcschr() will now compile only during pass 1
// of SYM_VXD compilations.
//
//    Rev 1.10   12 May 1997 22:04:16   RStanev
// Added strcspn(), wcscspn(), and _strlwr().
//
//    Rev 1.9   07 Mar 1997 13:20:20   RSTANEV
// Avoided the definition of more wcs* functions already exported by
// ntoskrnl.
//
//    Rev 1.8   23 Apr 1996 20:07:32   RSTANEV
// Avoided the definition of functions already exported by ntoskrnl.
//
//    Rev 1.7   21 Feb 1996 14:57:54   RSTANEV
// Removed DRIVER_..._SEGMENT(f) macro references.
//
//    Rev 1.6   08 Feb 1996 13:46:50   RSTANEV
// Shoot me.  Restored the asm version of _memccpy().
//
//    Rev 1.5   07 Feb 1996 19:17:38   RSTANEV
// Minor changes in the assembly code.
//
//    Rev 1.4   06 Feb 1996 19:16:30   RSTANEV
// strnicmp() and wcsnicmp() renamed to _strnicmp() and _wcsnicmp().
//
//    Rev 1.3   06 Feb 1996 19:12:28   RSTANEV
// Not defining _wcsicmp() and wcsnicmp() for SYM_VXD platform.
//
//    Rev 1.2   02 Feb 1996 10:45:50   RSTANEV
// Cleaned up.
//
//    Rev 1.1   01 Feb 1996 16:02:10   RSTANEV
// This file now generates both MBCS and WCHAR string functions.
//
//    Rev 1.0   26 Jan 1996 20:22:10   JREARDON
// Initial revision.
//
//    Rev 1.8   27 Nov 1995 11:49:52   RSTANEV
// Made sure it compiles for SYM_NTK.
//
//    Rev 1.7   14 Jun 1995 09:06:14   RSTANEV
// Explicitly putting code/data in locked segments.
//
//    Rev 1.6   23 May 1995 10:16:00   RSTANEV
// Fixed strncpy(), so that it pads the remaining n characters with 0 if
// the source string is too short to fill the destination buffer.
//
//    Rev 1.5   03 Jan 1995 20:27:12   RSTANEV
// Changed memcmp() the same way as strcmp().
//
//    Rev 1.4   03 Jan 1995 18:55:54   RSTANEV
// Fixed a comparison that was breaking strstr().
//
//    Rev 1.3   03 Jan 1995 16:30:56   RSTANEV
// Fixed a problem with strcmp().
//
//    Rev 1.2   20 Dec 1994 18:06:48   BRAD
// Added stricmp
//
//    Rev 1.1   09 Dec 1994 19:50:52   BRAD
// Added strrchr, strpbrk, strncmp, and strnicmp
//
//    Rev 1.0   09 Dec 1994 17:10:58   BRAD
// Initial revision.
//
//***************************************************************************

#if !defined(RTL32_NOW_COMPILING)
#define RTL32_NOW_COMPILING

#include "platform.h"

#pragma function(memcpy)
#pragma function(memcmp)
#pragma function(memset)
#pragma function(strcpy)
#pragma function(strcmp)
#pragma function(strlen)
#pragma function(strcat)

#if defined(SYM_NTK)
#define wcscpy    exported_by_ntoskrnl_wcscpy
#define wcsncpy   exported_by_ntoskrnl_wcsncpy
#define wcscmp    exported_by_ntoskrnl_wcscmp
#define _wcsicmp  exported_by_ntoskrnl__wcsicmp
#define wcsncmp   exported_by_ntoskrnl_wcsncmp
#define _wcsnicmp exported_by_ntoskrnl__wcsnicmp
#define wcscat    exported_by_ntoskrnl_wcscat
#define wcslen    exported_by_ntoskrnl_wcslen
#define wcschr    exported_by_ntoskrnl_wcschr
#endif

#if defined(SYM_VXD)
#pragma optimize("", off)
static void __cdecl vxd_strupr ( char *str, int charset );
#endif

#if defined(SYM_NTK)
#if defined(RtlMoveMemory)
#undef RtlMoveMemory
#endif
extern "C" void RtlMoveMemory ( void *, const void *, size_t count );;
#endif

#define RTL32_NOW_COMPILING_PASS1
#include __FILE__
#undef RTL32_NOW_COMPILING_PASS1

#define RTL32_NOW_COMPILING_PASS2
#include __FILE__
#undef RTL32_NOW_COMPILING_PASS2

#else

/////////////////////////////////////////////////////////////////////////////
//
// This file defines different RTL functions, depending on the current
// compilation pass:
//
// Pass 1 (RTL32_NOW_COMPILING_PASS1 defined)
//      _memccpy()
//      memcpy()
//      memcmp()
//      memmove()
//      memset()
//      strncpy()
//      strcpy()
//      strcmp()
//      strlen()
//      strcat()
//      _strrev()
//      strchr()
//      strrchr()
//      strncmp()
//      _stricmp()
//      _strnicmp()
//      strcspn()
//      strpbrk()
//      strstr()
//      toupper()
//      tolower()
//      _strupr()
//      _strlwr()
//      vxd_strupr()
//
//
// Pass 2 (RTL32_NOW_COMPILING_PASS2 defined)
//      wcsncpy()
//      wcscpy()
//      wcscmp()
//      wcslen()
//      wcscat()
//      _wcsrev()
//      wcschr()
//      wcsrchr()
//      wcsncmp()
//      _wcsicmp()
//      _wcsnicmp()
//      wcscspn()
//      wcspbrk()
//      wcsstr()
//      towupper()
//      towlower()
//      _wcsupr()
//      _wcslwr()
//
/////////////////////////////////////////////////////////////////////////////

#if defined(gen_char)
#undef gen_char
#endif

#if defined(RTL32_NOW_COMPILING_PASS1)
#define gen_char char
#else
#define gen_char wchar_t
#endif

#if defined(gen_intchar)
#undef gen_intchar
#endif

#if defined(RTL32_NOW_COMPILING_PASS1)
#define gen_intchar int
#else
#define gen_intchar wchar_t
#endif

#if defined(gen_toupper)
#undef gen_toupper
#endif

#if defined(RTL32_NOW_COMPILING_PASS1)
#define gen_toupper(x) toupper(x)
#else
#if defined(SYM_NTK)
#define gen_toupper(x) RtlUpcaseUnicodeChar(x)
#else
#define gen_toupper(x) towupper(x)
#endif
#endif

#if defined(RTL32_NOW_COMPILING_PASS1)

void * __cdecl _memccpy (void *dest, const void *src, int val, unsigned int n)
    {
#if defined(SYM_VXD)
    auto void* result;
    _asm
        {
//        push    esi
//        push    edi
        mov     result,0
        mov     ecx,n
        jecxz   ccpyend
        cld
        mov     eax,val
        mov     esi,src
        mov     edi,esi
        mov     edx,ecx
        repne   scasb
        mov     edi,dest
        jne     ccpynotfound
        sub     ecx,edx
        neg     ecx
        lea     eax,[edi + ecx]
        mov     result,eax
        jmp     ccpycopy
  ccpynotfound:
        sub     ecx,edx
        neg     ecx
  ccpycopy:
        test    esi,3
        jnz     unalignedccpycopy
        test    edi,3
        jnz     unalignedccpycopy
        mov     eax,ecx
        shr     ecx,2
        jz      ccpybyte
        rep     movsd
  ccpybyte:
        and     eax,3
        jz      ccpyend
        mov     ecx,eax
  unalignedccpycopy:
        rep     movsb
  ccpyend:
//        pop     edi
//        pop     esi
        }
    return result;
#else
    while ( n && ( *((char*)(dest=(char*)dest+1)-1) =
                   *((char*)(src=(char*)src+1)-1) ) != (char)val ) { n--; }
    return ( n ? dest : NULL );
#endif
    }

void * __cdecl memcpy (void *dest, const void *src, size_t n)
    {
#if defined(SYM_VXD)
    _asm
        {
//        push    esi
//        push    edi
        mov     eax,n
        mov     esi,src
        mov     edi,dest
        mov     ecx,eax
        cld
        test    esi,3
        jnz     unalignedcpybyte
        test    edi,3
        jnz     unalignedcpybyte
        shr     ecx,2
        jz      cpybyte
        rep     movsd
  cpybyte:
        and     eax,3
        jz      cpyno
        mov     ecx,eax
  unalignedcpybyte:
        rep     movsb
  cpyno:
//        pop     edi
//        pop     esi
        }
#elif defined(SYM_NTK)
    RtlMoveMemory ( dest, src, n );
#else
    while ( n-- )
        {
        *(char*)dest=*(char*)src;
        dest = (char*)dest + 1;
        src = (char*)src + 1;
        }
#endif
    return dest;
    }

int __cdecl memcmp (const void *buf1, const void *buf2, size_t n)
    {
#if defined(SYM_VXD)
    auto int iLastBuf1, iLastBuf2;
    _asm
        {
//        push    esi
//        push    edi
        mov     eax,n
        mov     esi,buf1
        mov     edi,buf2
        mov     ecx,eax
        jecxz   memcmpno
        cld
        repe    cmpsb
        movzx   eax,BYTE PTR [esi - 1]
        movzx   ecx,BYTE PTR [edi - 1]
  memcmpno:
        mov     iLastBuf1,eax
        mov     iLastBuf2,ecx
//        pop     edi
//        pop     esi
        }
    return (iLastBuf1 - iLastBuf2);
#else
    register int i = 0;
    while ( n-- && !(i = *(unsigned char*)buf1 - *(unsigned char*)buf2) )
        { buf1 = (char*)buf1 + 1;buf2 = (char*)buf2 + 1; }
    return i;
#endif
    }

// memove is an intrinsic on the alpha by default
#ifndef _ALPHA_
void * __cdecl memmove (void *dest, const void *src, size_t n)
    {
#if defined(SYM_VXD)
    _asm
        {
//        push    esi
//        push    edi
        mov     eax,n
        mov     esi,src
        mov     edi,dest
        mov     ecx,eax
        test    esi,3
        jnz     unalignedmemmove
        test    edi,3
        jnz     unalignedmemmove
        cmp     esi,edi
        jb      movereverse
        cld
        shr     ecx,2
        jz      movfbyte
        rep     movsd
  movfbyte:
        and     eax,3
        jz      moveend
        mov     ecx,eax
        rep     movsb
        jmp     moveend
  movereverse:
        lea     esi,[esi + eax - 1]
        lea     edi,[edi + eax - 1]
        std
        and     ecx,3
        jz      movrdword
        rep     movsb
  movrdword:
        shr     eax,2
        jz      movrno
        sub     esi,3
        sub     edi,3
        mov     ecx,eax
        rep     movsd
  movrno:
        cld
        jmp     moveend
  unalignedmemmove:
        jecxz   moveend
        cmp     esi,edi
        jb      unalignedmovereverse
        cld
        rep     movsb
        jmp     moveend
  unalignedmovereverse:
        lea     esi,[esi + eax - 1]
        lea     edi,[edi + eax - 1]
        std
        rep     movsb
        cld
  moveend:
//        pop     edi
//        pop     esi
        }
#elif defined(SYM_NTK)
    RtlMoveMemory ( dest, src, n );
#else
#error Not implemented
#endif
    return dest;
    }
#endif

void * __cdecl memset (void *s, int c, size_t n)
    {
#if defined(SYM_VXD)
    _asm
        {
//        push    edi
        cld
        mov     ecx,n
        jecxz   memsetend
        mov     eax,c
        mov     edi,s
        rep     stosb
  memsetend:
//        pop     edi
        }
#elif defined(SYM_NTK)
    RtlFillMemory ( s, n, (char)c );
#else
    register char* d = (char*)s;
    while ( n-- ) *d++ = (char)c;
#endif
    return s;
    }

#endif //#if defined(RTL32_NOW_COMPILING_PASS1)

gen_char * __cdecl
#if defined(RTL32_NOW_COMPILING_PASS1)
strncpy
#else
wcsncpy
#endif
(gen_char *dest, const gen_char *src, size_t n)
    {
#if defined(SYM_VXD) && defined(RTL32_NOW_COMPILING_PASS1)
    _asm
        {
//        push    esi
//        push    edi
        mov     ecx,n
        jecxz   strncpyend
        sub     eax,eax
        mov     esi,src
        mov     edi,dest
        cld
  strncpymore:
        lodsb
        stosb
        test    eax,eax
        loopnz  strncpymore
        jecxz   strncpyend
        rep     stosb
  strncpyend:
//        pop     edi
//        pop     esi
        }
#else
    register gen_char *dst = dest;
    while ( n && (*dst++ = *src++) ) n--;
    if ( n ) while ( --n ) *dst++ = '\0';
#endif
    return dest;
    }

gen_char * __cdecl
#if defined(RTL32_NOW_COMPILING_PASS1)
strcpy
#else
wcscpy
#endif
(gen_char *dest, const gen_char *src)
    {
#if defined(SYM_VXD)
    _asm
        {
//        push    esi
//        push    edi
        sub     eax,eax
        mov     esi,src
        mov     edi,dest
        cld
  strcpymore:
#if defined(RTL32_NOW_COMPILING_PASS1)
        lodsb
        stosb
#else
        lodsw
        stosw
#endif
        test    eax,eax
        jnz     strcpymore
//        pop     edi
//        pop     esi
        }
#else
    register gen_char *dst = dest;
    while ( *dst++ = *src++ );
#endif
    return dest;
    }

int __cdecl
#if defined(RTL32_NOW_COMPILING_PASS1)
strcmp
#else
wcscmp
#endif
(const gen_char *str1, const gen_char *str2)
    {
#if defined(SYM_VXD) && defined(RTL32_NOW_COMPILING_PASS1)
    auto int iLastChar1, iLastChar2;
    _asm
        {
//        push    esi
//        push    edi
        sub     eax,eax
        mov     edi,str2
        mov     ecx,-1
        cld
        repne   scasb
        not     ecx
        mov     esi,str1
        mov     edi,str2
        repe    cmpsb
        movzx   eax,BYTE PTR [esi - 1]
        movzx   ecx,BYTE PTR [edi - 1]
        mov     iLastChar1,eax
        mov     iLastChar2,ecx
//        pop     edi
//        pop     esi
        }
    return (iLastChar1 - iLastChar2);
#else
    auto int i;
    while ( !(i = *str1++ - *str2) && *str2++ );
    return i;
#endif
    }

size_t __cdecl
#if defined(RTL32_NOW_COMPILING_PASS1)
strlen
#else
wcslen
#endif
(const gen_char *s)
    {
    auto size_t len;
#if defined(SYM_VXD)
    _asm
        {
//        push    edi
        cld
        mov     edi,s
        sub     eax,eax
        mov     ecx,-1
#if defined(RTL32_NOW_COMPILING_PASS1)
        repne   scasb
#else
        repne   scasw
#endif
        not     ecx
        dec     ecx
        mov     len,ecx
//        pop     edi
        }
#else
    for ( len = 0; *s++; len++ );
#endif
    return len;
    }

gen_char * __cdecl
#if defined(RTL32_NOW_COMPILING_PASS1)
strcat
#else
wcscat
#endif
(gen_char *dest, const gen_char *src)
    {
#if defined(SYM_VXD) && defined(RTL32_NOW_COMPILING_PASS1)
    _asm
        {
//        push    esi
//        push    edi
        cld
        mov     esi,src
        mov     edi,dest
        sub     eax,eax
        mov     ecx,-1
        repne   scasb
        dec     edi
  strcatmore:
        lodsb
        stosb
        test    eax,eax
        jnz     strcatmore
//        pop     edi
//        pop     esi
        }
#else
    register gen_char *append = dest;
    while ( *append ) append++;
    while ( *append++ = *src++ );
#endif
    return dest;
    }

gen_char * __cdecl
#if defined(RTL32_NOW_COMPILING_PASS1)
_strrev
#else
_wcsrev
#endif
(gen_char *s)
    {
    auto gen_char *pcFirst, *pcLast, *pcStop;
    auto UINT     uLength;
    auto gen_char cChar;

    for ( uLength = 0; s[uLength]; uLength++ );

    pcFirst = s;
    pcLast  = &s[uLength - 1];
    pcStop  = &pcFirst[uLength / 2];

    while ( pcFirst != pcStop )
        {
        cChar    = *pcFirst;
        *pcFirst = *pcLast;
        *pcLast  = cChar;
        pcFirst++;
        pcLast--;
        }

    return s;
    }

gen_char * __cdecl
#if defined(RTL32_NOW_COMPILING_PASS1)
strchr
#else
wcschr
#endif
(const gen_char *s, gen_intchar c)
    {
#if defined(SYM_VXD) && defined(RTL32_NOW_COMPILING_PASS1)
    auto gen_char *result;
    _asm
        {
//        push    esi
        cld
        sub     eax,eax
        mov     ecx,eax
        movzx   edx,BYTE PTR c
        mov     esi,s
  strchrmore:
        lodsb
        cmp     eax,edx
        je      strchrfound
        cmp     eax,ecx
        jne     strchrmore
        mov     esi,1
  strchrfound:
        dec     esi
        mov     result,esi
//        pop     esi
        }
    return result;
#else
    do
        {
        if ( *s == (gen_char)c ) return (gen_char*) s;
        } while ( *s++ );
    return NULL;
#endif
    }

gen_char * __cdecl
#if defined(RTL32_NOW_COMPILING_PASS1)
strrchr
#else
wcsrchr
#endif
(const gen_char *s, gen_intchar c)
    {
    register gen_char *lpszStr = (gen_char *)s;

    while ( *lpszStr ) lpszStr++;

    while (lpszStr > s && *lpszStr != (gen_char) c)
        lpszStr--;

    if (*lpszStr != (gen_char) c)
        lpszStr = NULL;
    return lpszStr;
    }

int __cdecl
#if defined(RTL32_NOW_COMPILING_PASS1)
strncmp
#else
wcsncmp
#endif
(const gen_char *str1, const gen_char *str2, size_t n)
    {
    if (n == 0)
        return(0);

    while (--n && *str1 && (*str1 == *str2))
        {
        str1++;
        str2++;
        }
    return(*str1 - *str2);
    }

#if !defined(SYM_VXD) || defined(RTL32_NOW_COMPILING_PASS1)

int __cdecl
#if defined(RTL32_NOW_COMPILING_PASS1)
_stricmp
#else
_wcsicmp
#endif
(const gen_char *str1, const gen_char *str2)
    {
    while (*str1 && (gen_toupper(*str1) == gen_toupper(*str2)))
        {
        str1++;
        str2++;
        }
    return(gen_toupper(*str1) - gen_toupper(*str2));
    }

int __cdecl
#if defined(RTL32_NOW_COMPILING_PASS1)
_strnicmp
#else
_wcsnicmp
#endif
(const gen_char *str1, const gen_char *str2, size_t n)
    {
    if (n == 0)
        return(0);

    while (--n && *str1 && (gen_toupper(*str1) == gen_toupper(*str2)))
        {
        str1++;
        str2++;
        }
    return(gen_toupper(*str1) - gen_toupper(*str2));
    }

#endif

#if defined(RTL32_NOW_COMPILING_PASS1)

size_t __cdecl strcspn ( const char * string, const char * control )
    {
    const unsigned char *str = (unsigned char *)string;
    const unsigned char *ctrl = (unsigned char *)control;

    unsigned char map[32];
    int count;

    for (count=0; count<32; count++)
        {
        map[count] = 0;
        }

    while (*ctrl)
        {
        map[*ctrl >> 3] |= (1 << (*ctrl & 7));
        ctrl++;
        }

    count=0;

    map[0] |= 1;

    while (!(map[*str >> 3] & (1 << (*str & 7))))
        {
        count++;
        str++;
        }

    return(count);
    }

#else

size_t __cdecl wcscspn ( const wchar_t * string, const wchar_t * control )
    {
    wchar_t *str = (wchar_t *) string;
    wchar_t *wcset;

    while (*str)
        {
        for (wcset = (wchar_t *)control; *wcset; wcset++)
            {
            if (*wcset == *str)
                {
                return str - string;
                }
            }

        str++;
        }

    return str - string;
    }

#endif

gen_char * __cdecl
#if defined(RTL32_NOW_COMPILING_PASS1)
strpbrk
#else
wcspbrk
#endif
(const gen_char *s1, const gen_char *s2)
    {
    register gen_char *lpszChars;

    while (*s1)
        {
        lpszChars = (gen_char*) s2;
        while (*lpszChars)
            {
            if (*s1 == *lpszChars)
                return((gen_char*) s1);
            lpszChars++;
            }
        s1++;
        }
    return(NULL);
    }

gen_char * __cdecl
#if defined(RTL32_NOW_COMPILING_PASS1)
strstr
#else
wcsstr
#endif
(const gen_char *s1, const gen_char *s2)
    {
    auto const gen_char *result;
    auto size_t s1len, s2len;

    for ( result = s1, s1len = 0; *result++; s1len++ );
    for ( result = s2, s2len = 0; *result++; s2len++ );

    if ( s2len != 0 )
        {
        result = NULL;

        while ( (result == NULL) && (s1len >= s2len) )
            {
            if ( memcmp(s1,s2,s2len * sizeof(gen_char) ) == 0 )
                {
                result = s1;
                }
            else
                {
                s1++;
                s1len--;
                }
            }
        }
    else
        {
        result = s1;
        }

    return (gen_char*)result;
    }

#if defined(RTL32_NOW_COMPILING_PASS1) && defined(SYM_VXD)
int __cdecl toupper ( int ch )
    {
    auto char szTempString[2];

    szTempString[0] = ch;
    szTempString[1] = '\0';
    vxd_strupr ( szTempString, BCS_OEM );

    return ( szTempString[0] );
    }
#elif defined(RTL32_NOW_COMPILING_PASS2) && defined(SYM_NTK)
wchar_t __cdecl towupper ( wchar_t ch )
    {
    return RtlUpcaseUnicodeChar ( ch );
    }
#endif

#if defined(RTL32_NOW_COMPILING_PASS1) && defined(SYM_VXD)
int __cdecl tolower ( int ch )
    {
    return ch;
    }
#elif defined(RTL32_NOW_COMPILING_PASS2) && defined(SYM_NTK)
wchar_t __cdecl towlower ( wchar_t ch )
    {
    auto UNICODE_STRING src, dst;
    auto WCHAR          result;

    src.Length = src.MaximumLength = dst.MaximumLength = sizeof(WCHAR);
    src.Buffer = &ch;
    dst.Buffer = &result;
    if ( NT_SUCCESS ( RtlDowncaseUnicodeString ( &dst, &src, FALSE ) ) )
        {
        return result;
        }

    SYM_ASSERT ( FALSE );
    return ch;
    }
#endif

#if defined(RTL32_NOW_COMPILING_PASS1) && defined(SYM_VXD)
char * __cdecl _strupr ( char *str )
    {
    vxd_strupr ( str, BCS_OEM );
    return str;
    }
#elif defined(RTL32_NOW_COMPILING_PASS2) && defined(SYM_NTK)
wchar_t * __cdecl _wcsupr ( wchar_t *str )
    {
    auto UNICODE_STRING src, dst;

    RtlInitUnicodeString ( &src, str );
    dst = src;
    if ( !NT_SUCCESS ( RtlUpcaseUnicodeString ( &dst, &src, FALSE ) ) )
        {
        SYM_ASSERT ( FALSE );
        }
    return str;
    }
#endif

#if defined(RTL32_NOW_COMPILING_PASS1) && defined(SYM_VXD)
char * __cdecl _strlwr ( char *str )
    {
    return str;
    }
#elif defined(RTL32_NOW_COMPILING_PASS2) && defined(SYM_NTK)
wchar_t * __cdecl _wcslwr ( wchar_t *str )
    {
    auto UNICODE_STRING src, dst;

    RtlInitUnicodeString ( &src, str );
    dst = src;
    if ( !NT_SUCCESS ( RtlDowncaseUnicodeString ( &dst, &src, FALSE ) ) )
        {
        SYM_ASSERT ( FALSE );
        }
    return str;
    }
#endif

//---------------------------------------------------------------------------
//
// static void __cdecl vxd_strupr ( char *str, int charset );
//
// Converts the string pointed to by str to upper case.  This function
// supports both ANSI and OEM string conversion.  The source character set
// is specified by charset (BCS_WANSI, BCS_OEM.)
//
//---------------------------------------------------------------------------
#if defined(RTL32_NOW_COMPILING_PASS1) && defined(SYM_VXD)
static void __cdecl vxd_strupr ( char *str, int charset )
    {
    auto size_t         uStringLength, uNewLength;
    auto unsigned short *pszUnicode;

    if ( uStringLength = strlen(str) )
        {
        if ( pszUnicode = (unsigned short *)
                            MemAllocPtr ( GHND,
                                          uStringLength * sizeof(*pszUnicode) )
           )
            {
            uNewLength = BCSToUni ( pszUnicode,
                                    (unsigned char *)str,
                                    uStringLength,
                                    charset );

            // I am not sure if it is possible a non-empty MBCS string to be
            // converted to an empty Unicode string.  If this is possible,
            // remove the SYM_ASSERT() below.  Read further for comments on
            // similar checks in this function.

            SYM_ASSERT ( uNewLength );

            if ( uNewLength )
                {
                UniToUpper ( pszUnicode, pszUnicode, uNewLength );

                // Note, that the buffer for the MBCS string is passed as
                // the full size Unicode string.  This may overwrite some of
                // client's buffers, but they should have anticipated such
                // case!  Besides, I am not sure if the MBCS<->Unicode
                // conversions used here can generate such case.

                uNewLength = UniToBCS ( (unsigned char *)str,
                                        pszUnicode,
                                        uNewLength,
                                        uNewLength,
                                        charset );

                // This assert will fail if we had a unicode string with some
                // characters in it, and after its conversion to MBCS it either
                // returned an empty string, or the length in bytes of the
                // produced string did not match the length in bytes of the
                // source string.  While I am pretty sure the first should not
                // happen, I don't know if the second case can legally happen
                // (uppercasing a non-DBCS character to produce a DBCS character
                // and vice versa).  If the second case is legal, then the assert
                // should only read SYM_ASSERT ( uNewLength );

                SYM_ASSERT ( uNewLength && uNewLength == uStringLength );

                // This is a precaution in case the new string is larger than
                // the source string.

                str[uNewLength] = '\0';
                }

            MemFreePtr ( pszUnicode );
            }
        }
    }
#endif

#endif
