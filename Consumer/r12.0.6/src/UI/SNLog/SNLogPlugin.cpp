#include "StdAfx.h"

#include "SymInterface.h"
#include "ccSerializableEventEx.h"
#include "SNLogLogViewerPlugin.h"
#include "SNLogSymNetiCategory.h"
#include "LogViewerEventFactory.h"
bool	_InitResources();

#ifndef DEBUG_NEW
    #include <new>
    #include <new.h>
    // Use debug version of operator new if _DEBUG
    #ifdef _DEBUG
        #include <crtdbg.h>
        #define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
    #else // _DEBUG
        #define DEBUG_NEW new
    #endif // _DEBUG
#endif // DEBUG_NEW

#ifdef _DEBUG
    #define new DEBUG_NEW
#endif

#define INIT_RESOURCES() \
	if( false == _InitResources() )\
	return SYMERR_INVALID_FILE;\

SYM_OBJECT_MAP_BEGIN()                
	INIT_RESOURCES()
	SYM_OBJECT_ENTRY(SNLOG_PLUGIN_ID, CLogViewerPlugin)
	SYM_OBJECT_ENTRY(NISEVT_ALERTS_CATEGORY_ID, CLogViewerAlertsCategory)
	SYM_OBJECT_ENTRY(NISEVT_CONNECTIONS_CATEGORY_ID, CLogViewerConnectionsCategory)
	SYM_OBJECT_ENTRY(NISEVT_FIREWALL_CATEGORY_ID, CLogViewerFirewallCategory)
	SYM_OBJECT_ENTRY(NISEVT_INTRUSIONDETECTION_CATEGORY_ID, CLogViewerIntrusionDetectionCategory)
	SYM_OBJECT_ENTRY(NISEVT_SYSTEM_CATEGORY_ID, CLogViewerSystemLogCategory)
    SYM_OBJECT_ENTRY(cc::IID_ILogViewerEventFactory ,CLogViewerEventFactory)
SYM_OBJECT_MAP_END() 

BEGIN_LV_DLL_MAP(CLogViewerPlugin)
    LV_DLL_MAP_ENTRY(SNLOG_DLL)
END_LV_DLL_MAP(CLogViewerPlugin)

BEGIN_LV_CATEGORY_MAP(CLogViewerPlugin)
    LV_CATEGORY_MAP_ENTRY(NISEVT_CONNECTIONS_CATEGORY_ID,                   SNLOG_DLL)
    LV_CATEGORY_MAP_ENTRY(NISEVT_FIREWALL_CATEGORY_ID,                      SNLOG_DLL)
    LV_CATEGORY_MAP_ENTRY(NISEVT_INTRUSIONDETECTION_CATEGORY_ID,            SNLOG_DLL)
    LV_CATEGORY_MAP_ENTRY(NISEVT_SYSTEM_CATEGORY_ID,                        SNLOG_DLL)
    LV_CATEGORY_MAP_ENTRY(NISEVT_ALERTS_CATEGORY_ID,                        SNLOG_DLL)
END_LV_CATEGORY_MAP(CLogViewerPlugin)                           

SYMRESULT WINAPI GetLVPluginObject(SYMOBJECT_ID* pID)
{
	INIT_RESOURCES()
	if (pID == NULL)
    {
		return SYMERR_INVALIDARG;
    }

	*pID = SNLOG_PLUGIN_ID;

	return SYM_OK;
}