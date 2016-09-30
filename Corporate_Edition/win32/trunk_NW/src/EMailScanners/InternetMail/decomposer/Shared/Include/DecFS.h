//------------------------------------------------------------------------
// DecFS.h
//
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) Symantec, Corp. 2001, 2005. All rights reserved.
//------------------------------------------------------------------------

#ifndef DECFS_H
#define DECFS_H

#define DECFS_OK					0
#define DECFS_ERROR					1

#define DECFS_MAX_FILES				2048
#define DECFS_MAX_HANDLES			1024

#define DECFS_DEFAULT_FS_SIZE		4 * 1024 * 1024
#define DECFS_DEFAULT_FILE_SIZE_THRESHOLD 1024 * 1024

//
// Added to subvent non-POSIX platforms not supporting these macros
// note: POSIX systems will already have this.
//
#if !defined(S_ISDIR)
	#define S_ISDIR(x) (((x) & S_IFDIR) != 0)
#endif

#if !defined(S_ISREG)
	#define S_ISREG(x) (((x) & S_IFREG) != 0)
#endif


#if defined(SYM_LINUX)
#define _IOREAD		0x0001
#define _IOWRT		0x0002
#define _IORW		0x0080

#define _S_IFMT		__S_IFMT
#define _S_IFDIR	__S_IFDIR
#define _S_IFCHR	__S_IFCHR
#define _S_IFREG	__S_IFREG
#define _S_IREAD	__S_IREAD
#define _S_IWRITE	__S_IWRITE
#define _S_IEXEC	__S_IEXEC

#define _O_RDONLY	O_RDONLY
#define _O_WRONLY	O_WRONLY
#define _O_RDWR		O_RDWR
#define _O_APPEND	O_APPEND
#define _O_CREAT	O_CREAT
#define _O_TRUNC	O_TRUNC
#endif

#if defined(SYM_AIX)
#define _S_IREAD	S_IREAD
#define _S_IWRITE	S_IWRITE

#define _O_RDONLY	O_RDONLY
#define _O_WRONLY	O_WRONLY
#define _O_RDWR		O_RDWR
#define _O_APPEND	O_APPEND
#define _O_CREAT	O_CREAT
#define _O_TRUNC	O_TRUNC
#endif

#if defined(SYM_SOLARIS)
#define _S_IFMT		S_IFMT
#define _S_IFDIR	S_IFDIR
#define _S_IFCHR	S_IFCHR
#define _S_IFREG	S_IFREG
#define _S_IREAD	S_IREAD
#define _S_IWRITE	S_IWRITE
#define _S_IEXEC	S_IEXEC

#define _O_RDONLY	O_RDONLY
#define _O_WRONLY	O_WRONLY
#define _O_RDWR		O_RDWR
#define _O_APPEND	O_APPEND
#define _O_CREAT	O_CREAT
#define _O_TRUNC	O_TRUNC
#endif

#if defined(SYM_FREEBSD)
#define _S_IFMT		S_IFMT
#define _S_IFDIR	S_IFDIR
#define _S_IFCHR	S_IFCHR
#define _S_IFREG	S_IFREG
#define _S_IREAD	S_IREAD
#define _S_IWRITE	S_IWRITE
#define _S_IEXEC	S_IEXEC

#define _O_RDONLY	O_RDONLY
#define _O_WRONLY	O_WRONLY
#define _O_RDWR		O_RDWR
#define _O_APPEND	O_APPEND
#define _O_CREAT	O_CREAT
#define _O_TRUNC	O_TRUNC
#endif

#if defined(SYM_S390)
#define _S_IFMT		S_IFMT
#define _S_IFREG	S_IFREG
#define _S_IREAD	S_IRUSR
#define _S_IWRITE	S_IWUSR
#define _S_IEXEC	S_IEXEC

#define _O_RDONLY	O_RDONLY
#define _O_WRONLY	O_WRONLY
#define _O_RDWR		O_RDWR
#define _O_APPEND	O_APPEND
#define _O_CREAT	O_CREAT
#define _O_TRUNC	O_TRUNC
#endif

