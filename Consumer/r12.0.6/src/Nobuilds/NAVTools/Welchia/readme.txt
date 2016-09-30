W32.Welchia.Worm Removal Tool

Discovered on: August 18, 2003 
Last Updated on: April 1, 2004 04:44:38 PM GDT 

Symantec Security Response has developed a removal tool to clean infections of 
W32.Welchia.Worm, W32.Welchia.B.Worm, W32.Welchia.C.Worm, and 
W32.Welchia.D.Worm.

--------------------------------------------------------------------------------
WHAT THE TOOL DOES

The W32.Welchia.Worm Removal Tool does the following: 

1.  Terminates the W32.Welchia.Worm (or W32.Welchia.B / W32.Welchia.C / 
    W32.Welchia.D) Worm viral processes. 

2.  Deletes the files that the worm added. 

3.  Deletes the registry values that the worm added.

--------------------------------------------------------------------------------
Note: Deletion will be performed only if the operating system has not already 
removed these values upon terminating the viral processes, as mentioned in 
step 1.
--------------------------------------------------------------------------------

4.  Deletes the services that W32.Welchia.Worm created.


--------------------------------------------------------------------------------
COMMAND-LINE SWITCHES AVAILABLE WITH THIS TOOL

Switch             Description 
/HELP, /H, /?      Displays the help message. 

/NOFIXREG          Disables the registry repair. (We do not recommend using this
                   switch.) 

/SILENT, /S        Enables the silent mode. 

/LOG=<path name>   Creates a log file where <path name> is the location in which
                   to store the tool's output. By default, this switch creates 
                   the log file, FixWelch.log, within the same folder from which
                   the removal tool was executed. 

/MAPPED            Scans the mapped network drives. (We do not recommend using 
                   this switch. Refer to the Notes below.) 

/START             Forces the tool to immediately start scanning. 

/EXCLUDE=<path>    Excludes the specified <path> from scanning. (We do not 
                   recommend using this switch.) 
 
/NOFILESCAN        Scans the %System%\Wins folder for Dllhost.exe and 
                   Svchost.exe only.

--------------------------------------------------------------------------------
Notes: 

o  Using the /MAPPED switch does not ensure the complete removal of the virus 
   on remote computers, because: 

   => The scanning of mapped drives scans only the mapped folders. This may not
      include all the folders on the remote computer, which can lead to missed
      detections. 
   
   => If a viral file is detected on the mapped drive, the removal will fail if 
      a program on the remote computer uses this file. 

      Therefore, you should run the tool on every computer.

--------------------------------------------------------------------------------
OBTAINING AND RUNNING THE TOOL

Note: You must have administrative rights to run this tool on Windows NT 4.0, 
Windows 2000, or Windows XP.
--------------------------------------------------------------------------------
WARNING: For network administrators. If you are running MS Exchange 2000 Server,
we recommend that you exclude the M drive from the scan by running the tool from
a command line with the Exclude switch. For more information, read the Microsoft
knowledge base article, "XADM: Do Not Back Up or Scan Exchange 2000 Drive M" 
(Article 298924).
--------------------------------------------------------------------------------

1.  Download the FixWelch.exe file from: 
    http://www.symantec.com/avcenter/FixWelch.exe. 


2.  Save the file to a convenient location, such as your downloads folder or the
    Windows desktop, or removable media known to be uninfected. 

3.  To check the authenticity of the digital signature, refer to the "Digital 
    signature" section later in this writeup. 

4.  Close all the running programs before running the tool. 

5.  If you are on a network or if you have a full-time connection to the 
    Internet, disconnect the computer from the network and the Internet. 

6.  If you are running Windows Me or XP, then disable System Restore. Refer to 
    the "System Restore option in Windows Me/XP" section later in this writeup 
    for further details.

    WARNING: If you are running Windows Me/XP, we strongly recommend that you do
    not skip this step.

7.  Double-click the FixWelch.exe  file to start the removal tool. 

8.  Click Start to begin the process, and then allow the tool to run. 


--------------------------------------------------------------------------------
    If problems occur when running the tool:
   
    If the tool fails to run as expected or it reports that it was not able to
    remove all the files or end all the processes, run the tool again in Safe 
    mode. In this situation, turn off the power, wait 30 seconds, and then 
    restart the computer in Safe mode (Windows 95/98/Me/2000/XP), and then run 
    the tool again. For instructions on restarting the computer in Safe mode, 
    read the document, "How to start the computer in Safe Mode."

    Once the tool has run in Safe mode, proceed to step 9.

--------------------------------------------------------------------------------

9.  Restart the computer. 

