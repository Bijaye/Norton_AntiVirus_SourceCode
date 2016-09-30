W32.Sasser Removal Tool
=============================

Discovered on: May 1, 2004 
Last Updated on: June 11, 2004  

Symantec Security Response has developed a removal tool to clean the infections of the following variants of the W32.Sasser worm: 

  * W32.Sasser.Worm 
  * W32.Sasser.B.Worm 
  * W32.Sasser.C.Worm 
  * W32.Sasser.D 
  * W32.Sasser.E.Worm 
  * W32.Sasser.G

The W32.Sasser family of worms can run on (but not infect) Windows 95/98/Me computers. Although these operating systems cannot be infected, they can still be used to infect vulnerable systems that they are able to connect to. In this case, the worm will waste a lot of resources so that programs cannot run properly, including our removal tool. (On Windows 95/98/Me computers, the tool should be run in Safe mode.)

--------------------------------------------------------------------------------
Note: The execution of the worm causes LSASS.EXE to crash on some systems. The result of this is that the system may undergo a reboot. The fixtool can be successfully run only after the system has completed the reboot.

--------------------------------------------------------------------------------
WHAT THE TOOL DOES

The W32.Sasser Removal Tool does the following: 

1.  Terminates the W32.Sasser viral processes. 
2.  Deletes the W32.Sasser files. 
3.  Deletes the registry values that the worm adds.


--------------------------------------------------------------------------------
AVAILABLE COMMAND-LINE SWITCHES AVAILABLE WITH THIS TOOL

   Switch                  Description 

/HELP, /H, /?       Displays the help message. 

/NOFIXREG           Disables registry repair. (We do not recommend using this 
                    switch.) 

/SILENT, /S         Enables silent mode. 

/LOG=<path name>    Creates a log file where <path name> is the location in 
                    which to store the tool's output. By default, this switch 
                    creates the log file FxSasser.log in the same folder from 
                    which the removal tool was executed. 

/MAPPED             Scans the mapped network drives. (We do not recommend using 
                    this switch. Refer to the following Notes.) 

/START              Forces the tool to immediately start scanning. 

/EXCLUDE=<path>     Excludes the specified <path> from scanning. (We do not 
                    recommend using this switch.) 

-------------------------------------------------------------------------------------------
Note: Using the /MAPPED switch does not ensure the complete removal of the virus on the 
remote computer, because: 

* Scanning the mapped drives scans the mapped folders only. This action may not 
  include all the folders on the remote computer, leading to missed detections. 
* If a viral file is detected on the mapped drive, the removal will fail if a 
  program on the remote computer uses this file. 

  For these reasons, run the tool on every computer.

--------------------------------------------------------------------------------
The /EXCLUDE switch will only work with one path, not multiple. An alternative
is the /NOFILESCAN switch, followed by a manual scan with AntiVirus. This will 
allow the tool to alter the registry. 

Then, scan the computer with AntiVirus and the current virus definitions. You 
should be able to clean the file system after completing these steps.

The following example command line can be used to exclude a single drive:

>"C:\Documents and Settings\user1\Desktop\FxSasser.exe" 
/EXCLUDE=M:\ /LOG=c:\FxSasser.txt


Alternatively, the command line below will skip the scanning of the file system,
but will repair the registry modifications. Run a regular scan of the system 
with the proper exclusions:

>"C:\Documents and Settings\user1\Desktop\FxSasser.exe" 
/NOFILESCAN /LOG=c:\FxSasser.txt

--------------------------------------------------------------------------------
Notes: 
o  The greater than symbol (>) is not part of the path. 
o  The name of the log file can be whatever you select. The name listed is for 
   the sole purpose of this example.
--------------------------------------------------------------------------------

OBTAINING AND RUNNING THE TOOL
------------------------------------------------------------------------------------
Note: You must have administrative rights to run this tool on Windows 2000 or 
Windows XP.
------------------------------------------------------------------------------------
 
1.  Download the FxSasser.exe file from: 
    http://securityresponse.symantec.com/avcenter/FxSasser.exe. 

    Note: Version 1.0.1 (As shown in the removal tool dialog title bar) provides
    support for both W32.Sasser.B.Worm and W32.Sasser.Worm. 

2.  Save the file to a convenient location, such as your downloads folder or the
    Windows desktop, or removable media known to be uninfected.  

3.  To check the authenticity of the digital signature, refer to the section, 
    "Digital signature." 

4.  Close all the running programs before running the tool. 

5.  If you are on a network or if you have a full-time connection to the 
    Internet, disconnect the computer from the network and the Internet. 

6.  If you are running Windows XP, then disable System Restore. Refer to the 
    section, "System Restore option in Windows Me/XP," for additional details.

    CAUTION: If you are running Windows XP, we strongly recommend that you do 
    not skip this step. 

