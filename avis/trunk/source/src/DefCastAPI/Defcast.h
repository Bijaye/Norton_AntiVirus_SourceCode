// DefCast.h : main header file for the DEFCAST DLL
//
#ifndef __DEFCAST_H__
#define __DEFCAST_H__

#ifdef DEFCAST_EXPORTS
#define DEFCAST_API extern "C" __declspec(dllexport)
#else
#define DEFCAST_API extern "C" __declspec(dllimport)
#endif

#define DEFCAST_MAX_COMPUTERNAME (48)           // Based on Intel NAME_SIZE 
#define DEFCAST_MAX_DATA         (260)          // Based on MS _MAX_PATH

typedef enum  {    // values returned from DefCast functions:
     SUCCESS,           // function was successful
     NOTINITIALIZED,    // the DefCast component is not initialized
     TERMINATING,       // the DefCast component is terminating
     TOOMANY,           // [transient] too many concurrent function calls
     NOTCONNECTED,      // [transient] target is not connected to network
     INCOMPLETE,        // [transient] function did not complete successfully
     TIMEOUT,           // [transient] function did not complete within timeout limit
     INVALID,           // [permanent] target name and/or address values are invalid
     UNMANAGED,         // [permanent] target is not managed by NAVCE
     OUTOFMEMORY        // Unable to allocate memory. 
} defcaststatus ;   
// Functions.

DEFCAST_API defcaststatus DefCastInit();    // Call this one time per process before any thread 
                                            // attempts to use Defcast functions.

DEFCAST_API defcaststatus DefCastTerm();    // Releases resources held by Defcast.

DEFCAST_API defcaststatus DefCastInstall( // value returned indicates completion status
   int timeout,            // [input] time limit for completion, in milliseconds
   char* computer,         // [input] name of target, or NULL
   char* hostname,         // [input] TCP/IP fully qualified hostname of target, or NULL
   char* address,          // [input] IP and/or IPX network address of target, or NULL
   char* parent,           // [input] NAVCE distribution server for target, or NULL
   char* guid,             // [input] NAVCE client identifier of target, or NULL
   char* package);         // [input] complete pathname for definition package to install

DEFCAST_API defcaststatus DefCastQuery( // value returned indicates completion status
   int timeout,          // [input] time limit for completion (in milliseconds)
   char* computer,       // [input] name of target, or NULL
   char* hostname,       // [input] TCP/IP fully qualified hostname of target, or NULL
   char* address,        // [input] IP and/or IPX network address of target, or NULL
   char* parent,         // [input] NAVCE distribution server for target, or NULL
   char* guid,           // [input] NAVCE client identifier of target, or NULL
                         // [output] sequence number of installed definitions   
   unsigned long int* sequence);       


DEFCAST_API defcaststatus DefCastConcurrency(   // value returned indicates completion status
        int* maximum);   // [output] DefCast concurrency limit

#endif //__DEFCAST_H__
