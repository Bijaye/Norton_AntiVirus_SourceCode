/*
 *==========================================================================
 *
 * $Workfile:   enlzh45.c  $
 * $Revision:   1.0  $
 * $Modtime :$
 *
 * Module used to compress files using LZH-4 or LZH-5.
 *
 * Copyright (c) 1993 by Central Point Software.  All rights reserved
 *
 * Author      : Jon J. Sorensen
 * Language    : Microsoft C9.00
 * Model       : 32Bit Flat
 * Environment : Microsoft Windows 95 SDK
 *
 * This module contains functions used to compress a file using the LZH-4
 * or the LZH-5 method.
 *
 * $Date:   30 Jan 1996 15:48:24  
 * $Log:   S:/BVZIP/VCS/ENLZH45.C_v  
// 
//    Rev 1.0   30 Jan 1996 15:48:24   BARRY
// Initial revision.
// 
//    Rev 1.0   30 Jan 1996 09:55:04   BARRY
// Initial revision.
 *
 *==========================================================================
*/

//-----------------------------------------------------------
//   I N C L U D E   F I L E S
//-----------------------------------------------------------

// Common includes for Pre-compiled Header
// This should go before any other includes or definitions
#ifdef SYM_BUILD_SM
#include "fsv_pch.h"
#else
#include "..\inc\fsv_pch.h"
#endif
#pragma hdrstop
// Put all other includes after this

// File Services include files (not in PCH)
#ifdef SYM_BUILD_SM
#include "fsv.p"
#else
#include "..\inc\fsv.p"
#endif

// Program include files.
#include "lzh.p"
#include "lzh.h"

//-----------------------------------------------------------
//   S E G M E N T  F I L E
//-----------------------------------------------------------
#ifdef SYM_BUILD_SM
#include "wnfsv.seg"
#else
#include "..\inc\wnfsv.seg"
#endif

//-----------------------------------------------------------
//   C O N S T A N T S
//-----------------------------------------------------------

// ?????.
#define LZH_PERCOLATE    1
#define NIL              0
#define LZH_MAX_HASH_VAL ( 3 * LZH_DICSIZ + ( LZH_DICSIZ / 512 + 1 ) * UCHAR_MAX )
#define LZH_OUT_BUF_SIZE ( 16 * 1024 )

// The various encode states.  These states are used to resume encoding of
// the input stream after an end of input buffer exception or an end of
// output buffer exception has been generated.
#define LZH_EN_START           0
#define LZH_EN_1STGETNEXTMATCH 1
#define LZH_EN_2NDGETNEXTMATCH 2
#define LZH_EN_1STOUTPUT       3
#define LZH_EN_2NDOUTPUT       4
#define LZH_EN_FINALCLEANUP    5

// The various states during the SendBlock() function.  These states are
// used to resume encoding of the input stream after an end of output
// buffer exception has been generated.
#define LZH_SB_START           0
#define LZH_SB_SETSIZE         1
#define LZH_SB_1STWRITEPTLEN   2
#define LZH_SB_1STWRITECHLEN   3
#define LZH_SB_1STSMALLROOT    4
#define LZH_SB_2NDWRITEPTLEN   5
#define LZH_SB_2NDSMALLROOT    6
#define LZH_SB_ENCODEPOSTN     7
#define LZH_SB_ENCODECH        8
#define LZH_SB_FLUSHBITS       9


//-----------------------------------------------------------
//   T Y P E   D E F I N I T I O N S
//-----------------------------------------------------------


//-----------------------------------------------------------
//   M A C R O S
//-----------------------------------------------------------

// Compute a "hashed" index.
#define LZH_HASH(p, c) ( ( p ) +  ( ( c ) << ( LZH_DICBIT - 9 ) ) + LZH_DICSIZ * 2 )

// Save the state of the output buffer.
#define LZH_MarkBufState(lpInfo) ( *lpInfo ).biMarkBitInfo = ( *lpInfo ).biBitInfo

// Restore the state of the output buffer.
#define LZH_RestoreBufState(lpInfo) ( *lpInfo ).biBitInfo = ( *lpInfo ).biMarkBitInfo

// Access methods for the encode state variable.
#define LZH_EncodeSetState(lpInfo, wState) ( *lpInfo ).wEncodeState = ( wState )

// Access methods for the SendBlock() state variable.
#define LZH_SendBlockSetSate(lpInfo, wState) { ( *lpInfo ).wSendBlkState = ( wState ); LZH_MarkBufState( lpInfo ); }



//-----------------------------------------------------------
//   G L O B A L   F U N C T I O N S
//-----------------------------------------------------------


//==========================================================================
//
// WORD LZH_EncodeLZH45WorkSize( VOID  )
//
// This function returns the size the structure required for the LZH (4 & 5)
// compression function.
//
//==========================================================================

WORD WINAPI LZH_EncodeLZH45WorkSize( VOID )

{
   return( sizeof( LZH_ENCODEINFOLZH45 ) );
} // LZH_EncodeLZH45WorkSize



//==========================================================================
//
// LONG LZH_EncodeLZH45Init( lpvInfo, wMeth )
//
//    lpvInfo - Pointer to the structure that is used to contain state
//              information for the compression method.
//    wMeth   - The type of LZH compression method to use (4 or 5).
//
// This function initializes the state info used to "compress" a file that
// will be compressed using LZH-4 or LZH-5.
//
// Returns:
//    FSV_SUCCESS          - All dynamic memory was successfully allocated.
//    FSVERR_OUT_OF_MEMORY - A memory allocation error was encountered.
//
//==========================================================================

LONG WINAPI LZH_EncodeLZH45Init(
   LPVOID lpvInfo,
   WORD   wMeth
)

{
   LPLZH_ENCODEINFOLZH45 lpInfo = lpvInfo;
   LONG                  lErr;

   if ( lpInfo == NULL )
   // Bogus input parameter.
   {
      lErr = ZIP_BAD_ARG;
   }
   else
   {
      // Clean out the state info structure.
      _fmemset( lpInfo, 0, sizeof( LZH_ENCODEINFOLZH45 ) );

      if ( ( lpInfo->lpbText       = Mem_SubMallocHeap( LZH_DICSIZ * 2 + LZH_MAXMATCH, NULL ) ) == NULL ||
           ( lpInfo->lpbOutBuf     = Mem_SubMallocHeap( LZH_OUT_BUF_SIZE, NULL ) ) == NULL ||
           ( lpInfo->lpwLevel      = Mem_SubMallocHeap( ( LZH_DICSIZ + UCHAR_MAX + 1 ) * sizeof( lpInfo->lpwLevel[ 0 ] ), NULL ) ) == NULL ||
           ( lpInfo->lpbChildCount = Mem_SubMallocHeap( ( LZH_DICSIZ + UCHAR_MAX + 1 ) * sizeof( lpInfo->lpbChildCount[ 0 ] ), NULL ) ) == NULL ||
           ( lpInfo->lpnPostn      = Mem_SubMallocHeap( ( LZH_DICSIZ + UCHAR_MAX + 1 ) * sizeof( lpInfo->lpnPostn[ 0 ] ), NULL ) ) == NULL ||
           ( lpInfo->lpwParent     = Mem_SubMallocHeap( ( LZH_DICSIZ * 2 ) * sizeof( lpInfo->lpwParent[ 0 ] ), NULL ) ) == NULL ||
           ( lpInfo->lpwPrev       = Mem_SubMallocHeap( ( LZH_DICSIZ * 2 ) * sizeof( lpInfo->lpwPrev[ 0 ] ), NULL ) ) == NULL ||
           ( lpInfo->lpwNext       = Mem_SubMallocHeap( ( LZH_MAX_HASH_VAL + 1 ) * sizeof( lpInfo->lpwNext[ 0 ] ), NULL ) ) == NULL )
      // Bad news, not enough memory to allocate all the dynamic memory that
      // is required.
      {
         LZH_EncodeLZH45Free( lpInfo );
         lErr = FSVERR_OUT_OF_MEMORY;
      }
      else
      // We be kool!
      {
         // Initialize the sliding dictionary.
         LZH_InitSlide( lpInfo );

         // ?????.
         lpInfo->lpbOutBuf[ 0 ] = 0;
         lpInfo->wOutptIndx     = 0;
         lpInfo->wOutptMask     = 0;

         // ?????.
         _fmemset( lpInfo->wChFreq, 0, LZH_NC * sizeof( lpInfo->wChFreq[ 0 ] ) );

         // ?????.
         _fmemset( lpInfo->wPtFreq, 0, LZH_NP * sizeof( lpInfo->wPtFreq[ 0 ] ) );

         // Init state variables for Encode() and SendBlock().
         LZH_EncodeSetState(lpInfo, LZH_EN_START );
         LZH_SendBlockSetSate( lpInfo, LZH_SB_START );

         // ?????.
         LZHUtil_InitPutBits( &lpInfo->biBitInfo );

         lErr = FSV_SUCCESS;
      }
   }

   return( lErr );
} // LZH_EncodeLZH45Init



