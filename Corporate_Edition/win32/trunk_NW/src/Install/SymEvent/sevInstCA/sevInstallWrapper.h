// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#pragma once

#include <windows.h>
#include "msi.h"

#include "msiwrapper.h"

/////////////////////////////////////////////////////////////////////////////
// CSevInstallWrapper
// See CSevInstallWrapper.cpp for the implementation of this class
//

class CSevInstallWrapper : public CMSIWrapper
{
public:
	CSevInstallWrapper();
	CSevInstallWrapper( MSIHANDLE );
	CSevInstallWrapper( CString );
	~CSevInstallWrapper();

	// General 
	UINT openTable( CString strQuery = "" );
	UINT closeTable();
	UINT getNext();
	int count();

	// Put/Set methods for table data
	inline CString getID() { return m_strID; } ;
	inline void putID( CString strIn ) { m_strID = strIn; };

	inline CString getInstCmd() { return m_strInstCmd; } ;
	inline void putInstCmd( CString strIn ) { m_strInstCmd = strIn; };

	inline CString getRmveCmd() { return m_strRmveCmd; } ;
	inline void putRmveCmd( CString strIn ) { m_strRmveCmd = strIn; };

	inline CString getDivision() { return m_strDivision; } ;
	inline void putDivision( CString strIn ) { m_strDivision = strIn; };

	CString	m_strID;
private:
	PMSIHANDLE m_hTable;

	// Table data
	CString	m_strInstCmd;
	CString	m_strRmveCmd;
	CString	m_strDivision;
};
