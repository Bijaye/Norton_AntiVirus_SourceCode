/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

//#include <windows.h>
//#include <tchar.h>
#include "stdafx.h"
#include "vpcommon.h"
#define AMSDYNAMICUSAGE
#include "alrtprot.h"
#include <locale.h>
#include "AvisEventDetection.h"   // in QuarantineServer\Include
#include "aviseventrulesdata.h"
#include "EventData.h"
#include "icepacktokens.h"
#include "cba.h"
#include "amscons.h"
typedef long moduleId;       // module id
typedef long messageType;                // message type
#include "amslib.h"
#include <mbstring.h>
#include "vpstrutils.h"



typedef LRESULT (WINAPI *tAMSUIManageAlerts)(LPMANAGE_ALERTS_DLG pMd);
typedef BOOL (*tAMSUIShowAlertLog)(AMS2_ALERT_LOG *pAMS2AlertLog);
typedef int (*tAMSUIInitializeConsole)(void);
typedef void (*tAMSUIDestroyConsole)(void);

tbAMSOriginatorRegister bAMSOriginatorRegister;
tbAMSOriginatorRegisterEh bAMSOriginatorRegisterEh;
tpFindDelimiter pFindDelimiter;
tAMSFindNumberOfParams AMSFindNumberOfParams;
tAMSFindParamByNumber AMSFindParamByNumber;
tbAMSOriginatorUnregister bAMSOriginatorUnregister;
tAMSIssueAlert AMSIssueAlert;
tAMSIssueAlertEh AMSIssueAlertEh;
tAMSIsAgentActive AMSIsAgentActive;
tAddFlagToString AddFlagToString;
tAMSFindParamByName AMSFindParamByName;
tAMSGetParamDataType AMSGetParamDataType;
tAMSGetParamDataName AMSGetParamDataName;
tAMSGetParamAlias AMSGetParamAlias;
tAMSGetParamDataLen AMSGetParamDataLen;
tAMSGetParamData AMSGetParamData;
tAMSGetPastParamList AMSGetPastParamList;
tAMSRegisterAlert AMSRegisterAlert;
tAMSRegisterAlertEh AMSRegisterAlertEh;
tAMSAppendParam AMSAppendParam;
tAMSGetTextFromParam AMSGetTextFromParam;
tAMSGetTimeStringFromTimeT AMSGetTimeStringFromTimeT;
tAMSGetDateStringFromTimeT AMSGetDateStringFromTimeT;
tAMSProcessAlertMessage AMSProcessAlertMessage;
tAMSParseFormatString AMSParseFormatString;
tAMSSendAlertAck AMSSendAlertAck;


tAMSUIInitializeConsole g_fnAMSUIInitializeConsole = NULL;
tAMSUIDestroyConsole    g_fnAMSUIDestroyConsole = NULL;
tAMSUIShowAlertLog      g_fnAMSUIShowAlertLog = NULL;
tAMSUIManageAlerts      g_fnAMSUIManageAlerts = NULL;

//MNTS Functions
typedef size_t (*tNTSmbstowcs)(wchar_t *pWcStr,char *pChar,size_t charSize);
typedef int (*tNTS_GetHostAddress)(wchar_t *pWname, unsigned char protocol, CBA_NETADDR *pNetAddr);
typedef int (*tNTSBuildAddr)(CBA_NETADDR *pNetAddr,CBA_Addr *pCbaAddr,
							unsigned short port,unsigned char protocol);

tNTS_GetHostAddress NTS_GetHostAddress = NULL;
tNTSBuildAddr		NTSBuildAddr = NULL;
tNTSmbstowcs		NTSmbstowcs = NULL;

// Prototype for the InitAMS function.
BOOL InitAMS( void );
DWORD AmsActive=0;

/////////////////////////////////////////////////////////////////////////////
// Global AMS Handles.

HINSTANCE   g_hAMSUIDll = NULL;
HINSTANCE   g_hNTS = NULL;
HINSTANCE   ghAMSLibHan = NULL;
BOOL        g_bAMSInitialized = FALSE;

char AppName[MAX_NAME];
/****************************************************************************************/
/*
*/

