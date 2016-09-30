// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/CMDLINE/VCS/msgstr.cpv   1.3   12 Jun 1996 12:17:08   RAY  $
//
// Description:
//  Message strings.
//
// Contains:
//
// See Also:
//
//************************************************************************
// $Log:   S:/CMDLINE/VCS/msgstr.cpv  $
// 
//    Rev 1.3   12 Jun 1996 12:17:08   RAY
// Reorganized boot scanning code
// 
//    Rev 1.2   05 Jun 1996 21:01:34   RAY
// Added boot scanning and repair support
// 
//    Rev 1.1   21 May 1996 12:34:52   DCHI
// Added '\n' to end of scanning directory string.
// 
//    Rev 1.0   21 May 1996 12:25:48   DCHI
// Initial revision.
// 
//************************************************************************

#include "msgstr.h"

// CMDLINE.CPP

char gszMsgStrTerminating[] = "Terminating...\n";
char gszMsgStrParse[] = "Parse";
char gszMsgStrInvalidOption[] = "Invalid option error: %s\n";
char gszMsgStrUnknownError[] = "Unknown error.\n";
char gszMsgStrErrorGettingHomeDir[] =
    "Error getting home or initial directory!\n";
char gszMsgStrScanningMemory[] =
    "Scanning memory...\n";
char gszMsgStrScanningMBR[] =
    "Scanning master boot record...\n";
char gszMsgStrScanningBoot[] =
    "Scanning boot records...\n";
char gszMsgStrScanningFiles[] =
    "Scanning files...\n";

// SCANFILE.CPP

char gszMsgStrScanFileInit[] = "Scan file init";
char gszMsgStrScanFileClose[] = "Scan file close";

char gszMsgStrScanningDirectory[] = "Scanning directory: %s\n";

// SCANMEM.CPP

char gszMsgStrScanMemoryInit[] = "Scan memory init";
char gszMsgStrScanMemoryClose[] = "Scan memory close";

// SCANBOOT.CPP

char gszMsgStrScanBootInit[] =
    "boot scanner init";
char gszMsgStrScanBootClose[] =
    "boot scanner close";
char gszReportBootScanError[] =
    "Error scanning boot sector";
char gszReportFloppyScanError[] =
    "Error scanning floppy boot sector";
char gszReportMBRScanError[] =
    "Error scanning MBR";

char gszReportBootInfection[] =
    "Drive %x, Partition %d is infected with the [%s][%04X] virus.\n";

char gszReportFloppyInfection[] =
    "Drive %c: is infected with the [%s][%04X] virus.\n";

char gszReportMBRInfection[] =
    "Drive %x is infected with the [%s][%04X] virus.\n";

// SCANINIT.CPP

char gszMsgStrGlobalInit[] = "Global init";
char gszMsgStrGlobalClose[] = "Global close";

char gszMsgStrLocalInit[] = "Local init";
char gszMsgStrLocalClose[] = "Local close";

// VIRLIST.CPP

char gszMsgStrVirusName[] = "Virus Name";   // Up to 22 characters
char gszMsgStrVID[] = "VID";                // Up to 4 characters
char gszMsgStrSize[] = "Size";              // Up to 5 characters
char gszMsgStrInfects[] = "Infects";        // Up to 24 characters

char gszMsgStrErrorAccessingVirusList[] =
    "Error accessing virus list!\n";

char gszMsgStrFailedVirusDelete[] =
    "Failed to delete virus with ID: %04X\n";

char gszMsgStrSuccessfulVirusDelete[] =
    "Successfully deleted virus with ID: %04X\n";

// REPORT.CPP

// Virus found action strings

char gszReportInfection[] =
    "%s is infected with the [%s][%04X] virus.\n";

char gszReportDelete[] =
    "This file was deleted.\n";

char gszReportRepair[] =
    "The [%s] infection was repaired.\n";

char gszReportFailedRepair[] =
    "The [%s] infection could not be repaired.\n";

char gszReportFailedDelete[] =
    "The file could not be deleted.\n";

char gszReportFailedRename[] =
    "The file could not be renamed.\n";

char gszReportRename[] =
    "This file was renamed to %s.\n";

char gszReportNoAction[] =
    "No action was taken on this infection.\n";

char gszReportMultipleInfection[] =
    "This file is also multiply infected with the [%s][%04X] virus.\n";

char gszReportRepairBackUpFailed[] =
    "Failure backing up infected file %s.\n";

char gszReportFileAccessError[] =
    "Error accessing file %s.\n";

// Error strings

char gszReportErrorMemAlloc[] =
    "Error during memory allocation.\n";

char gszReportErrorMemFree[] =
    "Error during memory free.\n";

char gszReportErrorFileAccess[] =
    "File access error.\n";

char gszReportErrorInit[] =
    "Initialization error.\n";

char gszReportErrorClose[] =
    "Closedown error.\n";

char gszReportErrorDefaultError[] =
    "Error.";

