**********************************************************************
Symantec Central Quarantine 3.0                             README.TXT
Copyright (c)2001 Symantec Corporation                   November 2001

**********************************************************************
SYMANTEC CENTRAL QUARANTINE
**********************************************************************
This README.TXT file covers compatibility information, late-breaking
news, and usage tips for Symantec Central Quarantine.  The following 
topics are discussed:

* Minimum System Requirements
* Reboot required
* Quarantine Console & Quarantine Server on the same machine
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
Windows 2000 or NT Server 4.0 with Service Pack 5 or higher
NT Server DCOM module (needed for NT Server 4.0 only)
128 MB RAM
250 MB minimum disk space for swapfile 
12 MB available disk space for the Quarantine Console
15 MB available disk space for the Quarantine Server 
Up to 4 GB Available disk space for quarantined items
Internet Explorer 5.5 SP2
Microsoft Management Console (MMC) 1.2 for the Quarantine console

Reboot Required
---------------
You must reboot after the install of Quarantine Server or Quarantine 
Console for these components to function properly.

Quarantine Console & Quarantine Server on the same machine
----------------------------------------------------------
If you intend to install Quarantine Console and Quarantine Server on
the same machine, you must install Quarantine Console first and then 
install Quarantine Server.

Administrator Rights
--------------------
Administrator rights are required to install the Quarantine Console 
and Server.  Make sure that you have proper rights before installing.

Install destination
-------------------
Quarantine Server and Quarantine Console should not be installed on a 
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
The Quarantine Server name should not contain high-ASCII or double-
byte characters.  Forwarding to the Quarantine from clients will not 
work properly. 

To use a server name that contains high-ASCII characters, either the 
IP address of the server must be entered in the Quarantine server name 
field or an entry must be made in the LMHOSTS file.

NAV Server running Windows NT 4.0 with IPX/SPX Protocol only 
------------------------------------------------------------
The Service Advertising Protocol (SAP) is required for Symantec AntiVirus
Servers running under Windows NT 4.0 with only the IPX/SPX Protocol 
installed.  SAP is necessary for proper distribution of updated virus
definitions that have been sent to Central Quarantine from SARC.

Symantec Quarantine Agent service doesn't start after install
-------------------------------------------------------------
The Symantec Quarantine Agent service, which submits virus samples to
SARC, might not start with the initial restart after install.  If this 
occurs, either restart the computer again or start the Symantec 
Quarantine Agent service manually from the Services control panel. 

In the future, the Symantec Quarantine Agent service will start 
without intervention.

Certified definition status changes after restart
-------------------------------------------------
If the Central Quarantine properties report that certified definitions
have been downloaded to the Quarantine Server and you shutdown and 
restart the Quarantine Server computer, the definitions status is 
incorrectly reset to non-certified.  In practice, this status change 
does not affect processing and can be ignored.  The status is reset 
correctly the next time a definition update is received at the 
Quarantine Server.


**********************************************************************
                           End of File
**********************************************************************
