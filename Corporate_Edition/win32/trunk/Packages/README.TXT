--------------------------------------------------------
Symantec Client Security Beta 1		May 15, 2002
--------------------------------------------------------



Symantec Packager Installation
------------------------------

For Beta 1, it is advisable to install the Packager to
its default installation folder.  

The following are known issues surrounding installation:


1. The Symantec Packager only supports interactive 
install mode ( /i ) for Beta 1.  The use of other MSI
switches, including silent install will be supported in
the shipping product.

2. The "Repair" functionality of the installer is not 
supported for the Beta 1 release of the Symantec Packager.

3. Product Configuration and Package Definition data may
not be removed from the user's profile directory upon
uninstall of the Symantec Packager.  This data can safely
removed post uninstall.


Symantec Packager Functionality
-------------------------------

The following are known issues surrounding functionality:

1. Although the feature selection area for the individual
supported products is available, use of this area may
create installable packages that may not be stable.  
Ensure you test each generated package before deployment.

2. Use of custom defined installation panels for package
installations may give inconsistent results.

3. When configuring a product, changing the installation
destination folder may give inconsistent results.

4. Specifying a custom shortcut name and folder name for
the individual product installations may also generate
a default shortcut as well.

5. The "Rollback installation of all products on failure"
option may not effectively roll back all installs if a 
failure should occur.

6. The "Reboot at end of package install" options may not
be applied to the package as specified.


Package Deployment
------------------

The following are known issues surrounding deployment:

1. Using the Symantec Packager deployment tool, packages
deployed to target machines may not have their temporary
files removed after the remote install has completed.

2. Performing "cut and paste" operations from the 
Deployment Depot may cause the Packager Manager to 
freeze.  It is recommended that "copy" operations be 
performed instead of "cut".

--------------------------------------------------------
Installation
--------------------------------------------------------

Product Migration
-----------------
For Beta 1, installation over an existing version of
any Symantec client Security product is not supported.
Before installing, ensure all previous versions have
been uninstalled.

Repair Functionality
--------------------
For Beta 1, the "Repair" functionality of each product 
installer is not supported.  "Repair" will be supported
in the final product.

Advertised Installation
-----------------------
For Beta 1, the "Advertise" functionality of each
product install is not supported.  

Terminal Server Installation
----------------------------
The following products are not supported when installed
directly to a Terminal Server:

Symantec Client Firewall Administrator
Symantec System Center
Symantec Client Firewall

Central Quarantine
------------------
When installing Central Quarantine, you may need to 
reboot before the services will load.

Quarantine Console
------------------
When installing Quarantine Console to a Windows NT4.0
platform, ensure that Microsoft Management Console 1.2
(MMC) is preinstalled. Attempting to install 
Quarantine Console with MMC 1.1 may give inconsistent
results. 

Symantec System Center
----------------------
Attempting to install Symantec System Center to an 
operating system with a version of Internet Explorer 
previous to 5.5 may give inconsistent results.  For 
the Beta 1 release, it is recommended that you upgrade
to Internet Explorer 5.5 or higher before installation.

Symantec AntiVirus Client 
-------------------------
Launching the Symantec AntiVirus Client user interface
from the SYSTRAY icon before a post install reboot has
occurred, may cause problems if a manual scan is
initiated.

Attempting to launch another installation immediately
following a Symantec AntiVirus Client install may 
give inconsistent results.  It is recommended that a 
reboot occur before an additional installation is 
attempted.

Installing Symantec AntiVirus Client to a Windows 98/ME
environment may fail to prompt the user for a reboot.
Please ensure a reboot has occurred before launching the 
program.

Symantec Client Firewall
------------------------
Attempting to perform a silent install of Symantec
Client Firewall on Windows NT4.0 or Windows 98/ME
may give inconsistent results.

Other General Installation Issues
---------------------------------
For Beta 1, it is recommended that all Symantec Client
Security products be installed to their default location.
Modifying the installation target folder may give 
inconsistent results.


--------------------------------------------------------
UnInstall
--------------------------------------------------------

When attempting an uninstall any of the Symantec Client
Security products, it is recommended that the uninstall 
be launched from ADD\REMOVE Programs.  Attempting to 
uninstall by restarting the installation executable may
give inconsistent results.

Symantec AntiVirus Client
-------------------------
Canceling a Symantec AntiVirus Client uninstall may
corrupt an existing AMS Installation.  

An uninstall attempt of a managed client may not accept
the user defined password.  Using the default "symantec"
(case sensitive) password will allow the uninstall to 
progress.

Before uninstalling Symantec AntiVirus Client, ensure 
that the product's user interface is closed. Attempting
to uninstall with the main user interface running may
give inconsistent results.


--------------------------------------------------------
Deployment
--------------------------------------------------------

Intellimirror
-------------
Deploying Symantec AntiVirus Client using Intellimirror
should be done with a *.ZAP file.  A sample *.ZAP file
can be found on the CD in the \TOOLS\SAMPLE folder.

After installing Symantec AntiVirus Client from a 
published distribution point, you may be able to restart
the install, writing over and corrupting the existing
installation.    

NTREMOTE
--------
When pushing a Symantec AntiVirus Client install to 
either Windows NT4.0 or Windows 2000 clients, ensure the
client machine is in a "logged-on" state.


--------------------------------------------------------
Firewall Alerts
--------------------------------------------------------

Symantec Client Firewall
------------------------

When performing certain management actions within
Symantec System Center, it may be necessary to configure
Symantec Client Firewall to allow the network
communication before it is successfully completed. 
Symantec Client Firewall should be configured to 
display alerts for the user to customize them.

When deploying package via NTREMOTE, SERVER Rollout, or
the Symantec Packager Deployment Tool, it may be
necessary to disable the firewall until the actions are
completed.


--------------------------------------------------------
Definition Updates
--------------------------------------------------------

Symantec Antivirus Clients and Server
-------------------------------------

Before attempting to roll-back virus definitions make
sure that all the SAV clients and SAV Servers have
been rebooted once after the initial install. If this is 
not done, some clients may not rollback to the earlier
definitions.



--------------------------------------------------------
On-Demand Scans
--------------------------------------------------------

Symantec Antivirus Clients and Server
-------------------------------------

Thread throttling for on demand scans is not functional.
Changing the control will get written to the registry
and saved but the thread priority will only fluctuate
between 6 and 9. 


Scheduled Scans may get kicked off within minutes after 
being set rather than the schedule time.



--------------------------------------------------------
Management
--------------------------------------------------------


Symantec System Center
----------------------

Clients will not appear in client groups unless the
option, "show client machines when viewing groups" is
selected in the client filter dialog within the SSC
Console Options under the Tools menu.