7.  Do one of the following: 
    o If you are running Windows NT/2000/XP, skip to step 8. 
    o If you are running Windows 95/98/Me, restart the computer in Safe mode. 
      For instructions, read the document, "How to start the computer in Safe 
      Mode."

8.  Double-click the FxSasser.exe file to start the removal tool. 

9. Click Start to begin the process, and then allow the tool to run. 

10. Restart the computer. 

11. Run the removal tool again to ensure that the system is clean. 

12. If you are running Windows Me/XP, then re-enable System Restore. 

13. Run LiveUpdate to make sure that you are using the most current virus 
   definitions.

Note: The removal procedure may not be successful if Windows Me/XP System 
Restore is not disabled as previously directed, because Windows prevents outside
programs from modifying System Restore. 

When the tool has finished running, you will see a message indicating whether 
W32.Sasser infected the computer. In the case of a removal of the worm, the 
program displays the following results: 

o Total number of scanned files 
o Number of deleted files 
o Number of repaired files 
o Number of terminated viral processes 
o Number of fixed registry entries

---------------------------------------------------------------------------------
DIGITAL SIGNATURE

FxSasser.exe is digitally signed. Symantec recommends that you only use copies of 
FixBlast.exe that have been directly downloaded from the Symantec Security 
Response Web site. To check the authenticity of the digital signature, follow 
these steps: 

1.  Go to http://www.wmsoftware.com/free.htm. 

2.  Download and save the chktrust.exe file to the same folder in which you 
    saved FxSasser.exe (for example, C:\Downloads). 

3.  Depending on your operating system, do one of the following: 
    o  Click Start, point to Programs, and then click MS-DOS Prompt. 
    o  Click Start, point to Programs, click Accessories, and then click Command
       Prompt.

4.  Change to the folder in which FxSasser.exe and Chktrust.exe are stored, and 
    then type: chktrust -i FxSasser.exe.

    For example, if you saved the file to the C:\Downloads folder, you would 
    enter the following commands:

    cd\
    cd downloads
    chktrust -i FxSasser.exe

    Press Enter after typing each command. If the digital signature is valid, 
    you will see the following, ""Do you want to install and run "W32.Sasser 
    Removal Tool" signed on 05/10/2004 3:45 PM and distributed by: Symantec 
    Corporation"

    Notes: 
    * The date and time displayed in this dialog box will be adjusted to your 
      time zone if your computer is not set to the Pacific time zone. 
    * If you are using Daylight Saving time, the displayed time will be exactly 
      one hour earlier. 
    * If this dialog box does not appear, there are two possible reasons: 
      => The tool is not from Symantec. Unless you are sure that the tool is 
         legitimate and that you downloaded it from the legitimate Symantec Web 
         site, do not run it. 
      => The tool is from Symantec and is legitimate, however, your operating 
         system was previously instructed to always trust content from Symantec.
         For information on this and on how to view the confirmation dialog 
         again, read the document "How to restore the Publisher Authenticity 
         confirmation dialog box."

5.  Click Yes to close the dialog box. 

6.  Type Exit, and then press Enter. This will close the MS-DOS session.

---------------------------------------------------------------------------------
SYSTEM RESTORE OPTION IN WINDOWS XP

Users of Windows Me and Windows XP should temporarily turn off System Restore. 
Windows Me/XP uses this feature, which is enabled by default, to restore the 
files on your computer in case they become damaged. If a virus, worm, or Trojan 
infects a computer, System Restore may back up the virus, worm, or Trojan on the
computer.

Windows prevents outside programs, including antivirus programs, from modifying 
System Restore. Therefore, antivirus programs or tools cannot remove threats in 
the System Restore folder. As a result, System Restore has the potential of 
restoring an infected file on your computer, even after you have cleaned the 
infected files from all the other locations.

Also, a virus scan may detect a threat in the System Restore folder even though 
you have removed the threat.

For instructions on how to turn off System Restore, read your Windows 
documentation, or the article How to turn off or turn on Windows XP System 
Restore.

---------------------------------------------------------------------------------
Revision History: 

June 10, 2004: Added W32.Sasser.G as a variant fixtool version 1.0.4 can remove. 
May 10, 2004: Updated fixtool ( version 1.0.4) to remove infections of W32.Sasser.E.Worm. 
May 3, 2004: Updated fixtool ( version 1.0.3) to remove infections of W32.Sasser.D 
May 2, 2004: Updated fixtool ( version 1.0.2 ) to remove infections of W32.Sasser.C.Worm 
May 2, 2004: Updated fixtool ( version 1.0.1 ) to remove infections of W32.Sasser.B.Worm.
