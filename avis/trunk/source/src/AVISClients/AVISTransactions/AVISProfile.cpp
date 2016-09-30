// AVISProfile.cpp: implementation of the CAVISProfile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AVISTransactions.h"
#include "AVISProfile.h"

#include "utilexception.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

ConfigProfile* CAVISProfile::s_pCfgProf = NULL;

/*****/
BOOL CAVISProfile::OpenProfile(LPCSTR pszConfigProfileName)
{
	try
	{
		s_pCfgProf = new ConfigProfile(pszConfigProfileName);
	}
	catch (BadFilename&)
	{
		CAVISTransactionsApp::s_pTc->critical("\"BadFilename\" exception occured while creating ConfigProfile object.");
		return FALSE;
	}

	if (NULL == s_pCfgProf)
	{
		CAVISTransactionsApp::s_pTc->critical("Unable to create \"ConfigProfile\" object; exiting...");
		return FALSE;
	}

	return TRUE;
}
/*****/
void CAVISProfile::CloseProfile()
{
	delete s_pCfgProf;
	s_pCfgProf = NULL;
}
/*****/
std::string CAVISProfile::GetStringValue(LPCSTR pszKey, const std::string& strDefault)
{
	std::string strResult;
	try
	{
		// get the value associated with the key "pszKey" from the configuration profile
		strResult = (*s_pCfgProf)[pszKey];
		CAVISTransactionsApp::s_pTc->debug("Retrieved \"%s\" key string value of \"%s\".", pszKey, strResult.c_str());
	}
	catch (InvalidConfigVariable&)
	{
		// couldn't find the value; return the default
		CAVISTransactionsApp::s_pTc->warning("No string value found for \"%s\" key; using default of \"%s\".", pszKey, strDefault.c_str());
		strResult = strDefault;
	}

	return strResult;
}
/*****/
int CAVISProfile::GetIntValue(LPCSTR pszKey, int iDefault)
{
	int iResult;
	try
	{
		// get the value associated with the key "pszKey" from the configuration profile
		std::string strValue = (*s_pCfgProf)[pszKey];
		iResult = ::atoi(strValue.c_str());
		CAVISTransactionsApp::s_pTc->debug("Retrieved \"%s\" key int value of \"%d\".", pszKey, iResult);
	}
	catch (InvalidConfigVariable&)
	{
		// couldn't find the value; return the default
		CAVISTransactionsApp::s_pTc->warning("No int value found for \"%s\" key; using default of \"%d\".", pszKey, iDefault);
		iResult = iDefault;
	}

	return iResult;
}
/*****/
BOOL CAVISProfile::IsValueEnabled(LPCSTR pszKey, BOOL boDefault)
{
	int boResult;
	try
	{
		// get the value associated with the key "pszKey" from the configuration profile
		std::string strValue = (*s_pCfgProf)[pszKey];
		boResult = ((strValue == "on") || (strValue == "1") || 
					(strValue == "enabled") || (strValue == "true") ||
					(strValue == "yes"));
		CAVISTransactionsApp::s_pTc->debug("Retrieved \"%s\" key BOOL value of \"%s\".", pszKey, boResult ? "TRUE" : "FALSE");
	}
	catch (InvalidConfigVariable&)
	{
		// couldn't find the value; return the default
		CAVISTransactionsApp::s_pTc->warning("No BOOL value found for \"%s\" key; using default of \"%s\".", pszKey, boDefault ? "TRUE" : "FALSE");
		boResult = boDefault;
	}

	return boResult;
}
