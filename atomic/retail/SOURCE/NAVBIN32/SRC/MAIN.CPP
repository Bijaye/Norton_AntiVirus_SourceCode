///////////////////////////////////////////////////////////////////////////////////////
// $Header:   S:/NAVBIN32/VCS/main.cpv   1.4   19 Apr 1998 14:45:20   tcashin  $
///////////////////////////////////////////////////////////////////////////////////////
//
// NAVBIN32 implememtation file
//
//
//
//
//
//
//
///////////////////////////////////////////////////////////////////////////////////////
// $Log:   S:/NAVBIN32/VCS/main.cpv  $
// 
//    Rev 1.4   19 Apr 1998 14:45:20   tcashin
// Enable quarantine button (in "buttons to display when prompted").
// 
//    Rev 1.3   15 Apr 1998 17:24:56   MKEATIN
// Added Heurstic defaults
// 
//    Rev 1.2   15 Jun 1997 19:43:42   DBUCHES
// Added heuristic flags
//
//    Rev 1.1   23 May 1997 19:49:30   DBUCHES
// Added support for custom response and quarantine options.
//
//    Rev 1.0   20 May 1997 17:32:00   DBUCHES
// Initial revision.
///////////////////////////////////////////////////////////////////////////////////////

#pragma pack(push, 1)
#include "platform.h"
#define CREATE_NAVBIN
#include "options.h"
#include "navbin.h"
#pragma pack(pop)

#include "windows.h"
#include "stdio.h"



///////////////////////////////////////////////////////////////////////////////////////
// Protos

int CreateNAVBin();
void GetData();
void GetUser();
void GetTsr();
void GetActivity();
void GetAlert();
void GetInoc();
void GetScanner();
void GetPassword();
void GetGeneral();
void GetQuarantine();
void GetCustomResponse();
void GetCustomResponseAP();

///////////////////////////////////////////////////////////////////////////////////////
// Locals


char gszInputFile[MAX_PATH];
char gszOutputFile[MAX_PATH];

NAVOPTIONS  navOpts;

///////////////////////////////////////////////////////////////////////////////////////
// Macros

#define GET_BYTE(section, field)(navOpts.##section.##field = (BYTE)GetPrivateProfileInt(#section, #field, 0, gszInputFile))

#define GET_WORD(section, field)( navOpts.##section.##field = (WORD)GetPrivateProfileInt(#section, #field, 0, gszInputFile))

#define GET_DWORD(section, field)( navOpts.##section.##field = (DWORD)GetPrivateProfileInt(#section, #field, 0, gszInputFile))

#define GET_STRING(section, field, length)(GetPrivateProfileString(#section, #field, "", (LPSTR)navOpts.##section.##field, length, gszInputFile))


///////////////////////////////////////////////////////////////////////////////////////
// Code



int main(int argc, char* argv[])
{
    // Display banner.
    printf("\nNAVBIN32 - NAVOPTS creation utility\nCopyright (c)1997 Symantec Corporation\n");


    // Make sure required parameters are present.
    if(argc < 2 || strchr(argv[1], '?'))
	{
	if(argc < 2)
	    MessageBeep(0);
	printf("NAVBIN usage: NAVBIN inputfilename <outputfilename>\n");
	return -1;
	}

    // Save off input filename
    if( strchr(argv[1], ':') || strchr(argv[1], '\\'))
	lstrcpyn(gszInputFile, argv[1], MAX_PATH);
    else
	{
	// Get file from the current directory
	char szCurrentDir[MAX_PATH];

	GetCurrentDirectory(MAX_PATH, szCurrentDir );

	if( lstrlen(szCurrentDir) + lstrlen(argv[1]) > MAX_PATH)
	    {
	    MessageBeep(0);
	    printf("Error: Path too long.\n");
	    return -1;
	    }
	lstrcpy(gszInputFile, szCurrentDir);
	lstrcat(gszInputFile, "\\");
	lstrcat(gszInputFile, argv[1]);
	}

    // Save off output file if necessary
    if(argc > 2)
	lstrcpyn(gszOutputFile, argv[2], MAX_PATH);
    else
	lstrcpy(gszOutputFile, "NAVOPTS.DAT");

    // fire away!
    int iRet = CreateNAVBin();

    // display stats.
    if(iRet == 0)
	{
	printf("Done.\nCreated: %s\nfrom INI file: %s\n", gszOutputFile, gszInputFile);
	}

    return iRet;

}


