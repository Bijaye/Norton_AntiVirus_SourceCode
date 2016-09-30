// ResourceDialog.cpp : implementation file
//

// Class for displaying the resource information dialog. The same dialog is
// displayed for Add, Modify and Copy resource. The actual processing of the
// entered values are done in the DFLauncherView class
#include "stdafx.h"
#include "AVISDFRL.h"
#include "ResourceDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CString g_ModulePath;

/////////////////////////////////////////////////////////////////////////////
// CResourceDialog dialog


CResourceDialog::CResourceDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CResourceDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CResourceDialog)
	m_JobID = _T("");
	m_Program = _T("");
	m_Timeout = _T("");
	m_User = _T("");
	m_ntMachineName = _T("");
	m_ErrorCount = 0;
	m_TimeoutCount = 0;
	m_Machine = _T("");
	//}}AFX_DATA_INIT
}


void CResourceDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CResourceDialog)
	DDX_Control(pDX, IDC_COMBO_MACHINE, m_MachineCtrl);
	DDX_Control(pDX, IDC_STATIC_TIMEOUT_COUNT, m_TimeoutCountStaticCtrl);
	DDX_Control(pDX, IDC_STATIC_ERROR_COUNT, m_ErrorCountStaticCtrl);
	DDX_Control(pDX, IDC_EDIT_TIMEOUT_COUNT, m_TimeoutCountEditCtrl);
	DDX_Control(pDX, IDC_EDIT_ERROR_COUNT, m_ErrorCountEditCtrl);
	DDX_Control(pDX, IDC_BUTTON_TIMEOUT_COUNT_RESET, m_TimeoutCountResetBtn);
	DDX_Control(pDX, IDC_BUTTON_ERROR_COUNT_RESET, m_ErrorCountResetBtn);
	DDX_Control(pDX, IDC_STATIC_JOB_ID, m_JobIDPromptCtrl);
	DDX_Control(pDX, IDC_COMBO_JOBID, m_JobIDCtrl);
	DDX_CBString(pDX, IDC_COMBO_JOBID, m_JobID);
	DDX_Text(pDX, IDC_EDIT_PROGRAM, m_Program);
	DDX_Text(pDX, IDC_EDIT_TIMEOUT, m_Timeout);
	DDX_Text(pDX, IDC_EDIT_USERNAME, m_User);
	DDX_Text(pDX, IDC_EDIT_NTMACHINENAME, m_ntMachineName);
	DDX_Text(pDX, IDC_EDIT_ERROR_COUNT, m_ErrorCount);
	DDX_Text(pDX, IDC_EDIT_TIMEOUT_COUNT, m_TimeoutCount);
	DDX_CBString(pDX, IDC_COMBO_MACHINE, m_Machine);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CResourceDialog, CDialog)
	//{{AFX_MSG_MAP(CResourceDialog)
	ON_BN_CLICKED(IDC_BUTTON_ERROR_COUNT_RESET, OnButtonErrorCountReset)
	ON_BN_CLICKED(IDC_BUTTON_TIMEOUT_COUNT_RESET, OnButtonTimeoutCountReset)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResourceDialog message handlers

BOOL CResourceDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

    // Set the title of the dialog based on the mode for which the dialog
    // has been called
    if (m_DialogType == ADD_RESOURCE)
    {
        SetWindowText ("Add New Resource");
    }
    else
    if (m_DialogType == MODIFY_RESOURCE)
    {
        SetWindowText ("Modify Resource [" + m_JobID + "]");
        m_JobIDCtrl.EnableWindow(FALSE);
        m_JobIDPromptCtrl.EnableWindow(FALSE);

        m_ErrorCountEditCtrl.ShowWindow(SW_SHOW);
        m_ErrorCountStaticCtrl.ShowWindow(SW_SHOW);
        if (m_ErrorCount)
        {
            m_ErrorCountResetBtn.ShowWindow(SW_SHOW);
            m_ErrorCountResetBtn.EnableWindow(TRUE);
        }

        m_TimeoutCountEditCtrl.ShowWindow(SW_SHOW);
        m_TimeoutCountStaticCtrl.ShowWindow(SW_SHOW);
        if (m_TimeoutCount)
        {
            m_TimeoutCountResetBtn.ShowWindow(SW_SHOW);
            m_TimeoutCountResetBtn.EnableWindow(TRUE);
        }
    }
    else
    if (m_DialogType == COPY_RESOURCE)
    {
        SetWindowText ("Copy Resource [" + m_JobID + "]");
    }

    int resCount = m_pDFResourceList->GetSize();
    for (int i = 0; i < resCount; i++)
    {
	    CDFResource * pDFResource = (CDFResource *) (*m_pDFResourceList)[i];
        if (pDFResource)
        {
            // Create a single entry for each resource type
            if (CB_ERR == m_JobIDCtrl.FindStringExact(-1, pDFResource->m_JobIDString))
            {
                m_JobIDCtrl.AddString(pDFResource->m_JobIDString);
            }
            if (CB_ERR == m_MachineCtrl.FindStringExact(-1, pDFResource->m_IPAddress))
            {
                m_MachineCtrl.AddString(pDFResource->m_IPAddress);
            }
        }
    }

	CString evaluatorFileName;

	evaluatorFileName = g_ModulePath;
	evaluatorFileName += "AVISDFEV.DAT";

	FILE *fp;

	fp = fopen (evaluatorFileName, "r");
	if (fp != (FILE *) NULL)
	{
		char buf[1024];

		while (fgets (buf, sizeof(buf), fp))
		{
			char *resourceName = (char *) NULL;
			char *resourceType = (char *) NULL;
			char *ptr;

			int len = strlen (buf);
			for (int i = 0; i < len; i++)
			{
				if (buf[i] == '\r' || buf[i] == '\n')
					buf[i] = '\0';
			}
			len = strlen (buf);
			ptr = buf;
			while (*ptr == ' ' || *ptr == '\t')
				ptr ++;
			if (*ptr)
			{
				resourceName = ptr;
				while (*ptr && (*ptr != ' ' && *ptr != '\t'))
					ptr ++;

				if (*ptr)
				{
					*ptr = '\0';
					ptr++;
					while (*ptr == ' ' || *ptr == '\t')
						ptr ++;
					if (*ptr)
					{
						resourceType = ptr;
						while (*ptr && (*ptr != ' ' && *ptr != '\t'))
							ptr ++;

						if (*ptr)
						{
							*ptr = '\0';
							if (strlen (resourceName))
							{
								if (!stricmp(resourceType, "Processing") ||
									!stricmp(resourceType, "Scheduled"))
								{
									if (CB_ERR == m_JobIDCtrl.FindStringExact(-1, resourceName))
									{
										m_JobIDCtrl.AddString(resourceName);
									}
								}
							}
						}
					}
				}
			}
		}
		fclose (fp);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CResourceDialog::OnButtonErrorCountReset() 
{
    m_ErrorCount = 0;
    UpdateData(FALSE);
	
}

void CResourceDialog::OnButtonTimeoutCountReset() 
{
    m_TimeoutCount = 0;
    UpdateData(FALSE);
}
