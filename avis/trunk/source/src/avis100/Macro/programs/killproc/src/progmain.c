/*

filename:progmain.c

*/

//
// System include files
//
#include <stdio.h>
#include <windows.h>
#include <string.h>
#include <malloc.h>

#include <tlhelp32.h>   // windows 95 only

//
// User defined include files
//
#include "enumproc.h"
#include "killproc.h"
#include "softproc.h"

//
// Global Sectiom
//

//action flags; LIST is the only supported action right now
LPARAM Actions;

//list of processes names (eg. winword.exe, excel.exe) that have to be killed
char** NameList;

//number of items in NameList
int NameNum;

// file handle to log file
FILE *logfile    = NULL;
FILE *logfilePID = NULL;

// process entry structure
PROCESSENTRY32 Process_Entries[ MAX_PROCESSES ];

// index into process entry structure
int Process_Entry_Index = 0;

// Current Process IDs
DWORD dwCurrentProcessIDs[MAX_PROCESSES];

int iCurrentProcessIndex  = 0;
int iCurrentProcessNumber = 0;

// Logged Process IDs
DWORD dwLoggedProcessIDs[MAX_PROCESSES];

int iLoggedProcessIndex  = 0;
int iLoggedProcessNumber = 0;


//
// External references Section
//

//
// Prototypes Section
//
int DeltaProc( FILE *logfilePID );

void Init_Data( );

BOOL Process_Params(int argc, char * argv[]);

BOOL CALLBACK Proc(DWORD ProcID, DWORD ProcParentID , LPCSTR lpstr, LPARAM lParam);

//
// Local Modules Section
//

void Usage( )
{

        fprintf(stdout, "Usage: KillProc [-log] [-delta] [-list]\n");
        fprintf(stdout, "\n");
        fprintf(stdout, "-log:   Logs all current processes\n");
        fprintf(stdout, "\n");
        fprintf(stdout, "-delta: Performs process delta, and kills \n");
        fprintf(stdout, "        new processes created since first run. \n");
        fprintf(stdout, "\n");
        fprintf(stdout, "-list:  lists all processes\n");

}

void Init_Data( )
{

 int iSizetoInit = 0;

   iSizetoInit = sizeof( dwCurrentProcessIDs[MAX_PROCESSES] );

   memset( dwCurrentProcessIDs, 0, iSizetoInit );

   memset( dwLoggedProcessIDs, 0, iSizetoInit );

}

BOOL Process_Params(int argc, char * argv[])
{

        int i;
        BOOL bResult = TRUE;

        NameNum  = 0;
        Actions  = 0;
        NameList = NULL;

                // check for the correct number of command line parameters
        if (argc == 1)
        { // not enough parameters

          // tell user how to use program
          Usage();

          // return error code
          bResult = FALSE;

        } // end if

                // start check for parameters
        for ( i = 1; i < argc; ++i )
        { // process all the command line parmeters

          if ( !stricmp(argv[i], "-delta") )
          { // determine delta of processes

                  // indicate action
                  Actions = DELTA;

                  // open the file, proclog.log, read-only mode.
                  // if the file does not exist, open fails
                  if ( (logfile = fopen( "proclog.log", "r" )) == NULL )
                  { // if

                     bResult = FALSE;
                     return bResult;

                  } // end if

                  if ( (logfilePID = fopen( "proclog.pid", "rb" )) == NULL )
                  { // if

                     bResult = FALSE;
                     return bResult;

                  } // end if

          } // end determine delta
          else if ( !stricmp(argv[i], "-list") )
          { // list all the currently running processes

                  // indicate action
                  Actions = LIST;

          } // end list
          else if ( !stricmp(argv[i], "-log") )
          { // log the current processes

                  // indicate action
                  Actions = LOG;

                  // open the file, proclog.log, and trash anything in there already
                  // i.e. start fresh
                  if ( (logfile = fopen( "proclog.log", "w" )) == NULL )
                  { // if
					 perror("Unable to open proclog.log for output");
                     bResult = FALSE;
                     return bResult;

                  } // end if

                  if ( (logfilePID = fopen( "proclog.pid", "wb" )) == NULL )
                  { // if
					 perror("Unable to open proclog.pid for output");
                     bResult = FALSE;
                     return bResult;

                  } // end if

          } // end else log
          else
          { // error, none of the parameters are correct

                  fprintf(stdout, "Unknown parameter: %s\n", argv[i]);

                  Usage();

                  bResult = FALSE;

          } // end else error
  }

  return bResult;

}


//callback function called for each process found by EnumProcs (called in main())
BOOL CALLBACK Proc(DWORD ProcID, DWORD ProcParentID, LPCSTR lpstr, LPARAM lParam)
{

  int i;
  char * pos;
  BOOL bResult = TRUE;

  DWORD dwCurrentProcessID = 0;

        if ( ( lParam == LIST ) && (strcmp(lpstr, "" ) ) )
        { // display list of the current processed

            fprintf( stdout, "PID: %X: Process Name: %s\n", ProcID, lpstr );

        } // end if

        if ( ( lParam == LOG ) && (strcmp(lpstr, "" ) ) )
        { // log the current process ID


            // Get the current process ID, so we know who we are
            dwCurrentProcessID = GetCurrentProcessId( );

            if ( ProcID != dwCurrentProcessID )
            {

              fprintf( logfile, "PID Hex: %X, PID Dec %u,Process Name: %s\n", ProcID, ProcID, lpstr );

              fwrite( &ProcID, sizeof(DWORD), 1,  logfilePID );

            }

#ifdef _DEBUG
            fprintf( stdout, "PID Hex: %X\n",ProcID );
#endif

        } // end if

        if ( ( lParam == DELTA ) && (strcmp(lpstr, "" ) ) )
        { // if

           // if we doing delta of processes, save current process ID
           dwCurrentProcessIDs[iCurrentProcessIndex] = ProcID;

           Process_Entries[iCurrentProcessIndex].th32ProcessID       = ProcID;
           Process_Entries[iCurrentProcessIndex].th32ParentProcessID = ProcParentID;

           // bump index
           iCurrentProcessIndex ++;

        } // end if

       return bResult;

}

//
// Main Program
//
int main(int argc, char* argv[])
{

  //
  // local data
  //
  int iExitCode = 0;


   // initialize data structures
   Init_Data( );


   if ( ( !Process_Params(argc, argv) ) )
   {
      // major error, bail out...
        iExitCode = 1;
        goto cleanup;
   }

   if ( ( !EnumProcs(&Proc, Actions) ) )
   {
      // major error, bail out...
        iExitCode = 1;
        goto cleanup;
   }

   if ( Actions == DELTA )
   { // delta processing

      iExitCode = DeltaProc( logfilePID );

   } // end delta processing


// Clean up before we leave
cleanup:

   //
   // check if need to close any files that where opened
   //

   if ( logfile != NULL )
     fclose( logfile );

   if ( logfilePID != NULL )
     fclose( logfilePID );

  // return the exit code to whoever needs it
   return iExitCode;

}