QSAALERT QSAlertBlocks [] =
		{
			{
			_T(""),
			NO_SAMPLEEVENT_PARAMS,
				{
					{_T("Message Title"), DT_ASCII_STRING, 0},
					{_T("Hostname"), DT_ASCII_STRING, 1},
					{_T("Machine Name"), DT_ASCII_STRING, 2},
					{_T("Address"), DT_ASCII_STRING, 3},
					{_T("User"), DT_ASCII_STRING, 4},
					{_T("Platform"), DT_ASCII_STRING, 5},
					{_T("File Name"), DT_ASCII_STRING, 6},
					{_T("Virus Name"), DT_ASCII_STRING, 7},
					{_T("Defs Needed"), DT_ASCII_STRING, 8},
					{_T("Status"), DT_ASCII_STRING, 9},             // jhill 2/12/00
//					{_T("Sample State"), DT_ASCII_STRING, 9},       // tam 2/27/00
					{_T("Age"), DT_ASCII_STRING, 10},
					{_T("Error Message"), DT_ASCII_STRING, 11},
					{_T("Default message"), DT_ASCII_STRING, 12},
					{_T("Note"), DT_ASCII_STRING, 13},
				}
			},
			{
			_T(""),
			NO_GENERALEVENT_PARAMS,
				{
					{_T("Message Title"), DT_ASCII_STRING, 0},
					{_T("Hostname"), DT_ASCII_STRING, 1},
					{_T("Machine Name"), DT_ASCII_STRING, 2},
					{_T("Address"), DT_ASCII_STRING, 3},
					{_T("Error Message"), DT_ASCII_STRING, 4},
					{_T("Default Message"), DT_ASCII_STRING, 5},
				}
			}
		};

//////////////////////////////////////////////////////////////////////////////////////
//
//  Function : GetCBAAddress
//
//  Purpose : This function will retrieve and intel CBA address so that we can
//            send alerts to the AMS alerting system.  This function will resolve a
//            computer name into a properly formatted CBA address in order to do this.
//
//  Parameters :
//
//        LPCTSTR szMachineName  - [in]  Machine name to resolve.
//        CBA_Addr *pCbaAddr     - [out] CBA address of the machine that we resolved.
//
//  Return Values :
//
//        TRUE  - Address resolution was successfull.
//        FALSE - Failed to resolve computer name into CBA address.
//
//////////////////////////////////////////////////////////////////////////////////////
BOOL GetCBAAddress( LPSTR szMachName, CBA_Addr* pCbaAddr)
{
	CBA_NETADDR netAddr = {0};
	wchar_t         wName[64];
	BYTE            protocol = CBA_PROTOCOL_IP;
	DWORD			dwStrLen = 0;


	// convert machine name to wide character
	NTSmbstowcs(wName, szMachName, 64);
	// added by terrym 07-27-00 there is a problem in NTSmbstowcs.  If the input string is longer
	// than the number of chars specified to copy (last param) the 0 terminator is not copied.
	dwStrLen = wcslen(wName);
	if ( dwStrLen >= 64)
		wName[63] = '\0';

	// Get the CBA Address
	if (!NTS_GetHostAddress(wName, CBA_PROTOCOL_IP, &netAddr))

	{
		if (!NTS_GetHostAddress(wName, CBA_PROTOCOL_IPX, &netAddr))
		{
			if (dwStrLen < 16) // terrym this will crash nts if a name length longer than 16 is passed in.
			{
				if (!NTS_GetHostAddress(wName, CBA_PROTOCOL_NB, &netAddr))
					return (FALSE);
				protocol = CBA_PROTOCOL_NB;
			}
		}
		protocol = CBA_PROTOCOL_IPX;
	}

	NTSBuildAddr(&netAddr, pCbaAddr, 0, protocol);


	return(TRUE);
}


