#include "platform.h"
#include <stdio.h>
#include "ctsn.h"
#include "callback.h"
#include "navex.h"

void main(int argc, char **argv)
{
    DWORD dwAddress;
    PEXPORT_TABLE_TYPE lpNavexFuncs;

    sscanf(argv[1],"%08lX",&dwAddress);
    lpNavexFuncs = (PEXPORT_TABLE_TYPE)dwAddress;

    lpNavexFuncs->EXTScanFile = EXTScanFile;
    lpNavexFuncs->EXTScanBoot = EXTScanBoot;
    lpNavexFuncs->EXTScanPart = EXTScanPart;
    lpNavexFuncs->EXTScanMemory = EXTScanMemory;
    lpNavexFuncs->EXTRepairFile = EXTRepairFile;
    lpNavexFuncs->EXTRepairBoot = EXTRepairBoot;
    lpNavexFuncs->EXTRepairPart = EXTRepairPart;
}
    


