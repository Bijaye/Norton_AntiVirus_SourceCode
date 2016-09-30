// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.

#ifndef SECURE_ERR_DEFS
#define SECURE_ERR_DEFS


#define ERR_SECURE_BUG_1				PKG_SECURE+1
	errortext(ERR_SECURE_BUG_1,			"Security package software error #1")

#define ERR_SECURE_NOKEYFILE			PKG_SECURE+2
	errortext(ERR_SECURE_NOKEYFILE,		"Could not open the ID file")

#define ERR_SECURE_ENCRYPTED			PKG_SECURE+3
	errortext(ERR_SECURE_ENCRYPTED,		"Network traffic is being encrypted at the server's request...")

#define	ERR_SECURE_UNKNOWNFCT			PKG_SECURE+4
	errortext(ERR_SECURE_UNKNOWNFCT,	"Unknown authentication function code")

#define ERR_SECURE_NOCLIENTCERT			PKG_SECURE+5
	errortext(ERR_SECURE_NOCLIENTCERT,	"Your ID has not been certified to access the server")
									
#define	ERR_SECURE_NOSERVERCERT			PKG_SECURE+6
	errortext(ERR_SECURE_NOSERVERCERT,	"The server's ID has not been certified for your access")

#define ERR_SECURE_ESTABLISHED			PKG_SECURE+7
	internaltext(ERR_SECURE_ESTABLISHED,"Password handler already established")

#define ERR_SECURE_BADPASSWORD			PKG_SECURE+8
	errortext(ERR_SECURE_BADPASSWORD,	"Wrong Password. (Passwords are case sensitive - be sure to use correct upper and lower case.)")

#define	ERR_SECURE_PROTOCOL_1			PKG_SECURE+9
	errortext(ERR_SECURE_PROTOCOL_1,	"Security protocol error #1")

#define ERR_SECURE_INVALIDCTX			PKG_SECURE+10
	errortext(ERR_SECURE_INVALIDCTX,	"Security context is invalid")

#define ERR_SECURE_INVRESPONSE			PKG_SECURE+11
	errortext(ERR_SECURE_INVRESPONSE,	"Remote user failed authentication")

#define ERR_SECURE_INVCERTKEY			PKG_SECURE+13
	errortext(ERR_SECURE_INVCERTKEY,	"ID's certificate is invalid")

#define ERR_SECURE_NOUSERINFO			PKG_SECURE+14
	errortext(ERR_SECURE_NOUSERINFO,	"The remote ID was never received")

#define ERR_SECURE_INVTICKET			PKG_SECURE+15
	errortext(ERR_SECURE_INVTICKET,		"Ticket is invalid")

#define ERR_SECURE_FRAUD				PKG_SECURE+16
	errortext(ERR_SECURE_FRAUD,			"Remote user's identity is fraudulent")

#define ERR_SECURE_UNAUTH				PKG_SECURE+17
	errortext(ERR_SECURE_UNAUTH,		"Authentication has not yet been attempted")

#define ERR_SECURE_FAILED_AUTH			PKG_SECURE+18
	errortext(ERR_SECURE_FAILED_AUTH,	"failed to authenticate")

#define ERR_SECURE_TOO_MANY_PWDS		PKG_SECURE+19
	errortext(ERR_SECURE_TOO_MANY_PWDS,	"You cannot require more passwords than there are")

#define ERR_SECURE_ZERO_PWDS			PKG_SECURE+20
	errortext(ERR_SECURE_ZERO_PWDS,		"You must require at least one password")

#define ERR_SECURE_MAX_PWDS				PKG_SECURE+21
	errortext(ERR_SECURE_MAX_PWDS,		"There cannot be more than eight passwords on an ID")

#define	ERR_SECURE_OLDAUTHVERSION		PKG_SECURE+22
	errortext(ERR_SECURE_OLDAUTHVERSION, "Remote authentication protocol version is no longer supported")


#define ERR_SECURE_NOTRUSTED_CLIENTCERT	PKG_SECURE+23
	errortext(ERR_SECURE_NOTRUSTED_CLIENTCERT,	"Your ID has not been certified by a certifier that is trusted by the server")
									
#define	ERR_SECURE_NOTRUSTED_SERVERCERT	PKG_SECURE+24
	errortext(ERR_SECURE_NOTRUSTED_SERVERCERT,	"The server's ID has not been certified by a certifier that you trust")


#define	ERR_SECURE_BADMSGDATA			PKG_SECURE+25
	errortext(ERR_SECURE_BADMSGDATA,	"Network error: message has been corrupted")

#define ERR_SECURE_ZERO_LENGTH_PW		PKG_SECURE+26
	errortext(ERR_SECURE_ZERO_LENGTH_PW,"You must type something; you may not have a zero length password.")

#define ERR_SECURE_SHORTMSG				PKG_SECURE+27
	errortext(ERR_SECURE_SHORTMSG,		"Network authentication message is too short")

#define	ERR_SECURE_EXPIRED_CERT			PKG_SECURE+28
	errortext(ERR_SECURE_EXPIRED_CERT,	"Your certificate has expired")

#define	ERR_SECURE_FUTUREAUTHVERSION	PKG_SECURE+29
	errortext(ERR_SECURE_FUTUREAUTHVERSION, "Remote authentication protocol version is not yet supported")

#define	ERR_SECURE_EXPIRED_UI			PKG_SECURE+30
	errortext(ERR_SECURE_EXPIRED_UI,	"Your User ID has expired")

#define	ERR_SECURE_EXPIRED_SERVERUI		PKG_SECURE+31
	errortext(ERR_SECURE_EXPIRED_SERVERUI,	"The Server's ID has expired")

#define ERR_SECURE_AUTHENTICATING		PKG_SECURE+32
	stringtext(ERR_SECURE_AUTHENTICATING, 	"Verifying your authorization to use server.")

#define ERR_SECURE_DISTINFO_CHANGE		PKG_SECURE+33
	stringtext(ERR_SECURE_DISTINFO_CHANGE, "Authentication error: the remote name, license, or key changed while authenticating")

