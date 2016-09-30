// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/*===========================================================================*/

// Class CFileHandleStream implements an IStream interface for a specified file handle.
// The IStream is obtained by calling the static member function CreateStreamOnFileHandle().
//
// CFileHandleStream::CreateStreamOnFileHandle() assumes ownership for the file handle.  The caller
// should NOT use the file handle for any operations after (successful) return of
// CreateStreamOnFileHandle() or unpredictable results may occur.  The handle will be closed
// when the reference count goes to zero.
//
// Implementation details:
//	The returned IStream is a direct I/O implementation.
//	Commit() and Revert() do nothing.
//	Stat() always returns 0 for pwcsName (the file name).
//	Clone() always fails.
//		Clone() is not easily implemented for file handles.  The cloned instance should have a
//		separate seek pointer.  DuplicateHandle() does not give us this functionality.  To implement
//		Clone(), a copy of the current file position (m_liPos) would need to be maintained.  Methods
//		Read(), Write(), Seek(), CopyTo(), SetSize(), and Clone() would be effected.  These routines
//		would need to seek to the proper location before performing the operation.  Also to insure
//		thread safeness (free-threaded model), a critical section or mutex would need to be used
//		to insure that the entire update occurred without interferance from the clone.
//
// If Clone() is really needed consider doing the following:  Derive a class from CFileHandleStream whose
// create function takes and saves all (or most) of the arguments to CreateFile().  Re-implement Clone()
// (and possibly Stat()) to open the file again using the saved arguments.  Care would need to be taken
// so that Clone()s CreateFile() doesn't truncated the original file.  The clone would then have a
// different file object (with its own seek pointer).  The internal critical section issues would then
// go away.  Of course you would still have external critical section issues (the normal ones you get
// with shared access to files).

#include <windows.h>

#include "FileHandleStream.h"

STDMETHODIMP CFileHandleStream::QueryInterface(REFIID pIID, void **ppvObject)
{
	if (IsEqualIID(pIID, IID_IStream) || IsEqualIID(pIID, IID_IUnknown))
	{
		*ppvObject = (IStream *) this;
		AddRef();
		return S_OK;
	}

	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CFileHandleStream::AddRef(void)
{
    return ++m_cRef;
}

STDMETHODIMP_(ULONG) CFileHandleStream::Release(void)
{
	if ((--m_cRef) != 0)
		return m_cRef;

	delete this;

	return 0;
}

STDMETHODIMP CFileHandleStream::Read(void *pv, ULONG cb, ULONG *pcbRead)
{
	DWORD
		err;

	ULONG
		tRead;

	BOOL
		rv;
	
	if (!pcbRead)
		pcbRead = &tRead;

	if (IsBadWritePtr(pv, cb) || IsBadWritePtr(pcbRead, sizeof(*pcbRead)))
		return STG_E_INVALIDPOINTER;

	rv = ReadFile(m_hFile, pv, cb, pcbRead, 0);
	if (rv)
		return S_OK;

	err = GetLastError();

	if (err == ERROR_ACCESS_DENIED)
		return STG_E_ACCESSDENIED;

	return E_FAIL;
}

STDMETHODIMP CFileHandleStream::Write(void const *pv, ULONG cb, ULONG *pcbWritten)
{
	DWORD
		err;

	ULONG
		tWritten;

	BOOL
		rv;
	
	if (!pcbWritten)
		pcbWritten = &tWritten;

	if (IsBadWritePtr((void *) pv, cb) || IsBadWritePtr(pcbWritten, sizeof(*pcbWritten)))
		return STG_E_INVALIDPOINTER;

	rv = WriteFile(m_hFile, pv, cb, pcbWritten, 0);
	if (rv)
		return S_OK;

	err = GetLastError();

	if (err == ERROR_HANDLE_DISK_FULL || err == ERROR_DISK_FULL)
		return STG_E_MEDIUMFULL;

	if (err == ERROR_ACCESS_DENIED)
		return STG_E_ACCESSDENIED;

	return STG_E_CANTSAVE;
}

STDMETHODIMP CFileHandleStream::Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER *plibNewPosition)
{
	ULARGE_INTEGER
		tNewPosition;

	LONG
		low = dlibMove.LowPart,
		high = dlibMove.HighPart;

	DWORD
		err,
		origin;
	
	if (dwOrigin == STREAM_SEEK_SET)
		origin = FILE_BEGIN;
	else if (dwOrigin == STREAM_SEEK_CUR)
		origin = FILE_CURRENT;
	else if (dwOrigin == STREAM_SEEK_END)
		origin = FILE_END;
	else
		return STG_E_INVALIDFUNCTION;

	if (!plibNewPosition)
		plibNewPosition = &tNewPosition;

	if (IsBadWritePtr(plibNewPosition, sizeof(*plibNewPosition)))
		return STG_E_INVALIDPOINTER;

	low = SetFilePointer(m_hFile, low, &high, origin);
	err = GetLastError();

	plibNewPosition->LowPart = low;
	plibNewPosition->HighPart = high;

	return err == NO_ERROR ? S_OK : E_FAIL;
}

