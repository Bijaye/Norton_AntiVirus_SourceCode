// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#ifdef FINERESOLUTION
#else

// this is the default define - the user can provide their own if they like
      
#define FINERESOLUTION MILLISECONDS

#endif

// this code is shared with a number of places - it is included
// as an included .c file rather than building a library

// implementation of GetFineLinearTime, which returns fine grain time for the platforms
// Win32 and NLM - fine grain means millisecond resolution or better, platform permitting

// this calculates delta elapsed time - this allows for wraparound, and works regardless
// of the scale, as long as the begin and time are on the same scale!

DWORD ElapsedFineLinearTime( DWORD dwStartTime, DWORD dwEndTime )
{
    DWORD dwElapsed;

    if ( dwEndTime >= dwStartTime )
    {
        dwElapsed = dwEndTime - dwStartTime;
    }
    else
    {
        dwElapsed = 0xffffffff - dwStartTime;   // time from start up to the end
        dwElapsed += dwEndTime + 1;             // time from zero up to now
    }

    return dwElapsed;
}



#ifdef WIN32

static __int64 tTimeMultiplier[] = { 1000, 1000, 10000, 100000, 1000000 };   // precision multipliers

static int nFineLinearTimeMode = FINERESOLUTION;

void SetFineLinearTimeMode( int nMode )
{
    nFineLinearTimeMode = nMode;
}

// returns a linear time with a base time of when the routine was first called
// with resolution of nFineLinearTimeMode - the possiblities are either milliseconds
// or 1/10 milliseconds

DWORD GetFineLinearTime( VOID )
{

    static  __int64 tFirstLight = 0;
    static  __int64 tTicksPerSec;

    static  BOOL    bHiResTimerOK = TRUE;

    __int64 tCurrTime = 0;

    __int64 tSeconds;
    __int64 tFracSeconds;

    __int64 tResolutionMultiplier = tTimeMultiplier[nFineLinearTimeMode];

    if ( tFirstLight == 0 )       // first run through?
    {
        if ( QueryPerformanceFrequency( (LARGE_INTEGER *) &tTicksPerSec ) )
        {
            QueryPerformanceCounter( (LARGE_INTEGER *) &tFirstLight );
        }
        else
        {
            bHiResTimerOK = FALSE;
            tFirstLight = (__int64) GetTickCount( );
        }
    }

    if ( bHiResTimerOK )
    {
        __int64 tCurrCounter;

        QueryPerformanceCounter( (LARGE_INTEGER *) &tCurrCounter );
        tCurrCounter -= tFirstLight;

        tSeconds = tCurrCounter / tTicksPerSec;
        tFracSeconds = (tCurrCounter % tTicksPerSec) * tResolutionMultiplier;

        tCurrTime = tSeconds * tResolutionMultiplier + tFracSeconds / tTicksPerSec;
    }
    else
    {
        tCurrTime =  (__int64) GetTickCount( ) - tFirstLight;
    }

    return((DWORD) tCurrTime);
}

// end Win32 code

#else

// NLM code, none of the above follows

#ifdef NLM

typedef DWORD (* FGETHIGHRESOLUTIONTIMER) (void);
typedef DWORD (* FGETCURRENTTICKS) (void);

#define HIRES_TICKS_PER_SECOND_3X4X 9216

DWORD TicksToMilliseconds( DWORD dwTicks )
{
    return((dwTicks * 5492) / 100); 
}


static nFineLinearTimeMode = FINERESOLUTION;
static DWORD dwTimeMultiplier[] = { 1000, 1000, 10000, 100000, 1000000 };   // precision multipliers

void SetFineLinearTimeMode( int nMode )
{
    nFineLinearTimeMode = nMode;
}

// returns a linear time with a base time of when the routine was first called
// with resolution of nFineLinearTimeMode - the possiblities are either milliseconds
// or 1/10 milliseconds

