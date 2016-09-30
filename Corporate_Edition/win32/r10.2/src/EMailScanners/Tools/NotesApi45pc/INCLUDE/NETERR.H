// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.

#ifndef NETWORK_ERR_DEFS
#define NETWORK_ERR_DEFS

#define ERR_NO_MORE_SESSIONS	PKG_NETWORK+1
	errortext(ERR_NO_MORE_SESSIONS,	"Insufficient network or adapter resources.  Consult Network documentation to increase the maximum number of sessions.")
#define ERR_SESSION_CLOSED		PKG_NETWORK+2
	errortext(ERR_SESSION_CLOSED,	"Remote system no longer responding")
#define ERR_NAME_IN_USE			PKG_NETWORK+3
	errortext(ERR_NAME_IN_USE,
		"Another system is using the same server name as this system")
#define ERR_NET_FLAKY			PKG_NETWORK+4
	errortext(ERR_NET_FLAKY,
		"Network error due to transient network condition or hardware failure")
#define ERR_NET_HARDWARE		PKG_NETWORK+5
	errortext(ERR_NET_HARDWARE,
		"Network adapter not installed or not functioning")
#define ERR_NET_SOFTWARE		PKG_NETWORK+6
	errortext(ERR_NET_SOFTWARE,
		"Internal network software problem (or hardware malfunction)")
#define ERR_TIMEOUT				PKG_NETWORK+7
	errortext(ERR_TIMEOUT,		"Network operation did not complete in a reasonable amount of time; please retry")
#define ERR_NET_CMD_CANCELLED	PKG_NETWORK+8
	errortext(ERR_NET_CMD_CANCELLED,	"Network operation was cancelled")
#define ERR_RETRY				PKG_NETWORK+9
	internaltext(ERR_RETRY,		"(Retry command)")
#define ERR_NOT_RESPONDING		PKG_NETWORK+10
	errortext(ERR_NOT_RESPONDING,	"Remote system not responding")
#define ERR_NAME_IN_USE_HERE	PKG_NETWORK+11
	errortext(ERR_NAME_IN_USE_HERE,	"Network name already in use on this system")
#define ERR_INSUF_RCVD_DATA		PKG_NETWORK+12
	errortext(ERR_INSUF_RCVD_DATA,	"Network error: insufficient data received")
#define ERR_SEQUENCE_NUMBER		PKG_NETWORK+13
	errortext(ERR_SEQUENCE_NUMBER,	"Network error: message has incorrect sequence number")
#define ERR_BFR_TOO_SMALL		PKG_NETWORK+14
	errortext(ERR_BFR_TOO_SMALL,	"(Network error: buffer was too small)")
#define ERR_NO_NETBIOS			PKG_NETWORK+15
	errortext(ERR_NO_NETBIOS,	"NETBIOS not loaded or not running")
#define	ERR_WRONG_SERVER		PKG_NETWORK+16
	errortext(ERR_WRONG_SERVER,	"Specified port(s) currently in use")
#define ERR_DISCONNECTED		PKG_NETWORK+17
	errortext(ERR_DISCONNECTED,	"Disconnected from Server")
#define	ERR_HANGUP				PKG_NETWORK+18
	errortext(ERR_HANGUP,		"Phone hung up locally")
#define	ERR_TERMINATED			PKG_NETWORK+19
	debugtext(ERR_TERMINATED, 	"(Port successfully terminated)")
#define	ERR_WRONG_STATE			PKG_NETWORK+20
	errortext(ERR_WRONG_STATE,	"Port not ready or carrier not established")
#define	ERR_REMOTE_BUSY			PKG_NETWORK+21
	errortext(ERR_REMOTE_BUSY,	"Remote phone was busy")
#define	ERR_NO_ANSWER			PKG_NETWORK+22
	errortext(ERR_NO_ANSWER,	"Remote phone did not answer")
#define ERR_NO_CARRIER			PKG_NETWORK+23
	errortext(ERR_NO_CARRIER,	"Remote phone did not answer or modem could not detect carrier")
#define ERR_NO_DIALTONE			PKG_NETWORK+24
	errortext(ERR_NO_DIALTONE,	"Modem could not detect dial tone")
#define ERR_BADPORT_ARGS		PKG_NETWORK+25
	errortext(ERR_BADPORT_ARGS,	"Invalid port initialization argument list")
#define	ERR_MODEM_RESPONSE		PKG_NETWORK+26
	errortext(ERR_MODEM_RESPONSE, "Received unrecognizable response from modem")
#define	ERR_MODEM_NORESP		PKG_NETWORK+27
	errortext(ERR_MODEM_NORESP,	"Modem does not respond")
#define	ERR_HANDSHAKE			PKG_NETWORK+28
	errortext(ERR_HANDSHAKE,	"Could not establish dialog with remote system")
#define ERR_SESS_NOT_READY		PKG_NETWORK+29
	errortext(ERR_SESS_NOT_READY, "Session is not yet opened")
#define	ERR_NO_SUCH_UNIT		PKG_NETWORK+30
	errortext(ERR_NO_SUCH_UNIT,	"Communications port hardware not present or in use by another device.")
#define	ERR_DEVICE_IN_USE		PKG_NETWORK+31
	errortext(ERR_DEVICE_IN_USE,"Device or unit number already in use")
#define	ERR_DRIVER_IN_USE		PKG_NETWORK+32
	errortext(ERR_DRIVER_IN_USE,"Driver already in use")
#define	ERR_NO_RING_BUFFER		PKG_NETWORK+33
	errortext(ERR_NO_RING_BUFFER,"Driver could not allocate memory for communications buffers")
#define	ERR_NO_BUFFERS			PKG_NETWORK+34
	errortext(ERR_NO_BUFFERS,	"Unable to allocate network buffer(s)")
#define	ERR_BADDEV_PARAMS		PKG_NETWORK+35
	errortext(ERR_BADDEV_PARAMS,"Invalid driver initialization parameters")
#define	ERR_BAD_RESPONSE		PKG_NETWORK+36
	errortext(ERR_BAD_RESPONSE,	"Unrecognized response from system request")
#define ERR_NO_SUCH_PORT		PKG_NETWORK+37
	errortext(ERR_NO_SUCH_PORT,	"No such port known")
#define ERR_NO_SUCH_SESSION		PKG_NETWORK+38
	errortext(ERR_NO_SUCH_SESSION,	"No such session or session has been closed")
#define	ERR_SERVERS_UNKNOWN		PKG_NETWORK+39
	errortext(ERR_SERVERS_UNKNOWN,	"Other servers are not known by this system")
#define	ERR_DUPL_PORTNAME		PKG_NETWORK+40
	errortext(ERR_DUPL_PORTNAME,	 "A port with that name already exists")
#define	ERR_MSGQUEUE_EMPTY		PKG_NETWORK+41
	internaltext(ERR_MSGQUEUE_EMPTY, "Message queue is empty")
#define ERR_NOT_QUEUE			PKG_NETWORK+42
	internaltext(ERR_NOT_QUEUE,	"Structure is not a message queue head")
#define ERR_INSUFF_ARGS			PKG_NETWORK+43
	errortext(ERR_INSUFF_ARGS,	"Not enough parameters specified in NOTES.INI to initialize port driver")
#define ERR_INSUFF_ADAP_RES		PKG_NETWORK+44
	errortext(ERR_INSUFF_ADAP_RES,"Insufficient network or adapter resources.  Consult Network documentation to increase those resources.")
#define ERR_NO_PORT_FUNC		PKG_NETWORK+45
	errortext(ERR_NO_PORT_FUNC,	"Function not implemented on this port")
#define ERR_NO_PROGRESS			PKG_NETWORK+46
	internaltext(ERR_NO_PROGRESS,	"no progress in time allotted (but i/o still active)")
/* 46 Unused */
#define	ERR_NET_NOT_INIT		PKG_NETWORK+48
	debugtext(ERR_NET_NOT_INIT,	 "Network package has not been initialized")
#define ERR_NETBFR_YIELD			PKG_NETWORK+49
	internaltext(ERR_NETBFR_YIELD,		"Yield, then call back with same NETBFR")
#define	ERR_NETBFR_ISSUERCV			PKG_NETWORK+50
	internaltext(ERR_NETBFR_ISSUERCV,	"Use NETBFR to issue a receive")
#define	ERR_NETBFR_ISSUEXMT			PKG_NETWORK+51
	internaltext(ERR_NETBFR_ISSUEXMT,	"Use NETBFR to issue a transmit")
