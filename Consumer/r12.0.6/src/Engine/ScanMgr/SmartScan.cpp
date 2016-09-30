
#pragma warning (disable: 4786)

#include "Stdafx.h"
#include <algorithm>
#include "SmartScan.h"
#include "ccFile.h"
#include "FileAPI.h"
#include "OSInfo.h"

#include "NAVSettingsHelperEx.h"

#include <vector>

///////////////////////////////////////////////////////////////////////////////
//
// Globals
//

struct OfficeDocumentSig
{
	DWORD First; 
	DWORD Second;
};

// OLE2 beta signitures

#define OLE2_BETA_SIG1_HIGH (MAKEWORD(0xd0, 0xcf), MAKEWORD(0x11, 0xe0))
#define OLE2_BETA_SIG1_LOW  (MAKEWORD(0x0e, 0x11), MAKEWORD(0xcf, 0x0d))

#define OLE2_BETA_SIG2_HIGH	(MAKEWORD(0x0e, 0x11), MAKEWORD(0xfc, 0x0d))
#define OLE2_BETA_SIG2_LOW	(MAKEWORD(0xd0, 0xcf), MAKEWORD(0x11, 0xe0))

#define EXECUTABLE_SIG_MZ	MAKEWORD(0x4D, 0x5A)
#define EXECUTABLE_SIG_ZM	MAKEWORD(0x5A, 0x4D)
#define EXECUTABLE_SIG_NE	MAKEWORD(0x4E, 0x45)
#define EXECUTABLE_SIG_LE	MAKEWORD(0x4C, 0x45)
#define EXECUTABLE_SIG_PE	MAKEWORD(0x50, 0x45)

///////////////////////////////////////////////////////////////////////////////

CSmartScan::CSmartScan()
{
	m_bReady = false;
}

bool CSmartScan::Initialize()
{
	if (m_bReady == true)
		return true;

    ::std::string strExtensions;

	if (LoadExtensions(strExtensions) == false)
		return false;

	if (ParseExtensions(strExtensions) == false)
		return false;

	return (m_bReady = true);
}

bool CSmartScan::IsMatch(const char* szOemFileName) const
{
	_ASSERT(m_bReady == true);

    // If the Extension list could not be initialized scan everything
    if( !m_bReady )
        return true;

	// Make sure we're in OEM mode before trying to open the file.

	CFileAPI FileAPI;
	FileAPI.SwitchAPIToOEM();

	// First check to see if the file is an Office document or an executable.
	// If so, we should scan it.

	if (IsOfficeDocumentOrExecutable(szOemFileName) == true)
		return true;

	// If it's not an Office document or an executable, check it against the
	// file extension list.

	return IsFileInExtensionList(szOemFileName);
}

bool CSmartScan::LoadExtensions(::std::string& strExtensions)
{
	TRACEHR (h);

	bool bResult = true;
	CNAVOptSettingsEx NavOpts(m_ccSettings);

	try
	{
		h.Verify( NavOpts.Init() );
		strExtensions = NavOpts.GetString(SCANNER_Ext);
	}
	catch(_com_error& err)
	{
		h = err;
		bResult = false;
	}

	return bResult;
}

bool CSmartScan::ParseExtensions(const ::std::string& strExtensions)
{
	m_ExtensionsList.clear();

	char* pBuffer = _tcsdup(strExtensions.c_str());

	if (pBuffer == NULL)
		return false;

	char* pExtension = _tcstok(pBuffer, _T(" "));

	for(; pExtension != NULL; pExtension = _tcstok(NULL, _T(" ")))
	{
		CharUpper(pExtension);
		m_ExtensionsList.push_back(pExtension);
	}

	free(pBuffer);

	return true;
}

bool CSmartScan::IsFileInExtensionList(const char* szFileName) const
{
	const char* pSplit = _tcsrchr(szFileName, _T('.'));

	if (pSplit == NULL)
		return false;

	char* pExtension = _tcsdup(CharNext(pSplit));

	if (pExtension == NULL)
		return false;

	_tcsupr(pExtension);

	EXTENSIONS_LIST::const_iterator Iter;
	
	for(Iter = m_ExtensionsList.begin(); Iter != m_ExtensionsList.end(); Iter++)
	{
		if (IsMatchingExtension((*Iter).c_str(), pExtension) == true)
			break;
	}
	
	free(pExtension);

	if (Iter == m_ExtensionsList.end())
		return false;

	return true;
}

