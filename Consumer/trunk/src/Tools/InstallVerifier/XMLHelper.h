////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#import <msxml3.dll>
#include "MSIWrapper.h"

class CXMLHelper
{
public:
	CXMLHelper(void);
	~CXMLHelper(void);
	HRESULT MSIToXML(CString msipath, CString xmlpath);

protected:
	MSXML2::IXMLDOMDocumentPtr m_pDoc;
	MSXML2::IXMLDOMElementPtr m_pDocRoot;
	MSXML2::IXMLDOMNodeListPtr m_pNodeList;
	
	HRESULT CloseXMLDocument();
	

};
