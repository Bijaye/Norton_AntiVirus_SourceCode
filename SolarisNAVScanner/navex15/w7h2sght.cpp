// Copyright 1998 Symantec, SABU
//************************************************************************
//
// $Header:   S:/NAVEX/VCS/W7H2SGHT.CPv   1.2   09 Dec 1998 17:45:14   DCHI  $
//
// Description:
//  Contains Word 6.0/95 Heuristics Level II signature application code.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/W7H2SGHT.CPv  $
// 
//    Rev 1.2   09 Dec 1998 17:45:14   DCHI
// Added #include "gdefines.h"
// 
//    Rev 1.1   09 Nov 1998 13:58:40   DCHI
// Added #pragma data_seg()'s for global FAR data for SYM_WIN16.
// 
//    Rev 1.0   10 Jun 1998 13:05:58   DCHI
// Initial revision.
// 
//************************************************************************

#include "gdefines.h"

#ifdef MACROHEU

#include "platform.h"
#include "olessapi.h"
#include "w7h2sght.h"
#include "w7h2sgcm.h"

//********************************************************************
//
// Function:
//  LPW7H2_SCAN W7H2ScanCreate
//
// Parameters:
//  lpvRootCookie   Ptr to root cookie
//  lpstSigSet      Ptr to sig set
//
// Description:
//  Allocates a scan structure for the given sig set.
//
// Returns:
//  LPW7H2_SCAN     On success
//  NULL            On failure
//
//********************************************************************

LPW7H2_SCAN W7H2ScanCreate
(
    LPVOID              lpvRootCookie,
    LPW7H2_SIG_SET      lpstSigSet
)
{
    LPW7H2_SCAN         lpstScan;
    DWORD               dwNumHitBytes;
    LPBYTE              lpbyMem;
    long                l;

    dwNumHitBytes = 3 * ((lpstSigSet->nNumCmds + 7) >> 3) +
                    3 * ((lpstSigSet->nNumDlgs + 7) >> 3) +
                    3 * 16 +
                    lpstSigSet->nNumDlgs +
                    2 * ((lpstSigSet->nNumStrSigs + 7) >> 3) +
                    2 * ((lpstSigSet->nNumLineSigs + 7) >> 3) +
                    2 * lpstSigSet->nNumCats +
                    ((lpstSigSet->nNumFuncSigs + 7) >> 3);

    if (dwNumHitBytes > 32767)
        return(NULL);

    // Allocate structure

    if (SSMemoryAlloc(lpvRootCookie,
                      sizeof(W7H2_SCAN_T),
                      (LPLPVOID)&lpstScan) != SS_STATUS_OK)
        return(NULL);

    lpstScan->lpvRootCookie = lpvRootCookie;

    lpstScan->lpstSigSet = lpstSigSet;
    lpstScan->dwHitMemSize = dwNumHitBytes;

    if (SSMemoryAlloc(lpvRootCookie,
                      dwNumHitBytes,
                      (LPLPVOID)&lpbyMem) != SS_STATUS_OK)
    {
        SSMemoryFree(lpvRootCookie,lpstScan);
        return(NULL);
    }

    // Assign Cmd hit memory

    l = (lpstSigSet->nNumCmds + 7) >> 3;
    lpstScan->lpabyLCmdHit = lpbyMem;
    lpbyMem += l;
    lpstScan->lpabyFCmdHit = lpbyMem;
    lpbyMem += l;
    lpstScan->lpabyGCmdHit = lpbyMem;
    lpbyMem += l;

    // Assign Dlg hit memory

    l = (lpstSigSet->nNumDlgs + 7) >> 3;
    lpstScan->lpabyLDlgHit = lpbyMem;
    lpbyMem += l;
    lpstScan->lpabyDlgValue = lpbyMem;
    lpbyMem += lpstSigSet->nNumDlgs;
    lpstScan->lpabyFDlgHit = lpbyMem;
    lpbyMem += l;
    lpstScan->lpabyGDlgHit = lpbyMem;
    lpbyMem += l;

    // Assign Op hit memory

    lpstScan->lpabyLOpHit = lpbyMem;
    lpbyMem += 16;
    lpstScan->lpabyFOpHit = lpbyMem;
    lpbyMem += 16;
    lpstScan->lpabyGOpHit = lpbyMem;
    lpbyMem += 16;

    // Assign Str hit memory

    l = (lpstSigSet->nNumStrSigs + 7) >> 3;
    lpstScan->lpabyLStrSigHit = lpbyMem;
    lpbyMem += l;
    lpstScan->lpabyFStrSigHit = lpbyMem;
    lpbyMem += l;

    // Assign Line hit memory

    l = (lpstSigSet->nNumLineSigs + 7) >> 3;
    lpstScan->lpabyFLineSigHit = lpbyMem;
    lpbyMem += l;
    lpstScan->lpabyGLineSigHit = lpbyMem;
    lpbyMem += l;

    // Assign Category hit memory

    lpstScan->lpabyFCatScore = lpbyMem;
    lpbyMem += lpstSigSet->nNumCats;
    lpstScan->lpabyGCatScore = lpbyMem;
    lpbyMem += lpstSigSet->nNumCats;

    // Assign Func hit memory

    l = (lpstSigSet->nNumFuncSigs + 7) >> 3;
    lpstScan->lpabyGFuncSigHit = lpbyMem;
    lpbyMem += l;

    return(lpstScan);
}


//********************************************************************
//
// Function:
//  BOOL W7H2ScanDestroy
//
// Parameters:
//  lpstScan        Ptr to W7H2_SCAN_T structure
//
// Description:
//  Initializes the flags and initializes the hit memory to no hits.
//
// Returns:
//  TRUE            Always
//
//********************************************************************

