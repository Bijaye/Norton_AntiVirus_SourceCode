// AVISMonitorTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#ifndef FILE_BASED_CHECK
#include "AVISMonitorLib.h"
#endif


int main(int argc, char* argv[])
{
    int sleepDelay = 0;    // Seconds
    int loopCount = 0;

    char lockFileName[512];

    GetModuleFileName (NULL, lockFileName, sizeof (lockFileName));

    if (argc > 1)
        sleepDelay = atoi (argv[1]);

    if (argc > 2)
        loopCount = atoi (argv[2]);

    if (sleepDelay <= 0 || loopCount <= 0)
    {
        printf ("Usage : %s <Sleep Time in seconds> <Loop Count>\n",
            lockFileName);
        return 0;
    }


#ifdef FILE_BASED_CHECK
    strcat (lockFileName, ".lck");
    unlink (lockFileName);
#else
    unsigned long monRC;
    monRC = AVISMonitorInit ();
    if (monRC != ERROR_SUCCESS)
    {
        LPVOID lpMsgBuf;
        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                      FORMAT_MESSAGE_FROM_SYSTEM |
                      FORMAT_MESSAGE_IGNORE_INSERTS,
                      NULL,
                      monRC,
                      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                      (LPTSTR) &lpMsgBuf,
                      0,
                      NULL );

        printf ("Error in initializing AVIS Monitor\n%s\n", (LPCTSTR)lpMsgBuf);

// Free the buffer.
        LocalFree( lpMsgBuf );
        return (0);
    }
#endif

    while (loopCount > 0)
    {
#ifdef FILE_BASED_CHECK
        FILE *fp;

        fp = fopen (lockFileName, "r");
        if (fp)
        {
            // File Exists
            fclose (fp);
            printf ("File \"%s\" already exists. Remaining Iterations %d\n", 
                lockFileName, (loopCount - 1));
        }
        else
        {
            printf ("Creating file \"%s\". Remaining Iterations %d\n", 
                lockFileName, (loopCount - 1));
            fp = fopen (lockFileName, "w");
            if (fp)
                fclose (fp);
        }
#else
        monRC = AVISMonitorSignal ();
#endif

        printf ("Sleeping for %d seconds. Remaining Iterations %d\n",
            sleepDelay, (loopCount - 1));
        Sleep (sleepDelay * 1000);
        loopCount--;
    }

#ifndef FILE_BASED_CHECK
    monRC = AVISMonitorTerm ();
#endif
	return 0;
}
