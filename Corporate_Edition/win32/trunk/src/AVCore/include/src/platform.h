// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
/* Copyright 1992 - 2003, 2005 Symantec Corporation. All rights reserved.     */
/************************************************************************
 *                                                                      *
 * $Header:   S:/INCLUDE/VCS/platform.h_v   1.43   28 Sep 1998 15:33:50   ddrew  $ *
 *                                                                      *
 * Description:                                                         *
 *      Main Symantec Library header file for setting up a platform     *
 *      specific compilation.                                           *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * $Log:   S:/INCLUDE/VCS/platform.h_v  $ *
//
//    Rev 1.43   28 Sep 1998 15:33:50   ddrew
//
//    Rev 1.42   19 Jun 1997 17:12:30   RStanev
// Merged Kirin's DBCS changes for SYM_VXD.  Thanks, Davros!
//
//    Rev 1.41   31 May 1997 03:35:46   RStanev
// Enhanced SMY_VERIFY_STRING() and SYM_VERIFY_BUFFER().
//
//    Rev 1.40   20 Jan 1997 13:27:18   MSPIEGE
// Added SYM_NUKE macro to the debugging macro section.  When its condition
// is false, it will jump immediatly into the debugger.  It is defined only
// for the SYM_WIN platforms and will be useful in sorting out thread
// synchronization problems.
//
//
//    Rev 1.39   08 Jan 1997 14:32:54   BMCCORK
// Added _T() around character consts (e.g. EOS), and added more STR covers
//
//    Rev 1.38   27 Nov 1996 10:36:12   MSPIEGE
// 1) Moved Bruce's SymMsgWait... functions below the SYM_VERIFY code so
//    that it could be used in the new inline.
// 2) Added SymMsgSleep() to leverage the SymMsgWaitMultiple function and
//    allow broadcast messages to get through a Sleep() as well.
//
//
//    Rev 1.37   08 Nov 1996 16:25:44   MLELE
// Corrected #define for STRICMP for SYM_NLM
//
//    Rev 1.36   04 Nov 1996 17:31:08   SPAGLIA
// Added CharNext/CharPrev to OS/2
//
//    Rev 1.35   01 Nov 1996 12:05:42   JBRENNA
// Add SYM_ASSERT_CMD().
//
//    Rev 1.34   29 Oct 1996 16:30:40   HOKEKE
// For X86 and DOS platforms: added CharNext() and CharPrev() to cover
// AnsiNext() and AnsiPrev(). Implementations of AnsiNext() and AnsiPrev() are
// provided in dbcs.c
//
//    Rev 1.33   29 Oct 1996 15:32:56   JLAMB
// Removed C++ inlines for STRxxxx (they were broken under unicode)
//
//    Rev 1.33   29 Oct 1996 15:30:42   JLAMB
// Remove C++ inlines for STRxxx (they were breaking under unicode)
//
//    Rev 1.33   29 Oct 1996 15:29:50   JLAMB
// Removed C++ inlines for STRxxxx functions (breaking under unicode)
//
//    Rev 1.32   25 Oct 1996 16:14:08   BMCCORK
// "Made define of MBCS conditional"
//
//    Rev 1.31   25 Oct 1996 16:12:46   BMCCORK
// "Made define of MBCS conditional"
//
//    Rev 1.30   25 Oct 1996 11:22:20   BMCCORK
// Added _MBCS define for win32, non-unicode.
//
//
//    Rev 1.29   24 Oct 1996 17:02:40   BMCCORK
// Yet another fix to the os/2 stuff (sorry)
//
//    Rev 1.28   24 Oct 1996 15:51:20   BMCCORK
// Added more strXXX() covers for os/2, and Dave Buche's last change was to
// add UNREFERENCED_PARAMETER() macro calls to some of the SYM_MSG_WAIT_XXX()
// inlines.
//
//
//    Rev 1.27   24 Oct 1996 15:35:42   DBUCHES
//
//    Rev 1.26   23 Oct 1996 17:46:38   BMCCORK
// Added STRCHR() for OS2
//
//    Rev 1.25   23 Oct 1996 17:14:20   BMCCORK
// Fixed problem with _T macro
//
//    Rev 1.24   23 Oct 1996 15:25:32   BMCCORK
// Converted W32 str funcs to use _tc CRT funcs to cover multi-byte and unicode
//
//    Rev 1.23   23 Oct 1996 14:20:40   BMCCORK
// Fixed OS/2 stuff to have covers for all string functions and map them to crt
//
//    Rev 1.22   22 Oct 1996 18:40:54   BMCCORK
// Fixed problems in OS/2 (hopefully)
//
//    Rev 1.21   22 Oct 1996 15:36:08   BMCCORK
// Integrated David Alee's changes to fix problems which forced link to
// symkrnl.  Can now turn off use of DB safe string compare functions
// by defining NO_DBSTR_FUNCTIONS, which will cause things to revert
// to the way they were done before the change.  OS/2 is now
// back to the way it was before too.  W32 clients will use the CRT
// MB aware string functions.
//
//
//    Rev 1.20   18 Oct 1996 22:18:20   RSTANEV
// Fixed breakages introduced in Rev 1.18.
//
//    Rev 1.19   16 Oct 1996 19:26:12   DALLEE
// For SYM_WIN and SYM_DOS, added DBCS routines for strncpy, stricmp,
// strnicmp, and strstr.
//
// Other platforms should be unchanged, except for the addition of
// SBCS_STRxxx macros for consistency.
//
// Note custom quake users will need to add exports to their def files
// for the following: SYMstrncpy, SYMstricmp, SYMstrnicmp, SYMstrstr.
//
//    Rev 1.18   20 Sep 1996 16:30:40   RCHINTA
// Merging DBCS changes related to AnsiNext() and AnsiPrev()
// from Quake 9.
//
//    Rev 1.17   18 Sep 1996 14:53:28   BMCCORK
// Converted SymMsgWaitMultiple to return MsgWaitForMultipleObjects return codes
// and converted SymMsgWaitSingle to an inline/macro call to SymMsgWaitMultiple.
// Also added SYM_WAIT_XXX and SYM_WAIT_GET_XXX inline/macro helpers to
// interpret returns from SymMsgWaitMultiple() calls.
//
//    Rev 1.16   17 Sep 1996 18:53:20   BMCCORK
// Added SymMsgWaitSingle/Multiple() functions
//
//    Rev 1.15   17 Sep 1996 11:49:42   MLELE
//
//    Rev 1.14   22 Aug 1996 09:04:32   MLELE
// For NLM only: mapped STRCHR & STRRCHR to NW_StrChr & NW_StrRChr.
//
//    Rev 1.13   25 Jul 1996 11:11:04   BMCCORK
// Enabled SYM_PARANIOD
//
//    Rev 1.12   03 May 1996 16:43:12   BMCCORK
// Moved ALL code to within the #ifdef INCL_PLATFORM_H block
//
//    Rev 1.11   24 Apr 1996 17:54:58   BMCCORK
// Conditionally define TEXT only if it isn't already defined.
//
//    Rev 1.10   04 Apr 1996 09:41:06   RSTANEV
// Enhanced the SYM_VERIFY_STRING and SYM_VERIFY_BUFFER macros.
//
//    Rev 1.9   20 Mar 1996 21:52:08   FCHAPA
// Fixed unreferenced var warnings
//
//    Rev 1.8   14 Feb 1996 08:11:22   RSTANEV
// Added PCTSTR typedef.
//
//    Rev 1.7   07 Feb 1996 17:46:00   GDZIECI
// For WIN16: mapped CharNext() and CharPrev() to AnsiNext() and AnsiPrev().
//
//    Rev 1.6   07 Feb 1996 08:56:34   RSTANEV
// STRNICMP macro for VxD and NTK platforms now link to _strnicmp().
//
//    Rev 1.5   06 Feb 1996 19:27:16   RSTANEV
// SBCS_strrev() for SYM_VXD and SYM_NTK transfer control to _strrev(),
// which is the ANSI standard for strrev().  The other platforms still
// call strrev().
//
//    Rev 1.4   06 Feb 1996 18:19:44   RSTANEV
// Separated some STR* definintions for NTK & VXD platforms from the rest of
// the Core platforms.
//
//    Rev 1.3   06 Feb 1996 09:44:44   RSTANEV
// Added MemReAlloc() for SYM_VXD and SYM_NTK platforms
//
//    Rev 1.2   05 Feb 1996 08:31:16   RSTANEV
// No more name decoration of Core APIs for NTK platform.
//
//    Rev 1.1   30 Jan 1996 18:21:26   RSTANEV
// Synchronized SYM_VXD and SYM_NTK platforms.
//
//    Rev 1.0   26 Jan 1996 20:13:30   RJACKSO
// Initial revision.
//
//    Rev 1.241   24 Jan 1996 13:56:22   RSTANEV
// Added SYM_VERIFY_STRING() and SYM_VERIFY_BUFFER() macros.
//
//    Rev 1.240   17 Jan 1996 17:59:30   KYECK
// Added #ifndef _T so MFC users can build.
//
//    Rev 1.239   17 Jan 1996 10:40:06   RSTANEV
// Added partial unicode support
//
//    Rev 1.238   09 Jan 1996 19:29:38   ACHERNY
// Add SYM_DECLSPEC_THREAD macro
 ************************************************************************/

//***********************************************************************
// This file is formatted as follows.  Please try to retain this format.
//
//      * Platform defines
//      * Windows.H or equivalent processed
//      * Constants
//      * Modifiers
//      * New Types
//      * Memory/String Macros/functions
//      * Other Standard 'C' Macros/functions
//
//***********************************************************************
//***********************************************************************
// Set up platform define compatibilities.  Note that you should use
// the following table for platform defines (use the entry in the
// column marked "Correct":.  Aliases are only used for 3rd party
// headers/modules, which use the alternative name.  The aliases
// are set within here.  You should pass in the "correct" define.
// (NOTE: We set them to 1, since some usage is testing value, rather
// than if defined).
//**********************************************************************
//                                PLATFORM
//**********************************************************************
//
// Correct         Aliases                 Meaning
// -------         ---------------         ----------------------------
// SYM_WIN         _WINDOWS                Windows (WIN31, WIN32, WIN32s)
// SYM_DOS                                 DOS
// SYM_NLM                                 NetWare server
// SYM_OS2                                 OS/2
// SYM_MIRRORS                             OS/2, using Mirrors (Windows API)
// SYM_MAC                                 Macintosh
// SYM_VXD                                 VxD
// SYM_NTK                                 NT Kernel Mode
// SYM_UNIX                                anything unix generic, Solaris, AIX, etc.
//
//**********************************************************************
//                                SUB-PLATFORMS
//**********************************************************************
//
// Correct         Aliases                 Meaning
// -------         ---------------         ----------------------------
// SYM_WIN32       WIN32                   WIN32 API (SYM_WIN)
// SYM_WIN16                               16-bit-only Windows API (SYM_WIN)
// SYM_DOSX                                DOS-extended (SYM_DOS)
//
//
//**********************************************************************
//                                CPUS
//**********************************************************************
//
// Correct         Aliases                 Meaning
// -------         ---------------         ----------------------------
// SYM_CPU_X86      _X86_                  Intel computer
// SYM_CPU_MIPS     _MIPS_                 MIPS computer
// SYM_CPU_ALPHA    _ALPHA_                DEC Alpha computer
// SYM_CPU_POWERPC  _POWERPC_              PowerPC computer
// SOLARIS          _TSOL                  Solaris (what kind?)
//
//***********************************************************************

