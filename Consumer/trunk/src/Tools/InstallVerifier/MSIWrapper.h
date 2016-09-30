////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// MSIWrapper.h : header file
//

#if !defined(MSIWRAPPER_H)
#define MSIWRAPPER_H

#define WINDOWSNT "VersionNT"
#define WINDOWS95 "Version95"
#define WINDOWS9X "Version9x"

#include <msi.h>

/////////////////////////////////////////////////////////////////////////////
// CMSIWrapperFile
class CMSIWrapperFile
{
public:
	CString strFile;
	CString strComponent;
	CString strFileName;
	UINT	nFileSize;
	CString	strVersion;
	UINT	nLanguage;
	UINT	nAttributes;
	UINT	nSequence;

	CMSIWrapperFile()
	{
		strFile = _T("");
		strComponent = _T("");
		strFileName = _T("");
		nFileSize = 0;
		strVersion = _T("");
		nLanguage = 1033;
		nAttributes = 0;
		nSequence = 0;
	}
};

/////////////////////////////////////////////////////////////////////////////
// CMSIWrapperComponent
class CMSIWrapperComponent
{
public:
	CString strComponent;
	CString strComponentID;
	CString	strDirectory;
	UINT	nAttributes;
	CString	strCondition;
	CString strKeyPath;

	CMSIWrapperComponent()
	{
		strComponent = _T("");
		strComponentID = _T("");
		strDirectory = _T("");
		nAttributes = 0;
		strCondition = _T("");
		strKeyPath = _T("");
	}
};
/////////////////////////////////////////////////////////////////////////////
//CMSIWrapperDirectory
class CMSIWrapperDirectory
{
public:
	CString strDirectory;
	CString strParent;
	CString strDefault;

	CMSIWrapperDirectory()
	{
		strDirectory =_T("");
		strParent = _T("");
		strDefault = _T("");
	}
};


/////////////////////////////////////////////////////////////////////////////
// CMSIWrapperRegistry
class CMSIWrapperRegistry
{
public:
	CString strRegistry;
	UINT	nRoot;
	CString strKey;
	CString	strName;
	CString	strValue;
	CString strComponent;

	CMSIWrapperRegistry()
	{
		strRegistry = _T("");
		nRoot = 0;
		strKey = _T("");
		strName = _T("");
		strValue = _T("");
		strComponent = _T("");
	}
};

/////////////////////////////////////////////////////////////////////////////
   // CMSIWrapperAppID
   class CMSIWrapperAppID
   {
   public:
   	CString strAppID;
   	CString strRemoteSvrName;
   	CString	strLocalService;
   	CString	strServiceParameters;
   	CString strDllSurrogate;
   	UINT nActivateAtStorage;
   	UINT nRunAsInteractiveUser;
   
   	CMSIWrapperAppID()
   	{
   		strAppID = _T("");
   		nActivateAtStorage = 0;
   		nRunAsInteractiveUser = 0;
   		strRemoteSvrName = _T("");
   		strLocalService = _T("");
   		strServiceParameters = _T("");
   		strDllSurrogate = _T("");
   	}
   };

/////////////////////////////////////////////////////////////////////////////
   // CMSIWrapperTypeLib
   class CMSIWrapperTypeLib
   {
   public:
   	CString strLibID;
   	CString strComponent;
   	CString	strDescription;
   	CString	strDirectory;
   	CString strFeature;
   	UINT nLanguage;
   	UINT nVersion;
   	UINT nCost;
   
   	CMSIWrapperTypeLib()
   	{
   		strLibID = _T("");
   		nLanguage = 0;
   		nVersion = 0;
   		nCost= 0;
   		strComponent = _T("");
   		strDescription = _T("");
   		strDirectory = _T("");
   		strFeature = _T("");
   	}
   };

class CMSIWrapperMIME
{
  public:
   	CString strContentType;
   	CString strExtension;
   	CString	strCLSID;
   