bool CSmartScan::IsOfficeDocumentOrExecutable(const char* szFileName) const
{
    ccLib::CFile file;
    
    if ( !file.Open(szFileName, GENERIC_READ, FILE_SHARE_READ,NULL, OPEN_EXISTING, 0, NULL))
    {
		COSInfo osi;
		// Try unicode version for long paths
		if( osi.IsWinNT() )
		{
            ::std::vector<WCHAR> wName;
			CFileAPI::ConvertLongFileToUnicodeBuffer( szFileName, wName );
			HANDLE hFile = CreateFileW(&wName[0],
							            GENERIC_WRITE,
							            0,
							            NULL,
							            OPEN_EXISTING,
							            FILE_ATTRIBUTE_NORMAL,
							            NULL);
            if (!file.Attach (hFile))
                return false;
		}
    }

	DWORD dwBytesRead;
	OfficeDocumentSig SigBuffer;

	if (!ReadFile(file, &SigBuffer, sizeof(SigBuffer), &dwBytesRead, NULL))
		return false;

	if (dwBytesRead != sizeof(SigBuffer))
		return false;

	// Check the standard OLE sigs.

	OfficeDocumentSig Sig =
	{
		MAKELONG(MAKEWORD(0xd0, 0xcf), MAKEWORD(0x11, 0xe0)),
		MAKELONG(MAKEWORD(0xa1, 0xb1), MAKEWORD(0x1a, 0xe1))
	};
	
	if ((Sig.First == SigBuffer.First) && (Sig.Second == SigBuffer.Second))
		return true;

	// Check the two beta OLE sigs.

	if ((OLE2_BETA_SIG1_HIGH == SigBuffer.First) &&
		(OLE2_BETA_SIG1_LOW == SigBuffer.Second))
	{
		return true;
	}

	if ((OLE2_BETA_SIG2_HIGH == SigBuffer.First) &&
		(OLE2_BETA_SIG2_LOW == SigBuffer.Second))
	{
		return true;
	}

	// Check to see if the file is an executable.

	if ((LOWORD(SigBuffer.First) == EXECUTABLE_SIG_MZ) ||
		(LOWORD(SigBuffer.First) == EXECUTABLE_SIG_ZM) ||
		(LOWORD(SigBuffer.First) == EXECUTABLE_SIG_NE) ||
		(LOWORD(SigBuffer.First) == EXECUTABLE_SIG_LE) ||
		(LOWORD(SigBuffer.First) == EXECUTABLE_SIG_PE))
	{
		 return true;
	}
  
	return false;
}

bool CSmartScan::IsMatchingExtension(const char* szPattern, const char* szExtension) const
{
    while (*szPattern)
    {
        if (*szPattern == '*')
        {
			// The '*' character matches one or more characters in szExtension.
			// Go through szExtension until we have a character that matches the
			// next character in szPattern.

            while(true)
            {
				// Are we at the end of the extension?

				if (*szExtension == '\0')
				{
					if (*(CharNext(szPattern)) == '\0')
						return true;
					else
						return false;
				}

				// Do we have a match with the next character in szPattern?

				if (*szExtension == *(CharNext(szPattern)))
					break;

				szExtension = CharNext(szExtension);
            }

			szPattern = CharNext(CharNext(szPattern));
			szExtension = CharNext(szExtension);		
        }
        else if ('?' == *szPattern)
		{
			// '?' matches exactly one character in szExtension.

			if (*szExtension == '\0')
				return false;

			szPattern = CharNext(szPattern);
			szExtension = CharNext(szExtension);
		}
		else
        {
			// We're dealing with a non-wildcard character so the current
			// character in szPattern and szExtension must match.

			if (*szPattern != *szExtension)
				return false;

			// If we're dealing with a DBCS character, make sure both bytes match.

			if (IsDBCSLeadByte(*szPattern))
			{
				if (szPattern[1] != szExtension[1])
					return false;
			}

			szPattern = CharNext(szPattern);
			szExtension = CharNext(szExtension);
        }
    }
	
    // We've reached the end of the pattern so this should also be the end of
	// the extension if we have a match.
	
    return(*szExtension == '\0' ? true : false);
}