#ifndef INCL_PLATFORM
#define INCL_PLATFORM

//***********************************************************************
// UNIX Implementation
//
// The Unix version of the library is used by the NAV scanning engine,
// particularly PAM and NAVAPI.  Only the small subset of the library
// that is actually needed for NAV is implemented for Unix.
//***********************************************************************

#if defined (SYM_UNIX)

// the first thing that we define is the 'endianess' of the machine
// this is obviously platform dependent - please do not define this
// in makefiles, define only the platform, that way it sits in one place
                                                                        
#if defined(_TSOL)      // solaris?
    #ifndef BIG_ENDIAN
    #define BIG_ENDIAN
    #endif
#endif

#ifdef NO_TCSAPI

// the engine and navapi are insulated from tcsapi by the callback layer.

// Use the NAV header file avtypes.h to define the basic data types,
// constants, and string functions that are normally declared by this file.
// avtypes.h is also used by the portions of NAV that are totally isolated
// from this library (e.g. NAVEX).

#include "avtypes.h"

// Function declaration modifiers
#define WINAPI
#define SYM_EXPORT
#define STATIC static
#define LOCAL static

// Types for Mem functions
typedef void* HGLOBAL;

// Flags for MemAllocPtr()
#define GMEM_FIXED    0x0000
#define GMEM_MOVEABLE 0x0002
#define GMEM_ZEROINIT 0x0040
#define GHND (GMEM_MOVEABLE | GMEM_ZEROINIT)

// Memory functions
LPVOID MemAllocPtr(UINT uFlags, DWORD dwSize);
HGLOBAL MemFreePtr(LPVOID lpMem);

// Assert macros
#include "SavAssert.h"
#if defined(SYM_DEBUG)
  #define SYM_ASSERT(f) ((f) ? (void)0 : SAVASSERT(f))
  #define SYM_VERIFY(f) SYM_ASSERT(f)
#else
  #define SYM_ASSERT(f) ((void)0)
  #define SYM_VERIFY(f) ((void)(f))
#endif
#define SYM_VERIFY_STRING(s) ((void)0)
#define SYM_VERIFY_BUFFER(p,s) ((void)0)

// String function covers
#define SPRINTF    	sprintf
#define STRCMP      strcmp
#define STRNCMP    	strncmp
#define STRSPN    	strspn
#define STRSTR      strstr
#define STRCHR      strchr
#define STRRCHR     strrchr

// Misc. stuff
typedef WORD STATUS;
#define EOS     '\0'
#define NOERR   0
#define ERR     ((STATUS)(-1))   // Warning: NOT a UINT!

// Ignore the rest of platform.h.

#else // #ifdef NO_TCSAPI

#include "tcsdefs.h"

#define SYM_MAX_PATH MAX_PATH

#define STRLEN strlen
#define SPRINTF sprintf
#define STRCPY strcpy

// Ignore the rest of platform.h

#endif	// ifdef NO_TCSAPI

#else  // !defined(SYM_UNIX)

//---------------------------------------------------------------------
// Windows define compatibilities
//---------------------------------------------------------------------
#if defined(SYM_WIN) || defined(_WINDOWS)
#   if !defined(SYM_WIN)
#       define SYM_WIN          1
#   endif
#   if !defined(_WINDOWS)
#       define _WINDOWS         1
#   endif
#   if defined(SYM_WIN32) && !defined(WIN32)
#       define WIN32            1
#   endif
#   if !defined(SYM_WIN32) && !defined(SYM_WIN16)
#       define SYM_WIN16        1
#   endif
#endif

//---------------------------------------------------------------------
// Select character set choices in WIN32
//---------------------------------------------------------------------
#if defined(SYM_WIN32)
    #if !defined(UNICODE)
        #ifndef _MBCS
            #define _MBCS
        #endif
    #endif
#endif

//---------------------------------------------------------------------
// NLM define aliases (none)
//---------------------------------------------------------------------

//---------------------------------------------------------------------
// OS/2 define aliases (none)
//---------------------------------------------------------------------

//---------------------------------------------------------------------
// Macintosh define aliases (none)
//---------------------------------------------------------------------


//---------------------------------------------------------------------
// Mirrors define aliases
//---------------------------------------------------------------------
#if defined(SYM_MIRRORS)
#   if !defined(_WINDOWS)
#       define _WINDOWS         1
#   endif
#   if !defined(SYM_WIN)
#       define SYM_WIN          1
#   endif
#   if !defined(SYM_WIN16)
#       define SYM_WIN16        1
#   endif
#endif

//---------------------------------------------------------------------
// NT Kernel mode defines: force Unicode string manipulation
//---------------------------------------------------------------------
#if defined(SYM_NTK)

#if !defined(UNICODE)
#define UNICODE
#endif

#if !defined(_UNICODE)
#define _UNICODE
#endif

#endif

//---------------------------------------------------------------------
// NTK & VXD: all C++ RTL APIs are implemented locally, so tell RTL's
//            H-files to be careful with prototyping
//---------------------------------------------------------------------
#if defined(SYM_NTK) || defined(SYM_VXD)

#if defined(_CRTIMP)
#undef _CRTIMP
#endif

#define _CRTIMP

#endif

//---------------------------------------------------------------------
// If nothing is defined, then assume its DOS
//---------------------------------------------------------------------
#if !defined(SYM_WIN) && !defined(SYM_NLM) && !defined(SYM_OS2) && !defined(SYM_MIRRORS) && !defined(SYM_MAC) && !defined(SYM_DOS) && !defined(SYM_VXD) && !defined(SYM_NTK)
#   define SYM_DOS              1
#endif

