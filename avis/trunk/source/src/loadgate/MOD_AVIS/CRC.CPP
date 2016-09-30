//===========================================================================
//
//                               CRC.CPP
//
// Application to do stress and intensive tests for Immune system gateway
// functionality.
//
//===========================================================================


// Suppress warnings, coming from Bill Gates's code.
//
#pragma  warning (disable : 4201)   // Suppress warning: "Nonstandard extension used..." (from MS *.H files.)
#pragma  warning (disable : 4514)   // Suppress warning: "Unreferenced inline function has been removed..."

#include <windows.h>
#include <process.h>
#include <conio.h>
#include <stdio.h>

#include "MOD_MD5Calculator.h"
#include "crc.h"


//---------------------------------------------------------------------------
// Local definitions.
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Globals.
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Local function prototypes.
//---------------------------------------------------------------------------

//===========================================================================
//
// Function calculates 128bit(16 bytes) CRC of buffer and returns string 
// representation of it.
// (This is wrapper around MD5 CRC calculation code from AVIScommon 
// directory, excessively polluted by unnecessary C++ constructs).
//
// Out buffer: 'pCrcString' must be at least 16*2+1 bytes in size.

VOID  OLD_GetBufferCRCString (
         const char  *pBuffer,
         DWORD       dwBuffersize,
         char        *pCrcString
         )
{
//---------------------------------------------------------------------------
   MD5Calculator  md5Calc;
   char           acCrcString [40];
//---------------------------------------------------------------------------

   md5Calc.CRCCalculate ((const unsigned char *)pBuffer, dwBuffersize);
   md5Calc.Done (acCrcString);

   strcpy (pCrcString, acCrcString);
}
//===========================================================================

//===========================================================================
//
// It is ok to pass NULL and 0 for buffer params.
// Function just do nothing in that case.
//
// Function must be called last time with 'bLastBuffer'==TRUE to cleanup
// resources allocated during the previous calls.

VOID  *CalculateBufferCRCString (
         VOID        *pvContext,       //   NULL-first run | return code from previous call.
         const char  *pBuffer,         //   NULL|Buffer with binary data for CRC calcs.
         DWORD       dwBufferSize,     //   0|Size of buffer.
         BOOL        bLastBuffer /*=FALSE*/,   //   TRUE:last buffer in the sequence of buffers.
         char        *pCrcString   /*=NULL*/   // NULL|ptr to where to return CRC string. Must not be NULL, if 'bLastBuffer' is TRUE.
         )
{
//---------------------------------------------------------------------------
   MD5Calculator  *pMD5Calculator = (MD5Calculator  *)pvContext;
   char           acCrcString [40];
//---------------------------------------------------------------------------

   if (!pMD5Calculator)
      // First time run.
      pMD5Calculator = new MD5Calculator;

   // Calculate [next] CRC part.
   if (pBuffer && dwBufferSize)
      pMD5Calculator->CRCCalculate ((const unsigned char *)pBuffer, dwBufferSize);

   if (bLastBuffer)
      {
      pMD5Calculator->Done (acCrcString);
      if (pCrcString)
         strcpy (pCrcString, acCrcString);
      delete pMD5Calculator;

      return ((VOID *)NULL);
      }
   else
      return ((VOID *)pMD5Calculator);
}
//===========================================================================

