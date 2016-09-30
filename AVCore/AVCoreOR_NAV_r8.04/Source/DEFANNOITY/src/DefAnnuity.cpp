// VGLOSTER 5/18/1999
// This is a completely rewritten version of this file for use
// with the Venice phase II subscription management module

#include "stdafx.h"
#include "DefAnnty.h"
#include "SubWizard.h"
#include "SessProc.h"
#include "LSLib.h"	// Venice subscription module Client Helper Library

#define DEFANNUITYAPI __declspec(dllexport)

#include "DefAnnuity.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//------------------------------------------------------------------------------
//
// Define constants.
//
//------------------------------------------------------------------------------
#define DEFANNUITY_SUBSCRIPTION_LENGTH 366L
#define DEFANNUITY_SUBSCRIPTION_WARNING 60L
#define MAX_SUBS_ID_VALUE 255

//------------------------------------------------------------------------------
//
// Define registry keys.
//
// szDefAnnuityHive contains the name of the hive where the def annuity will
// keep its status.
//
// szDefAnnuityVersion is a DWORD value, which contains the version of the
// NAV product managing the subscriptions.
//
// szDefAnnuityEnabled is a DWORD value, which can be set up by external
// applications to disable the def annuity module.  If its value is 0, then
// the module will act as if the user is subscribed forever.  If its value is
// non-0, or if the value does not exist, then all checks will be done.
//
// szDefAnnuityConfirmationNumber is a DWORD value, which can be set up by
// external applications to disable the request for confirmation number when
// subscribing for definitions.  If its value is 0, then the module will not
// require a confirmation number.  If its value is not 0, or if the value is
// missong, then confirmation number will be required.
//
// szDefAnnuityFreeLength is a DWORD value, which defines the length in days
// of the free subscription period.  If this value is not present when the
// checks are performed, a free period of DEFANNUITY_FREE_LENGTH days will be
// used.
//
// szDefAnnuitySubscriptionLength is a DWORD value, which defines the length
// in days of a single subscription.  Whenever a user subscribes, the
// subscription length will be increased by this value.  If the value is not
// present, the subscription will be increased by
// DEFANNUITY_SUBSCRIPTION_LENGTH days.
//
// szDefAnnuityFreeEndWarning is a DWORD value, which can be set up by external
// application to tell the module how many days prior to the expiration of the
// expiration of the free period to start warning.  If this value is not
// present, the warning will start appearing DEFANNUITY_FREE_WARNING days prior
// to the expiration of the free period.
//
// szDefAnnuitySubscriptionEndWarning is a DWORD value, which can be set up by
// external application to tell this module how many days prior to the
// expiration of the subscription period to start warning.  If this value is not
// present, the warning will start appearing DEFANNUITY_SUBSCRIPTION_WARNING
// days before the subscription period expires.
//
// szDefAnnuityFreeStart is a DWORD value where we keep the install date in our
// own format (decimal view of the DWORD would look like YYYYMMDD).
//
// szDefAnnuitySubscriptionStart is a DWORD value where we keep the last date
// when a user has subscribed.  This value is not used, is is kept around for
// giggles.
//
// szDefAnnuitySubscription is a DWORD value where we keep the total length of
// subscriptions in days.
//
//------------------------------------------------------------------------------
static TCHAR szDefAnnuityHive[]                   = _T("SOFTWARE\\Symantec\\Norton AntiVirus\\DefAnnuity");

static TCHAR szDefAnnuityVersion[]                = _T("Version");
static TCHAR szDefAnnuityEnabled[]                = _T("Enabled");
static TCHAR szDefAnnuitySubscriptionLength[]     = _T("SubscriptionLength");
static TCHAR szDefAnnuitySubscriptionEndWarning[] = _T("SubscriptionEndWarning");
static TCHAR szDefAnnuitySubscriptionID[]         = _T("SubscriptionID");
static TCHAR szDefAnnuitySubscriptionXSKU[]       = _T("SubscriptionXSKU");
static TCHAR szDefAnnuitySubscriptionXPRO[]       = _T("SubscriptionXPRO");
static TCHAR szDefAnnuityInetEnable[]             = _T("InternetEnable");

//------------------------------------------------------------------------------
//
// We need to access the instance of our DLL when doing UI.
//
//------------------------------------------------------------------------------
extern CDefAnntyApp theApp;
CLSService *g_pLiveSubscribe = NULL;

//------------------------------------------------------------------------------
//
// These enumerators are passed around between the logic and the reporting
// functions.
//
//------------------------------------------------------------------------------
enum {
    DefAnnuityReportFreeOK = 0,
    DefAnnuityReportFreeWarning,
    DefAnnuityReportFreeExpired,
    DefAnnuityReportSubscriptionOK,
    DefAnnuityReportSubscriptionWarning,
    DefAnnuityReportSubscriptionExpired
};

//------------------------------------------------------------------------------
//
// This structure is used to keep the status of the def annuity.  It is set up
// by DefAnnuityPreUpdateCheck(), and is used by DefAnnuityPostUpdateCheck().
// The size of the structure should not exceed 64 bytes.
//
//------------------------------------------------------------------------------
typedef struct
{
    DWORD dwInternalTagStart;
    DWORD dwEnabled;
    DWORD dwPreUpdateResult;
    int iReport;
    long lDays;
    DWORD dwInternalTagEnd;
} *PDEFANNUITYSTATUS;

typedef HRESULT (STDAPICALLTYPE *CTLREGPROC)();

static TCHAR g_SessionFileName[] = _T("navsess.txt");
static TCHAR g_ServiceID[]  = _T("61D460E0-07C0-11d3-A985-00A0244D507A");

// Rad's registry functions

