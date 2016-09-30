********************************************************************************
Symantec Client Firewall                                              README.TXT 
Copyright 1999 - 2003 Symantec Corporation. All rights reserved.      April 2003 
********************************************************************************
Please review this document in its entirety before you install or roll out 
Symantec Client Firewall, or call for technical support. It contains information 
not included in the Symantec Client Firewall documentation or the online Help.

Installation issues
-------------------
*  Windows 98, Second Edition and uninstalling Symantec Personal Firewall
*  Reinstalling the firewall client without the /f command
*  Providing credentials for network shares before starting Symantec Packager
*  Using Symantec Packager to deploy firewall packages from a Windows 2000 
   computer to a remote computer running Windows NT 4.0
*  Reboot after installing the firewall client
*  Installing the firewall on computers that access other resources using UNC
*  Upgrading to MSI 2.0 causes forced reboot on Windows 98/Me

Migration issues
----------------
*  Symantec Client Firewall settings are not saved when migrating to the 
   current version on computers running Windows 98/98 SE

Updates
---------
*  The LiveUpdate Administration Utility product line list must be updated 
   before retrieving updates


Deployment with third-party products
----------------------------------
*  Deploying packages with Microsoft SMS


Symantec Client Firewall Administrator issues
---------------------------------------------
*  Renaming a folder for a policy package
*  Using a rule that blocks all network traffic
*  Specifying pRule version matching information
*  Upgrading DirectX 
*  Symantec Client Firewall Administrator and Symantec Enterprise Security
*  Selecting hidden files 
*  Symantec Client Firewall Administrator and the File Import/Export Utility 
   handle user rules differently


Symantec Client Firewall issues
-------------------------------
*  User-created rules deleted by pRules if Program rules are not in sync
*  Normal DNS transmissions may use a random port that could be blocked by a 
   Trojan Horse rule 
*  No support for shutting down Symantec Client Firewall using Windows Task 
   Manager 
*  Novell Netware clients and IP 
*  Solving Windows 98 authentication problems during logon
*  XP SP2 Firewall Status Incorrectly Reported As Enabled After Reboot


Documentation errors
--------------------
*  Enabling IP fragmentation blocking for Windows 9x/NT 4.0


-------------------
Installation issues 
-------------------

Windows 98, Second Edition and uninstalling Symantec Personal Firewall
----------------------------------------------------------------------
Before installing Symantec Client Firewall on computers running Windows 98, 
Second Edition and Symantec Personal Firewall, you must first manually uninstall 
Symantec Personal Firewall. 

To uninstall Symantec Personal Firewall:
1. Click Start > Settings > Control Panel > Add/Remove Programs. 
2. In the Add/Remove Programs Properties window, on the Install/Uninstall tab, 
   select Symantec Personal Firewall, and then click Add/Remove. When the uninstall 
   process finishes, restart your computer and install Symantec Client Firewall.


Reinstalling the firewall client without the /f command
-------------------------------------------------------
If you are reinstalling Symantec Client Firewall after having changed settings, 
do not use the /f command-line option. Reinstalling Symantec Client Firewall 
with this option after changing settings may result in a settings violation 
alert.

If you get the settings alert in this situation, make sure to click Yes. The 
system will prompt you for a reboot, which you should allow. After the system 
has rebooted, the previous Symantec Client Firewall settings will be restored.


Providing credentials for network shares before starting Symantec Packager
--------------------------------------------------------------------------
If using network shares with Symantec Packager, you must provide the credentials 
prior to starting Symantec Packager. You will not be prompted for credentials 
within Symantec Packager.


Using Symantec Packager to deploy firewall packages from a Windows 2000
computer to a remote computer running Windows NT 4.0
-----------------------------------------------------------------------
From Symantec Packager, if you deploy a Symantec Client Firewall package from a 
Windows 2000 computer to a remote computer running Windows NT 4.0 and the package 
runs without errors, a value of Unknown will appear in the Status column of the 
Deployment Status window.


Rebooting after installing the firewall client
----------------------------------------------
After a Symantec Client Firewall installation, if you cancel the requested 
reboot, network communications will not work. Make sure to reboot the system 
before attempting network access.


