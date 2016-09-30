// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#ifndef REG_ERR_DEFS
#define REG_ERR_DEFS

/* 	Returned errors	------------------------------------------------------ */


#define	ERR_REG_INC_CONTEXT				PKG_REG + 1
	apitext(ERR_REG_INC_CONTEXT, "Insufficient information supplied for requested registration function.")

#define	ERR_REG_ADDRBOOK_ENTRY_EXISTS	PKG_REG + 2
	apitext(ERR_REG_ADDRBOOK_ENTRY_EXISTS, "No permission (flag) to modify existing Address Book entry")

#define ERR_REG_USERID_EXISTS 			PKG_REG + 3
	apitext(ERR_REG_USERID_EXISTS, "No permission (flag) to modify existing UserID File")

#define ERR_REG_BEGIN_CERTIFYING		PKG_REG + 4
	stringtext(ERR_REG_BEGIN_CERTIFYING, "Begin certifying %A...") 

#define ERR_REG_NO_MAILFILE_CREATED 	PKG_REG + 5
	errortext(ERR_REG_NO_MAILFILE_CREATED, "Mail file for %A could not be created")

#define ERR_REG_BEGIN_REGISTERING		PKG_REG + 6
	stringtext(ERR_REG_BEGIN_REGISTERING, "Begin registering %A...")

#define ERR_REG_CREATINGNEWMAIL			PKG_REG + 7
	stringtext(ERR_REG_CREATINGNEWMAIL, "Creating mail file for %A...")

#define ERR_REG_ADDIDTOBOOK				PKG_REG + 8
	stringtext(ERR_REG_ADDIDTOBOOK, "Adding %A to the Name and Address Book...")

#define ERR_REG_DONE_REGISTERING		PKG_REG + 9
	stringtext(ERR_REG_DONE_REGISTERING, "%A successfully registered.")

#define	ERR_REG_LOG_ADDRBOOK_EXISTS		PKG_REG + 10
	stringtext(ERR_REG_LOG_ADDRBOOK_EXISTS, "Address Book entry for %A already exists.")

#define ERR_REG_LOG_USERID_EXISTS 		PKG_REG + 11
	stringtext(ERR_REG_LOG_USERID_EXISTS, "UserID file %p already exists.")

#define ERR_REG_LOG_NO_PASSWORD 		PKG_REG + 12
	stringtext(ERR_REG_LOG_NO_PASSWORD, "No password supplied for %s.")

#define ERR_REG_BEGIN_IDFILE_CREATE		PKG_REG + 13
	stringtext(ERR_REG_BEGIN_IDFILE_CREATE, "Creating ID for %A.  This requires 1-3 minutes of computation...")

#define ERR_REG_TOO_MANY_ENTRIES		PKG_REG + 14
	errortext(ERR_REG_TOO_MANY_ENTRIES, "The name cannot be registered.  More than one entry was found in the Address Book with this name")

#define ERR_REG_DONE_CERTIFYING			PKG_REG + 15
	stringtext(ERR_REG_DONE_CERTIFYING, "Done certifying %A...") 

#define ERR_REG_NO_IDFILE_NAME			PKG_REG + 16
	stringtext(ERR_REG_NO_IDFILE_NAME,	"No ID file name specified") 

#define ERR_REG_NO_MAIL_INFO			PKG_REG + 17
	stringtext(ERR_REG_NO_MAIL_INFO,	"No mail server or mail database name specified") 

#define ERR_REG_NO_LASTNAME				PKG_REG + 18
	stringtext(ERR_REG_NO_LASTNAME,		"No last name specified") 

#define ERR_REG_NO_SERVER_NAME			PKG_REG + 19
	stringtext(ERR_REG_NO_SERVER_NAME,	"No server name specified") 

#define ERR_REG_NO_CERTIFIER			PKG_REG + 20
	stringtext(ERR_REG_NO_CERTIFIER,	"No certifier specified") 

