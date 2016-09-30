// Copyright 1991-1997 Symantec Corporation, Peter Norton Product Group
//************************************************************************
//  $Header:   S:/IAL/VCS/perfcntr.cpv   1.1   25 Jun 1997 17:23:34   DSACKIN  $
//
//  Description:
//      This file contains performance counter functions.
//
//************************************************************************
//
//  $Log:   S:/IAL/VCS/perfcntr.cpv  $
// 
//    Rev 1.1   25 Jun 1997 17:23:34   DSACKIN
// Ported from Marconi
// 
//    Rev 1.1   24 Apr 1997 11:00:56   pvenkat
// No difference.
// 
//
//************************************************************************
// NOTE:    This is from Matt Pietrek's Under the Hood column of MSJ
//************************************************************************

#ifndef	WIN32_LEAN_AND_MEAN
	#define	WIN32_LEAN_AND_MEAN
#endif

#include	<windows.h>
#include	<winperf.h>
#include	<stdlib.h>
#include	<malloc.h>
#include	<tchar.h>

#pragma		hdrstop

#include	"perfcntr.h"


CPerfCounter::CPerfCounter (PTSTR const pszName, DWORD type, PBYTE const pData, DWORD cbData)
{
	m_pszName = _tcsdup (pszName);
	m_type = type;
	m_cbData = cbData;
	m_pData = new BYTE [m_cbData];
	memcpy (m_pData, pData, m_cbData);
}

CPerfCounter::~CPerfCounter (void)
{
	free (m_pszName);
	delete [] m_pData;
}

BOOL
CPerfCounter::GetData (PBYTE pBuffer, DWORD cbBuffer, DWORD *pType)
{
	if (cbBuffer < m_cbData) // Make sure the buffer is big enough
		return FALSE;

	memcpy (pBuffer, m_pData, m_cbData);	// copy the data
	if (pType)
		*pType = m_type;

	return TRUE;
}

BOOL
CPerfCounter::Format (PTSTR pszBuffer, DWORD nSize, BOOL fHex)
{
	// Do better formatting!! Check Length!!!

	LPTSTR	pszPrefix = TEXT ("");
	TCHAR	szTemp [512];

	// First, ascertain the basic type (number, counter, text, or zero)
	switch (m_type & 0x00000C00)
	{
		case	PERF_TYPE_ZERO:
			wsprintf (pszBuffer, TEXT ("ZERO"));
			return TRUE;

		case	PERF_TYPE_TEXT:
			wsprintf (pszBuffer, TEXT ("Text Counter"));
			return TRUE;

		case	PERF_TYPE_COUNTER:
		{
			switch (m_type & 0x00070000)
			{
				case	PERF_COUNTER_RATE:
					pszPrefix = TEXT ("Counter Rate");
					break;
				case	PERF_COUNTER_FRACTION:
					pszPrefix = TEXT ("Counter Fraction");
					break;
				case	PERF_COUNTER_BASE:
					pszPrefix = TEXT ("Counter Base");
					break;
				case	PERF_COUNTER_ELAPSED:
					pszPrefix = TEXT ("Counter ELAPSED");
					break;
				case	PERF_COUNTER_QUEUELEN:
					pszPrefix = TEXT ("Counter QUEUELEN");
					break;
				case	PERF_COUNTER_HISTOGRAM:
					pszPrefix = TEXT ("Counter Histogram");
					break;
				default:
					pszPrefix = TEXT ("Counter value");
					break;
			}
		}
	}

	PTSTR pszFmt = fHex ? TEXT ("%s%Xh") : TEXT ("%s%u");

	switch (m_cbData)
	{
		case	1:
			wsprintf (szTemp, pszFmt, pszPrefix, *(PBYTE) m_pData);
			break;
		case	2:
			wsprintf (szTemp, pszFmt, pszPrefix, *(PWORD) m_pData);
			break;
		case	4:
			wsprintf (szTemp, pszFmt, pszPrefix, *(PDWORD) m_pData);
			break;
		case	8:	// Danger ! Assumes little-ending (x86) byte ordering
			wsprintf (szTemp, 
					TEXT ("%s%X%X"), 
					pszPrefix, 
					*(PDWORD) (m_pData + 4),
					*(PDWORD) m_pData);
			break;
		default:
			wsprintf (szTemp, TEXT ("Unhandled size %u"), m_cbData);
			break;

	}

	switch (m_type & 0x70000000)
	{
		case	PERF_DISPLAY_SECONDS:
			_tcscat (szTemp, TEXT (" secs"));
			break;
		case	PERF_DISPLAY_PERCENT:
			_tcscat (szTemp, TEXT (" %%"));
			break;
		case	PERF_DISPLAY_PER_SEC:
			_tcscat (szTemp, TEXT (" /secs"));
			break;
	}
	lstrcpyn (pszBuffer, szTemp, nSize);
	return TRUE;
}
