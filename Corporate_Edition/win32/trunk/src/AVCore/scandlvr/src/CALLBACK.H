// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 1994, 2005 Symantec, Peter Norton Product Group. All rights reserved.
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/callback.h_v   1.14   02 May 1997 13:35:20   CNACHEN  $
//
// Description:
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/INCLUDE/VCS/callback.h_v  $
// 
//    Rev 1.14   02 May 1997 13:35:20   CNACHEN
// Added support for NAVEX1.5
// 
//    Rev 1.13   20 Nov 1996 15:24:50   AOONWAL
// No change.
// 
//    Rev 1.12   22 Oct 1996 12:08:34   AOONWAL
// No change.
// 
//    Rev 1.11   28 Mar 1996 18:32:46   TCASHIN
// Made comment for SReadDiskPhysical and SWriteDiskPhysical
// match the parameters.
// 
//    Rev 1.10   02 Feb 1996 20:05:32   RSTANEV
// Changed the prototypes of SDiskReadPhysical() and SDiskWritePhysical().
//
//    Rev 1.9   09 Jan 1996 09:55:42   RSTANEV
// Changed the APIs to accept LPTSTR instead of LPSTR.
//
//    Rev 1.8   13 Sep 1994 16:39:40   CAREY
// added more details to callback.h
//
//    Rev 1.7   09 Sep 1994 16:56:08   CAREY
//
//    Rev 1.6   09 Sep 1994 12:22:34   CAREY
// geologist infomration added.
//
//    Rev 1.5   03 Sep 1994 16:04:48   CAREY
//
//    Rev 1.4   02 Sep 1994 17:21:48   CAREY
// Contains new filelock and fileunlock callbacks...
//
//    Rev 1.3   25 Aug 1994 14:37:14   CAREY
//************************************************************************

#ifndef CALLBACK_INCLUDE

#define CALLBACK_INCLUDE

#ifdef NAVEX15
#include "callbk15.h"
#else

/*@API:*************************************************************************
@Declaration:

HFILE   (WINAPI FAR *FileOpen)(LPTSTR lpName, UINT uOpenMode)

@Description:

	This function works in exactly the same way as QUAKE FileOpen.
    Once the file is opened, the file pointer should point to the top of the
    file.

@Parameters:
$lpName$ The file name is null terminated string.

$uOpenMode$ The open mode shares the same bit organization as the INT 21,
            AH=3D function.

@Returns:
The function returns a UINT sized -1 to the calling process on error. The
function returns a UINT sized file handle on success (>= 0)
*******************************************************************************/


/*@API:*************************************************************************
@Declaration:

HFILE   (WINAPI FAR *FileClose)(HFILE hHandle);

@Description:

	This function works in exactly the same way as QUAKE FileClose.
    This function closes the specified file.

@Parameters:

$hHandle$ The UINT sized file handle must be a valid handle returned by
          FileOpen.

@Returns:
The function should return a UINT sized -1 to the calling process on error and
0 on success.
*******************************************************************************/


/*@API:*************************************************************************
@Declaration:

    UINT    (WINAPI FAR *FileRead)(HFILE hHandle,LPVOID lpBuffer,UINT uBytes);

@Description:

	This function works in exactly the same way as QUAKE FileRead.

@Parameters:

$hHandle$ The UINT sized file handle must be a valid handle returned by
          FileOpen.

$lpBuffer$ The lpBuffer must be at least uBytes long.

$uBytes$ This must be > 0 and the size of uBytes must be less than a UINT
         sized -1.

@Returns:
The function should return the number of bytes read from the specified file or
a UINT sized -1 on error.
*******************************************************************************/