//------------------------------------------------------------------------------
//
// static LONG DefAnnuityCreateHive ( PHKEY phKey )
//
// Creates the def annuity hive szDefAnnuityHive under HKEY_LOCAL_MACHINE.
//
// Entry:
//      phKey  - pointer to a variable to receive handle to the last key in the
//               hive.
//
// Exit:
//      ERROR_SUCCESS if successful (*phKey contains registry handle to hive);
//      ERROR_NOT_ENOUGH_MEMORY if unable to allocate memory for operation;
//      ERROR_PATH_NOT_FOUND if invalid szDefAnnuityHive;
//      other ERROR_ values may be returned by Windows.
//
//------------------------------------------------------------------------------
static LONG DefAnnuityCreateHive ( PHKEY phKey )
{
    auto LONG lResult;
    auto PTSTR pszBuffer, pszCreateKey, pszParseKey;
    auto TCHAR chSave;
    auto HKEY hKey, hNewKey;
    auto DWORD dwDisposition;

    if ( pszBuffer = new TCHAR[ _tcslen ( szDefAnnuityHive ) + 1 ] )
    {
        _tcscpy ( pszBuffer, szDefAnnuityHive );

        hKey = HKEY_LOCAL_MACHINE;

        pszParseKey = pszCreateKey = pszBuffer;

        do
        {
            while ( ( chSave = *pszParseKey ) && chSave != _T('\\') )
                pszParseKey = CharNext ( pszParseKey );

            *pszParseKey = _T('\0');

            lResult = RegCreateKeyEx ( hKey,
                                       pszCreateKey,
                                       0,
                                       _T(""),
                                       REG_OPTION_NON_VOLATILE,
                                       KEY_READ | KEY_WRITE,
                                       NULL,
                                       &hNewKey,
                                       &dwDisposition );

            if ( lResult != ERROR_SUCCESS )
                break;

            if ( hKey != HKEY_LOCAL_MACHINE )
                RegCloseKey ( hKey );

            hKey = hNewKey;

            *pszParseKey = chSave;

            pszCreateKey = ++pszParseKey;

        } while ( chSave );

        delete[] pszBuffer;

        if ( lResult == ERROR_SUCCESS )
        {
            if ( hKey == HKEY_LOCAL_MACHINE )
            {
                lResult = ERROR_PATH_NOT_FOUND;
            }
            else
            {
                *phKey = hKey;

                hKey = HKEY_LOCAL_MACHINE;
            }
        }

        if ( hKey != HKEY_LOCAL_MACHINE )
            RegCloseKey ( hKey );
    }
    else
    {
        lResult = ERROR_NOT_ENOUGH_MEMORY;
    }

    return lResult;
}

//------------------------------------------------------------------------------
//
// LONG DefAnnuityGetString ( LPTSTR szValue,  int iLength, LPCTSTR szName)
//
// Reads a string value under the def annuity hive.
//
// Entry:
//      szValue - pointer to a buffer that will contain the string
//      iLength - number of bytes long the buffer is
//		szName  - the name of the value that should be read
//
// Exit:
//      ERROR_SUCCESS if successful (szValue contains the value);
//      ERROR_INVALID_DATA if the value was not a string;
//      other ERROR_ values as defined by DefAnnuityCreateHive().
//
//------------------------------------------------------------------------------
LONG DefAnnuityGetString ( LPTSTR szValue,  int iLength, LPCTSTR szName)
{
    auto LONG lResult;
    auto HKEY hKey;
    auto DWORD dwType, dwLength;

    lResult = DefAnnuityCreateHive ( &hKey );

    if ( lResult == ERROR_SUCCESS )
    {
        dwLength = iLength;

        lResult = RegQueryValueEx ( hKey,
                                    szName,
                                    NULL,
                                    &dwType,
                                    LPBYTE(szValue),
                                    &dwLength );

        RegCloseKey ( hKey );

        if ( lResult == ERROR_SUCCESS )
        {
            if ( dwType != REG_SZ )
            {
                lResult = ERROR_INVALID_DATA;
            }
        }
    }

    return lResult;
}

//------------------------------------------------------------------------------
//
// LONG DefAnnuitySetString ( LPCTSTR szValue, LPCTSTR szName )
//
// Writes a string value under the def annuity hive.
//
// Entry:
//      szValue - pointer to a buffer that contains the string
//		szName  - name the string will be stored under
//
// Exit:
//      ERROR_SUCCESS if successful (szValue contains the value);
//      other ERROR_ values as defined by DefAnnuityCreateHive().
//
//------------------------------------------------------------------------------
LONG DefAnnuitySetString ( LPCTSTR szValue, LPCTSTR szName )
{
    auto LONG  lResult;
    auto HKEY  hKey;
	auto DWORD dwLength;

    lResult = DefAnnuityCreateHive ( &hKey );

    if ( lResult == ERROR_SUCCESS )
    {
        dwLength = _tcslen( szValue );

        lResult = RegSetValueEx ( hKey,
                                  szName,
                                  0,
                                  REG_SZ,
                                  LPBYTE(szValue),
                                  dwLength );

        RegCloseKey ( hKey );

    }

    return lResult;
}

//------------------------------------------------------------------------------
//
// static LONG DefAnnuityGetDWORD ( LPCTSTR pszValue, PDWORD pdwValue )
//
// Reads the DWORD value pszValue under the def annuity hive.
//
// Entry:
//      pszValue - pointer to a string containing the name of the value;
//      pdwValue - pointer to a DWORD variable to receive the value.
//
// Exit:
//      ERROR_SUCCESS if successful (*pdwValue contains the value);
//      ERROR_INVALID_DATA if the value was not a DWORD;
//      other ERROR_ values as defined by DefAnnuityCreateHive().
//
//------------------------------------------------------------------------------
static LONG DefAnnuityGetDWORD ( LPCTSTR pszValue, PDWORD pdwValue )
{
    auto LONG lResult;
    auto HKEY hKey;
    auto DWORD dwType, dwData, dwLength;

    lResult = DefAnnuityCreateHive ( &hKey );

    if ( lResult == ERROR_SUCCESS )
    {
        dwLength = sizeof(dwData);

        lResult = RegQueryValueEx ( hKey,
                                    pszValue,
                                    NULL,
                                    &dwType,
                                    LPBYTE(&dwData),
                                    &dwLength );

        RegCloseKey ( hKey );

        if ( lResult == ERROR_SUCCESS )
        {
            if ( dwType == REG_DWORD && dwLength == sizeof(dwData) )
            {
                *pdwValue = dwData;
            }
            else
            {
                lResult = ERROR_INVALID_DATA;
            }
        }
    }

    return lResult;
}


