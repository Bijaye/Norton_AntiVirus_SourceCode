// Copyright 1997 Symantec
// -------------------------------------------------------------------------
// SYSWORKS.H
//
// This is a skeletal about box System Works extension for NAV.  This the minimum 
// you need to implement to add a page to the Symantec Standard About box Extension.
//
// Declaration of CAboutExt, CAboutExtFactory, CSystemWorksPage
//
//
// $Header:   S:/ABOUTSW/VCS/sysworks.h_v   1.1   29 Oct 1998 18:21:44   CEATON  $
// -------------------------------------------------------------------------
// $Log:   S:/ABOUTSW/VCS/sysworks.h_v  $
// 
//    Rev 1.1   29 Oct 1998 18:21:44   CEATON
// Added a parameter to GetSystemWorksKey() which returns the parent key value.
// 
// 
//    Rev 1.0   20 Oct 1998 20:22:46   CEATON
// Initial revision.
// 
//    Rev 1.0   13 Oct 1998 18:44:58   rchinta
// Initial revision.
// 
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#ifndef __SYSWORKS_H__
#define __SYSWORKS_H__

///////////////////////////////////////
// CSystemWorksPage

class CSystemWorksPage : public CslDlg
{
public:
	CSystemWorksPage();
	virtual ~CSystemWorksPage();

// overrideables
	virtual BOOL OnInitDialog();
	virtual BOOL UpdateData( BOOL bSaveAndValidate = TRUE );
	virtual BOOL OnCommand( int nID, HWND hCtrl, UINT uNotify );

// attributes
protected:

private:
};

VOID GenerateSystemWorksKey( PHKEY pHKey,CslString& strSystemWorksKey );

#endif // __SYSWORKS_H__
