#include "platform.h"
#include <stdio.h>

#include "ctsn.h"
#include "callback.h"
#include "navex.h"

void LoadHeuristics(LPSTR);         // found in loadheur.cpp

extern char gszStartUpDir[256];     // in mcrheuen.cpp

void main(int argc, char **argv)
{
    DWORD dwAddress;
    PEXPORT_TABLE_TYPE lpNavexFuncs;
    int i;
    int nLastBackslash;

    sscanf(argv[1],"%08lX",&dwAddress);
    lpNavexFuncs = (PEXPORT_TABLE_TYPE)dwAddress;

    lpNavexFuncs->EXTScanFile = EXTScanFile;
    lpNavexFuncs->EXTScanBoot = EXTScanBoot;
    lpNavexFuncs->EXTScanPart = EXTScanPart;
    lpNavexFuncs->EXTScanMemory = EXTScanMemory;
    lpNavexFuncs->EXTRepairFile = EXTRepairFile;
    lpNavexFuncs->EXTRepairBoot = EXTRepairBoot;
    lpNavexFuncs->EXTRepairPart = EXTRepairPart;

#ifndef NAVEX15
    // Store the startup directory

    nLastBackslash = -1;
    for (i=0;argv[0][i] && i < 255;i++)
    {
        if ((gszStartUpDir[i] = argv[0][i]) == '\\')
            nLastBackslash = i;
    }

    gszStartUpDir[nLastBackslash + 1] = 0;
#endif // #ifndef NAVEX15

    // now take care of loading heuristics if necessary

#if !defined(NAVEX15)
#if defined (USE_BLOODHOUND_PICASSO)
    LoadHeuristics(argv[0]);
#endif // #if defined (USE_BLOODHOUND_PICASSO)
#endif
}
    


