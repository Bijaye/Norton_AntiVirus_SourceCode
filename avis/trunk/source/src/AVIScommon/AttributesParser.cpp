// AttributesParser.cpp: implementation of the AttributesParser class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AttributesParser.h"

//#include "AVISException.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

using namespace std;


int AttributesParser::GetNextKeyValuePair(const std::string& string, int pos,
									std::string& key, std::string& value)
{
	int	begining	= string.find(":", pos);
	int	end			= std::string::npos;
	
	if (std::string::npos != begining)
	{
		key = string.substr(pos, begining - pos);
		begining = string.find_first_not_of(" \t", begining +1);
		if (std::string::npos == begining)		// on last item, no value
		{
			value = "";
			end		= string.size();
		}
		else if (std::string::npos != (end = string.find("\n", begining)) ||
			(end = string.size()))			// the last string may not have \n
		{
			if (end >= 1 && '\r' == string[end-1])
				value = string.substr(begining, (end - begining) - 1);
			else
				value = string.substr(begining, end - begining);
			++end;
		}
		else
		{
			std::string	errorMessage("Unable to parse attribute value pair (");

			errorMessage	+= pos;
			errorMessage	+= ")";
			end				= std::string::npos;
//			throw IcePackException(IcePackException::SampleValueParse, errorMessage);
		}
	}

	return end;
}

void AttributesParser::RightCase(std::string& key)
{
	bool	nextIsUpper = true;

	for (int i = 0; &key[i] < key.end(); i++)
	{
		if ('-' == key[i])
			nextIsUpper = true;
		else if (' ' == key[i] || ':' == key[i])
			key.resize(i);
		else
		{
			if (nextIsUpper)
			{
				if (islower(key[i]))
					key[i] = toupper(key[i]);
				nextIsUpper = false;
			}
			else
			{
				if (isupper(key[i]))
					key[i] = tolower(key[i]);
			}
		}
	}
}

//
//	This method is intended to be used when only looking for one attribute out of
//	a string of attributes (for example X-Error or X-Date-Blessed)
bool AttributesParser::ParseOneAttribute(const string& attrs, const string& searchKey,
										string& searchResult)
{
	bool	found = false;
	string	key, value;
	int	pos = 0;

	searchResult = "";
	while (string::npos != (pos = GetNextKeyValuePair(attrs, pos, key, value)))
	{
		RightCase(key);
		if (key == searchKey)
		{
			found	= true;
			searchResult	= value;
			break;
		}
	}

	return found;
}

//
//	This method is intended to be used when only looking for as set of attributes that
//	match the begining of the search Key (ie. X-Error*)

bool AttributesParser::ParseMultipuleAttributes(const string& attrs, const string& searchKey,
												string& searchResult)
{
	bool	found = false;
	string	key, value;
	int	pos = 0;

	searchResult = "";
	while (string::npos != (pos = GetNextKeyValuePair(attrs, pos, key, value)))
	{
		RightCase(key);
		if (!strncmp(key.c_str(), searchKey.c_str(), searchKey.size()))
		{
			found	= true;
			searchResult	+= value;
		}
	}

	return found;
}