//------------------------------------------------------------------------------
//
// static LONG DefAnnuitySetDWORD ( LPCTSTR pszValue, DWORD dwValue )
//
// Writes a DWORD to the value pszValue under the def annuity hive.
//
// Entry:
//      pszValue - pointer to a string containing the name of the value;
//      dwValue  - a DWORD variable to write.
//
// Exit:
//      ERROR_SUCCESS if successful;
//      other ERROR_ values as defined by DefAnnuityCreateHive().
//
//------------------------------------------------------------------------------
static LONG DefAnnuitySetDWORD ( LPCTSTR pszValue, DWORD dwValue )
{
    auto LONG lResult;
    auto HKEY hKey;

    lResult = DefAnnuityCreateHive ( &hKey );

    if ( lResult == ERROR_SUCCESS )
    {
        lResult = RegSetValueEx ( hKey,
                                  pszValue,
                                  0,
                                  REG_DWORD,
                                  LPBYTE(&dwValue),
                                  sizeof(dwValue) );

        RegCloseKey ( hKey );
    }

    return lResult;
}

//------------------------------------------------------------------------------
//
// LONG DefAnnuityGetSubsID ( LPTSTR szValue,  int iLength)
//
// Reads the string value Subscription ID under the def annuity hive.
//
// Entry:
//      szValue - pointer to a buffer that will contain the ID
//      iLength - number of bytes long the buffer is
//
// Exit:
//      ERROR_SUCCESS if successful (szValue contains the value);
//      ERROR_INVALID_DATA if the value was not a string;
//      other ERROR_ values as defined by DefAnnuityCreateHive().
//
//------------------------------------------------------------------------------
LONG DefAnnuityGetSubsID ( LPTSTR szValue,  int iLength)
{
	return DefAnnuityGetString(szValue, iLength, szDefAnnuitySubscriptionID);
}

//------------------------------------------------------------------------------
//
// LONG DefAnnuitySetSubsID ( LPTSTR szValue )
//
// Writes the string value Subscription ID under the def annuity hive.
//
// Entry:
//      szValue - pointer to a buffer that contains the ID
//
// Exit:
//      ERROR_SUCCESS if successful (szValue contains the value);
//      other ERROR_ values as defined by DefAnnuityCreateHive().
//
//------------------------------------------------------------------------------
LONG DefAnnuitySetSubsID ( LPCTSTR szValue )
{
	return DefAnnuitySetString( szValue, szDefAnnuitySubscriptionID );
}

//------------------------------------------------------------------------------
//
// LONG DefAnnuityGetSubsXSKU ( LPTSTR szValue,  int iLength)
//
// Reads the string value Subscription extension SKU 
// under the def annuity hive.
//
// Entry:
//      szValue - pointer to a buffer that will contain the SKU
//      iLength - number of bytes long the buffer is
//
// Exit:
//      ERROR_SUCCESS if successful (szValue contains the value);
//      ERROR_INVALID_DATA if the value was not a string;
//      other ERROR_ values as defined by DefAnnuityCreateHive().
//
//------------------------------------------------------------------------------
LONG DefAnnuityGetSubsXSKU ( LPTSTR szValue,  int iLength)
{
	return DefAnnuityGetString( szValue, iLength, szDefAnnuitySubscriptionXSKU );
}

//------------------------------------------------------------------------------
//
// LONG DefAnnuitySetSubsXSKU ( LPTSTR szValue )
//
// Writes the string value Subscription extension SKU 
// under the def annuity hive.
//
// Entry:
//      szValue - pointer to a buffer that contains the SKU
//
// Exit:
//      ERROR_SUCCESS if successful (szValue contains the value);
//      other ERROR_ values as defined by DefAnnuityCreateHive().
//
//------------------------------------------------------------------------------
LONG DefAnnuitySetSubsXSKU ( LPCTSTR szValue )
{
	return DefAnnuitySetString( szValue, szDefAnnuitySubscriptionXSKU );
}

//------------------------------------------------------------------------------
//
// LONG DefAnnuityGetSubsXPRO ( LPTSTR szValue,  int iLength)
//
// Reads the string value Subscription extension promo code 
// under the def annuity hive.
//
// Entry:
//      szValue - pointer to a buffer that will contain the promo code
//      iLength - number of bytes long the buffer is
//
// Exit:
//      ERROR_SUCCESS if successful (szValue contains the value);
//      ERROR_INVALID_DATA if the value was not a string;
//      other ERROR_ values as defined by DefAnnuityCreateHive().
//
//------------------------------------------------------------------------------
LONG DefAnnuityGetSubsXPRO ( LPTSTR szValue,  int iLength)
{
	return DefAnnuityGetString( szValue, iLength, szDefAnnuitySubscriptionXPRO );
}

//------------------------------------------------------------------------------
//
// LONG DefAnnuitySetSubsXPRO ( LPTSTR szValue )
//
// Writes the string value Subscription extension promo code 
// under the def annuity hive.
//
// Entry:
//      szValue - pointer to a buffer that contains the promo code
//
// Exit:
//      ERROR_SUCCESS if successful (szValue contains the value);
//      other ERROR_ values as defined by DefAnnuityCreateHive().
//
//------------------------------------------------------------------------------
LONG DefAnnuitySetSubsXPRO ( LPCTSTR szValue )
{
	return DefAnnuitySetString( szValue, szDefAnnuitySubscriptionXPRO );
}

//------------------------------------------------------------------------------
//
// LONG DefAnnuityGetInetEnable ( DWORD *pdwValue )
//
// Reads the DWORD value for Internet subscription enable 
// under the def annuity hive.
//
// Entry:
//      pdwValue - pointer to a DWORD that will contain the value
//
// Exit:
//      ERROR_SUCCESS if successful (pdwValue contains the value);
//      ERROR_INVALID_DATA if the value was not a DWORD;
//      other ERROR_ values as defined by DefAnnuityCreateHive().
//
//------------------------------------------------------------------------------
LONG DefAnnuityGetInetEnable ( DWORD *pdwValue )
{
	return DefAnnuityGetDWORD( szDefAnnuityInetEnable, pdwValue );
}

