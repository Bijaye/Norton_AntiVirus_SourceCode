//===========================================================================
//
// $Header:   S:/ISNAVNT/VCS/trialon.h_v   1.4   13 Aug 1998 12:05:24   mdunn  $
//
// Description: NAV trial version settings here
//
//===========================================================================
// $Log:   S:/ISNAVNT/VCS/trialon.h_v  $
// 
//    Rev 1.4   13 Aug 1998 12:05:24   mdunn
// Added #define for TRIAL_LENGTH_SWITCH
//
//    Rev 1.3   01 Jul 1998 20:24:24   VGLOSTE
// Added new constants required for trialware.
//
//    Rev 1.2   01 Aug 1997 12:31:48   JBRENNA
// Remove the Fixed End Date trial expiration.
//
//    Rev 1.1   15 Jul 1997 08:35:08   JBRENNA
// Update fix experation to 9/22/97.
//
//    Rev 1.0   11 Jul 1997 14:33:52   jtaylor
// Initial revision.
//===========================================================================

#define NAV_TRIAL_VERSION
#define TRIAL_NUM_DAYS   "30"
#define TRIAL_OVERRIDE_KEY "SOFTWARE\\Symantec\\SymSE"
#define TRIAL_OVERRIDE_VAL "NAVTrialLength"
#define TRIAL_LENGTH_SWITCH     "TrialType"

// Set to "MM/DD/YY" format to enable. Set to "0" to disable.
#define TRIAL_EXPIRATION_DATE "0"