STDMETHODIMP CFileHandleStream::SetSize(ULARGE_INTEGER libNewSize)
{
	BOOL
		rv;

	LONG
		low = libNewSize.LowPart,
		high = libNewSize.HighPart,
		saveLow,
		saveHigh = 0;

	DWORD
		err;

	saveLow = SetFilePointer(m_hFile, 0, &saveHigh, FILE_CURRENT);
				// SetSize() should not effect file pointer.

	low = SetFilePointer(m_hFile, low, &high, FILE_BEGIN);
	err = GetLastError();
	if (err != NO_ERROR)
		return E_FAIL;

	rv = SetEndOfFile(m_hFile);
	SetFilePointer(m_hFile, saveLow, &saveHigh, FILE_BEGIN);	// Restore original file pointer
	if (rv)
		return S_OK;

	err = GetLastError();
	if (err == ERROR_HANDLE_DISK_FULL || err == ERROR_DISK_FULL)
		return STG_E_MEDIUMFULL;

	return E_FAIL;
}

STDMETHODIMP CFileHandleStream::CopyTo(IStream *pDest, ULARGE_INTEGER cb, ULARGE_INTEGER *pcbRead, ULARGE_INTEGER *pcbWritten)
{
	ULARGE_INTEGER
		tRead,
		tWritten;

	char
		buf[2048];

	unsigned __int64
		remaining = cb.QuadPart;

	HRESULT
		hr;

	BOOL
		rv;

	ULONG
		toRead = 0,
		cbRead,
		cbWritten;

	if (!pcbRead)
		pcbRead = &tRead;

	if (!pcbWritten)
		pcbWritten = &tWritten;

	if (IsBadWritePtr(pcbRead, sizeof(*pcbRead)) || IsBadWritePtr(pcbWritten, sizeof(*pcbWritten)))
		return STG_E_INVALIDPOINTER;

	pcbRead->QuadPart = 0;
	pcbWritten->QuadPart = 0;

	while (remaining > 0)
	{
		toRead = (ULONG) (remaining > sizeof(buf) ? sizeof(buf) : toRead);
		rv = ReadFile(m_hFile, buf, toRead, &cbRead, 0);
		if (!rv)
			return E_FAIL;
		if (!cbRead)
			break;
		pcbRead->QuadPart += cbRead;
		remaining -= cbRead;

		hr = pDest->Write(buf, cbRead, &cbWritten);
		if (hr != S_OK)
			return hr;

		if (cbRead != cbWritten)
			return E_FAIL;

		pcbWritten->QuadPart += cbWritten;
	}

	return S_OK;
}

STDMETHODIMP CFileHandleStream::Commit(DWORD)
{
	return S_OK;
}

STDMETHODIMP CFileHandleStream::Revert(void)
{
	return S_OK;
}


STDMETHODIMP CFileHandleStream::LockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType)
{
	if (dwLockType != LOCK_EXCLUSIVE)
		return STG_E_INVALIDFUNCTION;

	return LockFile(m_hFile, libOffset.LowPart, libOffset.HighPart, cb.LowPart, cb.HighPart) ? S_OK : STG_E_LOCKVIOLATION;
}

STDMETHODIMP CFileHandleStream::UnlockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType)
{
	if (dwLockType != LOCK_EXCLUSIVE)
		return STG_E_INVALIDFUNCTION;

	return UnlockFile(m_hFile, libOffset.LowPart, libOffset.HighPart, cb.LowPart, cb.HighPart) ? S_OK : STG_E_LOCKVIOLATION;
}

STDMETHODIMP CFileHandleStream::Stat(STATSTG *pstatstg, DWORD)
{
	BY_HANDLE_FILE_INFORMATION
		info;

	if (IsBadWritePtr(pstatstg, sizeof(*pstatstg)))
		return STG_E_INVALIDPOINTER;

	if (!GetFileInformationByHandle(m_hFile, &info))
		return STG_E_ACCESSDENIED;

	memset(pstatstg, 0, sizeof(*pstatstg));
	pstatstg->pwcsName = 0;			// We never return the name
    pstatstg->type = STGTY_STREAM;
    pstatstg->cbSize.LowPart = info.nFileSizeLow;
    pstatstg->cbSize.HighPart = info.nFileSizeHigh;
    pstatstg->mtime = info.ftLastWriteTime;
    pstatstg->ctime = info.ftCreationTime;
    pstatstg->atime = info.ftLastAccessTime;
    pstatstg->grfLocksSupported = LOCK_EXCLUSIVE;

	return S_OK;
}

STDMETHODIMP CFileHandleStream::Clone(IStream **ppstm)
{
	if (IsBadWritePtr(*ppstm, sizeof(*ppstm)))
		return STG_E_INVALIDPOINTER;

	return E_NOTIMPL;
}

HRESULT CFileHandleStream::CreateStreamOnFileHandle(HANDLE hFile, LPSTREAM *ppStm)
{
	CFileHandleStream
		*t = new CFileHandleStream(hFile);

	if (!t)
		return E_OUTOFMEMORY;

	(*ppStm = (IStream *) t)->AddRef();

	return S_OK;
}

/* end source file */