//------------------------------------------------------------------------
// C++ shouldn't decorate the names of Core APIs for VxD & NTK platforms.
//------------------------------------------------------------------------
#if defined(__cplusplus) && (defined(SYM_VXD) || defined(SYM_NTK))
    extern "C" {
#endif

//---------------------------------------------------------------------
// Either 32-bit or not.  Set flag that can be used, rather than having
// to list each and every platform.
//---------------------------------------------------------------------
#if defined(SYM_WIN32) || defined(SYM_OS2) || defined(SYM_NLM) || defined(SYM_MAC) || defined(SYM_VXD) || defined(SYM_NTK)
#   define SYM_32BIT    1
#endif

//---------------------------------------------------------------------
// Set flag if protected mode.
//---------------------------------------------------------------------
#if defined(SYM_WIN16) || defined(SYM_DOSX)
#   define SYM_PROTMODE 1
#endif

//---------------------------------------------------------------------
// CPU define aliases
//---------------------------------------------------------------------
#if defined(SYM_CPU_X86)
#    define _X86_       1
#endif

#if defined(SYM_CPU_MIPS)
#    define _MIPS_      1
#endif

#if defined(SYM_CPU_ALPHA)
#    define _ALPHA_     1
#endif

#if defined(SYM_CPU_POWERPC)
#    define _POWERPC_   1
#endif

//***********************************************************************
// Read in the "Windows.H" file.  For non-Windows platforms, there should
// be an equivalent.
//***********************************************************************
//-----------------------------------------------------------------
// Turn off all this stuff for non-Windows compiles
//-----------------------------------------------------------------
#if !defined(SYM_WIN)

//#define NOMINMAX        // min() and max() macros
#define NOLOGERROR      // LogError() and related definitions
#define NOPROFILER      // Profiler APIs
//#define NOMEMMGR        // Local and global memory management
//#define NOLFILEIO       // _l* file I/O routines
//#define NOOPENFILE      // OpenFile and related definitions
#define NORESOURCE      // Resource management
//#define NOATOM          // Atom management
//#define NOLANGUAGE      // Character test routines
//#define NOLSTRING       // lstr* string management routines
#define NODBCS          // Double-byte character set routines
#define NOKEYBOARDINFO  // Keyboard driver routines
#define NOGDICAPMASKS   // GDI device capability constants
#define NOCOLOR         // COLOR_* color values
//#define NOGDIOBJ        // GDI pens, brushes, fonts
#define NODRAWTEXT      // DrawText() and related definitions
#define NOTEXTMETRIC    // TEXTMETRIC and related APIs
#define NOSCALABLEFONT  // Truetype scalable font support
#define NOBITMAP        // Bitmap support
#define NORASTEROPS     // GDI Raster operation definitions
#define NOMETAFILE      // Metafile support
#define NOSYSMETRICS    // GetSystemMetrics() and related SM_* definitions
#define NOSYSTEMPARAMSINFO      // SystemParametersInfo() and SPI_* definitions
#define NOMSG           // APIs and definitions that use MSG structure
#define NOWINSTYLES     // Window style definitions
#define NOWINOFFSETS    // Get/SetWindowWord/Long offset definitions
#define NOSHOWWINDOW    // ShowWindow and related definitions
#define NODEFERWINDOWPOS        // DeferWindowPos and related definitions
#define NOVIRTUALKEYCODES       // VK_* virtual key codes
#define NOKEYSTATES     // MK_* message key state flags
#define NOWH            // SetWindowsHook and related WH_* definitions
#define NOMENUS         // Menu APIs
#define NOSCROLL        // Scrolling APIs and scroll bar control
#define NOCLIPBOARD     // Clipboard APIs and definitions
#define NOICONS         // IDI_* icon IDs
//#define NOMB            // MessageBox and related definitions
#define NOSYSCOMMANDS   // WM_SYSCOMMAND SC_* definitions
#define NOMDI           // MDI support
#define NOCTLMGR        // Control management and controls
#define NOWINMESSAGES   // WM_* window messages
#define NOHELP          // Help support

#endif  //  if !defined(SYM_WIN)

#ifndef STRICT
#   define STRICT 1                     // We use STRICT type-checking
#endif

#if defined(SYM_NLM)                    // if NLM, don't use Windows.h
#   include "platnlm.h"
#elif defined(SYM_OS2)                  // If OS/2, don't use Windows.h
#   include "os2win.h"
#   define  _OS2                        //  For compatibility with older code
#elif defined(SYM_VXD)                  // If VxD...
#   undef NOCTLMGR
#   include "platvxd.h"
#elif defined(SYM_NTK)
#   include "platntk.h"
#elif defined(SYM_MAC)
#   include "platmac.h"
#else
#   if defined(SYM_MFC)
#      undef STRICT                     // MFC will define STRICT
#      include <afxwin.h>               // MFC libraries use different Win.h
#   else
#      include <windows.h>              // Windows (all variants) and DOS
#   endif
#   if defined(SYM_WIN)
#      include <shellapi.h>             // Drag/drop; other important stuff
#      include <windowsx.h>             // Message crackers, etc
#      include "platwin.h"              // Stuff used by all windows apps
#      include "symvxd.h"               // SYMANTEC VxD IDs
#   elif defined(SYM_DOSX)
#      define _AFX          /* Avoid MS C++ 7.0 Class collision */
#      include <dos16.h>                // DOS-extender header
typedef struct
    {
    unsigned short es;
    unsigned short ds;
    unsigned short di;
    unsigned short si;
    unsigned short bp;
    unsigned short sp;
    unsigned short bx;
    unsigned short dx;
    unsigned short cx;
    unsigned short ax;
    unsigned short ip;
    unsigned short cs;
    unsigned short flags;
    } REGS16;
#   endif
#endif                                  // everything else remaining

// For most platforms, AnsiPrev() and AnsiNext() have been prototyped
// in windows.h but not #defined.  Other platforms may or may not have
// prototyped or #defined them in their equivalent header files.
// For the SYM_WIN platform, we will use the Windows definitions of
// these functions.  For the DOS platforms (Both DBCS and non-DBCS),
// we will use our own definitions in symkrnl\dbcs.c.  For other
// platforms, we will #define AnsiPrev() and AnsiNext() here if they
// are not already defined

#if defined(SYM_DOS)
#   if defined(AnsiPrev) || defined(AnsiNext)
#      error AnsiPrev or AnsiNext defined for DOS.
#   endif
#	define	CharNext(s) 	AnsiNext(s)
#	define	CharPrev(d,s) 	AnsiPrev(d,s)
#elif !defined(SYM_WIN) && !defined(SYM_VXD) && !defined(AnsiNext)
#   define AnsiNext(lp)      ((*((LPTSTR)lp)!='\0') ? (((LPTSTR)lp)+1) : ((LPTSTR)lp))
#   define AnsiPrev(lpS,lpC) (((LPTSTR)lpC>(LPTSTR)lpS) ? (((LPTSTR)lpC)-1) : ((LPTSTR)lpS))
#endif

#if defined(SYM_WIN16) || defined(SYM_OS2)
#define	CharNext 	AnsiNext
#define	CharPrev 	AnsiPrev
#endif


//*********************************************************************
// Maximum size of things
//*********************************************************************

#define SYM_MAX_PATH            260     /* max. length of full pathname     */
#define SYM_MAX_DRIVE           3       /* max. length of drive component   */

#define SYM_MAX_USER_INFO       40      // Max size of company and user name
#define SYM_MAX_DOS_DRIVES      26      // Drive 'A' through Drive 'Z'

#define SYM_MAX_INPUT_LEN       256
#define SYM_MAX_STRTABLE_LEN    256

//***********************************************************************
// Modifiers
// For 32-bit environments (NLM, WIN32, OS/2), most of these don't
// make sense.
//***********************************************************************

#if defined(SYM_32BIT)
#   define LOADDS
#   if defined(SYM_WIN32)
#       if defined(QCUSTNOX)
#           define SYM_EXPORT
#       else
#           define SYM_EXPORT       __declspec(dllexport)
#       endif
#       define SYM_DECLSPEC_THREAD  __declspec(thread)
#   else
#       if defined(SYM_OS2)
#           define SYM_EXPORT       _export
#       else
#           define SYM_EXPORT
#       endif
#       define SYM_DECLSPEC_THREAD
#   endif
#   if !defined(SYM_OS2)
#       undef  __export
#       define __export
#       undef  _export
#       define _export
#   endif
#   undef  __interrupt
#   define __interrupt
#   undef  _interrupt
#   define _interrupt
#   undef  interrupt
#   define interrupt
#   undef  __loadds
#   define __loadds
#if !defined(SYM_OS2)
#   undef  _loadds
#   define _loadds
#   undef  _huge
#   define _huge
#   undef  huge
#   define huge
#   undef  _near
#   define _near
#   undef  near
#   define near
#endif
#   undef  __huge
#   define __huge
#   undef  __near
#   define __near
#   undef  __far
#   define __far
#   undef  _far
#   define _far
#   undef  far
#   define far
#else
#   if _MSC_VER >= 700
#       define LOADDS        __loadds
#       define SYM_EXPORT    __export
#   else
#       define LOADDS        _loadds
#       define SYM_EXPORT    _export
#   endif
#   if !defined(SYM_WIN)
#       undef SYM_EXPORT
#       define SYM_EXPORT
#   endif
#endif

#ifndef RC_INVOKED
    #define CONST   const
    #define STATIC  static
#endif

#if !defined(LOCAL)
    #if defined(SYM_DOS)
    #   define LOCAL   static
    #else
    #   define LOCAL   static near
    #endif
#endif
#if !defined(HUGE)
#   define HUGE    _huge
#endif

//-----------------------------------------------------------------
// Make NEAR blank for X86 platform only.
#if defined(SYM_X86)
    #undef NEAR
    #define NEAR

                                        // This must be hard-coded for
                                        // _near because SYMKRNL assumes
                                        // this to be a sixteen-bit value.
    #undef DECLARE_HANDLE
    #define DECLARE_HANDLE(name)    struct name##__ { int unused; }; \
                                    typedef const struct name##__ _near* name
#endif

//-----------------------------------------------------------------
//-----------------------------------------------------------------
#ifdef __cplusplus                      // Compiling for C++?
#   define EXTERNC extern "C"           // Prototype modifier to force C
#else                                   // Not compiler for C++
#   define EXTERNC                      // Prototype modifier not valid in C
#endif

//-----------------------------------------------------------------
// How to define global variables that are to be referenced by others.
//-----------------------------------------------------------------
#if defined(MAIN)       /* MAIN should be defined iff allocations should */
#   define EXTERN       /* take place in that module.  It obviously must */
#else                   /* be defined before including this header.      */
#   define EXTERN extern
#endif


//*********************************************************************
// Macro for defining pointers to structures
//*********************************************************************
#define TYPEDEF_POINTERS(base, type)            \
typedef base *                  P##type;        \
typedef const base *            PC##type;       \
typedef base NEAR *             NP##type;       \
typedef const base NEAR *       NPC##type;      \
typedef base FAR *              LP##type;       \
typedef const base FAR *        LPC##type;


//*********************************************************************
// Types not found in Windows.h
//*********************************************************************

#ifndef SYM_OS2
    typedef BYTE NEAR       *NPBYTE;
#endif
typedef const BYTE FAR  *LPCBYTE;
typedef BYTE HUGE       *HPBYTE;
typedef const BYTE HUGE *HPCBYTE;
typedef WORD HUGE       *HPWORD;
typedef char HUGE       *HPSTR;
typedef const WORD HUGE *HPCWORD;
#if !defined(SYM_DOSX)
typedef VOID HUGE       *HPVOID;
#endif
typedef const VOID HUGE *HPCVOID;

#ifndef SYM_MAC
TYPEDEF_POINTERS(VOID, VOID)
TYPEDEF_POINTERS(HWND, HWND)
TYPEDEF_POINTERS(HINSTANCE, HINSTANCE)
TYPEDEF_POINTERS(HMODULE, HMODULE)
TYPEDEF_POINTERS(LRESULT, LRESULT)
TYPEDEF_POINTERS(HGLOBAL, HGLOBAL)
TYPEDEF_POINTERS(HLOCAL, HLOCAL)
TYPEDEF_POINTERS(HFILE, HFILE)
TYPEDEF_POINTERS(UINT, UINT)
#endif //SYM_WIN

typedef signed short int SWORD;         // 16-bit signed
typedef signed long int  SDWORD;        // 32-bit signed
typedef SWORD           SHORTBOOL;      // 16-bit BOOL - use for size dependent declarations

#ifndef SYM_MAC
TYPEDEF_POINTERS(SWORD, SWORD)
TYPEDEF_POINTERS(SDWORD, SDWORD)
TYPEDEF_POINTERS(BOOL, BOOL)
TYPEDEF_POINTERS(ATOM, ATOM)
TYPEDEF_POINTERS(LPSTR, LPSTR)
TYPEDEF_POINTERS(WPARAM, WPARAM)
TYPEDEF_POINTERS(LPARAM, LPARAM)
typedef NPSTR FAR       *LPNPSTR;
#endif //SYM_WIN


#ifdef SYM_NLM          // these definitions differ between NetWare and others

//
// various NetWare specific handle types. Some of these definitions reflect
// actual types defined by NetWare, although NetWare does not define them
// as types, but rather as direct declarations. Do not change these to
// something else.

typedef WORD            HSYMWND;    // NWND window handle
typedef int             HTHREAD;    // matches NLM definition
typedef int             HPROCESS;   // matches NLM definition
typedef WORD            HSYMSTR;    // string group handle
typedef int             HPORT;      // AIO port handle
typedef long            HSEMAPHORE; // NLM network semaphore

TYPEDEF_POINTERS(HSYMWND, HSYMWND)
TYPEDEF_POINTERS(HSYMSTR, HSYMSTR)

#else

typedef int             HTHREAD;
typedef HANDLE          HPORT;
typedef long            HSEMAPHORE;

#endif

TYPEDEF_POINTERS(HTHREAD, HTHREAD)
TYPEDEF_POINTERS(HPORT, HPORT)
TYPEDEF_POINTERS(HSEMAPHORE, HSEMAPHORE)

typedef WORD            (* wPROC)();  // no arguments since I can't know them
typedef DWORD           (*dwPROC)();  // no arguments since I can't know them
typedef BYTE            (*byPROC)();  // no arguments since I can't know them
typedef VOID            (* vPROC)();  // no arguments since I can't know them

#if !defined(SYM_OS2) && defined(SYM_WIN)
    typedef char            CHAR;
    TYPEDEF_POINTERS(HFONT, HFONT)
#endif

//-----------------------------------------------------------------
// These types are yanked directly from WINNT.H, so that other
// platforms can manage UNICODE objects, like the disk structures
// in Chicago.
//-----------------------------------------------------------------
//
// UNICODE (Wide Character) types
//

#include <stddef.h>                     // get wchar_t def

typedef wchar_t WCHAR;    // wc,   16-bit UNICODE character

typedef WCHAR *PWCHAR;
typedef WCHAR FAR *LPWCH;
typedef WCHAR *PWCH;
typedef CONST WCHAR FAR *LPCWCH;
typedef CONST WCHAR *PCWCH;
typedef WCHAR *NWPSTR;
typedef WCHAR FAR *LPWSTR;
typedef WCHAR *PWSTR;

typedef CONST WCHAR FAR *LPCWSTR;
typedef CONST WCHAR *PCWSTR;

//
// Neutral ANSI/UNICODE types and macros
//
#ifdef  UNICODE

#ifndef _TCHAR_DEFINED
typedef WCHAR TCHAR, *PTCHAR;
typedef WCHAR TBYTE , *PTBYTE ;
#define _TCHAR_DEFINED
#endif /* !_TCHAR_DEFINED */

typedef LPWSTR LPTCH, PTCH;
typedef LPWSTR PTSTR, LPTSTR;
typedef LPCWSTR PCTSTR, LPCTSTR;
typedef LPWSTR LP;

#ifndef __TEXT
#define __TEXT(quote) L##quote
#endif //__TEXT

#else   /* UNICODE */

#ifndef _TCHAR_DEFINED
typedef char TCHAR, *PTCHAR;
typedef unsigned char TBYTE , *PTBYTE ;
#define _TCHAR_DEFINED
#endif /* !_TCHAR_DEFINED */

typedef LPSTR LPTCH, PTCH;
typedef LPSTR PTSTR, LPTSTR;
typedef LPCSTR PCTSTR, LPCTSTR;
#define __TEXT(quote) quote

#endif /* UNICODE */

#ifndef TEXT
#define TEXT(quote) __TEXT(quote)
#endif // TEXT

#ifndef _T
    #define _T(quote)   __TEXT(quote)
#endif

#ifndef TEXT
#ifndef RC_INVOKED
    #define TEXT(quote) __TEXT(quote)
#endif
#endif


//*********************************************************************
// Constants
//*********************************************************************
#ifndef ABORT
#   define ABORT        -2              /* Abort the operation       */
#endif

#ifndef EOS
#   define EOS          _T('\0')        /* End Of String             */
#endif

#if defined(SYM_VXD) || defined(SYM_NTK)
typedef UINT            STATUS;
#else
typedef WORD            STATUS;
#endif

#ifndef NOERR
#   define NOERR        0               /* No error.                 */
#endif

#ifndef ERR
#   define ERR          ((STATUS) -1)   /* Generic error condition.  */
#endif

#ifndef EOF
#   define EOF          (-1)            /* Char returned at end of file */
#endif

#ifndef EOF_Z
#   define EOF_Z        26              /* DOS end of file marker    */
#endif

#ifndef PATH_CHAR
#   define PATH_CHAR    _T('\\')
#endif

#ifndef MODULE_NAME
#   define MODULE_NAME
#endif

#ifndef INVALID_HANDLE_VALUE
#   define INVALID_HANDLE_VALUE (HANDLE)-1
#endif

//***********************************************************************
// Macros for segment:offset manipulation.
//***********************************************************************
#ifndef MAKELP
#ifdef SYM_NLM

    // NLM uses linear addresses, not segment offset

#   define MAKELP(sel, off)    ((void FAR*)(sel*16 + off))
#else
#   define MAKELP(sel, off)    ((void FAR*)MAKELONG((off), (sel)))
#endif
#endif

#ifndef __WATCOMC__

#ifndef FP_SEG
#   define FP_SEG(fp)          (*((WORD __far *)&(fp)+1))
#endif

#ifndef FP_OFF
#   define FP_OFF(fp)          (*((WORD __far *)&(fp)))
#endif

#endif	// __WATCOMC__

//***********************************************************************
// Memory Management
//***********************************************************************

#include <string.h>                     // Simplifies our macros
#include "dbcs_str.h"

#define NO_PAGE_LOCK    0               // 0 == page lock failed


        // ************************************
        // Shared Memory Management Covers
        // ************************************
#ifdef SYM_OS2

#	include <malloc.h>

#ifdef __cplusplus

inline LPVOID   MemLock(HGLOBAL hHandle)
{
        return (LPVOID) hHandle;
}

inline UINT             MemPageLock(HGLOBAL hHandle)
{
        return (UINT) 1;
}

inline LPVOID   MemLockLow(HGLOBAL hHandle)
{
        return (LPVOID) hHandle;
}

inline HGLOBAL  MemFree(HGLOBAL hHandle)
{
        free((void *) hHandle);
        return (HGLOBAL) hHandle;
}

inline HGLOBAL  MemForceFree(HGLOBAL hHandle)
{
        free((void *) hHandle);
        return (HGLOBAL) hHandle;
}

inline UINT             MemFlags(HGLOBAL hHandle)
{
        return (UINT) 0;
}

inline DWORD    MemSize(HGLOBAL hHandle)
{
        return (DWORD) _msize((void *) hHandle);
}

inline DWORD    MemCompact(DWORD dwMinFree)
{
        return (DWORD) dwMinFree;
}

inline HGLOBAL  MemPtrHandle(LPVOID     lpMemBlk)
{
        return (HGLOBAL) lpMemBlk;
}

inline HGLOBAL  MemHandleFromPtr(LPVOID lpMemBlk)
{
        return (HGLOBAL) lpMemBlk;
}

inline HGLOBAL  MemAllocFixed(DWORD dwSize)
{
        return (HGLOBAL) malloc(dwSize);
}

inline HGLOBAL  MemFreeFixed(HGLOBAL hHandle)
{
        free((void *) hHandle);
        return (HGLOBAL) hHandle;
}

inline BOOL MemUnlock(HGLOBAL hHandle)
{
    return TRUE;
}

inline BOOL             MemUnlock(HGLOBAL hHandle, LPVOID lpMem)
{
        return TRUE;
}

inline BOOL             MemPageUnlock(HGLOBAL hHandle)
{
        return TRUE;
}

inline BOOL             MemRelease(HGLOBAL hHandle)
{
        return TRUE;
}

inline HGLOBAL  MemAlloc(UINT uFlags, DWORD dwSize)
{
        return (HGLOBAL) malloc(dwSize);
}

inline HGLOBAL  MemReAlloc(HGLOBAL hHandle, DWORD dwSize, UINT uFlags)
{
        return (HGLOBAL) realloc((void *) hHandle, dwSize);
}

inline BOOL             MemForceSwapOut(HGLOBAL hHandle)
{
        return TRUE;
}

inline LPVOID   MemAllocPtr(UINT uFlags, DWORD dwSize)
{
        return (LPVOID) malloc(dwSize);
}

inline LPVOID   MemReAllocPtr(LPVOID lpMem, DWORD dwSize, UINT uFlags)
{
        return (LPVOID) realloc(lpMem, dwSize);
}

inline HGLOBAL  MemFreePtr(LPVOID lpMem)
{
        free(lpMem);
        return (HGLOBAL) lpMem;
}

#else

#     define MemLock(h)              (void *)(h)
#     define MemPageLock(h)          (1)             // does nothing but succeed
#     define MemLockLow(h)           (void *)(h)
#     define MemFree(h)              MemFreePtr((void *)h)
#     define MemForceFree(h)         free((void *)h)
#     define MemFlags(h)
#     define MemSize(h)              _msize((void *)h)
#     define MemCompact(s)           /* No such requirement in NLM */
#     define MemPtrHandle(p)         (p)
#     define MemHandleFromPtr(p)     (p)
#     define MemAllocFixed(s)        malloc(s)
#     define MemFreeFixed(p)         free(p)
#     define MemUnlock(h,p)          /* No such requirement in NLM */
#     define MemPageUnlock(h)        /* No such requirement in NLM */
#     define MemRelease(h)           /* No such requirement in NLM */
#     define MemAlloc(f,s)           MemAllocPtr(f,s)
#     define MemReAlloc(h,s,f)       MemReAllocPtr(h,s,f)
#     define MemForceSwapOut(h)      /* No such requirement in NLM */

// these are covers for OS/2

#     define MemAllocPtr(f,s)           malloc(s)
#         define MemReAllocPtr(h,s,f)   realloc(h, s)
#     define MemFreePtr(h)                      free((void *) h)

#endif  /* __cplusplus */

#else   /* SYM_OS2 */

#ifndef SYM_MAC
    #if !defined(SYM_NLM) && !defined(SYM_VXD) && !defined(SYM_NTK)
        EXTERNC HGLOBAL SYM_EXPORT WINAPI MemReAlloc (HGLOBAL hMem, DWORD dwBytes, UINT wFlags);
        EXTERNC VOID SYM_EXPORT WINAPI MemAvail (DWORD FAR *lpdwFree, DWORD FAR *lpdwLargest);
    #   if !defined(SYM_WIN)
            EXTERNC HGLOBAL SYM_EXPORT WINAPI MemAlloc (UINT wFlags, DWORD dwBytes);
    #   endif                               // if !defined(SYM_WIN)
    #endif                                  // if !defined(SYM_NLM) && !defined(SYM_VXD) && !defined(SYM_NTK)
#endif

#if defined(SYM_WIN)                   // Memory management.

        // ************************************
        // Windows C++ Memory Management Covers
        // ************************************

#  ifdef __cplusplus                      // Windows, C++.

#if defined(SYM_WIN32)
                                        // C++, WIN32:

                                        // Function no longer supported.
                                        // If need to, use VirtualPageLock().
   inline UINT MemPageLock(HGLOBAL hMem)
        {  return (1); hMem; }

#  define MemPageUnlock(h)              // No such requirement in WIN32?

                                        // In Quake Win32, memory is automatically
   inline LPVOID MemLock(HGLOBAL hHandle)
        { return (LPVOID)(hHandle);}     // locked with MemAlloc.  MemLock simply
                                         // returns the handle casted as LPVOID

   inline BOOL MemUnlock(HGLOBAL h)
       { return TRUE; h; }

   inline BOOL MemUnlock(HGLOBAL h, LPVOID FAR & lpMem)
       { lpMem = NULL;
         return TRUE; h;}

   inline BOOL MemRelease(HGLOBAL h)
       { return TRUE; h;}

                                        // Return the MEM handle associated
                                        // with the given pointer, which
                                        // in this case is the same thing.
   inline HGLOBAL MemPtrHandle(LPVOID p)
      {
      return (HGLOBAL)p;
      }
/*-----------------12-16-94 04:39pm-----------------
 MemReturnGlobalHandle returns a real Handle
 when passed a (HANDLE)LPVOID.
--------------------------------------------------*/
    inline HGLOBAL MemReturnGlobalHandle(HGLOBAL hHandle)
        { return GlobalPtrHandle((LPVOID)hHandle);}

/*-----------------12-15-94 04:17pm BCF-----------------
 These two functions are located in PRM_MEM.c

 MemAlloc uses GlobalAlloc and GlobalLock to
 automatically lock memory when it is allocated.

 Note: MemAlloc returns an lpVoid casted as a handle.

 MemFree uses GlobalUnlock and GlobalFree to
 free memory allocated with MemAlloc.
-------------------------------------------------------*/
extern "C" {
HANDLE SYM_EXPORT WINAPI MemAlloc(UINT, DWORD);
HANDLE SYM_EXPORT WINAPI MemFree(HGLOBAL);
}

#else                                   // if defined(SYM_WIN32)
                                        // C++, NON WIN32:
   inline UINT MemPageLock(HGLOBAL hMem)
       { return GlobalPageLock(hMem); }

   inline BOOL MemPageUnlock(HGLOBAL h)
       { return GlobalPageUnlock(h); }

   inline HGLOBAL MemAlloc(UINT uFlags, DWORD dwBytes)
       { return GlobalAlloc(uFlags, dwBytes); }

   inline HGLOBAL MemFree(HGLOBAL h)
       { return GlobalFree(h); }

   inline LPVOID MemLock(HGLOBAL hMem)
       { return GlobalLock(hMem); }

   inline BOOL MemUnlock(HGLOBAL h)
       { return GlobalUnlock(h); }

                                        // To pass references to pointers
                                        // of types other than LPVOID,
                                        // cast them to (LPVOID FAR &), NOT
                                        // (LPVOID)!
   inline BOOL MemUnlock(HGLOBAL h, LPVOID FAR & lpMem)
       { lpMem = NULL;
         return GlobalUnlock(h); }

   inline BOOL MemRelease(HGLOBAL h)
       { return GlobalUnlock(h); }


                                        // Return the MEM handle associated
                                        // with the given pointer.
   inline HGLOBAL MemPtrHandle(LPVOID lp)
       {
       return (GlobalPtrHandle(lp));
       }


   inline HGLOBAL MemReturnGlobalHandle(HGLOBAL hMem)
      {
      return hMem;
      }

#endif                                  // if/else defined(SYM_WIN32)

                                        // C++, SYM_WIN:

                                        // The following are for all
                                        // platforms.

   inline UINT MemFlags(HGLOBAL h)
       { return GlobalFlags(MemReturnGlobalHandle(h)); }


   inline DWORD MemSize(HGLOBAL h)
       { return GlobalSize(MemReturnGlobalHandle(h)); }

   inline DWORD MemCompact(DWORD dwMinFree)
       { return GlobalCompact(dwMinFree); }


#  if !defined(SYM_MIRRORS)

      inline LPVOID MemAllocPtr(UINT uFlags, DWORD dwSize)
          { return MemLock(MemAlloc(uFlags, dwSize)); }

      inline LPVOID MemReAllocPtr(LPVOID lp, DWORD dwNewSize, UINT uFlags)
       {
         HGLOBAL h = MemPtrHandle(lp);
         MemUnlock(h);
         return MemLock(MemReAlloc(h, dwNewSize, uFlags));
       }

      inline HGLOBAL MemFreePtr(LPVOID lp)
          {
         HGLOBAL h = MemPtrHandle(lp);
         MemUnlock(h);
         return (MemFree(h));
       }

#  else  // SYM_MIRRORS
   // --------------------------------------------------------------

#     include <malloc.h>
      // Use of _fmalloc allows for suballocation in OS/2
      // GlobalAllocPtr with GHND flags does this, but it
      // GlobalHandle will not work on suballocated memory,
      // leaving us with no GlobalUnlockPtr and GlobalReAllocPtr

      extern "C" {
      LPVOID WINAPI MirAllocPtr(UINT uFlags, DWORD dwSize);
      LPVOID WINAPI MirReAllocPtr(LPVOID lp, DWORD dwNewSize, UINT uFlags);
      HGLOBAL WINAPI MirFreePtr(LPVOID lp);
      }
#     define MemAllocPtr MirAllocPtr
#     define MemReAllocPtr MirReAllocPtr
#     define MemFreePtr MirFreePtr

// --------------------------------------------------------------
#  endif // SYM_MIRRORS, still under __cplusplus and SYM_WIN


   inline BOOL MemFreeHandlePtr( HGLOBAL *lphHandle )
       {
       if ( MemFree( *lphHandle ) == NULL )
           {
           *lphHandle = NULL;
           return( TRUE );
           }
       else
           return( FALSE );
       }

   inline BOOL MemForceSwapOut(HGLOBAL hMem)
       {
       // Null function under Windows

       (void) hMem;

       return TRUE;
       }

        // ************************************
        // Windows C Memory Management Covers
        // ************************************

#   else      // Windows, C.  Still under #ifdef SYM_WIN also
                                        // Non-C++:
#if defined(SYM_WIN32)
                                        // WIN32, NON C++
#  define MemPageLock(h)          (1)   // No need for WIN32?
#  define MemPageUnlock(h)              // No need for WIN32?
#  define MemLock(h)             ((LPVOID)(h))// Memory is automatically locked with MemAlloc.
#  define MemUnlock(h, p)        ((p) = NULL) // Memory is automatically unlocked with MemFree.
#  define MemRelease(h)          (h)    // Memory is automatically unlocked
                                        // The pointer IS the handle for
                                        // MemAlloc'd memory in WIN32
#  define MemPtrHandle(lp)       ((HGLOBAL)(lp))

/*-----------------12-16-94 04:39pm-----------------
 MemReturnGlobalHandle returns a real Handle
 when passed a (HANDLE)LPVOID.
--------------------------------------------------*/
#define MemReturnGlobalHandle(h) GlobalPtrHandle((LPVOID)(h))

/*-----------------12-15-94 04:17pm BCF-----------------
 These two functions are located in PRM_MEM.c

 MemAlloc uses GlobalAlloc and GlobalLock to
 automatically lock memory when it is allocated.

 Note: MemAlloc returns an lpVoid casted as a handle.

 MemFree uses GlobalUnlock and GlobalFree to
 free memory allocated with MemAlloc.
-------------------------------------------------------*/
HGLOBAL SYM_EXPORT WINAPI MemAlloc(UINT, DWORD);
HGLOBAL SYM_EXPORT WINAPI MemFree(HGLOBAL);

#else                                   // if defined(SYM_WIN32)
                                        // The following is for NON-WIN32
                                        // NON C++
#  define MemAlloc                GlobalAlloc
#  define MemFree                 GlobalFree
#  define MemLock                 GlobalLock
#  define MemUnlock(h, p)         (p = NULL, GlobalUnlock(h))
#  define MemRelease(h)           GlobalUnlock(h)
#  define MemPageLock(h)          GlobalPageLock(h)
#  define MemPageUnlock(h)        GlobalPageUnlock(h)
#  define MemReturnGlobalHandle(h)    (h)
#  define MemPtrHandle(lp) \
      GlobalPtrHandle(lp)
#endif                                  // if defined(SYM_WIN32)/#else
                                        // The following is for both
                                        // WIN16 & WIN32, NON C++

#  define MemLockLow              MemLock
#  define MemFlags(h)             GlobalFlags(MemReturnGlobalHandle(h))

#  define MemSize(h)              GlobalSize(MemReturnGlobalHandle(h))
#  define MemCompact              GlobalCompact

                                        // Helper macros for MemReAllocPtr
                                        // and MemFreePtr.
#  define MemUnlockPtr(lp) \
      MemRelease(MemPtrHandle(lp))
#  define MemAllocPtr(flags, cb) \
      (MemLock(MemAlloc((flags), (cb))))
#  define MemReAllocPtr(lp, cbNew, flags) \
      (MemUnlockPtr(lp), MemLock(MemReAlloc(MemPtrHandle(lp) , (cbNew), (flags))))
#  define MemFreePtr(lp) \
      (MemUnlockPtr(lp), (BOOL)MemFree(MemPtrHandle(lp)))
#  define MemFreeHandlePtr(lph) \
      (MemFree(*lph), *lph=NULL, TRUE ) // always return TRUE
#  define MemForceSwapOut(h) (TRUE)        // Null function under Windows

#endif  // __cplusplus.  Still under #if defined(SYM_WIN)

        // **************************************************
        // Windows Shared (C && C++) Memory Management Covers
        // **************************************************

#  define MemForceFree            MemFree
#  define MemHandleFromPtr        MemPtrHandle
#  define MemAllocFixed(s)        MemAllocPtr(GHND, s)
#  define MemFreeFixed            MemFreePtr

        // End of Windows Memory section.

#else    // if defined(SYM_WIN) failed

#     if defined(SYM_VXD) || defined(SYM_NTK)

        PVOID MemAllocPtr ( UINT uFlags, DWORD dwBytes );
        PVOID MemReAllocPtr ( PVOID lpMemory, DWORD dwSize, UINT uMemType);
        PVOID MemFreePtr ( PVOID pvMemory );

#       if defined(SYM_NTK)
#           define MemAllocPtr(f,b)     MemAllocPtrEx(f,b,FALSE)
#           define MemFreePtr(p)        MemFreePtrEx(p,FALSE)
#           define MemAllocPtrX(f,b)    MemAllocPtrEx(f,b,TRUE)
#           define MemFreePtrX(p)       MemFreePtrEx(p,TRUE)
#       else
#           define MemAllocPtrX(f,b)    MemAllocPtr(f,b)
#           define MemFreePtrX(p)       MemFreePtr(p)
#       endif


        #define MemAlloc(f,s)     MemAllocPtr(f,s)
        #define MemReAlloc(p,s,f) MemReAllocPtr(p,s,f)
        #define MemFree(h)        MemFreePtr(h)
        #define MemLock(h)        ((PVOID)h)
        #define MemUnlock(h,p)    ((p=NULL)==NULL)
        #define MemRelease(h)     (1==1)

#     elif defined(SYM_NLM)

        // ****************************
        // NLM Memory Management Covers
        // ****************************

#     include <malloc.h>

#     define MemLock(h)              (void *)(h)
#     define MemPageLock(h)          (1)             // does nothing but succeed
#     define MemLockLow(h)           (void *)(h)
#     define MemFree(h)              MemFreePtr((void *)h)
#     define MemForceFree(h)         free((void *)h)
#     define MemFlags(h)
#     define MemSize(h)              _msize((void *)h)
#     define MemCompact(s)           /* No such requirement in NLM */
#     define MemPtrHandle(p)         (p)
#     define MemHandleFromPtr(p)     (p)
#     define MemAllocFixed(s)        malloc(s)
#     define MemFreeFixed(p)         free(p)
#     define MemUnlock(h,p)          /* No such requirement in NLM */
#     define MemPageUnlock(h)        /* No such requirement in NLM */
#     define MemRelease(h)           /* No such requirement in NLM */
#     define MemAlloc(f,s)           MemAllocPtr(f,s)
#     define MemReAlloc(h,s,f)       MemReAllocPtr(h,s,f)
#     define MemForceSwapOut(h)      /* No such requirement in NLM */


// these are not covers, but actual routines

     VOID *MemAllocPtr( UINT uMemType, size_t dwSize);
     VOID *MemReAllocPtr( VOID *lpMemory, size_t dwSize, UINT uMemType);
     VOID MemFreePtr( VOID *lpMemory );

#else           // SYM_DOS.     *** Not really.  SYM_WIN & SYM_NTK & SYM_VXD & SYM_NLM else ***

        // ****************************
        // DOS Memory Management Covers
        // ****************************
        //  (Also used by OS/2 - OS2VMM)

#ifndef SYM_MAC
    #include "dosvmm.h"                // handle DOS VMM
#endif

#     define MemPageLock(h)             /* No such requirement in DOS VMM */
#     define MemPageUnlock(h)           /* No such requirement in DOS VMM */

#     define MemLockLow(h)           VMMLockLow(h)
#     define MemFree(h)              VMMFree(h)
#     define MemForceFree(h)         VMMFree(h)
#     define MemFlags(h)             VMMGetFlags(h)
#     define MemSize(h)              VMMSize(h)
#     define MemCompact(s)           /* No such requirement in DOS VMM */
#     define MemHandleFromPtr(p)     VMMHandleFromPtr(p)
#     define MemAllocFixed(s)        VMMAllocFixed(s)
#     define MemFreeFixed(p)         VMMFreeFixed(p)
#     define MemRelease(h)           VMMRelease(h)
#     define MemAllocPtr(fl,ph)      VMMAllocFixed(ph)  // - Added, DEN 5/15/95
#     define MemFreePtr(ph)          VMMFreeFixed(ph)
#     define MemFreeHandlePtr(ph)    VMMFreePtr(ph)
#     define MemForceSwapOut(h)      VMMForceSwapOut(h)

#ifdef _OS2                             //  OS/2 Memory Functions - OS2 Memory Functions
#     define MemAlloc(f, s)          VMMAlloc(s)
#     define MemLock(handle)         VMMLock(handle)
#     define MemUnlock(h,p)          VMMUnlock(h,p)
#     define MemReAllocPtr(lp, cbNew, flags) \
         (MemLock(MemReAlloc(MemPtrHandle(lp) , (cbNew), (flags))))
#else
#     define MemLock(h)              _VMMLock(h)
#     define MemReAllocPtr(lp, cbNew, flags) \
         (MemUnlockPtr(lp), MemLock(MemReAlloc(MemPtrHandle(lp) , (cbNew), (flags))))
#     ifdef  MEMORY_TRACKING
         #define MemUnlock(h,p)      VMMReleaseDBG(h, (LPSTR FAR *)&(p), \
                                             (NPSTR) szMemoryTrackFileName, __LINE__)
