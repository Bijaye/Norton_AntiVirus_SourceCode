//************************************************************************
//
// $Header:   S:/NAVEX/VCS/WDSIGUTL.CPv   1.0   17 Jan 1997 11:24:00   DCHI  $
//
// Description:
//  Utility functions for getting values from signature streams.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/WDSIGUTL.CPv  $
// 
//    Rev 1.0   17 Jan 1997 11:24:00   DCHI
// Initial revision.
// 
//************************************************************************

#include "platform.h"

#include "wdsigutl.h"

//*************************************************************************
//
// int WDGetControlStreamNibble()
//
// Parameters:
//  lpabyControlStream  Ptr to beginning of control stream
//  lpnIndex            Ptr to int of nibble index in control stream
//
// Description:
//  Returns the value of the nibble of the control stream at the
//  given index.  The index is incremented before the function returns.
//
// Returns:
//  int                 Value of nibble
//
//*************************************************************************

int WDGetControlStreamNibble
(
    LPBYTE      lpabyControlStream,
    LPINT       lpnIndex
)
{
    int         nNibble;

    if ((*lpnIndex & 0x01) == 0)
    {
        // First nibble

        nNibble = lpabyControlStream[(*lpnIndex) >> 1] & 0x0F;
    }
    else
    {
        // Second nibble

        nNibble = (lpabyControlStream[(*lpnIndex) >> 1] >> 4) & 0x0F;
    }

    ++*lpnIndex;

    return(nNibble);
}


//*************************************************************************
//
// BYTE WDGetDataStreamBYTE()
//
// Parameters:
//  lplpabyDataStream   Ptr to ptr to current position in data stream
//
// Description:
//  Returns the BYTE value at the current position in the data stream.
//  The current position is incremented by one.
//
// Returns:
//  BYTE                Value of BYTE at current position
//
//*************************************************************************

BYTE WDGetDataStreamBYTE
(
    LPLPBYTE    lplpabyDataStream
)
{
    BYTE        by;

    by = **lplpabyDataStream;
    ++*lplpabyDataStream;
    return(by);
}


//*************************************************************************
//
// WORD WDGetDataStreamWORD()
//
// Parameters:
//  lplpabyDataStream   Ptr to ptr to current position in data stream
//
// Description:
//  Returns the WORD value at the current position in the data stream.
//  The current position is incremented by two.
//
//  The WORD value returned is properly endianized.
//
// Returns:
//  WORD                Value of WORD at current position
//
//*************************************************************************

WORD WDGetDataStreamWORD
(
    LPLPBYTE      lplpabyDataStream
)
{
    WORD        w;

    w = ((WORD)**lplpabyDataStream) |
        (((WORD)*(*lplpabyDataStream + 1)) << 8);
    *lplpabyDataStream += 2;
    return(w);
}


//*************************************************************************
//
// DWORD WDGetDataStreamDWORD()
//
// Parameters:
//  lplpabyDataStream   Ptr to ptr to current position in data stream
//
// Description:
//  Returns the DWORD value at the current position in the data stream.
//  The current position is incremented by four.
//
//  The DWORD value returned is properly endianized.
//
// Returns:
//  DWORD               Value of DWORD at current position
//
//*************************************************************************

DWORD WDGetDataStreamDWORD
(
    LPLPBYTE    lplpabyDataStream
)
{
    DWORD       dw;
    LPBYTE      lpabyDataStream;

    lpabyDataStream = *lplpabyDataStream;
    dw = ((DWORD)lpabyDataStream[0]) |
         (((DWORD)lpabyDataStream[1]) << 8) |
         (((DWORD)lpabyDataStream[2]) << 16) |
         (((DWORD)lpabyDataStream[3]) << 24);

    *lplpabyDataStream += 4;
    return(dw);
}


//*************************************************************************
//
// SBYTE WDGetDataStreamSBYTE()
//
// Parameters:
//  lplpabyDataStream   Ptr to ptr to current position in data stream
//
// Description:
//  Returns the SBYTE value at the current position in the data stream.
//  The current position is incremented by one.
//
// Returns:
//  SBYTE               Value of SBYTE at current position
//
//*************************************************************************

SBYTE WDGetDataStreamSBYTE
(
    LPLPBYTE    lplpabyDataStream
)
{
    SBYTE       sby;

    sby = **lplpabyDataStream;
    ++*lplpabyDataStream;
    return(sby);
}


//*************************************************************************
//
// SWORD WDGetDataStreamSWORD()
//
// Parameters:
//  lplpabyDataStream   Ptr to ptr to current position in data stream
//
// Description:
//  Returns the SWORD value at the current position in the data stream.
//  The current position is incremented by two.
//
//  The SWORD value returned is properly endianized.
//
// Returns:
//  SWORD               Value of SWORD at current position
//
//*************************************************************************

SWORD WDGetDataStreamSWORD
(
    LPLPBYTE    lplpabyDataStream
)
{
    SWORD       sw;

    sw = ((WORD)**lplpabyDataStream) |
         (((WORD)*(*lplpabyDataStream + 1)) << 8);
    *lplpabyDataStream += 2;
    return(sw);
}


//*************************************************************************
//
// SDWORD WDGetDataStreamSDWORD()
//
// Parameters:
//  lplpabyDataStream   Ptr to ptr to current position in data stream
//
// Description:
//  Returns the SDWORD value at the current position in the data stream.
//  The current position is incremented by four.
//
//  The SDWORD value returned is properly endianized.
//
// Returns:
//  SDWORD              Value of SDWORD at current position
//
//*************************************************************************

SDWORD WDGetDataStreamSDWORD
(
    LPLPBYTE    lplpabyDataStream
)
{
    SDWORD      sdw;
    LPBYTE      lpabyDataStream;

    lpabyDataStream = *lplpabyDataStream;
    sdw = ((DWORD)lpabyDataStream[0]) |
          (((DWORD)lpabyDataStream[1]) << 8) |
          (((DWORD)lpabyDataStream[2]) << 16) |
          (((DWORD)lpabyDataStream[3]) << 24);

    *lplpabyDataStream += 4;
    return(sdw);
}