BOOL InitAMS( void )
{
    BOOL  bRc = FALSE;
	// Only initialize if we are not initialized already.
    if ( FALSE == g_bAMSInitialized )
    {
        g_hNTS = LoadLibrary(_T("NTS.dll"));
	    if (g_hNTS == NULL)
		    return bRc;

		if (((NTS_GetHostAddress = (tNTS_GetHostAddress)GetProcAddress(g_hNTS ,"NTS_GetHostAddress")) != NULL) &&
			((NTSmbstowcs= (tNTSmbstowcs)GetProcAddress(g_hNTS ,"NTSmbstowcs")) != NULL) &&
			((NTSBuildAddr= (tNTSBuildAddr)GetProcAddress(g_hNTS ,"NTSBuildAddr")) != NULL))
		{
			g_hAMSUIDll = LoadLibrary(_T("AMSUI.DLL") );

			if ( NULL != g_hAMSUIDll )
			{

				// Look up the AMS functions we are going to use.
				g_fnAMSUIInitializeConsole = (tAMSUIInitializeConsole)GetProcAddress(g_hAMSUIDll, "AMSUIInitializeConsole");
				g_fnAMSUIDestroyConsole    = (tAMSUIDestroyConsole)GetProcAddress(g_hAMSUIDll, "AMSUIDestroyConsole");
				g_fnAMSUIManageAlerts      = (tAMSUIManageAlerts)GetProcAddress(g_hAMSUIDll, "AMSUIManageAlerts");
				g_fnAMSUIShowAlertLog      = (tAMSUIShowAlertLog)GetProcAddress(g_hAMSUIDll, "AMSUIShowAlertLog");

				// Make sure that we were able to load all of the AMS functions.
				if (( g_fnAMSUIManageAlerts      ) &&
					( g_fnAMSUIShowAlertLog      ) &&
					( g_fnAMSUIInitializeConsole ) &&
					( g_fnAMSUIDestroyConsole    ))
				{
					// Try to initialize ourselves as an AMS console.
					if ( g_fnAMSUIInitializeConsole() )
					{
						// Set our initialized flag for later.
						bRc = TRUE;
					}
				}
			}
		}
        ghAMSLibHan = LoadLibrary(_T("amslib.dll"));

	    if (ghAMSLibHan == NULL)
		    return ERROR_NO_AMSLIB;

	    if ((bAMSOriginatorRegister = (tbAMSOriginatorRegister)GetProcAddress(ghAMSLibHan,"bAMSOriginatorRegister")) == NULL) return ERROR_BAD_AMSLIB;
	    if ((bAMSOriginatorRegisterEh = (tbAMSOriginatorRegisterEh)GetProcAddress(ghAMSLibHan,"bAMSOriginatorRegisterEh")) == NULL) return ERROR_BAD_AMSLIB;
	    if ((pFindDelimiter = (tpFindDelimiter)GetProcAddress(ghAMSLibHan,"pFindDelimiter")) == NULL) return ERROR_BAD_AMSLIB;
	    if ((AMSFindNumberOfParams = (tAMSFindNumberOfParams)GetProcAddress(ghAMSLibHan,"AMSFindNumberOfParams")) == NULL) return ERROR_BAD_AMSLIB;
	    if ((AMSFindParamByNumber = (tAMSFindParamByNumber)GetProcAddress(ghAMSLibHan,"AMSFindParamByNumber")) == NULL) return ERROR_BAD_AMSLIB;
	    if ((bAMSOriginatorUnregister = (tbAMSOriginatorUnregister)GetProcAddress(ghAMSLibHan,"bAMSOriginatorUnregister")) == NULL) return ERROR_BAD_AMSLIB;
	    if ((AMSIssueAlert = (tAMSIssueAlert)GetProcAddress(ghAMSLibHan,"AMSIssueAlert")) == NULL) return ERROR_BAD_AMSLIB;
	    if ((AMSIssueAlertEh = (tAMSIssueAlertEh)GetProcAddress(ghAMSLibHan,"AMSIssueAlertEh")) == NULL) return ERROR_BAD_AMSLIB;
	    if ((AMSIsAgentActive = (tAMSIsAgentActive)GetProcAddress(ghAMSLibHan,"AMSIsAgentActive")) == NULL) return ERROR_BAD_AMSLIB;
	    if ((AddFlagToString = (tAddFlagToString)GetProcAddress(ghAMSLibHan,"AddFlagToString")) == NULL) return ERROR_BAD_AMSLIB;
	    if ((AMSFindParamByName = (tAMSFindParamByName)GetProcAddress(ghAMSLibHan,"AMSFindParamByName")) == NULL) return ERROR_BAD_AMSLIB;
	    if ((AMSGetParamDataType = (tAMSGetParamDataType)GetProcAddress(ghAMSLibHan,"AMSGetParamDataType")) == NULL) return ERROR_BAD_AMSLIB;
	    if ((AMSGetParamDataName = (tAMSGetParamDataName)GetProcAddress(ghAMSLibHan,"AMSGetParamDataName")) == NULL) return ERROR_BAD_AMSLIB;
	    if ((AMSGetParamAlias = (tAMSGetParamAlias)GetProcAddress(ghAMSLibHan,"AMSGetParamAlias")) == NULL) return ERROR_BAD_AMSLIB;
	    if ((AMSGetParamDataLen = (tAMSGetParamDataLen)GetProcAddress(ghAMSLibHan,"AMSGetParamDataLen")) == NULL) return ERROR_BAD_AMSLIB;
	    if ((AMSGetParamData = (tAMSGetParamData)GetProcAddress(ghAMSLibHan,"AMSGetParamData")) == NULL) return ERROR_BAD_AMSLIB;
	    if ((AMSGetPastParamList = (tAMSGetPastParamList)GetProcAddress(ghAMSLibHan,"AMSGetPastParamList")) == NULL) return ERROR_BAD_AMSLIB;
	    if ((AMSRegisterAlert = (tAMSRegisterAlert)GetProcAddress(ghAMSLibHan,"AMSRegisterAlert")) == NULL) return ERROR_BAD_AMSLIB;
	    if ((AMSRegisterAlertEh = (tAMSRegisterAlertEh)GetProcAddress(ghAMSLibHan,"AMSRegisterAlertEh")) == NULL) return ERROR_BAD_AMSLIB;
	    if ((AMSAppendParam = (tAMSAppendParam)GetProcAddress(ghAMSLibHan,"AMSAppendParam")) == NULL) return ERROR_BAD_AMSLIB;
	    if ((AMSGetTextFromParam = (tAMSGetTextFromParam)GetProcAddress(ghAMSLibHan,"AMSGetTextFromParam")) == NULL) return ERROR_BAD_AMSLIB;
	    if ((AMSGetTimeStringFromTimeT = (tAMSGetTimeStringFromTimeT)GetProcAddress(ghAMSLibHan,"AMSGetTimeStringFromTimeT")) == NULL) return ERROR_BAD_AMSLIB;
	    if ((AMSGetDateStringFromTimeT = (tAMSGetDateStringFromTimeT)GetProcAddress(ghAMSLibHan,"AMSGetDateStringFromTimeT")) == NULL) return ERROR_BAD_AMSLIB;
	    if ((AMSProcessAlertMessage = (tAMSProcessAlertMessage)GetProcAddress(ghAMSLibHan,"AMSProcessAlertMessage")) == NULL) return ERROR_BAD_AMSLIB;
	    if ((AMSParseFormatString = (tAMSParseFormatString)GetProcAddress(ghAMSLibHan,"AMSParseFormatString")) == NULL) return ERROR_BAD_AMSLIB;
	    if ((AMSSendAlertAck = (tAMSSendAlertAck)GetProcAddress(ghAMSLibHan,"AMSSendAlertAck")) == NULL) return ERROR_BAD_AMSLIB;

		g_bAMSInitialized = TRUE;


    }
	else
		bRc = TRUE;
	return bRc;
}