#     else  // MEMORY_TRACKING
         #define MemUnlock(h,p)      VMMRelease(h)
#     endif  // MEMORY_TRACKING
#endif

                                        // Helper macros for MemReAllocPtr
                                        // and MemFreePtr.
#     define MemPtrHandle(lp) \
         (MemHandleFromPtr(lp))
#     define MemUnlockPtr(lp) \
         MemUnlock(MemPtrHandle(lp), foo)

//----------------------------------------------------
// - Removed, doesn't match with MemFreePtr... added MemAllocPtr above that
//   matched. - DEN 5/15/95
//----------------------------------------------------
//#     define MemAllocPtr(flags, cb)
//         (MemLock(MemAlloc((flags), (cb))))
//----------------------------------------------------

#  endif  // SYM_NLM

#endif   // SYM_WIN.  Back to level 0

#endif	// SYM_OS2

//***********************************************************************
// String and Memory Buffer Management.
//***********************************************************************

//-----------------------------------------------------------------------
// Windows replacements for std 'C' functions
// Only put the "weird" ones here.  If they are "normal", then put them
// in the next section.
//-----------------------------------------------------------------------

#if defined(SYM_WIN)

// SBCS (single-byte versions of these functions)
    #define SBCS_STRICMP(x,y)    lstrcmpi(x,y)

    #define STRCAT(d,s)          lstrcat(d,s)
    #define STRCPY(d,s)          lstrcpy(d,s)
    #define STRCMP(x,y)          lstrcmp(x,y)
    #define STRLEN(s)            ((UINT) lstrlen(s))
    #if defined(SYM_WIN32)
        #define STRLWR(s)         CharLower(s)
        #define STRUPR(s)         CharUpper(s)
    #else
        #define STRLWR(s)         AnsiLower(s)
        #define STRUPR(s)         AnsiUpper(s)
    #endif

    #define SPRINTF              wsprintf
    #define VSPRINTF             wvsprintf

