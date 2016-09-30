// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#include "stdafx.h"
#include "FileParser.h"
 

CFileSection::CFileSection(LPCSTR sFile,LPCSTR sSection)
:	CStdioFile(),
	m_dwPosition(0)
{
	SetCurrentSection(sSection);
	m_Error.m_cause = 0;
	m_Error.m_lOsError = 0;

	CStdioFile::Open(sFile,CFile::modeRead | CFile::typeText | CFile::shareDenyWrite,&m_Error);

	if (!m_sSection.IsEmpty())
		FindSection();
}



CFileSection::~CFileSection() 
{
}


void CFileSection::SetCurrentSection(CString sSection)
{
	if (!sSection.IsEmpty())
		m_sSection = BEGIN_SECTION + sSection + END_SECTION;
}


DWORD CFileSection::GetLastError()
{
	if (!m_Error.m_lOsError && m_Error.m_cause)
		return ERROR_OFFSET + m_Error.m_cause;

	return m_Error.m_lOsError;
}


BOOL CFileSection::GetValidLine(CString& sLine)
{
	try
	{
		while (ReadString(sLine))
 		{   
 			m_dwPosition = GetPosition();

			sLine.TrimLeft();
			sLine.TrimRight();

			if (!sLine.Find(SEMICOLON))
				sLine.Empty();

			if (!sLine.IsEmpty())
				return TRUE;
		}
	}

	catch (CFileException *exception)
	{
		m_Error.m_cause = exception->m_cause;
		m_Error.m_lOsError = exception->m_lOsError;
		m_Error.m_strFileName = exception->m_strFileName;
	}
	return FALSE;
}


BOOL CFileSection::FindSection()
{
	if (!m_pStream)
		return FALSE;

	try
	{
		CString sLine;
		SeekToBegin();
	
		while (GetValidLine(sLine))
		{
 			if (IsSection(sLine) && !sLine.CompareNoCase(m_sSection))
 			{    
				m_dwPosition = GetPosition();
 	 			return TRUE;
			}
 		} 
		AfxThrowFileException(CFileException::none,ERROR_HANDLE_EOF,CFile::GetFileName());
	}


	catch (CFileException *exception)
	{
		m_Error.m_cause = exception->m_cause;
		m_Error.m_lOsError = exception->m_lOsError;
		m_Error.m_strFileName = exception->m_strFileName;
	}
	
	return FALSE;
}

BOOL CFileSection::IsSection(CString& sSection)
{
	int rpos = sSection.Find(END_SECTION);
	if (rpos != -1)
		sSection = sSection.Left(rpos+1);

	return sSection.Find(BEGIN_SECTION) != -1 && rpos != -1;
}



BOOL CFileSection::FindFirstLine(CString& sLine,CString sSection)
{
	SetCurrentSection(sSection);

	if (!FindSection() || !GetValidLine(sLine) || IsSection(sLine))
 	{
		m_dwPosition = 0;
		sLine.Empty();
		return FALSE;
	}
 	return TRUE;
}

BOOL CFileSection::FindNextLine(CString& sLine)
{
	if (!m_dwPosition) 
		return FALSE;

 	if (!GetValidLine(sLine) || IsSection(sLine))
 	{
		m_dwPosition = 0;
		sLine.Empty();
		return FALSE;
	}
 	return TRUE;
}


BOOL CFileSection::FindNextMatchLine(CString& sLine,CString sSection)
{
	CString sKey,sValue(sLine);

	if (sValue.IsEmpty())
		return FALSE;

	if (!sSection.IsEmpty() && FindFirstLine(sLine,sSection))
	{
		do
		{  
			if (GetStringField(sLine,0,sKey) && !sValue.CompareNoCase(sKey))
				return TRUE;
		} while (FindNextLine(sLine));
	}
	else if (sSection.IsEmpty())
	{
		while (FindNextLine(sLine))
		{
			if (GetStringField(sLine,0,sKey) && !sValue.CompareNoCase(sKey))
				return TRUE;
		}
	}

	return FALSE;
}