#define ERR_REG_NO_PASSWORD				PKG_REG + 21
	stringtext(ERR_REG_NO_PASSWORD,		"No password specified") 

#define ERR_REG_OPEN_CERTLOG		 	PKG_REG + 22
	stringtext(ERR_REG_OPEN_CERTLOG,	"Could not open the certifier log") 

#define ERR_REG_ACCESS_CERTLOG			PKG_REG + 23
	stringtext(ERR_REG_ACCESS_CERTLOG,	"You are not allowed to update the certifier log") 

#define ERR_REG_UPDATE_CERTLOG		 	PKG_REG + 24
	stringtext(ERR_REG_UPDATE_CERTLOG,	"Error attempting to update the certifier log") 

#define ERR_REG_OURSERVERS				PKG_REG + 25
	stringtext(ERR_REG_OURSERVERS,		"LocalDomainServers")

#define STR_REG_UPDATING_CERTLOG		PKG_REG + 26
	stringtext(STR_REG_UPDATING_CERTLOG,"Updating Certification Log")

#define ERR_REG_USER_NOT_FOUND 			PKG_REG + 27
	errortext(ERR_REG_USER_NOT_FOUND,"User or server name not found in Name and Address Book")

#define ERR_REG_IDFILE_NOT_FOUND		PKG_REG + 28
	errortext(ERR_REG_IDFILE_NOT_FOUND,"ID file not found in Name and Address Book")

#define	ERR_REG_NOSUCHUSER				PKG_REG + 29
	errortext(ERR_REG_NOSUCHUSER,		"User name not found in Name and Address Book")

#define	ERR_REG_NOTUNIQUE				PKG_REG + 30
	errortext(ERR_REG_NOTUNIQUE,		"Multiple users with same name found in Name and Address Book")

#define	ERR_REG_NOSUCHSERVER	   		PKG_REG + 31
	errortext(ERR_REG_NOSUCHSERVER,		"Server name not found in Name and Address Book")

#define	ERR_REG_SERVERNOTUNIQUE			PKG_REG + 32
	errortext(ERR_REG_SERVERNOTUNIQUE,	"Multiple servers with same name found in Name and Address Book")

#define STR_REG_NETWARE_NAME			PKG_REG + 33
	internaltext(ERR_REG_NETWARE_NAME, "NetWare")

#define ERR_REG_NO_STORED_CERTKEY		PKG_REG + 34
	errortext(ERR_REG_NO_STORED_CERTKEY, "The Name and Address Book entry does not include a certified public key")

#define ERR_REG_NO_SRV_VIEW				PKG_REG + 35
	errortext(ERR_REG_NO_SRV_VIEW, "The Address Book you are registering the server to does not contain a $Servers view.  In order to register a server you must use a Public Address Book.  You can replace the design of the Address Book you are using with the Public Address Book template.")

#define ERR_REG_LOG_PASSWORD_ADDRESSBOOK	PKG_REG + 36
	stringtext(ERR_REG_LOG_PASSWORD_ADDRESSBOOK, "No password supplied for %s (required for ID files to be stored in the Name and Address Book).")

#define ERR_REG_LOG_MINPSWDCHARS		PKG_REG + 37
	stringtext(ERR_REG_LOG_MINPSWDCHARS, "You must supply the minimum number of characters required for a password for %s.")

#define ERR_REG_CREATING_ADMINREQTOCREATEMAIL	PKG_REG + 38
	stringtext(ERR_REG_CREATING_ADMINREQTOCREATEMAIL, "Creating administration request to create mail file for %A...")

#define ERR_REG_NO_ADMINREQTOCREATEMAIL 	PKG_REG + 39
	errortext(ERR_REG_NO_ADMINREQTOCREATEMAIL, "The administration request could not be created: %s")

#define ERR_REG_NO_ADMINPROCESS 	PKG_REG + 40
	errortext(ERR_REG_NO_ADMINPROCESS, "The administration process has not been initialized.")

