//========================================================================
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 1993 - 2003, 2005 Symantec Corporation. All rights reserved.
//------------------------------------------------------------------------
// WndClass.h
// Contains defines for window class names of global shared window classes
// registered by the Quake libraries. These names change when the Quake
// version suffix character changes. This allows old apps to linke to old
// DLLs and newer apps to link to newer versions of the same DLLs
// simultaneously without interference.
//
// $Header:   S:/INCLUDE/VCS/WNDCLASS.H_v   1.16   22 Jun 1998 20:53:52   dwilson  $
//
//------------------------------------------------------------------------
// $Log:   S:/INCLUDE/VCS/WNDCLASS.H_v  $
// 
//    Rev 1.16   22 Jun 1998 20:53:52   dwilson
// Bump :24: to :25: for branch N.
//
//    Rev 1.15   25 Nov 1997 09:29:48   RFULLER
// Updated for CORE M
//
//    Rev 1.14   16 Oct 1997 13:16:12   RFULLER
// Incremented :21: to :25:
//
//    Rev 1.13   13 Aug 1997 00:30:28   RFULLER
// Updated for QAKK Branch
//
//    Rev 1.12   20 Feb 1997 00:05:24   PSPEAR
// Bumped #'s from 19 to 20.
//
//    Rev 1.11   19 Dec 1996 22:25:48   SPASNIK
// Wndclass versions should match branch number of nef
// which has now been set to 0
//
//    Rev 1.10   18 Dec 1996 21:11:44   jcubbag
// Major Build Reved to 19, Trunk is now J
//
//    Rev 1.9   27 Nov 1996 20:11:54   SPASNIK
// Changed quake_custom_nef to qcnef to shorten it
//
//    Rev 1.8   27 Nov 1996 20:38:50   jcubbag
// Core QAKH Branch for Zaphod
//
//    Rev 1.7   25 Nov 1996 21:15:50   THOFF
// Changed #elseif to #elif -- the former doesn't exist.
//
//    Rev 1.6   14 Nov 1996 18:45:00   SCHILUK
// wndclass.h -> S:\include\vcs\wndclass.h_v
// Added QUAKE_CUSTOM_NEF macro and NEF class strings
//
//    Rev 1.5   30 Oct 1996 14:39:04   rjackso
// H Branch Updates
//
//    Rev 1.4   22 Oct 1996 15:15:20   JCUBBAG
// Updated for Branch QAKF
//
//    Rev 1.3   08 Aug 1996 15:29:38   RJACKSO
// Updates for QAKE Branch
//
//    Rev 1.2   29 May 1996 18:12:12   RJACKSO
// changes for QUAKE E
//
//    Rev 1.1   19 Mar 1996 10:08:54   RANSCHU
//
//    Rev 1.0   27 Jan 1996 20:17:24   RJACKSO
// Initial revision.
//
//    Rev 1.32   27 Jan 1996 10:25:12   RJACKSO
// Changes for Quake B Branch
//
//    Rev 1.31   13 Oct 1995 17:38:58   JREARDON
// Changed 10's to 11's
//
//    Rev 1.30   02 Aug 1995 12:31:30   SCOTTP
// Okay now we go from 9 to 10
//
//========================================================================

#ifndef _WNDCLASS_H
#define _WNDCLASS_H

#ifndef SYM_WIN32

