; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CDFGuiView
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "DFGui.h"
LastPage=0

ClassCount=10
Class1=CDFGuiApp
Class2=CDFGuiDoc
Class3=CDFGuiView
Class4=CMainFrame

ResourceCount=7
Resource1=IDD_CHANGE_PRIORITY
Resource2=IDR_MAINFRAME (NEWBMP)
Class5=CAboutDlg
Resource3=IDD_CONFIGURE_DIALOG
Class6=DFConfigureDialog
Resource4=IDD_STATISTICS_DIALOG
Class7=CDFStatisticsDialog
Resource5=IDD_ABOUTBOX
Class8=CPriorityDialog
Class9=DFScannedSamples
Resource6=IDR_MAINFRAME
Class10=DFSampleAttributes
Resource7=IDD_SAMPLEATTRIBUTES

[CLS:CDFGuiApp]
Type=0
HeaderFile=DFGui.h
ImplementationFile=DFGui.cpp
Filter=N
BaseClass=CWinApp
VirtualFilter=AC

[CLS:CDFGuiDoc]
Type=0
HeaderFile=DFGuiDoc.h
ImplementationFile=DFGuiDoc.cpp
Filter=N

[CLS:CDFGuiView]
Type=0
HeaderFile=DFGuiView.h
ImplementationFile=DFGuiView.cpp
Filter=W
LastObject=CDFGuiView
BaseClass=CListViewEx
VirtualFilter=VWC

[CLS:CMainFrame]
Type=0
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=T
LastObject=CMainFrame
BaseClass=CFrameWnd
VirtualFilter=fWC



[CLS:CAboutDlg]
Type=0
HeaderFile=DFGui.cpp
ImplementationFile=DFGui.cpp
Filter=D
BaseClass=CDialog
VirtualFilter=dWC
LastObject=CAboutDlg

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=6
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889
Control5=IDC_BUILDNUMBER,static,1342308352
Control6=IDC_STATIC,static,1342308352

