**********************************************************************
Norton AntiVirus for Windows 95/98                          README.TXT
Copyright (c) 1995-1998 Symantec Corporation             November 1998

**********************************************************************
HOW TO USE THIS DOCUMENT
**********************************************************************
To view README.TXT on screen in Notepad or WordPad, maximize the
Notepad or WordPad window.

To print README.TXT in Notepad or WordPad, choose Print from the File
menu.  If you use another word processor, select the entire document
and format the text in 10-point Courier before printing to ensure
proper spacing.

To print README.TXT from the DOS prompt, type COPY README.TXT PRN:

**********************************************************************
NORTON ANTIVIRUS NOTES
**********************************************************************
This README.TXT file covers compatibility information, late-breaking
news, and usage tips for Norton AntiVirus for Windows 95. The
following topics are discussed:

 * Upgrading from Microsoft Windows 95 to Windows 98
 * Inoculation Alerts when upgrading to Microsoft Windows 98
 * LiveUpdate subscriptions
 * Print versions of Norton AntiVirus Guides
 * Uninstall Beta versions of Norton AntiVirus
 * Uninstalling LiveUpdate
 * Removing Norton AntiVirus from the Windows Start menu
 * Options settings from earlier versions of Norton AntiVirus
 * Scanning Removable Drives
 * Installing Norton AntiVirus and Microsoft Outlook 98
 * Novell NetWare: Installing from long filename (LFN) directories
 * Quarantining non-deletable files
 * NAVDX and Long Filenames (LFNs)
 * Inoculation Change Alerts
 * AUTOEXEC.BAT and NAVDX.EXE
 * Automatic Protection and Downloading Software
 * Auto-Protect (NAVAPW32.EXE, NAVAP.VxD, and SYMEVNT.386)
 * Intel platform only: Modifying Rescue Disks
 * Scheduling scans for Windows 95
 * Win 95/98 System Shutdown and Auto-Protect
 * Novell Client32 for Windows 95
 * "HIMEM.SYS is missing" Message when Restarting Your Computer
 * Named Pipes and Auto-Protect
 * Using Netscape with Norton AntiVirus
 * Changing LiveUpdate automatic schedules
 * Chips and Technologies display driver problem
 * NAVDX and ZIP drives
 * Preventing the Norton AntiVirus Startup Logo Screen from displaying
 * "IOS Failed to Initialize" error message after installation
 * Network application performance issues with Auto-Protect running
 * NEC platform only: CONFIG.SYS and NAVDX.EXE
 * Scanning Magneto-Optical (MO) drives larger than 640 MB
 * Updating Windows 95/98 video drivers for Auto-Protect
 * Mijenix ZipMagic and Norton AntiVirus installation
 * Internet access problems with LiveUpdate
 * Additional information for system administrators 
 * Activity Log Access Denied option
 * Online manuals: "Cannot find or run file" error message

**********************************************************************

Upgrading from Microsoft Windows 95 to Windows 98
-------------------------------------------------
Windows 98 stops and restarts your computer during a Windows 98
upgrade. Windows 98 may not restart properly if anti-virus programs
are running. For example, if Norton AntiVirus Auto-Protect is running,
Windows 98 Setup will not restart your computer automatically during
the upgrade.

If your computer stops and doesn't restart when it is supposed to
during the upgrade, turn the power off and then on again to restart
your computer manually. Windows 98 Setup will then properly continue.

Inoculation Alerts when upgrading to Microsoft Windows 98
---------------------------------------------------------
If you receive an Inoculation Alert during a virus scan after
upgrading from Microsoft Windows 95 to Windows 98, choose Inoculate
to respond to the alert. Do not choose Repair. In this case, the
inoculation change is expected because of the Windows upgrade. If
you choose Repair, you will corrupt the data on your disk.

LiveUpdate subscriptions
------------------------
With Norton AntiVirus 5.0, you automatically qualify for a limited
time, free use of LiveUpdate to receive virus definition updates.  For
more information, please review the Product Support information topic:

  * On the Windows Taskbar, click Start, point to Programs, point
    to Norton AntiVirus, and click Product Support. In the help window
    that appears, click Product Support.

Your installed Norton AntiVirus program will notify you when your 
subscription is about to expire and provide information on how to
extend your subscription to ensure continuing, up-to-date protection.

