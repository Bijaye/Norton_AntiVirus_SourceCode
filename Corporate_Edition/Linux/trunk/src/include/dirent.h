// Copyright 2004-2005 Symantec Corporation. All rights reserved.
// Author: Bertil Askelid <Bertil_Askelid@Symantec.com>
// Intent: Linux implementation of NDK File Directory Access functions

#ifndef _DIRENT_H

#include <system/dirent.h>
#include "symDirent.h"

#define dirent	  symDirent
#define DIR		  symDIR
#define opendir  symOpenDir
#define readdir  symReadDir
#define closedir symCloseDir

#endif // _DIRENT_H