#define ERR_SECURE_PW_NOT_CONFIRMED		PKG_SECURE+34
	errortext(ERR_SECURE_PW_NOT_CONFIRMED, "The password and its confirmation value are not the same")

#define	ERR_SECURE_PROTOCOL_2			PKG_SECURE+35
	errortext(ERR_SECURE_PROTOCOL_2,	"Security protocol error #2")
#define	ERR_SECURE_BADSESSKEY			PKG_SECURE+36
	errortext(ERR_SECURE_BADSESSKEY,	"An invalid session encryption key was received")
#define	ERR_SECURE_PROTOCOL_4			PKG_SECURE+37
	errortext(ERR_SECURE_PROTOCOL_4,	"Security protocol error #4")
#define	ERR_SECURE_PROTOCOL_5			PKG_SECURE+38
	errortext(ERR_SECURE_PROTOCOL_5,	"Security protocol error #5")
#define	ERR_SECURE_PROTOCOL_6			PKG_SECURE+39
	errortext(ERR_SECURE_PROTOCOL_6,	"Security protocol error #6")
#define	ERR_SECURE_UNKNOWN_MSG			PKG_SECURE+40
	errortext(ERR_SECURE_UNKNOWN_MSG,	"Unknown authentication message type was received")
#define	ERR_SECURE_PROTOCOL_8			PKG_SECURE+41
	errortext(ERR_SECURE_PROTOCOL_8,	"Security protocol error #8")
#define	ERR_SECURE_RCVUNEXPMSG			PKG_SECURE+42
	errortext(ERR_SECURE_RCVUNEXPMSG,	"Authentication message received out of sequence")
#define	ERR_SECURE_PROTOCOL_10			PKG_SECURE+43
	errortext(ERR_SECURE_PROTOCOL_10,	"Security protocol error #10")
#define	ERR_SECURE_PROTOCOL_11			PKG_SECURE+44
	errortext(ERR_SECURE_PROTOCOL_11,	"Security protocol error #11")

#define	ERR_SECURE_NOKEYMATCH			PKG_SECURE+50
	errortext(ERR_SECURE_NOKEYMATCH,	"Your public key does not match the one stored in the Address Book")
#define	ERR_SECURE_SERVER_EXPIRED_CERT	PKG_SECURE+51
	errortext(ERR_SECURE_SERVER_EXPIRED_CERT,	"The server's certificate has expired")
#define	ERR_SECURE_NO_SERVER_CROSS_CERT	PKG_SECURE+52
	errortext(ERR_SECURE_NO_SERVER_CROSS_CERT,	"The server's Address Book does not contain any cross certificates capable of authenticating you.")
#define	ERR_SECURE_NO_CLIENT_CROSS_CERT	PKG_SECURE+53
	errortext(ERR_SECURE_NO_CLIENT_CROSS_CERT,	"Your Address Book does not contain any cross certificates capable of authenticating the server.")
#define	ERR_SECURE_SERVER_ID_CORRUPT	PKG_SECURE+54
	errortext(ERR_SECURE_SERVER_ID_CORRUPT,	"The server's ID file has been corrupted, or is not an ID file.")
#define	ERR_SECURE_NO_PASSTHRU_CTX		PKG_SECURE+55
	errortext(ERR_SECURE_NO_PASSTHRU_CTX,	"A passthru message was received without the necessary context")
#define	ERR_SECURE_WRONG_STATE			PKG_SECURE+56
	errortext(ERR_SECURE_WRONG_STATE,	"The session is in the wrong state to accept that request")
#define ERR_NETBFR_PASSTHRU				PKG_SECURE+57
	internaltext(ERR_NETBFR_PASSTHRU,	"The session has been put into Passthru mode")
#define	ERR_SECURE_ILL_PASSTHRU_FCT		PKG_SECURE+58
	errortext(ERR_SECURE_ILL_PASSTHRU_FCT,	"Passthru function codes is not recognized")
#define ERR_PASSTHRU_NEEDED				PKG_SECURE+59
	errortext(ERR_PASSTHRU_NEEDED,		"Server is not directly reachable, a connection through an intermediate server is required")
#define	ERR_PASSTHRU_TOO_DEEP			PKG_SECURE+60
	errortext(ERR_PASSTHRU_TOO_DEEP,	"Server is too many hops away to establish a connection")
#define	ERR_LITE_TRANS_VIA_PT			PKG_SECURE+61
	errortext(ERR_LITE_TRANS_VIA_PT,	"Lite transaction attempted over passthru")

#define ERR_SECURE_BUG_2				PKG_SECURE+62
	errortext(ERR_SECURE_BUG_2,			"Security package software error #2")
#define ERR_SECURE_BUG_3				PKG_SECURE+63
	errortext(ERR_SECURE_BUG_3,			"Security package software error #3")
#define ERR_SECURE_BUG_4				PKG_SECURE+64
	errortext(ERR_SECURE_BUG_4,			"Security package software error #4")
#define ERR_SECURE_BUG_5				PKG_SECURE+65
	errortext(ERR_SECURE_BUG_5,			"Security package software error #5")
#define ERR_SECURE_BUG_6				PKG_SECURE+66
	errortext(ERR_SECURE_BUG_6,			"Security package software error #6")
#define ERR_SECURE_BUG_7				PKG_SECURE+67
	errortext(ERR_SECURE_BUG_7,			"Security package software error #7")
#define ERR_SECURE_BUG_8				PKG_SECURE+68
	errortext(ERR_SECURE_BUG_8,			"Security package software error #8")
#define ERR_SECURE_BUG_9				PKG_SECURE+69
	errortext(ERR_SECURE_BUG_9,			"Security package software error #9")

/*	Start again with PKG_SECURE+80 to leave room for expanding the _BUG codes */

#define	ERR_PASSTHRU_MORE_HOPS			PKG_SECURE+80
	errortext(ERR_PASSTHRU_MORE_HOPS,	"More hops to follow")