10. Run the removal tool again to ensure that the system is clean. 

11. If you are running Windows Me/XP, then re-enable System Restore. 

12. Run LiveUpdate to make sure that you are using the most current virus 
    definitions.

Note: The removal procedure may not be successful if Windows Me/XP System 
Restore is not disabled as previously directed, because Windows prevents outside
programs from modifying System Restore. 

When the tool has finished running, you will see a message indicating whether 
W32.Welchia.Worm infected the computer. In the case of a removal of the worm, 
the program displays the following results: 

o  Total number of scanned files 
o  Number of deleted files 
o  Number of terminated viral processes 
o  Number of fixed registry entries

--------------------------------------------------------------------------------
DIGITAL SIGNATURE

FixWelch.exe is digitally signed. Symantec recommends that you only use copies 
of FixWelch.exe, which have been directly downloaded from the Symantec Security 
Response Web site. To check the authenticity of the digital signature, follow 
these steps: 

1.  Go to http://www.wmsoftware.com/free.htm. 

2.  Download and save the chktrust.exe file to the same folder in which you 
    saved FixWelch.exe  (for example, C:\Downloads). 

3.  Depending on your operating system, do one of the following: 
  
    o  Click Start, point to Programs, and then click MS-DOS Prompt. 
    o  Click Start, point to Programs, click Accessories, and then click Command
       Prompt.

4.  Change to the folder in which FixWelch.exe  and Chktrust.exe are stored, and 
    then type: chktrust -i FixWelch.exe .

    For example, if you saved the file to the C:\Downloads folder, you would 
    enter the following commands:

    cd\
    cd downloads
    chktrust -i FixWelch.exe

    Press Enter after typing each command. If the digital signature is valid, 
    you will see the following, "Do you want to install and run 
    "W32.Welchia Removal Tool" signed on 04/28/2004 3:53 PM and distributed by: 
    Symantec Corporation"

    Note: 
    o  The date and time displayed in this dialog box will be adjusted to your 
       time zone, if your computer is not set to the Pacific time zone. 
    o  If you are using Daylight Saving time, the displayed time will be exactly
       one hour earlier. 
    o  If this dialog box does not appear, there are two possible reasons: 
       => The tool is not from Symantec: Unless you are sure that the tool is 
          legitimate and that you downloaded it from the legitimate Symantec Web
          site, you should not run it. 
       => The tool is from Symantec and is legitimate: However, your operating 
          system was previously instructed to always trust content from Symantec.
          For information on this and on how to view the confirmation dialog 
          again, read the document, "How to restore the Publisher Authenticity 
          confirmation dialog box."

5.  Click Yes to close the dialog box. 

6.  Type exit, and then press Enter. (This will close the MS-DOS session.)

--------------------------------------------------------------------------------
System Restore option in Windows Me/XP

Users of Windows Me and Windows XP should temporarily turn off System Restore. 
Windows Me/XP uses this feature, which is enabled by default, to restore the 
files on your computer in case they become damaged. If a virus, worm, or Trojan 
has infected a computer, it is possible that System Restore backed up the virus,
worm, or Trojan on the computer.

Windows prevents outside programs, including antivirus programs, from modifying 
System Restore. Therefore, antivirus programs or tools cannot remove threats in 
the System Restore folder. As a result, System Restore has the potential to 
restore an infected file onto your computer even after you have cleaned the 
infected files from all the other locations.

Also, in some cases, online scanners may detect a threat in the System Restore 
folder even though you scanned your computer with an antivirus program and did 
not find any infected files.


For instructions on how to turn off System Restore, read your Windows 
documentation, or one of the following articles: 
=>  "How to disable or enable Windows Me System Restore" 
=>  "How to turn off or turn on Windows XP System Restore"

For additional information, and an alternative to disabling Windows Me System 
Restore, see the Microsoft Knowledge Base article, "Antivirus Tools Cannot Clean
Infected Files in the _Restore Folder," Article ID: Q263455. 

--------------------------------------------------------------------------------
Revision History: 

March 25, 2004: Posted version 1.0.6 to support W32.Welchia.D.Worm. 
February 15, 2004: Posted version 1.0.5 to support W32.Welchia.C.Worm. 
February 12, 2004: Posted version 1.0.4 to support W32.Welchia.B.Worm. 
August 28, 2003: Added a Warning to exclude the M drive on MS Exchange 2000 Servers. 
August 20, 2003: Posted version 1.0.3 to update the support for the no-scanning-of-files option. 
August 20, 2003: Posted version 1.0.2 to support the no-scanning-of-files option. 
August 19, 2003: Posted version 1.0.1 to fix minor bug.