#define ERR_REG_CREATEMAILSETUP			PKG_REG + 41
	stringtext(ERR_REG_CREATEMAILSETUP, "Mail file for %A will be created during user setup.")

#define ERR_REG_NT_PASSWORD_SIZE		PKG_REG + 42
	stringtext(ERR_REG_NT_PASSWORD_SIZE, "NT user '%s' could not be added.  The Notes password violated the minimum or maximum NT password length.")

#define ERR_REG_NT_ACCESS_DENIED			PKG_REG + 43
	stringtext(ERR_REG_NT_ACCESS_DENIED, "You must be an NT Administrator or Account Operator to add NT users or groups.")

#define ERR_REG_NT_DC_NOT_FOUND			PKG_REG + 44
	stringtext(ERR_REG_NT_DC_NOT_FOUND, "NT account(s) cannot be added.  A primary domain controller for the current NT domain cannot be found.")

#define ERR_REG_NT_DLL_NOT_FOUND		PKG_REG + 45
	stringtext(ERR_REG_NT_DLL_NOT_FOUND, "%s could not be found.  If available, place it in the Windows system directory (SYSTEM32).")

#define ERR_REG_NT_USER_GROUP_ABSENT		PKG_REG + 46
	stringtext(ERR_REG_NT_USER_GROUP_ABSENT, "NT account for '%s' not added.  The NT user or group could not be found on the current NT domain.")

#define ERR_REG_NT_REG_FAILED		PKG_REG + 47
	stringtext(ERR_REG_NT_REG_FAILED, "NT account for '%s' could not be added.")

#define ERR_REG_NT_USER_EXISTS		PKG_REG + 48
	stringtext(ERR_REG_NT_USER_EXISTS, "NT account for '%s' could not be added; an NT user with the same Username already exists.")

#define ERR_REG_ADDCERTIFICATETOBOOK	PKG_REG + 49
	stringtext(ERR_REG_ADDCERTIFICATETOBOOK, "Adding the certificate to the Name and Address Book...")

/*	NOTE:  PKG_REG+51 through PKG_REG+151 are used by the setup program
			to provide network choices.

	The format of the stringtext is five fields separated by | (vertical bar):
		(1) List of platform names on which support is provided separated by
			; (semi-colons) -- must match platform name in global.h EXACTLY
		(2) Flags defined below (WORKSTATION_SUPPORT, etc.)
		(3) PortName in Ports= or DisabledPorts= line in notes.ini
		(4) Notes.ini PortName variable, specifying DLL name, unit number,
			number of simultaneous sessions, and buffer size in bytes
		(5) Dialog box description in Setup (TRANSLATE THIS ONE ONLY!).

	The order of the list of choices during setup for a given platform is
	determined by the order they are in the strings.  
		
	Network support may also be added by having a similar string in a
	text file which is shipped with the given network driver (like SNA).
	These networks will be at the end of the list.

	DO NOT USE PKG_REG+51 through PKG_REG+151!

*/

/************************** SETUP Network resource strings *****************/

#define NUMNETWORKSADDED	50
#define MAXDLGSTRING		50
#define MAXDLLNAME			9

#define WORKSTATION_SUPPORT 	0x00000001
#define SERVER_SUPPORT			0x00000002
#define PHONENUMBER_SUPPORT		0x00000004	/* Used for SNA support */
#define MODEM_SUPPORT			0x00000008

#define	PKG_SETUP				PKG_REG+51

/*	The following template is used so we can dynamically add the port name, unit
	number and driver name after querying the system.
*/
#define SETUP_XPC_TEMPLATE	PKG_SETUP
	stringtext(SETUP_XPC_TEMPLATE, "Macintosh|13|%s|XPC,%s,15,0,%s!%s|%s")

/* 	Note the strings, AppleTalk and VINES, in the port name are checked in newsetup.c
	to exclude them from Win95 server setup selection.  If you change the name here,
	change it there in subroutine, PutEntryInTable.  */


