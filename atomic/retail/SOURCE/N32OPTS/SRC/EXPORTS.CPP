/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/N32OPTS/VCS/EXPORTS.CPv   1.9   24 Jun 1998 15:34:26   PVENKAT  $
/////////////////////////////////////////////////////////////////////////////
//  Exports.CPP.  This file contains all of the exported functions for
//  N32OPTS.
//
//  NOTE:  The very first line of each exported function which may generate
//  calls into MFC must contain the following statement:
//
//          AFX_MANAGE_STATE(AfxGetStaticModuleState());
//
//  MFC needs this call in order to manage it's thread local state.  If you
//  omit this call, bad things will happen...
//
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/N32OPTS/VCS/EXPORTS.CPv  $
// 
//    Rev 1.9   24 Jun 1998 15:34:26   PVENKAT
// Fixed 118017.  For NEC, suppressed Innoculation Tab.
// 
// 
//    Rev 1.8   10 Apr 1998 00:27:38   MKEATIN
// Made inoculation related changes.
// 
//    Rev 1.6   11 Jul 1997 16:55:10   DBUCHES
// Added bIsNT parameter to NAVOptionsSheetEx()
// 
//    Rev 1.5   27 Jun 1997 13:01:42   DBUCHES
// Added support for specifiying active page.
// 
//    Rev 1.4   27 May 1997 20:55:58   DBUCHES
// I touched 30 files this rev.  Needed for support of NT Alerting.
// 
//    Rev 1.3   19 May 1997 20:37:54   DBUCHES
// Fixed problem with enabling password support.
// 
//    Rev 1.2   16 May 1997 18:37:28   DBUCHES
// Changed the way we deal with exclusions. 
//
//    Rev 1.1   16 May 1997 15:28:00   DBUCHES
// Removed unnecessary include
//
//    Rev 1.0   14 May 1997 14:58:12   DBUCHES
// Initial revision.
/////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"

#include "n32opts.h"
#include "sheet.h"
#include "navsheet.h"

#include "exclude.h"

/////////////////////////////////////////////////////////////////////////////
//
//  NAVOptionsSheet() - This function will display a property sheet allowing
//  a user to configure a NAVOPTIONS structure.  The pages inserted into
//  this sheet vary based on platform (Win 95 vs NT).  See NAVOptionsSheetEx()
//  for more info...
//
//  Parameters: hWnd -  Handle of window to use as parent for the property
//                      sheet dialog
//              pNavOpts - Pointer to a NAVOPTS structure.  This structure
//                      is used to populate the various pages of the property
//                      sheet.
//              iActivePage - 0 based index of active page
//
//
//  returns ERROR_SUCCESS if things went well.
//          IDCANCEL if the user hit cancel
//          ERROR_OUTOFMEMORY if things didn't...
//
/////////////////////////////////////////////////////////////////////////////
// Created 5/9/97 DBUCHES
/////////////////////////////////////////////////////////////////////////////
int DECLSPEC_NAVSHEET PASCAL NAVOptionsSheet(HWND hwnd, LPNAVOPTIONS pNavOpts, int iActivePage)
{
   	// Needed for MFC
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    int aiPages[N32OPTS_MAXPAGES];
    int iCount;
    
    // Allocate pages based on platform.  The Windows NT property sheet
    // does not contain a start up or inoculation page.
    if( IS_NT() )
        {
        aiPages[0] = N32OPTS_SCANNER_PAGE;
        aiPages[1] = N32OPTS_AP_PAGE;
        // bugbug:  need to use new define for NT super deluxe
        // alert page when it is ready.
        aiPages[2] = N32OPTS_ALERT_PAGE;
        aiPages[3] = N32OPTS_LOG_PAGE;
        aiPages[4] = N32OPTS_EXCLUDE_PAGE;
        aiPages[5] = N32OPTS_GENERAL_PAGE;
        aiPages[6] = N32OPTS_PASSWORD_PAGE;

        iCount = 7;
        }
    else
        {
        aiPages[0] = N32OPTS_SCANNER_PAGE;
        aiPages[1] = N32OPTS_AP_PAGE;
        aiPages[2] = N32OPTS_STARTUP_PAGE;
        aiPages[3] = N32OPTS_ALERT_PAGE;
        aiPages[4] = N32OPTS_LOG_PAGE;
        aiPages[5] = N32OPTS_EXCLUDE_PAGE;
        aiPages[6] = N32OPTS_GENERAL_PAGE;
        aiPages[7] = N32OPTS_PASSWORD_PAGE;
        if ( ! IS_NEC () )
        {
            aiPages[8] = N32OPTS_INOC_PAGE;
            iCount = 9 ;
        }
        else
            iCount = 8;
        }

    return NAVOptionsSheetEx( hwnd, pNavOpts, aiPages, iCount, iActivePage, IS_NT() );

}

