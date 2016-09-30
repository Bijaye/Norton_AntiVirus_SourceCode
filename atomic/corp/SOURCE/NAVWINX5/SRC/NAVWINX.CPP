// Copyright 1992 - 1994 Symantec, Peter Norton Group
// ************************************************************************
//
// $Header:   S:/NAVWINX5/VCS/NAVWINX.CPv   1.0   01 Jun 1998 16:07:46   DBuches  $
//
// Description:
//
// See Also:
//
// ************************************************************************
// $Log:   S:/NAVWINX5/VCS/NAVWINX.CPv  $
// 
//    Rev 1.0   01 Jun 1998 16:07:46   DBuches
// Initial revision.
//
//    Rev 1.1   19 May 1998 15:50:38   PAGBABI
// Added About object
//
//    Rev 1.0   01 Mar 1998 18:51:30   pagbabi
// Initial revision.
// ************************************************************************

// NavWinX.cpp : Implementation of DLL Exports.


// Note: Proxy/Stub Information
//		To build a separate proxy/stub DLL,
//		run nmake -f NavWinXps.mk in the project directory.


// for template debug info trucation

#include "stdafx.h"
#include "resource.h"
#include "about.h"
#include "NavWinX.h"
//#include "NavWinX_i.c"
#include "node.h"
#include "navnode.h"
#include "dataobj.h"


// {19012480-F9A0-11d1-911B-00C04FAC114A}
const CLSID CLSID_CNavExtension =
{ 0x19012480, 0xf9a0, 0x11d1, { 0x91, 0x1b, 0x0, 0xc0, 0x4f, 0xac, 0x11, 0x4a } };

// Define our Node type here: referenced by CDataObject in our library
const wchar_t*  cszNodeType = L"{19012481-F9A0-11d1-911B-00C04FAC114A}";
const GUID cNodeType =
{ 0x19012481, 0xf9a0, 0x11d1, { 0x91, 0x1b, 0x0, 0xc0, 0x4f, 0xac, 0x11, 0x4a } };

CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
    OBJECT_ENTRY(CLSID_CNavExtension, CNavWinNode)
    OBJECT_ENTRY(CLSID_About, CNavextAbout)
END_OBJECT_MAP()

class CNavWinXApp : public CWinApp
{
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
};

CNavWinXApp theApp;

BOOL CNavWinXApp::InitInstance()
{
	_Module.Init(ObjectMap, m_hInstance);
	return CWinApp::InitInstance();
}

int CNavWinXApp::ExitInstance()
{
	_Module.Term();
	return CWinApp::ExitInstance();
}

/////////////////////////////////////////////////////////////////////////////
// Used to determine whether the DLL can be unloaded by OLE

STDAPI DllCanUnloadNow(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return (AfxDllCanUnloadNow()==S_OK && _Module.GetLockCount()==0) ? S_OK : S_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Returns a class factory to create an object of the requested type

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	return _Module.GetClassObject(rclsid, riid, ppv);
}

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
	// registers object, but NOT typelib NOR all interfaces in typelib
	return _Module.RegisterServer(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
	_Module.UnregisterServer();
	return S_OK;
}


