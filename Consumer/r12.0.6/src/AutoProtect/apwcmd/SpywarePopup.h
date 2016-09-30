#pragma once

#include "AlertUI.h"

class CSpywarePopup : public CAlertUI
{
public:
    CSpywarePopup(void);
    virtual ~CSpywarePopup(void);

    int DoModal ();

    enum results
    {
        RESULT_FAIL = -1,
        RESULT_SCAN = 0,
        RESULT_EXCLUDE,
        RESULT_IGNORE
    };

    std::wstring strThreatName;
    std::string strThreatCats;
    unsigned long ulVirusID;
};
