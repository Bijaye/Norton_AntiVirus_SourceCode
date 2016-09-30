/////////////////////////////////////////////////////////////////////////////
//  Copyright (c)1998 - Symantec, Peter Norton Computing Group
/////////////////////////////////////////////////////////////////////////////
//
//  $Header:   S:/NAVINST95/VCS/DOSTRIAL.CPv   1.0   14 Aug 1998 10:40:54   mdunn  $
//
//  dostrial.CPP -  Code pulled from the DOSTRIAL project & used to set up
//                  the NAVDX trial info file during install.  This is not
//                  the whole of DOSTRIAL - just the minimum required to
//                  write the info file.  The trial-related function names
//                  were also changed to be more descriptive.
//
/////////////////////////////////////////////////////////////////////////////
//  $Log:   S:/NAVINST95/VCS/DOSTRIAL.CPv  $
// 
//    Rev 1.0   14 Aug 1998 10:40:54   mdunn
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#ifndef SYM_WIN32
#define SYM_WIN32
#endif

#include <windows.h>
#include <winbase.h>
#include <tchar.h>
#include "cwstring.h"
#include <limits.h>


char sz_strKey[256];                    // constructed encryption key
char sz_TrialDataFile[]   =  { "SYMTDAT.DAT" };

#define INVALIDARG            5
#define TRIAL_NOTINITIALIZED  10


/////////////////////////////////////////////////////////////////////////////
// The trial info structure

typedef struct tagTRIALINFO {
    WORD  wCheckSum;                // checksum of all other elements
    DWORD dwInstalled;            // julian date of install
    DWORD dwLastRan;                // julian date of last run
    DWORD dwTrialDays;            // duration of trial period in days
    DWORD dwExpires;                // julian date of expiration
    DWORD dwLicense;                // hash of key and OS product ID

    tagTRIALINFO() : wCheckSum( 0 ),
        dwInstalled( 0 ), dwLastRan( 0 ), dwTrialDays( 0 ),
        dwExpires( 0 ), dwLicense( 0 ) {}

    inline WORD ComputeCheckSum();
} TRIALINFO, *LPTRIALINFO;


/////////////////////////////////////////////////////////////////////////////
//  Local function prototypes

DWORD ToJulian( UINT uDay, UINT uMonth, UINT uYear );
DWORD Today();

int   DosTrialInitialize ( LPCTSTR pszCompany, LPCTSTR pszProduct,
                           UINT nMajorRev, UINT nMinorRev );

int   DosTrialSetup ( LPTSTR szNAVDir, DWORD dwTrialDays, DWORD dwExpires );

void  EncryptBuffer ( LPVOID lpBuffer, DWORD dwBufLen,
                      LPCVOID lpKey, DWORD dwKeyLen );


/////////////////////////////////////////////////////////////////////////////
//
// ToJulian() -- returns the Julian calendar date for a given day/month/year.
//
/////////////////////////////////////////////////////////////////////////////

DWORD ToJulian( UINT uDay, UINT uMonth, UINT uYear )
{
    if ( uYear <= 99 ) 
        uYear += 1900;

    unsigned yh = uYear;
    unsigned long c, ya, j;

    if ( uMonth > 2 ) 
    {
        // wash out the leap day
        uMonth -= 3;                    
    }
    else       
    { 
        uMonth += 9;    
        uYear--; 
    }

    c = uYear / 100;
    ya = uYear - 100 * c;
    j = (( 146097 * c )>>2 ) + (( 1461 * ya ) >> 2 ) + 
        ( 153 * uMonth + 2 ) / 5 + uDay + 1721119;

    return j;
} 


/////////////////////////////////////////////////////////////////////////////
//
// Today() -- returns the Julian calendar date for the current date.
//
/////////////////////////////////////////////////////////////////////////////

DWORD Today()
{
SYSTEMTIME time;

    GetLocalTime ( &time );

    return ToJulian ( time.wDay, time.wMonth, time.wYear );
}


///////////////////////////////////////
// ComputeCheckSum

inline WORD tagTRIALINFO::ComputeCheckSum()
{
    WORD wResult = 0;
     WORD wSize = sizeof( tagTRIALINFO ) - sizeof( wCheckSum );

    register LPBYTE lpBuf = (LPBYTE)this;

     // ignore first member (checksum)
     lpBuf += sizeof( wCheckSum );

   for ( DWORD dwIdx = 0; dwIdx < wSize; dwIdx++ )
    {
        wResult += (WORD)*( lpBuf++ );
    }

    return wResult;
}


