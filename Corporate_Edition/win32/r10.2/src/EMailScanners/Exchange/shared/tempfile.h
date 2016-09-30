// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/*===========================================================================*/

#ifndef __TEMPFILE_H__
#define __TEMPFILE_H__

// Class CTemporaryFileRemover holds a filename which will be deleted when the destructor is called.
//
// The MEC and WALKER copy attachments to temporary files.  The SE opens this file (in unknown modes).  We
// can't use the FILE_FLAG_DELETE_ON_CLOSE on the file for several reasons.  1) FILE_SHARE_DELETE isn't
// supported on WIN9x.  2) the SE uses _tsopen() to open the file and even on NT it doesn't support FILE_SHARE_DELETE.
// 3) In early implementations of the scan engine api, the filename (not the IO table) is passed to the scan engine.
// The scan engine may open the file in incompatible access modes.
//
// With the above reasons in mind, I have chosen to create the temporary file without the FILE_FLAG_DELETE_ON_CLOSE and
// delete the file when we're done with it.  CTemporaryFileRemover performs the delete when the object goes out of scope.
//
// There is one problem with this approach.  If a crash occurs during processing of the file, the temporary file will
// be left on the system.  On startup we should delete files in our temporary directory.

class CTemporaryFileRemover
{
public :
	TCHAR
		szPath[IMAX_PATH];

	void Clear(void) { *szPath = 0; }
	void Remove(void) { if (*szPath) DeleteFile(szPath); *szPath = 0; }

	CTemporaryFileRemover() { Clear(); }
	~CTemporaryFileRemover() { Remove(); }
};

#endif //__TEMPFILE_H__