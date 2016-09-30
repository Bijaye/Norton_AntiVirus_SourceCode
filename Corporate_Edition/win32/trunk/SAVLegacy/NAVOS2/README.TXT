**************************************************************
Norton AntiVirus for OS/2 V5.03.69A              README.TXT
Copyright (c) 1995-1999 Symantec Corporation    October 2000
**************************************************************

This README.TXT file covers compatibility information,
late-breaking news, and usage tips for Norton AntiVirus
for OS/2. The following topics are discussed:

SYSTEM REQUIREMENTS

INSTALLATION ISSUES
 * Installing directly from the CD-ROM
 * Uninstalling IBM AntiVirus for OS/2
 * CID Enablement
 * Preparing for silent installations
 * Post-Installation
 * Reinstalling over the installed Norton AntiVirus for OS/2
 * Uninstalling and removing installed files

PLATFORM ISSUES
 * Auto-Protect Coverage
 * Auto-Protect behavior in full-screen DOS or Win-OS/2 sessions
 * Monitor resolution for Auto-Protect alerts
 * Warp 4 Client printing
 * Inoculation component
 * LiveUpdate activity and Desktop Response

FIXES FOR THIS RELEASE
 * Command line scanner reports garbage for virus description
 * Command line scanner unable to repair boot sector/MBR viruses

ENHANCEMENTS FOR THIS RELEASE
 * "Virus library initialization failed code 2" running under
   Workspace on Demand
 * Ability to disable <Ctrl-C> and <Ctrl-Break> during command-
   line and scheduled scans

FIXES FROM PREVIOUS RELEASES
 * Install failed when no C drive exists
 * Upgrading from versions prior to 5.02.45 to 
     version 5.02.45 and later moved files off the boot drive.
 * Virus Definitions being reinstalled to wrong locations
 * Fixed Floppy Disk installation failure
 * Fixed Auto-Protect failing to launch some applications 
   from the dos command line
 * Norton AntiVirus crashing when scanning some corrupt 
   self-extracting zip files
 * Bogus Virus Library error in Command Line scanner during
   boot sector scanning
 * Command-line scanner - File names with % characters 
 * Command-line scanner - Bogus virus names
 * Increased the # of user configurable file extensions allowed
 * NAVDXOS2 and ARJ files contained in ARJ files
 * Crashing on corrupted zip files
 * Silent installs fail to do upgrades
 * Master Boot Record Scan Reporting
 * "Assertion Failed" Message
 * "Drive not ready" Messages
 * Japanese Desktop file scanning
 * Chinese long file and directory names
 * Uninstalling IBM AV 2.4
 * 24-hour Clock Scheduling
 * Scheduler in the Startup Folder
 * AutoProtect speed improvement

ENHANCEMENTS FROM PREVIOUS RELEASES
 * Updated Virus Definitions to 9/22/99
 * Virus definition installation location can be configured 
 * Launching a user defined application when a virus is found
 * Floppy Disk Install
 * Scheduler has a delay at startup
 * Scheduled Scans Now Use the Command Line Scanner
 * Melissa Virus Coverage
 * longmsg.txt File
 * Autorun of Missed Scheduled Events
 * New Command Line Scanner Options
 * New Command Line Scanner Return Codes

MISCELLANEOUS ISSUES
 * Auto-Protect performance
 * Scanning with Disabled Removable Drives
 * Command-line scanner and open files
 * Changing the Heuristics (bloodhound technology) setting
 * Reading the log created by NAVDXOS2.EXE
 * Virus List printouts
 * Folder Visibility in the Scan Folder window
 * Auto-Protect "Duplicate File Name" message
 * Auto-Protect disabled if Terminate Program selected
 * Scanning manually after restarting
 * LHA self-extracting archives

INTERNATIONAL ISSUES
 * Appearance on Warp 4 Japanese workstations
 * Viewing the Virus List in OS/2 2.11

FOR MORE INFORMATION
 * Getting more information
 * Installing Adobe Acrobat Reader

**************************************************************
SYSTEM REQUIREMENTS
**************************************************************

Norton AntiVirus for OS/2 has the following system requirements:

