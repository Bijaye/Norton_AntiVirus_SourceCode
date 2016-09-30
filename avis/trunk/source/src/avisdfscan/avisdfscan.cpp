// AVISDFScan.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <fstream>
#include <direct.h>
#include <time.h>

#include <NAVScan.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Define the maximum attempts we can make to delete the
// temporary copy of the sample file. For every attempt,
// it waits for 5 seconds. That makes it 12 attempts per minute.
// We will wait for a maximum of 30 minutes.
#define TEMP_FILE_DELETE_ATTEMPT_CNT 12*30


/*-----------------------------------------------------------------------------
*   Function: CheckScanExplicitPipe 
*   Description: 
*       Cehck for the existance of communication pipe to verify whether  
*       ScanExplicit is running. If not then start the ScanExplicit process 
*   Parameters: 
*       None 
*   Return: 
*       BOOL -  
*   Designed by: Srikant jalan
-----------------------------------------------------------------------------*/
BOOL CheckScanExplicitPipe()
{
    HANDLE pipeHandle;
    HANDLE mutexHandle;
    BOOL pipeFound = FALSE;

    // Make sure that we start only one instance of ScanExplicit process
    mutexHandle = CreateMutex (NULL, FALSE, "AVISDFScan_CheckScanExplicitPipe");
    if (mutexHandle)
        WaitForSingleObject (mutexHandle, INFINITE);

    printf ("Checking for communication pipe.\n");
    pipeHandle = CreateFile ("\\\\.\\pipe\\ScanExplicit",
        GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);

    if (pipeHandle == INVALID_HANDLE_VALUE)
    {
        // Start ScanExplicit Process
        STARTUPINFO si;
        PROCESS_INFORMATION pi;

        memset (&si, 0, sizeof (STARTUPINFO));
        si.cb = sizeof (STARTUPINFO);
        si.dwFlags = STARTF_USESHOWWINDOW;

        si.wShowWindow = SW_SHOWMINNOACTIVE;

        si.lpTitle = "ScanExplicit";
        memset (&pi, 0, sizeof (PROCESS_INFORMATION));
        printf ("Starting ScanExplicit.exe\n");
        if (CreateProcess (NULL,
            "ScanExplicit.exe -run",
            NULL,
            NULL,
            FALSE,
            CREATE_NEW_CONSOLE | CREATE_NEW_PROCESS_GROUP,
            NULL,
            NULL,
            &si,
            &pi) )
        {
            int tryCount = 0;
            CloseHandle (pi.hThread);
            CloseHandle (pi.hProcess);
            while (TRUE)
            {
                if (tryCount >= 20)
                {
                    printf ("exiting as error in creating communication pipe.\n");
                    pipeFound = FALSE;
                    break;
                }
                printf ("Waiting for ScanExplicit to create communication pipe.\n");
                Sleep (1000);
                pipeHandle = CreateFile ("\\\\.\\pipe\\ScanExplicit",
                    GENERIC_WRITE,
                    0,
                    NULL,
                    OPEN_EXISTING,
                    0,
                    NULL);
                if (pipeHandle != INVALID_HANDLE_VALUE)
                {
                    printf ("Communication pipe created.\n");
                    CloseHandle (pipeHandle);
                    pipeFound = TRUE;
                    break;
                }
                tryCount++;
            }
        }
    }
    else
    {
        printf ("Found communication pipe.\n");
        CloseHandle (pipeHandle);
        pipeFound = TRUE;
    }

    if (mutexHandle)
    {
        ReleaseMutex (mutexHandle);
        CloseHandle (mutexHandle);
    }

    return (pipeFound);
}

