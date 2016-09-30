// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/*===========================================================================*/

#include "ldvpmec.h"
#include "mec.h"

#include "FileHandleStream.h"

void CAttach::GetAttachProps(void)
{
	enum { AttachPropCount = 14 };
	static SizedSPropTagArray (AttachPropCount, sptAttachProps) = { AttachPropCount,
		PR_ATTACH_NUM, PR_DISPLAY_NAME, PR_ATTACH_METHOD, PR_ACCESS_LEVEL,
		PR_ATTACH_ENCODING, PR_ATTACH_TAG, PR_ATTACH_EXTENSION, PR_ATTACH_FILENAME, 
		PR_ATTACH_LONG_FILENAME, PR_ATTACH_PATHNAME, PR_ATTACH_LONG_PATHNAME,
		PR_ATTACH_SIZE, PR_RECORD_KEY, PR_RENDERING_POSITION };

		// Changes to sptAttachProps need to be reflected in the enum AttachPropIndexes.

	ULONG
		cValues;

	LPSPropValue
		paProps;

	HRESULT
		hr;

	if (m_paProps)			// Only do this once per instance
		return;

	hr = m_pIAttach->GetProps((LPSPropTagArray) &sptAttachProps, fMapiUnicode, &cValues, &paProps);
	if ((hr != S_OK && hr != MAPI_W_ERRORS_RETURNED) || cValues != AttachPropCount)
	{
		if (hr == MAPI_W_ERRORS_RETURNED || hr == S_OK)
			m_allocators.FreeBuffer(paProps);
		THROW_CMAPIException(m_allocators, hr, m_pIAttach);
	}

	m_paProps = paProps;
}


LPCTSTR CAttach::DisplayName(void)
{
	GetAttachProps();

	return IsValidProp(m_paProps, INDEX_PR_DISPLAY_NAME, PR_DISPLAY_NAME) ?
		m_paProps[INDEX_PR_DISPLAY_NAME].Value.LPSZ :
		_T("");
}

ULONG CAttach::AttachNum(void)
{
	// we need to reference the attachment via the number in the attachment table,
	// and not via the number in the attachment itself, return the number
	// that was given to us, and not the number in the attachment
	return m_ulAttachmentNumber;
/*	GetAttachProps();

	return IsValidProp(m_paProps, INDEX_PR_ATTACH_NUM, PR_ATTACH_NUM) ?
		m_paProps[INDEX_PR_ATTACH_NUM].Value.ul :
		0;
*/
}

ULONG CAttach::AttachMethod(void)
{
	GetAttachProps();

	return IsValidProp(m_paProps, INDEX_PR_ATTACH_METHOD, PR_ATTACH_METHOD) ?
		m_paProps[INDEX_PR_ATTACH_METHOD].Value.ul :
		0;
}

ULONG CAttach::AccessLevel(void)
{
	GetAttachProps();

	return IsValidProp(m_paProps, INDEX_PR_ACCESS_LEVEL, PR_ACCESS_LEVEL) ?
		m_paProps[INDEX_PR_ACCESS_LEVEL].Value.ul :
		0;
}

const LPSBinary CAttach::AttachEncoding(void)
{
	GetAttachProps();

	return IsValidProp(m_paProps, INDEX_PR_ATTACH_ENCODING, PR_ATTACH_ENCODING) ?
		&m_paProps[INDEX_PR_ATTACH_ENCODING].Value.bin :
		&g_NullSBinary;
}

const LPSBinary CAttach::AttachTag(void)
{
	GetAttachProps();

	return IsValidProp(m_paProps, INDEX_PR_ATTACH_TAG, PR_ATTACH_TAG) ?
		&m_paProps[INDEX_PR_ATTACH_TAG].Value.bin :
		&g_NullSBinary;
}

// CAttach::AttachExtension() returns the extension of the pathname.  This first checks
// PR_ATTACH_EXTENSION (which is not required).  If PR_ATTACH_EXTENSION is not found,
// it checks PR_ATTACH_LONG_FILENAME and finally PR_ATTACH_FILENAME.  If either of the
// filename properties is found, a pointer to the extension in that property string
// is returned.
//
LPCTSTR CAttach::AttachExtension(void)
{
	TCHAR
		szExt[_MAX_EXT];

	LPTSTR
		pName,
		pExt;

	size_t
		len;

	GetAttachProps();

	if (IsValidProp(m_paProps, INDEX_PR_ATTACH_LONG_FILENAME, PR_ATTACH_LONG_FILENAME))
	{
		pName = m_paProps[INDEX_PR_ATTACH_LONG_FILENAME].Value.LPSZ;
	}
	else if (IsValidProp(m_paProps, INDEX_PR_ATTACH_FILENAME, PR_ATTACH_FILENAME))
	{
		pName = m_paProps[INDEX_PR_ATTACH_FILENAME].Value.LPSZ;
	}
	else
		return _T("");	// No extension or filename found

	// need to return a pointer into one of the filename properties

	_tsplitpath(pName, 0, 0, 0, szExt);		// Determine length of extension
	len = _tcslen(szExt);

	pExt = _tcsrchr(pName, _T('\0'));		// backup from the end of the string extension length characters (in a MBCS safe way)
	while (len--)
		pExt = _tcsdec(pName, pExt);

	return pExt;
}

