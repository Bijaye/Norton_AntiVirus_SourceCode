;/////////////////////////////////////////////////////////////////////
;// QSERVER.MC - Created - 11/11/98
;//
;// Copyright 1992-1999 Symantec, Peter Norton Product Group
;/////////////////////////////////////////////////////////////////////
;// $Header: $
;//
;// Description:
;//
;//
;/////////////////////////////////////////////////////////////////////
;// $Log:$
;/////////////////////////////////////////////////////////////////////


SeverityNames=(Success=0x0
               Informational=0x1
               Warning=0x2
               Error=0x3
               )

MessageIdTypedef=DWORD

;///////////////////////////////////////////////////////////////////// 	  
;// Informational messages	  

MessageId=0x0001
Severity=Informational
SymbolicName=IDM_TITLE
Language=English
Central Quarantine
.

MessageId=0x0002
Severity=Informational
SymbolicName=IDM_INFO_STARTING
Language=English
Symantec Central Quarantine has started.
.

MessageId=0x0003
Severity=Informational
SymbolicName=IDM_INFO_NO_CONFIG
Language=English
No configuration information found, using defaults.
.

MessageId=0x0004
Severity=Informational
SymbolicName=IDM_GENERAL_INFO
Language=English
Central Quarantine info.
%1	   
.

MessageId=0x0005
Severity=Informational
SymbolicName=IDM_GENERAL_INFO_PURGE_SAMPLE
Language=English
Symantec Central Quarantine info:  The completed sample has been Purged to make room for incoming samples. %1	   
.

;/////////////////////////////////////////////////////////////////////	 
;// Warning messages

MessageId=0x2000
Severity=Warning
SymbolicName=IDM_WARNING_SERVER_FULL
Language=English
Central Quarantine is full.
.


;/////////////////////////////////////////////////////////////////////
;// Error messages

MessageId=0x8500
Severity=Error
SymbolicName=IDM_ERR_CREATING_IP_LISTENING_SOCKET
Language=English
Error creating IP listenening socket.
.

MessageId=0x8501
Severity=Error
SymbolicName=IDM_ERR_ACCEPTING_CONNECTION
Language=English
Error accepting client socket.
.

MessageId=0x8502
Severity=Error
SymbolicName=IDM_ERR_CREATING_SPX_LISTENING_SOCKET
Language=English
Error creating SPX listenening socket.
.

MessageId=0x8503
Severity=Error
SymbolicName=IDM_ERR_INITIALIZING_WINSOCK
Language=English
Error initializing WinSock.
.

MessageId=0x8504
Severity=Error
SymbolicName=IDM_ERR_INITIALIZING_QSERVER
Language=English
Error initializing Central Quarantine.
.

MessageId=0x8505
Severity=Error
SymbolicName=IDM_SAMPLE_ALERTABLE_EVENT	 
Language=English  
An error was detected in a Central Quarantine sample. 
%1
.

MessageId=0x8506
Severity=Error
SymbolicName=IDM_GENERAL_ALERTABLE_EVENT
Language=English
A general error was detected with Central Quarantine. 
%1
.

MessageId=0x8507
Severity=Informational
SymbolicName=IDS_AMS_P_VIRUS_NAME
Language=English
Virus.
.

MessageId=0x8508
Severity=Error
SymbolicName=IDM_ERR_INITIALIZING_AMS_EVENTS
Language=English
Error initializing Quarantine AMS Events.
.