BOOL W7H2ScanDestroy
(
    LPW7H2_SCAN     lpstScan
)
{
    SSMemoryFree(lpstScan->lpvRootCookie,lpstScan->lpabyLCmdHit);
    SSMemoryFree(lpstScan->lpvRootCookie,lpstScan);

    return(TRUE);
}


//********************************************************************
//
// Function:
//  void W7H2ScanInit()
//
// Parameters:
//  lpstScan        Ptr to W7H2_SCAN_T structure
//
// Description:
//  Initializes the flags and initializes the hit memory to no hits.
//
// Returns:
//  Nothing.
//
//********************************************************************

void W7H2ScanInit
(
    LPW7H2_SCAN     lpstScan
)
{
    DWORD           dw;
    LPBYTE          lpabyHitMem;

    // Zero out the hit arrays

    lpstScan->dwLExtraFlags = 0;
    lpstScan->dwFExtraFlags = 0;
    lpstScan->dwGExtraFlags = 0;
    lpabyHitMem = lpstScan->lpabyLCmdHit;
    for (dw=0;dw<lpstScan->dwHitMemSize;dw++)
        *lpabyHitMem++ = 0;
}


//********************************************************************
//
// Function:
//  void W7H2ScanCmdHit()
//
// Parameters:
//  lpstScan        Ptr to W7H2_SCAN_T structure
//  wCmd            Command value
//
// Description:
//  Sets the hit bit for the given command.
//
// Returns:
//  Nothing.
//
//********************************************************************

void W7H2ScanCmdHit
(
    LPW7H2_SCAN     lpstScan,
    WORD            wCmd
)
{
    int             nLow, nMid, nHigh;
    LPWORD          lpawCmd;

    nLow = 0;
    nHigh = lpstScan->lpstSigSet->nNumCmds;
    lpawCmd = lpstScan->lpstSigSet->lpawCmd;
    while (nLow <= nHigh)
    {
        nMid = (nLow + nHigh) >> 1;

        if (wCmd < lpawCmd[nMid])
        {
            // In lower half

            nHigh = nMid - 1;
        }
        else
        if (wCmd > lpawCmd[nMid])
        {
            // In upper half

            nLow = nMid + 1;
        }
        else
        {
            // Found a match

            lpstScan->lpabyLCmdHit[nMid >> 3] |= 1 << (nMid & 7);

            return;
        }
    }
}


//********************************************************************
//
// Function:
//  void W7H2ScanDlgHit()
//
// Parameters:
//  lpstScan        Ptr to W7H2_SCAN_T structure
//  wDlg            Dialog value
//  byOp            Operation
//
// Description:
//  Sets the hit bit for the given dialog value.
//
// Returns:
//  Nothing.
//
//********************************************************************

void W7H2ScanDlgHit
(
    LPW7H2_SCAN     lpstScan,
    WORD            wDlg,
    int             nValue
)
{
    int             nLow, nMid, nHigh;
    LPWORD          lpawDlg;

    nLow = 0;
    nHigh = lpstScan->lpstSigSet->nNumDlgs;
    lpawDlg = lpstScan->lpstSigSet->lpawDlg;
    while (nLow <= nHigh)
    {
        nMid = (nLow + nHigh) >> 1;

        if (wDlg < lpawDlg[nMid])
        {
            // In lower half

            nHigh = nMid - 1;
        }
        else
        if (wDlg > lpawDlg[nMid])
        {
            // In upper half

            nLow = nMid + 1;
        }
        else
        {
            // Found a match

            lpstScan->lpabyLDlgHit[nMid >> 3] |= 1 << (nMid & 7);
            lpstScan->lpabyDlgValue[nMid] = (BYTE)nValue;
            return;
        }
    }
}


//********************************************************************
//
// Function:
//  void W7H2ScanOpHit()
//
// Parameters:
//  lpstScan        Ptr to W7H2_SCAN_T structure.
//  byOp            Operation
//
// Description:
//  Sets the hit bit for the given operation.
//
// Returns:
//  Nothing.
//
//********************************************************************

void W7H2ScanOpHit
(
    LPW7H2_SCAN     lpstScan,
    BYTE            byOp
)
{
    if (byOp > 0x7F)
        return;

    lpstScan->lpabyLOpHit[byOp >> 3] |= 1 << (byOp & 7);
}


//********************************************************************
//
// Function:
//  BOOL W7H2ScanStrIsEncrypted()
//
// Parameters:
//  lpabypsStr      Ptr to the Pascal string to check
//
// Description:
//  Determines whether a string is encrypted.
//
// Returns:
//  TRUE            If the string appears encrypted
//  FALSE           If the string does not appear encrypted
//
//********************************************************************

#define CHFLAG_VOWEL        0x01
#define CHFLAG_CONSONANT    0x02

#ifdef SYM_WIN16
#pragma data_seg("FAR_DATA_0")
#endif