DWORD CFileSection::GetLineCount(CString sSection)
{
	DWORD dwCount = 0;
	CString sLine;

	if (FindFirstLine(sLine,sSection))
	{
		do
		{
			dwCount++;
		} while (FindNextLine(sLine));
	}
	return dwCount;
}


BOOL CFileSection::GetLineByIndex(DWORD dwIndex,CString& sLine,CString sSection)
{
	if (FindFirstLine(sLine,sSection))
	{
		for (DWORD i=0; i<dwIndex; i++)
		{
			if (!FindNextLine(sLine))
				return FALSE;
		}
		return TRUE;
	}
	return FALSE;
}


BOOL CFileSection::GetStringField(CString sLine,DWORD dwIndex,CString& sField)
{
	int pos, quote1, quote2, pos2; 
	sField.Empty();
		       
	for (DWORD idx=0; idx <= dwIndex; idx++)
	{
		pos = sLine.FindOneOf(SPACER_CHARSET);
		if (pos != -1)
		{  
			// if data is inside of quotes, set pos to appropriate location
			quote1 = sLine.FindOneOf(QUOTE);
			if (quote1 != -1 && quote1 < pos)
			{
				CString sTemp(sLine.Right(sLine.GetLength()-quote1-1));
				quote2 = sTemp.FindOneOf(QUOTE);
				if (quote2 != -1 && quote2 > pos)
				{
					sTemp = sTemp.Right(sTemp.GetLength()-quote2-1);
					pos2 = sTemp.FindOneOf(SPACER_CHARSET);
					if (pos2 != -1)
						pos = pos2+quote2+quote1+2;
					else
						pos = sLine.GetLength();
				}
			}
			if (idx != dwIndex)
				sLine = sLine.Right(sLine.GetLength()-pos-1);
			else
				sLine = sLine.Left(pos);
		}
		else if (idx != dwIndex)
			return FALSE;
	}
	sField = sLine;

	return TRUE;
}


BOOL CFileSection::GetIntField(CString sLine,DWORD dwIndex,DWORD& dwField)    
{
   CString sTemp;
   if (GetStringField(sLine,dwIndex,sTemp))
   {
    	dwField = atol(sTemp);
    	return TRUE;	
   }
   return FALSE;
}


BOOL CFileSection::AddSection(CStringArray *pArray,CString sSection,CString *pPrepend)
{
	CString sLine;

	SetCurrentSection(sSection);
	if (!pArray || !FindSection())
		return FALSE;

	while (GetValidLine(sLine) && !IsSection(sLine))
	{
		if (!pPrepend)
			pArray->Add(sLine);
		else
			pArray->Add(*pPrepend + sLine);
	}
 	return TRUE;
}


BOOL CFileSection::RemoveSection(CStringArray *pArray,CString sSection,CString sPath,FARPROC pErrProc)
{
	CString sLine;

	SetCurrentSection(sSection);
	if (!pArray || !FindSection())
		return FALSE;

	while (GetValidLine(sLine) && !IsSection(sLine))
		RemoveLine(pArray,sLine);
 	return TRUE;
}

void CFileSection::RemoveLine(CStringArray *pArray, CString sLine)
{
	for (int idx = pArray->GetSize()-1; idx>=0; idx--)
	{
		if (!sLine.CompareNoCase(pArray->GetAt(idx)))
		{
			pArray->RemoveAt(idx);
			break;
		}
	}
}


BOOL CFileSection::GetLine(CString& sLine)
{
	if (!m_pStream)
		return FALSE;

	sLine.Empty();
	try
	{

		if (!m_dwPosition)
			SeekToBegin();

		if (!ReadString(sLine))
			return FALSE;

		m_dwPosition = GetPosition();
	}

	catch (CFileException *exception)
	{
		m_Error.m_cause = exception->m_cause;
		m_Error.m_lOsError = exception->m_lOsError;
		m_Error.m_strFileName = exception->m_strFileName;
		return FALSE;
	}
	return TRUE;
}