#if defined(OS400)
#define _S_IFMT		S_IFMT
#define _S_IREAD	S_IRUSR
#define _S_IWRITE	S_IWUSR
#define _S_IEXEC	S_IEXEC

#define _O_RDONLY	O_RDONLY
#define _O_WRONLY	O_WRONLY
#define _O_RDWR		O_RDWR
#define _O_APPEND	O_APPEND
#define _O_CREAT	O_CREAT
#define _O_TRUNC	O_TRUNC
#endif

#include "dec_assert.h"
#include "DecNet.h"

#if defined (SYM_NLM)
	#include <nwsemaph.h>
	#include <nwthread.h>

	//
	// Make sure we don't hog the CPU
	//
	#define YIELD() ThreadSwitchWithDelay()
#else
	#define YIELD()
#endif

///////////////////////////////////////////////////////////////////////////////
// Debug compiler switches and macros
//
// Uncomment SYM_FAIL_FILESYSTEM_CALLS to turn on the ability
// to fail the file system API's in different ways.  
//
// FAIL_FILESYSTEM_TOPLEVEL will cause all dec_XXX API's to immediately fail.
// This is useful for testing how the engines handle I/O failures.
// This flag affects all namespaces and overrides the
// FAIL_FILESYSTEM_INTERNAL flag.
//
// FAIL_FILESYSTEM_INTERNAL will cause the underlying WriteFile() and ReadFile()
// API's to fail.  This is useful for testing the file system
// (dec_xxx) calls themselves.  This flag only affects the in-memory
// file system.  No effect if FAIL_FILESYSTEM_TOPLEVEL is 
// also defined.
//
//#define SYM_FAIL_FILESYSTEM_CALLS

#define FAIL_FILESYSTEM_NONE		0x0
#define FAIL_FILESYSTEM_TOPLEVEL	0x2
#define FAIL_FILESYSTEM_INTERNAL	0x4

#if defined(SYM_FAIL_FILESYSTEM_CALLS)
	#define CHECK_FOR_FAIL_TOPLEVEL_REQUEST(errReturnCode) if(g_nFailWhichFSCalls & FAIL_FILESYSTEM_TOPLEVEL) return errReturnCode;
	#define CHECK_FOR_FAIL_INTERNAL_REQUEST(errReturnCode) if(g_nFailWhichFSCalls & FAIL_FILESYSTEM_INTERNAL) return errReturnCode;
	#pragma message("***WARNING*** SYM_FAIL_FILESYSTEM_CALLS is defined in DecFS.h! For debugging only! Do not check in the file with this defined!")
#else
	#define CHECK_FOR_FAIL_TOPLEVEL_REQUEST(errReturnCode)
	#define CHECK_FOR_FAIL_INTERNAL_REQUEST(errReturnCode)
#endif
//
// End debug compiler switches and macros
///////////////////////////////////////////////////////////////////////////////

//
// File system type flags
//
#define FILE_SYSTEM_UNKNOWN			0x00000001
#define FILE_SYSTEM_MEM				0x00000002
#define FILE_SYSTEM_UNI				0x00000004
#define FILE_SYSTEM_RAPI			0x00000008
#define FILE_SYSTEM_CDFS			0x00000010

//
// Common file system flags
//
#define FILE_FLAG_EOF				0x00000001	// Set if we have read past the end-of-file.
#define FILE_FLAG_ERROR				0x00000002	// Set if an I/O error has occurred on the stream.

//
// Flags specific to the in-memory file system
//
#define MEMDIR_FLAG_SWAPPED			0x00000001	// Set if in-memory file has been swapped to disk.
#define MEMDIR_FLAG_SWAPPED_W		0x00000002	// Set if in-memory file has been swapped to disk (Unicode).
#define MEMDIR_FLAG_ATTACHED		0x00000004	// Set if memory block was attached.
												// If set, the caller owns the block and it
												// can not be modified (i.e. it is read-only).
//
// Forward defines
//
class CMemDir;
class CRapiFS;

