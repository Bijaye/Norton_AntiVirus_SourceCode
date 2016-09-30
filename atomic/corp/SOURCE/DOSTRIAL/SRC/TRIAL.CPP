// Copyright 1997 Symantec, Peter Norton Computing Group
// -------------------------------------------------------------------------
// TRIAL.CPP
// Implementation of DOS trial version code.
//
// $Header:   S:/DOSTRIAL/VCS/TRIAL.CPv   1.1   14 Aug 1998 15:25:42   mdunn  $
// -------------------------------------------------------------------------
// $Log:   S:/DOSTRIAL/VCS/TRIAL.CPv  $
// 
//    Rev 1.1   14 Aug 1998 15:25:42   mdunn
// Quickly hacked together a mechanism to let NAVDX know how many days
// the trial period was.  
// 
//    Rev 1.0   10 Jul 1997 13:33:00   MKEATIN
// Initial revision.
// 
//    Rev 1.0   03 Mar 1997 16:14:42   MKEATIN
// Initial revision.
// 
//    Rev 1.3   04 Feb 1997 16:23:22   JBELDEN
// fixed bug of putting key in cwd instead of where executed 
// program exists.
// 
//    Rev 1.2   26 Jan 1997 13:46:06   JBELDEN
// changed name of global trialInfo to m_trialInfo
// 
//    Rev 1.1   23 Jan 1997 19:13:22   JBELDEN
// 
//    Rev 1.0   23 Jan 1997 16:59:26   JBELDEN
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <dos.h>  // for _dos_setfileattr()
#include "trial.h"
#include "xapi.h"

#include <limits.h>

// Global for telling setup() that trial period has expired.
BOOL bExpired = FALSE;


// ***** This is a big-time hack-ola to get the Atomic trial version to
// support different-length trial periods.  This DWORD is actually declared
// in NAVC.  Yes it would be normal to have the variable here, but declaring
// it here resulted in it being name-mangled and I couldn't prevent the
// mangling (even with __cdecl).  So the variable is declared in NAVC.C
// which creates the right name decoration.

extern "C" DWORD g_dwTrialLength;
//DWORD __cdecl g_dwTrialLength; // this don't work - still makes a C++ mangled name


///////////////////////////////////////
//	Initialize
/*
	Initializes the encryption key and registry key for this TrialWare.
	You must call this method before calling the Setup() or IsValid() methods.

	Returns: TRUE if the operation succeeded, INVALIDARG if invalid
		arguments were passed.
*/
int Initialize(
	LPCTSTR pszCompany,					// company name (e.g. Symantec)
	LPCTSTR pszProduct,					// product name (e.g. Norton Utilities)
	UINT nMajorRev,						// major revision (e.g. 3)
	UINT nMinorRev )					// minor revision (e.g. 1)
{

	// validate arguments
	if (( pszCompany == NULL ) || ( *pszCompany == EOS ))
		return INVALIDARG;

	if (( pszProduct == NULL ) || ( *pszProduct == EOS ))
		return INVALIDARG;

   _fmemset(sz_strKey, '\0', sizeof(sz_strKey));
   sprintf(sz_strKey,"%s%s%d%d",pszCompany, pszProduct, nMajorRev, nMinorRev);

	return TRUE;
}

///////////////////////////////////////
//	Setup
/*
	Sets up the TrialWare by populating a TRIALINFO struct, encrypting
	it and writing it to a file.

	If the trial file was previously setup, this method will indicate
	success (with TRUE) without redoing the setup operation.

	Returns: TRUE if the operation succeeded, or FALSE if the data could
		not be written to the registry.
*/
int Setup(
	DWORD dwTrialDays,					// trial period length (in days)
	DWORD dwExpires )					// expiration date (julian)
{
	const int nDefaultTrial = 30;		// default trial period (in days)
	const int nMaxTrial = 365;			// maximum trial period (in days)
   char buffer[256];

	// fail if we haven't been successfully initialized
	if ( sz_strKey == NULL )
		return TRIAL_NOTINITIALIZED;

	DWORD dwDaysLeft = 0L;

	// don't overwrite a previously setup trial period
	if ( TRUE == ( IsValid(TRUE, &dwDaysLeft )) )
      {
		return TRUE;
      }

   if ( !bExpired )
   {
	   // if specified period is invalid, use default trial period
	   if (( dwTrialDays == 0 ) || ( dwTrialDays > nMaxTrial ))
		   dwTrialDays = nDefaultTrial;

	   // populate trial information structure
	   m_trialInfo.dwInstalled = today.Today();
	   m_trialInfo.dwLastRan = today.Today();
	   m_trialInfo.dwTrialDays = dwTrialDays;

	   // if no expiration date provided, set to maximum (no expiration date)
	   m_trialInfo.dwExpires = ( dwExpires == 0 ) ? ULONG_MAX : dwExpires;

	   // calculate checksum for trial settings
	   m_trialInfo.wCheckSum = m_trialInfo.ComputeCheckSum();

	   TRIALINFO trialInfo;
	   memcpy( &trialInfo, &m_trialInfo, sizeof( TRIALINFO ));

	   // encrypt transitory buffer for placement in registry
      EncryptBuffer( &trialInfo, sizeof( TRIALINFO ),
      		(LPCTSTR)sz_strKey, (DWORD)sizeof(sz_strKey) );


      DOSGetProgramName(buffer);
      NameStripFile(buffer);
      NameAppendFile(buffer,sz_TrialDataFile );


      fp1 = fopen( buffer, "wb");
      if ( fp1 == NULL)
         return FALSE;
      else
         {
         if ( fwrite(&trialInfo,sizeof(trialInfo),1,fp1) !=1 )
            {
            fclose(fp1);
            return FALSE;
            }
         }
      fclose(fp1);
      _dos_setfileattr( buffer, _A_HIDDEN );

      return TRUE;
   }
   else
      return FALSE;

}

