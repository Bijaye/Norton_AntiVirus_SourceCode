********************************************************************************
Symantec AntiVirus(TM)                                                README.TXT
Copyright 1999 - 2005 Symantec Corporation. All rights reserved.      June 2005
********************************************************************************

Please review this document in its entirety before you install or roll out 
Symantec AntiVirus Corporate Edition, or call for technical support. It 
contains information not included in the Symantec AntiVirus documentation 
or the online Help.



Installation and uninstallation issues
--------------------------------------
*  Symantec Packager is unsupported
*  Symantec AntiVirus cannot be added to a software installation Group 
   Policy Object for Active Directory (R) deployment if the same version of 
   Symantec AntiVirus is already installed
*  Restart may be necessary for Central Quarantine, and Symantec 
   System Center installs
*  Antivirus server installations require a restart if you install
   the Alert Management Service(2) software
*  Communication problem when you uninstall a Symantec AntiVirus server 
   that is on the same computer as the Symantec System Center
*  The SERVERNAME client installation MSI property must be a valid name
*  The "Server Name" field during managed client installation must be a
   valid name
*  "Installing AMS" portion of SAV server install may take several minutes
*  Upgrading SAV10 requires additional rights


Migration issues
----------------
*  Updating to a new parent server during client migration by using Grc.dat
*  Unmanaged to managed client migration is unsupported
*  Version 7.6.1 migration fails with opened client or server user interfaces


Deployment with third-party products
------------------------------------
*  Deploying user-based installation packages with elevated privileges fails


Management-related issues
-------------------------
*  Back up the \pki directory on your primary server
*  Unregistered file extensions for non-hidden files are resolved with the 
   FixFileTypes tool
*  Rolling back server system dates disables servers 
*  Stopping Symantec processes causes instability
*  SAVRoam /nearest command requires administrator rights on Windows
*  Deleting locked and empty server groups
*  Group settings are applied to out-of-sync clients
*  Symantec System Center prompts restricted users for server group
   certificate
*  Changing the management mode of a client
*  Users are no longer allowed to modify scheduled LiveUpdates
*  Information about a user's logon domain is not available until restart
*  Setting for allowing clients to modify LiveUpdate schedules is locked
*  When your client email applications use a single Inbox file
*  Only locked settings are propagated to clients
*  Simple host name resolution required to manage Symantec AntiVirus 
   servers and clients
*  Dragging and dropping servers after changing Login Certificate 
   Settings results in loss of communication
*  Time out-of-sync error when you promote a server to primary server


Antivirus client and server issues
----------------------------------
*  Cookies are not scanned
*  Internet E-mail Auto-Protect port changes are ignored
*  Client users who schedule scans must be logged on for the scans to run
*  Improving client restart performance
*  Adding a file to the Quarantine and removing it from its original location
*  Scanning files by type is no longer available
*  Cleaning risks and side effects
*  Cleaning worms and Trojan horses 
*  Do not use third-party security risk scanners when Tamper Protection 
   is enabled
*  Setting Client Tracking options
*  Auto-Protect option to scan for security risks does not apply to computers
   that run earlier versions of Symantec AntiVirus


NetWare information
-------------------
*  Auto-Protect option to scan for security risks does not apply to computers 
   that run NetWare
*  Quick Scans not supported on NetWare
*  Configuring exceptions for security risks
*  NetWare servers may fail to communicate during server group drag and
   drop operations
*  Secure Console requires search path additions for installation or
   upgrade
*  How to proceed with an upgrade if a timeout occurs waiting for SAV
   to shutdown
*  NetWare log file now stored in SYS:install.log
*  Password at server console disabled by default
*  Install command no longer attempts to repair a damaged installation
*  How to regenerate certificates
*  SAV server upgrade on NetWare defaults to bindery mode
*  Secondary server install and migrations may fail if IPX protocol is bound


64-bit support
--------------
*  Antivirus client is the only feature that is supported
*  LiveUpdate is the only virus definitions files update method supported
*  Managing 64-bit clients with the Symantec System Center
*  Side effects repair limitations


Documentation issues
--------------------
*  Reference guide Event Log entries chapter contains wrong information


Other issues
------------
*  XP SP2 Firewall Status Incorrectly Reported As Enabled After Reboot
*  Using System Restore on computers that run Windows XP
*  16-bit files used by Symantec AntiVirus components 
*  Files installed in the Windows directories
*  About security risks
*  About cookies
*  Security risk best practices 
*  Notice - Central Quarantine performance considerations


--------------------------------------------------------
Installation and uninstallation issues
--------------------------------------------------------

Symantec Packager is unsupported
--------------------------------
The use of Symantec Packager and PMI files is unsupported.


Symantec AntiVirus cannot be added to a software installation 
Group Policy Object for Active Directory(R) deployment if the same 
version of Symantec AntiVirus is already installed
--------------------------------------------------------------------------------
You cannot create a Group Policy Object (GPO) package for software installation 
when the same version of the application is installed on the computer. Create 
the Symantec AntiVirus installation GPO before you install Symantec 
AntiVirus to the server.


Restart might be necessary for Central Quarantine, and Symantec 
System Center installs
-------------------------------------------------------------
A restart might be necessary for Central Quarantine because it also updates 
system files. A restart might also be necessary for the Symantec System Center 
since it requires the update of Microsoft(R) Management Console plug-ins.


