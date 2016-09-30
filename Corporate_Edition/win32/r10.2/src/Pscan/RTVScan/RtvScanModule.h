#include "stdafx.h"
#include "resource.h"
#include "RTVScan.h"

class CRTVScanModule : public CAtlExeModuleT< CRTVScanModule >
{
public :
	DECLARE_LIBID(LIBID_RTVScanLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_RTVSCAN, "{7F9D125B-D23F-4275-8656-931F712092A7}")
};