//------------------------------------------------------------------------------
//
// LONG DefAnnuitySetInetEnable ( DWORD dwValue )
//
// Writes the DWORD value for Internet subscription enable 
// under the def annuity hive.
//
// Entry:
//      dwValue - DWORD that contains the value
//
// Exit:
//      ERROR_SUCCESS if successful
//      other ERROR_ values as defined by DefAnnuityCreateHive().
//
//------------------------------------------------------------------------------
LONG DefAnnuitySetInetEnable ( DWORD dwValue )
{
	return DefAnnuitySetDWORD( szDefAnnuityInetEnable, dwValue );
}

//////////////////////////////////////////////////////////////////
// Local Helper functions

BOOL IsVeniceMode()
{	CLSService srv;
	BOOL bInstalled, bActive;

	CoInitialize(NULL);
	if(!srv.Open(g_ServiceID))
		return FALSE;
	bInstalled = srv.IsThereVeniceClient();
	bActive = srv.IsVeniceClientActive();
	srv.Close();
	CoUninitialize();
	return ( bInstalled && bActive );
}

BOOL GetInstallDirectory(CString &csPath)
{	TCHAR szTemp[MAX_PATH];
	int i;
	CString csTemp;
	HINSTANCE hInstance = AfxGetInstanceHandle();

	if(hInstance == NULL)
		return FALSE;
	if(!GetModuleFileName(hInstance, szTemp, MAX_PATH))
		return FALSE;
	csTemp = szTemp;
	i = csTemp.ReverseFind('\\');
	if(i < 0)
		return FALSE;
	csPath = csTemp.Left(i);
	return TRUE;
}

/*#$-TODO:Add code to read the Free/Paid status-$#*/
BOOL IsFreeSubscription(CLSService *pSrv)
{
	return TRUE;
}

//------------------------------------------------------------------------------
//
// static LPTSTR DefAnnuityLoadString (
//      UINT uStringID
//      )
//
// This function loads a string from the resource file of our DLL.  Memory is
// automatically allocated to fit the whole string.  When the memory is no
// longer needed, it should be freed up by calling delete[] LPTSTR.
//
// Entry:
//      uStringID - ID of string to load.
//
// Exit:
//      Returns a pointer to the string uStringID.  If the string could not be
//      loaded, the function returns NULL.
//
//------------------------------------------------------------------------------
static LPTSTR DefAnnuityLoadString (
    UINT uStringID
    )
{
    auto int iStringLength, iMessageLength;
    auto LPTSTR pszString;

    iStringLength = 256;

    pszString = NULL;

    do
    {
        if ( pszString )
        {
            delete[] pszString;

            iStringLength += 256;
        }

        pszString = new TCHAR[iStringLength];

        if ( !pszString )
            return NULL;

        iMessageLength = LoadString ( theApp.m_hInstance, uStringID, pszString, iStringLength );

        if ( !iMessageLength )
        {
            delete[] pszString;

            return NULL;
        }
    } while ( iMessageLength > iStringLength - 4 );

    return pszString;
}

//------------------------------------------------------------------------------
//
// static DWORD DefAnnuityPreUpdateReport (
//      HWND hWnd,
//      int iReport,
//      long lDays,
//      long *plSubscription,
//      BOOL bQuiet,
//      BOOL bConfirmationNumber
//      )
//
// This function reports the status of the module prior to LU, and queries for
// action to take if necessary.
//
// Entry:
//      hWnd                - handle to caller's window, or NULL if no
//                            application owns the UI.
//      iReport             - one of the DefAnnuityReport* enumerators;
//      lDays               - "universal" argument, which changes depending on
//                            iReport;
//      plSubscription      - pointer to a variable containing the default
//                            length of a subscription;
//      bQuiet              - TRUE if we are running in quiet mode (not allowed
//                            to display UI, FALSE if UI is allowed).
//      bConfirmationNumber - TRUE if confirmation number should be required to
//                            validate subscription, FALSE if not.
//
// Exit:
//      Returns DefAnnuityUpdateAll, DefAnnuityUpdatePatches, or
//      DefAnnuityCancelUpdate, depending on user's response.  If an error has
//      occurred, this function returns DefAnnuityError.
//
// Notes:
//      When this function is called, the iReport argument should contain one
//      of the following:
//
//          DefAnnuityReportFreeOK,
//          DefAnnuityReportFreeWarning,
//          DefAnnuityReportFreeExpired,
//          DefAnnuityReportSubscriptionOK,
//          DefAnnuityReportSubscriptionWarning, or
//          DefAnnuityReportSubscriptionExpired.
//
//      Depending on the value of iReport, different UI and prompting will be
//      displayed.  If bQuiet is not FALSE, then this function will return an
//      action without prompting the user (in which case hWnd is not used).
//      If bQuiet is FALSE, then wizard will be displayed if any information
//      needs to be presented to the user (during warning or expiration
//      notifications), in which case hWnd is used for displaying dialogs.
//
//      The lDays argument should contain the number of days left in the free
//      period or subscription if iReport is one of the OK values, or if it is
//      one of the warning values.  lDays should contain the number of days
//      elapsed after the free period or subscription has expired if iReport
//      denotes that the free/subscription period has expired.
//
//      plSubscription points to a long variable, initially containing the
//      default length of a subscription in days.  Upon return, this function
//      will set this variable so that it represents the number of days to
//      change the subscription with.  If DefAnnuityPreUpdateReport() does not
//      want any changes to the subscription do be done, it will set this value
//      to 0.  If the user has subscribed, the value will either be left as it
//      is (which is the default length of a subscription), or it will be set
//      to the number of days of the subscription.  Changes to the subscription
//      will be recorded by the caller only if the value of this variable is a
//      positive number, and the function does not return DefAnnuityError.
//
//------------------------------------------------------------------------------
static DWORD DefAnnuityPreUpdateReport (
    HWND hWnd,
    int iReport,
    long lDays,
    long *plSubscription,
    BOOL bQuiet,
    BOOL bConfirmationNumber
    )
{
    auto CSubWizard *pWiz;
    auto SUB_STATE s;

    if ( bQuiet )
    {
        *plSubscription = 0;

        switch ( iReport )
        {
        case DefAnnuityReportFreeOK:
        case DefAnnuityReportFreeWarning:
        case DefAnnuityReportSubscriptionOK:
        case DefAnnuityReportSubscriptionWarning:

            return DefAnnuityUpdateAll;

        case DefAnnuityReportFreeExpired:
        case DefAnnuityReportSubscriptionExpired:

            return DefAnnuityUpdatePatches;

        default:

            return DefAnnuityError;
        }
    }

    switch ( iReport )
    {
    case DefAnnuityReportFreeOK:
    case DefAnnuityReportSubscriptionOK:

        *plSubscription = 0;

        return DefAnnuityUpdateAll;

    case DefAnnuityReportFreeWarning:

        s = SUB_FREEWARN;

        break;

    case DefAnnuityReportFreeExpired:

        s = SUB_FREEEXP;

        break;

    case DefAnnuityReportSubscriptionWarning:

        s = SUB_PAIDWARN;

        break;

    case DefAnnuityReportSubscriptionExpired:

        s = SUB_PAIDEXP;

        break;

    default:

        return DefAnnuityError;
    }

    if ( pWiz = new CSubWizard ( IDS_WIZTITLE, hWnd ? NULL : CWnd::FromHandle ( hWnd ) ) )
    {
        pWiz->SetState ( s );

        pWiz->SetRemaining ( lDays );

        pWiz->SetConfirmationNumberUI ( bConfirmationNumber );

        switch ( pWiz->DoModal() )
        {
        case 0:

            delete pWiz;

            return DefAnnuityError;

        case IDCANCEL:

            *plSubscription = 0;

            delete pWiz;

            return DefAnnuityCancelUpdate;
        }

        switch ( pWiz->GetResult() )
        {
        case RESULT_DOWNLOAD:

            *plSubscription = 0;

        case RESULT_SUBSCRIBED:

            delete pWiz;

            return DefAnnuityUpdateAll;

        case RESULT_PATCHES:

            *plSubscription = 0;

            delete pWiz;

            return DefAnnuityUpdatePatches;

        case RESULT_CANCEL:

            *plSubscription = 0;

            return DefAnnuityCancelUpdate;
        }

        delete pWiz;
    }

    return DefAnnuityError;
}

