
#include "platform.h"
#include "xapi.h"
#include "ctsn.h"

#define UNDOBOOT_STRING_CONSTANT       "UNDOBOOT"

const char szUndobootAltered[] =
{
"The "UNDOBOOT_STRING_CONSTANT".EXE file has been altered.  This may\n\
be due to virus activity.\n\n\
Please exit all applications  and turn off your computer.\n\
Then restart your computer from your NAVDX Rescue Disk and\n\
scan "UNDOBOOT_STRING_CONSTANT".EXE for viruses.\n\
If no virus is found, try reinstalling Norton AntiVirus\n\
from your original disks.\n"
};

const char szMBR[] = "MBR";
const char szBOOT_RECORD[] = "Boot record";
const char szRestoring[] = "Restoring: %s...\n";
const char szMBRSuccessRestore[] = "\tMBR successfully restored.\n";
const char szBootRecordSuccessRestore[] = "\tBoot record #%d successfully restored.\n";
const char szMBRFailedRestore[] = "\tFailed to resotre MBR.\n";
const char szBootRecordFailedRestore[] = "\tFailed to restore boot record #%d.\n";

const char szDataDoesNotDiffer[] = 
{ 
"MBR and BOOT record data located in: %s do not differ from system.\n\
Restore operation is unnecessary for this system.\n"
};

const char szErrorOpeningDataFile[] = 
{
"Error opening and or reading data file: %s\n\
Restore operation failed.\n"
};

const char szErrorInitRestoreUtility[] = 
{
"Error initializing restore utility.\n\
Restore operation failed\n"
};

const char szHelp[] = 
{
"*** Norton AntiVirus Inoculation undo utility ***\n\n\
Usage: "UNDOBOOT_STRING_CONSTANT".EXE [ UNDO DATA FILE ]\n"
};







