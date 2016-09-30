W32.Mydoom@mm Removal Tool

Discovered on: January 26, 2004 
Last Updated on: June 1, 2004 09:34:26 AM GDT 

Symantec Security Response has developed a removal tool to clean the following
infections:

=> W32.Mydoom.A@mm 
=> W32.Mydoom.B@mm 
=> W32.Mydoom.F@mm 
=> W32.Mydoom.G@mm 
=> W32.Mydoom.H@mm

--------------------------------------------------------------------------------
WHAT THE TOOL DOES

The W32.Mydoom@mm Removal Tool does the following: 

o  Terminates W32.Mydoom@mm viral processes. 
o  Terminates the viral thread running under Explorer.exe. 
o  Deletes W32.Mydoom@mm files. 
o  Reverses the changes made to the \Run and InProcServer32 registry keys.

   Note: Because these keys may be added by legitimate programs, the tool does 
   not delete the following keys:

   HKEY_LOCAL_MACHINE\Software\Microsoft\Windows\CurrentVersion\
   Explorer\ComDlg32\Version

   HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\
   Explorer\ComDlg32\Version


   If you are sure that a legitimate program did not add them, you can manually
   delete them. However, leaving them in place will not do any harm.

--------------------------------------------------------------------------------
COMMAND-LINE SWITCHES AVAILABLE WITH THIS TOOL

Switch             Description 
/HELP, /H, /?      Displays the help message. 

/NOFIXREG          Disables the registry repair. (We do not recommend using this
                   switch.) 

/SILENT, /S        Enables the silent mode. 

/LOG=<path name>   Creates a log file where <path name> is the location in which
                   to store the tool's output. By default, this switch creates 
                   the log file, FxMydoom.log, within the same folder from which
                   the removal tool was executed. 

/MAPPED            Scans the mapped network drives. (We do not recommend using 
                   this switch. Refer to the Notes below.) 

/START             Forces the tool to immediately start scanning. 

/EXCLUDE=<path>    Excludes the specified <path> from scanning. (We do not 
                   recommend using this switch.) 
 
/NOFILESCAN        Prevents the scanning of the file system. 

--------------------------------------------------------------------------------
Notes: 

o  Symantec Security Response strongly recommends that you not use the
   /NOFIXREG switch when running this removal tool for the first time. If the 
   removal tool is run using this switch, you will not be able to remove the 
   registry keys associated with this worm by running the tool again. 


o  Using the /MAPPED switch does not ensure the complete removal of the virus on
   remote computers, because: 

   => The scanning of mapped drives scans only the mapped folders. This may not
      include all the folders on the remote computer, which can lead to missed
      detections. 
   
   => If a viral file is detected on the mapped drive, the removal will fail if 
      a program on the remote computer uses this file. 

      Therefore, you should run the tool on every computer.


o  The /EXCLUDE switch will only work with one path, not multiple paths. An 
   alternative is the /NOFILESCAN switch, which will allow the tool to alter the
   registry. Then, scan the computer with your antivirus product, using the 
   current virus definitions. These steps should clean the file system.

   The following is an example command line that can be used to exclude a single
   drive:

   >"C:\Documents and Settings\user1\Desktop\FxMydoom.exe" 
   /EXCLUDE=M:\ /LOG=c:\FxMydoom.txt

   where the greater than symbol (>) is not part of the path. 


   Alternatively, the command line below will skip scanning the file system, 
   but will repair the modifications to the registry. You should then run a 
   regular scan of the system with proper exclusions:

   >"C:\Documents and Settings\user1\Desktop\FxMydoom.exe" 
   /NOFILESCAN /LOG=c:\FxMydoom.txt


   The name of the log file can be whatever you choose. The name listed is for
   the sole purpose of this example.

--------------------------------------------------------------------------------
OBTAINING AND RUNNING THE TOOL

--------------------------------------------------------------------------------
Note: You must have administrative rights to run this tool on Windows NT/2000/XP.
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
WARNING: For network administrators. If you are running MS Exchange 2000 Server,
we recommend that you exclude the M drive from the scan by running the tool from
a command line with the Exclude switch. For more information, read the 
Microsoft knowledge base article, "XADM: Do Not Back Up or Scan Exchange 2000 
Drive M" (Article 298924).
--------------------------------------------------------------------------------
 
1.  Download the FxMydoom.exe file from: 
    http://securityresponse.symantec.com/avcenter/FxMydoom.exe. 

2.  Save the file to a convenient location, such as your downloads folder or the
    Windows desktop, or removable media known to be uninfected. 

3.  To check the authenticity of the digital signature, refer to the 
    "Digital signature" section later in this writeup. 

4.  Close all the running programs before running the tool. 

5.  If you are on a network, or if you have a full-time connection to the 
    Internet, disconnect the computer from the network and the Internet. 

6.  If you are running Windows Me or XP, then disable System Restore. Refer to 
    the "System Restore option in Windows Me/XP" section later in this writeup 
    for further details.

--------------------------------------------------------------------------------
Caution: If you are running Windows Me/XP, we strongly recommend that you do not
skip this step.
--------------------------------------------------------------------------------

