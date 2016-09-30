/* Copyright 1993 Symantec Corporation                                  */
/************************************************************************
 *                                                                      *
 * $Header:   S:/SYMKRNL/VCS/PRM_CRPT.C_v   1.0   26 Jan 1996 20:23:00   JREARDON  $ *
 *                                                                      *
 * Description:                                                         *
 *      Functions for encryption/decryption.                            *
 *                                                                      *
 * Contains:                                                            *
 *      BufferEncrypt                                                   *
 *      BufferDecrypt                                                   *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * $Log:   S:/SYMKRNL/VCS/PRM_CRPT.C_v  $ *
// 
//    Rev 1.0   26 Jan 1996 20:23:00   JREARDON
// Initial revision.
// 
//    Rev 1.7   15 Mar 1994 12:33:48   BRUCE
// Changed EXPORT to SYM_EXPORT
// 
//    Rev 1.6   03 Mar 1994 10:59:20   BRAD
// AWK script cleanup.
// 
//    Rev 1.5   02 Mar 1994 00:11:30   BRAD
// Changed parameters for BufferEncrypt() and BufferDecrypt() to LPVOID.
// 
//    Rev 1.4   25 Feb 1994 12:21:40   BRUCE
// Ran scripts to change to SYM_ constants and get ready for Win32
// 
//    Rev 1.3   12 Oct 1993 17:32:24   EHU
// Ifndef'd SYM_NLM BufferEncrypt.
// 
//    Rev 1.2   12 Feb 1993 04:27:00   ENRIQUE
// No change.
// 
//    Rev 1.1   14 Jan 1993 15:12:24   EHU
// Added BufferDecrypt (same as BufferEncrypt right now).
// 
//    Rev 1.0   11 Jan 1993 19:04:18   EHU
// Initial revision.
 ************************************************************************/


#include "platform.h"
#include "xapi.h"

#if !defined(SYM_NLM)
/*
 * BufferEncrypt encrypts buffer lpbyTxt with buffer lpbyKey.
 * lpbyTxt is XOR'd with lpbyKey byte by byte.  If lpbyKey is too short,
 * it is repeated until all of lpbyTxt has been encrypted.
 *
 *  Inputs: lpbyTxt - string to encrypt
 *          lpbyKey - key used to encrypt
 *
 *  Outputs: lpbyTxt - encrypted in place
 */

VOID SYM_EXPORT WINAPI
BufferEncrypt(
    LPVOID  lpbyTxt,                    // buffer to encrypt
    LPVOID  lpbyKey,                    // key used to encrypt
    DWORD   dwTxtLen,                   // length of text
    DWORD   dwKeyLen)                   // length of key
{
    LPBYTE  lpbyTmp;
    LPBYTE  lpbyTxtTmp = (LPBYTE) lpbyTxt;

    for (lpbyTmp = (LPBYTE) lpbyKey; dwTxtLen; dwTxtLen--)
        {
        *lpbyTxtTmp++ ^= *lpbyTmp++;
        if (lpbyTmp > ((LPBYTE) lpbyKey) + dwKeyLen)
            lpbyTmp = (LPBYTE) lpbyKey;
        }
}
#endif



/*
 * BufferDecrypt decrypts buffer lpbyTxt with buffer lpbyKey.
 * lpbyTxt is XOR'd with lpbyKey byte by byte.  If lpbyKey is too short,
 * it is repeated until all of lpbyTxt has been decrypted.
 *
 *  Inputs: lpbyTxt - string to decrypt
 *          lpbyKey - key used to decrypt
 *
 *  Outputs: lpbyTxt - decrypted in place
 */

VOID SYM_EXPORT WINAPI
BufferDecrypt(
    LPVOID  lpbyTxt,                    // buffer to decrypt
    LPVOID  lpbyKey,                    // key used to decrypt
    DWORD   dwTxtLen,                   // length of text
    DWORD   dwKeyLen)                   // length of key
{
    LPBYTE  lpbyTmp;
    LPBYTE  lpbyTxtTmp = (LPBYTE) lpbyTxt;


    for (lpbyTmp = (LPBYTE) lpbyKey; dwTxtLen; dwTxtLen--)
        {
        *lpbyTxtTmp++ ^= *lpbyTmp++;
        if (lpbyTmp > ((LPBYTE) lpbyKey) + dwKeyLen)
            lpbyTmp = (LPBYTE) lpbyKey;
        }
}
