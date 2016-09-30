//************************************************************************
//
// $Header:   S:/NAVEX/VCS/WDENCRD.CPv   1.0   17 Jan 1997 11:23:52   DCHI  $
//
// Description:
//  Provides read functionality for encrypted documents.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/WDENCRD.CPv  $
// 
//    Rev 1.0   17 Jan 1997 11:23:52   DCHI
// Initial revision.
// 
//************************************************************************

#include "storobj.h"
#include "wdencdoc.h"
#include "worddoc.h"
#include "wdencrd.h"

//*************************************************************************
//
// WORD WordDocEncryptedRead()
//
// Parameters:
//  lpstCallBack        File op callbacks
//  lpstOLEFile         WordDoc stream OLE file info
//  lpstKey             The key to use
//  dwOffset            Read offset in stream
//  lpbyBuffer          Read buffer
//  wBytesToRead        Number of bytes to read
//
// Description:
//  The function performs a normal read.  Then if the document is
//  encrypted as determined from lpstKey->bEncrypted, then the
//  function decrypts the contents using the given key.
//
// Returns:
//  wBytesToRead        If successful
//  0xFFFF              If an error occurred
//
//*************************************************************************

WORD WordDocEncryptedRead
(
    LPCALLBACKREV1      lpstCallBack,
    LPOLE_FILE_T        lpstOLEFile,
    LPWDENCKEY          lpstKey,
    DWORD               dwOffset,
    LPBYTE              lpbyBuffer,
    WORD                wBytesToRead
)
{
    WORD                w;
    int                 i;

    if (OLESeekRead(lpstCallBack,
                    lpstOLEFile,
                    dwOffset,
                    lpbyBuffer,
                    wBytesToRead) != wBytesToRead)
    {
        // Error reading chunk

        return(0xFFFF);
    }

    if (lpstKey->bEncrypted == FALSE)
    {
        // Not encrypted, so just return

        return(wBytesToRead);
    }

    // Start no sooner than offset 48

    if (dwOffset < 48)
        w = (WORD)(48 - dwOffset);
    else
        w = 0;

    i = (int)((dwOffset + w) % WD_ENC_KEY_LEN);
    for (;w<wBytesToRead;w++)
    {
        if (lpbyBuffer[w] != 0)
        {
            if (lpbyBuffer[w] != lpstKey->abyKey[i])
                lpbyBuffer[w] ^= lpstKey->abyKey[i];
        }

        if (++i == 16)
            i = 0;
    }

    // Successful decryption

    return(wBytesToRead);
}



