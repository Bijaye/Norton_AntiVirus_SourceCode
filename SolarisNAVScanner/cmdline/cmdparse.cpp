// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/CMDLINE/VCS/CMDPARSE.CPv   1.3   12 Aug 1996 18:46:46   DCHI  $
//
// Description:
//  Functions used for parsing the command line.
//
// Contains:
//  FreeCommandLineData()
//  CommandLineParse()
//  PrintOptions()
//
// See Also:
//
//************************************************************************
// $Log:   S:/CMDLINE/VCS/CMDPARSE.CPv  $
// 
//    Rev 1.3   12 Aug 1996 18:46:46   DCHI
// Modifications for UNIX.
// 
//    Rev 1.2   12 Jun 1996 12:11:16   RAY
// Added /NOFILE switch to skip file scanning
// 
//    Rev 1.1   03 Jun 1996 17:21:00   DCHI
// Changes to support building on BORLAND, VC20, SYM_W32, SYM_DOS, and SYM_DOSX.
// 
//    Rev 1.0   21 May 1996 12:25:26   DCHI
// Initial revision.
// 
//************************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "avenge.h"

#include "cmdparse.h"

#ifdef SYM_DOS

extern "C" {
int __cdecl __memicmp(const void *, const void *, unsigned int);
}

#define memicmp     _memicmp

#endif // SYM_DOS

#ifdef SYM_UNIX

#include <ctype.h>

int memicmp(const void *s1, const void *s2, size_t n)
{
    int nDiff;

    while (n-- > 0)
    {
        nDiff = toupper(*(unsigned char *)s1) - 
	    toupper(*(unsigned char *)s2);
	if (nDiff != 0)
	    return nDiff;
    }

    return 0;
}

#endif // SYM_UNIX

//*************************************************************************
//
// Function:
//  ObtainFileAfterEquals()
//
// Parameters:
//  lpszOptionString    String containing file name after '='
//  lpszFileName        Buffer to store file name
//
// Description:
//  Searches for the first '=' in lpszOptionString and stores the
//  string following it into lpszFileName.
//
// Returns:
//  FALSE           If '=' was not found in lpszOptionString
//  TRUE            If a string was successfully copied to lpszFileName
//
//*************************************************************************

BOOL ObtainFileAfterEquals
(
	LPSTR		lpszOptionString,
	LPSTR      	lpszFileName
)
{
	int i;

	for (i=0;lpszOptionString[i];i++)
		if (lpszOptionString[i] == '=')
		{
			strcpy(lpszFileName,&lpszOptionString[i+1]);
			return(TRUE);
		}

	return(FALSE);
}


//*************************************************************************
//
// Function:
//  ObtainHexWORDValueAfterEquals()
//
// Parameters:
//  lpszOptionString    String containing file name after '='
//  lpwValue            Pointer to WORD to store hex WORD value
//
// Description:
//  Searches for the first '=' in lpszOptionString and stores the
//  value of the hexadecimal WORD following it into lpwValue.
//
// Returns:
//  FALSE           If '=' was not found in lpszOptionString
//  TRUE            If a WORD was successfully copied to lpwValue
//
//*************************************************************************

BOOL ObtainHexWORDValueAfterEquals
(
    LPSTR       lpszOptionString,
    LPWORD      lpwValue
)
{
    int         i;
    unsigned    uValue;

	for (i=0;lpszOptionString[i];i++)
		if (lpszOptionString[i] == '=')
		{
            sscanf(lpszOptionString+i+1,"%x",&uValue);
            *lpwValue = (WORD)uValue;
			return(TRUE);
		}

	return(FALSE);
}


//*************************************************************************
//
// Function:
//  CompareStringsIN()
//
// Parameters:
//  lpszInternal        The internal string
//  nLenInternal        Length of the internal string
//  lpszUser            The user string
//  nLenUser            Length of the user string
//
// Description:
//  Compares the string at lpszInternal with the string at lpszUser.
//  Only the first nLenInternal characters need to match.
//
// Returns:
//  FALSE           The internal string does not match the user string
//  TRUE            The internal string does match the user string
//
//*************************************************************************