#elif defined(SYM_OS2) || defined(SYM_NLM)

// SBCS (single-byte versions of these functions)
    #define SBCS_STRICMP(x,y)    stricmp(x,y)

    #define STRCAT(d,s)          strcat(d,s)
    #define STRCPY(d,s)          strcpy(d,s)
    #define STRCMP(x,y)          strcmp(x,y)
    #define STRLEN(s)            strlen(s)
    #define STRLWR(s)            strlwr(s)
    #define STRUPR(s)            strupr(s)

    #define SPRINTF              sprintf
    #define VSPRINTF             vsprintf

#elif defined(SYM_VXD)

// SBCS (single-byte versions of these functions)
    #define SBCS_STRICMP(x,y)    _stricmp(x,y)

    #define STRCAT(d,s)          strcat(d,s)
    #define STRCPY(d,s)          strcpy(d,s)
    #define STRCMP(x,y)          strcmp(x,y)
    #define STRLEN(s)            strlen(s)
    #define STRLWR(s)            _strlwr(s)
    #define STRUPR(s)            _strupr(s)

#elif defined(SYM_NTK)

// SBCS (single-byte versions of these functions)
// probably shouldn't be used since NTK compiles as UNICODE throughout.
    #define SBCS_STRICMP(x,y)    _stricmp(x,y)

    #define STRCAT(d,s)          wcscat(d,s)
    #define STRCPY(d,s)          wcscpy(d,s)
    #define STRCMP(x,y)          wcscmp(x,y)
    #define STRLEN(s)            wcslen(s)
    #define STRLWR(s)            _wcslwr(s)
    #define STRUPR(s)            _wcsupr(s)

