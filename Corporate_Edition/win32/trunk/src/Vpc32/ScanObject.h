// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//----------------------------------------------------------------
//
//  File: ScanObject.h
//  Purpose: CScanObject definition file
//
//	Date: 1-20-97
//
//	Owner: Ken Knapton
//
//	***** Intel Corp. Confidential ******
//
//----------------------------------------------------------------
#if!defined (SCANOBJECT_INCLUDED)
#define SCANOBJECT_INCLUDED

#include "LDVPTaskFS.h"

//-------------------------------------------------------------
//
// CScanObject class
//
//-------------------------------------------------------------
class CScanObject : public CObject
{   
	DECLARE_SERIAL(CScanObject);

public:
	enum scanPathType { typeFile, typeFolder };

private:
	CString	m_strPath;
	CString m_strSpec;

			
public:
	CScanObject();
	virtual ~CScanObject();
	             
	scanPathType m_pathType;

	//Set methods	             
	void SetScanPath(CString strPath ){ m_strPath = strPath;};
	void SetScanSpec(CString strSpec ){ m_strSpec = strSpec; };

	// Get methods
	void GetScanPath(CString& strPath ){ strPath = m_strPath;};
	void GetScanSpec(CString& strSpec ){ strSpec = m_strSpec; };

	void GetScanFullPath( CString &strPath );

	void Serialize( CArchive &ar );

};

#endif
