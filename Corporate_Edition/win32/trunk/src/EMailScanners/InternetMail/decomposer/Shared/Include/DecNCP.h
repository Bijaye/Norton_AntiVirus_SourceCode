////////////////////////////////////////////////////////////////////////////////////////
//
// Decomposer Network Communication Protocol (DecNCP)
//
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2001, 2005 - Symantec Corporation. All rights reserved.
//
// This header describes the protocol used to communicate between a
// Decomposer client and server.  The server is the machine that is
// running the Decomposer.  The client sends commands and files to it
// for remote decomposition.
//
// As of this writing, the 'client' is a Win32 machine running a DecNCP-enabled
// version of the Ferret test tool and the 'server' is a Netware 5 server
// running the Hawk Decomposer server NLM.  This could of course be expanded in 
// the future to run on a variety of other platforms where local access to the server
// is limited.
//
// 

#ifndef __DECNCP_H__
#define __DECNCP_H__


#define DECNCP_BUFFER_SIZE						4096

//
// DecNCP runs over TCP/IP.  
// 
#define DECNCP_COMMAND_PORT						6875	// This port is used for general communication
#define DECNCP_FTP_PORT							6876	// This port is used to send files to the server        
#define DECNCP_OUTPUT_PORT						6877	// This port is used to send session output to the client


#define DECNCP_HELLO							0x0
//
// Direction: Bi-directional
// Port that receives this command: Any
//
// Either side can issue this packet if they want to see if the other side is alive.
// This packet is also used when you want to cancel a session.  Since only the
// command port receives the abort signal, the other two threads will remain
// in a blocking accept or blocking recv.  Send the abort to the command port,
// and then send DECNCP_HELLO to the other two ports.  This will unblock them
// and they can determine that the cancel flag has been set and exit.
//

#define DECNCP_ACKNOWLEDGE						0x1
//
// Direction: Bi-directional
// Port that receives this command: Any
//
// This packet is always sent after any successful communication.  Whenever one side
// issues a command to the other, the receiving side must issue a DCP_ACKNOWLEDGE
// packet to tell the sender that the information was received unless there was
// an error, in which case DECNCP_ERROR is sent instead. 
// 

#define DECNCP_SESSION_DONE						0x2
//
// Direction: Bi-directional
// Port that receives this command: Any
//
// Tells the server that the current decomposition session is done and the the client
// will not send any more files.  The client sends this command to the server on the
// command and FTP ports which will cause them to close and recycle the connections.
// The server will then send this command to the client's output port to shut him down.
//

#define DECNCP_ERROR							0xff
//
// Direction: Bi-directional
// Port that receives this command: Any
//
// This packet is sent instead of DECNCP_ACKNOWLEDGE if an error occurred.
// If the client gets this packet, an unrecoverable error has occurred and
// he should not attempt further communication with the server during this
// session.
// 


#define DECNCP_SERVER_COMMAND_ARGLIST			0x10
//
// Direction: From client to server.
// Port that receives this command: DECNCP_COMMAND_PORT
//
// Tells the server that the client is going to send him a command-line argument
// list.  This command is always followed by three more instructions:
// 1) The number of arguments. (BYTE)
// 2) The length of the command-line including NULL terminator (DWORD).  (**See note below)
// 3) The actual command-line.
// 
// NOTE: The command-line is to be comprised of a single buffer that contains space
// delimted arguments.
//
// args = 'arg1' 'arg2' 'arg3'
//
// This command line must not contain any file/directory names for processing.
// This includes the path to the .cfg file and the temp dir path as well.
// The client will send any files/directories via a separate command and the server
// will store/create the appropriate directory structure on the server.  The server
// will then create the appropriate full command line for decomposing.
//


#define DECNCP_SERVER_COMMAND_SHUTDOWN			0x15
//
// Direction: From client to server.
// Port that receives this command: DECNCP_COMMAND_PORT
//
// Instructs the server to shut down.
//


#define DECNCP_SERVER_COMMAND_SENDFILE			0x40
//
// Direction: From client to server.
// Port that receives this command: DECNCP_FTP_PORT
//
// Tells the server that the client is going to send him a list of files
// to process.  This command simply prepares the server to receive the files.
// This command is always followed by 4 more instructions:
// 1) Length of the filename (DWORD)
// 2) Filename
// 3) Length of the file (DWORD)
// 4) Actual file.
//


#define DECNCP_SERVER_COMMAND_SEND_OUTPUT		0x80
//
// Direction: From client to server.
// Port that sends this command: DECNCP_OUTPUT_PORT
//
// Tells the server that the client is ready to accept output from the server.
// This command must be received by the server before he will send any output
// to the client.
//


#define DECNCP_CLIENT_COMMAND_SESSION_OUTPUT	0x81
//
// Direction: From server to client.
// Port that sends this command: DECNCP_OUTPUT_PORT
//
// Tells the client that the server is about to send him the session output of the
// decomposition as a stream of bytes.  This is always followed by two more
// instructions:
// 1) The length of the session output in bytes.
// 2) The actual session output.
//


#endif