#elif !defined(SYM_MAC)  // DOS

// SBCS (single-byte versions of these functions)
    #define SBCS_STRICMP(x,y)    _fstricmp(x,y)

    #define STRCAT(d,s)          _fstrcat(d,s)
    #define STRCPY(d,s)          _fstrcpy(d,s)
    #define STRCMP(x,y)          _fstrcmp(x,y)
    #define STRLEN(s)            _fstrlen(s)
    #define STRLWR(s)            AnsiLower(s)
    #define STRUPR(s)            AnsiUpper(s)

    #define SPRINTF              sprintf
    #define VSPRINTF             vsprintf

#endif

//-----------------------------------------------------------------------
// The remaining std 'C' functions are either the 32-bit or 16-bit
// versions.
//-----------------------------------------------------------------------

#if defined(SYM_32BIT)

// SBCS (single-byte versions of these functions)
    #define SBCS_STRCHR(s,c)        strchr(s,c)
    #define SBCS_STRRCHR(s,c)       strrchr(s,c)
    #define SBCS_STRPBRK(x,y)       strpbrk(x,y)
    #define SBCS_STRNCPY(d,s,n)     strncpy(d,s,n)
    #define SBCS_STRSTR(x,y)        strstr(x,y)

    #if defined(SYM_VXD) || defined(SYM_NTK)
        #define SBCS_STRREV(s)          _strrev(s)
        #define SBCS_STRNICMP(d,s,n)    _strnicmp(d,s,n)
    #else
        #define SBCS_STRREV(s)          strrev(s)
        #define SBCS_STRNICMP(d,s,n)    strnicmp(d,s,n)
    #endif

    #if defined(UNICODE)
        #define STRCSPN(x,y)            wcscspn(x,y)
        #define STRDUP(s)               _wcsdup(s)
        #define STRNCAT(d,s,n)          wcsncat(d,s,n)
        #define STRNCMP(d,s,n)          wcsncmp(d,s,n)
        #define STRNSET(s,c,n)          _wcsnset(s,c,n)
        #define STRSET(s,c)             _wcsset(s,c)
        #define STRSPN(x,y)             wcsspn(x,y)
        #define STRTOK(x,y)             wcstok(x,y)
    #else
        #define STRCSPN(x,y)            strcspn(x,y)
        #define STRDUP(s)               strdup(s)
        #define STRNCAT(d,s,n)          strncat(d,s,n)
        #define STRNCMP(d,s,n)          strncmp(d,s,n)
        #define STRNSET(s,c,n)          strnset(s,c,n)
        #define STRSET(s,c)             strset(s,c)
        #define STRSPN(x,y)             strspn(x,y)
        #define STRTOK(x,y)             strtok(x,y)
    #endif                              // if defined(UNICODE)

    #define MEMSET(d,c,n)           memset(d,c,n)
    #define MEMMOVE(d,s,n)          memmove(d,s,n)
    #define MEMCPY(d,s,n)           memcpy(d,s,n)
    #define MEMCHR(d,c,n)           memchr(d,c,n)
    #define MEMCMP(x,y,n)           memcmp(x,y,n)
    #define MEMICMP(x,y,n)          memicmp(x,y,n)

    #if defined(__BORLANDC__)           //  Borland doesn't use lead underscore, and OS/2 IS 32-bit
        #define FCVT                fcvt
    #else
        #define FCVT                _fcvt
    #endif

#else                                   // else to #if SYM_32BIT
    // DOS, Mirrors, and Win16 (16-bit environments)

    // SBCS (single-byte versions of these functions)
    #define SBCS_STRCHR(s,c)        _fstrchr(s,c)
    #define SBCS_STRRCHR(s,c)       _fstrrchr(s,c)
    #define SBCS_STRPBRK(x,y)       _fstrpbrk(x,y)
    #define SBCS_STRREV(s)          _fstrrev(s)
    #define SBCS_STRNCPY(d,s,n)     _fstrncpy(d,s,n)
    #define SBCS_STRNICMP(d,s,n)    _fstrnicmp(d,s,n)
    #define SBCS_STRSTR(x,y)        _fstrstr(x,y)

    #define STRCSPN(x,y)            _fstrcspn(x,y)
    #define STRDUP(s)               _fstrdup(s)
    #define STRNCAT(d,s,n)          _fstrncat(d,s,n)
    #define STRNCMP(d,s,n)          _fstrncmp(d,s,n)
    #define STRNSET(s,c,n)          _fstrnset(s,c,n)
    #define STRSET(s,c)             _fstrset(s,c)
    #define STRSPN(x,y)             _fstrspn(x,y)
    #define STRTOK(x,y)             _fstrtok(x,y)

    #define MEMSET(d,c,n)           _fmemset(d,c,n)
    #define MEMMOVE(d,s,n)          _fmemmove(d,s,n)
    #define MEMCPY(d,s,n)           _fmemcpy(d,s,n)
    #define MEMCHR(d,c,n)           _fmemchr(d,c,n)
    #define MEMCMP(x,y,n)           _fmemcmp(x,y,n)
    #define MEMICMP(x,y,n)          _fmemicmp(x,y,n)

    #define FCVT                    _fcvt
#endif                                  // endif to #if SYM_32BIT/else