Antivirus server installations require a restart if you install
the Alert Management Service(2) software
---------------------------------------------------------------
If you select to install the Alert Management Service(2) software 
when you install an antivirus management server, you must restart
the computer before you can configure alerts.


Communication problem when you uninstall a Symantec AntiVirus server 
that is on the same computer as the Symantec System Center 
--------------------------------------------------------------------
If you uninstall a Symantec AntiVirus server that is on the same 
computer as the Symantec System Center, and then reinstall the server, it 
cannot communicate with the Symantec System Center. To work around this 
issue, you can uninstall both the Symantec AntiVirus server and the 
Symantec System Center, then install them again.


The SERVERNAME client installation MSI property must be a valid name
--------------------------------------------------------------------
The SERVERNAME property specifies the name of an existing parent server
that will manage the client.  It must be a name.  Do not use an IP address
for this property.


The "Server Name" field during managed client installation must be a valid name
-------------------------------------------------------------------------------
The "Server Name" field during managed client installation specifies the name
of an existing parent server that will manage the client.  It must be a name.
Do not use an IP address for this field.  If you only know the IP address, click
the "Browse..." button then the "Find Computer..." button to search by IP
address.


"Installing AMS" portion of SAV server install may take several minutes
-----------------------------------------------------------------------
When deploying SAV server, the "Installing AMS" portion of the deployment
may take up to five minutes to complete.


Upgrading SAV10 requires additional rights
-----------------------------------------
Upgrading SAV10 requires modification of the cached install package or the
upgrade will fail.  If SAV10 is detected, the install will abort unless the
user is an administrator of the local machine.  Note that enabling MSI to
run with elevated privileges is not sufficient in this case.  In addition
to installing as a local administrator, the modification can be accomplished
in two other ways:
1.  Temporarily grant users write rights to the Windows\Installer directory
for the duration of the upgrade.
2.  Run the tool Tools\Sav9UninstallFix under the credentials of an account
with write access to Windows\Installer, then execute the upgrade with the
property SAV10UNINSTALLFIXRUN=1 on the command line.



--------------------------------------------------------
Migration issues
--------------------------------------------------------


Updating to a new parent server during client migration by using Grc.dat
------------------------------------------------------------------------
If you migrate a client from a previous version to the current version 
and you are assigning the client to a new parent server, you must add 
the name of the new parent server to the RoamManagingParentLevel0 
entry in the Grc.dat file that you include. This entry is 
a comma-delimited list of available parent servers. If the name of 
the new server is not added to this list, the migrating client will not 
use the new server that you specified.


Unmanaged to managed client migration is unsupported
----------------------------------------------------
The migration of an unmanaged client to a managed client is not supported. 
To resolve this issue, when a managed client is installed over an unmanaged 
client, you can copy a Grc.dat file (which specifies that the client 
be managed by a specific parent server) into the appropriate directory.
The installation chapter in the installation guide describes how to perform
this conversion. 


Version 7.6.1 migration fails with opened client or server user interfaces
--------------------------------------------------------------------------
If you migrate version 7.6.1 servers or clients, and if the user 
interface is open, the migration fails. To migrate version 7.6.1 servers 
and clients to the latest version, close the user interfaces before you
begin migration.



--------------------------------------------------------
Deployment with third-party products
--------------------------------------------------------

Deploying user-based installation packages with elevated privileges fails
-------------------------------------------------------------------------
By setting the Active Directory group policy for Always Install With Elevated
Privileges, users without administrator rights can install Windows Installer
packages. If you install per machine, this setting permits installation to
succeed. If you install per user, this setting causes the installation to
fail.


--------------------------------------------------------
Management-related issues
--------------------------------------------------------

Back up the \pki directory on your primary server
-------------------------------------------------
If you have a server group that contains one server only, that server
is a primary server and manages all clients in the server group. 
If for some reason you have to reinstall server software on the
primary server, you will lose all communications with your clients. 
The reason is that you created a new server group root certificate
that the clients do not trust.

To mitigate this potential problem, always install a secondary server
in your server group so that you can unlock your server group. Further,
always back up the entire \pki subdirectory that is located in the 
directory that contains your server software. If you have the \pki
subdirectory available to restore after your reinstall server software, 
you can reestablish client communications. For detailed procedures,
contact your Symantec technical support representative.


Unregistered file extensions for non-hidden files are resolved with the 
FixFileTypes tool
-----------------------------------------------------------------------
Symantec AntiVirus includes file extensions for non-hidden files that are 
not registered on Windows operating systems. To mark these file types as 
hidden, on the Symantec AntiVirus CD, in the \Tools\FixFileTypes folder, 
run FixFileTypes.exe.


Rolling back server system dates disables servers 
-------------------------------------------------
If you roll back the system date on your primary server to a date that
precedes the server group root certificate creation date, you will not 
be able to use your server.


Stopping Symantec processes causes instability
----------------------------------------------
If you stop Symantec processes that run in the background, the computers
on which you stop the processes will become unstable. For a list of Symantec
processes, refer to the reference guide that is located on your
installation CD.


SAVRoam /nearest command requires administrator rights on Windows
-----------------------------------------------------------------
SAVRoam is typically configured to look for a new parent server after an
amount of time and after a computer restarts. However, the command-line
command SAVRoam /nearest lets you force SAVRoam to look for a new 
parent server immediately. To use this command, users must have
administrator rights on Windows computers.