//
// Base class for Decomposer SDK file system objects (this is the only data member inside of the X_FILE structure).
//
class CDecSDKFileBase
{
public :
	CDecSDKFileBase()
	{
		//
		// This member must be set by any derived classes
		//
		dwFileSystem = FILE_SYSTEM_UNKNOWN;
		uiFlags = 0;
		memset(szMode, 0, sizeof(szMode));
	}

	CDecSDKFileBase(DWORD dwFileSystemType)
	{
		dwFileSystem = dwFileSystemType;
		uiFlags = 0;
		memset(szMode, 0, sizeof(szMode));
	}

	virtual ~CDecSDKFileBase() { };

	//
	// Member variables that are common to all file systems
	//
	DWORD			dwFileSystem;	// FILE_SYSTEM_xxx bit flags.
	unsigned int	uiFlags;		// FILE_FLAG_xxx bit-flags.
	char			szMode[6];		// Mode string used to fopen the stream (e.g. "r+b").

};

// 
// Class definition for an in-memory file system object.
//
class CMemFile1 : public CDecSDKFileBase
{
public:
	CMemFile1();
	virtual ~CMemFile1();

	int				iGetc;			// Next character to read (if not EOF) - set by ungetc call
	size_t			ulPos;			// Current file position/byte-offset.
	int				iMode;			// Open mode bit-flags (e.g. _O_RDONLY).
	FILE			*pdiskfile;     // On-disk POSIX-style FILE *.
	CMemDir			*pdir;          // Pointer back to this stream's directory object.
};


// 
// Class definition for in-memory file system's file/directory management system.
//
class CMemDir
{
public:
	CMemDir();
	~CMemDir();

	int				index;			// Save this object's index into m_ppdir array.
	unsigned int	uiFlags;		// MEMDIR_FLAG_xxx bit-flags.
	unsigned int	uiOpenCount;	// Number of opens on this object
	unsigned int	uiHash;
	size_t			ulSize;			// Actual size of stored data.
	unsigned char	*pStore;        // Pointer to back-end storage buffer.
	size_t			ulStoreSize;	// Byte-size of back-end storage buffer.
	char			szName[MAX_PATH];
	char			szDiskName[MAX_PATH];
	wchar_t			szDiskNameW[MAX_PATH];
	time_t			ftCreate;
	time_t			ftLastAccess;
	time_t			ftLastWrite;
	unsigned long int dwAttrs;
};

// 
// This structure is used in place of the FILE structure. The
// pfile pointer is a base class pointer.  File system implementors
// are expected to derive a new class that contains any data members
// that are specific to their file system.
//
typedef CDecSDKFileBase X_FILE;

//
// Code cleanup macro
//
#if defined(UNIX)
	#define GET_MEMFILE(x) (CMemFile1 *)(x);
#else
	#define GET_MEMFILE(x) reinterpret_cast<CMemFile1 *>(x);
#endif

/////////////////////////////////////////////////////////////////////////////
//
//  Platform independent critical section class
//
/////////////////////////////////////////////////////////////////////////////

class CCritSect
{
public:

	CCritSect()
	{
	#if defined (_WINDOWS)
		InitializeCriticalSection(&m_CriticalSection);
	#elif defined (SYM_NLM)
		m_CriticalSection = OpenLocalSemaphore(1);
	#else
		int rc = pthread_mutex_init(&m_CriticalSection, NULL);
		dec_assert(rc == 0);
		// Hush compiler warning when _NDEBUG is defined
		(void)(rc);
	#endif
	}

	~CCritSect()
	{
	#if defined (_WINDOWS)
		DeleteCriticalSection(&m_CriticalSection);
	#elif defined (SYM_NLM)
		int rc = CloseLocalSemaphore(m_CriticalSection);
		dec_assert(rc == 0);
		(void)(rc);
	#else
		int rc = pthread_mutex_destroy(&m_CriticalSection);
		dec_assert(rc == 0);
		(void)(rc);
	#endif
	}

