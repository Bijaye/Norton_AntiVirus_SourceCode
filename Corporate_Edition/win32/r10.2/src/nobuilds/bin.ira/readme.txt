*****************************************************************************
Symantec AntiVirus Corporate Edition 7.01                          README.TXT
Copyright 1999 - 2003 Symantec Corporation. All rights reserved. January 2000
*****************************************************************************

This README.TXT file covers compatibility information, 
late-breaking news, and usage tips for Symantec AntiVirus 
Corporate Edition 7.01. The following topics are discussed:

Install
-------
 * Installing over Symantec AntiVirus or Intel LANDesk 
   VirusProtect 
 * Installing over other AntiVirus products 
 * Installing the client over a server installation
 * Symantec AntiVirus Corprate Edition 7.01 and Symantec SystemWorks 
 * Symantec AntiVirus Corprate Edition 7.01 and Symantec Utilities
 * Administrator rights on Windows NT
 * Interrupted install
 * Upgrading from Symantec AntiVirus 4.X/5.0 to Symantec AntiVirus Corporate
   Edition
 * Re-installing over the installed Symantec AntiVirus on 
   Windows 9x
 * Re-installing Symantec AntiVirus on Microsoft Windows 2000 

Post Install
------------
 * No default scheduled scan on client
 * Symantec AntiVirus startup logo
 * Debug error on Windows 2000 client
 * Windows 2000 Multi-language feature 
 * Upgrading from Microsoft Windows 9x to Microsoft 
   Windows 2000 
 * cc:Mail 8.x email scanning 

Usage
-----
 * Scanning inside .CAB files 
 * Selected Types option for scans
 * Quarantine folder 
 * Manual scan on NTFS volumes
 * Realtime Protection and new ZIP drive
 * Symantec AntiVirus and Shutdown 
 * Symantec AntiVirus icon in Systray
 * Realtime Protection and compressed files 
 * Compressed files without extensions
 * Running LiveUpdate with normal user rights on 
   Windows NT
 * Formatting hard drive with SAV services running

Administrator Notes
-------------------
 * Parent server name at setup 
 * Reboot after install on Windows 9x
 * Uninstalling managed clients
 * Corporate Feature for Scan And Deliver 
 * Cluster server support
 * Symantec AntiVirus tray icon on Windows NT servers


**************************************************************
Install
**************************************************************

Installing over Symantec AntiVirus or Intel LANDesk VirusProtect 
----------------------------------------------------------------
Symantec AntiVirus Corporate Edition 7.01 Setup automatically 
uninstalls Symantec AntiVirus 4.0 or later and Intel LanDesk
VirusProtect 5.01 or later.  Uninstall earlier versions of
Symantec AntiVirus or LANDesk VirusProtect before installing
Symantec AntiVirus Corporate Edition 7.01.

Before you install Symantec AntiVirus Corporate Edition 7.01
over Symantec AntiVirus 4.0 on Windows NT machine, uninstall 
it or run LiveUpdate to download the latest patch, then 
reboot after installation of Symantec AntiVirus Corporate 
Edition 7.01.

Installing over other antivirus products
----------------------------------------
If you have other antivirus products on your machine, 
uninstall them before installing Symantec AntiVirus Corporate Edition 7.01.

Installing the client over a server installation
------------------------------------------------
Before installing the Symantec AntiVirus Corporate Edition client on a computer with Symantec AntiVirus Server already installed, uninstall the Symantec AntiVirus Server.

Symantec AntiVirus Corprate Edition 7.0 and Symantec SystemWorks 
----------------------------------------------------------------
If you install SystemWorks over Symantec AntiVirus Corprate
Edition 7.01, do not install Symantec AntiVirus in SystemWorks.

Symantec AntiVirus Corprate Edition 7.01 and Symantec Utilities
---------------------------------------------------------------
If you install Symantec AntiVirus Corporate Edition 7.01 over
other Symantec AntiVirus products with Symantec Utilities
2.03, run LiveUpdate to download the latest patch for Symantec Utilities.

Administrator rights on Windows NT
----------------------------------
Administrator rights are required to install Symantec AntiVirus 
Corporate Edition on Windows NT computers. If a standalone server is part of a domain, Domain Administrator rights are required to install.

Interrupted install
-------------------
If the install process is stopped anytime after the Start Copying Files dialog, you must reboot before installing again.

Upgrading from SAV 4.x/5.0 to SAV Corporate Edition
---------------------------------------------------
To upgrade from Symantec AntiVirus 4.x/5.0 to Symantec AntiVirus Corporate
Edition, you must run the Install rather than the Update program.