/*@API:*************************************************************************
@Declaration:

    UINT    (WINAPI FAR *FileWrite)(HFILE hHandle,LPVOID lpBuffer,UINT uBytes);

@Description:

	This function works in exactly the same way as QUAKE FileWrite.

@Parameters:

$hHandle$ The UINT sized file handle must be a valid handle returned by FileOpen.

$lpBuffer$ The lpBuffer must be at least uBytes long.  The size of uBytes must
           be less than a UINT sized -1.

$uBytes$ may be 0 or larger.  A value of 0 should truncate (or expand) the
         specified file at the file's write pointer.

@Returns:
The function should return the number of bytes written to the specified file
or a UINT sized -1 on error.
*******************************************************************************/

/*@API:*************************************************************************
@Declaration:

    DWORD   (WINAPI FAR *FileSeek)(HFILE hHandle,LONG dwOffset,int nFlag);

@Description:

    This function works in exactly the same way as QUAKE FileSeek.  The core
    may try to seek outside the file; this should be allowed.

@Parameters:

$hHandle$ The UINT sized file handle must be a valid handle returned by
          FileOpen.

$dwOffset$ is a signed quantity.

$nFlag$ The nFlag parameter can be SEEK_SET (0), SEEK_CUR (1), or SEEK_END (2)

@Returns:
This function should return the offset of the pointer in the file after the
seek operation has  completed.  On error, return a DWORD sized -1.
*******************************************************************************/

/*@API:*************************************************************************
@Declaration:

    DWORD   (WINAPI FAR *FileSize)(HFILE hHandle);

@Description:

	This function works in exactly the same way as QUAKE FileSize.

@Parameters:

$hHandle$ The UINT sized file handle must be a valid handle returned by
          FileOpen.

@Returns:
This function should return a DWORD sized -1 on error or the file's size on
success.
*******************************************************************************/

/*@API:*************************************************************************
@Declaration:

    BOOL    (WINAPI FAR *FileGetDateTime)(HFILE hHandle,
                                          UINT uType,
                                          WORD FAR *lpuDate,
                                          WORD FAR *lpuTime);

@Description:

    This function obtains the date and time stamps for the specified file.
    There are 3 types of times/dates which can be obtained: last write, last
    access and creation time/date.

    The date and time can be obtained with QUAKE calls using the following
    code:

    {
        auto    WORD        wDosDate = 0;
        auto    WORD        wDosTime = 0;
        auto    FILETIME    FileTime = { 0 };

        FileGetTime ( hHandle, uType, &FileTime );
        FileTimeToDosDateTime( &FileTime, &wDosDate, &wDosTime );
    }

@Parameters:

$hHandle$ The UINT sized file handle must be a valid handle returned by
          FileOpen.

$uType$ The value of uType can be one of the following (defined in FILE.H):

	TIME_LASTWRITE	- last time the file was modified
	TIME_CREATION	- when the file was created
    TIME_LASTACCESS - last time the file was accessed

$lpuDate$ The encoded date obtained will be placed in the UINT variable pointed
          to by this pointer.

$lpuTime$ The encoded time obtained will be placed in the UINT variable pointed
          to by this pointer.

@Returns:
This function returns TRUE on success and FALSE on failure.  The lpuDate and
lpuTime pointers will point to valid encoded date and time values if TRUE is
returned.
*******************************************************************************/

/*@API:*************************************************************************
@Declaration:

    BOOL    (WINAPI FAR *FileSetDateTime)(HFILE hHandle,UINT uType,WORD uDate,WORD uTime);

@Description:

    This function sets the date and time stamps for the specified file.

    The date and time can be set with QUAKE calls using the following code:

    {
        auto    FILETIME    FileTime = { 0 };

        DosDateTimeToFileTime ( wDate, wTime, &FileTime );
        FileSetTime ( hHandle, uType, &FileTime );
    }


@Parameters:

$hHandle$ The UINT sized file handle must be a valid handle returned by
          FileOpen.

$uType$ The value of uType can be one of the following (defined in FILE.H):

	TIME_LASTWRITE	- last time the file was modified
	TIME_CREATION	- when the file was created
	TIME_LASTACCESS     	- last time the file was accessed

$uDate$ is the encoded date which will be applied to the file.

$uTime$ is the encoded time which will be applied to the file.

@Returns:
This function returns TRUE on success and FALSE on failure.
*******************************************************************************/