Print versions of Norton AntiVirus Guides
-----------------------------------------
Print versions of the Norton AntiVirus User and Implementation Guides
can be ordered through Symantec Customer Service.

Uninstall Beta versions of Norton AntiVirus
-------------------------------------------
If you tested a Beta version of Norton AntiVirus, uninstall the Beta
before installing the release version.  This removes unneeded files
that would otherwise remain on your disk and prevents problems that
could arise due to changed file formats.

Uninstalling LiveUpdate
-----------------------
Because LiveUpdate is shared among Symantec products, it is not
uninstalled automatically if you select Uninstall Norton AntiVirus.

To uninstall LiveUpdate, choose Add/Remove Programs from the Windows
Control Panel and select LiveUpdate.

Removing Norton AntiVirus from the Windows Start menu
-----------------------------------------------------
If Uninstall does not remove the Norton AntiVirus program group from
the Windows Start menu, you must remove it manually.  To remove the
program group, right-click the taskbar, select Properties, click the
Start Menu Programs tab, click Remove, select Norton AntiVirus, and
then click Remove.

Options settings from earlier versions of Norton AntiVirus
----------------------------------------------------------
Options settings from earlier versions of Norton AntiVirus are not
preserved when you install Norton AntiVirus 4.0.  The new version
stores options settings in a different format.

Scanning Removable Drives
-------------------------
Norton AntiVirus incorrectly identifies removable hard drives as fixed
drives. Removable drives (such as Zip drives) show up as local hard
drives even when the "all removable drives" checkbox is not checked.

Installing Norton AntiVirus and Microsoft Outlook 98
----------------------------------------------------
If Microsoft Outlook 98 is running during Norton AntiVirus
installation, the install will take a long time. Close Microsoft
Outlook 98 before installing Norton AntiVirus.

Novell NetWare: Installing from long filename (LFN) directories
---------------------------------------------------------------
Norton AntiVirus cannot be installed from long filename (LFN)
directories under Novell NetWare.

Quarantining non-deletable files
--------------------------------
Norton AntiVirus cannot quarantine non-deletable files (for example,
files infected by the DirII virus). To manually add these types of
files to the Quarantine, open the Quarantine and choose Add Item.

NAVDX and Long Filenames (LFNs)
-------------------------------
NAVDX, the command-line scanner used for startup scans and emergency
recovery, does not properly display long filenames in a DOS box.

Inoculation Change Alerts
-------------------------
When responding to an inoculation change alert, you must determine if
the change is legitimate (choose Inoculate to let Norton AntiVirus
generate new inoculation data) or the change indicates the activity
of a virus (choose Repair to let Norton AntiVirus restore the item).

The following examples demonstrate legitimate changes. In these cases
you should choose Inoculate to let Norton AntiVirus generate new
inoculation data.

 * Installing or Upgrading Windows

   If you are running Norton AntiVirus and reinstalling or upgrading
   Windows, you may receive more than one inoculation change alert.
   Choose Inoculate, not Repair, to respond to the alert.

   Windows modifies boot records during the install operation and
   restarts your system more than once. Each time your system is
   restarted, these changes are properly detected by Norton AntiVirus.
   If you choose Repair, you are undoing some of the changes that
   Windows is making and, therefore, corrupting your boot records. You
   may not be able to start your system from its hard disk. If this
   occurs, start up from a floppy disk and reinstall Windows.

 * Partitioning Software

   If you use partitioning software (such as Partition Magic) or
   drive overlay software (such as Disk Manager and EZ drive), you
   may receive inoculation alerts for legitimate changes. When you
   set or modify partitions after Norton AntiVirus is installed,
   choose Inoculate, not Repair, to respond to these alerts.

   For example, Partition Magic may generate several legitimate
   inoculation alerts when setting or modifying partitions. If you
   choose Repair, your drive may be rendered inaccessible with little
   chance of recovery.

AUTOEXEC.BAT and NAVDX.EXE
--------------------------
During installation, an entry is placed in your AUTOEXEC.BAT file to
run NAVDX for startup scans, which run before Windows starts (for
example, @C:\PROGRA~1\NORTON~1\NAVDX.EXE /STARTUP). Whether or not
the startup scan actually takes place is determined by your Startup
tab settings in the Norton AntiVirus Options window. If, however, you
remove the NAVDX line from your AUTOEXEC.BAT file, the startup scans
can never occur, irrespective of your Startup tab settings.

