/************************************************************************
 *                                                                      *
 * $Header:   S:/SYMKRNL/VCS/BADCHAR.CPv   1.2   19 Mar 1997 21:46:10   BILL  $ *
 *                                                                      *
 * Description:  This module contains all the function required to      *
 *               obtain illegal characters for any platform. The        *
 *               function NameReturnIllegalChars() replaces the old     *
 *               NameGetIllegalChars() that is broken for LFN's.        *
 *                                                                      *
 * Contains:     NameReturnIllegalChars()                               *
 *                                                                      *
 * See Also:     XAPI.H for function prototype and for SYM_ILLEGAL_xxx  *
 *                                                                      *
 ************************************************************************
 * $Log:   S:/SYMKRNL/VCS/BADCHAR.CPv  $                  *
// 
//    Rev 1.2   19 Mar 1997 21:46:10   BILL
// Modified files to build properly under Alpha platform
// 
//    Rev 1.1   24 Apr 1996 11:41:40   BMCCORK
// Added check for failure on use of vxd and don't use results if failed.  
// This is the equivalent of what happens under NT and was added for 
// custom Quake clients that don't want to ship Symkrnl.vxd or a custom
// equivalent.  
// 
//    Rev 1.0   26 Jan 1996 20:21:18   JREARDON
// Initial revision.
// 
//    Rev 1.15   17 Jan 1996 12:26:38   RSTANEV
// Added Unicode support.
//
//    Rev 1.14   11 Dec 1995 15:08:08   LVAN
// No change.
//
//    Rev 1.13   17 Aug 1995 19:12:56   DAVID
// NameReturnIllegalChars() was not always returning wildcard characters
// on non LFN volumes.
//
//    Rev 1.12   16 Aug 1995 11:52:52   TORSTEN
// Fix to allow compilation for OS/2 platform.
//
//    Rev 1.11   27 Jul 1995 14:41:16   DBUCHES
// Merged quake 8 changes.
//
//    Rev 1.10   20 Jun 1995 16:41:28   DBUCHES
// Fixed DOS platform build problems.
//
//    Rev 1.9   20 Jun 1995 13:47:20   BILL
// Fixed to compile for SYM_VXD
//
//    Rev 1.8   19 Jun 1995 15:28:40   BILL
// Win32, Win16, DX, and DOS invalid character changes
//
//    Rev 1.7   14 Jun 1995 20:28:46   DBUCHES
// Add OEM lowercase characters to list of Illegal characters.
//
//    Rev 1.6   14 Jun 1995 18:17:28   DBUCHES
// Replaced call to IsCharLower with CharIsLower().
//
//    Rev 1.5   14 Jun 1995 17:54:12   BILL
// Fixed NameReturnIllegalChars().  Now returns correct Short filename illegals.
//
//    Rev 1.4   28 Feb 1995 00:37:24   BRUCE
//
// Fixed to NEVER EVER write more bytes to the user's buffer than he told
// us he had.  You can't write extra bytes to the buffer and then "remove" them
// (RemoveLowercaseChars())
//
//    Rev 1.3   23 Feb 1995 23:34:34   BRUCE
// One last fix - uSizeRequired <= uSizeIllegals
//
//    Rev 1.2   23 Feb 1995 23:14:58   BRUCE
//
// Fixed to return exact number of characters required when that is requested.
//
// Fixed to return exact number of characters copied into application supplied
// buffer - if we don't copy anything into it, return 0.
//
//    Rev 1.1   16 Feb 1995 09:40:16   MFALLEN
// Include file badchar.h is not needed. Function prototype is is XAPI.H
//
//    Rev 1.0   16 Feb 1995 09:37:32   MFALLEN
// Initial revision.
 ************************************************************************/

/*@Library: SYMKRNL*/                   // Used by Geologist project.



#ifndef SYM_WIN32
#include "dos.h"
#endif
#include "platform.h"
#include "xapi.h"
#include "disk.h"


// ---------------------------------------------------------------------------
//  Local definitions. All these definitions must be undefined at the end of
//  this function.
// ---------------------------------------------------------------------------

static const int nMAX_ASCII_LIMIT=255;


// DOS UpperCase table
typedef struct
    {
    BYTE byInfoID;
    DWORD dwUpper;
    } UPPER_INFO;


