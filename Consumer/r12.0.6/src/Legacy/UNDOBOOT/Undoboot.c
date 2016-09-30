//***************************************************************************
// undoinoc.c : Defines the entry point for the console application.
//***************************************************************************
#include "platform.h"
#include "stdio.h"
#include "syminteg.h"
#include "ctsn.h"

#include "strings.h"

//***************************************************************************
INTEG  glpIntegArray[] = 
{
	{0,"MBR80"},
#if 0 //[
// Partitions beyond 8 gig (required Extended INT 13 handling) currently are
// not being saved correctly into the data file, so we are not going to test
// or repair anything but the MBR on all systems. When the data is saved
// correctly, these items should be re-enabled.
	{0,"BOOT800"}, 
	{0,"BOOT801"}, 
	{0,"BOOT802"}, 
	{0,"BOOT803"}, 
#endif //]
	{0,NULL} 
};
typedef enum tagRECORDtypes
{
    MBR80,
    BOOT800,
    BOOT801,
    BOOT802,
    BOOT803,
    RECORD_COUNT 

} RECTYPES;

//***************************************************************************
int UndoInoculation( char* szFileName );
void DoUndoInocRepair( char* szFileName, LPINTEG lpIntegArray );
void DisplayHelp( int argc, char** argv );
int DoSelfTest( char * szFileName );

//***************************************************************************
//***************************************************************************
int DoSelfTest( char * szFileName )
{
	int nResult = 0;
	LPVOID lpvBuffer = NULL;

    lpvBuffer = MemAllocPtr( GHND, SELF_TEST_BUFFER_SIZE );

	if( lpvBuffer )
	{
		nResult = FileSelfTest( szFileName, lpvBuffer );
		MemFreePtr( lpvBuffer );
	}

	return nResult;
}

//***************************************************************************
//***************************************************************************
int main(int argc, char* argv[])
{
    int nResult = -1;
	LPVOID lpvBuffer = NULL;
    CALLBACKREV1 rCallbacks = {0};

    InitCertlib(&rCallbacks, INIT_CERTLIB_USE_DEFAULT); // init lpgCallBacks

	//
	// Do self-check test...
	//
	nResult = DoSelfTest( argv[0] );

	if( SELF_TEST_SUCCESS != nResult )
	{
		printf( szUndobootAltered );
		return nResult;
	}

    if( argc < 2 )
    {
        DisplayHelp( argc, argv );
    }
    else
    {
        nResult = UndoInoculation( argv[1] );
    }

	return nResult;
}

//***************************************************************************
//***************************************************************************
void DisplayHelp( int argc, char** argv )
{
    printf(szHelp);
}

//***************************************************************************
//***************************************************************************
int UndoInoculation( char* szFileName )
{
    int nResult = 0;

    nResult = IntegVerify( szFileName, glpIntegArray );

    switch( nResult )
    {
    case 0: // One or more records failed to match.
        DoUndoInocRepair( szFileName, glpIntegArray );

        break;

    case 1: // All records matched.

		printf( szDataDoesNotDiffer, szFileName );
        break;

    case 2: // Unable to open record file.

		printf( szErrorOpeningDataFile );
        break;

    case 3: // Unaable to alloc memory.
    case 4: // All existing records matched but some entries missing.
    default :

        printf( szErrorInitRestoreUtility );
        break;
    }
 
    return nResult;
}

//***************************************************************************
//***************************************************************************
void DoUndoInocRepair( char* szFileName, LPINTEG lpIntegArray )
{
	RECTYPES nIdx = 0;

    for( nIdx = 0; nIdx < RECORD_COUNT; nIdx++ )
    {
        if( INTEG_FAIL_VERIFY == lpIntegArray[nIdx].wStatus )
        {
            printf(szRestoring, ((nIdx==MBR80)?szMBR:szBOOT_RECORD));

            IntegRepair( szFileName, &lpIntegArray[nIdx] );

            if( INTEG_SUCCESS_REPAIR == lpIntegArray[nIdx].wStatus )
            {
                if( MBR80 == nIdx )
                    printf(szMBRSuccessRestore);
                else
                    printf(szBootRecordSuccessRestore, nIdx);
            }
            else if( INTEG_FAIL_REPAIR == lpIntegArray[nIdx].wStatus )
            {
                if( MBR80 == nIdx )
                    printf( szMBRFailedRestore );
                else
                    printf(szBootRecordFailedRestore, nIdx);
            }
        }
    }

   //return//
}

