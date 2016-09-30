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
Norton AntiVirus Quarantine Server
.

MessageId=0x0002
Severity=Informational
SymbolicName=IDM_INFO_STARTING
Language=English
Norton AntiVirus Quarantine Server has started.
.

MessageId=0x0003
Severity=Informational
SymbolicName=IDM_INFO_NO_CONFIG
Language=English
No configuration information found, using defaults.
.

;/////////////////////////////////////////////////////////////////////
;// Warning messages

MessageId=0x2000
Severity=Warning
SymbolicName=IDM_WARNING_SERVER_FULL
Language=English
The Quarantine server is full.
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
Error initializing Quarantine Server.
.

