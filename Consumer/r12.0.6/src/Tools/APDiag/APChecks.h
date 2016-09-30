#if !defined(APCHECKS_H__INCLUDED_)
#define APCHECKS_H__INCLUDED_

// This function will make sure that navapw32.dll and navapscr.dll is signed and exists
// it will also register navapscr.dll.
bool CheckAPFiles();

// This will ensure that the service is running
bool IsAPServiceRunning();

// This will ensure that the agent is running
bool IsAPAgentRunning();

class CCheckNavapsvc : public CDriverCheck
{
public:
	CCheckNavapsvc();

	bool Install();
	bool StopService();
	bool StartService();

private:

	bool VerifyStartType();
	bool ExtraChecks();

};

#endif // !defined(APCHECKS_H__INCLUDED_)