BYTE FAR gabyW7H2ScanCharFlags[256] =
{
    0x00,   // 0x00:
    0x00,   // 0x01:
    0x00,   // 0x02:
    0x00,   // 0x03:
    0x00,   // 0x04:
    0x00,   // 0x05:
    0x00,   // 0x06:
    0x00,   // 0x07:
    0x00,   // 0x08:
    0x00,   // 0x09:
    0x00,   // 0x0A:
    0x00,   // 0x0B:
    0x00,   // 0x0C:
    0x00,   // 0x0D:
    0x00,   // 0x0E:
    0x00,   // 0x0F:
    0x00,   // 0x10:
    0x00,   // 0x11:
    0x00,   // 0x12:
    0x00,   // 0x13:
    0x00,   // 0x14:
    0x00,   // 0x15:
    0x00,   // 0x16:
    0x00,   // 0x17:
    0x00,   // 0x18:
    0x00,   // 0x19:
    0x00,   // 0x1A:
    0x00,   // 0x1B:
    0x00,   // 0x1C:
    0x00,   // 0x1D:
    0x00,   // 0x1E:
    0x00,   // 0x1F:
    0x00,   // 0x20: ' ' Space
    0x00,   // 0x21: '!' Exclamation point
    0x00,   // 0x22: '"' Quotation mark
    0x00,   // 0x23: '#' Number sign
    0x00,   // 0x24: '$' Dollar sign
    0x00,   // 0x25: '%' Percent sign
    0x00,   // 0x26: '&' Ampersand
    0x00,   // 0x27: ''' Apostrophe
    0x00,   // 0x28: '(' Opening parenthesis
    0x00,   // 0x29: ')' Closing parenthesis
    0x00,   // 0x2A: '*' Asterisk
    0x00,   // 0x2B: '+' Plus sign
    0x00,   // 0x2C: ',' Comma
    0x00,   // 0x2D: '-' Hyphen or minus sign
    0x00,   // 0x2E: '.' Period
    0x00,   // 0x2F: '/' Slash
    0x00,   // 0x30: '0'
    0x00,   // 0x31: '1'
    0x00,   // 0x32: '2'
    0x00,   // 0x33: '3'
    0x00,   // 0x34: '4'
    0x00,   // 0x35: '5'
    0x00,   // 0x36: '6'
    0x00,   // 0x37: '7'
    0x00,   // 0x38: '8'
    0x00,   // 0x39: '9'
    0x00,   // 0x3A: ':' Colon
    0x00,   // 0x3B: ';' Semicolon
    0x00,   // 0x3C: '<' Less than sign
    0x00,   // 0x3D: '=' Equal sign
    0x00,   // 0x3E: '>' Greater than sign
    0x00,   // 0x3F: '?' Question mark
    0x00,   // 0x40: '@' At sign
    CHFLAG_VOWEL,   // 0x41: 'A'
    CHFLAG_CONSONANT,   // 0x42: 'B'
    CHFLAG_CONSONANT,   // 0x43: 'C'
    CHFLAG_CONSONANT,   // 0x44: 'D'
    CHFLAG_VOWEL,   // 0x45: 'E'
    CHFLAG_CONSONANT,   // 0x46: 'F'
    CHFLAG_CONSONANT,   // 0x47: 'G'
    CHFLAG_CONSONANT,   // 0x48: 'H'
    CHFLAG_VOWEL,   // 0x49: 'I'
    CHFLAG_CONSONANT,   // 0x4A: 'J'
    CHFLAG_CONSONANT,   // 0x4B: 'K'
    CHFLAG_CONSONANT,   // 0x4C: 'L'
    CHFLAG_CONSONANT,   // 0x4D: 'M'
    CHFLAG_CONSONANT,   // 0x4E: 'N'
    CHFLAG_VOWEL,   // 0x4F: 'O'
    CHFLAG_CONSONANT,   // 0x50: 'P'
    CHFLAG_CONSONANT,   // 0x51: 'Q'
    CHFLAG_CONSONANT,   // 0x52: 'R'
    CHFLAG_CONSONANT,   // 0x53: 'S'
    CHFLAG_CONSONANT,   // 0x54: 'T'
    CHFLAG_VOWEL,   // 0x55: 'U'
    CHFLAG_CONSONANT,   // 0x56: 'V'
    CHFLAG_CONSONANT,   // 0x57: 'W'
    CHFLAG_CONSONANT,   // 0x58: 'X'
    CHFLAG_VOWEL,   // 0x59: 'Y'
    CHFLAG_CONSONANT,   // 0x5A: 'Z'
    0x00,   // 0x5B: '['
    0x00,   // 0x5C: '\' Backward slash
    0x00,   // 0x5D: ']' Closing bracket
    CHFLAG_CONSONANT,   // 0x5E: '^' Caret
    0x00,   // 0x5F: '_' Underscore
    0x00,   // 0x60: '`' Grave
    CHFLAG_VOWEL,   // 0x61: 'a'
    CHFLAG_CONSONANT,   // 0x62: 'b'
    CHFLAG_CONSONANT,   // 0x63: 'c'
    CHFLAG_CONSONANT,   // 0x64: 'd'
    CHFLAG_VOWEL,   // 0x65: 'e'
    CHFLAG_CONSONANT,   // 0x66: 'f'
    CHFLAG_CONSONANT,   // 0x67: 'g'
    CHFLAG_CONSONANT,   // 0x68: 'h'
    CHFLAG_VOWEL,   // 0x69: 'i'
    CHFLAG_CONSONANT,   // 0x6A: 'j'
    CHFLAG_CONSONANT,   // 0x6B: 'k'
    CHFLAG_CONSONANT,   // 0x6C: 'l'
    CHFLAG_CONSONANT,   // 0x6D: 'm'
    CHFLAG_CONSONANT,   // 0x6E: 'n'
    CHFLAG_VOWEL,   // 0x6F: 'o'
    CHFLAG_CONSONANT,   // 0x70: 'p'
    CHFLAG_CONSONANT,   // 0x71: 'q'
    CHFLAG_CONSONANT,   // 0x72: 'r'
    CHFLAG_CONSONANT,   // 0x73: 's'
    CHFLAG_CONSONANT,   // 0x74: 't'
    CHFLAG_VOWEL,   // 0x75: 'u'
    CHFLAG_CONSONANT,   // 0x76: 'v'
    CHFLAG_CONSONANT,   // 0x77: 'w'
    CHFLAG_CONSONANT,   // 0x78: 'x'
    CHFLAG_VOWEL,   // 0x79: 'y'
    CHFLAG_CONSONANT,   // 0x7A: 'z'
    0x00,   // 0x7B: '{' Opening brace
    0x00,   // 0x7C: '|' Vertical line
    0x00,   // 0x7D: '}' Closing brace
    CHFLAG_CONSONANT,   // 0x7E: '~' Tilde
    0x00,   // 0x7F:
    0x00,   // 0x80:
    0x00,   // 0x81:
    0x00,   // 0x82:
    0x00,   // 0x83:
    0x00,   // 0x84:
    0x00,   // 0x85:
    0x00,   // 0x86:
    0x00,   // 0x87:
    0x00,   // 0x88:
    0x00,   // 0x89:
    0x00,   // 0x8A:
    0x00,   // 0x8B:
    0x00,   // 0x8C:
    0x00,   // 0x8D:
    0x00,   // 0x8E:
    0x00,   // 0x8F:
    0x00,   // 0x90:
    0x00,   // 0x91:
    0x00,   // 0x92:
    0x00,   // 0x93:
    0x00,   // 0x94:
    0x00,   // 0x95:
    0x00,   // 0x96:
    0x00,   // 0x97:
    0x00,   // 0x98:
    0x00,   // 0x99:
    0x00,   // 0x9A:
    0x00,   // 0x9B:
    0x00,   // 0x9C:
    0x00,   // 0x9D:
    0x00,   // 0x9E:
    0x00,   // 0x9F:
    0x00,   // 0xA0:
    0x00,   // 0xA1:
    0x00,   // 0xA2:
    0x00,   // 0xA3:
    0x00,   // 0xA4:
    0x00,   // 0xA5:
    0x00,   // 0xA6:
    0x00,   // 0xA7:
    0x00,   // 0xA8:
    0x00,   // 0xA9:
    0x00,   // 0xAA:
    0x00,   // 0xAB:
    0x00,   // 0xAC:
    0x00,   // 0xAD:
    0x00,   // 0xAE:
    0x00,   // 0xAF:
    0x00,   // 0xB0:
    0x00,   // 0xB1:
    0x00,   // 0xB2:
    0x00,   // 0xB3:
    0x00,   // 0xB4:
    0x00,   // 0xB5:
    0x00,   // 0xB6:
    0x00,   // 0xB7:
    0x00,   // 0xB8:
    0x00,   // 0xB9:
    0x00,   // 0xBA:
    0x00,   // 0xBB:
    0x00,   // 0xBC:
    0x00,   // 0xBD:
    0x00,   // 0xBE:
    0x00,   // 0xBF:
    0x00,   // 0xC0:
    0x00,   // 0xC1:
    0x00,   // 0xC2:
    0x00,   // 0xC3:
    0x00,   // 0xC4:
    0x00,   // 0xC5:
    0x00,   // 0xC6:
    0x00,   // 0xC7:
    0x00,   // 0xC8:
    0x00,   // 0xC9:
    0x00,   // 0xCA:
    0x00,   // 0xCB:
    0x00,   // 0xCC:
    0x00,   // 0xCD:
    0x00,   // 0xCE:
    0x00,   // 0xCF:
    0x00,   // 0xD0:
    0x00,   // 0xD1:
    0x00,   // 0xD2:
    0x00,   // 0xD3:
    0x00,   // 0xD4:
    0x00,   // 0xD5:
    0x00,   // 0xD6:
    0x00,   // 0xD7:
    0x00,   // 0xD8:
    0x00,   // 0xD9:
    0x00,   // 0xDA:
    0x00,   // 0xDB:
    0x00,   // 0xDC:
    0x00,   // 0xDD:
    0x00,   // 0xDE:
    0x00,   // 0xDF:
    0x00,   // 0xE0:
    0x00,   // 0xE1:
    0x00,   // 0xE2:
    0x00,   // 0xE3:
    0x00,   // 0xE4:
    0x00,   // 0xE5:
    0x00,   // 0xE6:
    0x00,   // 0xE7:
    0x00,   // 0xE8:
    0x00,   // 0xE9:
    0x00,   // 0xEA:
    0x00,   // 0xEB:
    0x00,   // 0xEC:
    0x00,   // 0xED:
    0x00,   // 0xEE:
    0x00,   // 0xEF:
    0x00,   // 0xF0:
    0x00,   // 0xF1:
    0x00,   // 0xF2:
    0x00,   // 0xF3:
    0x00,   // 0xF4:
    0x00,   // 0xF5:
    0x00,   // 0xF6:
    0x00,   // 0xF7:
    0x00,   // 0xF8:
    0x00,   // 0xF9:
    0x00,   // 0xFA:
    0x00,   // 0xFB:
    0x00,   // 0xFC:
    0x00,   // 0xFD:
    0x00,   // 0xFE:
    0x00    // 0xFF:
};

