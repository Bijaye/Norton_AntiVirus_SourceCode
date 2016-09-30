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
	void LoadData();
	void EditData();
	void SaveData();
};
