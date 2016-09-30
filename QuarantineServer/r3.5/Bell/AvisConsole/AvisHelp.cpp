/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/


/*--------------------------------------------------------------------
   AVISHELP.CPP

   Help routines to setup and call HtmlHelp(). 
   Actual call is made by passing the data to an MMC interface.


   Messages needing handlers on each Property Page:
   ON_WM_CONTEXTMENU                 ::OnContextMenu()
   ID_WHATS_THIS_MENUITEM:COMMAND    ::OnWhatsThisMenuitem()
   ON_WM_HELPINFO                    ::OnHelpInfo()
   OnNotify                          ::()

   Message Handlers on each Property Page:
   ::OnContextMenu()        Handles Prop Sheet Right Mouse Click on the property page. 
                            Not in an Edit box. Not on the Prop sheet buttons,
                            Calls DoHelpDisplayContextMenuWhatsThis() to display
                            a context menu with one item, ID_WHATS_THIS_MENUITEM.
                            Selection is handled below.
   ::OnWhatsThisMenuitem()  Called when the user selected "What's This?",
                            ID_WHATS_THIS_MENUITEM, after right click on the page.
                            Gives an overview of the page.

   ::OnNotify()             Calls DoHelpPropSheetHelpButton() to show
                            help for the Help Button. Gives an overview of the page.
                            
   ::OnHelpInfo()           Calls DoHelpPropSheetF1() to handle help for
                            the F1 key and for the user selecting the "?" mark
                            in the upper right corner. Able to give help
                            on any control or field on the page.

   Right mouse click "What's This?" menu is ID_WHATS_THIS_MENUITEM.

   Help TopicS
     LookupTopicInfo()      Needs HELP_ID_STRUCT *m_HelpIdArray defined for each
                            page and passed to it.




   Written by: Jim Hill                 
--------------------------------------------------------------------*/


#include "stdafx.h" 
#include "resource.h"
#include "avishelp.h"
#include "mmc.h"
#include "afx.h"
#include "htmlcall.h"
#include "ACHelp.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif                                      


//#include "QSHelp.h"
  
TCHAR szHelpFileName[MAX_PATH] = {0};

typedef struct tagavis_topic_IDs{
    DWORD    dwContrlID;
    DWORD    dwTopicID;
} AVIS_TOPIC_ID_STRUCT;

AVIS_TOPIC_ID_STRUCT  CurrentHelpTopic[] =
{
    0, 0,
    0, 0,
    0, 0
};
AVIS_TOPIC_ID_STRUCT * pCurrentHelpTopic = &CurrentHelpTopic[0];



AVIS_TOPIC_ID_STRUCT  HtmlHelpIdDwordArray[101] = {0};


// LOCAL FUNCTIONS
BOOL LookupTopicFileName( AVIS_HELP_ID_STRUCT *lpHelpIdArray, CString& sTopicFileName );
// FIND THE ID FROM THE POINT
DWORD GetDlgResID( CWnd* pWnd, CPoint point, DWORD* pdwTargetWnd );
// LOOK UP TOPIC FROM CONTROLID
DWORD LookupTopicInfo( AVIS_HELP_ID_STRUCT *lpHelpIdArray, DWORD dwControlID, 
                       CString& lpsTopicName, DWORD *lpdwHtmlHelpID = NULL );





// TEMP UNTIL INTEGRATION WITH DAVE'S CONSOLE
int ShowHelpMessageText(LPCTSTR lpszMsgText)
{
    HWND hWnd = GetActiveWindow();       //   MB_ICONSTOP
    int iRet  = 0;

    iRet=MessageBox( hWnd, lpszMsgText, _T("Help Topic Name"), MB_ICONASTERISK | MB_OK ); 

    return(S_OK);
}