int CreateNAVBin()
{
    int iRet = 0;

    // Make sure we can read input file.
    if(GetFileAttributes(gszInputFile) == 0xFFFFFFFF)
	{
	MessageBeep(0);
	printf("Error opening input file %s\n", gszInputFile);
	return -1;
	}

    // Create output file.
    HANDLE hOutputFile = CreateFile( gszOutputFile,
				    GENERIC_WRITE,
				    0,
				    NULL,
				    CREATE_ALWAYS,
				    FILE_ATTRIBUTE_NORMAL,
				    NULL);

    if(hOutputFile == INVALID_HANDLE_VALUE)
	{
	MessageBeep(0);
	printf("Cannot create output file: %s.\n", gszOutputFile );
	return -1;
	}

    // Retrieve data from ini file.
    GetData();

    // Save data to output file.

    // Create and save file header.
    NAVBINHEADER header;
    DWORD dwBytesWritten;
    ZeroMemory(&header, sizeof(NAVBINHEADER));
    GetPrivateProfileString("header", "signature", "NAVOPTS.DAT", header.szSignature, 15, gszInputFile);

    // Add 0x1A so when a user tries to TYPE this file, only the signature
    // will be displayed.
    lstrcat(header.szSignature,"\x1A");
    header.dwVersion = GetPrivateProfileInt("header","dwVersion", 0, gszInputFile);

    WriteFile(hOutputFile,
	      (LPCVOID)&header,
	      sizeof(NAVBINHEADER),
	      &dwBytesWritten,
	      NULL);

    if(dwBytesWritten != sizeof(NAVBINHEADER))
	{
	MessageBeep(0);
	printf("Error writing to outout file %s.\n", gszOutputFile);
	iRet = -1;
	goto cleanup;
	}

    // Save NAVOPTS.
    WriteFile(hOutputFile,
	      (LPCVOID)&navOpts,
	      sizeof(NAVOPTIONS),
	      &dwBytesWritten,
	      NULL);

    if(dwBytesWritten != sizeof(NAVOPTIONS))
	{
	MessageBeep(0);
	printf("Error writing to outout file %s.\n", gszOutputFile);
	iRet = -1;
	goto cleanup;
	}

cleanup:
    // Cleanup.
    CloseHandle(hOutputFile);

    return iRet;
}


void GetData()
{

    // Get User data.
    GetUser();

    // Get TSR data
    GetTsr();

    // Get Activity data
    GetActivity();

    // Get Alert data
    GetAlert();

    // Get Inoc data
    GetInoc();

    // Get Scanner data
    GetScanner();

    // Get Password data
    GetPassword();

    // Get General data
    GetGeneral();

}


void GetUser()
{
    GET_STRING(user, szName, MAX_USER_INFO + 1);
    GET_STRING(user, szCompany, MAX_USER_INFO + 1);
}

void GetTsr()
{
    GET_BYTE( tsr, bScanUnknown );
    GET_BYTE( tsr, bScanRun );
    GET_BYTE( tsr, bScanOpen );
    GET_BYTE( tsr, bScanCreate );
    GET_BYTE( tsr, bScanAll );
    GET_BYTE( tsr, uActionKnown );
    GET_BYTE( tsr, uActionUnknown );
    GET_BYTE( tsr, bDispRepair );
    GET_BYTE( tsr, bDispDelete );
    GET_BYTE( tsr, bDispContinue );
    GET_BYTE( tsr, bDispStop );
    GET_BYTE( tsr, bDispExclude );
    GET_BYTE( tsr, bDispUnknownRepair );
    GET_BYTE( tsr, bDispUnknownDelete );
    GET_BYTE( tsr, bDispUnknownContinue );
    GET_BYTE( tsr, bDispUnknownExclude );
    GET_BYTE( tsr, bDispQuarantine );
    GET_BYTE( tsr, bScanMem );
    GET_BYTE( tsr, bScanMBoot );
    GET_BYTE( tsr, bScanBootRecs );
    GET_BYTE( tsr, bUseHMA );
    GET_BYTE( tsr, bUseUMB );
    GET_BYTE( tsr, bUseEMS );
    GET_BYTE( tsr, bUseXMS );
    GET_BYTE( tsr, uBypassKey );
    GET_BYTE( tsr, bUnloadable );
    GET_BYTE( tsr, bHideIcon );
    GET_BYTE( tsr, uFormat );
    GET_BYTE( tsr, uWriteHard );
    GET_BYTE( tsr, uWriteFlop );
    GET_BYTE( tsr, uWriteExe );
    GET_BYTE( tsr, uROAttrib );
    GET_BYTE( tsr, bChkFlopAccess );
    GET_BYTE( tsr, bChkFlopOnBoot );
    GET_BYTE( tsr, bChkBothFlops );
    GET_BYTE( tsr, bEnableHeuristicScan);
    GET_BYTE( tsr, uHeuristicLevel);
}