#ifdef QCUST
#define QCLASS_NT_TABLE         "NGTableWindow:QCUST2:WIN"
#define QCLASS_NG_BUTTONBAR     "NGButtonBar:QCUST2:WIN"
#define QCLASS_FILLBAR          "NGFillbar:QCUST2:WIN"
#define QCLASS_SPN_BUTTON       "NGSpinButton:QCUST2:WIN"
#define QCLASS_SPN_SCROLL       "NGSpinScroll:QCUST2:WIN"
#define QCLASS_TB_BUTTON        "NGTunnelButton:QCUST2:WIN"
#define QCLASS_TB_BUTTON_COMBO  "NGTnlBtnCb:QCUST2:WIN"
#define QCLASS_TB_TUNNELER      "NGTunnelTunneler:QCUST2:WIN"
#define QCLASS_COLORBOX         "NGColorBox:QCUST2:WIN"
#define QCLASS_NT_TRAY          "NGTrayWindow:QCUST2:WIN"
#define QCLASS_STATUS           "NGStatus:QCUST2:WIN"
#define QCLASS_CPL              "NGCtrlPanl:QCUST2:WIN"
#define QCLASS_NDL              "NGDrvList:QCUST2:WIN"
#define QCLASS_NDC              "NGDrvCombo:QCUST2:WIN"
#define QCLASS_NUMEDIT          "NGNumericEdit:QCUST2:WIN"
#define QCLASS_TAB_BTN          "NGGroovyButton:QCUST2:WIN"
#define QCLASS_TAB_LIST         "NGGroovyList:QCUST2:WIN"
#define QCLASS_STATIC           "NGStatic:QCUST2:WIN"
#define QCLASS_VCRBUTTON        "NGVCRButton:QCUST2:WIN"
#define QCLASS_DRVMAP           "NDrvMap:QCUST2:WIN"
#define QCLASS_VWR_DSP          "SymVwrDspClass:QCUST2:WIN"
#define QCLASS_VWR_DSP_OLD      "VwrDspClass"
#define QCLASS_VWR_PRS          "SymVwrPrsClass:QCUST2:WIN"
#define QCLASS_VWR_COMM         "SymVwrCommWnd:QCUST2:WIN"
#define QCLASS_CHRT_DATATABLE   "SYMCHRT:DataTab:QCUST2:WIN"
#define QCLASS_CHRT_PAGENAME    "SYMCHRT:PgWin:QCUST2:WIN"
#define QCLASS_CHRT_GRAPHNAME   "SYMCHRT:GrWin:QCUST2:WIN"
#define QCLASS_CHRT_CUSBUTTON   "SYMCHRT:CusButton:QCUST2:WIN"
#define QCLASS_SLIDER           "NGSlider:QCUST2:WIN"
#define QCLASS_TREECONTROL      "Symgui:TreeControl:QCUST2:WIN"
#else //not QCUST give normal class names
#define QCLASS_NT_TABLE         "NGTableWindow:30:WIN"
#define QCLASS_NG_BUTTONBAR     "NGButtonBar:30:WIN"
#define QCLASS_FILLBAR          "NGFillbar:30:WIN"
#define QCLASS_SPN_BUTTON       "NGSpinButton:30:WIN"
#define QCLASS_SPN_SCROLL       "NGSpinScroll:30:WIN"
#define QCLASS_TB_BUTTON        "NGTunnelButton:30:WIN"
#define QCLASS_TB_BUTTON_COMBO  "NGTnlBtnCb:30:WIN"
#define QCLASS_TB_TUNNELER      "NGTunnelTunneler:30:WIN"
#define QCLASS_COLORBOX         "NGColorBox:30:WIN"
#define QCLASS_NT_TRAY          "NGTrayWindow:30:WIN"
#define QCLASS_STATUS           "NGStatus:30:WIN"
#define QCLASS_CPL              "NGCtrlPanl:30:WIN"
#define QCLASS_NDL              "NGDrvList:30:WIN"
#define QCLASS_NDC              "NGDrvCombo:30:WIN"
#define QCLASS_NUMEDIT          "NGNumericEdit:30:WIN"
#define QCLASS_TAB_BTN          "NGGroovyButton:30:WIN"
#define QCLASS_TAB_LIST         "NGGroovyList:30:WIN"
#define QCLASS_STATIC           "NGStatic:30:WIN"
#define QCLASS_VCRBUTTON        "NGVCRButton:30:WIN"
#define QCLASS_DRVMAP           "NDrvMap:30:WIN"
#define QCLASS_VWR_DSP          "SymVwrDspClass:30:WIN"
#define QCLASS_VWR_DSP_OLD      "VwrDspClass:WIN"
#define QCLASS_VWR_PRS          "SymVwrPrsClass:30:WIN"
#define QCLASS_VWR_COMM         "SymVwrCommWnd:30:WIN"
#define QCLASS_CHRT_DATATABLE   "SYMCHRT:DataTab:30:WIN"
#define QCLASS_CHRT_PAGENAME    "SYMCHRT:PgWin:30:WIN"
#define QCLASS_CHRT_GRAPHNAME   "SYMCHRT:GrWin:30:WIN"
#define QCLASS_CHRT_CUSBUTTON   "SYMCHRT:CusButton:30:WIN"
#define QCLASS_SLIDER           "NGSlider:30:WIN"
#define QCLASS_TREECONTROL      "Symgui:TreeControl:30:WIN"
#endif //QCUST

