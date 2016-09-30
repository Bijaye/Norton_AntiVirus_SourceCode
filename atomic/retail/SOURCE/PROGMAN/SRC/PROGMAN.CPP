// Copyright 1996 Symantec Corporation, Peter Norton Product Group
//***********************************************************************
//                                                                
// $Header:   S:/PROGMAN/VCS/progman.cpv   1.2   25 Apr 1997 13:24:48   JBRENNA  $ 
//                                                                
// Description:                                                   
//      This source file contains those routines necessary for    
//      Adding, Deleting - Items and Groups in Program Manager.
//                                                                
// Contains :           
//      ProgmanGroupManipulation(), GroupDDECallback()
//                                                                
//***********************************************************************
// $Log:   S:/PROGMAN/VCS/progman.cpv  $
// 
//    Rev 1.2   25 Apr 1997 13:24:48   JBRENNA
// Ops ... this time really add RestoreProgMan and MinimizeProgMan.
// 
//    Rev 1.1   16 Apr 1997 16:15:36   JBRENNA
// Port from Tesla: TSmith: Added functions to determine state of, minimize and
// maximize Program Manager.
// 
//    Rev 1.0   06 Feb 1997 21:08:22   RFULLER
// Initial revision
// 
//    Rev 1.11   27 Jul 1996 14:03:34   RPULINT
// Changed timeout limit for DDE operation to 10 minutes
// 
//    Rev 1.10   27 Jul 1996 13:12:02   RPULINT
// Fixed defect 62361: search for registry value of the common group keys in 
// LOCAL_MACHINE instead of CURRENT_USER in the case of NT4.0
// 
//    Rev 1.9   25 Jul 1996 13:46:40   RPULINT
// Fixed defect : 62131 - create "Startup" instead of "startup"
// Also changed Timeout values from TIMEOUT_ASYNC to TIMEOUT_ASYNC/2
// 
//    Rev 1.8   23 Jul 1996 19:12:30   RPULINT
// Defect 61712 Fixed: TimeOut Problem : changed the time limit from 
// 1000ms to TIMEOUT_ASYNC for all DdeClientTransaction() functions
// 
//    Rev 1.7   22 Jul 1996 09:45:04   RPULINT
// Fixed defect 61054: Added ProgmanGroupAddItemNT4()
// 
//    Rev 1.6   16 Jul 1996 11:08:32   RPULINT
// renamed GetProgramGroupName() to GetProgmanGroupName()
// 
//    Rev 1.5   16 Jul 1996 11:02:56   RPULINT
// Fixed defect 57791: resolved language dependent common startup group name
// problem
// 
//    Rev 1.4   15 Jul 1996 10:26:32   RPULINT
// Fixed 58146: Added a parameter to support common/private groups
// 
//    Rev 1.3   11 Jul 1996 09:21:12   RPULINT
// Fixes to defect 60116, 60114, 59916
// 
//    Rev 1.2   09 Jul 1996 17:50:48   RPULINT
// Fixed defects 59916, 60108, 60114, 60116
// deal NT3.51 & NT4.0 separately
// 
//    Rev 1.1   18 Jun 1996 14:10:54   RPULINT
// Defect 58336 - Added ProgmanQueryItemInGroup()
// 
//    Rev 1.0   11 Jun 1996 10:44:46   RPULINT
// Initial revision.

#include "platform.h"                   // Platform independence
#include "progman.h"                    
#include <shlguid.h>
#include <shlobj.h>
#include <objidl.h>

#define  COMMON_STARTUP_GROUP  "Common Startup"     // Registry Key NT4.0
#define  COMMON_PROGRAM_GROUP  "Common Programs"    // Registry Key NT4.0
#define  STARTUP_GROUP         "Startup"            // Registry Key NT4.0
#define  PROGRAM_GROUP         "Programs"           // Registry Key NT4.0

#define  STARTUP_GROUPNAME     "Startup"  // Should not be translated
#define  STARTUP_GROUPNAME_LC  "startup"  // Should not be translated

#define  EXPLORER_KEY          "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders"

const  maxregkey = 512;
static LPTSTR WinNT4GetExplorerKey(HKEY hHiveKey, LPTSTR pResult, LPTSTR pKey) ;

#define  PROGMANEXE             "PROGMAN.EXE"       // Name of Progman executable
//  The ID # of the default common startup group to be obtained from PROGMAN.EXE
#define  IDS_DEFAULTSTARTUP     173             

#define  TIMEOUT_GROUPOPERATION  600000   // 10 minutes - time out duration

#define  STARTUPNT351_KEY   "Software\\Microsoft\\Windows NT\\CurrentVersion\\Program Manager\\Settings"

static LPTSTR WinNT351GetProgManSettingsKey(LPTSTR pResult, LPTSTR pKey);

