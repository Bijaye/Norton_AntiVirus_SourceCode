; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CNPlugInDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "NPlugIn.h"

ClassCount=3
Class1=CNPlugInApp
Class2=CNPlugInDlg
Class3=CAboutDlg

ResourceCount=3
Resource1=IDR_CONTEXT_MENU
Resource2=IDR_MAINFRAME
Resource3=IDD_NPLUGIN_DIALOG

[CLS:CNPlugInApp]
Type=0
HeaderFile=NPlugIn.h
ImplementationFile=NPlugIn.cpp
Filter=N
LastObject=CNPlugInApp

[CLS:CNPlugInDlg]
Type=0
HeaderFile=NPlugDlg.h
ImplementationFile=NPlugDlg.cpp
Filter=D
BaseClass=CDialog
VirtualFilter=dWC
LastObject=CNPlugInDlg

[CLS:CAboutDlg]
Type=0
HeaderFile=NPlugDlg.h
ImplementationFile=NPlugDlg.cpp
Filter=D

[DLG:IDD_NPLUGIN_DIALOG]
Type=1
Class=CNPlugInDlg
ControlCount=9
Control1=IDOK,button,1342373889
Control2=IDCANCEL,button,1342242816
Control3=ID_HELP,button,1342242816
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352
Control7=IDC_MIME_LIST,listbox,1352859985
Control8=IDC_CHECK_ALL,button,1342373888
Control9=IDC_UNCHECK_ALL,button,1342242816

[MNU:IDR_CONTEXT_MENU]
Type=1
Class=CNPlugInDlg
Command1=ID_WHATS_THIS
CommandCount=1

