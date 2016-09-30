// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
/* Copyright 1994 - 2003, 2005 Symantec Corporation. All rights reserved.     */
/************************************************************************
 *                                                                      *
 * $Header:   S:/INCLUDE/VCS/dbcs.h_v   1.13   18 Feb 1998 16:09:46   MKEATIN  $ *
 *                                                                      *
 * Description:                                                         *
 *      DBCS stuff.                                                     *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * $Log:   S:/INCLUDE/VCS/dbcs.h_v  $ *
// 
//    Rev 1.13   18 Feb 1998 16:09:46   MKEATIN
// IDS_DBCS_FONT_SIZE and IDS_DBCS_MSG_FONT_SIZE had been changed from a 
// value of 2505 and 2506, to 3000 and 3001.  3000 and 3001 conflict with 
// values in msgstr.h in SYMUTIL - this broke AVCORE/SYMNAV. 
// IDS_DBCS_FONT_SIZE and IDS_DBCS_MSG_FONT_SIZE are now 2510 and 2511.
// 
//    Rev 1.12   21 Jan 1998 09:35:58   RPULINT
// Added Font/Size for Simplified Chinese
// 
//    Rev 1.11   18 Nov 1997 14:49:04   TSmith
// Bracketed new DBCS font handling functions in #ifdef SYM_WIN32.
// 
//    Rev 1.10   12 Nov 1997 17:14:16   TSmith
// Moved the string IDs out of the elif(SYM_WIN32) and into their own
// ifdef(SYM_WIN32) block.
// 
//    Rev 1.9   10 Nov 1997 13:41:30   TSMITH
// Added strings IDs for DBCS font names and sizes.
// 
//    Rev 1.8   10 Nov 1997 10:37:24   TSMITH
// Added declarations for 'DBCSGetFontFace', 'DBCSGetFontSize' and
// 'DBCSFreeFontFaceMemory'.
// 
//    Rev 1.7   23 Jul 1997 16:49:48   DBUCHES
// Undid last revision.  We will be developing a better solution to this problem later.
//
//    Rev 1.6   09 Apr 1997 07:36:38   RANSCHU
// Changed Korean font size from 8 to 9.
//
//    Rev 1.5   11 Oct 1996 10:20:04   JBRENNA
// Change DBCS_FONT_CHINESE to use the HEX values for the characters.
// The problem was that on Japanese machines the last Chinese character
// value seemed to be a lead byte for Japanese. Thus, on Japanese machines,
// the " character became the trail byte. As such, files #include-ing this
// file would not compile on Win95 Japanese. Minor inconvenience, but might
// as well be fixed.
//
//    Rev 1.4   09 Oct 1996 16:42:08   GWESTER
// Added system fonts for Chinese and Korean. These new fonts are the same
// size as the Japanese and English fonts we already use, so now an
// Int'l English application can use one dialog resource across English,
// Japanese, Korean, and Chinese versions of Windows95. Also see changes
// in SYMGUI NGFONT.CPP .
//
//    Rev 1.3   20 Sep 1996 13:10:54   JALLEE
// Added prototypes for IsDBCSSec() and IsDBCSString().
//
//    Rev 1.2   22 Apr 1996 17:04:14   RANSCHU
// Changed DBCS_DLG to SBCS_DLG and reversed its meaning and interpretation.
//
//    Rev 1.1   22 Apr 1996 16:52:44   RANSCHU
// DBCS_FONT... switches to domestic if DBCS_DLG is not defined.
//
//    Rev 1.0   26 Jan 1996 20:17:34   RJACKSO
// Initial revision.
//
//    Rev 1.14   17 Jan 1996 14:30:40   RSTANEV
// Unicode support.
//
//    Rev 1.13   06 Nov 1995 17:05:20   RANSCHU
// Created a DBCS_FONT_FIXED symbol for non-Win32 platforms.
//
//    Rev 1.12   16 Oct 1995 16:08:10   REESE
// Fixed DBCS font names (from QAK9).
//
//    Rev 1.11   10 Oct 1995 20:29:18   SRYAN
//
// Added DBCS font/size for WIN32 apps from QUAKE 9.
//
//    Rev 1.10   20 Apr 1995 15:41:06   LindaD
// Added another Kanji routine
//
//    Rev 1.9   08 Nov 1994 17:21:42   BRAD
// Moved string functions to dbcs_str.h.
//
//    Rev 1.8   04 Nov 1994 15:09:12   BRAD
// Added a number of constants
//
//    Rev 1.7   28 Oct 1994 17:39:58   BRAD
// Renamed constants
//      
//    Rev 1.6   28 Oct 1994 17:38:30   BRAD
// Can't be recursively included
 ************************************************************************/