LPCTSTR CAttach::AttachFilename(void)
{
	GetAttachProps();

	return IsValidProp(m_paProps, INDEX_PR_ATTACH_FILENAME, PR_ATTACH_FILENAME) ?
		m_paProps[INDEX_PR_ATTACH_FILENAME].Value.LPSZ :
		_T("");
}

LPCTSTR CAttach::AttachLongFilename(void)
{
	GetAttachProps();

	if (IsValidProp(m_paProps, INDEX_PR_ATTACH_LONG_FILENAME, PR_ATTACH_LONG_FILENAME))
		return m_paProps[INDEX_PR_ATTACH_LONG_FILENAME].Value.LPSZ;
	return AttachFilename();
}

LPCTSTR CAttach::AttachPathname(void)
{
	GetAttachProps();

	return IsValidProp(m_paProps, INDEX_PR_ATTACH_PATHNAME, PR_ATTACH_PATHNAME) ?
		m_paProps[INDEX_PR_ATTACH_PATHNAME].Value.LPSZ :
		_T("");
}

LPCTSTR CAttach::AttachLongPathname(void)
{
	GetAttachProps();

	if (IsValidProp(m_paProps, INDEX_PR_ATTACH_LONG_PATHNAME, PR_ATTACH_LONG_PATHNAME))
		return m_paProps[INDEX_PR_ATTACH_LONG_PATHNAME].Value.LPSZ;

	return AttachPathname();
}

ULONG CAttach::AttachSize(void)
{
	GetAttachProps();

	return IsValidProp(m_paProps, INDEX_PR_ATTACH_SIZE, PR_ATTACH_SIZE) ?
		m_paProps[INDEX_PR_ATTACH_SIZE].Value.ul :
		0;
}

const LPSBinary CAttach::RecordKey(void)
{
	GetAttachProps();

	return IsValidProp(m_paProps, INDEX_PR_RECORD_KEY, PR_RECORD_KEY) ?
		&m_paProps[INDEX_PR_RECORD_KEY].Value.bin :
		&g_NullSBinary;
}


HRESULT CAttach::OpenProperty(ULONG ulPropTag, LPCIID lpiid, ULONG ulInterfaceOptions, ULONG ulFlags, LPUNKNOWN *lppUnk, int bThrow)
{
	HRESULT
		hr;

	hr = m_pIAttach->OpenProperty(ulPropTag, lpiid, ulInterfaceOptions, ulFlags, lppUnk);
	if (hr != S_OK && bThrow)
		THROW_CMAPIException(m_allocators, hr, m_pIAttach);

	return hr;
}

// CAttach::CopyToTemporaryFile() copies a binary attachment to a temporary file
//
// Inputs
//	pszFilename			temporary file name
//
// Outputs
//	returns				S_OK if attachment copied
//						error code if attachment not copied.
//
// Exceptions
//	CMyMemoryException possible in creation of IStream interface
//	CMAPIException possible in accessing attachment