#define	ERR_PASSTHRU_FAILED				PKG_SECURE+81
	errortext(ERR_PASSTHRU_FAILED,		"Passthru connect to remote server failed")
#define ERR_SECURE_NON_PTSERVER			PKG_SECURE+82
	errortext(ERR_SECURE_NON_PTSERVER,	"The server just connected to is not a Passthru server")

/*	Passthru trace/log strings. */

#define	STR_PTLOG_CONNECTED_TO			PKG_SECURE+83
	stringtext(STR_PTLOG_CONNECTED_TO,	"Connected to")

#define ERR_SECURE_NT_SETPASSWORD		PKG_SECURE+84
	errortext(ERR_SECURE_NT_SETPASSWORD, "Unable to set your NT password successfully. You must manually change your NT password to match your new Notes password.")

#define STR_PTCONNECT_FOUND_ADDRESS		PKG_SECURE+85
	stringtext(STR_PTCONNECT_FOUND_ADDRESS,	"Obtained Address '%s' on %s from server record or other connection record")

#define STR_PTLOG_USE_PORT				PKG_SECURE+86
	stringtext(STR_PTLOG_USE_PORT,		"Connecting to %a over %s")
#define STR_PTLOG_ERROR					PKG_SECURE+87
	stringtext(STR_PTLOG_ERROR,			"Local Error:")
#define STR_PTLOG_PASSTHRU_TO			PKG_SECURE+88
	stringtext(STR_PTLOG_PASSTHRU_TO,	"Asking server for connection to")
#define STR_PTLOG_AUTHENTICATE_WITH		PKG_SECURE+89
	stringtext(STR_PTLOG_AUTHENTICATE_WITH,	"Authenticating with")
#define STR_PTLOG_MAX_BUF				PKG_SECURE+90
	stringtext(STR_PTLOG_MAX_BUF,		"Buffer size =")
#define STR_PTLOG_PATH_LENGTH			PKG_SECURE+91
	stringtext(STR_PTLOG_PATH_LENGTH,	"Path length = %u, including %u slow links")

#define STR_PTLOG_LEVEL_NONE			PKG_SECURE+92
	stringtext(STR_PTLOG_LEVEL_NONE,	"Nothing")
#define STR_PTLOG_USE_HOP				PKG_SECURE+93
	stringtext(STR_PTLOG_USE_HOP,		"Add server")


#define STR_PTLOG_REMOTE_PORT			PKG_SECURE+94
	stringtext(STR_PTLOG_REMOTE_PORT,	"Port to be used on passthru server is")
#define STR_PTLOG_LEVEL_ERROR			PKG_SECURE+95
	stringtext(STR_PTLOG_LEVEL_ERROR,	"Errors only")
#define STR_PTLOG_LEVEL_SUMMARY			PKG_SECURE+96
	stringtext(STR_PTLOG_LEVEL_SUMMARY,	"Summary progress information")
#define STR_PTLOG_LEVEL_DETAIL			PKG_SECURE+97
	stringtext(STR_PTLOG_LEVEL_DETAIL,	"Detailed progress information")
#define STR_PTLOG_LEVEL_MISC			PKG_SECURE+98
	stringtext(STR_PTLOG_LEVEL_MISC,	"Full trace information")
#define STR_PTLOG_LOG_INFO				PKG_SECURE+99
	stringtext(STR_PTLOG_LOG_INFO,		"Network: %s")
#define STR_PTLOG_LOG_SESSION			PKG_SECURE+100
	stringtext(STR_PTLOG_LOG_SESSION,	"Network session %s: %s")
#define STR_PTLOG_REMOTE_ERROR			PKG_SECURE+101
	stringtext(STR_PTLOG_REMOTE_ERROR,	"Passthru server error:")
#define STR_PTLOG_OCCURRED_ON			PKG_SECURE+102
	stringtext(STR_PTLOG_OCCURRED_ON,	"Problem detected on server")
#define STR_PTLOG_USE_HOP_SUFFIX		PKG_SECURE+103
	stringtext(STR_PTLOG_USE_HOP_SUFFIX, "to the path")
#define STR_PTLOG_PTNETCONNECT			PKG_SECURE+104
	stringtext(STR_PTLOG_PTNETCONNECT,	"On passthru server, connect to")
#define STR_PTLOG_PASSTHRU_SERVER		PKG_SECURE+105
	stringtext(STR_PTLOG_PASSTHRU_SERVER, "Using passthru server: ")

/* 106 to 107 unused */

#define STR_SECURE_BLANK					PKG_SECURE+108
	stringtext(STR_SECURE_BLANK, 			" ")
#define STR_CONNREC_TYPE_CLUSTER			PKG_SECURE+109
	stringtext(STR_CONNREC_TYPE_CLUSTER, 	" cluster ")
#define STR_CONNREC_TYPE_REMOTE				PKG_SECURE+110
	stringtext(STR_CONNREC_TYPE_REMOTE, 	"Modem")
#define STR_CONNREC_TYPE_NETWORK			PKG_SECURE+111
	stringtext(STR_CONNREC_TYPE_NETWORK, 	"Local network")
#define STR_CONNREC_TYPE_PASSTHRU			PKG_SECURE+112
	stringtext(STR_CONNREC_TYPE_PASSTHRU, 	"Passthru")
#define STR_CONNREC_TYPE_REMOTE_LAN			PKG_SECURE+113
	stringtext(STR_CONNREC_TYPE_REMOTE_LAN, "Remote LAN")
#define STR_CONNREC_TYPE_X25				PKG_SECURE+114
	stringtext(STR_CONNREC_TYPE_X25, 		"X.25")
#define STR_CONNREC_TYPE_HUNT_GROUP			PKG_SECURE+115
	stringtext(STR_CONNREC_TYPE_HUNT_GROUP, "Hunt group")

