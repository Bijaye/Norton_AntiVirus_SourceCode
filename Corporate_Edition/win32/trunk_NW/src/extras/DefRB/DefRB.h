// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.

#if !defined(AFX_DEFRB_H__07E032B1_54D6_432B_8137_85AA5C39C75D__INCLUDED_)
#define AFX_DEFRB_H__07E032B1_54D6_432B_8137_85AA5C39C75D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
DWORD ForceUpdate ( void );

typedef enum {
    ERR_NO_NAVC = 1,
    ERR_NO_NAVC_CLIENT,
    ERR_NOT_RESPONDING,
    ERR_DEFS_UNLOADED,
    ERR_REGISTRY_WRITE
}
;

#endif // !defined(AFX_DEFRB_H__07E032B1_54D6_432B_8137_85AA5C39C75D__INCLUDED_)
