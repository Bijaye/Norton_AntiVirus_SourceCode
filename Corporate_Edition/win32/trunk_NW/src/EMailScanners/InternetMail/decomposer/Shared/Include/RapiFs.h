//------------------------------------------------------------------------
// RapiFs.h
//
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) Symantec, Corp. 2001, 2005. All rights reserved.
//------------------------------------------------------------------------

#ifndef RAPI_FS_H
#define RAPI_FS_H

////////////////////////////////////////////////////////////////////////
// Class definition for a RAPI file system object.
//

//
// Pick a buffer size that will allow most non-container/non-infected files to be
// discarded.  Ideally, we want to do all processing within this buffer, unless
// the item is a container.
//
#define RAPI_BUFFER_SIZE (8 * 1024)

class CRapiFile : public CDecSDKFileBase
{
public:
	CRapiFile();
	virtual ~CRapiFile();

	HANDLE hFileHandle;			// CE devices use HANDLE-based I/O
	wchar_t *pwszFilename;      // Wide char version of current filename
	bool bTextMode;				// No native support for 'text' mode, so fake it (UNSUPPORTED)
	bool bAppendMode;			// No native support for 'append' mode so fake it
	bool bHaveUngetcBuff;		// We have a byte in the ungetc buffer
	int nChar;					// One character buffer for ungetc()
	BYTE *pbyBuffer;            // Read buffer.  CE communications are EXTREMELY slow
								// without some sort of buffering.  We are only buffering
								// the input.  Writes will still be slow, but we will rarely
								// need to write to the device (only when replacing
								// a container on the device) and then we will be doing
								// block writes anyway.
	DWORD dwSizeOfBuffer;		// How many bytes are in the buffer
	int nIndexInBuffer;			// Where are we in the buffer?
};


////////////////////////////////////////////////////////////////////////
// Callback definitions for the I/O functions we need (from RAPI.DLL)
//

const char gszRapiFilename[] = "rapi.dll";

typedef HANDLE (STDCALL *PFNCREATEFILE)(LPCWSTR lpFileName,         // CeCreateFile
										DWORD dwDesiredAccess, 
										DWORD dwShareMode, 
										LPSECURITY_ATTRIBUTES lpSecurityAttributes, 
										DWORD dwCreationDisposition, 
										DWORD dwFlagsAndAttributes, 
										HANDLE hTemplateFile);	
typedef BOOL (STDCALL *PFNCLOSEHANDLE)(HANDLE hObject);             // CeCloseHandle
typedef BOOL (STDCALL *PFNREADFILE)(HANDLE hFile,                   // CeReadFile       
									LPVOID lpBuffer, 
									DWORD nNumberOfBytesToRead, 
									LPDWORD lpNumberOfBytesRead, 
									LPOVERLAPPED lpOverlapped);
typedef BOOL (STDCALL *PFNWRITEFILE)(HANDLE hFile,                  // CeWriteFile
									LPCVOID lpBuffer, 
									DWORD nNumberOfBytesToWrite, 
									LPDWORD lpNumberOfBytesWritten, 
									LPOVERLAPPED lpOverlapped);
typedef DWORD (STDCALL *PFNSETFILEPOINTER)(HANDLE hFile,            // CeSetFilePointer
										  long lDistanceToMove, 
										  long *lpDistanceToMoveHigh, 
										  DWORD dwMoveMethod);		
typedef DWORD (STDCALL *PFNGETFILEATTRIBS)(LPCWSTR lpFileName);     // CeGetFileAttributes
typedef BOOL (STDCALL *PFNSETFILEATTRIBS)(LPCWSTR lpFileName,       // CeSetFileAttributes
										 DWORD dwFileAttributes);
typedef DWORD (STDCALL *PFNGETFILESIZE)(HANDLE hFile,               // CeGetFileSize
										LPDWORD lpFileSizeHigh);
typedef BOOL (STDCALL *PFNSETENDOFFILE)(HANDLE hFile);              // CeSetEndOfFile
typedef BOOL (STDCALL *PFNGETFILETIME)(HANDLE hFile,                // CeGetFileTime
									  LPFILETIME lpCreationTime, 
									  LPFILETIME lpLastAccessTime, 
									  LPFILETIME lpLastWriteTime);
typedef BOOL (STDCALL *PFNSETFILETIME)(HANDLE hFile,                // CeSetFileTime
									  const FILETIME *lpCreationTime, 
									  const FILETIME *lpLastAccessTime, 
									  const FILETIME *lpLastWriteTime);
typedef BOOL (STDCALL *PFNMOVEFILE)(LPCWSTR lpExistingFileName,     // CeMoveFile
									LPCWSTR lpNewFileName);
typedef BOOL (STDCALL *PFNDELETEFILE)(LPCWSTR lpFileName);          // CeDeleteFile
typedef DWORD (STDCALL *PFNGETLASTERROR)(void);                     // CeGetLastError
typedef DWORD (STDCALL *PFNRAPIGETERROR)(void);                     // CeRapiGetError