DWORD GetFineLinearTime( VOID )
{

    static  DWORD   dwFirstLight = 0;
    static  DWORD   dwTicksPerSec = 0;

    static  FGETHIGHRESOLUTIONTIMER fGetHighResolutionTimer = NULL;
    static  FGETCURRENTTICKS        fGetCurrentTicks = NULL;

    DWORD   dwOsStartTime = 0;

    DWORD   dwSeconds;
    DWORD   dwFracSeconds;

    DWORD   dwResolutionMultiplier = dwTimeMultiplier[nFineLinearTimeMode];

    if ( nFineLinearTimeMode != TICKS )
    {
        if ( dwFirstLight == 0 )  // first run through?
        {
            FILE_SERV_INFO  ServerInfo;

            GetServerInformation( sizeof( ServerInfo ), &ServerInfo );

            fGetHighResolutionTimer = ImportSymbol(GetNLMHandle(), "GetHighResolutionTimer");
            fGetCurrentTicks = ImportSymbol(GetNLMHandle(), "GetCurrentTicks");

            if ( fGetHighResolutionTimer )
                dwFirstLight = fGetHighResolutionTimer( );
            else if ( fGetCurrentTicks )
                dwFirstLight = fGetCurrentTicks( );
            else
                dwFirstLight = time( NULL );

            // they changed the undocumented API in 4.x - it used to be driven by the high
            // resolution timer, now they use the Pentium RDTSC instruction - when they
            // changed they changed the output resolution to 1/10000 s

            if ( ServerInfo.netwareVersion >= 4 )
            {
                dwTicksPerSec = 10000;
            }
            else
            {
                dwTicksPerSec = HIRES_TICKS_PER_SECOND_3X4X;
            }
        }

        if ( fGetHighResolutionTimer )
        {
            dwOsStartTime = fGetHighResolutionTimer( ) - dwFirstLight;

            dwSeconds = dwOsStartTime / dwTicksPerSec;
            dwFracSeconds = (dwOsStartTime % dwTicksPerSec) * dwResolutionMultiplier;

            dwOsStartTime = dwSeconds * dwResolutionMultiplier + dwFracSeconds / dwTicksPerSec;
        }
        else if ( fGetCurrentTicks )
        {
            dwOsStartTime =  TicksToMilliseconds( fGetCurrentTicks( ) - dwFirstLight );
        }
        else
        {
            dwOsStartTime = (time( NULL ) - dwFirstLight) * dwResolutionMultiplier;
        }       
    }
    else
    { // assume TICKS
        if ( dwFirstLight == 0 )
        {
            fGetCurrentTicks = ImportSymbol(GetNLMHandle(), "GetCurrentTicks");

            if ( fGetCurrentTicks )
                dwFirstLight = fGetCurrentTicks( );
            else
                dwFirstLight = 1;
        }

        if ( fGetCurrentTicks )
        {
            dwOsStartTime =  TicksToMilliseconds( fGetCurrentTicks( ) - dwFirstLight );
        }
        else
        {
            dwOsStartTime = (time( NULL ) - dwFirstLight) * dwResolutionMultiplier;
        }       
    }

    return(dwOsStartTime);
}

#else

// not Win32, not NLM platformas

static nFineLinearTimeMode = FINERESOLUTION;
static DWORD dwTimeMultiplier[] = { 1000, 1000, 10000, 100000, 1000000 };   // precision multipliers

void SetFineLinearTimeMode( int nMode )
{
    nFineLinearTimeMode = nMode;
}

// returns a linear time with a base time of when the routine was first called
// with resolution of nFineLinearTimeMode - the possiblities are either milliseconds
// or 1/10 milliseconds

DWORD GetFineLinearTime( VOID )
{

    static  DWORD   dwFirstLight = 0;

    DWORD   dwOsStartTime   =   0;

    DWORD   dwResolutionMultiplier = dwTimeMultiplier[nFineLinearTimeMode];

    // for generic platforms I settle for 1-second resolution

    if ( dwFirstLight == 0 )      // first run through?
    {
        dwFirstLight = time( NULL );
    }

    dwOsStartTime =  time( NULL ) - dwFirstLight;

    dwOsStartTime *= dwResolutionMultiplier;

    return(dwOsStartTime);
}

#endif  // #ifdef NLM

#endif // #ifdef WIN32


