
#include "Stdafx.h"
#include "FileAPI.h"

CFileAPI::CFileAPI()
{
	m_bOriginalANSI = (AreFileApisANSI() ? true : false);
}
	
CFileAPI::~CFileAPI()
{
	RestoreOriginalAPI();
}

void CFileAPI::SwitchAPIToANSI()
{
	SetFileApisToANSI();
}

void CFileAPI::SwitchAPIToOEM()
{
	SetFileApisToOEM();
}

void CFileAPI::RestoreOriginalAPI()
{
	if (m_bOriginalANSI == true)
	{
		SetFileApisToANSI();
	}
	else
	{
		SetFileApisToOEM();
	}
}

bool CFileAPI::IsOriginalANSI() const
{
	return m_bOriginalANSI;
}


void CFileAPI::ConvertLongFileToUnicodeBuffer( const char * szFileName, std::vector<WCHAR> &vUnicodeBuffer )
{
	// Compute length of buffer and reserve space for it
	int iBufferLength = strlen( szFileName ) + 10;
	vUnicodeBuffer.reserve( iBufferLength );

	// Construct extra long filename string.
	wcscpy( &vUnicodeBuffer[0], L"\\\\?\\" );
	MultiByteToWideChar( CP_ACP, 0, szFileName, -1, &vUnicodeBuffer[4], iBufferLength - 4 );
}	