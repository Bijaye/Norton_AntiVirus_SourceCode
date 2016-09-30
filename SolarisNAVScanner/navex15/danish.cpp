//************************************************************************
//
// $Header:   S:/NAVEX/VCS/danish.cpv   1.6   28 Jul 1997 12:57:22   AOONWAL  $
//
// Description:
//      Contains NAVEX code to scan an MBR or boot sector for Danish Boot
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/danish.cpv  $
// 
//    Rev 1.6   28 Jul 1997 12:57:22   AOONWAL
// Modified during AUG97 update
// 
//    Rev AUG97   23 Jul 1997 14:14:28   JWILBER
// Removed byte from end of Shin variant detections.  Had to check this
// in so it would compile.
//
//    Rev 1.5   18 Jul 1997 00:17:56   JWILBER
// Moved over from Newvir - Added EXTScanOneHalf() to detect OneHalf and
// Shin variants in boot and MBRs.
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

// EXTScanOneHalf   Boot detection for One Half boot variants.
//
// The sigs confuse certlib because they begin w/ 0xdf 0xdf, so we do
// a 'stupid search' w/ NAVEX.
//
// arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// scanbuffer           : contains image of appropriate boot sector
//                        to scan (ignore byDrive, byBootSecNum)
// lpwVID               : virus ID # of the virus if found
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if no virus found
//                        EXTSTATUS_VIRUS_FOUND         if virus was found

#define HLFBOOTLEN  14
#define HLFBOOTVID  0x0f1c

#define SHIN1LEN    11
#define SHIN1VID    0x0f1d

#define SHIN2LEN    11
#define SHIN2VID    0x0f1e

#define SHIN3LEN    11
#define SHIN3VID    0x0f1f

#define SHIN4LEN    11
#define SHIN4VID    0x0f20

#define SECTORLEN   0x100
#define BOOTCHK     5                   // Number of strings to search for

BYTE        onehalf[] = { 0xdb, 0x83, 0x2e, 0x13, 0x04, 0x04, 0xb1, 0x06,
                          0xcd, 0x12, 0xd3, 0xe0, 0xba, 0x80 };
BYTE        shin1[] = { 0x7c, 0x8b, 0xfe, 0x0e, 0x07, 0xb9, 0xc7, 0x00,
                        0x26, 0xad, 0x35 };
BYTE        shin2[] = { 0x7c, 0x89, 0xfe, 0x0e, 0x07, 0xb9, 0xc7, 0x00,
                        0x26, 0xad, 0x35 };
BYTE        shin3[] = { 0x7c, 0x8b, 0xfe, 0x0e, 0x07, 0xb9, 0xc7, 0x00,
                        0x2e, 0xad, 0x35 };
BYTE        shin4[] = { 0x7c, 0x89, 0xfe, 0x0e, 0x07, 0xb9, 0xc7, 0x00,
                        0x2e, 0xad, 0x35 };

WORD EXTScanOneHalf(LPCALLBACKREV1 lpcallback,
                    LPBYTE scanbuffer,
                    LPWORD lpwVID)
{
    BOOL        found;
    BYTE        temp;
    WORD        bootnum, i, j, maxpos;

    BYTE        len[BOOTCHK] = {    HLFBOOTLEN, SHIN1LEN, SHIN2LEN, SHIN3LEN,
                                    SHIN4LEN };
    WORD        vid[BOOTCHK] = {    HLFBOOTVID, SHIN1VID, SHIN2VID, SHIN3VID,
                                    SHIN4VID };
    BYTE        *array[BOOTCHK];    // All this array bogosity is necessary
                                    // so we can build for Windows - it's
    array[0] = onehalf;             // Microsoft compiler weirdness.  I wish
    array[1] = shin1;               // I could have used a structure instead.
    array[2] = shin2;
    array[3] = shin3;
    array[4] = shin4;

    found = FALSE;              // Boolean to make life easy
    bootnum = 0;                // Index into array of structures

    //  _asm    int 3

    do
    {
        i = 0;
        maxpos = SECTORLEN - len[bootnum];

        temp = array[bootnum][0];

        while ((i <= maxpos) && (FALSE == found))
        {
            if (scanbuffer[i] == temp)
            {
                j = 1;

                while ((scanbuffer[i + j] == array[bootnum][j]) &&
                                                        (j < len[bootnum]))
                    j++;

                if (j == len[bootnum])
                    found = TRUE;
            }

            i++;
        }

        if (FALSE == found)
            bootnum++;
 
    } while ((FALSE == found) && (bootnum < BOOTCHK));

    if (TRUE == found)
        *lpwVID = vid[bootnum];

    return(found ? EXTSTATUS_VIRUS_FOUND : EXTSTATUS_OK);
}   // End of EXTScanOneHalf
