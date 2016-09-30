
#include <stdafx.h>

#include "MappedMemCallback.h"
#include "IcePackException.h"

#include <iostream>

using namespace std;

MappedMemCallback::MappedMemCallback(const char* mappedFileName, Access& access) :
		handle(OpenFileMapping(FILE_MAP_WRITE, FALSE, mappedFileName)),
		ptr((uchar *) MapViewOfFile(handle, FILE_MAP_WRITE, 0, 0, 0))
{
	size	= eof	= GetFileSize(handle, NULL);
	if (mmcbReadWrite == access)
		loc	= eof;
	else
		loc	= 0;
}

MappedMemCallback::MappedMemCallback(const char* mappedFileName, uint size_) :
		handle(CreateFileMapping((HANDLE) 0xFFFFFFFF, NULL,
										PAGE_READWRITE, 0, size_,
										mappedFileName)),
		ptr((uchar *) MapViewOfFile(handle, FILE_MAP_WRITE, 0, 0, 0)),
		loc(0), eof(size_), size(size_)
{
}

MappedMemCallback::~MappedMemCallback()
{
	UnmapViewOfFile(ptr);
}

bool MappedMemCallback::Seek(long offset, Whence whence, DWORD& newOffset)
{
//	char	buffer[64];
//	sprintf(buffer, "Seek[loc = %d](%d, ", loc, offset);

	bool	rc	= false;

	switch (whence)
	{
	case mmcbBegining:
//		strcat(buffer, "Beginning, ...)\n");

		loc = offset;
		break;

	case mmcbCurrent:
//		strcat(buffer, "Current, ...)\n");

		loc	+= offset;
		break;

	case mmcbEnd:
//		strcat(buffer, "End, ...)\n");

		loc	= eof - offset;
		break;
	}

//	cerr << buffer;

	if (loc < 0)
		loc	= 0;
	else if (loc > eof)
		loc	= eof;
	else
		rc = true;

	newOffset = loc;


//	sprintf(buffer, "Seek, newOffset set to %d, returns %s\n",
//			newOffset, (rc ? "true" : "false"));
//	cerr << buffer;

	return rc;
}

bool MappedMemCallback::Read(void* buffer, DWORD bytes, DWORD& bytesRead)
{
//	char	msg[64];
//	sprintf(msg, "Read( ..., %d, ...), loc = %d\n", bytes, loc);
//	cerr << msg;

	try
	{
		if (loc + bytes > eof)
			bytes = eof - loc;
		memcpy(buffer, &ptr[loc], bytes);
		bytesRead = bytes;
		loc	+= bytes;
	}
	catch (...)
	{
		throw IcePackException(IcePackException::MappedMemCallback,
								"Read");
	}

//	sprintf(msg, "Read(...), bytesRead = %d, returned %s\n",
//				bytesRead, (bytesRead > 0 ? "true" : "false"));
//	cerr << msg;

	return bytesRead > 0;
}

bool MappedMemCallback::Write(void* buffer, DWORD bytes, DWORD& bytesWritten)
{
//	char	msg[64];
//	sprintf(msg, "Write( ..., %d, ...), loc = %d\n", bytes, loc);
//	cerr << msg;

	try
	{
		if (loc + bytes > size)
			bytesWritten = size - loc;
		else
			bytesWritten = bytes;

		memcpy(&ptr[loc], buffer, bytesWritten);
		loc	+= bytesWritten;
		if (loc > eof)
			eof = loc;
	}

	catch (...)
	{
		throw IcePackException(IcePackException::MappedMemCallback,
								"Write");
	}
//	sprintf(msg, "Write(...), bytesWritten = %d, returned %s\n",
//		bytesWritten, (bytesWritten == bytes ? "true" : "false"));
//	cerr << msg;

	return bytesWritten == bytes;
}

bool MappedMemCallback::Truncate(void)
{
//	char	buffer[64];
//	sprintf(buffer, "File truncated from %d to %d\n", eof, loc);
//	cerr << buffer;

	eof = loc;

	return true;
}


long MappedMemCallback::FileLength()
{
//	char	buffer[64];
//	sprintf(buffer, "FileLength returned %d\n", eof);
//	cerr << buffer;

	return eof;
}
