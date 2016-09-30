// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.

#ifndef _FILEPARSER_H
#define _FILEPARSER_H

#define ERROR_OFFSET				5000

#define TMP_EXT						"tmp"

#define EMPTY						""
#define BSLASH						"\\"
#define QUOTE						"\""
#define WILDCARD					"*"
#define SEMICOLON					";"
#define SPACER_CHARSET				"=,"
#define FILENAME_DOT				"."
#define BEGIN_SECTION				"["
#define END_SECTION					"]"
#define ENDLINE_MARKER				"\n"



// CFileSection Class
class CFileSection : protected CStdioFile
{
public: 
// Construction
	CFileSection(LPCSTR sFile,LPCSTR sSection=NULL);
	~CFileSection();

// Traditional Functions
   	BOOL FindFirstLine(CString& sLine,CString sSection=EMPTY);
   	BOOL FindNextLine(CString& sLine);
   	BOOL FindNextMatchLine(CString& sLine,CString sSection=EMPTY);
   	DWORD GetLineCount(CString sSection=EMPTY);  
   	BOOL GetLineByIndex(DWORD dwIndex,CString& sLine,CString sSection=EMPTY);

// CStringArray Functions
	BOOL AddSection(CStringArray *pArray,CString sSection=EMPTY,CString *pPrepend=NULL);
	BOOL RemoveSection(CStringArray *pArray,CString sSection=EMPTY,CString sPath=EMPTY,FARPROC pErrProc=NULL);

// Static Member Functions
   	static BOOL GetStringField(CString sLine,DWORD dwIndex,CString& sField);
   	static BOOL GetIntField(CString sLine,DWORD dwIndex,DWORD& dwField);

// Special Use Functions
	void SetCurrentSection(CString sSection);
   	DWORD GetLastError();
	BOOL GetLine(CString& sLine);

// Class Data Members
protected: 
	CString m_sSection;
	DWORD m_dwPosition;
	CFileException m_Error;
	BOOL IsSection(CString& sSection);
	BOOL FindSection();
	BOOL GetValidLine(CString& sLine);
	void RemoveLine(CStringArray *pArray, CString sLine);
};

#endif
