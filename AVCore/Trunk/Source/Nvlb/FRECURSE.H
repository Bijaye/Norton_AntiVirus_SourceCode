//***************************************************************************
// Copyright 1997 Symantec Corporation
//***************************************************************************
//	FILE_NAME:
//***************************************************************************
// $Header:   S:/MAPISEND/VCS/frecurse.H_v   1.0   21 Apr 1998 15:24:20   CBROWN  $
//
// Description:
//
//***************************************************************************
// $Log:   S:/MAPISEND/VCS/frecurse.H_v  $
//
//    Rev 1.0   21 Apr 1998 15:24:20   CBROWN
// Initial revision.
//
//    Rev 1.0   19 Dec 1997 16:44:24   CBROWN
// Initial revision.
//
//***************************************************************************
// credits.cpp : implementation file
//**************************************************************************

//////////////////////////////////////////////////////////////////////////////
// FILE: CRecurseFolder.H
#ifndef _RECURSEFOLDER_H
#define _RECURSEFOLDER_H

#include "afxwin.h"
#include <afxtempl.h>


//////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////
class  CRecurseStruct
{
  public:
    CString     m_strRoot;
    HANDLE      m_hHandle;

};

typedef CList < CRecurseStruct, CRecurseStruct& > CFolderList;

//////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////
//#if _MSC_VER > 1020   // if later than VC++ 4.2
//using namespace std;   // the std C++ libs are in namespace std
//#endif



//////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////
class  CRecurseFolder : public CFileFind
{

    CRecurseStruct      m_pRecurseStruct;
    CString				m_strMatchSpec;
    BOOL                m_bSubFolders;
	BOOL				m_bNoMoeFiles;
    BOOL                m_bNewFolder;

	CFolderList			m_FolderList;

	CRecurseFolder( CRecurseFolder& x );

  public:

    CRecurseFolder();
    virtual ~CRecurseFolder();

	virtual inline BOOL IsFolder(void){ return CFileFind::IsDirectory(); }

	virtual void Close
		(
			void
		);

    virtual BOOL FindFile
        (
            LPCSTR lpszPath,
            DWORD  dwUnused = 0,
            BOOL   bSubFolders = TRUE
        );

    virtual BOOL FindNextFile
        (
            void
        );

};

#endif //_RECURSEFOLDER_H