BOOL CompareStringsIN
(
    LPSTR   lpszInternal,
	int     nLenInternal,
	LPSTR  	lpszUser,
    int     nLenUser
)
{
    if (nLenInternal > nLenUser)
        return(FALSE);

    if (!memicmp(lpszInternal,lpszUser,nLenInternal))
        return(TRUE);

	return(FALSE);						// no match
}


//*************************************************************************
//
// Function:
//  CommandLineMatch()
//
// Parameters:
//  lpszOption          The option string from the command line
//  lpdwID              The ID of the matching option if found
//
// Description:
//  Performs a linear search through the valid command line options
//  looking for a match with the user given option.
//
// Returns:
//  FALSE           No matching valid command line option found
//  TRUE            A matching valid command line option was found
//
//*************************************************************************

BOOL CommandLineMatch
(
	LPSTR   	lpszOption,
	LPDWORD     lpdwID
)
{
	int                 i;
	extern  COPTS_T     CommandOpts[];

	for (i=0;CommandOpts[i].lpszOption != NULL;i++)
	{
		if (CompareStringsIN(CommandOpts[i].lpszOption,
							 strlen(CommandOpts[i].lpszOption),
							 lpszOption,
							 strlen(lpszOption)))
		{
			*lpdwID = CommandOpts[i].dwID;

			return(TRUE);
		}
	}

	return(FALSE);
}


//*************************************************************************
//
// Function:
//  FreeCommandLineData()
//
// Parameters:
//  lpCommandOptions    Pointer to command line data
//
// Description:
//  Frees memory allocated during command line data retrieval.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void FreeCommandLineData
(
	LPCMD_OPTIONS  lpCommandOptions
)
{
	LPSCAN_NODE     lpScanNode, lpScanNext;

	lpScanNode = lpCommandOptions->lpScanList;

	while (lpScanNode != NULL)
	{
		lpScanNext = lpScanNode->lpNext;
        free(lpScanNode);
		lpScanNode = lpScanNext;
	}

	lpCommandOptions->lpScanList = NULL;
}


//*************************************************************************
//
// Function:
//  CommandLineParse()
//
// Parameters:
//  argc                Number of command line strings
//  argv[]              Array of pointers to command line strings
//  lpCommandOptions    Pointer to command line data structure
//  lpwInvalidOption    If an invalid option was found, the WORD
//                      will be set to the index of the invalid option
//
// Description:
//  Parses command line, allocating memory as necessary for command
//  line option storage.
//
// Returns:
//  CMDSTATUS_INVALID_OPTION    Invalid option found on command line
//  CMDSTATUS_MEM_ERROR         Memory allocation error
//  CMDSTATUS_OK                Success
//
//*************************************************************************

CMDSTATUS CommandLineParse
(
	int                 argc,
	char                *argv[],
	LPCMD_OPTIONS       lpCommandOptions,
	LPWORD				lpwInvalidOption
)
{
    int                 i;
	LPSCAN_NODE         lpTempScanNode;
	DWORD				dwID;

	// reset all options

	lpCommandOptions->dwFlags = 0;
	lpCommandOptions->lpScanList = NULL;

	// go through command line arguments items

	for (i=argc-1;i>=FIRST_COMMAND_LINE_OBJECT;i--)
	{
		if (argv[i][0] == OPTION_CHAR1 ||
#ifdef SYM_UNIX
			0)
#else
			argv[i][0] == OPTION_CHAR2)
#endif
		{
			if (CommandLineMatch(&argv[i][1],
								 &dwID) == FALSE)
			{
				*lpwInvalidOption = i;

                return(CMDSTATUS_INVALID_OPTION);       // unknown option
			}

			switch(dwID)
			{
				case OPTION_FLAG_REPORT:
					if (!ObtainFileAfterEquals(argv[i],
											   lpCommandOptions->szReportFile))
					{
						// no filename specified

						*lpwInvalidOption = i;

						return(CMDSTATUS_INVALID_OPTION);
					}

                    lpCommandOptions->dwFlags |= dwID;
                    break;

                case OPTION_FLAG_REMVIR:
                    if (!ObtainHexWORDValueAfterEquals(argv[i],
                        &(lpCommandOptions->wRemVirVID)))
                    {
                        // No VID specified

                        *lpwInvalidOption = i;

                        return(CMDSTATUS_INVALID_OPTION);
					}

                    lpCommandOptions->dwFlags |= dwID;
                    break;

                case OPTION_FLAG_WATCH:
                    if (!ObtainHexWORDValueAfterEquals(argv[i],
                        &(lpCommandOptions->wDebugVID)))
                    {
                        // No VID specified

                        *lpwInvalidOption = i;

                        return(CMDSTATUS_INVALID_OPTION);
					}

                    lpCommandOptions->dwFlags |= dwID;
                    break;

				default:
					lpCommandOptions->dwFlags |= dwID;
					break;
			}
        }
        else
        {
            // path/drive string

            lpTempScanNode = (LPSCAN_NODE)malloc(sizeof(SCAN_NODE_T));
            if (lpTempScanNode == NULL)
            {
                FreeCommandLineData(lpCommandOptions);

                return(CMDSTATUS_MEM_ERROR);
            }

            // insert our scan item into the linked list

            lpTempScanNode->lpszScanThis = argv[i];
            lpTempScanNode->lpNext = lpCommandOptions->lpScanList;
            lpCommandOptions->lpScanList = lpTempScanNode;
        }
    }

    return(CMDSTATUS_OK);
}