//***************************************************************************
// ProgmanGroupManipulation()
// 
// Description:
//
// This function is used for addition/deletion of groups/items
// 
// Parameters: 
//      lpGroupName  -  Group Name
//      lpCommand    -  Command to be executed in appropriate form
//                      e.g., "[CreateGroup(\"Startup\",%d)]"
//      nActionFlag  -  ACTION_ADD_ITEM, ACTION_ADD_GROUP, ACTION_DELETE_ITEM,
//                      ACTION_DELETE_GROUP (all defined in progman,h)
//      nGroupType   -  COMMON/PRIVATE Group
// 
// Return Value: True - Successful, False - Failure
// 
// 
//***************************************************************************
// 06/10/1996 RPULINT Function Created.                                
//***************************************************************************
BOOL ProgmanGroupManipulation ( LPSTR lpGroupName, LPSTR lpCommand, int nActionFlag, int nGroupType )
{
    LPSTR           szGroups;
    unsigned long   lResult;
    unsigned long   lIdLocal ; 
    HCONV           hConv;
    HDDEDATA        hDdeData;
    HSZ             szProgMan;
    HSZ             szTopic;
    int             nNumberOfItemsInGroup = 0 ;
    CONVCONTEXT     CCFilter = { sizeof (CONVCONTEXT), 0, 0, 0, 0L, 0L };
    LPBYTE          lpByte;
    LPSTR           szToken;
    LPSTR           szMem;
    char            szLocalCommands[SI_MAXCOMMAND] ;
    BOOL            bRet = TRUE ;

    // Initialize the Dde id to 0
    lIdLocal = 0L;
    szGroups = NULL;
    szMem = NULL;

    // Attempt to initialize a conversation.
    if (DdeInitialize (&lIdLocal, (PFNCALLBACK) GroupDDECallback,
         (DWORD) APPCMD_CLIENTONLY, 0L)) { 
      return (FALSE);
    }/*endIf*/

    // Convert startup group to an appropriate name
    // eg., English-Startup, German-AutoStart, etc...
    GetProgmanGroupName ( lpGroupName, nGroupType ) ;

    switch ( nActionFlag )
    {
        case ACTION_ADD_GROUP :

            // Create a string handle for the Dde conversation.
            szProgMan = DdeCreateStringHandle (lIdLocal, "PROGMAN", CP_WINANSI);

            // Connect to the program manager.
            hConv = DdeConnect (lIdLocal, szProgMan, szProgMan, &CCFilter);

            // Create a handle for the Group topic.
            szTopic = DdeCreateStringHandle (lIdLocal, lpGroupName, CP_WINANSI);

            // Execute the command
            hDdeData = DdeClientTransaction ((LPBYTE)lpCommand, strlen(lpCommand)+1, hConv, 0L, 0,
                 XTYP_EXECUTE, TIMEOUT_GROUPOPERATION, &lResult);
       
            if ( !hDdeData )
                bRet = FALSE ;

            break ;

        case ACTION_DELETE_GROUP :

            // Create a string handle for the Dde conversation.
            szProgMan = DdeCreateStringHandle (lIdLocal, "PROGMAN", CP_WINANSI);

            // Connect to the program manager.
            hConv = DdeConnect (lIdLocal, szProgMan, szProgMan, &CCFilter);

            // Create a handle for the Group topic.
            szTopic = DdeCreateStringHandle (lIdLocal, lpGroupName, CP_WINANSI);

            hDdeData = DdeClientTransaction (NULL, 0L, hConv, szTopic, CF_TEXT,
                 XTYP_REQUEST, TIMEOUT_GROUPOPERATION, &lResult);

            if ( !hDdeData )
                bRet = FALSE ;

            // Get the number of items in a group
            if (hDdeData && (hDdeData != (HDDEDATA) TRUE)) 
            {
                // Access the data to get a character pointer.
                lpByte = DdeAccessData (hDdeData, &lResult);

                // allocate a block of memory to use.
                szGroups = szMem = (LPSTR) VirtualAlloc (NULL, lResult, MEM_COMMIT, PAGE_READWRITE);
                if (!szMem) 
                {
                    lResult = GetLastError ();
                } 
                else 
                {
                    // Copy the Dde data to our own block.
                    memcpy (szGroups, lpByte, lResult);
                }/*endIf*/

                // Free the Dde data that is there. This is our responsibility.
                DdeUnaccessData (hDdeData);
                DdeFreeDataHandle (hDdeData);
            }/*endIf*/

            // If we have a memory block then parse it.
            if (szMem != NULL) 
            {
                //  The first line in this data has the following format
                //  GROUPNAME, PATH, Number of Items in the Group

                // Search for 'cr'.
                szToken = strtok (szGroups, "\n");

                // Find the next 'cr'.
                szToken = strtok (szToken, ",");
                szToken = strtok (NULL, ",");
                szToken = strtok (NULL, ",");

                nNumberOfItemsInGroup = atoi (szToken) ;        

                // Release the memory block.
                VirtualFree (szMem, lResult, MEM_DECOMMIT);
            }/*endIf*/

            // if you want to Delete Group, make sure there are no items in them.
            if ( 0 == nNumberOfItemsInGroup ) 
            {
                // Execute the command
                hDdeData = DdeClientTransaction ((LPBYTE)lpCommand, strlen(lpCommand)+1, hConv, 0L, 0,
                     XTYP_EXECUTE, TIMEOUT_GROUPOPERATION, &lResult);
            }

            break ;

        case    ACTION_ADD_ITEM     :
        case    ACTION_DELETE_ITEM  :
        default:
            // Create a string handle for the Dde conversation.
            szProgMan = DdeCreateStringHandle (lIdLocal, "PROGMAN", CP_WINANSI);
            // Create a handle for the Group topic.
            szTopic = DdeCreateStringHandle (lIdLocal, lpGroupName, CP_WINANSI);

            // Connect to the program manager with service/topic name.
            hConv = DdeConnect (lIdLocal, szProgMan, szProgMan, &CCFilter);

            if ( ACTION_ADD_ITEM == nActionFlag )
            {
                // Execute the command (Create/Activate)
                wsprintf( szLocalCommands, "[CreateGroup(\"%s\",%d)]", lpGroupName, nGroupType ) ;
                hDdeData = DdeClientTransaction ((LPBYTE)szLocalCommands, strlen(szLocalCommands)+1, 
                                                    hConv, 0L, 0,XTYP_EXECUTE, TIMEOUT_GROUPOPERATION, &lResult);
            }

            // Activate & get it to normal size
            wsprintf( szLocalCommands, "[ShowGroup(\"%s\",1,%d)]", lpGroupName, nGroupType ) ;
            hDdeData = DdeClientTransaction ((LPBYTE)szLocalCommands, strlen(szLocalCommands)+1, 
                                                hConv, 0L, 0,XTYP_EXECUTE, TIMEOUT_GROUPOPERATION, &lResult);
            if ( !hDdeData )
            {
                // The group itself isnt there, so there's nothing to delete
                bRet = FALSE ;
                goto exitManipulation ;
            }

            hDdeData = DdeClientTransaction ((LPBYTE)lpCommand, strlen(lpCommand)+1, hConv, 0L, 0,
                 XTYP_EXECUTE, TIMEOUT_GROUPOPERATION, &lResult);

            if ( !hDdeData )
                bRet = FALSE ;

            // Shrink it to an Icon
            wsprintf( szLocalCommands, "[ShowGroup(\"%s\",6,%d)]", lpGroupName, nGroupType ) ;
            hDdeData = DdeClientTransaction ((LPBYTE)szLocalCommands, strlen(szLocalCommands)+1, 
                                                hConv, 0L, 0,XTYP_EXECUTE, TIMEOUT_GROUPOPERATION, &lResult);
            break ;

    } // end switch

exitManipulation :

    // Release the two string handles.
    DdeFreeStringHandle (lIdLocal, szProgMan);
    DdeFreeStringHandle (lIdLocal, szTopic);

    // Disconnect the conversation.
    DdeDisconnect (hConv);

    // Release the DDEML.
    DdeUninitialize (lIdLocal);

    return (bRet);

}/* end ProgmanGroupManipulation */