386 IBM PC or compatible
OS/2 2.11, Warp Versions 3 and 4 (Workstation and Server)
16 MB of RAM (32 MB or higher recommended)
10 MB of free hard disk space
CD-ROM drive

**************************************************************
INSTALLATION ISSUES
**************************************************************

Installing directly from the CD-ROM
-----------------------------------
You must first copy the install directory to a hard disk or 
network drive and run Install.exe from that location. Do not run 
Install.exe directly from the CD-ROM.


IBM AntiVirus for OS/2
----------------------
Upgrading from IBM AntiVirus for OS/2 to Norton AntiVirus for OS/2
the Norton AntiVirus for OS/2 installer automatically uninstalls IBM
AntiVirus for OS/2. It does not detect or uninstall other installed
virus protection products.


CID Enablement
--------------
Norton AntiVirus for OS/2 is now CID enabled.
* The installer will deliver the appropriate return codes
    upon completion.
* The ability to do silent un-installs with the appropriate 
    response file via the installer has been added. For the 
    silent un-installs, via the installer, add the /X and 
    /A:D parameters to the installer's command line arguments.
    Add these parameters only when doing an UN-INSTALL.

Network Administration: Preparing for Silent Installations
----------------------------------------------------------
Norton AntiVirus for OS/2 utility, NAVOS2UP.EXE is designed to
install Norton AntiVirus and run LiveUpdate silently.
Administrators can configure the source path, target path, and
command-line switches in a variety of ways.

For more information, please see the Norton AntiVirus for OS/2
Distribution Guide PDF file in the \DOCS\OS2 folder of the
Solutions CD. Another reference file, called SILENTOS2.PDF,
contains more information about configuring the Response file in
the IBM Installer to work with NAVOS2UP.EXE. This file is located
in the \NAVOS2\ADMIN folder on the Solutions CD.


Post-Installation
-----------------
After installing Norton AntiVirus and restarting OS/2, it is
recommended that you run Norton AntiVirus and scan your system for
viruses, allowing the Norton AntiVirus to complete the file scan
before shutting down OS/2. If you think your system is infected with
a virus, you can scan before installing.

It is also recommended that you run LiveUpdate as soon as Norton
AntiVirus for OS/2 is installed, to ensure that you have the most
up-to-date virus protection.


Reinstalling over the installed Norton AntiVirus for OS/2
---------------------------------------------------------
If you install Norton AntiVirus over an already installed version, a
warning message may appear until the computer is restarted. If it is
necessary to install Norton AntiVirus for OS/2 over an existing
installation, be sure to restart the computer. It's a good idea to
use the Uninstall utility to remove Norton AntiVirus prior to
reinstallation.


Uninstalling and removing installed files
-----------------------------------------
The Uninstall utility removes the installed Norton AntiVirus for
OS/2 program files, but because the Auto-Protect Server, OS2SRV.EXE
is still open and cannot be removed, it and its associated .DLL
files may need to be removed manually after Uninstall has been run.


********************************************************************
PLATFORM ISSUES
********************************************************************

Auto-Protect Coverage
---------------------
Please note that Auto-Protect functionality is not provided in OS/2
native mode. This means that during any OS/2 Presentation Manager or
OS/2 command-line activities, such as copying, moving, or creating
files, or downloading files from the Internet, floppy disks, or
network drives, users are not automatically protected against
viruses. However, full Auto-Protect functionality is provided in DOS
and Win-OS/2 sessions.

We strongly recommend that users create a "downloads" folder to scan
any incoming files before proceeding to use them. We also strongly
recommend that a regular scheduled scan be carried out at least
weekly, and virus definitions updated at least monthly.


Auto-Protect behavior in full-screen DOS or Win-OS/2 sessions
-------------------------------------------------------------
When Auto-Protect is active in a Win-OS/2 session, your screen may
appear to freeze if a virus is found. Auto-Protect also generates a
distinctive alert sound when this happens. This is an intentional
occurrence because Norton AntiVirus for OS/2 cannot display an Auto-
Protect alert message in the Win-OS/2 session, so the alert message
is displayed in the OS/2 Presentation Manager. To view the alert,
press Alt + Esc. This returns you to the Presentation Manager
desktop where you can view and respond to the Auto-Protect alert.