///////////////////////////////////////
//    Initialize
/*
    Initializes the encryption key and registry key for this TrialWare.
    You must call this method before calling the Setup() or IsValid() methods.

    Returns: TRUE if the operation succeeded, INVALIDARG if invalid
        arguments were passed.
*/
int DosTrialInitialize(
    LPCTSTR pszCompany,                    // company name (e.g. Symantec)
    LPCTSTR pszProduct,                    // product name (e.g. Norton Utilities)
    UINT nMajorRev,                        // major revision (e.g. 3)
    UINT nMinorRev )                    // minor revision (e.g. 1)
{

    // validate arguments
    if (( pszCompany == NULL ) || ( *pszCompany == '\0' ))
        return INVALIDARG;

    if (( pszProduct == NULL ) || ( *pszProduct == '\0' ))
        return INVALIDARG;

   ZeroMemory(sz_strKey, sizeof(sz_strKey));
   wsprintf(sz_strKey,"%s%s%d%d",pszCompany, pszProduct, nMajorRev, nMinorRev);

    return TRUE;
}


///////////////////////////////////////
//    Setup
/*
    Sets up the TrialWare by populating a TRIALINFO struct, encrypting
    it and writing it to a file.

    If the trial file was previously setup, this method will indicate
    success (with TRUE) without redoing the setup operation.

    Returns: TRUE if the operation succeeded, or FALSE if the data could
        not be written to the registry.
*/
int DosTrialSetup(
    LPTSTR szNAVDir,
    DWORD dwTrialDays,                    // trial period length (in days)
    DWORD dwExpires )                    // expiration date (julian)
{
const int nDefaultTrial = 30;           // default trial period (in days)
const int nMaxTrial = 365;              // maximum trial period (in days)
TRIALINFO m_trialInfo;                  // work area


    // fail if we haven't been successfully initialized
    if ( sz_strKey == NULL )
        return TRIAL_NOTINITIALIZED;

    // if specified period is invalid, use default trial period
    if (( dwTrialDays == 0 ) || ( dwTrialDays > nMaxTrial ))
        dwTrialDays = nDefaultTrial;

       // populate trial information structure
    m_trialInfo.dwInstalled = Today();
    m_trialInfo.dwLastRan = Today();
    m_trialInfo.dwTrialDays = dwTrialDays;

    // if no expiration date provided, set to maximum (no expiration date)
    m_trialInfo.dwExpires = ( dwExpires == 0 ) ? ULONG_MAX : dwExpires;

    // calculate checksum for trial settings
    m_trialInfo.wCheckSum = m_trialInfo.ComputeCheckSum();

TRIALINFO trialInfo;                    // This is the "real" info struct
                                        // that will be written to disk.

    CopyMemory ( &trialInfo, &m_trialInfo, sizeof( TRIALINFO ));

    // encrypt transitory buffer for placement in registry
    EncryptBuffer( &trialInfo, sizeof( TRIALINFO ),
           (LPCTSTR)sz_strKey, (DWORD)sizeof(sz_strKey) );

CWString cstrDatFile;
HANDLE   hDatFile;

    cstrDatFile = szNAVDir;
    cstrDatFile.AppendFile ( sz_TrialDataFile );

    hDatFile = CreateFile ( cstrDatFile, GENERIC_WRITE, 0, NULL, 
                            CREATE_ALWAYS, FILE_ATTRIBUTE_HIDDEN, NULL );

    if ( INVALID_HANDLE_VALUE == hDatFile )
       return FALSE;

BOOL  bRet;
DWORD dwWritten;

    bRet =  WriteFile ( hDatFile, &trialInfo, sizeof(trialInfo),
                        &dwWritten, NULL );

    CloseHandle ( hDatFile );

    return bRet;
}


///////////////////////////////////////
//    EncryptBuffer
/*
    The buffer to be encrypted is XOR'd with the key byte by byte. If the
    key is too short, it is repeated until all of the buffer is encrypted.
*/
void EncryptBuffer(
    LPVOID lpBuffer,                    // buffer to encrypt
    DWORD dwBufLen,                        // size of buffer to be encrypted
    LPCVOID lpKey,                        // key used to encrypt
    DWORD dwKeyLen )                    // length of encryption key
{
    LPBYTE lpTemp;
    LPBYTE lpTempBuf = (LPBYTE)lpBuffer;

    for ( lpTemp = (LPBYTE)lpKey; dwBufLen; dwBufLen-- )
    {
        *lpTempBuf++ ^= *lpTemp++;

        if ( lpTemp > ( (LPBYTE)lpKey ) + dwKeyLen )
            lpTemp = (LPBYTE)lpKey;
    }
}