/********************************************************************

   GroupDDECallback.
   
   Function exists for compatibility. Does nothing.

********************************************************************/

HDDEDATA CALLBACK GroupDDECallback (UINT uiType, UINT uiFmt, HANDLE hConv,
      HSZ sz1, HSZ sz2, HDDEDATA hData, LONG lData1, LONG lData2) {
// no need to do anything
    return ((HDDEDATA) NULL);
}/* end GroupDDECallback */

//***************************************************************************
// ProgmanQueryItemInGroup()
// 
// Description:
//
// This function is used to check for the presence of a particular item in a 
// group
// 
// Parameters: 
//      lpGroupName  -  Group Name
//      lpItemName   -  Item Name
//      nGroupType   -  COMMON/PRIVATE Group
//
// Return Value: DDE_ERROR     - DDE Failed
//               ITEM_NOTFOUND - Item is not present in the group
//               ITEM_FOUND    - Item is present in the group
//               ITEM_ERROR    - All other kinds of errors
//***************************************************************************
// 06/18/1996 RPULINT Function Created.                                
//***************************************************************************
int ProgmanQueryItemInGroup (  LPSTR lpGroupName, LPSTR lpItemName, int nGroupType )
{
    LPTSTR          szGroup;
    unsigned long   lResult;
    unsigned long   lIdLocal ; 
    HCONV           hConv;
    HDDEDATA        hDdeData;
    HSZ             szProgMan;
    HSZ             szTopic;
    int             nNumberOfItemsInGroup = 0 ;
    CONVCONTEXT     CCFilter = { sizeof (CONVCONTEXT), 0, 0, 0, 0L, 0L };
    LPBYTE          lpByte;
    LPTSTR          szToken;
    LPTSTR          szToken2;
    LPTSTR          szMem;
    int             iRet = ITEM_NOTFOUND ;
    int             iLoop ;
    LPTSTR          lpItem ;
    char            szLocalCommands[SI_MAXCOMMAND] ;
    int             verHi, verLow;
    DWORD           dwVersion;

    // Get the version number
    dwVersion = GetVersion();
    verHi  = LOBYTE(LOWORD(dwVersion));
    verLow = HIBYTE(LOWORD(dwVersion));

    if ( (verHi == 4) && (verLow == 0) )
    {
        return ( ProgmanQueryItemInGroupNT4 ( lpGroupName, lpItemName, nGroupType ) ) ;
    }

    // Convert startup group to an appropriate name
    // eg., English-Startup, German-AutoStart, etc...
    GetProgmanGroupName ( lpGroupName, nGroupType ) ;

    // Initialize the Dde id to 0
    lIdLocal = 0L;
    szGroup  = NULL;
    szMem    = NULL;
    lpItem   = NULL ;
 
    lpItem = (LPTSTR) VirtualAlloc (NULL, strlen (lpItemName) + 4, MEM_COMMIT, PAGE_READWRITE);
    if (!lpItem )
        return ( ITEM_ERROR ) ;

    SPRINTF ( lpItem, "\"%s\"", lpItemName ) ;

    // Attempt to initialize a conversation.
    if (DdeInitialize (&lIdLocal, (PFNCALLBACK) GroupDDECallback,
         (DWORD) APPCMD_CLIENTONLY, 0L)) { 

      if ( lpItem )
          VirtualFree (lpItem, strlen (lpItemName) + 4, MEM_DECOMMIT);

      return (DDE_ERROR);
    }/*endIf*/

    // Create a string handle for the Dde conversation.
    szProgMan = DdeCreateStringHandle (lIdLocal, "PROGMAN", CP_WINANSI);

    // Connect to the program manager.
    hConv = DdeConnect (lIdLocal, szProgMan, szProgMan, &CCFilter);

    wsprintf( szLocalCommands, "[ShowGroup(\"%s\",1,%d)]", lpGroupName, nGroupType ) ;
    hDdeData = DdeClientTransaction ((LPBYTE)szLocalCommands, strlen(szLocalCommands)+1, 
                                        hConv, 0L, 0,XTYP_EXECUTE, TIMEOUT_GROUPOPERATION, &lResult);
    if (!hDdeData)
    {
        if ( lpItem )
            VirtualFree (lpItem, strlen (lpItemName) + 4, MEM_DECOMMIT);

        // Release the string handle.
        DdeFreeStringHandle (lIdLocal, szProgMan);

        // Disconnect the conversation.
        DdeDisconnect (hConv);

        // Release the DDEML.
        DdeUninitialize (lIdLocal);

        return (ITEM_NOTFOUND) ;
    }

    // Create a handle for the Group topic.
    szTopic = DdeCreateStringHandle (lIdLocal, lpGroupName, CP_WINANSI);

    hDdeData = DdeClientTransaction (NULL, 0L, hConv, szTopic, CF_TEXT,
         XTYP_REQUEST, TIMEOUT_GROUPOPERATION, &lResult);

    if ( !hDdeData )
        iRet = DDE_ERROR ;

    // Get the number of items in a group
    if (hDdeData && (hDdeData != (HDDEDATA) TRUE)) 
    {
        // Access the data to get a character pointer.
        lpByte = DdeAccessData (hDdeData, &lResult);

        // allocate a block of memory to use.
        szGroup = (LPSTR) VirtualAlloc (NULL, lResult, MEM_COMMIT, PAGE_READWRITE);
        szMem = (LPSTR) VirtualAlloc (NULL, lResult, MEM_COMMIT, PAGE_READWRITE);
        if (!szMem || !szGroup) 
        {
            lResult = GetLastError ();
        } 
        else 
        {
            // Copy the Dde data to our own block.
            memcpy (szGroup, lpByte, lResult);
            memcpy (szMem, lpByte, lResult);
        }/*endIf*/

        // Free the Dde data that is there. This is our responsibility.
        DdeUnaccessData (hDdeData);
        DdeFreeDataHandle (hDdeData);

    }/*endIf*/

    // If we have a memory block then parse it.
    if ((szMem != NULL) && (szGroup != NULL))
    {
        //  The first line in this data has the following format
        //  GROUPNAME, PATH, Number of Items in the Group

        // Search for 'cr'.
        szToken = strtok (szMem, "\n");

        // Find the next 'cr'.
        szToken = strtok (szToken, ",");
        szToken = strtok (NULL, ",");
        szToken = strtok (NULL, ",");

        nNumberOfItemsInGroup = atoi (szToken) ;        

        if ( nNumberOfItemsInGroup > 0 )
        {
            // skip the first line - 2nd line onwards contains the item names.
            szToken = strtok (szGroup, "\n");
            // Find the next 'cr'.
            szToken = strtok (NULL, "\n");
    
            for ( iLoop = 0;(szToken) && (iLoop < nNumberOfItemsInGroup); iLoop++ )
            {
                if ( szToken2 = strchr (szToken, ','))
                    *szToken2 = '\0' ;

                if ( !STRCMP ( szToken, lpItem ) )
                {
                    iRet = ITEM_FOUND ;
                    break ;
                }    
                // Find the next 'cr'.
                szToken = strtok (NULL, "\n");
            }
        }

        // Release the memory block.
        VirtualFree (szGroup, lResult, MEM_DECOMMIT);
        VirtualFree (szMem, lResult, MEM_DECOMMIT);
    }/*endIf*/

    if ( lpItem )
        VirtualFree (lpItem, strlen (lpItemName) + 4, MEM_DECOMMIT);

    // Release the two string handles.
    DdeFreeStringHandle (lIdLocal, szProgMan);
    DdeFreeStringHandle (lIdLocal, szTopic);

    // Disconnect the conversation.
    DdeDisconnect (hConv);

    // Release the DDEML.
    DdeUninitialize (lIdLocal);

    return (iRet);

} /* end ProgmanQueryItemInGroup */