/*@API:*************************************************************************
@Declaration:

    BOOL    (WINAPI FAR *FileDelete)(LPTSTR lpFilename);

@Description:

    This function deletes the specified file.  If the user likes, they can open
    the file in the callback and wipe its contents first.

@Parameters:

$lpFilename$ is the name of the file to be deleted.

@Returns:
TRUE on success, FALSE on failure.
*******************************************************************************/

/*@API:*************************************************************************
@Declaration:

    UINT    (WINAPI FAR *FileGetAttr)(LPTSTR lpFilename, UINT FAR *lpuAttr);

@Description:

    This function works in exactly the same way as QUAKE FileGetAttr.  It
    retrieves the file attributes of the specified file.

@Parameters:

$lpFilename$ is the name of the file whos attributes we are getting.

$lpuAttr$ points to a UINT variable where the file's attributes will be stored.

@Returns:
This function should return a UINT sized -1 on error and 0 on success.
*******************************************************************************/

/*@API:*************************************************************************
@Declaration:

UINT    (WINAPI FAR *FileSetAttr)(LPTSTR lpFilename, UINT uAttr);

@Description:

	This function works in exactly the same way as QUAKE FileSetAttr.
    It sets the file attributes of the specified file to the specified uAttr.

@Parameters:

$lpFilename$ is the name of the file whos attributes we are getting.

$uAttr$ is a UINT variable containing the desired attributes which will be
        applied to the file.

@Returns:
This function should return a UINT sized -1 on error and 0 on success.
*******************************************************************************/

/*@API:*************************************************************************
@Declaration:

    HFILE   (WINAPI FAR *FileCreate)(LPTSTR lpFilename, UINT uAttr);

@Description:

	This function works in exactly the same way as QUAKE FileCreate.
    The specified file will be created with the appropriate attributes.  If a
    file already exists of the same name, it will be truncated to 0 bytes.

@Parameters:

$lpFilename$ is the name of the file to create.

$uAttr$ is the starting attributes of the file.

@Returns:
This function should return a UINT sized -1 on error and 0 on success.
*******************************************************************************/

/*@API:*************************************************************************
@Declaration:

    BOOL    (WINAPI FAR *FileLock)(HFILE hHandle,DWORD dwStart,DWORD dwLength);

@Description:

    This function should perform a file lock on the specified file handle
    starting at offset dwStart in the file for a length of dwLength bytes.
    A value of dwLength of 0xFFFFFFFF should lock the entire file.

@Parameters:

$hHandle$ The UINT sized file handle must be a valid handle returned by
          FileOpen.

$dwStart$ is the start offset of the region of the file to be locked.

$dwLength$ is the length of the region to be locked.

@Returns:
If the function is able to successfully lock the desired region, FileLock
callback should return TRUE, otherwise it should return FALSE.  If the attempt
to lock the file results in a "SHARE not loaded" type error (ie file locking
is not supported), the callback should return a TRUE value indicating
success.  This will allow inoculation to work on single user, single tasking
systems.
*******************************************************************************/

/*@API:*************************************************************************
@Declaration:

    BOOL    (WINAPI FAR *FileUnlock)(HFILE hHandle,DWORD dwStart,DWORD dwLength);

@Description:

    This function should perform a file unlock on the specified file handle
    starting at offset dwStart in the file for a length of dwLength bytes.
    A value of dwLength of 0xFFFFFFFF should unlock the entire file.

@Parameters:

$hHandle$ The UINT sized file handle must be a valid handle returned by
          FileOpen.

$dwStart$ is the start offset of the region of the file to be locked.

$dwLength$ is the length of the region to be locked.

@Returns:
If the function is able to successfully unlock the desired region,
FileUnlock callback should return TRUE, otherwise it should return FALSE.
If the attempt to unlock the file results in a "SHARE not loaded" type
error (ie file locking is not supported), the callback should return a
TRUE value indicating success.  This will allow inoculation to work on
single user, single tasking systems.
*******************************************************************************/