#define	ERR_NETBFR_INUSE			PKG_NETWORK+52
	internaltext(ERR_NETBFR_INUSE,		"NETBFR has been consumed")
#define	ERR_ADAP_NOT_INIT			PKG_NETWORK+53
	errortext(ERR_ADAP_NOT_INIT,	"Unable to initialize network adapter.  Check your LAN hardware configuration.")
#define ERR_NO_SERVER_INFO			PKG_NETWORK+54
	errortext(ERR_NO_SERVER_INFO,	"Name and Address database contains no Connection document for that server")
#define ERR_NO_PORT_FROM_DB			PKG_NETWORK+55
	errortext(ERR_NO_PORT_FROM_DB,	"No ports specified in connection document found in the Address Book (to reach the server) are configured on this system")
#define ERR_NO_SUCH_SITE			PKG_NETWORK+56
	errortext(ERR_NO_SUCH_SITE,		"Site is unknown")
#define ERR_NO_WORK_SCHEDULED		PKG_NETWORK+57
	internaltext(ERR_NO_WORK_SCHEDULED,"(No work is scheduled)")
#define ERR_UNKNOWN_APPL			PKG_NETWORK+58
	errortext(ERR_UNKNOWN_APPL,		"Connections database has no information for that application")
#define ERR_TOO_MANY_APPLS			PKG_NETWORK+59
	errortext(ERR_TOO_MANY_APPLS,	"Connections database contains too many different applications")
#define ERR_LANMAN_PROC_LIMIT		PKG_NETWORK+60
	errortext(ERR_LANMAN_PROC_LIMIT,"The LANMAN process limit has been exceeded.  This cannot be increased.  Reduce usage by limiting clients (see SERVER_MAXSESSIONS parameter in Notes Admin Guide).")
#define ERR_NET_CONTINUE			PKG_NETWORK+61
	internaltext(ERR_NET_CONTINUE,	"Contine operation.")

#define STR_MAC_DEFAULT_PORTNAME4					PKG_NETWORK+62
	stringtext(STR_MAC_DEFAULT_PORTNAME4,			"MacTCP")

#define ERR_LINK_TABLE_FULL			PKG_NETWORK+63
	internaltext(ERR_LINK_TABLE_FULL,	"Link notification table is full")
#define ERR_NET_PORT_VERSION		PKG_NETWORK+64
	errortext(ERR_NET_PORT_VERSION,	"The network port driver is an incompatible version")
#define ERR_NO_SCHED_INFO			PKG_NETWORK+65
	errortext(ERR_NO_SCHED_INFO,	"Name & Address Book contains a Connection entry with no schedule information")
#define ERR_NET_ONPORT_SUFFIX		PKG_NETWORK+66
	stringtext(ERR_NET_ONPORT_SUFFIX," on port ")
#define ERR_NET_CARRIER_DOWN		PKG_NETWORK+67
	errortext(ERR_NET_CARRIER_DOWN,	"Modem lost carrier")
#define	ERR_NET_CARRIER_UP			PKG_NETWORK+68
	stringtext(ERR_NET_CARRIER_UP,	" bits per second connection established")
#define	ERR_NET_PORT_REINIT			PKG_NETWORK+69
	errortext(ERR_NET_PORT_REINIT,	"Some changes may require a restart to take effect")
#define ERR_DIAL_QUERY1				PKG_NETWORK+70
	errortext(ERR_DIAL_QUERY1,		"Make call to server ")
#define ERR_DIAL_QUERY2				PKG_NETWORK+71
	stringtext(ERR_DIAL_QUERY2,		" on port ")
#define ERR_DIAL_QUERY3				PKG_NETWORK+72
	stringtext(ERR_DIAL_QUERY3,		"?")
#define ERR_DIAL_QUERY_NO			PKG_NETWORK+73
	errortext(ERR_DIAL_QUERY_NO,	"Remote computer is not currently accessible")
#define	ERR_1200_DIAL_ERROR			PKG_NETWORK+74
	errortext(ERR_1200_DIAL_ERROR,	"Could not detect carrier or dialtone; or phone was busy or did not answer")
#define	ERR_MODEM_BADCMD			PKG_NETWORK+75
	errortext(ERR_MODEM_BADCMD,		"Modem reported command error.  Check: (1) That the command file selected is correct; \
(2) If editing a command file, that all commands conform to modem vendor documentation of supported AT commands.")
#define	ERR_STRING_RESOURCE			PKG_NETWORK+76
	errortext(ERR_STRING_RESOURCE,	"String resource not provided")
#define	ERR_NET_CLEARING_DTR		PKG_NETWORK+77
	errortext(ERR_NET_CLEARING_DTR,	"Clearing modem's DTR")
#define	ERR_NET_ANSWERING_PHONE		PKG_NETWORK+78
	errortext(ERR_NET_ANSWERING_PHONE,	"Error encountered while answering an incoming call")
#define ERR_NO_SUCH_TASK			PKG_NETWORK+79
	errortext(ERR_NO_SUCH_TASK,		"No such task known")
#define ERR_DIAL_TIMER_EXPIRED		PKG_NETWORK+80
	errortext(ERR_DIAL_TIMER_EXPIRED, "Call Timer expired")
#define ERR_UNKNOWN_LANMAN			PKG_NETWORK+81
	errortext(ERR_UNKNOWN_LANMAN,	"Unknown LAN Manager error")
#define ERR_NET_NOT_STARTED			PKG_NETWORK+82
	errortext(ERR_NET_NOT_STARTED,	"Network not started")
#define ERR_INTERNAL_LANMAN			PKG_NETWORK+83
	errortext(ERR_INTERNAL_LANMAN,	"Internal error in network operating system")
#define ERR_DRIVER_HANDLE			PKG_NETWORK+84
	errortext(ERR_DRIVER_HANDLE,	"Invalid network driver or semaphore handle")
#define ERR_BAD_UNIT_NUM			PKG_NETWORK+85
	errortext(ERR_BAD_UNIT_NUM,		"NETBIOS unit/lana number specified in NOTES.INI is not valid on this system, or a network is not started")
#define ERR_INSUFF_OS_RES			PKG_NETWORK+86
	errortext(ERR_INSUFF_OS_RES,	"Insufficient operating system resources")
#define ERR_INSUFF_CMDS				PKG_NETWORK+87
	errortext(ERR_INSUFF_CMDS,		"Network adapter cannot issue any more commands.  Consult your Network documentation to increase the number of commands.")
#define ERR_ADAP_BUSY				PKG_NETWORK+88
	errortext(ERR_ADAP_BUSY,		"Network adapter too busy to handle request")
#define ERR_NOLOGFILE				PKG_NETWORK+89
	errortext(ERR_NOLOGFILE,		"Unable to open log file")
#define	ERR_DRIVER_NOT_INIT			PKG_NETWORK+90
	errortext(ERR_DRIVER_NOT_INIT,	 "Network driver has not been initialized")
#define ERR_LOGENTRY_INVALID		PKG_NETWORK+91
	debugtext(ERR_LOGENTRY_INVALID,	"LogEntry out of range")
#define ERR_LOG_NULLHANDLE			PKG_NETWORK+92
	debugtext(ERR_LOG_NULLHANDLE,  	"LogEntry not allocated")
#define ERR_LOG_RESERVED_ENTRY		PKG_NETWORK+93
	debugtext(ERR_LOG_RESERVED_ENTRY,	"LogEntry reserved")
#define ERR_NO_ITEM_NAME			PKG_NETWORK+94
	internaltext(ERR_NO_ITEM_NAME,	"(No item name)")
#define ERR_DIAL_QUERY2_REMOTE_LAN	PKG_NETWORK+95
	stringtext(ERR_DIAL_QUERY2_REMOTE_LAN,	" via remote LAN using connection info ")
#define ERR_ADAP_CABLE				PKG_NETWORK+96
	errortext(ERR_ADAP_CABLE,		"Network adapter not working; check cable")
#define ERR_ADAP_NAME_TABLE			PKG_NETWORK+97
	errortext(ERR_ADAP_NAME_TABLE,	"Network name cannot be used")
#define ERR_CANT_ENABLE_PORTS		PKG_NETWORK+98
	errortext(ERR_CANT_ENABLE_PORTS,"Additional ports may not be enabled while the server is running")
