// Copyright 1995 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/VIRLIST/VCS/v_vlist.cpv   1.4   29 May 1998 14:02:44   DBuches  $
//
// Description: Contains source specific to our VLIST custom control.
//
// Contains:
//           GetVirusInfoPtr()
//           GetVirusText()
//
// See Also:
//************************************************************************
// $Log:   S:/VIRLIST/VCS/v_vlist.cpv  $
// 
//    Rev 1.4   29 May 1998 14:02:44   DBuches
// Now, correctly classify macro viruses.
//
//    Rev 1.3   20 May 1997 18:58:18   MKEATIN
// We're now linking to an avenged api.
//
//    Rev 1.2   01 Mar 1996 12:00:12   JWORDEN
// DBCS enable
//
//    Rev 1.1   16 Jan 1996 17:05:26   JWORDEN
// Port Quake 6 to trunk
//
//    Rev 1.0.1.0   13 Nov 1995 21:59:28   DSACKIN
// Branch base for version QAK6
//
//    Rev 1.0   13 Nov 1995 21:58:42   DSACKIN
// Initial revision.
//
//    Rev 1.0.1.1   30 Oct 1995 11:35:02   DSACKIN
// Added changes to move VList class from SYMGUI to VIRLIST
//
//    Rev 1.0.1.0   09 Oct 1995 19:04:02   JREARDON
// Branch base for version QAK2
//
//    Rev 1.0   09 Oct 1995 01:58:56   DSACKIN
// Initial revision.
//
//************************************************************************

#include "platform.h"
#include "ngui.h"
#include "vlist.h"
#include "virusdbf.h"
#include "hstring.h"
#include "virusstb.h"

extern  HINSTANCE   hInstance;

#define SIZE_SMALL_STRING   20

                                        // MUST be static for speed!
                                        // Initialize first byte to Zero so
                                        // we can err out if strings not loaded

static    TCHAR   szBootRecords   [ SIZE_SMALL_STRING+1 ] = "\0";
static    TCHAR   szPrograms      [ SIZE_SMALL_STRING+1 ] = "\0";
static    TCHAR   szBootAndProgs  [ SIZE_SMALL_STRING+1 ] = "\0";
static    TCHAR   szDocuments     [ SIZE_SMALL_STRING+1 ] = "\0";
static    TCHAR   szNotApplicable [ SIZE_SMALL_STRING+1 ] = "\0";


//************************************************************************
// GetVirusInfoPtr()
//
// Used for getting the N30Notes record for the currently selected virus
//
// Parameters:
//
// Returns:
//  HPN30NOTES - pointer to the requested virus information
//
//************************************************************************
// 9/25/95 DSACKING Function created.
//************************************************************************

LPHVLVIRUSENTRY WINAPI GetVirusInfoPtr(HWND hListBox, WORD wIndex)
{
VLBSTRUCT rVLBStruct;
LPHVLVIRUSENTRY hpReturn;

   rVLBStruct.lIndex = wIndex;
   hpReturn = (LPHVLVIRUSENTRY) SendMessage(hListBox,
                                          VLB_GETITEMDATA,
                                          0, (LPARAM) (LPVLBSTRUCT) &rVLBStruct);

   return (hpReturn);
}




//************************************************************************
// GetVirusInfoPtr()
//
// Used for getting the N30Notes record for the currently selected virus
//
// Parameters:
//
// Returns:
//  HPN30NOTES - pointer to the requested virus information
//
//************************************************************************
// 9/25/95 DSACKING Function created.
//************************************************************************

BOOL WINAPI GetVirusText(HWND hListBox, WORD wIndex, LPBYTE lpWorkMem)
{
VLBSTRUCT rVLBStruct;

   rVLBStruct.lIndex = wIndex;
   rVLBStruct.lpTextPointer = (LPSTR) lpWorkMem;
   if (SendMessage(hListBox,
                   VLB_GETTEXT,
                   0, (LPARAM) (LPVLBSTRUCT) &rVLBStruct) == VLB_ERR)
      {
      return FALSE;
      }
   else
      {
      return TRUE;
      }
}

