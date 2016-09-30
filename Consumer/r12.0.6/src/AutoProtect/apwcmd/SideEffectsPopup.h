#pragma once

#include "AlertUI.h"

class CSideEffectsPopup : public CAlertUI
{
public:
    CSideEffectsPopup(void);
    virtual ~CSideEffectsPopup(void);

    int DoModal ();

    enum results
    {
        RESULT_FAIL = -1,
        RESULT_SCAN,
        RESULT_IGNORE
    };

    struct DATA
    {
        unsigned long ulVirusID;    // Used for side-effects, only
        std::wstring strThreatName; // Virus name
    };

    typedef std::vector <DATA> vecData;
    typedef vecData::iterator iterVecData;

    std::vector <DATA> vecSideEffects;
};