#define	ERR_ILL_REQ_CODE			PKG_NETWORK+99
	errortext(ERR_ILL_REQ_CODE,		"Illegal or unrecognized request code")

/* 100 unused	*/

#define ERR_PORT_NAME_CHARS			PKG_NETWORK+101
	stringtext(ERR_PORT_NAME_CHARS, "-_ ")
#define ERR_INVALID_NET_PORT_NAME	PKG_NETWORK+102
	errortext(ERR_INVALID_NET_PORT_NAME, "The only allowed characters for a port name are letters, numbers, hyphen, and underscore")
#define ERR_INACTIVE_PORT			PKG_NETWORK+103
	errortext(ERR_INACTIVE_PORT, "Request failed because the requested port is inactive")
#define ERR_BAD_BUFFER				PKG_NETWORK+104
	errortext(ERR_BAD_BUFFER, "The data in a buffer was too small or was corrupted (file %s, line %d)")
#define ERR_BAD_BUFFER_POST_MSG		PKG_NETWORK+105
	errortext(ERR_BAD_BUFFER_POST_MSG, "Bad buffer - please report this error.  See log.nsf for more info.")

/*  106-109 unused */

#define STR_SCHED_REPL_MISSED	    PKG_NETWORK+110
    stringtext(STR_SCHED_REPL_MISSED,"Missed scheduled replication with server %a at %s.  Last replication completion time: %s.")
#define STR_ITEM_TARGETSERVER		PKG_NETWORK+111
	stringtext(STR_ITEM_TARGETSERVER,"TargetServer")
#define STR_ITEM_REMOTESERVER		PKG_NETWORK+112
	stringtext(STR_ITEM_REMOTESERVER,"RemoteServer")
#define STR_ITEM_PHONENUMBER		PKG_NETWORK+113
	stringtext(STR_ITEM_PHONENUMBER,	"PhoneNumber")
#define STR_ITEM_INCOMINGCALL		PKG_NETWORK+114
	stringtext(STR_ITEM_INCOMINGCALL,"IncomingCall")
#define STR_NET_PORT_REDUCING_SESSIONS	PKG_NETWORK+115
	stringtext(STR_NET_PORT_REDUCING_SESSIONS,"%e - Automatically reducing %s port maximum sessions to %d. Currently, %d sessions are in the process of being recycled.")
#define STR_ITEM_CALLSTATUS			PKG_NETWORK+116
	stringtext(STR_ITEM_CALLSTATUS,	"CallStatus")
#define STR_ITEM_INBOUNDCONNECTS	PKG_NETWORK+117
	stringtext(STR_ITEM_INBOUNDCONNECTS,"InboundConnects")
#define STR_ITEM_OUTBOUNDCONNECTS	PKG_NETWORK+118
	stringtext(STR_ITEM_OUTBOUNDCONNECTS,"OutboundConnects")
#define STR_ITEM_BYTESXMT			PKG_NETWORK+119
	stringtext(STR_ITEM_BYTESXMT,	"BytesXmt")
#define STR_ITEM_BYTESRCV			PKG_NETWORK+120
	stringtext(STR_ITEM_BYTESRCV,	"BytesRcvd")
#define ERR_SERVER_ONLY				PKG_NETWORK+121
	errortext(ERR_SERVER_ONLY,		"This function is only available when used on a Notes Server")
#define STR_JS_ITEM_NAMES			PKG_NETWORK+122
	stringtext(STR_JS_ITEM_NAMES,	"Interval;Schedule;MailThreshold;Tasks;TimeLimit;ConnectionLocation")
#define STR_JS_NAMESPACE			PKG_NETWORK+123
	stringtext(STR_JS_NAMESPACE,		"1\\$Connections")
#define STR_REM_SERVER_SUFFIX		PKG_NETWORK+124
	stringtext(STR_REM_SERVER_SUFFIX," on remote server")
#define STR_JS_PGM_ITEM_NAMES		PKG_NETWORK+125
	stringtext(STR_JS_PGM_ITEM_NAMES, "Program;CmdLine;Schedule;Interval;Enabled;WeekDays")
#define STR_JS_PGM_NAMESPACE		PKG_NETWORK+126
	stringtext(STR_JS_PGM_NAMESPACE,	"1\\$Programs")
#define STR_JS_WKSREP_NAMESPACE		PKG_NETWORK+127
	stringtext(STR_JS_WKSREP_NAMESPACE,	"1\\$Locations")
#define STR_JS_WKSREP_ITEM_NAMES		PKG_NETWORK+128
	stringtext(STR_JS_WKSREP_ITEM_NAMES, "Name;Schedule;Interval;Enabled;WeekDays;Schedule_HI;Interval_HI;Enabled_HI;Weekdays_HI")

/* 129 unused */

#define	STR_ITEM_PORTERRORS			PKG_NETWORK+130
	stringtext(STR_ITEM_PORTERRORS, "PortErrors")
#define STR_ITEM_CRCERRORS			PKG_NETWORK+131
	stringtext(STR_ITEM_CRCERRORS, 	"CRCErrors")
#define STR_ITEM_REXMTS				PKG_NETWORK+132
	stringtext(STR_ITEM_REXMTS,		"ReXmts")
#define STR_ITEM_PORTSPEED			PKG_NETWORK+133
	stringtext(STR_ITEM_PORTSPEED,	"PortSpeed")
#define STR_ITEM_CARRIERSPEED		PKG_NETWORK+134
	stringtext(STR_ITEM_CARRIERSPEED,"CarrierSpeed")
#define ERR_ANQ_SIZE	  				PKG_NETWORK+135
	errortext(ERR_ANQ_SIZE,				"Asynchronous notification message too large")
#define STR_ANAME_LOOKUP_RQST		PKG_NETWORK+136
	stringtext(STR_ANAME_LOOKUP_RQST,"ASYNC_NAME_LOOKUP")
#define ERR_NET_GROUP_POOLFULL		PKG_NETWORK+137
	errortext(ERR_NET_GROUP_POOLFULL,"Insufficient memory - Group Cache Pool is full.")
#define ERR_NET_SCHEDULE_POOLFULL	PKG_NETWORK+138
	errortext(ERR_NET_SCHEDULE_POOLFULL,"Insufficient memory - Scheduling Database Pool is full.")
#define ERR_NET_JS_POOLFULL			PKG_NETWORK+139
	errortext(ERR_NET_JS_POOLFULL,	"Insufficient memory - Job Scheduling Pool is full.")
#define ERR_NET_POOLFULL			PKG_NETWORK+140
	errortext(ERR_NET_POOLFULL,		"Insufficient memory - Network Pool is full.")
#define ERR_TASK_POOLFULL			PKG_NETWORK+141
	errortext(ERR_TASK_POOLFULL,	"Insufficient memory - Network Task Pool is full.")
#define ERR_NET_JSPATH_POOLFULL		PKG_NETWORK+142
	errortext(ERR_NET_JSPATH_POOLFULL,	"Insufficient memory - Job Scheduling Path Cache Pool is full.")
#define ERR_LOG_QUOTA_EXCEEDED		PKG_NETWORK+143
	debugtext(ERR_LOG_QUOTA_EXCEEDED,	"Maximum number of in-memory log entries exceeded; cannot create another until some are flushed to the log database")

/*	Strings which are the names of the various NSF/NIF transactions.
	Used to display them for debugging (CLIENT_CLOCK). */

#define STR_GET_MULT_NOTE_INFO_UNID_RQST	PKG_NETWORK+144
	stringtext(STR_GET_MULT_NOTE_INFO_UNID_RQST,"GET_MULT_NOTE_INFO_BY_UNID")
#define	STR_ASYNC_NIFOPENNOTE_RQST		PKG_NETWORK+145
	stringtext(STR_ASYNC_NIFOPENNOTE_RQST, "ASYNC_NIF_OPEN_NOTE")
#define	STR_ASYNC_READ_ENTRIES_RQST		PKG_NETWORK+146
	stringtext(STR_ASYNC_READ_ENTRIES_RQST, "ASYNC_READ_ENTRIES")
#define	STR_ASYNC_CANCEL_RQST			PKG_NETWORK+147
	stringtext(STR_ASYNC_CANCEL_RQST,	"ASYNC_CANCEL")
#define	STR_ASYNC_OPEN_NOTE_RQST		PKG_NETWORK+148
	stringtext(STR_ASYNC_OPEN_NOTE_RQST, "ASYNC_OPEN_NOTE")