//***************************************************************************
// ProgmanQueryItemInGroupNT4()
// 
// Description:
//
// This function is used to check for the presence of a particular item in a 
// group (NT4.0)
// 
// Parameters: 
//      lpGroupName  -  Group Name
//      lpItemName   -  Item Name
//      nGroupType   -  COMMON/PRIVATE Group
// 
// Return Value: DDE_ERROR     - DDE Failed
//               ITEM_NOTFOUND - Item is not present in the group
//               ITEM_FOUND    - Item is present in the group
//               ITEM_ERROR    - All other kinds of errors
//***************************************************************************
// 07/09/1996 RPULINT Function Created.                                
//***************************************************************************
int ProgmanQueryItemInGroupNT4 (  LPSTR lpGroupName, LPSTR lpItemName, int nGroupType )
{
	TCHAR	 GroupDir [SI_MAXCOMMAND];
	OFSTRUCT ofs;
    
    if ( !STRICMP ( lpGroupName, STARTUP_GROUPNAME ) )
    {
        if ( nGroupType == GROUPTYPE_COMMON )
            WinNT4GetExplorerKey(HKEY_LOCAL_MACHINE, GroupDir, COMMON_STARTUP_GROUP );
        else
            WinNT4GetExplorerKey(HKEY_CURRENT_USER, GroupDir, STARTUP_GROUP );
    }
    else
    {
        if ( nGroupType == GROUPTYPE_COMMON )
            WinNT4GetExplorerKey(HKEY_LOCAL_MACHINE, GroupDir, COMMON_PROGRAM_GROUP );
        else
            WinNT4GetExplorerKey(HKEY_CURRENT_USER, GroupDir, PROGRAM_GROUP );

        lstrcat ( GroupDir, "\\" ) ;
        lstrcat ( GroupDir, lpGroupName ) ;
    }

    lstrcat ( GroupDir, "\\" ) ;
    lstrcat ( GroupDir, lpItemName ) ;
    lstrcat ( GroupDir, ".LNK" ) ;

	if ( OpenFile(GroupDir, &ofs, OF_EXIST) != HFILE_ERROR )
    {
        return ( ITEM_FOUND ) ;
    }
    return ( ITEM_NOTFOUND ) ;
}

