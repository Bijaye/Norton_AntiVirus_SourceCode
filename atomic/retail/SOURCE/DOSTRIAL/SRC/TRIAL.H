// Copyright 1997 Symantec, Peter Norton Computing Group
// -------------------------------------------------------------------------
// TRIAL.H
//
// $Header:   S:/DOSTRIAL/VCS/trial.h_v   1.0   10 Jul 1997 13:33:06   MKEATIN  $
// -------------------------------------------------------------------------
// $Log:   S:/DOSTRIAL/VCS/trial.h_v  $
// 
//    Rev 1.0   10 Jul 1997 13:33:06   MKEATIN
// Initial revision.
// 
//    Rev 1.0   03 Mar 1997 16:14:50   MKEATIN
// Initial revision.
// 
//    Rev 1.2   26 Jan 1997 13:45:46   JBELDEN
// fixed name of trialinfo to m_trialinfo
// 
//    Rev 1.1   23 Jan 1997 19:13:06   JBELDEN
// put strings in header
// 
//    Rev 1.0   23 Jan 1997 17:01:00   JBELDEN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __TRIAL_H__
#define __TRIAL_H__

#include "platform.h"

typedef signed short int SHORT;   // this is neeeded for sldate.h

#include "sldate.h"

///////////////////////////////////////
// TRIALINFO

typedef struct tagTRIALINFO {
	WORD  wCheckSum;				// checksum of all other elements
	DWORD dwInstalled;			// julian date of install
	DWORD dwLastRan;				// julian date of last run
	DWORD dwTrialDays;			// duration of trial period in days
	DWORD dwExpires;				// julian date of expiration
	DWORD dwLicense;				// hash of key and OS product ID

	tagTRIALINFO() : wCheckSum( 0 ),
		dwInstalled( 0 ), dwLastRan( 0 ), dwTrialDays( 0 ),
		dwExpires( 0 ), dwLicense( 0 ) {}

	inline WORD ComputeCheckSum();
	BOOL Verify()					{ return wCheckSum == ComputeCheckSum(); }
} TRIALINFO, *LPTRIALINFO;

char sz_strKey[256];				  // constructed encryption key
TRIALINFO m_trialInfo;		// trial period parameters
CDate today;					// today's date
FILE *fp1;                 // File handle for writing encrypted trialinfo.

#ifdef __cplusplus
extern "C" {
#endif

int Initialize( LPCTSTR pszCompany, LPCTSTR pszProduct,
		         UINT nMajorRev, UINT nMinorRev );
int Setup( DWORD dwTrialDays, DWORD dwExpires );
int IsValid( BOOL fSilentMode, LPDWORD lpdwDaysLeft );
void EncryptBuffer( LPVOID lpBuffer, DWORD dwBufLen,
		             LPCVOID lpKey, DWORD dwKeyLen );
void DecryptBuffer( LPVOID lpBuffer, DWORD dwBufLen,
		LPCVOID lpKey, DWORD dwKeyLen );

#ifdef __cplusplus
};
#endif


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


////////////////////////////////
// Strings

char  IDS_COMPONENT_NAME[] = { "Symantec TrialWare Component"};
char  IDS_INVALIDTRIAL[]  =  { "Cannot locate a valid Evaluation section in your registry.\nPlease run the SETUP utility to install this program."};
char  IDS_EXPIREDTRIAL[]  =  { "This product's trial period has expired. Please contact your\nsoftware reseller for or Symantec Corp. for information on how\nto get the non-trial version."};
char  sz_TrialDataFile[]   =  { "SYMTDAT.DAT" };




////////////////////////////////
// Error Codes

#define INVALIDARG            5
#define TRIAL_NOTINITIALIZED  10

#endif // __TRIAL_H__