//////////////////////////////////////////////////////////////////////////////////////
//
//  Function : InstallAMSEx
//
//  Purpose : Install AMS support on the targeted machine for the product specified.
//
//  Parameters :
//
//    LPTSTR  szRemoteMachineName - Machine to install our support for AMS on.
//    LPCTSTR szProductName       - Product to install.
//
//  Return Values :
//
//        DWORD - ERROR_SUCCESS = Success.
//              - All Else      = Failure code.
//
//////////////////////////////////////////////////////////////////////////////////////
DWORD InstallAMSEx( LPSTR szAppName, LPSTR szProductName)
{
	USES_CONVERSION;

	DWORD   cc;
	char    szText[MAX_NAME];
	char	szAlertName[MAX_NAME];
	BYTE    * pParamList;
	BYTE    * pEnd;
	QSAALERT * alert;
	int i=0;
	int j=0;
	BOOL r;
	int iSize;
	//	dprintf("loading AMS functions\n");
	cc = InitAMS();
	if (!cc)
		return cc;

	setlocale(LC_ALL, "");

    // Resolve the computer name into a CBA address.
    CBA_Addr rRemoteAMSServerAddress;
    GetCBAAddress( szAppName, &rRemoteAMSServerAddress );

    // Make the AMS style product (application) name.
	sprintf( AppName, "--%s", szProductName );
	AddFlagToString((BYTE *)AppName,0);


	//	dprintf("registering AMS Originator\n");
	r = bAMSOriginatorRegisterEh( &rRemoteAMSServerAddress, NULL, AMS_ORIG_REGISTER, AppName,0, NULL );
	//	r = bAMSOriginatorRegister( AMS_ORIG_REGISTER,AppName,0);

	if (r) {
		pParamList = (BYTE*)malloc(MAX_CONFIG_PARAMS*2);
		if(pParamList)
        {
			AVIS_EVENT_RULES *lpTable = NULL;

			// READ FOR GENERAL EVENTS
			lpTable = g_AvisEventRulesGeneralAttention;
			alert = &QSAlertBlocks[1];
			//			for (alert=&AAlerts[0];alert->name[0];alert++)
			for( j = 0; lpTable->lpszIcePackToken != NULL ; j++, lpTable++ )
            {
				if (!lpTable->dwIsConfigurable || lpTable->dwEventId == GENERAL_ATTENTION_SEND_TEST_EVENT)
					continue;
				if (alert->count)
                {
					memset(pParamList, 0, MAX_CONFIG_PARAMS*2);
                    pEnd = NULL;
                    // Next, append our Hostname variable.
//                    AppendHostnameParameter( pParamList, pEnd ,FALSE);
					lpTable->lpszEventName = StrTableLookUpMiscString( lpTable->dwEventNameID);
					memset(alert->name,0,sizeof(alert->name));
					ssStrnCpy(alert->name, W2T(lpTable->lpszEventName), sizeof (alert->name));
					for (i=0;i<alert->count;i++)
                    {
						strcpy(szText,"  ");
						strncat(szText,T2A(alert->prams[i].name),MAX_AMS_EVENTNAME-2);
						AddFlagToString((BYTE *)szText,STRING_MODE_ASCII);
						pEnd = AMSAppendParam(pParamList, pEnd, alert->prams[i].type,(BYTE *)szText, NULL, 0, NULL, MAX_CONFIG_PARAMS*2);
					}
                    // Fix the calculation of alert->name to work with the new AMS lib(Payson). With the old calculation registration fails.
                    iSize = strlen(T2A(alert->name));
                    memset(szAlertName, 0, sizeof(szAlertName));
					ssStrnCpy(szAlertName,T2A( alert->name),sizeof(szAlertName));
                    cc = AMSRegisterAlertEh(&rRemoteAMSServerAddress, NULL, AppName,0,(iSize+1),szAlertName,pParamList,0);

					if (cc != IA_SUCCESS)
						break;

				}
			}

			// READ FOR Sample EVENTS
			lpTable = g_AvisEventRulesSampleIntervention;
			alert = &QSAlertBlocks[0];
			//			for (alert=&AAlerts[0];alert->name[0];alert++)
			for( j = 0; lpTable->lpszIcePackToken != NULL ; j++, lpTable++ )
            {
				if (!lpTable->dwIsConfigurable || lpTable->dwEventId == GENERAL_ATTENTION_SEND_TEST_EVENT)
					continue;
				if (alert->count)
                {
					memset(pParamList, 0, MAX_CONFIG_PARAMS*2);
                    pEnd = NULL;
                    // Next, append our Hostname variable.
//                    AppendHostnameParameter( pParamList, pEnd, FALSE );

					lpTable->lpszEventName = StrTableLookUpMiscString( lpTable->dwEventNameID);
					memset(alert->name,0,sizeof(alert->name));
					ssStrnCpy(alert->name, W2T(lpTable->lpszEventName), sizeof(alert->name));
					for (i=0;i<alert->count;i++)
                    {
						strcpy(szText,"  ");
						strncat(szText,T2A(alert->prams[i].name),MAX_AMS_EVENTNAME-2);
						AddFlagToString((BYTE *)szText,STRING_MODE_ASCII);
						pEnd = AMSAppendParam(pParamList, pEnd, alert->prams[i].type,(BYTE *)szText, NULL, 0, NULL, MAX_CONFIG_PARAMS*2);
					}

                    // Fix the calculation of alert->name to work with the new AMS lib(Payson). With the old calculation registration fails.
                    iSize = strlen(T2A(alert->name));
                    memset(szAlertName, 0, sizeof(szAlertName));
					ssStrnCpy(szAlertName,T2A( alert->name),sizeof(szAlertName));
                    cc = AMSRegisterAlertEh(&rRemoteAMSServerAddress, NULL, AppName,0,(iSize+1),szAlertName,pParamList,0);

					if (cc != IA_SUCCESS)
						break;

				}
			}


			free(pParamList);
		}
	}
	else
		cc =  0xFFFFFFFFL;

	if (cc == IA_SUCCESS)
		AmsActive = TRUE;

	return cc==IA_SUCCESS?ERROR_SUCCESS:cc;
}