// ************************************************************************ //
// ****        (C) Copyright Symantec Corporation 19xx - 1993          **** //
// ************************************************************************ //
// ****                                                                **** //
// ****  Procedure:                                                    **** //
// ****                                                                **** //
// ****  Author: Dan Sackinger               Date: September 21, 1995  **** //
// ****                                                                **** //
// ****  Functionality:                                                **** //
// ****     Return a formatted virus string based on the indexlist     **** //
// ****     and the index passed in.  Actual virus data taken from     **** //
// ****     the lpN30Notes list.                                       **** //
// ****                                                                **** //
// ****  Input:                                                        **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Output:                                                       **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Update History:                                               **** //
// ****                                                                **** //
// ****                                                                **** //
// ************************************************************************ //

VOID WINAPI LoadListBoxStrings ()
{
    LoadString (hInstance, IDS_BOOTRECORDS, szBootRecords,
                sizeof(szBootRecords));

    LoadString (hInstance, IDS_PROGRAMS, szPrograms,
                sizeof(szPrograms));

    LoadString (hInstance, IDS_BOOTANDPROGS, szBootAndProgs,
                sizeof(szBootAndProgs));

    LoadString (hInstance, IDS_DOCUMENTS, szDocuments,
                sizeof(szDocuments));

	LoadString (hInstance, IDS_NOTAPPLICABLE, szNotApplicable,
                sizeof(szNotApplicable));

}


// ************************************************************************ //
// ****        (C) Copyright Symantec Corporation 19xx - 1993          **** //
// ************************************************************************ //
// ****                                                                **** //
// ****  Procedure:                                                    **** //
// ****                                                                **** //
// ****  Author: Dan Sackinger               Date: September 21, 1995  **** //
// ****                                                                **** //
// ****  Functionality:                                                **** //
// ****     Return a formatted virus string based on the indexlist     **** //
// ****     and the index passed in.  Actual virus data taken from     **** //
// ****     the lpN30Notes list.                                       **** //
// ****                                                                **** //
// ****  Input: szWork - buffer to hold output                         **** //
// ****                     MUST BE SIZE_WORK bytes!                   **** //
// ****                                                                **** //
// ****  Output:                                                       **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Update History:                                               **** //
// ****                                                                **** //
// ****                                                                **** //
// ************************************************************************ //

BOOL WINAPI GetListBoxString (
    LPVIRUSDBF    lpVir,                // [in] virus database pointer
    UINT          uIndex,               // [in] index of string to get
    TCHAR        *szWork)               // [out] string in buffer
{
    auto    UINT          uArrayIndex;
    auto    HVLVIRUSENTRY   lpVirusPtr;

    if (szBootRecords[0] == '\0')
       LoadListBoxStrings();

    SYM_ASSERT (uIndex < lpVir->ListboxArray[lpVir->uActiveEntry].uNumEntries);

    uArrayIndex = lpVir->ListboxArray[lpVir->uActiveEntry].lpuListBoxIndex[uIndex];

    lpVirusPtr = lpVir->lphVirusArray[uArrayIndex];

    STRCPY (szWork, lpVirusPtr->stVirusInfoBlock.lpVName);
    STRCAT (szWork, "\t");

    if (lpVirusPtr->stVirusInfoBlock.wInfo & bitINFO_MULTIPARTY)
        {
        STRCAT (szWork, szBootAndProgs);
        }
   
	else if(lpVir->uActiveEntry == VIRUSTYPE_AGENT )
        	STRCAT (szWork, szNotApplicable);
		
		else
        // Check for boot record infector
        if (lpVirusPtr->stVirusInfoBlock.wType == BOOtype ||
            lpVir->uActiveEntry == VIRUSTYPE_BOOT)
            STRCAT (szWork, szBootRecords);
        // Check for Macro infector
        else if( lpVirusPtr->stVirusInfoBlock.wControl2 & bitINFO_MACRO )
            STRCAT (szWork, szDocuments);
        // Must be a program file infector.
        else
            STRCAT (szWork, szPrograms);
       // }

    return (0);
}