/*@API:*************************************************************************
@Declaration:

    BOOL    (WINAPI FAR *Progress)(WORD wProgress, DWORD dwCookie);

@Description:

    This callback function is called by the various API's when long operations
    are being performed, such as inoculating or scanning a file (with a
    full-file CRC).  Only those API functions which accept a dwCookie as an
    argument can call the Progress callback.  If the progress callback is
    called, wProgress will contain a number between 0 and 100 specifying the
    % completion of whatever task is currently being performed.  In addition,
    the dwCookie parameter passed by the application to the API will be passed
    back ot the Progress callback.

@Parameters:

$wProgress$ is a number between 0 and 100 indicating the percent completion of
            the current task.

$dwCookie$ is a user cookie which was passed in to the API.  It is passed back
           to the callback unchanged.

@Returns:
The progress callback should return TRUE under normal circumstances and FALSE
if the user wants to abort whatever operation is being performed.
*******************************************************************************/

/*@API:*************************************************************************
@Declaration:

    UINT WINAPI FAR *SDiskReadPhysical)(LPVOID lpBuffer,
                                        BYTE bCount,
                                        WORD wCylinder,
                                        BYTE bSector,
                                        BYTE bHead,
                                        BYTE bDrive);

@Description:

    This callback should perform a physical disk read from the specified
    location on the diskette and place the resulting information into the
    lpBuffer.  The callback should attempt to retry the read operation 3
    or more times before returning with an error code.

@Parameters:

$lpBuffer$ is the buffer where the data from the file will be placed.

$bCount$ is the number of sectors to read.

$wCylinder$ is the cylinder number to read from.

$bSector$ is the sector number to read from (1 based)

$bHead$ is the head number to read from.

$bDrive$ is the drive number to read from.  0=A:, 0x80=first phys. HD, etc

@Returns:
This function should return the same result as the QUAKE DiskAbsOperation
function with a READ_COMMAND.  A value of 0 is returned on success, !0 on
failure. (See a DOS manual for error return codes for INT 13 operations)
*******************************************************************************/

/*@API:*************************************************************************
@Declaration:

    UINT    (WINAPI FAR *SDiskWritePhysical)(LPVOID lpBuffer,
                                             BYTE bCount,
                                             WORD wCylinder,
                                             BYTE bSector,
                                             BYTE bHead,
                                             BYTE bDrive);

@Description:

    This callback should perform a physical disk write to the specified
    location on the diskette from the the lpBuffer.  The callback should
    attempt to retry the write operation 3 or more times before returning
    with an error code.

@Parameters:

$lpBuffer$ is the buffer who's data will be written to the file.

$bCount$ is the number of sectors to write.

$wCylinder$ is the cylinder number to write to.

$bSector$ is the sector number to write to (1 based)

$bHead$ is the head number to write to.

$bDrive$ is the drive number to write to.  0=A:, 0x80=first phys. HD, etc

@Returns:
This function should return the same result as the QUAKE DiskAbsOperation
function with a WRITE_COMMAND.  A value of 0 is returned on success, !0 on
failure. (See a DOS manual for error return codes for INT 13 operations)
*******************************************************************************/

/*@API:*************************************************************************
@Declaration:

    LPVOID  (WINAPI FAR *TempMemoryAlloc)(DWORD uSize);

@Description:

    This callback will be used to allocate memory for various API functions.
    If TempMemoryAlloc is called from within the API, a TempMemoryFree call
    will always be made before the API function returns.  The QUAKE
    MemAllocPtr function can be used for this callback.

@Parameters:

$uSize$ is the number of bytes to allocate.

@Returns:
The function should return a non-NULL pointer to a buffer if the allocation
was successful, or NULL otherwise.
*******************************************************************************/

