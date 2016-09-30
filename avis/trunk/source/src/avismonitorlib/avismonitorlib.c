#include "AVISMonitorLib.h"
#include <windows.h>
#include <limits.h>
#include <time.h>

static HANDLE AVISMonitorSemHandle = (HANDLE) NULL;

/*-----------------------------------------------------------------------------
*   Function: long AVISMonitorInit 
*   Description: 
*       Initialize the AVISMonitor lib. Creates the semaphore used to  
*       indicate the active status of the process 
*   Parameters: 
*       None 
*   Return: 
*       unsigned -  
*   Designed by: Srikant jalan
-----------------------------------------------------------------------------*/
unsigned long AVISMonitorInit()
{
    char mySemName[MAX_PATH];
    char *semName;
    char filePath[MAX_PATH];
    DWORD lastError;

    // Thia function can be called only once by the process
    if (AVISMonitorSemHandle)
        return ((unsigned long) ERROR_ALREADY_EXISTS);

    // Generate the semaphore name based on the module name
    if (!GetModuleFileName (NULL, filePath, sizeof(filePath)))
        return ((unsigned long) GetLastError());

    semName = strrchr (filePath, '\\');
    if (!semName)
        semName = strrchr (filePath, ':');

    if (!semName)
        semName = filePath;
    else
        semName++;

    memset (mySemName, '\0', sizeof (mySemName));
    strncpy (mySemName, semName, sizeof(mySemName) - 14);
    strcat (mySemName, "_AVIS_MONITOR");

    // Create the semaphore with initial value of 1 and maximum value of LONG_MAX
    AVISMonitorSemHandle = CreateSemaphore (NULL, 1, LONG_MAX, mySemName);
    if (AVISMonitorSemHandle)
    {
        if ((lastError = GetLastError ()) == ERROR_ALREADY_EXISTS)
        {
            CloseHandle (AVISMonitorSemHandle);
            AVISMonitorSemHandle = (HANDLE) NULL;
        }
        else
            lastError = ERROR_SUCCESS;
    }
    else
        lastError = GetLastError();

    return ((unsigned long) lastError);
}

/*-----------------------------------------------------------------------------
*   Function: long AVISMonitorTerm 
*   Description: 
*       Close the semaphore created by the init function 
*   Parameters: 
*       None 
*   Return: 
*       unsigned -  
*   Designed by: Srikant jalan
-----------------------------------------------------------------------------*/
unsigned long AVISMonitorTerm()
{
    DWORD lastError;

    if (AVISMonitorSemHandle)
    {
        if (!CloseHandle(AVISMonitorSemHandle))
            lastError = GetLastError ();
        else
        {
            AVISMonitorSemHandle = (HANDLE) NULL;
            lastError = ERROR_SUCCESS;
        }
    }
    else
        lastError = ERROR_INVALID_HANDLE;

    return ((unsigned long) lastError);
}

/*-----------------------------------------------------------------------------
*   Function: long AVISMonitorSignal 
*   Description: 
*       Increase the value of the semaphore by 1. The increment is done only  
*       once per second even if the function is called multiple times 
*   Parameters: 
*       None 
*   Return: 
*       unsigned -  
*   Designed by: Srikant jalan
-----------------------------------------------------------------------------*/
unsigned long AVISMonitorSignal()
{
    DWORD lastError;
    static time_t signalTime;
    static BOOL firstTime = TRUE;

    if (firstTime == TRUE)
    {
        time (&signalTime);
        signalTime -= 100;
        firstTime = FALSE;
    }

    if (AVISMonitorSemHandle)
    {
        time_t currentTime = time(NULL);

        if (currentTime - signalTime > 1)
        {
            if (!ReleaseSemaphore (AVISMonitorSemHandle, 1, NULL))
                lastError = GetLastError ();
            else
                lastError = ERROR_SUCCESS;
        }
        else
            lastError = ERROR_SUCCESS;
    }
    else
        lastError = ERROR_INVALID_HANDLE;

    return ((unsigned long) lastError);
}

/*-----------------------------------------------------------------------------
*   Function: AVISMonitorCheck 
*   Description: 
*       Check if the semaphore value is changed since last call. If so then  
*       the process is active 
*   Parameters: 
*       char *filePath -  
*       LONG oldValue -  
*       LONG *newValue -  
*   Return: 
*       int -  
*   Designed by: Srikant jalan
-----------------------------------------------------------------------------*/
int AVISMonitorCheck(char *filePath, LONG oldValue, LONG *newValue)
{
    int rc = AVIS_MONITOR_ERROR;

    if (filePath)
    {
        char *semName;
        HANDLE semHandle;
        LONG currentValue = 0L;
        char mySemName[MAX_PATH];

        semName = strrchr (filePath, '\\');
        if (!semName)
            semName = strrchr (filePath, ':');

        if (!semName)
            semName = filePath;
        else
            semName++;

        memset (mySemName, '\0', sizeof (mySemName));
        strncpy (mySemName, semName, sizeof(mySemName) - 14);
        strcat (mySemName, "_AVIS_MONITOR");

        rc = AVIS_MONITOR_INACTIVE;
        semHandle = OpenSemaphore (SEMAPHORE_MODIFY_STATE, FALSE, mySemName);
        if (semHandle)
        {
            BOOL semRC;

            rc = AVIS_MONITOR_UNKNOWN;
            semRC = ReleaseSemaphore (semHandle, 1, &currentValue);
            CloseHandle (semHandle);

            if ( (currentValue - oldValue) >= 2 )
                rc = AVIS_MONITOR_ACTIVE;

        }
        else
            rc = AVIS_MONITOR_ACTIVE;

        if (newValue)
            *newValue = currentValue;
    }

    return (rc);
}
