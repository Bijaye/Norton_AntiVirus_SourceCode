//************************************************************************
//
// $Header:   S:/NAVEX/VCS/WORDDOC.H_v   1.12   08 May 1997 15:36:36   DCHI  $
//
// Description:
//      Contains WordDocument interface prototypes.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/WORDDOC.H_v  $
// 
//    Rev 1.12   08 May 1997 15:36:36   DCHI
// Added office 97 conversion of template to document and added criteria
// for absence of AutoText before converting to document for both Word versions.
// 
//    Rev 1.11   08 Apr 1997 12:41:02   DCHI
// Added support for FullSet(), FullSetRepair, Or()/Not(), MacroCount(), etc.
// 
//    Rev 1.10   13 Feb 1997 13:29:28   DCHI
// Modifications to support VBA 5 scanning.
// 
//    Rev 1.9   17 Jan 1997 11:02:00   DCHI
// Modifications supporting new macro engine.
// 
//    Rev 1.8   21 Nov 1996 17:15:12   AOONWAL
// No change.
// 
//    Rev 1.7   06 Nov 1996 17:17:20   DCHI
// Added functionality for wild macro scanning.
// 
//    Rev 1.6   29 Oct 1996 13:00:28   AOONWAL
// No change.
// 
//    Rev 1.5   13 Sep 1996 18:32:46   DCHI
// Added include of navexshr.h to take care of warnings.
// 
//    Rev 1.4   06 Sep 1996 14:53:50   DCHI
// Added SYM_VXD to pack changes.
// 
//    Rev 1.3   06 Sep 1996 14:20:54   DCHI
// pack changes.
// 
//    Rev 1.2   28 Aug 1996 16:44:14   DCHI
// Added BAT cache support.
// 
//    Rev 1.1   03 May 1996 14:40:58   DCHI
// Added dwMacroTableContents field to MACRO_TABLE_INFO_T.
// Added MACRO_TABLE_TYPE_0X constants and macro table information
// function prototypes.
// 
//    Rev 1.0   03 Jan 1996 17:12:06   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _WORDDOCH_

#define _WORDDOCH_

#include "platform.h"
#include "callback.h"

#include "navexshr.h"
#include "storobj.h"

#define WORDDOC_OK                  0x0000

#define WORDDOC_ERR_READ_ID			0xFFFF
#define WORDDOC_ERR_READ_LEN		0xFFFE
#define WORDDOC_ERR_READ_NAME		0xFFFD
#define WORDDOC_ERR_READ_MCD		0xFFFC
#define WORDDOC_ERR_NO_MCD			0xFFFB
#define WORDDOC_ERR_NO_SUCH_NAME	0xFFFA
#define WORDDOC_ERR_WRITE_MCD		0xFFF9

#define WORDDOC_ERR_READ            0xFF0F
#define WORDDOC_ERR_WRITE			0xFF0E

#define MACRO_TABLE_TYPE_NONE       0x00000000ul

#define MACRO_TABLE_TYPE_0X01       0x00000001ul
#define MACRO_TABLE_TYPE_0X02       0x00000002ul
#define MACRO_TABLE_TYPE_0X03       0x00000004ul
#define MACRO_TABLE_TYPE_0X04       0x00000008ul
#define MACRO_TABLE_TYPE_0X05       0x00000010ul
#define MACRO_TABLE_TYPE_0X06       0x00000020ul
#define MACRO_TABLE_TYPE_0X10       0x00000040ul
#define MACRO_TABLE_TYPE_0X11       0x00000080ul
#define MACRO_TABLE_TYPE_0X12       0x00000100ul
#define MACRO_TABLE_TYPE_0X40       0x00000200ul

typedef struct
{
    DWORD   dwMacroTableContents;
	DWORD 	dwMacroNameTableOffset;
	WORD 	wNumMacros;
	DWORD 	dwMCDTableOffset;
	WORD 	wNumMCDs;
	DWORD	dwULNameTableOffset;
	WORD	wULNameTableSize;
} MACRO_TABLE_INFO_T;