void GetActivity()
{

    GET_BYTE( activity, bKnown );
    GET_BYTE( activity, bUnknown );
    GET_BYTE( activity, bInocChange );
    GET_BYTE( activity, bVirusLike );
    GET_BYTE( activity, bStartEnd );
    GET_BYTE( activity, bVirusList );
    GET_BYTE( activity, bKeepLast );
    GET_BYTE( activity, bQuarantine );
    GET_WORD( activity, uEntries );
    GET_BYTE( activity, bKeepUpTo );
    GET_WORD( activity, uKiloBytes );
    GET_STRING( activity, szLogFile, MAX_PATH_SIZE+1 );

}

void GetAlert()
{
    GET_BYTE(alert, bDispAlertMsg );
    GET_STRING(alert, szAlertMsg, SIZE_ALERT_MSG+1 );
    GET_BYTE(alert, bRemoveAfter );
    GET_BYTE(alert, uSeconds );
    GET_BYTE(alert, bBeep );
    GET_BYTE(alert, bAlertNetUsr );
    GET_BYTE(alert, bAlertNetCon );
    GET_BYTE(alert, bAlertNLM );
    GET_BYTE(alert, bKnown );
    GET_BYTE(alert, bUnknown );
    GET_BYTE(alert, bInoculation );
    GET_BYTE(alert, bVirusLike );
    GET_BYTE(alert, bStartEnd );
    GET_BYTE(alert, bVirListChange );
}

void GetInoc()
{
    GET_BYTE(inoc, bUseSystemInoc );
    GET_BYTE(inoc, bUseInoc );
    GET_BYTE(inoc, bInocFloppies );
    GET_BYTE(inoc, bInocFast );
    GET_BYTE(inoc, uActionNewInoc );
    GET_BYTE(inoc, uActionInocChg );
    GET_BYTE(inoc, bDispRepair );
    GET_BYTE(inoc, bDispDelete );
    GET_BYTE(inoc, bDispInoc );
    GET_BYTE(inoc, bDispCont );
    GET_BYTE(inoc, bDispStop );
    GET_BYTE(inoc, bDispExcl );
    GET_STRING(inoc, szNetInocPath, MAX_PATH_SIZE+1 );
}

void GetScanner()
{
    GET_BYTE( scanner, bMemory );
    GET_BYTE( scanner, bMasterBootRec );
    GET_BYTE( scanner, bBootRecs );
    GET_BYTE( scanner, bScanAll );
    GET_BYTE( scanner, uAction );
    GET_BYTE( scanner, bDispRepair );
    GET_BYTE( scanner, bDispDelete );
    GET_BYTE( scanner, bDispCont );
    GET_BYTE( scanner, bDispExcl );
    GET_BYTE( scanner, bDispQuarantine );
    GET_BYTE( scanner, bAllowNetScan );
    GET_BYTE( scanner, bAllowScanStop );
    GET_BYTE( scanner, bPrestoNotify );
    GET_BYTE( scanner, bPresFlop );
    GET_BYTE( scanner, bPresHard );
    GET_BYTE( scanner, bPresNet );
    GET_BYTE( scanner, bEnableHeuristicScan);
    GET_BYTE( scanner, uHeuristicLevel);
}

void GetPassword()
{
    GET_BYTE( password, bProtect);
    GET_BYTE( password, bMaxProtection);
    GET_DWORD( password, dwPassword);

    // Special case for array of password protect values.
    for(int i = PASSWORD_VIEWACTIVITY; i < MAX_PROTECT_FEATURE; i++)
	{
	char szField[32];
	wsprintf(szField, "passwordField%d", i);
	navOpts.password.bProtectFeature[i] =
	    GetPrivateProfileInt( "password", szField, 0, gszInputFile);
	}

}

void GetGeneral()
{
    GET_BYTE( general, bBackupRep );
    GET_STRING( general, szBackExt, SIZE_EXTENSION+1 );
    GET_BYTE( general, bScanZipFiles );
    GET_BYTE( general, bScanHighMemory );
    GET_STRING( general, szExt, SIZE_EXTENSION_LIST+1 );
    GET_STRING( general, szDef, SIZE_EXTENSION_LIST+1 );
}

void GetQuarantine()
{
    GET_BYTE(quarantineset, bQuarantineAP);
    GET_BYTE(quarantineset, bQuarantineScan);
    GET_BYTE(quarantineset, bQuarantineInoc);
    GET_STRING(quarantineset, szQuarantineDir, 260);
}

void GetCustomResponse()
{
	GET_BYTE(customScan, bSelected);
	GET_BYTE(customScan, uchMacro);
	GET_BYTE(customScan, uchFile);
	GET_BYTE(customScan, uchBoot);
}
void GetCustomResponseAP()
{
	GET_BYTE(customAP, bSelected);
	GET_BYTE(customAP, uchMacro);
	GET_BYTE(customAP, uchFile);
	GET_BYTE(customAP, uchBoot);
}




