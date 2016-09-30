// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.

#ifndef CLIENT_ERR_DEFS
#define CLIENT_ERR_DEFS

/* CLIENT access routines error code */

#define ERR_RENAME						PKG_CLIENT+1
	errortext(ERR_RENAME,				"Cannot rename a file to a new server")
#define ERR_VERSION						PKG_CLIENT+2
	errortext(ERR_VERSION,				"Server does not support this version of the network protocol")
#define ERR_SERVER_NOT_FOUND			PKG_CLIENT+3
	errortext(ERR_SERVER_NOT_FOUND,		"Unable to find path to server")
#define ERR_SERVER_NAME_CHANGED			PKG_CLIENT+4
	errortext(ERR_SERVER_NAME_CHANGED,	"Connection denied. The server you connected to has a different name from the one requested.")
#define ERR_NOT_IMPLEMENTED				PKG_CLIENT+5
	errortext(ERR_NOT_IMPLEMENTED,		"Function not implemented")
#define ERR_MULTI_THREAD				PKG_CLIENT+6
	apitext(ERR_MULTI_THREAD,			"A database handle to a remote database cannot be used by more than one thread.")
#define ERR_SERVER_NOT_RESPONDING		PKG_CLIENT+7
	errortext(ERR_SERVER_NOT_RESPONDING, "Server not responding")
#define ERR_RETRY_NOTE_UPDATE			PKG_CLIENT+8
	internaltext(ERR_RETRY_NOTE_UPDATE,	"(Retry NoteUpdate)")
#define ERR_RETRY_TRANSACTION			PKG_CLIENT+9
	internaltext(ERR_RETRY_TRANSACTION,	"(internal status; retry the transaction)")
#define ERR_MSG_TOO_SMALL				PKG_CLIENT+10
	errortext(ERR_MSG_TOO_SMALL,		"Network protocol error: message from server is too small")
#define ERR_MSG_FROM_SERVER				PKG_CLIENT+11
	errortext(ERR_MSG_FROM_SERVER,		"Network protocol error: message from server cannot be deciphered")
#define ERR_SERVER_NOT_RESPONDING1		PKG_CLIENT+12
	errortext(ERR_SERVER_NOT_RESPONDING1, "Server ")
#define ERR_SERVER_NOT_RESPONDING2		PKG_CLIENT+13
	stringtext(ERR_SERVER_NOT_RESPONDING2, " is not responding")
#define ERR_NOT_AUTHENTIC				PKG_CLIENT+14
	errortext(ERR_NOT_AUTHENTIC,		"Remote system's identity is either unknown or fraudulent")
#define ERR_SCAN_LOCK_LOST				PKG_CLIENT+15
	errortext(ERR_SCAN_LOCK_LOST,		"Session to server lost while database replication or copy in progress")
#define ERR_MULTI_TRANS					PKG_CLIENT+16
	errortext(ERR_MULTI_TRANS,			"In order to do multiple transactions simultaneously, you cannot use the same DB handle.")
#define ERR_SERVER_NOT_RESPONDING3		PKG_CLIENT+17
	errortext(ERR_SERVER_NOT_RESPONDING3, "Name server is not responding")
#define ERR_CL_NO_REMOTE_CONSOLE_ACCESS PKG_CLIENT+18
	errortext(ERR_CL_NO_REMOTE_CONSOLE_ACCESS,"You are not authorized to use the remote console on this server")
#define ERR_CL_NO_RESPONSE 				PKG_CLIENT+19
	errortext(ERR_CL_NO_RESPONSE,		"No response from server for this command")
#define ERR_TERMINATE					PKG_CLIENT+20
	errortext(ERR_TERMINATE,			"Session Closed")
#define ERR_CL_NOT_RESPONDING 	  		PKG_CLIENT+21
	errortext(ERR_CL_NOT_RESPONDING,	"Server is not responding to remote console commands (try again later)")
#define ERR_NEED_RECEIVE_DATA			PKG_CLIENT+22
	internaltext(ERR_NEED_RECEIVE_DATA,	"(Out of received data, need more)")
#define ERR_SRV_LENGTH_MISMATCH			PKG_CLIENT+23
	errortext(ERR_SRV_LENGTH_MISMATCH,	"Multi-Segment ID table length from server is not the length expected")
#define ERR_LOG_NOFAILOVER				PKG_CLIENT+24
	errortext(ERR_LOG_NOFAILOVER,		"Unable to failover replica ID (%h) from server %A to any other cluster member")
#define ERR_LOOKFOR_REPLICA				PKG_CLIENT+25
	errortext(ERR_LOOKFOR_REPLICA,		"Looking for replica on server %A")
#define ERR_LOOKUP						PKG_CLIENT+26
	errortext(ERR_LOOKUP,				"Lookup on server %a")
#define ERR_FOUND_REPLICA				PKG_CLIENT+27
	errortext(ERR_FOUND_REPLICA,		"Failover on replica ID (%h) from server %A to %A")
#define ERR_LOOKFOR_REPLICA_EXT			PKG_CLIENT+28
	errortext(ERR_LOOKFOR_REPLICA_EXT,	"Looking for replica (%h) on server %A")
#define ERR_RETRY_WITH_NEW_SESSION		PKG_CLIENT+29
	internaltext(ERR_RETRY_WITH_NEW_SESSION,	"(internal status; retry the transaction after droping the session to the server)")
#define ERR_CLUSTER_NOMATES		 		PKG_CLIENT+30
	errortext(ERR_CLUSTER_NOMATES,		"No cluster mates found")
#define ERR_NO_CS_SUPPORT		 		PKG_CLIENT+31
	errortext(ERR_NO_CS_SUPPORT,		"The server doesn't support schedule free time lookups.")

#define ERR_REACHED_WRONG_SERVER		PKG_CLIENT+32
	errortext(ERR_REACHED_WRONG_SERVER, "Connection denied. You requested server %A but connected to %A.")
#define ERR_BAD_SUMMARY_BUFFER			PKG_CLIENT+33
	errortext(ERR_BAD_SUMMARY_BUFFER,  "Bad Summary buffer received from server.")
/* NOTE:	PKG_CLIENT+63 is the LAST code available for this file.  See */
/* globerr.h for dependency which restricts this string ID space. */

#endif
