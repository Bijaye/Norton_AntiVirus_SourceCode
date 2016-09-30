// CCFRAT.h: interface of FRAT class
//
//////////////////////////////////////////////////////////////////////

//constants
const char WIN9X_AUDITMODE_KEY[]	= "Software\\Microsoft\\Windows\\CurrentVersion";
const char WINNT_FRAT_KEY[]			= "SYSTEM\\Setup";
const char AUDIT_MODE_VALUE[]		= "AuditInProgress";
const char FACTORY_MODE_VALUE[]		= "FactoryPreInstallInProgress";
const char FACTORY_RESEAL_VALUE[]	= "CloneTag";

class CFRAT
{
public:
	CFRAT();
	~CFRAT();
	void Initialize();
	bool IsAuditMode();
	bool IsFactoryMode();
	bool IsReseal();
	
private:
	bool m_bIsAuditMode;
	bool m_bIsFactoryMode;
	bool m_bIsReseal;
};