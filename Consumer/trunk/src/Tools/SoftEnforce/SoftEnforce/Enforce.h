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
#include "ProductLine.h"

class Enforce
{
public:
	Enforce(void);
	~Enforce(void);

	CString m_strName;
	std::vector <CString> m_vProduct;
	std::vector <std::vector <CString> > m_vData;

	bool SoftCheck(LPCTSTR subkey);
	bool CreateKeys(LPCTSTR subkey);
	bool SetValues(LPCTSTR subKey);
	void LoadData();
	void EditData();
	void SaveData();
};
