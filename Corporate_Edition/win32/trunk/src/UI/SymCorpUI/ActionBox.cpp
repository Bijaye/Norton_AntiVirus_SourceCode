#include "ActionBox.h"
#include "SymCorpUIDoc.h"
#include <algorithm>

ActionBox::ProblemInfo ActionBox::problemData[5] =
{
    {ActionBox::ProblemType_Off,                    1, ActionBox::BackgroundMode_Problem,   true},
    {ActionBox::ProblemType_Error,                  2, ActionBox::BackgroundMode_Problem,   true},
    {ActionBox::ProblemType_OldDefs,                3, ActionBox::BackgroundMode_Caution,   true},
    {ActionBox::ProblemType_QuarantineNetwork,      2, ActionBox::BackgroundMode_Problem,   true},
    {ActionBox::ProblemType_SecurityPolicyFail,     5, ActionBox::BackgroundMode_Problem,   true},
};

ActionBox::ActionBox() : currMode(Mode_Informational), currBackgroundMode(BackgroundMode_Good)
{
    // No code needed
    /*
    ProblemListGreaterRank[ProblemType_Off]                  = 10;
    ProblemListGreaterRank[ProblemType_Error]                = 20;
    ProblemListGreaterRank[ProblemType_OldDefs]              = 30;
    ProblemListGreaterRank[ProblemType_QuarantineNetwork]    = 40;
    ProblemListGreaterRank[ProblemType_SecurityPolicyFail]   = 50;
    */
}

ActionBox::~ActionBox()
{
    // Nothing for now
}

void ActionBox::DoDataExchange( CDataExchange* pDX )
{
    DDX_Control(pDX, IDC_STATUSVIEW_ACTIONBOX, background);
    DDX_Control(pDX, IDC_STATUSVIEW_ACTIONBOX_TITLE, title);
    DDX_Control(pDX, IDC_STATUSVIEW_ACTIONBOX_DESCRIPTION, description);
    DDX_Control(pDX, IDC_STATUSVIEW_ACTIONBOX_ACTIONBUTTON, actionButton);

    DDX_Control(pDX, IDC_STATUSVIEW_ACTIONBOX_UPCOMING_TITLE, upcomingTitle);
    DDX_Control(pDX, IDC_STATUSVIEW_ACTIONBOX_UPCOMING_DESCRIPTION, upcomingDescription);
    DDX_Control(pDX, IDC_STATUSVIEW_ACTIONBOX_UPCOMING_BUTTON, upcomingButton);
}

void ActionBox::OnUpdate()
// Called whenever the caller wants us to rediscover problems and update our display
{
    HRESULT returnValHR = E_FAIL;

    // Nothing for now...
    returnValHR = GetProblemList(&currProblems);

    if (currProblems.size())
    {
        SetMode(Mode_Informational, BackgroundMode_Good, goodTitle, _T(""));

        // Determine if there are any scans coming up
        SetInformationalUpcomingText(_T(""), _T(""), false);
    }
    else
    {
        // Sorts the list by problem type, then provider
        // ProblemRank is the rank to assign to each problem type, where the index in the vector
        // equals the ProblemType enum value and the value at that index is the rank to use
        // Problem rank sort is ascending, so lower ranked values appear first in the sorted list
        // Sorts the problem list by rank
        sort(currProblems.begin(), currProblems.end(), ProblemListGreater);

        // Display only the most severe problem for now
    }
}