// ---------------------------------------------------------------------------
//   Inlines to examine the wOsFlag field passed to NameReturnIllegalChars()
// ---------------------------------------------------------------------------

                                        // -----------------------------------
                                        // To obtain the OS that we should be
                                        // returning invalid characters for.
                                        // Returns a flag.
                                        // -----------------------------------
inline WORD fGetOsField ( WORD w )
    { return ( ( w & SYM_ILLEGAL_USEVOLUME ) ?
                SYM_ILLEGAL_USEVOLUME : ( w & 0x000F ) ); }

                                        // -----------------------------------
                                        // Returns TRUE if wildcard characters
                                        // are to be considered as invalid.
                                        // -----------------------------------
inline BOOL bWildcardsInvalid ( WORD w )
        { return ( ( w & SYM_ILLEGAL_WILDCARDCHARS ) ? TRUE : FALSE ); }

                                        // -----------------------------------
                                        // Returns TRUE if path characters
                                        // are to be considered as invalid.
                                        // -----------------------------------
inline BOOL bPathCharsInvalid ( WORD w )
        { return ( ( w & SYM_ILLEGAL_PATHCHARS ) ? TRUE : FALSE ); }

                                        // -----------------------------------
                                        // Returns TRUE if lower case chars
                                        // are to be considered as invalid.
                                        // -----------------------------------
inline BOOL bLowerCharsInvalid ( WORD w )
        { return ( ( w & SYM_ILLEGAL_SFN_LOWERCASE ) ? TRUE : FALSE ); }

// ---------------------------------------------------------------------------
//                  Local function prototypes.
// ---------------------------------------------------------------------------

LPBYTE LOCAL GetNextIllegalRange (      // Get the next range to check
    LPBYTE  lpIllegalCharArray,         // [in] array of range words
    LPBYTE  lpbyLow,                    // [out] from this range
    LPBYTE  lpbyHigh                    // [out] to that range
    );

UINT LOCAL GetIllegalPathChars (        // Fill buffer with path chars
    LPTSTR  lpIllegals                  // [out] buffer to fill
    );

UINT LOCAL GetIllegalWildcardChars (    // Fill buffer with wildcard chars
    LPTSTR  lpIllegals                  // [out] buffer to fill
    );

UINT LOCAL ComputeIllegalCharSize (     // Compute number of illegal chars
    LPBYTE  lpIllegalCharArray,         // [in] array that tells us what to do
    BOOL    bWildcards,                 // [in] wildcards are invalid
    BOOL    bPathChars,                 // [in] path chars are invalid
    BOOL    bLowerCase                  // [in] lower case chars are invalid
    );

UINT LOCAL GetIllegalCharsFromArray (   // Initialize buffer with illegals
    LPTSTR  lpIllegals,                 // [out] illegals are placed here
    UINT    uSizeIllegals,              // [in]  size of lpIllegals buffer
    LPBYTE  lpIllegalCharArray,         // [in] array that tells us what to do
    BOOL    bLowerChars                 // [in] TRUE if lower case chars are allowed
    );

UINT LOCAL GetIllegalChars (            // Get illegal chars
    LPTSTR  lpIllegals,                 // [out] place them here
    UINT    uSizeIllegals,              // [in] size of buffer
    LPBYTE  lpIllegalCharArray,         // [in] index array
    BOOL    bWildcards,                 // [in] add wildcards to illegals
    BOOL    bPathChars,                 // [in] add path chars to illegals
    BOOL    bLowerChars                 // [in] lower case invalid, SFN only
    );

// ---------------------------------------------------------------------------

