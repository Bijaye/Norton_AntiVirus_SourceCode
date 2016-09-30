/* Copyright 1993-1996 Symantec Corporation                             */
/************************************************************************
 *                                                                      *
 * $Header:   S:/SYMKRNL/VCS/DBCS.C_v   1.9   23 Apr 1998 11:13:16   TSmith  $ *
 *                                                                      *
 * Description:                                                         *
 *      DBCS functions                                                  *
 *                                                                      *
 * Contains:                                                            *
 *      SYMstrchr                                                       *
 *      SYMstrrchr                                                      *
 *      SYMstrpbrk                                                      *
 *      SYMstrrev                                                       *
 *                                                                      *
 *      DBCSInstalled                                                   *
 *      DBCSIsLeadByte                                                  *
 *      DBCSIsTrailByte                                                 *
 *      DBCSGetByteType                                                 *
 *                                                                      *
 *      DBCS_SearchLastChar                                             *
 *      DBCS_SearchFirstChar                                            *
 *      DBCS_ReplaceAnd                                                 *
 *                                                                      *
 *      AnsiNext                                                        *
 *      AnsiPrev                                                        *
 *                                                                      *
 *      DBCSGetFontFace                                                 *
 *      DBCSGetFontSize                                                 *
 *      DBCSFreeFontFaceMemory                                          *
 *                                                                      *
 *              // Local to SYMKRNL                                     *
 *      DBCSstrchr                                                      *
 *      DBCSstrrchr                                                     *
 *      DBCSstrpbrk                                                     *
 *      DBCSstrrev                                                      *
 *      DBCSstrncpy                                                     *
 *      DBCSstricmp                                                     *
 *      DBCSstrnicmp                                                    *
 *      DBCSstrstr                                                      *
 *                                                                      *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * $Log:   S:/SYMKRNL/VCS/DBCS.C_v  $ */
// 
//    Rev 1.9   23 Apr 1998 11:13:16   TSmith
// Fixed 111689, 111783, others - Removed code to retrieve DBCS font names from
// string table. The names can be/are MBCS strings and the string table defaults
// to U.S. English. This caused problems when retrieving the strings via the
// LoadString() API. The font names are now just hard coded as before.
// 
//    Rev 1.8   18 Nov 1997 16:12:14   TSmith
// Changed DBCS_FONTFACE_BUF_SIZE from 64 to LF_FACESIZE (32) for compatibility
// with the lfFaceName field of the LOGFONT structure.
// 
// 
//    Rev 1.7   07 Nov 1997 10:02:12   TSmith
// Added several functions to facilitate retrieving DBCS font names and sizes
// from string resources. This replaces the technique of specifying this
// information with hard coded values. Localization teams can now alter the font
// face and size by editing the string table and should require no core development
// team effort. Functions added: 'DBCSLoadFontFaceFromResource', 'DBCSGetFontFace',
// 'DBCSGetFontSize' and 'DBCSFreeFontFaceMemory'.
// 
//    Rev 1.6   19 Jun 1997 17:07:28   RStanev
// Merged Kirin's DBCS changes for SYM_VXD.  Thanks, Davros!
// 
//    Rev 1.5   16 Oct 1996 19:27:32   DALLEE
// Added routines for strncpy, stricmp, strnicmp, and strstr.
// 
//    Rev 1.4   23 Sep 1996 10:32:38   RCHINTA
// Fixed a mistake in DBCSSetInternalTable().
// 
//    Rev 1.3   23 Sep 1996 09:54:48   RCHINTA
// Merged changes from Quake 9.  Changed DBCSSetInternalTable()
// to include trail bytes tables for Chinese and Korean Code
// Pages.
// 
//    Rev 1.2   20 Sep 1996 22:06:00   DHERTEL
// Use these definitions of AnsiPrev and AnsiNext on all DOS platforms, 
// not just DBCS DOS platforms, because that's how platform.h is deciding
// whether or not to define AnsiPrev and AnsiNext macros.  (Maybe this should
// probably be changed back to give non-DBCS programs a speed boost, but
// platform.h is not accessible right now.)
// 
//    Rev 1.1   20 Sep 1996 21:23:56   DHERTEL
// Added DBCS changed from Quake C.  The first time table1 is used it is
// automatically initialized.  Added DBCS implementations of AnsiPrev, AnsiNext,
// AnsiUpper and AnsiLower.  The assembly, non-DBCS versions of AnsiPrev and
// AnsiNext are in prm_lstr.asm.
// 
//    Rev 1.0   26 Jan 1996 20:21:22   JREARDON
// Initial revision.
// 
//    Rev 1.19   04 Nov 1995 00:17:48   DBUCHES
// Fixed problem in DBCSSetInternalTable() when called on non-DBCS machines.
// This function was erroneously reporting that the machine in question
// supported DBCS.
//
//    Rev 1.18   03 Nov 1995 13:23:06   DBUCHES
// Fixed GP for DX platform in DBCSSetInternalTable().
//
//    Rev 1.17   02 Aug 1995 00:43:24   AWELCH
// Merge changes from Quake 7 branch.
//
//    Rev 1.16   20 Apr 1995 15:41:26   LindaD
// Added IsKanjiSec routine for DBCS
//
//    Rev 1.15   14 Dec 1994 16:47:26   BRAD
// Init variables, since VXD doesn't init to 0
//
//    Rev 1.14   14 Dec 1994 16:45:22   BRAD
// Init variables, since VXD doesn't init to 0
//
//    Rev 1.13   09 Dec 1994 19:05:24   BRAD
// Cleaned up for SYM_VXD.
//
//    Rev 1.12   16 Nov 1994 07:49:06   JMILLARD
// fix temporarily to build for NLM platform - will need to revisit to
// find the permanent solution
//
//    Rev 1.11   08 Nov 1994 17:53:18   BRAD
// Some headers no longer exist
//
//    Rev 1.10   08 Nov 1994 17:48:38   BRAD
// Moved all routines here
//
//    Rev 1.9   08 Nov 1994 17:22:38   BRAD
// Moved string functions to DBCS_STR.C
//
//    Rev 1.8   04 Nov 1994 15:08:56   BRAD
// Renamed DBCS_CodeCheck() to DBCSGetByteType()
//
//    Rev 1.7   21 Oct 1994 11:58:34   BRAD
// New routines
// Initial revision.

