// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#include "stdafx.h"
#include "smalltxt.h"

CBuffer::CBuffer(DWORD size)
{
	fBufferPointer = new char[size];
	if (fBufferPointer != NULL)
		fSize = size;
	else
		fSize = 0;
}

CBuffer::~CBuffer()
{
	if (fBufferPointer != NULL)
		delete [] fBufferPointer;
}


CSmallTextFile::CSmallTextFile( LPCTSTR fileName )
{
	fOK = FALSE;
	DWORD size, bytesRead;
	fBuffer = NULL;
#ifdef WIN32
	fFile = CreateFile (fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL, NULL);
	if (fFile == INVALID_HANDLE_VALUE)
		return;
	size = GetFileSize(fFile, &bytesRead);
    fBuffer = new CBuffer(size);
	if (!fBuffer) return;
	fOK = ReadFile(fFile, (LPVOID) fBuffer->GetBuffer(), size, &bytesRead, NULL);
	CloseHandle(fFile);
	Rewind();
#else
	if ( ( fFile = OpenFile ( fileName, (LPOFSTRUCT)&fOfStruct, OF_READ )  ) == HFILE_ERROR ) return;
	size = (DWORD)_llseek ( fFile, 0L, 2 );
	if ( !( fBuffer = new CBuffer(size) ) ) return;
	bytesRead = _lread ( fFile, fBuffer->GetBuffer(), size );
	fOK = ( bytesRead == size );
	_lclose ( fFile );	
	Rewind();
#endif	
	
}

CSmallTextFile::~CSmallTextFile()
{
	if (fBuffer != NULL)
		delete fBuffer;
}

void CSmallTextFile::SkipWhiteSpace()
{
	char *currentPos	= fBuffer->GetBuffer() + fCurrentOffset;
	char *endPos		= fBuffer->GetBuffer() + fBuffer->GetSize(); 
	while (currentPos < endPos)
	{
		if (!IsWhiteSpace(*currentPos))
			return;
		currentPos++;
		fCurrentOffset++;
	}
}

void CSmallTextFile::ReadLine(char *line)
{
	char *currentPos	= fBuffer->GetBuffer() + fCurrentOffset;
	char *endPos		= fBuffer->GetBuffer() + fBuffer->GetSize();
	UINT currentchar = 0;
	*line = '\0';
	while (currentPos < endPos)
	{
		switch (*currentPos)
		{
			case '\r':
				fCurrentOffset += 2; 				
				*line = '\0';
				return;
			default:
				currentchar = *currentPos;
				if (currentchar >= ' ')
					*line++ = *currentPos;
				currentPos++;
				fCurrentOffset++;
		}
	}
}