#define	STR_ASYNC_READ_OBJECT_RQST		PKG_NETWORK+149
	stringtext(STR_ASYNC_READ_OBJECT_RQST, "ASYNC_READ_OBJECT")
#define STR_OPEN_DB_RQST				PKG_NETWORK+150
	stringtext(STR_OPEN_DB_RQST,			"OPEN_DB")
#define STR_CREATE_DB_RQST				PKG_NETWORK+151
	stringtext(STR_CREATE_DB_RQST,		"CREATE_DB")
#define STR_CLOSE_DB_RQST				PKG_NETWORK+152
	stringtext(STR_CLOSE_DB_RQST,		"CLOSE_DB")
#define STR_CLOSE_DB_RQST_ALT			PKG_NETWORK+153
	stringtext(STR_CLOSE_DB_RQST_ALT,	"CLOSE_DB")
#define STR_GET_SPECIAL_NOTE_ID_RQST	PKG_NETWORK+154
	stringtext(STR_GET_SPECIAL_NOTE_ID_RQST,	"GET_SPECIAL_NOTE_ID")
#define STR_ITEM_DEF_TABLE_RQST			PKG_NETWORK+155
	stringtext(STR_ITEM_DEF_TABLE_RQST,	"ITEM_DEF_TABLE")
#define STR_OPEN_NOTE_RQST				PKG_NETWORK+156
	stringtext(STR_OPEN_NOTE_RQST,		"OPEN_NOTE")
#define STR_DELETE_NOTE_RQST			PKG_NETWORK+157
	stringtext(STR_DELETE_NOTE_RQST,		"DELETE_NOTE")
#define STR_GET_NOTE_INFO_RQST			PKG_NETWORK+158
	stringtext(STR_GET_NOTE_INFO_RQST,	"GET_NOTE_INFO")
#define STR_SET_SPECIAL_NOTE_ID_RQST	PKG_NETWORK+159
	stringtext(STR_SET_SPECIAL_NOTE_ID_RQST,	"SET_SPECIAL_NOTE_ID")
#define STR_DB_INFO_GET_RQST			PKG_NETWORK+160
	stringtext(STR_DB_INFO_GET_RQST,		"DB_INFO_GET")
#define STR_DB_INFO_SET_RQST			PKG_NETWORK+161
	stringtext(STR_DB_INFO_SET_RQST,		"DB_INFO_SET")
#define STR_DB_MODIFIED_TIME_RQST		PKG_NETWORK+162
	stringtext(STR_DB_MODIFIED_TIME_RQST,"DB_MODIFIED_TIME")
#define STR_SEARCHSTART_RQST			PKG_NETWORK+163
	stringtext(STR_SEARCHSTART_RQST,		"SEARCH")
#define STR_SEARCHSTOP_RQST				PKG_NETWORK+164
	stringtext(STR_SEARCHSTOP_RQST,		"SEARCHSTOP")
#define STR_SERVER_TIME_RQST			PKG_NETWORK+165
	stringtext(STR_SERVER_TIME_RQST,		"SERVER_TIME")
#define STR_DELETE_DB_RQST				PKG_NETWORK+166
	stringtext(STR_DELETE_DB_RQST,		"DELETE_DB")
#define STR_FILE_SUMMARY_RQST			PKG_NETWORK+167
	stringtext(STR_FILE_SUMMARY_RQST,	"FILE_SUMMARY")
/* 168, 169 unused */
#define STR_DB_REPLINFO_SET_RQST		PKG_NETWORK+170
	stringtext(STR_DB_REPLINFO_SET_RQST,	"DB_REPLINFO_SET")
#define STR_DB_REPLINFO_GET_RQST		PKG_NETWORK+171
	stringtext(STR_DB_REPLINFO_GET_RQST,	"DB_REPLINFO_GET")
#define STR_GET_MODIFIED_NOTES_RQST		PKG_NETWORK+172
	stringtext(STR_GET_MODIFIED_NOTES_RQST,	"GET_MODIFIED_NOTES")
#define STR_STAMP_NOTES_RQST			PKG_NETWORK+173
	stringtext(STR_STAMP_NOTES_RQST,		"STAMP_NOTES")
#define STR_RENAME_DB_RQST				PKG_NETWORK+174
	stringtext(STR_RENAME_DB_RQST,		"RENAME_DB")
#define STR_REPLICATE_RQST				PKG_NETWORK+175
	stringtext(STR_REPLICATE_RQST,		"REPLICATE")
#define STR_GET_OBJECT_SIZE_RQST		PKG_NETWORK+176
	stringtext(STR_GET_OBJECT_SIZE_RQST,	"GET_OBJECT_SIZE")
#define STR_FREE_OBJECT_RQST			PKG_NETWORK+177
	stringtext(STR_FREE_OBJECT_RQST,		"FREE_OBJECT")
#define STR_ALLOC_OBJECT_RQST			PKG_NETWORK+178
	stringtext(STR_ALLOC_OBJECT_RQST,	"ALLOC_OBJECT")
#define STR_REALLOC_OBJECT_RQST			PKG_NETWORK+179
	stringtext(STR_REALLOC_OBJECT_RQST,	"REALLOC_OBJECT")
#define STR_READ_OBJECT_RQST			PKG_NETWORK+180
	stringtext(STR_READ_OBJECT_RQST,		"READ_OBJECT")
#define STR_WRITE_OBJECT_RQST			PKG_NETWORK+181
	stringtext(STR_WRITE_OBJECT_RQST,	"WRITE_OBJECT")
#define STR_OPEN_COLLECTION_RQST		PKG_NETWORK+182
	stringtext(STR_OPEN_COLLECTION_RQST,	"OPEN_COLLECTION")
#define STR_CLOSE_COLLECTION_RQST		PKG_NETWORK+183
	stringtext(STR_CLOSE_COLLECTION_RQST,"CLOSE_COLLECTION")
#define STR_CLOSE_COLLECTION_RQST_ALT	PKG_NETWORK+184
	stringtext(STR_CLOSE_COLLECTION_RQST_ALT,"CLOSE_COLLECTION")
#define STR_UPDATE_COLLECTION_RQST		PKG_NETWORK+185
	stringtext(STR_UPDATE_COLLECTION_RQST,"UPDATE_COLLECTION")
#define STR_UPDATE_FILTERS_RQST			PKG_NETWORK+186
	stringtext(STR_UPDATE_FILTERS_RQST,	"UPDATE_FILTERS")
#define STR_READ_ENTRIES_RQST			PKG_NETWORK+187
	stringtext(STR_READ_ENTRIES_RQST,	"READ_ENTRIES")
#define STR_LOCATE_NOTE_RQST			PKG_NETWORK+188
	stringtext(STR_LOCATE_NOTE_RQST,		"LOCATE_NOTE")
#define STR_FIND_NOTEID_RQST			PKG_NETWORK+189
	stringtext(STR_FIND_NOTEID_RQST,		"FIND_NOTEID")
#define STR_UPDATE_NOTE_RQST_ALT		PKG_NETWORK+190
	stringtext(STR_UPDATE_NOTE_RQST_ALT,	"UPDATE_NOTE")
#define STR_LOOKUP_HELP_NOTE_RQST		PKG_NETWORK+191
	stringtext(STR_LOOKUP_HELP_NOTE_RQST,"LOOKUP_HELP_NOTE")
#define STR_FIND_BY_KEY_RQST			PKG_NETWORK+192
	stringtext(STR_FIND_BY_KEY_RQST,		"FIND_BY_KEY")
#define STR_NAME_LOOKUP_RQST			PKG_NETWORK+193
	stringtext(STR_NAME_LOOKUP_RQST,		"NAME_LOOKUP")
#define STR_AUTHENTICATE_RQST			PKG_NETWORK+194
	stringtext(STR_AUTHENTICATE_RQST,	"EXPLICIT_AUTHENTICATE")
#define STR_DB_SPACE_USAGE_RQST			PKG_NETWORK+195
	stringtext(STR_DB_SPACE_USAGE_RQST,	"SPACE_USAGE")
#define STR_TEXT_SEARCH_RQST			PKG_NETWORK+196
	stringtext(STR_TEXT_SEARCH_RQST,		"TEXT SEARCH")
#define ERR_QUIT						PKG_NETWORK+197
	errortext(ERR_QUIT,					"Program shutdown in progress")