//==========================================================================
//
// VOID LZH_EncodeLZH45Free( lpvInfo )
//
//    lpvInfo - Pointer to the structure that is used to contain state
//              information for the compression method.
//
// This function frees all of the memory that was allocated for the LZH
// compression.
//
//==========================================================================

VOID WINAPI LZH_EncodeLZH45Free(
   LPVOID lpvInfo
)

{
   LPLZH_ENCODEINFOLZH45 lpInfo = lpvInfo;

   if ( lpInfo != NULL )
   // Bogus input parameter.
   {
      if ( lpInfo->lpbText != NULL )
      {
         Mem_SubFreeHeap( lpInfo->lpbText, NULL );
         lpInfo->lpbText = NULL;
      }

      if ( lpInfo->lpbOutBuf != NULL )
      {
         Mem_SubFreeHeap( lpInfo->lpbOutBuf, NULL );
         lpInfo->lpbOutBuf = NULL;
      }

      if ( lpInfo->lpwLevel != NULL )
      {
         Mem_SubFreeHeap( lpInfo->lpwLevel, NULL );
         lpInfo->lpwLevel = NULL;
      }

      if ( lpInfo->lpbChildCount != NULL )
      {
         Mem_SubFreeHeap( lpInfo->lpbChildCount, NULL );
         lpInfo->lpbChildCount = NULL;
      }

      if ( lpInfo->lpnPostn != NULL )
      {
         Mem_SubFreeHeap( lpInfo->lpnPostn, NULL );
         lpInfo->lpnPostn = NULL;
      }

      if ( lpInfo->lpwParent != NULL )
      {
         Mem_SubFreeHeap( lpInfo->lpwParent, NULL );
         lpInfo->lpwParent = NULL;
      }

      if ( lpInfo->lpwPrev != NULL )
      {
         Mem_SubFreeHeap( lpInfo->lpwPrev, NULL );
         lpInfo->lpwPrev = NULL;
      }

      if ( lpInfo->lpwNext != NULL )
      {
         Mem_SubFreeHeap( lpInfo->lpwNext, NULL );
         lpInfo->lpwNext = NULL;
      }
   }

   return;
} // LZH_EncodeLZH45Free



//==========================================================================
//
// BOOL LZH_EncodeLZH5( lpbInBuf, lpwInptSiz, lpbOutBuf, lpwOutptSiz,
//                      lpvVars, cEOF )
//
//    lpbInBuf    - Pointer to the buffer containing the "raw" data.
//    lpwInptSiz  - Pointer to the amount of bytes in the input buffer.
//                  The amount variable will be updated by subtracting the
//                  number of input bytes that were processed.
//    lpbOutBuf   - Pointer to the buffer that will contain the compressed
//                  data.
//    lpwOutptSiz - Pointer to the number of bytes that can be contained in
//                  the output buffer.  The affiliate variable will be
//                  updated to contain the number of output bytes that were
//                  produced.
//    lpvVars     - Pointer to the structure that is used to contain state
//                  information for the compression method.
//    cEOF        - A one byte flag that will be set to TRUE if this is the
//                  "last" input buffer.
//
// This function "compresses" the data contained in the input buffer into
// the specified output buffer.  This function supports the LZH-4 and the
// LZH-5 compression methods.
//
// Returns:
//    FALSE - The caller needs to pass in more input.
//    TRUE  - All of data for the file in question has been compressed.
//
//==========================================================================

BOOL WINAPI LZH_EncodeLZH45(
   LPBYTE lpbInBuf,
   LPWORD lpwInptSiz,
   LPBYTE lpbOutBuf,
   LPWORD lpwOutptSiz,
   LPVOID lpvVars,
   char   cEOF
)

{
   LPLZH_ENCODEINFOLZH45 lpInfo    = lpvVars;
   BOOL                  fDone     = FALSE;
   WORD                  wInptByts = *lpwInptSiz;

   if ( lpInfo->wBytsNeeded > 0 )
   // Fill up the local input buffer as much as possible.
   {
      WORD wBytsCopied = min( lpInfo->wBytsNeeded, wInptByts );

      // The number of input bytes processed.
      *lpwInptSiz = wBytsCopied;

      // Copy as much of the supplied input buffer as possible.
      _fmemcpy( &lpInfo->lpbText[ lpInfo->wStrtIndx ], lpbInBuf, wBytsCopied );

      // Update sliding dictionary state variables.
      lpInfo->wBytsNeeded -= wBytsCopied;
      lpInfo->wStrtIndx   += wBytsCopied;
      lpInfo->wInptByts   += wBytsCopied;

      if ( lpInfo->wBytsNeeded > 0 && ( !cEOF || ( cEOF && wInptByts != wBytsCopied ) ) )
      // More input is required for the sliding dictionary.
      {
         *lpwOutptSiz = 0;
         return( FALSE );
      }

      if ( lpInfo->fFrstBuf )
      // ?????.
      {
         LZH_InsertNode( lpInfo );
         lpInfo->fFrstBuf = FALSE;
      }

   }
   else
   // None of the input bytes were used.
   {
      *lpwInptSiz = 0;
   }

   // Init structure used to store bit oriented i/o into the output buffer.
   LZHUtil_SetupPutBits( &lpInfo->biBitInfo, lpbOutBuf, *lpwOutptSiz );

//   if ( Catch( lpInfo->nCatchBuf ) == 0 )
   gnJmpErr = 0;
   if (setjmp( gnJmp[gcnJmp++] ) == 0)
   {
      if ( lpInfo->wMatchLen > lpInfo->wInptByts )
      // The match length can not exceed the remaining amount of input.
      {
         lpInfo->wMatchLen = lpInfo->wInptByts;
      }

      // If necessary, resume processing before the end of input buffer
      // exception or end of output buffer exception was generated.
      switch ( lpInfo->wEncodeState )
      {
         case LZH_EN_1STGETNEXTMATCH:
            LZH_DeleteNode( lpInfo );
            LZH_InsertNode( lpInfo );
            goto FirstGetNextMatch;
         break;

         case LZH_EN_2NDGETNEXTMATCH:
            LZH_DeleteNode( lpInfo );
            LZH_InsertNode( lpInfo );
            goto SecondGetNextMatch;
         break;

         case LZH_EN_1STOUTPUT:
            lpInfo->wOutptMask = 1;
            goto FirstOutput;
         break;

         case LZH_EN_2NDOUTPUT:
            lpInfo->wOutptMask = 1;
            goto SecondOutput;
         break;

         case LZH_EN_FINALCLEANUP:
            goto FinalCleanup;
         break;
      }

      // ?????.
      while ( lpInfo->wInptByts > 0 )
      {
         // ?????.
         lpInfo->wLastMatchLen = lpInfo->wMatchLen;
         lpInfo->nLastMatchPos = lpInfo->nMatchPostn;

         // ?????.
         LZH_EncodeSetState(lpInfo, LZH_EN_1STGETNEXTMATCH );
         LZH_GetNextMatch( lpInfo );
FirstGetNextMatch:

         if ( lpInfo->wMatchLen > lpInfo->wInptByts )
         // The match length can not exceed the remaining amount of input.
         {
            lpInfo->wMatchLen = lpInfo->wInptByts;
         }

         if ( lpInfo->wMatchLen > lpInfo->wLastMatchLen || lpInfo->wLastMatchLen < LZH_THRESHOLD )
         // ?????.
         {
FirstOutput:
            LZH_EncodeSetState(lpInfo, LZH_EN_1STOUTPUT );
            LZH_Output( lpInfo, lpInfo->lpbText[ lpInfo->wCurInByt - 1 ], 0 );
         }
         else
         // ?????.
         {
SecondOutput:
            // ?????.
            LZH_EncodeSetState(lpInfo, LZH_EN_2NDOUTPUT );
            LZH_Output( lpInfo, (WORD)(lpInfo->wLastMatchLen + ( UCHAR_MAX + 1 - LZH_THRESHOLD )),
                        (WORD)(( lpInfo->wCurInByt - lpInfo->nLastMatchPos - 2 ) & ( LZH_DICSIZ - 1 )) );

            // ?????.
            LZH_EncodeSetState(lpInfo, LZH_EN_2NDGETNEXTMATCH );
            while ( --lpInfo->wLastMatchLen > 0 )
            {
               WORD wJunk;

               LZH_GetNextMatch( lpInfo );
SecondGetNextMatch:
               wJunk = wJunk; // The BoneHead compiler needs this line.
            }

            if ( lpInfo->wMatchLen > lpInfo->wInptByts )
            // The match length can not exceed the remaining amount of input.
            {
               lpInfo->wMatchLen = lpInfo->wInptByts;
            }
         }
      }

FinalCleanup:
      // Send final block of output and flush remaining bits.
      LZH_EncodeSetState(lpInfo, LZH_EN_FINALCLEANUP );
      LZH_SendBlock( lpInfo, TRUE );
      fDone = TRUE;
      gcnJmp--;
   }
   else
   // An exception has been generated.
   {
      WORD wThrowRtn;

      // The Throw() function puts the error code that was passed to
      // it in the AX register.
//      _asm Mov wThrowRtn,AX
//      gcnJmp--;
      wThrowRtn = gnJmpErr;

      // Handle the various exception conditions.
      switch ( wThrowRtn )
      {
         case LZH_END_OF_OUTPUT_BUF:
            // Restore the state of the output buffer when it was still valid.
            LZH_RestoreBufState( lpInfo );
         break;

         case LZH_END_OF_INPUT_BUF:
            // Copy the input that was just processed into the area for
            // the sliding dictionary.
            _fmemmove( &lpInfo->lpbText[ 0 ], &lpInfo->lpbText[ LZH_DICSIZ ], LZH_DICSIZ + LZH_MAXMATCH );

            // Reset state variables for another input buffer.
            lpInfo->wBytsNeeded = LZH_DICSIZ;
            lpInfo->wStrtIndx   = LZH_DICSIZ + LZH_MAXMATCH;
            lpInfo->wCurInByt   = LZH_DICSIZ;
         break;

         // An error must have been encountered.
         default:
            fDone = TRUE;
         break;
      }
   }

   // Return the number of output bytes generated.
   *lpwOutptSiz = lpInfo->biBitInfo.lpCurByt - lpInfo->biBitInfo.lpBuffr;

   return( fDone );
} // LZH_EncodeLZH45



