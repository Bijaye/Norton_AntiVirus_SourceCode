// PostCfgWizTasks.h: interface for the PostCfgWizTasks class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CPOSTCFGWIZTASKS_H__9FB8CCD4_C851_43F1_9307_DF17FE824B23__INCLUDED_)
#define AFX_CPOSTCFGWIZTASKS_H__9FB8CCD4_C851_43F1_9307_DF17FE824B23__INCLUDED_



const TCHAR g_cszNAVCfgWizRegKey[] = _T("Software\\Symantec\\Norton AntiVirus\\CfgWiz");

class CPostCfgWizTasks  
{
public:
	CPostCfgWizTasks();
	virtual ~CPostCfgWizTasks();
	void	CreateSystemRestorePoint();

	HRESULT RunTasks();
};

#endif // !defined(AFX_CPOSTCFGWIZTASKS_H__9FB8CCD4_C851_43F1_9307_DF17FE824B23__INCLUDED_)