#else   // SYM_WIN32

#ifdef QCNEF
#define QCLASS_NT_TABLE         "NGTableWindow:NEF0:W32"
#define QCLASS_NG_BUTTONBAR     "NGButtonBar:NEF0:W32"
#define QCLASS_FILLBAR          "NGFillbar:NEF0:W32"
#define QCLASS_SPN_BUTTON       "NGSpinButton:NEF0:W32"
#define QCLASS_SPN_SCROLL       "NGSpinScroll:NEF0:W32"
#define QCLASS_TB_BUTTON        "NGTunnelButton:NEF0:W32"
#define QCLASS_TB_BUTTON_COMBO  "NGTnlBtnCb:NEF0:W32"
#define QCLASS_TB_TUNNELER      "NGTunnelTunneler:NEF0:W32"
#define QCLASS_COLORBOX         "NGColorBox:NEF0:W32"
#define QCLASS_NT_TRAY          "NGTrayWindow:NEF0:W32"
#define QCLASS_STATUS           "NGStatus:NEF0:W32"
#define QCLASS_CPL              "NGCtrlPanl:NEF0:W32"
#define QCLASS_NDL              "NGDrvList:NEF0:W32"
#define QCLASS_NDC              "NGDrvCombo:NEF0:W32"
#define QCLASS_NUMEDIT          "NGNumericEdit:NEF0:W32"
#define QCLASS_TAB_BTN          "NGGroovyButton:NEF0:W32"
#define QCLASS_TAB_LIST         "NGGroovyList:NEF0:W32"
#define QCLASS_STATIC           "NGStatic:NEF0:W32"
#define QCLASS_VCRBUTTON        "NGVCRButton:NEF0:W32"
#define QCLASS_DRVMAP           "NDrvMap:NEF0:W32"
#define QCLASS_VWR_DSP          "SymVwrDspClass:NEF0:W32"
#define QCLASS_VWR_DSP_OLD      "VwrDspClass"
#define QCLASS_VWR_PRS          "SymVwrPrsClass:NEF0:W32"
#define QCLASS_VWR_COMM         "SymVwrCommWnd:NEF0:W32"
#define QCLASS_CHRT_DATATABLE   "SYMCHRT:DataTab:NEF0:W32"
#define QCLASS_CHRT_PAGENAME    "SYMCHRT:PgWin:NEF0:W32"
#define QCLASS_CHRT_GRAPHNAME   "SYMCHRT:GrWin:NEF0:W32"
#define QCLASS_CHRT_CUSBUTTON   "SYMCHRT:CusButton:NEF0:W32"
#define QCLASS_SLIDER           "NGSlider:NEF0:W32"
#define QCLASS_TREECONTROL      "Symgui:TreeControl:NEF0:W32"

#elif defined (QCUST)