/*-----------------------------------------------------------------------------
*   Function: main 
*   Description: 
*       Main function entry point 
*   Parameters: 
*       int argc -  
*       char* argv[] -  
*   Return: 
*       int -  
*   Designed by: Srikant jalan
-----------------------------------------------------------------------------*/
int main(int argc, char* argv[])
{
    std::string     cookieID;
    std::string     uncPath;
    std::string     parameters;
    std::string     fileName;
    std::string     filePath;
    std::string     processID;


    cookieID = "";
    uncPath = "";
    parameters = "";
    fileName = "";
    processID = "";

    for (int i = 0; i < argc; i++)
    {
        if (!strnicmp (argv[i], "--Cookie", strlen ("--Cookie")))
        {
            i++;
            if (i < argc)
                cookieID = argv[i];
        }
        if (!strnicmp (argv[i], "--UNC", strlen ("--UNC")))
        {
            i++;
            if (i < argc)
                uncPath = argv[i];
        }
        if (!strnicmp (argv[i], "--ProcessId", strlen ("--ProcessId")))
        {
            i++;
            if (i < argc)
                processID = argv[i];
        }
        if (!strnicmp (argv[i], "--Parameter", strlen ("--Parameter")))
        {
            i++;
            if (i < argc)
            {
                parameters = argv[i];

                while (1)
                {
                    i++;
                    if (i >= argc)
                        break;

                    if (!strnicmp (argv[i], "--", strlen ("--")))
                    {
                        i--;
                        break;
                    }

                    parameters += " ";
                    parameters += argv[i];
                }
            }
        }
    }

    if (!cookieID.compare("") || !uncPath.compare("") ||
        !parameters.compare("") || !processID.compare(""))
    {
//        printf ("Invalid Syntax\n\nSyntax = %s --Cookie <CookieID> --UNC <UNCPath> --Parameter <<blessedSig>,<scanSig>> --ProcessId <ProcessId>", argv[0]);
        printf ("Invalid Syntax\n\nSyntax = %s --Cookie <CookieID> --UNC <UNCPath> --Parameter <scanSigNum> --ProcessId <ProcessId>", argv[0]);
        return 0;
    }

    printf ("Issue Number : %s\n", cookieID.c_str());
    printf ("UNC Path : %s\n", uncPath.c_str());
    printf ("Parameters : %s\n", parameters.c_str());
    printf ("Process ID : %s\n\n", processID.c_str());
    filePath = uncPath + "\\" + "Sample" + "\\" ;
    std::string     searchPath;

    /*
    searchPath = filePath + "*.*";
    WIN32_FIND_DATA     info;
    HANDLE findHandle = FindFirstFile (searchPath.c_str(), &info);

    if (findHandle != INVALID_HANDLE_VALUE)
    {
        while (TRUE)
        {
    	    if (FILE_ATTRIBUTE_DIRECTORY & info.dwFileAttributes)
            {
                BOOL rc = FindNextFile (findHandle, &info);

                if (!rc)
                    break;
                else
                    continue;
            }
            else
            {
                fileName = filePath + info.cFileName;
                break;
            }
        }
        FindClose (findHandle);
    }
    */
    fileName = filePath + "sample.original";

    printf ("Checking file %s\n", fileName.c_str());

    uint            scanSigNum;
    uint            blessedSigNum;

    scanSigNum = 0;
    blessedSigNum = 0;

    /*
    // Parameters Format : <scanSigNum>,<blessedSigNum>
    std::string     tmp;
    int beginning = 0;
    int end = parameters.find (",", 0);
	if (std::string::npos != end)
	{
        tmp = parameters.substr (beginning, end);
        blessedSigNum = atoi (tmp.c_str());

		beginning = parameters.find_first_not_of(" ", end+1);
        end = parameters.find (",", beginning);
        tmp = parameters.substr (beginning, end);
        scanSigNum = atoi (tmp.c_str());
    }
    */
    scanSigNum = atoi (parameters.c_str());

	uint		    sigSeqNum;
	NAVScan::ScanRC	scanRC;
	uint		    virusID;
	std::string	    virusName;
	std::string	    navVersion;

    sigSeqNum = 0;
    scanRC = NAVScan::UnknownError;
    virusID = 0;
    virusName = "";
    navVersion = "";

    if (fileName.compare (""))
    {
        // Make a copy of the sample file to local directory

        BOOL rc = FALSE;
        char tmpFileName [520] = "";

        HANDLE mutexHandle;

        mutexHandle = CreateMutex (NULL, FALSE, "AVISDFScan_CopySampleToTempFile");
        if (mutexHandle)
            WaitForSingleObject (mutexHandle, INFINITE);

		// We decided to make a copy of the original file and scan that file instead of
		// the original sample file. It is done to avoid file locking problems due to
		// the extended scan times for certain samples. (Senthil - Aug/08/2000)
		// tmpnam function returns an unique filename with a 
		// backslash character prepended to the filename.
        char *tmpName = tmpnam (NULL);

        if (tmpName)
        {
			strcpy (tmpFileName, "");
			strcat (tmpFileName, "C:");
			strcat (tmpFileName, tmpName);
            if (CopyFile (fileName.c_str(), tmpFileName, TRUE))
                rc = TRUE;
        }

        if (mutexHandle)
        {
            ReleaseMutex (mutexHandle);
            CloseHandle (mutexHandle);
        }

        if (rc == TRUE)
        {

            BOOL brc = TRUE/*CheckScanExplicitPipe()*/;

            if (brc == TRUE)
            {
/*
// We decided not to scan the file using the blessed definition during rescans.
// Hence this block of code has been commented. (Senthil - Aug/07/2000)
                if (blessedSigNum)
                {
                    sigSeqNum = blessedSigNum;
                    printf ("Scanning with blessed definition number %d.\n", blessedSigNum);
	                scanRC = NAVScan::ScanExplicit(fileName.c_str(), 101, false, virusName, sigSeqNum,
									                 navVersion, virusID);

                    if (sigSeqNum != blessedSigNum)
                        scanRC = NAVScan::UnknownError;
                }
*/

                if (scanRC != NAVScan::Repaired && scanSigNum)
                {
                    sigSeqNum = scanSigNum;
//                  printf ("Scanning with unblessed definition number %d.\n", scanSigNum);
                    printf ("Scanning with definition number %d.\n", scanSigNum);
					printf ("Original file = %s\n", fileName.c_str());
					printf ("Copy of the original file = %s\n", tmpFileName);
	                scanRC = NAVScan::ScanExplicit(tmpFileName, 101, false, virusName, sigSeqNum,
									                 navVersion, virusID);

                    if (sigSeqNum != scanSigNum)
                        scanRC = NAVScan::UnknownError;
                }
            }
            else
            {
                scanRC = NAVScan::CommWithScannerOffline;
            }

        }

        if (tmpFileName)
		{			
			// Added the following block of code for deleting the copy of the
			// sample.original file we made at the beginning of this program.
			// Sometimes, the NAVScan::ScanExplicit function returns when the pipe established
			// to the ScanExplicit.exe process times out in 90 seconds where the 
			// ScanExplicit.exe process is still scanning the temporary file. In this case,
			// we will not be able to delete the copy of the sample file.
			// We have seen during our tests that for some WIN32 viruses, it takes 16 minutes
			// to scan the file on a Pentium Pro 200 MHZ machine. Hence it is necessary here to
			// be in a loop and attempt to delete the temporary sample file successfully for every
			// 5 seconds. We will try that for a maximum of 30 minutes until we succeed in deletion.
			// Senthil - Aug/08/2000.
            int loopCnt = 0;
			

			// Stay in a loop and do the maximum attempts we can make to delete the
			// temporary copy of the sample file. For every attempt,
			// it waits for 5 seconds. That makes it 12 attempts per minute.
			// We will wait for a maximum of 30 minutes.			
			for (loopCnt = 0; loopCnt < TEMP_FILE_DELETE_ATTEMPT_CNT; loopCnt++)			
			{
				printf ("Attempt %d of %d to delete the copy of the sample file (%s).\r", 
					loopCnt+1, TEMP_FILE_DELETE_ATTEMPT_CNT, tmpFileName);
			
				if (unlink (tmpFileName) == 0)
				{
					break;
				}

				// Wait for 5 seconds before attempting to delete it again.
				Sleep (5000);
			}

			if (loopCnt >= TEMP_FILE_DELETE_ATTEMPT_CNT)
			{
				printf ("\nUnable to delete the file (%s) after %d attempts.\n", tmpFileName, loopCnt);
			}
			else
			{
				printf ("\nSuccessfully deleted the file (%s).\n", tmpFileName);
			}
		}
    }

    // The following is just a test code to randomly force scan results to successfully repaired virus.
    /*
    if (scanRC != NAVScan::Repaired)
    {
        srand( (unsigned)time( NULL ) );
        int randNum = rand();

        if (randNum % 2)
        {
            virusName = "Dummy Virus Generated By AVISDFSCAN.EXE";
            virusID = randNum;
            scanRC = NAVScan::Repaired;
        }
    }
    */

    const char * charRC = NAVScan::RCtoChar (scanRC);
    printf ("\n%s:%s\n", fileName.c_str(), charRC);
    printf ("Scan definition number : %d\n", sigSeqNum);
    printf ("Virus Name : %s\n", virusName.c_str());
    printf ("Virus ID : %u\n", virusID);
    printf ("NAV Version : %s\n", navVersion.c_str());
    printf ("\n");

    std::string     resultFileName;

    resultFileName = uncPath + "\\" + "DFResults" ;
    _mkdir (resultFileName.c_str());

    resultFileName = uncPath + "\\" + "DFResults" + "\\" + "Result" + processID + ".dat";

    std::ofstream resultFile(resultFileName.c_str());
    if (resultFile.is_open())
    {
        resultFile << "CookieId: " << cookieID.c_str() << "\n";
        resultFile << "Parameters: " << sigSeqNum ;
        if (scanRC == NAVScan::Repaired)
            resultFile << " " << virusID << " \"" << virusName.c_str() << "\"" /*<< " \"" << navVersion.c_str() << "\""*/ << "\n";
        else
            resultFile << "\n";
        resultFile << "NextService: ";
        switch (scanRC)
        {
        case NAVScan::NotInfected:
            // Continue with Analysis
            printf ("Next Service: \n");
            break;

        //case NAVScan::NotInfectable:
        case NAVScan::Repaired:
            // End Analysis and return to user
            printf ("Next Service: ArchiveSample\n");
            resultFile << "ArchiveSample";
            break;

        default:
            // Some error but still continue with next state
            printf ("Next Service: \n");
            break;
        }
        resultFile << "\n";
        resultFile << "1122334455\n";

        resultFile.close();
    }

    std::string     stopFileName;
    stopFileName = uncPath + "\\" + "DFResults" + "\\" + "Stop" + processID + ".dat";

    std::ofstream stopFile(stopFileName.c_str());
    if (stopFile.is_open())
    {
        stopFile << "CookieId: " << cookieID.c_str() << "\n";
        stopFile << "StopCondition: 1 of 1\n" ;
        stopFile << "1122334455\n";

        stopFile.close();
    }

	return 0;
}