#define	ERR_SECURE_MULTI_PSW_PROMPT			(PKG_SECURE+116)
	errortext(ERR_SECURE_MULTI_PSW_PROMPT,	
		"Enter one of the multiple passwords (press the Esc key to abort): ")
#define	ERR_SECURE_MULTI_PSW_REQUIRED		(PKG_SECURE+117)
	errortext(ERR_SECURE_MULTI_PSW_REQUIRED,	
		"This ID file requires multiple passwords.")

/* 118 to 119 unused */

#define ERR_PT_WILDCARD_DST				PKG_SECURE+120
	errortext(ERR_PT_WILDCARD_DST,		"Illegal destination server name")
#define ERR_PROTERR_NO_AUTHENTICATION	PKG_SECURE+121
	errortext(ERR_PROTERR_NO_AUTHENTICATION, "Protocol error: Network authentication was never attempted")


#define ERR_SECURE_NOT_WITHIN_TIMERANGE	PKG_SECURE+122
	errortext(ERR_SECURE_NOT_WITHIN_TIMERANGE, "Object validity dates not within specified time range")
#define ERR_SECURE_FLAT_PUBLICINFO		PKG_SECURE+123
	errortext(ERR_SECURE_FLAT_PUBLICINFO, "You must first upgrade this entry to contain a hierarchical name before you can recertify it.")
#define ERR_NO_PATH_FROM_CONN_REC		PKG_SECURE+124
	errortext(ERR_NO_PATH_FROM_CONN_REC, "A path to the server could not be determined from the Connection records in the Name and Address Book")
#define ERR_PASSTHRU_NOT_SUPPORTED		PKG_SECURE+125
	errortext(ERR_PASSTHRU_NOT_SUPPORTED, "You have attempted to pass through a server that does not support passthru")
#define ERR_NO_PASSTHRU_BAD_VERSION		PKG_SECURE+126
	errortext(ERR_NO_PASSTHRU_BAD_VERSION, "You are running with a version of the passthru protocol which is no longer supported")
#define ERR_JSPATH_POOLFULL				PKG_SECURE+127
	errortext(ERR_JSPATH_POOLFULL, 		"Insufficient memory - search path pool is full.")

#define ERR_SECURE_UNEXPMSG				PKG_SECURE+128
	errortext(ERR_SECURE_UNEXPMSG,		"An unexpected authentication message was received")
#define ERR_SECURE_NEWNAME_QUERY		PKG_SECURE+129
	errortext(ERR_SECURE_NEWNAME_QUERY,	"Would you like to update your current name to the following:")
#define ERR_SECURE_NEWNAME_REFUSED		PKG_SECURE+130
	errortext(ERR_SECURE_NEWNAME_REFUSED, "The remote client refused the name update")
#define ERR_SECURE_WRONG_CHALLENGE		PKG_SECURE+131
	errortext(ERR_SECURE_WRONG_CHALLENGE, "The remote system failed to prove its identity")
#define ERR_SECURE_UNKNOWN_PROXY_FCT	PKG_SECURE+132
	errortext(ERR_SECURE_UNKNOWN_PROXY_FCT, "Internal error: request to make an unknown Proxy Database entry")

#define ERR_SECURE_MISSING_ARG			PKG_SECURE+133
	errortext(ERR_SECURE_MISSING_ARG,	"Internal error: a required parameter in a subroutine call has not been supplied")
#define ERR_SECURE_CHANGE_WRONG_NEW_NAME	PKG_SECURE+134
	errortext(ERR_SECURE_CHANGE_WRONG_NEW_NAME, "The new name in the supplied Change Request does not match the subject name in the supplied certificate")
#define ERR_SECURE_CROSS_CERT_NOT_ALLOWED	PKG_SECURE+135
	errortext(ERR_SECURE_CROSS_CERT_NOT_ALLOWED, "The supplied certificate is a cross certificate; a certificate issued by an ancestor of the subject name is required")
#define ERR_SECURE_CHANGE_CERT_ISSUER_MISMATCH	PKG_SECURE+136
	errortext(ERR_SECURE_CHANGE_CERT_ISSUER_MISMATCH, "The name of the authority issuing the Change Request does not match the certifier name in the supplied certificate")
#define ERR_SECURE_ILL_CHANGE_ISSUER	PKG_SECURE+137
	errortext(ERR_SECURE_ILL_CHANGE_ISSUER, "The name of the authority issuing the Change Request is not allowed to make that request.")
#define ERR_SECURE_UNKNOWN_CHANGE_TYPE	PKG_SECURE+138
	errortext(ERR_SECURE_UNKNOWN_CHANGE_TYPE, "The Change Request is of an unknown type")
#define ERR_SECURE_INVALID_CHANGE_SIG	PKG_SECURE+139
	errortext(ERR_SECURE_INVALID_CHANGE_SIG, "The signature on the Change Request was found to be invalid")
#define ERR_SECURE_INVALID_CERT_SIG		PKG_SECURE+140																 
	errortext(ERR_SECURE_INVALID_CERT_SIG, "The signature in one of the supplied certificates was found to be invalid")
#define ERR_SECURE_CHANGE_CERT_KEY_MISMATCH	PKG_SECURE+141
	errortext(ERR_SECURE_CHANGE_CERT_KEY_MISMATCH, "The public keys specified in the Change Request do not match those specified in the new certificate")
#define ERR_SECURE_STALE_ABPI			PKG_SECURE+142
	errortext(ERR_SECURE_STALE_ABPI,	"The information in the supplied certificates from the Address Book entry is out of date")
#define ERR_SECURE_UPGRADE_ONLY			PKG_SECURE+143
	errortext(ERR_SECURE_UPGRADE_ONLY,	"The Change Request specifies a name or key change, but the change allowed is an update from a flat to a hierarchical name")
#define ERR_SECURE_CHANGE_MUST_BE_HI	PKG_SECURE+144
	errortext(ERR_SECURE_CHANGE_MUST_BE_HI, "The new name specified in the Change Request is not a hierarchical name")