//------------------------------------------------------------------------------
//
// static DWORD DefAnnuityPostUpdateReport (
//      int iReport,
//      long lDays
//      )
//
// This function reports the status of the module after a successful LiveUpdate
// session.
//
// Entry:
//      iReport - one of the DefAnnuityReport* enumerators;
//      lDays   - "universal" argument, which changes depending on iReport.
//
// Exit:
//      Returns DefAnnuitySuccess if successful, DefAnnuityError if unable to
//      display UI.
//
// Notes:
//      When this function is called, the iReport argument should contain one
//      of the following:
//
//          DefAnnuityReportFreeOK,
//          DefAnnuityReportFreeWarning,
//          DefAnnuityReportFreeExpired,
//          DefAnnuityReportSubscriptionOK,
//          DefAnnuityReportSubscriptionWarning, or
//          DefAnnuityReportSubscriptionExpired.
//
//      Depending on the value of iReport, different messages will be
//      displayed (in the case of *OK, no messages will be displayed).
//
//      The lDays argument should contain the number of days left in the free
//      or subscription periods if iReport is one of the OK values, or if it is
//      one of the warning values.  lDays should contain the number of days
//      elapsed after the free period or subscription has expired if iReport
//      denotes that the free/subscription period has expired.
//
//------------------------------------------------------------------------------
static DWORD DefAnnuityPostUpdateReport (
    int iReport,
    long lDays
    )
{
    auto UINT uMessage, uInfo;
    auto int iMessageType;
    auto PTSTR pszString, pszFormatted, pszTitle;

    uMessage = 0;
    uInfo = 0;

    switch ( iReport )
    {
    case DefAnnuityReportFreeOK:

        break;

    case DefAnnuityReportFreeWarning:

        uMessage = ( lDays == 1 ) ? IDS_FREE_LAST_DAY_WARNING : IDS_FREE_WARNING;

        iMessageType = MB_OK | MB_ICONWARNING;

        break;

    case DefAnnuityReportFreeExpired:

        uMessage = IDS_FREE_EXPIRED;

        iMessageType = MB_OK | MB_ICONSTOP;

        uInfo = IDS_SUBSCRIBE_DIRECTIONS;

        break;

    case DefAnnuityReportSubscriptionOK:

        break;

    case DefAnnuityReportSubscriptionWarning:

        uMessage = ( lDays == 1 ) ? IDS_SUBSCRIPTION_LAST_DAY_WARNING : IDS_SUBSCRIPTION_WARNING;

        iMessageType = MB_OK | MB_ICONWARNING;

        break;

    case DefAnnuityReportSubscriptionExpired:

        uMessage = IDS_SUBSCRIPTION_EXPIRED;

        iMessageType = MB_OK | MB_ICONSTOP;

        uInfo = IDS_SUBSCRIBE_DIRECTIONS;

        break;

    default:

        return DefAnnuityError;
    }

    if ( uMessage != 0 )
    {
        if ( pszTitle = DefAnnuityLoadString ( IDS_DEFANNUITY_TITLE ) )
        {
            if ( pszString = DefAnnuityLoadString ( uMessage ) )
            {
                if ( pszFormatted = new TCHAR[_tcslen(pszString) + 20] )
                {
#if defined(_UNICODE)
                    swprintf ( pszFormatted, pszString, lDays );
#else
                    sprintf ( pszFormatted, pszString, lDays );
#endif

                    if ( MessageBox ( NULL, pszFormatted, pszTitle, iMessageType ) == IDOK )
                    {
                        delete[] pszFormatted;

                        delete[] pszString;

                        if ( uInfo )
                        {
                            if ( pszString = DefAnnuityLoadString ( uInfo ) )
                            {
                                if ( MessageBox ( NULL, pszString, pszTitle, MB_OK | MB_ICONEXCLAMATION ) != IDOK )
                                {
                                    delete[] pszString;

                                    delete[] pszTitle;

                                    return DefAnnuityError;
                                }

                                delete[] pszString;
                            }
                            else
                            {
                                delete[] pszTitle;

                                return DefAnnuityError;
                            }
                        }

                        delete[] pszTitle;

                        return DefAnnuitySuccess;
                    }

                    delete[] pszFormatted;
                }

                delete[] pszString;
            }

            delete[] pszTitle;
        }

        return DefAnnuityError;
    }

    return DefAnnuitySuccess;
}

