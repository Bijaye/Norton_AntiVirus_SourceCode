// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/*===========================================================================*/

#ifndef __FILEHANDLESTREAM_H__
#define __FILEHANDLESTREAM_H__

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
//

class CFileHandleStream : public IStream
{
public :
	// IUnknown methods

    STDMETHOD(QueryInterface)(REFIID riid, void **ppvObject);
    STDMETHOD_(ULONG, AddRef)(void);
    STDMETHOD_(ULONG, Release)(void);

	// ISequentialStream Methods

    STDMETHOD(Read)(void *pv, ULONG cb, ULONG *pcbRead);
    STDMETHOD(Write)(const void *pv, ULONG cb, ULONG *pcbWritten);

	// IStream Methods

    STDMETHOD(Seek)(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER *plibNewPosition);
    STDMETHOD(SetSize)(ULARGE_INTEGER libNewSize);
    STDMETHOD(CopyTo)(IStream *pstm, ULARGE_INTEGER cb, ULARGE_INTEGER *pcbRead, ULARGE_INTEGER *pcbWritten);
    STDMETHOD(Commit)(DWORD grfCommitFlags);
    STDMETHOD(Revert)(void);
    STDMETHOD(LockRegion)(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType);
    STDMETHOD(UnlockRegion)(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType);
    STDMETHOD(Stat)(STATSTG *pstatstg, DWORD grfStatFlag);
    STDMETHOD(Clone)(IStream **ppstm);

	// CFileHandleStream methods

	static HRESULT CreateStreamOnFileHandle(HANDLE hFile, LPSTREAM *ppStm);

private :
	ULONG m_cRef;
	HANDLE m_hFile;

	CFileHandleStream(HANDLE hFile) : m_cRef(0), m_hFile(hFile) {}
	~CFileHandleStream() { if (m_hFile) { CloseHandle(m_hFile); m_hFile = 0; } }
};

#endif //__FILEHANDLESTREAM_H__