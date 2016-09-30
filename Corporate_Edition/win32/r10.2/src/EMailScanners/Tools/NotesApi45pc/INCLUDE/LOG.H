// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#ifdef __cplusplus
extern "C" {
#endif


/* Log Routine Definitions */

#ifndef	LOG_DEFS
#define	LOG_DEFS



/*	Reserved Log Entry values. */


#define LOG_EVENT_ENTRY					1	
#define LOG_EVENT_ENTRY_NO_TIMESTAMP	2
#define LOG_EVENT_ENTRY_NO_CONSOLE		3
#define MAX_RESERVED_LOG_ENTRY		3

/*	Public log flags. */

#define	LOG_LEAVE_LOCKED	0x0002
#define	LOG_AUTO_ROLLOVER	0x0004
#define	LOG_TO_CONSOLE		0x0008
#define	LOG_ADD_TIMESTAMP	0x0010
#define	LOG_FORMATTED_TEXT	0x0020	/* Text is already formatted */

STATUS LNPUBLIC LogCreateEntry (char far *FormName, WORD far *retLogEntry);
STATUS LNPUBLIC LogCompleteEntry (WORD LogEntry);
void   LNPUBLIC LogCloseEntry (WORD LogEntry);
STATUS LNPUBLIC LogAddTimeItem (WORD LogEntry, WORD Flags, char far *ItemName, TIMEDATE far *Time);
STATUS LNPUBLIC LogAddNumberItem (WORD LogEntry, WORD Flags, char far *ItemName, NUMBER far *Number);
STATUS LNPUBLIC LogAddTextlistItem (WORD LogEntry, WORD Flags, char far *ItemName, char far *Text);
STATUS LNVARARGS  LogAddText (WORD LogEntry, WORD Flags, char far *ItemName, HMODULE hModule, STATUS Message, char far *FormatString,...);
STATUS LNVARARGS  LogEvent (STATUS StringID, HMODULE hModule, STATUS AdditionalErrorCode,...);
STATUS LNVARARGS  LogEventText (char far *String, HMODULE hModule, STATUS AdditionalErrorCode,...);

#endif


#ifdef __cplusplus
}
#endif