//***************************************************************************
// WinNT4GetExplorerKey()
// 
// Description:
//
// This function is used to get the value of a particular registry key in 
// Shell Folders (NT4.0)
// 
// Parameters: 
//      hHiveKey - Handle of the open Key
//      pResult  -  Result string
//      pKey     -  Registry Key
// 
// Return Value: Result string
//***************************************************************************
// 07/09/1996 RPULINT Function Created.                                
//***************************************************************************
static LPTSTR WinNT4GetExplorerKey(HKEY hHiveKey, LPTSTR pResult, LPTSTR pKey)
{
    DWORD       size = maxregkey; 
    DWORD       dataType;
    HKEY        hKey ;
    long        lResult = 0L;
    
	*pResult = '\0';

    if ( ERROR_SUCCESS == RegOpenKeyEx(hHiveKey, EXPLORER_KEY,  0, KEY_QUERY_VALUE, &hKey ) )
    {
        lResult = RegQueryValueEx( hKey, pKey, 0, &dataType, NULL, &size) ;
        if ( lResult == ERROR_SUCCESS )
        {
            RegQueryValueEx( hKey, pKey, 0, &dataType, (LPBYTE)pResult, &size) ;
        }
        else
        {
            wsprintf ( pResult , "" ) ; 
        }
        RegCloseKey( hKey ) ;
    }
	return pResult;
}