[MNU:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_PRINT
Command2=ID_FILE_PRINT_SETUP
Command3=ID_APP_EXIT
Command4=ID_VIEW_TOOLBAR
Command5=ID_VIEW_STATUS_BAR
Command6=ID_LAUNCHER_SHOW
Command7=ID_STATISTICS
Command8=ID_SAMPLEATTRIBUTES
Command9=ID_HOLD
Command10=ID_STOP
Command11=ID_RESUME
Command12=ID_DEFER
Command13=ID_HOLDALL
Command14=ID_RESUMEALL
Command15=ID_DISABLE
Command16=ID_ENABLE
Command17=ID_CHANGEPRIORITY
Command18=ID_STOPIMPORT
Command19=ID_STOPUNDEFERRER
Command20=ID_STOPUPDATER
Command21=ID_STOPATTRCOLLECTOR
Command22=ID_CONFIGURE
Command23=ID_APP_ABOUT
CommandCount=23

[ACL:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_PRINT
Command5=ID_EDIT_UNDO
Command6=ID_EDIT_CUT
Command7=ID_EDIT_COPY
Command8=ID_EDIT_PASTE
Command9=ID_EDIT_UNDO
Command10=ID_EDIT_CUT
Command11=ID_EDIT_COPY
Command12=ID_EDIT_PASTE
Command13=ID_NEXT_PANE
Command14=ID_PREV_PANE
CommandCount=14

[TB:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_PRINT
CommandCount=1

[TB:IDR_MAINFRAME (NEWBMP)]
Type=1
Class=?
Command1=ID_FILE_PRINT
CommandCount=1

[DLG:IDD_CONFIGURE_DIALOG]
Type=1
Class=DFConfigureDialog
ControlCount=26
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_MAX_SAMPLES,edit,1350639744
Control4=IDC_SUBMISSION_INTERVAL,edit,1350639744
Control5=IDC_ARRIVAL_INTERVAL,edit,1350639744
Control6=IDC_DATABASE_RESCAN_INTERVAL,edit,1350639744
Control7=IDC_DEF_IMPORT_INTERVAL,edit,1350631552
Control8=IDC_STATUS_UPDATE_INTERVAL,edit,1350631552
Control9=IDC_UNDEFERRER_INTERVAL,edit,1350631552
Control10=IDC_ATTRIBUTE_INTERVAL,edit,1350631552
Control11=IDC_UNC,edit,1350631552
Control12=IDC_BUILD_DEF_FILENAME,edit,1350631552
Control13=IDC_DEF_BASE_DIR,edit,1350631552
Control14=IDC_DEF_IMPORTER_DIR,edit,1350631552
Control15=IDC_STATIC0,static,1342177280
Control16=IDC_STATIC6,static,1342177280
Control17=IDC_STATIC4,static,1342177280
Control18=IDC_STATIC2,static,1342177280
Control19=IDC_STATIC1,static,1342177280
Control20=IDC_STATIC,static,1342308352
Control21=IDC_STATIC,static,1342308352
Control22=IDC_STATIC,static,1342308352
Control23=IDC_STATIC,static,1342308352
Control24=IDC_STATIC5,static,1342177280
Control25=IDC_STATIC,static,1342308352
Control26=IDC_STATIC,static,1342308352

[CLS:DFConfigureDialog]
Type=0
HeaderFile=DFConfigureDialog.h
ImplementationFile=DFConfigureDialog.cpp
BaseClass=CDialog
Filter=D
LastObject=DFConfigureDialog
VirtualFilter=dWC

[DLG:IDD_STATISTICS_DIALOG]
Type=1
Class=CDFStatisticsDialog
ControlCount=14
Control1=IDOK,button,1342242817
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,button,1342177287
Control6=IDC_NUMBER_OF_SAMPLES,static,1342308352
Control7=IDC_AVERAGE_TIME,static,1342308352
Control8=IDC_START_TIME,static,1342308352
Control9=IDC_STATIC,static,1342308352
Control10=IDC_NUMBER_OF_RESCANNED_SAMPLES,static,1342308352
Control11=IDC_NUMBER_OF_DEFERRED_SAMPLES,static,1342308352
Control12=IDC_STATIC,static,1342308352
Control13=IDC_STATIC,static,1342308352
Control14=IDC_NUMBER_OF_SUCCESS_SAMPLES,static,1342308352

[CLS:CDFStatisticsDialog]
Type=0
HeaderFile=DFStatisticsDialog.h
ImplementationFile=DFStatisticsDialog.cpp
BaseClass=CDialog
Filter=W
LastObject=CDFStatisticsDialog
VirtualFilter=dWC

[DLG:IDD_CHANGE_PRIORITY]
Type=1
Class=CPriorityDialog
ControlCount=6
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308352
Control4=IDC_TRACKING_NUMBER,edit,1350631552
Control5=IDC_STATIC,static,1342308352
Control6=IDC_PRIORITY,edit,1350631552

[CLS:CPriorityDialog]
Type=0
HeaderFile=PriorityDialog.h
ImplementationFile=PriorityDialog.cpp
BaseClass=CDialog
Filter=D
LastObject=CPriorityDialog
VirtualFilter=dWC

[CLS:DFScannedSamples]
Type=0
HeaderFile=DFScannedSamples.h
ImplementationFile=DFScannedSamples.cpp
BaseClass=CDialog
Filter=D
LastObject=DFScannedSamples
VirtualFilter=dWC

[DLG:IDD_SAMPLEATTRIBUTES]
Type=1
Class=DFSampleAttributes
ControlCount=3
Control1=IDOK,button,1342242817
Control2=IDC_EDIT1,edit,1353779204
Control3=IDC_SAMPLEID,static,1342308352

[CLS:DFSampleAttributes]
Type=0
HeaderFile=DFSampleAttributes.h
ImplementationFile=DFSampleAttributes.cpp
BaseClass=CDialog
Filter=D
LastObject=IDC_SAMPLEID
VirtualFilter=dWC