#include <dos.h>
#include "platform.h"
#include "xapi.h"
#include "dbcs.h"
#include "dbcs_str.h"
#include "codepage.h"
#include "symkrnli.h"
EXTERN WORD CDECL DOSGetCodePage(VOID);


///////////////////////////////////////////////////////////////////////////
// These are the tables of valid first bytes (dbcsTable1) and
// second bytes (dbcsTable2) of Double byte characters.
// Positions with 2's are valid first bytes. To get the next byte of
// a string ( say lpStr ) all we need to do is
//
// lpStr += dbcsTable1[*lpStr] ;
//

char dbcsTable1 [256] = { SINGLE_BYTE };
char dbcsTable2 [256] = { SINGLE_BYTE };

BOOL    gbDBCSInstalled = FALSE;                // Is DBCS installed?
STATIC BOOL gbDBCSInitialized = FALSE;          // Have we tried to initialize?

/*--------------------------------------------------------------------------*/
// LOCAL MACROS
/*--------------------------------------------------------------------------*/

// CONVERT_CASE() used by STRIxCMP functions.  Conversion to lowercase is
// more reliable, but isn't available for VXD.
#if defined(SYM_VXD)
  #define CONVERT_CASE(x)   CharToUpper(x)
#else
  #define CONVERT_CASE(x)   CharToLower(x)
#endif


/*--------------------------------------------------------------------------*/
// LOCAL PROTOTYPES
/*--------------------------------------------------------------------------*/
LPSTR LOCAL PASCAL DBCSstrchr(LPCSTR lpStr, int chSrch);
LPSTR LOCAL PASCAL DBCSstrrchr(LPCSTR lpStr, int chSrch);
LPSTR LOCAL PASCAL DBCSstrrev (LPSTR lpStr);
LPSTR LOCAL PASCAL DBCSstrpbrk(LPCSTR lpStr, LPCSTR lpChrs);
LPSTR LOCAL PASCAL DBCSstrncpy(LPSTR lpszDest, LPCSTR lpszSrc, size_t uCount);
int   LOCAL PASCAL DBCSstricmp(LPCSTR lpszStr1, LPCSTR lpszStr2);
int   LOCAL PASCAL DBCSstrnicmp(LPCSTR lpszStr1, LPCSTR lpszStr2, size_t uCount);
LPSTR LOCAL PASCAL DBCSstrstr(LPCSTR lpszStr1, LPCSTR lpszStr2);


/*--------------------------------------------------------------------------*/
// Wrapper for VMM's Test_DBCS_Lead_Byte
/*--------------------------------------------------------------------------*/

#if defined(SYM_VXD)
BOOL VXDINLINE  VxDIsDBCSLeadByte ( BYTE byTestChar )
    {
    _asm movzx eax, byte ptr byTestChar
    VMMCall(Test_DBCS_Lead_Byte)
    _asm jc NotLead

    return (TRUE);
NotLead:
    return (FALSE);
    }