Deleting locked and empty server groups
---------------------------------------
If you have a server group that contains a primary server only, and if
you uninstall the server, you cannot unlock the server group and delete
it. 

To delete the server group:
1. In the Symantec System Center console, click Tools > Discovery
   Service.
2. In the Discovery Services Properties dialog box, click Clear
   Cache Now.


Group settings are applied to out-of-sync clients
-------------------------------------------------
By default, clients that have system clocks set more than 24 hours plus or 
minus of the time set on the primary management server do not let 
administrators directly configure settings. For example, you cannot
right-click on an out-of-sync client in the Symantec System Center and 
view the client logs. Out-of-sync clients, however, do accept settings 
that administrators apply to groups.  For example, if you right-click a 
group in the Symantec System Center and change the Client Auto-Protect
setting, the out-of-sync client accepts the new setting. 

The reason that you cannot directly configure client settings is because
the system uses the Login certificate, which is valid for a specified
time only. You can change the times in the Symantec System Center with 
Configure login certificate settings at the group level. The reason 
that the out-of-sync clients accept group-level changes is because the 
system uses the server certificate, which is valid for five years.

For more information about certificates, refer to the Reference Guide
in the Docs folder on the installation CD.


Symantec System Center prompts restricted users for server group certificate
----------------------------------------------------------------------------
When you first unlock a server group, the Symantec System Center prompts
you to copy the server group root certificate to the Symantec System Center
directory structure. If you subsequently log on to the computer that runs the
Symantec System Center with administrator rights and unlock the server group, 
you are not prompted to copy the server group root certificate. If you 
subsequently log on to the same computer with low-level user rights, you are
always prompted to copy the server group root certificate.


Changing the management mode of a client 
----------------------------------------
In Chapter 2 of the administrator's guide, in the procedure for changing 
an unmanaged client to a managed client, steps 5 and 6 are no longer 
necessary. If the pki\roots folder on the client is empty, then the 
new parent server now automatically copies the server group root 
certificate and places it in the pki\roots folder on the client after 
you copy the Grc.dat file to the client and restart the client. 
If the pki\roots folder on the client contains its previous server 
group root certificate, you should delete it before you copy the new 
Grc.dat file to the client. 


Users are no longer allowed to modify scheduled LiveUpdates
-----------------------------------------------------------
The "Do not allow client to modify LiveUpdate schedule" option has been 
disabled on the Virus Definition Manager dialog box in the Symantec 
System Center. When you check the "Do not allow client to manually 
launch LiveUpdate" option or the "Schedule client for automatic updates 
using LiveUpdate" option, users are not allowed to modify any scheduled 
LiveUpdates that you configure. This automatic locking ensures that 
LiveUpdates that administrators schedule are always propagated to 
clients and cannot be modified by users.


Information about a user's logon domain is not available until restart  
----------------------------------------------------------------------
After an initial client software installation, the user's logon domain 
information does not appear in the Symantec System Center until the 
client computer is restarted. After a restart, this information is 
available in the Symantec System Center Symantec AntiVirus View, the 
network audit results, the Event Log, the Risk History, and the 
Tamper History. In the Symantec AntiVirus user interface, it is 
available in the Event Log, the Risk History, and the Tamper History.


Setting for allowing clients to modify LiveUpdate schedules is locked
---------------------------------------------------------------------
The Virus Definition Manager window in the Symantec System Center 
contains the following setting, which is locked and dimmed:

o  Do not allow client to modify LiveUpdate schedule

When both of the following settings are disabled, the locked setting
is automatically unchecked and disabled:

o  Schedule client for automatic updates using LiveUpdate
o  Do not allow client to manually launch LiveUpdate

When one or both of these settings are checked and enabled, the locked
setting is automatically checked and enabled. If this setting was 
not locked, client users could create or modify schedules that conflict 
with the group policy and would not receive group-scheduled virus 
definitions updates.


When your client email applications use a single Inbox file 
-----------------------------------------------------------
If your clients use email applications that store all email in a 
single file, such as Outlook Express, Eudora, Mozilla, and Netscape, 
you might want to exclude the Inbox file from manual and scheduled 
scans. If Symantec AntiVirus catches a virus in the Inbox file 
during a manual or scheduled scan, and the action configured for 
the virus is Quarantine, Symantec AntiVirus quarantines the 
entire Inbox and users cannot access their email.

Although regularly excluding a file from scanning is not recommended
as a general practice, excluding the Inbox file from being scanned 
prevents it from being quarantined while still allowing a virus to 
be detected. If Symantec AntiVirus finds a virus when you open an 
email message rather than when you download the message or during a 
scan, it can safely quarantine or delete the message without causing 
a problem for the entire Inbox.


Only locked settings are propagated to clients
----------------------------------------------
To change settings on clients, you must lock the settings in the Symantec
System Center. If you change a setting for clients, and if that setting 
is not locked, the change does not occur on the clients. This feature 
also affects client installations by using the ClientRemote feature in 
the Symantec System Center. Only changed settings that are locked are 
configured on clients during installation. 


Simple host name resolution is required to manage Symantec AntiVirus 
servers and clients
--------------------------------------------------------------------
You must have simple host name resolution configured in your environment 
to manage Symantec AntiVirus servers and clients.  Fully qualified 
domain name resolution is not required.