Note: This behavior occurs only in full screen DOS or Win-OS/2
sessions.


Monitor Resolution for Auto-Protect alerts
------------------------------------------
Norton AntiVirus Auto-Protect alerts are designed to display best on
monitors set at 800x600 resolution. Monitors with other resolution
settings may experience display limitations.


Warp 4 Client printing
----------------------
On client machines running OS/2 Warp 4, users may experience a
printing problem with the Scan results, Virus List/Info, Problems
Found, or Activity Log report, where the printer prints blank pages
if a DBCS font is not chosen. Users should verify that the
appropriate font has been selected in the Print dialog box prior to
printing.


Inoculation component
---------------------
Please note that Norton AntiVirus for OS/2 does not include an
Inoculation component similar to the functionality provided under
Windows 95/98/NT.


LiveUpdate Activity and Desktop Response
----------------------------------------
A slight delay in desktop response (especially non-networked
systems) may be experienced when LiveUpdate is attempting to
download virus updates from the Internet. This delay does not occur
when LiveUpdate is run in Silent mode on a network. If LiveUpdate
cannot connect to the network or ftp site, the desktop may appear to
be frozen until the network or ftp site returns an error message.
This may take a minute or two.


********************************************************************
FIXES FOR THIS RELEASE
********************************************************************


Command line scanner reports garbage for virus description
----------------------------------------------------------
Occasionally, the command line scanner (NAVDXOS2.EXE) could display
garbage characters on screen for the virus description.  This has
been fixed.


Command line scanner unable to repair boot sector/MBR viruses
-------------------------------------------------------------
Occasionally, the command line scanner (NAVDXOS2.EXE) may have
reported that it was unable to repair a boot sector or master boot
record (MBR) virus.  The infection was still logged and could be
repaired using the GUI scanner (NAVOS2.EXE).  This has been fixed.


********************************************************************
ENHANCEMENTS FOR THIS RELEASE
********************************************************************

"Virus library initialization failed code 2" running under
Workspace on Demand
----------------------------------------------------------
NAV for OS/2 has not been designed to function in a Workspace on
Demand environment and is not officially supported in this configuration.
However, the code 2 failure can be resolved by adding a key named
"ConfigReadOnly" to the "SymantecNAV" key of the OS2.INI using an INI
file editor.  Set the key equal to "1" to enable this change.  You
may wish to retrieve the documents "Using NavOS2 Under Workspace on
Demand" and "Files and INI Keys" for further information on how to
make NavOS2 function in this environment.


Ability to disable <Ctrl-C> and <Ctrl-Break> during command-
line and scheduled scans
------------------------------------------------------------
Administrators can now prevent users from breaking out of a scheduled
scan via <Ctrl-C> or <Ctrl-Break> by adding the "/NoBreak" switch to
the NAVDXOS2.EXE command line parameters in the scheduler.


********************************************************************
FIXES FROM PREVIOUS RELEASES
********************************************************************

Install failed when no C drive exists
-------------------------------------
The requirement for a C drive existing on an installation
machine has been removed. 

Upgrading from versions prior to 5.02.45 to version 5.02.45 and later
  moved files off the boot drive.
---------------------------------------------------------------------
In NAV for OS/2 version 5.02.45 the feature to allow a user to
customize the location of the virus definitions during installation
was added.  There was an issue when upgrading from an
installation of NAV for OS/2 prior to version 5.02.45 (i.e. version
5.02.44 or lower) to a version of NAV for OS/2 5.02.45 or later.
Files could be moved off the root of the boot drive and placed in
a temporary directory during the upgrade and not replaced after the
upgrade is complete.  This problem has been corrected.

Virus Definitions being reinstalled to wrong locations
------------------------------------------------------
Installing NAV for OS/2 on to a system with the virus definitions
going a custom location then uninstalling, would sometimes lead
to problems when reinstalling with a new custom location for the 
virus definitions.  On the reinstall the virus definitions could
be placed back in the location of the previous install.
This has been corrected.  

