#ifndef MAPPEDMEMCALLBACK_H
#define MAPPEDMEMCALLBACK_H

#include <AVIS.h>
#include <Handle.h>

class MappedMemCallback
{
public:
	enum Whence { mmcbBegining = 0, mmcbCurrent = 1, mmcbEnd = 2 };
	enum Access { mmcbRead = 0, mmcbWrite = 1, mmcbReadWrite = 2 };

	MappedMemCallback(const char* mappedFileName, Access& access);
	MappedMemCallback(const char* mappedFileName, uint size);
	~MappedMemCallback();

	bool	Seek(long offset, Whence whence, DWORD& newOffset);
	bool	Read(void* buffer, DWORD bytes, DWORD& bytesRead);
	bool	Write(void* buffer, DWORD bytes, DWORD& bytesWritten);
	bool	Truncate(void);
	long	FileLength(void);

private:
	Handle	handle;
	uchar*	ptr;		// pointer to the start of the memory buffer
	long	loc;		// current offset(location) in the memory buffer
	long	eof;		// current end of the memory buffer
	long	size;		// maximum size of the buffer
};

#endif
