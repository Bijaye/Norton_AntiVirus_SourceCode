////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include ".\xmlhelper.h"


CXMLHelper::CXMLHelper(void)
{
	CoInitialize(NULL);
}

CXMLHelper::~CXMLHelper(void)
{
}
HRESULT CXMLHelper::MSIToXML(CString msipath, CString xmlpath)
{  
	CloseXMLDocument();
	HRESULT hr;
	CString strSQL = _T("");
	CString strFullPath = _T("");
	CMSIWrapper *msiObj = NULL;
	CMSIWrapperFile *msiFileObj = NULL;
	CMSIWrapperComponent *msiCompObj = NULL;
	CMSIWrapperDirectory *msiDirObj = NULL;

	msiObj = new CMSIWrapper(msipath);
	msiFileObj = new CMSIWrapperFile();
	msiCompObj = new CMSIWrapperComponent();
	msiDirObj = new CMSIWrapperDirectory();

	try
	{
		hr = m_pDoc.CreateInstance("Msxml2.DOMDocument");
		if(FAILED(hr))
		{
			MessageBox(NULL,"Not able to create instance of dom document!", "Error", MB_OK|MB_ICONERROR );
			return E_FAIL;
		}

		m_pDoc->async = VARIANT_FALSE; // default - true,
		m_pDoc->preserveWhiteSpace = VARIANT_TRUE;
		
		m_pDocRoot = m_pDoc->documentElement;
		m_pDocRoot = m_pDoc->createElement("InstallVerifyData");
		
		m_pDoc->documentElement = m_pDocRoot;

		// open msi
		if (ERROR_SUCCESS != msiObj->OpenDatabase())
			throw (" ERROR opening MIS database.\n");

		if(ERROR_SUCCESS != msiObj->OpenPackage(msipath))
			throw(" ERROR opening pacakge. \n");

		// open FILE table
		if (ERROR_SUCCESS != msiObj->OpenFileTable())
		{
			MessageBox(NULL,"Not able to open file table", "Error", MB_OK|MB_ICONERROR );
			return E_FAIL;
		}

		//enum all files
		while( ERROR_SUCCESS == msiObj->GetNextFile( msiFileObj ) )
		{
			strSQL.Format(_T("SELECT * FROM Component WHERE Component = '%s'"), msiFileObj->strComponent);
			if (ERROR_SUCCESS != msiObj->OpenComponentTable(strSQL))
			{
				MessageBox(NULL,"Not able to open component", "Error", MB_OK|MB_ICONERROR );
				return E_FAIL;
			}
			//should retrun 1 row here
			if (ERROR_SUCCESS != msiObj->GetNextComponent( msiCompObj))
			{
				MessageBox(NULL,"Not able to get next component", "Error", MB_OK|MB_ICONERROR );
				return E_FAIL;
			}
			
			//msiCompObj->strDirectory = msiCompObj->strDirectory.SpanExcluding("|.");
			msiFileObj->strFileName = msiFileObj->strFileName.SpanExcluding("|");
			

			strFullPath = msiFileObj->strFileName;
			CString strTemp = msiCompObj->strDirectory;

			while(1)
			{
				strSQL.Format(_T("SELECT * FROM Directory WHERE Directory='%s'"), strTemp);
				if (ERROR_SUCCESS != msiObj->OpenDirectoryTable(strSQL))
				{
					MessageBox(NULL,"Not able to open component", "Error", MB_OK|MB_ICONERROR );
					return E_FAIL;
				}
				//should retrun 1 row here
				if (ERROR_SUCCESS != msiObj->GetNextDirectory (msiDirObj))
				{
					MessageBox(NULL,"Not able to get next directory", "Error", MB_OK|MB_ICONERROR );
					msiObj->CloseDirectoryTable();
					return E_FAIL;
				}

				msiDirObj->strDefault = msiDirObj->strDefault.SpanExcluding(".|:");
				
				if ( msiDirObj->strParent == _T("TARGETDIR"))
				{
					CString path;
					if (ERROR_SUCCESS != msiObj->GetTargetPath( msiDirObj->strDirectory, &path))
					{
						msiObj->CloseDirectoryTable();
						return E_FAIL;
					}
					
						path.Trim('\\');
						strFullPath = path + _T("\\") + strFullPath;
					
					break;
				}


				if (!msiDirObj->strDefault.IsEmpty())
					strFullPath = msiDirObj->strDefault + _T("\\") + strFullPath;
				strTemp = msiDirObj->strParent;
				msiObj->CloseDirectoryTable();

			}
			
			msiObj->CloseComponentTable();

			//now we have everything starting wrting this file node
			
			//adding file node
			MSXML2::IXMLDOMElementPtr fileNode = m_pDoc->documentElement;
			fileNode = m_pDoc->createElement(_T("File"));
			m_pDocRoot->appendChild(fileNode);

			//adding name node, including full path
			MSXML2::IXMLDOMElementPtr nameNode = m_pDoc->documentElement;
			nameNode = m_pDoc->createElement(_T("Name"));
			nameNode->text = (_bstr_t)strFullPath;
			fileNode->appendChild(nameNode);

			//ading version node
			MSXML2::IXMLDOMElementPtr versionNode = m_pDoc->documentElement;
			versionNode = m_pDoc->createElement(_T("Version"));
			versionNode->text = (_bstr_t)msiFileObj->strVersion;
			fileNode->appendChild(versionNode);

			//adinng condition node
			if (!msiCompObj->strCondition.IsEmpty())
			{
				MSXML2::IXMLDOMElementPtr conditionNode = m_pDoc->documentElement;
				conditionNode = m_pDoc->createElement(_T("Condition"));
				conditionNode->text= (_bstr_t)msiCompObj->strCondition;
				fileNode->appendChild(conditionNode);
			}



		}
		msiObj->CloseFileTable();
		msiObj->ClosePackage();

		hr = m_pDoc->save((_variant_t) xmlpath);
		if(FAILED(hr))
		{
			MessageBox(NULL,"Not able save xml!", "Error", MB_OK|MB_ICONERROR );
			return E_FAIL;
		}

	}
	catch(_com_error e)
	{
		CloseXMLDocument();
		return E_FAIL;
	}
	
	if( msiCompObj )
		delete msiCompObj;

	if( msiFileObj )
		delete msiFileObj;
	
	if(msiDirObj)
		delete msiDirObj;
	
	if( msiObj )
		delete msiObj;	

	CloseXMLDocument();
	return S_OK;

}
HRESULT CXMLHelper::CloseXMLDocument()
{
	if(m_pDoc)
		m_pDoc.Release();
	if(m_pDocRoot)
		m_pDocRoot.Release();
	if(m_pNodeList)
		m_pNodeList.Release();
	return S_OK;
}
