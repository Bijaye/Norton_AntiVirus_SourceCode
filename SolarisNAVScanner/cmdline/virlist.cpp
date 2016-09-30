// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/CMDLINE/VCS/VIRLIST.CPv   1.1   21 May 1996 12:30:06   DCHI  $
//
// Description:
//  Functions for displaying the virus list and for deleting virus
//  definitions from the database.
//
// Contains:
//  VirusListDisplay()
//  VirusListDeleteVirusVID()
//
// See Also:
//
//************************************************************************
// $Log:   S:/CMDLINE/VCS/VIRLIST.CPv  $
// 
//    Rev 1.1   21 May 1996 12:30:06   DCHI
// Changed #include "virlist.h" to #include "virlista.h".
// 
//    Rev 1.0   21 May 1996 12:25:46   DCHI
// Initial revision.
// 
//************************************************************************

#include <stdio.h>

#include "virlista.h"

#include "msgstr.h"

//*************************************************************************
//
// Function:
//  VirusListDisplay()
//
// Parameters:
//  lpScanGlobal        Pointer to global data
//
// Description:
//  Displays information for all non-deleted viruses in the following
//  format:
//
//  VirusName  VID  Size  Infects{COM,EXE,SYS,FBR,PBR,MBR}
//
// Returns:
//  Nothing.
//
//*************************************************************************

void VirusListDisplay
(
    LPSCANGLOBAL        lpScanGlobal
)
{
    WORD                w, wCount;
    VIRUS_USER_INFO_T   stVirusUserInfo;

    if (EngGetNumVirusRecords(lpScanGlobal->hGEng,
                              &wCount) == ENGSTATUS_ERROR)
    {
        printf("Error accessing virus list!\n");
        return;
    }

    printf("%-22s  %4s  %5s  %s\n",
        gszMsgStrVirusName,
        gszMsgStrVID,
        gszMsgStrSize,
        gszMsgStrInfects);

    printf("----------------------  ----  -----  ------------------------\n");
    for (w=0;w<wCount;w++)
    {
        if (EngGetUserInfoIndex(lpScanGlobal->hGEng,
                                w,
                                &stVirusUserInfo) == ENGSTATUS_ERROR)
        {
            printf(gszMsgStrErrorAccessingVirusList);
            return;
        }

        // Don't print out information for deleted viruses

        if (stVirusUserInfo.wFlags & AVENGE_TYPE_DELETED)
            continue;

        // Print virus name and virus ID

        printf("%-22s  %04X  ",stVirusUserInfo.sVirusName,
            stVirusUserInfo.wVirusID);

        // Print nfection size

        if (stVirusUserInfo.wInfSize == 0)
            printf("%5s  ","n/a");
        else
            printf("%5u  ",stVirusUserInfo.wInfSize);

        // Print infection areas

        if (stVirusUserInfo.dwInfo & bitINFO_COMFILE)
            printf("COM ");

        if (stVirusUserInfo.dwInfo & bitINFO_EXEFILE)
            printf("EXE ");

        if (stVirusUserInfo.dwInfo & bitINFO_SYSFILE)
            printf("SYS ");

        if (stVirusUserInfo.dwInfo & bitINFO_FLOPBOOT)
            printf("FBR ");

        if (stVirusUserInfo.dwInfo & bitINFO_HDISKBOOT)
            printf("PBR ");

        if (stVirusUserInfo.dwInfo & bitINFO_MBOOTREC)
            printf("MBR ");

        printf("\n");


    }
}


//*************************************************************************
//
// Function:
//  VirusListDeleteVirusVID()
//
// Parameters:
//  lpScanGlobal        Pointer to global data
//
// Description:
//  Deletes the virus corresponding to the VID given on the command
//  line.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void VirusListDeleteVirusVID
(
    LPSCANGLOBAL        lpScanGlobal
)
{
    WORD                wVID;
    HVIRUS              hVirus;
    BOOL                bFound;

    wVID = lpScanGlobal->lpCmdOptions->wRemVirVID;

    if (EngVIDToHVIRUS(lpScanGlobal->hGEng,
                       wVID,
                       &hVirus,
                       &bFound) == ENGSTATUS_ERROR ||
        bFound == FALSE ||
        EngDeleteVirusEntry(lpScanGlobal->hGEng,
                            hVirus) == ENGSTATUS_ERROR)
    {
        printf(gszMsgStrFailedVirusDelete,wVID);
        return;
    }

    printf(gszMsgStrSuccessfulVirusDelete,wVID);
}