void ActionBox::OnInitialUpdate()
{
    DWORD startTime = GetTickCount();
    // Initialize Action Button
    actionButton.LoadImages(IDR_NAVBAR_BUTTON_NORMAL, IDR_NAVBAR_BUTTON_NORMALSELECTED, IDR_NAVBAR_BUTTON_PUSHED, IDR_NAVBAR_BUTTON_ROLLOVER, CButtonEx::ScaleMode_StretchToFit);
    actionButton.defaultFont.LoadFromString(IDS_STATUS_ACTIONBOX_BAD_FIXBUTTON_FONT);

    // Initialize control-over-control transparency
    title.SetBackgroundControl(&background);
    description.SetBackgroundControl(&background);
    actionButton.SetBackgroundControl(&background);
    upcomingTitle.SetBackgroundControl(&background);
    upcomingDescription.SetBackgroundControl(&background);
    upcomingButton.SetBackgroundControl(&background);

    // Action box title/text fonts
    titleFontGood.LoadFromString(IDS_STATUS_ACTIONBOX_GOOD_TITLE_FONT);
    titleFontCaution.LoadFromString(IDS_STATUS_ACTIONBOX_CAUTION_TITLE_FONT);
    titleFontBad.LoadFromString(IDS_STATUS_ACTIONBOX_BAD_TITLE_FONT);
    descriptionFontGood.LoadFromString(IDS_STATUS_ACTIONBOX_GOOD_TEXT_FONT);
    descriptionFontCaution.LoadFromString(IDS_STATUS_ACTIONBOX_CAUTION_TEXT_FONT);
    descriptionFontBad.LoadFromString(IDS_STATUS_ACTIONBOX_BAD_TEXT_FONT);
    
    // Good state upcoming information
    upcomingTitle.fontInfo.LoadFromString(IDS_STATUS_ACTIONBOX_UPCOMING_TITLE_FONT);
    upcomingDescription.fontInfo.LoadFromString(IDS_STATUS_ACTIONBOX_UPCOMING_DESCRIPTION_FONT);
    upcomingButton.defaultFont.LoadFromString(IDS_STATUS_ACTIONBOX_UPCOMING_BUTTON_FONT);

    // Default to BackgroundMode_Good / Mode_Informational
    background.LoadImage(IDR_STATUS_ACTIONBOX_GOOD, CStaticEx::ScaleMode_FitKeepAspectRatio, _T("BINARY"), true, COLOR_WHITE);
    title.fontInfo = titleFontGood;
    description.fontInfo = descriptionFontBad;
}

void ActionBox::SetMode( Mode newMode, BackgroundMode newBackground, CString newActionTitle, CString newActionDescription )
{
    currMode = newMode;
    currBackgroundMode = newBackground;
 
    // Reconfigure the background
    switch (currBackgroundMode)
    {
    case BackgroundMode_Good:
        background.LoadImage(IDR_STATUS_ACTIONBOX_GOOD, CStaticEx::ScaleMode_FitKeepAspectRatio, _T("BINARY"), true, COLOR_WHITE);
        title.fontInfo = titleFontGood;
        description.fontInfo = descriptionFontGood;
        break;
    case BackgroundMode_Caution:
        title.fontInfo = titleFontCaution;
        background.LoadImage(IDR_STATUS_ACTIONBOX_CAUTION, CStaticEx::ScaleMode_FitKeepAspectRatio, _T("BINARY"), true, COLOR_WHITE);
        description.fontInfo = descriptionFontCaution;
        break;
    case BackgroundMode_Problem:
    default:
        title.fontInfo = titleFontBad;
        background.LoadImage(IDR_STATUS_ACTIONBOX_BAD, CStaticEx::ScaleMode_FitKeepAspectRatio, _T("BINARY"), true, COLOR_WHITE);
        description.fontInfo = descriptionFontBad;
        break;
    }
    title.SetWindowText(newActionTitle);
    description.SetWindowText(newActionDescription);
    background.ShowWindow(SW_SHOWNORMAL);
    title.ShowWindow(SW_SHOWNORMAL);
    description.ShowWindow(SW_SHOWNORMAL);
    
    // Display button as appropriate    
    if (currMode == Mode_Informational)
    {
        actionButton.ShowWindow(SW_HIDE);
    }
    else
    {
        actionButton.ShowWindow(SW_SHOWNORMAL);
        // Don't show Upcoming information in ActionRequested mode
        SetInformationalUpcomingText(_T(""), _T(""), false);
    }
}

void ActionBox::SetActionRequestedButton( bool showButton, CString newButtonText )
{
    if (showButton)
    {
        actionButton.SetWindowText(newButtonText);
        actionButton.ShowWindow(SW_SHOWNORMAL);
    }
    else
    {
        actionButton.ShowWindow(SW_HIDE);
        actionButton.SetWindowText(_T(""));
    }
}