#define STR_ALLOC_UPOBJECT_RQST			PKG_NETWORK+198
	stringtext(STR_ALLOC_UPOBJECT_RQST,	"ALLOC_UPDATE_OBJECT")
#define STR_FREE_UPOBJECT_RQST			PKG_NETWORK+199
	stringtext(STR_FREE_UPOBJECT_RQST,	"FREE_UPDATE_OBJECT")
#define STR_NIFOPENNOTE_RQST			PKG_NETWORK+200
	stringtext(STR_NIFOPENNOTE_RQST,		"NIF_OPEN_NOTE")
#define STR_NIFSTAMPNOTES_RQST			PKG_NETWORK+201
	stringtext(STR_NIFSTAMPNOTES_RQST,	"NIF_STAMP_NOTES")
#define STR_GET_SERVER_NAMES_RQST		PKG_NETWORK+202
	stringtext(STR_GET_SERVER_NAMES_RQST,"GET_SERVER_NAMES")
#define STR_GET_SERVER_STATS_RQST		PKG_NETWORK+203
	stringtext(STR_GET_SERVER_STATS_RQST,"GET_SERVER_STATS")
#define STR_RELAY_EVENT_RQST			PKG_NETWORK+204
	errortext(STR_RELAY_EVENT_RQST,		"RELAY_EVENT")
#define STR_FT_SEARCH_RQST				PKG_NETWORK+205
	stringtext(STR_FT_SEARCH_RQST,		"FULL TEXT SEARCH")
#define STR_FT_CLOSE_SEARCH_RQST		PKG_NETWORK+206
	stringtext(STR_FT_CLOSE_SEARCH_RQST,"CLOSE FULL TEXT SEARCH")
#define STR_REMOTE_CONSOLE_RQST			PKG_NETWORK+207
	errortext(STR_REMOTE_CONSOLE_RQST,	"REMOTE_CONSOLE")
#define STR_DB_SQUEEZE_RQST				PKG_NETWORK+208
	stringtext(STR_DB_SQUEEZE_RQST,		"SQUEEZE DATABASE")
#define STR_FT_INDEX_RQST				PKG_NETWORK+209
	stringtext(STR_FT_INDEX_RQST,		"BUILD FULL TEXT INDEX")
#define STR_FT_GET_LAST_INDEXTIME_RQST	PKG_NETWORK+210
	stringtext(STR_FT_GET_LAST_INDEXTIME_RQST,	"GET LAST INDEX TIME")
#define STR_FT_DELETE_INDEX_RQST		PKG_NETWORK+211
	stringtext(STR_FT_DELETE_INDEX_RQST,"DELETE FULL TEXT INDEX")
#define STR_GET_NAMED_OBJECT_ID_RQST	PKG_NETWORK+212
	stringtext(STR_GET_NAMED_OBJECT_ID_RQST,"GET_NAMED_OBJECT_ID")
#define STR_DB_READ_HIST_RQST			PKG_NETWORK+213
	stringtext(STR_DB_READ_HIST_RQST,"READ REPLICATION HISTORY")
#define STR_DB_WRITE_HIST_RQST			PKG_NETWORK+214
	stringtext(STR_DB_WRITE_HIST_RQST,"WRITE REPLICATION HISTORY")
#define STR_GET_NOTE_INFO_BY_UNID_RQST	PKG_NETWORK+215
	stringtext(STR_GET_NOTE_INFO_BY_UNID_RQST,"GET_NOTE_INFO_BY_UNID")
#define STR_NAME_LOOKUPID_RQST			PKG_NETWORK+216
	stringtext(STR_NAME_LOOKUPID_RQST,	"NAME_LOOKUPID")
#define STR_POLL_DEL_SEQNUM_RQST		PKG_NETWORK+217
	stringtext(STR_POLL_DEL_SEQNUM_RQST,"POLL_DEL_SEQNUM")
#define STR_GET_MULT_NOTE_INFO_RQST		PKG_NETWORK+218
	stringtext(STR_GET_MULT_NOTE_INFO_RQST,"GET_MULT_NOTE_INFO")
#define STR_GET_COLLECTION_DATA_RQST	PKG_NETWORK+219
	stringtext(STR_GET_COLLECTION_DATA_RQST,"GET_COLLECTION_DATA")

/* The following strings are used by newsetup to try to set up Netbios for
	unit number other than 0 - dba */

#define STR_LANX_PORT					PKG_NETWORK+220
	stringtext(STR_LANX_PORT,			"LAN")
#define STR_LANX_PORTINFO				PKG_NETWORK+221
	stringtext(STR_LANX_PORTINFO,		"NETBIOS, ")
#define STR_LANX_PORTINFO2				PKG_NETWORK+222
	stringtext(STR_LANX_PORTINFO2,		", 15, 2000")
#define STR_LAN1_PORT					PKG_NETWORK+223
	stringtext(STR_LAN1_PORT,			"LAN1")
#define STR_LAN1_PORTINFO				PKG_NETWORK+224
	stringtext(STR_LAN1_PORTINFO,		"NETBIOS, 1, 15, 2000")
#define	ERR_ILL_SVCTYPE					PKG_NETWORK+225
	errortext(ERR_ILL_SVCTYPE,
	"The specified network service class is not appropriate for this request")
#define ERR_OPEN_MDMSCRIPT				PKG_NETWORK+226
	errortext(ERR_OPEN_MDMSCRIPT,		"Error opening modem command file")
#define ERR_WRONG_SERVERNAME			PKG_NETWORK+227
	errortext(ERR_WRONG_SERVERNAME,		"The server that answered does not have the expected name")
#define	ERR_EXCEED_QUOTA				PKG_NETWORK+228
	errortext(ERR_EXCEED_QUOTA,			"Queue is full")
#define ERR_DIALTIMER_EXPIRED			PKG_NETWORK+229
	errortext(ERR_DIALTIMER_EXPIRED,	"Call timer expired")
#define ERR_LOGIN_INIT					PKG_NETWORK+230
	errortext(ERR_LOGIN_INIT,			"Cannot locate required login information")
#define ERR_NET_MODEM_INIT				PKG_NETWORK+231
	errortext(ERR_NET_MODEM_INIT,		"Initializing...")
#define ERR_NET_MODEM_DIALING			PKG_NETWORK+232
	errortext(ERR_NET_MODEM_DIALING,	"Calling %a at %s...")

/*	These strings are used in netinit.c and some of them by newsetup.c.
	Perhaps the code in netinit.c is no longer necessary, but I'm leaving
	it alone for Version 3.  I have invented new strings that newsetup.c
	needs and left in (and used some of) the ones used by netinit.c. -- dba 
*/

#define STR_DEFAULT_PORTLIST						PKG_NETWORK+233
	stringtext(STR_DEFAULT_PORTLIST,				"LAN0")
#define STR_MAC_DEFAULT_PORTLIST					PKG_NETWORK+234
	stringtext(STR_MAC_DEFAULT_PORTLIST,			"AppleTalk,Modem")

#define STR_DEFAULT_DISABLEDPORTS					PKG_NETWORK+235
	stringtext(STR_DEFAULT_DISABLEDPORTS,			"COM1,COM2")
#define STR_MAC_DEFAULT_DISABLEDPORTS				PKG_NETWORK+236
	stringtext(STR_MAC_DEFAULT_DISABLEDPORTS,		"Printer")
#define STR_UNIX_DEFAULT_DISABLEDPORTS				PKG_NETWORK+237
	stringtext(STR_UNIX_DEFAULT_DISABLEDPORTS,		"Serial1,Serial2")

#define STR_DEFAULT_PORTNAME1						PKG_NETWORK+238
	stringtext(STR_DEFAULT_PORTNAME1,				"LAN0")
#define STR_MAC_DEFAULT_PORTNAME1					PKG_NETWORK+239
	stringtext(STR_MAC_DEFAULT_PORTNAME1,			"AppleTalk")

#define STR_UNIX_DEFAULT_PORTNAME1					PKG_NETWORK+240
	stringtext(STR_UNIX_DEFAULT_PORTNAME1,			"TCPIP")

#define STR_DEFAULT_PORTNAME2						PKG_NETWORK+241
	stringtext(STR_DEFAULT_PORTNAME2,				"COM1")
#define STR_MAC_DEFAULT_PORTNAME2					PKG_NETWORK+242
	stringtext(STR_MAC_DEFAULT_PORTNAME2,			"Modem")

