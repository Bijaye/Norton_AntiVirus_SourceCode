; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CNvlbDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "nvlb.h"

ClassCount=4
Class1=CNvlbApp
Class2=CNvlbDlg
Class3=CAboutDlg

ResourceCount=6
Resource1=IDD_STATUS
Resource2=IDR_MAINFRAME
Resource3=IDD_NVLB_DIALOG
Resource4=IDD_ABOUTBOX
Resource5=IDD_HEUR_SETTING
Resource6=IDR_ACCELERATOR1

[CLS:CNvlbApp]
Type=0
HeaderFile=nvlb.h
ImplementationFile=nvlb.cpp
Filter=N

[CLS:CNvlbDlg]
Type=0
HeaderFile=nvlbDlg.h
ImplementationFile=nvlbDlg.cpp
Filter=D

[CLS:CAboutDlg]
Type=0
HeaderFile=nvlbDlg.h
ImplementationFile=nvlbDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=6
Control1=IDC_STATIC,static,1342308480
Control2=IDC_STATIC,static,1342308352
Control3=IDOK,button,1342373889
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342179342
Control6=IDC_STATIC,static,1342177288

[DLG:IDD_NVLB_DIALOG]
Type=1
Class=CNvlbDlg
ControlCount=13
Control1=IDC_BUTTON_EXIT,button,1342242816
Control2=IDC_BUILD_LIBRARY,button,1342242816
Control3=IDC_STATIC,button,1342177287
Control4=IDC_STATIC,button,1342177287
Control5=IDC_STATIC,button,1342177287
Control6=IDC_STATIC,button,1342177287
Control7=IDC_DEFS_DIR,static,1342312460
Control8=IDC_INF_PATH,static,1342312460
Control9=IDC_INCOMING_DIR,static,1342312460
Control10=IDC_VIRLIB_DIR,static,1342312460
Control11=IDC_STATIC,static,1342177294
Control12=IDC_STATIC,button,1342177287
Control13=IDC_HEUR_SETTING,static,1342312460

[DLG:IDD_STATUS]
Type=1
Class=?
ControlCount=6
Control1=IDCANCEL,button,1342245632
Control2=IDC_SCAN_STATUS,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDC_STATIC,static,1342308352
Control5=IDC_FILES_PROCESSED,static,1342308352
Control6=IDC_FILES_ADDED,static,1342308352

[DLG:IDD_HEUR_SETTING]
Type=1
Class=?
ControlCount=3
Control1=IDOK,button,1342245633
Control2=IDC_HEUR_LEVEL,msctls_trackbar32,1342242852
Control3=IDC_HEUR_LEVEL_STATUS,static,1342308352

[ACL:IDR_ACCELERATOR1]
Type=1
Class=?
Command1=IDR_ESCAPE
Command2=IDR_RETURN
CommandCount=2

