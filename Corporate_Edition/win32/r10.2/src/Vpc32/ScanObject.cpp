// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//----------------------------------------------------------------
//
//  File: ScanObjCpp
//  Purpose: ScanObject Implementation file
//
//	Date: 1-20-97
//
//	Owner: Ken Knapton
//
//	***** Intel Corp. Confidential ******
//----------------------------------------------------------------
#include "stdafx.h"
#include "ScanObject.h"
#include "resource.h"
#include "MainFrm.h"
#include "rpc.h"
#include "ClientReg.h"			//Client registry keys/values
#include "SaveScriptedTask.h"	//Save Scripted Task dialog
#include "TaskPadView.h"
#include "wprotect32.h"

#if !defined (CLIENTUSER)
#define CLIENTUSER
#endif

//#include "PScan.h"

//---------------------------------------------------------------
//
// CScanObject class
//
//---------------------------------------------------------------
IMPLEMENT_SERIAL( CScanObject, CObject, 0x1001 );

//---------------------------------------------------------------
// Constructor
//---------------------------------------------------------------
CScanObject::CScanObject()
{
	//Set the default values
	m_strPath.Empty();
	m_strSpec.Empty();
	m_pathType = typeFolder;
}

//---------------------------------------------------------------
// Destructor
//---------------------------------------------------------------
CScanObject::~CScanObject()
{
}

//---------------------------------------------------------------
// GetFullScanPath - return the full path & filename
//---------------------------------------------------------------
void CScanObject::GetScanFullPath( CString &strPath )
{
	//If it is a folder, then append the spec
	if( m_pathType == typeFolder )
	{
		//Now, determine if it has a trailing slash
		if( m_strPath.Right( 1 ) != '\\' )	
		{
		 	m_strPath += "\\";
		}                     
		
		//Now, set the return string to the path & filespec
		strPath = m_strPath + m_strSpec;
	}	
	else //Otherwise, return the filename as is
	{
	 	strPath = m_strPath;
	}
}


void CScanObject::Serialize( CArchive &ar )
{
	//Give the base class it's chance at it
	CObject::Serialize( ar );

	if( ar.IsStoring() )
	{
		ar << m_strPath << m_strSpec << (int)m_pathType;
	}
	else
	{
		int iType;

		ar >> m_strPath >> m_strSpec >> iType;
		m_pathType = (scanPathType)iType;
	}

}