//////////////////////////////////////////////////////////////////
// Exported functions

//------------------------------------------------------------------------------
//
// DWORD DEFANNUITYAPI DefAnnuityInitialize (
//      HWND hWnd
//      )
//
// This function intializes the def annuity.  It first cleans up registry keys
// which should not be present during initialization and leaves only keys that
// are acceptable to have after installation.  Then, it sets the start date of
// counting to today's date.
//
// Entry:
//      hWnd                     - handle to caller's window (currently not
//                                 used).
//      dwVersion                - DWORD value containing the version of the
//                                 product which is intializing the module.
//                                 IGNORED IN NEW VERSION
//      dwEnabled                - DWORD value containing 0 if the module should
//                                 be disabled, or non-0 if the module should be
//                                 enabled.
//                                 IGNORED IN NEW VERSION
//      dwConfirmationNumber     - DWORD value containing 0 if confirmation
//                                 number is not required for validating
//                                 subscription, or non-0 if confirmation number
//                                 should be required.
//                                 IGNORED IN NEW VERSION
//      dwFreeLength             - DWORD value containing the default length of
//                                 the free subscription period (in days).
//                                 IGNORED IN NEW VERSION
//      dwSubscriptionLength     - DWORD value containing the default length of
//                                 a single subscription period (in days).
//                                 IGNORED IN NEW VERSION
//      dwFreeEndWarning         - DWORD value containing the number of days to
//                                 start warning before the free subscription
//                                 period expires.
//                                 IGNORED IN NEW VERSION
//      dwSubscriptionEndWarning - DWORD value containing the number of days to
//                                 start warning before the paid subscription
//                                 period expires.
//                                 IGNORED IN NEW VERSION
//
// Exit:
//      Returns DefAnnuitySuccess if successful initialization, or
//      DefAnnuityError if failed.
//
// Notes:
//      Passing 0 as dwFreeLength, dwSubscriptionLength, dwFreeEndWarning, and
//      dwSubscriptionEndWarning will force the module to use the default
//      values for these parameters.
//
//------------------------------------------------------------------------------
DWORD DEFANNUITYAPI DefAnnuityInitialize (
    HWND hWnd,
    DWORD dwVersion,
    DWORD dwEnabled,
    DWORD dwConfirmationNumber,
    DWORD dwFreeLength,
    DWORD dwSubscriptionLength,
    DWORD dwFreeEndWarning,
    DWORD dwSubscriptionEndWarning
    )
{	_bstr_t bstrPath;
	CString csSessionFile;
	CLSService srv;
	BOOL bResult = FALSE;

    AFX_MANAGE_STATE ( AfxGetStaticModuleState() );

    DefAnnuitySetDWORD ( szDefAnnuityVersion, dwVersion );
    if( !dwEnabled )
    	DefAnnuitySetDWORD ( szDefAnnuityEnabled, 0 );
	if(dwSubscriptionLength)
    	DefAnnuitySetDWORD ( szDefAnnuitySubscriptionLength, dwSubscriptionLength );
	if(dwSubscriptionEndWarning)
	    DefAnnuitySetDWORD ( szDefAnnuitySubscriptionEndWarning, dwSubscriptionEndWarning );

	CoInitialize(NULL);
	if(!srv.Open(g_ServiceID))
		return DefAnnuityError;
	bResult = GetInstallDirectory(csSessionFile);
	if(!bResult)
	{	srv.Close();
		CoUninitialize();
		return DefAnnuityError;
	}
	csSessionFile += '\\';
	csSessionFile += g_SessionFileName;
	bResult = srv.ReadSessionFile(csSessionFile);
	srv.Close();
	CoUninitialize();
	return (bResult ? DefAnnuitySuccess : DefAnnuityError);
}

//------------------------------------------------------------------------------
//
// DWORD DEFANNUITYAPI DefAnnuityUninitialize (
//      HWND hWnd
//      )
//
// This function removes all traces of the def annuity module.  Any keys and
// values related to it (including the ones set up during by external
// applications) are deleted.
//
// Entry:
//      hWnd - handle to caller's window (currently not used).
//
// Exit:
//      Always returns DefAnnuitySuccess.
//
//------------------------------------------------------------------------------
DWORD DEFANNUITYAPI __stdcall DefAnnuityUninitialize (
    HWND hWnd
    )
{	CLSService srv;
	_bstr_t bstrPath;
	HINSTANCE hDLL;
	HRESULT hResult;
	TCHAR szSubscriptionID[100];
	BOOL bResult = FALSE, bRemove = FALSE;

    AFX_MANAGE_STATE ( AfxGetStaticModuleState() );

	CoInitialize(NULL);
	if (!srv.Open(g_ServiceID))
	{	CoUninitialize();
		return DefAnnuityError;
	}
	DefAnnuityGetSubsID(szSubscriptionID, sizeof(szSubscriptionID));
	if (!srv.RemoveService(szSubscriptionID))
	{	srv.Close();
		CoUninitialize();
		return DefAnnuityError;
	}

	// Find DLL
	bResult = srv.GetDLLPath(&bstrPath);
	srv.Close();
	if (!bResult)
	{	CoUninitialize();
		return DefAnnuityError;
	}
	hDLL = LoadLibrary((LPCTSTR) bstrPath);
	if (hDLL == NULL)
	{	CoUninitialize();
		return DefAnnuityError;
	}
	CTLREGPROC DLLUnregister = 
		(CTLREGPROC)GetProcAddress(hDLL, _T("DllUnregisterServer"));
	if (DLLUnregister == NULL)
	{	CoUninitialize();
		return DefAnnuityError;
	}
	switch (hResult = DLLUnregister())
	{	case S_OK:
			bRemove = TRUE;
		case S_FALSE:
			bResult = TRUE;
			break;
		default:
			break;
	}
	FreeLibrary(hDLL);
	// Oleg of the Venice team requested this not be done anymore
//	if(bRemove)
//		DeleteFile((LPCTSTR) bstrPath);
	CoUninitialize();
	return (bResult ? DefAnnuitySuccess : DefAnnuityError);
}

