========================================================================
  Project:  	Def Watch Load Point Scan
  Developer: 	Serge Pantic <pantic@ultimatetech.com>
  Description:	Load latest virus definition and Run a silent, on-demand 
		scan that scans just load points
  Location:	\\depot\Norton_AntiVirus\Corporate_Edition\Win32\src\DefWatchHandler\dwLdPntScan
========================================================================

Project Def Watch Load Point Scan (dwLdPntScan) is the in-process COM object 
that implements the IDefWatchEventHandler interface.

When new virus definitions arrive, the dwLdPntScan COM object is instantiated
through the IDefWatchEventHandler interface and the OnNewDefsInstalled function is
executed.

The fuction does the following:

 1. Request the main SAV service (Rtvscan) to load the latest virus definitions 
    (presumably the ones that just arrived) and
 2. Run a silent, on-demand scan that scans just load points.


The following is a detailed description of the handler:

  1. Load CLSID_CliProxy,

  2. Get an instance of the IUtil4 interface 

  3. Call the IUtil4::UseLatestDefs 

  4. Check how many latest virus definition are available. If it is
     more than one exit.

  5. Get an instance of the IScanConfig interface, specifying HKEY_VP_ADMIN_SCANS as 
     the RootID and "Defwatch Load Point Scan" as the scan name. 

  6. Check the IScanConfig::EnumOptions to see if there are any scan options:

     6.1 If it's empty, then create the default config as follows: 

	6.1.1 Open another IScanConfig interface, specifying HKEY_VP_ADMIN_SCANS as 
	      the RootID and a GUID as the scan name. 

	6.1.2 Use IScanConfig::CopyFromInterface to copy the manual scan settings into 
 	      the new temporary scan config. 

	6.1.3 Set the following values in the new temporary scan config: 
		ShowScanDialog == 0, 
		ScanForGreyware == 1, 
		ScanProcesses == 1, 
		ScanAllDrives == 0

     6.2 If it's not empty, use existing config.

   7. Check for the sub option Schedule\Enabled.  

	7.1 If it equals 1 run the scan.  	
	    Get an instance of the IScan interface and run the scan, 
 	    specifying TRUE for asynch.

	7.2 If it equals 0, quit.  The default should be 0 
	    (if the setting doesn't exist). 



----------------------------------------------------------------------------

This file contains a summary of what you will find in each of the files that
make up your dwLdPntScan application.


dwLdPntScan.vcproj
    This is the main project file for VC++ project. 

dwLdPntScan.cpp
    This is the main DLL source file where CLoadPointScanHandler is
    defined.

dwLdPntScan.h
    This is the manin header file for project with class definition
    for CLoadPointScanHandler.

dwLdPntScan.def
    Library definition.

factory.cpp 
    This source file contains IClassFactory implementation for project.

factory.h
    This header contains IClassFactory declaration for project.


/////////////////////////////////////////////////////////////////////////////
Other standard files:

StdAfx.h, StdAfx.cpp
    These files are used to build a precompiled header (PCH) file
    named dwLdPntScan.pch and a precompiled types file named StdAfx.obj.

/////////////////////////////////////////////////////////////////////////////
