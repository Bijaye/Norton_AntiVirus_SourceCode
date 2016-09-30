W32.Netsky@mm Removal Tool

Last Updated on: April 28, 2004 04:31:03 PM PDT 

Symantec Security Response has developed a removal tool to clean the infections 
of the following variants of the Netsky worm: 

W32.Netsky.B@mm 
W32.Netsky.C@mm 
W32.Netsky.D@mm 
W32.Netsky.E@mm 
W32.Netsky.K@mm 
W32.Netsky.P@mm 
W32.Netsky.Q@mm 
W32.Netsky.S@mm 
W32.Netsky.T@mm 
W32.Netsky.X@mm 
W32.Netsky.Y@mm 
W32.Netsky.Z@mm 
W32.Netsky.AB@mm

--------------------------------------------------------------------------------
WHAT THE TOOL DOES

The W32.Netsky@mm Removal Tools do the following: 

1.  Terminate the W32.Netsky@mm viral processes 
2.  Delete the W32.Netsky@mm files 
3.  Delete the registry values that the worms add

--------------------------------------------------------------------------------
COMMAND-LINE SWITCHES AVAILABLE WITH THIS TOOL

Switch             Description 
/HELP, /H, /?      Displays the help message. 

/NOFIXREG          Disables the registry repair. (We do not recommend using this
                   switch.) 

/SILENT, /S        Enables the silent mode. 

/LOG=<path name>   Creates a log file where <path name> is the location in which
                   to store the tool's output. By default, this switch creates 
                   the log file, FxNetsky.log, within the same folder from which
                   the removal tool was executed. 

/MAPPED            Scans the mapped network drives. (We do not recommend using 
                   this switch. Refer to the Notes below.) 

/START             Forces the tool to immediately start scanning. 

/EXCLUDE=<path>    Excludes the specified <path> from scanning. (We do not 
                   recommend using this switch.) 
 
/NOFILESCAN        Prevents the scanning of the file system. 

--------------------------------------------------------------------------------
Notes: 

o  Using the /MAPPED switch does not ensure the complete removal of the virus on
   remote computers, because: 

   => The scanning of mapped drives scans only the mapped folders. This may not
      include all the folders on the remote computer, which can lead to missed
      detections. 
   
   => If a viral file is detected on the mapped drive, the removal will fail if 
      a program on the remote computer uses this file. 

      Therefore, you should run the tool on every computer.

--------------------------------------------------------------------------------
o  The /EXCLUDE switch will only work with one path, not multiple. An 
   alternative is the /NOFILESCAN switch followed by a manual scan with 
   AntiVirus. This will let the tool alter the registry. Then, scan the computer
   with your AntiVirus, using the current virus definitions. You should be able 
   to clean the file system after completing these steps.

   The following is an example command line that can be used to exclude a single
   drive:

   >"C:\Documents and Settings\user1\Desktop\FxNetsky.exe" 
   /EXCLUDE=M:\ /LOG=c:\FxNetsky.txt

   
   Alternatively, the command line below will skip scanning the file system, 
   but will repair the registry modifications. You should then run a 
   regular scan of the system with proper exclusions:

   >"C:\Documents and Settings\user1\Desktop\FxNetsky.exe" 
   /NOFILESCAN /LOG=c:\FxNetsky.txt


   The name of the log file can be whatever you choose. The name listed is for
   the sole purpose of this example.

--------------------------------------------------------------------------------
Notes: 

o  The greater than symbol (>) is not part of the path. 
o  The name of the log file can be whatever you select. The name listed is for 
   the sole purpose of this example.

--------------------------------------------------------------------------------
OBTAINING AND RUNNING THE TOOL

--------------------------------------------------------------------------------
Note: You must have administrative rights to run this tool on Windows NT 4.0, 
Windows 2000, or Windows XP.
--------------------------------------------------------------------------------
WARNING: For network administrators. If you are running MS Exchange 2000 Server,
we recommend that you exclude the M drive from the scan by running the tool from
a command line with the Exclude switch. For more information, read the Microsoft
knowledge base article, "XADM: Do Not Back Up or Scan Exchange 2000 Drive M" 
(Article 298924).
--------------------------------------------------------------------------------

