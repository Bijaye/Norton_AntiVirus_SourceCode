**********************************************************************
Symantec Central Quarantine 3.1                             README.TXT
Copyright (c)2002 Symantec Corporation                       July 2002

**********************************************************************
SYMANTEC CENTRAL QUARANTINE
**********************************************************************
This README.TXT file covers compatibility information, late-breaking
news, and usage tips for Symantec Central Quarantine.  The following 
topics are discussed:

* Minimum System Requirements
* Reboot required
* Quarantine Console & Central Quarantine on the same machine
* Administrator Rights
* Install destination
* Microsoft Internet Explorer 5.5 SP2 or later is required
* "Invalid Internet Name" Error
* NAV Server running Windows NT 4.0 with only IPX/SPX Protocol
* Symantec Quarantine Agent service doesn't start after install
* Certified definition status changes after restart

**********************************************************************
Minimum System Requirements
---------------------------
Windows NT 4.0 Workstation and Server with Service Pack 6a; Windows 
2000 Professional, Server, Advanced Server; Windows XP Professional.

Internet Explorer 5.5 SP2

500 MB to 4 GB disk space recommended for quarantined items


Reboot Required
---------------
You must reboot after the install of Central Quarantine or Quarantine 
Console for these components to function properly.


Quarantine Console & Central Quarantine on the same machine
----------------------------------------------------------
If you intend to install Quarantine Console and Central Quarantine on
the same machine, you must install Quarantine Console first and then 
install Central Quarantine.


Administrator Rights
--------------------
Administrator rights are required to install the Quarantine Console 
and Server.  Make sure that you have proper rights before installing.


Install destination
-------------------
Central Quarantine and Quarantine Console should not be installed on a 
network drive.  

 * Do not select a network drive as the install destination.
 * Do not enter a UNC pathname as the destination.


Microsoft Internet Explorer 5.5 SP2 or later is required 
-----------------------------------------------------
Microsoft Management Console (MMC), used for the Quarantine Console
snap-in, requires Internet Explorer 5.5 SP2 or later.  Be sure Internet 
Explorer 5.5 SP2 or later is installed on the system. 


"Invalid Internet Name" Error
-----------------------------
The Central Quarantine name should not contain high-ASCII or double-
byte characters.  Forwarding to the Quarantine from clients will not 
work properly. 

To use a server name that contains high-ASCII characters, either the 
IP address of the server must be entered in the Central Quarantine name 
field or an entry must be made in the LMHOSTS file.


NAV Server running Windows NT 4.0 with IPX/SPX Protocol only 
------------------------------------------------------------
The Service Advertising Protocol (SAP) is required for Symantec 
antivirus servers running under Windows NT 4.0 with only the IPX/SPX Protocol installed.  SAP is necessary for proper distribution of 
updated virus definitions that have been sent to Central Quarantine 
from Symantec Security Response.


Symantec Quarantine Agent service doesn't start after install
-------------------------------------------------------------
The Symantec Quarantine Agent service, which submits virus samples to
Symantec Security Response, might not start with the initial restart 
after install.  If this occurs, either restart the computer again or 
start the Symantec Quarantine Agent service manually from the Services 
control panel. 

In the future, the Symantec Quarantine Agent service will start 
without intervention.


Certified definition status changes after restart
-------------------------------------------------
If the Central Quarantine properties report that certified definitions
have been downloaded to the Central Quarantine and you shutdown and 
restart the Central Quarantine computer, the definitions status is 
incorrectly reset to non-certified.  In practice, this status change 
does not affect processing and can be ignored.  The status is reset 
correctly the next time a definition update is received at the 
Central Quarantine.


**********************************************************************
                           End of File
**********************************************************************
