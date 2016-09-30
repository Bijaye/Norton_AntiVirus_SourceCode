
#include "Stdafx.h"
#include <tlhelp32.h>
#include "OEHScan.h"
#include "OEHUIInterface.h"
#include "Quar32.h"

using namespace std;

COEHScan::COEHScan()
{
	m_pQuarantine = NULL;
	m_pActivityLog = NULL;
	m_dwClientPID = 0;
}

COEHScan::~COEHScan()
{
}

bool COEHScan::Open(bool bSilent,
					CQuarantine* pQuarantine,
					CActivityLog* pActivityLog,
					DWORD dwClientPID,
					const string& strClientImage,
					const string& strEmailSubject,
					const string& strEmailSender,
					const string& strEmailRecipient)
{
	bool bResult = true;

	try
	{
		if (m_pOEH)
		{
			m_pOEH->WHTerminate();
			m_pOEH.Release();
		}

        if(SYM_FAILED(OEHLoader.CreateObject(&m_pOEH)))
			throw runtime_error("Unable to load the OEH engine.\n");

		if (!m_pOEH->WHInit(dwClientPID, strClientImage.c_str()))
			throw runtime_error("Unable to initialize the OEH engine.\n");

		// Save the parameters that were passed in.

		m_bSilent = bSilent;
		m_pQuarantine = pQuarantine;
		m_pActivityLog = pActivityLog;

        if( m_pActivityLog == NULL )
            CCTRACEE("COEHScan::Open() - Activity log is NULL, no logging will be performed.");
            
		m_dwClientPID = dwClientPID;
		m_strClientImage = strClientImage;
		m_strEmailSubject = strEmailSubject;
		m_strEmailSender = strEmailSender;
		m_strEmailRecipient = strEmailRecipient;

		// Log the result.

		CCTRACEI("OEH engine initialized with: %s.", strClientImage.c_str());
	}
	catch(exception& Ex)
	{
		bResult = true;

		CCTRACEE(Ex.what());
	}

	return bResult;
}

void COEHScan::Close()
{
	if (m_pOEH)
	{
		m_pOEH->WHTerminate();
		m_pOEH.Release();
	}

	m_pQuarantine = NULL;
	m_pActivityLog = NULL;
	m_dwClientPID = 0;
	m_strClientImage.erase();
}

bool COEHScan::Check(const char* szFileName, bool& bAbortScan)
{
	
	if (!m_pOEH)
		return false;

	// Do we have a worm?

	DWORD dwWormPID = 0;
	char szWormPath[MAX_PATH];
	
	if (!m_pOEH->WHCheckMatch(szFileName, szWormPath, MAX_PATH, &dwWormPID))
		return true;

	// Yes we do! Ask the user what to do.
	// If we're in silent mode, perform an automatic quarantine.

	IOEHeurUI* pUI = NULL;
	OEHACTION Action = OEHACTION_QUARANTINE;

	if (m_bSilent == false)
	{
		if (CreateOEHeurUI(&pUI) == true)
		{
			pUI->SetProcessName(szWormPath);
			pUI->SetEmailInfo(m_strEmailSubject.c_str(),
				m_strEmailSender.c_str(), m_strEmailRecipient.c_str());
			pUI->Show(OEHUI_TYPE_INITIAL, &Action);
		}
	}

	// First see if we need to quarantine the worm.

	if (Action == OEHACTION_QUARANTINE)
	{										 
		// Abort the scan and discard the message.

		bAbortScan = true;

		// Terminate the worm process

		if( !TerminateProcess(dwWormPID) )
        {
            CCTRACEE(_T("COEHScan::Check() - Failed to terminate process ID %d. Worm Path = %s"), dwWormPID, szWormPath);
        }

		// Close the OEH engine so we can quarantine the worm.
		// Unfortunately the engine keeps the worm EXE open until you close it!

		m_pOEH->WHTerminate();
        m_pOEH.Release();

		// Add a short Pause so we can remove the file. Without a sleep, CreateFile will not allow us to
		// Open the file. 
		Sleep(500);

		// If the file is read only, remove the read only attributes
		DWORD dwAttribs = GetFileAttributes(szWormPath);
		if ((dwAttribs != INVALID_FILE_ATTRIBUTES) && (dwAttribs & FILE_ATTRIBUTE_READONLY))
		{
			SetFileAttributes(szWormPath, dwAttribs & ~( FILE_ATTRIBUTE_READONLY));
		}

		// Quarantine the worm.

		if (m_pQuarantine->Add(szWormPath, QFILE_TYPE_OEH, true, false) == false)
		{
			// Show the failure dialog.

			if (pUI != NULL)
				pUI->Show(OEHUI_TYPE_QUARANTINE_FAILED, &Action);

			// If we're in silent mode, we'll just block the message since we
			// were unable to quarantine the worm.

			if (m_bSilent == true)
				Action = OEHACTION_BLOCK;
		}
		else
		{
			// Show the completion dialog.

			if (pUI != NULL)
				pUI->Show(OEHUI_TYPE_QUARANTINE_SUCCEEDED, &Action);

			// Log the action.
            if( m_pActivityLog != NULL )
                m_pActivityLog->LogOEHInfection(szWormPath, AState_Quarantined);
        }

		// Restore File attributes if necessary
		if ((dwAttribs != INVALID_FILE_ATTRIBUTES) && (dwAttribs & FILE_ATTRIBUTE_READONLY))
		{
			SetFileAttributes(szWormPath, dwAttribs);
		}
	}
	
	// Perform any other type of action.

	if (Action == OEHACTION_ALLOW)
	{
		// Log the action.
        if( m_pActivityLog != NULL )
		    m_pActivityLog->LogOEHInfection(szWormPath, AState_Allowed);
	}
	else if (Action == OEHACTION_BLOCK)
	{
		// Abort the scan and discard the message.

		bAbortScan = true;

		// Log the action.
        if( m_pActivityLog != NULL )
		    m_pActivityLog->LogOEHInfection(szWormPath, AState_Blocked);
	}
	else if (Action == OEHACTION_AUTHORIZE)
	{
		// Authorize the executable.

		m_pOEH->WHAuthorizeFile(szWormPath);

		// Log the action.
        if( m_pActivityLog != NULL )
		    m_pActivityLog->LogOEHInfection(szWormPath, AState_Authorized);
	}
	else if (Action == OEHACTION_LAUNCH_QUARANTINE)
	{
		// Launch QCONSOLE.EXE if the user wants to open quarantine.

		ShellExecute(NULL, "open", "qconsole.exe", NULL, NULL, SW_SHOW);
	}

	// Clean-up.

	if (pUI != NULL)
		pUI->DeleteObject();

	return true;
}


bool COEHScan::TerminateProcess(DWORD dwPID)
{
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID);

	if (hProcess == NULL)
		return false;

	bool bResult = (::TerminateProcess(hProcess, 0) == TRUE ? true : false);

	CloseHandle(hProcess);

	return bResult;
}