/*@API:*************************************************************************
@Declaration:

    VOID    (WINAPI FAR *TempMemoryFree)(LPVOID lpBuffer);

@Description:

    This callback will be used to free memory for various API functions. The
    MemFreePtr function can be used for this callback.

@Parameters:

$lpBuffer$ is pointer to a buffer allocated with TempMemoryAlloc.

@Returns:
This function returns nothing.
*******************************************************************************/

/*@API:*************************************************************************
@Declaration:

    LPVOID  (WINAPI FAR *PermMemoryAlloc)(DWORD uSize);

@Description:

    This callback will be used to allocate memory for various API functions.
    If PermMemoryAlloc is called from within the API, a PermMemoryFree call
    may or may not be made before the API function returns.  The QUAKE
    MemAllocPtr function can be used for this callback.

@Parameters:

$uSize$ is the number of bytes to allocate.

@Returns:
The function should return a non-NULL pointer to a buffer if the allocation
was successful, or NULL otherwise.
*******************************************************************************/

/*@API:*************************************************************************
@Declarations:

    VOID    (WINAPI FAR *PermMemoryFree)(LPVOID lpBuffer);

@Description:

    This callback will be used to free memory for various API functions. The
    MemFreePtr function can be used for this callback.

@Parameters:

$lpBuffer$ is pointer to a buffer allocated with TempMemoryAlloc.

@Returns:
This function returns nothing.
*******************************************************************************/


typedef struct
{
    HFILE   (WINAPI FAR *FileOpen)(LPTSTR lpName, UINT uOpenMode);
    HFILE   (WINAPI FAR *FileClose)(HFILE hHandle);
    UINT    (WINAPI FAR *FileRead)(HFILE hHandle, LPVOID lpBuffer, UINT uBytes);
    UINT    (WINAPI FAR *FileWrite)(HFILE hHandle, LPVOID lpBuffer, UINT uBytes);
    DWORD   (WINAPI FAR *FileSeek)(HFILE hHandle, LONG dwOffset, int nFlag);
    DWORD   (WINAPI FAR *FileSize)(HFILE hHandle);
    BOOL    (WINAPI FAR *FileGetDateTime)(HFILE hHandle, UINT uType, WORD FAR *lpuDate,
                                          WORD FAR *lpuTime);
    BOOL    (WINAPI FAR *FileSetDateTime)(HFILE hHandle, UINT uType, WORD uDate,
                                          WORD uTime);
    BOOL    (WINAPI FAR *FileDelete)(LPTSTR lpFilename);
    UINT    (WINAPI FAR *FileGetAttr)(LPTSTR lpFilename, UINT FAR *lpuAttr);
    UINT    (WINAPI FAR *FileSetAttr)(LPTSTR lpFilename, UINT uAttr);
    HFILE   (WINAPI FAR *FileCreate)(LPTSTR lpFilename, UINT uAttr);

    // new additions!

    BOOL    (WINAPI FAR *FileLock)(HFILE hHandle, DWORD dwStart, DWORD dwLength);
    BOOL    (WINAPI FAR *FileUnlock)(HFILE hHandle, DWORD dwStart, DWORD dwLength);
    BOOL    (WINAPI FAR *Progress)(WORD wProgress, DWORD dwCookie);

    // low-level disk i/o

    UINT    (WINAPI FAR *SDiskReadPhysical)(LPVOID lpBuffer, BYTE bCount,
                                            WORD wCylinder, BYTE  bSector,
                                            BYTE bHead,  BYTE bDrive);

    UINT    (WINAPI FAR *SDiskWritePhysical)(LPVOID lpBuffer, BYTE bCount,
                                             WORD wCylinder, BYTE  bSector,
                                             BYTE bHead,  BYTE bDrive);

    ////////////////////////////////////////////////
    // Memory allocation callback functions (common)
    ////////////////////////////////////////////////

    LPVOID  (WINAPI FAR *TempMemoryAlloc)(DWORD uSize);
    VOID    (WINAPI FAR *TempMemoryFree)(LPVOID lpBuffer);

    LPVOID  (WINAPI FAR *PermMemoryAlloc)(DWORD uSize);
    VOID    (WINAPI FAR *PermMemoryFree)(LPVOID lpBuffer);

} CALLBACKREV1;