Fixed Floppy Disk installation failure
---------------------------------------
The floppy disk installation script was using some incorrect data.  
The error in the script would cause an EXEC error to appear during
the installation process.  The error in the floppy disk 
installation script has been corrected.


Fixed Auto-Protect failing to launch some applications 
  from the dos command line
-------------------------------------------------------
When launching an application from a dos window the 
command line arguments would sometimes be truncated.
This usually happened when the arguments would become
be exceedingly large.  This problem has been corrected.


Norton AntiVirus crashing when scanning some corrupt 
  self-extracting zip files
----------------------------------------------------------------
The NAV scanners were crashing on corrupt self-extracting zip
file submitted by a user.  The scanners should no longer crash
when corrupt self-extracting zip file is encountered.

Bogus Virus Library error in Command Line scanner during boot sector scanning
-----------------------------------------------------------------------------
The Command Line scanner would give a Virus Library error 100 when
scanning network drives.  This has been fixed.  The problem was
the command line scanner would attempt to scan a non-existent 
boot sector of a mapped network drive.


Command-line scanner - File names with % characters 
---------------------------------------------------
The command line scanner will properly print out file names with 
the % character embedded in the name.


Command-line scanner - Bogus virus names
----------------------------------------
The problem of bogus virus names appearing in the text log file 
when there is a scanning error has been corrected.


Increased the # of user configurable file extensions allowed
-------------------------------------------------------------
We have increased the number of file extensions the user can 
specify for scanning from 25 to 256.  

This fix required adding a new file called NAVOPTSX.DAT to the 
NAVOS2 directory.  This is a text readable file but SHOULD be 
edited using NAVOS2.EXE.  To modify the extension list start
NAV and then click on the Options button.  The Options dialog 
will appear and from the Scanner or Auto-Protect tabs click 
on the Select button to modify the file extension list.  

Modify this file by hand at YOUR OWN RISK. We do not guarantee 
proper functionality this file is modified by hand.


NAVDXOS2 and ARJ files contained in ARJ files
---------------------------------------------
Scanning ARJ files within an ARJ has been fixed. NAVDXOS2 would 
halt with the error SYS1808.


Crashing on corrupted zip files
---------------------------------
During scans some types of corrupted zip files would crash
Norton AntiVirus for OS/2.  This has been fixed.


Silent installs fail to do upgrades
-----------------------------------
Silent installs would fail to complete properly on systems where
Norton AntiVirus for OS/2 had been previously installed.
This has been fixed.  


Master Boot Record Scan Reporting
---------------------------------
The number of master boot records should now be properly reported
in the scan results dialog.  Previously it reported one boot record
for each boot sector that was scanned.


"Assertion Failed" Message
--------------------------
During scans of some MS WORD document templates an "Assertion Failed"
error message box would appear.  This has been fixed.


"Drive not ready" Messages
--------------------------
During scans when boot record scanning was enabled, users with empty
floppy and/or CD ROM drives reported a number of "Drive not ready"
message boxes that appeared at the beginning of a scan.  These have
been fixed for most scanning operations.


Japanese Desktop file scanning
------------------------------
On some Japanese configurations, files in the desktop folder were not
being scanned.  This has been fixed.


Chinese long file and directory names
-----------------------
On Traditional Chinese systems, very long path names and file names
were causing the application to terminate abnormally.  This has been
fixed.


Uninstalling IBM AV 2.4
-----------------------
The NAVOS2 product correctly uninstalled version 3 and later of IBM AV,
but did not properly uninstall IBM AV 2.4.  This has been fixed.


24-hour Clock Scheduling
------------------------
In early releases of the product, the Hour field in the scheduler would
reset to 0 if the system used a 24 hour clock.  This has been fixed.


Scheduler in the Startup Folder
-------------------------------
In previous releases, if the scheduler was placed by the user into the
startup folder it would not run minimized.  Once minimized by the user,
it could not be re-activated.

The scheduler is now placed in the startup folder by the installer, and
runs minimized.  The user can now also activate it.

AutoProtect speed improvement
-----------------------------
The speed of AutoProtect for DOS and WinOS2 boxes has been
improved by approximately 300%.