   	CMSIWrapperMIME()
   	{
   		strContentType = _T("");
   		strExtension = _T("");
   		strCLSID = _T("");
   	}
  };
class CMSIWrapperClass
   {
   public:
   	CString strCLSID;
   	CString strContext;
   	CString	strComponent;
   	CString strProgID;
   	CString strAppId;
   	CString strFileTypeMask;
   	CString strIcon;
   	CString strDefInprocHandler;
   	CString strArgument;
   	CString strFeature;
   	CString strAttributes;
   	UINT nIconIndex;
   
   	CMSIWrapperClass()
   	{
   		strCLSID = _T("");
   	}
   };
   class CMSIWrapperProgID
   {
   public:
   	CString strProgID;
   	CString strProgID_Parent;
   	CString	strClass;
   	CString strDescription;
   	CString strIcon;
   	UINT nIconIndex;
   
   	CMSIWrapperProgID()
   	{
   		strProgID = _T("");
   	}
   };
/////////////////////////////////////////////////////////////////////////////
// CMSIWrapper
// See MSIWrapper.cpp for the implementation of this class
//

class CMSIWrapper
{
public:
	CMSIWrapper();
	CMSIWrapper( CString& );
	virtual ~CMSIWrapper();

	void SetFilename( CString& );
	CString GetFilename();
	UINT OpenDatabase();
	UINT CloseDatabase();

	UINT OpenFileTable( CString strQuery = _T("") );
	int  CountFiles();
	UINT GetNextFile( CMSIWrapperFile *);
	UINT CloseFileTable();

	UINT OpenComponentTable( CString strQuery = _T("") );
	int  CountComponents();
	UINT GetNextComponent( CMSIWrapperComponent *);
	UINT CloseComponentTable();

	UINT OpenDirectoryTable( CString strQuery = _T("") );
	int  CountDirectorys();
	UINT GetNextDirectory( CMSIWrapperDirectory *);
	UINT CloseDirectoryTable();
	
	UINT OpenRegistryTable( CString strQuery = _T("") );
	int  CountRegistry();
	UINT GetNextRegistry( CMSIWrapperRegistry *);
	UINT CloseRegistryTable();

	UINT OpenAppIDTable( CString strQuery = _T("") );
   	int  CountAppID();
   	UINT GetNextAppID( CMSIWrapperAppID *);
   	UINT CloseAppIDTable();
   
   	UINT OpenProgIDTable( CString strQuery = _T("") );
   	int  CountProgID();
   	UINT GetNextProgID( CMSIWrapperProgID *);
   	UINT CloseProgIDTable();
   
   	UINT OpenMIMETable( CString strQuery = _T("") );
   	int  CountMIME();
   	UINT GetNextMIME( CMSIWrapperMIME *);
   	UINT CloseMIMETable();
   
   	UINT OpenTypeLibTable( CString strQuery = _T("") );
   	int  CountTypeLib();
   	UINT GetNextTypeLib( CMSIWrapperTypeLib *);
   	UINT CloseTypeLibTable();
	
	UINT OpenPackage(LPCTSTR szPackagePath);
	UINT ClosePackage();
	UINT GetTargetPath(LPCTSTR szName, CString *path);

private:
	CString m_strFilename;
	MSIHANDLE m_MSIHandle;
	MSIHANDLE m_FileView;
	MSIHANDLE m_ComponentView;
	MSIHANDLE m_RegistryView;
	MSIHANDLE m_DirectoryView;

	MSIHANDLE m_TypeLibView;
 	MSIHANDLE m_MIMEView;
 	MSIHANDLE m_ClassView;
 	MSIHANDLE m_ProgIDView;
	MSIHANDLE m_AppIDView;

	MSIHANDLE m_hInstall;

	UINT OpenTable( CString&, MSIHANDLE * );
	UINT CloseTable( MSIHANDLE );
	int CountRecords( MSIHANDLE );

	

};

#endif // _MSIWRAPPER.H_
