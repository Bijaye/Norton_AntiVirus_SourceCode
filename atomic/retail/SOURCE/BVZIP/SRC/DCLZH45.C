/*
 *==========================================================================
 *
 * $Workfile:   dclzh45.c  $
 * $Revision:   1.0  $
 * $Modtime :$
 *
 * Module used to expand LZH-5.
 *
 * Copyright (c) 1993 by Central Point Software.  All rights reserved
 *
 * Author      : Jon J. Sorensen
 * Language    : Microsoft C9.00
 * Model       : 32Bit Flat
 * Environment : Microsoft Windows 95 SDK
 *
 * This module contains functions used to expand a file that was compressed
 * using the LZH-4 and LZH-5 methods.
 *
 * Date:
 * Log:
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

// Program include files.
#include "lzh.p"
#include "lzh.h"

// Segment file.
#ifdef SYM_BUILD_SM
#include "wnfsv.seg"
#else
#include "..\inc\wnfsv.seg"
#endif

//-----------------------------------------------------------
//   C O N S T A N T S
//-----------------------------------------------------------

// The various decode states.  These states are used to resume decoding of
// the input stream after an end of input buffer exception hae been
// generated.
#define LZH_DC_START           0
#define LZH_DC_GETBLOCKSIZE    1
#define LZH_DC_READPTLEN       2
#define LZH_DC_READCHLEN       3
#define LZH_DC_READPTLENAGAIN  4
#define LZH_DC_GETCHARORCODE   5
#define LZH_DC_GETPATTERNPOSTN 6


//-----------------------------------------------------------
//   T Y P E   D E F I N I T I O N S
//-----------------------------------------------------------


//-----------------------------------------------------------
//   M A C R O S
//-----------------------------------------------------------

#define LZH_DecodeSetSate(lpInfo, wState) ( *lpInfo ).wDecodeState = ( wState )
#define LZH_MarkBufState(lpInfo) ( *lpInfo ).biMarkBitInfo = ( *lpInfo ).biBitInfo
#define LZH_RestoreBufState(lpInfo) ( *lpInfo ).biBitInfo = ( *lpInfo ).biMarkBitInfo

#define LZH_GetDictPtr(lpInfo, wIndx) &( *lpInfo ).bDict[ ( wIndx ) ]

// Wrap the sliding dictionary buffer pointer.
// Point to next character in the sliding dictionary.
#define LZH_IncCircBufPtr(lpInfo, lpbBuf, wIndx)\
   if ( wIndx == LZH_DICSIZ - 1 )\
   {\
      wIndx  = 0;\
      lpbBuf = lpInfo->bDict;\
   }\
   else\
   {\
      wIndx++;\
      lpbBuf++;\
   }


//-----------------------------------------------------------
//   P R O T O T Y P E S
//-----------------------------------------------------------


//-----------------------------------------------------------
//   G L O B A L   V A R I A B L E S
//-----------------------------------------------------------



//-----------------------------------------------------------
//   G L O B A L   F U N C T I O N S
//-----------------------------------------------------------


//==========================================================================
//
// WORD LZH_DecodeLZH45WorkSize( VOID  )
//
// This function returns the size the structure required for the LZH (4 & 5)
// de-compression function.
//
//==========================================================================

WORD WINAPI LZH_DecodeLZH45WorkSize( VOID )

{
   return( sizeof( LZH_DECODEINFOLZH45 ) );
} // LZH_DecodeLZH45WorkSize



//==========================================================================
//
// VOID LZH_DecodeLZH45Init( lpvInfo, wMeth )
//
//    lpvInfo - Pointer to the structure that is used to contain state
//              information for the de-compression method.
//    wMeth   - The type of LZH method to expand (4 or 5).
//
// This function initializes the state info used to "expand" a file that
// has been compressed using LZH-4 or LZH-5.
//
//==========================================================================

VOID WINAPI LZH_DecodeLZH45Init(
   LPVOID lpvInfo,
   WORD   wMeth
)

{
   LPLZH_DECODEINFOLZH45 lpInfo = lpvInfo;

   _fmemset( lpInfo, 0, sizeof( LZH_DECODEINFOLZH45 ) );

   LZHUtil_InitGetBits( &lpInfo->biBitInfo );
   LZH_DecodeSetSate( lpInfo, LZH_DC_START );

   lpInfo->lpbDict = lpInfo->bDict;
} // LZH_DecodeLZH45Init



//==========================================================================
//
// BOOL LZH_DecodeLZH45( lpbInBuf, lpwInptSiz, lpbOutBuf, lpwOutptSiz,
//                       lpvVars, cEOF )
//
//    lpbInBuf    - Pointer to the buffer containing the compressed data.
//    lpwInptSiz  - Pointer to the amount of bytes in the input buffer.
//                  The amount variable will be updated by subtracting the
//                  number of input bytes that were processed.
//    lpbOutBuf   - Pointer to the buffer that will contain the uncompressed
//                  data.
//    lpwOutptSiz - Pointer to the number of bytes that can be contained in
//                  the output buffer.  The affiliate variable will be
//                  updated to contain the number of output bytes that were
//                  produced.
//    lpvVars     - Pointer to the structure that is used to contain state
//                  information for the de-compression method.
//    cEOF        - A one byte flag that will be set to TRUE if this is the
//                  "last" input buffer.
//
// This function "expands" the data contained in the input buffer into
// the specified output buffer.  This function supports the data that
// has been compressed using LZH-4 and LZH-5.
//
// Returns:
//    FALSE - The caller needs to pass in more input.
//    TRUE  - All of data for the compressed file in question has been
//            expanded.
//
// Notes:
//    1) The size of the *lpbOutBuf must be at least 2K for LZH-4 and at
//       least 4K for LZH-5.
//    2) Do not alter contents *lpOutBut after this function has returned
//       because this function might need to copy a the rest of a repeated
//       pattern upon entry.
//    3) If function does not process all of the input buffer then it is
//       the caller's responsibility to ....
//
//==========================================================================

BOOL WINAPI LZH_DecodeLZH45(
   LPBYTE lpbInBuf,
   LPWORD lpwInptSiz,
   LPBYTE lpbOutBuf,
   LPWORD lpwOutptSiz,
   LPVOID lpvVars,
   char   cEOF
)

{
   LPLZH_DECODEINFOLZH45 lpInfo = lpvVars;
   WORD                  wByts  = 0;
   BOOL                  fDone  = FALSE;
   int                   nExpCode;

   // Init structure used to extract bit oriented i/o from the input buffer.
   LZHUtil_SetupGetBits( lpInfo, ( LPBYTE ) lpbInBuf, *lpwInptSiz, cEOF );

   gnJmpErr = 0;
   if ( (nExpCode = setjmp( gnJmp[gcnJmp++] )) == 0 )
   {
      WORD   wOutptByts = *lpwOutptSiz;
      BOOL   fDecode    = TRUE;
      LPBYTE lpbOutpt   = lpbOutBuf;
      LPBYTE lpbTmp     = LZH_GetDictPtr( lpInfo, lpInfo->wRepIndx );

      // If necessary, resume processing before the end of input buffer
      // exception was generated.
      switch ( lpInfo->wDecodeState )
      {
         case LZH_DC_GETBLOCKSIZE:
            goto GetBlockSize;
         break;

         case LZH_DC_READPTLEN:
            goto ReadPtLen;
         break;

         case LZH_DC_READCHLEN:
            goto ReadChLen;
         break;

         case LZH_DC_READPTLENAGAIN:
            goto ReadPtLenAgain;
         break;

         case LZH_DC_GETCHARORCODE:
            goto GetCharOrCode;
         break;

         case LZH_DC_GETPATTERNPOSTN:
            goto GetPatternPostn;
         break;
      }

      // If necessary, finish copying the "repeated pattern".
      while ( lpInfo->wRepByts > 0 )
      {
         // Store the byte in the output buffer.
         *lpbOutpt++ = *lpbTmp;

         // Store the character in the sliding dictionary.
         LZH_PutDictChar( lpInfo, *lpbTmp );

         // One less byte to copy.
         lpInfo->wRepByts--;

         // Point to next character in the sliding dictionary.
         LZH_IncCircBufPtr( lpInfo, lpbTmp, lpInfo->wRepIndx )

         if ( ++wByts == wOutptByts )
         // The output buffer has been filled.
         {
            fDecode = FALSE;
            break;
         }
      }

      // Decompress as much of the input buffer as possible.
      while ( fDecode )
      {
         WORD wCh;

         // Inline the LZH_DecodeCh() function so processing can be resumed
         // if the end of the input buffer was encountered.
         {
            if ( lpInfo->wBlockSize == 0 )
            // ?????.
            {
               if ( LZHUtil_InputFileExhausted( lpInfo ) )
               // The file has been successfully expanded.
               {
                  gnJmpErr = LZH_EOF;
                  longjmp( gnJmp[--gcnJmp], -1 );
               }

GetBlockSize:  // Get the size of the next input block.
               LZH_DecodeSetSate( lpInfo, LZH_DC_GETBLOCKSIZE );
               lpInfo->wBlockSize = LZHUtil_GetBits( lpInfo, LZHUTIL_BITS_PER_WORD );

ReadPtLen:     // Build the ....
               LZH_DecodeSetSate( lpInfo, LZH_DC_READPTLEN );
               LZH_MarkBufState( lpInfo );
               LZH_ReadPtLen( lpInfo, LZH_NT, LZH_TBIT, 3 );

ReadChLen:     // Build the ....
               LZH_DecodeSetSate( lpInfo, LZH_DC_READCHLEN );
               LZH_MarkBufState( lpInfo );
               LZH_ReadChLen( lpInfo );

ReadPtLenAgain:// Build the ....
               LZH_DecodeSetSate( lpInfo, LZH_DC_READPTLENAGAIN );
               LZH_MarkBufState( lpInfo );
               LZH_ReadPtLen( lpInfo, LZH_NP, LZH_PBIT, ( WORD ) -1 );
            }

GetCharOrCode:
            // ?????.
            LZH_DecodeSetSate( lpInfo, LZH_DC_GETCHARORCODE );
            wCh = lpInfo->wChTable[ lpInfo->biBitInfo.wBitBuf >> ( LZHUTIL_BITBUFSIZ - 12 ) ];

            if ( wCh >= LZH_NC )
            {
               WORD wMask;

               // ?????.
               wMask = (WORD)((WORD)1 << ( LZHUTIL_BITBUFSIZ - 1 - 12 ));

               do
               {
                  // ?????.
                  if ( lpInfo->biBitInfo.wBitBuf & wMask )
                  {
                     wCh = lpInfo->wRight[ wCh ];
                  }
                  else
                  {
                     wCh = lpInfo->wLeft[ wCh ];
                  }

                  // ?????.
                  wMask >>= 1;
               }
               while ( wCh >= LZH_NC );
            }

            // ?????.
            LZHUtil_FillBuf( lpInfo, lpInfo->bChLen[ wCh ] );

            // Keep track of the number of characters/codes that have been
            // expanded in the current block.
            lpInfo->wBlockSize--;
         }

         if ( wCh <= UCHAR_MAX )
         // A single character has been decoded.
         {
            // Store the byte in the output buffer.
            *lpbOutpt++ = ( BYTE ) wCh;

            // Store the character in the sliding dictionary.
            LZH_PutDictChar( lpInfo, ( BYTE ) wCh );

            if ( ++wByts == wOutptByts )
            // The output buffer has been filled.
            {
               fDecode = FALSE;
               break;
            }
         }
         else
         // A "repeated pattern" has been encountered.
         {
            WORD wCode;

            // Compute size of the "repeated pattern".
            lpInfo->wRepByts = wCh - ( UCHAR_MAX + 1 - LZH_THRESHOLD );

            // Inline the LZH_DecodeP() function so processing can be resumed
            // if the end of the input buffer was encountered.
            {
               WORD wMask;

GetPatternPostn:
               // ?????.
               LZH_DecodeSetSate( lpInfo, LZH_DC_GETPATTERNPOSTN );
               LZH_MarkBufState( lpInfo );
               wCode = lpInfo->wPtTable[ lpInfo->biBitInfo.wBitBuf >> ( LZHUTIL_BITBUFSIZ - 8 ) ];

               if ( wCode >= LZH_NP )
               {
                  // ?????.
                  wMask = (WORD)((WORD)1 << ( LZHUTIL_BITBUFSIZ - 1 - 8 ));

                  // ?????.
                  do
                  {
                     if ( lpInfo->biBitInfo.wBitBuf & wMask )
                     {
                        wCode = lpInfo->wRight[ wCode ];
                     }
                     else
                     {
                        wCode = lpInfo->wLeft[ wCode ];
                     }

                     // ?????.
                     wMask >>= 1;
                  }
                  while ( wCode >= LZH_NP );
               }

               // ?????.
               LZHUtil_FillBuf( lpInfo, lpInfo->bPtLen[ wCode ] );

               if ( wCode != 0 )
               // ?????.
               {
                  wCode = (WORD)( (WORD)1 << ( wCode - 1 ) ) + LZHUtil_GetBits( lpInfo, (WORD)(wCode - 1) );
               }
            }

            // We are back to the normal entry state.
            LZH_DecodeSetSate( lpInfo, LZH_DC_START );

            // Point to start of the "repeated pattern" unless at the end
            // of the dictionary (the overflow should not happen).
//          lpInfo->wRepIndx = ( wByts - wCode - 1 ) & ( LZH_DICSIZ - 1 );
            lpInfo->wRepIndx = ( lpInfo->wDictIndx - wCode - 1 ) & ( LZH_DICSIZ - 1 );
//          lpbTmp           = &lpbOutBuf[ lpInfo->wRepIndx ];
            lpbTmp           = LZH_GetDictPtr( lpInfo, lpInfo->wRepIndx );

            // Copy as much of the "repeated pattern" as possible.
            while ( lpInfo->wRepByts > 0 )
            {
               // Store the byte in the output buffer.
               *lpbOutpt++ = *lpbTmp;

               // Store the character in the sliding dictionary.
               LZH_PutDictChar( lpInfo, *lpbTmp );

               // One less byte to copy.
               lpInfo->wRepByts--;

               // Point to next character in the sliding dictionary.
               LZH_IncCircBufPtr( lpInfo, lpbTmp, lpInfo->wRepIndx )

               if ( ++wByts == wOutptByts )
               // The output buffer has been filled.
               {
                  fDecode = FALSE;
                  break;
               }
            }
         }
      }
      --gcnJmp;
   }
   else
   // Fatal error, end of input buffer, or file has been successfully
   // expanded.
   {
      // Handle the various exception conditions.
      --gcnJmp;
      switch ( gnJmpErr )
      {
         case LZH_FATAL_ERROR:
         default:
            // Force a CRC error;
            wByts = 0;
            fDone = TRUE;
         break;

         // Infrom caller that all of the input has been processed.
         case LZH_EOF:
            fDone = TRUE;
         break;

         case LZH_END_OF_INPUT_BUF:
            if ( lpInfo->wDecodeState == LZH_DC_READPTLEN ||
                 lpInfo->wDecodeState == LZH_DC_READCHLEN ||
                 lpInfo->wDecodeState == LZH_DC_READPTLENAGAIN ||
                 lpInfo->wDecodeState == LZH_DC_GETPATTERNPOSTN )
            // Start decoding from the beginning.  Request more input
            // from the caller.
            {
               LZH_RestoreBufState( lpInfo );
            }
         break;
      }
   }

   // Return the number of input bytes processed and the number of
   // output bytes produced.
   *lpwInptSiz  = LZHUtil_NumBitBufBytsUsed( &lpInfo->biBitInfo );
   *lpwOutptSiz = wByts;

   return( fDone );
} // LZH_DecodeLZH45



//-----------------------------------------------------------
//   L O C A L   F U N C T I O N S
//-----------------------------------------------------------


//==========================================================================
//
// VOID LZH_PutDictChar( lpInfo, bCh )
//
//    lpInfo    - Pointer to the structure that is used to contain state
//                information for the de-compression method.
//    bCh       - The character to store in the sliding dictionary.
//
// This function stores a character in the sliding dictionary.  If the
// end of the dictionary is encountered then the pointer wraps around
// to the beginning.
//
//==========================================================================

VOID INLINE LZH_PutDictChar(
   LPLZH_DECODEINFOLZH45 lpInfo,
   BYTE                  bCh
)

{
   *lpInfo->lpbDict = bCh;
   LZH_IncCircBufPtr( lpInfo, lpInfo->lpbDict, lpInfo->wDictIndx )
} // LZH_PutDictChar



//==========================================================================
//
// WORD LZH_MakeTable( lpInfo, wCh, lpbBitLen, wTblBits, lpwTable )
//
//    lpInfo    - Pointer to the structure that is used to contain state
//                information for the de-compression method.
//    wCh       - ?????.
//    lpbBitlen - ?????.
//    wTblBits  - ?????.
//    lpwTable  - ?????.
//
// This function ...
//
// Notes:
//    1) It is assummed that the caller has ensured that the input buffer
//       contains enough data to contain ....
//
//==========================================================================

VOID WINAPI LZH_MakeTable(
   LPLZH_DECODEINFOLZH45 lpInfo,
   WORD                  wCh,
   LPBYTE                lpbBitLen,
   WORD                  wTblBits,
   LPWORD                lpwTable
)

{
   WORD   wCount[ 17 ];
   WORD   wWeight[ 17 ];
   WORD   wStart[ 18 ];
   LPWORD lpwTmp1;
   LPWORD lpwTmp2;
   WORD   wAvail;
   WORD   wCh1;
   WORD   wIndx;
   WORD   wJutBits;
   WORD   wMask;

   // ?????.
   _fmemset( wCount, 0, sizeof( wCount ) );
// for (wIndx = 1; wIndx <= 16; wIndx++) wCount[wIndx] = 0;

   // ?????.
   for ( wIndx = 0; wIndx < wCh; wIndx++ )
   {
      wCount[ lpbBitLen[ wIndx ] ]++;
   }

   // ?????.
   for ( wIndx = 1, wStart[ 1 ] = 0, lpwTmp1 = &wStart[ 1 ], lpwTmp2 = &wCount[ 1 ];
         wIndx <= 16;
         wIndx++, lpwTmp1++, lpwTmp2++ )
   {
      lpwTmp1[ 1 ] = lpwTmp1[ 0 ] + ( lpwTmp2[ 0 ] << ( 16 - wIndx ) );
   }

   if ( wStart[ 17 ] != (WORD)((WORD)1 << 16) )
   // Fatal error.  Bad table encountered.
   {
      gnJmpErr = LZH_FATAL_ERROR;
      longjmp( gnJmp[--gcnJmp], -1 );
   }

   // ?????.
   for ( wJutBits = 16 - wTblBits, wIndx = 1, lpwTmp1 = &wStart[ 1 ], lpwTmp2 = &wWeight[ 1 ];
         wIndx <= wTblBits;
         wIndx++, lpwTmp1++, lpwTmp2++ )
   {
      lpwTmp1[ 0 ] >>= wJutBits;
      lpwTmp2[ 0 ]   = (WORD)((WORD)1 << ( wTblBits - wIndx ));
   }

   // *****NUKE*****
   {
      WORD i = 0;
      WORD j[5];
      j[ i++ ] = (WORD)((WORD)1 << ( 16 - i ));
   }
   // *****NUKE*****

   // ?????.
   for ( ; wIndx <= 16; wIndx++, lpwTmp2++ )
   {
      lpwTmp2[ 0 ] = (WORD)((WORD)1 << ( 16 - wIndx )); //***** weight[i++] = (WORD)1 << (16-i);
   }

   // ?????.
   wIndx = wStart[ wTblBits + 1 ] >> wJutBits;

   if ( wIndx != (WORD)( (WORD)1 << 16 ) )
   // ?????.
   {
      WORD wLimit;

      // ?????.
      wLimit = (WORD)((WORD)1 << wTblBits);

      if ( wLimit > wIndx )
      // ?????.
      {
         _fmemset( &lpwTable[ wIndx ], 0, ( wLimit - wIndx ) * sizeof( WORD ) );
      }
   }

   // ?????.
   wAvail = wCh;
   wMask  = (WORD)((WORD)1 << ( 15 - wTblBits ));

   // ?????.
   for ( wCh1 = 0; wCh1 < wCh; wCh1++, lpbBitLen++ )
   {
      WORD wLen;
      WORD wNxtCode;

      // ?????.
      wLen = lpbBitLen[ 0 ];

      if ( wLen == 0 )
      // ?????.
      {
         continue;
      }

      // ?????.
      wNxtCode = wStart[ wLen ] + wWeight[ wLen ];

      if ( wLen <= wTblBits )
      // ?????.
      {
         for ( wIndx = wStart[ wLen ], lpwTmp1 = &lpwTable[ wIndx ];
               wIndx < wNxtCode;
               wIndx++, lpwTmp1++ )
         {
            lpwTmp1[ 0 ] = wCh1;
         }
      }
      else
      // ?????.
      {
         WORD wLimit;

         // ?????.
         wLimit  = wStart[ wLen ];
         lpwTmp1 = &lpwTable[ wLimit >> wJutBits ];
         wIndx   = wLen - wTblBits;

         // ?????.
         while ( wIndx != 0 )
         {
            if ( *lpwTmp1 == 0 )
            // ?????.
            {
               lpInfo->wRight[ wAvail ] = lpInfo->wLeft[ wAvail ] = 0;
               *lpwTmp1 = wAvail++;
            }

            if ( wLimit & wMask )
            // ?????.
            {
               lpwTmp1 = &lpInfo->wRight[ *lpwTmp1 ];
            }
            else
            {
               lpwTmp1 = &lpInfo->wLeft[ *lpwTmp1 ];
            }

            // ?????.
            wLimit <<= 1;
            wIndx--;
         }

         // ?????.
         *lpwTmp1 = wCh1;
      }

      // ?????.
      wStart[ wLen ] = wNxtCode;
   }

} // LZH_MakeTable



//==========================================================================
//
// WORD LZH_ReadPtLen( lpInfo, wNumEntries, wNumBits, wSpecial )
//
//    lpInfo      - Pointer to the structure that is used to contain state
//                  information for the de-compression method.
//    wNumEntries - The maximum number of ...
//    wNumBits    - The ...
//    wSpecial    - ????.
//
// This function ...
//
// Notes:
//    1) It is assummed that the caller has ensured that the input buffer
//       contains enough data to contain ....
//
//==========================================================================

VOID WINAPI LZH_ReadPtLen(
   LPLZH_DECODEINFOLZH45 lpInfo,
   WORD                  wNumEntries,
   WORD                  wNumBits,
   WORD                  wSpecial
)

{
   WORD wNum;

   // Get ....
   wNum = LZHUtil_GetBits( lpInfo, wNumBits );

   if ( wNum == 0 )
   // ?????.
   {
      WORD   wIndx;
      WORD   wCh     = 0;
      LPWORD lpwItem = lpInfo->wPtTable;

      // Get the ...
      wCh = LZHUtil_GetBits( lpInfo, wNumBits );

      // Initialize the "???? length" table.
      _fmemset( lpInfo->bPtLen, 0, sizeof( BYTE ) * wNumEntries );

      // Initialize the "????" table.
      for ( wIndx = 0; wIndx < 256; wIndx++, lpwItem++ )
      {
         *lpwItem = wCh;
      }
   }
   else
   // ????.
   {
      LPBYTE lpbTmp = lpInfo->bPtLen;
      WORD   wIndx  = 0;
      WORD   wCh    = 0;

      // ?????.
      while ( wIndx < wNum )
      {
         // Manually extract the bits from the input buffer.  This is
         // done because ...
         wCh = lpInfo->biBitInfo.wBitBuf >> ( LZHUTIL_BITBUFSIZ - 3 );

         if ( wCh == 7 )
         // ?????.
         {
            WORD wMask;

            // ?????.
            wMask = (WORD)((WORD)1 << ( LZHUTIL_BITBUFSIZ - 1 - 3 ));

            // ?????.
            while ( wMask & lpInfo->biBitInfo.wBitBuf )
            // ?????.
            {
               wMask >>= 1;
               wCh++;
            }
         }

         // ?????.
         LZHUtil_FillBuf( lpInfo, (WORD)(( wCh < 7 ) ? 3 : wCh - 3) );

         // ?????.
         *lpbTmp = ( BYTE ) wCh;
         wIndx++;
         lpbTmp++;

         if ( wIndx == wSpecial )
         // ?????.
         {
            // ?????.
            wCh = LZHUtil_GetBits( lpInfo, 2 );

            // ?????.
            _fmemset( lpbTmp, 0, wCh );
            lpbTmp += wCh;
            wIndx  += wCh;
         }
      }

      if ( wNum > wNumEntries || wIndx > wNumEntries )
      // Fatal error.  The compressed data is not valid.
      {
         gnJmpErr = LZH_FATAL_ERROR;
         longjmp( gnJmp[--gcnJmp], -1 );
      }

      // Initialize the rest of the "length" entries.
      _fmemset( lpbTmp, 0, wNumEntries - wIndx );

      // ?????.
      LZH_MakeTable( lpInfo, wNumEntries, lpInfo->bPtLen, 8, lpInfo->wPtTable );
   }

} // LZH_ReadPtLen



//==========================================================================
//
// WORD LZH_ReadChLen( lpInfo )
//
//    lpInfo - Pointer to the structure that is used to contain state
//             information for the de-compression method.
//
// This function ...
//
// Notes:
//    1) It is assummed that the caller has ensured that the input buffer
//       contains enough data to contain ....
//
//==========================================================================

VOID WINAPI LZH_ReadChLen(
   LPLZH_DECODEINFOLZH45 lpInfo
)

{
   WORD wNum;

   // Get ...
   wNum = LZHUtil_GetBits( lpInfo, LZH_CBIT );

   if ( wNum == 0 )
   // ?????.
   {
      WORD   wIndx;
      WORD   wCh;
      LPWORD lpwTmp;

      // Get ...
      wCh = LZHUtil_GetBits( lpInfo, LZH_CBIT );

      // ?????.
      _fmemset( lpInfo->bChLen, 0, LZH_NC );

      // ?????.
      for ( wIndx = 0, lpwTmp = lpInfo->wChTable; wIndx < 4096; wIndx++, lpwTmp++ )
      {
         lpwTmp[ 0 ] = wCh;
      }
   }
   else
   // ?????.
   {
      WORD   wCh;
      WORD   wIndx = 0;
      LPBYTE lpbTmp = lpInfo->bChLen;

      // ?????.
      while ( wIndx < wNum )
      {
         // ?????.
         wCh = lpInfo->wPtTable[ lpInfo->biBitInfo.wBitBuf >> ( LZHUTIL_BITBUFSIZ - 8 ) ];

         if ( wCh >= LZH_NT)
         {
            WORD wMask;

            // ?????.
            wMask = (WORD)((WORD)1 << ( LZHUTIL_BITBUFSIZ - 1 - 8 ));

            // ?????.
            do
            {
               if ( lpInfo->biBitInfo.wBitBuf & wMask )
               {
                  wCh = lpInfo->wRight[ wCh ];
               }
               else
               {
                  wCh = lpInfo->wLeft[ wCh ];
               }

               wMask >>= 1;
            }
            while ( wCh >= LZH_NT );
        }

        // ?????.
        LZHUtil_FillBuf( lpInfo, lpInfo->bPtLen[ wCh ] );

        if ( wCh <= 2)
        {
            if ( wCh == 0 )
            {
               wCh = 1;
            }
            else
            if ( wCh == 1 )
            {
               wCh = LZHUtil_GetBits( lpInfo, 4 ) + 3;
            }
            else
            {
               wCh = LZHUtil_GetBits( lpInfo, LZH_CBIT ) + 20;
            }

            // ?????.
            _fmemset( lpbTmp, 0, wCh );
            lpbTmp += wCh;
            wIndx  += wCh;
         }
         else
         {
            lpbTmp[ 0 ] = ( BYTE ) ( wCh - 2 );
            lpbTmp++;
            wIndx++;
         }
      }

      if ( wNum > LZH_NC || wIndx > LZH_NC )
      // Fatal error.  The compressed data is not valid.
      {
         gnJmpErr = LZH_FATAL_ERROR;
         longjmp( gnJmp[--gcnJmp], -1 );
      }

      // ?????.
      _fmemset( lpbTmp, 0, LZH_NC - wIndx );

      // ?????.
      LZH_MakeTable( lpInfo, LZH_NC, lpInfo->bChLen, 12, lpInfo->wChTable );
   }

} // LZH_ReadChLen



#ifdef TheseFunctionsAreNotInLine
//==========================================================================
//
// WORD LZH_DecodeCh( lpInfo, cEOF )
//
//    lpInfo - Pointer to the structure that is used to contain state
//             information for the de-compression method.
//    cEOF   - A one byte flag that will be set to TRUE if this is the
//             "last" input buffer.
//
// This function ...
//
// Returns:
//    FALSE -
//    TRUE  - All of data for the compressed file in question has been
//            expanded.
//
//==========================================================================

WORD WINAPI LZH_DecodeCh(
   LPLZH_DECODEINFOLZH45 lpInfo,
   char                  cEOF
)

{
   WORD wCode;

   if ( lpInfo->wBlockSize == 0 )
   // ?????.
   {
      if ( LZHUtil_InputFileExhausted( lpInfo ) )
      // The file has been successfully expanded.
      {
         gnJmpErr = LZH_EOF;
         longjmp( gnJmp[--gcnJmp], -1 );
      }

//    if ( ZipUtil_InptBytsAvail( &lpInfo->biBitInfo ) < ???? )
//    // Request more input bytes.
//    {
//       return( );
//    }

      // Get the size of the next input block.
      lpInfo->wBlockSize = LZHUtil_GetBits( lpInfo, LZHUTIL_BITS_PER_WORD );

      // Build the ....
      LZH_ReadPtLen( lpInfo, LZH_NT, LZH_TBIT, 3 );

      // Build the ....
      LZH_ReadChLen( lpInfo );

      // Build the ....
      LZH_ReadPtLen( lpInfo, LZH_NP, LZH_PBIT, ( WORD ) -1 );
   }

   // Keep track of the number of characters/codes that have been
   // expanded in the current block.
   lpInfo->wBlockSize--;

   // ?????.
   wCode = lpInfo->wChTable[ lpInfo->biBitInfo.wBitBuf >> ( LZHUTIL_BITBUFSIZ - 12 ) ];

   if ( wCode >= LZH_NC )
   {
      WORD wMask;

      // ?????.
      wMask = (WORD)((WORD)1 << ( LZHUTIL_BITBUFSIZ - 1 - 12 ));

      do
      {
         // ?????.
         if ( lpInfo->biBitInfo.wBitBuf & wMask )
         {
            wCode = lpInfo->wRight[ wCode ];
         }
         else
         {
            wCode = lpInfo->wLeft[ wCode ];
         }

         // ?????.
         wMask >>= 1;
      }
      while ( wCode >= LZH_NC );
   }

   // ?????.
   LZHUtil_FillBuf( lpInfo, lpInfo->bChLen[ wCode ] );

   return( wCode );
} // LZH_DecodeCh



//==========================================================================
//
// WORD LZH_DecodeP( lpInfo )
//
//    lpInfo - Pointer to the structure that is used to contain state
//             information for the de-compression method.
//
// This function returns the index position of a repeated pattern that is
// contained in the ?????
//
//==========================================================================

WORD WINAPI LZH_DecodeP(
   LPLZH_DECODEINFOLZH45 lpInfo
)

{
   WORD wCode;
   WORD wMask;

   // ?????.
   wCode = lpInfo->wPtTable[ lpInfo->biBitInfo.wBitBuf >> ( LZHUTIL_BITBUFSIZ - 8 ) ];

   if ( wCode >= LZH_NP )
   {
      // ?????.
      wMask = (WORD)((WORD)1 << ( LZHUTIL_BITBUFSIZ - 1 - 8 ));

      // ?????.
      do
      {
         if ( lpInfo->biBitInfo.wBitBuf & wMask )
         {
            wCode = lpInfo->wRight[ wCode ];
         }
         else
         {
            wCode = lpInfo->wLeft[ wCode ];
         }

         // ?????.
         wMask >>= 1;
      }
      while ( wCode >= LZH_NP );
   }

   // ?????.
   LZHUtil_FillBuf( lpInfo, lpInfo->bPtLen[ wCode ] );

   if ( wCode != 0 )
   // ?????.
   {
      wCode = (WORD)( (WORD)1 << ( wCode - 1 ) ) + LZHUtil_GetBits( lpInfo, wCode - 1 );
   }

   return( wCode );
} // LZH_DecodeP
#endif



#ifdef HiDad
   if (method != '0')
   {
      decode_start();
   }

   while (origsize != 0)
   {
      n = (uint)((origsize > DICSIZ) ? DICSIZ : origsize);

      if (method != '0')
      {
         decode(n, buffer);
      }
      else
      if (fread((char *)buffer, 1, n, arcfile) != n)
      {
         error("Can't read");
      }

      fwrite_crc(buffer, n, outfile);

      if (outfile != stdout)
      {
         putc('.', stderr);
      }

      origsize -= n;
   }
#endif

#ifdef UseThis
if ( lpInfo->wRepIndx < LZH_DICSIZ - 1 )
// Point to next character in the buffer.
{
   lpInfo->wRepIndx++;
   lpbTmp++;
}
else
// Wrap around to beginning of the buffer.
{
   lpInfo->wRepIndx = 0;
   lpbTmp           = LZH_GetDictPtr( lpInfo, 0 );
}
#endif