NAVDX is loaded from AUTOEXEC.BAT rather than WINSTART.BAT to
ensure that it scans as early as possible during the system startup
sequence.

Automatic Protection and Downloading Software
---------------------------------------------
If you configure Norton AntiVirus to monitor for virus-like activities,
you may get unknown virus alerts when downloading program files from
some online services. These alerts do not necessarily mean a file is
infected with an unknown virus. The alert may be generated due to the
way the file is transferred.

For example, the CompuServe WinCIM program creates a file the size of
the expected download when you begin a file transfer. During the
transfer, the contents of this file are replaced with the data being
downloaded. If you are downloading an executable program,
Auto-Protect will properly notice that the file is being modified and
alert you.

Simply select Continue when the alert is generated to complete the
download. If you frequently download programs, you can add exclusions
for this behavior so that the alerts are not generated at all. For
example, you could add exclusions for WinCIM for "Write to program
files."

Auto-Protect (NAVAPW32.EXE, NAVAP.VxD, and SYMEVNT.386)
-------------------------------------------------------
Auto-Protect and Inoculation ignore disk label (volume label) changes
to prevent false virus alerts. Viruses cannot use the disk label to
infect your system. For the same reason, changes to the OEM ID in a
boot record are not reported.

By design, Auto-Protect does not report writes to a boot sector or
master boot record if the new sector is identical to the one
previously on the drive. For example, you may use a disk editor to
write back the same data to a boot sector or master boot record.

Intel platform only: Modifying Rescue Disks
-------------------------------------------
Due to the number of product specific technologies used by
manufacturers to configure and initialize hard disks, Norton
AntiVirus cannot always create a bootable Norton Rescue Boot Disk
automatically.

To test your rescue disk, restart your computer from rescue disk 1,
"Norton Rescue Boot Disk," and type C: to change to your hard drive.
If you can access your hard drive, you can rely on the rescue disk in
an emergency situation. Write-protect the disk and store it in a safe
place.  If, however, you cannot pass this test, you must modify the
disk.

In all cases, restart your computer from the newly modified rescue
disk to verify proper operation.

 * Modifying a Norton Rescue Boot Disk for Ontrack Systems

   1 Choose Shutdown from the Start menu and select "Restart the
     computer in MS-DOS mode."
   2 After your computer restarts, insert your Ontrack Disk Manager
     disk in the A: drive.
   3 Type A:\DM and press Enter to run the Disk Manager program.
   4 Choose "Maintenance Menu" from the main window Select an
     Installation menu.
   5 Choose "Create Ontrack Boot Diskette."
   6 Choose "Make this diskette an Ontrack Boot Diskette."
   7 Remove the Ontrack Disk Manager disk and insert rescue disk 1,
     "Norton Rescue Boot Disk," in the A: drive. Make sure the disk
     is not write-protected.
   8 Press Enter to accept the default switches (/P-).
   9 When the operation is completed, write-protect the rescue disk.
  10 Restart your computer using you Norton Rescue Boot Disk and
     type C: to change to your hard drive to verify proper operation.

 * Modifying a Norton Rescue Boot Disk for embedded driver systems

   Because embedded driver implementations are hard drive specific,
   you must refer to the documentation that came with your hard disk
   for instructions on how to create a bootable floppy disk that
   gives you access to the hard disk. In some cases, you will have
   to contact the manufacturer's technical support department.

Scheduling scans in Windows 95
------------------------------
Norton AntiVirus for Windows 98 and Windows 95 use different
schedulers. The Windows 98 version uses the new built-in Windows
scheduler, while the Windows 95 version uses the Norton Program
Scheduler.

In Windows 95, your computer must be turned on and Norton Scheduler 
must be running when the scan is due to take place. If not, the
scheduled scan won't run.

System Shutdown and Auto-Protect
--------------------------------
Auto-Protect is often configured to scan removable media devices for
boot sector viruses during system shutdown. While scanning the boot
sector, Auto-Protect can display a text mode message that notes it is
currently scanning a boot record. Some video boards and video drivers
have problems switching to text mode after the shutdown screen is
displayed and cause a system lockup. Because of this, we have disabled
this behavior by default.

