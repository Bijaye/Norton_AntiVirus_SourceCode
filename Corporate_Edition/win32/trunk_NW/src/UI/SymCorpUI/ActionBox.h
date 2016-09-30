#ifndef SYMCORPUI_ACTIONBOX
#define SYMCORPUI_ACTIONBOX

#include "stdafx.h"
#include "StaticEx.h"
#include "ButtonEx.h"
#define PROTECTIONPROVIDER_HELPERTYPES_WITHNAMESPACE
#import "ProtectionProvider.tlb" raw_interfaces_only exclude("wireHWND", "_RemotableHandle", "__MIDL_IWinTypes_0009")
#include "ProtectionProvider.h"
#include <vector>

// Handles the ActionBox.  The ActionBox is the top piece of the status view
// and consists of several controls that work together to present current status information to
// the user.  The action box has 3 modes, described below
//      Good            All OK
//                      Also supports a "minor mode" showing upcoming scans to the right
//      Caution         Something needs attention but not broken
//                      Includes a fix button
//      Bad             Something's bad
//                      Includes a fix button
// 
// The behavior of the box is slightly different in each mode.
//
// To use:
// 1.  Instantiate in your view class
// 2.  Include calls to DoDataExchange and OnUpdate in the corresponding View member functions
class ActionBox
{
public:
    // ** DATA TYPES **
    // Mode of operation for the Action Box
    typedef enum {Mode_Informational, Mode_ActionRequested} Mode;
    // Type of background to display
    typedef enum {BackgroundMode_Good, BackgroundMode_Caution, BackgroundMode_Problem} BackgroundMode;

    // Type of problem found
    typedef enum {ProblemType_Off, ProblemType_Error, ProblemType_OldDefs, ProblemType_QuarantineNetwork, ProblemType_SecurityPolicyFail} ProblemType;
    // A problem record
    struct Problem
    {
        ProblemType                                         type;                   // Type of problem
        ProtectionProviderLib::IProtection_Provider*        protectionProvider;     // Protection provider with the problem
        ProtectionProviderLib::IProtection*                 specificAutoprotect;    // If Off or Error, the specific autoprotect with the problem
    };
    typedef std::vector<Problem>    ProblemList;
    typedef std::vector<int>        IntVector;
    // Describes a problem
    struct ProblemInfo
    {
        ProblemType     problemType;
        DWORD           sortRank;
        BackgroundMode  background;
        bool            showFixButton;
    };
    static ProblemInfo problemData[5];


    // ** FUNCTION MEMBERS **
    // Box Appearance and Content
    void SetMode( Mode newMode, BackgroundMode newBackground, CString newActionTitle, CString newActionDescription );
    void SetActionRequestedButton( bool showButton, CString newButtonText );
    void SetInformationalUpcomingText( CString newTitle, CString newDescription, bool showButton = false, CString newButtonText = _T("") );

    // Problems used to determine box appearance and content
    HRESULT GetProblemList( ProblemList* problems );
    // Returns TRUE if the admin has configured a policy that the specified protection provider
    // or provider's autoprotect should be ON, else FALSE
    bool GetProtectionShouldBeOn( GUID providerID, GUID autoprotectID = GUID_NULL );
    // Returns TRUE if the admin has configured a policy for the specified provider stating
    // that it's defs must be newer than a defined value and that provider has defs, else
    // FALSE
    bool GetAreProviderDefsOld( ProtectionProviderLib::IProtection_Provider* protectionProvider );
    HRESULT SortProblemList( std::vector<int> ProblemRank, ProblemList listToSort, ProblemList* sortedList );

    // Constructor-destructor
    ActionBox();
    ~ActionBox();

    // MFC support
    void DoDataExchange( CDataExchange* pDX );
    // Called whenever the caller wants us to rediscover problems and update our display
    void OnUpdate();
    void OnInitialUpdate();
private:
    CStaticEx                   background;
    CStaticEx                   title;
    CStaticEx                   description;
    CButtonEx                   actionButton;
    CStaticEx                   upcomingTitle;
    CStaticEx                   upcomingDescription;
    CButtonEx                   upcomingButton;

    DialogFont                  titleFontGood;
    DialogFont                  titleFontCaution;
    DialogFont                  titleFontBad;
    DialogFont                  descriptionFontGood;
    DialogFont                  descriptionFontCaution;
    DialogFont                  descriptionFontBad;

    CString                     goodTitle;
    
    Mode                        currMode;
    BackgroundMode              currBackgroundMode;
    ProblemList                 currProblems;

    std::vector<int>            DefaultProblemRanking;

    // used by SortProblemList
    // Return whether first element is greater than the second
    // Sort by type, then provider
    static bool ProblemListGreater( ActionBox::Problem elementA, ActionBox::Problem elementB );
};

#endif // SYMCORPUI_ACTIONBOX