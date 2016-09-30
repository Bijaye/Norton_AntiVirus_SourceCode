/*

filename:delproc.c

*/


//
// System include files
//

#include <stdio.h>
#include <windows.h>
#include <winuser.h>
#include <winbase.h>

// system include files
#include <tlhelp32.h>   // windows 95 only

//
// User defined include files
//
#include "softproc.h"


//
// Global Sectiom
//
DWORD dwDeltaProcessIDs[MAX_PROCESSES];

int iDeltaProcesses;
//
// External references Section
//

// Current Process IDs
extern DWORD dwCurrentProcessIDs[MAX_PROCESSES];

extern int iCurrentProcessNumber;

// Logged Process IDs
extern DWORD dwLoggedProcessIDs[MAX_PROCESSES];

extern int iLoggedProcessNumber;

extern PROCESSENTRY32 Process_Entries[ MAX_PROCESSES ];

//
//
//
int DeltaProc( FILE *logfilePID )
{

//
// local data
//

 HANDLE hAppProcess = NULL;

 int iResult = FALSE;

 int iLoggedProcIndex  = 0;
 int iCurrentProcIndex = 0;
 int iDeltaProcIndex   = 0;

 int iSizetoInit = 0;

 BOOL bFoundPIDMatch = FALSE;
 BOOL bTermProcStatus = FALSE;

 DWORD dwTempPID;

 DWORD dwCurrentProcessID = 0;

//
// Start of code
//

  // initialize the delta processes data structure

  iSizetoInit = sizeof( dwDeltaProcessIDs[MAX_PROCESSES] );

  // clean out memory
  memset( dwDeltaProcessIDs, 0, iSizetoInit );

  // init. counters
  iDeltaProcesses      = 0;

  iLoggedProcessNumber = 0;

  // set file back to beginning, should be there since file was opened,
  // but JUST to make sure
  rewind( logfilePID );

  // read the PID log file
  while ( feof(logfilePID) == 0 )
  { // while not end of file

   // read the logged PID from the file
   fread( &dwTempPID, sizeof(DWORD), 1, logfilePID );

   dwLoggedProcessIDs[iLoggedProcessNumber] = dwTempPID;

   iLoggedProcessNumber ++;

#ifdef _DEBUG
   fprintf( stdout, "PID Hex: %X\n", dwTempPID );
#endif

  } // end while not end of file

#ifdef _DEBUG
  fprintf( stdout, "Number of Current processes: %i\n", iCurrentProcessNumber );
  fprintf( stdout, "Number of Logged processes:  %i\n", iLoggedProcessNumber );
#endif

  //test code
  iLoggedProcessNumber --;
  dwLoggedProcessIDs[iLoggedProcessNumber] = 0;

  //test code

#ifdef _DEBUG
  fprintf( stdout, "Number of Adjusted Logged processes:  %i\n", iLoggedProcessNumber );
#endif

  // Get the current process ID, so we know who we are
  dwCurrentProcessID = GetCurrentProcessId( );

  //
  // Now, we need to start looking for any new processes running, except for us,
  // by checking the logged Process IDs to the current list
  //
  for ( iCurrentProcIndex = 0; iCurrentProcIndex < iCurrentProcessNumber; iCurrentProcIndex ++ )
  { // for

     // reset match boolean
     bFoundPIDMatch = FALSE;

     // start looping thru all the logged process IDs
     for ( iLoggedProcIndex = 0; iLoggedProcIndex < iLoggedProcessNumber; iLoggedProcIndex ++ )
     { // for

       // check for any new Processes launched
       if ( dwCurrentProcessIDs[iCurrentProcIndex] == dwLoggedProcessIDs[iLoggedProcIndex] )
       { // if

         // Nope, this is the same process as from the logged run
         bFoundPIDMatch = TRUE;

         // out of loop
         break;

       } // end if

     } // end for Logged process loop

     //
     // check id match found, if not new process
     // first check if the process found is not us, we know we are
     // new
     //
     if ( bFoundPIDMatch == FALSE )
     {

      if ( ( dwCurrentProcessIDs[iCurrentProcIndex] != dwCurrentProcessID ) )
//                && ( Process_Entries[iCurrentProcIndex].th32ParentProcessID != dwCurrentProcessID ) )
      { // if new process

          // save off the Process ID for "killing"
         dwDeltaProcessIDs[iDeltaProcesses] = dwCurrentProcessIDs[iCurrentProcIndex];

         iDeltaProcesses ++;

     } // end if new process

    } // end if

  } // end for Current Process Loop

  // check to see if we have any processes to "Kill"
  if ( iDeltaProcesses > 0 )
  {

     for ( iDeltaProcIndex = 0; iDeltaProcIndex < iDeltaProcesses; iDeltaProcIndex ++ )
     { // for

#ifdef _DEBUG
           fprintf( stdout, "Delta Process PID: %X \n", dwDeltaProcessIDs[iDeltaProcIndex]  );
#endif

           // get the handle to the process
           hAppProcess = OpenProcess( PROCESS_TERMINATE,
                                      FALSE,
                                      dwDeltaProcessIDs[iDeltaProcIndex] );

           if ( hAppProcess != NULL )
             bTermProcStatus = TerminateProcess( hAppProcess, 0 );

     } // end for


  }

  return iResult;

}
