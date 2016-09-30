
#ifndef OEH_SCAN_H
#define OEH_SCAN_H

#include "Quarantine.h"
#include "ActivityLog.h"
#include "ccModuleNames.h"
#include "OEHeurInterface.h"

class COEHScan
{
public:
	// Constructor.
	COEHScan();

	// Destructor.
	~COEHScan();

	// Opens (initializes) the OEH engine.
	bool Open(bool bSilent, CQuarantine* pQuarantine, CActivityLog* pActivityLog,
		DWORD dwClientPID, const ::std::string& strClientImage, const ::std::string& strEmailSubject,
		const ::std::string& strEmailSender, const ::std::string& strEmailRecipient);

	// Closes the OEH engine.
	void Close();

	// Performs an OEH check.
	bool Check(const char* szFileName, bool& bAbortScan);

private:
	// OEH engine handle.
    //typedef     CSymInterfaceManagedDLLHelper<&cc::sz_OEHeur_dll,
    //                            ccSym::CNAVPathProvider, 
    //                            cc::CSymInterfaceTrustedCacheMgdLoader,
    //                            IOEHeur, 
    //                            &IID_OEHeur, 
    //                            &IID_OEHeur> OEHLoader;
	CSymInterfaceDLLHelper<&cc::sz_OEHeur_dll,
                           ccSym::CNAVPathProvider, 
                           cc::CSymInterfaceTrustedCacheLoader,
                           IOEHeur, 
                           &IID_OEHeur, 
                           &IID_OEHeur> OEHLoader;

    IOEHeurPtr m_pOEH;

	// Silent mode (i.e. automatic quarantine/block)?
	bool m_bSilent;

	// Quarantine wrapper.
	CQuarantine* m_pQuarantine;

	// Activity log wrapper.
	CActivityLog* m_pActivityLog;

	// Client's process ID.
	DWORD m_dwClientPID;

	// Client image (i.e. path to the client executable).
	::std::string m_strClientImage;

	// Email subject.
	::std::string m_strEmailSubject;

	// Email sender.
	::std::string m_strEmailSender;

	// Email recipient.
	::std::string m_strEmailRecipient;
private:

	// Terminates the specified process by PID.
	bool TerminateProcess(DWORD dwPID);
};

#endif
