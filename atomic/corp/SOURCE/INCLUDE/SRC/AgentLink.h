// Copyright (C) 1997 Symantec Corporation
//*****************************************************************************
// $Header:   S:/INCLUDE/VCS/Agentlink.h_v   1.7   05 Aug 1997 12:24:42   DSACKIN  $
//
// Description:
//
//*****************************************************************************
// $Log:   S:/INCLUDE/VCS/Agentlink.h_v  $
// 
//    Rev 1.7   05 Aug 1997 12:24:42   DSACKIN
// Removed GetMachineInformation function.  This will be taken care of
// in the UI.
//
//    Rev 1.6   04 Aug 1997 16:47:12   DSACKIN
// Added prototypes for functions to get the Definitions directory and
// the NAV directory from the remote machine.
//
//    Rev 1.5   02 Aug 1997 03:19:10   DSACKIN
// Added AgentCopyFileFromHandles prototype.
//
//    Rev 1.4   21 Jul 1997 08:37:34   TSmith
// Added 'AgentRemoveDirectoryTree'.
//
//    Rev 1.3   18 Jul 1997 18:02:14   TSmith
// Added declarations for several new exports.
//
//    Rev 1.2   02 Jul 1997 12:42:00   TSmith
// Added 'AgentCreateFile', 'AgentWriteFile' and 'AgentCloseHandle' declarations
//
//    Rev 1.1   18 Jun 1997 11:02:44   TSmith
// Renamed 'AgentSetAdminHost' to 'AgentBeginJob' and added 'AgentEndJob' to
// correspond with changes to ISymAgent interface.
//
//    Rev 1.0   12 Jun 1997 13:58:58   TSmith
// Initial revision.
//*****************************************************************************
#if !defined( __AgentLink_h__ )
#define       __AgentLink_h__

typedef struct _RPC_LINK_INFO
    {
    LPTSTR pszProtocol;                 // Protocol to use
    LPTSTR pszAddress;                  // Address to contact
    LPTSTR pszEndpoint;                 // Endpoint to look for
    } RPC_LINK_INFO, *PRPC_LINK_INFO;

DWORD __declspec( dllexport ) WINAPI AgentRunProgram(
    PRPC_LINK_INFO  pLinkInfo,
    LPDWORD         lpError,
    LPCTSTR         lpCommandLine,
    LPCTSTR         lpProgramPath,
    LPCTSTR         lpClientName,
    LPCTSTR         lpJobFileName
    );

DWORD __declspec( dllexport ) WINAPI AgentGetMifResult(
    PRPC_LINK_INFO  pLinkInfo,
    LPCTSTR         lpszMifFileName
    );

DWORD __declspec( dllexport ) WINAPI AgentCreateFile(
    PRPC_LINK_INFO  pLinkInfo,
    LPDWORD         lpError,
    LPCTSTR         lpFileName,
    DWORD           dwDesiredAccess,
    DWORD           dwShareMode,
    DWORD           dwSecurityAttributes,
    BYTE*           lpSecurityAttributes,
    DWORD           dwCreationDistribution,
    DWORD           dwFlagsAndAttributes,
    DWORD           hTemplateFile
    );

BOOL  __declspec( dllexport ) WINAPI AgentWriteFile(
    PRPC_LINK_INFO  pLinkInfo,
    LPDWORD         lpError,
    DWORD           hFile,
    DWORD           dwBufferSize,
    BYTE*           lpBuffer,
    DWORD           nNumberOfBytesToWrite,
    LPDWORD         lpNumberOfBytesWritten
    );

BOOL __declspec( dllexport ) WINAPI AgentCloseHandle(
    PRPC_LINK_INFO  pLinkInfo,
    LPDWORD         lpError,
    DWORD           hObject
    );

BOOL __declspec( dllexport ) WINAPI AgentGetDiskFreeSpace(
    PRPC_LINK_INFO  pLinkInfo,
    LPDWORD         lpError,
    LPCTSTR         lpRootPathName,
    LPDWORD         lpSectorsPerCluster,
    LPDWORD         lpBytesPerSector,
    LPDWORD         lpNumberOfFreeClusters,
    LPDWORD         lpTotalNumberOfClusters
    );

BOOL __declspec( dllexport ) WINAPI AgentCreateDirectoryTree(
    PRPC_LINK_INFO  pLinkInfo,
    LPDWORD         lpError,
    LPTSTR          lpDirectoryTree
    );

BOOL __declspec( dllexport ) WINAPI AgentRemoveDirectoryTree(
    PRPC_LINK_INFO  pLinkInfo,
    LPDWORD         lpError,
    LPTSTR          lpDirectoryTree
    );

BOOL __declspec(dllexport) WINAPI AgentCopyFileFromHandles(
   PRPC_LINK_INFO  pLinkInfo,
   LPDWORD         lpError,
   HANDLE          hFileLocal,
   HANDLE          hFileRemote,
   DWORD           dwBufferSize);

DWORD __declspec( dllexport ) WINAPI AgentGetTempPath(
    PRPC_LINK_INFO  pLinkInfo,
    LPDWORD         lpError,
    DWORD           dwBufferSize,
    BYTE*           lpBuffer
    );

DWORD __declspec(dllexport) WINAPI AgentGetNAVDirectory(
   PRPC_LINK_INFO  pLinkInfo,
   LPDWORD         lpError,
   DWORD           dwBufferSize,
   BYTE*           lpBuffer);

DWORD __declspec(dllexport) WINAPI AgentGetDefinitionDirectory(
   PRPC_LINK_INFO  pLinkInfo,
   LPDWORD         lpError,
   DWORD           dwBufferSize,
   BYTE*           lpBuffer);

#endif     // __AgentLink_h__
