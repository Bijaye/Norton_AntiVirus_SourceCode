// AttributesParser.h: interface for the AttributesParser class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ATTRIBUTESPARSER_H__BBA86ED3_0995_11D3_ADE1_00A0C9C71BBC__INCLUDED_)
#define AFX_ATTRIBUTESPARSER_H__BBA86ED3_0995_11D3_ADE1_00A0C9C71BBC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include "AVIScommon.h"

class AVISCOMMON_API AttributesParser  
{
public:
	static int GetNextKeyValuePair(const std::string& string, int pos,
									std::string& key, std::string& value);
	static void RightCase(std::string& key);

	//
	//	This method is intended to be used when only looking for one attribute out of
	//	a string of attributes (for example X-Error or X-Date-Blessed)
	static bool ParseOneAttribute(const std::string& attr, const std::string& key,
									std::string& value);

//
//	This method is intended to be used when only looking for as set of attributes that
//	match the begining of the search Key (ie. X-Error*)

	static bool ParseMultipuleAttributes(const std::string& attrs, const std::string& searchKey,
											std::string& searchResult);

private:
	AttributesParser() {};
	virtual ~AttributesParser() {};

};

#endif // !defined(AFX_ATTRIBUTESPARSER_H__BBA86ED3_0995_11D3_ADE1_00A0C9C71BBC__INCLUDED_)