#endif


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
BOOL SYM_EXPORT WINAPI DBCSInstalled (VOID)
{
    return(gbDBCSInstalled);
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
VOID SYM_EXPORT WINAPI DBCSSetInternalTable(VOID)
{
#if defined(SYM_WIN)
    auto        UINT            i;
    auto        UINT            nCodePage;

    if (gbDBCSInitialized)
        return;
    gbDBCSInitialized = TRUE;
                                        // Initialize entire lead and
                                        // trail byte tables to 1,
                                        // meaning NO DBCS

    MEMSET(&dbcsTable1, SINGLE_BYTE, sizeof(dbcsTable1));
    MEMSET(&dbcsTable2, SINGLE_BYTE, sizeof(dbcsTable2));
    gbDBCSInstalled = FALSE;

                                        // Fill in lead byte Table

    for (i = 0; i <= 255; i++)
        {
        if (IsDBCSLeadByte((BYTE) i))
            {
            dbcsTable1[i]++;
            gbDBCSInstalled = TRUE;
            }
        }
                                        // If DBCS, fill in trail byte
                                        // table
     
    if (gbDBCSInstalled)
        {

#if defined(SYM_WIN32)
     nCodePage = GetACP();
#else
     nCodePage = GetKBCodePage();
#endif

        switch ( nCodePage )
            {
            case CODEPAGE_JAPAN:
                {
                for (i = 0x40; i <= 0x7e; i++)
                    dbcsTable2[i] = DOUBLE_BYTE;
                for (i = 0x80; i <= 0xfc; i++)
                    dbcsTable2[i] = DOUBLE_BYTE;
                break;
                }
            case CODEPAGE_CHINESE_TRADITIONAL:
                {
                for (i = 0x40; i <= 0x7e; i++)
                    dbcsTable2[i] = DOUBLE_BYTE;
                for (i = 0xa1; i <= 0xfe; i++)
                    dbcsTable2[i] = DOUBLE_BYTE;
                break;
                }
            case CODEPAGE_CHINESE_SIMPLIFIED:
                {
                for (i = 0xa1; i <= 0xfe; i++)
                    dbcsTable2[i] = DOUBLE_BYTE;
                break;
                }
            case CODEPAGE_KOREAN_WANSUNG:
                {
                for (i = 0x41; i <= 0x5a; i++)
                    dbcsTable2[i] = DOUBLE_BYTE;
                for (i = 0x61; i <= 0x7a; i++)
                    dbcsTable2[i] = DOUBLE_BYTE;
                for (i = 0x81; i <= 0xfe; i++)
                    dbcsTable2[i] = DOUBLE_BYTE;
                break;
                }
            case CODEPAGE_KOREAN_JOHAB:
                {
                for (i = 0x41; i <= 0x5a; i++)
                    dbcsTable2[i] = DOUBLE_BYTE;
                for (i = 0x61; i <= 0x7a; i++)
                    dbcsTable2[i] = DOUBLE_BYTE;
                for (i = 0x81; i <= 0xfe; i++)
                    dbcsTable2[i] = DOUBLE_BYTE;
                break;
                }
             }
        }

#elif defined(SYM_DOS)
                                        // For DOS, we need to get the
                                        // Lead Byte table.
  #define FCT_POINTER_SIZE    5

    auto        LPWORD          lpDBCSTable;
    auto        UINT            wResult = 0;
    auto        BYTE            byStartRange, byEndRange;
    auto        UINT            uRanges;
    auto        UINT            i, j;
    auto        UINT            nCodePage;

  #if defined(SYM_PROTMODE)

    auto union  REGS    regs;
    auto struct SREGS   sRegs;
    auto    BYTE    FAR *lpTable;
    auto    DWORD   dwSegSel;

    if (gbDBCSInitialized)
        return;
    gbDBCSInitialized = TRUE;

    dwSegSel = GlobalDosAlloc(FCT_POINTER_SIZE);

    lpTable = MAKELP(LOWORD(dwSegSel), 0);

    MEMSET(lpTable, 0, FCT_POINTER_SIZE);

    sRegs.es  = HIWORD(dwSegSel);
    regs.x.di = 0x0000;
    regs.x.ax = 0x6507;
    regs.x.bx = 0xFFFF;
    regs.x.dx = 0xFFFF;
    regs.x.cx = FCT_POINTER_SIZE;
    Int(0x21, &regs, &sRegs);

    wResult = regs.x.cflag;

    lpDBCSTable =  GetProtModePtr(*((LPWORD FAR *) (lpTable + 1)));

    GlobalDosFree(LOWORD(dwSegSel));

  #else
    auto        BYTE            fctBuffer[FCT_POINTER_SIZE];

    if (gbDBCSInitialized)
        return;
    gbDBCSInitialized = TRUE;

    MEMSET(fctBuffer, 0, FCT_POINTER_SIZE);

    _asm
        {
        mov     bx, -1                  ; use current code page
        mov     dx, -1                  ; use current country code

        lea     di,word ptr fctBuffer   ; return buffer
        mov     ax,ss
        mov     es,ax
        mov     cx,5                    ; size of return buffer

        mov     ax,6507h                ; get the filename table
        int     21h
        rcl     wResult,1               ; save the carry flag

        mov     ax, word ptr fctBuffer + 1
        mov     dx, word ptr fctBuffer + 3
        mov     word ptr lpDBCSTable, ax
        mov     word ptr lpDBCSTable + 2, dx
        }
  #endif
                                        // Initialize lead and trail
                                        // byte tables to 1,
                                        // meaning NO DBCS

    MEMSET(&dbcsTable1, SINGLE_BYTE, sizeof(dbcsTable1));
    MEMSET(&dbcsTable2, SINGLE_BYTE, sizeof(dbcsTable2));
    gbDBCSInstalled = FALSE;

    nCodePage = DOSGetCodePage();       // Get current code page

    // If not on American or European code page, return.  The code
    // here only seems to work properly on double-byte code pages.
    // For now, we only check for Japan, China, and Korea, but we will have to
    // add checks for other countries when we ship to them.  We need not check
    // for CODEPAGE_KOREAN_JOHAB since it is only an ANSI Code Page.
                                        
    if ( (nCodePage != CODEPAGE_JAPAN) &&
         (nCodePage != CODEPAGE_CHINESE_TRADITIONAL) &&
         (nCodePage != CODEPAGE_CHINESE_SIMPLIFIED) &&
         (nCodePage != CODEPAGE_KOREAN_WANSUNG)
         )
        return;
                                        // First WORD is size of entire
                                        // table.  If 0, then no DBCS Table
    if (!wResult && (*lpDBCSTable != 0))
        {
        uRanges = (*lpDBCSTable++ - sizeof(WORD)) / (2 * sizeof(BYTE));
        for (i = 0; i < uRanges; i++, lpDBCSTable++)
            {
            byStartRange = *((LPBYTE) lpDBCSTable);
            byEndRange   = *(((LPBYTE) lpDBCSTable + 1));
            for (j = byStartRange; j <= byEndRange; j++)
                {
                dbcsTable1[j] = DOUBLE_BYTE;   // Just in case overlapping ranges!
                }
            gbDBCSInstalled = TRUE;            // Only set if there is a range
            }

        }

  #ifdef SYM_PROTMODE
    SelectorFree(HIWORD(lpDBCSTable));
  #endif

                                                // Fill in trail byte table
    if (gbDBCSInstalled)
        {
        switch ( nCodePage )
            {
            case CODEPAGE_JAPAN:
                {
                for (i = 0x40; i <= 0x7e; i++)
                    dbcsTable2[i] = DOUBLE_BYTE;
                for (i = 0x80; i <= 0xfc; i++)
                    dbcsTable2[i] = DOUBLE_BYTE;
                break;
                }
            case CODEPAGE_CHINESE_TRADITIONAL:
                {
                for (i = 0x40; i <= 0x7e; i++)
                    dbcsTable2[i] = DOUBLE_BYTE;
                for (i = 0xa1; i <= 0xfe; i++)
                    dbcsTable2[i] = DOUBLE_BYTE;
                break;
                }
            case CODEPAGE_CHINESE_SIMPLIFIED:
                {
                for (i = 0xa1; i <= 0xfe; i++)
                    dbcsTable2[i] = DOUBLE_BYTE;
                break;
                }
            case CODEPAGE_KOREAN_WANSUNG:
                {
                for (i = 0x41; i <= 0x5a; i++)
                    dbcsTable2[i] = DOUBLE_BYTE;
                for (i = 0x61; i <= 0x7a; i++)
                    dbcsTable2[i] = DOUBLE_BYTE;
                for (i = 0x81; i <= 0xfe; i++)
                    dbcsTable2[i] = DOUBLE_BYTE;
                break;
                }
            }
        }

#elif defined(SYM_VXD)
    auto        UINT            i;

    if (gbDBCSInitialized)
        return;
    gbDBCSInitialized = TRUE;

                                        // Initialize entire lead and
                                        // trail byte tables to 1,
                                        // meaning NO DBCS

    MEMSET(&dbcsTable1, SINGLE_BYTE, sizeof(dbcsTable1));
    MEMSET(&dbcsTable2, SINGLE_BYTE, sizeof(dbcsTable2));
    gbDBCSInstalled = FALSE;
                                        // Fill in Table
    for (i = 0; i <= 255; i++)
        {
        if (VxDIsDBCSLeadByte((BYTE) i))
            {
            dbcsTable1[i]++;
            gbDBCSInstalled = TRUE;
            }
        }

    // Trail byte table not filled in yet for VXD.
    // Please update comment in DBCSIsTrailByte() if this is implemented.

#endif

}


///////////////////////////////////////////////////////////////////////////
// Is a charcter the first byte of a dbcs character
//
BOOL SYM_EXPORT WINAPI DBCSIsLeadByte(BYTE byCh)
{
    if (!gbDBCSInitialized)
        DBCSSetInternalTable();
    return ( dbcsTable1 [byCh] == DOUBLE_BYTE ) ;
}

///////////////////////////////////////////////////////////////////////////
// Is a charcter a valid second byte value for a dbcs character?
//
// WARNING: Trail byte table is not initialized for VxD platform.
//
BOOL SYM_EXPORT WINAPI DBCSIsTrailByte(BYTE byCh)
{
    return ( dbcsTable2 [byCh] == DOUBLE_BYTE ) ;
}

///////////////////////////////////////////////////////////////////////////
// Is a charcter a kana charachter
//
int SYM_EXPORT WINAPI DBCS_iskana( char c )
{
    return (unsigned char)(c) >= 0xA1 && (unsigned char)(c) <= 0xDF ;
}

///////////////////////////////////////////////////////////////////////////
// Check a character in a string to see if its a single byte char or
// the 1st or second byte of a double byte character
//
// Returns: _MBC_ILLEGAL(-1)    : error
//      _MBC_SINGLE(0)      : a single byte character
//      _MBC_LEAD(1)        : The first byte of a double byte char
//      _MBC_TRAIL(2)       : The second byte of a double byte char
//
// This function is equivalent to _mbsbtype().
//
int SYM_EXPORT WINAPI DBCSGetByteType(LPSTR lpStart, LPSTR lpChk)
{
    LPSTR   lpMove = lpStart;
    int iFlg;


    if (!gbDBCSInitialized)
        DBCSSetInternalTable();

    if (!gbDBCSInstalled)
       return(_MBC_SINGLE);

    if (lpStart > lpChk)
       return (_MBC_ILLEGAL);

    if (lpStart == lpChk)
       return (dbcsTable1[*(LPBYTE)lpChk] - 1);

    while( lpMove < lpChk )
       lpMove += dbcsTable1[*(LPBYTE)lpMove];

    iFlg = (lpMove == lpChk ? _MBC_SINGLE : _MBC_TRAIL);

    if( iFlg == _MBC_SINGLE && dbcsTable1[*(LPBYTE)lpChk] == DOUBLE_BYTE )
       iFlg = _MBC_LEAD;

    return (iFlg);
}

///////////////////////////////////////////////////////////////////////////
// Search a string between lpStart and lpEnd for the last occurance of
// of the character chChar
//
LPSTR SYM_EXPORT WINAPI DBCS_SearchLastChar( LPSTR lpStart, LPSTR lpEnd, char chChar )
{
    LPSTR   lpMove = lpStart;
    LPSTR   lpBuf = (LPSTR)NULL;
    int     i;

    if (!gbDBCSInitialized)
        DBCSSetInternalTable();

    if( lpEnd == (LPSTR)NULL ) {
        for( i = 0; lpStart[i]; i++ );
        lpEnd = &lpStart[--i];
    }

    while( lpMove < lpEnd ) {
        if( *(BYTE far *)lpMove == (BYTE)chChar )
            lpBuf = lpMove;
        lpMove += dbcsTable1[*(LPBYTE)lpMove] ;
    }

    return lpBuf;
}


///////////////////////////////////////////////////////////////////////////
// Search a string from lpStart for the first occurance of
// of the character chChar
//
LPSTR SYM_EXPORT WINAPI DBCS_SearchFirstChar( LPSTR lpStart, char chChar )
{
    LPSTR   lpMove = lpStart;

    if (!gbDBCSInitialized)
        DBCSSetInternalTable();

    while( *lpMove != '\0' ) {
        if( *(BYTE far *)lpMove == (BYTE)chChar )
            return lpMove;
        lpMove += dbcsTable1[*(LPBYTE)lpMove] ;
    }

    return (LPSTR)NULL;
}

///////////////////////////////////////////////////////////////////////////
// Count the first lead byte and all following bytes within a distance of
// 'cnt' bytes from 'lpStr'.  Calls to IsKanjiSec() should be replaced
// with IsDBCSSec().
//
int SYM_EXPORT WINAPI IsKanjiSec(LPSTR lpStr, int cnt)
{
    return( IsDBCSSec(lpStr, cnt) );
}

int SYM_EXPORT WINAPI IsDBCSSec(LPSTR lpStr, int cnt)
{
    int flag = 0;

    while (cnt > 0) {
        if (flag ==1)
            flag++;
        else if (DBCSIsLeadByte(*lpStr))
            flag = 1;
        else
            flag = 0;
        lpStr++;
        cnt--;
    }

    return(flag);
}


//////////////////////////////////////////////////////////////////////////
// Temporarily replace certain occurences of '&'
//
void SYM_EXPORT WINAPI DBCS_ReplaceAnd ( LPSTR lpStr )
{
#ifdef SYM_NLM

    (void) lpStr;

#else

    LPSTR    lpMove;
    BOOL    iSKey[2] = { FALSE, FALSE };        //[0]:ALF [1]:KANA
    unsigned char    chNext;
return ;

    lpMove = DBCSstrchr( lpStr, '&' );
    while( lpMove && !(iSKey[0] && iSKey[1]) ) {
        chNext = *(lpMove + 1);
        if( ((chNext >= (unsigned char)0x41 && chNext <= (unsigned char)0x5A) ||
            (chNext >= (unsigned char)0x61 && chNext <= (unsigned char)0x7A)) && !iSKey[0] ) {
            *lpMove = '\036';
            iSKey[0] = TRUE;
        }
        else if( chNext >= (unsigned char)0xA1 && chNext <= (unsigned char)0xDF && !iSKey[1] ) {
            *lpMove = '\037';
            iSKey[1] = TRUE;
        }

        lpMove = DBCSstrchr( lpMove+1, '&' );
    }

#endif

}


//////////////////////////////////////////////////////////////////////////
// Restore the temporarily replaced '&' bytes
//

void SYM_EXPORT WINAPI DBCS_PutBackAnd ( LPSTR lpStr )
{
    LPSTR   lpSrch;
    lpSrch = lpStr;
return ;
    while( lpSrch = DBCSstrpbrk( lpSrch, "\036\037" ) )
        *lpSrch = '&';
}

//////////////////////////////////////////////////////////////////////////
// Parses a string and returns TRUE if it contains any Japanese characters
// Calls to IsJapString() should be replaced with the more general
// IsDBCSString();

int SYM_EXPORT WINAPI IsJapString ( LPSTR lpStr )
{
    return( IsDBCSString( lpStr) );
}

int SYM_EXPORT WINAPI IsDBCSString ( LPSTR lpStr )
{
    LPSTR lpPtr = lpStr ;
    auto BOOL IsCodePageJapan;

#if defined(SYM_WIN32)
     IsCodePageJapan = (GetACP() == CODEPAGE_JAPAN);
#elif defined(SYM_WIN)
     IsCodePageJapan = (GetKBCodePage() == CODEPAGE_JAPAN);
#elif defined(SYM_DOS)
     IsCodePageJapan = (DOSGetCodePage() == CODEPAGE_JAPAN);
#endif


    while ( *lpPtr )
    {
        if ( DBCSGetByteType ( lpStr, lpPtr ) == _MBC_LEAD )
            return TRUE ;

        if (IsCodePageJapan)            // Check for characters
           {                            // specific to Japan
           if ( (BYTE)*lpPtr >= (BYTE)143 && (BYTE)*lpPtr <=(BYTE)236 )
              return TRUE ;
           }

         lpPtr ++ ;
    }

    return FALSE ;
}

///////////////////////////////////////////////////////////////////////////
// Compares double by and 2 single byte versions of japanese chars with
// accents. Just the first two bytes of each the string are compared
//
// Look at these in a japanese editor, and it will become
// obvious what they are.

static char szDoubleByte[] = "ƒKƒMƒOƒQƒSƒUƒWƒYƒ[ƒ]ƒ_ƒaƒdƒfƒhƒoƒpƒrƒsƒuƒvƒxƒyƒ{ƒ|" ;
static char szSingleByte[] = "¶Þ·Þ¸Þ¹ÞºÞ»Þ¼Þ½Þ¾Þ¿ÞÀÞÁÞÂÞÃÞÄÞÊÞÊßËÞËßÌÞÌßÍÞÍßÎÞÎß" ;

int SYM_EXPORT WINAPI JapAccentCompare ( LPSTR lpStr1, LPSTR lpStr2 )
{
    int iIndex ;

    for ( iIndex = 0;  szDoubleByte[iIndex] != 0; iIndex+=2 )
        if ( ( *(PWORD)&szDoubleByte[iIndex] == *(LPWORD)&lpStr1[0] &&
               *(PWORD)&szSingleByte[iIndex] == *(LPWORD)&lpStr2[0] ) ||
             ( *(PWORD)&szDoubleByte[iIndex] == *(LPWORD)&lpStr2[0] &&
               *(PWORD)&szSingleByte[iIndex] == *(LPWORD)&lpStr1[0] )
           )
           return TRUE ;

    return FALSE ;
}


///////////////////////////////////////////////////////////////////////////
// Reverse a string
//
LPSTR LOCAL PASCAL DBCSstrrev (LPSTR lpStr)
{
    LPSTR       lpStart;
    LPSTR       lpEnd;
    LPSTR       lpMove;
    BYTE        byTemp1;
    BYTE        byTemp2;

    int         swapType;   // 0 = swap single byte with single byte
                    // 1 = swap double byte with single byte
                    // 2 = swap single byte with double byte
                    // 3 = swap double byte with double byte

    int         firstType;  // The type of the first character to swap
    int         secondType;     // The type of the second character to swap



    for( lpEnd = lpStr; *lpEnd; lpEnd++ )
        ;

    lpEnd-- ;

    for ( lpStart = lpStr ; lpStart < lpEnd ; lpStart++, lpEnd-- )
        {
    firstType   = DBCSGetByteType ( lpStr, lpStart );
    secondType  = DBCSGetByteType ( lpStr, lpEnd );

    if ( firstType == _MBC_LEAD )
            if ( secondType == _MBC_TRAIL )
            swapType = 3 ;          // swap double with double
        else
            swapType = 1 ;          // swap double with single
    else if ( secondType == _MBC_TRAIL )
        swapType = 2 ;              // swap single with double
    else
        swapType = 0 ;              // swap single with single

    switch ( swapType )
        {
        case 0:
            {
            byTemp1 = *lpStart ;    // Simply swap the bytes
            *lpStart = *lpEnd ;
            *lpEnd = byTemp1 ;
            break ;
            }

        case 3:                     // Simply swap 2 bytes
        {
        byTemp1 = *lpStart ;
        byTemp2 = *(lpStart+1) ;
        *lpStart = *(lpEnd-1) ;
        *(lpStart+1) = *(lpEnd) ;
        *(lpEnd-1) = byTemp1 ;
        *lpEnd = byTemp2 ;
        lpStart++ ;
        lpEnd-- ;
        break ;
        }

        case 1:                     // Swap double with single
        {
        byTemp1 = *lpStart ;
        byTemp2 = *(lpStart+1) ;
        for ( lpMove = lpStart + 1 ; lpMove < lpEnd; lpMove++ )
            *lpMove = *(lpMove + 1) ;

        *lpStart = *lpEnd ;
        *(lpEnd-1) = byTemp1 ;
        *(lpEnd) = byTemp2 ;
        lpEnd-- ;
        break ;
        }

        case 2:                     // Swap single with double
        {
        byTemp1 = *(lpEnd-1) ;
        byTemp2 = *lpEnd ;
        for ( lpMove = lpEnd; lpMove > lpStart; lpMove-- )
            *lpMove = *(lpMove - 1) ;

        *lpEnd = *lpStart ;
        *(lpStart) = byTemp1 ;
        *(lpStart+1) = byTemp2 ;
        lpStart++ ;
        break ;
        }
        }
    }

    return( lpStr );               //  return the original address
}

///////////////////////////////////////////////////////////////////////////
// search a string for any characters in another string
//
LPSTR LOCAL PASCAL DBCSstrpbrk(LPCSTR lpStr, LPCSTR lpChrs)
{
    int iCnt = 0, iLen, iSearch;

    if (!gbDBCSInitialized)
        DBCSSetInternalTable();

    for( iLen = 0; lpChrs[iLen]; iLen++ )
        ;

    while( lpStr[iCnt] )
        {                   // Ignore DBCS characters
        if( dbcsTable1[(BYTE)(lpStr[iCnt])] == DOUBLE_BYTE )
            {
            iCnt += 2;
            continue;
            }

        for( iSearch = 0; iSearch < iLen; iSearch += dbcsTable1[(BYTE)(lpChrs[iSearch])] )
            {
            // This will never compare dbcs characters.
            if (lpStr[iCnt] == lpChrs[iSearch])
                return (LPSTR)(&lpStr[iCnt]);
            }
        iCnt++;
        }

    return (NULL);
}

///////////////////////////////////////////////////////////////////////////
// reverse search a string for a character
//
LPSTR LOCAL PASCAL DBCSstrrchr(LPCSTR lpStr, int chSrch)
{
    LPSTR   lpMove = (LPSTR)lpStr;
    LPSTR   lpBuf = NULL;
    LPSTR   lpEnd;
    int     i;

    if (!gbDBCSInitialized)
        DBCSSetInternalTable();

    for( i = 0; lpStr[i]; i++ )
        ;

    lpEnd = (LPSTR)(&lpStr[--i]);

    while( lpMove < lpEnd )
        {
        if( *(BYTE far *)lpMove == (BYTE)chSrch )
            lpBuf = lpMove;

        lpMove += dbcsTable1[*(LPBYTE)lpMove] ;
        }

    return lpBuf;
}

///////////////////////////////////////////////////////////////////////////
// search a string for a character
//
LPSTR LOCAL PASCAL DBCSstrchr(LPCSTR lpStr, int chSrch)
{
    int iCnt;

    if (!gbDBCSInitialized)
        DBCSSetInternalTable();

    if( *lpStr == '\0' )
        return (NULL);

    for( iCnt = 0; ; iCnt += dbcsTable1[(BYTE)(lpStr[iCnt])] )
        {
        if ( lpStr[iCnt] == chSrch )
            return (LPSTR)&lpStr[iCnt];
        if (!lpStr[iCnt])
            break;
        }

    return (NULL);
}


///////////////////////////////////////////////////////////////////////////
// Copy up to nCount bytes from one string to another.
//
// Implementation notes:
// 1. If smaller than nCount, destination is padded with 0's.
//
// 2. If nCount is less than the length of the source, a '\0' is NOT
//    automatically appended.
//
// 3. uCount is the number of bytes to copy, NOT the number of characters.
//    This differs from _mbsncpy().
//
// 10/08/1996 DALLEE, created.
//
LPSTR LOCAL PASCAL DBCSstrncpy(LPSTR lpszDest, LPCSTR lpszSrc, size_t uCount)
{
    LPSTR   lpszDestPtr = lpszDest;
    LPCSTR  lpszSrcPtr  = lpszSrc;

    if ( !gbDBCSInitialized )
        DBCSSetInternalTable();

    while ( uCount && (EOS != *lpszSrcPtr) )
        {
                                        // stop one short rather than
                                        // split a DB character.
        if ( (1 == uCount) &&
             (_MBC_LEAD == DBCSGetByteType((LPSTR)lpszSrc, (LPSTR)lpszSrcPtr)) )
            {
            break;
            }
        else
            {
            *lpszDestPtr++ = *lpszSrcPtr++;
            uCount--;
            }
        }

                                        // pad to end.
    while ( uCount-- )
        {
        *lpszDestPtr++ = EOS;
        }

    return ( lpszDest );
}

///////////////////////////////////////////////////////////////////////////
// Case insensitive string comparison.
//
// Implementation notes:
// 1. Upper/Lower case double byte characters are NOT considered equivalent
//    by this function. This mirrors file system behavior, but differs from
//    _mbsicmp().
//
// 2. This function compares characters, not bytes. Double byte characters
//    are considered greater than single byte characters.
//
// 3. Ill-formed DBCS strings are handled by checking the low byte of
//    c1 for EOS -- Don't remove the cast!
//
// 10/08/1996 DALLEE, created.
//
int LOCAL PASCAL DBCSstricmp(LPCSTR lpszStr1, LPCSTR lpszStr2)
{
    UINT c1, c2;

    if ( !gbDBCSInitialized )
        DBCSSetInternalTable();

    do
        {
                                        // c1 is 1st character - may be 2 bytes
        c1 = *lpszStr1;
        if ( DOUBLE_BYTE == dbcsTable1[c1] )  // inline of DBCSIsLeadByte()
            {
            c1 = (c1 << 8);
            c1 |= *(lpszStr1+1);
            lpszStr1 += 2;
            }
        else
            {
            c1 = CONVERT_CASE(c1);
            lpszStr1++;
            }

                                        // c2 is 2nd character - may be 2 bytes
        c2 = *lpszStr2;
        if ( DOUBLE_BYTE == dbcsTable1[c2] )  // inline of DBCSIsLeadByte()
            {
            c2 = (c2 << 8);
            c2 |= *(lpszStr2+1);
            lpszStr2 += 2;
            }
        else
            {
            c2 = CONVERT_CASE(c2);
            lpszStr2++;
            }

        if ( c1 != c2 )
            {
                                        // Guarantee valid signed int return.
            return ( c1 > c2 ? 1 : -1 );
            }
                                        // Check low byte of c1 for EOS
        } while ( EOS != (BYTE)c1 );    // to handle ill-formed DBCS strings.

    return ( 0 );                       // Strings were equivalent.
}

///////////////////////////////////////////////////////////////////////////
// Case insensitive string comparison comparing at most uCount bytes.
//
// Implementation notes:
// 1. uCount is number of bytes to compare, NOT number of characters.
//    This differs from _mbsnicmp().
//
// 2. Upper/Lower case double byte characters are NOT considered equivalent
//    by this function. This mirrors file system behavior, but differs from
//    _mbsicmp().
//
// 3. Even though uCount is in bytes, this function compares characters,
//    not bytes. Double byte characters are considered greater than single
//    byte characters.
//
// 4. Ill-formed DBCS strings are handled by checking the low byte of
//    c1 for EOS -- Don't remove the cast!
//
// 10/08/1996 DALLEE, created.
//
int LOCAL PASCAL DBCSstrnicmp(LPCSTR lpszStr1, LPCSTR lpszStr2, size_t uCount)
{
    UINT c1, c2;

    if ( !gbDBCSInitialized )
        DBCSSetInternalTable();

    if ( 0 == uCount )                  // Sanity check here lets me be lazy
        return ( 0 );                   // and mostly copy DBCSstricmp's loop.

    do
        {
                                        // c1 is 1st character - may be 2 bytes
        c1 = *lpszStr1;
        if ( DOUBLE_BYTE == dbcsTable1[c1] )  // inline of DBCSIsLeadByte()
            {
            if ( uCount >= 2 )
                {
                c1 = (c1 << 8);
                c1 |= *(lpszStr1+1);
                lpszStr1 += 2;
                }
            }
        else
            {
            c1 = CONVERT_CASE(c1);
            lpszStr1++;
            }

                                        // c2 is 2nd character - may be 2 bytes
        c2 = *lpszStr2;
        uCount--;
        if ( DOUBLE_BYTE == dbcsTable1[c2] )  // inline of DBCSIsLeadByte()
            {
            if ( uCount )
                {
                c2 = (c2 << 8);
                c2 |= *(lpszStr2+1);
                lpszStr2 += 2;
                uCount--;
                }
            }
        else
            {
            c2 = CONVERT_CASE(c2);
            lpszStr2++;
            }

        if ( c1 != c2 )
            {
                                        // Guarantee valid signed int return.
            return ( c1 > c2 ? 1 : -1 );
            }
                                        // Check low byte of c1 for EOS
                                        // to handle ill-formed DBCS strings.
        } while ( uCount && (EOS != (BYTE)c1) );

    return ( 0 );                       // Strings were equivalent.
}

///////////////////////////////////////////////////////////////////////////
// Returns a pointer to the first occurance of lpszStr2 in lpszStr1 or
// NULL if not found.
//
// 10/08/1996 DALLEE, created.
//
LPSTR LOCAL PASCAL DBCSstrstr(LPCSTR lpszStr1, LPCSTR lpszStr2)
{
    LPCSTR  lpszEnd;                    // Last point where lpszStr2 can fit.
    UINT    uLength1;                   // Length (in bytes) of main string.
    UINT    uLength2;                   // Length (in bytes) of search string.

    if ( !gbDBCSInitialized )
        DBCSSetInternalTable();

                                        // Sanity check, since I don't know
                                        // if MEMCMP()'s behavior for
                                        // comparing 0 bytes is really defined.
    if ( EOS == *lpszStr2 )
        return ( (LPSTR)lpszStr1 );

    
                                        // Initialize locals.
    uLength1 = STRLEN(lpszStr1);
    uLength2 = STRLEN(lpszStr2);

                                        // If length of search string GT
                                        // main string, impossible to match.
    if (uLength1 < uLength2)
        return ( NULL );

    lpszEnd = lpszStr1 + (uLength1 - uLength2 );

                                        // Guts...
    while ( lpszStr1 <= lpszEnd )
        {
                                        // MEMCMP okay since DB strings must
                                        // match exactly.
        if ( 0 == MEMCMP(lpszStr1, lpszStr2, uLength2) )
            {
            return ( (LPSTR)lpszStr1 ); // Success.
            }

        lpszStr1 = AnsiNext(lpszStr1);
        }

    return ( NULL );
}


///////////////////////////////////////////////////////////////////////////
// Return a pointer to the next character in a null-terminated string.
// If lpszCurrent points to the terminating null character, lpszCurrent
// is returned.

//#if defined(NCD) || defined(DVD)  // otherwise it's a macro or Windows API
#if defined(SYM_DOS) || defined(SYM_VXD)    // otherwise it's a macro or Windows API

LPSTR SYM_EXPORT WINAPI AnsiNext ( LPCSTR lpszCurrent)
{
    if (!gbDBCSInitialized)
        DBCSSetInternalTable();

    if (*lpszCurrent == '\0')
        return (LPSTR) lpszCurrent;

    if (dbcsTable1[*lpszCurrent] == DOUBLE_BYTE)  // inline of DBCSIsLeadByte()
        {
        if (lpszCurrent[1] == '\0')  // never skip past a null character
            return (LPSTR) lpszCurrent + 1;
        else
            return (LPSTR) lpszCurrent + 2;
        }
    else
        return (LPSTR) lpszCurrent + 1;
}

#endif


///////////////////////////////////////////////////////////////////////////
// Return a pointer to the previous character in a null-terminated string.
// if lpszCurrent equals lpszStart, lpszStart is returned.

//#if defined(NCD) || defined(DVD)  // otherwise it's a macro or Windows API
#if defined(SYM_DOS) || defined(SYM_VXD)    // otherwise it's a macro or Windows API

LPSTR SYM_EXPORT WINAPI AnsiPrev ( LPCSTR lpszStart, LPCSTR lpszCurrent)
{
    LPCSTR lpszScan = lpszStart;
    LPCSTR lpszPrev = lpszStart;

    // Scan forward until we reach or pass the current position

    if (!gbDBCSInitialized)
        DBCSSetInternalTable();

    while (*lpszScan && lpszScan < lpszCurrent)
        {
        // Bring previous character up to date before advancing

        lpszPrev = lpszScan;

        // The remainder of the loop body is an inline
        // version of "lpszScan = AnsiNext(lpszScan);"

        if (dbcsTable1[*lpszScan] == DOUBLE_BYTE)
            {
            if (lpszScan[1] == '\0')
                lpszScan += 1;
            else
                lpszScan += 2;
            }
        else
            lpszScan += 1;
        }

    return (LPSTR) lpszPrev;
}

#endif

///////////////////////////////////////////////////////////////////////////
// Uppercase a string, ignoring double-byte characters

#if !defined(SYM_WIN)
#if defined(NCD) || defined(DVD)    // otherwise it's in prm_lstr.asm

LPSTR SYM_EXPORT WINAPI AnsiUpper (LPSTR string)
{
    LPSTR s = string;

    if (!gbDBCSInitialized)
        DBCSSetInternalTable();

    while (*s != '\0')
        {
    if (dbcsTable1[*s] == DOUBLE_BYTE)  // inline of DBCSIsLeadByte()
        {
        if (s[1] == '\0')  // ignore a lead byte with no trail byte
        return string;
        s += 2;
        }
    else
        {
        *s = CharToUpper(*s);
        ++s;
        }
    }
    return string;
}

#endif
#endif

///////////////////////////////////////////////////////////////////////////
// Lowercase a string, ignoreing double-byte characters

#if !defined(SYM_WIN)
#if defined(NCD) || defined(DVD)    // otherwise it's in prm_lstr.asm

LPSTR SYM_EXPORT WINAPI AnsiLower (LPSTR string)
{
    LPSTR s = string;

    if (!gbDBCSInitialized)
        DBCSSetInternalTable();

    while (*s != '\0')
        {
    if (dbcsTable1[*s] == DOUBLE_BYTE)  // inline of DBCSIsLeadByte()
        {
        if (s[1] == '\0')  // ignore a lead byte with no trail byte
        return string;
        s += 2;
        }
        else
        {
        *s = CharToLower(*s);
        ++s;
        }
    }
    return string;
}

#endif
#endif


/*--------------------------------------------------------------------------*/
/*                                                                          */
/* SYMxxx covers for string routines that have DBCS versions.               */
/*                                                                          */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
LPSTR SYM_EXPORT WINAPI SYMstrchr(LPCSTR lpszStr, int nChar)
{
    return((gbDBCSInstalled) ? DBCSstrchr(lpszStr, nChar) :
                               SBCS_STRCHR(lpszStr, nChar));
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
LPSTR SYM_EXPORT WINAPI SYMstrrchr(LPCSTR lpszStr, int nChar)
{
    return((gbDBCSInstalled) ? DBCSstrrchr(lpszStr, nChar) :
                               SBCS_STRRCHR(lpszStr, nChar));
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
LPSTR SYM_EXPORT WINAPI SYMstrpbrk(LPCSTR lpszStr1, LPCSTR lpszStr2)
{
    return((gbDBCSInstalled) ? DBCSstrpbrk(lpszStr1, lpszStr2) :
                               SBCS_STRPBRK(lpszStr1, lpszStr2));
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
LPSTR SYM_EXPORT WINAPI SYMstrrev(LPSTR lpszStr)
{
    return((gbDBCSInstalled) ? DBCSstrrev(lpszStr) :
                               SBCS_STRREV(lpszStr));
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
LPSTR SYM_EXPORT WINAPI SYMstrncpy(LPSTR lpszDest, LPCSTR lpszSrc, size_t uCount)
{
    return((gbDBCSInstalled) ? DBCSstrncpy(lpszDest, lpszSrc, uCount) :
                               SBCS_STRNCPY(lpszDest, lpszSrc, uCount));
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
int SYM_EXPORT WINAPI SYMstricmp(LPCSTR lpszStr1, LPCSTR lpszStr2)
{
    return((gbDBCSInstalled) ? DBCSstricmp(lpszStr1, lpszStr2) :
                               SBCS_STRICMP(lpszStr1, lpszStr2));
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
int SYM_EXPORT WINAPI SYMstrnicmp(LPCSTR lpszStr1, LPCSTR lpszStr2, size_t uCount)
{
    return((gbDBCSInstalled) ? DBCSstrnicmp(lpszStr1, lpszStr2, uCount) :
                               SBCS_STRNICMP(lpszStr1, lpszStr2, uCount));
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
LPSTR SYM_EXPORT WINAPI SYMstrstr(LPCSTR lpszStr1, LPCSTR lpszStr2)
{
    return((gbDBCSInstalled) ? DBCSstrstr(lpszStr1, lpszStr2) :
                               SBCS_STRSTR(lpszStr1, lpszStr2));
}



/*****************************************************************************

The following defines, variables and functions are to facilitate the loading
of DBCS font point sizes from string resources. The intent is to allow the
localization teams to be able to change the font sizes without the need to
involve the development teams as this information will no longer be
hard coded.

*****************************************************************************/
#if defined( SYM_WIN32 )

//
// To represent default DBCS font size if resource retrieval fails.
//
#define DBCS_DEFAULT_FONT_SIZE 8

//
// Used by 'DBCSGetFontSize'.
//
extern HINSTANCE hInst_SYMKRNL;



/*@API************************************************************************
@Declaration:
int SYM_EXPORT WINAPI DBCSGetFontSize(UINT nResId);
    
@Description:
Retrievest the string resource specified by nResId, converts it to a numeric value.

@Parameters:
$nResId$ Id of string resource to load.

@Returns:
Integer representation of string resource referred to by nResId.

@Include: dbcs.h

@Compatibility: Win32
*****************************************************************************/
int SYM_EXPORT WINAPI DBCSGetFontSize(UINT nResId)
    {
    //
    // Three digits should be enough to store the font size.
    //
    TCHAR szTemp[3 + 1];

    MEMSET(szTemp, 0, sizeof(szTemp));

    if (!LoadString(hInst_SYMKRNL, nResId, szTemp, sizeof(szTemp)/sizeof(TCHAR)))
        {
        //
        // If string load fails, return a default value.
        //
        return DBCS_DEFAULT_FONT_SIZE;
        }

    //
    // Convert the string to integer and return.
    //
    return _ttoi(szTemp);
    }



/*@API************************************************************************
@Declaration:
LPCTSTR SYM_EXPORT WINAPI DBCSGetFontFace(UINT nFontId);


@Description:
This function returns a pointer to a DBCS font name defined in DBCS.H.

@Parameters:
$nResId$ Id of font name to get.

@Returns:
Pointer to global constant containing the font name specified by nFontId.
    
@See:

@Include: dbcs.h

@Compatibiity: Win32
*****************************************************************************/
LPSTR SYM_EXPORT WINAPI DBCSGetFontFace(UINT nResId)
    {
    switch (nResId)
        {
        case IDS_DBCS_FONT_FIXED:
            return DBCS_FONT_FIXED;
            break;

        case IDS_DBCS_FONT_CHINESE:
            return DBCS_FONT_CHINESE;
            break;

        case IDS_DBCS_FONT_SIMPLIFIED_CHINESE:
            return DBCS_FONT_SIMPLIFIED_CHINESE;
            break;

        case IDS_DBCS_FONT_KOREAN:
            return DBCS_FONT_KOREAN;
            break;

        case IDS_DBCS_FONT_JAPANESE:
            return DBCS_FONT_JAPANESE;
            break;
        
        case IDS_DBCS_FONT:
        default:
            return DBCS_FONT;
            break;
        }
    }



/*@API************************************************************************
@Declaration:
void SYM_EXPORT WINAPI DBCSFreeFontFaceMemory(void);

@Description:
Obsolete. Do not use.

@Parameters:

@Returns:
Nothing.

@See:

@Compatibility:
*****************************************************************************/
void SYM_EXPORT WINAPI DBCSFreeFontFaceMemory(void)
    {
    return;
    }



#endif // SYM_WIN32