You can control whether or not the text message is displayed by
merging the following Registry Entry files with your Registry:

   APMSGOFF.REG      Prevents the AP text message from displaying
   APMSGON.REG       Permits the AP text message to be displayed

To merge a Registry Entry file with your Registry, simply double
click the file from the Explorer or a My Computer window. Both files
are located where Norton AntiVirus is installed. By default, this is
the C:\Program Files\Norton AntiVirus folder.

Note that if a virus is found, Auto-Protect will attempt to display a
message irrespective of this setting.

Novell Client32 for Windows 95
------------------------------
A problem has been identified with the initial release of the Novell
Client32 for Windows 95 and Auto-Protect. Under some circumstances,
Auto-Protect can miss infected files when files are going between a
NetWare 4.1 server and a Windows 95 client running the Novell Client32
software. Contact Novell for an upgraded version of Client32.

"HIMEM.SYS is missing" Message when Restarting Your Computer
------------------------------------------------------------
On certain computers, you may see the following message when
restarting your computer:

   HIMEM.SYS is missing.
   Make sure that the file is in your Windows directory.

This error occurs only if you restart your computer with a warm boot
(Ctrl-Alt-Del). If you turn the power off and then on again, or press
the reset button, your computer starts properly.

To prevent the problem from occurring, add the following line to the
beginning of your AUTOEXEC.BAT file:

   SET DOS16M=2

After making the modification and saving your AUTOEXEC.BAT file, turn
the power off and then on (or press the reset button) to restart your
computer.

Named Pipes and Auto-Protect
----------------------------
Auto-Protect is not compatible with the named pipes protocol, used by
products such as Microsoft SQL Server 6.0. To prevent Auto-Protect
scans of pipe files, select the Program Files option (the default) in
the What To Scan group box on the Options/Auto-Protect tab. Do not
select the All Files option. Contact technical support if you want to
be notified when the program fix becomes available.

Using Netscape with Norton AntiVirus
------------------------------------
If Norton AntiVirus is installed as a Netscape helper application, a
a downloaded file will be lost if Netscape is closed before the
download is complete.

Changing LiveUpdate automatic schedules
---------------------------------------
The first time that LiveUpdate completes a successful session, it
offers to schedule future sessions automatically. If you want to
modify or delete the scheduled sessions, access the Norton Program
Scheduler from the Norton AntiVirus group on the Start menu or the
Norton AntiVirus main window.

Chips and Technologies display driver problem
---------------------------------------------
A problem with the Chips and Technologies display driver, which is
common in Toshiba laptops, may cause a system hang after an
Auto-Protect alert is generated.  Changing to a standard SVGA driver
avoids the problem.

NAVDX and ZIP drives
--------------------
Some ZIP drives require that a disk be present when they are started.
You may see an "Invalid Drive type on drive <ZIP drive>" with NAVDX,
the Norton AntiVirus component that performs startup scans and scans
in emergency situations, if no disk is in the ZIP drive. Insert a disk
in the drive and choose "Retry."

Preventing the Norton AntiVirus Startup Logo Screen from Displaying
-------------------------------------------------------------------
Some users prefer not to see the startup logo screens that display when
Norton AntiVirus components are loaded. They can be disabled or enabled
by merging the following Registry Entry files with your Registry:

   LOGO_OFF.REG      Disables the startup logo screen
   LOGO_ON.REG       Enables the startup logo screen

To merge a Registry Entry file with your Registry, simply double click
the file from the Explorer or a My Computer window. Both files are
located where Norton AntiVirus is installed. By default, this is the
C:\Program Files\Norton AntiVirus folder.

"IOS Failed to Initialize" error message after installation
-----------------------------------------------------------
When you restart Windows 95 after installing a program or making a
configuration change to your computer, you may receive one of the
following error messages:

 * Windows initializing device IOS: Windows protection error. IOS
   failed to Initialize, Please restart
 * While initializing IOS: Windows protection error. You need to
   restart your computer.

This is a Windows 95 problem that occurs on a very small number of
system configurations. After the error message is displayed, you may
not be able to start Windows 95 normally. However, you should be able
to start Windows 95 in Safe mode.

