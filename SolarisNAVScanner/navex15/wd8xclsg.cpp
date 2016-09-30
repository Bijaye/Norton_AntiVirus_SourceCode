#ifndef MACRODAT

#include "platform.h"

DWORD gdwWD8ExclusionSigCount = 10;

#ifdef SYM_WIN16
#pragma data_seg("FAR_DATA_SIGS")
#endif

DWORD FAR gadwWD8ExclusionCRCs[] =
{
    0x0F19E7F3, // MSKCC
    0x1B9BE790, // DEVELOPMENTLIB
    0x483683B7, // GETAPPENDICES
    0x69DDE3A9, // ALLKEYS
    0x9118E395, // SUBSTITUTES (PROTO97A.DOT)
    0x964E2A1F, // GETANSWERS (PROTO97A.DOT)
    0xC2146973, // SUBSTITUTES
    0xE5A490C4, // CHECKBUTTONS
    0xE84BCD9F, // SETBUTTONS
    0xFAAB76F1, // GETANSWERS
};

DWORD FAR *gpadwWD8ExclusionCRCs = gadwWD8ExclusionCRCs;

DWORD FAR gadwWD8ExclusionCRCByteCounts[] =
{
    0x0000D868, // MSKCC
    0x00006222, // DEVELOPMENTLIB
    0x00001C5F, // GETAPPENDICES
    0x000027E2, // ALLKEYS
    0x0000AF69, // SUBSTITUTES (PROTO97A.DOT)
    0x0000A7C9, // GETANSWERS (PROTO97A.DOT)
    0x0000B02E, // SUBSTITUTES
    0x00000312, // CHECKBUTTONS
    0x000003BE, // SETBUTTONS
    0x0000A85D, // GETANSWERS
};

DWORD FAR *gpadwWD8ExclusionCRCByteCounts = gadwWD8ExclusionCRCByteCounts;

#ifdef SYM_WIN16
#pragma data_seg()
#endif

#endif // #ifndef MACRODAT