void RemoveAMS( void )
{
    if ( TRUE == g_bAMSInitialized )
    {
        // KJS Revisit this later.  This is causing a hang in the AMS code.
//        g_fnAMSUIDestroyConsole();
        FreeLibrary( g_hAMSUIDll );

        g_bAMSInitialized = FALSE;
        g_hAMSUIDll = NULL;

        FreeLibrary( g_hNTS );

        g_hNTS  = FALSE;
        g_hNTS = NULL;

        FreeLibrary( ghAMSLibHan );
        ghAMSLibHan = FALSE;
		ghAMSLibHan = NULL;


    }
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : OnConfigureAMS
//
// Description   :
//
// Return type   : HRESULT
//
// Argument      :  bool& bHandled
// Argument      : CSnapInObjectRootBase * pObj
//
///////////////////////////////////////////////////////////////////////////////
// 2-11-00 - TMARLES: Function created / header added
///////////////////////////////////////////////////////////////////////////////
HRESULT ConfigureAMS ( LPSTR szAppName, LPSTR szHostName, LPSTR szCaption)
{
	HRESULT hr = S_OK;
    MANAGE_ALERTS_DLG   dlg;

	if (InstallAMSEx(szHostName,szAppName ))
		return E_FAIL;
    // Zero out the memory for this structure.
    ZeroMemory(&dlg, sizeof(MANAGE_ALERTS_DLG));

    // Lookup the address of the AMS server quarantine is configured to send alerts too.
	if (GetCBAAddress( szHostName, &dlg.address))
    {


		dlg.szHostName = (LPTSTR)(szHostName);

	    dlg.szAppName = (LPTSTR)szAppName;
//	    dlg.szAppName = NULL;
	    dlg.szCaption = (LPTSTR)szCaption;

	    g_fnAMSUIManageAlerts(&dlg);
    }



//	RemoveAMS( );


    return hr;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function name : OnViewAMSLog
//
// Description   :
//
// Return type   : HRESULT
//
// Argument      :  bool& bHandled
// Argument      : CSnapInObjectRootBase * pObj
//
///////////////////////////////////////////////////////////////////////////////
// 2-11-00 - TMARLES: Function created / header added
///////////////////////////////////////////////////////////////////////////////
HRESULT ViewAMSLog( LPSTR szAppName, LPSTR szHostName, LPSTR szCaption )
{
	HRESULT hr = S_OK;
    AMS2_ALERT_LOG     dlg;


	if (InstallAMSEx(szHostName,szAppName ))
		return E_FAIL;


    // Zero out the memory for this structure.
    ZeroMemory(&dlg, sizeof(AMS2_ALERT_LOG));


    if ( GetCBAAddress( szHostName, &dlg.address ))
    {

        //Database ID not displayed text. IPE NO BUG
	    dlg.pszApplication = (LPTSTR)szAppName;
//	    dlg.pszApplication = NULL;
	    dlg.pszHost = (LPTSTR)szHostName;
	    dlg.pszCaption = (LPTSTR)szCaption;

        //SetRect( &(dlg.rect), 0, 0, 800, 600 );
        SetRect( &(dlg.rect), 0, 0, 640, 460 );

	    g_fnAMSUIShowAlertLog(&dlg);
    }


//	RemoveAMS( );
    return hr;
}

/****************************************************************************************/
DWORD UninstallAMS(void) {

	if (AmsActive)
	{
		bAMSOriginatorUnregister();

		AmsActive = 0;
	}


	Sleep(1000);

	RemoveAMS();
	return 0;
}

/****************************************************************************************/

//////////////////////////////////////////////////////////////////////////////////////
//
//  Function : StartAMSEx
//
//  Purpose : Start the AMS subsystem.  This will install support on the remote
//            machine for the product specified.
//
//  Parameters :
//
//    LPTSTR  szRemoteMachineName - Machine to install our support for AMS on.
//    LPCTSTR szProductName       - Product to install.
//
//  Return Values :
//
//    DWORD - ERROR_SUCCESS = Success.
//          - All Else      = Failure code.
//
//////////////////////////////////////////////////////////////////////////////////////
BOOL StartAMSEx(LPSTR szAppName, LPSTR szProductName)
{

	DWORD cc;

//	REF(nothing);
//	NTxSleep(1000);
//	dprintf("Registering with AMS\n");

    // Load AMS, and register our alerts.
	cc = InstallAMSEx( szAppName, szProductName);


	if (cc != ERROR_SUCCESS)
    {
//		dprintf("AMS did not install %08X\n",cc);
        return FALSE;
	}
	else
    {
//		dprintf("AMS installed\n");
        return TRUE;
	}
}


