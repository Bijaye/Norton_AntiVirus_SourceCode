/*********************

   file: EnumProc.c

*********************/

// user include files
#include "EnumProc.h"
#include "error.h"
#include "softproc.h"

// system include files
#include <tlhelp32.h>   // windows 95 only

// global data
unsigned int uiNumberofProcesses = 0;


//
// External references
//
extern FILE *logfile;
extern int iCurrentProcessNumber;

   // The EnumProcs function takes a pointer to a callback function
   // that will be called once per process in the system providing
   // process EXE filename and process ID.
   // Callback function definition:
   // BOOL CALLBACK Proc( DWORD dw, LPCSTR lpstr, LPARAM lParam ) ;
   //
   // lpProc -- Address of callback routine.
   //
   // lParam -- A user-defined LPARAM value to be passed to
   //           the callback routine.
   BOOL WINAPI EnumProcs( PROCENUMPROC lpProc, LPARAM lParam )
   {

      OSVERSIONINFO  osver ;
      HINSTANCE      hInstLib ;
      HINSTANCE      hInstLib2 ;
      HANDLE         hSnapShot ;
      PROCESSENTRY32 procentry ;
      BOOL           bFlag ;
      LPDWORD        lpdwPIDs ;
      DWORD          dwSize, dwSize2, dwIndex ;
      HMODULE        hMod ;
      HANDLE         hProcess ;
      char           szFileName[ MAX_PATH ] ;
      EnumInfoStruct sInfo ;

      BOOL bResult = TRUE; //return code

      // ToolHelp Function Pointers.
      HANDLE (WINAPI *lpfCreateToolhelp32Snapshot)(DWORD,DWORD) ;
      BOOL (WINAPI *lpfProcess32First)(HANDLE,LPPROCESSENTRY32) ;
      BOOL (WINAPI *lpfProcess32Next)(HANDLE,LPPROCESSENTRY32) ;


      // for error messages
      char szErrMsg[100];

      // Check to see if were running under Windows95 or
      // Windows NT.
      osver.dwOSVersionInfoSize = sizeof( osver ) ;

      if ( !GetVersionEx( &osver ) )
      {
                PrintErr("GetVersionEx failed:", GetLastError());
                return FALSE ;
      }

      //
      // Need to check if Windows 95, TOOLHELP library only for win9x
      //
      // Note: Do NT later, need PSAPI.DLL for NT
      //
      if ( osver.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS )
      { // Running on Windows 9x

         hInstLib = LoadLibraryA( "Kernel32.DLL" ) ;

         if ( hInstLib == NULL )
            return FALSE ;

         //
         // Get procedure addresses.
         // We are linking to these functions of Kernel32
         // explicitly, because otherwise a module using
         // this code would fail to load under Windows NT,
         // which does not have the Toolhelp32
         // functions in the Kernel 32.
         //

         lpfCreateToolhelp32Snapshot= (HANDLE(WINAPI *)(DWORD,DWORD)) GetProcAddress( hInstLib, "CreateToolhelp32Snapshot" ) ;

         lpfProcess32First = (BOOL(WINAPI *)(HANDLE,LPPROCESSENTRY32)) GetProcAddress( hInstLib, "Process32First" ) ;

         lpfProcess32Next  = (BOOL(WINAPI *)(HANDLE,LPPROCESSENTRY32)) GetProcAddress( hInstLib, "Process32Next" ) ;

         if ( lpfProcess32Next == NULL ||
            lpfProcess32First == NULL ||
            lpfCreateToolhelp32Snapshot == NULL )
         {
            FreeLibrary( hInstLib ) ;
            return FALSE ;
         }

         // Get a handle to a Toolhelp snapshot of the systems
         // processes.
         hSnapShot = lpfCreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0 ) ;

         if ( hSnapShot == INVALID_HANDLE_VALUE )
         {
            FreeLibrary( hInstLib ) ;
            return FALSE ;
         }

         // Get the first process' information.
         procentry.dwSize = sizeof(PROCESSENTRY32) ;

         bFlag = lpfProcess32First( hSnapShot, &procentry ) ;

         // While there are processes, keep looping.
         while ( bFlag )
         {

            // bump the counter for number of processes running
            uiNumberofProcesses ++;

            // Call the enum func with the filename and ProcID.
            if ( lpProc( procentry.th32ProcessID,
                         procentry.th32ParentProcessID,
                         procentry.szExeFile,
                         lParam ) )
            {

               procentry.dwSize = sizeof(PROCESSENTRY32) ;

               bFlag = lpfProcess32Next( hSnapShot, &procentry );

            }
            else
               bFlag = FALSE ;
         }


      } else
          {
                  fprintf(stderr, "Could not figure out the platform type\n");
         return FALSE ;
          }

      // Free the library.
      FreeLibrary( hInstLib ) ;

      if ( lParam == LOG )
      {
         fprintf( logfile, "Number of Processes: %i\n", uiNumberofProcesses );
         fprintf( stdout,  "Number of Processes: %i\n", uiNumberofProcesses );
      }

      if ( lParam == DELTA )
      {
          iCurrentProcessNumber =  uiNumberofProcesses;
      }

      return bResult;
   }

