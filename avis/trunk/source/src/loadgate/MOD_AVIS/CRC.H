//===========================================================================
// 
//                               CRC.H
// 
// 
// 
//===========================================================================



#ifndef  _CRC_H_
#define  _CRC_H_


//---------------------------------------------------------------------------
// Forward declarations.
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Definitions.
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Classes.
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Function prototypes.
extern VOID  *CalculateBufferCRCString (
         VOID        *pvContext,          //   NULL-first run | return code from previous call.
         const char  *pBuffer,            //   NULL|Buffer with binary data for CRC calcs.
         DWORD       dwBufferSize,        //   0|Size of buffer.
         BOOL        bLastBuffer = FALSE, //   TRUE:last buffer in the sequence of buffers.
         char        *pCrcString = NULL   // NULL|ptr to where to return CRC string. Must not be NULL, if 'bLastBuffer' is TRUE.
         );
//---------------------------------------------------------------------------


#endif   // _CRC_H_