//-----------------------------------------------------------
//   L O C A L   F U N C T I O N S
//-----------------------------------------------------------


//==========================================================================
//
// VOID LZH_InitSlide( lpInfo )
//
//    lpInfo - Pointer to the structure that is used to contain state
//             information for the compression method.
//
// This function initializes the state info used to build and manage the
// "sliding dictionary".
//
//==========================================================================

VOID WINAPI LZH_InitSlide(
   LPLZH_ENCODEINFOLZH45 lpInfo
)

{
   WORD   wIndx;
   LPWORD lpwTmp1;
   LPWORD lpwTmp2;

   // ?????.
   for ( wIndx = LZH_DICSIZ, lpwTmp1 = &lpInfo->lpwLevel[ LZH_DICSIZ ], lpwTmp2 = &lpInfo->lpnPostn[ LZH_DICSIZ ];
         wIndx <= LZH_DICSIZ + UCHAR_MAX;
         wIndx++, lpwTmp1++, lpwTmp2++ )
   {
      *lpwTmp1 = 1;
      *lpwTmp2 = NIL;
   }

   // ?????.
   for ( wIndx = 2, lpwTmp1 = &lpInfo->lpwNext[ 1 ]; wIndx < LZH_DICSIZ; wIndx++, lpwTmp1++ )
   {
      *lpwTmp1 = wIndx;
   }

   // ?????.
   _fmemset( &lpInfo->lpwParent[ LZH_DICSIZ ], 0, LZH_DICSIZ * sizeof( lpInfo->lpwParent[ 0 ] ) );

   // ?????.
   lpInfo->lpwNext[ LZH_DICSIZ - 1 ] = NIL;
   _fmemset( &lpInfo->lpwNext[ LZH_DICSIZ * 2 ], 0,
             ( LZH_MAX_HASH_VAL - ( LZH_DICSIZ * 2 ) + 1 ) * sizeof( lpInfo->lpwNext[ 0 ] ) );

   // Init sliding dictionary state variables.
   lpInfo->wInptByts   = 0;
   lpInfo->wMatchLen   = 0;
   lpInfo->wCurInByt   = LZH_DICSIZ;
   lpInfo->wBytsNeeded = LZH_DICSIZ + LZH_MAXMATCH;
   lpInfo->wStrtIndx   = LZH_DICSIZ;
   lpInfo->fFrstBuf    = TRUE;
   lpInfo->wAvail      = 1;
} // LZH_InitSlide



//==========================================================================
//
// VOID LZH_Child( lpInfo, wNode, bCh )
//
//    lpInfo - Pointer to the structure that is used to contain state
//             information for the compression method.
//    wNode  -
//    bCh    -
//
// This function initializes the state info used to build and manage the
// "sliding dictionary".
//
// This function returns the child character for bCh or NIL if not found.
//
//==========================================================================

WORD WINAPI LZH_Child(
   LPLZH_ENCODEINFOLZH45 lpInfo,
   WORD                  wNode,
   BYTE                  bCh
)

{
   WORD wIndx;

   // ?????.
   wIndx = lpInfo->lpwNext[ LZH_HASH( wNode, bCh ) ];

   // Sentinel.
   lpInfo->lpwParent[ NIL ] = wNode;

   // Scan for ...
   while ( lpInfo->lpwParent[ wIndx ] != wNode )
   {
      wIndx = lpInfo->lpwNext[ wIndx ];
   }

   return( wIndx );
} // LZH_Child



//==========================================================================
//
// VOID LZH_MakeChild( lpInfo, wNode1, bCh, wNode2 )
//
//    lpInfo - Pointer to the structure that is used to contain state
//             information for the compression method.
//    wNode1 -
//    bCh    -
//    wNode2 -
//
// This function ... /* Let r be q's child for character c. */
//
//==========================================================================

VOID WINAPI LZH_MakeChild(
   LPLZH_ENCODEINFOLZH45 lpInfo,
   WORD                  wNode1,
   BYTE                  bCh,
   WORD                  wNode2
)

{
   WORD wHashedIndx;
   WORD wNxtIndx;

   // ?????.
   wHashedIndx = LZH_HASH( wNode1, bCh );
   wNxtIndx    = lpInfo->lpwNext[ wHashedIndx ];

   // ?????.
   lpInfo->lpwNext[ wHashedIndx ] = wNode2;
   lpInfo->lpwNext[ wNode2 ]      = wNxtIndx;
   lpInfo->lpwPrev[ wNxtIndx ]    = wNode2;
   lpInfo->lpwPrev[ wNode2 ]      = wHashedIndx;

   // ?????.
   lpInfo->lpwParent[ wNode2 ] = wNode1;
   lpInfo->lpbChildCount[ wNode1 ]++;
} // LZH_MakeChild



