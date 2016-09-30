// MappedMemory.cpp: implementation of the MappedMemory class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MappedMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MappedMemory::MappedMemory(const std::string& fileName, const uint mSize) :
							view(fileName, mSize), maxSize(mSize), currEnd(0)
{

}

MappedMemory::~MappedMemory()
{

}

bool MappedMemory::WriteToFile(const std::string& fileName) 
{
	bool	rc = false;

	Handle	handle(CreateFile(fileName.c_str(), GENERIC_WRITE, 0, NULL,
							  CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL));
	if (INVALID_HANDLE_VALUE != handle)
	{
		DWORD	bytesWritten	= 0;
		rc = WriteFile(handle, view, currEnd, &bytesWritten, NULL);
	}

	return rc;
}