typedef struct
{
	DWORD	dwNameOffset;	// This points to name, not wID, nor size
	DWORD	dwMCDOffset;
	DWORD	dwOffset;		// Offset of macro in document stream
	DWORD	dwSize;			// Size of macro in bytes

	BYTE	byNameLen;		// Length of macro name
	BYTE	byEncrypt;		// Encryption byte

	WORD	wULNameIdx;		// Index of name in UL name table
	WORD	wMenuHelp;		// Index of macro description
							// string in UL name table
} MACRO_INFO_T;

typedef MACRO_TABLE_INFO_T FAR *LPMACRO_TABLE_INFO;
typedef MACRO_INFO_T FAR *		LPMACRO_INFO_T;

#if defined(SYM_WIN32) || defined(SYM_NTK) || defined(SYM_VXD)
#include "pshpack1.h"
#else
#pragma PACK1
#endif

typedef struct
{
	BYTE	byCMG;
	BYTE	byEncrypt;	// Encryption byte

	WORD	wID;		// ID in macro name table
	WORD	wULNameIdx;	// Index of name in UL name table
	WORD	wMenuHelp;	// Index of macro description
						// string in UL name table

	DWORD	dwFirst;
	DWORD	dwSize;		// Size of macro in bytes
	DWORD	dwN;  		// Zero for invalid macro
	DWORD	dwOffset;	// Offset of macro in document stream
} MCD_T;

#if defined(SYM_WIN32) || defined(SYM_NTK) || defined(SYM_VXD)
#include "poppack.h"
#else
#pragma PACK
#endif

WORD WordDocOpen
(
	LPCALLBACKREV1 	lpCallBack,				// File op callbacks
	HFILE 			hFile,					// Handle to OLE file
	LPOLE_FILE_T	pOLEFile,				// OLE file info
    LPBYTE          lpbyStreamBATCache,     // Non NULL if cache, 512 bytes
    LPWORD          pwMagicNum              // Magic number
);

#if defined(SYM_DOSX) || !defined(SYM_DOS)

// Decryption key enhanced functions below

WORD WordDocContainsMacros
(
	LPCALLBACKREV1 		lpCallBack,			// File op callbacks
	LPOLE_FILE_T		pOLEFile,			// OLE file info
    LPWDENCKEY          lpstKey,            // Decryption key
    LPBYTE              lpbyWorkBuffer,     // Work buffer >= 512 bytes
	LPMACRO_TABLE_INFO  pMacroTableInfo		// Macro table info
);

WORD WordDocGetMacroInfo
(
	LPCALLBACKREV1 		lpCallBack,			// File op callbacks
	LPOLE_FILE_T		pOLEFile,			// OLE file info
    LPWDENCKEY          lpstKey,            // Decryption key
    LPMACRO_TABLE_INFO  pMacroTableInfo,    // Macro table info
	LPMACRO_INFO_T		pMacroInfo,			// Macro info
	LPBYTE 				lpbyWorkBuffer,		// Work buffer >= 512 bytes
	LPSTR 				lpszMacroName		// Name of macro
);

WORD WordDocHasActiveMacros
(
	LPCALLBACKREV1 		lpCallBack,			// File op callbacks
	LPOLE_FILE_T		pOLEFile,			// OLE file info
    LPWDENCKEY          lpstKey,            // Decryption key
    LPMACRO_TABLE_INFO  pMacroTableInfo,    // Macro table info
    LPBYTE              lpbyWorkBuffer      // Work buffer >= 512 bytes
);