//==========================================================================
//
// VOID LZH_Split( lpInfo, wOldNode )
//
//    lpInfo   - Pointer to the structure that is used to contain state
//               information for the compression method.
//    wOldNode -
//
// This function ... /* Let r be q's child for character c. */
//
//==========================================================================

VOID WINAPI LZH_Split(
   LPLZH_ENCODEINFOLZH45 lpInfo,
   WORD                  wOldNode
)

{
   WORD wNew;
   WORD wTmp;

   // ?????.
   wNew                          = lpInfo->wAvail;
   lpInfo->wAvail                = lpInfo->lpwNext[ wNew ];
   lpInfo->lpbChildCount[ wNew ] = 0;

   // ?????.
   wTmp                    = lpInfo->lpwPrev[ wOldNode ];
   lpInfo->lpwPrev[ wNew ] = wTmp;
   lpInfo->lpwNext[ wTmp ] = wNew;
   wTmp                    = lpInfo->lpwNext[ wOldNode ];
   lpInfo->lpwNext[ wNew]  = wTmp;
   lpInfo->lpwPrev[ wTmp ] = wNew;

   // ?????.
   lpInfo->lpwParent[ wNew ] = lpInfo->lpwParent[ wOldNode ];
   lpInfo->lpwLevel[ wNew ]  = lpInfo->wMatchLen;
   lpInfo->lpnPostn[ wNew ]  = lpInfo->wCurInByt;

   // ?????.
   LZH_MakeChild( lpInfo, wNew, lpInfo->lpbText[ lpInfo->nMatchPostn + lpInfo->wMatchLen ], wOldNode );
   LZH_MakeChild( lpInfo, wNew, lpInfo->lpbText[ lpInfo->wCurInByt + lpInfo->wMatchLen ], lpInfo->wCurInByt );
} // LZH_Split



//==========================================================================
//
// VOID LZH_InsertNode( lpInfo )
//
//    lpInfo - Pointer to the structure that is used to contain state
//             information for the compression method.
//
// This function ...
//
//==========================================================================

VOID WINAPI LZH_InsertNode(
   LPLZH_ENCODEINFOLZH45 lpInfo
)

{
   WORD wNode1;
   WORD wNode2;
   WORD wTmp;

   if ( lpInfo->wMatchLen >= 4 )
   // ?????.
   {
      // ?????.
      lpInfo->wMatchLen--;
      wNode1 = ( lpInfo->nMatchPostn + 1 ) | LZH_DICSIZ;

      // ?????.
      while ( ( wNode2 = lpInfo->lpwParent[ wNode1] ) == NIL )
      {
         wNode1 = lpInfo->lpwNext[ wNode1 ];
      }

      // ?????.
      while ( lpInfo->lpwLevel[ wNode2 ] >= lpInfo->wMatchLen )
      {
         wNode1 = wNode2;
         wNode2 = lpInfo->lpwParent[ wNode2 ];
      }

      #if LZH_PERCOLATE
      {
         // ?????.
         for ( wTmp = wNode2; lpInfo->lpnPostn[ wTmp ] < 0; )
         {
            lpInfo->lpnPostn[ wTmp ] = lpInfo->wCurInByt;
            wTmp                     = lpInfo->lpwParent[ wTmp ];
         }

         if ( wTmp < LZH_DICSIZ )
         // ?????.
         {
            lpInfo->lpnPostn[ wTmp ] = lpInfo->wCurInByt | LZH_PERC_FLAG;
         }
      }
      #else
      {
         // ?????.
         for ( wTmp = wNode2; wTmp < LZH_DICSIZ; )
         {
            lpInfo->lpnPostn[ wTmp ] = lpInfo->wCurInByt;
            wTmp                     = lpInfo->lpwParent[ wTmp ];
         }
      }
      #endif
   }
   else
   // ?????.
   {
      BYTE bCh;

      // ?????.
      wNode2 = lpInfo->lpbText[ lpInfo->wCurInByt ] + LZH_DICSIZ;
      bCh    = lpInfo->lpbText[ lpInfo->wCurInByt + 1 ];

      if ( ( wNode1 = LZH_Child( lpInfo, wNode2, bCh ) ) == NIL )
      // ?????.
      {
         LZH_MakeChild( lpInfo, wNode2, bCh, lpInfo->wCurInByt );
         lpInfo->wMatchLen = 1;
         return;
      }

      // ?????.
      lpInfo->wMatchLen = 2;
   }

   // ?????.
   for ( ; ; )
   {
      LPBYTE lpbTmp1;
      LPBYTE lpbTmp2;

      if ( wNode1 >= LZH_DICSIZ )
      // ?????.
      {
         wTmp                = LZH_MAXMATCH;
         lpInfo->nMatchPostn = wNode1;
      }
      else
      // ?????.
      {
         wTmp                = lpInfo->lpwLevel[ wNode1 ];
         lpInfo->nMatchPostn = lpInfo->lpnPostn[ wNode1 ] & ~LZH_PERC_FLAG;
      }

      if ( lpInfo->nMatchPostn >= ( __int16 ) lpInfo->wCurInByt )
      // ?????.
      {
         lpInfo->nMatchPostn -= LZH_DICSIZ;
      }

      // ?????.
      lpbTmp1 = &lpInfo->lpbText[ lpInfo->wCurInByt + lpInfo->wMatchLen ];
      lpbTmp2 = &lpInfo->lpbText[ lpInfo->nMatchPostn + lpInfo->wMatchLen ];

      // ?????.
      while ( lpInfo->wMatchLen < wTmp )
      {
         if ( *lpbTmp1 != *lpbTmp2)
         // ?????.
         {
            LZH_Split( lpInfo, wNode1 );
            return;
         }

         // ?????.
         lpInfo->wMatchLen++;
         lpbTmp1++;
         lpbTmp2++;
      }

      if ( lpInfo->wMatchLen >= LZH_MAXMATCH )
      // ?????.
      {
         break;
      }

      // ?????.
      lpInfo->lpnPostn[ wNode1 ] = lpInfo->wCurInByt;
      wNode2 = wNode1;

      if ( ( wNode1 = LZH_Child( lpInfo, wNode2, *lpbTmp1 ) ) == NIL )
      // ?????.
      {
         LZH_MakeChild( lpInfo, wNode2, *lpbTmp1, lpInfo->wCurInByt );
         return;
      }

      // ?????.
      lpInfo->wMatchLen++;
   }

   // ?????.
   wTmp                                 = lpInfo->lpwPrev[ wNode1 ];
   lpInfo->lpwPrev[ lpInfo->wCurInByt ] = wTmp;
   lpInfo->lpwNext[ wTmp ]              = lpInfo->wCurInByt;

   // ?????.
   wTmp                                 = lpInfo->lpwNext[ wNode1 ];
   lpInfo->lpwNext[ lpInfo->wCurInByt ] = wTmp;
   lpInfo->lpwPrev[ wTmp ]              = lpInfo->wCurInByt;

   // ?????.
   lpInfo->lpwParent[ lpInfo->wCurInByt ] = wNode2;
   lpInfo->lpwParent[ wNode1 ]            = NIL;
   lpInfo->lpwNext[ wNode1 ]              = lpInfo->wCurInByt; // **special use of lpInfo->lpwNext[]
} // LZH_InsertNode



//==========================================================================
//
// VOID LZH_DeleteNode( lpInfo )
//
//    lpInfo - Pointer to the structure that is used to contain state
//             information for the compression method.
//
// This function ...
//
//==========================================================================

VOID WINAPI LZH_DeleteNode(
   LPLZH_ENCODEINFOLZH45 lpInfo
)

