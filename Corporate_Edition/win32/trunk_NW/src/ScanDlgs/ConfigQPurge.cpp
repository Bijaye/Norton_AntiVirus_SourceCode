// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// ConfigQPurge.cpp : implementation file
//

#include "stdafx.h"
#include "scandlgs.h"
#include "ConfigQPurge.h"
#include "ClientReg.h"
#include "SymSaferRegistry.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define szLogFrequency "LogFrequency"

/////////////////////////////////////////////////////////////////////////////
// CConfigQPurge dialog


CConfigQPurge::CConfigQPurge(CWnd* pParent /*=NULL*/)
: CDialog(CConfigQPurge::IDD, pParent), m_pConfig(NULL)
{
	//{{AFX_DATA_INIT(CConfigQPurge)
	m_EnablePurgingFlags = FALSE;
	//}}AFX_DATA_INIT

	m_bManualChange = FALSE;
	m_pConfig2=NULL;
}


void CConfigQPurge::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConfigQPurge)
	DDX_Control(pDX, IDC_TYPE, m_ctlType);
	DDX_Control(pDX, IDC_SPIN2, m_ctlSpin);
	DDX_Control(pDX, IDC_VALUE, m_ctlEdit);
	DDX_Check(pDX, IDC_CHECK_ENABLE_QPURGE, m_EnablePurgingFlags);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConfigQPurge, CDialog)
	//{{AFX_MSG_MAP(CConfigQPurge)
	ON_EN_KILLFOCUS(IDC_VALUE, OnKillfocusValue)
	ON_BN_CLICKED(CONFIG_HISTORY_HELP, OnHistoryHelp)
	ON_BN_CLICKED(IDC_CHECK_ENABLE_QPURGE, OnCheckEnableQpurge)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConfigQPurge message handlers

BOOL CConfigQPurge::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CComboBox* pType = (CComboBox*)GetDlgItem(IDC_TYPE);

	//Add extra initialization here

	// populte the frequency type
	CString sText;
	sText.LoadString(IDS_LOG_DAYS);
	pType->AddString(sText);
	sText.LoadString(IDS_LOG_MONTHS);
	pType->AddString(sText);
	sText.LoadString(IDS_LOG_YEARS);
	pType->AddString(sText);
	

	// set the frequencly value from the registry
	long val=30;
	long ifreq=0;
	double dval=0;


//////////Get settings


    DWORD   dwRegKeyType;
    DWORD   dwRetValue = ERROR_NO_KEY;
	HKEY	pKey;
    char    szQuarRegKey[IMAX_PATH] = REGHEADER;
	DWORD dwOptionBufferSize = 0;

    // Build up the reg key string
    strcat(szQuarRegKey, "\\");
    strcat(szQuarRegKey, szReg_Key_Quarantine);

	if( ERROR_SUCCESS == RegOpenKeyEx(	HKEY_LOCAL_MACHINE,
										szQuarRegKey,
										0,
										KEY_READ,
										&pKey ) )
	{
		LPTSTR szEnabled = NULL;
		LPTSTR szAge = NULL;
		LPTSTR szFreq = NULL;

		switch( m_QuarantineType )
		{
		case RV_TYPE_VIRUS_BIN:
			szEnabled = szReg_Val_QuarantinePurgeEnabled;
			szAge = szReg_Val_QuarantinePurgeAgeLimit;
			szFreq = szReg_Val_QuarantinePurgeFrequency;
			break;
		case RV_TYPE_BACKUP:
			szEnabled = szReg_Val_BackupItemPurgeEnabled;
			szAge = szReg_Val_BackupItemPurgeAgeLimit;
			szFreq = szReg_Val_BackupItemPurgeFrequency;
			break;
		case RV_TYPE_REPAIR:
			szEnabled = szReg_Val_RepairedItemPurgeEnabled;
			szAge = szReg_Val_RepairedItemPurgeAgeLimit;
			szFreq = szReg_Val_RepairedItemPurgeFrequency;
		}

		if( szEnabled != NULL )
		{
			dwRegKeyType = REG_DWORD;
			dwOptionBufferSize = sizeof(DWORD);

			dwRetValue = SymSaferRegQueryValueEx( pKey,
							 szEnabled,
							 NULL,
							 &dwRegKeyType,
							 (BYTE*)&m_EnablePurgingFlags,
							 &dwOptionBufferSize );

		}

		if( szAge != NULL )
		{
			dwRegKeyType = REG_DWORD;
			dwOptionBufferSize = sizeof(DWORD);

			dwRetValue = SymSaferRegQueryValueEx( pKey,
							 szAge,
							 NULL,
							 &dwRegKeyType,
							 (BYTE*)&val,
							 &dwOptionBufferSize );
		}

		if( szFreq != NULL )
		{
			dwRegKeyType = REG_DWORD;
			dwOptionBufferSize = sizeof(DWORD);

			dwRetValue = SymSaferRegQueryValueEx( pKey,
							 szFreq,
							 NULL,
							 &dwRegKeyType,
							 (BYTE*)&ifreq,
							 &dwOptionBufferSize );
		}

		RegCloseKey( pKey );
	}