WORD WordDocCountActiveMacros
(
    LPCALLBACKREV1      lpstCallBack,       // File op callbacks
    LPOLE_FILE_T        lpstOLEFile,        // OLE file info
    LPWDENCKEY          lpstKey,            // Decryption key
    LPMACRO_TABLE_INFO  lpstMacroTableInfo, // Macro table info
    LPWORD              lpwNumMacros,       // Ptr to WORD for count
    LPBYTE              lpbyWorkBuffer      // Work buffer >= 64 bytes
);

typedef struct tagWD_GET_MACRO_INFO
{
    // bUseIndex - TRUE if the caching info should not be used
    //           - FALSE if the caching info should be used

    BOOL                bUseIndex;
    WORD                wNextNameIndex;

    DWORD               dwNextNameOffset;

    WORD                wNextMCDIndex;
    DWORD               dwNextMCDOffset;
} WD_GET_MACRO_INFO_T, FAR *LPWD_GET_MACRO_INFO;

WORD WordDocGetMacroInfoAtIndex
(
    LPCALLBACKREV1      lpCallBack,         // File op callbacks
    LPWD_GET_MACRO_INFO lpstGetInfo,        // Get info cache structure
    LPOLE_FILE_T        pOLEFile,           // OLE file info
    LPWDENCKEY          lpstKey,            // Decryption key
    LPMACRO_TABLE_INFO  pMacroTableInfo,    // Macro table info
    LPMACRO_INFO_T      pMacroInfo,         // Macro info
    LPBYTE              lpbyWorkBuffer,     // Work buffer >= 512 bytes
    LPSTR               lpszMacroName       // Buffer to store name of macro
);

WORD WordDocHasNonMacroTemplateInfo
(
    LPMACRO_TABLE_INFO  pMacroTableInfo     // Macro table info
);

WORD WordDocHasAutoText
(
    LPCALLBACKREV1      lpstCallBack,       // File op callbacks
    LPOLE_FILE_T        lpstOLEFile,        // OLE file info
    LPWDENCKEY          lpstKey,            // Decryption key
    LPBOOL              lpbHasAutoText      // Ptr to BOOL
);

// Decryption key enhanced functions above

#else

// Plain DOS below

WORD WordDocContainsMacros
(
	LPCALLBACKREV1 		lpCallBack,			// File op callbacks
	LPOLE_FILE_T		pOLEFile,			// OLE file info
	LPBYTE 				lpbyWorkBuffer,		// Work buffer >= 512 bytes
	LPMACRO_TABLE_INFO  pMacroTableInfo		// Macro table info
);

WORD WordDocGetMacroInfo
(
	LPCALLBACKREV1 		lpCallBack,			// File op callbacks
	LPOLE_FILE_T		pOLEFile,			// OLE file info
	LPMACRO_TABLE_INFO  pMacroTableInfo,	// Macro table info
	LPMACRO_INFO_T		pMacroInfo,			// Macro info
	LPBYTE 				lpbyWorkBuffer,		// Work buffer >= 512 bytes
	LPSTR 				lpszMacroName		// Name of macro
);

WORD WordDocHasActiveMacros
(
	LPCALLBACKREV1 		lpCallBack,			// File op callbacks
	LPOLE_FILE_T		pOLEFile,			// OLE file info
	LPMACRO_TABLE_INFO  pMacroTableInfo,	// Macro table info
    LPBYTE              lpbyWorkBuffer      // Work buffer >= 512 bytes
);

WORD WordDocGetMacroInfoAtIndex
(
    LPCALLBACKREV1      lpCallBack,         // File op callbacks
    WORD                wIndex,             // Index to get
    LPOLE_FILE_T        pOLEFile,           // OLE file info
    LPMACRO_TABLE_INFO  pMacroTableInfo,    // Macro table info
    LPMACRO_INFO_T      pMacroInfo,         // Macro info
    LPBYTE              lpbyWorkBuffer,     // Work buffer >= 512 bytes
    LPSTR               lpszMacroName       // Buffer to store name of macro
);

// Plain DOS above

#endif

#endif


