#include <afxinet.h>

class CAVISInternetSession : public CInternetSession
{
public:
	// constructor
	CAVISInternetSession(
		LPCTSTR pstrAgent,
		DWORD dwContext,
		DWORD dwAccessType,
		LPCTSTR pstrProxyName,
		LPCTSTR pstrProxyBypass,
		DWORD dwFlags = 0);

	// virtual override to recieve callbacks about internet events
	virtual void OnStatusCallback(
		DWORD dwContext,
		DWORD dwInternetStatus,
		LPVOID lpvStatusInformation,
		DWORD dwStatusInformationLen);
};