////////////////////////////////////////////////////////////////////////
// CRapiFS definition
//
class CRapiFS :
	public IDecFiles
{
public:
	CRapiFS();
	virtual ~CRapiFS();

	//
	// Public file system support functions
	//
	void	SetNetService(INetData *pnet);
	unsigned long int GetFileSystemSize();
	void	SetFileSystemSize(unsigned long int ulSize);
	unsigned long int GetFileSizeThreshold();
	void	SetFileSizeThreshold(unsigned long int ulSize);
	const char *GetTempFilePath();
	void	SetTempPath(const char *path);
	const wchar_t *GetTempFilePathW();
	void	SetTempPathW(const wchar_t *path);
	bool	GetTempFile(char *pszFilename, int iSize);
	bool	GetTempFileW(wchar_t *pszFilename, int iSize);
	bool	SwitchFileToDisk(const char *filename, char *pszondiskname, int iBufferSize);
	bool	SwitchFileToDiskW(const char *filename, wchar_t *pszondiskname, int iBufferSize);
	bool	AttachMemoryBlock(char *pszName, size_t ulNameBufSize, void *ptr, size_t sizeOfPtr);
	void	SetSingleThreadMode(bool bMode);

	//
	// Main file system interface - add new functions to the end.
	//
	int		x_access(const char *path, int mode);
	int		x_chmod(const char *filename, int pmode);
	void	x_clearerr(X_FILE *stream);
	int		x_fclose(X_FILE *stream);
	int		x_fcloseall(void);
	int		x_feof(X_FILE *stream);
	int		x_ferror(X_FILE *stream);
	int		x_fflush(X_FILE *stream);
	int		x_fgetc(X_FILE *stream);
	char	*x_fgets(char *string, int n, X_FILE *stream);
	X_FILE	*x_fopen(const char *filename, const char *mode);
	int		x_fprintf(X_FILE *stream, const char *format ...);
	int		x_fputc(int c, X_FILE *stream);
	int		x_fputs(const char *szstring, X_FILE *stream);
	size_t	x_fread(void *buffer, size_t size, size_t count, X_FILE *stream);
	int		x_fseek(X_FILE *stream, long offset, int origin);
	long	x_ftell(X_FILE *stream);
	int		x_ftruncate(X_FILE *stream, size_t size);
	size_t	x_fwrite(const void *buffer, size_t size, size_t count, X_FILE *stream);
	int		x_remove(const char *path);
	int		x_rename(const char *oldname, const char *newname);
	void	x_rewind(X_FILE *stream);
	int		x_setvbuf(X_FILE *stream, char *buffer, int mode, size_t size);
	int		x_ungetc(int c, X_FILE *stream);
	bool	x_memattach(char *pszName, size_t ulNameBufSize, void *ptr, size_t ulSize);

#if defined(_WINDOWS)
	int		x_stat(const char *path, struct _stat *buffer);
	int		x_fstat(X_FILE *stream, struct _stat *buffer);
	int		x_utime(const char *filename, struct _utimbuf *times);
	int		x_futime(X_FILE *stream, struct _utimbuf *filetime);
#else
	int		x_stat(const char *path, struct stat *buffer);
	int		x_fstat(X_FILE *stream, struct stat *buffer);
	int		x_utime(const char *filename, struct utimbuf *times);
	int		x_futime(X_FILE *stream, struct utimbuf *filetime);
#endif

	//
	// This function will attempt to load the RAPI.DLL and get all
	// the needed function pointers.  We do it this way instead of
	// linking to RAPI.LIB because if RAPI.DLL isn't present then
	// we cannot proceed...
	//
	bool	Init();

	//
	// This is called whenever we need to refresh the input buffer.
	// 
	bool	FillInputBuffer(X_FILE *stream);

private:
	bool	m_bInitialized;
	bool	m_bMultiThread;
	char	*m_pszTempPath;
	HINSTANCE m_hInst;

	//
	// Function pointers into RAPI.DLL
	//
	PFNCREATEFILE m_pfnCreateFile;				 
	PFNCLOSEHANDLE m_pfnCloseHandle;
	PFNREADFILE m_pfnReadFile;
	PFNWRITEFILE m_pfnWriteFile;	
	PFNSETFILEPOINTER m_pfnSetFilePointer;
	PFNGETFILEATTRIBS m_pfnGetFileAttribs;
	PFNSETFILEATTRIBS m_pfnSetFileAttribs;
	PFNGETFILESIZE m_pfnGetFileSize; 
	PFNSETENDOFFILE m_pfnSetEndOfFile;
	PFNGETFILETIME m_pfnGetFileTime;
	PFNSETFILETIME m_pfnSetFileTime; 
	PFNMOVEFILE m_pfnMoveFile;
	PFNDELETEFILE m_pfnDeleteFile;	 
	PFNGETLASTERROR m_pfnGetLastError;
	PFNRAPIGETERROR m_pfnRapiGetError;

	DWORD m_dwLastError;
	DWORD m_dwRapiError;
};

#endif // RAPI_FS_H