#define STR_UNIX_DEFAULT_PORTNAME2					PKG_NETWORK+243
	stringtext(STR_UNIX_DEFAULT_PORTNAME2,			"Serial1")

#define STR_DEFAULT_PORTNAME3						PKG_NETWORK+244
	stringtext(STR_DEFAULT_PORTNAME3,				"COM2")
#define STR_MAC_DEFAULT_PORTNAME3					PKG_NETWORK+245
	stringtext(STR_MAC_DEFAULT_PORTNAME3,			"Printer")


#define STR_UNIX_DEFAULT_PORTNAME3					PKG_NETWORK+246
	stringtext(STR_UNIX_DEFAULT_PORTNAME3,			"Serial2")

/* 247-251 unused */

#define STR_DEFAULT_COMPORTS_LIST					PKG_NETWORK+252
	stringtext(STR_DEFAULT_COMPORTS_LIST,			"COM1,COM2,COM3,COM4,COM5")
#define STR_MAC_DEFAULT_COMPORTS_LIST				PKG_NETWORK+253
	stringtext(STR_MAC_DEFAULT_COMPORTS_LIST,		"Modem,Printer")
#define STR_UNIX_DEFAULT_COMPORTS_LIST				PKG_NETWORK+254
	stringtext(STR_UNIX_DEFAULT_COMPORTS_LIST,		"Serial1,Serial2")
#define STR_NETWORK_COMPORTS_NONE					PKG_NETWORK+255
	stringtext(STR_NETWORK_COMPORTS_NONE,			"(None)")


/*  PKG_NETWORK2  Begins here */


#define STR_REPLICA_MATCHES_RQST					PKG_NETWORK2+1
	stringtext(STR_REPLICA_MATCHES_RQST,			"DBGETREPLICAMATCHES")
#define ERR_LOCATIONNOTFOUND						PKG_NETWORK2+2
	stringtext(ERR_LOCATIONNOTFOUND,				"You do not have any locations defined.  Choose File\\Mobile\\Define Locations to create a location.")
#define STR_UPDATE_NOTE_RQST						PKG_NETWORK2+3
	stringtext(STR_UPDATE_NOTE_RQST,				"UPDATE_NOTE(7)")
#define STR_DB_LSEC_INFO_GET_RQST					PKG_NETWORK2+4
	stringtext(STR_DB_LSEC_INFO_GET_RQST,			"DB_LSEC_INFO_GET")
#define STR_DB_LSEC_INFO_SET_RQST					PKG_NETWORK2+5
	stringtext(STR_DB_LSEC_INFO_SET_RQST,			"DB_LSEC_INFO_SET")
#define STR_CREATE_COLLECTION_RQST					PKG_NETWORK2+6
	stringtext(STR_CREATE_COLLECTION_RQST,			"CREATE_COLLECTION")
#define STR_GET_SERVER_NAMES_LITE_RQST				PKG_NETWORK2+7
	stringtext(STR_GET_SERVER_NAMES_LITE_RQST,		"GET_SERVER_NAMES_LITE")
#define STR_NAME_GET_AB_RQST						PKG_NETWORK2+8
	stringtext(STR_NAME_GET_AB_RQST,				"NAME_GET_AB")
#define STR_OPEN_SESSION_RQST						PKG_NETWORK2+9
	stringtext(STR_OPEN_SESSION_RQST,				"OPEN_SESSION")
#define STR_UPDATE_UNID_TABLE_RQST					PKG_NETWORK2+10
	stringtext(STR_UPDATE_UNID_TABLE_RQST,			"UPDATE_UNID_TABLE")
#define STR_LAN_LOCATIONNAME						PKG_NETWORK2+11
	stringtext(STR_LAN_LOCATIONNAME,				"Office (Network)")
#define STR_TRAVEL_LOCATIONNAME						PKG_NETWORK2+12
	stringtext(STR_TRAVEL_LOCATIONNAME,				"Travel (Modem)")
#define STR_NONE_LOCATIONNAME						PKG_NETWORK2+13
	stringtext(STR_NONE_LOCATIONNAME,				"Island (Disconnected)")
#define STR_ILLEGAL_RQST							PKG_NETWORK2+14
	stringtext(STR_ILLEGAL_RQST,					"ILLEGAL")
#define STR_NOTFOUND_LOCATIONNAME					PKG_NETWORK2+15
	stringtext(STR_NOTFOUND_LOCATIONNAME,			"None")
#define STR_SET_COLLATION_RQST                      PKG_NETWORK2+16
	stringtext(STR_SET_COLLATION_RQST,				"SET_COLLATION")
#define STR_DB_QUOTA_SET_RQST						PKG_NETWORK2+17
	stringtext(STR_DB_QUOTA_SET_RQST,				"DB_QUOTA_SET")
#define STR_SERVER_AVAILABLE_RQST					PKG_NETWORK2+18
	stringtext(STR_SERVER_AVAILABLE_RQST,			"SERVER_AVAILABLE")
#define STR_SERVER_FIND_REPID_RQST					PKG_NETWORK2+19
	stringtext(STR_SERVER_FIND_REPID_RQST,			"SERVER_FIND_REPID")
#define STR_DB_QUOTA_GET_RQST						PKG_NETWORK2+20
	stringtext(STR_DB_QUOTA_GET_RQST,				"DB_QUOTA_GET")
#define ERR_REMOTE_LAN_SERVICE_NOT_AVAIL			PKG_NETWORK2+21
	stringtext(ERR_REMOTE_LAN_SERVICE_NOT_AVAIL,	"Remote LAN service is not available")
#define ERR_REMOTE_LAN_SERVICE_INITIALIZATION		PKG_NETWORK2+22
	stringtext(ERR_REMOTE_LAN_SERVICE_INITIALIZATION, "Error initializing remote LAN service")
#define ERR_REMOTE_LAN_KNOWN_ERROR					PKG_NETWORK2+23
	stringtext(ERR_REMOTE_LAN_KNOWN_ERROR,			"%s Error %d: %s")
#define ERR_REMOTE_LAN_UNKNOWN_ERROR				PKG_NETWORK2+24
	stringtext(ERR_REMOTE_LAN_UNKNOWN_ERROR, 		"Unknown %s error %d")
#define ERR_REMOTE_LAN_ERROR						PKG_NETWORK2+25
	stringtext(ERR_REMOTE_LAN_ERROR,				"Error returned from external Remote LAN service. See status display or Notes log for message")
#define ERR_REMOTE_LAN_STARTING_CONNECTION			PKG_NETWORK2+26
	stringtext(ERR_REMOTE_LAN_STARTING_CONNECTION, 	"Calling remote LAN")
#define ERR_REMOTE_LAN_PHYSICALLY_CONNECTED			PKG_NETWORK2+27
	stringtext(ERR_REMOTE_LAN_PHYSICALLY_CONNECTED, "Connected to remote LAN")
#define ERR_REMOTE_LAN_AUTHENTICATING				PKG_NETWORK2+28
	stringtext(ERR_REMOTE_LAN_AUTHENTICATING, 		"Authenticating with remote LAN")
#define ERR_REMOTE_LAN_AUTHENTICATED				PKG_NETWORK2+29
	stringtext(ERR_REMOTE_LAN_AUTHENTICATED, 		"Authenticated")
#define ERR_REMOTE_LAN_WAITING_FOR_CALLBACK			PKG_NETWORK2+30
	stringtext(ERR_REMOTE_LAN_WAITING_FOR_CALLBACK,"Waiting for call back")
#define ERR_REMOTE_LAN_LINK_ESTABLISHED				PKG_NETWORK2+31
	stringtext(ERR_REMOTE_LAN_LINK_ESTABLISHED, 	"Remote LAN link established")
#define ERR_REMOTE_LAN_LINK_FAILED					PKG_NETWORK2+32
	stringtext(ERR_REMOTE_LAN_LINK_FAILED, 			"Link to remote LAN failed")
#define ERR_REMOTE_LAN_ALREADY_CONNECTED			PKG_NETWORK2+33
#ifndef MAC
	stringtext(ERR_REMOTE_LAN_ALREADY_CONNECTED, 	"We are already connected to that remote LAN")
#else
	stringtext(ERR_REMOTE_LAN_ALREADY_CONNECTED, 	"You are already connected over ARA. Please make sure your ARA connection document has the same name as your ARA server.")