typedef CALLBACKREV1 FAR *LPCALLBACKREV1;

#if defined(SYM_WIN32)
typedef struct
{
	DWORD			dwReserved;				// must be zero
	DWORD			dwCallBackInfoSize;	    // initialize this member to the sizeof this structure
	CALLBACKREV1	CBR1;
	// Registry functions
	LONG 	(WINAPI FAR *RegistryCreateKeyEx)(  
     	HKEY hKey,             // handle to open key
     	LPCTSTR lpSubKey,      // address of name of subkey to open
     	DWORD Reserved,        // reserved  
     	LPTSTR lpClass,        // address of class string
     	DWORD dwOptions,       // options flag
     	REGSAM samDesired,     // security access mask
    	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	    PHKEY phkResult,       // address of handle to open key
		LPDWORD lpdwDisp );             // disposition
	LONG 	(WINAPI FAR *RegistryOpenKeyEx)(  
     	HKEY hKey,             // handle to open key
     	LPCTSTR lpSubKey,      // address of name of subkey to open
     	DWORD dwOptions,       // options flag
     	REGSAM samDesired,     // security access mask
	    PHKEY phkResult );     // address of handle to open key
	LONG	(WINAPI FAR *RegistryCloseKey) (HKEY hKey);
	LONG	(WINAPI FAR *RegistryDeleteKey) (HKEY hKey, LPCTSTR szSubKey);
	LONG 	(WINAPI FAR *RegistrySetValueEx)(
		HKEY hKey,           	// handle to key to set value for
	    LPCTSTR lpValueName, 	// name of the value to set
  		DWORD Reserved,      	// reserved  
		DWORD dwType,		 	// flag for value type
  		CONST BYTE *lpData,  	// address of value data
  		DWORD cbData );      	// size of value data
	LONG	(WINAPI FAR *RegistryDeleteValue) (HKEY hKey, LPCTSTR szValueName);
    LONG    (WINAPI FAR *RegistryQueryValueEx)(  
	    HKEY hKey,              // handle to open key
    	LPTSTR szValueName,
	    LPDWORD lpReserved,  // reserved
    	LPDWORD lpType,      // address of buffer for value type
	    LPBYTE lpData,       // address of data buffer
    	LPDWORD lpcbData );  // address of data buffer size
    LONG 	(WINAPI FAR *RegistryQueryInfoKey)(
		HKEY hKey,				  // handle to key to query
		LPTSTR lpClass,			  // address of buffer for class string
		LPDWORD lpcbClass,		  // address of size of class string buffer
		LPDWORD lpReserved,		  // reserved
		LPDWORD lpcSubKeys,		  // address of buffer for number of subkeys
		LPDWORD lpcbMaxSubKeyLen,  // address of buffer for longest subkey name length
		LPDWORD lpcbMaxClassLen,  // address of buffer for longest class string length
		LPDWORD lpcValues,		  // address of buffer for number of value entries
		LPDWORD lpcbMaxValueNameLen,  // address of buffer for longest value name length
		LPDWORD lpcbMaxValueLen,  // address of buffer for longest value data length	
		LPDWORD lpcbSecurityDescriptor, // address of buffer for security descriptor length
		PFILETIME lpftLastWriteTime );  // address of buffer for last write time
	LONG 	(WINAPI FAR *RegistryEnumKeyEx)(
		HKEY hKey,			// handle to key to enumerate
		DWORD dwIndex,		// index of subkey to enumerate
		LPTSTR lpName,		// address of buffer for subkey name
		LPDWORD lpcbName,	// address for size of subkey buffer
		LPDWORD lpReserved, // reserved
		LPTSTR lpClass,		// address of buffer for class string
		LPDWORD lpcbClass,	// address for size of class buffer
		PFILETIME lpftLastWriteTime );  // address for time key last written to
	LONG 	(WINAPI FAR *RegistryEnumValue)(  
		HKEY hKey,				// handle to key to query
		DWORD dwIndex,			// index of value to query
		LPTSTR lpValueName,		// address of buffer for value string
		LPDWORD lpcbValueName,	// address for size of value buffer
		LPDWORD lpReserved,		// reserved
		LPDWORD lpType,			// address of buffer for type code
		LPBYTE lpData,			// address of buffer for value data
		LPDWORD lpcbData );		        // address for size of data buffer);
	BOOL 	(WINAPI FAR *GetOSVersion)(
		LPOSVERSIONINFO lpVersionInformation );// pointer to version information structure
	BOOL 	(WINAPI FAR *FileMoveEx)(
  		LPCTSTR lpExistingFileName,  // pointer to the name of the existing file
  		LPCTSTR lpNewFileName,       // pointer to the new name for the file
  		DWORD dwFlags );             // flag that specifies how to move file
	DWORD 	(WINAPI FAR *GetOSLastError)(VOID);
	UINT    (WINAPI FAR *GetOSDirectory)(
		DWORD  code,		// indicates desired directory
  		LPTSTR lpBuffer,  	// address of buffer 
  		UINT uSize );     	// size of directory buffer
	BOOL     (WINAPI FAR *WritePrivateProfileStr)(
        LPCTSTR lpAppName,      // pointer to section name
        LPCTSTR lpKeyName,  	// pointer to key name
        LPCTSTR lpString,   	// pointer to string to add
        LPCTSTR lpFileName );   // pointer to initialization filename
	UINT 	(WINAPI FAR *GetPrivateProfileInteger)(  
		LPCTSTR lpAppName,	  // address of section name
		LPCTSTR lpKeyName,	  // address of key name
		INT nDefault,		  // return value if key name is not found
		LPCTSTR lpFileName ); // address of initialization filename
	DWORD 	(WINAPI FAR *GetPrivateProfileStr)(
		LPCTSTR lpAppName,		  // points to section name
		LPCTSTR lpKeyName,		  // points to key name
		LPCTSTR lpDefault,		  // points to default string
		LPTSTR lpReturnedString,  // points to destination buffer
		DWORD nSize,			  // size of destination buffer
		LPCTSTR lpFileName );     // points to initialization filename
	DWORD 	(WINAPI FAR *GetPrivateProfileSecNames)(
		LPTSTR lpszReturnBuffer,  // address of return buffer
		DWORD nSize,			  // size of return buffer
		LPCTSTR lpFileName );     // address of initialization filename
	DWORD 	(WINAPI FAR *GetPrivateProfileSec)(
		LPCTSTR lpAppName,        // address of section name
  		LPTSTR lpReturnedString,  // address of return buffer
  		DWORD nSize,              // size of return buffer
  		LPCTSTR lpFileName );     // address of initialization filename
	DWORD 	(WINAPI FAR *WritePrivateProfileSec)(
		LPCTSTR lpAppName,        // address of section name
        LPTSTR lpString,          // address of data buffer
  		LPCTSTR lpFileName );     // address of initialization filename
    BOOL    (WINAPI FAR *RemoveDir)(LPCTSTR lpFilename);
	DWORD 	(WINAPI FAR *GetShortFileName)(
  		LPCTSTR lpszLongPath,  	// pointer to a null-terminated path string
  		LPTSTR lpszShortPath,  	// pointer to a buffer to receive the 
        	                   	// null-terminated short form of the path
  		DWORD cchBuffer ); 	    // specifies the size of the buffer pointed 
                         		// to by lpszShortPath
} CALLBACKINFO;

typedef CALLBACKINFO FAR *LPCALLBACKINFO;
#endif // SYM_WIN32

#endif

#endif