/*@API:*************************************************************************
@Declaration:
EXTERNC UINT SYM_EXPORT WINAPI NameReturnIllegalChars ( // Get illegal characters
    WORD        wOsFlag,                // [in] use any OS_ILLEGAL_xxx
    LPCTSTR     lpVolume,               // [in] pointer to volume, for LFN
    LPTSTR      lpIllegals,             // [out] illegal chars placed here
    UINT        uSizeIllegals           // [in] max size of lpIllegals
    );

@Description:
This function return the invalid characters for a specific volume or an
operating system. This function should be used instead of NameGetIllegalChars()
because the old function is broke.

@Parameters:
$wOsFlag$   should a combination of SYM_ILLEGAL_xxx flags:

    SYM_ILLEGAL_USEVOLUME
            The function should use the contents of lpVolume to check if
            specified volume supports long filenames. If the OS that we're
            running on does not support long filenames or if the specified
            volume does not support long filename the invalid characters for
            short filenames are returned otherwise the invalid characters
            for long filenames are returned.

    SYM_ILLEGAL_DOS
    SYM_ILLEGAL_WIN16
    SYM_ILLEGAL_WIN32s
    SYM_ILLEGAL_WIN95
    SYM_ILLEGAL_WINNT
    SYM_ILLEGAL_OS2
            These flags are ignored if SYM_ILLEGAL_USEVOLUME is set
            otherwise this function returns the invalid characters for the
            specified operating system. Note that the application can not
            specify one operating system for each function call. If neither
            SYM_ILLEGAL_USEVOLUME or any of the above flags are set this
            function doesn't do anything and returns with a zero.


    SYM_ILLEGAL_WILDCARDCHARS
            When this flag is set all the wildcard characters are added to the
            list of invalid characters. Use this instead of manually joggle with
            buffers.

    SYM_ILLEGAL_PATHCHARS
            When this flag is set all the path characters are added to the
            list of invalid characters. Use this instead of manually joggle with
            buffers.

    SYM_ILLEGAL_SFN_LOWERCASE
            If you would like lower case characters to be added to the list of
            invalid character set this flag. This flag is only used when the
            operating system that we are requesting invalid characters for does
            not support long filenames, or if the specified volume does not
            support long filenames.

$lpVolume$
    Specified the volume that we should check if it supports long filenames.
    This information is only used if the SYM_ILLEGAL_USEVOLUME flag is set.

$lpIllegals$
    Pointer to a buffer that will hold all the invalid characters. If this
    parameter is NULL the function return the minimum size of the buffer that
    is required to complete the operation.

$uSizeIllegals$
    Specified the maximum number of characters that lpIllegals can hold.

@Returns:
Number of invalid characters placed in the application provided buffer, or the
minimum size of the application provided buffer if lpIllegals parameter is NULL.

@Compatibility: DOS, Win16 and Win32
*******************************************************************************/