	inline void In()
	{
		#if defined (_WINDOWS)
			EnterCriticalSection(&m_CriticalSection);
		#elif defined (SYM_NLM)
			int rc = WaitOnLocalSemaphore(m_CriticalSection);
			dec_assert(0 == rc);
			(void)(rc);
		#else
			int rc = pthread_mutex_lock(&m_CriticalSection);
			dec_assert(0 == rc);
			(void)(rc);
		#endif	
	}

	inline void Out()
	{
		#if defined (_WINDOWS)
			LeaveCriticalSection(&m_CriticalSection);
		#elif defined (SYM_NLM)
			int rc = SignalLocalSemaphore(m_CriticalSection);
			dec_assert(0 == rc);
			(void)(rc);
		#else
			int rc = pthread_mutex_unlock(&m_CriticalSection);
			dec_assert(0 == rc);
			(void)(rc);
		#endif
	}

private:
#if defined(_WINDOWS)
	CRITICAL_SECTION m_CriticalSection;
#elif defined(SYM_NLM)
	long m_CriticalSection;
#elif defined(UNIX)
	pthread_mutex_t m_CriticalSection;
#endif
};


/////////////////////////////////////////////////////////////////////////////
// Interface IDecFiles - interface definition for Decomposer file system
// C++ interface for Decomposer file system functions.

class IDecFiles
{
public:
	IDecFiles() { }
	virtual ~IDecFiles() { }

	//
	// Public file system support functions
	//
	virtual void	SetNetService(INetData *pnet) = 0;
	virtual unsigned long int GetFileSystemSize() = 0;
	virtual void	SetFileSystemSize(unsigned long int ulSize) = 0;
	virtual unsigned long int GetFileSizeThreshold() = 0;
	virtual void	SetFileSizeThreshold(unsigned long int ulSize) = 0;
	virtual const char *GetTempFilePath() = 0;
	virtual void	SetTempPath(const char *path) = 0;
	virtual const wchar_t *GetTempFilePathW() = 0;
	virtual void	SetTempPathW(const wchar_t *path) = 0;
	virtual bool	GetTempFile(char *pszFilename, int iSize) = 0;
	virtual bool	GetTempFileW(wchar_t *pszFilename, int iSize) = 0;
	virtual bool	SwitchFileToDisk(const char *filename, char *pszondiskname, int iBufferSize) = 0;
	virtual bool	SwitchFileToDiskW(const char *filename, wchar_t *pszondiskname, int iBufferSize) = 0;
	virtual bool	AttachMemoryBlock(char *pszName, size_t ulNameBufSize, void *ptr, size_t sizeOfPtr) = 0;
	virtual void	SetSingleThreadMode(bool bMode) = 0;

	//
	// Main file system interface - add new functions to the end.
	//
	virtual int		x_access(const char *path, int mode) = 0;
	virtual int		x_chmod(const char *filename, int pmode) = 0;
	virtual void	x_clearerr(X_FILE *stream) = 0;
	virtual int		x_fclose(X_FILE *stream) = 0;
	virtual int		x_fcloseall(void) = 0;
	virtual int		x_feof(X_FILE *stream) = 0;
	virtual int		x_ferror(X_FILE *stream) = 0;
	virtual int		x_fflush(X_FILE *stream) = 0;
	virtual int		x_fgetc(X_FILE *stream) = 0;
	virtual char	*x_fgets(char *string, int n, X_FILE *stream) = 0;
	virtual X_FILE	*x_fopen(const char *filename, const char *mode) = 0;
	virtual int		x_fprintf(X_FILE *stream, const char *format, ...) = 0;
	virtual int		x_fputc(int c, X_FILE *stream) = 0;
	virtual int		x_fputs(const char *szstring, X_FILE *stream) = 0;
	virtual size_t	x_fread(void *buffer, size_t size, size_t count, X_FILE *stream) = 0;
	virtual int		x_fseek(X_FILE *stream, long offset, int origin) = 0;
	virtual long	x_ftell(X_FILE *stream) = 0;
	virtual int		x_ftruncate(X_FILE *stream, size_t size) = 0;
	virtual size_t	x_fwrite(const void *buffer, size_t size, size_t count, X_FILE *stream) = 0;
	virtual int		x_remove(const char *path) = 0;
	virtual int		x_rename(const char *oldname, const char *newname) = 0;
	virtual void	x_rewind(X_FILE *stream) = 0;
	virtual int		x_setvbuf(X_FILE *stream, char *buffer, int mode, size_t size) = 0;
	virtual int		x_ungetc(int c, X_FILE *stream) = 0;
	virtual bool	x_memattach(char *pszName, size_t ulNameBufSize, void *ptr, size_t ulSize) = 0;

#if defined(_WINDOWS)
	virtual int		x_stat(const char *path, struct _stat *buffer) = 0;
	virtual int		x_fstat(X_FILE *stream, struct _stat *buffer) = 0;
	virtual int		x_utime(const char *filename, struct _utimbuf *times) = 0;
	virtual int		x_futime(X_FILE *stream, struct _utimbuf *filetime) = 0;
#else
	virtual int		x_stat(const char *path, struct stat *buffer) = 0;
	virtual int		x_fstat(X_FILE *stream, struct stat *buffer) = 0;
	virtual int		x_utime(const char *filename, struct utimbuf *times) = 0;
	virtual int		x_futime(X_FILE *stream, struct utimbuf *filetime) = 0;
#endif
};