#ifdef SYM_WIN16
#pragma data_seg()
#endif

BOOL W7H2ScanStrIsEncrypted
(
    LPBYTE          lpabypsStr
)
{
    int             nCount;
    BOOL            bLastChWasVowel;
    int             nCharCount;
    int             i;
    BYTE            byChar;

    nCharCount = lpabypsStr[0];
    if (nCharCount == 0)
        return(FALSE);

    // Check the first character

    if (gabyW7H2ScanCharFlags[lpabypsStr[1]] & CHFLAG_VOWEL)
        bLastChWasVowel = TRUE;
    else
        bLastChWasVowel = FALSE;

    nCount = 1;

    for (i=2;i<=nCharCount;i++)
    {
        byChar = lpabypsStr[i];
        if (gabyW7H2ScanCharFlags[byChar] & CHFLAG_CONSONANT)
        {
            if (bLastChWasVowel != FALSE)
            {
                bLastChWasVowel = FALSE;
                nCount = 1;
            }
            else
            {
                if (nCount++ >= 5)
                {
                    // Six or more consecutive consonants

                    return(TRUE);
                }
            }
        }
        else
        {
            nCount = 0;
        }
    }

    // Doesn't appear encrypted

    return(FALSE);
}


//********************************************************************
//
// Function:
//  void W7H2ScanStrHit()
//
// Parameters:
//  lpstScan        Ptr to W7H2_SCAN_T structure.
//  lpabyStr        Ptr to string to scan
//
// Description:
//  Applies the string signatures on the given string and determines
//  whether the string looks encrypted.
//
// Returns:
//  Nothing.
//
//********************************************************************

