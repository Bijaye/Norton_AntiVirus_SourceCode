//************************************************************************
//
// $Header:   S:/NAVEX/VCS/WDSIGUTL.H_v   1.0   17 Jan 1997 11:25:08   DCHI  $
//
// Description:
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/WDSIGUTL.H_v  $
// 
//    Rev 1.0   17 Jan 1997 11:25:08   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _WDSIGUTL_H_

#define _WDSIGUTL_H_

typedef signed char     SBYTE;
typedef LPBYTE FAR *    LPLPBYTE;

int WDGetControlStreamNibble
(
    LPBYTE      lpabyControlStream,
    LPINT       lpnIndex
);

BYTE WDGetDataStreamBYTE
(
    LPLPBYTE    lplpabyDataStream
);

WORD WDGetDataStreamWORD
(
    LPLPBYTE      lplpabyDataStream
);

DWORD WDGetDataStreamDWORD
(
    LPLPBYTE    lplpabyDataStream
);

SBYTE WDGetDataStreamSBYTE
(
    LPLPBYTE    lplpabyDataStream
);

SWORD WDGetDataStreamSWORD
(
    LPLPBYTE    lplpabyDataStream
);

SDWORD WDGetDataStreamSDWORD
(
    LPLPBYTE    lplpabyDataStream
);

#endif // #ifndef _WDSIGUTL_H_