void ActionBox::SetInformationalUpcomingText( CString title, CString newDescription, bool showButton, CString newButtonText )
{
    if ((title == "") && (newDescription == "") && (!showButton))
    {
        upcomingTitle.ShowWindow(SW_HIDE);
        upcomingDescription.ShowWindow(SW_HIDE);
        upcomingButton.ShowWindow(SW_HIDE);
    }
    else
    {
        upcomingTitle.SetWindowText(title);
        upcomingDescription.SetWindowText(newDescription);
        upcomingTitle.ShowWindow(SW_SHOWNORMAL);
        upcomingDescription.ShowWindow(SW_SHOWNORMAL);
        if (showButton)
        {
            upcomingButton.SetWindowText(newButtonText);
            upcomingButton.ShowWindow(SW_SHOWNORMAL);
        }
        else
        {
            upcomingButton.ShowWindow(SW_HIDE);
            upcomingButton.SetWindowText(_T(""));
        }
    }
}

HRESULT ActionBox::GetProblemList( ActionBox::ProblemList* problems )
// Sets *problems equal to the list of problems found, in no particular order
{
    CWnd*           mainWindow = NULL;
    CFrameWnd*      mainFrame = NULL;
    CSymCorpUIDoc*  currDocument = NULL;

    ProtectionProviderLib::ProtectionProviderList::iterator     currProtectionProvider;
    ProtectionProviderLib::IProtectionPtr                       currProviderProtection;
    VARIANT_BOOL                                                protectionConfiguration     = ProtectionProviderLib::ProtectionStatus_Error;
    ProtectionProviderLib::ProtectionStatus                     protectionStatus            = ProtectionProviderLib::ProtectionStatus_Error;
    ProtectionProviderLib::IProtection_Container*               autoprotects                = NULL;
    ProtectionProviderLib::IEnumProtection*                     autoprotectsEnumerator      = NULL;
    ProtectionProviderLib::IProtection*                         currAP                      = NULL;
    ProtectionProviderLib::IProtection_DefinitionInfoPtr        currProviderDefInfo;
    unsigned long int                                           noRetrieved                 = 0;
    GUID                                                        providerID                  = GUID_NULL;
    GUID                                                        autoprotectID               = GUID_NULL;
    bool                                                        protectionRequiredToBeOn    = false;
    Problem                                                     newProblem;
    HRESULT                                                     returnValHR                 = E_NOTIMPL;

    // Validate parameters
    if (problems == NULL)
        return E_POINTER;

    // Get the active document
    mainWindow = AfxGetMainWnd();
    mainFrame = DYNAMIC_DOWNCAST(CFrameWnd, mainWindow);
    currDocument = DYNAMIC_DOWNCAST(CSymCorpUIDoc, mainFrame->GetActiveDocument());
    if (currDocument == NULL)
        return S_FALSE;

    // Scan for protection technologies off or in error
    problems->clear();
    for (currProtectionProvider = currDocument->protectionProviders.begin(); currProtectionProvider != currDocument->protectionProviders.end(); currProtectionProvider++)
    {
        // Initialize
        currProviderProtection = *currProtectionProvider;
        (*currProtectionProvider)->get_ID(&providerID);
        currProviderProtection->get_ProtectionConfiguration(&protectionConfiguration);
        currProviderProtection->get_ProtectionStatus(&protectionStatus);
        protectionRequiredToBeOn = GetProtectionShouldBeOn(providerID);

        // Is this provider required to be ON?
        if (protectionRequiredToBeOn)
        {
            // Yes.  Report a problem if not ON
            if (protectionStatus == ProtectionProviderLib::ProtectionStatus_Off)
            {
                newProblem.type = ProblemType_Off;
                newProblem.protectionProvider = *currProtectionProvider;
                newProblem.specificAutoprotect = NULL;
                try
                {
                    problems->push_back(newProblem);
                }
                catch (std::bad_alloc&)
                {
                    returnValHR = E_OUTOFMEMORY;
                }
            }
            else if (protectionStatus == ProtectionProviderLib::ProtectionStatus_Error)
            {
                newProblem.type = ProblemType_Error;
                newProblem.protectionProvider = *currProtectionProvider;
                newProblem.specificAutoprotect = NULL;
                try
                {
                    problems->push_back(newProblem);
                }
                catch (std::bad_alloc&)
                {
                    returnValHR = E_OUTOFMEMORY;
                }
            }
        }

        // Next, check the autoprotects of this provider to see if any of them are required
        // to be on.
        returnValHR = (*currProtectionProvider)->get_Autoprotects(&autoprotects);
        if (SUCCEEDED(returnValHR))
        {
            returnValHR = autoprotects->get__NewEnum(&autoprotectsEnumerator);
            if (SUCCEEDED(returnValHR))
            {
                returnValHR = autoprotectsEnumerator->RemoteNext(1, &currAP, &noRetrieved);
                while (returnValHR == S_OK)
                {
                    currAP->get_ProtectionStatus(&protectionStatus);
                    currAP->get_ID(&autoprotectID);

                    protectionRequiredToBeOn = GetProtectionShouldBeOn(providerID, autoprotectID);

                    // Is this autoprotect required to be ON?
                    if (protectionRequiredToBeOn)
                    {
                        // Yes.  Report a problem if not ON
                        if (protectionStatus == ProtectionProviderLib::ProtectionStatus_Off)
                        {
                            newProblem.type = ProblemType_Off;
                            newProblem.protectionProvider = *currProtectionProvider;
                            newProblem.specificAutoprotect = currAP;
                            try
                            {
                                problems->push_back(newProblem);
                            }
                            catch (std::bad_alloc&)
                            {
                                returnValHR = E_OUTOFMEMORY;
                            }
                        }
                        else if (protectionStatus == ProtectionProviderLib::ProtectionStatus_Error)
                        {
                            newProblem.type = ProblemType_Error;
                            newProblem.protectionProvider = *currProtectionProvider;
                            newProblem.specificAutoprotect = currAP;
                            try
                            {
                                problems->push_back(newProblem);
                            }
                            catch (std::bad_alloc&)
                            {
                                returnValHR = E_OUTOFMEMORY;
                            }
                        }
                    }

                    currAP->Release();
                }
                autoprotectsEnumerator->Release();
                autoprotectsEnumerator = NULL;
            }
            autoprotects->Release();
        }

        // Are there old defs?
        if (GetAreProviderDefsOld(*currProtectionProvider))
        {
            newProblem.type = ProblemType_OldDefs;
            newProblem.protectionProvider = *currProtectionProvider;
            newProblem.specificAutoprotect = NULL;
        }

        // Determining ProblemType_QuarantineNetwork and ProblemType_SecurityPolicyFail will
        // require specific interactions with those providers

        currProviderProtection->Release();
    }
    return returnValHR;
}

bool ActionBox::GetProtectionShouldBeOn( GUID providerID, GUID autoprotectID )
// Returns TRUE if the specified provider or provider's autoprotect should be ON,
// else FALSE
{
    // TBD once the XML is defined and I know where this will be stored
    return true;
}

bool ActionBox::GetAreProviderDefsOld( ProtectionProviderLib::IProtection_Provider* protectionProvider )
// Returns TRUE if the specified provider has defs, a policy is configured for them, and
// they are out of date according to that policy, else FALSE
{
    // TBD once the XML is defined and I know where this will be stored
    return false;
}


bool ActionBox::ProblemListGreater( ActionBox::Problem elementA, ActionBox::Problem elementB )
// Return whether first element is greater than the second
// Sort by type, then provider
{
    return true;
    /*
    if (ProblemListGreaterRank[elementA.type] > ProblemListGreaterRank[elementB.type])
        return true;
    else if (ProblemListGreaterRank[elementA.type] == ProblemListGreaterRank[elementB.type])
        return elementA.protectionProvider > elementB.protectionProvider;
    else
        return false;
        */
}