EXTERNC UINT SYM_EXPORT WINAPI NameReturnIllegalChars ( // Get illegal characters
    WORD        wOsFlag,                // [in] use any OS_ILLEGAL_xxx
    LPCTSTR     lpVolume,               // [in] pointer to volume, for LFN
    LPTSTR      lpIllegals,             // [out] illegal chars placed here
    UINT        uSizeIllegals           // [in] max size of lpIllegals
    )
{
                                        // -----------------------------------
                                        // &? This function is not double byte
                                        // enabled. DBCS lead bytes are valid.
                                        // -----------------------------------

                                        // -----------------------------------
                                        // If the caller did not supply a buffer,
                                        // use a temporary one.
                                        // -----------------------------------
    TCHAR tempIllegals[256];
    if (lpIllegals == NULL)
        {
        lpIllegals = tempIllegals;
        uSizeIllegals = 256;
        }

                                        // -----------------------------------
                                        // If the application set the use
                                        // volume flag then the volume buffer
                                        // must be valid.
                                        // -----------------------------------
#ifdef SYM_DEBUG
    if ( fGetOsField ( wOsFlag ) == SYM_ILLEGAL_USEVOLUME )
        {
        SYM_ASSERT ( lpVolume && *lpVolume );
        }
#endif


    auto    UINT    uNumChars       =   0;
    auto    BOOL    bSupportsLFN    =   FALSE;
    auto    STATUS  Status          =   NOERR;

                                        // -----------------------------------
                                        // Clear destination buffer.
                                        // -----------------------------------

    MEMSET ( lpIllegals, EOS, uSizeIllegals * sizeof(TCHAR) );

                                        // -----------------------------------
                                        // Check what operating system we're
                                        // to return illegal characters for.
                                        // -----------------------------------

    switch ( fGetOsField ( wOsFlag ) )
        {
        case SYM_ILLEGAL_USEVOLUME:
            if ( lpVolume )
                {
                bSupportsLFN = VolumeSupportsLFN ( lpVolume );
                }
            else
                Status = ERR;
            break;

        case SYM_ILLEGAL_DOS:
        case SYM_ILLEGAL_WIN16:
        case SYM_ILLEGAL_WIN32s:
            bSupportsLFN = FALSE;
            break;

        case SYM_ILLEGAL_WIN95:
        case SYM_ILLEGAL_WINNT:
            {
            bSupportsLFN = TRUE;
            }
            break;

        case SYM_ILLEGAL_OS2:
            Status = ERR;
            break;

        default:
            SYM_ASSERT ( 0 );
            Status = ERR;
            break;
        }

                                        // -----------------------------------
                                        // If a valid uOsFlag was specified.
                                        // -----------------------------------
    if ( Status == NOERR )
        {
                                        // -----------------------------------
                                        // Obtain illegals the correct way!!!
                                        // -----------------------------------

                                        // -----------------------------------
                                        // This table specifies the ranges of
                                        // illegal characters for LONG
                                        // filenames. The values should be in
                                        // pairs specifying the beginning
                                        // and end of the invalid ASCII code
                                        // ranges. According to Microsoft docs
                                        // valid characters are the same as
                                        // for short filenames with the addition
                                        // of the period. Following six special
                                        // characters are valid for long names:
                                        // +,;=[]
                                        // -----------------------------------
                                        // NOTE: Path characters and wildcard
                                        // characters are not included in this
                                        // array as invalid character. In our
                                        // case it is the application that
                                        // specifies if they should be consi-
                                        // dered as invalid.
                                        // -----------------------------------

        static  BYTE    byLFNIllegalIndexArray[] =
            {
            0,      31,                 // Space is valid
            34,     34,                 // Quotation mark is not valid.
            60,     60,                 // < less than sign
            62,     62,
            124,    124,                // pipe symbol
            127,    127,                // DEL
            0,      0
            };
                                        // These are characters which DOS
                                        // does not return as illegal,
                                        // but ScanDisk complains about.
        static  BYTE    bySFNIllegalIndexArray[] =
            {
            44,      44,                // Comma
            59,      59,                // semi-colon
            0,       0
            };

        if(bSupportsLFN)
            uNumChars = GetIllegalChars (
                                    lpIllegals,
                                    uSizeIllegals,
                                    byLFNIllegalIndexArray,
                                    bWildcardsInvalid ( wOsFlag ),
                                    bPathCharsInvalid ( wOsFlag ),
                                    ( bSupportsLFN == TRUE ) ?
                                      TRUE :
                                      bLowerCharsInvalid( wOsFlag )
                                    );
        else
            {
            uNumChars = NameGetIllegalChars ( lpIllegals, bPathCharsInvalid ( wOsFlag ));
            uNumChars += GetIllegalCharsFromArray ( &lpIllegals[uNumChars], uSizeIllegals, bySFNIllegalIndexArray, TRUE );
            if(bLowerCharsInvalid( wOsFlag ))
                {
                                        // Note that this inserts characters
                                        // which are lowercase for the current
                                        // OEM character set, since that's
                                        // how DOS/Win95 stores filenames.
#ifdef SYM_WIN32
                #if defined(UNICODE)
                SYM_ASSERT ( FALSE );
                #endif

                int i;
                for(i = 'a'; i <= 'z'; i++)
                    {
                    if(STRCHR(lpIllegals, (char)i) == NULL)
                        lpIllegals[uNumChars++] = (char) i;
                    }

#ifndef _M_ALPHA                
                if (SystemGetWindowsType() != SYM_SYSTEM_WIN_NT)
                    {
                    auto  UPPER_INFO    upperInfo;
                    auto  SYM_REGS      regs;
                    auto  LPVOID        lpUpper;

                    MEMSET(&upperInfo, 0, sizeof(upperInfo));
                    regs.EAX = 0x00006502;
                    regs.EBX = 0x0000FFFF;          // default code page
                    regs.ECX = sizeof(upperInfo);
                    regs.EDX = 0x0000FFFF;          // default country ID
                    regs.EDI = (DWORD) &upperInfo;
                    BOOL bVxDAvailable = VxDIntBasedCall( 0x21, &regs );
                    if ( bVxDAvailable && (regs.EFlags & 0x00000001) == 0)
                        {
                        auto    char    buffer[128+sizeof(WORD)];

                        MEMSET(&buffer, 0, sizeof(buffer));
                        lpUpper = (LPVOID) upperInfo.dwUpper;

                        MemCopyPhysical(buffer, lpUpper, sizeof(buffer), FALSE);

                        LPBYTE lpByte = (LPBYTE) &buffer[sizeof(WORD)];

                        for(i = 128; i < 256; i++)
                            if((lpByte[i-128] != 0) && (lpByte[i-128] != i))
                                if(STRCHR(lpIllegals, (char)i) == NULL)
                                    lpIllegals[uNumChars++] = (char) i;
                        }
                    }
#endif                    
#elif !defined(SYM_VXD) && !defined(SYM_NLM) && !defined(SYM_OS2) && !defined(SYM_NTK)
                auto    int             i;
                auto    union REGS      regs;
                auto    struct SREGS    sregs;
                auto    UPPER_INFO far *lpUpperInfo;
#ifdef SYM_PROTMODE
                auto    DWORD           dwSegSel;
#else
                auto    LPVOID          lpBuffer;
#endif

                for(i = 'a'; i <= 'z'; i++)
                    {
                    if(STRCHR(lpIllegals, (char)i) == NULL)
                        lpIllegals[uNumChars++] = (char) i;
                    }

                MEMSET(&regs, 0, sizeof(REGS));
                MEMSET(&sregs, 0, sizeof(SREGS));
#ifdef SYM_PROTMODE
                dwSegSel = GlobalDosAlloc(sizeof(UPPER_INFO));
                lpUpperInfo = (UPPER_INFO far *)MAKELP(LOWORD(dwSegSel), 0);
#else
                lpBuffer = MemAllocPtr(0 ,sizeof(UPPER_INFO));
                lpUpperInfo = (UPPER_INFO far *)lpBuffer;
#endif
                if (lpUpperInfo != NULL)
		    {
		    regs.x.ax = 0x6502;          // Get UpperCase table.
		    regs.x.bx = 0xFFFF;          // default code page
		    regs.x.cx = sizeof(UPPER_INFO);
		    regs.x.dx = 0xFFFF;          // default country ID
#ifdef SYM_PROTMODE
		    sregs.es = HIWORD(dwSegSel);
		    regs.x.di = 0x0000;
		    IntWin(0x21, &regs, &sregs);
#else
		    sregs.es = FP_SEG(lpBuffer);
		    regs.x.di = FP_OFF(lpBuffer);
		    Int(0x21, &regs, &sregs);
#endif
		    if ( (regs.x.cflag & 0x0001) == 0)
			{
			auto    char    buffer[128+sizeof(WORD)];
			auto    LPVOID  lpUpper;

			MEMSET(&buffer, 0, sizeof(buffer));
			lpUpper = (LPVOID) lpUpperInfo->dwUpper;

			MemCopyPhysical(buffer, lpUpper, sizeof(buffer), FALSE);

			LPBYTE lpByte = (LPBYTE)&buffer[sizeof(WORD)];

			for(i = 128; i < 256; i++)
			    if((lpByte[i-128] != 0) && (lpByte[i-128] != i))
				if(STRCHR(lpIllegals, (char)i) == NULL)
				    lpIllegals[uNumChars++] = (char) i;
			}
#ifdef SYM_PROTMODE
		    GlobalDosFree(LOWORD(dwSegSel));
#else
		    MemFreePtr(lpBuffer);
#endif
		    } // end if (lpUpperInfo != NULL)
#endif
                } // end if (bLowerCharsInvalid(wOsFlag)

#ifdef SYM_WIN32  // why do we do this?
	    if (SystemGetWindowsType() != SYM_SYSTEM_WIN_NT && bWildcardsInvalid ( wOsFlag ) )
                uNumChars += GetIllegalWildcardChars ( &lpIllegals[uNumChars] );
#else
            if( bWildcardsInvalid ( wOsFlag ) )
                uNumChars += GetIllegalWildcardChars ( &lpIllegals[uNumChars] );
#endif
            } // end if (bSupportsLFN) else

        } // end if (Status == NOERR)


    return ( uNumChars );
}