{
   WORD wPrev;
   WORD wNext;
   WORD wTmp1;
   WORD wTmp2;

   if ( lpInfo->lpwParent[ lpInfo->wCurInByt ] == NIL )
   // ?????.
   {
      return;
   }

   // ?????.
   wPrev = lpInfo->lpwPrev[ lpInfo->wCurInByt ];
   wNext = lpInfo->lpwNext[ lpInfo->wCurInByt ];

   // ?????.
   lpInfo->lpwNext[ wPrev ] = wNext;
   lpInfo->lpwPrev[ wNext ] = wPrev;

   // ?????.
   wPrev                                  = lpInfo->lpwParent[ lpInfo->wCurInByt ];
   lpInfo->lpwParent[ lpInfo->wCurInByt ] = NIL;

   if ( wPrev >= LZH_DICSIZ || --lpInfo->lpbChildCount[ wPrev ] > 1 )
   // ?????.
   {
      return;
   }

   // ?????.
   #if LZH_PERCOLATE
   {
      wTmp1 = lpInfo->lpnPostn[ wPrev ] & ~LZH_PERC_FLAG;
   }
   #else
   {
      wTmp1 = lpInfo->lpnPostn[ wPrev ];
   }
   #endif

   if ( wTmp1 >= lpInfo->wCurInByt )
   // ?????.
   {
      wTmp1 -= LZH_DICSIZ;
   }

   #if LZH_PERCOLATE
   // ?????.
   {
      WORD wTmp3;

      // ?????.
      wNext = wTmp1;
      wTmp3 = lpInfo->lpwParent[ wPrev ];

      // ?????.
      while ( ( wTmp2 = lpInfo->lpnPostn[ wTmp3 ] ) & LZH_PERC_FLAG )
      {
         // ?????.
         wTmp2 &= ~LZH_PERC_FLAG;

         if (wTmp2 >= lpInfo->wCurInByt )
         // ?????.
         {
            wTmp2 -= LZH_DICSIZ;
         }

         if ( wTmp2 > wNext )
         // ?????.
         {
            wNext = wTmp2;
         }

         // ?????.
         lpInfo->lpnPostn[ wTmp3 ] = ( wNext | LZH_DICSIZ );
         wTmp3                     = lpInfo->lpwParent[ wTmp3 ];
      }

      if ( wTmp3 < LZH_DICSIZ )
      // ?????.
      {
         if ( wTmp2 >= lpInfo->wCurInByt)
         // ?????.
         {
            wTmp2 -= LZH_DICSIZ;
         }

         if ( wTmp2 > wNext )
         // ?????.
         {
            wNext = wTmp2;
         }

         // ?????.
         lpInfo->lpnPostn[ wTmp3 ] = wNext | LZH_DICSIZ | LZH_PERC_FLAG;
      }
   }
   #endif

   // ?????.
   wNext                    = LZH_Child( lpInfo, wPrev, lpInfo->lpbText[ wTmp1 + lpInfo->lpwLevel[ wPrev ] ] );
   wTmp1                    = lpInfo->lpwPrev[ wNext ];
   wTmp2                    = lpInfo->lpwNext[ wNext ];
   lpInfo->lpwNext[ wTmp1 ] = wTmp2;
   lpInfo->lpwPrev[ wTmp2 ] = wTmp1;

   // ?????.
   wTmp1                    = lpInfo->lpwPrev[ wPrev ];
   lpInfo->lpwNext[ wTmp1 ] = wNext;
   lpInfo->lpwPrev[ wNext ] = wTmp1;

   // ?????.
   wTmp1                      = lpInfo->lpwNext[ wPrev ];
   lpInfo->lpwPrev[ wTmp1 ]   = wNext;
   lpInfo->lpwNext[ wNext ]   = wTmp1;

   // ?????.
   lpInfo->lpwParent[ wNext ] = lpInfo->lpwParent[ wPrev ];
   lpInfo->lpwParent[ wPrev ] = NIL;
   lpInfo->lpwNext[ wPrev ]   = lpInfo->wAvail;
   lpInfo->wAvail             = wPrev;
} // LZH_DeleteNode



//==========================================================================
//
// VOID LZH_GetNextMatch( lpInfo )
//
//    lpInfo - Pointer to the structure that is used to contain state
//             information for the compression method.
//
// This function ...
//
//==========================================================================

VOID WINAPI LZH_GetNextMatch(
   LPLZH_ENCODEINFOLZH45 lpInfo
)

{
   // Keep track of the bytes remaining in the sliding dictionary buffer.
   lpInfo->wInptByts--;

   // Process the next byte of input.
   lpInfo->wCurInByt++;

   if ( lpInfo->wCurInByt == 2 * LZH_DICSIZ )
   // The sliding dictionary input buffer has been exhausted.
   {
      gnJmpErr = LZH_END_OF_INPUT_BUF;
      longjmp( gnJmp[--gcnJmp], -1 );
//      Throw( lpInfo->nCatchBuf, LZH_END_OF_INPUT_BUF );
   }

   // ?????.
   LZH_DeleteNode( lpInfo );
   LZH_InsertNode( lpInfo );
} // LZH_GetNextMatch



//==========================================================================
//
// VOID LZH_CountTFreq( lpInfo )
//
//    lpInfo - Pointer to the structure that is used to contain state
//             information for the compression method.
//
// This function ...
//
//==========================================================================

VOID WINAPI LZH_CountTFreq(
   LPLZH_ENCODEINFOLZH45 lpInfo
)

{
   LPBYTE lpbTmp;
   WORD   wIndx;
   WORD   wNum;

   // ?????.
   _fmemset( lpInfo->wTFreq, 0, LZH_NT * sizeof( lpInfo->wTFreq[ 0 ] ) );

   // ?????.
   for ( wNum = LZH_NC, lpbTmp = &lpInfo->bChLen[  LZH_NC - 1 ];
         wNum > 0 && *lpbTmp == 0;
         wNum--, lpbTmp-- );

   // ?????.
   for ( wIndx = 0, lpbTmp = lpInfo->bChLen; wIndx < wNum; )
   {
      BYTE

      // ?????.
      bLen = *lpbTmp;
      lpbTmp++;
      wIndx++;

      if ( bLen == 0 )
      // ?????.
      {
         WORD wCount;

         // ?????.
         for ( wCount = 1; wIndx < wNum && *lpbTmp == 0; wIndx++, lpbTmp++, wCount++ );

         if ( wCount <= 2 )
         {
            lpInfo->wTFreq[ 0 ] += wCount;
         }
         else
         if ( wCount <= 18 )
         {
            lpInfo->wTFreq[ 1 ]++;
         }
         else
         if ( wCount == 19 )
         {
            lpInfo->wTFreq[ 0 ]++;
            lpInfo->wTFreq[ 1 ]++;
         }
         else
         {
            lpInfo->wTFreq[ 2 ]++;
         }
      }
      else
      // ?????.
      {
         lpInfo->wTFreq[ bLen + 2 ]++;
      }
   }

} // LZH_CountTFreq



//==========================================================================
//
// VOID LZH_WritePtLen( lpInfo, wNumEntries, wNumBits, wSpecial )
//
//    lpInfo      - Pointer to the structure that is used to contain state
//                  information for the compression method.
//    wNumEntries - The maximum number of ...
//    wNumBits    - The ...
//    wSpecial    - ????.
//
// This function builds...
//
// Notes:
//    1) This function ...
//
//==========================================================================

VOID WINAPI LZH_WritePtLen(
   LPLZH_ENCODEINFOLZH45 lpInfo,
   WORD                  wNumEntries,
   WORD                  wNumBits,
   WORD                  wSpecial
)

{
   LPBYTE lpbTmp;
   WORD   wIndx;

   // ?????.
   for ( lpbTmp = &lpInfo->bPtLen[ wNumEntries - 1 ];
         wNumEntries > 0 && *lpbTmp == 0;
         wNumEntries--, lpbTmp-- );

   // Write out the number of entries in the table.
   LZHUtil_PutBits( lpInfo, wNumBits, wNumEntries );

   // ?????.
   for ( wIndx = 0, lpbTmp = lpInfo->bPtLen; wIndx < wNumEntries; )
   {
      BYTE bLen;

      // ?????.
      bLen = *lpbTmp;
      wIndx++;
      lpbTmp++;

      if ( bLen <= 6 )
      // Store the length in 3 bits.
      {
         LZHUtil_PutBits( lpInfo, 3, bLen );
      }
      else
      // Store the length in ?????.
      {
         LZHUtil_PutBits( lpInfo, (WORD)(bLen - 3), (WORD)(( 1U << ( bLen - 3 ) ) - 2) );
      }

      if ( wIndx == wSpecial )
      // ?????.
      {
         for ( ; wIndx < 6 && *lpbTmp == 0; wIndx++, lpbTmp++ );
         LZHUtil_PutBits( lpInfo, 2, (WORD)(( wIndx - 3 ) & 3) );
      }
   }

} // LZH_WritePtLen



