///////////////////////
// This modules translates SymNetDrv message and string IDs into NisEvt values

#include "stdafx.h"
#include "SNLogMessages.h"
#include "SNEventMsgID.h"
#include "ccLogViewerInterface.h"
#include "resource.h"
#include "..\SNLogRes\ResResource.h"

// Voodo spells to prevent us from exposing SymNetDrv-private stuff
#define FILTER_DEVICE_IP						0
#define FILTER_DEVICE_UDP						1
#define FILTER_DEVICE_TCP						2
#define FILTER_DEVICE_RAW_IP					3 
#define FILTER_DEVICE_NDIS						4
#define FILTER_DEVICE_UNKNOWN					5
#define TDIT_OBJECT_ADDRESS					0
#define TDIT_OBJECT_CONNECTION				1
#define TDIT_OBJECT_CONTROL_CHANNEL			2
#define TDIT_OBJECT_CONNECTION_CONTEXT		3

#include "SNEventStrID.h"

using namespace cc;


// Remove this line to enable seperate short IDs
#define CALC_SHORT_ID


typedef struct 
{
	DWORD	dwSNID;
	DWORD	dwNISEvtID_Normal;
	DWORD	dwNISEvtID_HTML;
#ifndef CALC_SHORT_ID
	DWORD	dwNISEvtID_Short;
#endif
} SNMCLOOKUP, *PSNMCLOOKUP;

#ifdef CALC_SHORT_ID
// Use the definition below when the HTML ids are checked in 
#define SNMC(x) {x, SN_##x, SN_##x##_HTML}
#else
#define SNMC(x) {x, SN_##x, SN_##x##_HTML, SN_##x##_SHORT}
#endif

#pragma message ("Define these locally until we update to SND's latest build.  There are currently some issues in doing so")
#define EV_NDIS_UNCMNPROTOCOL_NOW_ALLOWED 0x000004E0L
#define EV_NDIS_UNCMNPROTOCOL_NOW_BLOCKED 0x000004E1L
#define EV_NDIS_UNCMNPROTOCOL_ALLOW_ALL  0x000004E2L
#define EV_NDIS_UNCMNPROTOCOL_BLOCK_ALL  0x000004E3L
#define EV_NDIS_UNCMNPROTOCOL_ALLOWED    0x000004E4L
#define EV_NDIS_UNCMNPROTOCOL_BLOCKED    0x000004E5L


//---------------------------------------------------------------------------
// 10 easy steps to maintaining SymNetDrv messages and resource strings
// Introduction:
// For each message in the Symnetdrv message file (//depot/Systems_Software/products/SymNetDrv/Trunk/Src/DrvInc/Tditlist.mc),
// there is a corresponding message in the SNLogMessages.mc message file (//depot/Norton_Internet_Security/trunk/src/UI/SNLog/SNLogMessages.mc)
// For each string in //depot/Systems_Software/products/SymNetDrv/Trunk/Src/tdit_msg.rc, there is a corresponding string in 
// //depot/Norton_Internet_Security/trunk/src/UI/SNLog/SNLog.rc (with symbols generated in resource.h)
// Each SymNetDrv message ID is defined in tditlist.h, which is posted to the Include directory of the symnetdrv release area as SNEventMsgID.h.
// Each SymNetDrv string ID is defined in tdit_msg.h, which is posted to the include directory of the symnetdrv release area as SNEventStrID.h
// //depot/Norton_Internet_Security/Shared_Components/trunk/src/UI/SNLog/SNLookup.cpp contains the tables that associate each SymNetDrv message/string
// with its NisEvt equivalent, this ensures the complete decoupling of the two.
//
// Adding a new message:
// 1. Add the message to tditlist.mc
// 2. Compile the message file.
// 3. build and post SymNetDrv (you can simulate this by copying tditlist.h -> SNEventMsgID.h in the SymNetDrv release area).
// 4. Add the corresponding messages to SNLogMessages.mc with an ID the same as the SymNetDrv id above, but prefixed by SN_
//    e.g. EV_MY_MESSAGE in SymNetDrv maps to SN_EV_MY_MESSAGE, you also need to add the HTML version (SN_EV_MY_MESSAGE_HTML) and
//    the Short version (SN_EV_MY_MESSAGE_Short), if separate short ids are being kept (see snlookup.cpp).
// 5. Add an entry to SNLookup.cpp of the form SNMC(<SymNetDrv id>),   e.g. SNMC(EV_MY_MESSAGE), preferably with comments describing the message
// 
// Adding a new string resource
// 1. Add the string to tdit_msg.rc
// 2. Compile the resource file.
// 3. build and post SymNetDrv (you can simulate this by copying tdit_msg.h -> SNEventStrID.h in the SymNetDrv release area).
// 4. Add the corresponding string to SNLog.rc with an ID in resource.h that has the same symbol as the SymNetDrv id above, but prefixed by SN_
//    e.g. IDS_MY_STRING in SymNetDrv maps to SN_IDS_MY_STRING
// 5. Add an entry to SNLookup.cpp of the form SNSTR(<SymNetDrv id>),   e.g. SNSTR(IDS_MY_STRING).
//
//---------------------------------------------------------------------------