///////////////////////////////////////
//	IsValid
/*
	Used to determine if the trial period is still valid (whether or not
	the TrialWare should be allowed to execute). The lpdwDaysLeft parameter
	is optional (you may legally pass NULL).

	Returns: TRUE if the trial is valid (indicating the software can be
		run, bExpired = FALSE if the operation succeeded but the trial
		is invalid, or FALSE if the operation failed.
*/
int IsValid(
	BOOL fSilentMode,					// display no UI (ignore hwndParent)
	LPDWORD lpdwDaysLeft )				// days remaining in trial period
{

   char buffer[256];
	// fail if we haven't been successfully initialized
	if ( sz_strKey == NULL)
      {
		return TRIAL_NOTINITIALIZED;
      }

   DOSGetProgramName(buffer);
   NameStripFile(buffer);
   NameAppendFile(buffer,sz_TrialDataFile );

   fp1 = fopen( buffer, "rb");
   if ( fp1 == NULL)
      {
      return FALSE;
      }
   else
      {
	   // assume success
	   BOOL bResult = TRUE;

      if ( fread(&m_trialInfo, sizeof(m_trialInfo), 1, fp1) != 1 )
         {
         fclose(fp1);
         return FALSE;
         }
      fclose(fp1);

		DecryptBuffer( &m_trialInfo, sizeof( TRIALINFO ),
			(LPCTSTR)sz_strKey, (DWORD)sizeof(sz_strKey) );

      g_dwTrialLength = m_trialInfo.dwTrialDays;

		// confirm validity of trial information
		if ( !m_trialInfo.Verify() ||
			  m_trialInfo.dwInstalled > today.Today() ||
			  m_trialInfo.dwLastRan > today.Today() )
		{
			// trial information is corrupt or has been tampered with
			if ( !fSilentMode )
            {
				printf( IDS_INVALIDTRIAL );
            }
         bExpired = TRUE;
			bResult = FALSE;
		}

		// has the trial period ended or expired?
		if (( today.Today() > m_trialInfo.dwExpires ) ||
			(( today.Today() - m_trialInfo.dwInstalled ) >
			m_trialInfo.dwTrialDays ))
		{
			// trial period has ended or expiration date has passed
   		if ( !fSilentMode )
				printf( IDS_EXPIREDTRIAL );
         bExpired = TRUE;
			bResult =  FALSE;
		}

		// update date of last run member
		if  ( today.Today() > m_trialInfo.dwLastRan )
			m_trialInfo.dwLastRan = today.Today();

		if ( lpdwDaysLeft != NULL )
		{
			// calculate days remaining in trial period
			DWORD dwTrialEnd = m_trialInfo.dwInstalled +
				m_trialInfo.dwTrialDays;

			// use expiration date if it precedes end of trial period
			if ( m_trialInfo.dwExpires < dwTrialEnd )
				dwTrialEnd = m_trialInfo.dwExpires;

			*lpdwDaysLeft = dwTrialEnd - today.Today();
		}

		// calculate checksum for trial settings
		m_trialInfo.wCheckSum = m_trialInfo.ComputeCheckSum();

		TRIALINFO trialInfo;
		memcpy( &trialInfo, &m_trialInfo, sizeof( TRIALINFO ));

		// encrypt transitory buffer for placement in registry
		EncryptBuffer( &trialInfo, sizeof( TRIALINFO ),
			(LPCTSTR)sz_strKey, (DWORD)sizeof(sz_strKey) );

      fp1 = fopen( buffer, "wb");
      if ( fp1 == NULL)
         return FALSE;
      else
         {
         if ( fwrite(&trialInfo,sizeof(trialInfo),1,fp1) !=1 )
            {
            fclose(fp1);
            return FALSE;
            }
         }
         fclose(fp1);
		return bResult;
	}

	if ( !fSilentMode )
      {
		printf( IDS_INVALIDTRIAL);
      }

	return FALSE;
}

///////////////////////////////////////

///////////////////////////////////////
//	EncryptBuffer
/*
	The buffer to be encrypted is XOR'd with the key byte by byte. If the
	key is too short, it is repeated until all of the buffer is encrypted.
*/
void EncryptBuffer(
	LPVOID lpBuffer,					// buffer to encrypt
	DWORD dwBufLen,						// size of buffer to be encrypted
	LPCVOID lpKey,						// key used to encrypt
	DWORD dwKeyLen )					// length of encryption key
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

///////////////////////////////////////
//	DecryptBuffer
/*
	The buffer to be encrypted is XOR'd with the key byte by byte. If the
	key is too short, it is repeated until all of the buffer is decrypted.
*/
void DecryptBuffer(
	LPVOID lpBuffer,					// buffer to decrypt
	DWORD dwBufLen,						// size of buffer to be decrypted
	LPCVOID lpKey,						// key used to decrypt
	DWORD dwKeyLen )					// length of decryption key
{
    LPBYTE lpTemp;
    LPBYTE lpTempBuf = (LPBYTE) lpBuffer;

	for ( lpTemp = (LPBYTE)lpKey; dwBufLen; dwBufLen-- )
	{
		*lpTempBuf++ ^= *lpTemp++;

		if ( lpTemp > ( (LPBYTE)lpKey) + dwKeyLen )
			lpTemp = (LPBYTE)lpKey;
	}
}