//==========================================================================
//
// VOID LZH_LZH_WriteChLen( lpInfo )
//
//    lpInfo - Pointer to the structure that is used to contain state
//             information for the compression method.
//
// This function builds...
//
// Notes:
//    1) This function ...
//
//==========================================================================

VOID WINAPI LZH_WriteChLen(
   LPLZH_ENCODEINFOLZH45 lpInfo
)

{
   LPBYTE lpbTmp;
   WORD   wIndx;
   WORD   wNum;

   // ?????.
   for ( wNum = LZH_NC, lpbTmp = &lpInfo->bChLen[ LZH_NC - 1];
         wNum > 0 && *lpbTmp == 0;
         wNum--, lpbTmp-- );

   // Store number of entries in the character length table.
   LZHUtil_PutBits( lpInfo, LZH_CBIT, wNum );

   // ?????.
   for ( wIndx = 0, lpbTmp = lpInfo->bChLen; wIndx < wNum; )
   {
      BYTE bLen;

      // ?????.
      bLen = *lpbTmp;
      wIndx++;
      lpbTmp++;

      if ( bLen == 0 )
      // ?????.
      {
         WORD wCount;

         // ?????.
         for ( wCount = 1; wIndx < wNum && *lpbTmp == 0; wIndx++, lpbTmp++, wCount++ );

         if ( wCount <= 2 )
         // ?????.
         {
            WORD wIndx2;

            for ( wIndx2 = 0; wIndx2 < wCount; wIndx2++ )
            {
               LZHUtil_PutBits( lpInfo, lpInfo->bPtLen[ 0 ], lpInfo->wPtCode[ 0 ] );
            }
         }
         else
         if ( wCount <= 18 )
         // ?????.
         {
            LZHUtil_PutBits( lpInfo, lpInfo->bPtLen[ 1 ], lpInfo->wPtCode[ 1 ] );
            LZHUtil_PutBits( lpInfo, 4, (WORD)(wCount - 3) );
         }
         else
         // ?????.
         if ( wCount == 19 )
         // ?????.
         {
            LZHUtil_PutBits( lpInfo, lpInfo->bPtLen[ 0 ], lpInfo->wPtCode[ 0 ] );
            LZHUtil_PutBits( lpInfo, lpInfo->bPtLen[ 1 ], lpInfo->wPtCode[ 1 ] );
            LZHUtil_PutBits( lpInfo, 4, 15 );
         }
         else
         // ?????.
         {
            LZHUtil_PutBits( lpInfo, lpInfo->bPtLen[ 2 ], lpInfo->wPtCode[ 2 ] );
            LZHUtil_PutBits( lpInfo, LZH_CBIT, (WORD)(wCount - 20) );
         }
      }
      else
      // ?????.
      {
         LZHUtil_PutBits( lpInfo, lpInfo->bPtLen[ bLen + 2 ], lpInfo->wPtCode[ bLen + 2 ] );
      }
   }

} // LZH_WriteChLen



//==========================================================================
//
// VOID LZH_EncodeCh( lpInfo, wCh )
//
//    lpInfo - Pointer to the structure that is used to contain state
//             information for the compression method.
//    wCh    - The character to store as a Huffman bit code.
//
// This function encodes ...
//
//==========================================================================

VOID WINAPI LZH_EncodeCh(
   LPLZH_ENCODEINFOLZH45 lpInfo,
   WORD                  wCh
)

{
   LZHUtil_PutBits( lpInfo, lpInfo->bChLen[ wCh ], lpInfo->wChCode[ wCh ] );
} // LZH_EncodeCh


//==========================================================================
//
// VOID LZH_EncodePostn( lpInfo, wPostn )
//
//    lpInfo - Pointer to the structure that is used to contain state
//             information for the compression method.
//    wPostn - The index within the fixed size sliding dictionary of
//             a "repeated" pattern.
//
// This function encodes ...
//
//==========================================================================

VOID WINAPI LZH_EncodePostn(
   LPLZH_ENCODEINFOLZH45 lpInfo,
   WORD                  wPostn
)

{
   WORD wTmp1;
   WORD wCount;

   // Determine the number of bits in the ...
   for ( wTmp1 = wPostn, wCount = 0; wTmp1 != 0; wTmp1 >>= 1, wCount++ );

   // ?????.
   LZHUtil_PutBits( lpInfo, lpInfo->bPtLen[ wCount ], lpInfo->wPtCode[ wCount ] );

   if ( wCount > 1 )
   // ?????.
   {
      LZHUtil_PutBits( lpInfo, (WORD)(wCount - 1), (WORD)(wPostn & ( 0xFFFFU >> ( 17 - wCount ) )) );
   }

} // LZH_EncodePostn



//==========================================================================
//
// VOID LZH_CountLen( lpInfo, wRoot )
//
//    lpInfo - Pointer to the structure that is used to contain state
//             information for the compression method.
//    wRoot  - The ...
//
// This function ...
//
//==========================================================================

VOID WINAPI LZH_CountLen(
   LPLZH_ENCODEINFOLZH45 lpInfo,
   WORD                  wRoot
)

{
   if ( wRoot < lpInfo->wNumNodes )
   // ?????.
   {
      lpInfo->wLenCnt[ ( lpInfo->wDepth < LZH_LEN_COUNT_SIZ ) ? lpInfo->wDepth : LZH_LEN_COUNT_SIZ ]++;
   }
   else
   // ?????.
   {
      lpInfo->wDepth++;
      LZH_CountLen( lpInfo, lpInfo->wLeft[ wRoot ] );
      LZH_CountLen( lpInfo, lpInfo->wRight[ wRoot ] );
      lpInfo->wDepth--;
   }

} // LZH_CountLen



//==========================================================================
//
// VOID LZH_MakeLen( lpInfo, wRoot, lpwSrtPtr )
//
//    lpInfo    - Pointer to the structure that is used to contain state
//                information for the compression method.
//    wRoot     - The ...
//    lpwSrtPtr - The ...
//
// This function ...
//
//==========================================================================

VOID WINAPI LZH_MakeLen(
   LPLZH_ENCODEINFOLZH45 lpInfo,
   WORD                  wRoot,
   LPWORD                lpwSrtPtr
)

{
   LPWORD lpwTmp;
   WORD   wIndx;
   WORD   wCum;

   // ?????.
   _fmemset( lpInfo->wLenCnt, 0, sizeof( lpInfo->wLenCnt ) );

   // ?????.
   LZH_CountLen( lpInfo, wRoot );

   // ?????.
   for ( wIndx = LZH_LEN_COUNT_SIZ, wCum = 0, lpwTmp = &lpInfo->wLenCnt[ LZH_LEN_COUNT_SIZ ];
         wIndx != 0;
         wIndx--, lpwTmp-- )
   {
      wCum += *lpwTmp << ( LZH_LEN_COUNT_SIZ - wIndx );
   }

   // ?????.
   while ( wCum != (WORD)( 1U << LZH_LEN_COUNT_SIZ ) )
   {
      // ?????.
      lpInfo->wLenCnt[ LZH_LEN_COUNT_SIZ ]--;


      // ????.
      for ( wIndx = 15, lpwTmp = &lpInfo->wLenCnt[ 15 ]; wIndx != 0; wIndx--, lpwTmp-- )
      {
         if ( *lpwTmp != 0 )
         // ????.
         {
            lpwTmp[ 0 ]--;
            lpwTmp[ 1 ] += 2;
            break;
         }
      }

      // ????.
      wCum--;
   }

   // ????.
   for ( wIndx = LZH_LEN_COUNT_SIZ, lpwTmp = &lpInfo->wLenCnt[ LZH_LEN_COUNT_SIZ ]; wIndx != 0; wIndx--, lpwTmp-- )
   {
      __int16 nIndx2 = ( __int16 ) *lpwTmp;

      // ?????.
      while ( --nIndx2 >= 0 )
      {
         lpInfo->lpbLen[ *lpwSrtPtr++ ] = ( BYTE ) wIndx;
      }
   }

} // LZH_MakeLen