Re-installing over the installed Symantec AntiVirus on Windows 9x
-----------------------------------------------------------------
If you install Symantec AntiVirus client over an existing Symantec AntiVirus client, you must reboot the system when the install completes.

If the Symantec AntiVirus client needs to be reinstalled on a Windows 9x system that is running Outlook 97 or Outlook 98, either the existing Symantec AntiVirus client will need to be uninstalled prior to reinstall, or the Load Symantec AntiVirus Services checkbox needs to be unchecked in the main UI window. 

Re-installing Symantec AntiVirus on Microsoft Windows 2000 
----------------------------------------------------------
Symantec AntiVirus Corporate Edition 7.0 cannot be re-installed 
over an existing installation of Symantec AntiVirus Corporate
Edition 7.01 on Microsoft Windows 2000 computers. Uninstall
the current Symantec AntiVirus Corporate Edition before re-installing.

**************************************************************
Post Install
**************************************************************

No default scheduled scan on client
-----------------------------------
By default, scheduled scans are not enabled on the Symantec 
AntiVirus Corporate Edition client. To supplement Realtime Protection and ensure that your computer remains virus-free, schedule a scan to run once per week at the very least. A 
scheduled scan of all hard disks is a key component of virus protection.  

See your Symantec AntiVirus Corporate Edition documentation 
for instructions on how to schedule a scan.

Symantec AntiVirus startup logo
-------------------------------
Some users prefer not to see the startup logo screens that display when Symantec AntiVirus components load. To disable or enable the logos, set the DisableSplashScreen value in the following registry key:

HKEY_LOCAL_MACHINE\SOFTWARE\INTEL\LANDesk\VirusProtect6\CurrentVersion\

  DisableSplashScreen=0 Enable the startup logo screen
  DisableSplashScreen=1 Disable the startup logo screen


Debug error on Windows 2000 client
----------------------------------
Upon rebooting a Windows 2000 Professional (Build 2195) machine after installing the Symantec AntiVirus Corporate Edition client, a Symantec AntiVirus debug error sometimes appears. Click Continue and the program will load properly.


Windows 2000 Multi-language feature 
-----------------------------------
Symantec AntiVirus Corporate Edition 7.0 does not support the 
multi-language/multi-codepage feature of Microsoft Windows 2000.


Upgrading from Microsoft Windows 9x to Microsoft Windows 2000
-------------------------------------------------------------
Uninstall Symantec AntiVirus and reboot before upgrading from Windows 9x to Windows 2000. After the upgrade, reinstall Symantec AntiVirus. Symantec AntiVirus will not operate properly unless it is uninstalled before the upgrade.

cc:Mail 8.x email scanning
--------------------------
cc:Mail version 8.x uses MAPI. During the cc:Mail 8.x install, if Microsoft Exchange or Microsoft Outlook clients are not installed on the computer, cc:Mail automatically installs the Microsoft Exchange client. 

Symantec AntiVirus Corporate Edition 7.01 setup detects both cc:Mail and Exchange on the computer and installs the email scanning components for Exchange and cc:Mail, even though only cc:Mail is used.

When an infected compressed attachment is received and cc:Mail 
RealTime Protection options are set to clean or delete, the virus alert dialog indicates clean or delete succeeded but the modified e-mail message indicates clean and delete operations failed. E-mail message modification can be turned off to void confusion.

*****************************************************************
Usage
*****************************************************************

Scanning inside .CAB files
--------------------------
By default, Symantec AntiVirus Corporate Edition 7.0 does not scan inside .CAB format compressed files. 

To enable .CAB file scanning:

 * Copy DEC2CAB.DLL to the folder where Symantec AntiVirus Corporate 
   Edition 7.01 is installed.

DEC2CAB.DLL is located in the same folder as SETUP.EXE.

Selected Types option for scans
-------------------------------
If the File Types scan option is set for Selected Types/Program Files and Advanced options are set to Scan Files Inside Compressed Files, compressed files are considered program files. Files within the compressed file are scanned, regardless of type.

Quarantine folder 
-----------------
If the number of quarantined files grows too large, scanning speed
may decrease. 

 To delete files from Quarantine:
 1 Select View > Quarantine.
 2 Highlight one or more files in the list.
 3 Right-click and choose Delete Permanently.

Manual scan on NTFS volumes
---------------------------
On an NTFS volume, Administator or Everyone-Full Control access is required to scan.  Other access rights are not sufficient. Folders or files will not be scanned without Administators or Everyone-Full Access access.

Realtime Protection and new ZIP drive
-------------------------------------
Some driver software for ZIP drives does not require a reboot after
installing a new drive. You must, however, reboot to enable Realtime Protection to catch boot sector viruses on the new ZIP drive.