/////////////////////////////////////////////////////////////////////////////
//
//  NAVOptionsSheet() - This function will display a property sheet allowing
//  a user to configure a NAVOPTIONS structure.
//
//
//  Parameters: hWnd -  Handle of window to use as parent for the property
//                      sheet dialog
//              
//              pNavOpts - Pointer to a NAVOPTS structure.  This structure
//                      is used to populate the various pages of the property
//                      sheet.
//              
//              iCount - Number of pages to display.
//              
//              *iPanels - Pointer to array of integers containing page IDs.
//                      The following are valid page IDs:
//
//                      N32OPTS_SCANNER_PAGE;
//                      N32OPTS_AP_PAGE;
//                      N32OPTS_STARTUP_PAGE;
//                      N32OPTS_ALERT_PAGE;
//                      N32OPTS_LOG_PAGE;
//                      N32OPTS_EXCLUDE_PAGE;
//                      N32OPTS_GENERAL_PAGE;
//                      N32OPTS_PASSWORD_PAGE;
//                      N32OPTS_INOC_PAGE;
//
//              iActivePanel - 0 based id of panel to make active.
//              
//              bIsNT - specify whether to use NT specific panels.
//
//  returns ERROR_SUCCESS if things went well.
//          IDCANCEL if the user hit cancel
//	    ERROR_OUTOFMEMORY if things didn't...
//
/////////////////////////////////////////////////////////////////////////////
// Created 5/9/97 DBUCHES
/////////////////////////////////////////////////////////////////////////////
int PASCAL NAVOptionsSheetEx(HWND hwnd, LPNAVOPTIONS pNavOpts, int *iPanels, int iCount, int iActivePage, BOOL bIsNT)
{
	// Needed for MFC
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    // Validate input
    if(iPanels == NULL || pNavOpts == NULL)
        {
        ASSERT(FALSE);
        return ERROR_OUTOFMEMORY;
        }

    // Create new options sheet.
    CNavOptionsSheet* dlg = new CNavOptionsSheet(IDS_OPTIONS_TITLE, CWnd::FromHandle(hwnd));
    if( dlg == NULL )
        {
        ASSERT(FALSE);
        return ERROR_OUTOFMEMORY;
        }

    // Make sure we use NT specific stuff.
    ((CN32optsApp*)AfxGetApp())->SetNTFlag(bIsNT);

    // Copy options to dialog.  We don't want to be partying on
    // these guys just yet...
    dlg->navOpts = *pNavOpts;

    // Add pages to property sheet.
    if(dlg->InsertPages(iCount, iPanels) != ERROR_SUCCESS)
        return ERROR_OUTOFMEMORY;

    // Add password to dialog.
    dlg->m_dwPassword = pNavOpts->password.dwPassword;

    // Enable password checking if password protection is on.
    dlg->m_bPasswordValid = !pNavOpts->password.bProtect;

    // Set active page 
    dlg->SetActivePage(iActivePage);
                                                
    // Fire off the dialog
    int iResult = dlg->DoModal();

    if(iResult == IDOK)
        {
        // Update copy of NAVOPTIONS.
        dlg->GatherData();

        // Copy data back to user buffer.  Note that the GatherData()
	    // function will update the exclusions.
        *pNavOpts = dlg->navOpts;

	    iResult = ERROR_SUCCESS;
	    }

    // Clean things up.
    delete dlg;

    // All good.  Return success.
    return iResult;
}