void W7H2ScanStrHit
(
    LPW7H2_SCAN     lpstScan,
    LPBYTE          lpabyStr
)
{
    LPBYTE          lpabyControl;
    LPBYTE          lpabyData;
    int             nControlLen;
    int             nDataLen;
    int             i;
    int             nStrLen;
    LPLPBYTE        lpalpabySigs;
    LPWORD          lpawStrSigIdx;
    WORD            wIdx;
    WORD            wNumSigs;
    BYTE            byFirst;

    if (W7H2ScanStrIsEncrypted(lpabyStr) == TRUE)
        lpstScan->dwLExtraFlags |= W7H2_SCAN_FLAG_ENCRYPTED_STR;

    wNumSigs = lpstScan->lpstSigSet->nNumStrSigs;
    if (wNumSigs == 0)
        return;

    lpalpabySigs = lpstScan->lpstSigSet->lpalpabyStrSigs;
    lpawStrSigIdx = lpstScan->lpstSigSet->lpawStrSigIdx;

    nStrLen = *lpabyStr++;
    while (nStrLen-- != 0)
    {
        byFirst = SSToLower(*lpabyStr++);
        wIdx = lpawStrSigIdx[byFirst];
        if (wIdx != 0xFFFF)
        {
            // Encrypt the first byte for easy check

            ++byFirst;

            // Get sig info

            lpalpabySigs = lpstScan->lpstSigSet->lpalpabyStrSigs + wIdx;
            while (wIdx < wNumSigs)
            {
                lpabyControl = *lpalpabySigs++;
                nControlLen = *lpabyControl++;
                nDataLen = *lpabyControl++;
                lpabyData = lpabyControl + nControlLen + 1;

                // No need to decrypt the sig byte, since we encrypted
                //  the first char

                if (*lpabyData++ != byFirst)
                    break;

                // Apply sig

                nDataLen -= 2;
                for (i=0;i<nDataLen;i++)
                    if ((BYTE)(*lpabyData++ - 1) != SSToLower(lpabyStr[i]))
                        break;

                if (i >= nDataLen)
                {
                    // It was a hit

                    lpstScan->lpabyLStrSigHit[wIdx >> 3] |= 1 << (wIdx & 7);
                }

                ++wIdx;
            }
        }
    }
}


//********************************************************************
//
// Function:
//  int W7H2ScanApplySigs()
//
// Parameters:
//  nNumSigs        Number of signatures
//  lpalpabySigs    Ptr to array of sig ptrs
//  dwExtraFlags    Flags
//  lpabyCmdHit     Cmd hit array
//  lpabyDlgHit     Dlg hit array
//  lpabyOpHit      Op hit array
//  lpabyDlgValue   Dlg value
//  lpabyStrSigHit  String sig hit array
//  lpabyLineSigHit Line sig hit array
//  lpabyFuncSigHit Func sig hit array
//  lpabyCatScore   Category score array
//  lpabySigHit     Resulting sig hit array
//
// Description:
//  Applies the given signature set.
//
// Returns:
//  int nNumSigs    when applying line and func sigs or
//                      when applying set sigs and no hit
//     <nNumSigs    when applying set sigs and hit
//
//********************************************************************

