////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include <TCHAR.h>
#include <atlstr.h>
#include <atlbase.h>
#define _WTL_NO_CSTRING
#include <atlapp.h>

extern CAppModule _Module;

#include <atlmisc.h>
#include <atlwin.h>
#include <initguid.h>

#if !defined(_NO_SIMON_INITGUID)
    #define SIMON_INITGUID
#endif
#define INITIIDS
#include "ccSettingsInterface.h"
#include "SymHTMLWindow.h"
#include "SymHTMLDocument.h"
#include "SymHTMLEventIterator.h"
#include "ccScanInterface.h"
#include "ccCloneInterface.h"
#include "ccSerializeInterface.h"
#include "PatchInstInterface.h"
#include "ccVerifyTrustLibLink.h"
#include "ccVerifyTrustLoader.h"
#include "ccVerifyTrustStatic.h"
#include "IWPSettingsInterface.h"
#include "DefUtilsInterface.h"  
#include "ccErrorDisplayLoader.h"
#include "IPreFlightScanner.h"
#include "simon.h"
#include "FWSetupInterface.h"
#include "cltLicenseInterfaces.h"
#include "ccServicesConfigInterface.h"
#include "SymProtectControlHelper.h"
#include <QBackupInterfaces.h>
#include "SymSkin.h"
#include "SymSkinWindow.h"
#include "SymSkinManager.h"
#include "WSCHelperImpl.h"
#include "SymIDSI.h"
#include "IICFManager.h"
#include "uiPEPConstants.h"
#include "CLTProductHelper.h"
#include "IICFManager.h"