//==========================================================================
//
// VOID LZH_DownHeap( lpInfo, wIndx )
//
//    lpInfo - Pointer to the structure that is used to contain state
//             information for the compression method.
//    wIndx  - The ...
//
// This function ...
// *** priority queue; send i-th entry down heap ***
//
//==========================================================================

VOID WINAPI LZH_DownHeap(
   LPLZH_ENCODEINFOLZH45 lpInfo,
   WORD                  wNode
)

{
   WORD wIndx2;
   WORD wIndx1 = lpInfo->wHeap[ wNode ];

   // ?????.
   while ( ( wIndx2 = 2 * wNode ) <= lpInfo->wHeapSize )
   {
      if ( wIndx2 < lpInfo->wHeapSize &&
           lpInfo->lpwFreq[ lpInfo->wHeap[ wIndx2 ] ] > lpInfo->lpwFreq[ lpInfo->wHeap[ wIndx2 + 1 ] ] )
      // ?????.
      {
         wIndx2++;
      }

      if ( lpInfo->lpwFreq[ wIndx1 ] <= lpInfo->lpwFreq[ lpInfo->wHeap[ wIndx2 ] ] )
      {
         break;
      }

      // ?????.
      lpInfo->wHeap[ wNode ] = lpInfo->wHeap[ wIndx2 ];
      wNode                  = wIndx2;
   }

   // ?????.
   lpInfo->wHeap[ wNode ] = wIndx1;
} // LZH_DownHeap



//==========================================================================
//
// VOID LZH_MakeCode( lpInfo, wNum, lpbLen, lpwCode )
//
//    lpInfo  - Pointer to the structure that is used to contain state
//              information for the compression method.
//    wNum    - The ...
//    lpbLen  -
//    lpwCode -
//
// This function ...
// *** priority queue; send i-th entry down heap ***
//
//==========================================================================

VOID WINAPI LZH_MakeCode(
   LPLZH_ENCODEINFOLZH45 lpInfo,
   WORD                  wNum,
   BYTE                  lpbLen[],
   WORD                  lpwCode[]
)

{
   WORD   wIndx;
   WORD   wStart[ 18 ];
   LPWORD lpwTmp1;
   LPWORD lpwTmp2;

   // ?????.
   wStart[ 1 ] = 0;

   // ?????.
   for ( wIndx = 1, lpwTmp1 = &wStart[ 1 ], lpwTmp2 = &lpInfo->wLenCnt[ 1 ];
         wIndx <= 16;
         wIndx++, lpwTmp1++, lpwTmp2++ )
   {
      lpwTmp1[ 1 ] = ( lpwTmp1[ 0 ] + lpwTmp2[ 0 ] ) << 1;
   }

   // ?????.
   for ( wIndx = 0; wIndx < wNum; wIndx++, lpbLen++, lpwCode++ )
   {
      lpwCode[ 0 ] = wStart[ lpbLen[ 0 ] ]++;
   }

} // LZH_MakeCode



//==========================================================================
//
// VOID LZH_MakeTree( lpInfo, wNum, lpwFreq, lpbLen, lpwCode )
//
//    lpInfo  - Pointer to the structure that is used to contain state
//              information for the compression method.
//    wNum    - The ...
//    lpwFreq -
//    lpbLen  -
//    lpwCode -
//
// This function ...
// *** make tree, calculate len[], return root ***
//
//==========================================================================

WORD WINAPI LZH_MakeTree(
   LPLZH_ENCODEINFOLZH45 lpInfo,
   WORD                  wNum,
   WORD                  lpwFreq[],
   BYTE                  lpbLen[],
   WORD                  lpwCode[]
)

{
   LPWORD lpwSrtPtr;
   LPWORD lpwTmp1;
   LPWORD lpwTmp2;
   WORD   nAvail;
   WORD   wIndx;
   WORD   wNode;

   // Make global copies of the input parameters.
   lpInfo->wNumNodes = wNum;
   lpInfo->lpwFreq   = lpwFreq;
   lpInfo->lpbLen    = lpbLen;

   // Initialize the "heap".
   lpInfo->wHeapSize  = 0;
   lpInfo->wHeap[ 1 ] = 0;

   // ?????.
   nAvail = wNum;

   // ?????.
   _fmemset( lpbLen, 0, wNum * sizeof( lpbLen[ 0 ] ) );

   // ?????.
   for ( wIndx = 0, lpwTmp1 = lpwFreq, lpwTmp2 = &lpInfo->wHeap[ 1 ];
         wIndx < wNum; wIndx++, lpwTmp1++ )
   {
      if ( *lpwTmp1 != 0 )
      // ?????.
      {
         *lpwTmp2 = wIndx;
         lpwTmp2++;
         lpInfo->wHeapSize++;
      }
   }

   if ( lpInfo->wHeapSize < 2 )
   // ?????.
   {
      lpwCode[ lpInfo->wHeap[ 1 ] ] = 0;
      return( lpInfo->wHeap[ 1 ] );
   }

   // Make the priority queue.
   for ( wIndx = lpInfo->wHeapSize / 2; wIndx >= 1; wIndx-- )
   {
      LZH_DownHeap( lpInfo, wIndx );
   }

   // This pointer will be ?????.
   lpwSrtPtr = lpwCode;

   // ???? while the queue has at least two entries.
   do
   {
      WORD wNext;

      // Take out least-frequently used entry.
      wIndx = lpInfo->wHeap[ 1 ];

      if ( wIndx < wNum )
      // ?????.
      {
         *lpwSrtPtr++ = wIndx;
      }

      // ?????.
      lpInfo->wHeap[ 1 ] = lpInfo->wHeap[ lpInfo->wHeapSize-- ];

      // ?????.
      LZH_DownHeap( lpInfo, 1 );

      // Next least-freq entry.
      wNext = lpInfo->wHeap[ 1 ];

      if ( wNext < wNum )
      // ?????.
      {
         *lpwSrtPtr++ = wNext;
      }

      // Generate new node.
      wNode = nAvail++;

      // Put into queue.
      lpInfo->lpwFreq[ wNode ] = lpwFreq[ wIndx ] + lpInfo->lpwFreq[ wNext ];
      lpInfo->wHeap[ 1 ]       = wNode;

      // ?????.
      LZH_DownHeap( lpInfo, 1 );

      // ?????.
      lpInfo->wLeft[ wNode  ] = wIndx;
      lpInfo->wRight[ wNode ] = wNext;
   }
   while ( lpInfo->wHeapSize > 1 );

   // This pointer will be ?????.
   LZH_MakeLen( lpInfo, wNode, lpwCode );

   // ?????.
   LZH_MakeCode( lpInfo, wNum, lpbLen, lpwCode );

   // Return the root.
   return( wNode );
} // LZH_MakeTree



//==========================================================================
//
// VOID LZH_SendBlock( lpInfo, fFlush )
//
//    lpInfo - Pointer to the structure that is used to contain state
//             information for the compression method.
//    fFlush - This flag is TRUE if the bits in the holding bit buffer
//             are to be "flushed" after the output buffer has been processed
//             and written out.
//
// This function ...
//
//==========================================================================

VOID WINAPI LZH_SendBlock(
   LPLZH_ENCODEINFOLZH45 lpInfo,
   BOOL                  fFlush
)

