========================================================================
       WIN32 APPLICATION : APDiag - For NT Platforms only
========================================================================
This tool is dependent on ToolBox.lib and on ccTrust.lib...


This application will attempt to check that all depedencies for AP are working
correctly and log the results of each check. If any problems are found it will
attempt to correct the problem.

Dependencies checked:
Symevent.sys
Naveng.sys
Navex15.sys
Savrtpel.sys
Savrt.sys
Navapsvc.exe
Navapw32.dll
ccapp.exe
Navapscr.dll
Hawkings

/////////////////////////////////////////////////////////////////////////////
Initial checks (APDiag.cpp):
*1. Check that the system is NT
*2. Check for administrative rights
*3. The version of NAV on the system is 9.00.67 or greater (could possibly have
    a bad version.dat so check a file version as well and update the version.dat
    accordingly).

/////////////////////////////////////////////////////////////////////////////
Symevent checks (symevent.cpp):
*1. The Service key exists for Symevent.sys.
*2. The file exists that this key points to.
*3. Log the version of the symevent.sys file.
4. Do a createfile on the Symevent driver to ensure that it is running.

/////////////////////////////////////////////////////////////////////////////
Naveng and Navex15 checks (engines.cpp):
*1. The Service keys exist.
*2. The files exist that the keys point to.

/////////////////////////////////////////////////////////////////////////////
Savrt and Savrtpel checks (savrt.cpp):
*1. The Service keys exist.
*2. The files exist that the keys point to.
*3. NAVAPEL.sys and NAVAP.sys keys and files do not exist (if so destroy them).

/////////////////////////////////////////////////////////////////////////////
Hawkings checks (engines.cpp):
*1. The AvengeDefs key exists (if not create it).
*2. InitWindowsApp on the AP key
*3. Get the current virus defs directory and make sure it exists with the
    proper files in it.
4. Verify the virus defs folder exists with correct files

/////////////////////////////////////////////////////////////////////////////
Ccapp checks (apchecks.cpp)
1. The file exists and is signed
2. The process is currently running
3. The plug-in key for NAV exists and is correct

/////////////////////////////////////////////////////////////////////////////
Navapw32.dll and Navapscr.dll checks (apchecks.cpp)
*1. The files exist and are signed
2. The navap window class is created (agent is running)
3. Put_Enabled call succeeds
*4. Registering Navapscr.dll succeeds

/////////////////////////////////////////////////////////////////////////////
Navapsvc.exe checks (apchecks.cpp)
*1. The file exists and is signed
*2. The Service key exists and points to the correct location of the service

/////////////////////////////////////////////////////////////////////////////
Logging (logging.cpp):
*1. Create a log file
*2. Log a printf formatted text string
*3. Close the log file (destructor)

4. turn on debug logs and retrieve them...
5. turn on AP debug switch and get system logs...

/////////////////////////////////////////////////////////////////////////////
Generic funcitons (shared.cpp)
*1. Ensure a specified file exists is signed (using ccTrust to check).
*2. Register a COM DLL and report any errors.