/*--------------------------------------------------------------------
   BuildHtmlHelpIdArray()
   Written by: Jim Hill                 
--------------------------------------------------------------------*/
BOOL BuildHtmlHelpIdDwordArray( AVIS_HELP_ID_STRUCT *lpHelpIdArray )
{
    int i = 0;
    memset( &HtmlHelpIdDwordArray, 0, sizeof(HtmlHelpIdDwordArray));

    for( i = 0; ; i++, lpHelpIdArray++ )
    {
        if( lpHelpIdArray->dwContrlID == 0xFFFF )
            break;

        // SKIP 0, 0
        if( lpHelpIdArray->dwContrlID == 0 && lpHelpIdArray->dwTopicID == 0 )
        {
            if( i > 0 )
                --i;
            continue;
        }

        HtmlHelpIdDwordArray[i].dwContrlID = lpHelpIdArray->dwContrlID;
        HtmlHelpIdDwordArray[i].dwTopicID  = lpHelpIdArray->dwTopicID;
    }

    return(TRUE);
}


/*--------------------------------------------------------------------
   DisplayHtmlTopicHelp()
   Written by: Jim Hill                 
--------------------------------------------------------------------*/
BOOL DisplayHtmlTopicHelp( DWORD dwControlID, DWORD dwHtmlHelpID, DWORD dwHtmlCommand, CString& sTopicName )
{
    char szPath[512];
    CString sTopic;
    DWORD dwData = 0;
    HWND hWnd = NULL;

    // INITIALIZE
    sTopic = _T(" ");

    if( sTopicName.IsEmpty() )
        return(FALSE);

    hWnd = GetActiveWindow();
    GetHelpfileName();
    sTopic.Format( _T("%s::/%s"), szHelpFileName, sTopicName);
    //sTopic.Format( _T("%s::/%s"), szHelpFileName, _T("Acon_Overview.htm"));


#ifdef _UNICODE
     int iRet = WideCharToMultiByte(CP_ACP,0,(LPCWSTR)sTopic.GetBuffer(0),-1,
                                   (LPSTR)szPath,sizeof(szPath),NULL,NULL);
     sTopic.ReleaseBuffer(-1);
     if( iRet == FALSE )
        return(FALSE);
#else
    vpstrncpy( szPath, sTopic.GetBuffer(0),sizeof(szPath) );
    sTopic.ReleaseBuffer(-1);
#endif

#ifdef  CALL_HTML_HELP  
    CallHtmlHelpTopic( hWnd, szPath, 0, dwData );
#else
    ShowHelpMessageText( (LPCTSTR) sTopicName );
#endif
    return(TRUE);
}


/*--------------------------------------------------------------------
   DisplayHtmlContextHelp()
   Written by: Jim Hill                 
--------------------------------------------------------------------*/
BOOL DisplayHtmlContextHelp( HWND hWnd, DWORD dwControlID, DWORD dwHtmlHelpID, DWORD dwHtmlCommand, 
                             CString& sTopicName, AVIS_HELP_ID_STRUCT *lpHelpIdArray )
{
    CString sTopic2 = _T(" ");
    DWORD dwData = 0;
    //HWND hWnd = NULL;
    char szPath[512];

    if( hWnd == NULL )
        hWnd = GetActiveWindow();
    GetHelpfileName();
    sTopic2.Format( _T("%s::/%s"), szHelpFileName, AVIS_CONTEXT_FILENAME );  


#ifdef _UNICODE
     int iRet = WideCharToMultiByte(CP_ACP,0,(LPCWSTR)sTopic2.GetBuffer(0),-1,
                                   (LPSTR)szPath,sizeof(szPath),NULL,NULL);
     sTopic2.ReleaseBuffer(-1);
     if( iRet == FALSE )
        return(FALSE);                
#else
    vpstrncpy( szPath, sTopic2.GetBuffer(0), sizeof(szPath) );
    sTopic2.ReleaseBuffer(-1);
#endif


    //pCurrentHelpTopic->dwContrlID = dwControlID;
    //pCurrentHelpTopic->dwTopicID  = IDH_Quar_Server_Folder;
    //dwData = (DWORD)(LPVOID) pCurrentHelpTopic;

#ifdef  CALL_HTML_HELP  
    BuildHtmlHelpIdDwordArray( lpHelpIdArray );
    dwData = (DWORD)(LPVOID) &HtmlHelpIdDwordArray;
    CallHtmlHelpContext( hWnd, szPath, 0, dwData );
#else
    ShowHelpMessageText( (LPCTSTR) sTopicName );
#endif
    return(TRUE);
}