class CDecFiles :
	public IDecFiles
{
public:
	CDecFiles();
	virtual ~CDecFiles();

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

	// New public interface functions for examining/manipulating the
	// in-memory file system from client code.
	virtual CMemDir *GetEmptyEntry(void);
	virtual CMemDir *FindEntry(const char *pszName);
	virtual CMemDir *FindFirstEntry(unsigned long int *pulHandle);
	virtual CMemDir *FindNextEntry(unsigned long int ulHandle);
	virtual void	FindEntryClose(unsigned long int ulHandle);

private:
	void AdjustReopenMode(char *pszMode);

	X_FILE	*NewFile(void);
	X_FILE	*AddDirEntry(const char *pszName, const char *pszMode);

	size_t	WriteFile(const void *buffer, size_t size, size_t count, X_FILE *stream, CMemDir *pdir);
	bool	SwitchMemoryFileToDisk(X_FILE *stream, CMemDir *pdir);
	bool	CopyMemoryFileToDisk(X_FILE *stream, CMemDir *pdir, FILE *fp);
	bool	InitMemoryFileSystem();

	bool	CreateTempFile(char *pszFile, int iSize);
	bool	CreateTempFile(const char *szFile, bool *pbFileExists);
	bool	GetNextTempFileName(char *pszName, int iSize, bool bAddNamespace = true);
	
	X_FILE	*SetEndOfFile(X_FILE *stream, size_t size);

	int		uni_access(const char *path, int mode);
	int		uni_chmod(const char *filename, int pmode);
	X_FILE	*uni_fopen(const char *filename, const char *mode);
	int		uni_remove(const char *path);
	int		uni_rename(const char *oldname, const char *newname);
#if defined(_WINDOWS)
	int		uni_stat(const char *path, struct _stat *buffer);
	int		uni_utime(const char *filename, struct _utimbuf *times);
#else
	int		uni_stat(const char *path, struct stat *buffer);
	int		uni_utime(const char *filename, struct utimbuf *times);
#endif

	CCritSect			m_CriticalDir;		// Associated with pdir
	CCritSect			m_CriticalFile;		// Associated with pfile
	CCritSect			m_CriticalTempFile; // Associated with temp file creation

	int					m_iUseWideTempPath;
	int					m_iCachedDirIndex;
	char				*m_pszTempPath;
	wchar_t				*m_pszTempPathW;
	bool				m_bInitDone;
	bool				m_bMultiThread;
	CMemDir				**m_ppdir;
	INetData			*m_pnet;
	unsigned char		*m_pfs;
	unsigned long int	m_ulMaxFSSize;
	unsigned long int	m_ulFSSize;
	unsigned long int	m_ulFileSizeThreshold;

	unsigned long int	m_dwUnique;
	char				m_szProcessString[6];

	CRapiFS				*m_pRapiFS;         // Pointer to RAPI file system
};

#endif // DECFS_H
