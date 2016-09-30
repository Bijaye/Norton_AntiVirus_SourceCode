// Scan.h: interface for the Scan class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCAN_H__2D9CC666_7E60_11D2_ACEA_00A0C9C71BBC__INCLUDED_)
#define AFX_SCAN_H__2D9CC666_7E60_11D2_ACEA_00A0C9C71BBC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <AVIS.h>
#include <time.h>

#include <Handle.h>
#include <CMclThread.h>

#include <NAVScan.h>

#include <NAVAPI.h>

class NavEngineHandle;

class Scan : public CMclThreadHandler
{
public:
  Scan() :  CMclThreadHandler(), pipeHandle((HANDLE)0xFFFFFFFF), serviceOLap(NULL),
            overlapEvent(CreateEvent(NULL, TRUE, FALSE, NULL)), messageID(0),
            returnID(0), localCookie(0), updateDB(false), sigSeqNum(0),
            fileToScan(""), active(false), activeScanning(false), ticksAtStart(0)
  {};

  virtual ~Scan();

  enum	Status { NotInfected, Infected, UnableToOpenFile };

  //	Handle the "scan" message
  bool	ScanFile(char* buffer, DWORD bytesRead); 


  //  Handle the "query" message
  bool	QueryScan(char* buffer, DWORD bytesRead);

  //
  //	These methods are called by the NAVEngine object to manipulate the
  //	data in the files.  Since this code could scan either files or 
  //	memory mapped files, these methods will be working at the file level.
  //	I'm not sure how much slower this will make the scanning of the memory
  //	mapped files.  There will be no more disk access then if we accessed
  //	the memory directly.  On the other hand it is additional system calls.
  //	In version two maybe we can distinguish between the two.  (Or always use
  //	memory mapped files - may I should do that now - the interface could map the
  //	file if it was not already mapped?)
  DWORD	ioOpen(DWORD dwOpenMode);
  BOOL	ioClose();
  DWORD	ioSeek(LONG lOffset, int nFrom);
  UINT	ioRead(LPVOID lpvBuffer, UINT nBytesToRead);
  UINT	ioWrite(LPVOID lpvBuffer, UINT nBytesToWrite);


  void	SetPipeData(HANDLE pHandle, OVERLAPPED* sOLap)
  { pipeHandle = pHandle; serviceOLap = sOLap; };

  unsigned ThreadHandlerProc(void);

  void	DisconnectPipe(void);

  bool		Active(void)		{ return active; }
  bool		ActiveScanning(void){ return activeScanning; }
  clock_t		StartTicks(void)	{ return ticksAtStart; }

  std::string	ExceptionHandler(const char* where);

private:
  HANDLE		pipeHandle;
  OVERLAPPED*	serviceOLap;
  OVERLAPPED	overlapped;
  Handle		overlapEvent;
  bool		mappedFile;
  HANDLE		fileHandle;
  uchar		*mappedPointer;
  long		currLoc;
  ulong		fileSize;

  uint		messageID, returnID;
  uint		localCookie;
  bool		updateDB;
  uint		sigSeqNum;
  std::string	fileToScan;
  bool		writeAccess;
  bool		repairInitialized;
  bool		overflowed;

  uint		padding;			// make repairable copies this much bigger so NAV
  // can write past the end of the file.
  bool		active;
  bool		activeScanning;
  clock_t		ticksAtStart;

  enum		consts { InitPaddingSize = 256*1024 };

  bool			IsInfectable(NavEngineHandle& navHandle, std::string& fileName,
                                     std::string& extension);
  NAVScan::ScanRC	RepairFile(NavEngineHandle& navHandle, std::string& fileName,
                                   std::string& extension);

  DWORD			GetVirusInfo(HNAVVIRUS virusInfo, NAVVIRINFO flag, char* data, DWORD* length);
  NAVSTATUS		NavFreeVirusHandle(HNAVVIRUS& hVirus);
};

#endif // !defined(AFX_SCAN_H__2D9CC666_7E60_11D2_ACEA_00A0C9C71BBC__INCLUDED_)
