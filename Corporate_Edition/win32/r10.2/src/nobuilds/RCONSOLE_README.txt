********************************************************************************
Symantec Client Security Reporting                                    README.TXT
Copyright 1999 - 2004 Symantec Corporation. All rights reserved.      March 2005
********************************************************************************

Installation and uninstallation issues
--------------------------------------
*  Use a static IP address for a primary server if you install Report Manager
*  Primary servers must be installed on server operating systems if you
   install Report Manager


Updates
-------
*  


Deployment with third-party products
------------------------------------
*


Management-related issues
-------------------------
*  Demoting a Primary Server that has an Instance of Report Manager Installed



Documentation issues
--------------------
*


Other issues
------------
*  
Open source and freeware components used in SESA
-------------------------------------------------
This product includes software developed by the Apache Software 
Foundation (http://www.apache.org/), namely Tomcat Servlet Container,
Xerces XML Parser, Piccolo XML Parser, Log4J, and Apache XML-RPC.
A copy of the license may be found at www.apache.org/LICENSE 
Copyright (c) The Apache Software Foundation. 


Netscape LDAP SDK for Java is under license from the Mozilla 
Organization. The source code for Netscape LDAP SDK for Java
may be found in Third Party\Source\LDAP SDK on CD3 of the 
distribution media. A copy of the license may be found at 
www.mozilla.org/MPL/NPL-1.1.html 
Copyright (c) Netscape Communication Corporation.


JoeSNMP library is under license from GNU Lesser General Public 
License (LGPL). The source code for JoeSNMP library may be 
found in Third Party\Source\SNMP on CD3 of the distribution media.
A copy of the license may be found at www.fsf.org/copyleft/lesser.html
Copyright (c) OpenNMS Group.


SNIA CIMOM is under license from the Storage Networking Industry 
Association, and is now hosted by the Open Group. The source 
code for SNIA CIMOM may be found in Third Party\Source\SNIA CIMOM 
on CD3 of the distribution media. A copy of the license may be 
found at http://www.snia.org/smi/developers/open_source/#accept
Copyright (c) Storage Networking Industry Association.


jTDS from the jTDS Project is made available under the terms of the 
GNU Lesser General Public License (LGPL). The source code for jTDS
may be found in Third Party\Source\jTDS on CD3 of the distribution 
media. A copy of the license may be found at 
http://jtds.sourceforge.net/license.html
Copyright (c) jTDS Project

OpenLDAP code (OpenLDAP project) is under license from the OpenLDAP
Public License. The full text of the license may be found
at http://www.openldap.org/software/release/license.html.
Copyright (c) OpenLDAP Foundation

Sun’s Java Runtime Environment (JRE) and Java Secure Sockets Extension 
(JSSE) are licensed under Sun’s binary license, the text of which can be 
found at http://java.sun.com/products/jdk/1.1/LICENSE
Copyright (c) Sun Microsystems.


Copies of all licenses can be found in the \Third Party directory on ________?
(Installation Disk 3 of ?) of the distribution media.






--------------------------------------------------------
Installation and uninstallation issues
--------------------------------------------------------

Use a static IP address for a primary server if you install Report Manager
--------------------------------------------------------------------------
In you install Report Manager on a primary server, use a static IP address on 
the primary server. If the primary server IP address changes after installing
Report Manager, Report Manager will not function properly.

Primary servers must be installed on server operating systems if you
install Report Manager
--------------------------------------------------------------------
In you install Report Manager on a primary server, the primary server must be 
installed on either Windows 2000 Server or Windows 2003 Server operating 
systems.



--------------------------------------------------------
Updates
--------------------------------------------------------



--------------------------------------------------------
Management-related issues
--------------------------------------------------------

Demoting a Primary Server that has an Instance of Report Manager Installed
--------------------------------------------------------------------------
If you promote a secondary server to a primary server in a server group, and
if the demoted primary server had an instance of Report Manager installed, you
must uninstall Report Manager and all of its components from the demoted 
primary server, and install Report Manager and all of its components on the 
newly promoted primary server. If you do not perform this operation, you will
not receive all events from all clients and servers in your server group.



--------------------------------------------------------
Documentation issues
--------------------------------------------------------



--------------------------------------------------------
Other issues
--------------------------------------------------------



********************************************************************************
END OF FILE
********************************************************************************