#define ERR_SECURE_UPGRADE_NAME_MISMATCH	PKG_SECURE+145
	errortext(ERR_SECURE_UPGRADE_NAME_MISMATCH, "When upgrading an name from flat to hierarchical, the flat name must match the first component in the hierarchical name")
#define ERR_SECURE_CHANGE_ILL_MOVE_NAMES	PKG_SECURE+146
	errortext(ERR_SECURE_CHANGE_ILL_MOVE_NAMES, "When moving a name in the name hierarchicy, the first component of the old and new names must be the same")
#define ERR_SECURE_INCONSIST_CHANGE_REQ	PKG_SECURE+147
	errortext(ERR_SECURE_INCONSIST_CHANGE_REQ, "The Change Request contains inconsistent or incomplete information")
#define ERR_SECURE_CHANGE_UNKNOWN_FIELDS	PKG_SECURE+148
	errortext(ERR_SECURE_CHANGE_UNKNOWN_FIELDS, "One or more of the fields in the Change Request could not be interperted")
#define ERR_SECURE_CHANGE_WRONG_OLD_NAME	PKG_SECURE+149
	errortext(ERR_SECURE_CHANGE_WRONG_OLD_NAME, "The old name in the Change Request does not match the name trying to be changed")
#define ERR_SECURE_NO_CHANGE_REQ			PKG_SECURE+150
	errortext(ERR_SECURE_NO_CHANGE_REQ,		"Authentication protocol error: no Change Request was sent to the client")
#define ERR_SECURE_NEWCERTS_QUERY		PKG_SECURE+151
	errortext(ERR_SECURE_NEWCERTS_QUERY,	"The hierarchical certificates in your ID file have been updated with new expiration dates")
#define ERR_SECURE_NOKEY_IN_AB			PKG_SECURE+152
	errortext(ERR_SECURE_NOKEY_IN_AB,	"There is no appropriate key or certificate stored in the Address Book for this entry")
#define ERR_SECURE_WRONG_CHANGE_SIG		PKG_SECURE+153
	errortext(ERR_SECURE_WRONG_CHANGE_SIG,	"The signatures in the Change Request and Change Signature items did not match")
#define ERR_SECURE_MISSING_CHANGE_SIG	PKG_SECURE+154
	errortext(ERR_SECURE_MISSING_CHANGE_SIG, "The change request document does not contain a Change Signature item")
#define ERR_SECURE_PW_YOU_MUST_BE_HI		PKG_SECURE+155
	errortext(ERR_SECURE_PW_YOU_MUST_BE_HI, "You must be running with a hierarchical name to access this server")
#define ERR_SECURE_PW_SUPPORT_VERS		PKG_SECURE+156
	errortext(ERR_SECURE_PW_SUPPORT_VERS, "The server requires that you be running Notes version 4.5 or greater in order to test your password")
#define ERR_SECURE_CERT_NOT_NEWER		PKG_SECURE+157
	errortext(ERR_SECURE_CERT_NOT_NEWER, "Did not accept the new certificates becuase they were not issued after the current certificates")


/* PT connection trace strings */

#define STR_PTCONNECT_START_SEARCH			PKG_SECURE+160
	stringtext(STR_PTCONNECT_START_SEARCH, "Determining path to server %A")

#define STR_PTCONNECT_OPENING_DB			PKG_SECURE+161
	stringtext(STR_PTCONNECT_OPENING_DB, 	"Opening Database")

#define STR_PTCONNECT_CHECK_SAVED_ADDRESS PKG_SECURE+162
	stringtext(STR_PTCONNECT_CHECK_SAVED_ADDRESS, "Checking for %A at last known address '%s' on %s...")

#define STR_PTCONNECT_SEARCHING_PREFERRED_CONNRECS PKG_SECURE+163
	stringtext(STR_PTCONNECT_SEARCHING_PREFERRED_CONNRECS, "Checking normal priority connection records only...")
#define STR_PTCONNECT_SEARCHING_ALL_CONNRECS PKG_SECURE+164
	stringtext(STR_PTCONNECT_SEARCHING_ALL_CONNRECS, "Checking low and normal priority connection records...")

#define STR_PTCONNECT_PT_SEARCH_FOR			PKG_SECURE+165
	stringtext(STR_PTCONNECT_PT_SEARCH_FOR, "%sSearching for path to %A")

#define STR_PTCONNECT_NO_PATH_FOUND			PKG_SECURE+166
	stringtext(STR_PTCONNECT_NO_PATH_FOUND, "%sNo path found to %A")
#define STR_PTCONNECT_CONNREC_FOUND			PKG_SECURE+167
	stringtext(STR_PTCONNECT_CONNREC_FOUND, "%s%e%econnection record found for %A")

#define STR_PTCONNECT_FOUND_LAN_TABLE		PKG_SECURE+168
	stringtext(STR_PTCONNECT_FOUND_LAN_TABLE, "%sAddress found in local %s names table for %A")
#define STR_PTCONNECT_WAN_PORT_IN_USE		PKG_SECURE+169
	stringtext(STR_PTCONNECT_WAN_PORT_IN_USE, "%sPort %s is already connected to %A")
#define STR_PTCONNECT_NAME_SERVICE			PKG_SECURE+170
	stringtext(STR_PTCONNECT_NAME_SERVICE, "%sRequesting address of %A from %A on %s")
#define STR_PTCONNECT_FOUND_FROM_NS			PKG_SECURE+171
	stringtext(STR_PTCONNECT_FOUND_FROM_NS, "%sAddress '%s' returned for %A on %s")

#define STR_PTCONNECT_PROBE					PKG_SECURE+172
	stringtext(STR_PTCONNECT_PROBE,			"%sChecking for %A on %s using address '%s'")
#define STR_PTCONNECT_FOUND_BY_PROBE		PKG_SECURE+173
	stringtext(STR_PTCONNECT_FOUND_BY_PROBE,"%s%A is available on %s")