int W7H2ScanApplySigs
(
    int             nNumSigs,
    LPBYTE FAR *    lpalpabySigs,
    DWORD           dwExtraFlags,
    LPBYTE          lpabyCmdHit,
    LPBYTE          lpabyDlgHit,
    LPBYTE          lpabyOpHit,
    LPBYTE          lpabyDlgValue,
    LPBYTE          lpabyStrSigHit,
    LPBYTE          lpabyLineSigHit,
    LPBYTE          lpabyFuncSigHit,
    LPBYTE          lpabyCatScore,
    LPBYTE          lpabySigHit
)
{
    LPBYTE          lpabyControl;
    LPBYTE          lpabyData;
    int             nControlLen;
    int             nDataLen;
    int             i;
    int             nNibble;
    BYTE            abyStack[256];
    int             nStackTopIndex;
    int             nID;
    int             nScore;
    int             nLastDlgID;

    // Apply the line sigs

    nLastDlgID = 0;
    for (i=0;i<nNumSigs;i++)
    {
        // Get sig info

        lpabyControl = *lpalpabySigs++;
        nControlLen = *lpabyControl++;
        nDataLen = *lpabyControl++;
        lpabyData = lpabyControl + nControlLen;

        // Apply sig

        nControlLen *= 2;
        nStackTopIndex = -1;
        while (nControlLen > 0)
        {
            if ((nControlLen-- & 1) == 0)
                nNibble = lpabyControl[0] & 0xF;
            else
                nNibble = (*lpabyControl++ >> 4) & 0xF;

            switch (nNibble)
            {
                case W7H2_HNIB0_CMD_SIG_BYTE:
                    nID = *lpabyData++;
                    if ((lpabyCmdHit[nID >> 3] & (1 << (nID & 7))) == 0)
                        abyStack[++nStackTopIndex] = 0;
                    else
                        abyStack[++nStackTopIndex] = 1;
                    break;

                case W7H2_HNIB0_CMD_SIG_WORD:
                    nID = *lpabyData++;
                    nID |= (int)((WORD)*lpabyData++ << 8);
                    if ((lpabyCmdHit[nID >> 3] & (1 << (nID & 7))) == 0)
                        abyStack[++nStackTopIndex] = 0;
                    else
                        abyStack[++nStackTopIndex] = 1;
                    break;

                case W7H2_HNIB0_DLG_SIG_BYTE:
                    nID = *lpabyData++;
                    nLastDlgID = nID;
                    if ((lpabyDlgHit[nID >> 3] & (1 << (nID & 7))) == 0)
                        abyStack[++nStackTopIndex] = 0;
                    else
                        abyStack[++nStackTopIndex] = 1;
                    break;

                case W7H2_HNIB0_DLG_SIG_WORD:
                    nID = *lpabyData++;
                    nID |= (int)((WORD)*lpabyData++ << 8);
                    nLastDlgID = nID;
                    if ((lpabyDlgHit[nID >> 3] & (1 << (nID & 7))) == 0)
                        abyStack[++nStackTopIndex] = 0;
                    else
                        abyStack[++nStackTopIndex] = 1;
                    break;

                case W7H2_HNIB0_LINE_SIG_BYTE:
                    nID = *lpabyData++;
                    if ((lpabyLineSigHit[nID >> 3] & (1 << (nID & 7))) == 0)
                        abyStack[++nStackTopIndex] = 0;
                    else
                        abyStack[++nStackTopIndex] = 1;
                    break;

                case W7H2_HNIB0_LINE_SIG_WORD:
                    nID = *lpabyData++;
                    nID |= (int)((WORD)*lpabyData++ << 8);
                    if ((lpabyLineSigHit[nID >> 3] & (1 << (nID & 7))) == 0)
                        abyStack[++nStackTopIndex] = 0;
                    else
                        abyStack[++nStackTopIndex] = 1;
                    break;

                case W7H2_HNIB0_FUNC_SIG_BYTE:
                    nID = *lpabyData++;
                    if ((lpabyFuncSigHit[nID >> 3] & (1 << (nID & 7))) == 0)
                        abyStack[++nStackTopIndex] = 0;
                    else
                        abyStack[++nStackTopIndex] = 1;
                    break;

                case W7H2_HNIB0_FUNC_SIG_WORD:
                    nID = *lpabyData++;
                    nID |= (int)((WORD)*lpabyData++ << 8);
                    if ((lpabyFuncSigHit[nID >> 3] & (1 << (nID & 7))) == 0)
                        abyStack[++nStackTopIndex] = 0;
                    else
                        abyStack[++nStackTopIndex] = 1;
                    break;

                case W7H2_HNIB0_STR_SIG_BYTE:
                    nID = *lpabyData++;
                    if ((lpabyStrSigHit[nID >> 3] & (1 << (nID & 7))) == 0)
                        abyStack[++nStackTopIndex] = 0;
                    else
                        abyStack[++nStackTopIndex] = 1;
                    break;

                case W7H2_HNIB0_STR_SIG_WORD:
                    nID = *lpabyData++;
                    nID |= (int)((WORD)*lpabyData++ << 8);
                    if ((lpabyStrSigHit[nID >> 3] & (1 << (nID & 7))) == 0)
                        abyStack[++nStackTopIndex] = 0;
                    else
                        abyStack[++nStackTopIndex] = 1;
                    break;

                case W7H2_HNIB0_OP:
                    nID = *lpabyData++;
                    if ((lpabyOpHit[nID >> 3] & (1 << (nID & 7))) == 0)
                        abyStack[++nStackTopIndex] = 0;
                    else
                        abyStack[++nStackTopIndex] = 1;
                    break;

                case W7H2_HNIB0_AND:
                    --nStackTopIndex;
                    abyStack[nStackTopIndex] &= abyStack[nStackTopIndex + 1];
                    break;

                case W7H2_HNIB0_OR:
                    --nStackTopIndex;
                    abyStack[nStackTopIndex] |= abyStack[nStackTopIndex + 1];
                    break;

                case W7H2_HNIB0_NOT:
                    abyStack[nStackTopIndex] ^= 1;
                    break;

                case W7H2_HNIB0_MISC:
                    if ((nControlLen-- & 1) == 0)
                        nNibble = lpabyControl[0] & 0xF;
                    else
                        nNibble = (*lpabyControl++ >> 4) & 0xF;

                    switch (nNibble)
                    {
                        case W7H2_HNIB1_DLG_NOT_0:
                            if (lpabyDlgValue[nLastDlgID] != 0)
                                abyStack[++nStackTopIndex] = 1;
                            else
                                abyStack[++nStackTopIndex] = 0;
                            break;

                        case W7H2_HNIB1_CAT_GT:
                            nScore = *lpabyData++;
                            nID = *lpabyData++;
                            if (lpabyCatScore[nID] > nScore)
                                abyStack[++nStackTopIndex] = 1;
                            else
                                abyStack[++nStackTopIndex] = 0;
                            break;

                        case W7H2_HNIB1_CAT_LT:
                            nScore = *lpabyData++;
                            nID = *lpabyData++;
                            if (lpabyCatScore[nID] < nScore)
                                abyStack[++nStackTopIndex] = 1;
                            else
                                abyStack[++nStackTopIndex] = 0;
                            break;

                        case W7H2_HNIB1_CAT_EQ:
                            nScore = *lpabyData++;
                            nID = *lpabyData++;
                            if (lpabyCatScore[nID] == nScore)
                                abyStack[++nStackTopIndex] = 1;
                            else
                                abyStack[++nStackTopIndex] = 0;
                            break;

                        case W7H2_HNIB1_ENCRYPTED_STR:
                            if ((dwExtraFlags & W7H2_SCAN_FLAG_ENCRYPTED_STR) != 0)
                                abyStack[++nStackTopIndex] = 1;
                            else
                                abyStack[++nStackTopIndex] = 0;
                            break;

                        default:
                            break;
                    }
                    break;

                default:
                    break;
            }
        }

        // Was the signature a hit?

        if (nStackTopIndex == 0 && abyStack[0] != 0)
        {
            // It was a hit

            if (lpabySigHit != NULL)
            {
                // This is the case while applying line and func sigs

                lpabySigHit[i >> 3] |= 1 << (i & 7);
            }
            else
            {
                // This is the case while applying set sigs

                return(i);
            }
        }
    }

    return(i);
}