/*@API:*************************************************************************
@Declaration:
UINT LOCAL GetIllegalChars (            // Get illegal chars
    LPTSTR  lpIllegals,                 // [out] place them here
    UINT    uSizeIllegals,              // [in] size of buffer
    LPBYTE  lpIllegalCharArray,         // [in] index array
    BOOL    bWildcards,                 // [in] add wildcards to illegals
    BOOL    bPathChars,                 // [in] add path chars to illegals
    BOOL    bLowerChars                 // [in] lower case invalid, SFN only
    )

@Description:
This function is called locally to fill the application provided buffer with
invalid characters accordig to the invalid character index table.

@Parameters:
$lpIllegals$
    Pointer to application buffer that will contain all the invalid characters

$uSizeIllegals$
    Maximum number of characters that lpIllegals can hold

$lpIllegalCharArray$
    Pointer to an array of bytes that specified the start and beginning of
    invalid characters in the ascii table.

$bWildcards$
    TRUE if we're to add wildcard characters to the list of invalid characters.

$bPathChars$
    TRUE if we're to add path characters to the list of invalid characters.

$bLowerChars$
    TRUE if we're to add lower case characters to the list of invalid characters.

@Returns:
Number of invalid characters placed in the application provided buffer, or the
minimum size of the application provided buffer if lpIllegals parameter is NULL.

@Compatibility: DOS, Win16 and Win32
*******************************************************************************/