#if !defined(SYM_NLM)
//-----------------------------------------------------------------------
// Spawn macro covers
//-----------------------------------------------------------------------
#if defined(__BORLANDC__)               //  Borland doesn't use lead underscore, and OS/2 IS 32-bit
#   define  SPAWNV(i,p,a)           spawnv(i,p,a)
#   define  SPAWNVP(i,p,a)          spawnvp(i,p,a)
#   define  SPAWNVE(i,p,a,e)        spawnve(i,p,a,e)
#   define  SPAWNVPE(i,p,a,e)       spawnvpe(i,p,a,e)
#else
#   define  SPAWNV(i,p,a)           _spawnv(i,p,a)
#   define  SPAWNVP(i,p,a)          _spawnvp(i,p,a)
#   define  SPAWNVE(i,p,a,e)        _spawnve(i,p,a,e)
#   define  SPAWNVPE(i,p,a,e)       _spawnvpe(i,p,a,e)
#   define  P_WAIT                  _P_WAIT
#   define  P_NOWAIT                _P_NOWAIT
#   define  P_WAITO                 _P_WAITO
#   define  P_DETACH                _P_DETACH
#   define  P_OVERLAY               _P_OVERLAY
#endif
#endif  // !defined(SYM_NLM)


#ifndef SYM_NLM
#ifndef RC_INVOKED
    //-----------------------------------------------------------------------
    //  File operation covers
    //-----------------------------------------------------------------------
    #if defined(__BORLANDC__)               //  Borland doesn't use lead underscore, and OS/2 IS 32-bit
    #   define  SOPEN                   sopen
    #   define  CLOSE(h)                close(h)
    #else
    #   define  SOPEN                   _sopen
#ifndef __WATCOMC__
    #   define  O_CREAT                 _O_CREAT
    #   define  O_TRUNC                 _O_TRUNC
    #   define  O_EXCL                  _O_EXCL
    #   define  O_RDONLY                _O_RDONLY
    #   define  O_WRONLY                _O_WRONLY
    #   define  O_RDWR                  _O_RDWR
    #   define  O_BINARY                _O_BINARY
    #   define  O_TEXT                  _O_TEXT
    #   define  S_IREAD                 _S_IREAD
    #   define  S_IWRITE                _S_IWRITE
    #   define  SH_COMPAT               _SH_COMPAT
    #   define  SH_DENYRW               _SH_DENYRW
    #   define  SH_DENYWR               _SH_DENYWR
    #   define  SH_DENYRD               _SH_DENYRD
    #   define  SH_DENYNO               _SH_DENYNO
#endif
    #   define  CLOSE(h)                _close(h)
    #endif
#endif
#endif // SYM_NLM

//-----------------------------------------------------------------------
// These are the string functions which need to be DBCS-enabled.  Some
// platforms don't support DBCS yet, so we don't need to remap them.
//
// NOTE: As we need DBCS-enabled versions of the functions above, we
// need to move them from above to here.
//-----------------------------------------------------------------------
#if defined(SYM_NTK)

    #define STRICMP(x,y)            _wcsicmp(x,y)

    #if defined(UNICODE)
        #define STRCHR(s,c)             wcschr(s,c)
        #define STRRCHR(s,c)            wcsrchr(s,c)
        #define STRPBRK(x,y)            wcspbrk(x,y)
        #define STRREV(s)               _wcsrev(s)
        #define STRNCPY(d,s,n)          wcsncpy(d,s,n)
        #define STRNICMP(d,s,n)         _wcsnicmp(d,s,n)
        #define STRSTR(x,y)             wcsstr(x,y)
    #else                               // else to if UNICODE
        #define STRCHR(s,c)             strchr(s,c)
        #define STRRCHR(s,c)            strrchr(s,c)
        #define STRPBRK(x,y)            strpbrk(x,y)
        #define STRREV(s)               _strrev(s)
        #define STRNCPY(d,s,n)          strncpy(d,s,n)
        #define STRNICMP(d,s,n)         _strnicmp(d,s,n)
        #define STRSTR(x,y)             strstr(x,y)
    #endif                              // end to if/else UNICODE

#elif defined(SYM_MAC)

// No STRICMP here for MAC -- it's currently in platmac.h

    #if defined(UNICODE)
        #define STRCHR(s,c)             wcschr(s,c)
        #define STRRCHR(s,c)            wcsrchr(s,c)
        #define STRPBRK(x,y)            wcspbrk(x,y)
        #define STRREV(s)               _wcsrev(s)
        #define STRNCPY(d,s,n)          wcsncpy(d,s,n)
        #define STRNICMP(d,s,n)         _wcsnicmp(d,s,n)
        #define STRSTR(x,y)             wcsstr(x,y)
    #else
        #define STRCHR(s,c)             strchr(s,c)
        #define STRRCHR(s,c)            strrchr(s,c)
        #define STRPBRK(x,y)            strpbrk(x,y)
        #define STRREV(s)               strrev(s)
        #define STRNCPY(d,s,n)          strncpy(d,s,n)
        #define STRNICMP(d,s,n)         strnicmp(d,s,n)
        #define STRSTR(x,y)             strstr(x,y)
    #endif

#elif defined(SYM_NLM)

    #define STRICMP(x,y)                stricmp(x,y)

    #if defined(UNICODE)
        #define STRCHR(s,c)             wcschr(s,c)
        #define STRRCHR(s,c)            wcsrchr(s,c)
        #define STRPBRK(x,y)            wcspbrk(x,y)
        #define STRREV(s)               _wcsrev(s)
        #define STRNCPY(d,s,n)          wcsncpy(d,s,n)
        #define STRNICMP(d,s,n)         _wcsnicmp(d,s,n)
        #define STRSTR(x,y)             wcsstr(x,y)
    #else
        #include "nlm_str.h"    //  Char & Str func. prototypes

        #define STRCHR(s,c)             NW_StrChr(s,c)
        #define STRRCHR(s,c)            NW_StrRChr(s,c)
        #define STRPBRK(x,y)            strpbrk(x,y)
        #define STRREV(s)               strrev(s)
        #define STRNCPY(d,s,n)          strncpy(d,s,n)
        #define STRNICMP(d,s,n)         strnicmp(d,s,n)
        #define STRSTR(x,y)             strstr(x,y)
    #endif

#elif defined(SYM_OS2)
        #define STRCSPN(x,y)    strcspn(x,y)
        #define STRDUP(s)       strdup(s)
        #define STRNCPY(d,s,n)  strncpy(d,s,n)
        #define STRNCAT(d,s,n)  strncat(d,s,n)
        #define STRNCMP(d,s,n)  strncmp(d,s,n)
        #define STRNICMP(d,s,n) strnicmp(d,s,n)
        #define STRICMP(x,y)    stricmp(x,y)
        #define STRNSET(s,c,n)  strnset(s,c,n)
        #define STRSET(s,c)     strset(s,c)
        #define STRSPN(x,y)     strspn(x,y)
        #define STRSTR(x,y)     strstr(x,y)
        #define STRTOK(x,y)     strtok(x,y)
        #define STRCHR(s,c)     strchr(s,c)
        #define STRRCHR(s,c)    strrchr(s,c)
        #define STRPBRK(x,y)    strpbrk(x,y)
        #define STRREV(s)       strrev(s)


#else
                                        // NOT SYM_VXD, SYM_NTK, SYM_NLM, SYM_MAC, or SYM_OS2

    #if defined(NO_DBSTR_FUNCTIONS)    // No DB string routines from SYMKRNL
        #define STRCHR                  SBCS_STRCHR
        #define STRRCHR                 SBCS_STRRCHR
        #define STRPBRK                 SBCS_STRPBRK
        #define STRREV                  SBCS_STRREV
        #define STRNCPY                 SBCS_STRNCPY
        #define STRICMP                 SBCS_STRICMP
        #define STRNICMP                SBCS_STRNICMP
        #define STRSTR                  SBCS_STRSTR

    #elif defined(SYM_WIN32)            // W32 uses new run-time functions
        #ifdef _T                       // tchar.h will supply the _T macro definition
            #ifndef _INC_TCHAR          // Only undef it if it wasn't defined by TCHAR.H
                #undef _T
            #endif
        #endif
        #include <tchar.h>

                                        // Due to the incredible labyrinth of ifdefs and
                                        // code paths through these platforms, I'm just
                                        // going to turn off the compiler warning of
                                        // macro redefinition here and redefine the
                                        // string macros to what they should be.
                                        // - BEM.
        #ifdef STRCHR
            #undef STRCHR
        #endif
        #ifdef STRRCHR
            #undef STRRCHR
        #endif
        #ifdef STRPBRK
            #undef STRPBRK
        #endif
        #ifdef STRREV
            #undef STRREV
        #endif
        #ifdef STRNCPY
            #undef STRNCPY
        #endif
        #ifdef STRICMP
            #undef STRICMP
        #endif
        #ifdef STRNICMP
            #undef STRNICMP
        #endif
        #ifdef STRNCMP
            #undef STRNCMP
        #endif
        #ifdef STRSTR
            #undef STRSTR
        #endif
        #ifdef STRCSPN
            #undef STRCSPN
        #endif
        #ifdef STRDUP
            #undef STRDUP
        #endif
        #ifdef STRNCAT
            #undef STRNCAT
        #endif
        #ifdef STRNSET
            #undef STRNSET
        #endif
        #ifdef STRSET
            #undef STRSET
        #endif
        #ifdef STRSPN
            #undef STRSPN
        #endif
        #ifdef STRSTR
            #undef STRSTR
        #endif
        #ifdef STRTOK
            #undef STRTOK
        #endif
                                        // NOTE: The "n" versions of the string functions
                                        //       are talking about number of BYTES in the string,
                                        //       NOT the number of CHARACTERS.
        #define STRCHR(s,c)             _tcschr(s,c)
        #define STRRCHR(s,c)            _tcsrchr(s,c)
        #define STRPBRK(x,y)            _tcspbrk(x,y)
        #define STRREV(s)               _tcsrev(s)
        #define STRNCPY(d,s,n)          _tcsncpy(d,s,n)
        #define STRICMP(x,y)            _tcsicmp(x,y)
        #define STRNICMP(d,s,n)         _tcsnicmp(d,s,n)
        #define STRNCMP(d,s,n)          _tcsncmp(d,s,n)
        #define STRSTR(x,y)             _tcsstr(x,y)
        #define STRCSPN(x,y)            _tcscspn(x,y)
        #define STRDUP(s)               _tcsdup(s)
        #define STRNCAT(d,s,n)          _tcsncat(d,s,n)
        #define STRNSET(s,c,n)          _tcsncset(s,c,n)
        #define STRSET(s,c)             _tcsset(s,c)
        #define STRSPN(x,y)             _tcsspn(x,y)
        #define STRSTR(x,y)             _tcsstr(x,y)
        #define STRTOK(x,y)             _tcstok(x,y)

    #else                               // else to if defined(NO_DBSTR_FUNCTIONS)/elif defined(WIN32)
                                        // Else DB support comes from SYMKRNL
        #define STRCHR(s,c)             SYMstrchr(s,c)
        #define STRRCHR(s,c)            SYMstrrchr(s,c)
        #define STRPBRK(x,y)            SYMstrpbrk(x,y)
        #define STRREV(s)               SYMstrrev(s)
        #define STRNCPY(d,s,n)          SYMstrncpy(d,s,n)
        #define STRICMP(x,y)            SYMstricmp(x,y)
        #define STRNICMP(d,s,n)         SYMstrnicmp(d,s,n)
        #define STRSTR(x,y)             SYMstrstr(x,y)

    #endif

