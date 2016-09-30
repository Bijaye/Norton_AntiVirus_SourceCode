/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/


#include "stdafx.h"
#include "qscon.h"
#include "qsconsole.h"
#include <atlsnap.h>
#include "extend.h"

// 
// Initialize static extension class members here.
// 
static const GUID CExtendComputerGUID_NODETYPE = 
{ 0x476E6449, 0xAAFF, 0x11d0, { 0xB9, 0x44, 0x00, 0xC0, 0x4F, 0xD8, 0xD5, 0xB0 } };
const GUID*  CExtendComputerManagement::m_NODETYPE = &CExtendComputerGUID_NODETYPE;
const OLECHAR* CExtendComputerManagement::m_SZNODETYPE = OLESTR("{476E6449-AAFF-11D0-B944-00C04FD8D5B0}");
const OLECHAR* CExtendComputerManagement::m_SZDISPLAY_NAME = OLESTR("Symantec AntiVirus Quarantine");
const CLSID* CExtendComputerManagement::m_SNAPIN_CLASSID = &CLSID_QSConsole;

static const GUID CExtendNSCStaticGUID_NODETYPE = 
{ 0x0A624A66, 0x269C, 0x11d3, { 0x80, 0xF4, 0x00, 0xC0, 0x4F, 0x68, 0xD9, 0x69 } };
const GUID*  CExtendNSCStatic::m_NODETYPE = &CExtendNSCStaticGUID_NODETYPE;
const OLECHAR* CExtendNSCStatic::m_SZNODETYPE = OLESTR("{0A624A66-269C-11d3-80F4-00C04F68D969}");
const OLECHAR* CExtendNSCStatic::m_SZDISPLAY_NAME = OLESTR("Symantec AntiVirus Quarantine");
const CLSID* CExtendNSCStatic::m_SNAPIN_CLASSID = &CLSID_QSConsole;

