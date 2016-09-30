========================================================================
  Project:  	Def Watch Load Point Scan
  Developer: 	Serge Pantic <pantic@ultimatetech.com>
  Description:	Load latest virus definition and Run a silent, on-demand 
		scan that scans just load points
  Location:	\depot\Norton_AntiVirus\Corporate_Edition\Win32\src\DefWatchHandler
========================================================================

Project Def Watch Load Point Scan (dwLdPntScan) is the in-process COM object 
that implements the IDefWatchEventHandler interface.

When new virus definitions arrive, the dwLdPntScan COM object is instantiated
through the IDefWatchEventHandler interface and the OnNewDefsInstalled function
is executed.

The fuction does the following:

 1. Request the main SAV service (Rtvscan) to load the latest virus definitions 
    (presumably the ones that just arrived), and
 2. Run a silent, on-demand scan that scans just load points.


For more details please see .\dwLdPntScan\README-COM.txt.

The COM object has been unit tested with the COM client console application 
available at .\client.  For more details about the client please see 
.\client\README-client.txt.

 All unit testing was performed using dprintf flags L V X SC.