#endif


//-----------------------------------------------------------------------
// These are the same for all platforms.
//-----------------------------------------------------------------------
#ifdef SYM_OS2
    #define LTOA(l,s,r)             ltoa(l,s,r)
    #define ULTOA(ul,s,r)           ultoa(ul,s,r)
    #define ITOA(i,s,r)             itoa(i,s,r)
#else
    #define LTOA(l,s,r)             _ltoa(l,s,r)
    #define ULTOA(ul,s,r)           _ultoa(ul,s,r)
    #define ITOA(i,s,r)             _itoa(i,s,r)
#endif
#define STREQUAL(x,y)           (STRCMP(x,y) == 0)

#ifndef ABS
    #define ABS(a)           ((a)> 0 ? (a) : -(a))
#endif

//-----------------------------------------------------------------------
// No more extern "C" for VxD & NTK platforms.
//-----------------------------------------------------------------------
#if defined(__cplusplus) && (defined(SYM_VXD) || defined(SYM_NTK))
    }
#endif


//-----------------------------------------------------------------------
// Symantec debug macros. They are only included when codeview info is
// turned ON.       Martin 6/24/94
//-----------------------------------------------------------------------

#if defined(SYM_DEBUG) || defined(PRERELEASE)

    #ifdef SYM_WIN
        EXTERNC VOID WINAPI AssertFailedLine(// Display assertion failed information
            const LPSTR lpFileName,     // [in] name of the file that err
            int         iLine );        // [in] line number where it occured

        #define SYM_ASSERT( f ) ( ( f ) ? ( void )0 : \
                    AssertFailedLine( __FILE__, __LINE__ ) )
        #define SYM_VERIFY( f ) SYM_ASSERT( f )

        //MWS CODE CHANGE...01/20/96
        //SYM_NUKE jumps right into the debugger if the condition is false.
        #define SYM_NUKE( f ) ( f ) ? ( void )0 : DebugBreak()
        //END MWS CHANGE
    #endif

    #ifdef SYM_NLM
        EXTERNC VOID WINAPI AssertFailedLine(// Display assertion failed information
            const LPSTR lpFileName,     // [in] name of the file that err
            int         iLine );        // [in] line number where it occured

        #define SYM_ASSERT( f ) ( ( f ) ? ( void )0 : \
                    AssertFailedLine( __FILE__, __LINE__ ) )
        #define SYM_VERIFY( f ) SYM_ASSERT( f )
    #endif

    #ifdef SYM_DOS
        EXTERNC VOID WINAPI AssertFailedLine(// Display assertion failed information
            const LPSTR lpFileName,     // [in] name of the file that err
            int         iLine );        // [in] line number where it occured

        #define SYM_ASSERT( f ) ( ( f ) ? ( void )0 : \
                    AssertFailedLine( __FILE__, __LINE__ ) )
        #define SYM_VERIFY( f ) SYM_ASSERT( f )
    #endif

    #ifdef SYM_VXD
        EXTERNC VOID __VxDAssertFailed ( LPSTR lpszFile, int iLine );

        #define SYM_ASSERT( f ) ( ( f ) ? ( void )0 : \
                    __VxDAssertFailed( __FILE__, __LINE__ ) )
    #endif

    #ifdef SYM_NTK
        #define SYM_ASSERT(f) ASSERT(f)
    #endif

    #if defined(SYM_PARANOID)
        #define SYM_VERIFY_STRING(p)               \
                {                                  \
                auto UINT lc;                      \
                for ( lc = 0; (p)[lc]; lc++ )      \
                    {                              \
                    SYM_ASSERT ( lc != (UINT)-1 ); \
                    }                              \
                }

        #define SYM_VERIFY_BUFFER(p,s)        \
                {                             \
                auto UINT  lc;                \
                auto UINT  ls = (UINT)(s);    \
                auto PBYTE lp = (PBYTE)(p);   \
                for ( lc = 0; lc < ls; lc++ ) \
                    {                         \
                    volatile BYTE lb;         \
                    lb = lp[lc];              \
                    lp[lc] = lb;              \
                    }                         \
                }
    #endif

                                        // Used to conditionally compile a set
                                        // statements. The statement that follows
                                        // this command is only compiled when
                                        // debugging is on.
    #define SYM_ASSERT_CMD() if (1)

#endif

#ifndef SYM_ASSERT
    #define SYM_ASSERT( f ) ( (void)0 )
#endif
#ifndef SYM_VERIFY
    #define SYM_VERIFY( f ) ( (void)( f ) )
#endif
#ifndef SYM_VERIFY_STRING
    #define SYM_VERIFY_STRING(s) ( (void)0 )
#endif
#ifndef SYM_VERIFY_BUFFER
    #define SYM_VERIFY_BUFFER(p,s) ( (void)0 )
#endif
#ifndef SYM_ASSERT_CMD
    #define SYM_ASSERT_CMD() if (0)
#endif

//MWS CODE CHANGE...01/20/96
#ifndef SYM_NUKE
    #define SYM_NUKE( f ) ( (void)0 )
#endif
//END MWS CHANGE

#ifdef SYM_WIN32
//-----------------------------------------------------------------------
// BEM - 9/17/96
// Symantec special wait functions that allow message processing.
// Use these instead of WaitForSingleObject() in an OLE/COM thread.
// See KB Article: OLE Threads Must Dispatch Messages
//-----------------------------------------------------------------------

// Wait for one of the given objects to be signaled, while allowing messages to be dispatched:
// Returns same as MsgWaitForMultipleObjects()
EXTERNC DWORD SYM_EXPORT WINAPI SymMsgWaitMultiple(
    DWORD dwCount,                      // Number of handles in lpHandles
    LPHANDLE lpHandles,                 // Array of handles - wiat for a signal from
                                        // one of these.
    DWORD dwMilliseconds);              // Timeout value (use INFINITE for none)

#if defined(__cplusplus)

// Helper inline cover for waiting on only a single handle:
    inline DWORD SymMsgWaitSingle(
        HANDLE hHandle,
        DWORD dwMilliseconds)
    {
        return SymMsgWaitMultiple(1, &hHandle, dwMilliseconds);
    }

    //MWS CODE CHANGE...11/27/96
    //Leveraging SymMsgWaitMultiple to allow broadcast messages through a
    //  Sleep as well.
    inline void SymMsgSleep(
        DWORD dwMilliseconds)           //Timeout value
    {
        SYM_VERIFY(SymMsgWaitMultiple(0, NULL, dwMilliseconds) == WAIT_TIMEOUT);
    }
    //END MWS CHANGE

// Helper inlines to interpret returns from SymMsgWaitMultiple:

    inline BOOL SYM_WAIT_SATISFIED(     // Returns TRUE if wait was satisfied by one of the objects
        DWORD dwCount,                  // Count of handles passed to SymMsgWaitMultiple
        DWORD dwRet)                    // Return code from SymMsgWaitMultiple
    {
        return (dwRet >= WAIT_OBJECT_0 && dwRet <= WAIT_OBJECT_0 + dwCount -1);
    }

    inline BOOL SYM_WAIT_TIMEDOUT(      // Returns TRUE if wait timed out
        DWORD dwRet)                    // Return code from SymMsgWaitMultiple
    {
        return (dwRet == WAIT_TIMEOUT);
    }
                                        // Returns the index of the handle that was signaled
    inline DWORD SYM_GET_WAIT_SATISFIED_INDEX(
        DWORD dwCount,                  // Count of handles passed to SymMsgWaitMultiple
        DWORD dwRet)                    // Return code from SymMsgWaitMultiple
    {
        UNREFERENCED_PARAMETER(dwCount);
        return dwRet - WAIT_OBJECT_0;
    }

    inline BOOL SYM_WAIT_ABANDONED(     // Returns TRUE if wait was terminated by an abandoned mutex
        DWORD dwCount,                  // Count of handles passed to SymMsgWaitMultiple
        DWORD dwRet)                    // Return code from SymMsgWaitMultiple
    {
        return (dwRet >= WAIT_ABANDONED_0 && dwRet <= WAIT_ABANDONED_0 + dwCount - 1);
    }

                                        // Returns the index of the abandoned mutex
    inline DWORD SYM_GET_WAIT_ABANDONED_INDEX(
        DWORD dwCount,                  // Count of handles passed to SymMsgWaitMultiple
        DWORD dwRet)                    // Return code from SymMsgWaitMultiple
    {
        UNREFERENCED_PARAMETER(dwCount);
        return dwRet - WAIT_ABANDONED_0;
    }

#else                                   // else to #if defined (__cplusplus)
// Macro versions:
// Helper inlines to interpret returns from SymMsgWaitMultiple:
    #define SymMsgWaitSingle(hHandle, dwMilliseconds) SymMsgWaitMultiple(1, &hHandle, dwMilliseconds)
    //MWS CODE CHANGE...11/27/96    See C++ comment above.
    #define SymMsgSleep(dwMilliseconds) SYM_VERIFY(SymMsgWaitMultiple(0, NULL, dwMilliseconds) == WAIT_TIMEOUT)
    //END MWS CHANGE
    #define SYM_WAIT_SATISFIED(dwCount, dwRet) ((BOOL)(dwRet >= WAIT_OBJECT_0 && dwRet <= WAIT_OBJECT_0 + dwCount -1))
    #define SYM_WAIT_TIMEDOUT(dwRet) ((BOOL)(dwRet == WAIT_TIMEOUT))
    #define SYM_GET_WAIT_SATISFIED_INDEX(dwCount, dwRet) ((DWORD)(dwRet - WAIT_OBJECT_0))
    #define SYM_WAIT_ABANDONED(dwCount, dwRet) ((BOOL)(dwRet >= WAIT_ABANDONED_0 && dwRet <= WAIT_ABANDONED_0 + dwCount - 1))
    #define SYM_GET_WAIT_ABANDONED_INDEX(dwCount, dwRet) ((DWORD)(dwRet - WAIT_ABANDONED_0))
#endif                                  // if defined (__cplusplus)
#endif  // ifdef SYM_WIN32

#endif // !defined(SYM_UNIX)

#endif                                  // ifndef INCL_PLATFORM_H - DON'T ADD CODE BELOW HERE