Dragging and dropping servers after changing Login Certificate 
Settings results in loss of communication
--------------------------------------------------------------
In the Symantec System Center, when you increase the time interval 
set for the two options in the Login Certificate Settings dialog box 
to more than one day (24 hours) to account for time out-of-sync issues 
between servers and clients, and then drag and drop a server into a 
new server group, communication between the server and the Symantec 
System Center is lost.  This does not occur if your actual time 
discrepancy is 24 hours or less.


Time out-of-sync error when you promote a server to primary server
------------------------------------------------------------------
When you promote a server to primary in the Symantec System Center, 
you might get a login certificate time out-of-sync error. In most 
instances, you can work around this issue by clearing the cache, and 
then running a new Discovery. You can then promote the server to be
a primary server.



----------------------------------
Antivirus client and server issues
----------------------------------


Cookies are not scanned
-----------------------
Cookies are not scanned for viruses, threats, or security risks.


Internet E-mail Auto-Protect port changes are ignored
-----------------------------------------------------
The antivirus client Auto-Protect feature for Internet E-mail Advanced
Options lets you change the ports for POP3 and SMTP. The defaults 
for these ports are 110 and 25. The antivirus client ignores 
changes to these defaults. This issue applies to all email 
programs that use POP3 and SMTP, including Microsoft Outlook.

If you change these defaults with the antivirus client but your email
program uses the defaults, Auto-Protect still scans for risks in your
email traffic. If your email program does not use the defaults and
you change the Auto-Protect ports to match the ports used by your
email program, Auto-Protect does not scan for risks in your email 
traffic.


Client users who schedule scans must be logged on for the scans to run
----------------------------------------------------------------------
End users who schedule scans with the antivirus client user interface 
to run when they are not using their computers must be logged on to 
the computer or the scan does not run. Also, if a scheduled scan runs
and the user logs off, the scan stops running. The workaround 
is to have the users lock their computers without logging off. Scans
that are scheduled to run on clients in a server group are not
affected, and always run when scheduled.


Improving client restart performance
------------------------------------
By default, the antivirus client software performs an Auto-Generated
QuickScan when the client computer restarts. QuickScan includes an
enhancement that scans for security risks such as adware and spyware.
This enhancement increases the time required to restart client computers,
and is not configurable from the Symantec System Center.

To improve client startup performance:
1. In the Symantec AntiVirus window, in the left pane, expand 
   Startup Scans, and then click Auto-Generated QuickScan.
2. In the right pane, click Edit.
3. In the Scan dialog box, on the Quick Scan tab, click Options.
4. In the Scan Options dialog box, under Scan Enhancements, uncheck
   Scanning for traces of well-known viruses and Security Risks.
5. Click OK. 
6. In the Scan dialog box, click OK.


Adding a file to the Quarantine and removing it from its 
original location
--------------------------------------------------------
The option to "Remove file from original location" in the Add File 
to Quarantine dialog box does not work after you turn Auto-Protect 
off. When you use the Quarantine View icon to add a file to the 
Quarantine, and you uncheck the option to remove the file from its 
original location, the file is still removed. 


Scanning files by type is no longer available 
---------------------------------------------
Scanning files by type is no longer an option when you configure 
any scan. All types of files are scanned. Any previously configured 
scan that is migrated to the current version will also scan all 
file types. All information about scanning by selected file type 
in the administrator's guide is no longer applicable.


Cleaning risks and side effects
-------------------------------
When Symantec AntiVirus handles complicated risks and their side 
effects, at times it applies the first and second actions you 
configure to different portions of the risk. 

For example, when Symantec AntiVirus detects risks such as the 
Trojan.QQMess Trojan horse, if the first and second actions 
configured are Clean risk and Leave alone (log only), it might appear 
as if the first action failed on the detected files but 
was applied to some components of the risk. This occurs because 
if Symantec AntiVirus detects an infected file that it cannot clean, 
Symantec AntiVirus takes the second action and leaves the 
files alone. However, Symantec AntiVirus applies the first action 
to clean the side effects of the risk, and places these 
remediation targets in the Quarantine.

If the first and second configured actions are Clean risk and 
Delete risk, and Symantec AntiVirus cannot clean the files, 
Symantec AntiVirus applies the second action to the infected files 
and deletes them. Symantec AntiVirus applies the first 
action to the side effects of the risk, cleaning them and placing 
the remediation targets in the Quarantine.


Cleaning worms and Trojan horses 
--------------------------------
When the first configured action for non-macro viruses is Clean risk, 
the action that Symantec AntiVirus takes when it detects some worms 
and Trojan horses by a manual or scheduled scan is different from the 
action that Symantec AntiVirus takes when it detects the same risks by 
an Auto-Protect scan.

In rare cases, when a worm or Trojan horse cannot be cleaned from a 
file, but Auto-Protect determines that it is too dangerous to leave 
on the computer, Symantec AntiVirus deletes the infected files even 
when the first configured action for Auto-Protect is Clean risk and 
the second configured action for Auto-Protect is Leave alone (log 
only).  If a manual or scheduled scan has the same actions configured 
and detects the same uncleanable risk, the Clean action fails and the 
files are left alone (the configured second action).  To remove the 
risk from the computer, you must either change the configured action 
to Delete risk or Quarantine risk, or you must manually remove these files.