UINT LOCAL GetIllegalChars (            // Get illegal chars
    LPTSTR  lpIllegals,                 // [out] place them here
    UINT    uSizeIllegals,              // [in] size of buffer
    LPBYTE  lpIllegalCharArray,         // [in] index array
    BOOL    bWildcards,                 // [in] add wildcards to illegals
    BOOL    bPathChars,                 // [in] add path chars to illegals
    BOOL    bLowerChars                 // [in] lower case invalid, SFN only
    )
{
    auto    UINT    uNumChars   =   0;

                                        // -----------------------------------
                                        // Make sure buffer is large enough to
                                        // hold all the invalid characters.
                                        // -----------------------------------

    UINT uSizeRequired = ComputeIllegalCharSize (
                                        lpIllegalCharArray,
                                        bWildcards,
                                        bPathChars,
                                        bLowerChars
                                        );

                                        // -----------------------------------
                                        // If the application provided buffer
                                        // pointer is zero or if the size of
                                        // the buffer that the application
                                        // specified is zero, return the size
                                        // of the buffer than is required for
                                        // all the invalid characters.
                                        // -----------------------------------

    if ( lpIllegals && uSizeRequired <= uSizeIllegals )
        {
                                        // -----------------------------------
                                        // Fill buffer with invalid characters
                                        // according to what the array.
                                        // -----------------------------------

        uNumChars = GetIllegalCharsFromArray ( lpIllegals, uSizeIllegals, lpIllegalCharArray, bLowerChars );
        SYM_ASSERT(uNumChars <= uSizeIllegals);

                                        // -----------------------------------
                                        // Add wildcard characters if required
                                        // -----------------------------------
        if ( bWildcards == TRUE )
            {
            uNumChars += GetIllegalWildcardChars ( &lpIllegals[uNumChars] );
            }

                                        // -----------------------------------
                                        // Add path characters if required
                                        // -----------------------------------
        if ( bPathChars == TRUE )
            {
            uNumChars += GetIllegalPathChars ( &lpIllegals[uNumChars] );
            }
        }
    else
        {
        if (lpIllegals == NULL)
            uNumChars = uSizeRequired;
        }
                                        // If they didn't pass a big enough
                                        // buffer, then return 0, since
                                        // we don't put anything into their
                                        // buffer in that case.

    return ( uNumChars );
}


/*@API:*************************************************************************
@Declaration:
UINT LOCAL GetIllegalCharsFromArray (   // Initialize buffer with illegals
    LPTSTR  lpIllegals,                 // [out] illegals are placed here
    UINT    uSizeIllegals,              // [in]  size of lpIllegals buffer
    LPBYTE  lpIllegalCharArray,         // [in] array that tells us what to do
    BOOL    bLowerChars                 // [in] TRUE if we're to add lower
                                        //      case chars to the list of
                                        //      invalid characters.
    );

@Description:
This function is called locally to fill the application provided buffer with
invalid characters accordig to the invalid character index table.

@Parameters:
$lpIllegals$
    Pointer to application buffer that will contain all the invalid characters

$uSizeIllegals$
    Size of lpIllegals buffer (number of characters)

$lpIllegalCharArray$
    Pointer to an array of bytes that specified the start and beginning of
    invalid characters in the ascii table.

$bLowerChars$
    TRUE if we're to add lower case characters to the list of
    invalid characters.

@Returns:
Number of invalid characters placed in the application provided buffer.

@Compatibility: DOS, Win16 and Win32
*******************************************************************************/

