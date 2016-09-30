// DefsManager.h: interface for the CDefsManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEFSMANAGER_H__79C51509_5ACD_41BA_9AFB_42D8FA7179CE__INCLUDED_)
#define AFX_DEFSMANAGER_H__79C51509_5ACD_41BA_9AFB_42D8FA7179CE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDefsManager  
{
public:
	CDefsManager();
	virtual ~CDefsManager();

	void Initialize();
    // bUseDefs : if this set of defs is the one we are going to use or if we are just
    //            adjusting the App ID for AP kernel.
    //
    // szTestDefs : if this is not NULL we will turn off def authentication and use
    //              these defs.
    //
    // bAuthenticate : Authenticate the defs (hash)
    //
	void SwitchToLatestDefs( LPCSTR szAppID,
                             bool bUseDefs = true,
                             const char* szTestDefs = NULL,
                             bool bAuthenticate = true);
    bool GetRootDefsDirectory (LPTSTR pszDefsDir, int nMaxCount);
	
protected:	
    void updateDefsServicePaths( std::string& sDefsDir );

	
private:
	// Critical section for defutils object.
    ccLib::CCriticalSection	m_csDefUtils;
};

#endif // !defined(AFX_DEFSMANAGER_H__79C51509_5ACD_41BA_9AFB_42D8FA7179CE__INCLUDED_)