Do not use third-party security risk scanners when Tamper Protection 
is enabled
--------------------------------------------------------------------
The antivirus clients and servers have a new real-time feature called 
Tamper Protection that protects Symantec processes from unauthorized access
and tampering. If you run third-party security risk scanners that detect and
defend against unwanted adware and spyware, these scanners generally touch 
Symantec processes. Tamper Protection then generates tens and possibly 
hundreds of alerts and log entries. If you want to use third-party security 
risk scanners, disable Tamper Protection.


Setting Client Tracking options
-------------------------------
In the Symantec System Center, when you set Client Tracking options in the 
Server Tuning Options dialog box, the changes do not take effect until you 
restart the Rtvscan service on the server that you are configuring. 


Auto-Protect option to scan for security risks does not apply to computers 
that run earlier versions of Symantec AntiVirus 
--------------------------------------------------------------------------
When you configure Auto-Protect options in the Symantec System Center, the 
option to Scan for Security Risks does not apply to computers that run 
earlier versions of Symantec AntiVirus.



-------------------
NetWare information
-------------------


Auto-Protect option to scan for security risks does not apply to computers 
that run NetWare 
--------------------------------------------------------------------------
When you configure Server Auto-Protect options in the Symantec System Center, 
the option to Scan for Security Risks does not apply to computers that run 
NetWare.


Quick Scans not supported on NetWare
------------------------------------
Quick Scans are not supported on computers that run NetWare. 


Configuring exceptions for security risks 
-----------------------------------------
When you configure scan options in the Symantec System Center for 
Auto-Protect and other scans, if the parent server or primary server 
runs NetWare, the Available risks list in the Select risks dialog 
box is empty and you cannot configure exceptions.  To work 
around this issue, use a computer running Windows as your primary 
or parent server, or configure exceptions directly on the 
computers managed by the NetWare parent and primary servers.


NetWare servers may fail to communicate during server group drag and
drop operations
--------------------------------------------------------------------
Under some circumstances NetWare servers configured with both IP and
IPX protocols may fail to communicate during drag and drop server group
operations.  This is due to protocol and address resolution failures.
A workaround is to disable the IPX protocol bindings from server(s)
being moved and the primary server of the new server group.  After
restarting both servers and verifying network communication and name
resolution (type "Ping <other server name>" on both server consoles),
then perform the move.  After server drag and drop operations have
been completed, IPX protocol bindings can be enabled on servers that
require this protocol.


Secure Console requires search path additions for installation or
upgrade
--------------------------------------------------------------------
The NetWare Secure Console feature locks down a file server so it can
only load NLMs from the SYS:SYSTEM directory or any search path.  
Consequently, this can prevent a SAV installation or upgrade from
running.  To resolve this, you must add both the SAV and the temporary
deployment directory (e.g., SYS:SAV\Deploy0) to the search path before
loading vpstart for the install or upgrade, and then launch VPStart
from the temporary deployment directory with the /update command line.
The temporary deployment directory can be removed from the search path
after the install or upgrade is complete.

The temporary deployment directory is a subdirectory of the target
directory for installation and will normally be called Deploy0 (e.g.,
full pathname is SYS:SAV\Deploy0).  If a directory with this name
already exists, the deployment process will try ten alternate names,
first using sequential numbers (e.g., Deploy1, Deploy2, etc.), then
random numbers.

The easiest way to perform an upgrade with secure console is to
perform the deployment and allow the vpstart launch to fail.  At the
point, all necessary files reside on the server and you now know the
path of the temporary deployment directory.  To proceed, add this
directory to the search path in addition to the target directory, then
relaunch vpstart using the command "load <target volume path>\vpstart
/update".  For example:
    load sys:sav\deploy0\vpstart /update


How to proceed with an upgrade if a timeout occurs waiting for SAV
to shutdown
--------------------------------------------------------------------
In rare circumstances, Symantec AntiVirus for NetWare may take a
very long time to shutdown, resulting in a timeout during the upgrade
process.  To complete the upgrade, simply find the temporary
deployment directory on the server and run vpstart /update from it.
In most cases, this will be the Deploy0 subdirectory, so the correct
command would be:
   load sys:sav\deploy0\vpstart /update


NetWare log file now stored in SYS:install.log
--------------------------------------------------------------------
The NetWare installation log file is now SYS:\install.log.  This is
used for installs, upgrades, and uninstalls.


Password at server console disabled by default
--------------------------------------------------------------------
The shutdown password at the server console is now disabled by default.
It can be restored by using the Set Password option on the Admin menu


Install command no longer attempts to repair a damaged installation
--------------------------------------------------------------------
The /install command line parameter no longer repairs damaged
installations.  Please note that repairing damaged installations is
not support and may result in incorrect operation of the product.
Symantec recommends that you uninstall and reinstall the product if
the installation is damaged.

In most cases, installation damage is the result of damaged registry
files (e.g., cannot load Symantec AntiVirus).  Damaged registry files
cannot be repaired, but they can be rebuilt.  Rebuilding a damaged
registry requires three steps:
a.  Regenerating the default registry file
b.  Re-populating the address cache
c.  Updating the DomainGUID for secure communications

