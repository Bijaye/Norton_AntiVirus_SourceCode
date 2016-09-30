// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// QuarItem.h: interface for the CQuarItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUARITEM_H__0FA77146_E85D_11D2_A455_00A02438707D__INCLUDED_)
#define AFX_QUARITEM_H__0FA77146_E85D_11D2_A455_00A02438707D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define QUARITEM_STATE_QUARANTINED              1
#define QUARITEM_STATE_REPAIRABLE               2
#define QUARITEM_STATE_REPAIRED                 3
#define QUARITEM_STATE_NOTREPAIRED              4
#define QUARITEM_STATE_REPAIRED_FILE_IN_QUAR    5
#define QUARITEM_STATE_RESTORED_TO_ORIG_LOC     6
#define QUARITEM_STATE_NOT_RESTORED             7

class CQuarItem  
{
public:
	CQuarItem();
    CQuarItem(IVBin2* pIVBin2, VBININFO *pVBinInfo);
	virtual     ~CQuarItem();

    DWORD GetState();
	void  SetState(DWORD dwState);

	PEVENTBLOCK GetVBinEvent();
	BOOL        SetVBinEvent();

	BOOL        SetNewEvent(PEVENTBLOCK lpEventBlk);
	PEVENTBLOCK GetNewEvent();

	PVBININFO   GetVBinInfo();
	BOOL        SetVBinInfo(PVBININFO lpVBinInfo);

	BOOL        DeleteTempFile();
	DWORD       CreateTempFile(LPSTR* lpTempFile);
    BOOL        CreateMissingDir(LPTSTR lpFullPath);
	BOOL        ReplaceFile(LPTSTR lpTempFile, LPTSTR lpNewFile);
    LPCTSTR     GetDWHTemporaryFile();

    DWORD       SetItemToBackup();

    // Virtual repair and restore methods used by the various
    // darivations of this base class.
    virtual DWORD Repair( void )  = 0;
    virtual BOOL  Restore( BOOL bMoveToBackup ) = 0;
    virtual DWORD SaveToQuarantine( IVBinSession *pVBinSession = NULL, LPCTSTR szNewLoggingGUID = NULL ) = 0;

protected:

	DWORD       m_dwState;

	CString     m_szTempFile;
	IVBin2*     m_pVBin2;
	PEVENTBLOCK m_pVBinEventBlock;
	VBININFO    m_stVBinInfo;
	PEVENTBLOCK m_pNewEventBlock;
    char        m_szNewLogLine [MAX_LOG_LINE_SIZE];
};

#endif // !defined(AFX_QUARITEM_H__0FA77146_E85D_11D2_A455_00A02438707D__INCLUDED_)
