//************************************************************************
//
// $Header:   S:/NAVEX/VCS/danish.cpv   1.4   26 Dec 1996 15:21:42   AOONWAL  $
//
// Description:
//      Contains NAVEX code to scan an MBR or boot sector for Danish Boot
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/danish.cpv  $
// 
//    Rev 1.4   26 Dec 1996 15:21:42   AOONWAL
// No change.
// 
//    Rev 1.3   02 Dec 1996 14:01:02   AOONWAL
// No change.
// 
//    Rev 1.2   29 Oct 1996 12:59:04   AOONWAL
// No change.
// 
//    Rev 1.1   25 Jun 1996 17:00:34   JWILBER
// Fixed minor typos, removed INT 3.
//
//    Rev 1.0   25 Jun 1996 16:25:58   JWILBER
// Initial revision.
//

//  This is called from EXTScanPartDanish and EXTScanBootDanish, and used to
// check for the Danish Boot virus (of course, you say).  It's the same
// algorithm in both cases, so I put it here to save space, and keep it
// maintainable.  NAVEXSB.CPP and NAVEXSP.CPP #include this file to make use
// of it.  The prototype for this function is in NAVEXSHR.H.

// EXTScanDanish arguments:
//
// lpcallback           : Pointer to callback structure
// scanbuffer           : Buffer containing sector to scan
// lpwVID               : Pointer used to return VID of Danish Boot, if found
//
// Returns:
//
//  WORD                : EXTSTATUS_OK                  if no virus found
//                        EXTSTATUS_VIRUS_FOUND         if virus was found
//

#define DANESCAN    15                  // Number of bytes to scan
#define DANEWILD    0xff                // Wildcard value

WORD EXTScanDanish(LPCALLBACKREV1 lpcallback,
                    LPBYTE scanbuffer,
                    LPWORD lpwVID)
{                                       // 0xff is a wildcard!
    BYTE    scanstring[DANESCAN] = {    0x0e, 0xbb, 0x00, 0x7c, 0xb6,
                                        DANEWILD, 0xb9, DANEWILD, DANEWILD,
                                        0x07, 0xb8, 0x01, 0x02, 0xcd, 0x13  };
    WORD    begin, count, end, i, retval = EXTSTATUS_OK;

    if (0xeb == scanbuffer[0])
    {
        i = begin = scanbuffer[1] + 2;
        count = 0;
        end = begin + DANESCAN;

        do
        {
            if ((scanbuffer[i] == scanstring[i - begin]) ||
                                        (DANEWILD == scanstring[i - begin]))
                count++;

            i++;
        } while ((count == (i - begin)) && (count < DANESCAN));

        if (DANESCAN == count)
        {
            *lpwVID = VID_DANISHBOOT;
            retval = EXTSTATUS_VIRUS_FOUND;
        }
    }

    return(retval);
}