{
// WORD wFlags;
// WORD wIndx;
// WORD wPos;
// WORD wSize;
// WORD wRoot;

   // If necessary, resume execution before the end of the output
   // buffer exception was encountered.
   switch ( lpInfo->wSendBlkState )
   {
      case LZH_SB_SETSIZE:
         goto SetSize;
      break;

      case LZH_SB_1STWRITEPTLEN:
         goto FirstWritePtLen;
      break;

      case LZH_SB_1STWRITECHLEN:
         goto FirstWriteChLen;
      break;

      case LZH_SB_1STSMALLROOT:
         goto FirstSmallRoot;
      break;

      case LZH_SB_2NDWRITEPTLEN:
         goto SecondWritePtLen;
      break;

      case LZH_SB_2NDSMALLROOT:
         goto SecondSmallRoot;
      break;

      case LZH_SB_ENCODEPOSTN:
         goto EncodePostn;
      break;

      case LZH_SB_ENCODECH:
         goto EncodeCh;
      break;

      case LZH_SB_FLUSHBITS:
         goto FlushBits;
      break;
   }

   // ?????.
   lpInfo->wRoot = LZH_MakeTree( lpInfo, LZH_NC, lpInfo->wChFreq, lpInfo->bChLen, lpInfo->wChCode );

   // ?????.
   lpInfo->wSize = lpInfo->wChFreq[ lpInfo->wRoot ];

SetSize:
   // ?????.
   LZH_SendBlockSetSate( lpInfo, LZH_SB_SETSIZE );
   LZHUtil_PutBits( lpInfo, 16, lpInfo->wSize );

   if ( lpInfo->wRoot >= LZH_NC )
   // ?????.
   {
      // ?????.
      LZH_CountTFreq( lpInfo );

      // ?????.
      lpInfo->wRoot = LZH_MakeTree( lpInfo, LZH_NT, lpInfo->wTFreq, lpInfo->bPtLen, lpInfo->wPtCode );

FirstWritePtLen:
      // ?????.
      LZH_SendBlockSetSate( lpInfo, LZH_SB_1STWRITEPTLEN );

      if ( lpInfo->wRoot >= LZH_NT )
      // ?????.
      {
         LZH_WritePtLen( lpInfo, LZH_NT, LZH_TBIT, 3 );
      }
      else
      // ?????.
      {
         LZHUtil_PutBits(lpInfo, LZH_TBIT, 0 );
         LZHUtil_PutBits(lpInfo, LZH_TBIT, lpInfo->wRoot );
      }

FirstWriteChLen:
      // ?????.
      LZH_SendBlockSetSate( lpInfo, LZH_SB_1STWRITECHLEN );

      // ?????.
      LZH_WriteChLen( lpInfo );
   }
   else
FirstSmallRoot:
   // ?????.
   {
      LZH_SendBlockSetSate( lpInfo, LZH_SB_1STSMALLROOT );
      LZHUtil_PutBits( lpInfo, LZH_TBIT, 0 );
      LZHUtil_PutBits( lpInfo, LZH_TBIT, 0 );
      LZHUtil_PutBits( lpInfo, LZH_CBIT, 0 );
      LZHUtil_PutBits( lpInfo, LZH_CBIT, lpInfo->wRoot );
   }

   // ?????.
   lpInfo->wRoot = LZH_MakeTree( lpInfo, LZH_NP, lpInfo->wPtFreq, lpInfo->bPtLen, lpInfo->wPtCode );

   if ( lpInfo->wRoot >= LZH_NP )
SecondWritePtLen:
   // ?????.
   {
      LZH_SendBlockSetSate( lpInfo, LZH_SB_2NDWRITEPTLEN );
      LZH_WritePtLen( lpInfo, LZH_NP, LZH_PBIT, ( WORD ) -1 );
   }
   else
SecondSmallRoot:
   // ?????.
   {
      LZH_SendBlockSetSate( lpInfo, LZH_SB_2NDSMALLROOT );
      LZHUtil_PutBits( lpInfo, LZH_PBIT, 0 );
      LZHUtil_PutBits( lpInfo, LZH_PBIT, lpInfo->wRoot );
   }

   // ?????.
   for ( lpInfo->wIndx = 0, lpInfo->wPos = 0; lpInfo->wIndx < lpInfo->wSize; lpInfo->wIndx++ )
   {
      if ( lpInfo->wIndx % CHAR_BIT == 0 )
      {
         lpInfo->wFlags = lpInfo->lpbOutBuf[ lpInfo->wPos++ ];
      }
      else
      {
         lpInfo->wFlags <<= 1;
      }

      if ( lpInfo->wFlags & (WORD)( 1U << ( CHAR_BIT - 1 ) ) )
EncodePostn:
      // ?????.
      {
         WORD wPostn;
         WORD wPos = lpInfo->wPos;

         LZH_SendBlockSetSate( lpInfo, LZH_SB_ENCODEPOSTN );
         LZH_EncodeCh( lpInfo, (WORD)(lpInfo->lpbOutBuf[ wPos++ ] + ( 1U << CHAR_BIT )) );
         wPostn  = lpInfo->lpbOutBuf[ wPos++ ] << CHAR_BIT;
         wPostn += lpInfo->lpbOutBuf[ wPos++ ];
         LZH_EncodePostn( lpInfo, wPostn );
         lpInfo->wPos = wPos;
      }
      else
EncodeCh:
      // ?????.
      {
         LZH_SendBlockSetSate( lpInfo, LZH_SB_ENCODECH );
         LZH_EncodeCh( lpInfo, lpInfo->lpbOutBuf[ lpInfo->wPos ] );
         lpInfo->wPos++;
      }
   }

   if ( fFlush )
FlushBits:
   // Write out any bits contained in the "bit buffer".
   {
      LZH_SendBlockSetSate( lpInfo, LZH_SB_FLUSHBITS );
      LZHUtil_PutBits( lpInfo, CHAR_BIT - 1, 0);
   }

   // ?????.
   _fmemset( lpInfo->wChFreq, 0, LZH_NC * sizeof( lpInfo->wChFreq[ 0 ] ) );

   // ?????.
   _fmemset( lpInfo->wPtFreq, 0, LZH_NP * sizeof( lpInfo->wPtFreq[ 0 ] ) );

   // The SendBlock() was successful.
   LZH_SendBlockSetSate( lpInfo, LZH_SB_START );
} // LZH_SendBlock



//==========================================================================
//
// VOID LZH_Output( lpInfo, wCh, wPostn )
//
//    lpInfo - Pointer to the structure that is used to contain state
//             information for the compression method.
//    wCh    -
//    wPostn -
//
// This function ...
//
//==========================================================================

VOID WINAPI LZH_Output(
   LPLZH_ENCODEINFOLZH45 lpInfo,
   WORD                  wCh,
   WORD                  wPostn
)

{
   if ( ( lpInfo->wOutptMask >>= 1 ) == 0 )
   // ?????.
   {
      lpInfo->wOutptMask = 1U << ( CHAR_BIT - 1 );

      if ( lpInfo->wOutptIndx >= LZH_OUT_BUF_SIZE - ( 3 * CHAR_BIT ) )
      // ?????.
      {
         LZH_SendBlock( lpInfo, FALSE );
         lpInfo->wOutptIndx = 0;
      }

      lpInfo->wChIndx                      = lpInfo->wOutptIndx++;
      lpInfo->lpbOutBuf[ lpInfo->wChIndx ] = 0;
   }

   // ?????.
   lpInfo->lpbOutBuf[ lpInfo->wOutptIndx++ ] = ( BYTE ) wCh;
   lpInfo->wChFreq[ wCh ]++;

   if ( wCh >= (WORD)( 1U << CHAR_BIT ) )
   // ?????.
   {
      lpInfo->lpbOutBuf[ lpInfo->wChIndx ]      |= lpInfo->wOutptMask;
      lpInfo->lpbOutBuf[ lpInfo->wOutptIndx++ ]  = ( BYTE ) ( wPostn >> CHAR_BIT );
      lpInfo->lpbOutBuf[ lpInfo->wOutptIndx++ ]  = ( BYTE ) wPostn;

      // ?????.
      for ( wCh = 0;  wPostn != 0; wPostn >>= 1, wCh++ );
      lpInfo->wPtFreq[ wCh ]++;
   }

} // LZH_Output