Installing the firewall on computers that access other resources using UNC
--------------------------------------------------------------------------
If you install the firewall on a computer that accesses other resources using 
UNC (for example,\\server-name\shared-resource-pathname), you will need to change 
the Symantec Client Firewall Default NetBIOS Name Inbound rule setting to Permit 
TCP/UDP in order to access shared UNC resources from the computer.


Upgrading to MSI 2.0 causes forced reboot on Windows 98/Me
----------------------------------------------------------
Symantec Client Security components utilize MSI version 2.0. Computers running 
previous versions of MSI will automatically be upgraded to version 2.0. On 
Windows 98/Me, this upgrade involves an additional reboot to activate the new 
version. This reboot is unrelated to any reboots required to install the 
Symantec Client Security components, so a reboot may be required during install
even though all reboots are suppressed in the component installations. This 
reboot is initiated by the MSI upgrade and cannot be suppressed.



----------------
Migration issues
----------------

Symantec Client Firewall settings are not saved when migrating to the current 
version on computers running Windows 98/98 SE
-----------------------------------------------------------------------------
When you migrate a Windows 98/98 SE computer from a previous version of 
Symantec Client Firewall or Symantec Client Security to the current version of 
the firewall, the settings in the previous version are not preserved during 
migration. After the installation is complete, you must reconfigure the 
firewall settings on the affected computer.



---------
Updates
---------

The LiveUpdate Administration Utility product line list must be updated before 
retrieving updates
------------------------------------------------------------------------------
The LiveUpdate Administration Utility product line list must be updated before 
you can use it to download updates. When the update is complete, new products 
appear in the Symantec product line list in the LiveUpdate Administration 
Utility.

To update the LiveUpdate Administration Utility product line list:
1. Click Start > Programs > LiveUpdate Administration Utility > LiveUpdate 
   Administration Utility.
2. Make at least one product selection in the Symantec product line list.
3. Click Retrieve.
4. Follow the on-screen instructions.



--------------------------------------------------------
Deployment with third-party products
--------------------------------------------------------

Deploying packages with Microsoft SMS
-------------------------------------
If you are deploying packages using Microsoft Systems Management Server (SMS), 
you may need to disable the Show Status Icon On The Toolbar For All System 
Activity feature on the clients in the Advertised Programs Monitor. 

In some situations, Setup.exe may need to update a shared file that is in 
use by the Advertised Programs Monitor. If the file is in use, the installation 
will fail.



---------------------------------------------
Symantec Client Firewall Administrator issues
---------------------------------------------

Renaming a folder for a policy package
-------------------------------------
The Java-based dialog boxes in the Symantec Client Firewall Administrator 
sometimes work differently than Windows dialog boxes. 

For example, to rename a folder while saving a policy package, use the following 
steps:
1. Go to File > Save As > OK.
2. Click the folder in the Save window.
3. Type a new name for the folder, then press Enter.
4. Double-click the folder.
5. Click Save to save the policy package.


Using a rule that blocks all network traffic
--------------------------------------------
When using a Block All rule that prevents network traffic from getting through 
to clients, two additional permit rules must be enabled and applied before the 
Block All rule. These two rules are the Inbound BootP and Default Outbound BootP 
system-wide rules that are supplied by default with Symantec Client Firewall.

This action is necessary so that DHCP will function properly, renewing IP 
addresses on computers affected by the Block All rule and reestablishing 
communication between those computers and the network when the Block All rule 
is disabled. 


Specifying pRule version matching information
---------------------------------------------
You can use pRule version matching criteria to match executable file names with 
up to four qualifiers; for example, 6.0.1.2466. The firewall client will match 
the qualifiers you specify and include more specific sub-qualifiers as matches. 
For example, if you specify 6 as a version match criteria value when 
configuring a pRule, the pRule will match versions 6, 6.0, 6.4, 6.11, 6.1.2, and 
so forth.

If you specify 6.0.1 as a version match criteria value when configuring a pRule, 
the pRule will match versions 6.0.1.2, 6.0.1.3, and so forth. It will not match 
versions 6.1.1 or 6.0.2.

Note that if you use the pRule required digest matching feature, you do not need 
to use match criteria like version checking. If you do use version checking, it 
is possible to use additional criteria such as company name to make the pRule 
more specific. 


Upgrading DirectX
-----------------
If Symantec Client Firewall Administrator does not run (that is, the user 
interface does not appear), and the system requirements are fulfilled, upgrading 
the version of DirectX on the computer to 8.1 or later may solve the problem.