********************************************************************
ENHANCEMENTS FROM PREVIOUS RELEASES
********************************************************************

Updated Virus Definitions to 9/22/99
------------------------------------
Norton AntiVirus for OS/2 now installs with the Virus Definitions
dated September 22, 1999.

Virus definition installation location can be configured
--------------------------------------------------------
The user has the option of switching the default location where
the Virus Definition will reside after when an install is completed
successfully.  The current default location is 
C:\Symantec\Common\Shared\VirusDefs.  

* For Silent installs
  - In the Response file AUX1 is the location where the Virus Defs
      will reside when an install completes successfully.
  - In the Response file AUX2 is a temporary location where the
      the install can put the Virus Definition before they are
      moved to their final location.  [This is a new entry to 
      the response file]

      NOTE: AUX2 a MUST be a directory that will only contain
        the virus definitions.  In a successful install the 
        AUX2 directory is REMOVED from the user's system.

Launching a user defined application when a virus is found
-----------------------------------------------------------
Norton AntiVirus for OS/2, Auto-protect, and the command line 
scanner have now can launch a user defined application when a virus 
is found.  

The user must provide a tool to edit the OS2.ini
file.  Symantec currently DOES NOT have any tools available to edit
the OS2.INI file.   We have included a sample REXX script that can
edit the OS2.INI file.  This sample however is for demonstration 
purposes only.  The user can feel free to use the sample REXX script
as a starting point.   Symantec in no way, shape, or form gives
any warranty, implied or otherwise, for the sample code included
in this section.


The user must add the following entry into the OS2.ini
Application name: SymantecNAV 
Key name: LaunchApp
Value: <Fully qualified path to application followed by any 
        command line arguments>

An example: SymantecNAV -> LaunchApp -> c:\e.exe filename.txt

A fully qualified is preferable and NO spaces before the app.
If the OS2.INI entry is not set then no application will 
be launched

NOTES:
* User defined application launch behavior
  When scanning, if multiple viruses are found then for each virus
    found the user-defined application will be launch.  This applies
    to Auto-protect and the Norton AntiVirus for OS/2 GUI.  
    The exception is command line scanner which will launch the 
    the user defined application once at the end of the scan.

* Command line Scanner
    The app will launch only once all viruses found.

* Auto-protect
  - If Auto-Protect was NOT launched at startup or from
      a OS/2 Command prompt then the user-defined application 
      may not always launch.
  - When a virus is found, if the user is prompted and selects the
      continue option then Auto-protect will detect the same virus 
      again and launch the user define application again.

* Running an OS/2 Command File
  - When launching an OS/2 command file, a CMD file, the following MUST 
      precede the command file "cmd.exe /c".  This text starts a command
      prompt that the command file will run in.  An example of what
      the entry in the OS/2 ini file would look like is as follows

      cmd.exe /c c:\apps\alert.cmd argument1 argument2 ... argumentx

      The arguments are optional. The applications that are called by 
      the command file must either be in the path or the command file 
      should switch to the directory where the application resides.      

* Sample Rexx script to update the OS2.INI
  /*
   This is a sample file that modifies the OS2.INI.
   This will add one string to the OS2.ini to identify an 
   executable that can be run when a virus is detected.

   This is a REXX script that can be run in an os2
   command prompt.  

   This code is AS-IS.  Symantec does not guarantee that
   this code will work on all OS/2 platforms, the robustness 
   of this sample Rexx script, or any side effects
   it may have on a user's system.  This is for demonstration
   purposes only.  Symantec in no way, shape, or form gives
   any warranty, implied or otherwise, for  this sample code.
  */

  call RxFuncAdd "SysLoadFuncs", "RexxUtil", "SysLoadFuncs"
  call SysLoadFuncs


  /* 
    This will only take the first text argument, delineated by a space.
    Any text after the first space is ignored in this sample file
  */
  Say 'Enter in the command line to be run when a virus is detected'
  pull name .

  /* If there was no text given than display the error message. */
  /* Otherwise, update the os2.ini file and display the actual */
  /* value stored in the os2.ini file. */
  if name = ""
   then
      Say 'You must enter a command line'
   else
    do
      call SysIni, 'SymantecNAV', 'LaunchApp', name
      say ''
      say 'The value enter to the os2.ini file was:'
      say SysIni(,'SymantecNAV', 'LaunchApp')
    end /* do */

  exit