To regenerate the default registry file:
a.  Enter "unload vpreg.nlm" at the server console
b.  Delete the SYS:SYSTEM\VPREG directory
c.  Enter "load sys:sav\vpstart /regrepair"

To re-populate the address cache:
a.  Enter "load sys:sav\vpstart runsection ResolveAddress"

The method to update the DomainGUID entry varies depending on the
server type.  If this is a primary server, the domain GUID can be
extracted from the filenames of the certificate files on the server.
In this case, enter the command:
    load sys:sav\vpstart runsection SetDomainGUID

However, if this is a secondary server, the DomainGUID must match the
parent's DomainGUID.  In this case a different command must be used.
Edit the SetDomainGUID section of the install.ini file.  Replace the
SetDomainGuid=cert line with "SetDomainGuid=parent", save the file,
then run the command above.


How to regenerate certificates
--------------------------------------------------------------------
If you ever need to regenerate certificates, this can be performed
using a runsection diagnostic command.  Please note that regenerating
certificates can result in all clients and other servers refusing
to communicate with the server.  To regenerate certificates, shutdown
Symantec AntiVirus for NetWare, then enter the following command:
   load sys:sav\vpstart runsection CertGen


SAV server upgrade on NetWare defaults to bindery mode
------------------------------------------------------
If the previous version of SAV server was installed using NDS, upgrades
to the current version will default to bindery mode. This can be verified
by checking the install.log file located at the root of the sys directory
for the message "ActionsBinderyInstallScript". 


Secondary server install and migrations may fail if IPX
protocol is bound
----------------------------------------------------------
Under some circumstances NetWare servers configured with both IP and
IPX protocols may fail to communicate during server install and migrations.  
This is due to protocol and address resolution failures. A workaround is to 
disable the IPX protocol bindings from server(s).  After restarting the secondary 
server(s) and verifying network communication and name resolution 
(type "Ping <server name>" on secondary server(s) consoles), unload SAV 
server and type "vpstart runsection certgen" to re-run the generation of 
server certificates. Run network discovery on the SSC console to ensure that the 
servers appear. IPX protocol bindings can then be enabled on servers that
require this protocol.



--------------------------------------------------------
64-bit support
--------------------------------------------------------

Antivirus client is the only feature that is supported
------------------------------------------------------
The antivirus client provides antivirus and security risk protection for 
clients and servers, and is the only feature supported for this release. All 
other components, products, and tools are not supported.


LiveUpdate is the only virus definitions files update method supported
----------------------------------------------------------------------  
LiveUpdate is the only virus definitions files update method supported. All 
other update methods, including the Virus Definition Transport Method, are 
not supported. 


Managing 64-bit clients with the Symantec System Center
-------------------------------------------------------
To manage 64-bit clients with the Symantec System Center, configure 
the client settings so that the clients do not receive automatic
virus definitions updates. The easiest way to configure the 
client settings is to configure a client group and place your 64-bit clients
in this group. You can rely on users to click LiveUpdate, or you can 
set a LiveUpdate schedule for the group by using the Symantec System Center.

To manage 64-bit clients with the Symantec System Center:
1. Create a client group.
2. Right-click the client group.
3. Uncheck Inherit settings from Server Group.
4. Right-click the client group, and then select All Tasks > Symantec
   AntiVirus > Virus Definition Manager.
5. In the Virus Definition Manager dialog box, uncheck Update virus
   definitions from parent server.
6  Do one or both of the following:
   o  Uncheck Do not allow client to manually launch LiveUpdate.
   o  Check Schedule client for automatic updates using
      LiveUpdate, click Schedule, specify a schedule, and then click OK.
7. Click OK.
   
   After you install 64-bit client software, drag the client computers
   into this client group. 


Side effects repair limitations
-------------------------------
64-bit antivirus clients support side effect repair only for 32-bit viruses 
and security risks that have infected the WOW64 portions of the 64-bit 
operating systems on AMD(R)/64 and EM64T hardware. 64-bit antivirus clients
do not currently support the repair of 64-bit viruses and security risks 
on the 64-bit native operating system facilities outside of WOW64 on AMD/64 
and EM64T hardware. 



--------------------------------------------------------
Documentation issues
--------------------------------------------------------

Reference guide Event Log entries chapter contains wrong information
--------------------------------------------------------------------
The Event Log entries chapter in the reference guide associates incorrect
event numbers with events.  Typically, above event number 40, the numbers
in the reference guide are one number low. 



--------------------------------------------------------
Other issues
--------------------------------------------------------

XP SP2 firewall status incorrectly reported as enabled after restart
--------------------------------------------------------------------
In a specific circumstance, the Windows Firewall might temporarily report itself
as enabled when it actually is not.  This occurs when the following installation
sequence has occurred:
1.  XP SP1 is installed.
2.  Symantec AntiVirus is installed and is configured to disable Windows
    Firewall (default).
3.  XP SP2 is installed.

In this case, the Windows Security Center correctly displays Windows
Firewall as disabled after a one minute wait.


Using System Restore on computers that run Windows XP
-----------------------------------------------------
The Symantec service Rtvscan.exe disables the System Restore Windows XP 
functionality. 

To use System Restore on computers that run Windows XP:
1. Disconnect the computers from your network.
2. Display the Administrate tool called Services.
3. In the Services window, stop Symantec AntiVirus.
4. Use System Restore to restore the computers to a known state.
5. Restart the computers.



