////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <vector>

class ProductLine
{
public:
	CString m_name;
	std::vector <CString> m_vKeys;
	
	ProductLine(void);
	~ProductLine(void);
};