/////////////////////////


	// convert frequency to days
	if (ifreq==1)
	{
		dval = ((double)val/30.4375); // convert months
		
	}
	// convert frequncy to years
	else if (ifreq==2)
	{
		dval = ((double)val/365.25);  // convert years
	}
	// frequency is alreay in days 
	else
		dval = val;

	pType->SetCurSel(ifreq);

	// round the number
	if (dval - long(dval) > 0.5) 
		val = (long)dval+1;
	else
		val = (long)dval;
		
	
	sText.Format("%d",val);
	m_ctlEdit.LimitText(4);
	m_ctlEdit.SetWindowText(sText);
	m_ctlSpin.SetRange(1,9999);
	m_ctlSpin.SetPos(val);
	
	m_ctlEdit.EnableWindow( m_EnablePurgingFlags );
	m_ctlSpin.EnableWindow( m_EnablePurgingFlags );
	m_ctlType.EnableWindow( m_EnablePurgingFlags );
	UpdateData(FALSE);

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CConfigQPurge::OnOK() 
{
	//get frequency type
	CComboBox* pType = (CComboBox*)GetDlgItem(IDC_TYPE);
	// get frequency value
	CWnd *pNum = (CEdit*)GetDlgItem(IDC_VALUE);
	CString sText;
	
	pNum->GetWindowText(sText);
	DWORD dwDays = atol(sText);
	DWORD ifreq= pType->GetCurSel();

	// convert frequency to days
	if (ifreq==1)
		dwDays = (DWORD)((double)dwDays*30.4375); // convert months
	else if (ifreq==2)
		dwDays = (DWORD)((double)dwDays*365.25);  // convert years
	//else frequency is already in day no need to convert

	
//////////// save out to option


    DWORD   dwRegKeyType;
    DWORD   dwRetValue = ERROR_NO_KEY;
	HKEY	pKey;
    char    szQuarRegKey[IMAX_PATH] = REGHEADER;
	DWORD dwOptionBufferSize = 0;


    // Build up the reg key string
    strcat(szQuarRegKey, "\\");
    strcat(szQuarRegKey, szReg_Key_Quarantine);

	if( ERROR_SUCCESS == RegOpenKeyEx(	HKEY_LOCAL_MACHINE,
										szQuarRegKey,
										0,
										KEY_WRITE,
										&pKey ) )
	{
        dwRegKeyType = REG_DWORD;
		dwOptionBufferSize = sizeof(DWORD);

		switch( m_QuarantineType )
		{
		case RV_TYPE_VIRUS_BIN:
			dwRetValue = RegSetValueEx( pKey,
							 szReg_Val_QuarantinePurgeEnabled,
							 NULL,
							 dwRegKeyType,
							 (BYTE*)&m_EnablePurgingFlags,
							 dwOptionBufferSize );


			if( m_EnablePurgingFlags )
			{
				dwRegKeyType = REG_DWORD;
				dwOptionBufferSize = sizeof(DWORD);

				dwRetValue = RegSetValueEx( pKey,
								 szReg_Val_QuarantinePurgeAgeLimit,
								 NULL,
								 dwRegKeyType,
								 (BYTE*)&dwDays,
								 dwOptionBufferSize );


				dwRegKeyType = REG_DWORD;
				dwOptionBufferSize = sizeof(DWORD);

				dwRetValue = RegSetValueEx( pKey,
								 szReg_Val_QuarantinePurgeFrequency,
								 NULL,
								 dwRegKeyType,
								 (BYTE*)&ifreq,
								 dwOptionBufferSize );
			}		
			break;
		case RV_TYPE_BACKUP:
			dwRetValue = RegSetValueEx( pKey,
							 szReg_Val_BackupItemPurgeEnabled,
							 NULL,
							 dwRegKeyType,
							 (BYTE*)&m_EnablePurgingFlags,
							 dwOptionBufferSize );

			if( m_EnablePurgingFlags )
			{
				dwRegKeyType = REG_DWORD;
				dwOptionBufferSize = sizeof(DWORD);

				dwRetValue = RegSetValueEx( pKey,
								 szReg_Val_BackupItemPurgeAgeLimit,
								 NULL,
								 dwRegKeyType,
								 (BYTE*)&dwDays,
								 dwOptionBufferSize );


				dwRegKeyType = REG_DWORD;
				dwOptionBufferSize = sizeof(DWORD);

				dwRetValue = RegSetValueEx( pKey,
								 szReg_Val_BackupItemPurgeFrequency,
								 NULL,
								 dwRegKeyType,
								 (BYTE*)&ifreq,
								 dwOptionBufferSize );
			}


			break;
		case RV_TYPE_REPAIR:
			dwRetValue = RegSetValueEx( pKey,
							 szReg_Val_RepairedItemPurgeEnabled,
							 NULL,
							 dwRegKeyType,
							 (BYTE*)&m_EnablePurgingFlags,
							 dwOptionBufferSize );

			if( m_EnablePurgingFlags )
			{
				dwRegKeyType = REG_DWORD;
				dwOptionBufferSize = sizeof(DWORD);

				dwRetValue = RegSetValueEx( pKey,
								 szReg_Val_RepairedItemPurgeAgeLimit,
								 NULL,
								 dwRegKeyType,
								 (BYTE*)&dwDays,
								 dwOptionBufferSize );


				dwRegKeyType = REG_DWORD;
				dwOptionBufferSize = sizeof(DWORD);

				dwRetValue = RegSetValueEx( pKey,
								 szReg_Val_RepairedItemPurgeFrequency,
								 NULL,
								 dwRegKeyType,
								 (BYTE*)&ifreq,
								 dwOptionBufferSize );
			}

		}

		RegCloseKey( pKey );
	}

/////////////////////////

	CDialog::OnOK();
}

void CConfigQPurge::OnKillfocusValue() 
{
	m_ctlSpin.SetPos(LOWORD( m_ctlSpin.GetPos() ));
}

void CConfigQPurge::OnHistoryHelp() 
{
	AfxGetApp()->WinHelpInternal( IDD );
}

void CConfigQPurge::OnCheckEnableQpurge() 
{
	m_EnablePurgingFlags = (!m_EnablePurgingFlags);
	m_ctlEdit.EnableWindow( m_EnablePurgingFlags );
	m_ctlSpin.EnableWindow( m_EnablePurgingFlags );
	m_ctlType.EnableWindow( m_EnablePurgingFlags );
	
}