To correct the problem:

 1 Boot Windows 95 in Safe mode or to a command prompt.
 2 Do one of the following:
   * Edit your CONFIG.SYS and AUTOEXEC.BAT files and remove or
     disable any references to SMARTDRV.EXE
   * Rename SMARTDRV.EXE to another name (for example, SMARTDRV.EX?).

For more information, visit the Microsoft Knowledge Base and review
article Q157924.

Network application performance issues with Auto-Protect running
----------------------------------------------------------------
Auto-Protect by default, opens files on a network drive in shared
mode. This can increase the time it takes for an application to load
over the network. To speedup load time of applications, you can
configure Auto-Protect to open files in non-shared mode. To do this,
double-click the AP_FAST.REG file located in the Norton AntiVirus
folder, then restart your computer.

If sharing violations occur after setting Auto-Protect to non-shared
mode, reset Auto-Protect to shared mode. To do this, double-click the
AP_NORM.REG file, then restart your computer.

NEC platform only: CONFIG.SYS and NAVDX.EXE
-------------------------------------------
During installation, Norton AntiVirus checks your system's CONFIG.SYS
file for a line that loads the EMM386 memory manager. If such a line
is in the file, it is remarked out so that EMM386 does not load. This
is done to avoid reported problems with NAVDX on machines with 32 MB
or more of RAM.

If your system has less than 32 MB, you can edit CONFIG.SYS to remove
the remark that Norton AntiVirus adds so that EMM386 loads as before.
If you do this, you must remove any switches and parameters to EMM386,
and replace them with the /DPMI switch. For example, you should change

   device=a:\windows\emm386.exe noems ram
to
   device=a:\windows\emm386.exe /DPMI

NAVDX will not run if you load EMM386 without the /DPMI switch.

Scanning Magneto-Optical (MO) drives larger than 640 MB
-------------------------------------------------------
Norton AntiVirus does not properly scan boot records on magneto-
optical (MO) drives larger than 640 MB.

To disable boot record scans of MO drives larger than 640 MB:

 * Start Norton AntiVirus, click Options, click the Scanner tab,
   and then uncheck Boot Records in the What To Scan group.

Updating Windows 95/98 video drivers for Auto-Protect
-----------------------------------------------------
Some versions of Windows 95/98 video drivers cause display corruption
or an apparent system lock up when an Auto_protect alert is generated.
To correct the problem, upgrade to the latest version of the video
driver.

Mijenix ZipMagic and Norton AntiVirus installation
--------------------------------------------------
Norton AntiVirus should be installed before ZipMagic is installed.
If ZipMagic is installed first, you must change a ZipMagic option
setting for proper operation.

To configure an already installed ZipMagic:

 * Access ZipMagic options and set up Norton AntiVirus as a program
   that recognizes ZipMagic archives as files, not as folders.

For more information, consult your ZipMagic documentation.

Internet access problems with LiveUpdate
----------------------------------------
LiveUpdate operation may be restricted because of firewalls or proxy
servers.  Contact your network administrator if you have problems
using LiveUpdate due to your network security policy.

Additional information for system administrators 
------------------------------------------------
If you're a system administrator and would like information about
Norton AntiVirus Administration Tools or other Corporate products,
please visit our website at:

    http://www.symantec.com/solutions/navcorp.html

Activity Log "Access Denied Errors" option
------------------------------------------
The Activity Log includes a new option to report attempts to scan when
access is denied.  For example, an administrator may try to scan a
folder to which you have prevented others from accessing by setting
access permissions.  Norton AntiVirus will report in the Activity Log
that an attempt was made to scan your restricted-access folder. 

Online manuals: "Cannot find or run file" error message
-------------------------------------------------------
You must install Adobe Acrobat Reader to view the Norton AntiVirus
online manuals. 

To install Adobe Acrobat Reader:

  1 On the Windows Taskbar, click Start, point to Programs, 
    point to Norton AntiVirus, and click Norton AntiVirus to open
    the Norton AntiVirus main window.

  2 From the Help menu, point to Product Support Online, and click 
    Product Support.

  3 In the Product Support help window that appears, click 
    Install Adobe Acrobat Reader.

**********************************************************************
                                END OF FILE
**********************************************************************