//********************************************************************
//
// Function:
//  void W7H2ScanDoCatScores()
//
// Parameters:
//  nNumSigs        Number of signatures
//  lpabySigHit     Signature hit array
//  lpabyCatScores  Ptr to cat scores for each signature
//  lpabyScores     Ptr to result score array
//
// Description:
//  Applies the category scores.
//
// Returns:
//  Nothing.
//
//********************************************************************

void W7H2ScanDoCatScores
(
    int             nNumSigs,
    LPBYTE          lpabySigHit,
    LPBYTE          lpabyCatScores,
    LPBYTE          lpabyScores
)
{
    int             i;
    BYTE            byMask;
    BYTE            byCat;
    BYTE            byScore;

    byMask = 1;
    for (i=0;i<nNumSigs;i++)
    {
        if ((*lpabySigHit & byMask) != 0)
        {
            // Add the Cat score

            byCat = lpabyCatScores[i * 2];
            byScore = lpabyCatScores[i * 2 + 1];
            if (lpabyScores[byCat] + byScore <= 255)
                lpabyScores[byCat] += byScore;
            else
                lpabyScores[byCat] = 255;
        }

        if (byMask == 0x80)
        {
            ++lpabySigHit;
            byMask = 0x01;
        }
        else
            byMask <<= 1;
    }
}


//********************************************************************
//
// Function:
//  void W7H2ScanDoLineSigs()
//
// Parameters:
//  lpstScan        Ptr to W7H2_SCAN_T structure.
//
// Description:
//  Applies the line sigs on the just completed scan of a line.
//
// Returns:
//  Nothing.
//
//********************************************************************

void W7H2ScanDoLineSigs
(
    LPW7H2_SCAN     lpstScan
)
{
    int             i;
    int             nLen;
    LPBYTE          lpbySrc;
    LPBYTE          lpbyDst;

    W7H2ScanApplySigs(lpstScan->lpstSigSet->nNumLineSigs,
                      lpstScan->lpstSigSet->lpalpabyLineSigs,
                      lpstScan->dwLExtraFlags,
                      lpstScan->lpabyLCmdHit,
                      lpstScan->lpabyLDlgHit,
                      lpstScan->lpabyLOpHit,
                      lpstScan->lpabyDlgValue,
                      lpstScan->lpabyLStrSigHit,
                      NULL,
                      NULL,
                      NULL,
                      lpstScan->lpabyFLineSigHit);

    // Merge Line Cmd hits with Func Cmd hits and clear Line Cmd hits

    nLen = (lpstScan->lpstSigSet->nNumCmds + 7) >> 3;
    lpbySrc = lpstScan->lpabyLCmdHit;
    lpbyDst = lpstScan->lpabyFCmdHit;
    for (i=0;i<nLen;i++)
    {
        *lpbyDst++ |= *lpbySrc;
        *lpbySrc++ = 0;
    }

    // Merge Line Dlg hits with Func Dlg hits and clear Line Dlg hits

    nLen = (lpstScan->lpstSigSet->nNumDlgs + 7) >> 3;
    lpbySrc = lpstScan->lpabyLDlgHit;
    lpbyDst = lpstScan->lpabyFDlgHit;
    for (i=0;i<nLen;i++)
    {
        *lpbyDst++ |= *lpbySrc;
        *lpbySrc++ = 0;
    }

    // Merge Line Op hits with Func Op hits and clear Line Op hits

    lpbySrc = lpstScan->lpabyLOpHit;
    lpbyDst = lpstScan->lpabyFOpHit;
    for (i=0;i<16;i++)
    {
        *lpbyDst++ |= *lpbySrc;
        *lpbySrc++ = 0;
    }

    // Merge Line Str hits with Func Str hits and clear Line Str hits

    nLen = (lpstScan->lpstSigSet->nNumStrSigs + 7) >> 3;
    lpbySrc = lpstScan->lpabyLStrSigHit;
    lpbyDst = lpstScan->lpabyFStrSigHit;
    for (i=0;i<nLen;i++)
    {
        *lpbyDst++ |= *lpbySrc;
        *lpbySrc++ = 0;
    }

    // Zero out extra flags

    lpstScan->dwLExtraFlags = 0;
}