/*--------------------------------------------------------------------
   GetHelpfileName()
   Return full path to the help file
   Written by: Jim Hill                 
--------------------------------------------------------------------*/
TCHAR *GetHelpfileName()
{
    if( szHelpFileName[0] == 0 )
        wsprintf( szHelpFileName, _T("%s\\%s"), g_szMorseFullPath, AVIS_HELPFILE_NAME );

    return( szHelpFileName );
}

/*--------------------------------------------------------------------
   DoHelpPropSheetHelpButton()
   Handles the help button on the property sheets.
   
   Written by: Jim Hill                 
--------------------------------------------------------------------*/
BOOL DoHelpPropSheetHelpButton(DWORD dwControlID, AVIS_HELP_ID_STRUCT *lpHelpIdArray, NMHDR* pNMHDR )
{
     CString  sTopicName;
     DWORD    dwRet = 0;
     BOOL     bRet = FALSE;
     DWORD    dwHtmlHelpID = 0;

    // VALIDATE
    //if(dwControlID == 0 )     
    //    return(S_FALSE);
    // WAS HELP BUTTON ON PROPERTY SHEET PRESSED?
    if( pNMHDR->code != PSN_HELP)   //********* in "Prsht.h"
        return(S_FALSE);

    // dwRet = LookupTopicInfo( lpHelpIdArray, dwControlID, sTopicName, &dwHtmlHelpID );

    bRet = LookupTopicFileName( lpHelpIdArray, sTopicName );

    if( bRet )  //dwRet == S_OK
        DisplayHtmlTopicHelp( dwControlID, dwHtmlHelpID, 0, sTopicName );  // CallHtmlHelp
    //else
    //    // DISPLAY ERROR

    return(S_OK);
}


//     
/*--------------------------------------------------------------------
   DoHelpPropSheetF1()
   For Poperty Sheets,, handles both F1 and "What's This?".
   The "What's This?" is from selecting the '?' in upper right corner.

   Written by: Jim Hill                 
--------------------------------------------------------------------*/
BOOL DoHelpPropSheetF1( DWORD dwDefaultID, AVIS_HELP_ID_STRUCT *lpHelpIdArray, HELPINFO* pHelpInfo )
{
    return FALSE;
	CString  sTopicName;
    DWORD    dwRet = 0;
    DWORD    dwHtmlHelpID = 0;
    DWORD    dwControlID = 0;

    if( pHelpInfo->iContextType != HELPINFO_WINDOW)
        return(FALSE);

    HANDLE  hWinHandle = pHelpInfo->hItemHandle;
//    CWnd*  pTempWin = new CWnd;
//    if( pTempWin!=NULL )
//        pTempWin->Attach(hWinHandle);


    if( pHelpInfo->iCtrlId == -1)
        pHelpInfo->iCtrlId = dwDefaultID; // USE THE DEFAULT FOR THE PROP PAGE

    dwRet = LookupTopicInfo(lpHelpIdArray, pHelpInfo->iCtrlId , sTopicName, &dwHtmlHelpID );
    dwControlID = pHelpInfo->iCtrlId;
    // IF IT FAILED, RETRY WITH THE DEFAULT
    if( dwRet != S_OK && pHelpInfo->iCtrlId != (int) dwDefaultID )
    {
        dwRet = LookupTopicInfo(lpHelpIdArray, dwDefaultID , sTopicName, &dwHtmlHelpID );
        dwControlID = dwDefaultID;
    }

    if( dwRet == S_OK )
        DisplayHtmlContextHelp( (HWND)hWinHandle, dwControlID, dwHtmlHelpID, 0, sTopicName, lpHelpIdArray );   // HH_TP_HELP_CONTEXTMENU  //  CallHtmlHelp

    return(TRUE);
}



