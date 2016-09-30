#include "platform.h"
#include "os2disk.h"

#define DURATION 1
#define BASE_FREQUENCY 220
#define NUM_PITCHES 8
#define SCALER 3
#define REPEATS 3
#define HIGH_MULT 2

VOID WINAPI MUS4_BEEP(BYTE bType)
{
    int f, i, low, high, inc;
    
    if ( bType < SCALER )
        bType = SCALER;

    low = BASE_FREQUENCY * (bType / SCALER);
    high = BASE_FREQUENCY * (bType / SCALER) * HIGH_MULT;
    inc = (high-low) / NUM_PITCHES;

    for ( i = REPEATS; i > 0; i-- )
        {
        for ( f = low; f <= high; f+=inc )
            DosBeep ( f, DURATION );
        }
}

#if 0
#include <cts4.h>
#define PAUSE while(!kbhit()); getch()

void main(void)
{
    printf ( "\r\nBEEP_UNDEFINED" );
    MUS4_BEEP ( BEEP_UNDEFINED );
    PAUSE;

    printf ( "\r\nBEEP_KNOWNBOX" );
    MUS4_BEEP ( BEEP_KNOWNBOX );
    PAUSE;
    
    printf ( "\r\nBEEP_FBOOTBOX" );
    MUS4_BEEP ( BEEP_FBOOTBOX );
    PAUSE;
    
    printf ( "\r\nBEEP_INOCBOX" );
    MUS4_BEEP ( BEEP_INOCBOX );
    PAUSE;
    
    printf ( "\r\nBEEP_REINOCBOX" );
    MUS4_BEEP ( BEEP_REINOCBOX );
    PAUSE;
    
    printf ( "\r\nBEEP_AUDITERROR" );
    MUS4_BEEP ( BEEP_AUDITERROR );
    PAUSE;
    
    printf ( "\r\nBEEP_INOCERROR" );
    MUS4_BEEP ( BEEP_INOCERROR );
    PAUSE;
    
    printf ( "\r\nBEEP_BOOTPROTECT" );
    MUS4_BEEP ( BEEP_BOOTPROTECT );
    PAUSE;
    
    printf ( "\r\nBEEP_PTABPROTECT" );
    MUS4_BEEP ( BEEP_PTABPROTECT );
    PAUSE;
    
    printf ( "\r\nBEEP_SYSFILEPROTECT" );
    MUS4_BEEP ( BEEP_SYSFILEPROTECT );
    PAUSE;
    
    printf ( "\r\nBEEP_WBOOTSCAN" );
    MUS4_BEEP ( BEEP_WBOOTSCAN );
    PAUSE;
    
    printf ( "\r\nBEEP_DURATION" );
    MUS4_BEEP ( BEEP_DURATION );
}
#endif

