// Copyright 2004-2005 Symantec Corporation. All rights reserved.
// Author: Bertil Askelid <Bertil_Askelid@Symantec.com>
// Intent: Linux implementation of NDK File Information functions

#ifndef _SYS_STAT_H

#include <system/sys/stat.h>
#include <symStat.h>

#define stat  symStat
#define fstat symFstat

#endif // _SYS_STAT_H
