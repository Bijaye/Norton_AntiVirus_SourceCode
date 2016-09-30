// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#ifndef __SMALLTXT_H
#define __SMALLTXT_H

#ifndef WIN32
#ifndef LPTSTR
#define LPTSTR char FAR *
#endif
#ifndef LPCTSTR
#define LPCTSTR const char FAR *
#endif
#ifndef TCHAR
#define TCHAR char
#endif
#endif

class CBuffer
{
public:
	enum CBufEnums { DEFAULT_BUFFER_SIZE = (int)0xFFFF };
	CBuffer(DWORD size = (DWORD)DEFAULT_BUFFER_SIZE);
	~CBuffer();
	char *GetBuffer() {return fBufferPointer; }
	DWORD GetSize() {return fSize; }
private:
	DWORD fSize;
	char *fBufferPointer;
};

// this class defines a text file of less than 64K
class CSmallTextFile
{
public:
	CSmallTextFile( LPCTSTR fileName );
	~CSmallTextFile();

	void ReadLine(char *line);
	void SkipWhiteSpace();
	void Rewind() { fCurrentOffset = 0; }
#ifdef WIN32
	BOOL IsOpen() { return fFile != INVALID_HANDLE_VALUE; }
#else
	BOOL IsOpen() { return fFile != HFILE_ERROR; }
#endif
	BOOL IsOK() {return fOK; }
	BOOL IsEOF() { return fBuffer->GetSize() <= fCurrentOffset; }
	BOOL IsWhiteSpace(char c) { return c <= ' '; } 
	DWORD GetSize() { return fBuffer->GetSize(); }

private:
	CBuffer 	*fBuffer;
	DWORD 		fCurrentOffset;
#ifdef WIN32
	HANDLE		fFile;
#else
	HFILE		fFile;
	OFSTRUCT	fOfStruct;
#endif
	BOOL		fOK;
};

#endif

