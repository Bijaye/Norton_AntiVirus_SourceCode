// NAVTasksDlgsObj.h : Declaration of the CNAVTasksDlgs

#ifndef __NAVTASKSDLGS_H_
#define __NAVTASKSDLGS_H_

#include "resource.h"       // main symbols
#include "..\NavTasksRes\ResResource.h"

/////////////////////////////////////////////////////////////////////////////
// CNAVTasksDlgs
class ATL_NO_VTABLE CNAVTasksDlgs : public CComObjectRootEx<CComSingleThreadModel>
                                  , public CComCoClass<CNAVTasksDlgs, &CLSID_NAVTasksDlgs>
                                  , public ISupportErrorInfoImpl<&IID_INAVTasksDlgs>
                                  , public INAVTasksDlgs
{
public:
	CNAVTasksDlgs(void)
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_NAVTASKSDLGS)
DECLARE_NOT_AGGREGATABLE(CNAVTasksDlgs)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CNAVTasksDlgs)
	COM_INTERFACE_ENTRY(INAVTasksDlgs)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()

// INAVTasksDlgs
public:
	STDMETHOD(ShowFileListDlg)(VARIANT ScanTask);
	STDMETHOD(ShowDriveListDlg)(VARIANT ScanTask);
	STDMETHOD(ShowFolderListDlg)(HWND hWnd, VARIANT ScanTask, ESFLDO eShowFolderListDlgOptions);

private:
	static UINT_PTR CALLBACK OFNHookProc(HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam);
};

#endif //__NAVTASKSDLGS_H_