/*--------------------------------------------------------------------

   Written by: Jim Hill                   g_AvisSampleAttributeNamesArray
--------------------------------------------------------------------*/
DWORD LookupTopicIDFromAttributeName( TCHAR* lpszAttributeName )
{
    AVIS_TOPIC_FILENAMES_STRUCT *pSampleAttributeNames = &g_AvisSampleAttributeNamesArray[0];
    DWORD  dwTopicID = 0;
    int    i         = 0;

    if( lpszAttributeName == NULL )
        return( 0 );

    try
    {
        //  SEARCH THROUGH THE ARRAY  
        for( i = 0; ; i++, pSampleAttributeNames++ ) 
        {
            // ARE WE AT THE END?
            if( pSampleAttributeNames->dwTopicNumber == 0xFFFF || pSampleAttributeNames->lpszTopicFilename == NULL )
            {
                return( IDH_ATTRIBUTE_NO_HELP_AVAILABLE );
            }

            // IS THIS THE RIGHT ATTRIBUTE NAME?
            if( STRCMP( lpszAttributeName, pSampleAttributeNames->lpszTopicFilename ) == 0)
            {
                // RETURN THE TOPICID
                return( pSampleAttributeNames->dwTopicNumber );
            }

        }
    }
    catch(...)
    {
    }
    return(0);
}


/*--------------------------------------------------------------------
   DoHelpAttributeNameContextHelp()
   Provide Context help for individual attribute names in Attribute Listbox.
   Written by: Jim Hill               
--------------------------------------------------------------------*/
BOOL DoHelpAttributeNameContextHelp( DWORD dwControlID, TCHAR* lpszAttributeName, CWnd* pWnd, HWND hTargetWnd ) 
{
    CString sTopic;
    DWORD  dwData      = 0;
    DWORD  dwTopicID   = 0;
    char   szPath[512];

    if( hTargetWnd == NULL && pWnd != NULL )
        hTargetWnd = pWnd->GetSafeHwnd();

    if( dwControlID == 0 && pWnd != NULL )
        dwControlID = pWnd->GetDlgCtrlID();
    if( dwControlID == 0 )
        return(FALSE);

    GetHelpfileName();
    sTopic.Format( _T("%s::/%s"), szHelpFileName, AVIS_CONTEXT_FILENAME );  
#ifdef _UNICODE
     int iRet = WideCharToMultiByte(CP_ACP,0,(LPCWSTR)sTopic.GetBuffer(0),-1,
                                   (LPSTR)szPath,sizeof(szPath),NULL,NULL);
     sTopic.ReleaseBuffer(-1);
     if( iRet == FALSE )
        return(FALSE);                
#else
    vpstrncpy( szPath, sTopic.GetBuffer(0), sizeof(szPath) );
    sTopic.ReleaseBuffer(-1);
#endif

    // LOOKUP THE TOPIC ID
    dwTopicID = LookupTopicIDFromAttributeName( lpszAttributeName );
    if( dwTopicID == 0 )
        return(FALSE);

    // SET THE CONTROL ID TO WHAT WE NEED
    // int iCurrentCtrlID = pWnd->SetDlgCtrlID(  );

    pCurrentHelpTopic->dwContrlID = dwControlID;
    pCurrentHelpTopic->dwTopicID  = dwTopicID;
    dwData = (DWORD)(LPVOID) pCurrentHelpTopic;

    // NOW, FAKE OUT HTMLHELP
    CallHtmlHelpContext( hTargetWnd, szPath, 0, dwData );


    // RESET THE CONTROL ID
    // pWnd->SetDlgCtrlID( iCurrentCtrlID );
    return(TRUE);
}


/*--------------------------------------------------------------------
   DoHelpPropSheetContextHelp()
   Handles Prop Sheet Right Mouse Click on the property page. 
   Not in an Edit box. Not on the Prop sheet buttons,
   Called from ::OnWhatsThisMenuitem()

   Written by: Jim Hill                 CWnd* pWnd = NULL, CPoint point = 0
--------------------------------------------------------------------*/
BOOL DoHelpPropSheetContextHelp( DWORD dwControlID, AVIS_HELP_ID_STRUCT *lpHelpIdArray, CWnd* pWnd, HWND hTargetWnd )
{
    CString  sTopicName;
    DWORD    dwRet = 0;
    DWORD    dwHtmlHelpID = 0;
    DWORD    dwDlgCtrlID = 0;

#ifdef  CALL_HTML_HELP  
    if( pWnd )
        dwDlgCtrlID = pWnd->GetDlgCtrlID();    // THIS WILL BE 0, WHEN VURSOR ON ONLY THE PROP PAGE
#endif

    if( hTargetWnd == NULL )
        hTargetWnd = pWnd->GetSafeHwnd();

    dwRet = LookupTopicInfo(lpHelpIdArray, dwControlID, sTopicName, &dwHtmlHelpID );

    if( dwRet == S_OK  )  // && dwDlgCtrlID != 0
        DisplayHtmlContextHelp( hTargetWnd, dwControlID, dwHtmlHelpID, 0, sTopicName, lpHelpIdArray );   // CallHtmlHelp

    return(TRUE);
}


