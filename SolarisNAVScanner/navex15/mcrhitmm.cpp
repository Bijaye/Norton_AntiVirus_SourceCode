//************************************************************************
//
// $Header:   S:/NAVEX/VCS/mcrhitmm.cpv   1.1   18 Jun 1997 11:55:18   DCHI  $
//
// Description:
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/mcrhitmm.cpv  $
// 
//    Rev 1.1   18 Jun 1997 11:55:18   DCHI
// SYM_WIN32 now uses GlobalAlloc() and GlobalFree() rather than
// NAVEXs memory callbacks which are not implemented in decomposer.
// 
//    Rev 1.0   27 May 1997 16:46:04   DCHI
// Initial revision.
// 
//************************************************************************

#include "storobj.h"
#include "wdencdoc.h"
#include "worddoc.h"
#include "wdencrd.h"
#include "wdscan.h"

#include "wd7sigs.h"
#include "wd8sigs.h"
#include "xl97sigs.h"

#include "mcrhitmm.h"

#if defined(ALLOC_MACR_SIG_HIT_MEM)

#define WD7_HIT_MEM_SIZE \
    (WD7_NAME_SIG_INTER_MACRO_HIT_ARRAY_SIZE + \
     WD7_NAME_SIG_INTRA_MACRO_HIT_ARRAY_SIZE + \
     WD7_NAMED_CRC_SIG_HIT_ARRAY_SIZE +        \
     WD7_MACRO_SIG_HIT_ARRAY_SIZE +            \
     WD7_CRC_HIT_ARRAY_SIZE)

#define WD8_HIT_MEM_SIZE \
    (WD8_NAME_SIG_INTER_MACRO_HIT_ARRAY_SIZE + \
     WD8_NAME_SIG_INTRA_MACRO_HIT_ARRAY_SIZE + \
     WD8_NAMED_CRC_SIG_HIT_ARRAY_SIZE +        \
     WD8_MACRO_SIG_HIT_ARRAY_SIZE +            \
     WD8_CRC_HIT_ARRAY_SIZE)

#define XL97_HIT_MEM_SIZE \
    (XL97_NAME_SIG_INTER_MACRO_HIT_ARRAY_SIZE + \
     XL97_NAME_SIG_INTRA_MACRO_HIT_ARRAY_SIZE + \
     XL97_NAMED_CRC_SIG_HIT_ARRAY_SIZE +        \
     XL97_MACRO_SIG_HIT_ARRAY_SIZE +            \
     XL97_CRC_HIT_ARRAY_SIZE)

#define MAX_HIT_MEM_SIZE    WD7_HIT_MEM_SIZE

#if WD8_HIT_MEM_SIZE > MAX_HIT_MEM_SIZE

#define MAX_HIT_MEM_SIZE    WD8_HIT_MEM_SIZE

#elif XL97_HIT_MEM_SIZE > MAX_HIT_MEM_SIZE

#define MAX_HIT_MEM_SIZE    XL97_HIT_MEM_SIZE

#endif

BOOL AllocMacroSigHitMem
(
    LPCALLBACKREV1      lpstCallBack,
    LPBYTE FAR *        lplpbyHitMem,
    int                 nType
)
{
    LPBYTE              lpbyHitMem;
    LPWORD              lpwArraySizes;

#ifdef SYM_WIN32
    lpbyHitMem = (LPBYTE)GlobalAlloc(GMEM_FIXED,
                                     5 * sizeof(WORD) + MAX_HIT_MEM_SIZE);
#else
    lpbyHitMem = (LPBYTE)lpstCallBack->
        PermMemoryAlloc(5 * sizeof(WORD) + MAX_HIT_MEM_SIZE);
#endif
    if (lpbyHitMem == NULL)
        return(FALSE);

    lpwArraySizes = (LPWORD)lpbyHitMem;
    switch (nType)
    {
        case HIT_MEM_WD7:
            lpwArraySizes[0] = WD7_NAME_SIG_INTER_MACRO_HIT_ARRAY_SIZE;
            lpwArraySizes[1] = WD7_NAME_SIG_INTRA_MACRO_HIT_ARRAY_SIZE;
            lpwArraySizes[2] = WD7_NAMED_CRC_SIG_HIT_ARRAY_SIZE;
            lpwArraySizes[3] = WD7_MACRO_SIG_HIT_ARRAY_SIZE;
            lpwArraySizes[4] = WD7_CRC_HIT_ARRAY_SIZE;
            break;

        case HIT_MEM_WD8:
            lpwArraySizes[0] = WD8_NAME_SIG_INTER_MACRO_HIT_ARRAY_SIZE;
            lpwArraySizes[1] = WD8_NAME_SIG_INTRA_MACRO_HIT_ARRAY_SIZE;
            lpwArraySizes[2] = WD8_NAMED_CRC_SIG_HIT_ARRAY_SIZE;
            lpwArraySizes[3] = WD8_MACRO_SIG_HIT_ARRAY_SIZE;
            lpwArraySizes[4] = WD8_CRC_HIT_ARRAY_SIZE;
            break;

        case HIT_MEM_XL97:
            lpwArraySizes[0] = XL97_NAME_SIG_INTER_MACRO_HIT_ARRAY_SIZE;
            lpwArraySizes[1] = XL97_NAME_SIG_INTRA_MACRO_HIT_ARRAY_SIZE;
            lpwArraySizes[2] = XL97_NAMED_CRC_SIG_HIT_ARRAY_SIZE;
            lpwArraySizes[3] = XL97_MACRO_SIG_HIT_ARRAY_SIZE;
            lpwArraySizes[4] = XL97_CRC_HIT_ARRAY_SIZE;
            break;

        default:
            return(FALSE);
    }

    *lplpbyHitMem = lpbyHitMem;
    return(TRUE);
}

void FreeMacroSigHitMem
(
    LPCALLBACKREV1      lpstCallBack,
    LPBYTE              lpbyHitMem
)
{
#ifdef SYM_WIN32
    GlobalFree(lpbyHitMem);
#else
    lpstCallBack->PermMemoryFree(lpbyHitMem);
#endif
}

void AssignMacroSigHitMem
(
    LPBYTE              lpbyHitMem,
    LPWD_SCAN           lpstScan
)
{
    LPWORD              lpwArraySizes;

    lpwArraySizes = (LPWORD)lpbyHitMem;
    lpbyHitMem += 5 * sizeof(WORD);

    lpstScan->lpabyNameSigInterMacroHit = lpbyHitMem;
    lpbyHitMem += lpwArraySizes[0];

    lpstScan->lpabyNameSigIntraMacroHit = lpbyHitMem;
    lpbyHitMem += lpwArraySizes[1];

    lpstScan->lpabyNamedCRCSigHit = lpbyHitMem;
    lpbyHitMem += lpwArraySizes[2];

    lpstScan->lpabyMacroSigHit = lpbyHitMem;
    lpbyHitMem += lpwArraySizes[3];

    lpstScan->lpabyCRCHit = lpbyHitMem;
}

#endif