//***************************************************************************
// ProgmanGroupManipulationNT4()
// 
// Description:
//
// This function is used for addition/deletion of groups/items
// 
// Parameters: 
//      lpGroupName  -  Group Name
//      lpItemName   -  Item Name
//      nActionFlag  -  ACTION_ADD_ITEM, ACTION_ADD_GROUP, ACTION_DELETE_ITEM,
//                      ACTION_DELETE_GROUP (all defined in progman,h)
//      nGroupType   -  COMMON/PRIVATE Group
// 
// Return Value: True - Successful, False - Failure
// 
//***************************************************************************
// 07/09/1996 RPULINT Function Created.                                
//***************************************************************************
BOOL ProgmanGroupManipulationNT4 ( LPSTR lpGroupName, LPSTR lpItemName, int nActionFlag, int nGroupType )
{
	TCHAR	 GroupDir [SI_MAXCOMMAND];
    
    if ( !STRICMP ( lpGroupName, STARTUP_GROUPNAME ) )
    {
        if ( nGroupType == GROUPTYPE_COMMON )
            WinNT4GetExplorerKey(HKEY_LOCAL_MACHINE, GroupDir, COMMON_STARTUP_GROUP );
        else
            WinNT4GetExplorerKey(HKEY_CURRENT_USER, GroupDir, STARTUP_GROUP );
    }
    else
    {
        if ( nGroupType == GROUPTYPE_COMMON )
            WinNT4GetExplorerKey(HKEY_LOCAL_MACHINE, GroupDir, COMMON_PROGRAM_GROUP );
        else
            WinNT4GetExplorerKey(HKEY_CURRENT_USER, GroupDir, PROGRAM_GROUP );

        lstrcat ( GroupDir, "\\" ) ;
        lstrcat ( GroupDir, lpGroupName ) ;
    }

    lstrcat ( GroupDir, "\\" ) ;
    lstrcat ( GroupDir, lpItemName ) ;
    lstrcat ( GroupDir, ".LNK" ) ;

    switch ( nActionFlag )
    {
        case    ACTION_DELETE_ITEM  :
        {
            IShellLink FAR*  pShellLink;
            LPUNKNOWN        pUnkOuter = NULL;
            HRESULT          hr;
        	OFSTRUCT         ofs;

            // Check if file exists - delete only if they exist
            if ( OpenFile(GroupDir, &ofs, OF_EXIST) == HFILE_ERROR )
                return (TRUE);
            
            if(FAILED(CoInitialize(NULL)))
                return FALSE;

            if((FAILED(hr = CoCreateInstance(CLSID_ShellLink, pUnkOuter, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID *)&pShellLink))))
            {
                CoUninitialize();
                return FALSE;
            }

            pShellLink->SetPath("");
            pShellLink->SetArguments("");
            pShellLink->SetDescription("");
            pShellLink->SetIconLocation("",0);
            pShellLink->SetWorkingDirectory("");

            BOOL bReturn = FALSE;

            LPPERSISTFILE pFile;
            if(!FAILED(pShellLink->QueryInterface(IID_IPersistFile, (LPVOID *)&pFile)))
            {
                WCHAR szOlePath[SYM_MAX_PATH];
                if(MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,GroupDir,-1,szOlePath,sizeof(szOlePath)))
                    pFile->Save(szOlePath,FALSE);
                pFile->Release();
                bReturn = TRUE;
            }

            pShellLink->Release();

            CoUninitialize();

            if (DeleteFile(GroupDir) )
            {
                return (TRUE ) ;
            }
            else
            {
                return (FALSE) ;
            }
        }

        default:
            break ;    
    }
    
    return ( TRUE ) ;

} /* ProgmanGroupManipulationNT4 */