1.  Download the FxNetsky.exe file from: 
    http://securityresponse.symantec.com/avcenter/FxNetsky.exe. 

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

    Caution: If you are running Windows Me/XP, we strongly recommend that you do
    not skip this step.

7.  Double-click the FxNetsky.exe file to start the removal tool. 

8.  Click Start to begin the process, and then allow the tool to run. 

9.  Restart the computer. 

10. Run the removal tool again to ensure that the system is clean. 

11. If you are running Windows Me/XP, then re-enable System Restore. 

12. Run LiveUpdate to make sure that you are using the most current virus 
    definitions.

Note: The removal procedure may not be successful if Windows Me/XP System 
Restore is not disabled as previously directed, because Windows prevents outside
programs from modifying System Restore. 

When the tool has finished running, you will see a message indicating whether 
W32.Netsky@mm infected the computer. In the case of a removal of the worm, the 
program displays the following results: 

o  Total number of scanned files 
o  Number of deleted files 
o  Number of repaired files 
o  Number of terminated viral processes 
o  Number of fixed registry entries

--------------------------------------------------------------------------------
DIGITAL SIGNATURE
FxNetsky.exe is digitally signed. Symantec recommends that you use only copies 
of FxNetsky.exe, which have been directly downloaded from the Symantec Security 
Response Web site. To check the authenticity of the digital signature, follow 
these steps: 

1.  Go to http://www.wmsoftware.com/free.htm. 

2.  Download and save the chktrust.exe file to the same folder in which you 
    saved FxNetsky.exe (for example, C:\Downloads). 

3.  Depending on your operating system, do one of the following: 
  
    o  Click Start, point to Programs, and then click MS-DOS Prompt. 
    o  Click Start, point to Programs, click Accessories, and then click Command
       Prompt.

4.  Change to the folder in which FxNetsky.exe and Chktrust.exe are stored, and 
    then type: chktrust -i FxNetsky.exe.

    For example, if you saved the file to the C:\Downloads folder, you would 
    enter the following commands:

    cd\
    cd downloads
    chktrust -i FxNetsky.exe

    Press Enter after typing each command. If the digital signature is valid, 
    you will see the following, "Do you want to install and run "W32.Netsky 
    Removal Tool" signed on 04/28/2004 3:53 PM and distributed by: Symantec 
    Corporation"

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
=>  "How to disable or enable Windows Me System Restore" 
=>  "How to turn off or turn on Windows XP System Restore"

For additional information, and an alternative to disabling Windows Me System 
Restore, see the Microsoft Knowledge Base article, "Antivirus Tools Cannot Clean
Infected Files in the _Restore Folder," Article ID: Q263455. 

--------------------------------------------------------------------------------
Revision History: 

April 28, 2004: Updated fixtool (version 1.0.11.0) to remove infections of W32.Netsky.AB@mm 
April 23, 2004: Updated fixtool (version 1.0.10.0) to remove infections of W32.Netsky.Z@mm. 
April 20, 2004: 
Updated fixtool (version 1.0.9.0) to remove infections of W32.Netsky.Y@mm. 
Updated fixtool (version 1.0.8.0) to remove infections of W32.Netsky.X@mm.
April 7, 2004: Updated fixtool (version 1.0.7.0) to remove infections of W32.Netsky.S@mm and W32.Netsky.T@mm. 
March 29, 2004: Updated fixtool (version 1.0.6.0) to remove infections of W32.Netsky.Q@mm. 
March 22, 2004: Updated fixtool (version 1.0.5.0) to remove infections of W32.Netsky.P@mm. 
March 8, 2004: Updated fixtool (version 1.0.4.0) to remove infections of W32.Netsky.K@mm. 
March 1, 2004: Updated fixtool (version 1.0.3.0) to remove infections of W32.Netsky.E@mm. 
March 1, 2004: Updated fixtool (version 1.0.2.0) to remove infections of W32.Netsky.D@mm. 
February 25, 2004: Updated fixtool ( version 1.0.1.0 ) to remove infections of W32.Netsky.C@mm.
 