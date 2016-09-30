**********************************************************************
Network Manager for Windows 95/NT                           README.TXT
Copyright (C) 1997 Symantec Corporation                 September 1997

**********************************************************************
HOW TO USE THIS DOCUMENT
**********************************************************************
To view README.TXT on screen in Notepad, maximize the Notepad window.

To print README.TXT in Notepad, choose Print from the File menu. If
you use another word processor, select the entire document and format
the text in 10-point Courier before printing to ensure proper spacing.

**********************************************************************
NORTON ANTIVIRUS ADMINISTRATION NOTES
**********************************************************************
This README.TXT file covers compatibility information, late-breaking 
news, and usage tips for Norton AntiVirus network administration
components. The following topics are discussed:

 * Norton AntiVirus for Windows 95 user-definable install switches
 * Installing from LFN Folders      
 * Using LFN paths in the NNS file.
 * Don't append \ when specifying directories in the NNS file
 * Creating Norton AntiVirus Options Files for Distribution
 * File Extension Required for the Network Settings File
 * Limitation on changing NAV options to display AutoProtect icon
 * Novell NetWare Client 32 and the Activity Log File (Windows 95)
 * Additional Activity Log Settings
 * Troubleshooting Tips

To help us improve Norton AntiVirus for Windows NT, please send e-mail
with suggestions for enhancements to:

   navnt.feedback@symantec.com

Your comments are greatly appreciated.

**********************************************************************


Norton AntiVirus for Windows 95 user-definable install switches
---------------------------------------------------------------
switch88
 
  When set to FALSE (the default), the user can stop a scan 
  at any time during the install. When set to TRUE, the scan
  is forced and the user is not allowed to cancel it.

switch89

   When set to FALSE (the default), a full virus scan runs during
   install.

switch90

   When set to TRUE (the default), any Netscape plug-in for 
   scanning executable files is replaced by Norton AntiVirus.

switch91

   When set to FALSE (the default for a normal install), memory, boot
   records, and files are scanned. When set to TRUE, only memory is
   scanned.

   If running in silent mode (switch99=IsSilentMode), switch91 is, by 
   default, set to TRUE (only memory is scanned). This is done because
   the complete scan may take several minutes during which time only
   the blue setup screen is displayed. Users may wrongly think the 
   computer is locked up.
           
switch92

   When set to TRUE (the default), LiveUpdate files are installed.

For information about Norton AntiVirus for Windows NT user definable
switches, read the commented sections of the Norton AntiVirus for
Windows NT INSTALL.INF file.

Installing Norton AntiVirus from LFN Folders
--------------------------------------------
Norton AntiVirus cannot be installed from a long-filename (LFN) 
folder on a Novell NetWare server. Similarly, Norton AntiVirus
cannot be installed from a long-filename folder on a Windows NT
server if short-filename (SFN) aliasing is turned off.

Using LFN paths in the NNS file
-------------------------------
If you are using an LFN path on the ScannerCmdLine setting in the NNS
file, then the path must be enclosed in quotation marks, as follows.

ScannerCmdLine=""c:\program files""

             or 

ScannerCmdLine="/b- "c:\program files" /s-"

Don't append \ when specifying directories in the NNS file
----------------------------------------------------------
Doing so will cause problems under NT 3.51. Some of the distribution
components will have difficulty locating the files. 

Creating Norton AntiVirus Configuration Files for Distribution
--------------------------------------------------------------
NAVSTART.DAT retains the drive letter of the Windows directory of the
workstation it was created on. Though typically this will be "C:",
you should not distribute a NAVSTART.DAT file that you have created
on a workstation in which Windows has been installed to the D:
drive.

File Extension Required for the Network Settings File
-----------------------------------------------------
The network settings file (by default, _DEFLT32.NNS), must have a file
extension. If an extension is missing, it will not be processed by
NAV32UP.EXE.

Limitation on changing NAV options to display AutoProtect icon
--------------------------------------------------------------
You cannot modify the setting to show the AutoProtect icon on the
task bar or desktop by distributing the options file. This option
must be changed at the local machine.

Novell NetWare Client 32 and the Activity Log File (Windows 95)
--------------------------------------------------
Under Novell NetWare Client 32, the Network Manager cannot write to 
the activity log file if NAV32UP is run from a NetWare login script.  
If you want to write to the log, run NAV32UP from the workstation 
Startup group instead.  Don't forget to include the pathname for the 
network settings file with NAV32UP.

        <path>\NAV32UP.EXE  <path>\_DEFLT32.NNS

To disable the activity log, set ActivityLog=0 in the network settings
file (_DEFLT32.NNS).  

Additional Activity Log Settings
--------------------------------
By default, the Network Manager activity log is disabled 
(ActivityLog=0) in the network settings file (_DEFLT32.NNS).
It is intended for use as a debugging tool to resolve distribution 
problems and will not support use on a large network.  

   ActivityLog=0
        No activity log is generated.

   ActivityLog=1
        Records system error messages (for example, corrupted network
        settings files).
   
   ActivityLog=2
        Records system error messages as well as workstation events
        (for example, administrator-initiated scans and updates).

If you use the log, either set it to the workstation (for example, 
LogLocation=C:\NAVLOG.TXT) or use only a handful of workstations as
a test when writing to a network log.

Troubleshooting Tips
--------------------
 * If you're receiving generic errors while using Network Manager,
   verify that all files are present in the proper folders on the 
   server and that users have the proper access rights.  Simple as 
   it may seem, these two items solve almost all usage problems.

 * To use Network Manager, users need the following access rights 
   in NetWare:

     RF for the NAVNTSRC, NAVDEFS, NAVADMIN, and NAVNTOPT folders.

     RWCE for the NAVLOG folder.

 * Network Manager does not support high-ASCII characters in 
   pathnames (for example, the file locations specified in the 
   network settings file).

The next tip applies only to distributing to Windows 95 machines

 * If you are running Network Manager from a Windows NT server, don't
   run NAV32UP.EXE directly as the login script in the User Profile. 
   Instead, specify a batch file that runs NAV32UP.EXE.


Known issues with Impersonation
--------------------------------
At this time, administrator-level rights are still required on 
computers running Windows NT 3.51. Impersonation only works for
Windows 4.0 computers.

**********************************************************************
                                END OF FILE
**********************************************************************