//------------------------------------------------------------------------------
//
// DWORD DEFANNUITYAPI __stdcall DefAnnuityGetDownloadStatus ( void )
//
// This function checks the definitions subscription status, and determines
// whether downloading of definitions is allowed or not allowed.
//
// Entry:
//      -
//
// Exit:
//      DefAnnuityError if error has occurred; DefAnnuityDownloadAllowed if the
//      current subscription status allows for downloading of the defs; and
//      DefAnnuityDownloadNotAllowed if the current subscription status does
//      not allow for downloading of the defs.
//
//------------------------------------------------------------------------------
DWORD DEFANNUITYAPI __stdcall DefAnnuityGetDownloadStatus ( void )
{
    auto BYTE abyBuffer[64];

	CoInitialize(NULL);
    switch ( DefAnnuityPreUpdateCheck ( NULL, TRUE, abyBuffer ) )
    {
    case DefAnnuityUpdateAll:

		CoUninitialize();
        return DefAnnuityDownloadAllowed;

    case DefAnnuityUpdatePatches:
    case DefAnnuityCancelUpdate:

		CoUninitialize();
        return DefAnnuityDownloadNotAllowed;
    }

	CoUninitialize();
    return DefAnnuityError;
}

void LaunchUrl()
{	_bstr_t bString;
	CString csUrl;
	TCHAR szSubsID[MAX_SUBS_ID_VALUE];

	DefAnnuityGetSubsID(szSubsID, sizeof(szSubsID));

	if(!g_pLiveSubscribe)
		return;
	g_pLiveSubscribe->GetSubscriptionUrl(&bString, szSubsID);
	csUrl = (LPCTSTR) bString;
	ShellExecute(NULL, _T("open"), csUrl, NULL, _T("."), SW_SHOWMAXIMIZED);
	return;
}

//------------------------------------------------------------------------------
//
// DWORD DEFANNUITYAPI DefAnnuityPreUpdateCheck (
//      HWND hWnd,
//      BOOL bQuiet,
//      LPBYTE lpbyBuffer
//      )
//
// This function validates the subscription status, determines what kind of
// LiveUpdate we need to do, queries the user for any information that may be
// needed and returns an action to do.  Depending on the parameters, this
// function will work differently: if bQuiet is set to TRUE, then no UI will
// be displayed, and an action will be returned based on the current settings;
// if bQuiet is FALSE, then this function will display UI as necessary, and
// may prompt the client to subscribe, or warn the client that the current
// subscription is about to expire.  The hWnd parameter is used for UI, and it
// is needed only when bQuiet is set to FALSE.  Upon return, lpbyBuffer is
// filled with the status of the def annuity, and this buffer should be passed
// to DefAnnuityPostUpdateCheck() if the client chooses to call that function
// later.
//
// Entry:
//      hWnd       - handle to caller's window (can be NULL).
//      bQuiet     - set to TRUE if the caller is started as a scheduled
//                   program, or FALSE if the caller was started on demand.
//      lpbyBuffer - pointer to a 64-byte buffer.
//
// Exit:
//      DefAnnuityError if error has occurred; DefAnnuityUpdateAll if LiveUpdate
//      should proceed with downloading the definitions together with product
//      patches; DefAnnuityUpdatePatches if the client has declined to subscribe
//      and definitions should not be downloaded; DefAnnuityCancelUpdate if
//      LiveUpdate session should not be started.
//
//------------------------------------------------------------------------------
DWORD DEFANNUITYAPI __stdcall DefAnnuityPreUpdateCheck (
    HWND hWnd,
    BOOL bQuiet,
    LPBYTE lpbyBuffer
    )
{	CLSService srv;
	BOOL bResult, bWarning, bFree;
    PDEFANNUITYSTATUS prDefAnnuityStatus;
	LONG lDays, lNewSubscription;
	DWORD dwResult;
	int iReport;
	DWORD dwEnabled, dwSubscriptionLength, dwSubscriptionEndWarning;

    AFX_MANAGE_STATE ( AfxGetStaticModuleState() );

	dwEnabled = dwSubscriptionLength = dwSubscriptionEndWarning = 0;
    DefAnnuityGetDWORD ( szDefAnnuityEnabled, &dwEnabled );
    DefAnnuityGetDWORD ( szDefAnnuitySubscriptionLength, &dwSubscriptionLength );
    DefAnnuityGetDWORD ( szDefAnnuitySubscriptionEndWarning, &dwSubscriptionEndWarning );

	CoInitialize(NULL);

    prDefAnnuityStatus = PDEFANNUITYSTATUS(lpbyBuffer);
    prDefAnnuityStatus->dwInternalTagStart = 'SdaR';
    prDefAnnuityStatus->dwInternalTagEnd = 'SdaR';

	bResult = srv.Open(g_ServiceID);
	if(!bResult)
	{	CoUninitialize();
		return DefAnnuityError;
	}
	g_pLiveSubscribe = &srv;
	bFree = IsFreeSubscription(&srv);
	bWarning = srv.IsWarningPeriod();
	lDays = srv.GetSubscriptionRemaining();
	if(lDays < 0)
		lDays = 0;
	if(bFree)
	{	if(lDays == 0)
			iReport = DefAnnuityReportFreeExpired;
		else if(bWarning)
			iReport = DefAnnuityReportFreeWarning;
		else
			iReport = DefAnnuityReportFreeOK;
	}
	else
	{	if(lDays == 0)
			iReport = DefAnnuityReportSubscriptionExpired;
		else if(bWarning)
			iReport = DefAnnuityReportSubscriptionWarning;
		else
			iReport = DefAnnuityReportSubscriptionOK;
	}

    dwResult = DefAnnuityPreUpdateReport ( hWnd, iReport, lDays, &lNewSubscription, bQuiet, TRUE );
    if ( dwResult != DefAnnuityError && lNewSubscription > 0 )
	{	TCHAR szSubsID[MAX_SUBS_ID_VALUE];

		DefAnnuityGetSubsID(szSubsID, sizeof(szSubsID));
		if(!srv.ExtendSubscription(szSubsID, LSBS_DAY, dwSubscriptionLength, dwSubscriptionEndWarning))
			dwResult = DefAnnuityError;
    }
    prDefAnnuityStatus->iReport = iReport;
    prDefAnnuityStatus->lDays = lDays;
	g_pLiveSubscribe = NULL;
	srv.Close();
	CoUninitialize();
    return prDefAnnuityStatus->dwPreUpdateResult = dwResult;
}

