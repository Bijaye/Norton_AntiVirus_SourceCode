; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CALMDlg
LastTemplate=CWinThread
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "ALM.h"

ClassCount=6
Class1=CALMApp
Class2=CALMDlg
Class3=CAboutDlg

ResourceCount=4
Resource1=IDD_ALM_DIALOG
Resource2=IDR_MAINFRAME
Class4=CSampleListCtrl
Class5=CDFListCtrl
Resource3=IDD_ABOUTBOX
Class6=DFThread
Resource4=IDR_MENU1

[CLS:CALMApp]
Type=0
HeaderFile=ALM.h
ImplementationFile=ALM.cpp
Filter=N
LastObject=CALMApp

[CLS:CALMDlg]
Type=0
HeaderFile=ALMDlg.h
ImplementationFile=ALMDlg.cpp
Filter=W
BaseClass=CDialog
VirtualFilter=dWC
LastObject=CALMDlg

[CLS:CAboutDlg]
Type=0
HeaderFile=ALMDlg.h
ImplementationFile=ALMDlg.cpp
Filter=D
LastObject=CAboutDlg

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_ALM_DIALOG]
Type=1
Class=CALMDlg
ControlCount=1
Control1=IDC_LIST1,SysListView32,1350632461

[CLS:CSampleListCtrl]
Type=0
HeaderFile=SampleListCtrl.h
ImplementationFile=SampleListCtrl.cpp
BaseClass=CListCtrl
Filter=W
LastObject=CSampleListCtrl

[CLS:CDFListCtrl]
Type=0
HeaderFile=DFListCtrl.h
ImplementationFile=DFListCtrl.cpp
BaseClass=CListCtrl
Filter=W
LastObject=CDFListCtrl
VirtualFilter=FWC

[MNU:IDR_MENU1]
Type=1
Class=?
Command1=ID_REQUESTED
Command2=ID_RUNNING
Command3=ID_SHOWALL
Command4=ID_SHOWHIGH
Command5=ID_LOW
Command6=ID_ACCEPT
Command7=ID_END
Command8=ID_POSTPONE
Command9=ID_TIMEOUT
CommandCount=9

[CLS:DFThread]
Type=0
HeaderFile=DFThread.h
ImplementationFile=DFThread.cpp
BaseClass=CWinThread
Filter=N
LastObject=DFThread

