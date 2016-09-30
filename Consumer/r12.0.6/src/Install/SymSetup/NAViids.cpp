#include <TCHAR.h>
#include <atlbase.h>
#define _WTL_NO_CSTRING
#include <atlapp.h>

extern CAppModule _Module;

#include <atlstr.h>
#include <atlmisc.h>
#include <atlwin.h>


#if !defined(_NO_SIMON_INITGUID)
    #define SIMON_INITGUID
#endif
#define INITIIDS
#include "ccScanInterface.h"
#include "PatchInstInterface.h"
#include "ccVerifyTrustLibLink.h"
#include "ccVerifyTrustLoader.h"
#include "ccVerifyTrustStatic.h"
#include "IWPSettingsInterface.h"
#include "DefUtilsInterface.h"  
#include "ccErrorDisplayLoader.h"
#include "iquaran.h"
#include "IPreFlightScanner.h"
#include "simon.h"
#include "cltLicenseInterfaces.h"
#include "ccServicesConfigInterface.h"
#include "SymProtectControlHelper.h"
