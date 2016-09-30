// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
///////////////////////////////////////////////////////////////////////////////////
//
//
//
//
#include "stdafx.h"
#include "ItemStorage.h"


CItemStorage::CItemStorage()
{
    m_bOpen     = FALSE;
    m_bChanged  = FALSE;
    m_sCurrentPath.Empty();
}

CItemStorage::~CItemStorage()
{
	POSITION pos;
	CString sKey,
			sFile;

    // Close our temp file
    Close();

	for( pos = m_map.GetStartPosition(); pos != NULL; )    
	{
		m_map.GetNextAssoc( pos, sKey,sFile );
		CFile::Remove(sFile);
	}
	m_map.RemoveAll();
}

void CItemStorage::Close()
{
    if(m_bOpen)
        {
        m_file.Close();
        m_bOpen = FALSE;
        m_sCurrentPath.Empty();
        }
}

void CItemStorage::Flush()
{
    if(m_bOpen)
        {
        m_file.Flush();
        m_bChanged=FALSE;
        }
}

BOOL CItemStorage::AddToStorage(LPCSTR strLogLine)
{

	CResultItem		Item(strLogLine);
	CString sKey  = GetKey(Item);
	CString			sFile;
	BOOL			bOk=TRUE;

	if (!Item.IsFilterType(CResultItem::fInfection)) 
        return FALSE;

	// the key has not been mapped to a file
	if (!m_map.Lookup(sKey,sFile))
	{
		CString sTempPath;
		
		GetTempPath(MAX_PATH,sTempPath.GetBuffer(MAX_PATH));
		sTempPath.ReleaseBuffer();
		GetTempFileName(sTempPath,"LVP",0,sFile.GetBuffer(MAX_PATH));
		sFile.ReleaseBuffer();
		m_map[sKey]=sFile;

        m_sCurrentPath.Empty();
	}; 

    // If temp file is open and were changing to a new temp file close the old one
    if(m_sCurrentPath != sFile)
        {
        // Close our temp file
        Close();

        // Reset the current path so that we will know when we change log files
        m_sCurrentPath = sFile;
        }

    // Open the storage file if needed
    if(!m_bOpen)
	    m_bOpen = m_file.Open(sFile,CFile::modeWrite|CFile::shareDenyNone);

	// Write the log line at the end
	if (m_bOpen)
	{
		m_file.SeekToEnd();
		m_file.WriteString(CString(strLogLine)+"\r\n");
        m_bChanged=TRUE;
	}
	
	return bOk;
}


BOOL CItemStorage::LoadStorage(LPCSTR sKey, CStringList& stringList)
{
	CStdioFile  stdFile;
	CString		sFile,
				strLogLine;

    // Close off our current temp storage file before loading a new one
    Close();
    
    stringList.RemoveAll();
	
	if (m_map.Lookup(sKey,sFile))
	{
		stdFile.Open(sFile,CFile::modeRead|CFile::shareDenyNone);
		while (stdFile.ReadString(strLogLine))
            stringList.AddTail(strLogLine);

		stdFile.Close();
		return TRUE;
	}
	return FALSE;
}

CString CItemStorage::GetKey(CResultItem& Item)
{
	CString sName;
	DWORD scanid;

	Item.GetComputerName(sName);
	Item.GetScanID(scanid);
	return GetKey(sName,scanid);
	
}

CString CItemStorage::GetKey(LPCSTR sComputer,DWORD id)
{
	CString sId;
	itoa(id,sId.GetBuffer(40),10);
	sId.ReleaseBuffer();
	return sComputer + sId;

}