//***************************************************************************
// GetProgmanGroupName()
// 
// Description:
//
// This function is used to get the appropriate common/private startup group name
// 
// Parameters: 
//      lpGroupName - Group name
//      nGroupType  - COMMON OR PRIVATE group (1/0)
// 
// Return Value: Result string
//***************************************************************************
// 07/16/1996 RPULINT Function Created.                                
//***************************************************************************
LPSTR GetProgmanGroupName ( LPSTR lpGroupName, int nGroupType )
{
    int      verHi, verLow;
    DWORD    dwVersion;
	TCHAR	 szGroupName [SI_MAXCOMMAND];
	TCHAR	 szTemp [SI_MAXCOMMAND];
    int      index = 0 ;
        
    if ( !STRICMP ( lpGroupName, STARTUP_GROUPNAME ) )
    {
        // Get the version number
        dwVersion = GetVersion();
        verHi  = LOBYTE(LOWORD(dwVersion));
        verLow = HIBYTE(LOWORD(dwVersion));

        STRCPY ( szGroupName, lpGroupName ) ;
        if ( verHi >= 4 ) // NT4
        {
            if ( nGroupType == GROUPTYPE_COMMON )
                WinNT4GetExplorerKey(HKEY_LOCAL_MACHINE, szGroupName, COMMON_STARTUP_GROUP );
            else
                WinNT4GetExplorerKey(HKEY_CURRENT_USER, szGroupName, STARTUP_GROUP );

            if ( !STRICMP (szGroupName, "" ) )
            {
                STRCPY ( szGroupName, STARTUP_GROUPNAME ) ;
            }
            // strip the pathname - remove the text before the backslash if any.
            STRCPY ( szTemp, szGroupName ) ;            
            index = strlen (szTemp ) ;
            while ( ( index > 0 ) && (szTemp[index - 1] != '\\' ))
            {
                index-- ;    
            }
            strcpy ( szGroupName, &szTemp[index] ) ;
        }
        else // NT3.51
        {
            if ( nGroupType == GROUPTYPE_COMMON )
            {
                // Need to open PROGMAN.EXE to obtain common group name only 
                // for common startup group, private startup group name can be 
                // obtained from Registry.
                TCHAR           szProgmanEXE[SYM_MAX_PATH];
                LPSTR           lpszProgmanEXE = szProgmanEXE;
                HINSTANCE       hPMInst ;

                GetSystemDirectory(lpszProgmanEXE,SYM_MAX_PATH);
                STRCAT ( lpszProgmanEXE, "\\" );
                STRCAT ( lpszProgmanEXE, PROGMANEXE );
        
                hPMInst = LoadLibrary ( lpszProgmanEXE ) ;
                if ( hPMInst )
                {
                    LoadString( hPMInst, IDS_DEFAULTSTARTUP, szGroupName, 
                                sizeof(szGroupName) - 1);

                    FreeLibrary ( hPMInst ) ;
                }
                else
                    STRCPY ( szGroupName, STARTUP_GROUPNAME ) ;

                // Create "Startup" instead of "startup"
                if ( !STRCMP ( szGroupName, STARTUP_GROUPNAME_LC ) )
                {
                    STRCPY ( szGroupName, STARTUP_GROUPNAME ) ;
                }
            }
            else
                WinNT351GetProgManSettingsKey(szGroupName, STARTUP_GROUP) ;
        
            if ( !STRICMP (szGroupName, "" ) )
            {
                STRCPY ( szGroupName, STARTUP_GROUP ) ;
            }
        }
    
        STRCPY ( lpGroupName, szGroupName ) ;
    }

    return ( lpGroupName ) ;
}

//***************************************************************************
// WinNT351GetProgManSettingsKey()
// 
// Description:
//
// This function is used to get the value of a particular registry key in 
// ProgMan (NT3.51)
// 
// Parameters: 
//      pResult  -  Result string
//      pKey     -  Registry Key
// 
// Return Value: Result string
//***************************************************************************
// 07/16/1996 RPULINT Function Created.                                
//***************************************************************************
static LPTSTR WinNT351GetProgManSettingsKey(LPTSTR pResult, LPTSTR pKey)
{
    DWORD       size = maxregkey; 
    DWORD       dataType;
    HKEY        hKey ;
    long        lResult = 0L;
    
	*pResult = '\0';

    if ( ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, STARTUPNT351_KEY,  0, KEY_QUERY_VALUE, &hKey ) )
    {
        lResult = RegQueryValueEx( hKey, pKey, 0, &dataType, NULL, &size) ;
        if ( lResult == ERROR_SUCCESS )
        {
            RegQueryValueEx( hKey, pKey, 0, &dataType, (LPBYTE)pResult, &size) ;
        }
        else
        {
            wsprintf ( pResult , "" ) ; 
        }
        RegCloseKey( hKey ) ;
    }
	return pResult;
}

