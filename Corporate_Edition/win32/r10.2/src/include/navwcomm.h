// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#ifndef _NAVW_COMM_
#define _NAVW_COMM_

// ---------------------------------------------------------------------------
//  Required to register a message so NAVW32 can talk with NAVAPW
// ---------------------------------------------------------------------------

#define NAVW_NAVAPW_COMM    "Navw32NavApWComm"

// ---------------------------------------------------------------------------
//  Possible values for WPARAM parameter when the above registered message is
//  received.
// ---------------------------------------------------------------------------

enum
{
    NAVW_CMD_SHOWHIDEICON = 0,          // LPARAM == TRUE to SHOW icon.
    NAVW_CMD_CANBEDISABLED,             // LPARAM == TRUE to allow disabeling.
    NAVW_CMD_UNLOADNAVAPW               // LPARAM == doesn't care.
};

#endif // #ifndef _NAVW_COMM_