UINT LOCAL GetIllegalCharsFromArray (   // Initialize buffer with illegals
    LPTSTR  lpIllegals,                 // [out] illegals are placed here
    UINT    uSizeIllegals,              // [in]  size of lpIllegals buffer
    LPBYTE  lpIllegalCharArray,         // [in] array that tells us what to do
    BOOL    bLowerChars                 // [in] TRUE if we're to add lower
                                        //      case chars to the list of
                                        //      invalid characters.
    )
{
    auto    UINT    uCount  =   0;
    auto    BYTE    byLow;
    auto    BYTE    byHigh;

    SYM_ASSERT ( lpIllegalCharArray );

    lpIllegalCharArray = GetNextIllegalRange (
                                            lpIllegalCharArray,
                                            &byLow,
                                            &byHigh
                                            );

    for ( WORD wAsciiCode = 0; wAsciiCode <= nMAX_ASCII_LIMIT; wAsciiCode++ )
        {
        if ( wAsciiCode > byHigh )
            {
            lpIllegalCharArray = GetNextIllegalRange (
                                            lpIllegalCharArray,
                                            &byLow,
                                            &byHigh
                                            );
            }

        if ( wAsciiCode >= byLow && wAsciiCode <= byHigh )
            {
                                        // -----------------------------------
                                        // Lower case letters are always part
                                        // of the list for short filenames.
                                        // If this flag is CLEARED then lower
                                        // case letters are removed.
                                        // -----------------------------------

            if (!bLowerChars && (wAsciiCode >= 'a' && wAsciiCode <= 'z' ))
                continue;

            *lpIllegals = (TCHAR)wAsciiCode;
            lpIllegals++;
            uCount++;
            if (uCount == uSizeIllegals)
                break;                  // Make sure we don't overwrite the
                                        // user's buffer (even though we
                                        // shouldn't even get here)
            }
        }

    return ( uCount );
}

/*@API:*************************************************************************
@Declaration:
UINT LOCAL ComputeIllegalCharSize (     // Compute number of illegal chars
    LPBYTE  lpIllegalCharArray,         // [in] array that tells us what to do
    BOOL    bWildcards,                 // [in] wildcards are invalid
    BOOL    bPathChars,                 // [in] path chars are invalid
    BOOL    bLowerCase                  // [in] lower case chars are invalid
    )

@Description:
This function is called locally to compute the number of invalid characters
that will be placed in the application provided buffer for the application
specified operating system or volume.

@Parameters:

$lpIllegalCharArray$
    Pointer to an array of bytes that specified the start and beginning of
    invalid characters in the ascii table.

$bWildcards$
    TRUE if we're to add wildcard characters to the list of invalid characters.

$bPathChars$
    TRUE if we're to add path characters to the list of invalid characters.

$bLowerChars$
    TRUE if we're to add lower case characters to the list of invalid characters.

@Returns:
Number of invalid characters that will be placed in the application provided
buffer.

@Compatibility: DOS, Win16 and Win32
*******************************************************************************/

UINT LOCAL ComputeIllegalCharSize (     // Compute number of illegal chars
    LPBYTE  lpIllegalCharArray,         // [in] array that tells us what to do
    BOOL    bWildcards,                 // [in] wildcards are invalid
    BOOL    bPathChars,                 // [in] path chars are invalid
    BOOL    bLowerCase                  // [in] lower case chars are invalid
    )
{
    auto    UINT    uCount  =   0;
    auto    BYTE    byLow;
    auto    BYTE    byHigh;
    auto    TCHAR   szDummy[SYM_MAX_PATH];

    SYM_ASSERT ( lpIllegalCharArray );

    lpIllegalCharArray = GetNextIllegalRange (
                                            lpIllegalCharArray,
                                            &byLow,
                                            &byHigh
                                            );

    for ( WORD wAsciiCode = 0; wAsciiCode <= nMAX_ASCII_LIMIT; wAsciiCode++ )
        {
        if ( byHigh && wAsciiCode > byHigh )
            {
            lpIllegalCharArray = GetNextIllegalRange (
                                            lpIllegalCharArray,
                                            &byLow,
                                            &byHigh
                                            );
            }

        if ( wAsciiCode >= byLow && wAsciiCode <= byHigh )
            uCount++;
        }

    if ( bWildcards == TRUE )
        {
        uCount += GetIllegalWildcardChars ( szDummy );
        }

    if ( bPathChars == TRUE )
        {
        uCount += GetIllegalPathChars ( szDummy );
        }

    if ( bLowerCase == FALSE )
        {
        uCount -= ( 'z' - 'a' + 1 );
        }

    return ( uCount );
}


