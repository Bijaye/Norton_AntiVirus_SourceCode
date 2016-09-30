#undef _UNICODE
#include <windows.h>
#include <tchar.h>
//#include "stdafx.h"
#include "cba.h"
#include "amscons.h"

typedef LRESULT (WINAPI *tAMSUIManageAlerts)(LPMANAGE_ALERTS_DLG pMd);
typedef BOOL (*tAMSUIShowAlertLog)(AMS2_ALERT_LOG *pAMS2AlertLog);
typedef int (*tAMSUIInitializeConsole)(void);
typedef void (*tAMSUIDestroyConsole)(void);

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

/////////////////////////////////////////////////////////////////////////////
// Global AMS Handles.

HINSTANCE   g_hAMSUIDll = NULL;
HINSTANCE   g_hNTS = NULL;
BOOL        g_bAMSInitialized = FALSE;

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
     
        // convert machine name to wide character 
        NTSmbstowcs(wName, szMachName, 32);
     
        // Get the CBA Address
        if (!NTS_GetHostAddress(wName, CBA_PROTOCOL_IP, &netAddr)) 

        {
                if (!NTS_GetHostAddress(wName, CBA_PROTOCOL_IPX, &netAddr))
                        {
                                if (!NTS_GetHostAddress(wName, CBA_PROTOCOL_NB, &netAddr))
                                        return (FALSE);
                        protocol = CBA_PROTOCOL_NB;
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
        g_hNTS = LoadLibrary("NTS.dll");
	    if (g_hNTS == NULL)
		    return bRc;

		if (((NTS_GetHostAddress = (tNTS_GetHostAddress)GetProcAddress(g_hNTS ,"NTS_GetHostAddress")) != NULL) &&
			((NTSmbstowcs= (tNTSmbstowcs)GetProcAddress(g_hNTS ,"NTSmbstowcs")) != NULL) &&
			((NTSBuildAddr= (tNTSBuildAddr)GetProcAddress(g_hNTS ,"NTSBuildAddr")) != NULL))
		{
			g_hAMSUIDll = LoadLibrary("AMSUI.DLL") ;

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
						g_bAMSInitialized = TRUE;
						bRc = TRUE;
					}
				}
			}
		}
    }
	else
		bRc = TRUE;
	return bRc;
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

    if (!InitAMS())
		return hr;

    // Zero out the memory for this structure.
    ZeroMemory(&dlg, sizeof(MANAGE_ALERTS_DLG));

    // Lookup the address of the AMS server quarantine is configured to send alerts too.
	if (GetCBAAddress( szHostName, &dlg.address))
    {


		dlg.szHostName = (LPTSTR)(szHostName);

	    dlg.szAppName = (LPTSTR)_T("Central Quarantine");
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
	

    if (!InitAMS())
		return hr;


    // Zero out the memory for this structure.
    ZeroMemory(&dlg, sizeof(AMS2_ALERT_LOG));


    if ( GetCBAAddress( szHostName, &dlg.address ))
    {

        //Database ID not displayed text. IPE NO BUG
	    dlg.pszApplication = (LPTSTR)_T("Central Quarantine");
//	    dlg.pszApplication = NULL;
	    dlg.pszHost = (LPTSTR)szHostName;
	    dlg.pszCaption = (LPTSTR)szCaption;

        SetRect( &(dlg.rect), 0, 0, 640, 460 );

	    g_fnAMSUIShowAlertLog(&dlg);
    }


//	RemoveAMS( );
    return hr;
}

