/////////////////////////////////////////////////////////////////////////////
//  Copyright (c)1997 - Symantec, Peter Norton Computing Group
/////////////////////////////////////////////////////////////////////////////
//
//  $Header:   S:/SETUPBLD/VCS/prodspec.h_v   1.3   20 Sep 1998 23:34:38   VGLOSTE  $
//
//  Setup Builder Include file
//
//    File Name:  ProdSpec.H
//
//  Description:  Include file to use for building
//
//       Author:  Bruce Hellstrom       Date:  3-17-97
//
//     Comments:  This include file defines certain values to be used in
//                creation of the setup
//
/////////////////////////////////////////////////////////////////////////////
//  $Log:   S:/SETUPBLD/VCS/prodspec.h_v  $
// 
//    Rev 1.3   20 Sep 1998 23:34:38   VGLOSTE
// Made changes to accommodate new targets.
// 
//    Rev 1.2   19 Feb 1998 16:03:30   mdunn
// Added NEC versions.
// 
//    Rev 1.1   21 Oct 1997 17:17:06   jtaylor
// -- Added support for OEM builds
//
//    Rev 1.0   11 Jul 1997 20:09:12   JBRENNA
// Fulling integrate Access database and Trial versions.
//
////////////////////////////////////////////////////////////////////////////

#include "NavCom.h"                         // Common Defines.

#ifdef OROS95
#include "oRos95.h"
#elif OROSNT
#include "oRosNt.h"
#elif OTESLA
#include "oTesla.h"
#elif OTESLA_AXP
#include "oTeslaA.h"
#elif CROS95
#include "cRos95.h"
#elif CROSNT
#include "cRosNt.h"
#elif CTESLA
#include "cTesla.h"
#elif CTESLA_AXP
#include "cTeslaA.h"
#elif ROS95
#include "Ros95.h"
#elif ROS95_TRIAL
#include "Ros95Tr.h"
#elif ROSNT
#include "RosNt.h"
#elif ROSNT_TRIAL
#include "RosNtTr.h"
#elif TESLA
#include "Tesla.h"
#elif TESLA_TRIAL
#include "TeslaTr.h"
#elif TESLA_AXP
#include "TeslaAxp.h"
#elif TESLA_AXP_TRIAL
#include "TeslaATr.h"
#elif ROS95NEC
#include "Ros95NEC.h"
#elif ROSNTNEC
#include "RosNTNEC.h"
#elif TESLANEC
#include "TeslaNEC.h"
#elif ROS95NEC_TRIAL
#include "Ros95NECTr.h"
#elif ROSNTNEC_TRIAL
#include "RosNTNECTr.h"
#elif TESLANEC_TRIAL
#include "TeslaNECTr.h"
#else
#error Product name not defined correctly.
#endif
