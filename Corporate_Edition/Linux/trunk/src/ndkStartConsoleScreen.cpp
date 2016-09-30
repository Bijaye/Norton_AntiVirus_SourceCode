#include "PSCAN.H"
#include <ndkServerManagement.h>

extern "C" char SystemRunning;

void StartConsoleScreen()
{
BOOL ShutdownSystem = FALSE;
char c[250];

    while (SystemRunning && !ShutdownSystem)
    {
        NTxSleep (5000);
        printf("Enter q to quit rtvscan\n");
        scanf("%s",c);
        switch ( c[0] )
        {
        case 'q':
            ShutdownSystem = TRUE;
            break;
        default:printf("Invalid input.\n");break;
        }
    }
}