Floppy Disk Install
-------------------
Diskette installation requires four (4) formatted blank diskettes.
The volume labels on these diskettes must be NAVINST1, NAVINST2,
NAVINST3, and NAVINST4 in order for the installer to prompt for
the correct diskettes during installation.

Creating the floppy disks:
In the Norton AntiVirus installation directory there are four 
sub-directories name Disk1 though Disk4.  
1) Copy the contents of the Disk1 directory to floppy disk labeled Disk 1. 
2) Change the volume label of Disk 1 to NAVINST1
   * Open an OS/2 Command prompt
   * Switch the command prompt to the floppy drive (i.e. at the 
     command prompt type A:)
   * At the command prompt type: label 
   * You will be prompted for a label name type NAVINST1
3) Copy the contents of the Disk2 directory to floppy disk labeled Disk 2. 
4) Change the volume label of Disk 2 to NAVINST2
5) Copy the contents of the Disk3 directory to floppy disk labeled Disk 3. 
6) Change the volume label of Disk 3 to NAVINST3
7) Copy the contents of the Disk4 directory to floppy disk labeled Disk 4. 
8) Change the volume label of Disk 4 to NAVINST4
9) You now have a set Norton AntiVirus for OS/2 installation floppies

Notes:
* Out of Disk Space Error when using UI to copy files to floppy.
  - If you get an out of disk space error when copying the files to a floppy
     using the UI then they the following:
     1) Open an OS/2 command prompt  
     2) Goto the disk directory 
     3) Use xcopy or copy to transfer the files to the floppy drive.

Scheduler has a delay at startup
--------------------------------
There is a 5 minute delay when the scheduler is launched.
The scheduler does not run any scheduled or missed events until
the delay period has completed.  This is to allow Auto-protect to
startup properly when a system boots up.

The delay can be extend by adding the following entry into the 
initsked.ini file.  

[STARTUP_DELAY]
Minutes=X

This file is located in directory where Norton AntiVirus for OS/2 was 
installed.  If the file does not exist just create the file using a 
text editor and add the setting.  X is the number of minutes the 
Program Scheduler delays.  The minimum delay is 5 minutes, even if
you specify a time less than 5 minutes.


Scheduled Scans Now Use the Command Line Scanner
------------------------------------------------
Originally, scheduled scans took place through the user interface. Users
reported that this interfered with their other scanning activity, and that
scans could not take place when the screen was locked.  There were also
reports of the results screen not shutting down when the scan completed.

Scheduled scans now use the command line scanner, and are run independent
of other user activities.  Scans will now run within a locked screen, and
do not require any user intervention.

Note that the command line scanner scans compressed files (zips, arc,
arj, lzh, lha) by default using the /ZIPS option.  To change this or other
scheduled scanning options, open up the scheduler, double click your scan
entry, and add any command line options to the "What to Scan" field.  See
the section "New Command Line Scanner Options" for a full list of arguments
you may can add to this field.  System Administrators will likely want to
add /NOPROMPT to the list of command line arguments as well as
/TEXTLOG:<filename>.  These will allow repeated scans without intervention
and output any viruses detected to a text based log file.


Autorun of Missed Scheduled Events
----------------------------------
There is a new Autorun checkbox in the Scheduler Options dialog.  To
have the scheduler automatically run all missed events (e.g. the
machine was powered down during a scheduled event) check this box.


Melissa Virus Coverage
----------------------
This release includes updated virus definitions that will detect and
repair files infected with the Melissa virus.


longmsg.txt File
--------------------
You can customize the message that is displayed when a virus is found
by creating a plain text file named longmsg.txt, and placing it in
the same directory as the NAVOS2 application.  A typical use of this
file is to include text such as,

   "A virus was found on your system. Contact SysAdmin at ext. 2222"


New Command Line Scanner Options
--------------------------------
There are two new command line switches:

 /ZIPS             Enable scanning of all compressed files
 /TEXTLOG:file     Create or append to a plain text log file.