#define STR_PTCONNECT_USE_DEFAULT_PT		PKG_SECURE+174
	stringtext(STR_PTCONNECT_USE_DEFAULT_PT,"The default passthru server is %A")

#define STR_PTCONNECT_NO_DEFAULT_PT			PKG_SECURE+175
	stringtext(STR_PTCONNECT_NO_DEFAULT_PT, "No default passthru server defined")

#define STR_PTCONNECT_TRY_CONNECTED_WAN		PKG_SECURE+176
	stringtext(STR_PTCONNECT_TRY_CONNECTED_WAN, "%A is connected on %s; Using it as passthru server")

#define STR_PTCONNECT_TARGET_NS_NOT_RESPONDING	PKG_SECURE+177
	stringtext(STR_PTCONNECT_TARGET_NS_NOT_RESPONDING, 	"Server %A is not responding")

#define STR_PTCONNECT_JSGETPATH_FAILED		PKG_SECURE+178
	stringtext(STR_PTCONNECT_JSGETPATH_FAILED, 	"Unable to find any path to %A")

#define STR_PTCONNECT_FB_NAME_SERVICE			PKG_SECURE+179
	stringtext(STR_PTCONNECT_FB_NAME_SERVICE, "%sRequesting address of %A from secondary name server %A on %s")

#define STR_PTCONNECT_PASS_THROUGH			PKG_SECURE+180
	stringtext(STR_PTCONNECT_PASS_THROUGH, "Pass through %A to connect to %A")

#define STR_PTCONNECT_CHECK_ADDRESS_IN_NAME PKG_SECURE+181
	stringtext(STR_PTCONNECT_CHECK_ADDRESS_IN_NAME, "Checking for address in server name ...")

#define STR_PTCONNECT_ENABLE_NS_REQS		PKG_SECURE+182
	stringtext(STR_PTCONNECT_ENABLE_NS_REQS, "Enabling name service requests and probes...")

#define STR_PTCONNECT_ENABLE_WILD_CARDS		PKG_SECURE+183
	stringtext(STR_PTCONNECT_ENABLE_WILD_CARDS, "Allowing wild card connection records...")

#define STR_PTCONNECT_PT_CONNREC_FOUND		PKG_SECURE+184
	stringtext(STR_PTCONNECT_PT_CONNREC_FOUND, "%s%e%econnection record found for %A via %A")

#define STR_PTCONNECT_ADDRESS				PKG_SECURE+185
	stringtext(STR_PTCONNECT_ADDRESS, 		"Using address '%A' on %s")

#define STR_PTCONNECT_CONNECTION_FAILED		PKG_SECURE+186
	stringtext(STR_PTCONNECT_CONNECTION_FAILED, "Unable to connect to %A on %s (%e)")

#define STR_PTCONNECT_REMOTE_LAN			PKG_SECURE+187
	stringtext(STR_PTCONNECT_REMOTE_LAN, 	"Establishing %s link to %A")

#define STR_PTCONNECT_NAME_NOT_FOUND		PKG_SECURE+188
	stringtext(STR_PTCONNECT_NAME_NOT_FOUND, "%s%A has no address for %a on %s")

#define STR_PTCONNECT_TRACE_CONNECTED		PKG_SECURE+189
	stringtext(STR_PTCONNECT_TRACE_CONNECTED, "Connected to server %A")

#define STR_SECURE_RENAME_CMD_UNKNOWN		PKG_SECURE+190
	stringtext(STR_SECURE_RENAME_CMD_UNKNOWN, "Unknown")
#define STR_SECURE_RENAME_CMD_UPGRADE		PKG_SECURE+191
	stringtext(STR_SECURE_RENAME_CMD_UPGRADE, "Upgrade flat name to hierarchical")
#define STR_SECURE_RENAME_CMD_RENEW			PKG_SECURE+192
	stringtext(STR_SECURE_RENAME_CMD_RENEW,	"Renew certificate")
#define STR_SECURE_RENAME_CMD_RENAME		PKG_SECURE+193
	stringtext(STR_SECURE_RENAME_CMD_RENAME, "Rename")
#define STR_SECURE_RENAME_CMD_MOVE_REQ		PKG_SECURE+194
	stringtext(STR_SECURE_RENAME_CMD_MOVE_REQ, "Request that name be moved to a new certifier")
#define STR_SECURE_RENAME_CMD_MOVE_COMP		PKG_SECURE+195
	stringtext(STR_SECURE_RENAME_CMD_MOVE_COMP, "Complete the move of a name to a new certifier")

#define ERR_SECURE_WRONG_NEWABPI			PKG_SECURE+196
	errortext(ERR_SECURE_WRONG_NEWABPI,		"The certificate stored in the change request entry is not for the key specified in the change request")
#define ERR_SECURE_WRONG_OLDABPI			PKG_SECURE+197
	errortext(ERR_SECURE_WRONG_OLDABPI,		"The keys stored in the Address Book record for this entry are not the same as the keys specified in the change request")

#define ERR_SECURE_CERTLOG_ERROR			PKG_SECURE+200
	errortext(ERR_SECURE_CERTLOG_ERROR,		"Error occurred attempting to log to the Certifier Log")

#define ERR_SECURE_NAME_ALREADY_HI			PKG_SECURE+201
	errortext(ERR_SECURE_NAME_ALREADY_HI,	"The name cannot be upgraded to hierarchical, it is already hierarchical")
#define ERR_SECURE_WRONG_CERTIFIER			PKG_SECURE+202
	errortext(ERR_SECURE_WRONG_CERTIFIER,	"The Certificate contained in the note was not issued by the selected certifier")
#define ERR_SECURE_CERT_CHANGEREQ_MISMATCH	PKG_SECURE+203
	errortext(ERR_SECURE_CERT_CHANGEREQ_MISMATCH, "The name in the Change Request field does not match the name in the Certificate field")
#define ERR_SECURE_UPDATE_NOT_ANCESTOR		PKG_SECURE+204
	errortext(ERR_SECURE_UPDATE_NOT_ANCESTOR, "The selected certifier is not an ancestor of the entity to be updated")