//*************************************************************************
//
// Function:
//  PrintOptions()
//
// Parameters:
//  lpCommandOptions    Pointer to command line data structure
//
// Description:
//  Prints out all command line options found on command line as well
//  as the objects to be scanned.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void PrintOptions
(
    LPCMD_OPTIONS       lpCommandOptions
)
{
    LPSCAN_NODE         lpScanItemPtr;

    printf("Options:\n");

    if (lpCommandOptions->dwFlags & OPTION_FLAG_REPORT)
		printf(" Report file: %s\n", lpCommandOptions->szReportFile);

    if (lpCommandOptions->dwFlags & OPTION_FLAG_DOALL)
        printf(" Scan all files\n");

    if (lpCommandOptions->dwFlags & OPTION_FLAG_HELP)
        printf(" Help requested\n");

    if (lpCommandOptions->dwFlags & OPTION_FLAG_NOBOOT)
        printf(" Skip boot scanning\n");

    if (lpCommandOptions->dwFlags & OPTION_FLAG_NOPART)
        printf(" Skip partition scanning\n");

    if (lpCommandOptions->dwFlags & OPTION_FLAG_NOMEM)
        printf(" Skip memory scanning\n");

    if (lpCommandOptions->dwFlags & OPTION_FLAG_NOFILE)
        printf(" Skip file scanning\n");

    if (lpCommandOptions->dwFlags & OPTION_FLAG_REPAIR)
        printf(" Repair automatically\n");

    if (lpCommandOptions->dwFlags & OPTION_FLAG_DELETE)
        printf(" Delete automatically\n");

    if (lpCommandOptions->dwFlags & OPTION_FLAG_SCANHIGH)
        printf(" Scan high memory\n");

    if (lpCommandOptions->dwFlags & OPTION_FLAG_RECURSE)
        printf(" Recursively scan subdirectories\n");

    if (lpCommandOptions->dwFlags & OPTION_FLAG_NOPOLY)
        printf(" Do not scan for polymorphic viruses\n");

    if (lpCommandOptions->dwFlags & OPTION_FLAG_SCANWILD)
        printf(" Only scan for WILD viruses\n");

    if (lpCommandOptions->dwFlags & OPTION_FLAG_REMVIR)
        printf(" Remove viruses: %04X\n",
            lpCommandOptions->wRemVirVID);

    if (lpCommandOptions->dwFlags & OPTION_FLAG_VIRLIST)
        printf(" Display virus list\n");

    if (lpCommandOptions->dwFlags & OPTION_FLAG_MEMAVAIL)
        printf(" Check available memory\n");

    if (lpCommandOptions->dwFlags & OPTION_FLAG_WATCH)
        printf(" Watch VID: %04X\n",lpCommandOptions->wDebugVID);

    if (lpCommandOptions->dwFlags & OPTION_FLAG_DUMP)
        printf(" Dump\n");

    printf("Scan list:\n");

    lpScanItemPtr = lpCommandOptions->lpScanList;

    while (lpScanItemPtr != NULL)
    {
        printf(" %s\n",lpScanItemPtr->lpszScanThis);
        lpScanItemPtr = lpScanItemPtr->lpNext;
    }
}

