**********************************************************************
Symantec System Center 4.01                                 README.TXT
Symantec AntiVirus Corporate Edition 7.01                January, 2000
      Copyright 1999 - 2003 Symantec Corporation. All rights reserved.
**********************************************************************
This README.TXT file discusses BackOffice/SMS support issues.

SMS Installation Issues
-----------------------
 * Managed silent install
 * Replacing SETUP.ISS for a silent, unmanaged install

Other Issues
------------
 * Administrator README

**********************************************************************
SMS Installation Issues
**********************************************************************

Managed silent install
----------------------
For the default silent (managed) install on Disk 2 to work properly,
you must edit the GRC.DAT file in the corresponding OS folder (for
example, WINNT for Windows NT/2000).

Complete the following steps:

 1. Open the appropriate copy of GRC.DAT with a text editor, such
    as Notepad.
 2. Locate the line that beings with Parent= 
 3. After the = sign, add the following:
      S<SERVERNAME>
    where <SERVERNAME> is the name of your server.
 4. Save and close the text file.

The install folder must be copied locally and GRC.DAT must be changed
and saved before the install is rolled out (via the SMS PDF).

Replacing SETUP.ISS for a silent, unmanaged package
---------------------------------------------------
If you want to silently install the unmanaged version, you must
replace the SETUP.ISS file before rolling out.

Before rolling out (via the PDF), copy the SETUP.ISS file from the 
UNMANAGE folder and replace the existing one in the appropriate
platform folder (for example, copy it to the WINNT folder for a 
Windows NT package).

The install folder must be copied locally in order to save and 
make these changes. 

**********************************************************************
Other Issues
**********************************************************************

Administrator README
--------------------
Please review the README.TXT file in the root directory of this CD in
its entirety before you install Symantec System Center, roll out the
Symantec AntiVirus Corporate Edition, or call technical support, as it
contains information not included in the Symantec System Center 
Administrator's Guide, the Symantec AntiVirus Corporate Edition 
Implementation Guide, and the online help.

**********************************************************************
                          End of File
**********************************************************************