Symantec AntiVirus and Shutdown 
-------------------------------
Symantec AntiVirus displays an alert message if a disk is found in 
the floppy drive when you shut down the computer. Do not unload SAV 
Services while this message is displayed.

Under Windows 2000, you can access the Control Panel after initiating Shutdown from the Start menu. Your computer may fail to shut down if you try to stop SAV service.

Symantec AntiVirus icon in Systray
----------------------------------
If you see yellow exclamation mark over Symantec AntiVirus icon in 
systray, Symantec AntiVirus Service is not loaded properly. To reload
Symantec AntiVirus Services, check "Load Symantec AntiVirus Services" 
in the right pane of main UI, or reboot the system. 

Realtime Protection and compressed files 
----------------------------------------
In File System Realtime Protection>Selected Extensions, extensions of compressed files (ZIP, AMG, ARJ, etc.) are included to be scanned. Realtime protection scans the compressed files, but does not scan files within the compressed files.  

Files within compressed files are scanned, by default, with on-demand, scheduled, custom, and startup scans, as well as
email Realtime Protection. In addition, File System Realtime Protection scans the files as they are extracted from compressed files.

Compressed files without extensions
-----------------------------------
Symantec AntiVirus may fail to detect compressed files without 
extensions.

Running LiveUpdate with normal user rights on Windows NT
--------------------------------------------------------
On Windows NT NTFS partitions, read and write access is required 
to the root directory of the boot drive. If LiveUpdate is run
from a normal user account that does not have these rights, LiveUpdate will fail.

If the root directory of the boot drive is a FAT partition, or
if definitions on the client are updated directly from the
internal LiveUpdate server, LiveUpdate will succeed.

Formatting hard drive with SAV services running
-----------------------------------------------
On Windows NT, you cannot format your hard drive while Symantec AntiVirus Service is running. You need to stop the service to format your hard drive.

 To stop Symantec AntiVirus Services:

 * Open Control Panel/Services
 * Highlight Symantec AntiVirus Service
 * Click Stop button

After you format the hard drive, you need to restart SAV Service to 
protect your system.

******************************************************************
Administrator Notes
******************************************************************

Parent server name at setup
---------------------------
When a standalone installation of a Symantec AntiVirus Corporate Edition client is begun, Setup requests the server name for managed clients.
 
If you intend to manage the client through a Symantec AntiVirus server, inform end users of the server name. 

Reboot after silent install on Windows 9x
-----------------------------------------
For proper operation, reboot after installing on Windows 9x computers.

When Symantec AntiVirus is installed silently, the reboot is not forced.

On Windows NT computers, a reboot is not required, but is recommended.

Uninstalling managed clients
----------------------------
By default, a password is required to uninstall the Symantec AntiVirus Client managed by Symantec System Center. The password 
is set in the Client Administrator Only Options task of the Symantec System Center.

After install, "symantec" is the default password.

Corporate Feature for Scan And Deliver 
--------------------------------------
There is a registry key to control Scan and Deliver for corporate users.

HKLM\Software\Symantec\Symantec AntiVirus\
CorporateFeatures=1 Enabled Corporate Features

This key enables Corporate information panel in Scan and Deliver, and allows you send more than one file that does not appear to be infected.

By default, the value is set to 1 to enable Corporate Features. 
To disable this features, delete this key.

Cluster server support
----------------------
The Symantec AntiVirus Corporate client is only supported on cluster servers locally. There should be an instance of the client on each system. DO NOT install the client to the shared drives.

Administrators should not install either the client or the server through the rollout mechanism.

A SAV client must be installed on each local system that is part of the cluster server.

To roll out clients, use the local server names and not the shared cluster name.

Manageability of the clients can be synchonized if they are children of the same parent server and configuration is performed at the server level.

The shared drives are protected in real time by each system's Auto-Protect when that system has control of the drives. When control of the shared drives is passed to another system, its Auto-Protect automatically takes over the protection.

If a manual scan of the shared drives is being performed when a fail over occurs, the scan itself will not restart on the new system. A new scan will need to be initiated.

If one system in the cluster is down, it will receive the latest virus definitions when RTVScan starts back up and it checks in with the parent.

Logging and alerting will include the name of the local system and not the cluster server name. This will help to identify which system had the event.


Symantec AntiVirus tray icon on Windows NT server
-------------------------------------------------
Symantec AntiVirus icon will not show after an Symantec AntiVirus Server install. The Symantec AntiVirus icon will not show because install runs vptray.exe in silent mode.  This is only when vptray is executed during install. Normally, VPTRAY is execute from the RUN key by windows at startup.  This normal execution is not affected by this change.  This means that the icon will not show after install until the machine is restarted.

*****************************************************************
                                END OF FILE
*****************************************************************