//********************************************************************
//
// Function:
//  void W7H2ScanDoFuncSigs()
//
// Parameters:
//  lpstScan        Ptr to W7H2_SCAN_T structure.
//
// Description:
//  Applies the function sigs on the just completed scan of a
//  function and all its subfunctions.
//
// Returns:
//  Nothing.
//
//********************************************************************

void W7H2ScanDoFuncSigs
(
    LPW7H2_SCAN     lpstScan
)
{
    int             i;
    int             nLen;
    LPBYTE          lpbySrc;
    LPBYTE          lpbyDst;

    // Set up Cat scores based on Str hits

    W7H2ScanDoCatScores(lpstScan->lpstSigSet->nNumStrSigs,
                        lpstScan->lpabyFStrSigHit,
                        lpstScan->lpstSigSet->lpabyStrSigCatScores,
                        lpstScan->lpabyFCatScore);

    // Set up Cat scores based on Line hits

    W7H2ScanDoCatScores(lpstScan->lpstSigSet->nNumLineSigs,
                        lpstScan->lpabyFLineSigHit,
                        lpstScan->lpstSigSet->lpabyLineSigCatScores,
                        lpstScan->lpabyFCatScore);

    // Apply Func sigs

    W7H2ScanApplySigs(lpstScan->lpstSigSet->nNumFuncSigs,
                      lpstScan->lpstSigSet->lpalpabyFuncSigs,
                      0,
                      lpstScan->lpabyFCmdHit,
                      lpstScan->lpabyFDlgHit,
                      lpstScan->lpabyFOpHit,
                      NULL,
                      lpstScan->lpabyFStrSigHit,
                      lpstScan->lpabyFLineSigHit,
                      NULL,
                      lpstScan->lpabyFCatScore,
                      lpstScan->lpabyGFuncSigHit);

    // Merge Func Cmd hits with Set Cmd hits and clear Func Cmd hits

    nLen = (lpstScan->lpstSigSet->nNumCmds + 7) >> 3;
    lpbySrc = lpstScan->lpabyFCmdHit;
    lpbyDst = lpstScan->lpabyGCmdHit;
    for (i=0;i<nLen;i++)
    {
        *lpbyDst++ |= *lpbySrc;
        *lpbySrc++ = 0;
    }

    // Merge Func Dlg hits with Set Dlg hits and clear Func Dlg hits

    nLen = (lpstScan->lpstSigSet->nNumDlgs + 7) >> 3;
    lpbySrc = lpstScan->lpabyFDlgHit;
    lpbyDst = lpstScan->lpabyGDlgHit;
    for (i=0;i<nLen;i++)
    {
        *lpbyDst++ |= *lpbySrc;
        *lpbySrc++ = 0;
    }

    // Merge Func Op hits with Set Op hits and clear Func Op hits

    lpbySrc = lpstScan->lpabyFOpHit;
    lpbyDst = lpstScan->lpabyGOpHit;
    for (i=0;i<16;i++)
    {
        *lpbyDst++ |= *lpbySrc;
        *lpbySrc++ = 0;
    }

    // Clear Func Str Hits

    nLen = (lpstScan->lpstSigSet->nNumStrSigs + 7) >> 3;
    lpbySrc = lpstScan->lpabyFStrSigHit;
    for (i=0;i<nLen;i++)
        *lpbySrc++ = 0;

    // Merge Func Line hits with Set Line hits and clear Func Line hits

    nLen = (lpstScan->lpstSigSet->nNumLineSigs + 7) >> 3;
    lpbySrc = lpstScan->lpabyFLineSigHit;
    lpbyDst = lpstScan->lpabyGLineSigHit;
    for (i=0;i<nLen;i++)
    {
        *lpbyDst++ |= *lpbySrc;
        *lpbySrc++ = 0;
    }

    // Clear Func Cat scores

    nLen = lpstScan->lpstSigSet->nNumCats;
    for (i=0;i<nLen;i++)
        lpstScan->lpabyFCatScore[i] = 0;
}


//********************************************************************
//
// Function:
//  int W7H2ScanDoSetSigs()
//
// Parameters:
//  lpstScan        Ptr to W7H2_SCAN_T structure.
//
// Description:
//  Determines whether the just completed scan of the macro set
//  matched any of the set sigs.
//
// Returns:
//  int             Index of sig that hits
//                   <  nNumSetSigs     if hit
//                   >= nNumSetSigs     if no hits
//
//********************************************************************

int W7H2ScanDoSetSigs
(
    LPW7H2_SCAN     lpstScan
)
{
    // Set up Set scores based on Line hits

    W7H2ScanDoCatScores(lpstScan->lpstSigSet->nNumLineSigs,
                        lpstScan->lpabyGLineSigHit,
                        lpstScan->lpstSigSet->lpabyLineSigCatScores,
                        lpstScan->lpabyGCatScore);

    // Set up Set scores based on Func hits

    W7H2ScanDoCatScores(lpstScan->lpstSigSet->nNumFuncSigs,
                        lpstScan->lpabyGFuncSigHit,
                        lpstScan->lpstSigSet->lpabyFuncSigCatScores,
                        lpstScan->lpabyGCatScore);

    // Apply Set sigs

    return W7H2ScanApplySigs(lpstScan->lpstSigSet->nNumSetSigs,
                             lpstScan->lpstSigSet->lpalpabySetSigs,
                             0,
                             lpstScan->lpabyGCmdHit,
                             lpstScan->lpabyGDlgHit,
                             lpstScan->lpabyGOpHit,
                             NULL,
                             NULL,
                             lpstScan->lpabyGLineSigHit,
                             lpstScan->lpabyGFuncSigHit,
                             lpstScan->lpabyGCatScore,
                             NULL);
}

#endif // #ifdef MACROHEU