static SNMCLOOKUP	aMessageTable[] =
{
	SNMC(EV_SOURCE_ERROR),
//
// MessageId: SN_EV_SIZE_ERROR
//
// MessageText:
//
//  Unable to log event %1 from source %2 - event too large.
//
	SNMC(EV_SIZE_ERROR),

//
// MessageId: SN_EV_EVENT_TYPE_ERROR
//
// MessageText:
//
//  Invalid type %1 specified in event %2 from source %3. 
//
	SNMC(EV_EVENT_TYPE_ERROR),

//
// MessageId: SN_EV_TEST_EVENT
//
// MessageText:
//
//  Test event, message number %1, event index %2.
//
	SNMC(EV_TEST_EVENT),

//
// MessageId: SN_EV_EVENT_ID_ERROR
//
// MessageText:
//
//  Invalid event ID %1 specified from source %2, type %3.
//
	SNMC(EV_EVENT_ID_ERROR),

//---------------------------------------------------------------------------
//    T D I    M e s s a g e s
//---------------------------------------------------------------------------
//
//
// MessageId: SN_EV_TDI_UNKNOWN_IOCTL
//
// MessageText:
//
//  Unknown device I/O control code
//
	SNMC(EV_TDI_UNKNOWN_IOCTL),

//
// MessageId: SN_EV_TDI_ASSOCIATE_ADDRESS
//
// MessageText:
//
//     %1: %2, Associated Address Object: %3, %4
//
	SNMC(EV_TDI_ASSOCIATE_ADDRESS),

//
// MessageId: SN_EV_TDI_DISASSOCIATE_ADDRESS
//
// MessageText:
//
//     %1: %2, %3
//
	SNMC(EV_TDI_DISASSOCIATE_ADDRESS),

//
// MessageId: SN_EV_TDI_CONNECT
//
// MessageText:
//
//  %1 %2: %3, Remote: %4(%5)
//
	SNMC(EV_TDI_CONNECT),

//
// MessageId: SN_EV_TDI_LISTEN
//
// MessageText:
//
//     Listen
//
	SNMC(EV_TDI_LISTEN),

//
// MessageId: SN_EV_TDI_ACCEPT
//
// MessageText:
//
//     %1: %2, Local IP(Port): %3(%4), %5
//
	SNMC(EV_TDI_ACCEPT),

//
// MessageId: SN_EV_TDI_DISCONNECT
//
// MessageText:
//
//  %1 %2: %3, %4
//
	SNMC(EV_TDI_DISCONNECT),

//
// MessageId: SN_EV_TDI_SEND
//
// MessageText:
//
//  %1 %2: %3, ToSend: %4, Flags: %5
//
	SNMC(EV_TDI_SEND),

//
// MessageId: SN_EV_TDI_RECEIVE
//
// MessageText:
//
//     %1: %2, Received: %3, BufferSize: %4, Flags: %5, %6
//
	SNMC(EV_TDI_RECEIVE),

//
// MessageId: SN_EV_TDI_SEND_DATAGRAM
//
// MessageText:
//
//  %1 %2: %3, %4(%5), ToSend: %6, Flags: %7
//
	SNMC(EV_TDI_SEND_DATAGRAM),

//
// MessageId: SN_EV_TDI_RECEIVE_DATAGRAM
//
// MessageText:
//
//     %1: %2, %3(%4), Length: %5, %6
//
	SNMC(EV_TDI_RECEIVE_DATAGRAM),

//
// MessageId: SN_EV_TDI_SET_EVENT_HANDLER
//
// MessageText:
//
//     %1: %2, %3 Handler: %4, Context: %5, %6
//
	SNMC(EV_TDI_SET_EVENT_HANDLER),

//
// MessageId: SN_EV_TDI_QUERY_INFORMATION
//
// MessageText:
//
//     %1: %2, Query: %3, %4
//
	SNMC(EV_TDI_QUERY_INFORMATION),

//
// MessageId: SN_EV_TDI_SET_INFORMATION
//
// MessageText:
//
//     Set Information
//
	SNMC(EV_TDI_SET_INFORMATION),

//
// MessageId: SN_EV_TDI_ACTION
//
// MessageText:
//
//     Action
//
	SNMC(EV_TDI_ACTION),

//
// MessageId: SN_EV_TDI_DIRECT_SEND
//
// MessageText:
//
//     Direct Send
//
	SNMC(EV_TDI_DIRECT_SEND),

//
// MessageId: SN_EV_TDI_DIRECT_SEND_DATAGRAM
//
// MessageText:
//
//     Direct Send Datagram
//
	SNMC(EV_TDI_DIRECT_SEND_DATAGRAM),

//
// MessageId: SN_EV_TDI_STATUS_ERROR
//
// MessageText:
//
//  Unknown status code returned %1
//
	SNMC(EV_TDI_STATUS_ERROR),

//
// MessageId: SN_EV_TDI_QUERY_ADDRESS_INFORMATION
//
// MessageText:
//
//     %1: %2, Query: %3, %4(%5), %6
//
	SNMC(EV_TDI_QUERY_ADDRESS_INFORMATION),

//
// MessageId: SN_EV_TDI_CREATE
//
// MessageText:
//
//     %1: %2, %3 Object, %4
//
	SNMC(EV_TDI_CREATE),

//
// MessageId: SN_EV_TDI_CLEANUP
//
// MessageText:
//
//     %1: %2, %3
//
	SNMC(EV_TDI_CLEANUP),

//
// MessageId: SN_EV_TDI_CLOSE
//
// MessageText:
//
//     %1: %2, %3
//
	SNMC(EV_TDI_CLOSE),

//
// MessageId: SN_EV_TDI_DEVICE_CONTROL
//
// MessageText:
//
//     %1: %2, Type: %3, Access: %4, Func: %5, Method: %6, %7, CtrlCode: %8
//
	SNMC(EV_TDI_DEVICE_CONTROL),

//
// MessageId: SN_EV_TDI_CREATE_ADDRESS
//
// MessageText:
//
//     %1: %2, %3 Object, %4(%5), %6
//
	SNMC(EV_TDI_CREATE_ADDRESS),

//
// MessageId: SN_EV_TDI_CREATE_CONNECTION_CONTEXT
//
// MessageText:
//
//     %1: %2, %3 Object, Context: %4, %5
//
	SNMC(EV_TDI_CREATE_CONNECTION_CONTEXT),

//
// MessageId: SN_EV_TDI_QUERY_INFORMATION_EX
//
// MessageText:
//
//     %1: %2, %3, %4, %5, Id: %6, Inst: %7, %8
//
	SNMC(EV_TDI_QUERY_INFORMATION_EX),

//
// MessageId: SN_EV_TDI_SET_INFORMATION_EX
//
// MessageText:
//
//     %1: %2, %3, %4, %5, Id: %6, Inst: %7, %8
//
	SNMC(EV_TDI_SET_INFORMATION_EX),

//
// MessageId: SN_EV_TDI_CONNECT_EVENT
//
// MessageText:
//
//     %1: %2, %3(%4), Connection: %5, Context: %6, %7
//
	SNMC(EV_TDI_CONNECT_EVENT),

//
// MessageId: SN_EV_TDI_DISCONNECT_EVENT
//
// MessageText:
//
//  %1 %2: %3, %4
//
	SNMC(EV_TDI_DISCONNECT_EVENT),

//
// MessageId: SN_EV_TDI_ERROR_EVENT
//
// MessageText:
//
//     %1: %2, %3
//
	SNMC(EV_TDI_ERROR_EVENT),

//
// MessageId: SN_EV_TDI_RECEIVE_EVENT
//
// MessageText:
//
//     %1: %2, Avail: %3, Ind: %4, Taken: %5, Flags: %6, %7
//
	SNMC(EV_TDI_RECEIVE_EVENT),

//
// MessageId: SN_EV_TDI_RECEIVE_DATAGRAM_EVENT
//
// MessageText:
//
//  %1 %2: %3, %4(%5), Avail: %6, Ind: %7, Taken: %8, Flags: %9, %10
//
	SNMC(EV_TDI_RECEIVE_DATAGRAM_EVENT),

//
// MessageId: SN_EV_TDI_RECEIVE_EXPEDITED_EVENT
//
// MessageText:
//
//  %1: %2, Available: %3, Indicated: %4, Taken: %5, Flags: %6, %7
//
	SNMC(EV_TDI_RECEIVE_EXPEDITED_EVENT),

//
// MessageId: SN_EV_TDI_SEND_POSSIBLE_EVENT
//
// MessageText:
//
//     %1: %2, Event Handler: %3, Context: %4, %5
//
	SNMC(EV_TDI_SEND_POSSIBLE_EVENT),

//
// MessageId: SN_EV_TDI_CHAINED_RECEIVE_EVENT
//
// MessageText:
//
//     %1: %2, Event Handler: %3, Context: %4, %5
//
	SNMC(EV_TDI_CHAINED_RECEIVE_EVENT),

//
// MessageId: SN_EV_TDI_CHAINED_RECEIVE_DATAGRAM_EVENT
//
// MessageText:
//
//  %1 %2: %3, Event Handler: %4, Context: %5, %6
//
	SNMC(EV_TDI_CHAINED_RECEIVE_DATAGRAM_EVENT),

//
// MessageId: SN_EV_TDI_CHAINED_RECEIVE_EXPEDITED_EVENT
//
// MessageText:
//
//     %1: %2, Event Handler: %3, Context: %4, %5
//
	SNMC(EV_TDI_CHAINED_RECEIVE_EXPEDITED_EVENT),

//
// MessageId: SN_EV_TDI_NOT_ENOUGH_STACKS
//
// MessageText:
//
//     Not enough stacks for IRP: %1, %2, %3
//
	SNMC(EV_TDI_NOT_ENOUGH_STACKS),

//
// MessageId: SN_EV_TDI_RECEIVE_FILTERED
//
// MessageText:
//
//  %1 %2: %3, BufferSize: %4, %5
//
	SNMC(EV_TDI_RECEIVE_FILTERED),

//
// MessageId: SN_EV_TDI_RECEIVE_COMPLETE_FILTERED
//
// MessageText:
//
//  %1 %2: %3, Received: %4, BufferSize: %5, Flags: %6, %7
//
	SNMC(EV_TDI_RECEIVE_COMPLETE_FILTERED),

//
// MessageId: SN_EV_TDI_RECEIVE_EVENT_FILTER_IN
//
// MessageText:
//
//  >> %1: %2, Available: %3, Indicated: %4, Copied: %5, Flags: %6 
//
	SNMC(EV_TDI_RECEIVE_EVENT_FILTER_IN),

//
// MessageId: SN_EV_TDI_RECEIVE_EVENT_FILTER_OUT
//
// MessageText:
//
//  -> %1: %2, Indicated: %3, Taken: %4, Flags: %5, %6
//
	SNMC(EV_TDI_RECEIVE_EVENT_FILTER_OUT),

//
// MessageId: SN_EV_TDI_DISCONNECT_EVENT_DEFERRED
//
// MessageText:
//
//     %1: %2, Receive data pending - Disconnect deferred
//
	SNMC(EV_TDI_DISCONNECT_EVENT_DEFERRED),

//
// MessageId: SN_EV_TDI_REFERENCE_COUNT_ERROR
//
// MessageText:
//
//     %1: %2, Reference count error, Count: %3
//
	SNMC(EV_TDI_REFERENCE_COUNT_ERROR),

//
// MessageId: SN_EV_TDI_DUPLICATE_HASH_ENTRY
//
// MessageText:
//
//     Duplicate hash entry: new --> %1 %2 Object: %3, existing --> %4 %5 Object: %6
//
	SNMC(EV_TDI_DUPLICATE_HASH_ENTRY),

//
// MessageId: SN_EV_TDI_QUEUE_DELETE
//
// MessageText:
//
//     %1 %2: %3, delete %4 from %5 queue
//
	SNMC(EV_TDI_QUEUE_DELETE),

//
// MessageId: SN_EV_TDI_FILTER_CONNECT
//
// MessageText:
//
//     %1: %2, FilterConnect: %3, Status: %4
//
	SNMC(EV_TDI_FILTER_CONNECT),

//
// MessageId: SN_EV_TDI_FILTER_DISCONNECT
//
// MessageText:
//
//     %1: %2, FilterDisconnect: %3, Status: %4
//
	SNMC(EV_TDI_FILTER_DISCONNECT),

//
// MessageId: SN_EV_TDI_FILTER_FLUSH_RECEIVE_QUEUE
//
// MessageText:
//
//     %1: %2,  FilterFlushReceiveQueue: %3, Status: %4
//
	SNMC(EV_TDI_FILTER_FLUSH_RECEIVE_QUEUE),

//
// MessageId: SN_EV_TDI_FILTER_FLUSH_SEND_QUEUE
//
// MessageText:
//
//     %1: %2,  FilterFlushSendQueue: %3, Status: %4
//
	SNMC(EV_TDI_FILTER_FLUSH_SEND_QUEUE),

//
// MessageId: SN_EV_TDI_FILTER_RECEIVE
//
// MessageText:
//
//     %1: %2,  FilterReceive: %3, BytesFiltered: %4, Status: %5
//
	SNMC(EV_TDI_FILTER_RECEIVE),

//
// MessageId: SN_EV_TDI_FILTER_SEND
//
// MessageText:
//
//     %1: %2,  FilterSend: %3, BytesFiltered: %4, Status: %5
//
	SNMC(EV_TDI_FILTER_SEND),

//
// MessageId: SN_EV_HTTP_FLUSHPROB
//
// MessageText:
//
//  Flushing with undelivered data - Parser state %1
//  The HTTP filter had undelivered data at the time that the connection was terminated.
//  Causes include: user reset, app slow to read, badly formatted HTML, or a parser bug.
//
	SNMC(EV_HTTP_FLUSHPROB),

//
// MessageId: SN_EV_HTTP_COOKIE
//
// MessageText:
//
//  %1
//
	SNMC(EV_HTTP_COOKIE),

//
// MessageId: SN_EV_HTTP_ERROR
//
// MessageText:
//
//  Error - %1
//
	SNMC(EV_HTTP_ERROR),

//
// MessageId: SN_EV_HTTP_AD_REMOVED
//
// MessageText:
//
//  Removed %1
//  From    %2
//  Because %3
//
	SNMC(EV_HTTP_AD_REMOVED),

//
//
// MessageId: SN_EV_HTTP_FRAGMENT
//
// MessageText:
//
//  %1
//  This is an HTML fragment likely to be associated with an image that appeared
//  on a web page that was recently viewed.  Placing it (or a portion of it) in
//  the blocklist will prevent further reception of the image.
//
	SNMC(EV_HTTP_FRAGMENT),

//
// MessageId: SN_EV_TDI_FILTER_ACCEPT
//
// MessageText:
//
//  %1: %2, FilterAccept: %3, Status: %4
//
	SNMC(EV_TDI_FILTER_ACCEPT),

//
// MessageId: SN_EV_TDI_FILTER_RECEIVE_DATAGRAM
//
// MessageText:
//
//  %1: %2,  FilterDatagramReceive: %3, BytesFiltered: %4, Status: %5
//
	SNMC(EV_TDI_FILTER_RECEIVE_DATAGRAM),

//
// MessageId: SN_EV_TDI_FILTER_SEND_DATAGRAM
//
// MessageText:
//
//  %1: %2,  FilterDatagramSend: %3, BytesFiltered: %4, Status: %5
//
	SNMC(EV_TDI_FILTER_SEND_DATAGRAM),

//
// MessageId: SN_EV_URL_LOG
//
// MessageText:
//
//  %1
//
	SNMC(EV_URL_LOG),

//
// MessageId: SN_EV_TDI_FILTER_CONNECT_COMPLETED
//
// MessageText:
//
//  %1: %2, FilterConnectCompleted: %3, Status: %4
//
	SNMC(EV_TDI_FILTER_CONNECT_COMPLETED),

//
// MessageId: SN_EV_TDI_DISCONNECT_EVENT_COMPLETE
//
// MessageText:
//
//  %1 %2: %3, %4, %5
//
	SNMC(EV_TDI_DISCONNECT_EVENT_COMPLETE),

//
// MessageId: SN_EV_TDI_CONNECT_COMPLETE
//
// MessageText:
//
//  %1 %2: %3, Local: %4(%5), %6
//
	SNMC(EV_TDI_CONNECT_COMPLETE),

//
// MessageId: SN_EV_TDI_CONNECT1
//
// MessageText:
//
//  %1 %2: %3
//
	SNMC(EV_TDI_CONNECT1),

//
// MessageId: SN_EV_TDI_DISCONNECT_COMPLETE
//
// MessageText:
//
//  %1 %2: %3, %4
//
	SNMC(EV_TDI_DISCONNECT_COMPLETE),

//
// MessageId: SN_EV_TDI_SEND_COMPLETE
//
// MessageText:
//
//  %1 %2: %3, Sent: %4, %5
//
	SNMC(EV_TDI_SEND_COMPLETE),

//
// MessageId: SN_EV_TDI_SEND_DATAGRAM_COMPLETE
//
// MessageText:
//
//  %1 %2: %3, Sent: %4, %5
//
	SNMC(EV_TDI_SEND_DATAGRAM_COMPLETE),

//
// MessageId: SN_EV_TDI_RECEIVE_DATAGRAM_COMPLETE_FILTERED
//
// MessageText:
//
//  %1 %2: %3, %4(%5), Received: %6, BufferSize: %7, Flags: %8, %9
//
	SNMC(EV_TDI_RECEIVE_DATAGRAM_COMPLETE_FILTERED),

//
// MessageId: SN_EV_TDI_DUPLICATE_CC_HASH_ENTRY
//
// MessageText:
//
//     Duplicate hash entry for %1 %2: %3, connection objects: new -> %4, old -> %5
//
	SNMC(EV_TDI_DUPLICATE_CC_HASH_ENTRY),

//
// MessageId: SN_EV_TDI_RING0_THREAD_START
//
// MessageText:
//
//  Ring 0 Thread has started successfully.
//
	SNMC(EV_TDI_RING0_THREAD_START),

//
// MessageId: SN_EV_TDI_DEVICE_INIT
//
// MessageText:
//
//  Device Init called.
//
	SNMC(EV_TDI_DEVICE_INIT),

//
// MessageId: SN_EV_TDI_INIT_COMPLETE
//
// MessageText:
//
//  Init Complete called.
//
	SNMC(EV_TDI_INIT_COMPLETE),

//
// MessageId: SN_EV_TDI_SYS_VM_INIT
//
// MessageText:
//
//  Sys VM Init called.
//
	SNMC(EV_TDI_SYS_VM_INIT),

//
// MessageId: SN_EV_TDI_OPEN_ALERT_HANDLER
//
// MessageText:
//
//  Open alert handler.
//
	SNMC(EV_TDI_OPEN_ALERT_HANDLER),

//
// MessageId: SN_EV_TDI_CLOSE_ALERT_HANDLER
//
// MessageText:
//
//  Close alert handler.
//
	SNMC(EV_TDI_CLOSE_ALERT_HANDLER),

//
// MessageId: SN_EV_TDI_FILTER_FAILED_TO_LOAD
//
// MessageText:
//
//  %1 failed to load.
//
	SNMC(EV_TDI_FILTER_FAILED_TO_LOAD),

//---------------------------------------------------------------------------
//    I P	F i l t e r		M e s s a g e s
//---------------------------------------------------------------------------
//
//
// MessageId: SN_EV_IP_FILTER_GET_IP_FILTER_RULE_FAILED
//
// MessageText:
//
//  GetIPFilterRule() call failed with an error "%1"
//
	SNMC(EV_IP_FILTER_GET_IP_FILTER_RULE_FAILED),

//
// MessageId: SN_EV_IP_FILTER_PROCESSED_RULE
//
// MessageText:
//
//  Firewall configuration updated: %1 rules
//
	SNMC(EV_IP_FILTER_PROCESSED_RULE),

//
// MessageId: SN_EV_IP_FILTER_INVALID_SECTION_HEADER_OBJECT_TYPE
//
// MessageText:
//
//  GetObjectInfoFromSectionKey() found an invalid object type.  Section Key is "%1"
//
	SNMC(EV_IP_FILTER_INVALID_SECTION_HEADER_OBJECT_TYPE),

//
// MessageId: SN_EV_IP_FILTER_INVALID_SECTION_HEADER_OBJECT_NAME
//
// MessageText:
//
//  GetObjectInfoFromSectionKey() found an invalid object name.  Section Key is "%1"
//
	SNMC(EV_IP_FILTER_INVALID_SECTION_HEADER_OBJECT_NAME),

//
// MessageId: SN_EV_IP_FILTER_UNKNOWN_IP_FILTER_RULE_VALUE
//
// MessageText:
//
//  GetIPFilterRule() ignoring unknown IP Filter Rule value of "%1".  IP Filter Object name is "%2"
//
	SNMC(EV_IP_FILTER_UNKNOWN_IP_FILTER_RULE_VALUE),

//
// MessageId: SN_EV_IP_FILTER_UNABLE_TO_ADD_OBJECT
//
// MessageText:
//
//  FillInObjectForRule() unable to add object type "%1" named "%2", from section key "%3".
//
	SNMC(EV_IP_FILTER_UNABLE_TO_ADD_OBJECT),

//
// MessageId: SN_EV_IP_FILTER_UNKNOWN_IP_OBJECT_TYPE
//
// MessageText:
//
//  AddIPObjectToList() unknown IP Object type of #%1
//
	SNMC(EV_IP_FILTER_UNKNOWN_IP_OBJECT_TYPE),

//
// MessageId: SN_EV_IP_FILTER_UNKNOWN_HOST_OBJECT_VALUE
//
// MessageText:
//
//  InsertIPHostObject() ignoring unknown host object value of "%1"
//
	SNMC(EV_IP_FILTER_UNKNOWN_HOST_OBJECT_VALUE),

//
// MessageId: SN_EV_IP_FILTER_UNKNOWN_NETWORK_OBJECT_VALUE
//
// MessageText:
//
//  InsertIPNetworkObject() ignoring unknown network object value of "%1"
//
	SNMC(EV_IP_FILTER_UNKNOWN_NETWORK_OBJECT_VALUE),

//
// MessageId: SN_EV_IP_FILTER_UNKNOWN_RANGE_OBJECT_VALUE
//
// MessageText:
//
//  InsertIPRangeObject() ignoring unknown range object value of "%1"
//
	SNMC(EV_IP_FILTER_UNKNOWN_RANGE_OBJECT_VALUE),

//
// MessageId: SN_EV_IP_FILTER_UNKNOWN_GROUP_IP_OBJECT_TYPE
//
// MessageText:
//
//  ExpandIPGroupObject() ignoring unknown IP object type of "%1"
//
	SNMC(EV_IP_FILTER_UNKNOWN_GROUP_IP_OBJECT_TYPE),

//
// MessageId: SN_EV_IP_FILTER_UNABLE_TO_EXPAND_IP_GROUP_OBJECT
//
// MessageText:
//
//  InsertIPGroupObject() unable to expand IP Group object of "%1"
//
	SNMC(EV_IP_FILTER_UNABLE_TO_EXPAND_IP_GROUP_OBJECT),

//
// MessageId: SN_EV_IP_FILTER_UNABLE_TO_ADD_GROUP_LIST_MEMBER_OBJECT
//
// MessageText:
//
//  ExpandIPGroupObject() unable to add group "%1" list member object of section key "%2"
//
	SNMC(EV_IP_FILTER_UNABLE_TO_ADD_GROUP_LIST_MEMBER_OBJECT),

//
// MessageId: SN_EV_IP_FILTER_UNABLE_TO_ADD_SERVICE_LIST_MEMBER_OBJECT
//
// MessageText:
//
//  AddServiceObjectToList() unable to add service "%1" list member object of section key "%2"
//
	SNMC(EV_IP_FILTER_UNABLE_TO_ADD_SERVICE_LIST_MEMBER_OBJECT),

//
// MessageId: SN_EV_IP_FILTER_UNKNOWN_SERVICE_VALUE
//
// MessageText:
//
//  AddServiceObjectToList() ignoring unknown service object value of "%1"
//
	SNMC(EV_IP_FILTER_UNKNOWN_SERVICE_VALUE),

//
// MessageId: SN_EV_IP_FILTER_UNKNOWN_SERVICE_OBJECT_TYPE
//
// MessageText:
//
//  AddServiceObjectToList() unknown service object type of %1
//
	SNMC(EV_IP_FILTER_UNKNOWN_SERVICE_OBJECT_TYPE),

//
// MessageId: SN_EV_IP_FILTER_UNKNOWN_TIME_VALUE
//
// MessageText:
//
//  AddActivationTimeObjectToList() ignoring unknown time object value of "%1"
//
	SNMC(EV_IP_FILTER_UNKNOWN_TIME_VALUE),

//
// MessageId: SN_EV_IP_FILTER_UNKNOWN_TIME_OBJECT_TYPE
//
// MessageText:
//
//  AddActivationTimeObjectToList() unknown service object type of %1
//
	SNMC(EV_IP_FILTER_UNKNOWN_TIME_OBJECT_TYPE),

//
// MessageId: SN_EV_IP_FILTER_MEMORY_ALLOCATION_FAILED
//
// MessageText:
//
//  IPFilterAllocateMemory() unable to allocate %1 bytes of memory
//
	SNMC(EV_IP_FILTER_MEMORY_ALLOCATION_FAILED),

//
// MessageId: SN_EV_IP_FILTER_UNABLE_TO_FREE_OBJECT_TYPE
//
// MessageText:
//
//  FreeIPGroupObject() unable to free object type %1 
//
	SNMC(EV_IP_FILTER_UNABLE_TO_FREE_OBJECT_TYPE),

//
// MessageId: SN_EV_IP_FILTER_BUILD_RULES_FROM_REGISTRY_FAILED
//
// MessageText:
//
//  Unable to build IP filter rule base.   Error is "%1".
//
	SNMC(EV_IP_FILTER_BUILD_RULES_FROM_REGISTRY_FAILED),

//
// MessageId: SN_EV_IP_FILTER_OUTGOING_CONNECT_REQUEST_REJECTED
//
// MessageText:
//
//  Rule "%1" blocked (%2,%3).
//  Outbound TCP connection
//  Remote address,service is (%4,%5)
//  Process name is "%6"
//
	SNMC(EV_IP_FILTER_OUTGOING_CONNECT_REQUEST_REJECTED),

//
// MessageId: SN_EV_IP_FILTER_UNABLE_TO_PARSE_SECTION_HEADER
//
// MessageText:
//
//  FillInObjectForRule() unable to parse section key "%1".
//
	SNMC(EV_IP_FILTER_UNABLE_TO_PARSE_SECTION_HEADER),

//
// MessageId: SN_EV_IP_FILTER_UNABLE_TO_ADD_HOST_LIST_MEMBER_OBJECT
//
// MessageText:
//
//  InsertIPHostObject() unable to add host "%1" list member object of section key "%2"
//
	SNMC(EV_IP_FILTER_UNABLE_TO_ADD_HOST_LIST_MEMBER_OBJECT),

//
// MessageId: SN_EV_IP_FILTER_UNABLE_TO_ADD_APPLICATION_LIST_MEMBER_OBJECT
//
// MessageText:
//
//  AddApplicationObjectToList() unable to add application "%1" list member object of section key "%2"
//
	SNMC(EV_IP_FILTER_UNABLE_TO_ADD_APPLICATION_LIST_MEMBER_OBJECT),

//
// MessageId: SN_EV_IP_FILTER_UNKNOWN_APPLICATION_VALUE
//
// MessageText:
//
//  AddApplicationObjectToList() ignoring unknown application object value of "%1"
//
	SNMC(EV_IP_FILTER_UNKNOWN_APPLICATION_VALUE),

//
// MessageId: SN_EV_IP_FILTER_UNKNOWN_APPLICATION_OBJECT_TYPE
//
// MessageText:
//
//  AddApplicationObjectToList() unknown application object type of %1
//
	SNMC(EV_IP_FILTER_UNKNOWN_APPLICATION_OBJECT_TYPE),

//
// MessageId: SN_EV_IP_FILTER_RUNTIME_UNKNOWN_APPLICATION_OBJECT_TYPE
//
// MessageText:
//
//  DoesApplicationNameMatch() looking for object "%1" but instead found object "%2"
//
	SNMC(EV_IP_FILTER_RUNTIME_UNKNOWN_APPLICATION_OBJECT_TYPE),

//
// MessageId: SN_EV_IP_FILTER_RUNTIME_UNKNOWN_TIME_OBJECT_TYPE
//
// MessageText:
//
//  IsApplicationTimeInEffect() looking for object "%1" but instead found object "%2"
//
	SNMC(EV_IP_FILTER_RUNTIME_UNKNOWN_TIME_OBJECT_TYPE),

//
// MessageId: SN_EV_IP_FILTER_RUNTIME_UNKNOWN_SERVICE_OBJECT_TYPE
//
// MessageText:
//
//  IsServiceInObjectList() looking for object "%1" but instead found object "%2"
//
	SNMC(EV_IP_FILTER_RUNTIME_UNKNOWN_SERVICE_OBJECT_TYPE),

//
// MessageId: SN_EV_IP_FILTER_RUNTIME_UNKNOWN_IP_OBJECT_TYPE
//
// MessageText:
//
//  IsIPAddressInObjectList() looking for object "%1", "%2", "%3", or "%4", but instead found object "%5"
//
	SNMC(EV_IP_FILTER_RUNTIME_UNKNOWN_IP_OBJECT_TYPE),

//
// MessageId: SN_EV_IP_FILTER_UNKNOWN_RUNTIME_HOST_OBJECT_TYPE
//
// MessageText:
//
//  IsHostIPObjectInList() looking for object "%1", but instead found object "%2"
//
	SNMC(EV_IP_FILTER_UNKNOWN_RUNTIME_HOST_OBJECT_TYPE),

//
// MessageId: SN_EV_IP_FILTER_UNKNOWN_RUNTIME_NETWORK_OBJECT_TYPE
//
// MessageText:
//
//  IsNetworkIPObjectInList() looking for object "%1", but instead found object "%2"
//
	SNMC(EV_IP_FILTER_UNKNOWN_RUNTIME_NETWORK_OBJECT_TYPE),

//
// MessageId: SN_EV_IP_FILTER_UNKNOWN_RUNTIME_RANGE_OBJECT_TYPE
//
// MessageText:
//
//  IsRangeIPObjectInList() looking for object "%1", but instead found object "%2"
//
	SNMC(EV_IP_FILTER_UNKNOWN_RUNTIME_RANGE_OBJECT_TYPE),

//
// MessageId: SN_EV_IP_FILTER_UNKNOWN_RUNTIME_GROUP_OBJECT_TYPE
//
// MessageText:
//
//  IsGroupIPObjectInList() looking for object "%1", but instead found object "%2"
//
	SNMC(EV_IP_FILTER_UNKNOWN_RUNTIME_GROUP_OBJECT_TYPE),

//
// MessageId: SN_EV_IP_FILTER_RUNTIME_UNKNOWN_GROUP_IP_OBJECT_TYPE
//
// MessageText:
//
//  IsGroupIPObjectInList() looking for object "%1", "%2", "%3", or "%4", but instead found object "%5"
//
	SNMC(EV_IP_FILTER_RUNTIME_UNKNOWN_GROUP_IP_OBJECT_TYPE),

//
// MessageId: SN_EV_IP_FILTER_POST_PROCESS_OUTGOING_CONNECT_REQUEST_REJECTED
//
// MessageText:
//
//  Rule "%1" blocked (%2,%3).
//  Outbound TCP connection 
//  Local address,service is (%4,%5)
//  Remote address,service is (%6,%7)
//  Process name is "%8"
//
	SNMC(EV_IP_FILTER_POST_PROCESS_OUTGOING_CONNECT_REQUEST_REJECTED),

//
// MessageId: SN_EV_IP_FILTER_INCOMING_CONNECT_REQUEST_REJECTED
//
// MessageText:
//
//  Rule "%1" blocked (%2,%3).
//  Inbound TCP connection 
//  Local address,service is (%4,%5)
//  Remote address,service is (%6,%7)
//  Process name is "%8"
//
	SNMC(EV_IP_FILTER_INCOMING_CONNECT_REQUEST_REJECTED),

//
// MessageId: SN_EV_IP_FILTER_BASE_KEY_INITIALIZATION_FAILED
//
// MessageText:
//
//  BuildIPFilterRulesFromRegistry() call failed with an error "%1"
//
	SNMC(EV_IP_FILTER_BASE_KEY_INITIALIZATION_FAILED),

//
// MessageId: SN_EV_IP_FILTER_LEAF_ENUMERATION_FAILED
//
// MessageText:
//
//  Enumerating section "%1" failed with an error "%2"
//
	SNMC(EV_IP_FILTER_LEAF_ENUMERATION_FAILED),

//
// MessageId: SN_EV_IP_FILTER_UNKNOWN_GROUP_OBJECT_VALUE
//
// MessageText:
//
//  InsertIPGroupObject() ignoring unknown group object value of "%1"
//
	SNMC(EV_IP_FILTER_UNKNOWN_GROUP_OBJECT_VALUE),

//
// MessageId: SN_EV_IP_FILTER_OUTGOING_UDP_SEND_DATAGRAM_REQUEST_REJECTED
//
// MessageText:
//
//  Rule "%1" blocked (%2,%3).
//  Outbound UDP packet 
//  Local address,service is (%4,%5)
//  Remote address,service is (%6,%7)
//  Process name is "%8"
//
	SNMC(EV_IP_FILTER_OUTGOING_UDP_SEND_DATAGRAM_REQUEST_REJECTED),

//
// MessageId: SN_EV_IP_FILTER_INCOMING_UDP_RECEIVE_DATAGRAM_REQUEST_REJECTED
//
// MessageText:
//
//  Rule "%1" blocked (%2,%3).
//  Inbound UDP packet 
//  Local address,service is (%4,%5)
//  Remote address,service is (%6,%7)
//  Process name is "%8"
//
	SNMC(EV_IP_FILTER_INCOMING_UDP_RECEIVE_DATAGRAM_REQUEST_REJECTED),

//
// MessageId: SN_EV_IP_FILTER_UNKNOWN_FIREWALL_STATE_VALUE
//
// MessageText:
//
//  BuildIPFilterRulesFromRegistry() encounted unknown firewall state value of "%1"
//
	SNMC(EV_IP_FILTER_UNKNOWN_FIREWALL_STATE_VALUE),

//
// MessageId: SN_EV_IP_FILTER_OUTGOING_CONNECT_ALERT
//
// MessageText:
//
//  %1 "%2" communications.
//  Outbound TCP connection 
//  Remote address,service is (%3,%4)
//  Process name is "%5"
//
	SNMC(EV_IP_FILTER_OUTGOING_CONNECT_ALERT),

//
// MessageId: SN_EV_IP_FILTER_INCOMING_CONNECT_ALERT
//
// MessageText:
//
//  %1 "%2" communications.
//  Inbound TCP connection 
//  Local address,service is (%3,%4)
//  Remote address,service is (%5,%6)
//  Process name is "%7"
//
	SNMC(EV_IP_FILTER_INCOMING_CONNECT_ALERT),

//
// MessageId: SN_EV_IP_FILTER_OUTGOING_UDP_SEND_DATAGRAM_ALERT
//
// MessageText:
//
//  %1 "%2" communications.
//  Outbound UDP packet 
//  Local address,service is (%3,%4)
//  Remote address,service is (%5,%6)
//  Process name is "%7"
//
	SNMC(EV_IP_FILTER_OUTGOING_UDP_SEND_DATAGRAM_ALERT),

//
// MessageId: SN_EV_IP_FILTER_INCOMING_UDP_RECEIVE_DATAGRAM_ALERT
//
// MessageText:
//
//  %1 "%2" communications.
//  Inbound UDP packet 
//  Local address,service is (%3,%4)
//  Remote address,service is (%5,%6)
//  Process name is "%7"
//
	SNMC(EV_IP_FILTER_INCOMING_UDP_RECEIVE_DATAGRAM_ALERT),

//
// MessageId: SN_EV_IP_FILTER_REMOVE_INCOMING_ALERT_LIST_FAILED
//
// MessageText:
//
//  %1 "%2":
//  Unable to free alert list object memory for inbound TCP connection 
//  Local address,service is (%3,%4)
//  Remote address,service is (%5,%6)
//  Process name is "%7"
//
	SNMC(EV_IP_FILTER_REMOVE_INCOMING_ALERT_LIST_FAILED),

//
// MessageId: SN_EV_IP_FILTER_REMOVE_OUTGOING_ALERT_LIST_FAILED
//
// MessageText:
//
//  %1 "%2":
//  Unable to free alert list object memory for outbound TCP connection 
//  Local address,service is (%3,%4)
//  Remote address,service is (%5,%6)
//  Process name is "%7"
//
	SNMC(EV_IP_FILTER_REMOVE_OUTGOING_ALERT_LIST_FAILED),

//
// MessageId: SN_EV_IP_FILTER_INCOMING_CONNECT_REQUEST_ALLOWED
//
// MessageText:
//
//  Rule "%1" permitted (%2,%3).
//  Inbound TCP connection 
//  Local address,service is (%4,%5)
//  Remote address,service is (%6,%7)
//  Process name is "%8"
//
	SNMC(EV_IP_FILTER_INCOMING_CONNECT_REQUEST_ALLOWED),

//
// MessageId: SN_EV_IP_FILTER_OUTGOING_CONNECT_REQUEST_ALLOWED
//
// MessageText:
//
//  Rule "%1" permitted (%2,%3).
//  Outbound TCP connection 
//  Remote address,service is (%4,%5)
//  Process name is "%6"
//
	SNMC(EV_IP_FILTER_OUTGOING_CONNECT_REQUEST_ALLOWED),

//
// MessageId: SN_EV_IP_FILTER_POST_PROCESS_OUTGOING_CONNECT_REQUEST_ALLOWED
//
// MessageText:
//
//  Rule "%1" permitted (%2,%3).
//  Outbound TCP connection 
//  Local address,service is (%4,%5)
//  Remote address,service is (%6,%7)
//  Process name is "%8"
//
	SNMC(EV_IP_FILTER_POST_PROCESS_OUTGOING_CONNECT_REQUEST_ALLOWED),

//
// MessageId: SN_EV_IP_FILTER_OUTGOING_UDP_SEND_DATAGRAM_REQUEST_ALLOWED
//
// MessageText:
//
//  Rule "%1" permitted (%2,%3).
//  Outbound UDP packet 
//  Local address,service is (%4,%5)
//  Remote address,service is (%6,%7)
//  Process name is "%8"
//
	SNMC(EV_IP_FILTER_OUTGOING_UDP_SEND_DATAGRAM_REQUEST_ALLOWED),

//
// MessageId: SN_EV_IP_FILTER_INCOMING_UDP_RECEIVE_DATAGRAM_REQUEST_ALLOWED
//
// MessageText:
//
//  Rule "%1" permitted (%2,%3).
//  Inbound UDP packet 
//  Local address,service is (%4,%5)
//  Remote address,service is (%6,%7)
//  Process name is "%8"
//
	SNMC(EV_IP_FILTER_INCOMING_UDP_RECEIVE_DATAGRAM_REQUEST_ALLOWED),

//
// MessageId: SN_EV_IP_FILTER_OUTGOING_CONNECT_REQUEST_REJECTED2
//
// MessageText:
//
//  Rule "%1" blocked (%2,%3).
//  Outbound TCP connection
//  Local address,service is (%4,%5)
//  Remote address,service is (%6,%7)
//  Process name is "%8"
//
	SNMC(EV_IP_FILTER_OUTGOING_CONNECT_REQUEST_REJECTED2),

//
// MessageId: SN_EV_IP_FILTER_OUTGOING_CONNECT_REQUEST_ALLOWED2
//
// MessageText:
//
//  Rule "%1" permitted (%2,%3).
//  Outbound TCP connection 
//  Local address,service is (%4,%5)
//  Remote address,service is (%6,%7)
//  Process name is "%8"
//
	SNMC(EV_IP_FILTER_OUTGOING_CONNECT_REQUEST_ALLOWED2),

//
// MessageId: SN_EV_IP_FILTER_OUTGOING_CONNECT_REQUEST_IGNORED2
//
// MessageText:
//
//  Rule "%1" ignored (%2,%3).
//  Outbound TCP connection 
//  Local address,service is (%4,%5)
//  Remote address,service is (%6,%7)
//  Process name is "%8"
//
	SNMC(EV_IP_FILTER_OUTGOING_CONNECT_REQUEST_IGNORED2),

//
// MessageId: SN_EV_IP_FILTER_POST_PROCESS_OUTGOING_CONNECT_REQUEST_IGNORED
//
// MessageText:
//
//  Rule "%1" ignored (%2,%3).
//  Outbound TCP connection 
//  Local address,service is (%4,%5)
//  Remote address,service is (%6,%7)
//  Process name is "%8"
//
	SNMC(EV_IP_FILTER_POST_PROCESS_OUTGOING_CONNECT_REQUEST_IGNORED),

//
// MessageId: SN_EV_IP_FILTER_INCOMING_CONNECT_REQUEST_IGNORED
//
// MessageText:
//
//  Rule "%1" ignored (%2,%3).
//  Inbound TCP connection 
//  Local address,service is (%4,%5)
//  Remote address,service is (%6,%7)
//  Process name is "%8"
//
	SNMC(EV_IP_FILTER_INCOMING_CONNECT_REQUEST_IGNORED),

//
// MessageId: SN_EV_IP_FILTER_INCOMING_UDP_RECEIVE_DATAGRAM_REQUEST_IGNORED
//
// MessageText:
//
//  Rule "%1" ignored (%2,%3).
//  Inbound UDP packet 
//  Local address,service is (%4,%5)
//  Remote address,service is (%6,%7)
//  Process name is "%8"
//
	SNMC(EV_IP_FILTER_INCOMING_UDP_RECEIVE_DATAGRAM_REQUEST_IGNORED),

//
// MessageId: SN_EV_IP_FILTER_OUTGOING_UDP_SEND_DATAGRAM_REQUEST_IGNORED
//
// MessageText:
//
//  Rule "%1" ignored (%2,%3).
//  Outbound UDP packet 
//  Local address,service is (%4,%5)
//  Remote address,service is (%6,%7)
//  Process name is "%8"
//
	SNMC(EV_IP_FILTER_OUTGOING_UDP_SEND_DATAGRAM_REQUEST_IGNORED),

//
// MessageId: SN_EV_IP_FILTER_INCOMING_ICMP_REJECTED
//
// MessageText:
//
//  Rule "%1" blocked (%2,%3).
//  Inbound ICMP request 
//  Local address is (%4)
//  Remote address is (%5)
//  Message type is "%6"
//  Process name is "%7"
//
	SNMC(EV_IP_FILTER_INCOMING_ICMP_REJECTED),

//
// MessageId: SN_EV_IP_FILTER_INCOMING_ICMP_ALLOWED
//
// MessageText:
//
//  Rule "%1" permitted (%2,%3).
//  Inbound ICMP request 
//  Local address is (%4)
//  Remote address is (%5)
//  Message type is "%6"
//  Process name is "%7"
//
	SNMC(EV_IP_FILTER_INCOMING_ICMP_ALLOWED),

//
// MessageId: SN_EV_IP_FILTER_INCOMING_ICMP_IGNORED
//
// MessageText:
//
//  Rule "%1" ignored (%2,%3).
//  Inbound ICMP request 
//  Local address is (%4)
//  Remote address is (%5)
//  Message type is "%6"
//  Process name is "%7"
//
	SNMC(EV_IP_FILTER_INCOMING_ICMP_IGNORED),

//
// MessageId: SN_EV_IP_FILTER_OUTGOING_ICMP_REJECTED
//
// MessageText:
//
//  Rule "%1" blocked (%2,%3).
//  Outbound ICMP request 
//  Local address is (%4)
//  Remote address is (%5)
//  Message type is "%6"
//  Process name is "%7"
//
	SNMC(EV_IP_FILTER_OUTGOING_ICMP_REJECTED),

//
// MessageId: SN_EV_IP_FILTER_OUTGOING_ICMP_ALLOWED
//
// MessageText:
//
//  Rule "%1" permitted (%2,%3).
//  Outbound ICMP request 
//  Local address is (%4)
//  Remote address is (%5)
//  Message type is "%6"
//  Process name is "%7"
//
	SNMC(EV_IP_FILTER_OUTGOING_ICMP_ALLOWED),

//
// MessageId: SN_EV_IP_FILTER_OUTGOING_ICMP_IGNORED
//
// MessageText:
//
//  Rule "%1" ignored (%2,%3).
//  Outbound ICMP request 
//  Local address is (%4)
//  Remote address is (%5)
//  Message type is "%6"
//  Process name is "%7"
//
	SNMC(EV_IP_FILTER_OUTGOING_ICMP_IGNORED),

//
// MessageId: SN_EV_IP_FILTER_INCOMING_ICMP_ALERT
//
// MessageText:
//
//  %1 "%2" communications.
//  Inbound ICMP request 
//  Local address (%3)
//  Remote address is (%4)
//  Message type is "%5"
//  Process name is "%6"
//
	SNMC(EV_IP_FILTER_INCOMING_ICMP_ALERT),

//
// MessageId: SN_EV_IP_FILTER_OUTGOING_ICMP_ALERT
//
// MessageText:
//
//  %1 "%2" communications.
//  Outbound ICMP request 
//  Local address (%3)
//  Remote address is (%4)
//  Message type is "%5"
//  Process name is "%6"
//
	SNMC(EV_IP_FILTER_OUTGOING_ICMP_ALERT),

//
// MessageId: SN_EV_IP_FILTER_OUTGOING_CONNECT_RESTRICTED
//
// MessageText:
//
//  Application restricted.
//  Process name is "%1"
//  Restricted category is "%2"
//
	SNMC(EV_IP_FILTER_OUTGOING_CONNECT_RESTRICTED),

//
// MessageId: SN_EV_IP_FILTER_INCOMING_UNUSED_RESTRICTED
//
// MessageText:
//
//  Unused port blocking has blocked communications.
//  Inbound TCP connection 
//  Remote address,local service is (%1,%2)
//
	SNMC(EV_IP_FILTER_INCOMING_UNUSED_RESTRICTED),

//
// MessageId: SN_EV_IP_FILTER_INCOMING_CONNECT_REQUEST_STEALTHED
//
// MessageText:
//
//  Rule "%1" stealthed (%2,%3).
//  Inbound TCP connection 
//  Local address,service is (%4,%5)
//  Remote address,service is (%6,%7)
//  Process name is "%8"
//
	SNMC(EV_IP_FILTER_INCOMING_CONNECT_REQUEST_STEALTHED),

//
// MessageId: SN_EV_IP_FILTER_INCOMING_UDP_RECEIVE_DATAGRAM_REQUEST_STEALTHED
//
// MessageText:
//
//  Rule "%1" stealthed (%2,%3).
//  Inbound UDP packet 
//  Local address,service is (%4,%5)
//  Remote address,service is (%6,%7)
//  Process name is "%8"
//
	SNMC(EV_IP_FILTER_INCOMING_UDP_RECEIVE_DATAGRAM_REQUEST_STEALTHED),

//
// MessageId: SN_EV_IP_FILTER_PORT_SCAN_DETECTED
//
// MessageText:
//
//  Port scan detected from address %1.
//  Blocked further access for %3 minutes after detecting at least %2 ports being probed.
//
	SNMC(EV_IP_FILTER_PORT_SCAN_DETECTED),

//
// MessageId: SN_EV_IP_FILTER_IDS_BLOCK
//
// MessageText:
//
//	Intrusion Detection detected and blocked the %2 Trojan horse.
//	All comunication with %1 will be blocked for %3 minutes.
//
	SNMC(EV_IP_FILTER_IDS_BLOCK),

//
// MessageId: SN_EV_HTTP_ACTIVEX_REMOVED
//
// MessageText:
//
//  Removed Entire OBJECT tag
//  From    %1
//  Because ActiveX blocking enabled
//
	SNMC(EV_HTTP_ACTIVEX_REMOVED),

//
// MessageId: SN_EV_HTTP_APPLET_REMOVED
//
// MessageText:
//
//  Removed Entire APPLET tag
//  From    %1
//  Because Applet blocking enabled
//
	SNMC(EV_HTTP_APPLET_REMOVED),

//
// MessageId: SN_EV_HTTP_SCRIPT_REMOVED
//
// MessageText:
//
//  Removed Entire SCRIPT tag
//  From    %1
//  Because Script blocking enabled
//
	SNMC(EV_HTTP_SCRIPT_REMOVED),

//
// MessageId: SN_EV_HTTP_SCRIPTBASEDPOPUP_REMOVED
//
// MessageText:
//
//  Removed %1
//  From    %2
//  Because Script-based Popup
//
	SNMC(EV_HTTP_SCRIPTBASEDPOPUP_REMOVED),

//
// MessageId: SN_EV_HTTP_USERAGENT_BLOCKED
//
// MessageText:
//
//  Blocked User-Agent: %1 sent for %2
//
	SNMC(EV_HTTP_USERAGENT_BLOCKED),

//
// MessageId: SN_EV_HTTP_USERAGENT_ALLOWED
//
// MessageText:
//
//  Allowed User-Agent: %1 sent to %2
//
	SNMC(EV_HTTP_USERAGENT_ALLOWED),

//
// MessageId: SN_EV_HTTP_REFERER_BLOCKED
//
// MessageText:
//
//  Blocked Referer: %1 sent for %2
//
	SNMC(EV_HTTP_REFERER_BLOCKED),

//
// MessageId: SN_EV_HTTP_REFERER_ALLOWED
//
// MessageText:
//
//  Allowed Referer: %1 sent to %2
//
	SNMC(EV_HTTP_REFERER_ALLOWED),

//
// MessageId: SN_EV_HTTP_FROM_BLOCKED
//
// MessageText:
//
//  Blocked From: %1 sent for %2
//
	SNMC(EV_HTTP_FROM_BLOCKED),

//
// MessageId: SN_EV_HTTP_FROM_ALLOWED
//
// MessageText:
//
//  Allowed From: %1 sent to %2
//
	SNMC(EV_HTTP_FROM_ALLOWED),

//
// MessageId: SN_EV_HTTP_COOKIE_BLOCKED
//
// MessageText:
//
//  Blocked Cookie: %1 sent for %2
//
	SNMC(EV_HTTP_COOKIE_BLOCKED),

//
// MessageId: SN_EV_HTTP_COOKIE_ALLOWED
//
// MessageText:
//
//  Allowed Cookie: %1 sent to %2
//
	SNMC(EV_HTTP_COOKIE_ALLOWED),

//
// MessageId: SN_EV_IP_FILTER_INCOMING_UDP_EXCEPTION
//
// MessageText:
//
//  Inbound UDP packet allowed.
//  Local address,service is (%1,%2)
//  Remote address,service is (%3,%4)
//  Process name is "%5"
//
	SNMC(EV_IP_FILTER_INCOMING_UDP_EXCEPTION),

//
// MessageId: SN_EV_IP_FILTER_PORT_SCAN_DETECTED_NOT_BLOCKED
//
// MessageText:
//
//  Port scan detected from address %1 At least %2 ports being probed.
//
	SNMC(EV_IP_FILTER_PORT_SCAN_DETECTED_NOT_BLOCKED),

//
// MessageId: SN_EV_IP_FILTER_ALLOW_RULE_PRESENT_FOR_LISTEN
//
// MessageText:
//
//  Rule "%1" permitted (%2,%3).
//  Process name is "%4"
//
	SNMC(EV_IP_FILTER_ALLOW_RULE_PRESENT_FOR_LISTEN),

//
// MessageId: SN_EV_IP_FILTER_REJECT_RULE_PRESENT_FOR_LISTEN
//
// MessageText:
//
//  Rule "%1" blocked (%2,%3).
//  Process name is "%4"
//
	SNMC(EV_IP_FILTER_REJECT_RULE_PRESENT_FOR_LISTEN),

//
// MessageId: SN_EV_IP_FILTER_IGNORE_RULE_PRESENT_FOR_LISTEN
//
// MessageText:
//
//  Rule "%1" ignored (%2,%3).
//  Process name is "%4"
//
	SNMC(EV_IP_FILTER_IGNORE_RULE_PRESENT_FOR_LISTEN),

//
// MessageId: SN_EV_IP_FILTER_DEFAULT_RULES_RESTORED
//
// MessageText:
//
//  Missing or damaged firewall rules. Default rules restored.
//
	SNMC(EV_IP_FILTER_DEFAULT_RULES_RESTORED),

//
// MessageId: SN_EV_IP_FILTER_STORAGE_CORRUPTED
//
// MessageText:
//
//  Damaged configuration settings. Default setting restored.
//
	SNMC(EV_IP_FILTER_STORAGE_CORRUPTED),

//---------------------------------------------------------------------------
// N D I S    F I L T E R I N G    M E S S A G E S
//---------------------------------------------------------------------------
//
// MessageId: SN_EV_NDIS_FILTER_INTERNAL_ERROR
//
// MessageText:
//
//  While processing an incoming IP packet, a corrupted packet was encountered
//
	SNMC(EV_NDIS_FILTER_INTERNAL_ERROR),

//
// MessageId: SN_EV_NDIS_FILTER_ACTIVE
//
// MessageText:
//
//  NDIS filtering is enabled
//
	SNMC(EV_NDIS_FILTER_ACTIVE),

// The following message has been obsoleted in Spartacus
//
// MessageId: SN_EV_NDIS_FILTER_BLOCK_IP_FRAGMENTS
//
// MessageText:
//
//  Inbound IP fragments are being blocked
//
	SNMC(EV_NDIS_FILTER_BLOCK_IP_FRAGMENTS),

//
// MessageId: SN_EV_NDIS_FILTER_BLOCKED_IP_FRAGMENT
//
// MessageText:
//
//  Blocked inbound IP fragment.
//  Protocol "%1"
//  Remote address (%2) 
//  Local address (%3)
//
	SNMC(EV_NDIS_FILTER_BLOCKED_IP_FRAGMENT),

// The following message has been obsoleted in Spartacus, since the one below also
// displays the IP addresses
//
// MessageId: SN_EV_NDIS_FILTER_BLOCK_IGMP
//
// MessageText:
//
//  Inbound IGMP packets are being blocked.
//
	SNMC(EV_NDIS_FILTER_BLOCK_IGMP),

//
// MessageId: SN_EV_NDIS_FILTER_BLOCKED_IGMP
//
// MessageText:
//
//  Blocked inbound IGMP packet.
//  Remote address: %1.
//  Local address: %2.
//
	SNMC(EV_NDIS_FILTER_BLOCKED_IGMP),

//
// MessageId: SN_EV_NDIS_FILTER_INVALID_IPHEADER_LENGTH
//
// MessageText:
//
//  Invalid IP Header Length: %1 bytes. Packet has been dropped.
//
	SNMC(EV_NDIS_FILTER_INVALID_IPHEADER_LENGTH),

//
// MessageId: SN_EV_NDIS_FILTER_HLEN_GREATER_TOTALLEN
//
// MessageText:
//
//  Invalid IP Packet. The IP Header Length indicates a value of %3 bytes which is greater than IP Total Length of %4 bytes.  
//  Packet has been dropped.
//  Source IP address: %1.
//  Destination IP address: %2.
//
	SNMC(EV_NDIS_FILTER_HLEN_GREATER_TOTALLEN),

//
// MessageId: SN_EV_IDS_FILTER_IP_FLAGS_INVALID
//
// MessageText:
//
//  Invalid IP Flags "%3".  Packet has been dropped.
//  Source IP address: %1.
//  Destination IP address: %2.
//
	SNMC(EV_IDS_FILTER_IP_FLAGS_INVALID),

//
// MessageId: SN_EV_NDIS_FILTER_FRAGMENT_OFFSET_INVALID
//
// MessageText:
//
//  Invalid fragment offset: %3.  Packet has been dropped.
//  Source IP address: %1.
//  Destination IP address: %2.
//  Protocol: %4.
//
	SNMC(EV_NDIS_FILTER_FRAGMENT_OFFSET_INVALID),

//
// MessageId: SN_EV_NDIS_FILTER_FIRST_FRAGMENT_TOO_SMALL
//
// MessageText:
//
//  First fragment too small: %3 bytes.  Packet has been dropped.
//  Source IP address: %1.
//  Destination IP address: %2.
//  Protocol: %4.
//
	SNMC(EV_NDIS_FILTER_FIRST_FRAGMENT_TOO_SMALL),

//
// MessageId: SN_EV_IDS_FILTER_IP_CHECKSUM_INVALID
//
// MessageText:
//
//  IP Checksum Invalid. Expected: %3. Actual: %4. Packet has been dropped.
//  Source IP address: %1.
//  Destination IP address: %2.
//
	SNMC(EV_IDS_FILTER_IP_CHECKSUM_INVALID),

//
// MessageId: SN_EV_IDS_FILTER_SOURCE_IP_INVALID
//
// MessageText:
//
//  Invalid IP Address: %3.  Packet has been dropped.
//  Source IP address: %1.
//  Destination IP address: %2.
//
	SNMC(EV_IDS_FILTER_SOURCE_IP_INVALID),

//
// MessageId: SN_EV_IDS_FILTER_DEST_IP_INVALID
//
// MessageText:
//
//  Invalid IP Address: %3.  Packet has been dropped.
//  Source IP address: %1.
//  Destination IP address: %2.
//
	SNMC(EV_IDS_FILTER_DEST_IP_INVALID),

//
// MessageId: SN_EV_IDS_FILTER_IP_OPTIONS_INVALID
//
// MessageText:
//
//  Invalid IP Options.  Packet has been dropped.
//  Source IP address: %1.
//  Destination IP address: %2.
//
	SNMC(EV_IDS_FILTER_IP_OPTIONS_INVALID),

//
// MessageId: SN_EV_IDS_FILTER_TCP_SRC_PORT_INVALID
//
// MessageText:
//
//  TCP Source Port Invalid: %3.  Packet has been dropped.
//  Source IP address: %1.
//  Destination IP address: %2.
//
	SNMC(EV_IDS_FILTER_TCP_SRC_PORT_INVALID),

//
// MessageId: SN_EV_IDS_FILTER_TCP_DEST_PORT_INVALID
//
// MessageText:
//
//  TCP Destination Port Invalid: %4.  Packet has been dropped.
//  Source IP address: %1.
//  Destination IP address: %2.
//
	SNMC(EV_IDS_FILTER_TCP_DEST_PORT_INVALID),

//
// MessageId: SN_EV_NDIS_FILTER_TCP_HLEN_INVALID
//
// MessageText:
//
//  TCP Header Length invalid: %5.  Packet has been dropped.
//  Source IP address: %1.
//  Destination IP address: %2.
//  TCP Source Port: %3.
//  TCP Destination Port: %4.
//
	SNMC(EV_NDIS_FILTER_TCP_HLEN_INVALID),

//
// MessageId: SN_EV_IDS_FILTER_TCP_FLAGS_INVALID
//
// MessageText:
//
//  TCP Flags invalid: %5.  Packet has been dropped.
//  Source IP address: %1.
//  Destination IP address: %2.
//  TCP Source Port: %3.
//  TCP Destination Port: %4.
//
	SNMC(EV_IDS_FILTER_TCP_FLAGS_INVALID),

//
// MessageId: SN_EV_IDS_FILTER_TCP_OPTIONS_INVALID
//
// MessageText:
//
//  TCP Options invalid.  Packet has been dropped.
//  Source IP address: %1.
//  Destination IP address: %2.
//  TCP Source Port: %3.
//  TCP Destination Port: %4.
//
	SNMC(EV_IDS_FILTER_TCP_OPTIONS_INVALID),

//
// MessageId: SN_EV_NDIS_FILTER_UDP_HEADER_INVALID
//
// MessageText:
//
//  Invalid UDP Header Length.  Packet has been dropped.
//  Source IP address: %1.
//  Destination IP address: %2.
//
	SNMC(EV_NDIS_FILTER_UDP_HEADER_INVALID),

//
// MessageId: SN_EV_IDS_FILTER_UDP_SRC_PORT_INVALID
//
// MessageText:
//
//  UDP Source Port invalid.  Packet has been dropped.
//  Source IP address: %1.
//  Destination IP address: %2.
//  UDP Source Port: %3.
//  UDP Destination Port: %4.
//
	SNMC(EV_IDS_FILTER_UDP_SRC_PORT_INVALID),

//
// MessageId: SN_EV_IDS_FILTER_UDP_DEST_PORT_INVALID
//
// MessageText:
//
//  UDP Destination Port invalid.  Packet has been dropped.
//  Source IP address: %1.
//  Destination IP address: %2.
//  UDP Source Port: %3
//  UDP Destination Port: %4
//
	SNMC(EV_IDS_FILTER_UDP_DEST_PORT_INVALID),

//
// MessageId: SN_EV_NDIS_FILTER_UDP_LENGTH_INVALID
//
// MessageText:
//
//  UDP Length invalid: %5.  Packet has been dropped.
//  Source IP address: %1.
//  Destination IP address: %2.
//  UDP Source Port: %3.
//  UDP Destination Port: %4.
//
	SNMC(EV_NDIS_FILTER_UDP_LENGTH_INVALID),

//
// MessageId: SN_EV_NDIS_FILTER_ICMP_LENGTH_INVALID
//
// MessageText:
//
//  ICMP Header length invalid: %3 bytes.  Packet has been dropped.
//  Source IP address: %1.
//  Destination IP address: %2.
//
	SNMC(EV_NDIS_FILTER_ICMP_LENGTH_INVALID),

//
// MessageId: SN_EV_IDS_FILTER_ICMP_CODE_INVALID
//
// MessageText:
//
//  ICMP Code invalid: %4.  Packet has been dropped.
//  Source IP address: %1.
//  Destination IP address: %2.
//  ICMP Type: %3
//
	SNMC(EV_IDS_FILTER_ICMP_CODE_INVALID),

//
// MessageId: SN_EV_IDS_FILTER_ICMP_TYPE_INVALID
//
// MessageText:
//
//  ICMP Type invalid: %3.  Packet has been dropped.
//  Source IP address: %1.
//  Destination IP address: %2.
//  ICMP Code: %4.
//
	SNMC(EV_IDS_FILTER_ICMP_TYPE_INVALID),

//
// MessageId: SN_EV_NDIS_FILTER_LAND
//
// MessageText:
//
//  Source IP address is equal to Destination IP address. Possible "Land" attack.  Packet has been dropped.
//  Source IP address: %1.
//  Destination IP address: %2.
//  TCP Source Port: %3.
//  TCP Destination Port: %4.
//
	SNMC(EV_NDIS_FILTER_LAND),

//
// MessageId: SN_EV_NDIS_FILTER_LATIERRA
//
// MessageText:
//
//  Source IP address is equal to Destination IP address. Possible "La Tierra" attack.  Packet has been dropped.
//  Source IP address: %1.
//  Destination IP address: %2.
//  TCP Source Port: %3
//  TCP Destination Port: %4
//
	SNMC(EV_NDIS_FILTER_LATIERRA),

//
// MessageId: SN_EV_NDIS_FILTER_SRCDEST_IP_INVALID
//
// MessageText:
//
//  Source IP address is equal to Destination IP address. Packet has been dropped.
//  Source IP address: %1.
//  Destination IP address: %2.
//  TCP Source Port: %3
//  TCP Destination Port: %4
//
	SNMC(EV_NDIS_FILTER_SRCDEST_IP_INVALID),

//
// MessageId: SN_EV_NDIS_FILTER_IP_TOO_LARGE
//
// MessageText:
//
//  Invalid IP packet. Fragment overflows the 64K limit on IP packets. Packet has been dropped.
//  Source IP address: %1.
//  Destination IP address: %2.
//  IP Payload length: %3.
//  Fragment offset: %4.
//  Protocol: %5.
//
	SNMC(EV_NDIS_FILTER_IP_TOO_LARGE),

//
// MessageId: SN_EV_NDIS_FILTER_JOLT
//
// MessageText:
//
//  Invalid IP packet. Fragment overflows the 64K limit on IP packets. Jolt attack. Packet has been dropped.
//  Source IP address: %1.
//  Destination IP address: %2.
//  IP Payload length: %3.
//  Fragment offset: %4.
//  Protocol: %5.
//
	SNMC(EV_NDIS_FILTER_JOLT),

//
// MessageId: SN_EV_NDIS_FILTER_JOLT2
//
// MessageText:
//
//  Invalid IP packet. Fragment overflows the 64K limit on IP packets. Jolt2 attack. Packet has been dropped.
//  Source IP address: %1.
//  Destination IP address: %2.
//  IP Payload length: %3.
//  Fragment offset: %4.
//  Protocol: %5.
//
	SNMC(EV_NDIS_FILTER_JOLT2),

//
// MessageId: SN_EV_NDIS_FILTER_PING_OF_DEATH
//
// MessageText:
//
//  Invalid IP packet. Fragment overflows the 64K limit on IP packets. Ping of death attack. Packet has been dropped.
//  Source IP address: %1.
//  Destination IP address: %2.
//  IP Payload length: %3.
//  Fragment offset: %4.
//  Protocol: %5.
//
	SNMC(EV_NDIS_FILTER_PING_OF_DEATH),

//
// MessageId: SN_EV_IDS_FILTER_TCP_FLAGS_INVALID_NMAP_NULL
//
// MessageText:
//
//  TCP Header Flags invalid: %5. Nmap Null scan. Packet has been dropped.
//  Source IP address: %1.
//  Destination IP address: %2.
//  TCP Source Port: %3
//  TCP Destination Port: %4
//
	SNMC(EV_IDS_FILTER_TCP_FLAGS_INVALID_NMAP_NULL),

//
// MessageId: SN_EV_IDS_FILTER_TCP_FLAGS_INVALID_NMAP_XMAS
//
// MessageText:
//
//  TCP Header Flags invalid: %5.  Nmap Xmas scan. Packet has been dropped.
//  Source IP address: %1.
//  Destination IP address: %2.
//  TCP Source Port: %3
//  TCP Destination Port: %4
//
	SNMC(EV_IDS_FILTER_TCP_FLAGS_INVALID_NMAP_XMAS),

//
// MessageId: SN_EV_NDIS_FILTER_INVALID_ARP_HWSIZE
//
// MessageText:
//
//  Invalid Hardware Size in ARP header: %1. Packet has been dropped.
//
	SNMC(EV_NDIS_FILTER_INVALID_ARP_HWSIZE),

//
// MessageId: SN_EV_NDIS_FILTER_INVALID_ARP_PROTSIZE
//
// MessageText:
//
//  Invalid Protocol Size in ARP header: %1. Packet has been dropped.
//
	SNMC(EV_NDIS_FILTER_INVALID_ARP_PROTSIZE),

//
// MessageId: SN_EV_NDIS_FILTER_BLOCKED_TCP_ON_CLOSED_PORT
//
// MessageText:
//
//  Unexpected TCP Packet. Packet has been dropped.
//  Source IP address: %1.
//  Destination IP address: %2.
//  TCP Source Port: %3
//  TCP Destination Port: %4
//
	SNMC(EV_NDIS_FILTER_BLOCKED_TCP_ON_CLOSED_PORT),

//
// MessageId: SN_EV_NDIS_FILTER_BLOCKED_SYN_ON_NONLISTENING_PORT
//
// MessageText:
//
//  TCP Syn Packet on non-listening port. Packet has been dropped.
//  Source IP address: %1.
//  Destination IP address: %2.
//  TCP Source Port: %3
//  TCP Destination Port: %4
//
	SNMC(EV_NDIS_FILTER_BLOCKED_SYN_ON_NONLISTENING_PORT),

//
// MessageId: SN_EV_NDIS_FILTER_BLOCKED_PACKET_ON_INVALID_CONNECTION
//
// MessageText:
//
//  TCP non-syn/non-ack packet on invalid connection. Packet has been dropped.
//  Source IP address: %1.
//  Destination IP address: %2.
//  TCP Source Port: %3
//  TCP Destination Port: %4
//
	SNMC(EV_NDIS_FILTER_BLOCKED_PACKET_ON_INVALID_CONNECTION),

//
// MessageId: SN_EV_NDIS_FILTER_TOTALLEN_GREATER_PKTSIZE
//
// MessageText:
//
//  Invalid IP Packet. The IP Total Length field indicates a value of %4 bytes which is greater than the size of the packet.
//  Packet has been dropped.
//  Source IP address: %1.
//  Destination IP address: %2.
//  IP Header Length: %3
//
	SNMC(EV_NDIS_FILTER_TOTALLEN_GREATER_PKTSIZE),

//
// MessageId: SN_EV_NDIS_NETWORK_DISCONNECTED
//
// MessageText:
//
//  At the user's request, the network has been disconnected from this machine.
//
	SNMC(EV_NDIS_NETWORK_DISCONNECTED),

//
// MessageId: SN_EV_NDIS_NETWORK_RECONNECTED
//
// MessageText:
//
//  At the user's request, the network has been reconnected to this machine.
//
	SNMC(EV_NDIS_NETWORK_RECONNECTED),

//
// MessageId: SN_EV_NDIS_UNCMNPROTOCOL_NOW_ALLOWED
//
// MessageText:
//
//  Uncommon Protocol %1 is now allowed.
//
	SNMC(EV_NDIS_UNCMNPROTOCOL_NOW_ALLOWED),
//
// MessageId: SN_EV_NDIS_UNCMNPROTOCOL_NOW_BLOCKED
//
// MessageText:
//
//  Uncommon Protocol %1 is now blocked.
//
	SNMC(EV_NDIS_UNCMNPROTOCOL_NOW_BLOCKED),
//
// MessageId: SN_EV_NDIS_UNCMNPROTOCOL_ALLOW_ALL
//
// MessageText:
//
//  All Uncommon Protocols are now allowed.
//
	SNMC(EV_NDIS_UNCMNPROTOCOL_ALLOW_ALL),
//
// MessageId: SN_EV_NDIS_UNCMNPROTOCOL_BLOCK_ALL
//
// MessageText:
//
//  All Uncommon Protocols are now blocked.
//
	SNMC(EV_NDIS_UNCMNPROTOCOL_BLOCK_ALL),
//
// MessageId: SN_EV_NDIS_UNCMNPROTOCOL_ALLOWED
//
// MessageText:
//
//  An Uncommon Protocol packet %1 was allowed.
//
	SNMC(EV_NDIS_UNCMNPROTOCOL_ALLOWED),

//
// MessageId: SN_EV_NDIS_UNCMNPROTOCOL_BLOCKED
//
// MessageText:
//
//  An Uncommon Protocol packet %1 was blocked.
//
	SNMC(EV_NDIS_UNCMNPROTOCOL_BLOCKED),

//---------------------------------------------------------------------------
//    I A M S E R V		M e s s a g e s
//---------------------------------------------------------------------------
//
//
// MessageId: SN_EV_IAMSERV_STARTED_AS_SERVICE
//
// MessageText:
//
//  NISServ started as Windows Service.
//
	SNMC(EV_IAMSERV_STARTED_AS_SERVICE),

//
// MessageId: SN_EV_IAMSERV_STRING
//
// MessageText:
//
//  %1
//
	SNMC(EV_IAMSERV_STRING),

//
// MessageId: SN_EV_IAMSERV_REMOTE_CONFIG_OBTAINED
//
// MessageText:
//
//  Successfully obtained remote configuration update.
//
	SNMC(EV_IAMSERV_REMOTE_CONFIG_OBTAINED),

//
// MessageId: SN_EV_IAMSERV_REMOTE_CONFIG_CURRENT
//
// MessageText:
//
//  Local configuration is identical to remote configuration
//
	SNMC(EV_IAMSERV_REMOTE_CONFIG_CURRENT),

//
// MessageId: SN_EV_IAMSERV_REMOTE_CONFIG_FAILED
//
// MessageText:
//
//  Obtaining remote configuration update failed.
//  Internal error is "%1" failure
//
	SNMC(EV_IAMSERV_REMOTE_CONFIG_FAILED),

//
// MessageId: SN_EV_IAMSERV_REMOTE_CONFIG_POPULATED
//
// MessageText:
//
//  Successfully populated remote configuration update.
//
	SNMC(EV_IAMSERV_REMOTE_CONFIG_POPULATED),

//
// MessageId: SN_EV_IAMSERV_FILTER_FIREWALL_REFRESH_FAILED
//
// MessageText:
//
//  Firewall refresh ioctl call failed.
//
	SNMC(EV_IAMSERV_FILTER_FIREWALL_REFRESH_FAILED),

//
// MessageId: SN_EV_IAMSERV_FILTER_HTTP_REFRESH_FAILED
//
// MessageText:
//
//  Http refresh ioctl call failed.
//
	SNMC(EV_IAMSERV_FILTER_HTTP_REFRESH_FAILED),

//
// MessageId: SN_EV_IAMSERV_FILTER_GET_REMOTE_CONFIGURATION_FAILED
//
// MessageText:
//
//  Get remote configuration update ioctl called failed.
//
	SNMC(EV_IAMSERV_FILTER_GET_REMOTE_CONFIGURATION_FAILED),

//
// MessageId: SN_EV_IAMSERV_CONFIG_FILE_OPEN_FAILED
//
// MessageText:
//
//  Local copy of configuration file "%1" could not be opened for reading.
//
	SNMC(EV_IAMSERV_CONFIG_FILE_OPEN_FAILED),

//
// MessageId: SN_EV_IAMSERV_CONFIG_FILE_MISSING_REGEDIT4
//
// MessageText:
//
//  First line of configuration file must be REGEDIT4
//
	SNMC(EV_IAMSERV_CONFIG_FILE_MISSING_REGEDIT4),

//
// MessageId: SN_EV_IAMSERV_CONFIG_FILE_EXTENDED_LINE_MISSING
//
// MessageText:
//
//  The line "%1" ends in a backslash, meaning that it is to be continued 
//  on the next line. The next line could not be read.
//
	SNMC(EV_IAMSERV_CONFIG_FILE_EXTENDED_LINE_MISSING),

//
// MessageId: SN_EV_IAMSERV_CONFIG_FILE_CLOSE_FAILED
//
// MessageText:
//
//  Local copy of configuration file "%1" could not be closed correctly.
//
	SNMC(EV_IAMSERV_CONFIG_FILE_CLOSE_FAILED),

//
// MessageId: SN_EV_IAMSERV_CONFIG_FILE_MISSING_SQUARE_BRACKET
//
// MessageText:
//
//  The line "%1" appears to be a subkey, but does not have a closing ].
//
	SNMC(EV_IAMSERV_CONFIG_FILE_MISSING_SQUARE_BRACKET),

//
// MessageId: SN_EV_IAMSERV_CONFIG_FILE_MISSING_BACKSLASH
//
// MessageText:
//
//  The subkey "%1" is not fully qualified with backslashes separating each key.
//
	SNMC(EV_IAMSERV_CONFIG_FILE_MISSING_BACKSLASH),

//
// MessageId: SN_EV_IAMSERV_CONFIG_FILE_INVALID_BASE_KEY
//
// MessageText:
//
//  The subkey "%1" is not a valid subkey for configuration information.
//
	SNMC(EV_IAMSERV_CONFIG_FILE_INVALID_BASE_KEY),

//
// MessageId: SN_EV_IAMSERV_CONFIG_FILE_INVALID_ROOT_KEY
//
// MessageText:
//
//  The root key "%1" is not valid for configuration information.
//  Only HKEY_LOCAL_USER and HKEY_LOCAL_MACHINE are valid.
//
	SNMC(EV_IAMSERV_CONFIG_FILE_INVALID_ROOT_KEY),

//
// MessageId: SN_EV_IAMSERV_CONFIG_FILE_COULD_NOT_CREATE_SUBKEY
//
// MessageText:
//
//  The registry could not create the subkey "%1".
//
	SNMC(EV_IAMSERV_CONFIG_FILE_COULD_NOT_CREATE_SUBKEY),

//
// MessageId: SN_EV_IAMSERV_CONFIG_FILE_INVALID_LINE
//
// MessageText:
//
//  The line "%1" is not a valid subkey, name-value pair, comment or blank line.
//
	SNMC(EV_IAMSERV_CONFIG_FILE_INVALID_LINE),

//
// MessageId: SN_EV_IAMSERV_CONFIG_FILE_SUBKEY_NOT_SPECIFIED
//
// MessageText:
//
//  The line "%1" appears to be a name-value pair, but no subkey has been specified.
//
	SNMC(EV_IAMSERV_CONFIG_FILE_SUBKEY_NOT_SPECIFIED),

//
// MessageId: SN_EV_IAMSERV_CONFIG_FILE_EQUALS_MISSING
//
// MessageText:
//
//  The line "%1" appears to be a name-value pair, but there is no equals sign.
//
	SNMC(EV_IAMSERV_CONFIG_FILE_EQUALS_MISSING),

//
// MessageId: SN_EV_IAMSERV_CONFIG_FILE_COULD_NOT_CREATE_DWORD
//
// MessageText:
//
//  The dword value specified by the line "%1" could not be created.
//
	SNMC(EV_IAMSERV_CONFIG_FILE_COULD_NOT_CREATE_DWORD),

//
// MessageId: SN_EV_IAMSERV_CONFIG_FILE_INVALID_BYTE_VALUE
//
// MessageText:
//
//  The line "%1" contains an invalid byte value. All hex (binary), hex(2) (expand_sz),
//  and hex(7) (multi_sz) values must be specified as a series of comma delimited bytes.
//  Each byte must be represented as a 2 digit hexidecimal value.
//
	SNMC(EV_IAMSERV_CONFIG_FILE_INVALID_BYTE_VALUE),

//
// MessageId: SN_EV_IAMSERV_CONFIG_FILE_COULD_NOT_CREATE_EXPAND_SZ
//
// MessageText:
//
//  The expand_sz value specified by the line "%1" could not be created.
//
	SNMC(EV_IAMSERV_CONFIG_FILE_COULD_NOT_CREATE_EXPAND_SZ),

//
// MessageId: SN_EV_IAMSERV_CONFIG_FILE_COULD_NOT_CREATE_MULTI_SZ
//
// MessageText:
//
//  The multi_sz value specified by the line "%1" could not be created.
//
	SNMC(EV_IAMSERV_CONFIG_FILE_COULD_NOT_CREATE_MULTI_SZ),

//
// MessageId: SN_EV_IAMSERV_CONFIG_FILE_COULD_NOT_CREATE_BINARY
//
// MessageText:
//
//  The binary value specified by the line "%1" could not be created.
//
	SNMC(EV_IAMSERV_CONFIG_FILE_COULD_NOT_CREATE_BINARY),

//
// MessageId: SN_EV_IAMSERV_CONFIG_FILE_UNKNOWN_VALUE_TYPE
//
// MessageText:
//
//  The line "%1" appears to be a name-value pair of an unknown value type. Valid
//  types are dword, hex (binary), hex(2) (expand_sz), hex(7) (multi_sz) and quoted 
//  strings.
//
	SNMC(EV_IAMSERV_CONFIG_FILE_UNKNOWN_VALUE_TYPE),

//
// MessageId: SN_EV_IAMSERV_CONFIG_FILE_COULD_NOT_CREATE_STRING
//
// MessageText:
//
//  The string value specified by the line "%1" could not be created.
//
	SNMC(EV_IAMSERV_CONFIG_FILE_COULD_NOT_CREATE_STRING),

//
// MessageId: SN_EV_IAMSERV_STOPPED_AS_SERVICE
//
// MessageText:
//
//  NISServ stopped as Windows Service.
//
	SNMC(EV_IAMSERV_STOPPED_AS_SERVICE),

//
// MessageId: SN_EV_IAMSERV_SECURITY_ALERT_DISPLAYED
//
// MessageText:
//
//  Security alert displayed for rule %1.
//  Remote computer (%2, %3)
//
	SNMC(EV_IAMSERV_SECURITY_ALERT_DISPLAYED),

//
// MessageId: SN_EV_IAMSERV_IP_ADDRESS_CHANGE
//
// MessageText:
//
//  Protecting your connection to a newly detected network on adapter "%1" (IP address: %2).
//
	SNMC(EV_IAMSERV_IP_ADDRESS_CHANGE),
//
// MessageId: EV_IAMSERV_IP_ADDRESS_GONE
//
// MessageText:
//
//  IP address %1 has disappeared and is no longer being protected.
//
	SNMC(EV_IAMSERV_IP_ADDRESS_GONE),

//
// MessageId: SN_EV_IP_FILTER_BLACKED_OUT
//
// MessageText:
//
//  %1 is blocked for Internet access at this time.
//  Category: %2.
//  Session: %3.
//
	SNMC(EV_IP_FILTER_BLACKED_OUT),

//
// MessageId: SN_EV_IP_FILTER_TIME_LIMIT_REACHED
//
// MessageText:
//
//  You have reached the maximum time allotted to use %1.
//  Category: %2.
//  Session: %3.
//
	SNMC(EV_IP_FILTER_TIME_LIMIT_REACHED),

//
// MessageId: SN_EV_IP_FILTER_SYMC_APP_ALLOWED
//
// MessageText:
//
//  The following Symantec application was implicitly allowed to communicate:
//  Application: %1
//  Protocol: %2
//  Local IP address,port: %3,%4
//  Remote IP address,port: %5,%6
//
	SNMC(EV_IP_FILTER_SYMC_APP_ALLOWED),

//
// MessageId: SN_EV_IP_FILTER_SYMC_APP_ALLOWED_LISTEN
//
// MessageText:
//
//  The following Symantec application was implicitly allowed to listen:
//  Application: %1
//  Local IP address,port: %3,%4
//
	SNMC(EV_IP_FILTER_SYMC_APP_ALLOWED_LISTEN),

//
// MessageId: SN_EV_IP_FILTER_SYMC_APP_ALLOWED_ACCEPT
//
// MessageText:
//
//  The following Symantec application was implicitly allowed to accept an incoming TCP connection:
//  Application: %1
//  Local IP address,port: %2,%3
//  Remote IP address,port: %4,%5
//
	SNMC(EV_IP_FILTER_SYMC_APP_ALLOWED_ACCEPT),

//
// MessageId: SN_EV_IP_FILTER_SYMC_APP_ALLOWED_CONNECT
//
// MessageText:
//
//  The following Symantec application was implicitly allowed outbound TCP connection:
//  Application: %1
//  Local IP address,port: %2,%3
//  Remote IP address,port: %4,%5
//
	SNMC(EV_IP_FILTER_SYMC_APP_ALLOWED_CONNECT),

//---------------------------------------------------------------------------
// X F E R   E v e n t   M e s s a g e
//---------------------------------------------------------------------------
//
// MessageId: SN_EV_IAMSERV_REMOTE_CONFIG_QUERY
//
// MessageText:
//
//  Attempting to obtain a remote configuration update.
//  Remote IP address is %1
//  Remote port is %2
//  Protocol is %3
//  Local file is "%4"
//  Remote file is "%5"
//
	SNMC(EV_IAMSERV_REMOTE_CONFIG_QUERY),

//
// MessageId: SN_EV_IAMSERV_REMOTE_TFTP_ERROR
//
// MessageText:
//
//  Remote TFTP error.
//  TFTP error is %1
//  Error string is "%2"
//
	SNMC(EV_IAMSERV_REMOTE_TFTP_ERROR),

//
// MessageId: SN_EV_IAMSERV_REMOTE_TFTP_TIMEOUT
//
// MessageText:
//
//  TFTP transfer timed out. 
//  TFTP packets sent %1
//  TFTP retransmit timeouts %2
//  TFTP packets received %3
//  TFTP valid packets received %4
//  TFTP packets received out of order %5
//  TFTP invalid data packet %6
//  TFTP invalid packet size %7
//  TFTP no room for packet %8
//
	SNMC(EV_IAMSERV_REMOTE_TFTP_TIMEOUT),

//
// MessageId: SN_EV_IAMSERV_REMOTE_HTTP_FAILURE
//
// MessageText:
//
//  HTTP transfer failed.
//  HTTP bytes send request %1
//  HTTP receive state %2
//  HTTP bytes received %3
//  HTTP received packets %4
//  HTTP header bytes received %5
//  HTTP data bytes received %6
//  HTTP transfer status %7
//  HTTP error status %8
//
	SNMC(EV_IAMSERV_REMOTE_HTTP_FAILURE),

//---------------------------------------------------------------------------
//    S t a t i s t i c s   E v e n t   M e s s a g e
//---------------------------------------------------------------------------
//
// MessageId: SN_EV_SYM_ERROR
//
// MessageText:
//
//  Error - %1
//
	SNMC(EV_SYM_ERROR),

//
// MessageId: SN_EV_SYM_RESTRICTED_SITE_BLOCKED
//
// MessageText:
//
//  Blocked access for account %1 to restricted site "%2" due to %3
//
	SNMC(EV_SYM_RESTRICTED_SITE_BLOCKED),

//
// MessageId: SN_EV_SYM_CONFIDENTIAL_BLOCKED
//
// MessageText:
//
//  Confidential data "%1" blocked to site "%2"
//
	SNMC(EV_SYM_CONFIDENTIAL_BLOCKED),

//
// MessageId: SN_EV_SYM_CONFIDENTIAL_PERMITTED
//
// MessageText:
//
//  Confidential data "%1" permitted to site "%2"
//
	SNMC(EV_SYM_CONFIDENTIAL_PERMITTED),

//
// MessageId: SN_EV_SYM_CONFIDENTIAL_BLOCKED_IM
//
// MessageText:
//
//  Confidential data "%1" blocked to remote computer at "%2"
//
	SNMC(EV_SYM_CONFIDENTIAL_BLOCKED_IM),

//
// MessageId: SN_EV_SYM_CONFIDENTIAL_PERMITTED_IM
//
// MessageText:
//
//  Confidential data "%1" permitted to remote comuter at "%2"
//
	SNMC(EV_SYM_CONFIDENTIAL_PERMITTED_IM),

//---------------------------------------------------------------------------
//    I A M C P L		M e s s a g e s
//---------------------------------------------------------------------------
//
//
// MessageId: SN_EV_IAMCPL_ALE_RULE_AUTO_CREATE
//
// MessageText:
//
//  A rule for "%1" was automatically created using a pre-configured rule
//
	SNMC(EV_IAMCPL_ALE_RULE_AUTO_CREATE),

//---------------------------------------------------------------------------
//    M i s c e l l a n e o u s   E v e n t   M e s s a g e s
//---------------------------------------------------------------------------
//
//
// MessageId: SN_EV_MISC_APPLICATION_ACCESSING_NETWORK
//
// MessageText:
//
//  An instance of "%1" is preparing to access the Internet
//
	SNMC(EV_MISC_APPLICATION_ACCESSING_NETWORK),

//---------------------------------------------------------------------------
//    A l e r t T r a c k e r		M e s s a g e s
//---------------------------------------------------------------------------
//
//
// MessageId: SN_EV_ATRACK_RULE_MATCHED
//
// MessageText:
//
//  Rule %1 matched
//  Remote address (%2,%3)
//
	SNMC(EV_ATRACK_RULE_MATCHED),

//
// MessageId: SN_EV_ATRACK_RULE_MATCHED_LISTEN
//
// MessageText:
//
//  Rule %1 matched
//  Local address (%2,%3)
//
	SNMC(EV_ATRACK_RULE_MATCHED_LISTEN),

//---------------------------------------------------------------------------
//    I D S	M e s s a g e s
//---------------------------------------------------------------------------
//
//
// MessageId: SN_EV_IDS_ALERT_DETECTED_ATTACKER_IS_REMOTE
//
// MessageText:
//
//  Attempted Intrusion "%1" against your machine was detected and blocked. 
//  Intruder: %4(%5). 
//  Protocol: %3.
//  Attacked Port: %7.
//
	SNMC(EV_IDS_ALERT_DETECTED_ATTACKER_IS_REMOTE),

//
// MessageId: SN_EV_IDS_ALERT_SRC_DEST_IP_EQUAL
//
// MessageText:
//
//  Attempted Intrusion "%1" against your machine was detected and blocked. 
//  Remote Port : %5. 
//  Local Port : %7. 
//  Protocol : %3.
//  Due to the nature of this attack, it is not possible to identify the attacker.
//
	SNMC(EV_IDS_ALERT_SRC_DEST_IP_EQUAL),

//
// MessageId: SN_EV_IDS_ALERT_DETECTED_ATTACKER_IS_LOCAL
//
// MessageText:
//
//  Attempted Intrusion "%1" from your machine against %6 was detected and blocked.
//  Protocol: %3.
//  Attacked Port: %7. 
//  Local Port: %5.
//
	SNMC(EV_IDS_ALERT_DETECTED_ATTACKER_IS_LOCAL),

//	MessageId = 1922
	SNMC(EV_IDS_FILTER_IP_FLAGS_INVALID),
//	Language=English
//	Invalid IP Flags "%3".  Packet has been dropped.
//	Source IP address: %1.
//	Destination IP address: %2.

//	MessageId = 1923
	SNMC(EV_IDS_FILTER_IP_CHECKSUM_INVALID),
//	Language=English
//	IP Checksum Invalid. Expected: %3. Actual: %4. Packet has been dropped.
//	Source IP address: %1.
//	Destination IP address: %2.

//	MessageId = 1924
	SNMC(EV_IDS_FILTER_SOURCE_IP_INVALID),
//	Language=English
//	Invalid IP Address: %3.  Packet has been dropped.
//	Source IP address: %1.
//	Destination IP address: %2.

//	MessageId = 1925
	SNMC(EV_IDS_FILTER_DEST_IP_INVALID),
//	Language=English
//	Invalid IP Address: %3.  Packet has been dropped.
//	Source IP address: %1.
//	Destination IP address: %2.

//	MessageId = 1926
	SNMC(EV_IDS_FILTER_IP_OPTIONS_INVALID),
//	Language=English
//	Invalid IP Options.  Packet has been dropped.
//	Source IP address: %1.
//	Destination IP address: %2.

//	MessageId = 1927
	SNMC(EV_IDS_FILTER_TCP_SRC_PORT_INVALID),
//	Language=English
//	TCP Source Port Invalid: %3.  Packet has been dropped.
//	Source IP address: %1.
//	Destination IP address: %2.

//	MessageId = 1928
	SNMC(EV_IDS_FILTER_TCP_DEST_PORT_INVALID),
//	Language=English
//	TCP Destination Port Invalid: %4.  Packet has been dropped.
//	Source IP address: %1.
//	Destination IP address: %2.

//	MessageId = 1929
	SNMC(EV_IDS_FILTER_TCP_FLAGS_INVALID),
//	Language=English
//	TCP Flags invalid: %5.  Packet has been dropped.
//	Source IP address: %1.
//	Destination IP address: %2.
//	TCP Source Port: %3.
//	TCP Destination Port: %4.

//	MessageId = 1930
	SNMC(EV_IDS_FILTER_TCP_OPTIONS_INVALID),
//	Language=English
//	TCP Options invalid.  Packet has been dropped.
//	Source IP address: %1.
//	Destination IP address: %2.
//	TCP Source Port: %3.
//	TCP Destination Port: %4.

//	MessageId = 1931
	SNMC(EV_IDS_FILTER_UDP_SRC_PORT_INVALID),
//	Language=English
//	UDP Source Port invalid.  Packet has been dropped.
//	Source IP address: %1.
//	Destination IP address: %2.
//	UDP Source Port: %3.
//	UDP Destination Port: %4.

//	MessageId = 1932
	SNMC(EV_IDS_FILTER_UDP_DEST_PORT_INVALID),
//	Language=English
//	UDP Destination Port invalid.  Packet has been dropped.
//	Source IP address: %1.
//	Destination IP address: %2.
//	UDP Source Port: %3
//	UDP Destination Port: %4

//	MessageId = 1933
	SNMC(EV_IDS_FILTER_ICMP_CODE_INVALID),
//	Language=English
//	ICMP Code invalid: %4.  Packet has been dropped.
//	Source IP address: %1.
//	Destination IP address: %2.
//	ICMP Type: %3

//	MessageId = 1934
	SNMC(EV_IDS_FILTER_ICMP_TYPE_INVALID),
//	Language=English
//	ICMP Type invalid: %3.  Packet has been dropped.
//	Source IP address: %1.
//	Destination IP address: %2.
//	ICMP Code: %4.

//	MessageId = 1935
	SNMC(EV_IDS_FILTER_TCP_FLAGS_INVALID_NMAP_NULL),
//	Language=English
//	TCP Header Flags invalid: %5. Nmap Null scan. Packet has been dropped.
//	Source IP address: %1.
//	Destination IP address: %2.
//	TCP Source Port: %3
//	TCP Destination Port: %4

//	MessageId = 1936
	SNMC(EV_IDS_FILTER_TCP_FLAGS_INVALID_NMAP_XMAS),
//	Language=English
//	TCP Header Flags invalid: %5.  Nmap Xmas scan. Packet has been dropped.
//	Source IP address: %1.
//	Destination IP address: %2.
//	TCP Source Port: %3
//	TCP Destination Port: %4

//	MessageId = 681
	SNMC(EV_IDS_FILTER_PORTSCAN),
//	Language=English
//	Intrusion	: %1.
//	Attacker	: %2.
//	Risk Level	: %3
//	At least %4 ports were probed.

//
// MessageId: SN_EV_IDS_PORT_SCAN_DETECTED
//
// MessageText:
//
//  Attempted Intrusion "%1" against your machine was detected and blocked. 
//  Intruder: %4. 
//  At least %10 ports were probed.
//
	SNMC(EV_IDS_PORT_SCAN_DETECTED),

//
// MessageId: SN_EV_IDS_ENABLED
//
// MessageText:
//
//  Intrusion Detection has been enabled.
//
	SNMC(EV_IDS_ENABLED),

//
// MessageId: SN_EV_IDS_DISABLED
//
// MessageText:
//
//  Intrusion Detection has been disabled.
//
	SNMC(EV_IDS_DISABLED),

//
// MessageId: SN_EV_IDS_NUM_SIG_MON
//
// MessageText:
//
//  Intrusion Detection (%2) is monitoring %1 signatures.
//
	SNMC(EV_IDS_NUM_SIG_MON),

//
// MessageId: SN_EV_IDS_EXCLUSIONS_KEY_NOT_FOUND
//
// MessageText:
//
//  An error occurred while loading the IDS Signature Exclusions. All signatures are being monitored.
//
	SNMC(EV_IDS_EXCLUSIONS_KEY_NOT_FOUND),

//
// MessageId: SN_EV_IDS_EXCLUSIONS_MALLOC_ERR
//
// MessageText:
//
//  Not enough memory available to read the IDS Signature Exclusions. All signatures are being monitored.
//
	SNMC(EV_IDS_EXCLUSIONS_MALLOC_ERR),

//
// MessageId: SN_EV_IDS_EXCLUSIONS_CORRUPT
//
// MessageText:
//
//  The list of IDS Signatures Exclusions is corrupt. Please re-install the product.
//
	SNMC(EV_IDS_EXCLUSIONS_CORRUPT),

//
// MessageId: SN_EV_IDS_SIG_STORE_KEY_NOT_FOUND
//
// MessageText:
//
//  The list of IDS signatures is corrupt. The IDS is not monitoring your machine.
//  Please re-install the product.
//
	SNMC(EV_IDS_SIG_STORE_KEY_NOT_FOUND),

//
// MessageId: SN_EV_IDS_SIG_STORE_CORRUPT
//
// MessageText:
//
//  The list of IDS signatures is corrupt. The IDS is not monitoring your machine.
//  Please re-install the product.
//
	SNMC(EV_IDS_SIG_STORE_CORRUPT),

//
// MessageId: SN_EV_IDS_SIG_STORE_MALLOC_ERR
//
// MessageText:
//
//  Not enough memory available to read the IDS Signatures.
//
	SNMC(EV_IDS_SIG_STORE_MALLOC_ERR),

//
// MessageId: SN_EV_IDS_ENGINE_INIT_ERROR
//
// MessageText:
//
//  An error occurred while starting the Intrusion Detection Module. Please re-install the product.
//
	SNMC(EV_IDS_ENGINE_INIT_ERROR),

//
// MessageId: SN_EV_IDS_SIGNATURES_UPDATED
//
// MessageText:
//
//  LiveUpdate has updated your IDS signatures.
//
	SNMC(EV_IDS_SIGNATURES_UPDATED),

//
// MessageId: SN_EV_IDS_ALERT_CAUSED_AUTOBLOCK
//
// MessageText:
//
//  Intrusion detected and blocked. All communication with %1 will be blocked for %2 minutes.
//
	SNMC(EV_IDS_ALERT_CAUSED_AUTOBLOCK),

//
// MessageId: SN_EV_IDS_NUM_SIG_MON_ZERO
//
// MessageText:
//
//  IDS is not monitoring any signatures. This is because the list of IDS signatures
//  is corrupt or you have excluded all the IDS signatures. If you are not aware of 
//  having excluded all the IDS signatures, please re-install the product.
//
	SNMC(EV_IDS_NUM_SIG_MON_ZERO),

//
// MessageId: SN_EV_SYMIDSCO_VER_INCOMPATIBLE
//
// MessageText:
//
//  An incompatible version of SymIDSCo was found. please re-install the product.
//
	SNMC(EV_SYMIDSCO_VER_INCOMPATIBLE),

// MessageId = 1920
// SymbolicName = EV_IDS_ALERT_DETECTED_ATTACKER_IS_REMOTE_NO_PORT
// Language=English
// Attempted Intrusion "%1" against your machine was detected and blocked. 
// Intruder: %4. 
// Protocol: %3.
	SNMC(EV_IDS_ALERT_DETECTED_ATTACKER_IS_REMOTE_NO_PORT),

// MessageId = 1921
// SymbolicName = EV_IDS_ALERT_DETECTED_ATTACKER_IS_LOCAL_NO_PORT
// Language=English
// Attempted Intrusion "%1" from your machine against %6 was detected and blocked.
// Protocol: %3.
	SNMC(EV_IDS_ALERT_DETECTED_ATTACKER_IS_LOCAL_NO_PORT),

// MessageId = 1938
// SymbolicName = EV_IDS_SETTINGS_FILE_CORRUPT
// Language=English
// The IDS configuration is corrupt. Default settings have been restored.
    SNMC (EV_IDS_SETTINGS_FILE_CORRUPT),

// MessageId = 1939
// SymbolicName = EV_IDS_DEFS_ENGINE_VERSION_INFO
// Language=English
// IDS Signature File Version: %1.
// IDS Engine Version: %2.
    SNMC (EV_IDS_DEFS_ENGINE_VERSION_INFO),


//---------------------------------------------------------------------------
//    N N T P	M e s s a g e s
//---------------------------------------------------------------------------
//
//
// MessageId: SN_EV_NNTP_GROUP_BLOCKED
//
// MessageText:
//
//  NNTP request to connect to newsgroup %1 was blocked
//
	SNMC(EV_NNTP_GROUP_BLOCKED),

//
// MessageId: SN_EV_NNTP_GROUP_REMOVED
//
// MessageText:
//
//  NNTP server response contained newsgroup: %1.  Newsgroup was removed from server response.
//
	SNMC(EV_NNTP_GROUP_REMOVED),

//
// MessageId: SN_EV_NNTP_POST_BLOCKED
//
// MessageText:
//
//  NNTP POST request to newsgroup: %1 was blocked.
//
	SNMC(EV_NNTP_POST_BLOCKED),

//---------------------------------------------------------------------------
//    S t a t i s t i c s   E v e n t   M e s s a g e
//---------------------------------------------------------------------------
//
//
// MessageId: SN_EV_STATS_CONNECTION
//
// MessageText:
//
//  Connection:  %1: %2  %3  %4: %5,  %6 bytes sent,  %7 bytes received,  %8 elapsed time
//
	SNMC(EV_STATS_CONNECTION),

//
// MessageId: SN_EV_STATS_REDIRECTED_CONNECTION
//
// MessageText:
//
//  Redirected Connection:  %1: %2  %3  %4: %5,  %6 bytes sent,  %7 bytes received,  %8 elapsed time
//
	SNMC(EV_STATS_REDIRECTED_CONNECTION),

//---------------------------------------------------------------------------
//    C o n f i g u r a t i o n   C h a n g e d / U p d a t e d   M e s s a g e s
//---------------------------------------------------------------------------
//
//
// MessageId: SN_EV_CONFIG_SETTINGS_PKG_UPDATED
//
// MessageText:
//
//  Settings package "%1" posted by admin on %2 installed successfully.
//  Package server: (%3,%4)
//  Package path: %5
//
	SNMC(EV_CONFIG_SETTINGS_PKG_UPDATED),

//
// MessageId: SN_EV_CONFIG_SETTINGS_PKG_DOWNLOAD_FAILED
//
// MessageText:
//
//  Settings package "%1" posted by admin on %2 could not be downloaded.
//  Package server: (%3,%4)
//  Package path: %5
//
	SNMC(EV_CONFIG_SETTINGS_PKG_DOWNLOAD_FAILED),

//
// MessageId: SN_EV_CONFIG_SETTINGS_PKG_VALIDATION_FAILED
//
// MessageText:
//
//  Settings package "%1" posted by admin on %2 failed validation.
//  Package server: (%3,%4)
//  Package path: %5
//
	SNMC(EV_CONFIG_SETTINGS_PKG_VALIDATION_FAILED),

//
// MessageId: SN_EV_CONFIG_SETTINGS_PKG_INSTALL_FAILED
//
// MessageText:
//
//  Settings package "%1" posted by admin on %2 failed to install.
//  Package server: (%3,%4)
//  Package path: %5
//
	SNMC(EV_CONFIG_SETTINGS_PKG_INSTALL_FAILED),

//
// MessageId: SN_EV_CONFIG_SETTINGS_PKG_FIREWALL_REFRESH_FAILED
//
// MessageText:
//
//  Firewall failed to refresh after installing settings package "%1" posted by admin on %2.
//  Package server: (%3,%4)
//  Package path: %5
//
	SNMC(EV_CONFIG_SETTINGS_PKG_FIREWALL_REFRESH_FAILED),

//
// MessageId: SN_EV_CONFIG_RULE_PKG_UPDATED
//
// MessageText:
//
//  Rule package "%1" posted by admin on %2 installed successfully.
//  Package server: (%3,%4)
//  Package path: %5
//
	SNMC(EV_CONFIG_RULE_PKG_UPDATED),

//
// MessageId: SN_EV_CONFIG_RULE_PKG_DOWNLOAD_FAILED
//
// MessageText:
//
//  Rule package "%1" posted by admin on %2 could not be downloaded.
//  Package server: (%3,%4)
//  Package path: %5
//
	SNMC(EV_CONFIG_RULE_PKG_DOWNLOAD_FAILED),

//
// MessageId: SN_EV_CONFIG_RULE_PKG_VALIDATION_FAILED
//
// MessageText:
//
//  Rule package "%1" posted by admin on %2 failed validation.
//  Package server: (%3,%4)
//  Package path: %5
//
	SNMC(EV_CONFIG_RULE_PKG_VALIDATION_FAILED),

//
// MessageId: SN_EV_CONFIG_RULE_PKG_INSTALL_FAILED
//
// MessageText:
//
//  Rule package "%1" posted by admin on %2 failed to install.
//  Package server: (%3,%4)
//  Package path: %5
//
	SNMC(EV_CONFIG_RULE_PKG_INSTALL_FAILED),

//
// MessageId: SN_EV_CONFIG_RULE_PKG_FIREWALL_REFRESH_FAILED
//
// MessageText:
//
//  Firewall failed to refresh after installing rule package "%1" posted by admin on %2.
//  Package server: (%3,%4)
//  Package path: %5
//
	SNMC(EV_CONFIG_RULE_PKG_FIREWALL_REFRESH_FAILED),

//
// MessageId: SN_EV_CONFIG_APP_UPDATED
//
// MessageText:
//
//  Application updated from version %1 to version %2.
//
	SNMC(EV_CONFIG_APP_UPDATED),

//
// MessageId: SN_EV_CONFIG_CHANGE_FIREWALL
//
// MessageText:
//
//  Firewall setting %1 changed
//  Old Value: %2
//  New Value: %3
//
	SNMC(EV_CONFIG_CHANGE_FIREWALL),

//
// MessageId: SN_EV_CONFIG_CHANGE_ADD_RESTRICTED
//
// MessageText:
//
//  Addresses added to the block list
//  Address count: %1
//
	SNMC(EV_CONFIG_CHANGE_ADD_RESTRICTED),

//
// MessageId: SN_EV_CONFIG_CHANGE_REMOVE_RESTRICTED
//
// MessageText:
//
//  Addresses removed from block list
//  First address: %1
//
	SNMC(EV_CONFIG_CHANGE_REMOVE_RESTRICTED),

//
// MessageId: SN_EV_CONFIG_CHANGE_USER_LOGGED_IN
//
// MessageText:
//
//  User logged in
//
	SNMC(EV_CONFIG_CHANGE_USER_LOGGED_IN),

//
// MessageId: SN_EV_CONFIG_CHANGE_USER_LOGGED_OUT
//
// MessageText:
//
//  User logged out
//
	SNMC(EV_CONFIG_CHANGE_USER_LOGGED_OUT),

//
// MessageId: SN_EV_CONFIG_CHANGE_APP_CATEGORY_CHANGE
//
// MessageText:
//
//  Application categories have changed
//
	SNMC(EV_CONFIG_CHANGE_APP_CATEGORY_CHANGE),

//
// MessageId: SN_EV_CONFIG_CHANGE_STATISTICS_CLEARED
//
// MessageText:
//
//  Statistics cleared
//  Statistics category identifier: %1
//
	SNMC(EV_CONFIG_CHANGE_STATISTICS_CLEARED),

//
// MessageId: SN_EV_CONFIG_CHANGE_IDS
//
// MessageText:
//
//  IDS setting %1 changed
//  Old Value: %2
//  New Value: %3
//
	SNMC(EV_CONFIG_CHANGE_IDS),

//
// MessageId: SN_EV_CONFIG_CHANGE_NDIS
//
// MessageText:
//
//  NDIS setting %1 changed
//  Old Value: %2
//  New Value: %3
//
	SNMC(EV_CONFIG_CHANGE_NDIS),

//
// MessageId: SN_EV_CONFIG_CHANGE_TDI
//
// MessageText:
//
//  TDI setting %1 changed
//  Old Value: %2
//  New Value: %3
//
	SNMC(EV_CONFIG_CHANGE_TDI),

//
// MessageId: EV_IP_STORAGE_PATH_CORRUPTED
//
// MessageText:
//
//  The configuration can not be loaded or saved. Please reinstall.
//
	SNMC(EV_IP_STORAGE_PATH_CORRUPTED),
//
// MessageId: EV_TDI_INITIALIZATION_FAILED
//
// MessageText:
//
//  Driver Verification Failed. Please reinstall.
//
	SNMC(EV_TDI_INITIALIZATION_FAILED),

//
// MessageId: EV_IP_FILTER_ENABLED
//
// MessageText:
//
//  Firewall has been enabled.
	SNMC(EV_IP_FILTER_ENABLED),

//
// MessageId: EV_IP_FILTER_DISABLED
//
// MessageText:
//
//  Firewall has been disabled.
	SNMC(EV_IP_FILTER_DISABLED),

//
// MessageId: EV_TDI_STARTUP_MODE_NETWORK
//
// MessageText:
//
//  Startup Mode has been set to Network
	SNMC(EV_TDI_STARTUP_MODE_NETWORK),


//
// MessageId: EV_TDI_STARTUP_MODE_AUTO
//
// MessageText:
//
//  Startup Mode has been set to Automatic
	SNMC(EV_TDI_STARTUP_MODE_AUTO),


//
// MessageId: EV_TDI_STARTUP_MODE_MANUAL
//
// MessageText:
//
//  Startup Mode has been set to Manual
	SNMC(EV_TDI_STARTUP_MODE_MANUAL),

//
// MessageId: SN_EV_TDI_INTERNET_SECURITY_ON
//
// MessageText:
//
//  Internet Security has been turned on.
//
	SNMC(EV_TDI_INTERNET_SECURITY_ON),

//
// MessageId: SN_EV_TDI_INTERNET_SECURITY_OFF
//
// MessageText:
//
//  Internet Security has been turned off.
//
	SNMC(EV_TDI_INTERNET_SECURITY_OFF),

// MessageId: SN_EV_IP_FILTER_RULE_CREATED_ON_LISTEN
//
// MessageText:
//
//  %1 "%2" communications.
//  Local address is %3(%4).
//  Process name is "%5".
//
	SNMC(EV_IP_FILTER_RULE_CREATED_ON_LISTEN),


//
// MessageId: SN_EV_IP_FILTER_RULE_CREATED_ON_LISTEN_FOR_ANY_ADAPTER
//
// MessageText:
//
//  %1 "%2" communications.
//  Local address: All local network adapters(%4).
//  Process name is "%5".
//
//
	SNMC(EV_IP_FILTER_RULE_CREATED_ON_LISTEN_FOR_ANY_ADAPTER),

//
// MessageId: SN_EV_ATRACK_RULE_MATCHED_LISTEN_ANY_ADAPTER
//
// MessageText:
//
//  Rule %1 matched
//  Local address: All local network adapters(%3).
//
	SNMC(EV_ATRACK_RULE_MATCHED_LISTEN_ANY_ADAPTER),

//
// MessageId: SN_EV_IP_FILTER_ALLOW_RULE_PRESENT_FOR_LISTEN_ANY_ADAPTER
//
// MessageText:
//
//  Rule "%1" permitted communication.
//  Local address: All local network adapters(%3).
//  Process name: "%4".
//
	SNMC(EV_IP_FILTER_ALLOW_RULE_PRESENT_FOR_LISTEN_ANY_ADAPTER),

//
// MessageId: SN_EV_IP_FILTER_REJECT_RULE_PRESENT_FOR_LISTEN_ANY_ADAPTER
//
// MessageText:
//
//  Rule "%1" blocked communication.
//  Local address: All local network adapters(%3).
//  Process name is "%4".
//
	SNMC(EV_IP_FILTER_REJECT_RULE_PRESENT_FOR_LISTEN_ANY_ADAPTER),

//
// MessageId: SN_EV_IP_FILTER_IGNORE_RULE_PRESENT_FOR_LISTEN_ANY_ADAPTER
//
// MessageText:
//
//  Rule "%1" ignored communication.
//  Local address: All local network adapters(%3).
//  Process name is "%4".
//
	SNMC(EV_IP_FILTER_IGNORE_RULE_PRESENT_FOR_LISTEN_ANY_ADAPTER),

//
// MessageId: SN_EV_IP_FILTER_BACKUP_RULES_RESTORED
//
// MessageText:
//
//  Missing or damaged firewall rules. Backup rules restored.
//
	SNMC(EV_IP_FILTER_BACKUP_RULES_RESTORED),

//
// MessageId: SN_EV_IP_FILTER_FAILED_TO_LOAD_RULES
//
// MessageText:
//
//  Missing or damaged firewall rules. Unable to load backup or default rules.
//  The firewall will be disabled.
//
	SNMC(EV_IP_FILTER_FAILED_TO_LOAD_RULES),

//
// MessageId: SN_EV_IP_FILTER_PORTBLOCK_BLOCKED
//
// MessageText:
//
//  Port Blocking blocked %1(%2).
//
	SNMC(EV_IP_FILTER_PORTBLOCK_BLOCKED),

//
// MessageId: SN_EV_IP_FILTER_PORTBLOCK_ALLOWED
//
// MessageText:
//
//  Port Blocking allowed %1(%2).
//
	SNMC(EV_IP_FILTER_PORTBLOCK_ALLOWED)
};



