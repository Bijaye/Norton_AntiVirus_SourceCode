// VirusAlertPopup.h: interface for the CVirusAlertPopup class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "ResourceHelper.h"
#include "AlertUI.h"

class CVirusAlertPopup : public CAlertUI
{
public:
    CVirusAlertPopup(void);
	virtual ~CVirusAlertPopup();

    enum results
    {
        RESULT_FAIL = 0,
        RESULT_OK,
        RESULT_IGNORE_CONTAINER
    };

// Call this!
    int DoModal (long lIndex = 0 /*for containers*/);

    struct DATA
    {
        unsigned long ulVirusID;
        std::wstring sObjectName, sVirusName;
        std::vector <BYTE> vecActions;
	    long lType;
    };

    std::vector<DATA> vecData; // For containers
    std::wstring sContainerName;

private:
    typedef std::vector <std::wstring> vecStrings;
    void composeStrings(DATA& alertdata, vecStrings& vecsActions, std::wstring& strResultDesc);
};
