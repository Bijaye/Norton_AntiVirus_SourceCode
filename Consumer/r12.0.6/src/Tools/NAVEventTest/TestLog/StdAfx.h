#pragma once

#define _WIN32_WINNT 0x0400
#define VC_EXTRALEAN		

#include <afxwin.h>         
#include <afxcmn.h>

#include <comdef.h>

// Identifier was truncated to x characters in the debug information
#pragma warning(disable:4786)

#import "ccEventManager.tlb"
#import "NAVEvents.tlb"

#include "ccLib.h"
#include "ccLibStd.h"
#include "ccLibLink.h"