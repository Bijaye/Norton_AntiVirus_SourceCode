#include <stdafx.h>
#include <string>
#include <sys\types.h>
#include <sys\stat.h>
#include <direct.h>

#include "AnalysisStateInfo.h"
#include "AnalysisRequest.h"
#include "avisdbexception.h"

#include <winsock.h>
#include "AVISSendMail.h"

#define CRASH_RECOVER_MAIN
#include "CrashRecover.h"

static BOOL dirtyCookieFound;
static time_t curTime;

void CleanupDir(CString &dirPath)
{
	CString fileName;
	CString temp; // temporary for directory
    int rc;              // return code from findfirst

    temp = dirPath;
    temp += "\\*.*";

    HANDLE hFindFile;
    WIN32_FIND_DATA result;

    hFindFile = FindFirstFile(temp, &result);
    rc = (hFindFile != INVALID_HANDLE_VALUE) ? 1 : 0;

    while (rc)
    {
        if (stricmp(result.cFileName, "sample.original") &&
            stricmp(result.cFileName, "attributes.txt") &&
            stricmp(result.cFileName, "import.complete") &&
            stricmp(result.cFileName, "BackOfficeSampleExport.complete") &&
            stricmp(result.cFileName, ".") &&
            stricmp(result.cFileName, ".."))
        {
            fileName = dirPath;
            fileName += "\\";
            fileName += result.cFileName;
            if (result.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                CleanupDir (fileName);
                _rmdir (fileName);
            }
            else
                _unlink (fileName);
        }
        rc = FindNextFile (hFindFile, &result);
    }

    if (hFindFile != INVALID_HANDLE_VALUE)
        FindClose (hFindFile);
	return;
}

void CleanSampleStorage(CString &uncPath)
{
    CString cookiePath;
	CString startFileName;
	CString finishFileName;
    FILE *fp;
 
    CString temp; // temporary for directory
    int rc;              // return code from findfirst

    temp = uncPath;
    temp += "\\*.*";

    HANDLE hFindFile;
    WIN32_FIND_DATA result;

    hFindFile = FindFirstFile(temp, &result);
    rc = (hFindFile != INVALID_HANDLE_VALUE) ? 1 : 0;

    std::string importedString = "imported";
    std::string rescanString = "rescan";
	AnalysisStateInfo importedStateObject( importedString );
    while (rc)
    {
        if (crashRecoverStatus == CRASH_RECOVER_SIGNALLED_TO_STOP)
        {
            fp = fopen ("AVISDFCrashRecovery.Log", "a");
            if (fp)
            {
                fprintf (fp, "\nAVIS DataFlow Crash Recovery stopped by AVIS DataFlow Manager.\n");
                fclose (fp);
            }
            break;
        }
        if ((result.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && (atoi(result.cFileName)))
        {
            /*
            fp = fopen ("AVISDFCrashRecovery.Log", "a");
            if (fp)
            {
                fprintf (fp, "Checking Directory %s\n", result.cFileName);
                fclose (fp);
            }
            */
	        try
            {
		        AnalysisRequest request(atoi(result.cFileName));
                AnalysisStateInfo oldState = request.State();
                std::string oldStateStdStr = oldState;
                CString oldStateStr = oldStateStdStr.c_str();
                if (oldStateStr != importedString.c_str() && oldStateStr != rescanString.c_str())
                {
                    cookiePath = uncPath;
                    cookiePath += "\\";
                    cookiePath += result.cFileName;

                    startFileName = cookiePath;
                    startFileName += "\\dfstart.dat";

                    struct _stat statBuf;

                    if (!(_stat(startFileName, &statBuf)) && statBuf.st_ctime < curTime)
                    {
                        finishFileName = cookiePath;
                        finishFileName += "\\dffinish.dat";

                        dirtyCookieFound = TRUE;
                        fp = fopen ("AVISDFCrashRecovery.Log", "a");
			            if (_stat(finishFileName, &statBuf)) 
                        {
                            if (fp)
                            {
                                fprintf (fp, "Cleaning cookie directory <%s>. Previous state of the cookie was <%s>.\n", result.cFileName, oldStateStr);
                                fclose (fp);
                            }
				            CleanupDir(cookiePath);
		                    request.State(importedStateObject);
	                    }
                        else
                        {
                            if (fp)
                            {
                                fprintf (fp, "Not cleaning cookie directory <%s> as dffinish.dat exists. Current state of the cookie is <%s>.\n", result.cFileName, oldStateStr);
                                fclose (fp);
                            }
                        }
                    }
                }
            }
	        catch (AVISDBException exception)
            {
            }
        }
        rc = FindNextFile (hFindFile, &result);
        if (!rc)
        {
            DWORD lastError = GetLastError();
            fp = fopen ("AVISDFCrashRecovery.Log", "a");
            if (fp)
            {

                if (lastError == ERROR_NO_MORE_FILES)
                {
//                    fprintf (fp, "FindNextFile returned ERROR_NO_MORE_FILES.\n");
                }
                else
                {
                    fprintf (fp, "FindNextFile returned ERROR_%ld.\n", lastError);
                }
                fclose (fp);
            }
        }
    }
    if (hFindFile != INVALID_HANDLE_VALUE)
        FindClose (hFindFile);
	return;
}

UINT CleanAfterProgramCrash( LPVOID pParam )
{
    crashRecoverStatus = CRASH_RECOVER_STARTED;
    curTime = time(NULL);
    CString uncPath;
    FILE *fp;

    fp = fopen ("AVISDFCrashRecovery.Log", "w");
    if (fp)
    {
        fprintf (fp, "DataFlow crash recovery started at %s", ctime (&curTime));
        fclose (fp);
    }

    uncPath = CString((char *) pParam);

    uncPath.TrimLeft();
    uncPath.TrimRight();

    dirtyCookieFound = FALSE;
    if (!uncPath.IsEmpty())
    {
        CleanSampleStorage(uncPath);
    }

    fp = fopen ("AVISDFCrashRecovery.Log", "a");
    if (fp)
    {
        curTime = time(NULL);
        fprintf (fp, "\nDataFlow crash recovery ended at %s", ctime (&curTime));
        fclose (fp);
    }
    if (dirtyCookieFound == TRUE)
    {
    	TCHAR computerName[ MAX_COMPUTERNAME_LENGTH + 1];
        DWORD compNameSize;

        compNameSize = sizeof (computerName);
        if (!GetComputerName (computerName, &compNameSize))
        {
            computerName[0] = '\0';
        }
        class CAVISSendMail avisSendMail;
        CString msgStr;
        msgStr.Empty();
        msgStr.Format ("AVIS DataFlow recovery log from the file AVISDFCrashRecovery.Log on machine <%s>.\n\n", computerName);

        fp = fopen ("AVISDFCrashRecovery.Log", "r");
        if (fp)
        {
            char buf[2048];

            while (fgets (buf, sizeof(buf), fp))
            {
                msgStr += CString (buf);
            }
            fclose (fp);
        }

        BOOL brc = avisSendMail.AVISSendMail (AVISSENDMAIL_PROFILE_DATAFLOW_TYPE_INDEX, msgStr);
    }
    crashRecoverStatus = CRASH_RECOVER_STOPPED;
    return 0;
}

