/*
 *==========================================================================
 *
 * $Workfile:   lzhutil.c  $
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
 * using the LZH-5 method.
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

//-----------------------------------------------------------
//   S E G M E N T  F I L E
//-----------------------------------------------------------
#ifdef SYM_BUILD_SM
#include "wnfsv.seg"
#else
#include "..\inc\wnfsv.seg"
#endif

//-----------------------------------------------------------
//   G L O B A L   V A R I A B L E S
//-----------------------------------------------------------

jmp_buf  gnJmp[LZH_JUMP_COUNT];
int      gnJmpErr;
int      gcnJmp = 0;



//-----------------------------------------------------------
//   G L O B A L   F U N C T I O N S
//-----------------------------------------------------------


//==========================================================================
//
// VOID WINAPI LZHUtil_FillBuf( lpInfo, wNumBits )
//
//    lpInfo   - Pointer to the structure that is used to contain state
//               information for the de-compression method.
//    wNumBits - The number of bits to load into the input word.  This
//               number cannot exceed the capacity of a WORD.
//
// This function tries to read in wNumBits from the input buffer.
//
// Returns:
//    FALSE - There is insufficient data in the input buffer to satisfy
//            the request.
//    TRUE  - The bits were successfully loaded into the "holding" buffer.
//
//==========================================================================

VOID WINAPI LZHUtil_FillBuf(
   LPLZH_DECODEINFOLZH45 lpInfo,
   WORD                  wNumBits
)

{
   BOOL fLoadByt = ( wNumBits > lpInfo->biBitInfo.wBitCnt );

   if ( fLoadByt && ( ( wNumBits - lpInfo->biBitInfo.wBitCnt + ( CHAR_BIT - 1 ) ) >> 3 ) > lpInfo->biBitInfo.wByts &&
        !lpInfo->biBitInfo.fEOF )
   // There is not enough data in the input buffer to satisfy the request.
   {
      gnJmpErr = LZH_END_OF_INPUT_BUF;
      longjmp( gnJmp[--gcnJmp], -1 );
//      Throw( lpInfo->nCatchBuf, LZH_END_OF_INPUT_BUF );
   }

   // Remove the specified number of bits.
   lpInfo->biBitInfo.wBitBuf <<= wNumBits;

   // Load in appropriate number of bytes.
   while ( fLoadByt )
   {
      // ?????.
      wNumBits                  -= lpInfo->biBitInfo.wBitCnt;
      lpInfo->biBitInfo.wBitBuf |= lpInfo->biBitInfo.wSubBitBuf << wNumBits;

      if ( lpInfo->biBitInfo.wByts != 0 )
      // Get the next byte of input.
      {
         lpInfo->biBitInfo.wSubBitBuf = ( WORD ) *lpInfo->biBitInfo.lpCurByt;
         lpInfo->biBitInfo.lpCurByt++;
         lpInfo->biBitInfo.wByts--;
      }
      else
      // At end of file so all bits are zero.
      {
         lpInfo->biBitInfo.wSubBitBuf = 0;
      }

      // ?????.
      lpInfo->biBitInfo.wBitCnt = CHAR_BIT;
      fLoadByt                  = ( wNumBits > CHAR_BIT );
   }

   // ?????.
   lpInfo->biBitInfo.wBitCnt -= wNumBits;
   lpInfo->biBitInfo.wBitBuf |= lpInfo->biBitInfo.wSubBitBuf >> lpInfo->biBitInfo.wBitCnt;
} // LZHUtil_FillBuf



//==========================================================================
//
// VOID WINAPI LZHUtil_BitReadInit( lpBitInfo )
//
//    lpBitInfo - Pointer to the structure used to buffer bit oriented i/o.
//    wBitCnt   - The number of bits to load into the input word.  The number
//                of bits to return cannot exceed the capacity of a WORD.
//
// This function initializes the structure used to buffer bit oriented
// i/o.
//
//==========================================================================

VOID WINAPI LZHUtil_InitGetBits(
   LPLZHUTIL_BITINFO lpBitInfo
)

{
   memset( lpBitInfo, 0, sizeof( LZHUTIL_BITINFO ) );
} // LZHUtil_InitGetBits



//==========================================================================
//
// VOID LZHUtil_SetupGetBits( lpInfo, lpInptBuf, wInptByts )
//
//    lpInfo    - Pointer to the structure that is used to contain state
//                information for the de-compression method.
//    lpInptBuf - Pointer to the input buffer that contains the stream
//                of bits.
//    wInptByts - The number of input bytes.
//    fEOF      - This flag is set to TRUE if this is the last buffer of
//                data.
//
// This function initializes the sturcture used to process an input
// buffer containing bytes as a bit stream.
//
//==========================================================================

VOID WINAPI LZHUtil_SetupGetBits(
   LPLZH_DECODEINFOLZH45 lpInfo,
   LPBYTE                lpInptBuf,
   WORD                  wInptByts,
   BOOL                  fEOF
)

{
   BOOL fFillBits = ( lpInfo->biBitInfo.lpBuffr == NULL );

   // Store state info used to process the input buffer.
   lpInfo->biBitInfo.lpBuffr  = lpInptBuf;
   lpInfo->biBitInfo.lpCurByt = lpInptBuf;
   lpInfo->biBitInfo.wByts    = wInptByts;
   lpInfo->biBitInfo.fEOF     = fEOF;

   if ( fFillBits )
   // Fill the "holding" word with the 1st 16 bits of compressed data.
   {
      LZHUtil_FillBuf( lpInfo, LZHUTIL_BITBUFSIZ );
   }

} // LZHUtil_SetupGetBits



//==========================================================================
//
// WORD LZHUtil_GetBits( lpInfo, wNumBits )
//
//    lpInfo   - Pointer to the structure that is used to contain state
//               information for the de-compression method.
//    wNumBits - The number of bits to retreive.  This number cannot
//               exceed the capacity of a WORD.
//
// This function returns the desired number of bits and then loads the
// same number of bits back into the holding buffer.
//
//==========================================================================

WORD WINAPI LZHUtil_GetBits(
   LPLZH_DECODEINFOLZH45 lpInfo,
   WORD                  wNumBits
)

{
   WORD wBits;

   wBits = lpInfo->biBitInfo.wBitBuf >> ( LZHUTIL_BITBUFSIZ - wNumBits );
   LZHUtil_FillBuf( lpInfo, wNumBits );

   return( wBits );
} // LZHUtil_GetBits



//==========================================================================
//
// VOID LZHUtil_SetupPutBits( lpBitInfo, lpOutptBuf, wOutptByts )
//
//    lpBitInfo  - Pointer to the structure used to buffer bit oriented i/o.
//    lpOutptBuf - Pointer to the output buffer that will contain the stream
//                 of bits.
//    wOutptByts - The size of the output buffer.
//
// This function initializes the structure used to process an output
// buffer as a bit stream.
//
//==========================================================================

VOID WINAPI LZHUtil_SetupPutBits(
   LPLZHUTIL_BITINFO lpBitInfo,
   LPBYTE            lpOutptBuf,
   WORD              wOutptByts
)

{
   lpBitInfo->lpBuffr  = lpOutptBuf;
   lpBitInfo->lpCurByt = lpOutptBuf;
   lpBitInfo->wByts    = wOutptByts;
   lpBitInfo->fEOF     = FALSE;
} // LZHUtil_SetupPutBits



//==========================================================================
//
// VOID LZHUtil_InitPutBits( lpBitInfo )
//
//    lpBitInfo - Pointer to the structure used to buffer bit oriented i/o.
//
// This function initializes bit i/o to the output stream.
//
//==========================================================================

VOID WINAPI LZHUtil_InitPutBits(
   LPLZHUTIL_BITINFO lpBitInfo
)

{
   memset( lpBitInfo, 0, sizeof( LZHUTIL_BITINFO ) );
   lpBitInfo->wBitCnt = CHAR_BIT;
} // LZHUtil_InitPutBits



//==========================================================================
//
// VOID LZHUtil_PutBits( lpInfo, wNumBits, wBits )
//
//    lpInfo   - Pointer to the structure that is used to contain state
//               information for the compression method.
//    wNumBits - The number of bits to write.  This number cannot
//               exceed the capacity of a WORD.
//    wBits    - The bits to add to the output stream.
//
// This function tries to write the specified number of bits to the output
// stream.
//
//==========================================================================

VOID WINAPI LZHUtil_PutBits(
   LPLZH_ENCODEINFOLZH45 lpInfo,
   WORD                  wNumBits,
   WORD                  wBits
)

{
   if ( wNumBits < lpInfo->biBitInfo.wBitCnt )
   // Store bits in the current byte.
   {
      lpInfo->biBitInfo.wBitCnt -= wNumBits;
      lpInfo->biBitInfo.wBitBuf |= wBits << lpInfo->biBitInfo.wBitCnt;
   }
   else
   {
      // Account for bits that can be stored in the current byte.
      wNumBits -= lpInfo->biBitInfo.wBitCnt;

      if ( ( wNumBits / CHAR_BIT ) + 1 > lpInfo->biBitInfo.wByts )
      // There is not enough space in the output buffer.
      {
         gnJmpErr = LZH_END_OF_OUTPUT_BUF;
         longjmp( gnJmp[--gcnJmp], -1 );
//         Throw( lpInfo->nCatchBuf, LZH_END_OF_OUTPUT_BUF );
      }

      // Store the current byte in the output buffer.
      *lpInfo->biBitInfo.lpCurByt = ( BYTE ) ( lpInfo->biBitInfo.wBitBuf | ( wBits >> wNumBits ) );
      lpInfo->biBitInfo.lpCurByt++;
      lpInfo->biBitInfo.wByts--;

      if ( wNumBits < CHAR_BIT )
      // Store rest of bits in the byte buffer.
      {
         lpInfo->biBitInfo.wBitCnt = CHAR_BIT - wNumBits;
         lpInfo->biBitInfo.wBitBuf = wBits << lpInfo->biBitInfo.wBitCnt;
      }
      else
      // Store another byte in the output buffer.
      {
         // Store the current byte in the output buffer.
         *lpInfo->biBitInfo.lpCurByt = ( BYTE ) ( wBits >> ( wNumBits - CHAR_BIT ) );
         lpInfo->biBitInfo.lpCurByt++;
         lpInfo->biBitInfo.wByts--;

         // Store rest of bits in the byte buffer.
         lpInfo->biBitInfo.wBitCnt = ( 2 * CHAR_BIT ) - wNumBits;
         lpInfo->biBitInfo.wBitBuf = wBits << lpInfo->biBitInfo.wBitCnt;
      }
   }

} // LZHUtil_PutBits
