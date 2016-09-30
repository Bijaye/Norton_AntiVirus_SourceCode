// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#ifdef __cplusplus
extern "C" {
#endif

#ifndef MAILSERV_DEFS
#define MAILSERV_DEFS

/* Mail service package definitions */

#ifndef DARRAY_DEFS
#include "darray.h"							/* Dynamic array definitions */
#endif

#ifndef POOL_DEFS
#include "pool.h"							/* BLOCK definition */
#endif

#ifndef NSF_DATA_DEFS
#include "nsfdata.h"						/* DBHANDLE definition */
#endif


/* Mail delivery priorities.  Note: order is assumed. */

#define DELIVERY_PRIORITY_LOW 		0
#define DELIVERY_PRIORITY_NORMAL 	1
#define DELIVERY_PRIORITY_HIGH 		2

/* Mail delivery report requests. Note: order is assumed. */

#define DELIVERY_REPORT_NONE 				0	
#define DELIVERY_REPORT_BASIC				1
#define DELIVERY_REPORT_CONFIRMED			2
#define DELIVERY_REPORT_TRACE				3
#define DELIVERY_REPORT_TRACE_NO_DELIVER	4
#define DELIVERY_REPORT_CONFIRM_NO_DELIVER 	5

/* Mail delivery time constants. */

#define DELIVERY_HOUR 		  3600
#define DELIVERY_MINUTE 	  60
#define DELIVERY_MESSAGE_SIZE 1024

/* Message types - Returned by MailGetMessageType. Note: order is assumed. */

#define MAIL_MESSAGE_UNKNOWN			0
#define MAIL_MESSAGE_MEMO				1
#define MAIL_MESSAGE_DELIVERYREPORT		2
#define MAIL_MESSAGE_NONDELIVERYREPORT	3
#define MAIL_MESSAGE_RETURNRECEIPT		4
#define MAIL_MESSAGE_PHONEMESSAGE		5
#define MAIL_MESSAGE_TRACEREPORT		6
#define MAIL_MESSAGE_NOTICE				7

/* Address file functions */

STATUS LNPUBLIC	 MailGetDomainName(char far *Domain);

STATUS LNPUBLIC	 MailLookupAddress(char far *UserName, 
								char far *MailAddress);
STATUS LNPUBLIC	 MailLookupUser(char far *UserName, 
								char far *FullName, 
								char far *MailServerName, char far *MailFileName, 
								char far *MailAddress, char far *ShortName);

/* Message mailing functions */

STATUS LNPUBLIC	 MailGetMessageItem (HANDLE hMessage, WORD ItemCode,
								char far *retString, WORD StringSize, WORD far *retStringLength);

STATUS LNPUBLIC	 MailGetMessageItemHandle (HANDLE hMessage, WORD ItemCode,
								BLOCKID far *retbhValue, WORD far *retValueType, DWORD far *retValueLength);

STATUS LNPUBLIC	 MailGetMessageItemTimeDate (HANDLE hMessage, WORD ItemCode,
								TIMEDATE far *retTimeDate);

STATUS LNPUBLIC	 MailCreateMessage (DBHANDLE hFile, HANDLE far *rethMessage);

STATUS LNPUBLIC	 MailAddHeaderItem (HANDLE hMessage, WORD ItemCode,
								void far *Value, WORD ValueLength);

STATUS LNPUBLIC	 MailAddHeaderItemByHandle (HANDLE hMessage, WORD ItemCode,
								HANDLE hValue, WORD ValueLength,
								WORD ItemFlags);

STATUS LNPUBLIC	 MailReplaceHeaderItem (HANDLE hMessage, WORD ItemCode,
								void far *Value, WORD ValueLength);

STATUS LNPUBLIC	 MailCreateBodyItem (HANDLE far *rethBodyItem, DWORD far *retBodyLength);

STATUS LNPUBLIC	 MailAppendBodyItemLine (HANDLE hBodyItem, DWORD far *BodyLength,
								char far *Text, WORD TextLength);

STATUS LNPUBLIC	 MailAddBodyItem (HANDLE hMessage, HANDLE hBodyItem,
								DWORD BodyLength, char far *CTFName);

STATUS LNPUBLIC	 MailAddRecipientsItem (HANDLE hMessage,
								HANDLE hRecipientsItem, WORD RecipientsLength);


STATUS LNPUBLIC	 MailTransferMessageLocal (HANDLE hMessage);

BOOL LNPUBLIC	 MailIsNonDeliveryReport (HANDLE hMessage);
WORD LNPUBLIC	 MailGetMessageType (HANDLE hMessage);

STATUS LNPUBLIC	 MailCloseMessage (HANDLE hMessage);

STATUS LNPUBLIC	 MailExpandNames(HANDLE hWorkList,
								WORD WorkListSize,
								HANDLE far *hOutputList,
								WORD far *OutputListSize,
								BOOL UseExpanded,
								HANDLE hRecipsExpanded);

STATUS LNVARARGS MailLogEvent (WORD Flags, STATUS StringID, 
							HMODULE hModule, STATUS AdditionalErrorCode, ...);
							
STATUS LNVARARGS MailLogEventText (WORD Flags, char *String, 
							HMODULE hModule, STATUS AdditionalErrorCode, ...);
							
/* Mail event logging flags */							
#define MAIL_LOG_TO_MISCEVENTS	0x0001	/* Log message to Miscellaneuos Events view */
#define MAIL_LOG_TO_MAILEVENTS	0x0002	/* Log message to Mail Events view */
#define MAIL_LOG_TO_EVENTS_ONLY 0x0004	/* Don't log messages to console */
#define MAIL_LOG_FORMATTED_TEXT 0x0008	/* Text already formatted */
#define MAIL_LOG_TO_BOTH		(MAIL_LOG_TO_MAILEVENTS | MAIL_LOG_TO_MISCEVENTS)
							

/* Message attachment handling functions */

BOOL LNPUBLIC	 MailGetMessageAttachmentInfo (HANDLE hMessage, 
								WORD Num, BLOCKID far *bhItem,
								char far *FileName, DWORD far *FileSize,
								WORD far *FileAttributes, WORD far *FileHostType,
								TIMEDATE far *FileCreated, TIMEDATE far *FileModified);

STATUS LNPUBLIC	 MailExtractMessageAttachment (HANDLE hMessage, 
								BLOCKID bhItem, char far *FileName);

STATUS LNPUBLIC	 MailAddMessageAttachment (HANDLE hMessage, 
								char far *FileName, char far *OriginalFileName);


/* Message file functions */

STATUS LNPUBLIC	 MailOpenMessageFile (char far *FileName, DBHANDLE far *rethFile);

STATUS LNPUBLIC	 MailCreateMessageFile (char far *FileName, char far *TemplateFileName, 
								char far *Title, DBHANDLE far *rethFile);

STATUS LNPUBLIC	 MailPurgeMessageFile (DBHANDLE hFile);

STATUS LNPUBLIC	 MailCloseMessageFile (DBHANDLE hFile);

STATUS LNPUBLIC	 MailGetMessageFileModifiedTime (DBHANDLE hFile, TIMEDATE far *retModifiedTime);

/* Message list functions */

STATUS LNPUBLIC	 MailCreateMessageList (DBHANDLE hFile, 
								HANDLE far *hMessageList, 
								DARRAY far * far *MessageList, WORD far *MessageCount);

STATUS LNPUBLIC	 MailFreeMessageList (HANDLE hMessageList, DARRAY far *MessageList);

STATUS LNPUBLIC	 MailGetMessageInfo (DARRAY far *MessageList, WORD Message, 
								WORD far *RecipientCount, 
								WORD far *Priority, 
								WORD far *Report);

STATUS LNPUBLIC  MailGetMessageSize(DARRAY far *MessageList, WORD Message, DWORD far *MessageSize);

STATUS LNPUBLIC	 MailGetMessageRecipient (DARRAY far *MessageList, WORD Message, 
								WORD RecipientNum, 
								char far *RecipientName, 
								WORD RecipientNameSize, 
								WORD far *RecipientNameLength);

STATUS LNPUBLIC	 MailDeleteMessageRecipient (DARRAY far *MessageList, WORD Message, 
								WORD RecipientNum);

STATUS LNPUBLIC	 MailGetMessageOriginator (DARRAY far *MessageList, WORD Message, 
								char far *OriginatorName, 
								WORD OriginatorNameSize, 
								WORD far *OriginatorNameLength);

STATUS LNPUBLIC	 MailGetMessageOriginatorDomain(DARRAY far *MessageList, WORD Message,
								char far *OriginatorDomain,
								WORD OriginatorDomainSize,
								WORD far *OriginatorNameLength);

STATUS LNPUBLIC	 MailOpenMessage (DARRAY far *MessageList, WORD Message, 
								HANDLE far *hMessage);

STATUS LNPUBLIC	 MailGetMessageBody (HANDLE hMessage, 
								HANDLE far *hBody, DWORD far *BodyLength);

STATUS LNPUBLIC	 MailGetMessageBodyText(HANDLE hMessage,
										char far *ItemName,
										char far *LineDelims,
										WORD LineLength,
										BOOL ConvertTabs,
										char far *OutputFileName,
										DWORD far *OutputFileSize);

STATUS LNPUBLIC	 MailGetMessageBodyComposite(HANDLE hMessage,
											char far *ItemName,
											char far *OutputFileName,
											DWORD far *OutputFileSize);

STATUS LNPUBLIC	 MailAddMessageBodyText(HANDLE hMessage,
										char far *ItemName,
										char far *InputFileName,
										DWORD FontID,
										char far *LineDelim,
										WORD ParaDelim,
										char far *CTFName);

STATUS LNPUBLIC	 MailAddMessageBodyComposite(HANDLE hMessage,
											char far *ItemName,
											char far *InputFileName);

STATUS LNPUBLIC	 MailSetMessageLastError (DARRAY far *MessageList, WORD Message, 
								char far *ErrorText);

STATUS LNPUBLIC	 MailPurgeMessage (DARRAY far *MessageList, WORD Message);

STATUS LNPUBLIC	 MailSendNonDeliveryReport (DARRAY far *MessageList, WORD Message, 
								WORD RecipientNums, WORD far *RecipientNumList, 
								char far *ReasonText, WORD ReasonTextLength);

STATUS LNPUBLIC	 MailSendDeliveryReport (DARRAY far *MessageList, WORD Message, 
								WORD RecipientNums, WORD far *RecipientNumList);

/* Mail address to user and domain name parsing function */

STATUS LNPUBLIC	 MailParseMailAddress (char far *MailAddress, WORD MailAddressLength,
								char far *UserName, WORD UserNameSize, WORD far *UserNameLength,
								char far *DomainName, WORD DomainNameSize, WORD far *DomainNameLength);


/* Broadcast newmail recieved message */

void LNPUBLIC	 MailBroadcastNewMail (char far *MessageText);	/* V2 Compatible, NETBIOS-ONLY */

/* Routing Table Services */


/*	LoadFlags for MailLoadRoutingTablesExt */

#define	LOAD_INTERNALSMTP			0x00000001	/* Support internal SMTP transfer thread and DNS */

STATUS LNPUBLIC MailLoadRoutingTablesExt(DBHANDLE hAddressBook, 
									char far *LocalServersName,
									char far *LocalServersDomain,
									char far *LocalServersInternetDomain,
									char far *TaskName,
									BOOL EnableTrace, 
									BOOL EnableDebug,
									DWORD LoadFlags,
									HANDLE far *rethTables);

STATUS LNPUBLIC	 MailLoadRoutingTables(DBHANDLE hAddressBook, 
									char far *LocalServerName,
									char far *LocalDomainDomain,
									char far *TaskName,
									BOOL EnableTrace,
									BOOL EnableDebug,
									HANDLE far *rethTables);

STATUS LNPUBLIC	 MailReloadRoutingTables(HANDLE hTables,
									BOOL EnableTrace,
									BOOL EnableDebug,
									BOOL far *retAddressBookModified);

STATUS LNPUBLIC	 MailUnloadRoutingTables(HANDLE hTables);

/* Input flags for MailFindNextHopTo* routines */

#define	NEXTHOP_CALENDAR		0x00000001	/* Message being routed is a Calendar & Scheduling message */

/* Output flags for MailFindNextHopTo* routines */

#define	NEXTHOP_INTRANET		0x00000001	/* Next Hop is on same network */
#define NEXTHOP_USESMTP			0x00000004	/* Use SMTP to reach next hop */

STATUS LNPUBLIC	 MailFindNextHopToDomain(HANDLE hTables,
									char far *OriginatorsDomain, 
									char far *DestDomain, 
									char far *NextHopServer, 
									char far *NextHopMailbox, 
									DWORD far *NextHopFlags, 
									char far *ErrorServer);

STATUS LNPUBLIC	 MailFindNextHopToDomainExt(HANDLE hTables,
									char far *OriginatorsDomain, 
									char far *DestDomain, 
									DWORD FindFlags,
									char far *NextHopServer, 
									char far *NextHopMailbox, 
									DWORD far *NextHopFlags, 
									char far *ErrorServer);

STATUS LNPUBLIC	 MailFindNextHopToServer(HANDLE hTables,
									 char far *DestDomain, 
									 char far *DestServer, 
									 char far *NextHopServer, 
									 char far *NextHopMailbox, 
									 DWORD far *NextHopFlags, 
									 WORD far *ActualCost);

typedef enum {MAIL_ERROR, MAIL_TRANSFER, 
			  MAIL_DELIVER, MAIL_FORWARD} 
	MAIL_ROUTING_ACTIONS;

STATUS LNPUBLIC	 MailFindNextHopToRecipient(HANDLE hTables,
									 char far *OriginatorsDomain,
									 char far *RecipientAddress,
									 MAIL_ROUTING_ACTIONS far *Action, 
									 char far *NextHopServer, 
									 char far *NextHopMailbox,
									 char far *ForwardAddress,
									 char far *ErrorText, 
									 DWORD far *NextHopFlags); 

STATUS LNPUBLIC	 MailFindNextHopViaRules(HANDLE hTables,
									 char far *RecipientAddress,
									 char far *retDestServer,
									 char far *retDestDomain);

BOOL LNPUBLIC	 MailSetDynamicCost(HANDLE hTables, 
									char far *Server, 
									SWORD CostBias);

BOOL LNPUBLIC	 MailResetAllDynamicCosts(HANDLE hTables);


#endif /* MAILSERV_DEFS */

#ifdef __cplusplus
}
#endif