Here is the format and full list of command line scanning options:

NAVDXOS2 pathname [options]
   pathname          Drive, directory, file, or combination of
                     items separated by spaces.
   /?                Display the help screen.
   /A                Scan all drives (A: and B: are skipped.)
   /L                Scan local drives (A: and B: are skipped.)
   /B[+|-]           Enable or disable scanning of boot records.
                     If /B[+|-] is omitted, /B+ is the default.
   /BOOT             Scan only the boot sectors of specified drives.
   /S[+|-]           Enable or disable scanning subdirectories.
                     If /S[+|-] is omitted, /S- is the default.
   /REPAIR           Repair infected files automatically.
   /DELETE           Delete infected files automatically.
   /HALT             Halt the scanning operation if a virus is found.
   /CFG:[directory]  Specify the directory containing NAVDXOS2
                     configuration files.
   /LOG:file         Create and log to the specified file.
   /APPENDLOG:file   Append to an existing log file.
   /TEXTLOG:file     Create or append to readable text log file specified.
   /DOALLFILES       Scan all files, not just executables.
   /NOBEEP           Run silently (no beeps).
   /PROMPT           Prompt for an action when an infection is found.
   /NOPROMPT         Do not prompt for an action when an infection is found.
   /NOBREAK          Disables <Ctrl-C> and <Ctrl-Break> break signals.
   /HELPERROR        List possible errorlevels returned by NAVDXOS2.
                     If desired, you can run NAVDXOS2 from a batch file
                     and process the errorlevel with IF ERRORLEVEL
                     constructions.
   /ZIPS             Enable scanning of container files.


New Command Line Scanner Return Codes
-------------------------------------
The command line scanner, NAVDXOS2, will return one of the following
error codes after a scan:

 0    No errors occurred and no virus were found.
 5    Not enough memory to complete the scan.
 6    One or more command line parameters were invalid.
 11   An internal program error occurred.
 13   Multiple virus were found in the master boot record, boot sector,
      or files.
 20   A file virus was found.
 21   A boot virus was found (master boot record or boot sector).
 22   A macro virus was found.
 102  CTRL-C or CTRL-BREAK was used to interrupt the scan.

********************************************************************
MISCELLANEOUS ISSUES
********************************************************************

Auto-Protect Performance
------------------------
On older machines with less memory running OS/2 v 2.11 there may be
some lag in Auto-Protect performance when it is monitoring DOS and
Win-OS/2 sessions and applications.

Here are suggestions to improve performance in these situations:

* Exclude frequently used executables/DLLs that you know are virus
free by adding them to the Exclusions list.  For example, if you are
using MS Word add winword.exe to the exclusions list, for MS Excel
add excel.exe, etc.  Consult your MS documentation for a list of DLL's
to add to the list.

* Deselect some of the selected options in the Options Auto-Protect
tab. This may reduce protection slightly, but will improve
performance.


Scanning with Disabled Removable Drives
---------------------------------------
If you run a scan (UI or command line) with either of the boot scanning
options enabled and there is a disabled or non-functional drive present,
OS/2 will display a "Drive not ready" error message. If this error message
appears, selecting "Return error code to program" will cause Norton AntiVirus
to skip boot scanning of that drive and proceed normally.


Command-Line Scanner and Open Files
-----------------------------------
When the Norton AntiVirus for OS/2 command-line scanner, NAVOS2DX is
run with the /L or /A switch and there are open files, a message
temporarily appears on screen, "Unable to Scan <pathname/filename>"
to notify users that the named files could not be scanned because
they were open.


Changing the setting of Heuristics (Bloodhound technology)
----------------------------------------------------------
If you change the setting for Heuristics (Bloodhound technology), on
the Auto-Protect Options tab, you must restart your system before
the change takes effect for Auto-Protect in Win-OS/2 and DOS
sessions.


Reading The Log Created By NAVDXOS2
-----------------------------------
The log created by the command-line scanner, NAVOS2DX, is by
default not a text based file, and is intended to be read using
the NAVOS2 product. To read it you must open the main window of Norton
AntiVirus for OS/2 and use the Activity Log to open the file and
view its contents.

