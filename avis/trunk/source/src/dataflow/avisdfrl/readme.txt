========================================================================
       MICROSOFT FOUNDATION CLASS LIBRARY : DFLauncher
========================================================================

This file contains a summary of what you will find in each of the files that
make up your DFLauncher DLL.

DFLauncher.h
	This is the main header file for the DLL.  It declares the
	CDFLauncherApp class.

DFLauncher.cpp
	This is the main DLL source file.  It contains the class CDFLauncherApp.
    It also contains the entry point functions for the DLL i.e.
    __declspec( dllexport ) HWND InitDFLauncher(HWND hALMWnd)
    __declspec( dllexport ) void TermDFLauncher()
    __declspec( dllexport ) BOOL FilterDFLauncherMessage (MSG *pMsg)

DFLauncher.rc
    This is a listing of all of the Microsoft Windows resources that the
    program uses.  It includes the icons, bitmaps, and cursors that are stored
    in the RES subdirectory.  This file can be directly edited in Microsoft
	Developer Studio.

res\DFLauncher.rc2
    This file contains resources that are not edited by Microsoft 
	Developer Studio.  You should place all resources not
	editable by the resource editor in this file.

DFLauncher.def
    This file contains information about the DLL that must be
    provided to run with Microsoft Windows.  It defines parameters
    such as the name and description of the DLL.  It also exports
	functions from the DLL.

DFLauncher.clw
    This file contains information used by ClassWizard to edit existing
    classes or add new classes.  ClassWizard also uses this file to store
    information needed to create and edit message maps and dialog data
    maps and to create prototype member functions.

DFLauncherDoc.cpp
DFLauncherDoc.h
    Dummy document class required to emulate the SDI interface for MFC. There
    is no real document used by this DLL.

DFLauncherView.cpp
DFLauncherView.h
    The view class for the DLL. The resources are displayed as a ListView. All 
    the Menu commands and messages sent by the main application window are 
    processed here.

MainFrm.cpp
MainFrm.h
    The main window of the DLL. The messages from the main application window
    are handled here and are passed to the view class for processing.

DFResource.cpp
DFResource.h
    The class CDFResource to handle the operations and parametes for each 
    available resource. It also defines the class CDFResourceList as a collection
    of CDFResource objects.

Common classes used by the DLL.

Listvwex.cpp
Listvwex.h
    CListViewEx class. This is an extension of the MFC CListView class. the 
    additional functions supported by the class are the Title tips for individual
    cells. Tooltips for the header cells. Printing of the list view. Full row
    selection support. Automatic adjustment of the column widths.

TitleTip.cpp
TitleTip.h
    CTitleTip class. Support for the title tips for individual cells for the
    ListViewEx window.

ToolBarEx.cpp
ToolBarEx.h
    CToolBarEx and CToolDockBar classes. These are extension for the MFC CToolBar
    classes used by the main window to display the tool bar in the new style
    used by MS Office etc.

ParamValue.cpp
ParamValue.h
    CParamValue class. This class is used to read the parameter-attribute values
    from the resource definition file. This can also be used to read other
    configuration parameters which are in the same format.

CoolMenu.cpp
CoolMenu.h
Subclass.cpp
Subclass.h
Emboss.cpp
    CCoolMenuManager clas. Replace standard menu bar with the cool menu bar which
    displays toolbar buttons and accelerator key info along with menu items.

DFJob.h
    CDFJob class used by the application main window and resources window to 
    pass on job request and result parameters.

DFMsg.h
    #define values for the message types used by the main window and resources
    window to communicate.

DFLauncherImp.h
    Import function definitions for the funcions exported by this DLL

DFEvalState.h
DFEvalStatus.h
    Header files used by DFJob.h to maintain the status etc. returned by the
    DataFlow Evaluator.

/////////////////////////////////////////////////////////////////////////////
Other standard files:

StdAfx.h, StdAfx.cpp
    These files are used to build a precompiled header (PCH) file
    named DFLauncher.pch and a precompiled types file named StdAfx.obj.

Resource.h
    This is the standard header file, which defines new resource IDs.
    Microsoft Developer Studio reads and updates this file.

