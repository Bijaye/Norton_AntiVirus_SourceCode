; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
ClassCount=1
ResourceCount=6
NewFileInclude1=#include "stdafx.h"
Class1=CNetBrowsDlg
LastClass=CNetBrowsDlg
Resource1=IDM_CONTEXT_GENERIC
Resource2=IDD_NETBROWS_DIALOG
Resource3=IDM_CONTEXT_BROWSE_ITEM_TITLE
Resource4=IDR_MENU1
Resource5=IDM_CONTEXT_TREE
Resource6=IDM_CONTEXT_BROWSER_INFORMATION_TEXT

[DLG:IDD_NETBROWS_DIALOG]
Type=1
Class=CNetBrowsDlg
ControlCount=8
Control1=IDC_SIZEGRIP_SCROLL_BAR,SCROLLBAR,1342177308
Control2=IDT_BROWSE_ITEM_TITLE,static,1342308608
Control3=IDE_SELECTED_ITEM_NAME,edit,1350631552
Control4=IDOK,button,1476460545
Control5=IDCANCEL,button,1342242816
Control6=IDHELP,button,1342242816
Control7=IDC_NET_TREE,SysTreeView32,1350631447
Control8=IDT_BROWSER_INFORMATION_TEXT,static,1342308620

[CLS:CNetBrowsDlg]
Type=0
HeaderFile=netbrwsd.h
ImplementationFile=netbrwsd.cpp
BaseClass=CDialog
LastObject=CNetBrowsDlg
Filter=D
VirtualFilter=dWC

[MNU:IDM_CONTEXT_TREE]
Type=1
Class=?
Command1=IDM_WHATS_THIS_TREE_CONTROL
Command2=IDM_HT_BROWSE_THE_NETWORK
Command3=IDM_CM_INFODESK
CommandCount=3

[MNU:IDM_CONTEXT_GENERIC]
Type=1
Class=CNetBrowsDlg
Command1=IDM_CM_INFODESK
CommandCount=1

[MNU:IDR_MENU1]
Type=1
Class=?
Command1=IDM_CM_WHAT_THIS_BROWSE_ITEM_TITLE
Command2=IDM_HT_BROWSE_THE_NETWORK
Command3=IDM_CM_INFODESK
CommandCount=3

[MNU:IDM_CONTEXT_BROWSE_ITEM_TITLE]
Type=1
Class=?
Command1=IDM_WHATS_THIS_TREE_CONTROL
Command2=IDM_HT_ENTER_TARGET_MANUALLY
Command3=IDM_CM_INFODESK
CommandCount=3

[MNU:IDM_CONTEXT_BROWSER_INFORMATION_TEXT]
Type=1
Class=?
Command1=IDM_WHATS_THIS_TREE_CONTROL
Command2=IDM_HT_BROWSE_THE_NETWORK
Command3=IDM_CM_INFODESK
CommandCount=3