#ifndef INCL_DBCS
#define INCL_DBCS

// Same values found in mbctype.h, but this header is not available in
// 16-bit version, so we just include the constants here.
#if !defined(_MBC_SINGLE)
   #define _MBC_SINGLE  0               /* valid single byte char */
   #define _MBC_LEAD    1               /* lead byte */
   #define _MBC_TRAIL   2               /* trailing byte */
   #define _MBC_ILLEGAL (-1)            /* illegal byte */
#endif

#define SINGLE_BYTE             1
#define DOUBLE_BYTE             2

#if defined(SBCS_DLG) //[

    #define DBCS_FONT               "Helv"
    #define DBCS_FONT_FIXED         "Courier"
    #define DBCS_FONT_SIZE          8
    #define DBCS_MSG_FONT_SIZE      10       // Font size in warning dialogs etc.

#elif defined(SYM_WIN32) //][

    #define DBCS_FONT               "ÇlÇr ÇoÉSÉVÉbÉN"
    #define DBCS_FONT_SIZE          9
    //#define DBCS_FONT "\x82\x6c\x82\x72 \x82\x6f\x83\x53\x83\x56\x83\x62\x83\x4e"
    // MS proportional gothic, believe me !!

    #define DBCS_FONT_FIXED         "ÇlÇr ÉSÉVÉbÉN"
    #define DBCS_MSG_FONT_SIZE      11       // Font size in warning dialogs etc.
    //#define DBCS_FONT_FIXED "\x82\x6c\x82\x72 \x83\x53\x83\x56\x83\x62\x83\x4e"
    // MS gothic, believe me !!
    
    #define DBCS_FONT_JAPANESE      DBCS_FONT
    #define DBCS_FONT_SIZE_JAPANESE DBCS_FONT_SIZE
    //These are redefinitions for readability. I don't want to remove DBCS_FONT because
    //I don't know who's using it outside of Core.

    //----------------------------------------------------------------------------------
    // A Chinese Windows95 system font that provides both English and Chinese characters.
    //
    // JBRENNA - This is the "∑s≤”©˙≈È" font defined as hex values so that Win95
    // Japanese can compile. Apparently the last character for the Chinese font
    // is a lead byte for the Japanese code page. This leads to an unterminated string
    // when compiled on a J machine.
    #define DBCS_FONT_CHINESE       "\xB7\x73\xB2\xD3\xA9\xFA\xC5\xE9"
    #define DBCS_FONT_SIZE_CHINESE  9
    //Don't need DBCS_FONT_FIXED_CHINESE or DBCS_MSG_FONT_SIZE_CHINESE yet

    // Simplified Chinese font - "Song Ti"
    // For now, we use Font size = 9 (same as traditional chinese)
    #define DBCS_FONT_SIMPLIFIED_CHINESE  "\xCB\xCE\xCC\xE5"
    #define DBCS_FONT_SIZE_SIMPLIFIED_CHINESE  9
    
    #define DBCS_FONT_KOREAN        "±º∏≤"
    #define DBCS_FONT_SIZE_KOREAN   8
    //#define DBCS_FONT_KOREAN "\xB1\xBC\xB8\xB2"
    //A Korean Windows95 system font that provides both English and Korean characters.
    //Note: The reduced size may cause certain Korean characters to crumple; however,
    //for Int'l English, they'll fit in the same space as Japanese, Chinese, and English
    //characters, so that we won't have to resize any resources for Int'l English running
    //on Korean 95.
    //Don't need DBCS_FONT_FIXED_KOREAN or DBCS_MSG_FONT_SIZE_KOREAN yet

