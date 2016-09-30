

//-----------------------------------------------------------------------------
//  Utils.cpp
//
//  A component of the MacNuke Project
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
//  Copyright 2003, 2005 (c) Symantec Corp.. All rights reserved.
//  SYMANTEC CONFIDENTIAL
//
//  Revision History
//
//  Who                     When        What / Why
//
//  tedn@thuridion.com	  12/30/03		Added as generic utility class for 
//										helper functions that do not fit 
//										into an object class.
//
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "shutdown.h"
#include "utils.h"


void Utils::DisplayUsage ()
{
      _tprintf(_T("\n"));
      _tprintf(_T("Uninstalls applications using script files (*.aut).\n"));
      _tprintf(_T("See SSUninst.pdf for instructions.\n"));
      _tprintf(_T("\n"));
      _tprintf(_T("Script        Specifies the script file to process.\n"));
      _tprintf(_T("/ALL          Scans and processes all script files in the current directory.\n"));
      _tprintf(_T("/IP           Causes platform specifier in script file to be ignored. [Optional]\n"));
      _tprintf(_T("\n"));
      _tprintf(_T("A script file or /ALL is required.  See Status.ini for output result details."));
      _tprintf(_T("\n"));
}