//------------------------------------------------------------------------------
//
// DWORD DEFANNUITYAPI DefAnnuityPostUpdateCheck (
//      HWND hWnd,
//      BOOL bQuiet,
//      LPBYTE lpbyBuffer
//      )
//
// This function reports the subscription status after a successful LiveUpdate
// session.  If bQuiet is set to FALSE, this function may display messages
// since DefAnnuityPreUpdateCheck() returned an action without displaying any
// UI.  If bQuiet is not FALSE, this function will complete quietly.
//
// Entry:
//      hWnd           - handle to caller's window (currently not used).
//      bQuiet         - set to TRUE if the caller is started as a scheduled
//                       program, or FALSE if the caller was started on
//                       demand.  This parameter needs to be the same as the
//                       bQuiet parameter passed to DefAnnuityPreUpdateCheck().
//      lpbyByteBuffer - pointer to a 64-byte buffer, previously passed to
//                       DefAnnuityPreUpdateCheck().
//
// Exit:
//      DefAnnuitySuccess or DefAnnuityError.
//
//------------------------------------------------------------------------------
DWORD DEFANNUITYAPI __stdcall DefAnnuityPostUpdateCheck (
    HWND hWnd,
    BOOL bQuiet,
    LPBYTE lpbyBuffer
    )
{
    AFX_MANAGE_STATE ( AfxGetStaticModuleState() );

    auto PDEFANNUITYSTATUS prDefAnnuityStatus;

	CoInitialize(NULL);

    prDefAnnuityStatus = PDEFANNUITYSTATUS(lpbyBuffer);

    if ( prDefAnnuityStatus->dwInternalTagStart == 'SdaR' &&
         prDefAnnuityStatus->dwInternalTagEnd == 'SdaR' )
    {
        if ( prDefAnnuityStatus->dwEnabled && bQuiet )
        {
            switch ( prDefAnnuityStatus->dwPreUpdateResult )
            {
            case DefAnnuityUpdateAll:
            case DefAnnuityUpdatePatches:

                return DefAnnuityPostUpdateReport ( prDefAnnuityStatus->iReport, prDefAnnuityStatus->lDays );
            }
        }

        return DefAnnuitySuccess;
    }

	CoUninitialize();
    return DefAnnuityError;
}

//------------------------------------------------------------------------------
//
// DWORD DEFANNUITYAPI DefAnnuityQueryStatus (
//      LPDWORD pdwVersion,
//      LPDWORD pdwInstallDate,
//      LPDWORD pdwSubscriptionDate,
//      LPLONG plExpiresIn
//      )
//
// This is yet another function which returns the status of the def annuity.
//
// Entry:
//      pdwVersion          - Pointer to a DWORD to receive the version of the
//                            def annuity.
//      pdwInstallDate      - Pointer to a DWORD to receive the date when the
//                            def annuity was installed (this is the starting
//                            date of the free subscription).  The format of the
//                            returned value is the same as we write dates to
//                            the registry (read above).
//      pdwSubscriptionDate - Pointer to a DWORD to receive the last date when
//                            the user has subscribed.  If the def annuity is
//                            disabled, the return value will be 0xFFFFFFFF.  If
//                            the user has not subscribed, the return value will
//                            be 0xFFFFFFFE.
//      plExpiresIn         - Pointer to a long to receive the number of days
//                            left in the free/paid subscription.  A positive
//                            value represents that the subscription is still
//                            active.  A value of 1 means that this is the last
//                            day of the subscription.  A value of 0 means that
//                            the subscription has expired.
//
// Exit:
//      DefAnnuitySuccess or DefAnnuityError.
//
//------------------------------------------------------------------------------
DWORD DEFANNUITYAPI __stdcall DefAnnuityQueryStatus (
    LPDWORD pdwVersion,
    LPDWORD pdwInstallDate,
    LPDWORD pdwSubscriptionDate,
    LPLONG plExpiresIn
    )
{	CLSService srv;
	BOOL bResult;
	DWORD dwVersion;

    AFX_MANAGE_STATE ( AfxGetStaticModuleState() );

	dwVersion = 0;
    DefAnnuityGetDWORD ( szDefAnnuityVersion, &dwVersion );

	CoInitialize(NULL);
	bResult = srv.Open(g_ServiceID);
	if(!bResult)
		return DefAnnuityError;
	*plExpiresIn = srv.GetSubscriptionRemaining();
	if(*plExpiresIn < 0)
		*plExpiresIn = 0;
	*pdwVersion = dwVersion;

	// The date when the subscription was activated at the first time.
	// Format of returning value is YYYYMMDD.
	if (!IsBadWritePtr(pdwSubscriptionDate, sizeof(DWORD)))
	{
		_bstr_t strValue;
		if (srv.GetProperty(_T("ACTIVATION_DATE"), &strValue))
		{
			LPSTR p;
			*pdwInstallDate = _tcstol((LPCTSTR)strValue, &p, 10);
		}
		else
		{
			*pdwInstallDate = 0;
		}
	}

	// The date when the current subscription was initialized.
	// Format of returning value is YYYYMMDD.
	if (!IsBadWritePtr(pdwSubscriptionDate, sizeof(DWORD)))
	{
		_bstr_t strValue;
		if (srv.GetProperty(_T("BEGIN_DATE"), &strValue))
		{
			LPSTR p;
			*pdwSubscriptionDate = _tcstol((LPCTSTR)strValue, &p, 10);
		}
		else
		{
			*pdwSubscriptionDate = 0;
		}
	}

	srv.Close();

	CoUninitialize();
    return DefAnnuitySuccess;
}

DWORD DEFANNUITYAPI DefAnnuityCreateSession(LPCTSTR szIntopts)
{
    AFX_MANAGE_STATE ( AfxGetStaticModuleState() );
	
	return (ProcessSessionTemplate(szIntopts) ? DefAnnuitySuccess : DefAnnuityError);
}