#else

    #define DBCS_FONT               "ïWèÄñæí©"      // This may not actually be proportional width!
    #define DBCS_FONT_FIXED         "ïWèÄñæí©"      // This may not actually be fixed width!
    #define DBCS_FONT_SIZE          12
    #define DBCS_MSG_FONT_SIZE      14       // Font size in warning dialogs etc.
    
#endif

#if !defined(UNICODE)
    
    #ifdef __cplusplus
    extern "C" {
    #endif
    
    BOOL    SYM_EXPORT WINAPI DBCSInstalled(VOID);
    VOID    SYM_EXPORT WINAPI DBCSSetInternalTable(VOID);
    BOOL    SYM_EXPORT WINAPI DBCSIsLeadByte(BYTE byCh);
    BOOL    SYM_EXPORT WINAPI DBCSIsTrailByte(BYTE byCh);
    int     SYM_EXPORT WINAPI IsDBCSSec(LPSTR lpStr, int cnt);
    int     SYM_EXPORT WINAPI IsKanjiSec(LPSTR lpStr, int cnt);
    int     SYM_EXPORT WINAPI DBCSGetByteType(LPSTR lpStart, LPSTR lpChk);
    
    LPSTR   SYM_EXPORT WINAPI DBCS_SearchLastChar( LPSTR lpStart, LPSTR lpEnd, char chChar );
    LPSTR   SYM_EXPORT WINAPI DBCS_SearchFirstChar( LPSTR lpStart, char chChar );
    int     SYM_EXPORT WINAPI DBCS_iskana( char c ) ;
    void    SYM_EXPORT WINAPI DBCS_ReplaceAnd ( LPSTR lpStr ) ;
    void    SYM_EXPORT WINAPI DBCS_PutBackAnd ( LPSTR lpStr ) ;
    int     SYM_EXPORT WINAPI IsDBCSString( LPSTR lpStr ) ;
    int     SYM_EXPORT WINAPI IsJapString ( LPSTR lpStr ) ;
    int     SYM_EXPORT WINAPI JapAccentCompare ( LPSTR lpStr1, LPSTR lpStr2 ) ;
    
    #if defined( SYM_WIN32 )
        LPSTR SYM_EXPORT WINAPI DBCSGetFontFace( UINT nResId );
        void  SYM_EXPORT WINAPI DBCSFreeFontFaceMemory( void );
        int   SYM_EXPORT WINAPI DBCSGetFontSize( UINT nResId );
    #endif
     
    #ifdef __cplusplus
    }
    #endif
    
#else // #if !defined(UNICODE)
    
                                            // Add macros on as-needed basis.
    #define DBCSInstalled()         ((BOOL)FALSE)
    #define DBCSIsLeadByte(b)       ((BOOL)FALSE)
    #define DBCSGetByteType(a,b)    ((int)_MBC_SINGLE)

#endif  // if defined(UNICODE)

#if defined(SYM_WIN32)
    //
    // IDs for string resources for DBCS font names and sizes.
    //
    #define IDS_DBCS_FONT                          2500
    #define IDS_DBCS_FONT_FIXED                    2501
    #define IDS_DBCS_FONT_CHINESE                  2502
    #define IDS_DBCS_FONT_KOREAN                   2503
    #define IDS_DBCS_FONT_JAPANESE                 2504
    #define IDS_DBCS_FONT_SIMPLIFIED_CHINESE       2505
    
    #define IDS_DBCS_FONT_SIZE                     2510
    #define IDS_DBCS_MSG_FONT_SIZE                 2511
    #define IDS_DBCS_FONT_SIZE_CHINESE             2512
    #define IDS_DBCS_FONT_SIZE_KOREAN              2513
    #define IDS_DBCS_FONT_SIZE_JAPANESE            2514
    #define IDS_DBCS_FONT_SIZE_SIMPLIFIED_CHINESE  2515 

#endif

#endif  // INCL_DBCS