16-bit files used by Symantec AntiVirus components 
-------------------------------------------------- 
The following 16-bit files are included with Symantec AntiVirus as part of 
the virus definitions files that are distributed with each installation and 
definitions update. Although the current version of Symantec AntiVirus 
is not supported on 16-bit operating systems, these 16-bit virus definitions 
files are provided for backwards compatibility with legacy versions of 
Symantec AntiVirus that might still be in use. 

Files that are associated with NetWare protection are stored on the computer 
on which the Symantec System Center is installed. These files are pushed to 
NetWare servers that might support 16-bit NetWare clients. These 16-bit files
are only executed on the target computers that receive these files; the files 
are not executed on the managing computer. 

Installations and updates on supported Windows platforms might include: 
Directories: 
 C:\Program Files\Common Files\Symantec Shared\VirusDefs\BinHub\ 
 C:\Program Files\Symantec\Quarantine\Server\Signatures\00027599\ 
Files: 
 ECBOOTIL.VXD 
 NAVENG.VXD 
 NAVENG16.DLL 
 NAVEX15.VXD 
 NAVEX16A.DLL 

16-bit files that are pushed from the Symantec System Center to supported NetWare 
platforms might include: 

Directory on the managing computer: 
 C:\Program Files\Symantec\Symantec System Center\Deployment\Server Rollout\ 
    SERVER\NETWARE\LOGIN\ 
Files: 
 CTL3D.DLL 
 DY_LOH.DLL 
 I2_LDVP.DLL 
 LDRTSC16.386 
 MSCOMSTF.DLL 
 MSDETSTF.DLL 
 MSINSSTF.DLL 
 MSSHLSTF.DLL 
 MSUILSTF.DLL 
 NAVAPI16.DLL 
 SETUP.EXE 
 VPCCC16.EXE 
 VPDNUI.EXE 
 VPDN_FTP.EXE 
 VPDOWN.EXE 
 VPREMOVE.EXE 
 WPUSHPOP.EXE


Files installed in the Windows directories
-------------------------------------------------- 

The following files created by Symantec will be installed into the Windows System32 directory (typically, c:\windows\system32):
 NavLogon.dll	- WinLogin notifications for this product.
 S32EVNT1.DLL	- Support file for this product
 SymNeti.dll	- Support file for this product
 SymRedir.dll	- Support file for this product

The following files created by Symantec will be installed into the Windows System32\drivers directory:
 SYMEVENT.SYS	- Supporting driver for this product
 symdns.sys	- Supporting driver for this product
 symfw.sys	- Supporting driver for this product
 symids.sys	- Supporting driver for this product
 symndis.sys	- Supporting driver for this product
 symredrv.sys	- Supporting driver for this product
 symtdi.sys	- Supporting driver for this product

The following files created by Microsoft and distributed with this product will be install in the Windows System32 directory:
 capicom.dll	- COM CryptoAPI support
 atl71.dll	- ATL Class Library runtime
 mfc71.dll	- MFC Class Library runtime
 mfc71u.dll	- MFC Unicode Class Library runtime
 msvcp71.dll	- C++ runtime
 msvcr71.dll	- C runtime
 comctl32.ocx	- Common Control library
 msxml3.dll	- XML library
 msxml3r.dll	- XML library

The following files created by Intel and distributed with this product may be installed into the Windows directory:
 ENUAMS2.CNT	- AMS Help file
 ENUAMS2.HLP	- AMS Help file

The following files created by Intel and distributed with this product may be installed into the Windows System32 directory:
 AMS.DLL	- AMS support file
 AMSLIB.DLL	- AMS support file
 AMSUI.DLL	- AMS support file
 BCSTHCFG.DLL	- AMS support file
 CBA.DLL	- AMS support file
 CBAXFR.DLL	- AMS support file
 CLUTIL_S.DLL	- AMS support file
 CSL.DLL	- AMS support file
 CSSM32S.DLL	- AMS support file
 CSSMS_IN.DLL	- AMS support file
 ENUAMS.LRC	- AMS support file
 ENUAMSUI.LRC	- AMS support file
 INDSM_S.DLL	- AMS support file
 ITMLHCFG.DLL	- AMS support file
 IX509CLS.DLL	- AMS support file
 LOC32VC0.DLL	- AMS support file
 MSBXHCFG.DLL	- AMS support file
 MSGSYS.EXE	- AMS support file
 Msgsys.dll	- AMS support file
 NLMXHCFG.DLL	- AMS support file
 NTELHCFG.DLL	- AMS support file
 NTS.DLL	- AMS support file
 NTSU2T.DLL	- AMS support file
 PAGEHCFG.DLL	- AMS support file
 PDS.DLL	- AMS support file
 PRGXHCFG.DLL	- AMS support file
 SNMPHCFG.DLL	- AMS support file

The following files created by Intel and distributed with this product may be installed into the System32\AMS_II directory:
 BCSTHNDL.DLL	- AMS support file
 HNDLRSVC.EXE	- AMS support file
 IAO.EXE	- AMS support file
 ITMLHNDL.DLL	- AMS support file
 MSBXHNDL.DLL	- AMS support file
 NTELHNDL.DLL	- AMS support file
 ORIGREG.DLL	- AMS support file
 PAGEHNDL.DLL	- AMS support file
 PRGXHNDL.DLL	- AMS support file
 SNMPHNDL.DLL	- AMS support file