/*--------------------------------------------------------------------

   Written by: Jim Hill                 
--------------------------------------------------------------------*/



// Handles requests for Help forwarded by MMC to CSnapper1Data::Notify
//BOOL DoHelpMmcContextHelp()
//{
//    return(TRUE);
//}


/*--------------------------------------------------------------------
   GetDlgResID()
   Try and determine the resource ID from the position on the screen.

   Written by: Jim Hill                 
--------------------------------------------------------------------*/
DWORD GetDlgResID(CWnd* pWnd, CPoint point, DWORD* pdwTargetWnd)
{
    CPoint pointClicked;
    CRect  rcChild;
    DWORD  dwHelpID = 0;

    // INITIALIZE
    pointClicked = point;
    pWnd->ScreenToClient(&pointClicked);

    // Check to see if the point is in the dialog
    pWnd->GetClientRect(&rcChild);       
    if( !rcChild.PtInRect(pointClicked) )
        return(0);

    CWnd* pTargetWnd = pWnd->ChildWindowFromPoint( pointClicked, CWP_SKIPTRANSPARENT | CWP_SKIPINVISIBLE );  // CWP_SKIPDISABLED
//  CWnd* pTargetWnd = pWnd->ChildWindowFromPoint( pointClicked, CWP_SKIPINVISIBLE );
    if( pTargetWnd == NULL )
        return(0);

    if( pdwTargetWnd != NULL )
        *pdwTargetWnd = (DWORD)(LPVOID) pTargetWnd->GetSafeHwnd();

    dwHelpID = pTargetWnd->GetWindowContextHelpId();

    return(dwHelpID);
}



/*--------------------------------------------------------------------
   DoHelpDisplayContextMenuWhatsThis()
   Adds menu item ID_WHATS_THIS_MENUITEM as "What's This?".
   Called from each property page.

   Written by: Jim Hill                 
--------------------------------------------------------------------*/
BOOL DoHelpDisplayContextMenuWhatsThis( DWORD dwDefaultID, DWORD *lpdwResID, AVIS_HELP_ID_STRUCT *lpHelpIdArray,
                                        HWND hPropPageWnd, CWnd* pWnd, CPoint point, DWORD dwForcedDefault, 
                                        DWORD* pdwTargetWnd )
{
    CString  sTopicName, sMenuText;
    DWORD    dwRet = 0;
    BOOL     bRet = 0;
    DWORD    nID = 0;      
    CWnd*    pWndForMenu = 0;
    DWORD    dwHtmlHelpID = 0;

// disable
	return FALSE;
    if( pdwTargetWnd != NULL )
        *pdwTargetWnd = (DWORD)(LPVOID) pWnd->GetSafeHwnd();

    dwForcedDefault = 0;   // DISABLE
    // IF IT'S NOT A FORCED DEFAULT, THEN TRY TO FIND THE ID
    // if( dwForcedDefault ) 
    // {
    //     nID = dwForcedDefault;
    // }
    // else
    {   // TRY TO FIND THE ID
        nID = pWnd->GetDlgCtrlID();
        // IF IT'S 0, THEN TRY TO GET THE HELPID 
        if( nID == 0)
        {
            nID = GetDlgResID( pWnd, point, pdwTargetWnd );
            if( nID == 0)                     // IF IT'S STILL 0, THEN  WE'RE POINTING TO THE PROP PAGE
            {
                //    nID = dwDefaultID;      // DEFAULT
                return(FALSE);                // DPN'T SHOW THE MENU. 6/5/99
            }
        }
    }

    // LOOKUP THE OWNER
    CWnd* pOwnerWnd = pWnd->GetOwner();
    if( pOwnerWnd != NULL )
    {   // IS THE PROP PAGE THE OWNER?
        if( pOwnerWnd->m_hWnd == hPropPageWnd)
            pWnd = pOwnerWnd;                  // IF SO POINT THE MSGS FROM MENU TO PARENT
    }

    // IF WE CAN'T LOOKUP THIS ID, DON'T DISPLAY THE MENU
    dwRet = LookupTopicInfo( lpHelpIdArray, nID, sTopicName, &dwHtmlHelpID );
    // IF IT FAILED, RETRY WITH THE DEFAULT
    if( dwRet != S_OK && nID != dwDefaultID )
    {
        dwRet = LookupTopicInfo( lpHelpIdArray, dwDefaultID , sTopicName, &dwHtmlHelpID );
        if( dwRet == S_OK )
            nID = dwDefaultID;
    }

    // IF WE CAN'T LOOKUP THIS ID, DON'T DISPLAY THE MENU
    if( dwRet != S_OK )
        return(FALSE);

    // CREATE THE MENU
    CMenu *lpContextMenu = new CMenu;
    if( lpContextMenu == NULL )
        return(FALSE);
    lpContextMenu->CreatePopupMenu();
    sMenuText.LoadString(IDS_WHATS_THIS_MENUITEM);      //  _T("What's This?")
    lpContextMenu->AppendMenu( MF_STRING, ID_WHATS_THIS_MENUITEM, sMenuText );
    pWnd->DrawMenuBar();                                                          // 

    // TELL THE CALLER WE HAVE A VALID ID
    if( lpdwResID != NULL )
        *lpdwResID = nID;

    // DISPLAY THE MENU. IF SELECTED WILL CALL ::OnWhatsThisMenuitem()
    bRet = lpContextMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, pWnd, NULL );  

    //lpContextMenu->DestroyMenu();
    delete lpContextMenu;
    return(TRUE);
}

