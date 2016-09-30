// OEHeurUI.h: interface for the COEHeurUI class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OEHEURUI_H__970EA803_605C_481E_B083_6E93EF0CA35D__INCLUDED_)
#define AFX_OEHEURUI_H__970EA803_605C_481E_B083_6E93EF0CA35D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OEHUIInterface.h"

class COEHeurUI : public IOEHeurUI, 
				  public ISymBaseImpl<CSymThreadSafeRefCount>
{
public:
	SYM_INTERFACE_MAP_BEGIN()                
		SYM_INTERFACE_ENTRY( IID_IOEHeurUI, IOEHeurUI ) 
	SYM_INTERFACE_MAP_END()  
	
	COEHeurUI();
	virtual ~COEHeurUI();

	// IOEHeurUI overrides.
	void SetProcessName(const char* szProcessName);
	void SetEmailInfo(const char* szSubject, const char* szSender, 
		const char* szRecipient);
	bool Show(OEHUI_TYPE UIType, OEHACTION* pAction = NULL);
	void DeleteObject();

private:
	// Process Name.
	::std::string m_strProcessName;

	// Email sender.
	::std::string m_strSender;

	// Email recipient(s).
	::std::string m_strRecipient;

	// Email subject.
	::std::string m_strSubject;
};

#endif // !defined(AFX_OEHEURUI_H__970EA803_605C_481E_B083_6E93EF0CA35D__INCLUDED_)
