//************************************************************************
//
// $Header:   S:/NAVEX/VCS/wdencwr.cpv   1.1   14 Mar 1997 16:35:02   DCHI  $
//
// Description:
//  Provides write functionality for encrypted documents.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/wdencwr.cpv  $
// 
//    Rev 1.1   14 Mar 1997 16:35:02   DCHI
// Added support for Office 97 repair.
// 
//    Rev 1.0   17 Jan 1997 11:23:56   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef SYM_NLM

#include "storobj.h"
#include "wdencdoc.h"
#include "worddoc.h"
#include "wdencwr.h"

//*************************************************************************
//
// WORD WordDocEncryptedWrite()
//
// Parameters:
//  lpstCallBack        File op callbacks
//  lpstOLEFile         WordDoc stream OLE file info
//  lpstKey             The key to use
//  dwOffset            Write offset in stream
//  lpbyBuffer          Write buffer
//  wBytesToWrite       Number of bytes to write
//
// Description:
//  If the document is encrypted as determined from lpstKey->bEncrypted,
//  then the function first encrypts the contents using the given key.
//  The function then performs a normal write.
//
//  If the document is encrypted, the buffer will return with its
//  contents encrypted.
//
// Returns:
//  wBytesToRead        If successful
//  0xFFFF              If an error occurred
//
//*************************************************************************

WORD WordDocEncryptedWrite
(
    LPCALLBACKREV1      lpstCallBack,
    LPOLE_FILE_T        lpstOLEFile,
    LPWDENCKEY          lpstKey,
    DWORD               dwOffset,
    LPBYTE              lpbyBuffer,
    WORD                wBytesToWrite
)
{
    WORD                w;
    int                 i;

    if (lpstKey->bEncrypted == TRUE)
    {
        // Start no sooner than offset 48

        if (dwOffset < 48)
            w = (WORD)(48 - dwOffset);
        else
            w = 0;

        i = (int)((dwOffset + w) % WD_ENC_KEY_LEN);
        for (;w<wBytesToWrite;w++)
        {
            if (lpbyBuffer[w] != 0)
            {
                if (lpbyBuffer[w] != lpstKey->abyKey[i])
                    lpbyBuffer[w] ^= lpstKey->abyKey[i];
            }

            if (++i == 16)
                i = 0;
        }
    }

    // Write out the buffer

    if (OLESeekWrite(lpstCallBack,
                     lpstOLEFile,
                     dwOffset,
                     lpbyBuffer,
                     wBytesToWrite) != wBytesToWrite)
    {
        // Error writing buffer

        return(0xFFFF);
    }

    // Successful write

    return(wBytesToWrite);
}

#endif  // #ifndef SYM_NLM