#define ERR_SECURE_MOVE_REQ_NOT_ANCESTOR	PKG_SECURE+205
	errortext(ERR_SECURE_MOVE_REQ_NOT_ANCESTOR, "The move request was not initiated by one of the named entity's ancestors")
#define ERR_SECURE_MOVE_REQ_NOT_TARGET		PKG_SECURE+206
	errortext(ERR_SECURE_MOVE_REQ_NOT_TARGET, "The selected certifier is not the target certifier mentioned in the move request")

#define ERR_SECURE_INVALID_MOVE_REQUEST		PKG_SECURE+207
	errortext(ERR_SECURE_INVALID_MOVE_REQUEST, "The note does not contain a valid move request")
#define ERR_SECURE_NONAME_IN_NOTE			PKG_SECURE+208
	errortext(ERR_SECURE_NONAME_IN_NOTE,	"The note does not contain a name")
#define ERR_SECURE_NOPUBKEY_IN_NOTE			PKG_SECURE+209
	errortext(ERR_SECURE_NOPUBKEY_IN_NOTE,	"The note does not contain either a public key or a certificate")
#define ERR_SECURE_NOCHANGEREQ_IN_NOTE		PKG_SECURE+210
	errortext(ERR_SECURE_NOCHANGEREQ_IN_NOTE, "The note does not contain a Change Request field")
#define STR_SECURE_VIEWRENAME_STATS			PKG_SECURE+211

/*	NOTE TO TRANSLATORS:

	Thats a lower case "L", not the numeral one, in the "%8lu" references in the next string.

*/
#ifdef PM
	stringtext(STR_SECURE_VIEWRENAME_STATS, "\xd%8lu Entries Processed\xd%8lu Entries Succeeded\xd%8lu Entries Failed\xd%8lu Entries Skipped\xd\xd%s")
#else
	stringtext(STR_SECURE_VIEWRENAME_STATS, "\n%8lu Entries Processed\n%8lu Entries Succeeded\n%8lu Entries Failed\n%8lu Entries Skipped\n\n%s")
#endif

#define STR_SECURE_VIEWRENAME_LOGREF		PKG_SECURE+212
	stringtext(STR_SECURE_VIEWRENAME_LOGREF, "See the Certifier Log for details")


#define STR_PTCONNECT_NS_CONNECT_ERROR		PKG_SECURE+213
	stringtext(STR_PTCONNECT_NS_CONNECT_ERROR, "%sUnable to connect to %A: %e")

#define STR_PTCONNECT_PT_CONNECTION_FAILED	PKG_SECURE+214
	stringtext(STR_PTCONNECT_PT_CONNECTION_FAILED, "Attempt to reach %A by passthru failed (%e) ")


#define STR_PTCONNECT_ERROR_FROM_NS		PKG_SECURE+215
	stringtext(STR_PTCONNECT_ERROR_FROM_NS, "%sStatus on %A: %e")

#define ERR_SECURE_INV_ACTION_ITEM		PKG_SECURE+216
	errortext(ERR_SECURE_INV_ACTION_ITEM,	"The request note's required ACTION field is missing or is improperly formed")
#define ERR_SECURE_INV_CERT_ITEM		PKG_SECURE+217
	errortext(ERR_SECURE_INV_CERT_ITEM,	"The request note's required CERTIFICATE field is missing or is improperly formed")
#define ERR_SECURE_INV_CHANGE_ITEM		PKG_SECURE+218
	errortext(ERR_SECURE_INV_CHANGE_ITEM,	"The request note's required CHANGE REQUEST field is missing or is improperly formed")

#define ERR_SECURE_MULTIPLEMAPPINGS		PKG_SECURE+219
	errortext(ERR_SECURE_MULTIPLEMAPPINGS, "The port mapper does not allow multiple hops.")

#define ERR_SECURE_RCVMSG_TYPE			PKG_SECURE+220
	errortext(ERR_SECURE_RCVMSG_TYPE,	"Error in received message type %d (decimal)")

#define STR_SECURE_FORMAT_DATABASE_INFO	PKG_SECURE+221
	stringtext(STR_SECURE_FORMAT_DATABASE_INFO,  "Database Title: %s File name: %s")

#define ERR_SECURE_UPDATE_DB_ERR		PKG_SECURE+222
	errortext(ERR_SECURE_UPDATE_DB_ERR,	"Error changing our name in database: %s (File name: %s)")

#define ERR_SECURE_UPDATE_DB_OK			PKG_SECURE+223
	errortext(ERR_SECURE_UPDATE_DB_OK,	"Successfully changed our name in database: %s (File name: %s)")

#define ERR_SECURE_CHECK_ID_UPDATE		PKG_SECURE+224
	errortext(ERR_SECURE_CHECK_ID_UPDATE, "Error updating local ID file")

#define STR_DESK_NAME_UPD_CAPTION			PKG_SECURE+225
	stringtext(STR_DESK_NAME_UPD_CAPTION, "Name update request found on the server")

#define  STR_PTCONNECT_SAVED_ADDRESS_NOT_FOUND 	PKG_SECURE+226
	stringtext(STR_PTCONNECT_SAVED_ADDRESS_NOT_FOUND, "No answer at last known address")

#define STR_PTCONNECT_IMBEDDED_ADDRESS_FOUND PKG_SECURE+227
	stringtext(STR_PTCONNECT_IMBEDDED_ADDRESS_FOUND, "Server name matches valid address format for %s")

#define STR_PTCONNECT_NAME_NOT_VERIFIED		PKG_SECURE+228
	stringtext(STR_PTCONNECT_NAME_NOT_VERIFIED, "%s No answer on address '%s' from %A on %s")

#define STR_PTCONNECT_LINKED_ELSEWHERE		PKG_SECURE+229
	stringtext(STR_PTCONNECT_LINKED_ELSEWHERE, "%sModem%econnection record found for %A, but all WAN ports are in use")

