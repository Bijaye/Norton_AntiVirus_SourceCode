////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

//*********************************************************************************************************//
#define DECLARE_SYMNETI_CATEGORY_CLASS(CategoryClass, EventId, CategoryId, HelpId)                          \
                                                                                                            \
class CategoryClass : public CLogViewerSymNetiCategory                                                      \
{                                                                                                           \
public:                                                                                                     \
    CategoryClass() :                                                                                       \
    CLogViewerSymNetiCategory(EventId, CategoryId, HelpId) {}                                               \
    virtual ~CategoryClass() {}                                                                             \
                                                                                                            \
private:                                                                                                    \
    CategoryClass(const CategoryClass&);                                                                    \
    CategoryClass& operator =(const CategoryClass&);                                                        \
};                                                                                                          \
//*********************************************************************************************************//

#include "ccEventId.h"
#include "sndEventId.h"
#include "ccLogViewerPluginId.h"
#include "SNLogLogViewerSymNetiCategory.h"
#include "SymHelp.h"
#include "ISSharedLVPluginId.h"

DECLARE_SYMNETI_CATEGORY_CLASS(CLogViewerAlertsCategory, 
                               CC_SYMNETDRV_EVENT_LOG_ALERTS, 
                               CC_NIS_CAT_ALERTS_ID,
							   IDH_NAVW_CCLOGVIEW_IWP_ALERTS)
SYM_DEFINE_OBJECT_ID(NISEVT_ALERTS_CATEGORY_ID, 
	                 0x87e75734, 0xa3f2, 0x454d, 0x9c, 0x7b, 0x5d, 0x76, 0xbe, 0xf, 0x64, 0xa6);

DECLARE_SYMNETI_CATEGORY_CLASS(CLogViewerConnectionsCategory, 
                               CC_SYMNETDRV_EVENT_LOG_CONNECTIONS, 
                               CC_NIS_CAT_CONNECTIONS_ID,
							   IDH_NAVW_CCLOGVIEW_IWP_CONNECTIONS)
SYM_DEFINE_OBJECT_ID(NISEVT_CONNECTIONS_CATEGORY_ID, 
	                 0x1d7c0470, 0x63dd, 0x42c8, 0xaa, 0xaf, 0xb3, 0x90, 0xcb, 0xcd, 0xa9, 0x8a);

DECLARE_SYMNETI_CATEGORY_CLASS(CLogViewerDebugCategory, 
                               CC_SYMNETDRV_EVENT_LOG_DEBUG, 
                               CC_NIS_CAT_DEBUG_ID,
							   0)
SYM_DEFINE_OBJECT_ID(NISEVT_DEBUG_CATEGORY_ID, 
	                 0x8e41937d, 0xa3f1, 0x4d50, 0xa0, 0xb7, 0xa, 0x1, 0x8c, 0xfa, 0x60, 0xde);

DECLARE_SYMNETI_CATEGORY_CLASS(CLogViewerFirewallCategory, 
                               CC_SYMNETDRV_EVENT_LOG_FIREWALL, 
                               CC_NIS_CAT_FIREWALL_ID,
							   IDH_NAVW_CCLOGVIEW_IWP_ACTIVITIES)
SYM_DEFINE_OBJECT_ID(NISEVT_FIREWALL_CATEGORY_ID, 
	                 0xae9ad348, 0x3b50, 0x42bf, 0x87, 0xc3, 0x8a, 0x57, 0x59, 0x88, 0xd8, 0x25);

DECLARE_SYMNETI_CATEGORY_CLASS(CLogViewerIntrusionDetectionCategory, 
                               CC_SYMNETDRV_EVENT_LOG_IDS, 
                               CC_NIS_CAT_INTRUSIONDETECTION_ID,
							   IDH_NAVW_CCLOGVIEW_IWP_WORMDETECTION)
SYM_DEFINE_OBJECT_ID(NISEVT_INTRUSIONDETECTION_CATEGORY_ID, 
	                 0x45da289, 0x8a3, 0x4f11, 0xb8, 0x9f, 0x40, 0x71, 0x72, 0x5b, 0x95, 0x1e);

DECLARE_SYMNETI_CATEGORY_CLASS(CLogViewerSystemLogCategory, 
                               CC_SYMNETDRV_EVENT_LOG_SYSTEM, 
                               CC_NIS_CAT_SYSTEM_ID,
							   IDH_NAVW_CCLOGVIEW_IWP_SYSTEM)
SYM_DEFINE_OBJECT_ID(NISEVT_SYSTEM_CATEGORY_ID, 
	                 0xbb060bf, 0xb335, 0x4fbb, 0x88, 0x9, 0x8c, 0x85, 0x4e, 0xbb, 0x15, 0xdf);