typedef struct 
{
	DWORD	dwNISEvtID;
	DWORD	dwSNID;
} SNSTRLOOKUP, *PSNSTRLOOKUP;

#define SNSTR(x) {SN_##x, x}

static SNSTRLOOKUP	aStringTable[] =
{
//
// misc. string IDs
//
	SNSTR(EV_STR_NULL),
	SNSTR(EV_STR_INVALID_PARAMETER),
	SNSTR(EV_STR_BLANK),
	SNSTR(EV_STR_NOT_VALID),
	SNSTR(EV_STR_NA),
	SNSTR(EV_STR_TO),
	SNSTR(EV_STR_FROM),
	SNSTR(EV_NDIS_UNKNOWN),
//
// tdi set event handler 'event types'
//
	SNSTR(EV_STR_TDI_EVENT_BASE),
	SNSTR(EV_STR_TDI_EVENT_CONNECT),
	SNSTR(EV_STR_TDI_EVENT_DISCONNECT),
	SNSTR(EV_STR_TDI_EVENT_ERROR),
	SNSTR(EV_STR_TDI_EVENT_RECEIVE),
	SNSTR(EV_STR_TDI_EVENT_RECEIVE_DATAGRAM),
	SNSTR(EV_STR_TDI_EVENT_RECEIVE_EXPEDITED),
	SNSTR(EV_STR_TDI_EVENT_SEND_POSSIBLE),
	SNSTR(EV_STR_TDI_EVENT_CHAINED_RECEIVE),
	SNSTR(EV_STR_TDI_EVENT_CHAINED_RECEIVE_DATAGRAM),
	SNSTR(EV_STR_TDI_EVENT_CHAINED_RECEIVE_EXPEDITED),
//                                                 
// query info types
//                                                 
	SNSTR(EV_STR_TDI_QUERY_BASE),
	SNSTR(EV_STR_TDI_QUERY_BROADCAST_ADDRESS),
	SNSTR(EV_STR_TDI_QUERY_PROVIDER_INFO),
	SNSTR(EV_STR_TDI_QUERY_ADDRESS_INFO),
	SNSTR(EV_STR_TDI_QUERY_CONNECTION_INFO),
	SNSTR(EV_STR_TDI_QUERY_PROVIDER_STATISTICS),
	SNSTR(EV_STR_TDI_QUERY_DATAGRAM_INFO),
	SNSTR(EV_STR_TDI_QUERY_DATA_LINK_ADDRESS),
	SNSTR(EV_STR_TDI_QUERY_NETWORK_ADDRESS),
	SNSTR(EV_STR_TDI_QUERY_MAX_DATAGRAM_INFO),
	SNSTR(EV_STR_TDI_QUERY_ADAPTER_STATUS),
	SNSTR(EV_STR_TDI_QUERY_SESSION_STATUS),
	SNSTR(EV_STR_TDI_QUERY_FIND_NAME),
//
// tdi errors
//
	SNSTR(EV_STR_STATUS_ADDRESS_ALREADY_ASSOCIATED),
	SNSTR(EV_STR_STATUS_ADDRESS_ALREADY_EXISTS),
	SNSTR(EV_STR_STATUS_ADDRESS_CLOSED),
	SNSTR(EV_STR_STATUS_ADDRESS_NOT_ASSOCIATED),
	SNSTR(EV_STR_STATUS_BUFFER_OVERFLOW),
	SNSTR(EV_STR_STATUS_BUFFER_TOO_SMALL),
	SNSTR(EV_STR_STATUS_CANCELLED),
	SNSTR(EV_STR_STATUS_CONNECTION_ABORTED),
	SNSTR(EV_STR_STATUS_CONNECTION_ACTIVE),
	SNSTR(EV_STR_STATUS_CONNECTION_INVALID),
	SNSTR(EV_STR_STATUS_CONNECTION_REFUSED),
	SNSTR(EV_STR_STATUS_CONNECTION_RESET),
	SNSTR(EV_STR_STATUS_DATA_NOT_ACCEPTED),
	SNSTR(EV_STR_STATUS_GRACEFUL_DISCONNECT),
	SNSTR(EV_STR_STATUS_HOST_UNREACHABLE),
	SNSTR(EV_STR_STATUS_IO_TIMEOUT),
	SNSTR(EV_STR_STATUS_INSUFFICIENT_RESOURCES),
	SNSTR(EV_STR_STATUS_INVALID_ADDRESS),
	SNSTR(EV_STR_STATUS_INVALID_ADDRESS_COMPONENT),
	SNSTR(EV_STR_STATUS_INVALID_DEVICE_REQUEST),
	SNSTR(EV_STR_STATUS_INVALID_DEVICE_STATE),
	SNSTR(EV_STR_STATUS_INVALID_PARAMETER),
	SNSTR(EV_STR_STATUS_MORE_PROCESSING_REQUIRED),
	SNSTR(EV_STR_STATUS_NETWORK_UNREACHABLE),
	SNSTR(EV_STR_STATUS_PENDING),
	SNSTR(EV_STR_STATUS_PORT_UNREACHABLE),
	SNSTR(EV_STR_STATUS_PROTOCOL_UNREACHABLE),
	SNSTR(EV_STR_STATUS_REQUEST_ABORTED),
	SNSTR(EV_STR_STATUS_SUCCESS),
	SNSTR(EV_STR_STATUS_TOO_MANY_ADDRESSES),
	SNSTR(EV_STR_STATUS_UNKNOWN),
//
// Known Address types
//
	SNSTR(EV_STR_TDI_ADDRESS_TYPE_BASE),
	SNSTR(EV_STR_TDI_ADDRESS_TYPE_UNSPEC),
	SNSTR(EV_STR_TDI_ADDRESS_TYPE_UNIX),
	SNSTR(EV_STR_TDI_ADDRESS_TYPE_IP),
	SNSTR(EV_STR_TDI_ADDRESS_TYPE_IMPLINK),
	SNSTR(EV_STR_TDI_ADDRESS_TYPE_PUP),
	SNSTR(EV_STR_TDI_ADDRESS_TYPE_CHAOS),
	SNSTR(EV_STR_TDI_ADDRESS_TYPE_IPX),
	SNSTR(EV_STR_TDI_ADDRESS_TYPE_NBS),
	SNSTR(EV_STR_TDI_ADDRESS_TYPE_ECMA),
	SNSTR(EV_STR_TDI_ADDRESS_TYPE_DATAKIT),
	SNSTR(EV_STR_TDI_ADDRESS_TYPE_CCITT),
	SNSTR(EV_STR_TDI_ADDRESS_TYPE_SNA),
	SNSTR(EV_STR_TDI_ADDRESS_TYPE_DECnet),
	SNSTR(EV_STR_TDI_ADDRESS_TYPE_DLI),
	SNSTR(EV_STR_TDI_ADDRESS_TYPE_LAT),
	SNSTR(EV_STR_TDI_ADDRESS_TYPE_HYLINK),
	SNSTR(EV_STR_TDI_ADDRESS_TYPE_APPLETALK),
	SNSTR(EV_STR_TDI_ADDRESS_TYPE_NETBIOS),
	SNSTR(EV_STR_TDI_ADDRESS_TYPE_8022),
	SNSTR(EV_STR_TDI_ADDRESS_TYPE_OSI_TSAP),
	SNSTR(EV_STR_TDI_ADDRESS_TYPE_NETONE),
	SNSTR(EV_STR_TDI_ADDRESS_TYPE_VNS),
	SNSTR(EV_STR_TDI_ADDRESS_TYPE_NETBIOS_EX),
//
// Disconnect Flags
//
	SNSTR(EV_STR_TDI_DISCONNECT_FLAGS),
	SNSTR(EV_STR_TDI_DISCONNECT_WAIT),
	SNSTR(EV_STR_TDI_DISCONNECT_ABORT),
	SNSTR(EV_STR_TDI_DISCONNECT_RELEASE),
//
// Device Types
//
	SNSTR(EV_STR_DEVICE_IP),
	SNSTR(EV_STR_DEVICE_UDP),
	SNSTR(EV_STR_DEVICE_TCP),
	SNSTR(EV_STR_DEVICE_RAW_IP),
	SNSTR(EV_STR_DEVICE_UNKNOWN),
//
// create object types
//
	SNSTR(EV_STR_OBJECT_ID_BASE),
	SNSTR(EV_STR_TRANSPORT_ADDRESS),
	SNSTR(EV_STR_CONNECTION),
	SNSTR(EV_STR_CONTROL_CHANNEL),
	SNSTR(EV_STR_CONNECTION_CONTEXT),
//
// file device types 
//
	SNSTR(EV_STR_FILE_DEVICE_BASE),
	SNSTR(EV_STR_FILE_DEVICE_BEEP),
	SNSTR(EV_STR_FILE_DEVICE_CD_ROM),
	SNSTR(EV_STR_FILE_DEVICE_CD_ROM_FILE_SYSTEM),
	SNSTR(EV_STR_FILE_DEVICE_CONTROLLER),
	SNSTR(EV_STR_FILE_DEVICE_DATALINK),
	SNSTR(EV_STR_FILE_DEVICE_DFS),
	SNSTR(EV_STR_FILE_DEVICE_DISK),
	SNSTR(EV_STR_FILE_DEVICE_DISK_FILE_SYSTEM),
	SNSTR(EV_STR_FILE_DEVICE_FILE_SYSTEM),
	SNSTR(EV_STR_FILE_DEVICE_INPORT_PORT),
	SNSTR(EV_STR_FILE_DEVICE_KEYBOARD),
	SNSTR(EV_STR_FILE_DEVICE_MAILSLOT),
	SNSTR(EV_STR_FILE_DEVICE_MIDI_IN),
	SNSTR(EV_STR_FILE_DEVICE_MIDI_OUT),
	SNSTR(EV_STR_FILE_DEVICE_MOUSE),
	SNSTR(EV_STR_FILE_DEVICE_MULTI_UNC_PROVIDER),
	SNSTR(EV_STR_FILE_DEVICE_NAMED_PIPE),
	SNSTR(EV_STR_FILE_DEVICE_NETWORK),
	SNSTR(EV_STR_FILE_DEVICE_NETWORK_BROWSER),
	SNSTR(EV_STR_FILE_DEVICE_NETWORK_FILE_SYSTEM),
	SNSTR(EV_STR_FILE_DEVICE_NULL),
	SNSTR(EV_STR_FILE_DEVICE_PARALLEL_PORT),
	SNSTR(EV_STR_FILE_DEVICE_PHYSICAL_NETCARD),
	SNSTR(EV_STR_FILE_DEVICE_PRINTER),
	SNSTR(EV_STR_FILE_DEVICE_SCANNER),
	SNSTR(EV_STR_FILE_DEVICE_SERIAL_MOUSE_PORT),
	SNSTR(EV_STR_FILE_DEVICE_SERIAL_PORT),
	SNSTR(EV_STR_FILE_DEVICE_SCREEN),
	SNSTR(EV_STR_FILE_DEVICE_SOUND),
	SNSTR(EV_STR_FILE_DEVICE_STREAMS),
	SNSTR(EV_STR_FILE_DEVICE_TAPE),
	SNSTR(EV_STR_FILE_DEVICE_TAPE_FILE_SYSTEM),
	SNSTR(EV_STR_FILE_DEVICE_TRANSPORT),
	SNSTR(EV_STR_FILE_DEVICE_UNKNOWN),
	SNSTR(EV_STR_FILE_DEVICE_VIDEO),
	SNSTR(EV_STR_FILE_DEVICE_VIRTUAL_DISK),
	SNSTR(EV_STR_FILE_DEVICE_WAVE_IN),
	SNSTR(EV_STR_FILE_DEVICE_WAVE_OUT),
	SNSTR(EV_STR_FILE_DEVICE_8042_PORT),
	SNSTR(EV_STR_FILE_DEVICE_NETWORK_REDIRECTOR),
	SNSTR(EV_STR_FILE_DEVICE_BATTERY),
	SNSTR(EV_STR_FILE_DEVICE_BUS_EXTENDER),
	SNSTR(EV_STR_FILE_DEVICE_MODEM),
	SNSTR(EV_STR_FILE_DEVICE_VDM),
	SNSTR(EV_STR_FILE_DEVICE_MASS_STORAGE),
	SNSTR(EV_STR_FILE_DEVICE_MAX),
//
// Define the method codes for how buffers are passed for I/O and FS controls
//
	SNSTR(EV_STR_METHOD_BASE),
	SNSTR(EV_STR_METHOD_BUFFERED),
	SNSTR(EV_STR_METHOD_IN_DIRECT),
	SNSTR(EV_STR_METHOD_OUT_DIRECT),
	SNSTR(EV_STR_METHOD_NEITHER),
//
// Define the access check value for any access
//
	SNSTR(EV_STR_FILE_ACCESS_BASE),
	SNSTR(EV_STR_FILE_ANY_ACCESS),
	SNSTR(EV_STR_FILE_READ_ACCESS),
	SNSTR(EV_STR_FILE_WRITE_ACCESS),
	SNSTR(EV_STR_FILE_READ_WRITE_ACCESS),
//
// Query Information Entities (input)
//
	SNSTR(EV_STR_GENERIC_ENTITY),
	SNSTR(EV_STR_CO_TL_ENTITY),
	SNSTR(EV_STR_CL_TL_ENTITY),
	SNSTR(EV_STR_ER_ENTITY),
	SNSTR(EV_STR_CO_NL_ENTITY),
	SNSTR(EV_STR_CL_NL_ENTITY),
	SNSTR(EV_STR_AT_ENTITY),
	SNSTR(EV_STR_IF_ENTITY),
	SNSTR(EV_STR_UNKNOWN_ENTITY),
//
// Query Information Entities (Output)
//
	SNSTR(EV_STR_CO_TL_NBF),
	SNSTR(EV_STR_CO_TL_SPX),
	SNSTR(EV_STR_CO_TL_TCP),
	SNSTR(EV_STR_CO_TL_SPP),
	SNSTR(EV_STR_CL_TL_NBF),
	SNSTR(EV_STR_CL_TL_UDP),
	SNSTR(EV_STR_ER_ICMP),
	SNSTR(EV_STR_CL_NL_IPX),
	SNSTR(EV_STR_CL_NL_IP),
	SNSTR(EV_STR_AT_ARP),
	SNSTR(EV_STR_AT_NULL),
	SNSTR(EV_STR_IF_GENERIC),
	SNSTR(EV_STR_IF_MIB),
//
// Query Information Classes
//
	SNSTR(EV_STR_INFO_CLASS_GENERIC),
	SNSTR(EV_STR_INFO_CLASS_PROTOCOL),
	SNSTR(EV_STR_INFO_CLASS_IMPLEMENTATION),
	SNSTR(EV_STR_INFO_CLASS_UNKNOWN),
//
// Query Information Types
//
	SNSTR(EV_STR_INFO_TYPE_PROVIDER),
	SNSTR(EV_STR_INFO_TYPE_ADDRESS_OBJECT),
	SNSTR(EV_STR_INFO_TYPE_CONNECTION),
	SNSTR(EV_STR_INFO_TYPE_UNKNOWN),
//
// Query Information IDs
//
	SNSTR(EV_STR_ENTITY_LIST_ID),
	SNSTR(EV_STR_ENTITY_TYPE_ID),
	SNSTR(EV_STR_IP_MIB_ADDRTABLE_ENTRY_ID),
	SNSTR(EV_STR_IP_INTFC_INFO_ID),

//
// IDS Threat Levels
//
	{IDS_LOW_STRING ,	EV_STR_IDS_LOW},
	{IDS_MEDIUM_STRING,  	EV_STR_IDS_MEDIUM},
	{IDS_HIGH_STRING,	EV_STR_IDS_HIGH},

//
// Receive Flags
//
	SNSTR(TDI_RECEIVE_MASK),
                                                                // supported bits in the
                                                                // received flags
	SNSTR(EV_STR_TDI_RECEIVE_FLAGS),
	SNSTR(EV_STR_TDI_RECEIVE_TRUNCATED),
	SNSTR(EV_STR_TDI_RECEIVE_FRAGMENT),
	SNSTR(EV_STR_TDI_RECEIVE_BROADCAST),
	SNSTR(EV_STR_TDI_RECEIVE_MULTICAST),
	SNSTR(EV_STR_TDI_RECEIVE_PARTIAL),
	SNSTR(EV_STR_TDI_RECEIVE_NORMAL),
	SNSTR(EV_STR_TDI_RECEIVE_EXPEDITED),
	SNSTR(EV_STR_TDI_RECEIVE_PEEK),
	SNSTR(EV_STR_TDI_RECEIVE_NO_RESPONSE_EXP),
	SNSTR(EV_STR_TDI_RECEIVE_COPY_LOOKAHEAD),
	SNSTR(EV_STR_TDI_RECEIVE_ENTIRE_MESSAGE),
	SNSTR(EV_STR_TDI_RECEIVE_AT_DISPATCH_LEVEL),
//
// receive filter flags
//
	SNSTR(TO_PROTOCOL),
	SNSTR(FROM_PROTOCOL),
	SNSTR(TO_TDITRACE),
	SNSTR(FROM_TDITRACE),
//
// queue names
//
	SNSTR(EV_STR_TDI_Q_TRX_RECEIVE),
	SNSTR(EV_STR_TDI_Q_BD_RECEIVE_IN),
	SNSTR(EV_STR_TDI_Q_BD_RECEIVE_OUT),
//
// filter status codes
//
	SNSTR(EV_STR_FILTER_CONNECT_ALLOW),
	SNSTR(EV_STR_FILTER_CONNECT_ALLOW_FILTER),
	SNSTR(EV_STR_FILTER_CONNECT_REJECT),
	SNSTR(EV_STR_FILTER_DATAGRAM_ALLOW),
	SNSTR(EV_STR_FILTER_DATAGRAM_REJECT),

//
// Send Flags
//
	SNSTR(TDI_SEND_MASK),
                                                                // supported bits in the
                                                                // send flags
	SNSTR(EV_STR_TDI_SEND_FLAGS),
	SNSTR(EV_STR_TDI_SEND_EXPEDITED),
	SNSTR(EV_STR_TDI_SEND_PARTIAL),
	SNSTR(EV_STR_TDI_SEND_NO_RESPONSE_EXPECTED),
	SNSTR(EV_STR_TDI_SEND_NON_BLOCKING),

//-------------------------------------------------------------------
//
//  I P   F I L T E R   D E F I N E S 
//
//------------------------------------------------------------------

	SNSTR(EV_STR_IP_FILTER_NO_MEMORY),
	SNSTR(EV_STR_IP_FILTER_BAD_TYPE_OBJECT),
	SNSTR(EV_STR_IP_FILTER_BAD_NAME_OBJECT),
	SNSTR(EV_STR_IP_FILTER_HOST),
	SNSTR(EV_STR_IP_FILTER_NETWORK),
	SNSTR(EV_STR_IP_FILTER_RANGE),
	SNSTR(EV_STR_IP_FILTER_GROUP),
	SNSTR(EV_STR_IP_FILTER_SERVICE),
	SNSTR(EV_STR_IP_FILTER_TIME),
	SNSTR(EV_STR_IP_FILTER_GENERIC),
	SNSTR(EV_STR_IP_FILTER_UNKNOWN_IP_OBJECT_TYPE),
	SNSTR(EV_STR_IP_FILTER_APPLICATION),
	SNSTR(EV_STR_IP_FILTER_USER_ACTION_ONCE),
	SNSTR(EV_STR_IP_FILTER_DEFAULT_ACTION),
	SNSTR(EV_STR_IP_FILTER_ALLOW),
	SNSTR(EV_STR_IP_FILTER_DENY),
	SNSTR(EV_STR_IP_FILTER_USER_ACTION_CREATE_RULE),
	SNSTR(EV_IP_IMPLICIT_BLOCK_RULE),
	SNSTR(EV_STR_IP_FILTER_USER_ACTION_CREATE_BOGUS_RULE),


	{IDS_NONE,				EVL_STR_NONE},
	{IDS_STR_FORMAT_ERROR,		EVL_STR_FORMAT_ERROR},
	{IDS_INVALID_STR_ID,	EVL_STR_INVALID_STR_ID},

//-------------------------------------------------------------------
//
// ICMP Message Types
//
//-------------------------------------------------------------------

	SNSTR(EV_STR_ICMP_ECHO_REPLY),
	SNSTR(EV_STR_ICMP_DESTINATION_UNREACHABLE),
	SNSTR(EV_STR_ICMP_SOURCH_QUENCH),
	SNSTR(EV_STR_ICMP_REDIRECT),
	SNSTR(EV_STR_ICMP_ECHO_REQUEST),
	SNSTR(EV_STR_ICMP_ROUTER_ADVERTISEMENT),
	SNSTR(EV_STR_ICMP_ROUTER_SOLICITATION),
	SNSTR(EV_STR_ICMP_DATAGRAM_TIME_EXCEEDED),
	SNSTR(EV_STR_ICMP_DATAGRAM_PARAMETER_PROBLEM),
	SNSTR(EV_STR_ICMP_TIMESTAMP_REQUEST),
	SNSTR(EV_STR_ICMP_TIMESTAMP_REPLY),
	SNSTR(EV_STR_ICMP_INFORMATION_REQUEST),
	SNSTR(EV_STR_ICMP_INFORMATION_REPLY),
	SNSTR(EV_STR_ICMP_ADDRESS_MASK_REQUEST),
	SNSTR(EV_STR_ICMP_ADDRESS_MASK_REPLY),
	SNSTR(EV_STR_ICMP_UNKNOWN_INVALID_MESSAGE),


//-------------------------------------------------------------------
//
// Settings change strings
//
//-------------------------------------------------------------------

	SNSTR(EV_STR_AUTOCORRECTENABLED),
	SNSTR(EV_STR_AUTOCORRECTTIMEOUT),
	SNSTR(EV_STR_ICMPFILTERENABLED),
	SNSTR(EV_STR_FRAGMENTEDPKTACTION_FW),
	SNSTR(EV_STR_FRAGMENTEDPKTACTION_IDS),
	SNSTR(EV_STR_LOGIPFRAGMENTS),
	SNSTR(EV_STR_BLOCKIGMP),
	SNSTR(EV_STR_PROTECTEXPLORER),
	SNSTR(EV_STR_DATAGRAMRECEIVEACTION),
	SNSTR(EV_STR_DEFAULTCONNECTRESULT),
	SNSTR(EV_STR_DEFAULTACCEPTRESULT),
	SNSTR(EV_STR_DEFAULTRECEIVEDATAGRAMRESULT),
	SNSTR(EV_STR_DEFAULTSENDDATAGRAMRESULT),
	SNSTR(EV_STR_DEFAULTLISTENRESULT),
	SNSTR(EV_STR_STEALTHBLOCKED),
	SNSTR(EV_STR_MOSTRECENTSIZE),
	SNSTR(EV_STR_SCANDETECTENABLED),
	SNSTR(EV_STR_SCANDETECTAUTOCORRECT),
	SNSTR(EV_STR_SCANDETECTTIMEOUT),
	SNSTR(EV_STR_SCANDETECTTHRESHOLD),
	SNSTR(EV_STR_SCANDETECTPURGEINTERVAL),
	SNSTR(EV_STR_SCANDETECTCLOSEDPORTSONLY),
	SNSTR(EV_STR_ALERTACCESSEDUNUSEDPORTS),
	SNSTR(EV_STR_DISCONNECTEVERYTHING),
	SNSTR(EV_STR_MULTIHOMED),
	SNSTR(EV_STR_ENABLEDDISABLEIDS),
	SNSTR(EV_STR_ENABLEMODULEFINGERPRINTING),
	SNSTR(EV_STR_ENABLEAPPPARENTCHECK),
	SNSTR(EV_STR_SYNSCANSTEALTHING),
	SNSTR(EV_STR_FIREWALLSTATE),
	SNSTR(EV_STR_NDISSTATE),
	SNSTR(EV_STR_ENABLELOCATIONAWARENESS),
	SNSTR(EV_STR_MAXIMUMRULEFILE),
	SNSTR(EV_STR_NDISALERTENABLED),
	SNSTR(EV_STR_PORTBLOCKLISTSIZE),
	SNSTR(EV_STR_PORTBLOCKENABLED),
	SNSTR(EV_STR_FILTERALLOUTBOUND),
	SNSTR(EV_STR_PORTBLOCKALERTFLAG),
	SNSTR(EV_STR_LOGGINGFILTERMASK),
	SNSTR(EV_STR_PORTBLOCKALLOWINBOUNDNETBIOS),
	SNSTR(EV_STR_PORTBLOCKBROADCASTTIMEOUT),
    SNSTR(EV_STR_UNCMNPROTOGATEDIRECTION),
	SNSTR(EV_STR_RULEUPDATEBUFFERSIZE)
};



HRESULT LookupMessageID ( DWORD dwSNID, DWORD& dwNISEvtIDShort, DWORD& dwNISEvtIDNormal, DWORD& dwNISEvtIDHTML )
{
	for (int x=0; x < CCDIMOF(aMessageTable); x++)
		if (aMessageTable[x].dwSNID == dwSNID)
		{
			#ifdef CALC_SHORT_ID
			dwNISEvtIDShort = aMessageTable[x].dwNISEvtID_Normal + (1<<15);
			#else
			dwNISEvtIDShort = aMessageTable[x].dwNISEvtID_Short;
			#endif

			dwNISEvtIDNormal = aMessageTable[x].dwNISEvtID_Normal;
			dwNISEvtIDHTML = aMessageTable[x].dwNISEvtID_HTML;
			return S_OK;
		}

	return E_FAIL;
}


HRESULT LookupStringID (DWORD dwSNID, DWORD& dwNISEvtID)
{
	for (int x=0; x < CCDIMOF(aStringTable); x++)
		if (aStringTable[x].dwSNID == dwSNID)
		{
			dwNISEvtID = aStringTable[x].dwNISEvtID;
			return S_OK;
		}

	return E_FAIL;
}

