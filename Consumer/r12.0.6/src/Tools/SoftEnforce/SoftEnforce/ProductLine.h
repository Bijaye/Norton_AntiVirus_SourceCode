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
