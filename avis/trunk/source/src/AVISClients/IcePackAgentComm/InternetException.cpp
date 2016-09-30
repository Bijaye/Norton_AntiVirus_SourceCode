//------ standard headers ---------------------------------------------
#include "base.h"
#include <wininet.h>

//------ custom headers -----------------------------------------------
#include "InternetException.h"

//------ InternetException --------------------------------------------

//
// constructor
//
InternetException::InternetException(DWORD dwError)
:	m_dwError( dwError ),
	m_dwContext( 0 )
{
}


//
// GetErrorMessage
//
BOOL InternetException::GetErrorMessage(LPTSTR pstrError, UINT nMaxError, PUINT pnHelpContext)
{
//	ASSERT(pstrError != NULL && AfxIsValidString(pstrError, nMaxError));

	if (pnHelpContext != NULL)
		*pnHelpContext = 0;

	LPTSTR lpBuffer;
	BOOL bRet = TRUE;

	HINSTANCE hWinINetLibrary;
	hWinINetLibrary = ::LoadLibraryA("WININET.DLL");

	if (hWinINetLibrary == NULL ||
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_HMODULE,
			hWinINetLibrary, m_dwError,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_SYS_DEFAULT),
			(LPTSTR) &lpBuffer, 0, NULL) == 0)
	{
		// it failed! try Windows...

		bRet = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,  m_dwError,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_SYS_DEFAULT),
			(LPTSTR) &lpBuffer, 0, NULL);
	}

	if (!bRet)
		*pstrError = '\0';
	else
	{
		if (m_dwError == ERROR_INTERNET_EXTENDED_ERROR)
		{
			LPTSTR lpExtended;
			DWORD dwLength = 0;
			DWORD dwError;

			// find the length of the error
			if (!InternetGetLastResponseInfo(&dwError, NULL, &dwLength) &&
				GetLastError() == ERROR_INSUFFICIENT_BUFFER)
			{
				lpExtended = (LPTSTR) LocalAlloc(LPTR, dwLength);
				InternetGetLastResponseInfo(&dwError, lpExtended, &dwLength);
				lstrcpyn(pstrError, lpExtended, nMaxError);
				pstrError += dwLength;
				nMaxError -= dwLength;
				if (nMaxError < 0)
					nMaxError = 0;
				LocalFree(lpExtended);
			}
			//else
			//	TRACE0("Warning: Extended error reported with no response info\n");
			bRet = TRUE;
		}
		else
		{
			lstrcpyn(pstrError, lpBuffer, nMaxError);
			bRet = TRUE;
		}

		LocalFree(lpBuffer);
	}

	::FreeLibrary(hWinINetLibrary);

	return bRet;
}


//
// ThrowInternetException
//
void ThrowInternetException(DWORD dwContext, DWORD dwError /* = 0 */)
{
	if (dwError == 0)
		dwError = ::GetLastError();
//
//	InternetException* pException = new CInternetException(dwError);
//	pException->m_dwContext = dwContext;
//
//	TRACE1("Warning: throwing CInternetException for error %d\n", dwError);
//	THROW(pException);

	InternetException exc( dwError );
	exc.m_dwContext = dwContext;

	throw exc;
}


//------ InternetException --------------------------------------------

//
// constructor
//
MemoryException::MemoryException()
{
}


//
// ThrowMemoryException
//
void ThrowMemoryException()
{
	MemoryException exc;

	throw exc;
}