//***************************************************************************
// ProgmanGroupAddItemNT4 ()
// 
// Description:
//
// This function is used to add items to a group in NT4.0
// When using this function you need to link 2 libraries - OLE32.LIB and UUID.LIB
// 
// Parameters: 
//      lpGroupName - Group name
//      lpItemName  - Item name
//      lpCmdLine   - Command Line
//      lpArguments - Command Line Parameters
//      lpIconPath  - Icon Path
//      nIconIndex  - Icon Index
//      lpDefDir    - Working directory
//      nGroupType  - Group Type - Common(1)/Private(0)
// 
// Return Value: TRUE - success / FALSE - Failure
//***************************************************************************
// 07/18/1996 RPULINT Function Created.                                
//***************************************************************************
BOOL ProgmanGroupAddItemNT4 ( LPCSTR lpGroupName, LPCSTR lpItemName, LPCSTR lpCmdLine, 
                              LPCSTR lpArguments, LPCSTR lpIconPath, int nIconIndex,
                              LPCSTR lpDefDir, int nGroupType )
{
	TCHAR	         GroupDir [SI_MAXCOMMAND];
	TCHAR	         szGroupName [SI_MAXCOMMAND];
    IShellLink FAR*  pShellLink;
    LPUNKNOWN        pUnkOuter = NULL;
    HRESULT          hr;
    
    if ( !STRICMP ( lpGroupName, STARTUP_GROUPNAME ) )
    {
        if ( nGroupType == GROUPTYPE_COMMON )
            WinNT4GetExplorerKey(HKEY_LOCAL_MACHINE, GroupDir, COMMON_STARTUP_GROUP );
        else
            WinNT4GetExplorerKey(HKEY_CURRENT_USER, GroupDir, STARTUP_GROUP );
    }
    else
    {
        if ( nGroupType == GROUPTYPE_COMMON )
            WinNT4GetExplorerKey(HKEY_LOCAL_MACHINE, GroupDir, COMMON_PROGRAM_GROUP );
        else
            WinNT4GetExplorerKey(HKEY_CURRENT_USER, GroupDir, PROGRAM_GROUP );

        STRCAT ( GroupDir, "\\" ) ;
        STRCAT ( GroupDir, lpGroupName ) ;
    }

    STRCPY ( szGroupName, GroupDir ) ;
    
    STRCAT ( GroupDir, "\\" ) ;
    STRCAT ( GroupDir, lpItemName ) ;
    STRCAT ( GroupDir, ".LNK" ) ;

    CreateDirectory(szGroupName, NULL);

    if(FAILED(CoInitialize(NULL)))
        return FALSE;

    if((FAILED(hr = CoCreateInstance(CLSID_ShellLink, pUnkOuter, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID *)&pShellLink))))
    {
        CoUninitialize();
        return FALSE;
    }

    pShellLink->SetPath(lpCmdLine);
    if(lpArguments)
        pShellLink->SetArguments(lpArguments);
    if(lpItemName)
        pShellLink->SetDescription(lpItemName);
    if(lpIconPath)
        pShellLink->SetIconLocation(lpIconPath,nIconIndex);
    if(lpDefDir)
        pShellLink->SetWorkingDirectory(lpDefDir);

    BOOL bReturn = FALSE;

    LPPERSISTFILE pFile;
    if(!FAILED(pShellLink->QueryInterface(IID_IPersistFile, (LPVOID *)&pFile)))
    {
        WCHAR szOlePath[SYM_MAX_PATH];
        if(MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,GroupDir,-1,szOlePath,sizeof(szOlePath)))
            pFile->Save(szOlePath,FALSE);
        pFile->Release();
        bReturn = TRUE;
    }

    pShellLink->Release();

    CoUninitialize();

    return bReturn;
} // ProgmanGroupAddItemNT4



/*****************************************************************************
FUNCTION:
    BOOL RestoreProgman

DESCRIPTION:
    Restores Program Manager from iconic state to normal state.
    
PARAMETERS:
    None.

RETURN VALUE:
    TRUE - Program Manager successfully restored to normal state from iconic.
    FALSE - Window handle could not be retrieved.
******************************************************************************
01/29/97 TSMITH  Function created.
*****************************************************************************/
BOOL RestoreProgman(
    VOID
    )
    {
    HWND hProgman;
    BOOL bResult = FALSE;

    hProgman = FindWindow( "Progman", NULL );

    if ( ( hProgman ) && ( IsIconic( hProgman ) ) )
        {
        bResult = OpenIcon( hProgman );
        }

    return( bResult );
    }



/*****************************************************************************
FUNCTION:
    BOOL MinimizeProgman

DESCRIPTION:
    Minimizes the Program Manager window.
        
PARAMETERS:
    None.

RETURN VALUE:
    TRUE - Program Manager successfully minimized.
    FALSE - Handle to Program Manager could not be obtained or window
            could not be mimimized.
******************************************************************************
01/29/97 TSMITH  Function created.
*****************************************************************************/
BOOL MinimizeProgman(
    VOID
    )
    {
    HWND hProgman;
    BOOL bResult = FALSE;

    hProgman = FindWindow( "Progman", NULL );

    if ( ( hProgman ) && ( !IsIconic( hProgman ) ) )
        {
        bResult = ShowWindow( hProgman, SW_MINIMIZE );
        }

    return( bResult );
    }



/*****************************************************************************
FUNCTION:
    BOOL IsProgmanIconic

DESCRIPTION:
    Determines iconic state of Program Manager window.

PARAMETERS:
    None.

RETURN VALUE:
    TRUE - The Program Manager window is iconic.
    FALSE - Not iconic.
******************************************************************************
01/29/97 TSMITH  Function created.
*****************************************************************************/
BOOL IsProgmanIconic(
    VOID
    )
    {
    HWND hProgman;
    BOOL bResult = FALSE;

    hProgman = FindWindow( "Progman", NULL );

    if ( hProgman )
        {
        bResult = IsIconic( hProgman );
        }

    return( bResult );
    }
