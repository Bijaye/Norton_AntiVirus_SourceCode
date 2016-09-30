; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CModuleDialog
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "avismonitor.h"
LastPage=0

ClassCount=8
Class1=CAVISMonitorApp
Class2=CAboutDlg
Class3=CAVISMonitorView
Class4=CMainFrame
Class5=CListViewEx
Class6=CTitleTip

ResourceCount=4
Resource1=IDD_ABOUTBOX
Resource2=IDD_DIALOG_MODULE_INFO
Class7=CModuleDialog
Resource3=IDR_MAINFRAME
Class8=CMonitorSetupDialog
Resource4=IDD_DIALOG_MONITOR_SETTINGS

[CLS:CAVISMonitorApp]
Type=0
BaseClass=CWinApp
HeaderFile=AVISMonitor.h
ImplementationFile=AVISMonitor.cpp
LastObject=CAVISMonitorApp

[CLS:CAboutDlg]
Type=0
BaseClass=CDialog
HeaderFile=AVISMonitor.cpp
ImplementationFile=AVISMonitor.cpp
LastObject=IDC_ABOUT_COPYRIGHT_INFO
Filter=D
VirtualFilter=dWC

[CLS:CAVISMonitorView]
Type=0
BaseClass=CListView
HeaderFile=AVISMonitorView.h
ImplementationFile=AVISMonitorView.cpp
Filter=C
VirtualFilter=VWC
LastObject=CAVISMonitorView

[CLS:CMainFrame]
Type=0
BaseClass=CFrameWnd
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=T
VirtualFilter=fWC
LastObject=CMainFrame

[CLS:CListViewEx]
Type=0
BaseClass=CListView
HeaderFile=\is100.dev\src\dataflow\com\listvwex.h
ImplementationFile=\is100.dev\src\dataflow\com\listvwex.cpp

[CLS:CTitleTip]
Type=0
BaseClass=CWnd
HeaderFile=\is100.dev\src\dataflow\com\titletip.h
ImplementationFile=\is100.dev\src\dataflow\com\titletip.cpp

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=5
Control1=IDC_STATIC,static,1342177283
Control2=IDOK,button,1342373889
Control3=IDC_ABOUT_PRODUCT_NAME,static,1342308352
Control4=IDC_ABOUT_VERSION_INFO,static,1342308352
Control5=IDC_ABOUT_COPYRIGHT_INFO,static,1342308352

[TB:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_PRINT
CommandCount=1

[MNU:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_MONITOR_LOAD
Command2=ID_MONITOR_SAVE
Command3=ID_FILE_PRINT
Command4=ID_FILE_PRINT_SETUP
Command5=ID_APP_EXIT
Command6=ID_VIEW_TOOLBAR
Command7=ID_VIEW_STATUS_BAR
Command8=ID_ACTIONS_ADD
Command9=ID_ACTIONS_REMOVE
Command10=ID_ACTIONS_START
Command11=ID_ACTIONS_STOP
Command12=ID_ACTIONS_EDIT
Command13=ID_SETUP_OPTIONS
Command14=ID_APP_ABOUT
CommandCount=14

[DLG:IDR_MAINFRAME]
Type=1
Class=?
ControlCount=1
Control1=IDC_STATIC,static,1342308352

[DLG:IDD_DIALOG_MODULE_INFO]
Type=1
Class=CModuleDialog
ControlCount=15
Control1=IDC_STATIC,static,1342308866
Control2=IDC_EDIT_MODULE_NAME,edit,1350631552
Control3=IDC_STATIC,static,1342308866
Control4=IDC_EDIT_MODULE_FILE_PATH,edit,1350631552
Control5=IDC_STATIC,static,1342308866
Control6=IDC_EDIT_MODULE_ARGUMENTS,edit,1350631552
Control7=IDC_STATIC,static,1342308866
Control8=IDC_EDIT_AUTO_STOP_TIME,edit,1350639744
Control9=IDC_STATIC,static,1342308864
Control10=IDC_CHECK_RESTART_STOPPED_MODULE,button,1342242819
Control11=IDC_EDIT_RESTART_TIME,edit,1350631552
Control12=IDC_STATIC,static,1342308864
Control13=IDC_CHECK_SEND_MAIL,button,1342242819
Control14=IDOK,button,1342242817
Control15=IDCANCEL,button,1342242816

[CLS:CModuleDialog]
Type=0
HeaderFile=ModuleDialog.h
ImplementationFile=ModuleDialog.cpp
BaseClass=CDialog
Filter=D
LastObject=CModuleDialog
VirtualFilter=dWC

[DLG:IDD_DIALOG_MONITOR_SETTINGS]
Type=1
Class=CMonitorSetupDialog
ControlCount=8
Control1=IDC_STATIC,static,1342308866
Control2=IDC_EDIT_STATUS_CHECK_INTERVAL,edit,1350639744
Control3=IDC_STATIC,static,1208091138
Control4=IDC_EDITAUTO_RESTART_ERROR_COUNT,edit,1216422016
Control5=IDC_CHECK_START_ON_MONITOR_STARTUP,button,1342242819
Control6=IDC_CHECK_AUTO_RESTART,button,1208025091
Control7=IDOK,button,1342242817
Control8=IDCANCEL,button,1342242816

[CLS:CMonitorSetupDialog]
Type=0
HeaderFile=MonitorSetupDialog.h
ImplementationFile=MonitorSetupDialog.cpp
BaseClass=CDialog
Filter=D
LastObject=CMonitorSetupDialog
VirtualFilter=dWC