7.  Double-click the FxMydoom.exe file to start the removal tool. 

8.  Click Start to begin the process, and then allow the tool to run. 

9.  Restart the computer. 

10. Run the removal tool again to ensure that the system is clean. 

11. If you are running Windows Me/XP, then re-enable System Restore. 

12. If you are using Active Desktop, you may need to restore it. 

13. Run LiveUpdate to make sure that you are using the most current virus 
    definitions.

--------------------------------------------------------------------------------
Note: The removal procedure may not be successful if Windows Me/XP System 
Restore is not disabled, as previously directed, because Windows prevents 
outside programs from modifying System Restore. 
--------------------------------------------------------------------------------

When the tool has finished running, you will see a message indicating whether 
W32.Mydoom@mm infected the computer. In the case of a removal of the worm, the 
program displays the following results:

o  Total number of scanned files 
o  Number of deleted files 
o  Number of repaired files 
o  Number of terminated viral processes 
o  Number of fixed registry entries

--------------------------------------------------------------------------------
DIGITAL SIGNATURE

FxMydoom.exe is digitally signed. Symantec recommends that you use only copies 
of FxMydoom.exe, which have been directly downloaded from the Symantec Security 
Response Web site. To check the authenticity of the digital signature, follow 
these steps: 

1.  Go to http://www.wmsoftware.com/free.htm. 

2.  Download and save the chktrust.exe file to the same folder in which you 
    saved FxMydoom.exe (for example, C:\Downloads). 

3.  Depending on your operating system, do one of the following:

    o  Click Start, point to Programs, and then click MS-DOS Prompt. 
    o  Click Start, point to Programs, click Accessories, and then click 
       Command Prompt.


4.  Change to the folder in which FxMydoom.exe and Chktrust.exe are stored, and
    then type: 

    chktrust -i FxMydoom.exe.

    For example, if you saved the file to the C:\Downloads folder, you would 
    enter the following commands:

    cd\
    cd downloads
    chktrust -i FxMydoom.exe

    Press Enter after typing each command. If the digital signature is valid, 
    you will see the following: "Do you want to install and run "W32.Mydoom@mm 
    Removal Tool" signed on 3/5/2004 09:13 AM and distributed by Symantec
    Corporation?"


--------------------------------------------------------------------------------
Notes: 
o  The date and time displayed in this dialog box will be adjusted to your time 
   zone, if your computer is not set to the Pacific time zone. 
o  If you are using Daylight Saving time, the displayed time will be exactly one
   hour earlier. 
o  If this dialog box does not appear, there are two possible reasons:
   => The tool is not from Symantec: Unless you are sure that the tool is 
      legitimate and that you downloaded it from the legitimate Symantec Web 
      site, you should not run it. 
   => The tool is from Symantec and is legitimate: However, your operating 
      system was previously instructed to always trust content from Symantec. 
      For information on this and on how to view the confirmation dialog again, 
      read the document, "How to restore the Publisher Authenticity confirmation
      dialog box."
--------------------------------------------------------------------------------

5.  Click Yes to close the dialog box. 

6.  Type exit, and then press Enter. (This will close the MS-DOS session.)


--------------------------------------------------------------------------------
SYSTEM RESTORE OPTION IN WINDOWS ME/XP

Users of Windows Me and Windows XP should temporarily turn off System Restore. 
Windows Me/XP uses this feature, which is enabled by default, to restore the 
files on your computer in case they become damaged. If a virus, worm, or Trojan 
infects a computer, System Restore may back up the virus, worm, or Trojan on the
computer.

Windows prevents outside programs, including antivirus programs, from modifying 
System Restore. Therefore, antivirus programs or tools cannot remove threats in 
the System Restore folder. As a result, System Restore has the potential of 
restoring an infected file onto your computer, even after you have cleaned the 
infected files from all the other locations.

Also, a virus scan may detect a threat in the System Restore folder even though
you have removed the threat.

For instructions on how to turn off System Restore, read your Windows 
documentation, or one of the following articles:
o  "How to disable or enable Windows Me System Restore" 
o  "How to turn off or turn on Windows XP System Restore"

For additional information, and an alternative to disabling Windows Me System 
Restore, see the Microsoft Knowledge Base article, "Antivirus Tools Cannot
Clean Infected Files in the _Restore Folder," Article ID: Q263455. 


--------------------------------------------------------------------------------
Revision History: 

March 5, 2004: Version 1.0.7.1 release to support W32.Mydoom.G@mm and W32.Mydoom.H@mm. 
February 25, 2004: Updated the fixtool GUI to enable scanning of mapped drives without invoking the tool from the command prompt. 
February 24, 2004: Version 1.0.6 release to support W32.Mydoom.F@mm. 
February 4, 2004: Renamed to W32.Mydoom@mm Removal Tool from W32.Novarg.A@mm/W32.Mydoom.B@mm Removal Tool. 
February 2, 2004: Updated information about registry key removal. 
January 30, 2004: Updated fix tool to incorporate W32.Mydoom.B@mm. 
January 27, 2003: Version 1.0.3 released to correct fix minor bugs

