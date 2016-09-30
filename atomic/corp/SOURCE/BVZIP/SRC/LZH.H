#ifndef lzh_h
#define lzh_h

/*
 *==========================================================================
 *
 * $Workfile:   lzh.h  $
 * $Revision:   1.0  $
 * $Modtime :$
 *
 * Public header file for LZH-4 and LZH-5.
 *
 * Copyright (c) 1993 by Central Point Software.  All rights reserved
 *
 * Author      : Jon J. Sorensen
 * Language    : Microsoft C8.00
 * Model       : Large
 * Environment : Microsoft Windows 3.1 SDK
 *
 * This is a public header file that contains functions that support
 * (compress and expand) the LZH-4 and LZH-5 compression method.
 *
 * Date:
 * Log:
 *
 *==========================================================================
*/


//-----------------------------------------------------------
//   I N C L U D E   F I L E S
//-----------------------------------------------------------


//-----------------------------------------------------------
//   C O N S T A N T S
//-----------------------------------------------------------


//-----------------------------------------------------------
//   T Y P E   D E F I N I T I O N S
//-----------------------------------------------------------


//-----------------------------------------------------------
//   M A C R O S
//-----------------------------------------------------------


//-----------------------------------------------------------
//   P R O T O T Y P E S
//-----------------------------------------------------------

WORD WINAPI LZH_DecodeLZH45WorkSize( VOID );
VOID WINAPI LZH_DecodeLZH45Init( LPVOID lpvInfo, WORD wMeth );
BOOL WINAPI LZH_DecodeLZH45( LPBYTE lpInBuf, LPWORD lpwInptSiz, LPBYTE lpOutBuf, LPWORD lpwOutptSiz, LPVOID lpvVars, char cEOF );

WORD WINAPI LZH_EncodeLZH45WorkSize( VOID );
LONG WINAPI LZH_EncodeLZH45Init( LPVOID lpvInfo, WORD wMeth );
VOID WINAPI LZH_EncodeLZH45Free( LPVOID lpvInfo );
BOOL WINAPI LZH_EncodeLZH45( LPBYTE, LPWORD, LPBYTE, LPWORD, LPVOID, char );


#endif