#define STR_QSFAIL_SEARCHING				PKG_SECURE+230
#ifdef MAC
	errortext(STR_QSFAIL_SEARCHING, 		"Performing extended search... (<Command>-period to Cancel)")
#else
	errortext(STR_QSFAIL_SEARCHING, 		"Performing extended search... (CTRL+BREAK to cancel)")
#endif

#define STR_PTCONNECT_SEARCH_MSG			PKG_SECURE+231
	stringtext(STR_PTCONNECT_SEARCH_MSG, 	"Unable to detect server %A at last-used address.%nContinue searching?")

#define STR_PTCONNECT_SKIP_EXTENDED_SEARCH	PKG_SECURE+232
	stringtext(STR_PTCONNECT_SKIP_EXTENDED_SEARCH, 	"Bypassing extended search")

#define STR_PTCONNECT_PTSERVER_FAILOVER		PKG_SECURE+233
	stringtext(STR_PTCONNECT_PTSERVER_FAILOVER, "Passthru server failed-over to %A")

#define STR_PTCONNECT_SERVER_CONNECTED_OVER_WAN		PKG_SECURE+234
	stringtext(STR_PTCONNECT_SERVER_CONNECTED_OVER_WAN, "Already connected to server on port %s")

#define STR_PTCONNECT_VERIFY_ADDRESS		PKG_SECURE+235
	stringtext(STR_PTCONNECT_VERIFY_ADDRESS,	"%sVerifying address '%s' for %A on %s")

#define ERR_SECURE_PASS_NOT_SAME			PKG_SECURE+236
	errortext(ERR_SECURE_PASS_NOT_SAME, "Your NT account password does not match your Notes password. To synchronize the passwords, use File - Tools - User ID and click Set Password.")

#define ERR_SECURE_NT_RELOGON				PKG_SECURE+237
	errortext(ERR_SECURE_NT_RELOGON, "Your new NT password is set. To complete the synchronization, you must log off and back into your NT account. Do you want to log off now?")

#define ERR_SECURE_UNSYNCED_PASSWORD		PKG_SECURE+238
	errortext(ERR_SECURE_UNSYNCED_PASSWORD, "You have a different password on another copy of your ID file and you must change the password on this copy to match")

#define ERR_SECURE_NAME_IN_SIGNED_DOCS		PKG_SECURE+239
	errortext(ERR_SECURE_NAME_IN_SIGNED_DOCS, "The name %s could not be changed to %s in the Readers and Authors fields of the signed and encrypted documents in %s.")

#define ERR_SECURE_NO_MULTIPW				PKG_SECURE+240
	errortext(ERR_SECURE_NO_MULTIPW, 		"The server you are trying to access requires that your ID file have a single password")

#define ERR_SECURE_PWD_TO_EXPIRE			PKG_SECURE+241
	errortext(ERR_SECURE_PWD_TO_EXPIRE, 	"WARNING: Your password will expire on ")

#define ERR_SECURE_PWD_EXPIRED				PKG_SECURE+242
	errortext(ERR_SECURE_PWD_EXPIRED,		"You must change your password. It expired on ")

#define ERR_SECURE_PWD_REALLY_EXPIRED		PKG_SECURE+243
	errortext(ERR_SECURE_PWD_REALLY_EXPIRED,"Your password expired and your account is locked out; see your system administrator to reset it")

#define ERR_SECURE_PWD_EXPIRED_AUTH			PKG_SECURE+244
	errortext(ERR_SECURE_PWD_EXPIRED_AUTH, "Your password has expired. You cannot authenticate to this server until you change it")

#define ERR_SECURE_PASSWORD_REQUIRED		PKG_SECURE+245
	errortext(ERR_SECURE_PASSWORD_REQUIRED, "The server you are trying to access requires that your ID file have a password")

#define ERR_SECURE_BAD_TEMPLATE_KEY			PKG_SECURE+246
	errortext(ERR_SECURE_BAD_TEMPLATE_KEY,	"The version of the Notes you are running does not recognise the Template Development key that signed this document")

#define STR_SECURE_VIEWRENAME_ELOGREF		PKG_SECURE+247
	stringtext(STR_SECURE_VIEWRENAME_ELOGREF, "See the Certifier Log for details on failures.")

#define ERR_SECURE_FLAT_PASSWORD			PKG_SECURE+248
	errortext(ERR_SECURE_FLAT_PASSWORD, 	"The server you are trying to access requires password enhanced authentication, which is not possible with a non-hierarchical ID. Tell your system administrator to either upgrade your ID or remove the requirement on the server.")

#define ERR_SECURE_PASSWORD_REUSE			PKG_SECURE+249
	errortext(ERR_SECURE_PASSWORD_REUSE, 	"You have used this password before. Please choose a new one")

#define ERR_SECURE_PW_MISMATCH				PKG_SECURE+250
	errortext(ERR_SECURE_PW_MISMATCH,		"Your password does not match the one stored in your entry in the server's Address Book.")

#define ERR_SECURE_AUTH_LOCKOUT				PKG_SECURE+251
	errortext(ERR_SECURE_AUTH_LOCKOUT,		"Your entry in the server's Address Book specifies that authentication is not allowed.")

#define ERR_SECURE_PW_EXPIRED				PKG_SECURE+252
	errortext(ERR_SECURE_PW_EXPIRED,		"Your password has expired.")

#define ERR_SECURE_NEW_PW_REPEAT			PKG_SECURE+253
	errortext(ERR_SECURE_NEW_PW_REPEAT,		"The new password could not be accepted because it is a repeat of a former password")

#define ERR_SECURE_NEW_PW_TOO_OLD			PKG_SECURE+254
	errortext(ERR_SECURE_NEW_PW_TOO_OLD,	"The new password could not be accepted because it is older than the current password")

#define ERR_SECURE_CHANGE_SIG_INVALID		PKG_SECURE+255
	errortext(ERR_SECURE_CHANGE_SIG_INVALID, "The signature in the Change Signature field was found to be invalid")








#endif
