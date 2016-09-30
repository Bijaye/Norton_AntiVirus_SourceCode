/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

#include "stdafx.h"
//#include "pscan.h"
#include "vpcommon.h"
#include <locale.h>
//#include "nts.h"
#define AMSDYNAMICUSAGE

#include "AvisEventDetection.h"   // in QuarantineServer\Include
#include "EventObject.h"

#include "alrtprot.h"
//#include "msgsys.h"
typedef long moduleId;       // module id
typedef long messageType;                // message type
#include "amslib.h"

#include "EventData.h"
#include "vpstrutils.h"


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

//MNTS Functions
typedef int (*tNTS_GetHostAddress)(wchar_t *pWname, unsigned char protocol, CBA_NETADDR *pNetAddr);
typedef int (*tNTSBuildAddr)(CBA_NETADDR *pNetAddr,CBA_Addr *pCbaAddr,
							unsigned short port,unsigned char protocol);

tNTS_GetHostAddress NTS_GetHostAddress;
tNTSBuildAddr		NTSBuildAddr;

DWORD AmsActive=0;
BOOL bInitialized = FALSE;
/****************************************************************************************/
#ifdef WIN32
/****************************************************************************************/
HINSTANCE AMSLibHan=0;
HINSTANCE hAMSUI=0;
HINSTANCE hMNTS=0;
DWORD LoadAMSFunctions(void) {


    // Make sure that we only load these functions once. We may need to call
    // this function from more than one place, but we don't need to reload
    // all of the functions from these DLLs every time.
    if ( !bInitialized )
    {

        // Load all AMSLIB functions that we are going to use.

        AMSLibHan = LoadLibrary(_T("amslib.dll"));

	    if (AMSLibHan == NULL)
		    return ERROR_NO_AMSLIB;

	    if ((bAMSOriginatorRegister = (tbAMSOriginatorRegister)GetProcAddress(AMSLibHan,"bAMSOriginatorRegister")) == NULL) return ERROR_BAD_AMSLIB;
	    if ((bAMSOriginatorRegisterEh = (tbAMSOriginatorRegisterEh)GetProcAddress(AMSLibHan,"bAMSOriginatorRegisterEh")) == NULL) return ERROR_BAD_AMSLIB;
	    if ((pFindDelimiter = (tpFindDelimiter)GetProcAddress(AMSLibHan,"pFindDelimiter")) == NULL) return ERROR_BAD_AMSLIB;
	    if ((AMSFindNumberOfParams = (tAMSFindNumberOfParams)GetProcAddress(AMSLibHan,"AMSFindNumberOfParams")) == NULL) return ERROR_BAD_AMSLIB;
	    if ((AMSFindParamByNumber = (tAMSFindParamByNumber)GetProcAddress(AMSLibHan,"AMSFindParamByNumber")) == NULL) return ERROR_BAD_AMSLIB;
	    if ((bAMSOriginatorUnregister = (tbAMSOriginatorUnregister)GetProcAddress(AMSLibHan,"bAMSOriginatorUnregister")) == NULL) return ERROR_BAD_AMSLIB;
	    if ((AMSIssueAlert = (tAMSIssueAlert)GetProcAddress(AMSLibHan,"AMSIssueAlert")) == NULL) return ERROR_BAD_AMSLIB;
	    if ((AMSIssueAlertEh = (tAMSIssueAlertEh)GetProcAddress(AMSLibHan,"AMSIssueAlertEh")) == NULL) return ERROR_BAD_AMSLIB;
	    if ((AMSIsAgentActive = (tAMSIsAgentActive)GetProcAddress(AMSLibHan,"AMSIsAgentActive")) == NULL) return ERROR_BAD_AMSLIB;
	    if ((AddFlagToString = (tAddFlagToString)GetProcAddress(AMSLibHan,"AddFlagToString")) == NULL) return ERROR_BAD_AMSLIB;
	    if ((AMSFindParamByName = (tAMSFindParamByName)GetProcAddress(AMSLibHan,"AMSFindParamByName")) == NULL) return ERROR_BAD_AMSLIB;
	    if ((AMSGetParamDataType = (tAMSGetParamDataType)GetProcAddress(AMSLibHan,"AMSGetParamDataType")) == NULL) return ERROR_BAD_AMSLIB;
	    if ((AMSGetParamDataName = (tAMSGetParamDataName)GetProcAddress(AMSLibHan,"AMSGetParamDataName")) == NULL) return ERROR_BAD_AMSLIB;
	    if ((AMSGetParamAlias = (tAMSGetParamAlias)GetProcAddress(AMSLibHan,"AMSGetParamAlias")) == NULL) return ERROR_BAD_AMSLIB;
	    if ((AMSGetParamDataLen = (tAMSGetParamDataLen)GetProcAddress(AMSLibHan,"AMSGetParamDataLen")) == NULL) return ERROR_BAD_AMSLIB;
	    if ((AMSGetParamData = (tAMSGetParamData)GetProcAddress(AMSLibHan,"AMSGetParamData")) == NULL) return ERROR_BAD_AMSLIB;
	    if ((AMSGetPastParamList = (tAMSGetPastParamList)GetProcAddress(AMSLibHan,"AMSGetPastParamList")) == NULL) return ERROR_BAD_AMSLIB;
	    if ((AMSRegisterAlert = (tAMSRegisterAlert)GetProcAddress(AMSLibHan,"AMSRegisterAlert")) == NULL) return ERROR_BAD_AMSLIB;
	    if ((AMSRegisterAlertEh = (tAMSRegisterAlertEh)GetProcAddress(AMSLibHan,"AMSRegisterAlertEh")) == NULL) return ERROR_BAD_AMSLIB;
	    if ((AMSAppendParam = (tAMSAppendParam)GetProcAddress(AMSLibHan,"AMSAppendParam")) == NULL) return ERROR_BAD_AMSLIB;
	    if ((AMSGetTextFromParam = (tAMSGetTextFromParam)GetProcAddress(AMSLibHan,"AMSGetTextFromParam")) == NULL) return ERROR_BAD_AMSLIB;
	    if ((AMSGetTimeStringFromTimeT = (tAMSGetTimeStringFromTimeT)GetProcAddress(AMSLibHan,"AMSGetTimeStringFromTimeT")) == NULL) return ERROR_BAD_AMSLIB;
	    if ((AMSGetDateStringFromTimeT = (tAMSGetDateStringFromTimeT)GetProcAddress(AMSLibHan,"AMSGetDateStringFromTimeT")) == NULL) return ERROR_BAD_AMSLIB;
	    if ((AMSProcessAlertMessage = (tAMSProcessAlertMessage)GetProcAddress(AMSLibHan,"AMSProcessAlertMessage")) == NULL) return ERROR_BAD_AMSLIB;
	    if ((AMSParseFormatString = (tAMSParseFormatString)GetProcAddress(AMSLibHan,"AMSParseFormatString")) == NULL) return ERROR_BAD_AMSLIB;
	    if ((AMSSendAlertAck = (tAMSSendAlertAck)GetProcAddress(AMSLibHan,"AMSSendAlertAck")) == NULL) return ERROR_BAD_AMSLIB;

        hMNTS = LoadLibrary(_T("NTS.dll"));
	    if (hMNTS == NULL)
		    return P_NO_DLL;

		if ((NTS_GetHostAddress = (tNTS_GetHostAddress)GetProcAddress(hMNTS,"NTS_GetHostAddress")) == NULL) return ERROR_BAD_AMSLIB;
	    if ((NTSBuildAddr= (tNTSBuildAddr)GetProcAddress(hMNTS,"NTSBuildAddr")) == NULL) return ERROR_BAD_AMSLIB;


        // Make sure that we are initialized as a console.
//        fnAMSUIInitializeConsole();

        bInitialized = TRUE;
    }

	return 0;
}
/****************************************************************************************/
DWORD UnloadAMSFunctions(void) {

	if (bInitialized)
	{
		if (AMSLibHan)  // causes crash,  for now leave it.
			FreeLibrary(AMSLibHan);
		if (hMNTS)
			FreeLibrary(hMNTS);
		bInitialized = FALSE;
	}

	return 0;
}
/****************************************************************************************/
#endif
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
BOOL GetCBAAddress( LPCTSTR szMachName, CBA_Addr* pCbaAddr)
{
	CBA_NETADDR netAddr = {0};
	wchar_t         wName[64] = {0};            // jhill 7/31/00 clear buff to nulls
	BYTE            protocol = CBA_PROTOCOL_IP;
	DWORD			dwStrLen = 0;

	dwStrLen = wcslen(wName);

	// convert machine name to wide character
	//        NTSmbstowcs(wName, szMachName, 32);
	ssStrnCpy(wName, szMachName, sizeof(wName));
	if ( dwStrLen >= 64)
		wName[63] = '\0';


	// Get the CBA Address
	if (!NTS_GetHostAddress(wName, CBA_PROTOCOL_IP, &netAddr))

	{
		if (!NTS_GetHostAddress(wName, CBA_PROTOCOL_IPX, &netAddr))
		{
			if (dwStrLen < 16) // terrym this will crash nts if a name length longer than 16 is passed in.
			{
//				if (!NTS_GetHostAddress(wName, CBA_PROTOCOL_NB, &netAddr))
//					return (FALSE);
//				protocol = CBA_PROTOCOL_NB;
			}
		}
		protocol = CBA_PROTOCOL_IPX;
	}

	NTSBuildAddr(&netAddr, pCbaAddr, 0, protocol);


	return(TRUE);
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
DWORD InstallAMSEx( void)
{
	USES_CONVERSION;

	DWORD   cc;
	CHAR    szText[MAX_NAME];
	char	szAlertName[MAX_NAME];
	BYTE    * pParamList;
	BYTE    * pEnd;
	QSAALERT * alert;
	int i=0;
	int j=0;
	BOOL r;
	CString sAppName=_Module.GetAMSServerName();
	CString sProductName= (_Module.GetQSEventAppName());
	int iSize;
	//	dprintf("loading AMS functions\n");
	cc = LoadAMSFunctions();
	if (cc)
		return cc;
	setlocale(LC_ALL, "");

    // Resolve the computer name into a CBA address.
    CBA_Addr rRemoteAMSServerAddress;
    GetCBAAddress( sAppName, &rRemoteAMSServerAddress );

    // Make the AMS style product (application) name.
	sprintf( AppName, "--%s", T2A(sProductName) );
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

					memset(alert->name,0,sizeof(alert->name));
					ssStrnCpy(alert->name, lpTable->lpszEventName, sizeof(alert->name));
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
                    cc = AMSRegisterAlertEh(&rRemoteAMSServerAddress, NULL, AppName,0,iSize+1,szAlertName,pParamList,0);

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

					memset(alert->name,0,sizeof(alert->name));
					ssStrnCpy(alert->name, lpTable->lpszEventName, sizeof(alert->name));
					for (i=0;i<alert->count;i++)
                    {
						strcpy(szText,"  ");
						strncat(szText,T2A(alert->prams[i].name),MAX_AMS_EVENTNAME);
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
/****************************************************************************************/
DWORD UninstallAMS(void) {

	if (AmsActive)
	{
		bAMSOriginatorUnregister();

		AmsActive = 0;
	}


	Sleep(1000);

	UnloadAMSFunctions();
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
BOOL StartAMSEx(void)
{

	DWORD cc;

//	REF(nothing);
//	NTxSleep(1000);
//	dprintf("Registering with AMS\n");

    // Load AMS, and register our alerts.
	cc = InstallAMSEx( );


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


//////////////////////////////////////////////////////////////////////////////////////
//
//  Function : SendQSGeneralAMSEvent
//
//  Purpose : Sends an alert to the remote machine specified under the
//            product name specified.
//
//  Parameters :
//
//    LPTSTR  szRemoteMachineName - Remote machine to send alert to.
//    LPCTSTR szProductName       - Product to send an alert for.
//
//  Return Values :
//
//    None.
//
//////////////////////////////////////////////////////////////////////////////////////
void SendQSGeneralAMSEvent(CEvent  *pData, CString& sTitle)
{
	char    szText[MAX_NAME];
	BYTE    *pParamList;
	BYTE    *pEnd;
	QSAALERT  *alert;
	int     i;
	BYTE    sev;
    char    szParamString[1024];    // MAX_PATH  jhill 2/29/00
	DWORD   cc;
	TCHAR szComputerName[MAX_COMPUTERNAME_LENGTH + 1];
	DWORD dwComputerNameSize = sizeof(szComputerName);

	if (_Module.IsSESAAgentPresent())
	{
		QSSesGenEvent SesGenEvent(_Module.m_pQSSesInterface);


		if ((SesGenEvent.SendQSSESEvent(&pData->m_AvisAlertEvent)!= S_OK) &&
			((_Module.m_ulItemInterfaceTrace & DEBUGLEVEL_LOW) == DEBUGLEVEL_LOW))
		{
			fWidePrintString("SendQSGeneralAMSEvent Unable to send SESA General Event");
		}
	}
	if (AmsActive == FALSE)
		return;
	USES_CONVERSION;
	setlocale(LC_ALL, "");

	// Resolve the computer name into a CBA address.
    CBA_Addr rRemoteAMSServerAddress;
    GetCBAAddress( _Module.GetAMSServerName(), &rRemoteAMSServerAddress );

    // Make the AMS style product (application) name.
	strcpy( AppName, "  " );
	strcat( AppName+2, T2A(_Module.GetQSEventAppName()));
	AddFlagToString( (BYTE *)AppName, STRING_MODE_ASCII);


    // Hard code the alert we are going to send to the first one.
	alert = &QSAlertBlocks [1];

    // JHILL 2/12/00 COPY IN THE ACTUAL EVENT NAME
    memset( alert->name, 0, sizeof(alert->name) );
    if( !pData->m_AvisAlertEvent.sEventName.IsEmpty() )
		ssStrnCpy( alert->name, pData->m_AvisAlertEvent.sEventName, sizeof(alert->name));


    GetComputerName( szComputerName, &dwComputerNameSize);

	switch (pData->m_AvisAlertEvent.dwAlertSeverityLevel)
	{
	case _SEVERITY_CRITICAL:
		sev = SEVERITY_CRITICAL;
		break;
	case _SEVERITY_MAJOR:
	case _SEVERITY_MINOR:
		sev = SEVERITY_NON_CRITICAL;
		break;
	case _SEVERITY_INFORMATIONAL:
	case _SEVERITY_UNKNOWN:
		sev = SEVERITY_INFORMATION;
		break;
	default:
		sev = SEVERITY_INFORMATION;
	}


	pParamList = (BYTE *) malloc(MAX_CONFIG_PARAMS*2);
	if(pParamList)
    {
		if (alert->count)
        {
			memset(pParamList, 0, MAX_CONFIG_PARAMS*2);
            // Next, append our Hostname variable.
//            AppendHostnameParameter( pParamList, pEnd, TRUE );
			for (pEnd=NULL,i=0;i<alert->count;i++)
            {
				strcpy(szParamString ,"--");
				switch(i)
				{
				case 0:
					strcat( szParamString + 2, T2A(sTitle));
					break;
				case 1:
					strcat( szParamString + 2, T2A(szComputerName));
					break;
				case 2:
					strcat( szParamString + 2, T2A(pData->m_AvisAlertEvent.sMachineName));
					break;
				case 3:
					strcat( szParamString + 2, T2A(pData->m_AvisAlertEvent.sMachineAddress));
					break;
				case 4:
					strcat( szParamString + 2, T2A(pData->m_AvisAlertEvent.sMessage));
					break;
				case 5:
					strcat( szParamString + 2, T2A(pData->m_AvisAlertEvent.sDefaultOrHelpMsg));
					break;

				}
				strcpy(szText,"  ");
				strncat(szText,T2A(alert->prams[i].name),MAX_AMS_EVENTNAME);
				AddFlagToString((BYTE *)szText,STRING_MODE_ASCII);

                // Append this parameter to our AMS style parameter block.
				pEnd = AMSAppendParam(pParamList, pEnd, alert->prams[i].type,(BYTE *)szText, NULL, (WORD)(strlen( szParamString ) + 1), (BYTE*)szParamString , MAX_CONFIG_PARAMS*2);
			}

            // Issue this alert to the remote AMS Server.
            // Fix the calculation of alert->name to work with the new AMS lib(Payson). With the old calculation registration fails.
            memset(szText, 0, sizeof(szText));
            int iSize = strlen(T2A(alert->name))+1;
			ssStrnCpy(szText,T2A( alert->name),sizeof(szText));

            cc = AMSIssueAlertEh( &rRemoteAMSServerAddress,
                             NULL,
                             (AppName),
                             0,
                             (BYTE)iSize,
                             szText,
                             time(NULL),
                             pParamList,
                             sev,
                             0 );
		}


        // Free the buffer that we allocated for our parameter block.
    	free(pParamList);
	}

}
//////////////////////////////////////////////////////////////////////////////////////
//
//  Function : SendQSSampleAMSEvent
//
//  Purpose : Sends an alert to the remote machine specified under the
//            product name specified.
//
//  Parameters :
//
//    LPTSTR  szRemoteMachineName - Remote machine to send alert to.
//    LPCTSTR szProductName       - Product to send an alert for.
//
//  Return Values :
//
//    None.
//
//////////////////////////////////////////////////////////////////////////////////////
void SendQSSampleAMSEvent(CEvent  *pData , CString& sTitle)
{
	char    szText[MAX_NAME];
	BYTE    *pParamList;
	BYTE    *pEnd;
	QSAALERT  *alert;
	int     i;
	BYTE    sev;
    char    szParamString[1024];  // jhill 2/28/00      MAX_PATH
	DWORD   cc;
    DWORD   dwLenAllStrings  = 0;        // jhill 2/28/00
    DWORD   dwLenAllText     = 0;        // jhill 2/28/00
    DWORD   dwParmListLength = 0;        // jhill 2/28/00
	TCHAR szComputerName[MAX_COMPUTERNAME_LENGTH + 1];
	DWORD dwComputerNameSize = sizeof(szComputerName);
	if (AmsActive == FALSE)
		return;
	USES_CONVERSION;
	setlocale(LC_ALL, "");

	if (_Module.IsSESAAgentPresent())
	{
		QSSesSampleEvent SesSampleEvent(_Module.m_pQSSesInterface);


		if ((SesSampleEvent.SendQSSESEvent(&pData->m_AvisAlertEvent)!= S_OK) &&
			((_Module.m_ulItemInterfaceTrace & DEBUGLEVEL_LOW) == DEBUGLEVEL_LOW))
		{
			fWidePrintString("SendQSSampleAMSEvent Unable to send SESA Sample Event");
		}
	}
    memset( szParamString, 0, sizeof( szParamString ) );  // jhill 2/28/00


	// Resolve the computer name into a CBA address.
    CBA_Addr rRemoteAMSServerAddress;
    GetCBAAddress( _Module.GetAMSServerName(), &rRemoteAMSServerAddress );

    // Make the AMS style product (application) name.
	strcpy( AppName, ("  ") );
	strcat( AppName+2, T2A(_Module.GetQSEventAppName()));
	AddFlagToString( (BYTE *)AppName, STRING_MODE_ASCII);


    // Hard code the alert we are going to send to the first one.
	alert = &QSAlertBlocks [0];

    // JHILL 2/12/00 COPY IN THE EVENT NAME
    //fWidePrintString( "Ams.cpp  Event ID= %d  Name= %s", pData->m_AvisAlertEvent.dwEventId, pData->m_AvisAlertEvent.sEventName );
    memset( alert->name, 0, sizeof(alert->name) );
    if( !pData->m_AvisAlertEvent.sEventName.IsEmpty() )
		ssStrnCpy( alert->name, pData->m_AvisAlertEvent.sEventName,sizeof(alert->name));

    GetComputerName( szComputerName, &dwComputerNameSize);

	switch (pData->m_AvisAlertEvent.dwAlertSeverityLevel)
	{
	case _SEVERITY_CRITICAL:
		sev = SEVERITY_CRITICAL;
		break;
	case _SEVERITY_MAJOR:
	case _SEVERITY_MINOR:
		sev = SEVERITY_NON_CRITICAL;
		break;
	case _SEVERITY_INFORMATIONAL:
	case _SEVERITY_UNKNOWN:
		sev = SEVERITY_INFORMATION;
		break;
	default:
		sev = SEVERITY_INFORMATION;
	}

    // TEST MAKING STRING TOO LONG jhill 2/29/00
    // pData->m_AvisAlertEvent.m_sNote = pData->m_AvisAlertEvent.sMessage;


    // AMS APPEARS TO HAVE A MAX AROUND 340 - 400 FOR THE COMBINED LENGTH OF ALL THE TEXT STRINGS PASSED IN jhill 2/29/00
    dwLenAllStrings += strlen( T2A(sTitle ));
    dwLenAllStrings += strlen( T2A(szComputerName));
    dwLenAllStrings += strlen( T2A(pData->m_AvisAlertEvent.sMachineName ));
    dwLenAllStrings += strlen( T2A(pData->m_AvisAlertEvent.sMachineAddress ));
    dwLenAllStrings += strlen( T2A(pData->m_AvisAlertEvent.sUser ));
    dwLenAllStrings += strlen( T2A(pData->m_AvisAlertEvent.sPlatformName ));
    dwLenAllStrings += strlen( T2A(pData->m_AvisAlertEvent.sFileName ));
    dwLenAllStrings += strlen( T2A(pData->m_AvisAlertEvent.sVirusName ));
    dwLenAllStrings += strlen( T2A(pData->m_AvisAlertEvent.sDefinitions ));
    dwLenAllStrings += strlen( T2A(pData->m_AvisAlertEvent.sSampleStatus ));
    //dwLenAllStrings += strlen( T2A(pData->m_AvisAlertEvent.m_sSampleState ));
    dwLenAllStrings += strlen( T2A(pData->m_AvisAlertEvent.sAgeOfSample ));
    dwLenAllStrings += strlen( T2A(pData->m_AvisAlertEvent.sMessage ));
    dwLenAllStrings += strlen( T2A(pData->m_AvisAlertEvent.sDefaultOrHelpMsg ));
    dwLenAllStrings += strlen( T2A(pData->m_AvisAlertEvent.m_sNote ));


	pParamList = (BYTE *) malloc(MAX_CONFIG_PARAMS*2);
	if(pParamList)
    {
		if (alert->count)
        {
			memset(pParamList, 0, MAX_CONFIG_PARAMS*2);
			for (pEnd=NULL,i=0;i<alert->count;i++)
            {
				strcpy(szParamString ,("--"));
				switch(i)
				{
				case 0:
					strcat( szParamString + 2, T2A(sTitle));
					break;
				case 1:
					strcat( szParamString + 2, T2A(szComputerName));
					break;
				case 2:
					strcat( szParamString + 2, T2A(pData->m_AvisAlertEvent.sMachineName));
					break;
				case 3:
					strcat( szParamString + 2, T2A(pData->m_AvisAlertEvent.sMachineAddress));
					break;
				case 4:
					strcat( szParamString + 2, T2A(pData->m_AvisAlertEvent.sUser));
					break;
				case 5:
					strcat( szParamString + 2, T2A(pData->m_AvisAlertEvent.sPlatformName));
					break;
				case 6:
					strcat( szParamString + 2, T2A(pData->m_AvisAlertEvent.sFileName));
					break;
				case 7:
					strcat( szParamString + 2, T2A(pData->m_AvisAlertEvent.sVirusName));
					break;
				case 8:
					strcat( szParamString + 2, T2A(pData->m_AvisAlertEvent.sDefinitions));
					break;
				case 9:
					strcat( szParamString + 2, T2A(pData->m_AvisAlertEvent.sSampleStatus));
					break;
//				case 9:
//					strcat( szParamString + 2, T2A(pData->m_AvisAlertEvent.m_sSampleState));
//					break;
				case 10:
					strcat( szParamString + 2, T2A(pData->m_AvisAlertEvent.sAgeOfSample));
					break;
				case 11:
					strcat( szParamString + 2, T2A(pData->m_AvisAlertEvent.sMessage));
					break;
				case 12:
					strcat( szParamString + 2, T2A(pData->m_AvisAlertEvent.sDefaultOrHelpMsg));
					break;
				case 13:
					strcat( szParamString + 2, T2A(pData->m_AvisAlertEvent.m_sNote));
					break;
				}
				strcpy(szText,("  "));
				strncat(szText,T2A(alert->prams[i].name),MAX_AMS_EVENTNAME);
				AddFlagToString((BYTE *)szText,STRING_MODE_ASCII);

                dwLenAllText += strlen( szParamString ) +1;                          // jhill 2/29/00
                dwLenAllText += strlen( szText ) +1;                                 // jhill 2/29/00
            	//BYTE    *pEnd2 = pEnd;


                // Append this parameter to our AMS style parameter block.
				pEnd = AMSAppendParam(pParamList, pEnd, alert->prams[i].type,(BYTE *)szText, NULL, (WORD)(strlen( szParamString ) + 1), (BYTE*)szParamString , MAX_CONFIG_PARAMS*2);

                //if( pEnd2 != NULL )
                //    dwParmListLength += AMSGetParamDataLen( pEnd2 );                // jhill 2/29/00

			}

#ifdef _DEBUG
            //fWidePrintString( "Send to Ams - AllText= %d  AllCstrings= %d TotalParmLen= %d", dwLenAllText, dwLenAllStrings, dwParmListLength );
            fWidePrintString( "Send to Ams - Filename= %s  AllText= %d  AllCstrings= %d ", pData->m_AvisAlertEvent.sFileName, dwLenAllText, dwLenAllStrings );
#endif
            // Fix the calculation of alert->name to work with the new AMS lib(Payson). With the old calculation registration fails.
            memset(szText, 0, sizeof(szText));
            int iSize = strlen(T2A(alert->name))+1;
			ssStrnCpy(szText,T2A( alert->name),sizeof(szText));

            // Issue this alert to the remote AMS Server.
            cc = AMSIssueAlertEh( &rRemoteAMSServerAddress,
                             NULL,
                             (AppName),
                             0,
                             (BYTE)iSize,
                             szText,
                             time(NULL),
                             pParamList,
                             sev,
                              0 );
		}


        // Free the buffer that we allocated for our parameter block.
    	free(pParamList);
	}

}


