#include "NavOptions.h"
#include "NAVAPSCR.h"
#include "OptNames.h"
#include "APOptnames.h"
#include "atlbase.h"
#include <winreg.h>
#include "Shellapi.h"
#include "NAVInfo.h"
#include "OSInfo.h"

class COEMOptions
{
public:
	//public functions
	COEMOptions();
	~COEMOptions();
	
	bool Initialize();
	bool TurnOnThreatCat(int iIndex, bool bYes_No);
	bool TurnOnAP(bool bYes_No);
	bool TurnOnDelayLoad(bool bYes_No);
	bool TurnOnEMI(bool bYes_No);
	bool TurnOnEMO(bool bYes_No);
	bool TurnOnMSN(bool bYes_No);
	bool SetSAVRTPELManual();
	bool SetCCServicesStates(BOOL bAutoStart, BOOL bStartNow);

private:
	//member vars
	bool RebootNeeded();
	bool m_bAllOK;
	INAVOptions* pOptionsObject;
	IScriptableAutoProtect* pAPScriptObject;	
	HRESULT m_hrCOM;
};