HRESULT CAttach::CopyToTemporaryFile(LPTSTR pszFilename)
{
	static TCHAR s_verboseName[] = _T("CAttach::CopyToTemporaryFile");

	HANDLE
		hFile;

	HRESULT
		hr;

	CComPtr<IStream>
		pAttach,
		pStream;

	ULARGE_INTEGER
		bytesRead,
		bytesWritten;

	if (AttachMethod() != ATTACH_BY_VALUE)
		return MAPI_E_UNEXPECTED_TYPE;

	hr = OpenProperty(PR_ATTACH_DATA_BIN, &IID_IStream, STGM_READ, 0, (LPUNKNOWN *) &pAttach);

	if (hr != S_OK)
	{
		DebugOut(SM_ERROR, "ERROR: %s [%u] %s unable to open attachment", s_verboseName, AttachNum(), DisplayName());
		return hr;
	}

	hFile = CreateFile(pszFilename, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN | FILE_ATTRIBUTE_TEMPORARY, 0);
	if (!hFile)
	{
		DebugOut(SM_ERROR, "ERROR: %s [%u] %s unable create temporary file \"%s\" (%x)", s_verboseName, AttachNum(), DisplayName(), pszFilename, GetLastError());
		return MAPI_E_DISK_ERROR;
	}

	hr = CFileHandleStream::CreateStreamOnFileHandle(hFile, &pStream);
	if (hr != S_OK)
	{
		CloseHandle(hFile);
		DeleteFile(pszFilename);
		DebugOut(SM_ERROR, "ERROR: %s [%u] %s unable to create temporary stream \"%s\"", s_verboseName, AttachNum(), DisplayName(), pszFilename);
		return hr;
	}
	hFile = 0;	// pStream now owns file handle

	bytesRead.QuadPart = 0xFFFFFFFFFFFFFF;
	hr = pAttach->CopyTo(pStream, bytesRead, &bytesRead, &bytesWritten);

	DebugOut(SM_GENERAL, "%s: [%u] %s, file \"%s\", read %I64u, written %I64u", s_verboseName, AttachNum(), DisplayName(), pszFilename, bytesRead.QuadPart, bytesWritten.QuadPart);

	return hr;
}

// CAttach::CopyFromFile() copies a file to the attachment
//
// Inputs
//	pszFile				pointer to path of file				
//
// Outputs
//	returns				S_OK if attachment copied
//						error code if attachment not copied.
// Exceptions
//	CMyMemoryException possible in creation of IStream interface
//	CMAPIException possible in accessing attachment
//
HRESULT CAttach::CopyFromFile(LPTSTR pszFile)
{
	static TCHAR s_verboseName[] = _T("CAttach::CopyFromFile");

	HANDLE
		hFile;

	HRESULT
		hr;

	CComPtr<IStream>
		pAttach,
		pStream;

	ULARGE_INTEGER
		bytesRead,
		bytesWritten;

	if (AttachMethod() != ATTACH_BY_VALUE)
		return MAPI_E_UNEXPECTED_TYPE;

	hr = OpenProperty(PR_ATTACH_DATA_BIN, &IID_IStream, STGM_WRITE, MAPI_CREATE | MAPI_MODIFY, (LPUNKNOWN *) &pAttach);
	if (hr != S_OK)
	{
		DebugOut(SM_ERROR, "ERROR: %s [%u] %s unable to open attachment", s_verboseName, AttachNum(), DisplayName());
		return hr;
	}

	hFile = CreateFile(pszFile, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN | FILE_ATTRIBUTE_TEMPORARY, 0);
	if (!hFile)
	{
		DebugOut(SM_ERROR, "ERROR: %s [%u] %s unable to open file \"%s\" (%x)", s_verboseName, AttachNum(), DisplayName(), pszFile, GetLastError());
		return MAPI_E_DISK_ERROR;
	}

	hr = CFileHandleStream::CreateStreamOnFileHandle(hFile, &pStream);
	if (hr != S_OK)
	{
		CloseHandle(hFile);
		DebugOut(SM_ERROR, "ERROR: %s [%u] %s unable to create file stream \"%s\"", s_verboseName, AttachNum(), DisplayName(), pszFile);
		return hr;
	}
	hFile = 0;	// pStream now owns file handle

	bytesRead.QuadPart = 0xFFFFFFFFFFFFFF;
	hr = pStream->CopyTo(pAttach, bytesRead, &bytesRead, &bytesWritten);

	DebugOut(SM_GENERAL, "%s: [%u] %s, file \"%s\", read %I64u, written %I64u", s_verboseName, AttachNum(), DisplayName(), pszFile, bytesRead.QuadPart, bytesWritten.QuadPart);

	hr = pAttach->SetSize(bytesWritten);

	return hr;
}

ULONG CAttach::RenderingPosition(void)
{
	GetAttachProps();

	return IsValidProp(m_paProps, INDEX_PR_RENDERING_POSITION, PR_RENDERING_POSITION) ?
		m_paProps[INDEX_PR_RENDERING_POSITION].Value.ul :
		0;
}

HRESULT CAttach::SetProps(ULONG cValues, LPSPropValue lpPropArray, LPSPropProblemArray FAR * lppProblems, int bThrow)
{
	static TCHAR s_verboseName[] = _T("CAttach::SetProps");

	HRESULT
		hr;

	hr = m_pIAttach->SetProps(cValues, lpPropArray, lppProblems);
	if (hr != S_OK && bThrow)
		THROW_CMAPIException(m_allocators, hr, m_pIAttach);

	return hr;
}

/* end source file */