To create a text based log file, add the option /TEXTLOG:filename.txt
where filename.txt is the name of your text file.


Virus List Printouts
--------------------
When you print the Virus List, the text may not be aligned if you
are using proportional fonts. This also happens when you print out
Scan results. To avoid this problem, use a fixed font such as
Courier.


Folder Visibility in the Scan Folder Window when Windows
attributes are set
--------------------------------------------------------
When Scan Folder is selected, any folder whose Windows attribute
(Read Only, Archive, Hidden, or System) has been set will not appear
in the Scan Folder window. In order for the folder to be visible in
the Scan Folder list, the Windows attribute must be removed.


Auto-Protect "Duplicate File Name" message
------------------------------------------
A message similar to "A duplicate file name exists," or "the file cannot
be found" or "the file is being used" may appear when Auto-Protect
detects a file with a virus and the Auto-Protect option has been set
to "Deny Access". This is normal behavior when the "Deny Access"
option is active for Auto-Protect in Norton AntiVirus for OS/2.


Auto-Protect disabled if Terminate Program selected
---------------------------------------------------
If Auto-Protect detects an infected file on a write-protected floppy
disk, an OS/2 system error dialog box appears, displaying the
message, "Can't write on write-protected disk." If you respond to the
error message by selecting "Terminate program/command," the Auto-Protect
program is disabled. Auto-Protect is enabled again when you restart
Norton AntiVirus for OS/2.


Scanning manually after restarting
----------------------------------
If Norton AntiVirus is open when a soft reboot (pressing Ctrl-Alt-
Del keys) occurs, manual scanning may not be available without
closing and re-opening the application. Always close Norton
AntiVirus before "soft rebooting" your computer.


LHA self-extracting archives
----------------------------
NAV for OS/2 supports scanning of LHA archives, however some files
that are LHA self-extracting executables may cause the NAV application
to terminate.  Please report any files that exhibit this behavior to
Symantec.


********************************************************************
INTERNATIONAL ISSUES
********************************************************************

Appearance Of Norton AntiVirus For OS/2 On Warp 4 Japanese
Workstations
----------------------------------------------------------
Due to limitations of supporting OS/2 2.11, Warp 3, and Warp 4 on
600 x 400 monitor sizes and larger, some text boxes will appear
clipped. This is because of sizing limitations of text boxes in
OS/2, but will not affect performance.


Viewing the Virus List in OS/2 2.11
-----------------------------------
Because of some limitations in OS/2 2.11, the virus list will not be
fully displayed, although you can print the full virus list. The
list displays viruses up to the letter F only on English language
systems, and up to the letter B on Japanese language systems. On
Warp 3 and Warp 4, the entire list displays.

********************************************************************
FOR MORE INFORMATION
********************************************************************

Getting more information
------------------------
There is extensive information available in the Norton AntiVirus for
OS/2 User's Guide PDF file on this CD. For network administrators,
the Norton AntiVirus for OS/2 Distribution Guide PDF file and the
separate document on configuring silent installations, SILENTOS2.PDF,
provide information on using the administration utility, NAVOS2UP.EXE,
and its associated files. The User's Guide and Distribution Guide PDF
files are located in the \DOCS\OS2 folder on the Solutions CD.
The SILENTOS2.PDF file is located in the \NAVOS2\ADMIN folder
on the Solutions CD.

Adobe Acrobat Reader is provided to view the PDFs.


Installing Adobe Acrobat
------------------------
1. Locate the Acrobat folder on the Norton AntiVirus Solution CD.
2. Open the OS2 folder.
3. For Warp 3 and above, double-click the aro2e30.exe file and
follow the on-screen message to complete installation. This is the
OS/2 version of Acrobat Reader.

For OS/2 2.11, you must use the Windows 3.1 version of Acrobat
Reader in a Win-OS/2 session. Double-click the ar16e301.exe file and
follow the on-screen messages to complete installation.


Error Message for Online manuals: "Cannot find or run file"
-------------------------------------------------------
You must install Adobe Acrobat Reader to view the Norton AntiVirus
online manuals.

********************************************************************
                                END OF FILE
********************************************************************


