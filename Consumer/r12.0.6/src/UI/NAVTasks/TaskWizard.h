// TaskWizard.h: interface for the CTaskWizard class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TASKWIZARD_H__EA11AA07_F468_4435_B44B_2F81385ADB51__INCLUDED_)
#define AFX_TASKWIZARD_H__EA11AA07_F468_4435_B44B_2F81385ADB51__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Helper class for calling NAV Task Wizard
//

// From NAVTskWz.dll
typedef HRESULT (WINAPI *pfnNAVTaskWizard)(IScanTask*, int);
typedef HRESULT (WINAPI *pfnFolderDialog)(IScanTask*);
typedef HRESULT (WINAPI *pfnDrivesDialog)(IScanTask*);

class CTaskWizard  
{
public:
	CTaskWizard();
	virtual ~CTaskWizard();

    HRESULT NAVTaskWizard ( IScanTask* pNAVScanTask, int iMode );
    HRESULT FolderDialog ( IScanTask* pNAVScanTask );
    HRESULT DrivesDialog ( IScanTask* pNAVScanTask );

protected:
    HMODULE m_hmodNAVTaskWiz;

    pfnNAVTaskWizard m_pfnTaskWizard;
    pfnFolderDialog m_pfnFolderDialog;
    pfnDrivesDialog m_pfnDrivesDialog;

};

#endif // !defined(AFX_TASKWIZARD_H__EA11AA07_F468_4435_B44B_2F81385ADB51__INCLUDED_)
