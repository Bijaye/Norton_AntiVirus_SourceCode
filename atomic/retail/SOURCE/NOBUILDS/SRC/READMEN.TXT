**********************************************************************
Norton AntiVirus for Windows 95 - NEC                       README.TXT
Copyright (c) 1995-1997 Symantec Corporation              October 1997

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

 * Print versions of Norton AntiVirus Guides
 * Options settings from earlier versions of Norton AntiVirus
 * NAVDX and Long Filenames (LFNs)
 * Inoculation Change Alerts
 * AUTOEXEC.BAT and NAVDX.EXE
 * CONFIG.SYS and NAVDX.EXE
 * Automatic Protection and Downloading Software
 * Windows 95 System Shutdown and Auto-Protect
 * Novell Client32 for Windows 95
 * "HIMEM.SYS is missing" Message when Restarting Your Computer
 * Named Pipes and Auto-Protect
 * Creating a test text file that looks like a virus
 * Using Netscape with Norton AntiVirus
 * Changing LiveUpdate automatic schedules
 * Chips and Technologies display driver problem
 * NAVDX and ZIP drives
 * Preventing the Norton AntiVirus Startup Logo Screen from displaying
 * "IOS Failed to Initialize" error message after installation

To help us improve Norton AntiVirus for Windows 95, please send email
with suggestions for enhancements to:

   nav40feedback@symantec.com

Your comments are greatly appreciated.
**********************************************************************

Print versions of Norton AntiVirus Guides
-----------------------------------------
Print versions of the Norton AntiVirus User and Implementation Guides
can be ordered through Symantec Customer Service.

Options settings from earlier versions of Norton AntiVirus
----------------------------------------------------------
Options settings from earlier versions of Norton AntiVirus are not
preserved when you install Norton AntiVirus 4.0.  The new version
stores options settings in a different format.

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

 * Program Files

   If you use inoculation protection for program files, Norton
   AntiVirus reports an inoculation change when the newly installed
   file is accessed (or during a manual scan). Choose Inoculate, not
   Repair, to respond to the alert. In this case, the inoculation
   change is legitimate because it is a new file.

AUTOEXEC.BAT and NAVDX.EXE
--------------------------
During installation, an entry is placed in your AUTOEXEC.BAT file to
run NAVDX for startup scans, which run before Windows starts (for 
example, @A:\PROGRA~1\NORTON~1\NAVDX.EXE /STARTUP). Whether or not 
the startup scan actually takes place is determined by your Startup 
tab settings in the Norton AntiVirus Options window. If, however, you 
remove the NAVDX line from your AUTOEXEC.BAT file, the startup scans 
can never occur, irrespective of your Startup tab settings.

NAVDX is loaded from AUTOEXEC.BAT rather than WINSTART.BAT to
ensure that it scans as early as possible during the system startup
sequence.

CONFIG.SYS and NAVDX.EXE
------------------------
During installation, Norton AntiVirus checks your system's CONFIG.SYS
file for a line that loads the EMM386 memory manager.  If such a line
is in the file, it is remarked out so that EMM386 does not load.  This
is done to avoid reported problems with NAVDX on machines with 32 MB
or more of RAM.

If your system has less than 32 MB, you can edit CONFIG.SYS to remove
the remark that Norton AntiVirus adds so that EMM386 loads as before.  If
you do this, you must remove any switches and parameters to EMM386, and
replace them with the /DPMI switch.  For example, you should change

device=a:\windows\emm386.exe noems ram

to

device=a:\windows\emm386.exe /DPMI

NAVDX will not run if you load EMM386 without the /DPMI switch.

Automatic Protection and Downloading Software
---------------------------------------------
If you configure Norton AntiVirus to monitor for virus-like activities
or use the Virus Sensor feature, you may get unknown virus alerts when
downloading program files from some online services. These alerts do
not necessarily mean a file is infected with an unknown virus. The
alert may be generated due to the way the file is transferred.

For example, the CompuServe WinCIM program creates a file the size of
the expected download when you begin a file transfer. During the
transfer, the contents of this file are replaced with the data being
downloaded. If you are downloading an executable program,
Auto-Protect will properly notice that the file is being modified and
alert you.

Simply select Continue when the alert is generated to complete the
download. If you frequently download programs, you can add exclusions
for this behavior so that the alerts are not generated at all. For
example, you could add exclusions for WinCIM for "Unknown virus
detection" and "Write to program files." 

Windows 95 System Shutdown and Auto-Protect
-------------------------------------------
Auto-Protect is often configured to scan removable media devices for
boot sector viruses during system shutdown. While scanning the boot
sector, Auto-Protect displays a text mode message that notes it is
currently scanning a boot record. Some video boards and video drivers
have problems switching to text mode after the shutdown screen is 
displayed and cause a system lockup.

You can control whether or not the text message is displayed by
merging the following Registry Entry files with your Registry:

   APMSGOFF.REG      Prevents the AP text message from displaying
   APMSGON.REG       Permits the AP text message to be displayed

To merge a Registry Entry file with your Registry, simply double
click the file from the Explorer or a My Computer window. Both files
are located where Norton AntiVirus is installed. By default, this is
the A:\Program Files\Norton AntiVirus folder.

Note that if a virus is found, Auto-Protect will attempt to display a
message irrespective of this setting.

Also note that, if you use the default configuration, the text mode
message displayed when the Auto-Protect is scanning removable media
devices will have red letters on a black background, instead of
the normal colors.  This is normal, and is not related to the video
driver problems mentioned above.

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

Creating a test text file that looks like a virus
-------------------------------------------------
To create a harmless text file that will be detected as a virus,
which you can use to verify detection of viruses, logging, and
alert functioning, copy the following line in to a separate file,
saving it as TRIGGER.COM. This file is not a virus, but will be
detected as the "EICAR Test String.70" virus. Disable Auto-Protect
temporarily before you save the file.

X5O!P%@AP[4\PZX54(P^)7CC)7}$EICAR-STANDARD-ANTIVIRUS-TEST-FILE!$H+H*

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
A:\Program Files\Norton AntiVirus folder. 

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
**********************************************************************
                                END OF FILE
**********************************************************************