You can download the upgrade for DirectX on the Microsoft Web site at:
http://www.microsoft.com
Use DirectX as a search keyword to locate the Web page.


Symantec Client Firewall Administrator and Symantec Enterprise Security
Architecture (SESA)
-----------------------------------------------------------------------
Do not install the Symantec Client Firewall Administrator on any computer that 
is also running SESA or a SESA Agent. SESA components use different versions of 
the Java Runtime Environment (JRE). Symantec Client Firewall Administrator 
installs version 1.4 of the JRE; SESA and SESA Agents use version 1.3.1 of the 
JRE. 


Selecting hidden files
----------------------
When browsing the file system with Symantec Client Firewall Administrator, you 
will not be able to view or select any hidden files and folders. If you need to 
select and use a file through Symantec Client Firewall Administrator, make sure 
that the file's hidden attribute is not turned on.


Symantec Client Firewall Administrator and the File Import/Export Utility handle 
user rules differently
--------------------------------------------------------------------------------
If an XML file containing newly created user rules is opened using Symantec 
Client Firewall Administrator, the user rules are lost when the XML file is 
saved. If you use the File Import/Export Utility (fio.exe), the user rules are 
retained in the Symantec Client Firewall package.



-------------------------------
Symantec Client Firewall issues
-------------------------------

User-created rules deleted by pRules if Program rules are not in sync
---------------------------------------------------------------------
If a user creates a Program rule for the same executable file contained in 
a pRule, but the user-created Program rule contains different communication 
rules for the Program, the pRule deletes all user-created rules and replaces 
them with the rules contained in the pRule.


Normal DNS transmissions may use a random port that could be blocked by a Trojan
Horse rule
--------------------------------------------------------------------------------
Part of the DNS protocol involves communication between a random UDP port 
(1024 through 65536) on the client and UDP port 53 on the server. If one of the 
ports chosen randomly is also a port used by a Trojan horse threat, the 
communication will be blocked by the firewall. You can prevent this situation 
by taking either of two possible courses of action:

-  Include the DNS server in the Trusted Zone.
-  Create a system-wide rule that allows UDP communications through specific  
   ports that are also blocked by Trojan Horse rules. Since the system-wide 
   rule allowing the UDP communication always has greater precedence, a Trojan 
   Horse rule will not get triggered if DNS is randomly using one of the ports. 


No support for shutting down Symantec Client Firewall using Windows Task Manager
--------------------------------------------------------------------------------
Shutting down Symantec Client Firewall services on any Windows NT/2000/XP 
computer using Windows Task Manager is not supported. 


Novell NetWare clients and IP
-----------------------------
Symantec Client Firewall affects logons with Novell NetWare clients. In order to 
permit logon to Novell NetWare servers from Novell NetWare clients running IP, 
an explicit rule that permits communication through port 427 needs to be 
implemented and rolled out to the NetWare clients.


Solving Windows 98 authentication problems during logon
-------------------------------------------------------
On Windows 98, if you have authentication problems logging in to a domain after 
installing Symantec Client Firewall, disable the following Symantec Client 
Firewall rules: 

- Default Inbound NetBIOS Name
- Inbound NetBIOS


XP SP2 Firewall Status Incorrectly Reported As Enabled After Reboot
-------------------------------------------------------------------
In a specific circumstance, the Windows Firewall may temporarilly report itself
as enabled when it is actually not.  This occurs when the following installation
sequence has occured:
1.  XP SP1 installed.
2.  Symantec Client Security installed and is configured to disable Windows
    Firewall (default).
3.  XP SP2 installed.

In this case, the Windows Security Center will generally correctly display Windows
Firewall as disabled after a 1 minute wait.



--------------------
Documentation errors
--------------------

Enabling IP fragmentation blocking for Windows 9x/NT 4.0
--------------------------------------------------------
Page 68 of the Symantec Client Firewall Implementation Guide contains 
information on how to configure pRules to support networking in an Active 
Directory environment. For clients running Windows 9x/NT 4.0 only, IP 
fragmentation blocking must be disabled.  

To disable IP fragmentation blocking:
1. On the Options menu, click Advanced Options.
2. Uncheck Block Fragmented IP Packets.



********************************************************************************
END OF FILE 
********************************************************************************
