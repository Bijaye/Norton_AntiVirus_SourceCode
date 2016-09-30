// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// Helper functions
//

BOOL IsPathEmpty(CString& strPath);
BOOL IsPathValid(CString strPath, BOOL bAutoLogon=FALSE, CWnd* pParentWnd=NULL);
BOOL BuildPathName(CString& strPath, CString& strFilename);
BOOL GetRoot(CString& strRoot, CString& strPath, BOOL bTrailingSlash=TRUE);
BOOL FileExists(CString& strFile);
BOOL GetUniqueFile(CString& strExisting, CString& strUnique);
DWORD CreateDirectories(CString sPath);

#if 0
DWORD _CopyFile(CString strSource, CString strTarget, CProgressCtrl* pProgress, int nBufferSize, BOOL* pbCancel);
#endif