#endif
#define STR_UPDATE_FOLDER_RQST						PKG_NETWORK2+34
	stringtext(STR_UPDATE_FOLDER_RQST,				"UPDATE_FOLDER")
#define STR_PURGE_FOLDER_RQST						PKG_NETWORK2+35
	stringtext(STR_PURGE_FOLDER_RQST,				"PURGE_FOLDER")
#define STR_COPY_FOLDER_RQST						PKG_NETWORK2+36
	stringtext(STR_COPY_FOLDER_RQST,				"COPY_FOLDER")
#define STR_START_FOLDER_REPL_SOURCE_RQST			PKG_NETWORK2+37
	stringtext(STR_START_FOLDER_REPL_SOURCE_RQST,	"START_FOLDER_REPL_SOURCE")
#define STR_START_FOLDER_REPL_DEST_RQST				PKG_NETWORK2+38
	stringtext(STR_START_FOLDER_REPL_DEST_RQST,		"START_FOLDER_REPL_DEST")
#define STR_GET_FOLDER_REPL_OPS_RQST				PKG_NETWORK2+39
	stringtext(STR_GET_FOLDER_REPL_OPS_RQST,		"GET_FOLDER_REPL_OPS")
#define STR_APPLY_FOLDER_REPL_OPS_RQST				PKG_NETWORK2+40
	stringtext(STR_APPLY_FOLDER_REPL_OPS_RQST,		"APPLY_FOLDER_REPL_OPS")
#define STR_END_FOLDER_REPL_SOURCE_RQST				PKG_NETWORK2+41
	stringtext(STR_END_FOLDER_REPL_SOURCE_RQST,		"END_FOLDER_REPL_SOURCE")
#define STR_END_FOLDER_REPL_DEST_RQST				PKG_NETWORK2+42
	stringtext(STR_END_FOLDER_REPL_DEST_RQST,		"END_FOLDER_REPL_DEST")
#define STR_FOLDER_GETIDTABLE_RQST					PKG_NETWORK2+43
	stringtext(STR_FOLDER_GETIDTABLE_RQST,			"FOLDER_GETIDTABLE")
#define STR_HOMESERVER_FAILOVER						PKG_NETWORK2+44
	stringtext(STR_HOMESERVER_FAILOVER,				"Home Server failed over to %A")
#define STR_OPEN_NOTE_BY_URL_RQST	                PKG_NETWORK2+45
	stringtext(STR_OPEN_NOTE_BY_URL_RQST,           "OPEN_NOTE_BY_URL")
#define STR_ASYNC_OPEN_NOTE_BY_URL_RQST	            PKG_NETWORK2+46
	stringtext(STR_ASYNC_OPEN_NOTE_BY_URL_RQST,     "ASYNC_OPEN_NOTE_BY_URL")
#define STR_DB_ADMIN_FUNC_RQST						PKG_NETWORK2+47
	stringtext(STR_DB_ADMIN_FUNC_RQST,				"DB_ADMIN_FUNC")
#define STR_DB_ADMIN_SET_RQST						PKG_NETWORK2+48
	stringtext(STR_DB_ADMIN_SET_RQST,				"DB_ADMIN_SET")
#define STR_DB_ADMIN_GET_RQST						PKG_NETWORK2+49
	stringtext(STR_DB_ADMIN_GET_RQST,				"DB_ADMIN_GET")
#define STR_DB_FTSIZE_GET_RQST						PKG_NETWORK2+50
	stringtext(STR_DB_FTSIZE_GET_RQST,				"DB_FTSIZE_GET")
#define STR_PURGE_COLLECTION_RQST					PKG_NETWORK2+51
	stringtext(STR_PURGE_COLLECTION_RQST,			"PURGE_COLLECTION")
#define STR_NIF_UPDATE_FOLDER_RQST					PKG_NETWORK2+52
	stringtext(STR_NIF_UPDATE_FOLDER_RQST,			"NIF_UPDATE_FOLDER")
#define STR_CLUSTER_NAME							PKG_NETWORK2+53
	stringtext(STR_CLUSTER_NAME,					" Cluster name: %A, Server name: %A%n")
#define STR_CLUSTER_PROBE_TIMEOUT					PKG_NETWORK2+54
	stringtext(STR_CLUSTER_PROBE_TIMEOUT,			" Server cluster probe timeout: %d minute(s)%n")
#define STR_CLUSTER_PROBE_COUNT						PKG_NETWORK2+55
	stringtext(STR_CLUSTER_PROBE_COUNT,				" Server cluster probe count: %ld%n")
#define STR_CLUSTER_AVAILABILITY_INDEX				PKG_NETWORK2+56
	stringtext(STR_CLUSTER_AVAILABILITY_INDEX,		" Server availability index: %d (state: %e)%n")
#define STR_CLUSTER_AVAILABILITY_THRESHOLD			PKG_NETWORK2+57
	stringtext(STR_CLUSTER_AVAILABILITY_THRESHOLD,	" Server availability threshold: %d%n")
#define STR_CLUSTER_STATE_AVAILABLE					PKG_NETWORK2+58
	stringtext(STR_CLUSTER_STATE_AVAILABLE,			"AVAILABLE")
#define STR_CLUSTER_STATE_RESTRICTED				PKG_NETWORK2+59
	stringtext(STR_CLUSTER_STATE_RESTRICTED,		"RESTRICTED")
#define STR_CLUSTER_STATE_BUSY						PKG_NETWORK2+60
	stringtext(STR_CLUSTER_STATE_BUSY,				"BUSY")
#define STR_CLUSTER_STATE_UNREACHABLE				PKG_NETWORK2+61
	stringtext(STR_CLUSTER_STATE_UNREACHABLE,		"UNREACHABLE")
#define STR_CLUSTER_STATE_INVALID					PKG_NETWORK2+62
	stringtext(STR_CLUSTER_STATE_INVALID,			"INVALID")
#define STR_CLUSTER_MEMBERS							PKG_NETWORK2+63
	stringtext(STR_CLUSTER_MEMBERS,					" Cluster members (%d)...%n")
#define ERR_CANT_FIND_PT_SERVER						PKG_NETWORK2+64
	stringtext(ERR_CANT_FIND_PT_SERVER,				"Unable to find path to pass-through server")
#define STR_CLUSTER_MEMBER_NAME						PKG_NETWORK2+65
	stringtext(STR_CLUSTER_MEMBER_NAME,				"\tserver: %A, availability index: %d%n")
#define STR_CLUSTER_MEMBER_NAME_ERROR				PKG_NETWORK2+66
	stringtext(STR_CLUSTER_MEMBER_NAME_ERROR,		"\tserver: %A, availability: %e%n")
#define STR_ASYNC_RUNDOWN_RQST						PKG_NETWORK2+67
	stringtext(STR_ASYNC_RUNDOWN_RQST,				"ASYNC_RUNDOWN")
#define STR_START_SERVER_RQST						PKG_NETWORK2+68
	stringtext(STR_START_SERVER_RQST,				"START_SERVER")
#define STR_RUNDOWN_TRANS_RQST						PKG_NETWORK2+69
	stringtext(STR_RUNDOWN_TRANS_RQST,				"RUNDOWN")
#define STR_HOME_LOCATIONNAME						PKG_NETWORK2+70
	stringtext(STR_HOME_LOCATIONNAME,				"Home (Modem)")
#define ERR_CONNECT_FAILED_EARLIER					PKG_NETWORK2+71
	errortext(ERR_CONNECT_FAILED_EARLIER,			"A previous attempt to reach the server was unsuccessful.")
#define STR_SERVER_AVAILABLE_LITE_RQST				PKG_NETWORK2+72
	stringtext(STR_SERVER_AVAILABLE_LITE_RQST,		"SERVER_AVAILABLE_LITE")
#define STR_NIF_PURGE_FOLDER_RQST					PKG_NETWORK2+73
	stringtext(STR_NIF_PURGE_FOLDER_RQST,			"NIF_PURGE_FOLDER")
#define STR_NIF_FOLDER_COUNT_RQST					PKG_NETWORK2+74
	stringtext(STR_NIF_FOLDER_COUNT_RQST,			"NIF_FOLDER_COUNT")
#define ERR_NOLOC_NABOOKNOTFOUND					PKG_NETWORK2+75
	errortext(ERR_NOLOC_NABOOKNOTFOUND,				"Unable to open Name and Address Book (%s), due to error '%e'.  Locations cannot be used until the problem has been corrected.")