The following files created by Intel and distributed with this product may be installed into the System32\CBA directory:
 ENUPDSRC.LRC	- AMS support file
 ENUXFRRC.LRC	- AMS support file
 PDS.EXE	- AMS support file
 XFR.EXE	- AMS support file


About security risks
--------------------
When possible, installers for security risks are detected 
and removed prior to them loading adware and spyware programs 
on a system. This is an effective approach when the installer 
application is solely used for delivery of security risks to a 
host machine. Nonetheless, more general purpose installers may 
very well be used in conjunction with spyware or adware and 
cannot be blocked given the broad range of application types 
they serve. When it is not possible to block the security risk 
installer, the spyware or adware application is allowed to 
load into the system for several reasons. 

Firstly, interruption of a partially installed spyware program 
may put the host in an unstable state, leaving the user with 
error messages or residual files and folders left on the machine. 
If the installation is completed prior to removal, all aspects 
of the security risk can be analyzed and properly removed such 
that the host is left in a safe, predictable state. 

Secondly, spyware and adware programs behave in a fashion that 
can be very similar to a normal application as opposed to the 
more obvious, unusual behavior that malware typically exhibits. 
Thus, in order to ensure accurate detection, the program is allowed 
to load before it is fully identified and subsequently removed or 
quarantined. Lastly, security risks are potentially desirable 
programs and how acceptable they are is determined by the preferences
 of the user or administrator. Given this, ensuring such applications 
are not automatically blocked is essential.

Despite the fact that a security risk program may be momentarily 
loaded on a system, little can be accomplished by such a program 
given the additional security measures in place on a properly 
protected host. For example, customers using SCS/NIS benefit 
from the protection of robust firewalls that will block any 
attempts for the application to phone home or otherwise transmit 
data without the user's consent. Thus, in the brief time between 
the security risk being loaded and before it's removal, the risk 
of any unauthorized action being taken is extremely small.

We believe this approach to similar to that of our major 
competitors and have not seen evidence yet of an approach 
that is effective at achieving accurate detection and removal 
without full analysis of a security risk program such as adware 
or spyware.


About cookies
-------------
Cookies are a widely used technology for maintaining information 
during and across Web site sessions. There are a few major forms 
of cookies, each with a separate intent and usage. The first major 
difference among cookies is whether or not they remain on your 
system after you visit a Web site. If they remain (as part of a 
small text file), they are called “persistent” cookies which can be 
used by the Web site to help tailor your experience the next time 
you visit based on your previous actions. If they do not, they 
are called “session” or temporary cookies and are deleted after 
you close your browser.
 
The second major distinction among cookies is that of first party 
versus third party cookies. First party cookies are presented by 
the Web site you are visiting and are only used by that Web site. 
Third party or tracking cookies are presented and used by one 
or many Web site to track basic online behavior within and across 
Web sites, typically for online marketing purposes. While there 
is a general acceptance of most types of cookies, third party 
cookies have been considered objectionable by some since they 
have the potential to disclose information such as Web browsing 
behavior or more personal data to marketers.
 
While there have been privacy concerns since the introduction 
of cookies in 1996, early concerns were eventually alleviated 
as understanding of the technology grew and Web browsers gave 
users the ability to clearly state their preference for handling 
cookies. Specifically, popular Web browsers today such as Internet 
Explorer allow users to set their privacy preferences, including 
detailed handling of third party cookies, and the default settings 
prevent practices such as personally identifiable information 
being tracked without a user’s explicit consent. 
 
Alongside the ready availability of features to control use of the 
third party cookies, forthcoming legislation has made that cookies 
will not be included within the scope of regulated software or features. 
The leading Federal bill, the SPY ACT or H.R. 29, was recently amended 
to exclude third party cookies from the scope of regulation so as to 
not unfairly restrict online commerce.
 
Given the ease at which customers can control cookies within today’s 
Web browsers as well as the widespread nature of more serious security 
risks such as spyware, Symantec does not detect the presence of cookies 
on a system today in order to better focus customers on the most 
pressing security issues.
 

Security risk best practices 
----------------------------
o  If a customer does not take an aggressive stance against Spyware 
   or Adware removal Symantec recommends that Real-Time scanning be 
   turned off for that category.
o  Removing spyware/adware may require a process to be terminated 
   (for instance a Web browser), in some cases the system must be 
   rebooted to completely clean it.
o  Administrator can allow end-user to choose when to terminate/reboot.
o  If end-user delays this action, real-time protection will continue 
   to find the security risk.
o  This acts as an aggressive reminder.


Notice - Central Quarantine performance considerations
--------------------------------------------------------
The configurable limit on the number of samples allowed in Central Quarantine 
has been increased in release 9.0.4 and 2.0.4 from 2500 to 5000 samples; 
however, there are performance aspects to consider. First, the workload on the 
Central Quarantine server increases dramatically as the number of samples is 
increased. Under a heavy load, it can take the Quarantine Console viewer 
(even on another machine) a long time to load the list. Second, the Central 
Quarantine server can take a significant amount of processor overhead and all 
this activity is very disk-intensive on the server. Thus, increasing the limit 
above 2500 may incur relative performance degradation on the Central Quarantine 
server.


********************************************************************************
END OF FILE
********************************************************************************
