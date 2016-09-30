#include "platform.h"

// Standard includes
#include <stdio.h>
#include <stdlib.h>

#include "snavapi.h"
#include "scanboot.h"
#include "scanmbr.h"
#include "scanmem.h"
#include "scanfile.h"
#include "apiver.h"

// WIN 32 declarations
#if defined (SYM_WIN32)
	#include <windows.h>

// UNIX declarations
#elif defined (SYM_UNIX)

// NLM declarations
#elif defined (SYM_NLM)

#endif


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		DWORD main()
//
//	Description:
//
//	Parameters:
//
//	Returns:
//
///////////////////////////////////////////////////////////////////////////////
DWORD main (int argc, char* argv[])
{

	if (argc < 2)
	{
		help();
		return (1);
	}

	switch (GetOption(argv[1]))
	{

#if defined (SYM_UNIX) || defined (SYM_NLM)

		case SCAN_FILE:
			ScanFile (argc, argv);
			break;

		case INFO_TEST_M:
			InfoTest (argc, argv);
			break;

		case NAVAPI_VER:
			Version (argc, argv);
			break;

		case NAVAPI_HELP:
			PrintDetailedHelp();
			break;

#else 
		case SCAN_BOOT:
			ScanBoot (argc, argv);
			break;

		case SCAN_MBR:
			ScanMBR (argc, argv);
			break;

		case SCAN_FILE:
			ScanFile (argc, argv);
			break;

		case SCAN_MEMORY:
			ScanMemory (argc, argv);
			break;

		case INFO_TEST_M:
			InfoTest (argc, argv);
			break;

		case NAVAPI_VER:
			Version (argc, argv);
			break;

		case NAVAPI_HELP:
			PrintDetailedHelp();
			break;

#endif


		default:
			help();
			break;

	}

	return (0);
}


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		void help()
//
//	Description:
//
//	Parameters:
//
//	Returns:
//
///////////////////////////////////////////////////////////////////////////////
void help ( )
{
	printf("SNAVAPI version 2.1.0\n");
#if defined (SYM_NLM)
	printf("Usage: snavapi.exe [option] [parameters]\n");
#elif defined (SYM_UNIX)
	printf("Usage: snavapi [option] [parameters]\n");
#else
	printf("Usage: snavapi.exe [option] [parameters]\n");
#endif

	printf("Options:\n");

#if defined (SYM_UNIX) || defined (SYM_NLM)	
	printf("    FILE      Scan files\n");
	printf("    INFO      NAVAPI info function tests\n");
	printf("    VER       NAVAPI version\n");
	printf("    HELP      Display detailed help\n");

#else
	printf("    MEMORY    Scan memory\n");
	printf("    FILE      Scan files\n");
	printf("    BOOT      Scan boot sector\n");
	printf("    MBR       Scan master boot record\n");
	printf("    INFO      NAVAPI info function tests\n");
	printf("    VER       NAVAPI version\n");
	printf("    HELP      Display detailed help\n");

#endif
}



	
///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		WORD GetOption()
//
//	Description:
//
//	Parameters:
//
//	Returns:
//
///////////////////////////////////////////////////////////////////////////////
WORD GetOption
(
	LPTSTR	lpszOption
)
{
	char	szBuffer[80]={0};
    int     iCnt;
	strcpy (szBuffer, lpszOption);
	strupr (szBuffer);

	for (iCnt = 0; iCnt < (sizeof(stOptionList))/(sizeof(OPTION_LIST)); iCnt++)
	{
		if (strstr(stOptionList[iCnt].szOptionStr, szBuffer))
		{
			return(stOptionList[iCnt].Option);
		}

	}
	return(0);
}


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		WORD PrintDetailedHelp()
//
//	Description:
//
//	Parameters:
//
//	Returns:
//
///////////////////////////////////////////////////////////////////////////////
void PrintDetailedHelp(void)
{
	printf("OVERVIEW:\n");
	printf("Snavapi is a test harness designed to test NAVAPI. Snavapi will\n");
	printf("allow you to run all functions in NAVAPI.\n");
	printf("\n");
	printf("USAGE EXAMPLES:\n");
	printf("On a windows system, if you have snavapi32.exe in the c:\\snav\n");
	printf("directory, navapi32.dll in the c:\\snav directory, and windows\n");
	printf("virus definitions in the c:\\snav\\defs directory, you can scan\n");
	printf("files in the directory c:\\test by typing the following from the\n");
	printf("c:\\snav directory:\n");
	printf("c:\\snav>snavapi32 file /defs=defs /dir=c:\\test\n");
	printf("To scan and repair a floppy's boot record, type the following:\n");
	printf("c:\\snav>snavapi32 boot /defs=defs /drv=a: /rep\n");
	printf("\n");
	printf("SCANNING OPTIONS:\n");
	printf("/HEUR=:   Set heuristics level. This can range from 0 to 2.\n");
	printf("/INF=:    Full path to optional engine config file, including filename.\n");
	printf("/INFO=:   Run info tests in addition to scanning tests. The info test\n");
	printf("          will first load every virus def using the NAVLoadVirusDefTable\n");
	printf("          functions. For each virus detected in the scan, compare the\n");
	printf("          NAVGetVirusInfo info returned from the scan with the values\n");
	printf("          from NAVLoadVirusDef and NAVLoadVirusDefTable.\n");
	printf("/ITERATIONS=: How many times to scan each file/boot/mbr. This will do the\n");
	printf("              scan this many times. This is useful to get a more accurate\n");
	printf("              timing per file, or for testing lockup situations. The time\n");
	printf("              per file will be the average time to scan each iteration.\n");
	printf("              Specifying zero for this value will cause each scan to repeat\n");
	printf("              forever (only the first item will be scanned). Note that an\n");
	printf("              extra scan is done before running the number of scans\n");
	printf("              if /ITERATIONS is greater than 1. This places the item being\n");
	printf("              scanned into the system cache, giving more accurate timing\n");
	printf("              results. On Win32, per file timing is done using the High\n");
	printf("              Performance Frequency counter. Other platforms do not\n");
	printf("              currently have per file timing in SNAVAPI. Overall scan\n");
	printf("              time is calculated using the time() function.\n");
	printf("/PLOG=:   Log short scan results into specified file.\n");
	printf("/FLOG=:   Log short scan results into specified file.\n");
	printf("/MD5:     Generate an MD5 hash before and after scanning/repairing.\n");
	printf("/TAB:     Print output to screen and \"/PLOG\" in semi-colon delimited format. (see below)\n");
	printf("(note: Timing functions are implemented for file scanning only.\n");
	printf("\n");
	printf("INFO TEST (SNAVAPI32.EXE INFO):\n");
	printf("The info test option is designed to find errors in the info functions\n");
	printf("of NAVAPI that can be found automatically. Errors in the actual\n");
	printf("information will have to be verified by hand from looking at the virus\n");
	printf("definitions.\n");
	printf("\n");
	printf("The first test will load every bool value from the GetVirusDefTableInfo\n");
	printf("function, comparing them to see if any of them are returning the same\n");
	printf("value for all virus entries. If so, it reports this. It also reports\n");
	printf("the total entries processed.\n");
	printf("\n");
	printf("The other tests use the other Info functions, and return their values.\n");
	printf("You need to check these values manually from looking at the actual virus\n");
	printf("definition source.\n");
	printf("\n");
	printf("NOTE: This is for format of the semi-colon delimited output:\n");
	printf("1. filepath\n");
	printf("2. Scan Result Code\n");
	printf("3. Is file infected?\n");
	printf("4. Virus Name\n");
	printf("5. Was Repair or Delete attempted?\n");
	printf("6. Repair/Delete result\n");
	printf("7. Rep/Delete Result code\n");
	printf("8. Time to process this file\n");
	printf("9. MD5 hash before repair or delete\n");
	printf("10. Number of files scanned\n");
	printf("11. Number of infected files\n");
	printf("12. Number of repaired files\n");
	printf("13. Number of deleted files\n");
}