#define QCLASS_NT_TABLE         "NGTableWindow:QCUST2:W32"
#define QCLASS_NG_BUTTONBAR     "NGButtonBar:QCUST2:W32"
#define QCLASS_FILLBAR          "NGFillbar:QCUST2:W32"
#define QCLASS_SPN_BUTTON       "NGSpinButton:QCUST2:W32"
#define QCLASS_SPN_SCROLL       "NGSpinScroll:QCUST2:W32"
#define QCLASS_TB_BUTTON        "NGTunnelButton:QCUST2:W32"
#define QCLASS_TB_BUTTON_COMBO  "NGTnlBtnCb:QCUST2:W32"
#define QCLASS_TB_TUNNELER      "NGTunnelTunneler:QCUST2:W32"
#define QCLASS_COLORBOX         "NGColorBox:QCUST2:W32"
#define QCLASS_NT_TRAY          "NGTrayWindow:QCUST2:W32"
#define QCLASS_STATUS           "NGStatus:QCUST2:W32"
#define QCLASS_CPL              "NGCtrlPanl:QCUST2:W32"
#define QCLASS_NDL              "NGDrvList:QCUST2:W32"
#define QCLASS_NDC              "NGDrvCombo:QCUST2:W32"
#define QCLASS_NUMEDIT          "NGNumericEdit:QCUST2:W32"
#define QCLASS_TAB_BTN          "NGGroovyButton:QCUST2:W32"
#define QCLASS_TAB_LIST         "NGGroovyList:QCUST2:W32"
#define QCLASS_STATIC           "NGStatic:QCUST2:W32"
#define QCLASS_VCRBUTTON        "NGVCRButton:QCUST2:W32"
#define QCLASS_DRVMAP           "NDrvMap:QCUST2:W32"
#define QCLASS_VWR_DSP          "SymVwrDspClass:QCUST2:W32"
#define QCLASS_VWR_DSP_OLD      "VwrDspClass"
#define QCLASS_VWR_PRS          "SymVwrPrsClass:QCUST2:W32"
#define QCLASS_VWR_COMM         "SymVwrCommWnd:QCUST2:W32"
#define QCLASS_CHRT_DATATABLE   "SYMCHRT:DataTab:QCUST2:W32"
#define QCLASS_CHRT_PAGENAME    "SYMCHRT:PgWin:QCUST2:W32"
#define QCLASS_CHRT_GRAPHNAME   "SYMCHRT:GrWin:QCUST2:W32"
#define QCLASS_CHRT_CUSBUTTON   "SYMCHRT:CusButton:QCUST2:W32"
#define QCLASS_SLIDER           "NGSlider:QCUST2:W32"
#define QCLASS_TREECONTROL      "Symgui:TreeControl:QCUST2:W32"
#else //not QCUST give normal class names
#define QCLASS_NT_TABLE         "NGTableWindow:30:W32"
#define QCLASS_NG_BUTTONBAR     "NGButtonBar:30:W32"
#define QCLASS_FILLBAR          "NGFillbar:30:W32"
#define QCLASS_SPN_BUTTON       "NGSpinButton:30:W32"
#define QCLASS_SPN_SCROLL       "NGSpinScroll:30:W32"
#define QCLASS_TB_BUTTON        "NGTunnelButton:30:W32"
#define QCLASS_TB_BUTTON_COMBO  "NGTnlBtnCb:30:W32"
#define QCLASS_TB_TUNNELER      "NGTunnelTunneler:30:W32"
#define QCLASS_COLORBOX         "NGColorBox:30:W32"
#define QCLASS_NT_TRAY          "NGTrayWindow:30:W32"
#define QCLASS_STATUS           "NGStatus:30:W32"
#define QCLASS_CPL              "NGCtrlPanl:30:W32"
#define QCLASS_NDL              "NGDrvList:30:W32"
#define QCLASS_NDC              "NGDrvCombo:30:W32"
#define QCLASS_NUMEDIT          "NGNumericEdit:30:W32"
#define QCLASS_TAB_BTN          "NGGroovyButton:30:W32"
#define QCLASS_TAB_LIST         "NGGroovyList:30:W32"
#define QCLASS_STATIC           "NGStatic:30:W32"
#define QCLASS_VCRBUTTON        "NGVCRButton:30:W32"
#define QCLASS_DRVMAP           "NDrvMap:30:W32"
#define QCLASS_VWR_DSP          "SymVwrDspClass:30:W32"
#define QCLASS_VWR_DSP_OLD      "VwrDspClass"
#define QCLASS_VWR_PRS          "SymVwrPrsClass:30:W32"
#define QCLASS_VWR_COMM         "SymVwrCommWnd:30:W32"
#define QCLASS_CHRT_DATATABLE   "SYMCHRT:DataTab:7"
#define QCLASS_CHRT_PAGENAME    "SYMCHRT:PgWin:30:W32"
#define QCLASS_CHRT_GRAPHNAME   "SYMCHRT:GrWin:30:W32"
#define QCLASS_CHRT_CUSBUTTON   "SYMCHRT:CusButton:30:W32"
#define QCLASS_SLIDER           "NGSlider:30:W32"
#define QCLASS_TREECONTROL      "Symgui:TreeControl:30:W32"
#endif //QCUST

#endif // SYM_WIN32
#endif // _WNDCLASS_H
