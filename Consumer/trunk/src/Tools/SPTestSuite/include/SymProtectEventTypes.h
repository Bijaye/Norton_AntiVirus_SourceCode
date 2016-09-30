////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace SymProtectEvt
{
    enum SP_ACTION_TYPE
    { 
        ACTION_UNKNOWN = 0x0,
        ACTION_FS_CREATE,
        ACTION_FS_DELETE,
        ACTION_FS_OPEN,
        ACTION_FS_RENAME,
        ACTION_FS_SETATTRIBUTES,
        ACTION_FS_DIRECTORY_CREATE,
        ACTION_FS_DIRECTORY_DELETE,
        ACTION_FS_DIRECTORY_RENAME,
        ACTION_FS_DIRECTORY_SETATTRIBUTES,
        ACTION_API_ZWOPENPROCESS,
        ACTION_API_ZWOPENTHREAD,
        ACTION_API_ZWDUPLICATEOBJECT,
        ACTION_REG_OPEN_KEY,         
        ACTION_REG_CREATE_KEY,           
        ACTION_REG_DELETE_KEY,           
        ACTION_REG_DELETE_VALUE,         
        ACTION_REG_SET_VALUE,            
    };

    enum SP_PROTECTION_LEVEL
    {
        PROTECTION_DISABLED,
        PROTECTION_ENABLED,
        PROTECTION_LOG_ONLY
    };

    // BB device type constants (for device authorization events)
    const ULONG BB_DEVICE_INVALID = 0;
    const ULONG BB_DEVICE_CD      = 1;

    // BB hash algorithm constants (for auhtorization via hash)
    const ULONG BB_HASH_INVALID   = 0;
    const ULONG BB_HASH_MD5       = 1;
    const ULONG BB_HASH_SHA1      = 2;
}