/*--------------------------------------------------------------------
   LookupTopicFileName()                                  
   Lookup the name of the HTML file
   Written by: Jim Hill                 
--------------------------------------------------------------------*/
BOOL LookupTopicFileName( AVIS_HELP_ID_STRUCT *lpHelpIdArray, CString& sTopicName )
{ 
    AVIS_TOPIC_FILENAMES_STRUCT *pTopicFileName = &g_AvisTopicFilenamesArray[0];
    DWORD dwTopicNumber = 0;          
    BOOL  bRet = FALSE;
    int   i, j;

    try
    {
        if( lpHelpIdArray == NULL)
            return(FALSE);
        sTopicName.Empty();

        //  SEARCH THROUGH THE ARRAY  
        for( i = 0; ; i++, lpHelpIdArray++ ) 
        {
            //  SEARCH THROUGH THE ARRAY FOR 0xFFFF.
            if( lpHelpIdArray->dwContrlID == 0xFFFF )
            {
                // THE TOPIC FILENAME IS THE NEXT ENTRY
                lpHelpIdArray++;
                dwTopicNumber = lpHelpIdArray->dwTopicID;
                if( dwTopicNumber == 0 || dwTopicNumber == 0xFFFF )
                    return(FALSE);
                for( j = 0; ; j++, pTopicFileName++)
                {
                    if( pTopicFileName->dwTopicNumber == 0xFFFF )
                        return(FALSE);
                    if( pTopicFileName->dwTopicNumber == dwTopicNumber && pTopicFileName->lpszTopicFilename != NULL)
                    {
                        sTopicName = pTopicFileName->lpszTopicFilename;
                        return(TRUE);
                    }
                }
                break;  // WE'RE AT THE END OF THE LIST, SO BAIL
            }
        }
    }
    catch(...)
    {
    }
    
    return(bRet);
}