/*@API:*************************************************************************
@Declaration:
UINT LOCAL GetIllegalWildcardChars (    // Fill buffer with wildcard chars
    LPTSTR  lpIllegals                  // [out] buffer to fill
    );

@Description:
This function is called locally to add wildcard characters to the list of
invalid characters.

@Parameters:

$lpIllegals$
    Pointer to application buffer that will contain all the invalid characters

@Returns:
Number of wildcard characters that were placed in the application provided
buffer.

@Compatibility: DOS, Win16 and Win32
*******************************************************************************/

UINT LOCAL GetIllegalWildcardChars (    // Fill buffer with wildcard chars
    LPTSTR  lpIllegals                  // [out] buffer to fill
    )
{
    SYM_ASSERT ( lpIllegals );

    lpIllegals[0] = '*';
    lpIllegals[1] = '?';

    return ( 2 );
}

/*@API:*************************************************************************
@Declaration:
UINT LOCAL GetIllegalPathChars (        // Fill buffer with path chars
    LPTSTR  lpIllegals                  // [out] buffer to fill
    );

@Description:
This function is called locally to add path characters to the list of
invalid characters.

@Parameters:

$lpIllegals$
    Pointer to application buffer that will contain all the invalid characters

@Returns:
Number of path characters that were placed in the application provided
buffer.

@Compatibility: DOS, Win16 and Win32
*******************************************************************************/

UINT LOCAL GetIllegalPathChars (        // Fill buffer with path chars
    LPTSTR  lpIllegals                  // [out] buffer to fill
    )
{
    SYM_ASSERT ( lpIllegals );

    lpIllegals[0] = '\\';
    lpIllegals[1] = '/';
    lpIllegals[2] = ':';

    return ( 3 );
}

/*@API:*************************************************************************
@Declaration:
LPBYTE LOCAL GetNextIllegalRange (      // Get the next range to check
    LPBYTE  lpIllegalCharArray,         // [in] array of range words
    LPBYTE  lpbyLow,                    // [out] from this range
    LPBYTE  lpbyHigh                    // [out] to that range
    );

@Description:
This function is called locally to obtain the next range of invalid characters.

@Parameters:

$lpIllegalCharArray$
    Pointer to an array of bytes that specified the start and beginning of
    invalid characters in the ascii table.

$lpbyLow$
    Pointer to a byte variable that will contain the start range of
    invalid characters.

$lpbyHigh$
    Pointer to a byte variable that will contain the end range of
    invalid characters.

@Returns:
Pointer to the next pair of invalid range indicators.

@Compatibility: DOS, Win16 and Win32
*******************************************************************************/

LPBYTE LOCAL GetNextIllegalRange (      // Get the next range to check
    LPBYTE  lpIllegalCharArray,         // [in] array of range words
    LPBYTE  lpbyLow,                    // [out] from this range
    LPBYTE  lpbyHigh                    // [out] to that range
    )
{
    SYM_ASSERT ( lpIllegalCharArray );
    SYM_ASSERT ( lpbyLow );
    SYM_ASSERT ( lpbyHigh );

    if ( lpIllegalCharArray[0] == 0 && lpIllegalCharArray[1] == 0 )
        {
        *lpbyLow    =   0;
        *lpbyHigh   =   0;
        }
    else
        {
        *lpbyLow    =   lpIllegalCharArray[0];
        *lpbyHigh   =   lpIllegalCharArray[1];

        lpIllegalCharArray += 2;
        }

    SYM_ASSERT ( *lpbyLow <= *lpbyHigh );

    return ( lpIllegalCharArray );
}