#define SETUP_NETWORK_FIRST		PKG_SETUP+10
#define SETUP_NETWORK_LAST		PKG_SETUP+100

	stringtext(SETUP_NETWORK_FIRST+0, "Windows/16|1|TCP|TCP, 0, 15, 0|TCP/IP")
	stringtext(SETUP_NETWORK_FIRST+1, "OS/2v2|3|TCPIP|TCP, 0, 15, 0|TCP/IP")
	stringtext(SETUP_NETWORK_FIRST+2, "Windows/32;UNIX;Netware|3|TCPIP|TCP, 0, 15, 0|TCP/IP")	
	stringtext(SETUP_NETWORK_FIRST+3, "Windows/16|1|VINES|VINES, 0, 15, 0|Banyan VINES")
	stringtext(SETUP_NETWORK_FIRST+4, "OS/2v2;Windows/32|3|VINES|VINES, 0, 15, 0|Banyan VINES")
	stringtext(SETUP_NETWORK_FIRST+5, "Macintosh|1|MacTCP|TCP, 0, 15, 0|MacTCP")
	stringtext(SETUP_NETWORK_FIRST+6, "Windows/32;Windows/16;OS/2v2|3|LAN0|NETBIOS, 0, 15, 0|NetBIOS (NetWare, LAN Manager, etc.)")	
	stringtext(SETUP_NETWORK_FIRST+7, "Windows/32;OS/2v2;Netware|3|SPX|NWSPX, 0, 15, 0|NetWare SPX")
	stringtext(SETUP_NETWORK_FIRST+8, "Windows/16|1|SPX|NWSPX, 0, 15, 0|NetWare SPX")
	stringtext(SETUP_NETWORK_FIRST+9, "OS/2v2|3|IBM_LAN0|IBMEENB, 0, 50, 0|IBM EE LAN Requestor")
	stringtext(SETUP_NETWORK_FIRST+10, "OS/2v2|2|AppleTalk|ATALK, 0, 15, 0|AppleTalk")								
	stringtext(SETUP_NETWORK_FIRST+11, "Windows/32;Netware|2|AppleTalk|ATALK, 0, 15, 0|AppleTalk")								
	stringtext(SETUP_NETWORK_FIRST+12, "Macintosh|1|AppleTalk|ATALK, 0, 15, 0|Appletalk")	
	stringtext(SETUP_NETWORK_FIRST+13, "Windows/32;Windows/16;OS/2v2;Netware|15|COM1|XPC, 1, 15, 0|COM1")
	stringtext(SETUP_NETWORK_FIRST+14, "Windows/32;Windows/16;OS/2v2;Netware|15|COM2|XPC, 2, 15, 0|COM2")
	stringtext(SETUP_NETWORK_FIRST+15, "Windows/32;Windows/16;OS/2v2;Netware|15|COM3|XPC, 3, 15, 0|COM3")
	stringtext(SETUP_NETWORK_FIRST+16, "Windows/32;Windows/16;OS/2v2;Netware|15|COM4|XPC, 4, 15, 0|COM4")
	stringtext(SETUP_NETWORK_FIRST+17, "Windows/32;Windows/16;OS/2v2;Netware|15|COM5|XPC, 5, 15, 0|COM5")
	stringtext(SETUP_NETWORK_FIRST+18, "UNIX|15|Serial1|XPC, 1, 15, 0|Serial1")
	stringtext(SETUP_NETWORK_FIRST+19, "UNIX|15|Serial2|XPC, 2, 15, 0|Serial2")
	stringtext(SETUP_NETWORK_FIRST+20, "UNIX|3|SPX|SPX, 0, 15, 0|SPX")

/*	NOTE:  PKG_REG+51 through PKG_REG+151 are used by the setup program
			to provide network choices.

	DO NOT USE PKG_REG+51 through PKG_REG+151!
*/

#endif