/*--------------------------------------------------------------------
   LookupTopicInfo()                                  HELPINFO_WINDOW
   Lookup the text associated with a Control ID
   Written by: Jim Hill                 
--------------------------------------------------------------------*/
DWORD LookupTopicInfo( AVIS_HELP_ID_STRUCT *lpHelpIdArray, DWORD dwControlID, 
                       CString& sTopicName, DWORD *lpdwHtmlHelpID )
{
    BOOL  bRet = 0;

   // VALIDATE
   if(dwControlID == 0 )         // || lpHelpIdArray == NULL)
      return(S_FALSE);
   
try
{
   //  SEARCH THROUGH THE ARRAY 
   while(  lpHelpIdArray->dwContrlID != 0xFFFF ) {     //lpHelpIdArray->dwContrlID != 0 &&
      if( dwControlID == lpHelpIdArray->dwContrlID)  
      {
         if( lpdwHtmlHelpID != NULL )
             *lpdwHtmlHelpID = lpHelpIdArray->dwTopicID;

         if( lpHelpIdArray->dwTopicNameStringID != 0 )
         {
             bRet = sTopicName.LoadString(lpHelpIdArray->dwTopicNameStringID);
             if( bRet )
                 return(S_OK);
         }
         return(S_OK);
      }
      lpHelpIdArray++;
   }
}
catch(...)
{
}

   return(S_FALSE);
}





#if 0
    CWnd *pChild = pWnd->GetWindow(GW_CHILD);
    if(pChild)
        pChild = pChild->GetWindow(GW_HWNDFIRST);

    HWND  hMostEnclosedChild = NULL;
    int   iMostEnclosedHelpID = -1;
    CRect rcMostEnclosedChild;
    CRect rcResult;
    rcMostEnclosedChild.SetRectEmpty();

// CWnd* GetNextDlgGroupItem( CWnd* pWndCtl, FALSE );
// int GetDlgCtrlID( );
// CWnd* ChildWindowFromPoint( POINT point );
// DWORD GetWindowContextHelpId( );

    while(pChild)
    {
        // Get WindowRect and convert to parents coordinate system
        pChild->GetWindowRect(&rcChild);     
        pChild->ScreenToClient(&rcChild);

        if(rcChild.PtInRect(pointClicked) && pChild->IsWindowVisible())
        {
             // Compare this rect with the last one found
             rcResult.IntersectRect( &rcChild, &rcMostEnclosedChild );
             if( rcResult.IsRectEmpty() || rcResult.EqualRect(&rcChild) )
             {
                 iMostEnclosedHelpID = 0;

             }
        }

        pChild = pChild->GetWindow(GW_HWNDNEXT);
    }

    return(0);
#endif




#if 0
/*--------------------------------------------------------------------
   CallHtmlHelp()
   Written by: Jim Hill                 
--------------------------------------------------------------------*/
BOOL CallHtmlHelp( DWORD dwControlID, DWORD dwHtmlHelpID, DWORD dwHtmlCommand, CString& sTopicName )
{
    CString sTopic = _T(" ");
    DWORD dwData = 0;
    HWND hWnd = NULL;
    char szPath[512];

    hWnd = GetActiveWindow();
    GetHelpfileName();
    // sTopic.Format( _T("%s::/%s"), szHelpFileName, sTopicName);
    sTopic.Format( _T("%s::/%s"), szHelpFileName, _T("Quar_Overview.htm"));

    ShowHelpMessageText( (LPCTSTR) sTopicName );


#ifdef _UNICODE
     int iRet = WideCharToMultiByte(CP_ACP,0,(LPCWSTR)sTopic.GetBuffer(0),-1,
                                   (LPSTR)szPath,sizeof(szPath),NULL,NULL);
     sTopic.ReleaseBuffer(-1);
     if( iRet == FALSE )
        return(FALSE);
#else
    vpstrncpy( szPath, sTopic.GetBuffer(0), sizeof(szPath) );
    sTopic.ReleaseBuffer(-1);
#endif

#ifdef  CALL_HTML_HELP  
    // CHECK DEFAULT
    if( dwHtmlCommand == 0 )
        dwHtmlCommand = HH_DISPLAY_TOPIC;
    else if( dwHtmlCommand == HH_TP_HELP_CONTEXTMENU )
    {
        //CurrentHelpTopic.dwContrlID = dwControlID;
        //CurrentHelpTopic.dwTopicID  =;
        //dwData = (DWORD)(LPVOID) &CurrentHelpTopic;
    }

    HWND hHelpWin = NULL;
    hHelpWin =HtmlHelpA(
                    hWnd,
                    szPath,                // (LPCTSTR)sTopic,            // "c:\\Help.chm::/Intro.htm"
                    dwHtmlCommand,         // HH_DISPLAY_TOPIC
                    dwData) ;
#endif
    return(TRUE);
}
#endif