#define ERR_NOLOC_NABOOKBADDESIGN					PKG_NETWORK2+76
	errortext(ERR_NOLOC_NABOOKBADDESIGN,			"Unable to create any locations, '%s' does not contain a required view.  Please replace the design with the Address book template.  After the design is replaced, restarting Notes will create default locations.")
#define ERR_NOLOC_OTHERERROR						PKG_NETWORK2+77
	errortext(ERR_NOLOC_OTHERERROR,					"Unable to create locations due to error '%e'.  Locations cannot be used until the problem has been corrected.")
#define STR_CLUSTER_PROBE_PORT						PKG_NETWORK2+78
	stringtext(STR_CLUSTER_PROBE_PORT,				" Server cluster default port: %s%n")
#define ERR_REMOTE_LAN_HANGING_UP					PKG_NETWORK2+79
	stringtext(ERR_REMOTE_LAN_HANGING_UP, 			"Terminating remote LAN link")
#define ERR_REMOTE_LAN_HANGUP_COMPLETE				PKG_NETWORK2+80
	stringtext(ERR_REMOTE_LAN_HANGUP_COMPLETE, 		"Remote LAN link terminated")
#define ERR_CANT_DISABLE_PORTS						PKG_NETWORK2+81
	errortext(ERR_CANT_DISABLE_PORTS,				"ports may not be disabled while the server is running")
#define STR_DB_GET_PURGE_INFO_RQST					PKG_NETWORK2+82
	stringtext(STR_DB_GET_PURGE_INFO_RQST,			"DB_GET_PURGE_INFO")
#define ERR_CONNECT_DISALLOWED						PKG_NETWORK2+83
	errortext(ERR_CONNECT_DISALLOWED,				"Unable to connect to server because user hung up the phone line.")
#define STR_INTERNET_LOCATIONNAME					PKG_NETWORK2+84
	stringtext(STR_INTERNET_LOCATIONNAME,			"Internet")
#define STR_GETSET_DEL_SEQNUM_RQST					PKG_NETWORK2+85
	stringtext(STR_GETSET_DEL_SEQNUM_RQST,			"DB_GETSET_DEL_SEQNUM")
#define STR_ASYNC_URL_GET_HEADER_RQST				PKG_NETWORK2+92
	stringtext(STR_ASYNC_URL_GET_HEADER_RQST,		"ASYNC_URL_GET_HEADER")
#define STR_CLUSTER_STATE_MAXUSERS					PKG_NETWORK2+93
	stringtext(STR_CLUSTER_STATE_MAXUSERS,			"MAXUSERS")
#define STR_DB_DIRLINK_SET_RQST						PKG_NETWORK2+94
	stringtext(STR_DB_DIRLINK_SET_RQST,				"DB_DIRLINK_SET")
#define STR_DB_DIRLINK_GET_RQST						PKG_NETWORK2+95
	stringtext(STR_DB_DIRLINK_GET_RQST,				"DB_DIRLINK_GET")
#define STR_DB_SET_TRUNC_INFO_RQST					PKG_NETWORK2+96
	stringtext(STR_DB_SET_TRUNC_INFO_RQST,			"SET_TRUNC_INFO_RQST")
#define STR_SCHED_RQST								PKG_NETWORK2+97
	stringtext(STR_SCHED_RQST,						"SCHED_RQST")
#define ERR_NAME_FORMAT_NOT_RECOGNIZED				PKG_NETWORK2+98
	errortext(ERR_NAME_FORMAT_NOT_RECOGNIZED,		"Server name format is not recognized by protocol")
#define ERR_NET_DLG_TIMEOUT							PKG_NETWORK2+99
	internaltext(ERR_NET_DLG_TIMEOUT,				"(internal - used for dialog timeout")
#define STR_COPY_OBJECT_RQST						PKG_NETWORK2+100
	stringtext(STR_COPY_OBJECT_RQST,				"COPY_OBJECT")
#define STR_ASYNC_REMOTE_CONSOLE_RQST				PKG_NETWORK2+101
	errortext(STR_ASYNC_REMOTE_CONSOLE_RQST,		"ASYNC_REMOTE_CONSOLE")
#define STR_NET_CREATED_AUTOMATICALLY				PKG_NETWORK2+102
	stringtext(STR_NET_CREATED_AUTOMATICALLY,		"This was created automatically by Notes.")

#define ERR_CREATECONNREC_QUERY1				PKG_NETWORK2+103
	stringtext(ERR_CREATECONNREC_QUERY1,		"To connect to ")
#define ERR_CREATECONNREC_QUERY2				PKG_NETWORK2+104
	stringtext(ERR_CREATECONNREC_QUERY2,		" again, you will need a connection record specifying ")
#define ERR_CREATECONNREC_QUERY3				PKG_NETWORK2+105
	stringtext(ERR_CREATECONNREC_QUERY3,		" as its address. Would you like one created for you?")
#define ERR_CREATECONNREC_DONE					PKG_NETWORK2+106
	stringtext(ERR_CREATECONNREC_DONE,			"The connection record has been created.")
#define ERR_DIAL_QUERY1_ALT						PKG_NETWORK2+107
	errortext(ERR_DIAL_QUERY1_ALT,				"Make call to ")
#define STR_DB_SETSTREAM_MODE_RQST					PKG_NETWORK2+108
	stringtext(STR_DB_SETSTREAM_MODE_RQST,			"DB_SETSTREAM_MODE")
#define STR_ASYNC_READ_OBJECT_BY_URL_RQST			PKG_NETWORK2+109
	stringtext(STR_ASYNC_READ_OBJECT_BY_URL_RQST,	"ASYNC_READ_OBJECT_BY_URL")
#define STR_GET_UNREAD_TABLE_RQST					PKG_NETWORK2+110
	stringtext(STR_GET_UNREAD_TABLE_RQST,			"GET_UNREAD_NOTE_TABLE")
#define STR_SET_UNREAD_TABLE_RQST					PKG_NETWORK2+111
	stringtext(STR_SET_UNREAD_TABLE_RQST,			"SET_UNREAD_NOTE_TABLE")

#define ERR_AFFIRMCONNECTION_QUERY1				PKG_NETWORK2+112
	stringtext(ERR_AFFIRMCONNECTION_QUERY1,		"You have requested ")
#define ERR_AFFIRMCONNECTION_QUERY2				PKG_NETWORK2+113
	stringtext(ERR_AFFIRMCONNECTION_QUERY2,		" but have reached ")
#define ERR_AFFIRMCONNECTION_QUERY3				PKG_NETWORK2+114
	stringtext(ERR_AFFIRMCONNECTION_QUERY3,		".  Is this the server you want?")

#define ERR_NET_RESOLVER						PKG_NETWORK2+115
	stringtext(ERR_NET_RESOLVER,				"DNS resolver error")
#define ERR_NET_RES_HOST_NOT_FOUND				PKG_NETWORK2+116
	stringtext(ERR_NET_RES_HOST_NOT_FOUND,		"Unknown host")
#define ERR_NET_RES_TRY_AGAIN					PKG_NETWORK2+117
	stringtext(ERR_NET_RES_TRY_AGAIN,			"Host name lookup failure")
#define ERR_NET_RES_NO_RECOVERY					PKG_NETWORK2+118
	stringtext(ERR_NET_RES_NO_RECOVERY,			"Unknown server error")
#define ERR_NET_RES_NO_ADDRESS					PKG_NETWORK2+119
	stringtext(ERR_NET_RES_NO_ADDRESS,			"No address associated with name")
#define ERR_REMOTE_LAN_NORESPONSE				PKG_NETWORK2+120
	errortext(ERR_REMOTE_LAN_NORESPONSE,		"Remote LAN service not responding.  Please restart system or re-install service.")
#define ERR_REMOTE_LAN_CONNECT_FAILED			PKG_NETWORK2+121
	errortext(ERR_REMOTE_LAN_CONNECT_FAILED,	"The connection attempt has failed. The modems did not connect properly. Please try again.")
#define ERR_REMOTE_LAN_LINK_DROPPED				PKG_NETWORK2+122
	stringtext(ERR_REMOTE_LAN_LINK_DROPPED, 	"Remote LAN link was dropped outside of Notes")
#define ERR_LOC_POOLFULL						PKG_NETWORK2+123
	errortext(ERR_LOC_POOLFULL,					"Insufficient memory - Location Pool is full.")


/* Only have up to PKG_NETWORK2+127  */

#endif
