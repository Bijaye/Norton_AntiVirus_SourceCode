;//---------------------------------------------------------------------------
;// Please make sure you update the next message ID at the end of this file 
;//---------------------------------------------------------------------------

MessageIdTypedef=DWORD

SeverityNames=(Success=0x0:STATUS_SEVERITY_SUCCESS
               Informational=0x1:STATUS_SEVERITY_INFORMATIONAL
               Warning=0x2:STATUS_SEVERITY_WARNING
               Error=0x3:STATUS_SEVERITY_ERROR
              )

FacilityNames=(System=0x0:FACILITY_SYSTEM
               ccSNEvt=0xFF:FACILITY_CCSNEVT
              )

LanguageNames=(English=0x409:MSG00409)


;//---------------------------------------------------------------------------
;//   SymNetDrv messages
;// Note: each entry in this area must have a lookup entry in SNLookup.cpp in
;//       order to translate SymNetDrv values into their NISEvt equivalent.
;//---------------------------------------------------------------------------
;//
MessageId = 23
SymbolicName = SN_EV_SOURCE_ERROR
Language = English
Invalid source %1 specified for event %2.
.
MessageId = 24
SymbolicName = SN_EV_SOURCE_ERROR_HTML
Language = English
<p>Invalid source %1 specified for event %2.</p>
.

MessageId = 25
SymbolicName = SN_EV_SIZE_ERROR
Language = English
Unable to log event %1 from source %2 - event too large.
.
MessageId = 26
SymbolicName = SN_EV_SIZE_ERROR_HTML
Language = English
<p>Unable to log event %1 from source %2 - event too large.</p>
.

MessageId = 27
SymbolicName = SN_EV_EVENT_TYPE_ERROR
Language = English
Invalid type %1 specified in event %2 from source %3. 
.
MessageId = 28
SymbolicName = SN_EV_EVENT_TYPE_ERROR_HTML
Language = English
<p>Invalid type %1 specified in event %2 from source %3.</p>
.

MessageId = 29
SymbolicName = SN_EV_TEST_EVENT
Language = English
Test event, message number %1, event index %2.
.
MessageId = 30
SymbolicName = SN_EV_TEST_EVENT_HTML
Language = English
<p>Test event, message number %1, event index %2.</p>
.

MessageId = 31
SymbolicName = SN_EV_EVENT_ID_ERROR
Language = English
Invalid event ID %1 specified from source %2, type %3.
.
MessageId = 32
SymbolicName = SN_EV_EVENT_ID_ERROR_HTML
Language = English
<p>Invalid event ID %1 specified from source %2, type %3.</p>
.

;//---------------------------------------------------------------------------
;//    T D I    M e s s a g e s
;//---------------------------------------------------------------------------
;//
MessageId = 33
SymbolicName = SN_EV_TDI_UNKNOWN_IOCTL
Language=English
Unknown device I/O control code.
.
MessageId = 34
SymbolicName = SN_EV_TDI_UNKNOWN_IOCTL_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>Unknown device I/O control code.</p>
.

MessageId = 35
SymbolicName = SN_EV_TDI_ASSOCIATE_ADDRESS
Language=English
   %1: %2, Associated Address Object: %3, %4.
.
MessageId = 36
SymbolicName = SN_EV_TDI_ASSOCIATE_ADDRESS_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>%1: %2, Associated Address Object: %3, %4.</p>
.

MessageId = 37
SymbolicName = SN_EV_TDI_DISASSOCIATE_ADDRESS
Language=English
   %1: %2, %3.
.
MessageId = 38
SymbolicName = SN_EV_TDI_DISASSOCIATE_ADDRESS_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>%1: %2, %3.</p>
.

MessageId = 39
SymbolicName = SN_EV_TDI_CONNECT
Language=English
%1 %2: %3, Remote: %4(%5).
.
MessageId = 40
SymbolicName = SN_EV_TDI_CONNECT_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>%1 %2: %3, Remote: %4(%5).</p>
.

MessageId = 41
SymbolicName = SN_EV_TDI_LISTEN
Language=English
   Listen.
.
MessageId = 42
SymbolicName = SN_EV_TDI_LISTEN_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>Listen.</p>
.

MessageId = 43
SymbolicName = SN_EV_TDI_ACCEPT
Language=English
   %1: %2, Local IP(Port): %3(%4), %5.
.
MessageId = 44
SymbolicName = SN_EV_TDI_ACCEPT_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>%1: %2, Local IP(Port): %3(%4), %5.</p>
.

MessageId = 45
SymbolicName = SN_EV_TDI_DISCONNECT
Language=English
%1 %2: %3, %4.
.
MessageId = 46
SymbolicName = SN_EV_TDI_DISCONNECT_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>%1 %2: %3, %4.</p>
.

MessageId = 47
SymbolicName = SN_EV_TDI_SEND
Language=English
%1 %2: %3, ToSend: %4, Flags: %5.
.
MessageId = 48
SymbolicName = SN_EV_TDI_SEND_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>%1 %2: %3, ToSend: %4, Flags: %5.<p>
.

MessageId = 49
SymbolicName = SN_EV_TDI_RECEIVE
Language=English
   %1: %2, Received: %3, BufferSize: %4, Flags: %5, %6.
.
MessageId = 50
SymbolicName = SN_EV_TDI_RECEIVE_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>%1: %2, Received: %3, BufferSize: %4, Flags: %5, %6.</p>
.

MessageId = 51
SymbolicName = SN_EV_TDI_SEND_DATAGRAM
Language=English
%1 %2: %3, %4(%5), ToSend: %6, Flags: %7.
.
MessageId = 52
SymbolicName = SN_EV_TDI_SEND_DATAGRAM_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>%1 %2: %3, %4(%5), ToSend: %6, Flags: %7.</p>
.

MessageId = 53
SymbolicName = SN_EV_TDI_RECEIVE_DATAGRAM
Language=English
   %1: %2, %3(%4), Length: %5, %6.
.
MessageId = 54
SymbolicName = SN_EV_TDI_RECEIVE_DATAGRAM_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>%1: %2, %3(%4), Length: %5, %6.</p>
.

MessageId = 55
SymbolicName = SN_EV_TDI_SET_EVENT_HANDLER
Language=English
   %1: %2, %3 Handler: %4, Context: %5, %6.
.
MessageId = 56
SymbolicName = SN_EV_TDI_SET_EVENT_HANDLER_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>%1: %2, %3 Handler: %4, Context: %5, %6.</p>
.

MessageId = 57
SymbolicName = SN_EV_TDI_QUERY_INFORMATION
Language=English
   %1: %2, Query: %3, %4.
.
MessageId = 58
SymbolicName = SN_EV_TDI_QUERY_INFORMATION_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>%1: %2, Query: %3, %4.</p>
.

MessageId = 59
SymbolicName = SN_EV_TDI_SET_INFORMATION
Language=English
   Set Information.
.
MessageId = 60
SymbolicName = SN_EV_TDI_SET_INFORMATION_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>Set Information.</p>
.

MessageId = 61
SymbolicName = SN_EV_TDI_ACTION
Language=English
   Action.
.
MessageId = 62
SymbolicName = SN_EV_TDI_ACTION_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>Action.</p>
.

MessageId = 63
SymbolicName = SN_EV_TDI_DIRECT_SEND
Language=English
   Direct Send.
.
MessageId = 64
SymbolicName = SN_EV_TDI_DIRECT_SEND_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>Direct Send.</p>
.

MessageId = 65
SymbolicName = SN_EV_TDI_DIRECT_SEND_DATAGRAM
Language=English
   Direct Send Datagram.
.
MessageId = 66
SymbolicName = SN_EV_TDI_DIRECT_SEND_DATAGRAM_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>Direct Send Datagram.</p>
.

MessageId = 67
SymbolicName = SN_EV_TDI_STATUS_ERROR
Language=English
Unknown status code returned %1.
.
MessageId = 68
SymbolicName = SN_EV_TDI_STATUS_ERROR_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>Unknown status code returned %1.</p>
.

MessageId = 69
SymbolicName = SN_EV_TDI_QUERY_ADDRESS_INFORMATION
Language=English
   %1: %2, Query: %3, %4(%5), %6.
.
MessageId = 70
SymbolicName = SN_EV_TDI_QUERY_ADDRESS_INFORMATION_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>%1: %2, Query: %3, %4(%5), %6.</p>
.

MessageId = 71
SymbolicName = SN_EV_TDI_CREATE
Language=English
   %1: %2, %3 Object, %4.
.
MessageId = 72
SymbolicName = SN_EV_TDI_CREATE_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>%1: %2, %3 Object, %4.</p>
.

MessageId = 73
SymbolicName = SN_EV_TDI_CLEANUP
Language=English
   %1: %2, %3.
.
MessageId = 74
SymbolicName = SN_EV_TDI_CLEANUP_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>%1: %2, %3.</p>
.

MessageId = 75
SymbolicName = SN_EV_TDI_CLOSE
Language=English
   %1: %2, %3.
.
MessageId = 76
SymbolicName = SN_EV_TDI_CLOSE_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>%1: %2, %3.</p>
.

MessageId = 77
SymbolicName = SN_EV_TDI_DEVICE_CONTROL
Language=English
   %1: %2, Type: %3, Access: %4, Func: %5, Method: %6, %7, CtrlCode: %8.
.
MessageId = 78
SymbolicName = SN_EV_TDI_DEVICE_CONTROL_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>%1: %2, Type: %3, Access: %4, Func: %5, Method: %6, %7, CtrlCode: %8.</p>
.

MessageId = 79
SymbolicName = SN_EV_TDI_CREATE_ADDRESS
Language=English
   %1: %2, %3 Object, %4(%5), %6.
.
MessageId = 80
SymbolicName = SN_EV_TDI_CREATE_ADDRESS_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>%1: %2, %3 Object, %4(%5), %6.</p>
.

MessageId = 81
SymbolicName = SN_EV_TDI_CREATE_CONNECTION_CONTEXT
Language=English
   %1: %2, %3 Object, Context: %4, %5.
.
MessageId = 82
SymbolicName = SN_EV_TDI_CREATE_CONNECTION_CONTEXT_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>%1: %2, %3 Object, Context: %4, %5.</p>
.

MessageId = 83
SymbolicName = SN_EV_TDI_QUERY_INFORMATION_EX
Language=English
   %1: %2, %3, %4, %5, Id: %6, Inst: %7, %8.
.
MessageId = 84
SymbolicName = SN_EV_TDI_QUERY_INFORMATION_EX_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>%1: %2, %3, %4, %5, Id: %6, Inst: %7, %8.</p>
.

MessageId = 85
SymbolicName = SN_EV_TDI_SET_INFORMATION_EX
Language=English
   %1: %2, %3, %4, %5, Id: %6, Inst: %7, %8.
.
MessageId = 86
SymbolicName = SN_EV_TDI_SET_INFORMATION_EX_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>%1: %2, %3, %4, %5, Id: %6, Inst: %7, %8.</p>
.

MessageId = 87
SymbolicName = SN_EV_TDI_CONNECT_EVENT
Language=English
   %1: %2, %3(%4), Connection: %5, Context: %6, %7.
.
MessageId = 88
SymbolicName = SN_EV_TDI_CONNECT_EVENT_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>%1: %2, %3(%4), Connection: %5, Context: %6, %7.</p>
.

MessageId = 89
SymbolicName = SN_EV_TDI_DISCONNECT_EVENT
Language=English
%1 %2: %3, %4.
.
MessageId = 90
SymbolicName = SN_EV_TDI_DISCONNECT_EVENT_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>%1 %2: %3, %4.</p>
.

MessageId = 91
SymbolicName = SN_EV_TDI_ERROR_EVENT
Language=English
   %1: %2, %3.
.
MessageId = 92
SymbolicName = SN_EV_TDI_ERROR_EVENT_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>%1: %2, %3.</p>
.

MessageId = 93
SymbolicName = SN_EV_TDI_RECEIVE_EVENT
Language=English
   %1: %2, Avail: %3, Ind: %4, Taken: %5, Flags: %6, %7.
.
MessageId = 94
SymbolicName = SN_EV_TDI_RECEIVE_EVENT_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>%1: %2, Avail: %3, Ind: %4, Taken: %5, Flags: %6, %7.</p>
.

MessageId = 95
SymbolicName = SN_EV_TDI_RECEIVE_DATAGRAM_EVENT
Language=English
%1 %2: %3, %4(%5), Avail: %6, Ind: %7, Taken: %8, Flags: %9, %10.
.
MessageId = 96
SymbolicName = SN_EV_TDI_RECEIVE_DATAGRAM_EVENT_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>%1 %2: %3, %4(%5), Avail: %6, Ind: %7, Taken: %8, Flags: %9, %10.</p>
.

MessageId = 97
SymbolicName = SN_EV_TDI_RECEIVE_EXPEDITED_EVENT
Language=English
%1: %2, Available: %3, Indicated: %4, Taken: %5, Flags: %6, %7.
.
MessageId = 98
SymbolicName = SN_EV_TDI_RECEIVE_EXPEDITED_EVENT_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>%1: %2, Available: %3, Indicated: %4, Taken: %5, Flags: %6, %7.</p>
.

MessageId = 99
SymbolicName = SN_EV_TDI_SEND_POSSIBLE_EVENT
Language=English
   %1: %2, Event Handler: %3, Context: %4, %5.
.
MessageId = 100
SymbolicName = SN_EV_TDI_SEND_POSSIBLE_EVENT_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
   %1: %2, Event Handler: %3, Context: %4, %5.
.

MessageId = 101
SymbolicName = SN_EV_TDI_CHAINED_RECEIVE_EVENT
Language=English
%1: %2, Event Handler: %3, Context: %4, %5.
.
MessageId = 102
SymbolicName = SN_EV_TDI_CHAINED_RECEIVE_EVENT_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>%1: %2, Event Handler: %3, Context: %4, %5.</p>
.

MessageId = 103
SymbolicName = SN_EV_TDI_CHAINED_RECEIVE_DATAGRAM_EVENT
Language=English
%1 %2: %3, Event Handler: %4, Context: %5, %6.
.
MessageId = 104
SymbolicName = SN_EV_TDI_CHAINED_RECEIVE_DATAGRAM_EVENT_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>%1 %2: %3, Event Handler: %4, Context: %5, %6.</p>
.

MessageId = 105
SymbolicName = SN_EV_TDI_CHAINED_RECEIVE_EXPEDITED_EVENT
Language=English
%1: %2, Event Handler: %3, Context: %4, %5.
.
MessageId = 106
SymbolicName = SN_EV_TDI_CHAINED_RECEIVE_EXPEDITED_EVENT_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>%1: %2, Event Handler: %3, Context: %4, %5.</p>
.

MessageId = 107
SymbolicName = SN_EV_TDI_NOT_ENOUGH_STACKS
Language=English
   Not enough stacks for IRP: %1, %2, %3.
.
MessageId = 108
SymbolicName = SN_EV_TDI_NOT_ENOUGH_STACKS_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>Not enough stacks for IRP: %1, %2, %3.</p>
.

MessageId = 109
SymbolicName = SN_EV_TDI_RECEIVE_FILTERED
Language=English
%1 %2: %3, BufferSize: %4, %5.
.
MessageId = 110
SymbolicName = SN_EV_TDI_RECEIVE_FILTERED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>%1 %2: %3, BufferSize: %4, %5.</p>
.

MessageId = 111
SymbolicName = SN_EV_TDI_RECEIVE_COMPLETE_FILTERED
Language=English
%1 %2: %3, Received: %4, BufferSize: %5, Flags: %6, %7.
.
MessageId = 112
SymbolicName = SN_EV_TDI_RECEIVE_COMPLETE_FILTERED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>%1 %2: %3, Received: %4, BufferSize: %5, Flags: %6, %7.</p>
.

MessageId = 113
SymbolicName = SN_EV_TDI_RECEIVE_EVENT_FILTER_IN
Language=English
>> %1: %2, Available: %3, Indicated: %4, Copied: %5, Flags: %6.
.
MessageId = 114
SymbolicName = SN_EV_TDI_RECEIVE_EVENT_FILTER_IN_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>>> %1: %2, Available: %3, Indicated: %4, Copied: %5, Flags: %6.</p>
.

MessageId = 115
SymbolicName = SN_EV_TDI_RECEIVE_EVENT_FILTER_OUT
Language=English
-> %1: %2, Indicated: %3, Taken: %4, Flags: %5, %6.
.
MessageId = 116
SymbolicName = SN_EV_TDI_RECEIVE_EVENT_FILTER_OUT_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>-> %1: %2, Indicated: %3, Taken: %4, Flags: %5, %6.</p>
.

MessageId = 117
SymbolicName = SN_EV_TDI_DISCONNECT_EVENT_DEFERRED
Language=English
   %1: %2, Receive data pending - Disconnect deferred.
.
MessageId = 118
SymbolicName = SN_EV_TDI_DISCONNECT_EVENT_DEFERRED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>%1: %2, Receive data pending - Disconnect deferred.</p>
.

MessageId = 119
SymbolicName = SN_EV_TDI_REFERENCE_COUNT_ERROR
Language=English
   %1: %2, Reference count error, Count: %3.
.
MessageId = 120
SymbolicName = SN_EV_TDI_REFERENCE_COUNT_ERROR_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>%1: %2, Reference count error, Count: %3.</p>
.

MessageId = 121
SymbolicName = SN_EV_TDI_DUPLICATE_HASH_ENTRY
Language=English
   Duplicate hash entry: new --> %1 %2 Object: %3, existing --> %4 %5 Object: %6.
.
MessageId = 122
SymbolicName = SN_EV_TDI_DUPLICATE_HASH_ENTRY_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>Duplicate hash entry: new --> %1 %2 Object: %3, existing --> %4 %5 Object: %6.</p>
.

MessageId = 123
SymbolicName = SN_EV_TDI_QUEUE_DELETE
Language=English
   %1 %2: %3, delete %4 from %5 queue.
.
MessageId = 124
SymbolicName = SN_EV_TDI_QUEUE_DELETE_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>%1 %2: %3, delete %4 from %5 queue.</p>
.

MessageId = 125
SymbolicName = SN_EV_TDI_FILTER_CONNECT
Language=English
   %1: %2, FilterConnect: %3, Status: %4.
.
MessageId = 126
SymbolicName = SN_EV_TDI_FILTER_CONNECT_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>%1: %2, FilterConnect: %3, Status: %4.</p>
.

MessageId = 127
SymbolicName = SN_EV_TDI_FILTER_DISCONNECT
Language=English
   %1: %2, FilterDisconnect: %3, Status: %4.
.
MessageId = 128
SymbolicName = SN_EV_TDI_FILTER_DISCONNECT_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>%1: %2, FilterDisconnect: %3, Status: %4.</p>
.

MessageId = 129
SymbolicName = SN_EV_TDI_FILTER_FLUSH_RECEIVE_QUEUE
Language=English
   %1: %2,  FilterFlushReceiveQueue: %3, Status: %4.
.
MessageId = 130
SymbolicName = SN_EV_TDI_FILTER_FLUSH_RECEIVE_QUEUE_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>%1: %2,  FilterFlushReceiveQueue: %3, Status: %4.</p>
.

MessageId = 131
SymbolicName = SN_EV_TDI_FILTER_FLUSH_SEND_QUEUE
Language=English
   %1: %2,  FilterFlushSendQueue: %3, Status: %4.
.
MessageId = 132
SymbolicName = SN_EV_TDI_FILTER_FLUSH_SEND_QUEUE_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>%1: %2,  FilterFlushSendQueue: %3, Status: %4.</p>
.

MessageId = 133
SymbolicName = SN_EV_TDI_FILTER_RECEIVE
Language=English
   %1: %2,  FilterReceive: %3, BytesFiltered: %4, Status: %5.
.
MessageId = 134
SymbolicName = SN_EV_TDI_FILTER_RECEIVE_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>%1: %2,  FilterReceive: %3, BytesFiltered: %4, Status: %5.</p>
.

MessageId = 135
SymbolicName = SN_EV_TDI_FILTER_SEND
Language=English
   %1: %2,  FilterSend: %3, BytesFiltered: %4, Status: %5.
.
MessageId = 136
SymbolicName = SN_EV_TDI_FILTER_SEND_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>%1: %2,  FilterSend: %3, BytesFiltered: %4, Status: %5.</p>
.

MessageId = 137
SymbolicName = SN_EV_HTTP_FLUSHPROB
Language=English
Flushing with undelivered data - Parser state %1
The HTTP filter had undelivered data at the time that the connection was terminated.
Causes include: user reset, app slow to read, badly formatted HTML, or a parser bug.
.
MessageId = 138
SymbolicName = SN_EV_HTTP_FLUSHPROB_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>Flushing with undelivered data - Parser state %1
The HTTP filter had undelivered data at the time that the connection was terminated.
Causes include: user reset, app slow to read, badly formatted HTML, or a parser bug.</p>
.

MessageId = 139
SymbolicName = SN_EV_HTTP_COOKIE
Language=English
%1.
.
MessageId = 140
SymbolicName = SN_EV_HTTP_COOKIE_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>%1.</p
.
MessageId = 32907
SymbolicName = SN_EV_HTTP_COOKIE_SHORT
Language=English
%1.
.

MessageId = 141
SymbolicName = SN_EV_HTTP_ERROR
Language=English
Error - %1.
.
MessageId = 142
SymbolicName = SN_EV_HTTP_ERROR_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>Error - %1.</p>
.

MessageId = 143
SymbolicName = SN_EV_HTTP_AD_REMOVED
Language=English
Removed %1
From    %2
Because %3
.
MessageId = 144
SymbolicName = SN_EV_HTTP_AD_REMOVED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Removed:</td>
		<td class="NisEvtEventValue">%1</td>
	</tr>
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">From:</td>
		<td class="NisEvtEventValue">%2</td>
	</tr>
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Because:</td>
		<td class="NisEvtEventValue">%3</td>
	</tr>
</table>
.
MessageId = 32911
SymbolicName = SN_EV_HTTP_AD_REMOVED_SHORT
Language=English
Blocked ad from %2.
.

MessageId = 145
SymbolicName = SN_EV_HTTP_FRAGMENT
Language=English
%1
This is an HTML fragment likely to be associated with an image that appeared
on a Web page that was recently viewed.  Placing it (or a portion of it) in
the blocklist will prevent further reception of the image.
.
MessageId = 146
SymbolicName = SN_EV_HTTP_FRAGMENT_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			%1<br>
			This is an HTML fragment likely to be associated with an image that appeared 
			on a Web page that was recently viewed.  Placing it (or a portion of it) in 
			the blocklist will prevent further reception of the image.
		</td>
	</tr>
</table>
.

MessageId = 147
SymbolicName = SN_EV_TDI_FILTER_ACCEPT
Language=English
%1: %2, FilterAccept: %3, Status: %4.
.
MessageId = 148
SymbolicName = SN_EV_TDI_FILTER_ACCEPT_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>%1: %2, FilterAccept: %3, Status: %4.</p>
.

MessageId = 149
SymbolicName = SN_EV_TDI_FILTER_RECEIVE_DATAGRAM
Language=English
%1: %2,  FilterDatagramReceive: %3, BytesFiltered: %4, Status: %5.
.
MessageId = 150
SymbolicName = SN_EV_TDI_FILTER_RECEIVE_DATAGRAM_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>%1: %2,  FilterDatagramReceive: %3, BytesFiltered: %4, Status: %5.</p>
.

MessageId = 151
SymbolicName = SN_EV_TDI_FILTER_SEND_DATAGRAM
Language=English
%1: %2,  FilterDatagramSend: %3, BytesFiltered: %4, Status: %5.
.
MessageId = 152
SymbolicName = SN_EV_TDI_FILTER_SEND_DATAGRAM_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>%1: %2,  FilterDatagramSend: %3, BytesFiltered: %4, Status: %5.</p>
.

MessageId = 153
SymbolicName = SN_EV_URL_LOG
Language=English
%1.
.
MessageId = 154
SymbolicName = SN_EV_URL_LOG_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>%1.</p>
.

MessageId = 155
SymbolicName = SN_EV_TDI_FILTER_CONNECT_COMPLETED
Language=English
%1: %2, FilterConnectCompleted: %3, Status: %4.
.
MessageId = 156
SymbolicName = SN_EV_TDI_FILTER_CONNECT_COMPLETED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>%1: %2, FilterConnectCompleted: %3, Status: %4.</p>
.

MessageId = 157
SymbolicName = SN_EV_TDI_DISCONNECT_EVENT_COMPLETE
Language=English
%1 %2: %3, %4, %5.
.
MessageId = 158
SymbolicName = SN_EV_TDI_DISCONNECT_EVENT_COMPLETE_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>%1 %2: %3, %4, %5.</p>
.

MessageId = 159
SymbolicName = SN_EV_TDI_CONNECT_COMPLETE
Language=English
%1 %2: %3, Local: %4(%5), %6.
.
MessageId = 160
SymbolicName = SN_EV_TDI_CONNECT_COMPLETE_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>%1 %2: %3, Local: %4(%5), %6.</p>
.

MessageId = 161
SymbolicName = SN_EV_TDI_CONNECT1
Language=English
%1 %2: %3.
.
MessageId = 162
SymbolicName = SN_EV_TDI_CONNECT1_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>%1 %2: %3.</p>
.

MessageId = 163
SymbolicName = SN_EV_TDI_DISCONNECT_COMPLETE
Language=English
%1 %2: %3, %4.
.
MessageId = 164
SymbolicName = SN_EV_TDI_DISCONNECT_COMPLETE_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>%1 %2: %3, %4.</p>
.

MessageId = 165
SymbolicName = SN_EV_TDI_SEND_COMPLETE
Language=English
%1 %2: %3, Sent: %4, %5.
.
MessageId = 166
SymbolicName = SN_EV_TDI_SEND_COMPLETE_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>%1 %2: %3, Sent: %4, %5.</p>
.

MessageId = 167
SymbolicName = SN_EV_TDI_SEND_DATAGRAM_COMPLETE
Language=English
%1 %2: %3, Sent: %4, %5.
.
MessageId = 168
SymbolicName = SN_EV_TDI_SEND_DATAGRAM_COMPLETE_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>%1 %2: %3, Sent: %4, %5.</p>
.

MessageId = 169
SymbolicName = SN_EV_TDI_RECEIVE_DATAGRAM_COMPLETE_FILTERED
Language=English
%1 %2: %3, %4(%5), Received: %6, BufferSize: %7, Flags: %8, %9.
.
MessageId = 170
SymbolicName = SN_EV_TDI_RECEIVE_DATAGRAM_COMPLETE_FILTERED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>%1 %2: %3, %4(%5), Received: %6, BufferSize: %7, Flags: %8, %9.</p>
.

MessageId = 171
SymbolicName = SN_EV_TDI_DUPLICATE_CC_HASH_ENTRY
Language=English
   Duplicate hash entry for %1 %2: %3, connection objects: new -> %4, old -> %5.
.
MessageId = 172
SymbolicName = SN_EV_TDI_DUPLICATE_CC_HASH_ENTRY_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>Duplicate hash entry for %1 %2: %3, connection objects: new -> %4, old -> %5.</p>
.

MessageId = 173
SymbolicName = SN_EV_TDI_RING0_THREAD_START
Language=English
Ring 0 Thread has started successfully.
.
MessageId = 174
SymbolicName = SN_EV_TDI_RING0_THREAD_START_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>Ring 0 Thread has started successfully.</p>
.

MessageId = 175
SymbolicName = SN_EV_TDI_DEVICE_INIT
Language=English
Device Init called.
.
MessageId = 176
SymbolicName = SN_EV_TDI_DEVICE_INIT_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>Device Init called.</p>
.

MessageId = 177
SymbolicName = SN_EV_TDI_INIT_COMPLETE
Language=English
Init Complete called.
.
MessageId = 178
SymbolicName = SN_EV_TDI_INIT_COMPLETE_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>Init Complete called.</p>
.

MessageId = 179
SymbolicName = SN_EV_TDI_SYS_VM_INIT
Language=English
Sys VM Init called.
.
MessageId = 180
SymbolicName = SN_EV_TDI_SYS_VM_INIT_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>Sys VM Init called.</p>
.

MessageId = 181
SymbolicName = SN_EV_TDI_OPEN_ALERT_HANDLER
Language=English
Open alert handler.
.
MessageId = 182
SymbolicName = SN_EV_TDI_OPEN_ALERT_HANDLER_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>Open alert handler.</p>
.

MessageId = 183
SymbolicName = SN_EV_TDI_CLOSE_ALERT_HANDLER
Language=English
Close alert handler.
.
MessageId = 184
SymbolicName = SN_EV_TDI_CLOSE_ALERT_HANDLER_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>Close alert handler.</p>
.

MessageId = 185
SymbolicName = SN_EV_TDI_FILTER_FAILED_TO_LOAD
Language=English
%1 failed to load (%2).
.
MessageId = 186
SymbolicName = SN_EV_TDI_FILTER_FAILED_TO_LOAD_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>%1 failed to load (%2).</p>
.

;//---------------------------------------------------------------------------
;//    I P	F i l t e r		M e s s a g e s
;//---------------------------------------------------------------------------
;//
MessageId = 187
SymbolicName = SN_EV_IP_FILTER_GET_IP_FILTER_RULE_FAILED
Language=English
GetIPFilterRule() call failed with an error "%1".
.
MessageId = 188
SymbolicName = SN_EV_IP_FILTER_GET_IP_FILTER_RULE_FAILED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>GetIPFilterRule() call failed with an error "%1".</p>
.

MessageId = 189
SymbolicName = SN_EV_IP_FILTER_PROCESSED_RULE
Language=English
Internet Worm Protection configuration updated: %1 rules.
.
MessageId = 190
SymbolicName = SN_EV_IP_FILTER_PROCESSED_RULE_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>Internet Worm Protection configuration updated: %1 rules.</p>
.

MessageId = 191
SymbolicName = SN_EV_IP_FILTER_INVALID_SECTION_HEADER_OBJECT_TYPE
Language=English
GetObjectInfoFromSectionKey() found an invalid object type.  Section Key is "%1".
.
MessageId = 192
SymbolicName = SN_EV_IP_FILTER_INVALID_SECTION_HEADER_OBJECT_TYPE_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>GetObjectInfoFromSectionKey() found an invalid object type.  Section Key is "%1".</p>
.

MessageId = 193
SymbolicName = SN_EV_IP_FILTER_INVALID_SECTION_HEADER_OBJECT_NAME
Language=English
GetObjectInfoFromSectionKey() found an invalid object name.  Section Key is "%1".
.
MessageId = 194
SymbolicName = SN_EV_IP_FILTER_INVALID_SECTION_HEADER_OBJECT_NAME_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>GetObjectInfoFromSectionKey() found an invalid object name.  Section Key is "%1".</p>
.

MessageId = 195
SymbolicName = SN_EV_IP_FILTER_UNKNOWN_IP_FILTER_RULE_VALUE
Language=English
GetIPFilterRule() ignoring unknown IP Filter Rule value of "%1".  IP Filter Object name is "%2".
.
MessageId = 196
SymbolicName = SN_EV_IP_FILTER_UNKNOWN_IP_FILTER_RULE_VALUE_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>GetIPFilterRule() ignoring unknown IP Filter Rule value of "%1".  IP Filter Object name is "%2".</p>
.

MessageId = 197
SymbolicName = SN_EV_IP_FILTER_UNABLE_TO_ADD_OBJECT
Language=English
FillInObjectForRule() unable to add object type "%1" named "%2", from section key "%3".
.
MessageId = 198
SymbolicName = SN_EV_IP_FILTER_UNABLE_TO_ADD_OBJECT_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>FillInObjectForRule() unable to add object type "%1" named "%2", from section key "%3".</p>
.

MessageId = 199
SymbolicName = SN_EV_IP_FILTER_UNKNOWN_IP_OBJECT_TYPE
Language=English
AddIPObjectToList() unknown IP Object type of #%1.
.
MessageId = 200
SymbolicName = SN_EV_IP_FILTER_UNKNOWN_IP_OBJECT_TYPE_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>AddIPObjectToList() unknown IP Object type of #%1.</p>
.

MessageId = 201
SymbolicName = SN_EV_IP_FILTER_UNKNOWN_HOST_OBJECT_VALUE
Language=English
InsertIPHostObject() ignoring unknown host object value of "%1".
.
MessageId = 202
SymbolicName = SN_EV_IP_FILTER_UNKNOWN_HOST_OBJECT_VALUE_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>InsertIPHostObject() ignoring unknown host object value of "%1".</p>
.

MessageId = 203
SymbolicName = SN_EV_IP_FILTER_UNKNOWN_NETWORK_OBJECT_VALUE
Language=English
InsertIPNetworkObject() ignoring unknown network object value of "%1".
.
MessageId = 204
SymbolicName = SN_EV_IP_FILTER_UNKNOWN_NETWORK_OBJECT_VALUE_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>InsertIPNetworkObject() ignoring unknown network object value of "%1".</p>
.

MessageId = 205
SymbolicName = SN_EV_IP_FILTER_UNKNOWN_RANGE_OBJECT_VALUE
Language=English
InsertIPRangeObject() ignoring unknown range object value of "%1".
.
MessageId = 206
SymbolicName = SN_EV_IP_FILTER_UNKNOWN_RANGE_OBJECT_VALUE_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>InsertIPRangeObject() ignoring unknown range object value of "%1".</p>
.

MessageId = 207
SymbolicName = SN_EV_IP_FILTER_UNKNOWN_GROUP_IP_OBJECT_TYPE
Language=English
ExpandIPGroupObject() ignoring unknown IP object type of "%1".
.
MessageId = 208
SymbolicName = SN_EV_IP_FILTER_UNKNOWN_GROUP_IP_OBJECT_TYPE_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>ExpandIPGroupObject() ignoring unknown IP object type of "%1".</p>
.

MessageId = 209
SymbolicName = SN_EV_IP_FILTER_UNABLE_TO_EXPAND_IP_GROUP_OBJECT
Language=English
InsertIPGroupObject() unable to expand IP Group object of "%1".
.
MessageId = 210
SymbolicName = SN_EV_IP_FILTER_UNABLE_TO_EXPAND_IP_GROUP_OBJECT_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>InsertIPGroupObject() unable to expand IP Group object of "%1".</p>
.

MessageId = 211
SymbolicName = SN_EV_IP_FILTER_UNABLE_TO_ADD_GROUP_LIST_MEMBER_OBJECT
Language=English
ExpandIPGroupObject() unable to add group "%1" list member object of section key "%2".
.
MessageId = 212
SymbolicName = SN_EV_IP_FILTER_UNABLE_TO_ADD_GROUP_LIST_MEMBER_OBJECT_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>ExpandIPGroupObject() unable to add group "%1" list member object of section key "%2".</p>
.

MessageId = 213
SymbolicName = SN_EV_IP_FILTER_UNABLE_TO_ADD_SERVICE_LIST_MEMBER_OBJECT
Language=English
AddServiceObjectToList() unable to add service "%1" list member object of section key "%2".
.
MessageId = 214
SymbolicName = SN_EV_IP_FILTER_UNABLE_TO_ADD_SERVICE_LIST_MEMBER_OBJECT_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>AddServiceObjectToList() unable to add service "%1" list member object of section key "%2".</p>
.

MessageId = 215
SymbolicName = SN_EV_IP_FILTER_UNKNOWN_SERVICE_VALUE
Language=English
AddServiceObjectToList() ignoring unknown service object value of "%1".
.
MessageId = 216
SymbolicName = SN_EV_IP_FILTER_UNKNOWN_SERVICE_VALUE_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>AddServiceObjectToList() ignoring unknown service object value of "%1".</p>
.

MessageId = 217
SymbolicName = SN_EV_IP_FILTER_UNKNOWN_SERVICE_OBJECT_TYPE
Language=English
AddServiceObjectToList() unknown service object type of %1.
.
MessageId = 218
SymbolicName = SN_EV_IP_FILTER_UNKNOWN_SERVICE_OBJECT_TYPE_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>AddServiceObjectToList() unknown service object type of %1.</p>
.

MessageId = 219
SymbolicName = SN_EV_IP_FILTER_UNKNOWN_TIME_VALUE
Language=English
AddActivationTimeObjectToList() ignoring unknown time object value of "%1".
.
MessageId = 220
SymbolicName = SN_EV_IP_FILTER_UNKNOWN_TIME_VALUE_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>AddActivationTimeObjectToList() ignoring unknown time object value of "%1".</p>
.

MessageId = 221
SymbolicName = SN_EV_IP_FILTER_UNKNOWN_TIME_OBJECT_TYPE
Language=English
AddActivationTimeObjectToList() unknown service object type of %1.
.
MessageId = 222
SymbolicName = SN_EV_IP_FILTER_UNKNOWN_TIME_OBJECT_TYPE_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>AddActivationTimeObjectToList() unknown service object type of %1.</p>
.

MessageId = 223
SymbolicName = SN_EV_IP_FILTER_MEMORY_ALLOCATION_FAILED
Language=English
IPFilterAllocateMemory() unable to allocate %1 bytes of memory.
.
MessageId = 224
SymbolicName = SN_EV_IP_FILTER_MEMORY_ALLOCATION_FAILED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>IPFilterAllocateMemory() unable to allocate %1 bytes of memory.</p>
.

MessageId = 225
SymbolicName = SN_EV_IP_FILTER_UNABLE_TO_FREE_OBJECT_TYPE
Language=English
FreeIPGroupObject() unable to free object type %1. 
.
MessageId = 226
SymbolicName = SN_EV_IP_FILTER_UNABLE_TO_FREE_OBJECT_TYPE_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>FreeIPGroupObject() unable to free object type %1.</p>
.

MessageId = 227
SymbolicName = SN_EV_IP_FILTER_BUILD_RULES_FROM_REGISTRY_FAILED
Language=English
Unable to build IP filter rule base.   Error is "%1".
.
MessageId = 228
SymbolicName = SN_EV_IP_FILTER_BUILD_RULES_FROM_REGISTRY_FAILED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>Unable to build IP filter rule base.   Error is "%1".</p>
.

MessageId = 229
SymbolicName = SN_EV_IP_FILTER_OUTGOING_CONNECT_REQUEST_REJECTED
Language=English
Rule "%1" blocked (%2,%3).
Outbound TCP connection.
Remote address,service is (%4,%5).
Process name is "%6".
.
MessageId = 230
SymbolicName = SN_EV_IP_FILTER_OUTGOING_CONNECT_REQUEST_REJECTED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Rule "%1" blocked (%2,%3).<br>
			Outbound TCP connection.<br>
			Remote address,service is (%4,%5).<br>
			Process name is "%6".<br>
		</td>
	</tr>
</table>
.

MessageId = 231
SymbolicName = SN_EV_IP_FILTER_UNABLE_TO_PARSE_SECTION_HEADER
Language=English
FillInObjectForRule() unable to parse section key "%1".
.
MessageId = 232
SymbolicName = SN_EV_IP_FILTER_UNABLE_TO_PARSE_SECTION_HEADER_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>FillInObjectForRule() unable to parse section key "%1".</p>
.

MessageId = 233
SymbolicName = SN_EV_IP_FILTER_UNABLE_TO_ADD_HOST_LIST_MEMBER_OBJECT
Language=English
InsertIPHostObject() unable to add host "%1" list member object of section key "%2".
.
MessageId = 234
SymbolicName = SN_EV_IP_FILTER_UNABLE_TO_ADD_HOST_LIST_MEMBER_OBJECT_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>InsertIPHostObject() unable to add host "%1" list member object of section key "%2".</p>
.

MessageId = 235
SymbolicName = SN_EV_IP_FILTER_UNABLE_TO_ADD_APPLICATION_LIST_MEMBER_OBJECT
Language=English
AddApplicationObjectToList() unable to add application "%1" list member object of section key "%2".
.
MessageId = 236
SymbolicName = SN_EV_IP_FILTER_UNABLE_TO_ADD_APPLICATION_LIST_MEMBER_OBJECT_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>AddApplicationObjectToList() unable to add application "%1" list member object of section key "%2".</p>
.

MessageId = 237
SymbolicName = SN_EV_IP_FILTER_UNKNOWN_APPLICATION_VALUE
Language=English
AddApplicationObjectToList() ignoring unknown application object value of "%1".
.
MessageId = 238
SymbolicName = SN_EV_IP_FILTER_UNKNOWN_APPLICATION_VALUE_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>AddApplicationObjectToList() ignoring unknown application object value of "%1".</p>
.

MessageId = 239
SymbolicName = SN_EV_IP_FILTER_UNKNOWN_APPLICATION_OBJECT_TYPE
Language=English
AddApplicationObjectToList() unknown application object type of %1.
.
MessageId = 240
SymbolicName = SN_EV_IP_FILTER_UNKNOWN_APPLICATION_OBJECT_TYPE_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>AddApplicationObjectToList() unknown application object type of %1.</p>
.

MessageId = 241
SymbolicName = SN_EV_IP_FILTER_RUNTIME_UNKNOWN_APPLICATION_OBJECT_TYPE
Language=English
DoesApplicationNameMatch() looking for object "%1" but instead found object "%2".
.
MessageId = 242
SymbolicName = SN_EV_IP_FILTER_RUNTIME_UNKNOWN_APPLICATION_OBJECT_TYPE_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>DoesApplicationNameMatch() looking for object "%1" but instead found object "%2".</p>
.

MessageId = 243
SymbolicName = SN_EV_IP_FILTER_RUNTIME_UNKNOWN_TIME_OBJECT_TYPE
Language=English
IsApplicationTimeInEffect() looking for object "%1" but instead found object "%2".
.
MessageId = 244
SymbolicName = SN_EV_IP_FILTER_RUNTIME_UNKNOWN_TIME_OBJECT_TYPE_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>IsApplicationTimeInEffect() looking for object "%1" but instead found object "%2".</p>
.

MessageId = 245
SymbolicName = SN_EV_IP_FILTER_RUNTIME_UNKNOWN_SERVICE_OBJECT_TYPE
Language=English
IsServiceInObjectList() looking for object "%1" but instead found object "%2".
.
MessageId = 246
SymbolicName = SN_EV_IP_FILTER_RUNTIME_UNKNOWN_SERVICE_OBJECT_TYPE_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>IsServiceInObjectList() looking for object "%1" but instead found object "%2".</p>
.

MessageId = 247
SymbolicName = SN_EV_IP_FILTER_RUNTIME_UNKNOWN_IP_OBJECT_TYPE
Language=English
IsIPAddressInObjectList() looking for object "%1", "%2", "%3", or "%4", but instead found object "%5".
.
MessageId = 248
SymbolicName = SN_EV_IP_FILTER_RUNTIME_UNKNOWN_IP_OBJECT_TYPE_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>IsIPAddressInObjectList() looking for object "%1", "%2", "%3", or "%4", but instead found object "%5".</p>
.

MessageId = 249
SymbolicName = SN_EV_IP_FILTER_UNKNOWN_RUNTIME_HOST_OBJECT_TYPE
Language=English
IsHostIPObjectInList() looking for object "%1", but instead found object "%2".
.
MessageId = 250
SymbolicName = SN_EV_IP_FILTER_UNKNOWN_RUNTIME_HOST_OBJECT_TYPE_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>IsHostIPObjectInList() looking for object "%1", but instead found object "%2".</p>
.

MessageId = 251
SymbolicName = SN_EV_IP_FILTER_UNKNOWN_RUNTIME_NETWORK_OBJECT_TYPE
Language=English
IsNetworkIPObjectInList() looking for object "%1", but instead found object "%2".
.
MessageId = 252
SymbolicName = SN_EV_IP_FILTER_UNKNOWN_RUNTIME_NETWORK_OBJECT_TYPE_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>IsNetworkIPObjectInList() looking for object "%1", but instead found object "%2".</p>
.

MessageId = 253
SymbolicName = SN_EV_IP_FILTER_UNKNOWN_RUNTIME_RANGE_OBJECT_TYPE
Language=English
IsRangeIPObjectInList() looking for object "%1", but instead found object "%2".
.
MessageId = 254
SymbolicName = SN_EV_IP_FILTER_UNKNOWN_RUNTIME_RANGE_OBJECT_TYPE_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>IsRangeIPObjectInList() looking for object "%1", but instead found object "%2".</p>
.

MessageId = 255
SymbolicName = SN_EV_IP_FILTER_UNKNOWN_RUNTIME_GROUP_OBJECT_TYPE
Language=English
IsGroupIPObjectInList() looking for object "%1", but instead found object "%2".
.
MessageId = 256
SymbolicName = SN_EV_IP_FILTER_UNKNOWN_RUNTIME_GROUP_OBJECT_TYPE_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>IsGroupIPObjectInList() looking for object "%1", but instead found object "%2".</p>
.

MessageId = 257
SymbolicName = SN_EV_IP_FILTER_RUNTIME_UNKNOWN_GROUP_IP_OBJECT_TYPE
Language=English
IsGroupIPObjectInList() looking for object "%1", "%2", "%3", or "%4", but instead found object "%5".
.
MessageId = 258
SymbolicName = SN_EV_IP_FILTER_RUNTIME_UNKNOWN_GROUP_IP_OBJECT_TYPE_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>IsGroupIPObjectInList() looking for object "%1", "%2", "%3", or "%4", but instead found object "%5".</p>
.

MessageId = 259
SymbolicName = SN_EV_IP_FILTER_POST_PROCESS_OUTGOING_CONNECT_REQUEST_REJECTED
Language=English
Rule "%1" blocked (%2,%3).
Outbound TCP connection. 
Local address,service is (%4,%5).
Remote address,service is (%6,%7).
Process name is "%8".
.
MessageId = 260
SymbolicName = SN_EV_IP_FILTER_POST_PROCESS_OUTGOING_CONNECT_REQUEST_REJECTED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Rule "%1" blocked (%2,%3).<br>
			Outbound TCP connection.<br>
			Local address,service is (%4,%5).<br>
			Remote address,service is (%6,%7).<br>
			Process name is "%8".<br>
		</td>
	</tr>
</table>
.

MessageId = 261
SymbolicName = SN_EV_IP_FILTER_INCOMING_CONNECT_REQUEST_REJECTED
Language=English
Rule "%1" blocked (%2,%3).
Inbound TCP connection. 
Local address,service is (%4,%5).
Remote address,service is (%6,%7).
Process name is "%8".
.
MessageId = 262
SymbolicName = SN_EV_IP_FILTER_INCOMING_CONNECT_REQUEST_REJECTED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Rule "%1" blocked (%2,%3).<br>
			Inbound TCP connection.<br>
			Local address,service is (%4,%5).<br>
			Remote address,service is (%6,%7).<br>
			Process name is "%8".<br>
		</td>
	</tr>
</table>
.

MessageId = 263
SymbolicName = SN_EV_IP_FILTER_BASE_KEY_INITIALIZATION_FAILED
Language=English
BuildIPFilterRulesFromRegistry() call failed with an error "%1".
.
MessageId = 264
SymbolicName = SN_EV_IP_FILTER_BASE_KEY_INITIALIZATION_FAILED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>BuildIPFilterRulesFromRegistry() call failed with an error "%1".</p>
.

MessageId = 265
SymbolicName = SN_EV_IP_FILTER_LEAF_ENUMERATION_FAILED
Language=English
Enumerating section "%1" failed with an error "%2".
.
MessageId = 266
SymbolicName = SN_EV_IP_FILTER_LEAF_ENUMERATION_FAILED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>Enumerating section "%1" failed with an error "%2".</p>
.

MessageId = 267
SymbolicName = SN_EV_IP_FILTER_UNKNOWN_GROUP_OBJECT_VALUE
Language=English
InsertIPGroupObject() ignoring unknown group object value of "%1".
.
MessageId = 268
SymbolicName = SN_EV_IP_FILTER_UNKNOWN_GROUP_OBJECT_VALUE_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>InsertIPGroupObject() ignoring unknown group object value of "%1".</p>
.

MessageId = 269
SymbolicName = SN_EV_IP_FILTER_OUTGOING_UDP_SEND_DATAGRAM_REQUEST_REJECTED
Language=English
Rule "%1" blocked (%2,%3).
Outbound UDP packet. 
Local address,service is (%4,%5).
Remote address,service is (%6,%7).
Process name is "%8".
.
MessageId = 270
SymbolicName = SN_EV_IP_FILTER_OUTGOING_UDP_SEND_DATAGRAM_REQUEST_REJECTED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Rule "%1" blocked (%2,%3).<br>
			Outbound UDP packet.<br>
			Local address,service is (%4,%5).<br>
			Remote address,service is (%6,%7).<br>
			Process name is "%8".<br>
		</td>
	</tr>
</table>
.

MessageId = 271
SymbolicName = SN_EV_IP_FILTER_INCOMING_UDP_RECEIVE_DATAGRAM_REQUEST_REJECTED
Language=English
Rule "%1" blocked (%2,%3).
Inbound UDP packet. 
Local address,service is (%4,%5).
Remote address,service is (%6,%7).
Process name is "%8".
.
MessageId = 272
SymbolicName = SN_EV_IP_FILTER_INCOMING_UDP_RECEIVE_DATAGRAM_REQUEST_REJECTED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Rule "%1" blocked (%2,%3).<br>
			Inbound UDP packet.<br>
			Local address,service is (%4,%5).<br>
			Remote address,service is (%6,%7).<br>
			Process name is "%8".<br>
		</td>
	</tr>
</table>
.

MessageId = 273
SymbolicName = SN_EV_IP_FILTER_UNKNOWN_FIREWALL_STATE_VALUE
Language=English
BuildIPFilterRulesFromRegistry() encounted unknown Internet Worm Protection state value of "%1".
.
MessageId = 274
SymbolicName = SN_EV_IP_FILTER_UNKNOWN_FIREWALL_STATE_VALUE_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<p>BuildIPFilterRulesFromRegistry() encounted unknown Internet Worm Protection state value of "%1".</p>
.

MessageId = 275
SymbolicName = SN_EV_IP_FILTER_OUTGOING_CONNECT_ALERT
Language=English
%1 "%2" communications.
Outbound TCP connection. 
Remote address,service is (%3,%4).
Process name is "%5".
.
MessageId = 276
SymbolicName = SN_EV_IP_FILTER_OUTGOING_CONNECT_ALERT_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			%1 "%2" communications.<br>
			Outbound TCP connection.<br>
			Remote address,service is (%3,%4).<br>
			Process name is "%5".<br>
		</td>
	</tr>
</table>
.

MessageId = 277
SymbolicName = SN_EV_IP_FILTER_INCOMING_CONNECT_ALERT
Language=English
%1 "%2" communications.
Inbound TCP connection. 
Local address,service is (%3,%4).
Remote address,service is (%5,%6).
Process name is "%7".
.
MessageId = 278
SymbolicName = SN_EV_IP_FILTER_INCOMING_CONNECT_ALERT_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			%1 "%2" communications.<br>
			Inbound TCP connection.<br>
			Local address,service is (%3,%4).<br>
			Remote address,service is (%5,%6).<br>
			Process name is "%7".<br>
		</td>
	</tr>
</table>
.

MessageId = 279
SymbolicName = SN_EV_IP_FILTER_OUTGOING_UDP_SEND_DATAGRAM_ALERT
Language=English
%1 "%2" communications.
Outbound UDP packet. 
Local address,service is (%3,%4).
Remote address,service is (%5,%6).
Process name is "%7".
.
MessageId = 280
SymbolicName = SN_EV_IP_FILTER_OUTGOING_UDP_SEND_DATAGRAM_ALERT_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			%1 "%2" communications.<br>
			Outbound UDP packet.<br>
			Local address,service is (%3,%4).<br>
			Remote address,service is (%5,%6).<br>
			Process name is "%7".<br>
		</td>
	</tr>
</table>
.

MessageId = 281
SymbolicName = SN_EV_IP_FILTER_INCOMING_UDP_RECEIVE_DATAGRAM_ALERT
Language=English
%1 "%2" communications.
Inbound UDP packet. 
Local address,service is (%3,%4).
Remote address,service is (%5,%6).
Process name is "%7".
.
MessageId = 282
SymbolicName = SN_EV_IP_FILTER_INCOMING_UDP_RECEIVE_DATAGRAM_ALERT_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			%1 "%2" communications.<br>
			Inbound UDP packet.<br>
			Local address,service is (%3,%4).<br>
			Remote address,service is (%5,%6).<br>
			Process name is "%7".<br>
		</td>
	</tr>
</table>
.

MessageId = 283
SymbolicName = SN_EV_IP_FILTER_REMOVE_INCOMING_ALERT_LIST_FAILED
Language=English
%1 "%2":
Unable to free alert list object memory for inbound TCP connection. 
Local address,service is (%3,%4).
Remote address,service is (%5,%6).
Process name is "%7".
.
MessageId = 284
SymbolicName = SN_EV_IP_FILTER_REMOVE_INCOMING_ALERT_LIST_FAILED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			%1 "%2":<br>
			Unable to free alert list object memory for inbound TCP connection.<br>
			Local address,service is (%3,%4).<br>
			Remote address,service is (%5,%6).<br>
			Process name is "%7".<br>
		</td>
	</tr>
</table>
.

MessageId = 285
SymbolicName = SN_EV_IP_FILTER_REMOVE_OUTGOING_ALERT_LIST_FAILED
Language=English
%1 "%2":
Unable to free alert list object memory for outbound TCP connection. 
Local address,service is (%3,%4).
Remote address,service is (%5,%6).
Process name is "%7".
.
MessageId = 286
SymbolicName = SN_EV_IP_FILTER_REMOVE_OUTGOING_ALERT_LIST_FAILED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			%1 "%2":<br>
			Unable to free alert list object memory for outbound TCP connection.<br>
			Local address,service is (%3,%4).<br>
			Remote address,service is (%5,%6).<br>
			Process name is "%7".<br>
		</td>
	</tr>
</table>
.

MessageId = 287
SymbolicName = SN_EV_IP_FILTER_INCOMING_CONNECT_REQUEST_ALLOWED
Language=English
Rule "%1" permitted (%2,%3).
Inbound TCP connection. 
Local address,service is (%4,%5).
Remote address,service is (%6,%7).
Process name is "%8".
.
MessageId = 288
SymbolicName = SN_EV_IP_FILTER_INCOMING_CONNECT_REQUEST_ALLOWED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Rule "%1" permitted (%2,%3).<br>
			Inbound TCP connection.<br>
			Local address,service is (%4,%5).<br>
			Remote address,service is (%6,%7).<br>
			Process name is "%8".<br>
		</td>
	</tr>
</table>
.

MessageId = 289
SymbolicName = SN_EV_IP_FILTER_OUTGOING_CONNECT_REQUEST_ALLOWED
Language=English
Rule "%1" permitted (%2,%3).
Outbound TCP connection. 
Remote address,service is (%4,%5).
Process name is "%6".
.
MessageId = 290
SymbolicName = SN_EV_IP_FILTER_OUTGOING_CONNECT_REQUEST_ALLOWED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Rule "%1" permitted (%2,%3).<br>
			Outbound TCP connection.<br>
			Remote address,service is (%4,%5).<br>
			Process name is "%6".<br>
		</td>
	</tr>
</table>
.

MessageId = 291
SymbolicName = SN_EV_IP_FILTER_POST_PROCESS_OUTGOING_CONNECT_REQUEST_ALLOWED
Language=English
Rule "%1" permitted (%2,%3).
Outbound TCP connection. 
Local address,service is (%4,%5).
Remote address,service is (%6,%7).
Process name is "%8".
.
MessageId = 292
SymbolicName = SN_EV_IP_FILTER_POST_PROCESS_OUTGOING_CONNECT_REQUEST_ALLOWED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Rule "%1" permitted (%2,%3).<br>
			Outbound TCP connection.<br>
			Local address,service is (%4,%5).<br>
			Remote address,service is (%6,%7).<br>
			Process name is "%8".<br>
		</td>
	</tr>
</table>
.

MessageId = 293
SymbolicName = SN_EV_IP_FILTER_OUTGOING_UDP_SEND_DATAGRAM_REQUEST_ALLOWED
Language=English
Rule "%1" permitted (%2,%3).
Outbound UDP packet. 
Local address,service is (%4,%5).
Remote address,service is (%6,%7).
Process name is "%8".
.
MessageId = 294
SymbolicName = SN_EV_IP_FILTER_OUTGOING_UDP_SEND_DATAGRAM_REQUEST_ALLOWED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Rule "%1" permitted (%2,%3).<br>
			Outbound UDP packet.<br>
			Local address,service is (%4,%5).<br>
			Remote address,service is (%6,%7).<br>
			Process name is "%8".<br>
		</td>
	</tr>
</table>
.

MessageId = 295
SymbolicName = SN_EV_IP_FILTER_INCOMING_UDP_RECEIVE_DATAGRAM_REQUEST_ALLOWED
Language=English
Rule "%1" permitted (%2,%3).
Inbound UDP packet. 
Local address,service is (%4,%5).
Remote address,service is (%6,%7).
Process name is "%8".
.
MessageId = 296
SymbolicName = SN_EV_IP_FILTER_INCOMING_UDP_RECEIVE_DATAGRAM_REQUEST_ALLOWED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Rule "%1" permitted (%2,%3).<br>
			Inbound UDP packet.<br>
			Local address,service is (%4,%5).<br>
			Remote address,service is (%6,%7).<br>
			Process name is "%8".<br>
		</td>
	</tr>
</table>
.

MessageId = 297
SymbolicName = SN_EV_IP_FILTER_OUTGOING_CONNECT_REQUEST_REJECTED2
Language=English
Rule "%1" blocked (%2,%3).
Outbound TCP connection.
Local address,service is (%4,%5).
Remote address,service is (%6,%7).
Process name is "%8".
.
MessageId = 298
SymbolicName = SN_EV_IP_FILTER_OUTGOING_CONNECT_REQUEST_REJECTED2_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Rule "%1" blocked (%2,%3).<br>
			Outbound TCP connection.<br>
			Local address,service is (%4,%5).<br>
			Remote address,service is (%6,%7).<br>
			Process name is "%8".<br>
		</td>
	</tr>
</table>
.

MessageId = 299
SymbolicName = SN_EV_IP_FILTER_OUTGOING_CONNECT_REQUEST_ALLOWED2
Language=English
Rule "%1" permitted (%2,%3).
Outbound TCP connection. 
Local address,service is (%4,%5).
Remote address,service is (%6,%7).
Process name is "%8".
.
MessageId = 300
SymbolicName = SN_EV_IP_FILTER_OUTGOING_CONNECT_REQUEST_ALLOWED2_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Rule "%1" permitted (%2,%3).<br>
			Outbound TCP connection.<br>
			Local address,service is (%4,%5).<br>
			Remote address,service is (%6,%7).<br>
			Process name is "%8".<br>
		</td>
	</tr>
</table>
.

MessageId = 301
SymbolicName = SN_EV_IP_FILTER_OUTGOING_CONNECT_REQUEST_IGNORED2
Language=English
Rule "%1" ignored (%2,%3).
Outbound TCP connection. 
Local address,service is (%4,%5).
Remote address,service is (%6,%7).
Process name is "%8".
.
MessageId = 302
SymbolicName = SN_EV_IP_FILTER_OUTGOING_CONNECT_REQUEST_IGNORED2_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Rule "%1" ignored (%2,%3).<br>
			Outbound TCP connection.<br>
			Local address,service is (%4,%5).<br>
			Remote address,service is (%6,%7).<br>
			Process name is "%8".<br>
		</td>
	</tr>
</table>
.

MessageId = 303
SymbolicName = SN_EV_IP_FILTER_POST_PROCESS_OUTGOING_CONNECT_REQUEST_IGNORED
Language=English
Rule "%1" ignored (%2,%3).
Outbound TCP connection. 
Local address,service is (%4,%5).
Remote address,service is (%6,%7).
Process name is "%8".
.
MessageId = 304
SymbolicName = SN_EV_IP_FILTER_POST_PROCESS_OUTGOING_CONNECT_REQUEST_IGNORED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Rule "%1" ignored (%2,%3).<br>
			Outbound TCP connection.<br>
			Local address,service is (%4,%5).<br>
			Remote address,service is (%6,%7).<br>
			Process name is "%8".<br>
		</td>
	</tr>
</table>
.

MessageId = 305
SymbolicName = SN_EV_IP_FILTER_INCOMING_CONNECT_REQUEST_IGNORED
Language=English
Rule "%1" ignored (%2,%3).
Inbound TCP connection. 
Local address,service is (%4,%5).
Remote address,service is (%6,%7).
Process name is "%8".
.
MessageId = 306
SymbolicName = SN_EV_IP_FILTER_INCOMING_CONNECT_REQUEST_IGNORED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Rule "%1" ignored (%2,%3).<br>
			Inbound TCP connection.<br>
			Local address,service is (%4,%5).<br>
			Remote address,service is (%6,%7).<br>
			Process name is "%8".<br>
		</td>
	</tr>
</table>
.

MessageId = 307
SymbolicName = SN_EV_IP_FILTER_INCOMING_UDP_RECEIVE_DATAGRAM_REQUEST_IGNORED
Language=English
Rule "%1" ignored (%2,%3).
Inbound UDP packet. 
Local address,service is (%4,%5).
Remote address,service is (%6,%7).
Process name is "%8".
.
MessageId = 308
SymbolicName = SN_EV_IP_FILTER_INCOMING_UDP_RECEIVE_DATAGRAM_REQUEST_IGNORED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Rule "%1" ignored (%2,%3).<br>
			Inbound UDP packet.<br>
			Local address,service is (%4,%5).<br>
			Remote address,service is (%6,%7).<br>
			Process name is "%8".<br>
		</td>
	</tr>
</table>
.

MessageId = 309
SymbolicName = SN_EV_IP_FILTER_OUTGOING_UDP_SEND_DATAGRAM_REQUEST_IGNORED
Language=English
Rule "%1" ignored (%2,%3).
Outbound UDP packet. 
Local address,service is (%4,%5).
Remote address,service is (%6,%7).
Process name is "%8".
.
MessageId = 310
SymbolicName = SN_EV_IP_FILTER_OUTGOING_UDP_SEND_DATAGRAM_REQUEST_IGNORED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Rule "%1" ignored (%2,%3).<br>
			Outbound UDP packet.<br>
			Local address,service is (%4,%5).<br>
			Remote address,service is (%6,%7).<br>
			Process name is "%8".<br>
		</td>
	</tr>
</table>
.

MessageId = 311
SymbolicName = SN_EV_IP_FILTER_INCOMING_ICMP_REJECTED
Language=English
Rule "%1" blocked (%2,%3).
Inbound ICMP request. 
Local address is (%4).
Remote address is (%5).
Message type is "%6".
Process name is "%7".
.
MessageId = 312
SymbolicName = SN_EV_IP_FILTER_INCOMING_ICMP_REJECTED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Rule "%1" blocked (%2,%3).<br>
			Inbound ICMP request.<br>
			Local address is (%4).<br>
			Remote address is (%5).<br>
			Message type is "%6".<br>
			Process name is "%7".<br>
		</td>
	</tr>
</table>
.

MessageId = 313
SymbolicName = SN_EV_IP_FILTER_INCOMING_ICMP_ALLOWED
Language=English
Rule "%1" permitted (%2,%3).
Inbound ICMP request. 
Local address is (%4).
Remote address is (%5).
Message type is "%6".
Process name is "%7".
.
MessageId = 314
SymbolicName = SN_EV_IP_FILTER_INCOMING_ICMP_ALLOWED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Rule "%1" permitted (%2,%3).<br>
			Inbound ICMP request.<br>
			Local address is (%4).<br>
			Remote address is (%5).<br>
			Message type is "%6".<br>
			Process name is "%7".<br>
		</td>
	</tr>
</table>
.

MessageId = 315
SymbolicName = SN_EV_IP_FILTER_INCOMING_ICMP_IGNORED
Language=English
Rule "%1" ignored (%2,%3).
Inbound ICMP request. 
Local address is (%4).
Remote address is (%5).
Message type is "%6".
Process name is "%7".
.
MessageId = 316
SymbolicName = SN_EV_IP_FILTER_INCOMING_ICMP_IGNORED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Rule "%1" ignored (%2,%3).<br>
			Inbound ICMP request.<br>
			Local address is (%4).<br>
			Remote address is (%5).<br>
			Message type is "%6".<br>
			Process name is "%7".<br>
		</td>
	</tr>
</table>
.

MessageId = 317
SymbolicName = SN_EV_IP_FILTER_OUTGOING_ICMP_REJECTED
Language=English
Rule "%1" blocked (%2,%3).
Outbound ICMP request.
Local address is (%4).
Remote address is (%5).
Message type is "%6".
Process name is "%7".
.
MessageId = 318
SymbolicName = SN_EV_IP_FILTER_OUTGOING_ICMP_REJECTED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Rule "%1" blocked (%2,%3).<br>
			Outbound ICMP request.<br>
			Local address is (%4).<br>
			Remote address is (%5).<br>
			Message type is "%6".<br>
			Process name is "%7".<br>
		</td>
	</tr>
</table>
.

MessageId = 319
SymbolicName = SN_EV_IP_FILTER_OUTGOING_ICMP_ALLOWED
Language=English
Rule "%1" permitted (%2,%3).
Outbound ICMP request. 
Local address is (%4).
Remote address is (%5).
Message type is "%6".
Process name is "%7".
.
MessageId = 320
SymbolicName = SN_EV_IP_FILTER_OUTGOING_ICMP_ALLOWED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Rule "%1" permitted (%2,%3).<br>
			Outbound ICMP request.<br>
			Local address is (%4).<br>
			Remote address is (%5).<br>
			Message type is "%6".<br>
			Process name is "%7".<br>
		</td>
	</tr>
</table>
.

MessageId = 321
SymbolicName = SN_EV_IP_FILTER_OUTGOING_ICMP_IGNORED
Language=English
Rule "%1" ignored (%2,%3).
Outbound ICMP request. 
Local address is (%4).
Remote address is (%5).
Message type is "%6".
Process name is "%7".
.
MessageId = 322
SymbolicName = SN_EV_IP_FILTER_OUTGOING_ICMP_IGNORED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Rule "%1" ignored (%2,%3)<.br>
			Outbound ICMP request.<br>
			Local address is (%4).<br>
			Remote address is (%5).<br>
			Message type is "%6".<br>
			Process name is "%7".<br>
		</td>
	</tr>
</table>
.

MessageId = 323
SymbolicName = SN_EV_IP_FILTER_INCOMING_ICMP_ALERT
Language=English
%1 "%2" communications.
Inbound ICMP request. 
Local address (%3).
Remote address is (%4).
Message type is "%5".
Process name is "%6".
.
MessageId = 324
SymbolicName = SN_EV_IP_FILTER_INCOMING_ICMP_ALERT_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			%1 "%2" communications.<br>
			Inbound ICMP request.<br>
			Local address (%3).<br>
			Remote address is (%4).<br>
			Message type is "%5".<br>
			Process name is "%6".<br>
		</td>
	</tr>
</table>
.

MessageId = 325
SymbolicName = SN_EV_IP_FILTER_OUTGOING_ICMP_ALERT
Language=English
%1 "%2" communications.
Outbound ICMP request. 
Local address (%3).
Remote address is (%4).
Message type is "%5".
Process name is "%6".
.
MessageId = 326
SymbolicName = SN_EV_IP_FILTER_OUTGOING_ICMP_ALERT_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			%1 "%2" communications.<br>
			Outbound ICMP request.<br>
			Local address (%3).<br>
			Remote address is (%4).<br>
			Message type is "%5".<br>
			Process name is "%6".<br>
		</td>
	</tr>
</table>
.

MessageId = 327
SymbolicName = SN_EV_IP_FILTER_OUTGOING_CONNECT_RESTRICTED
Language=English
Application restricted.
Process name is "%1".
Restricted category is "%2".
.
MessageId = 328
SymbolicName = SN_EV_IP_FILTER_OUTGOING_CONNECT_RESTRICTED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Application restricted.<br>
			Process name is "%1".<br>
			Restricted category is "%2".<br>
		</td>
	</tr>
</table>
.

MessageId = 329
SymbolicName = SN_EV_IP_FILTER_INCOMING_UNUSED_RESTRICTED
Language=English
Unused port blocking has blocked communications.
Inbound TCP connection. 
Remote address,local service is (%1,%2).
.
MessageId = 330
SymbolicName = SN_EV_IP_FILTER_INCOMING_UNUSED_RESTRICTED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<body OnLoad="IDSEventOnLoad()">
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Unused port blocking has blocked communications.<br>
			Inbound TCP connection.<br>
			Remote address,local service is (<span class="BTLink" id="BTLink" onclick="LaunchBackTrace('%1')" title ="Trace this connection">%1</span>,%2).<br>
		</td>
	</tr>
	<tr id="BTDescriptionRow">
		<td colspan=2 class="NisEvtEventValue">
			Click the address to trace the attacker.
		</td>
	</tr>
</table>
</body>
.
MessageId = 33097
SymbolicName = SN_EV_IP_FILTER_INCOMING_UNUSED_RESTRICTED_SHORT
Language=English
Blocked unused port %2 (%1).
.

MessageId = 331
SymbolicName = SN_EV_IP_FILTER_INCOMING_CONNECT_REQUEST_STEALTHED
Language=English
Rule "%1" stealthed (%2,%3).
Inbound TCP connection.
Local address,service is (%4,%5).
Remote address,service is (%6,%7).
Process name is "%8".
.
MessageId = 332
SymbolicName = SN_EV_IP_FILTER_INCOMING_CONNECT_REQUEST_STEALTHED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Rule "%1" stealthed (%2,%3).<br>
			Inbound TCP connection.<br>
			Local address,service is (%4,%5).<br>
			Remote address,service is (%6,%7).<br>
			Process name is "%8".<br>
		</td>
	</tr>
</table>
.
MessageId = 33099
SymbolicName = SN_EV_IP_FILTER_INCOMING_CONNECT_REQUEST_STEALTHED_SHORT
Language=English
Blocked stealth TCP port %3.
.

MessageId = 333
SymbolicName = SN_EV_IP_FILTER_INCOMING_UDP_RECEIVE_DATAGRAM_REQUEST_STEALTHED
Language=English
Rule "%1" stealthed (%2,%3).
Inbound UDP packet. 
Local address,service is (%4,%5).
Remote address,service is (%6,%7).
Process name is "%8".
.
MessageId = 334
SymbolicName = SN_EV_IP_FILTER_INCOMING_UDP_RECEIVE_DATAGRAM_REQUEST_STEALTHED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Rule "%1" stealthed (%2,%3).<br>
			Inbound UDP packet.<br>
			Local address,service is (%4,%5).<br>
			Remote address,service is (%6,%7).<br>
			Process name is "%8".<br>
		</td>
	</tr>
</table>
.
MessageId = 33101
SymbolicName = SN_EV_IP_FILTER_INCOMING_UDP_RECEIVE_DATAGRAM_REQUEST_STEALTHED_SHORT
Language=English
Blocked stealth UDP port %3.
.

MessageId = 335
SymbolicName = SN_EV_IP_FILTER_PORT_SCAN_DETECTED
Language=English
Port scan detected from address %1.
Blocked further access for %3 minutes after detecting at least %2 ports being probed.
.
MessageId = 336
SymbolicName = SN_EV_IP_FILTER_PORT_SCAN_DETECTED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<body OnLoad="IDSEventOnLoad()">
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Port scan detected from address <span class="BTLink" id="BTLink" onclick="LaunchBackTrace('%1')" title ="Trace this connection">%1</span>.<br>
			Blocked further access for %3 minutes after detecting at least %2 ports being probed.<br>
		</td>
	</tr>
	<tr id="BTDescriptionRow">
		<td colspan=2 class="NisEvtEventValue">
			Click the address to trace the attacker.
		</td>
	</tr>
</table>
</body>
.
MessageId = 33103
SymbolicName = SN_EV_IP_FILTER_PORT_SCAN_DETECTED_SHORT
Language=English
Port scan by %1, blocked for %3 minutes.
.

MessageId = 337
SymbolicName = SN_EV_IP_FILTER_IDS_BLOCK
Language=English
%2 was detected and blocked.
All comunication with %1 will be blocked for %3 minutes.
.
MessageId = 338
SymbolicName = SN_EV_IP_FILTER_IDS_BLOCK_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			%2 was detected and blocked.<br>
			All comunication with %1 will be blocked for %3 minutes.<br>
		</td>
	</tr>
</table>
.
MessageId = 33105
SymbolicName = SN_EV_IP_FILTER_IDS_BLOCK_SHORT
Language=English
%1 blocked for %3 minutes.
.

MessageId = 339
SymbolicName = SN_EV_HTTP_ACTIVEX_REMOVED
Language=English
Removed Entire OBJECT tag
From    %1
Because ActiveX blocking enabled
.
MessageId = 340
SymbolicName = SN_EV_HTTP_ACTIVEX_REMOVED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Removed:</td>
		<td class="NisEvtEventValue">Entire OBJECT tag</td>
	</tr>
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">From:</td>
		<td class="NisEvtEventValue">%1</td>
	</tr>
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Because:</td>
		<td class="NisEvtEventValue">ActiveX blocking enabled</td>
	</tr>
</table>
.
MessageId = 33107
SymbolicName = SN_EV_HTTP_ACTIVEX_REMOVED_SHORT
Language=English
Blocked ActiveX control from %1.
.

MessageId = 341
SymbolicName = SN_EV_HTTP_APPLET_REMOVED
Language=English
Removed Entire APPLET tag
From    %1
Because Applet blocking enabled
.
MessageId = 342
SymbolicName = SN_EV_HTTP_APPLET_REMOVED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Removed:</td>
		<td class="NisEvtEventValue">Entire APPLET tag</td>
	</tr>
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">From:</td>
		<td class="NisEvtEventValue">%1</td>
	</tr>
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Because:</td>
		<td class="NisEvtEventValue">Applet blocking enabled</td>
	</tr>
</table>
.
MessageId = 33109
SymbolicName = SN_EV_HTTP_APPLET_REMOVED_SHORT
Language=English
Blocked Java Applet from %1.
.

MessageId = 343
SymbolicName = SN_EV_HTTP_SCRIPT_REMOVED
Language=English
Removed Entire SCRIPT tag
From    %1
Because Script blocking enabled
.
MessageId = 344
SymbolicName = SN_EV_HTTP_SCRIPT_REMOVED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Removed:</td>
		<td class="NisEvtEventValue">Entire SCRIPT tag</td>
	</tr>
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">From:</td>
		<td class="NisEvtEventValue">%1</td>
	</tr>
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Because:</td>
		<td class="NisEvtEventValue">Script blocking enabled</td>
	</tr>
</table>
.
MessageId = 33111
SymbolicName = SN_EV_HTTP_SCRIPT_REMOVED_SHORT
Language=English
Blocked script from %1.
.

MessageId = 345
SymbolicName = SN_EV_HTTP_SCRIPTBASEDPOPUP_REMOVED
Language=English
Removed %1
From    %2
Because Script-based Popup
.
MessageId = 346
SymbolicName = SN_EV_HTTP_SCRIPTBASEDPOPUP_REMOVED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Removed:</td>
		<td class="NisEvtEventValue">%1</td>
	</tr>
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">From:</td>
		<td class="NisEvtEventValue">%2</td>
	</tr>
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Because:</td>
		<td class="NisEvtEventValue">Script-based Popup</td>
	</tr>
</table>
.
MessageId = 33113
SymbolicName = SN_EV_HTTP_SCRIPTBASEDPOPUP_REMOVED_SHORT
Language=English
Blocked popup window from %2.
.

MessageId = 347
SymbolicName = SN_EV_HTTP_USERAGENT_BLOCKED
Language=English
Blocked User-Agent: %1 sent for %2.
.
MessageId = 348
SymbolicName = SN_EV_HTTP_USERAGENT_BLOCKED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Blocked User-Agent: %1 sent for %2.
		</td>
	</tr>
</table>
.
MessageId = 33115
SymbolicName = SN_EV_HTTP_USERAGENT_BLOCKED_SHORT
Language=English
Blocked User-Agent: %2.
.

MessageId = 349
SymbolicName = SN_EV_HTTP_USERAGENT_ALLOWED
Language=English
Allowed User-Agent: %1 sent to %2.
.
MessageId = 350
SymbolicName = SN_EV_HTTP_USERAGENT_ALLOWED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Allowed User-Agent: %1 sent to %2.
		</td>
	</tr>
</table>
.
MessageId = 33117
SymbolicName = SN_EV_HTTP_USERAGENT_ALLOWED_SHORT
Language=English
Allowed User-Agent: %2.
.

MessageId = 351
SymbolicName = SN_EV_HTTP_REFERER_BLOCKED
Language=English
Blocked Referer: %1 sent for %2.
.
MessageId = 352
SymbolicName = SN_EV_HTTP_REFERER_BLOCKED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Blocked Referer: %1 sent for %2.
		</td>
	</tr>
</table>
.
MessageId = 33119
SymbolicName = SN_EV_HTTP_REFERER_BLOCKED_SHORT
Language=English
Blocked Referer from %2.
.

MessageId = 353
SymbolicName = SN_EV_HTTP_REFERER_ALLOWED
Language=English
Allowed Referer: %1 sent to %2.
.
MessageId = 354
SymbolicName = SN_EV_HTTP_REFERER_ALLOWED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Allowed Referer: %1 sent to %2.
		</td>
	</tr>
</table>
.
MessageId = 33121
SymbolicName = SN_EV_HTTP_REFERER_ALLOWED_SHORT
Language=English
Allowed Referer from %2.
.

MessageId = 355
SymbolicName = SN_EV_HTTP_FROM_BLOCKED
Language=English
Blocked From: %1 sent for %2.
.
MessageId = 356
SymbolicName = SN_EV_HTTP_FROM_BLOCKED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Blocked From: %1 sent for %2.
		</td>
	</tr>
</table>
.
MessageId = 33123
SymbolicName = SN_EV_HTTP_FROM_BLOCKED_SHORT
Language=English
Blocked "From": %2.
.

MessageId = 357
SymbolicName = SN_EV_HTTP_FROM_ALLOWED
Language=English
Allowed From: %1 sent to %2.
.
MessageId = 358
SymbolicName = SN_EV_HTTP_FROM_ALLOWED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Allowed From: %1 sent to %2.
		</td>
	</tr>
</table>
.
MessageId = 33125
SymbolicName = SN_EV_HTTP_FROM_ALLOWED_SHORT
Language=English
Allowed "From": %2.
.

MessageId = 359
SymbolicName = SN_EV_HTTP_COOKIE_BLOCKED
Language=English
Blocked Cookie: %1 sent for %2.
.
MessageId = 360
SymbolicName = SN_EV_HTTP_COOKIE_BLOCKED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Blocked Cookie: %1 sent for %2.
		</td>
	</tr>
</table>
.
MessageId = 33127
SymbolicName = SN_EV_HTTP_COOKIE_BLOCKED_SHORT
Language=English
Blocked Cookie from %2.
.

MessageId = 361
SymbolicName = SN_EV_HTTP_COOKIE_ALLOWED
Language=English
Allowed Cookie: %1 sent to %2.
.
MessageId = 362
SymbolicName = SN_EV_HTTP_COOKIE_ALLOWED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Allowed Cookie: %1 sent to %2.
		</td>
	</tr>
</table>
.
MessageId = 33129
SymbolicName = SN_EV_HTTP_COOKIE_ALLOWED_SHORT
Language=English
Allowed Cookie from %2.
.

MessageId = 363
SymbolicName = SN_EV_IP_FILTER_INCOMING_UDP_EXCEPTION
Language=English
Inbound UDP packet allowed.
Local address,service is (%1,%2).
Remote address,service is (%3,%4).
Process name is "%5".
.
MessageId = 364
SymbolicName = SN_EV_IP_FILTER_INCOMING_UDP_EXCEPTION_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Inbound UDP packet allowed.<br>
			Local address,service is (%1,%2).<br>
			Remote address,service is (%3,%4).<br>
			Process name is "%5".<br>
		</td>
	</tr>
</table>
.
MessageId = 33131
SymbolicName = SN_EV_IP_FILTER_INCOMING_UDP_EXCEPTION_SHORT
Language=English
Inbound UDP packet allowed from (%3,%4) for process "%5".
.

MessageId = 365
SymbolicName = SN_EV_IP_FILTER_PORT_SCAN_DETECTED_NOT_BLOCKED
Language=English
Port scan detected from address %1 At least %2 ports being probed.
.
MessageId = 366
SymbolicName = SN_EV_IP_FILTER_PORT_SCAN_DETECTED_NOT_BLOCKED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Port scan detected from address %1 At least %2 ports being probed.
		</td>
	</tr>
</table>
.
MessageId = 33133
SymbolicName = SN_EV_IP_FILTER_PORT_SCAN_DETECTED_NOT_BLOCKED_SHORT
Language=English
Port scan by %1.
.

MessageId = 367
SymbolicName = SN_EV_IP_FILTER_ALLOW_RULE_PRESENT_FOR_LISTEN
Language=English
Rule "%1" permitted communication.
Local address: %2(%3).
Process name is "%4".
.
MessageId = 368
SymbolicName = SN_EV_IP_FILTER_ALLOW_RULE_PRESENT_FOR_LISTEN_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Rule "%1" permitted communication.
			Local address: %2(%3).<br>
			Process name is "%4".<br>
		</td>
	</tr>
</table>
.

MessageId = 369
SymbolicName = SN_EV_IP_FILTER_REJECT_RULE_PRESENT_FOR_LISTEN
Language=English
Rule "%1" blocked communication.
Local address: %2(%3).
Process name is "%4".
.
MessageId = 370
SymbolicName = SN_EV_IP_FILTER_REJECT_RULE_PRESENT_FOR_LISTEN_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Rule "%1" blocked communication.
			Local address: %2(%3).<br>
			Process name is "%4".<br>
		</td>
	</tr>
</table>
.

MessageId = 371
SymbolicName = SN_EV_IP_FILTER_IGNORE_RULE_PRESENT_FOR_LISTEN
Language=English
Rule "%1" ignored communication.
Local address: %2(%3).
Process name is "%4".
.
MessageId = 372
SymbolicName = SN_EV_IP_FILTER_IGNORE_RULE_PRESENT_FOR_LISTEN_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Rule "%1" ignored communication.
			Local address: %2(%3).<br>
			Process name is "%4".<br>
		</td>
	</tr>
</table>
.

MessageId = 373
SymbolicName = SN_EV_IP_FILTER_DEFAULT_RULES_RESTORED
Language=English
Missing or damaged Internet Worm Protection rules. Default rules restored.
.
MessageId = 374
SymbolicName = SN_EV_IP_FILTER_DEFAULT_RULES_RESTORED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Missing or damaged Internet Worm Protection rules. Default rules restored.
		</td>
	</tr>
</table>
.
MessageId = 33141
SymbolicName = SN_EV_IP_FILTER_DEFAULT_RULES_RESTORED_SHORT
Language=English
Default rules restored.
.

MessageId = 375
SymbolicName = SN_EV_IP_FILTER_STORAGE_CORRUPTED
Language=English
Damaged configuration settings. Default setting restored.
.
MessageId = 376
SymbolicName = SN_EV_IP_FILTER_STORAGE_CORRUPTED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Damaged configuration settings. Default setting restored.
		</td>
	</tr>
</table>
.

;//---------------------------------------------------------------------------
;// N D I S    F I L T E R I N G    M E S S A G E S
;//---------------------------------------------------------------------------
MessageId = 377
SymbolicName = SN_EV_NDIS_FILTER_INTERNAL_ERROR
Language=English
While processing an incoming IP packet, a corrupted packet was encountered.
.
MessageId = 378
SymbolicName = SN_EV_NDIS_FILTER_INTERNAL_ERROR_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			While processing an incoming IP packet, a corrupted packet was encountered.
		</td>
	</tr>
</table>
.

MessageId = 379
SymbolicName = SN_EV_NDIS_FILTER_ACTIVE
Language=English
NDIS filtering is enabled.
.
MessageId = 380
SymbolicName = SN_EV_NDIS_FILTER_ACTIVE_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			NDIS filtering is enabled.
		</td>
	</tr>
</table>
.

;// The following message has been obsoleted in Spartacus
MessageId = 381
SymbolicName = SN_EV_NDIS_FILTER_BLOCK_IP_FRAGMENTS
Language=English
Inbound IP fragments are being blocked.
.
MessageId = 382
SymbolicName = SN_EV_NDIS_FILTER_BLOCK_IP_FRAGMENTS_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Inbound IP fragments are being blocked.
		</td>
	</tr>
</table>
.

MessageId = 383
SymbolicName = SN_EV_NDIS_FILTER_BLOCKED_IP_FRAGMENT
Language=English
Blocked inbound IP fragment.
Protocol "%1".
Remote address (%2).
Local address (%3).
.
MessageId = 384
SymbolicName = SN_EV_NDIS_FILTER_BLOCKED_IP_FRAGMENT_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Blocked inbound IP fragment.<br>
			Protocol "%1".<br>
			Remote address (%2).<br>
			Local address (%3).<br>
		</td>
	</tr>
</table>
.

;// The following message has been obsoleted in Spartacus, since the one below also
;// displays the IP addresses
MessageId = 385
SymbolicName = SN_EV_NDIS_FILTER_BLOCK_IGMP
Language=English
Inbound IGMP packets are being blocked.
.
MessageId = 386
SymbolicName = SN_EV_NDIS_FILTER_BLOCK_IGMP_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Inbound IGMP packets are being blocked.
		</td>
	</tr>
</table>
.

MessageId = 387
SymbolicName = SN_EV_NDIS_FILTER_BLOCKED_IGMP
Language=English
Blocked inbound IGMP packet.
Remote address: %1.
Local address: %2.
.
MessageId = 388
SymbolicName = SN_EV_NDIS_FILTER_BLOCKED_IGMP_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Blocked inbound IGMP packet.<br>
			Remote address: %1.<br>
			Local address: %2.<br>
		</td>
	</tr>
</table>
.

MessageId = 389
SymbolicName = SN_EV_NDIS_FILTER_INVALID_IP_VERSION
Language=English
Unsupported IP Version: %3.  Packet has been dropped.
Source IP address: %1.
Destination IP address: %2.
.
MessageId = 390
SymbolicName = SN_EV_NDIS_FILTER_INVALID_IP_VERSION_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Unsupported IP Version: %3.  Packet has been dropped.<br>
			Source IP address: %1.<br>
			Destination IP address: %2.<br>
		</td>
	</tr>
</table>
.

MessageId = 391
SymbolicName = SN_EV_NDIS_FILTER_INVALID_IPHEADER_LENGTH
Language=English
A packet from %1 with an invalid IP Header length of %2 bytes was detected and blocked.
.
MessageId = 392
SymbolicName = SN_EV_NDIS_FILTER_INVALID_IPHEADER_LENGTH_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			A packet from %1 with an invalid IP Header length of %2 bytes was detected and blocked. IP Header length must be at least 20 bytes.<br>
			Source IP address: %1.<br>
			Destination IP address: %3.<br>
			Protocol: %4.<br>
		</td>
	</tr>
</table>
.

MessageId = 393
SymbolicName = SN_EV_NDIS_FILTER_HLEN_GREATER_TOTALLEN
Language=English
A packet from %1 with an invalid IP Header length of %2 bytes was detected and blocked.
.
MessageId = 394
SymbolicName = SN_EV_NDIS_FILTER_HLEN_GREATER_TOTALLEN_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			A packet from %1 with an invalid IP Header length of %2 bytes was detected and blocked. IP Header length is greater than the IP Total length of %3 bytes.<br>
			Source IP address: %1.<br>
			Destination IP address: %4.<br>
			Protocol: %5.<br>
		</td>
	</tr>
</table>
.

MessageId = 395
SymbolicName = SN_EV_NDIS_FILTER_IP_FLAGS_INVALID
Language=English
Invalid IP Flags "%3".  Packet has been dropped.
Source IP address: %1.
Destination IP address: %2.
.
MessageId = 396
SymbolicName = SN_EV_NDIS_FILTER_IP_FLAGS_INVALID_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Invalid IP Flags "%3".  Packet has been dropped.<br>
			Source IP address: %1.<br>
			Destination IP address: %2.<br>
		</td>
	</tr>
</table>
.

MessageId = 397
SymbolicName = SN_EV_NDIS_FILTER_FRAGMENT_OFFSET_INVALID
Language=English
A packet from %1 with an invalid IP fragment offset of %2 was detected and blocked.
.
MessageId = 398
SymbolicName = SN_EV_NDIS_FILTER_FRAGMENT_OFFSET_INVALID_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			IP fragments with a IP fragment offset field equal to 8, are not permitted, since this would split the TCP header across multiple fragments.<br>
			Source IP address: %1.<br>
			Destination IP address: %3.<br>
			Protocol: %4.<br>
		</td>
	</tr>
</table>
.

MessageId = 399
SymbolicName = SN_EV_NDIS_FILTER_FIRST_FRAGMENT_TOO_SMALL
Language=English
A packet from %1 with an invalid IP payload size of %2 bytes was detected and blocked.
.
MessageId = 400
SymbolicName = SN_EV_NDIS_FILTER_FIRST_FRAGMENT_TOO_SMALL_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			IP Payload length: %2 bytes. This is less than the minimum size permitted for the first fragment.<br>
			Source IP address: %1.<br>
			Destination IP address: %3.<br>
			Protocol: %4.<br>
			Minimum IP Payload size permitted for TCP packets is 16 bytes and for UDP/ICMP/IGMP is 8 bytes.<br>
		</td>
	</tr>
</table>
.

MessageId = 401
SymbolicName = SN_EV_NDIS_FILTER_IP_CHECKSUM_INVALID
Language=English
IP Checksum Invalid. Expected= %3. Actual= %4. Packet has been dropped.
Source IP address: %1.
Destination IP address: %2.
.
MessageId = 402
SymbolicName = SN_EV_NDIS_FILTER_IP_CHECKSUM_INVALID_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			IP Checksum Invalid. Expected: %3. Actual: %4. Packet has been dropped.<br>
			Source IP address: %1.<br>
			Destination IP address: %2.<br>
		</td>
	</tr>
</table>
.

MessageId = 403
SymbolicName = SN_EV_NDIS_FILTER_SOURCE_IP_INVALID
Language=English
Invalid IP Address: %3.  Packet has been dropped.
Source IP address: %1.
Destination IP address: %2.
.
MessageId = 404
SymbolicName = SN_EV_NDIS_FILTER_SOURCE_IP_INVALID_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Invalid IP Address: %3.  Packet has been dropped.<br>
			Source IP address: %1.<br>
			Destination IP address: %2.<br>
		</td>
	</tr>
</table>
.

MessageId = 405
SymbolicName = SN_EV_NDIS_FILTER_DEST_IP_INVALID
Language=English
Invalid IP Address: %3.  Packet has been dropped.
Source IP address: %1.
Destination IP address: %2.
.
MessageId = 406
SymbolicName = SN_EV_NDIS_FILTER_DEST_IP_INVALID_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Invalid IP Address: %3.  Packet has been dropped.<br>
			Source IP address: %1.<br>
			Destination IP address: %2.<br>
		</td>
	</tr>
</table>
.

MessageId = 407
SymbolicName = SN_EV_NDIS_FILTER_IP_OPTIONS_INVALID
Language=English
Invalid IP Options.  Packet has been dropped.
Source IP address: %1.
Destination IP address: %2.
.
MessageId = 408
SymbolicName = SN_EV_NDIS_FILTER_IP_OPTIONS_INVALID_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Invalid IP Options.  Packet has been dropped.<br>
			Source IP address: %1.<br>
			Destination IP address: %2.<br>
		</td>
	</tr>
</table>
.

MessageId = 409
SymbolicName = SN_EV_NDIS_FILTER_TCP_SRC_PORT_INVALID
Language=English
TCP Source Port Invalid: %3.  Packet has been dropped.
Source IP address: %1.
Destination IP address: %2.
.
MessageId = 410
SymbolicName = SN_EV_NDIS_FILTER_TCP_SRC_PORT_INVALID_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			TCP Source Port Invalid: %3.  Packet has been dropped.<br>
			Source IP address: %1.<br>
			Destination IP address: %2.<br>
		</td>
	</tr>
</table>
.

MessageId = 411
SymbolicName = SN_EV_NDIS_FILTER_TCP_DEST_PORT_INVALID
Language=English
TCP Destination Port Invalid: %4.  Packet has been dropped.
Source IP address: %1.
Destination IP address: %2.
.
MessageId = 412
SymbolicName = SN_EV_NDIS_FILTER_TCP_DEST_PORT_INVALID_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			TCP Destination Port Invalid: %4.  Packet has been dropped.<br>
			Source IP address: %1.<br>
			Destination IP address: %2.<br>
		</td>
	</tr>
</table>
.

MessageId = 413
SymbolicName = SN_EV_NDIS_FILTER_TCP_HLEN_INVALID
Language=English
A packet with an invalid TCP Header length of %1 bytes from %2 was detected and blocked.
.
MessageId = 414
SymbolicName = SN_EV_NDIS_FILTER_TCP_HLEN_INVALID_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			TCP Header Length invalid: %1 bytes.  Minimum allowed length is 20 bytes. Must not be greater than IP payload length.<br>
			Source IP address: %2.<br>
			Destination IP address: %3.<br>
			IP Payload length: %4 bytes.<br>
			TCP Source Port: %5.<br>
			TCP Destination Port: %6.<br>
		</td>
	</tr>
</table>
.

MessageId = 415
SymbolicName = SN_EV_NDIS_FILTER_TCP_FLAGS_INVALID
Language=English
TCP Flags invalid: %5.  Packet has been dropped.
Source IP address: %1.
Destination IP address: %2.
TCP Source Port: %3.
TCP Destination Port: %4.
.
MessageId = 416
SymbolicName = SN_EV_NDIS_FILTER_TCP_FLAGS_INVALID_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			TCP Flags invalid: %5.  Packet has been dropped.<br>
			Source IP address: %1.<br>
			Destination IP address: %2.<br>
			TCP Source Port: %3.<br>
			TCP Destination Port: %4.<br>
		</td>
	</tr>
</table>
.

MessageId = 417
SymbolicName = SN_EV_NDIS_FILTER_TCP_OPTIONS_INVALID
Language=English
TCP Options invalid.  Packet has been dropped.
Source IP address: %1.
Destination IP address: %2.
TCP Source Port: %3.
TCP Destination Port: %4.
.
MessageId = 418
SymbolicName = SN_EV_NDIS_FILTER_TCP_OPTIONS_INVALID_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			TCP Options invalid.  Packet has been dropped.<br>
			Source IP address: %1.<br>
			Destination IP address: %2.<br>
			TCP Source Port: %3.<br>
			TCP Destination Port: %4.<br>
		</td>
	</tr>
</table>
.

MessageId = 419
SymbolicName = SN_EV_NDIS_FILTER_UDP_HEADER_INVALID
Language=English
An invalid UDP packet from %1 was detected and blocked. UDP header is missing or too small.
.
MessageId = 420
SymbolicName = SN_EV_NDIS_FILTER_UDP_HEADER_INVALID_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			IP Payload Length: %2 bytes. This should be at least 8 bytes in length to contain a valid UDP header.<br>
			IP Header Length: %3.<br>
			IP Total Length: %4.<br>
			Source IP address: %1.<br>
			Destination IP address: %5.<br>
			UDP Source Port: %6.<br>
			UDP Destination Port: %7.<br>
		</td>
	</tr>
</table>
.

MessageId = 421
SymbolicName = SN_EV_NDIS_FILTER_UDP_SRC_PORT_INVALID
Language=English
An invalid UDP packet from %1 was detected and blocked. UDP length field is invalid.
.
MessageId = 422
SymbolicName = SN_EV_NDIS_FILTER_UDP_SRC_PORT_INVALID_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			UDP Length field: %2. This should be at least 8 bytes in length.<br>
			Source IP address: %1.<br>
			Destination IP address: %3.<br>
			UDP Source Port: %4.<br>
			UDP Destination Port: %5.<br>
		</td>
	</tr>
</table>
.

MessageId = 423
SymbolicName = SN_EV_NDIS_FILTER_UDP_DEST_PORT_INVALID
Language=English
UDP Destination Port invalid.  Packet has been dropped.
Source IP address: %1.
Destination IP address: %2.
UDP Source Port: %3.
UDP Destination Port: %4.
.
MessageId = 424
SymbolicName = SN_EV_NDIS_FILTER_UDP_DEST_PORT_INVALID_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			UDP Destination Port invalid.  Packet has been dropped.<br>
			Source IP address: %1.<br>
			Destination IP address: %2.<br>
			UDP Source Port: %3.<br>
			UDP Destination Port: %4.<br>
		</td>
	</tr>
</table>
.

MessageId = 425
SymbolicName = SN_EV_NDIS_FILTER_UDP_LENGTH_INVALID
Language=English
An invalid UDP packet from %1 was detected and blocked. UDP Length field is invalid.
.
MessageId = 426
SymbolicName = SN_EV_NDIS_FILTER_UDP_LENGTH_INVALID_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			UDP Length field: %2 bytes. This value must be at least 8 bytes and must be consistent with the IP Header length and IP Total length fields.<br>
			IP Header length: %3.<br>
			IP Total length: %4.<br>
			IP Payload length: %5.<br>
			Source IP address: %1.<br>
			Destination IP address: %6.<br>
			UDP Source Port: %7.<br>
			UDP Destination Port: %8.<br>
		</td>
	</tr>
</table>
.

MessageId = 427
SymbolicName = SN_EV_NDIS_FILTER_ICMP_LENGTH_INVALID
Language=English
An invalid ICMP packet from %1 was detected and blocked. ICMP header missing or too small.
.
MessageId = 428
SymbolicName = SN_EV_NDIS_FILTER_ICMP_LENGTH_INVALID_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			IP Payload Length is %2 bytes, which is less than the minimum valid ICMP header size of 8 bytes.<br>
			IP Header length: %3.<br>
			IP Total length: %4.<br>
			Source IP address: %1.<br>
			Destination IP address: %5.<br>
			ICMP Type: %6.<br>
			ICMP Code: %7.<br>
		</td>
	</tr>
</table>
.

MessageId = 429
SymbolicName = SN_EV_NDIS_FILTER_ICMP_CODE_INVALID
Language=English
ICMP Code invalid: %4.  Packet has been dropped.
Source IP address: %1.
Destination IP address: %2.
ICMP Type: %3.
.
MessageId = 430
SymbolicName = SN_EV_NDIS_FILTER_ICMP_CODE_INVALID_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			ICMP Code invalid: %4.  Packet has been dropped.<br>
			Source IP address: %1.<br>
			Destination IP address: %2.<br>
			ICMP Type: %3.<br>
		</td>
	</tr>
</table>
.

MessageId = 431
SymbolicName = SN_EV_NDIS_FILTER_ICMP_TYPE_INVALID
Language=English
ICMP Type invalid: %3.  Packet has been dropped.
Source IP address: %1.
Destination IP address: %2.
ICMP Code: %4.
.
MessageId = 432
SymbolicName = SN_EV_NDIS_FILTER_ICMP_TYPE_INVALID_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			ICMP Type invalid: %3.  Packet has been dropped.<br>
			Source IP address: %1.<br>
			Destination IP address: %2.<br>
			ICMP Code: %4.<br>
		</td>
	</tr>
</table>
.

MessageId = 433
SymbolicName = SN_EV_NDIS_FILTER_LAND
Language=English
A packet with Source IP address equal to the Destination IP address from %1 was detected and blocked. Possible Land attack.
.
MessageId = 434
SymbolicName = SN_EV_NDIS_FILTER_LAND_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Source IP address is equal to Destination IP address. Possible "Land" attack.  Packet has been dropped.<br>
			Source IP address: %1.<br>
			Destination IP address: %2.<br>
			TCP Source Port: %3.<br>
			TCP Destination Port: %4.<br>
			TCP Flags: %5.<br>
		</td>
	</tr>
</table>
.

MessageId = 435
SymbolicName = SN_EV_NDIS_FILTER_LATIERRA
Language=English
A packet with Source IP address equal to the Destination IP address from %1 was detected and blocked. Possible Land attack.
.
MessageId = 436
SymbolicName = SN_EV_NDIS_FILTER_LATIERRA_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Source IP address is equal to Destination IP address. Possible "La Tierra" attack.  Packet has been dropped.<br>
			Source IP address: %1.<br>
			Destination IP address: %2.<br>
			TCP Source Port: %3.<br>
			TCP Destination Port: %4.<br>
			TCP Flags: %5.<br>
		</td>
	</tr>
</table>
.

MessageId = 437
SymbolicName = SN_EV_NDIS_FILTER_SRCDEST_IP_INVALID
Language=English
Source IP address is equal to Destination IP address. Packet has been dropped.
Source IP address: %1.
Destination IP address: %2.
.
MessageId = 438
SymbolicName = SN_EV_NDIS_FILTER_SRCDEST_IP_INVALID_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Source IP address is equal to Destination IP address. Packet has been dropped.<br>
			Source IP address: %1.<br>
			Destination IP address: %2.<br>
		</td>
	</tr>
</table>
.

MessageId = 439
SymbolicName = SN_EV_NDIS_FILTER_IP_TOO_LARGE
Language=English
An invalid IP packet from %1 was detected and blocked. Packet size exceeds the 64 KB limit on IP packets.
.
MessageId = 440
SymbolicName = SN_EV_NDIS_FILTER_IP_TOO_LARGE_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			IP Fragment Offset: %2. IP Payload length: %3. This IP fragment exceeds the 64 KB limit on IP packets.<br>
			Source IP address: %1.<br>
			Destination IP address: %4.<br>
			Protocol: %5.<br>
		</td>
	</tr>
</table>
.

MessageId = 441
SymbolicName = SN_EV_NDIS_FILTER_JOLT
Language=English
A Jolt attack packet from %1 was detected and blocked. Packet size exceeds the 64 KB limit on IP packets.
.
MessageId = 442
SymbolicName = SN_EV_NDIS_FILTER_JOLT_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			IP Fragment Offset: %2. IP Payload length: %3. This IP fragment exceeds the 64 KB limit on IP packets.<br>
			This packet is characteristic of the Jolt attack.<br>
			Source IP address: %1.<br>
			Destination IP address: %4.<br>
			Protocol: %5.<br>
		</td>
	</tr>
</table>
.

MessageId = 443
SymbolicName = SN_EV_NDIS_FILTER_JOLT2
Language=English
A Jolt2 attack packet from %1 was detected and blocked. Packet size exceeds the 64 KB limit on IP packets.
.
MessageId = 444
SymbolicName = SN_EV_NDIS_FILTER_JOLT2_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			IP Fragment Offset: %2. IP Payload length: %3. This IP fragment exceeds the 64 KB limit on IP packets.<br>
			This packet is characteristic of the Jolt2 attack.<br>
			Source IP address: %1.<br>
			Destination IP address: %4.<br>
			Protocol: %5.<br>
		</td>
	</tr>
</table>
.

MessageId = 445
SymbolicName = SN_EV_NDIS_FILTER_PING_OF_DEATH
Language=English
A Ping-of-death attack packet from %1 was detected and blocked. Packet size exceeds the 64 KB limit on IP packets.
.
MessageId = 446
SymbolicName = SN_EV_NDIS_FILTER_PING_OF_DEATH_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			IP Fragment Offset: %2. IP Payload length: %3. This IP fragment exceeds the 64 KB limit on IP packets.<br>
			This packet is characteristic of the Ping-of-death attack.<br>
			Source IP address: %1.<br>
			Destination IP address: %4.<br>
			Protocol: %5.<br>
		</td>
	</tr>
</table>
.

MessageId = 447
SymbolicName = SN_EV_NDIS_FILTER_TCP_FLAGS_INVALID_NMAP_NULL
Language=English
TCP Header Flags invalid: %5. Nmap Null scan. Packet has been dropped.
Source IP address: %1.
Destination IP address: %2.
TCP Source Port: %3.
TCP Destination Port: %4.
.
MessageId = 448
SymbolicName = SN_EV_NDIS_FILTER_TCP_FLAGS_INVALID_NMAP_NULL_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			TCP Header Flags invalid: %5. Nmap Null scan. Packet has been dropped.<br>
			Source IP address: %1.<br>
			Destination IP address: %2.<br>
			TCP Source Port: %3.<br>
			TCP Destination Port: %4.<br>
		</td>
	</tr>
</table>
.

MessageId = 449
SymbolicName = SN_EV_NDIS_FILTER_TCP_FLAGS_INVALID_NMAP_XMAS
Language=English
TCP Header Flags invalid: %5.  Nmap Xmas scan. Packet has been dropped.
Source IP address: %1.
Destination IP address: %2.
TCP Source Port: %3.
TCP Destination Port: %4.
.
MessageId = 450
SymbolicName = SN_EV_NDIS_FILTER_TCP_FLAGS_INVALID_NMAP_XMAS_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			TCP Header Flags invalid: %5.  Nmap Xmas scan. Packet has been dropped.<br>
			Source IP address: %1.<br>
			Destination IP address: %2.<br>
			TCP Source Port: %3.<br>
			TCP Destination Port: %4.<br>
		</td>
	</tr>
</table>
.

MessageId = 451
SymbolicName = SN_EV_NDIS_FILTER_INVALID_ARP_HWSIZE
Language=English
Invalid Hardware Size in ARP header: %1. Packet has been dropped.
.
MessageId = 452
SymbolicName = SN_EV_NDIS_FILTER_INVALID_ARP_HWSIZE_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Invalid Hardware Size in ARP header: %1. Packet has been dropped.
		</td>
	</tr>
</table>
.

MessageId = 453
SymbolicName = SN_EV_NDIS_FILTER_INVALID_ARP_PROTSIZE
Language=English
Invalid Protocol Size in ARP header: %1. Packet has been dropped.
.
MessageId = 454
SymbolicName = SN_EV_NDIS_FILTER_INVALID_ARP_PROTSIZE_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Invalid Protocol Size in ARP header: %1. Packet has been dropped.
		</td>
	</tr>
</table>
.

MessageId = 455
SymbolicName = SN_EV_NDIS_FILTER_BLOCKED_TCP_ON_CLOSED_PORT
Language=English
Unexpected TCP Packet. Packet has been dropped.
Source IP address: %1.
Destination IP address: %2.
TCP Source Port: %3.
TCP Destination Port: %4.
.
MessageId = 456
SymbolicName = SN_EV_NDIS_FILTER_BLOCKED_TCP_ON_CLOSED_PORT_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Unexpected TCP Packet. Packet has been dropped.<br>
			Source IP address: %1.<br>
			Destination IP address: %2.<br>
			TCP Source Port: %3.<br>
			TCP Destination Port: %4.<br>
		</td>
	</tr>
</table>
.

MessageId = 457
SymbolicName = SN_EV_NDIS_FILTER_BLOCKED_SYN_ON_NONLISTENING_PORT
Language=English
TCP Syn Packet on non-listening port. Packet has been dropped.
Source IP address: %1.
Destination IP address: %2.
TCP Source Port: %3.
TCP Destination Port: %4.
Flags: %5.
.
MessageId = 458
SymbolicName = SN_EV_NDIS_FILTER_BLOCKED_SYN_ON_NONLISTENING_PORT_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			TCP Syn Packet on non-listening port. Packet has been dropped.<br>
			Source IP address: %1.<br>
			Destination IP address: %2.<br>
			TCP Source Port: %3.<br>
			TCP Destination Port: %4.<br>
			TCP Message Flags: %5.<br>
		</td>
	</tr>
</table>
.

MessageId = 459
SymbolicName = SN_EV_NDIS_FILTER_BLOCKED_PACKET_ON_INVALID_CONNECTION
Language=English
TCP non-syn/non-ack packet on invalid connection. Packet has been dropped.
Source IP address: %1.
Destination IP address: %2.
TCP Source Port: %3.
TCP Destination Port: %4.
Flags: %5.
.
MessageId = 460
SymbolicName = SN_EV_NDIS_FILTER_BLOCKED_PACKET_ON_INVALID_CONNECTION_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			TCP non-syn/non-ack packet on invalid connection. Packet has been dropped.<br>
			Source IP address: %1.<br>
			Destination IP address: %2.<br>
			TCP Source Port: %3.<br>
			TCP Destination Port: %4.<br>
			TCP Message Flags: %5.<br>
		</td>
	</tr>
</table>
.

MessageId = 461
SymbolicName = SN_EV_NDIS_FILTER_TOTALLEN_GREATER_PKTSIZE
Language=English
A packet from %1 with an invalid IP Total length of %2 bytes was detected and blocked.
.
MessageId = 462
SymbolicName = SN_EV_NDIS_FILTER_TOTALLEN_GREATER_PKTSIZE_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			A packet from %1 with an invalid IP Total length of %2 bytes was detected and blocked. IP Total length is greater than the packet size of %3 bytes.<br>
			Source IP address: %1.<br>
			Destination IP address: %4.<br>
			Protocol: %5.<br>
		</td>
	</tr>
</table>
.

MessageId = 463
SymbolicName = SN_EV_NDIS_NETWORK_DISCONNECTED
Language=English
At the user's request, the network has been disconnected from this machine.
.
MessageId = 464
SymbolicName = SN_EV_NDIS_NETWORK_DISCONNECTED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			At the user's request, the network has been disconnected from this machine.
		</td>
	</tr>
</table>
.
MessageId = 465
SymbolicName = SN_EV_NDIS_NETWORK_RECONNECTED
Language=English
At the user's request, the network has been reconnected to this machine.
.
MessageId = 466
SymbolicName = SN_EV_NDIS_NETWORK_RECONNECTED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			At the user's request, the network has been reconnected to this machine.
		</td>
	</tr>
</table>
.
MessageId = 743
SymbolicName = SN_EV_NDIS_UNCMNPROTOCOL_NOW_ALLOWED
Language=English
Uncommon Protocol %1 is now allowed.
.
MessageId = 744
SymbolicName = SN_EV_NDIS_UNCMNPROTOCOL_NOW_ALLOWED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Uncommon Protocol %1 is now allowed.
		</td>
	</tr>
</table>
.
MessageId = 745
SymbolicName = SN_EV_NDIS_UNCMNPROTOCOL_NOW_BLOCKED
Language=English
Uncommon Protocol %1 is now blocked.
.
MessageId = 746
SymbolicName = SN_EV_NDIS_UNCMNPROTOCOL_NOW_BLOCKED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Uncommon Protocol %1 is now blocked.
		</td>
	</tr>
</table>
.
MessageId = 747
SymbolicName = SN_EV_NDIS_UNCMNPROTOCOL_ALLOW_ALL
Language=English
All Uncommon Protocols are now allowed.
.
MessageId = 748
SymbolicName = SN_EV_NDIS_UNCMNPROTOCOL_ALLOW_ALL_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			All Uncommon Protocols are now allowed.
		</td>
	</tr>
</table>
.
MessageId = 749
SymbolicName = SN_EV_NDIS_UNCMNPROTOCOL_BLOCK_ALL
Language=English
All Uncommon Protocols are now blocked.
.
MessageId = 750
SymbolicName = SN_EV_NDIS_UNCMNPROTOCOL_BLOCK_ALL_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			All Uncommon Protocols are now blocked.
		</td>
	</tr>
</table>
.
MessageId = 751
SymbolicName = SN_EV_NDIS_UNCMNPROTOCOL_ALLOWED
Language=English
An Uncommon Protocol packet %1 was allowed.
.
MessageId = 752
SymbolicName = SN_EV_NDIS_UNCMNPROTOCOL_ALLOWED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			An Uncommon Protocol packet %1 was allowed.
		</td>
	</tr>
</table>
.
MessageId = 753
SymbolicName = SN_EV_NDIS_UNCMNPROTOCOL_BLOCKED
Language=English
An Uncommon Protocol packet %1 was blocked.
.
MessageId = 754
SymbolicName = SN_EV_NDIS_UNCMNPROTOCOL_BLOCKED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			An Uncommon Protocol packet %1 was blocked.
		</td>
	</tr>
</table>
.
;//---------------------------------------------------------------------------
;//    I A M S E R V		M e s s a g e s
;//---------------------------------------------------------------------------
;//
MessageId = 467
SymbolicName = SN_EV_IAMSERV_STARTED_AS_SERVICE
Language=English
NISServ started as Windows Service.
.
MessageId = 468
SymbolicName = SN_EV_IAMSERV_STARTED_AS_SERVICE_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			NISServ started as Windows Service.
		</td>
	</tr>
</table>
.

MessageId = 469
SymbolicName = SN_EV_IAMSERV_STRING
Language=English
%1.
.
MessageId = 470
SymbolicName = SN_EV_IAMSERV_STRING_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">%1.</td>
	</tr>
</table>
.

MessageId = 471
SymbolicName = SN_EV_IAMSERV_REMOTE_CONFIG_OBTAINED
Language=English
Successfully obtained remote configuration update.
.
MessageId = 472
SymbolicName = SN_EV_IAMSERV_REMOTE_CONFIG_OBTAINED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Successfully obtained remote configuration update.
		</td>
	</tr>
</table>
.

MessageId = 473
SymbolicName = SN_EV_IAMSERV_REMOTE_CONFIG_CURRENT
Language=English
Local configuration is identical to remote configuration.
.
MessageId = 474
SymbolicName = SN_EV_IAMSERV_REMOTE_CONFIG_CURRENT_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Local configuration is identical to remote configuration.
		</td>
	</tr>
</table>
.

MessageId = 475
SymbolicName = SN_EV_IAMSERV_REMOTE_CONFIG_FAILED
Language=English
Obtaining remote configuration update failed.
Internal error is "%1" failure.
.
MessageId = 476
SymbolicName = SN_EV_IAMSERV_REMOTE_CONFIG_FAILED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Obtaining remote configuration update failed.<br>
			Internal error is "%1" failure.<br>
		</td>
	</tr>
</table>
.

MessageId = 477
SymbolicName = SN_EV_IAMSERV_REMOTE_CONFIG_POPULATED
Language=English
Successfully populated remote configuration update.
.
MessageId = 478
SymbolicName = SN_EV_IAMSERV_REMOTE_CONFIG_POPULATED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Successfully populated remote configuration update.
		</td>
	</tr>
</table>
.

MessageId = 479
SymbolicName = SN_EV_IAMSERV_FILTER_FIREWALL_REFRESH_FAILED
Language=English
Internet Worm Protection refresh ioctl call failed.
.
MessageId = 480
SymbolicName = SN_EV_IAMSERV_FILTER_FIREWALL_REFRESH_FAILED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Internet Worm Protection refresh ioctl call failed.
		</td>
	</tr>
</table>
.

MessageId = 481
SymbolicName = SN_EV_IAMSERV_FILTER_HTTP_REFRESH_FAILED
Language=English
Http refresh ioctl call failed.
.
MessageId = 482
SymbolicName = SN_EV_IAMSERV_FILTER_HTTP_REFRESH_FAILED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Http refresh ioctl call failed.
		</td>
	</tr>
</table>
.

MessageId = 483
SymbolicName = SN_EV_IAMSERV_FILTER_GET_REMOTE_CONFIGURATION_FAILED
Language=English
Get remote configuration update ioctl called failed.
.
MessageId = 484
SymbolicName = SN_EV_IAMSERV_FILTER_GET_REMOTE_CONFIGURATION_FAILED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Get remote configuration update ioctl called failed.
		</td>
	</tr>
</table>
.

MessageId = 485
SymbolicName = SN_EV_IAMSERV_CONFIG_FILE_OPEN_FAILED
Language=English
Local copy of configuration file "%1" could not be opened for reading.
.
MessageId = 486
SymbolicName = SN_EV_IAMSERV_CONFIG_FILE_OPEN_FAILED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Local copy of configuration file "%1" could not be opened for reading.
		</td>
	</tr>
</table>
.

MessageId = 487
SymbolicName = SN_EV_IAMSERV_CONFIG_FILE_MISSING_REGEDIT4
Language=English
First line of configuration file must be REGEDIT4.
.
MessageId = 488
SymbolicName = SN_EV_IAMSERV_CONFIG_FILE_MISSING_REGEDIT4_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			First line of configuration file must be REGEDIT4.
		</td>
	</tr>
</table>
.

MessageId = 489
SymbolicName = SN_EV_IAMSERV_CONFIG_FILE_EXTENDED_LINE_MISSING
Language=English
The line "%1" ends in a backslash, meaning that it is to be continued 
on the next line. The next line could not be read.
.
MessageId = 490
SymbolicName = SN_EV_IAMSERV_CONFIG_FILE_EXTENDED_LINE_MISSING_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			The line "%1" ends in a backslash, meaning that it is to be continued on the next line. The next line could not be read.
		</td>
	</tr>
</table>
.

MessageId = 491
SymbolicName = SN_EV_IAMSERV_CONFIG_FILE_CLOSE_FAILED
Language=English
Local copy of configuration file "%1" could not be closed correctly.
.
MessageId = 492
SymbolicName = SN_EV_IAMSERV_CONFIG_FILE_CLOSE_FAILED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Local copy of configuration file "%1" could not be closed correctly.
		</td>
	</tr>
</table>
.

MessageId = 493
SymbolicName = SN_EV_IAMSERV_CONFIG_FILE_MISSING_SQUARE_BRACKET
Language=English
The line "%1" appears to be a subkey, but does not have a closing ].
.
MessageId = 494
SymbolicName = SN_EV_IAMSERV_CONFIG_FILE_MISSING_SQUARE_BRACKET_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			The line "%1" appears to be a subkey, but does not have a closing ].
		</td>
	</tr>
</table>
.

MessageId = 495
SymbolicName = SN_EV_IAMSERV_CONFIG_FILE_MISSING_BACKSLASH
Language=English
The subkey "%1" is not fully qualified with backslashes separating each key.
.
MessageId = 496
SymbolicName = SN_EV_IAMSERV_CONFIG_FILE_MISSING_BACKSLASH_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			The subkey "%1" is not fully qualified with backslashes separating each key.
		</td>
	</tr>
</table>
.

MessageId = 497
SymbolicName = SN_EV_IAMSERV_CONFIG_FILE_INVALID_BASE_KEY
Language=English
The subkey "%1" is not a valid subkey for configuration information.
.
MessageId = 498
SymbolicName = SN_EV_IAMSERV_CONFIG_FILE_INVALID_BASE_KEY_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			The subkey "%1" is not a valid subkey for configuration information.
		</td>
	</tr>
</table>
.

MessageId = 499
SymbolicName = SN_EV_IAMSERV_CONFIG_FILE_INVALID_ROOT_KEY
Language=English
The root key "%1" is not valid for configuration information.
Only HKEY_LOCAL_USER and HKEY_LOCAL_MACHINE are valid.
.
MessageId = 500
SymbolicName = SN_EV_IAMSERV_CONFIG_FILE_INVALID_ROOT_KEY_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			The root key "%1" is not valid for configuration information. Only HKEY_LOCAL_USER and HKEY_LOCAL_MACHINE are valid.
		</td>
	</tr>
</table>
.

MessageId = 501
SymbolicName = SN_EV_IAMSERV_CONFIG_FILE_COULD_NOT_CREATE_SUBKEY
Language=English
The registry could not create the subkey "%1".
.
MessageId = 502
SymbolicName = SN_EV_IAMSERV_CONFIG_FILE_COULD_NOT_CREATE_SUBKEY_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			The registry could not create the subkey "%1".
		</td>
	</tr>
</table>
.

MessageId = 503
SymbolicName = SN_EV_IAMSERV_CONFIG_FILE_INVALID_LINE
Language=English
The line "%1" is not a valid subkey, name-value pair, comment, or blank line.
.
MessageId = 504
SymbolicName = SN_EV_IAMSERV_CONFIG_FILE_INVALID_LINE_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			The line "%1" is not a valid subkey, name-value pair, comment, or blank line.
		</td>
	</tr>
</table>
.

MessageId = 505
SymbolicName = SN_EV_IAMSERV_CONFIG_FILE_SUBKEY_NOT_SPECIFIED
Language=English
The line "%1" appears to be a name-value pair, but no subkey has been specified.
.
MessageId = 506
SymbolicName = SN_EV_IAMSERV_CONFIG_FILE_SUBKEY_NOT_SPECIFIED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			The line "%1" appears to be a name-value pair, but no subkey has been specified.
		</td>
	</tr>
</table>
.

MessageId = 507
SymbolicName = SN_EV_IAMSERV_CONFIG_FILE_EQUALS_MISSING
Language=English
The line "%1" appears to be a name-value pair, but there is no equal sign.
.
MessageId = 508
SymbolicName = SN_EV_IAMSERV_CONFIG_FILE_EQUALS_MISSING_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			The line "%1" appears to be a name-value pair, but there is no equal sign.
		</td>
	</tr>
</table>
.

MessageId = 509
SymbolicName = SN_EV_IAMSERV_CONFIG_FILE_COULD_NOT_CREATE_DWORD
Language=English
The dword value specified by the line "%1" could not be created.
.
MessageId = 510
SymbolicName = SN_EV_IAMSERV_CONFIG_FILE_COULD_NOT_CREATE_DWORD_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			The dword value specified by the line "%1" could not be created.
		</td>
	</tr>
</table>
.

MessageId = 511
SymbolicName = SN_EV_IAMSERV_CONFIG_FILE_INVALID_BYTE_VALUE
Language=English
The line "%1" contains an invalid byte value. All hex (binary), hex(2) (expand_sz),
and hex(7) (multi_sz) values must be specified as a series of comma delimited bytes.
Each byte must be represented as a 2-digit hexidecimal value.
.
MessageId = 512
SymbolicName = SN_EV_IAMSERV_CONFIG_FILE_INVALID_BYTE_VALUE_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			The line "%1" contains an invalid byte value. All hex (binary), hex(2) (expand_sz),
			and hex(7) (multi_sz) values must be specified as a series of comma delimited bytes.
			Each byte must be represented as a 2-digit hexidecimal value.
		</td>
	</tr>
</table>
.

MessageId = 513
SymbolicName = SN_EV_IAMSERV_CONFIG_FILE_COULD_NOT_CREATE_EXPAND_SZ
Language=English
The expand_sz value specified by the line "%1" could not be created.
.
MessageId = 514
SymbolicName = SN_EV_IAMSERV_CONFIG_FILE_COULD_NOT_CREATE_EXPAND_SZ_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			The expand_sz value specified by the line "%1" could not be created.
		</td>
	</tr>
</table>
.

MessageId = 515
SymbolicName = SN_EV_IAMSERV_CONFIG_FILE_COULD_NOT_CREATE_MULTI_SZ
Language=English
The multi_sz value specified by the line "%1" could not be created.
.
MessageId = 516
SymbolicName = SN_EV_IAMSERV_CONFIG_FILE_COULD_NOT_CREATE_MULTI_SZ_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			The multi_sz value specified by the line "%1" could not be created.
		</td>
	</tr>
</table>
.

MessageId = 517
SymbolicName = SN_EV_IAMSERV_CONFIG_FILE_COULD_NOT_CREATE_BINARY
Language=English
The binary value specified by the line "%1" could not be created.
.
MessageId = 518
SymbolicName = SN_EV_IAMSERV_CONFIG_FILE_COULD_NOT_CREATE_BINARY_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			The binary value specified by the line "%1" could not be created.
		</td>
	</tr>
</table>
.

MessageId = 519
SymbolicName = SN_EV_IAMSERV_CONFIG_FILE_UNKNOWN_VALUE_TYPE
Language=English
The line "%1" appears to be a name-value pair of an unknown value type. Valid
types are dword, hex (binary), hex(2) (expand_sz), hex(7) (multi_sz) and quoted 
strings.
.
MessageId = 520
SymbolicName = SN_EV_IAMSERV_CONFIG_FILE_UNKNOWN_VALUE_TYPE_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			The line "%1" appears to be a name-value pair of an unknown value type. Valid
			types are dword, hex (binary), hex(2) (expand_sz), hex(7) (multi_sz) and quoted 
			strings.
		</td>
	</tr>
</table>
.

MessageId = 521
SymbolicName = SN_EV_IAMSERV_CONFIG_FILE_COULD_NOT_CREATE_STRING
Language=English
The string value specified by the line "%1" could not be created.
.
MessageId = 522
SymbolicName = SN_EV_IAMSERV_CONFIG_FILE_COULD_NOT_CREATE_STRING_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			The string value specified by the line "%1" could not be created.
		</td>
	</tr>
</table>
.

MessageId = 523
SymbolicName = SN_EV_IAMSERV_STOPPED_AS_SERVICE
Language=English
NISServ stopped as Windows Service.
.
MessageId = 524
SymbolicName = SN_EV_IAMSERV_STOPPED_AS_SERVICE_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			NISServ stopped as Windows Service.
		</td>
	</tr>
</table>
.

MessageId = 525
SymbolicName = SN_EV_IAMSERV_SECURITY_ALERT_DISPLAYED
Language=English
Security alert displayed for rule %1.
Remote computer (%2, %3).
.
MessageId = 526
SymbolicName = SN_EV_IAMSERV_SECURITY_ALERT_DISPLAYED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Security alert displayed for rule %1.
			Remote computer (%2, %3).
		</td>
	</tr>
</table>
.
MessageId = 33293
SymbolicName = SN_EV_IAMSERV_SECURITY_ALERT_DISPLAYED_SHORT
Language=English
Security Alert!
"%1".
.

MessageId = 527
SymbolicName = SN_EV_IAMSERV_IP_ADDRESS_CHANGE
Language=English
Protecting your connection to a newly detected network on adapter "%1" (IP address: %2).
.
MessageId = 528
SymbolicName = SN_EV_IAMSERV_IP_ADDRESS_CHANGE_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Protecting your connection to a newly detected network on adapter "%1" (IP address: %2).
		</td>
	</tr>
</table>
.
MessageId = 33295
SymbolicName = SN_EV_IAMSERV_IP_ADDRESS_CHANGE_SHORT
Language=English
Protecting connection to new network on adapter "%1".
.

;//---------------------------------------------------------------------------
;// X F E R   E v e n t   M e s s a g e
;//---------------------------------------------------------------------------
MessageId = 529
SymbolicName = SN_EV_IAMSERV_REMOTE_CONFIG_QUERY
Language=English
Attempting to obtain a remote configuration update.
Remote IP address is %1.
Remote port is %2.
Protocol is %3.
Local file is "%4".
Remote file is "%5".
.
MessageId = 530
SymbolicName = SN_EV_IAMSERV_REMOTE_CONFIG_QUERY_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Attempting to obtain a remote configuration update.<br>
			Remote IP address is %1.<br>
			Remote port is %2.<br>
			Protocol is %3.<br>
			Local file is "%4".<br>
			Remote file is "%5".<br>
		</td>
	</tr>
</table>
.

MessageId = 531
SymbolicName = SN_EV_IAMSERV_REMOTE_TFTP_ERROR
Language=English
Remote TFTP error.
TFTP error is %1.
Error string is "%2".
.
MessageId = 532
SymbolicName = SN_EV_IAMSERV_REMOTE_TFTP_ERROR_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Remote TFTP error.<br>
			TFTP error is %1.<br>
			Error string is "%2".<br>
		</td>
	</tr>
</table>
.

MessageId = 533
SymbolicName = SN_EV_IAMSERV_REMOTE_TFTP_TIMEOUT
Language=English
TFTP transfer timed out. 
TFTP packets sent %1.
TFTP retransmit time-outs %2.
TFTP packets received %3.
TFTP valid packets received %4.
TFTP packets received out of order %5.
TFTP invalid data packet %6.
TFTP invalid packet size %7.
TFTP no room for packet %8.
.
MessageId = 534
SymbolicName = SN_EV_IAMSERV_REMOTE_TFTP_TIMEOUT_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			TFTP transfer timed out.<br>
			TFTP packets sent %1.<br>
			TFTP retransmit time-outs %2.<br>
			TFTP packets received %3.<br>
			TFTP valid packets received %4.<br>
			TFTP packets received out of order %5.<br>
			TFTP invalid data packet %6.<br>
			TFTP invalid packet size %7.<br>
			TFTP no room for packet %8.<br>
		</td>
	</tr>
</table>
.

MessageId = 535
SymbolicName = SN_EV_IAMSERV_REMOTE_HTTP_FAILURE
Language=English
HTTP transfer failed.
HTTP bytes send request %1.
HTTP receive state %2.
HTTP bytes received %3.
HTTP received packets %4.
HTTP header bytes received %5.
HTTP data bytes received %6.
HTTP transfer status %7.
HTTP error status %8.
.
MessageId = 536
SymbolicName = SN_EV_IAMSERV_REMOTE_HTTP_FAILURE_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			HTTP transfer failed.<br>
			HTTP bytes send request %1.<br>
			HTTP receive state %2.<br>
			HTTP bytes received %3.<br>
			HTTP received packets %4.<br>
			HTTP header bytes received %5.<br>
			HTTP data bytes received %6.<br>
			HTTP transfer status %7.<br>
			HTTP error status %8.<br>
		</td>
	</tr>
</table>
.

;//---------------------------------------------------------------------------
;//    S t a t i s t i c s   E v e n t   M e s s a g e
;//---------------------------------------------------------------------------
MessageId = 537
SymbolicName = SN_EV_SYM_ERROR
Language=English
Error - %1.
.
MessageId = 538
SymbolicName = SN_EV_SYM_ERROR_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Error - %1.
		</td>
	</tr>
</table>
.

MessageId = 539
SymbolicName = SN_EV_SYM_RESTRICTED_SITE_BLOCKED
Language=English
Blocked access for account %1 to restricted site "%2" due to %3.
.
MessageId = 540
SymbolicName = SN_EV_SYM_RESTRICTED_SITE_BLOCKED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Blocked access for account %1 to restricted site "%2" due to %3.
		</td>
	</tr>
</table>
.

MessageId = 541
SymbolicName = SN_EV_SYM_CONFIDENTIAL_BLOCKED
Language=English
Confidential data "%1" blocked to site "%2".
.
MessageId = 542
SymbolicName = SN_EV_SYM_CONFIDENTIAL_BLOCKED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Confidential data "%1" blocked to site "%2".
		</td>
	</tr>
</table>
.

MessageId = 543
SymbolicName = SN_EV_SYM_CONFIDENTIAL_BLOCKED_SHORT
Language=English
Confidential data "%1" blocked to site "%2".
.
MessageId = 544
SymbolicName = SN_EV_SYM_CONFIDENTIAL_BLOCKED_SHORT_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Confidential data "%1" blocked to site "%2".
		</td>
	</tr>
</table>
.

MessageId = 545
SymbolicName = SN_EV_SYM_CONFIDENTIAL_PERMITTED
Language=English
Confidential data "%1" permitted to site "%2".
.
MessageId = 546
SymbolicName = SN_EV_SYM_CONFIDENTIAL_PERMITTED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Confidential data "%1" permitted to site "%2".
		</td>
	</tr>
</table>
.

MessageId = 547
SymbolicName = SN_EV_SYM_CONFIDENTIAL_BLOCKED_IM
Language=English
Confidential data "%1" blocked to remote computer at "%2".
.
MessageId = 548
SymbolicName = SN_EV_SYM_CONFIDENTIAL_BLOCKED_IM_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Confidential data "%1" blocked to remote computer at "%2".
		</td>
	</tr>
</table>
.

MessageId = 549
SymbolicName = SN_EV_SYM_CONFIDENTIAL_PERMITTED_IM
Language=English
Confidential data "%1" permitted to remote comuter at "%2".
.
MessageId = 550
SymbolicName = SN_EV_SYM_CONFIDENTIAL_PERMITTED_IM_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Confidential data "%1" permitted to remote comuter at "%2".
		</td>
	</tr>
</table>
.

;//---------------------------------------------------------------------------
;//    I A M C P L		M e s s a g e s
;//---------------------------------------------------------------------------
;//
MessageId = 551
SymbolicName = SN_EV_IAMCPL_ALE_RULE_AUTO_CREATE
Language=English
A rule for "%1" was automatically created using a preconfigured rule.
.
MessageId = 552
SymbolicName = SN_EV_IAMCPL_ALE_RULE_AUTO_CREATE_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			A rule for "%1" was automatically created using a preconfigured rule.
		</td>
	</tr>
</table>
.
MessageId = 33319
SymbolicName = SN_EV_IAMCPL_ALE_RULE_AUTO_CREATE_SHORT
Language=English
Access rules automatically created for %1.
.

;//---------------------------------------------------------------------------
;//    M i s c e l l a n e o u s   E v e n t   M e s s a g e s
;//---------------------------------------------------------------------------
;//
MessageId = 553
SymbolicName = SN_EV_MISC_APPLICATION_ACCESSING_NETWORK
Language=English
An instance of "%1" is preparing to access the Internet.
.
MessageId = 554
SymbolicName = SN_EV_MISC_APPLICATION_ACCESSING_NETWORK_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">An instance of "%1" is preparing to access the Internet.</td>
	</tr>
</table>
.
MessageId = 33321
SymbolicName = SN_EV_MISC_APPLICATION_ACCESSING_NETWORK_SHORT
Language=English
%1 is preparing to access the Internet.
.

;//---------------------------------------------------------------------------
;//    A l e r t T r a c k e r		M e s s a g e s
;//---------------------------------------------------------------------------
;//
MessageId = 555
SymbolicName = SN_EV_ATRACK_RULE_MATCHED
Language=English
Rule %1 matched.
Remote address (%2,%3).
.
MessageId = 556
SymbolicName = SN_EV_ATRACK_RULE_MATCHED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Rule %1 matched.<br>
			Remote address (%2,%3).<br>
		</td>
	</tr>
</table>
.
MessageId = 33323
SymbolicName = SN_EV_ATRACK_RULE_MATCHED_SHORT
Language=English
Rule %1 matched.
.

MessageId = 557
SymbolicName = SN_EV_ATRACK_RULE_MATCHED_LISTEN
Language=English
Rule %1 matched.
Local address: %2(%3).
.
MessageId = 558
SymbolicName = SN_EV_ATRACK_RULE_MATCHED_LISTEN_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Rule %1 matched.<br>
			Local address: %2(%3).<br>
		</td>
	</tr>
</table>
.
MessageId = 33325
SymbolicName = SN_EV_ATRACK_RULE_MATCHED_LISTEN_SHORT
Language=English
Rule %1 matched.
.

;//---------------------------------------------------------------------------
;//    I D S	M e s s a g e s
;//---------------------------------------------------------------------------
;//
MessageId = 559
SymbolicName = SN_EV_IDS_ALERT_DETECTED_ATTACKER_IS_REMOTE
Language=English
Intrusion: %1.
Intruder: %2(%4). 
Risk Level: %3.
Protocol: %5.
Attacked IP: %7.
Attacked Port: %6.
.
MessageId = 560
SymbolicName = SN_EV_IDS_ALERT_DETECTED_ATTACKER_IS_REMOTE_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<body OnLoad="IDSEventOnLoad()">
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Attempted Intrusion "%1" against your machine was detected and blocked.<br>
			Intruder: <span class="BTLink" id="BTLink" onclick="LaunchBackTrace('%2')" title ="Trace this connection">%2</span>(%4).<br>
			Risk Level: %3.<br>
			Protocol: %5.<br>
			Attacked IP: %7.<br>
			Attacked Port: %6.<br>
		</td>
	</tr>
	<tr id="BTDescriptionRow">
		<td colspan=2 class="NisEvtEventValue">
			Click the address to trace the attacker.
		</td>
	</tr>
</table>
</body>
.
MessageId = 33327
SymbolicName = SN_EV_IDS_ALERT_DETECTED_ATTACKER_IS_REMOTE_SHORT
Language=English
"%1". 
Intrusion detected.
.

MessageId = 561
SymbolicName = SN_EV_IDS_ALERT_SRC_DEST_IP_EQUAL
Language=English
Intrusion: %1.
Risk Level: %3.
Port: %4.
Protocol: %5.
Due to the nature of this attack, it is not possible to identify the attacker.
.
MessageId = 562
SymbolicName = SN_EV_IDS_ALERT_SRC_DEST_IP_EQUAL_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Attempted Intrusion "%1" against your machine was detected and blocked.<br>
			Risk Level: %3.<br>
			Port: %4.<br>
			Protocol: %5.<br>
			Due to the nature of this attack, it is not possible to identify the attacker.<br>
		</td>
	</tr>
</table>
.
MessageId = 33329
SymbolicName = SN_EV_IDS_ALERT_SRC_DEST_IP_EQUAL_SHORT
Language=English
"%1". 
Intrusion detected.
.

MessageId = 563
SymbolicName = SN_EV_IDS_ALERT_DETECTED_ATTACKER_IS_LOCAL
Language=English
Intrusion: %1. 
Intruder: %2(%4). 
Risk Level: %3.
Protocol: %5.
Attacked IP: %6.
Attacked Port: %7.

.
MessageId = 564
SymbolicName = SN_EV_IDS_ALERT_DETECTED_ATTACKER_IS_LOCAL_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<body OnLoad="IDSEventOnLoad()">
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Attempted Intrusion "%1" from your machine against <span class="BTLink" id="BTLink" onclick="LaunchBackTrace('%6')" title ="Trace this connection">%6</span> was detected and blocked.<br>
			Intruder: %2(%4).<br>
			Risk Level: %3.<br>
			Protocol: %5.<br>
			Attacked IP: <span class="BTLink" id="BTLink" onclick="LaunchBackTrace('%6')" title ="Trace this connection">%6</span>.<br>
			Attacked Port: %7.<br>
		</td>
	</tr>
	<tr id="BTDescriptionRow">
		<td colspan=2 class="NisEvtEventValue">
			Click the address to trace the attacker.
		</td>
	</tr>
</table>
</body>
.

MessageId = 33331
SymbolicName = SN_EV_IDS_ALERT_DETECTED_ATTACKER_IS_LOCAL_SHORT
Language=English
"%1".
Intrusion detected.
.
MessageId = 565
SymbolicName = SN_EV_IDS_PORT_SCAN_DETECTED
Language=English
Attempted Intrusion "%1" against your machine was detected and blocked. 
Intruder: %4. 
At least %10 ports were probed.
.
MessageId = 566
SymbolicName = SN_EV_IDS_PORT_SCAN_DETECTED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<body OnLoad="IDSEventOnLoad()">
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Attempted Intrusion "%1" against your machine was detected and blocked.<br>
			Intruder: <span class="BTLink" id="BTLink" onclick="LaunchBackTrace('%4')" title ="Trace this connection">%4</span>.<br>
			At least %10 ports were probed.<br>
		</td>
	</tr>
	<tr id="BTDescriptionRow">
		<td colspan=2 class="NisEvtEventValue">
			Click the address to trace the attacker.
		</td>
	</tr>
</table>
</body>
.
MessageId = 33333
SymbolicName = SN_EV_IDS_PORT_SCAN_DETECTED_SHORT
Language=English
"%1". 
Intrusion detected.
.

MessageId = 567
SymbolicName = SN_EV_IDS_ENABLED
Language=English
Internet Worm Protection has been enabled.
.
MessageId = 568
SymbolicName = SN_EV_IDS_ENABLED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Internet Worm Protection has been enabled.
		</td>
	</tr>
</table>
.

MessageId = 569
SymbolicName = SN_EV_IDS_DISABLED
Language=English
Internet Worm Protection has been disabled.
.
MessageId = 570
SymbolicName = SN_EV_IDS_DISABLED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Internet Worm Protection has been disabled.
		</td>
	</tr>
</table>
.

MessageId = 571
SymbolicName = SN_EV_IDS_NUM_SIG_MON
Language=English
Internet Worm Protection is monitoring %1 signatures.
.
MessageId = 572
SymbolicName = SN_EV_IDS_NUM_SIG_MON_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Internet Worm Protection is monitoring %1 signatures.
		</td>
	</tr>
</table>
.

MessageId = 573
SymbolicName = SN_EV_IDS_EXCLUSIONS_KEY_NOT_FOUND
Language=English
An error occurred while loading the IDS Signature Exclusions. All signatures are being monitored.
.
MessageId = 574
SymbolicName = SN_EV_IDS_EXCLUSIONS_KEY_NOT_FOUND_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			An error occurred while loading the IDS Signature Exclusions. All signatures are being monitored.
		</td>
	</tr>
</table>
.

MessageId = 575
SymbolicName = SN_EV_IDS_EXCLUSIONS_MALLOC_ERR
Language=English
Not enough memory available to read the IDS Signature Exclusions. All signatures are being monitored.
.
MessageId = 576
SymbolicName = SN_EV_IDS_EXCLUSIONS_MALLOC_ERR_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Not enough memory available to read the IDS Signature Exclusions. All signatures are being monitored.
		</td>
	</tr>
</table>
.

MessageId = 577
SymbolicName = SN_EV_IDS_EXCLUSIONS_CORRUPT
Language=English
The list of IDS Signatures Exclusions is corrupt. Please reinstall the product.
.
MessageId = 578
SymbolicName = SN_EV_IDS_EXCLUSIONS_CORRUPT_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			The list of IDS Signatures Exclusions is corrupt. Please reinstall the product.
		</td>
	</tr>
</table>
.

MessageId = 579
SymbolicName = SN_EV_IDS_SIG_STORE_KEY_NOT_FOUND
Language=English
The list of IDS signatures is corrupt. The IDS is not monitoring your machine.
Please contact Technical Support if the problem persists after restarting your machine.
.
MessageId = 580
SymbolicName = SN_EV_IDS_SIG_STORE_KEY_NOT_FOUND_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			The list of IDS signatures is corrupt. The IDS is not monitoring your machine. Please contact Technical Support if the problem persists after restarting your machine.
		</td>
	</tr>
</table>
.

MessageId = 581
SymbolicName = SN_EV_IDS_SIG_STORE_CORRUPT
Language=English
The list of IDS signatures is corrupt. The IDS is not monitoring your machine.
Please contact Technical Support if the problem persists after restarting your machine.
.
MessageId = 582
SymbolicName = SN_EV_IDS_SIG_STORE_CORRUPT_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			The list of IDS signatures is corrupt. The IDS is not monitoring your machine. Please contact Technical Support if the problem persists after restarting your machine.
		</td>
	</tr>
</table>
.

MessageId = 583
SymbolicName = SN_EV_IDS_SIG_STORE_MALLOC_ERR
Language=English
Not enough memory available to read the IDS Signatures.
.
MessageId = 584
SymbolicName = SN_EV_IDS_SIG_STORE_MALLOC_ERR_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Not enough memory available to read the IDS Signatures.
		</td>
	</tr>
</table>
.

MessageId = 585
SymbolicName = SN_EV_IDS_ENGINE_INIT_ERROR
Language=English
An error occurred while starting the Internet Worm Protection Module. Please reinstall the product.
.
MessageId = 586
SymbolicName = SN_EV_IDS_ENGINE_INIT_ERROR_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			An error occurred while starting the Internet Worm Protection Module. Please reinstall the product.
		</td>
	</tr>
</table>
.

MessageId = 587
SymbolicName = SN_EV_IDS_SIGNATURES_UPDATED
Language=English
LiveUpdate has updated your IDS signatures.
.
MessageId = 588
SymbolicName = SN_EV_IDS_SIGNATURES_UPDATED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			LiveUpdate has updated your IDS signatures.
		</td>
	</tr>
</table>
.
MessageId = 33355
SymbolicName = SN_EV_IDS_SIGNATURES_UPDATED_SHORT
Language=English
LiveUpdate has updated your IDS signatures.
.

MessageId = 589
SymbolicName = SN_EV_IDS_ALERT_CAUSED_AUTOBLOCK
Language=English
Intrusion detected and blocked. All communication with %1 will be blocked for %2 minutes.
.
MessageId = 590
SymbolicName = SN_EV_IDS_ALERT_CAUSED_AUTOBLOCK_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<body OnLoad="IDSEventOnLoad()">
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
		Intrusion detected and blocked. All communication with
		<span class="BTLink" id="BTLink" onclick="LaunchBackTrace('%1')" title ="Trace this connection">%1</span> will be blocked for %2 minutes.
		</td>
	</tr>
	<tr id="BTDescriptionRow">
		<td colspan=2 class="NisEvtEventValue">
			Click the address to trace the attacker.
		</td>
	</tr>
</table>
</body>
.
MessageId = 33357
SymbolicName = SN_EV_IDS_ALERT_CAUSED_AUTOBLOCK_SHORT
Language=English
%1 blocked for %2 minutes.
.

MessageId = 591
SymbolicName = SN_EV_IDS_NUM_SIG_MON_ZERO
Language=English
IDS is not monitoring any signatures. This is because the list of IDS signatures
is corrupt or you have excluded all the IDS signatures. If you are not aware of 
having excluded all the IDS signatures, please reinstall the product.
.
MessageId = 592
SymbolicName = SN_EV_IDS_NUM_SIG_MON_ZERO_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			IDS is not monitoring any signatures. This is because the list of IDS signatures
			is corrupt or you have excluded all the IDS signatures. If you are not aware of 
			having excluded all the IDS signatures, please reinstall the product.
		</td>
	</tr>
</table>
.

MessageId = 593
SymbolicName = SN_EV_SYMIDSCO_VER_INCOMPATIBLE
Language=English
An incompatible version of SymIDSCo was detected. 
SymIDS Version:%1.%2.%3.%4. SymIDSCo Version:%5.%6.%7.%8. 
Please run LiveUpdate to correct this problem or reinstall the product.
.
MessageId = 594
SymbolicName = SN_EV_SYMIDSCO_VER_INCOMPATIBLE_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			An incompatible version of SymIDSCo was detected. <br>
			SymIDS Version:%1.%2.%3.%4. SymIDSCo Version:%5.%6.%7.%8. <br>
			Please run LiveUpdate to correct this problem or reinstall the product.<br>
		</td>
	</tr>
</table>
.

MessageId = 595
SymbolicName = SN_EV_IDS_ALERT_DETECTED_ATTACKER_IS_REMOTE_NO_PORT
Language=English
Intrusion: %1.
Intruder: %2. 
Risk Level: %3.
Protocol: %4.
Attacked IP: %5.
.
MessageId = 596
SymbolicName = SN_EV_IDS_ALERT_DETECTED_ATTACKER_IS_REMOTE_NO_PORT_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<body OnLoad="IDSEventOnLoad()">
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Attempted Intrusion "%1" against your machine was detected and blocked.<br>
			Intruder: <span class="BTLink" id="BTLink" onclick="LaunchBackTrace('%2')" title ="Trace this connection">%2.</span><br>
			Risk Level: %3.<br>
			Protocol: %4.<br>
			Attacked IP: %5.<br>
		</td>
	</tr>
	<tr id="BTDescriptionRow">
		<td colspan=2 class="NisEvtEventValue">
			Click the address to trace the attacker.
		</td>
	</tr>
</table>
</body>
.
MessageId = 33363
SymbolicName = SN_EV_IDS_ALERT_DETECTED_ATTACKER_IS_REMOTE_NO_PORT_SHORT
Language=English
"%1". 
Intrusion detected.
.

MessageId = 597
SymbolicName = SN_EV_IDS_ALERT_DETECTED_ATTACKER_IS_LOCAL_NO_PORT
Language=English
Intrusion: %1.
Intruder: %2. 
Risk Level: %3.
Protocol: %4.
Attacked IP: %5.
.
MessageId = 598
SymbolicName = SN_EV_IDS_ALERT_DETECTED_ATTACKER_IS_LOCAL_NO_PORT_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<body OnLoad="IDSEventOnLoad()">
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Attempted Intrusion "%1" from your machine against <span class="BTLink" id="BTLink" onclick="LaunchBackTrace('%5')" title ="Trace this connection">%5</span> was detected and blocked.<br>
			Intruder: %2.<br>
			Risk Level: %3.<br>
			Protocol: %4.<br>
		</td>
	</tr>
	<tr id="BTDescriptionRow">
		<td colspan=2 class="NisEvtEventValue">
			Click the address to trace the attacker.
		</td>
	</tr>
</table>
</body>
.
MessageId = 33365
SymbolicName = SN_EV_IDS_ALERT_DETECTED_ATTACKER_IS_LOCAL_NO_PORT_SHORT
Language=English
"%1". 
Intrusion detected.
.

MessageId = 651
SymbolicName = SN_EV_IDS_FILTER_IP_FLAGS_INVALID
Language=English
Intrusion: %1.
Intruder: %2.
Risk Level: %3.
Attacked IP: %5.
Protocol: %7.
IP Flags and Fragment Offset= %6.
.
MessageId = 652
SymbolicName = SN_EV_IDS_FILTER_IP_FLAGS_INVALID_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<body OnLoad="IDSEventOnLoad()">
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Intrusion: %1.<br>
			Intruder: <span class="BTLink" id="BTLink" onclick="LaunchBackTrace('%2')" title ="Trace this connection">%2</span>.<br>
			Risk Level: %3.<br>
			Source IP address: <span class="BTLink" id="BTLink" onclick="LaunchBackTrace('%4')" title ="Trace this connection">%4</span>.<br>
			Destination IP address: <span class="BTLink" id="BTLink" onclick="LaunchBackTrace('%5')" title ="Trace this connection">%5</span>.<br>
			Protocol: %7.<br>
			IP Flags and Fragment Offset: %6. This field is invalid.<br>
		</td>
	</tr>
	<tr id="BTDescriptionRow">
		<td colspan=2 class="NisEvtEventValue">
			Click the address to trace the attacker.
		</td>
	</tr>
</table>
</body>
.
MessageId = 33419
SymbolicName = SN_EV_IDS_FILTER_IP_FLAGS_INVALID_SHORT
Language=English
"%1". 
Intrusion detected.
.

MessageId = 653
SymbolicName = SN_EV_IDS_FILTER_IP_CHECKSUM_INVALID
Language=English
Intrusion: %1.
Intruder: %2.
Risk Level: %3.
Attacked IP: %5.
Protocol: %8.
IP Checksum= %7. Expected IP Checksum= %6.
.
MessageId = 654
SymbolicName = SN_EV_IDS_FILTER_IP_CHECKSUM_INVALID_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<body OnLoad="IDSEventOnLoad()">
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Intrusion: %1.<br>
			Intruder: <span class="BTLink" id="BTLink" onclick="LaunchBackTrace('%2')" title ="Trace this connection">%2</span>.<br>
			Risk Level: %3.<br>
			Source IP address: <span class="BTLink" id="BTLink" onclick="LaunchBackTrace('%4')" title ="Trace this connection">%4</span>.<br>
			Destination IP address: <span class="BTLink" id="BTLink" onclick="LaunchBackTrace('%5')" title ="Trace this connection">%5</span>.<br>
			Protocol: %8.<br>
			The IP Checksum field in the packet indicates a incorrect value of %7, instead of %6.<br>
		</td>
	</tr>
	<tr id="BTDescriptionRow">
		<td colspan=2 class="NisEvtEventValue">
			Click the address to trace the attacker.
		</td>
	</tr>
</table>
</body>
.
MessageId = 33421
SymbolicName = SN_EV_IDS_FILTER_IP_CHECKSUM_INVALID_SHORT
Language=English
"%1". 
Intrusion detected.
.

MessageId = 655
SymbolicName = SN_EV_IDS_FILTER_SOURCE_IP_INVALID
Language=English
Intrusion: %1.
Intruder: %2.
Risk Level: %3.
Destination IP address: %5.
Protocol: %6.
The Source IP address %4 is invalid.
.
MessageId = 656
SymbolicName = SN_EV_IDS_FILTER_SOURCE_IP_INVALID_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<body OnLoad="IDSEventOnLoad()">
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Intrusion: %1.<br>
			Intruder: <span class="BTLink" id="BTLink" onclick="LaunchBackTrace('%2')" title ="Trace this connection">%2</span>.<br>
			Risk Level: %3.<br>
			Source IP address: <span class="BTLink" id="BTLink" onclick="LaunchBackTrace('%4')" title ="Trace this connection">%4</span>.This IP address is invalid.<br>
			Destination IP address: <span class="BTLink" id="BTLink" onclick="LaunchBackTrace('%5')" title ="Trace this connection">%5</span>.<br>
			Protocol: %6.<br>
		</td>
	</tr>
	<tr id="BTDescriptionRow">
		<td colspan=2 class="NisEvtEventValue">
			Click the address to trace the attacker.
		</td>
	</tr>
</table>
</body>
.
MessageId = 33423
SymbolicName = SN_EV_IDS_FILTER_SOURCE_IP_INVALID_SHORT
Language=English
"%1". 
Intrusion detected.
.

MessageId = 657
SymbolicName = SN_EV_IDS_FILTER_DEST_IP_INVALID
Language=English
Intrusion: %1.
Intruder: %2.
Risk Level: %3.
Source IP address: %4.
Protocol: %6.
The Destination IP address %5 is invalid.
.
MessageId = 658
SymbolicName = SN_EV_IDS_FILTER_DEST_IP_INVALID_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<body OnLoad="IDSEventOnLoad()">
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Intrusion: %1.<br>
			Intruder: <span class="BTLink" id="BTLink" onclick="LaunchBackTrace('%2')" title ="Trace this connection">%2</span>.<br>
			Risk Level: %3.<br>
			Source IP address: <span class="BTLink" id="BTLink" onclick="LaunchBackTrace('%4')" title ="Trace this connection">%4</span>.<br>
			Destination IP address: <span class="BTLink" id="BTLink" onclick="LaunchBackTrace('%5')" title ="Trace this connection">%5</span>. This IP address is invalid.<br>
			Protocol: %6.<br>
		</td>
	</tr>
	<tr id="BTDescriptionRow">
		<td colspan=2 class="NisEvtEventValue">
			Click the address to trace the attacker.
		</td>
	</tr>
</table>
</body>
.
MessageId = 33425
SymbolicName = SN_EV_IDS_FILTER_DEST_IP_INVALID_SHORT
Language=English
"%1". 
Intrusion detected.
.

MessageId = 659
SymbolicName = SN_EV_IDS_FILTER_IP_OPTIONS_INVALID
Language=English
Intrusion: %1.
Intruder: %2.
Risk Level: %3.
Source IP address: %4.
Destination IP address: %5.
Protocol: %6.
Invalid IP Option: %7.
.
MessageId = 660
SymbolicName = SN_EV_IDS_FILTER_IP_OPTIONS_INVALID_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<body OnLoad="IDSEventOnLoad()">
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Intrusion: %1.<br>
			Intruder: <span class="BTLink" id="BTLink" onclick="LaunchBackTrace('%2')" title ="Trace this connection">%2</span>.<br>
			Risk Level: %3.<br>
			Source IP address: <span class="BTLink" id="BTLink" onclick="LaunchBackTrace('%4')" title ="Trace this connection">%4</span>.<br>
			Destination IP address: <span class="BTLink" id="BTLink" onclick="LaunchBackTrace('%5')" title ="Trace this connection">%5</span>.<br>
			Protocol: %6.<br>
			Invalid IP Option: %7.<br>
		</td>
	</tr>
	<tr id="BTDescriptionRow">
		<td colspan=2 class="NisEvtEventValue">
			Click the address to trace the attacker.
		</td>
	</tr>
</table>
</body>
.
MessageId = 33427
SymbolicName = SN_EV_IDS_FILTER_IP_OPTIONS_INVALID_SHORT
Language=English
"%1". 
Intrusion detected.
.

MessageId = 661
SymbolicName = SN_EV_IDS_FILTER_TCP_SRC_PORT_INVALID
Language=English
Intrusion: %1.
Intruder: %2.
Risk Level: %3.
Source IP address: %4.
Destination IP address: %5.
TCP Source Port: %6. Invalid.
TCP Destination Port: %7.
.
MessageId = 662
SymbolicName = SN_EV_IDS_FILTER_TCP_SRC_PORT_INVALID_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<body OnLoad="IDSEventOnLoad()">
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Intrusion: %1.<br>
			Intruder: <span class="BTLink" id="BTLink" onclick="LaunchBackTrace('%2')" title ="Trace this connection">%2</span>.<br>
			Risk Level: %3.<br>
			Source IP address: <span class="BTLink" id="BTLink" onclick="LaunchBackTrace('%4')" title ="Trace this connection">%4</span>.<br>
			Destination IP address: <span class="BTLink" id="BTLink" onclick="LaunchBackTrace('%5')" title ="Trace this connection">%5</span>.<br>
			TCP Source Port: %6. This is an invalid port number.<br>
			TCP Destination Port: %7<br>
		</td>
	</tr>
	<tr id="BTDescriptionRow">
		<td colspan=2 class="NisEvtEventValue">
			Click the address to trace the attacker.
		</td>
	</tr>
</table>
</body>
.
MessageId = 33429
SymbolicName = SN_EV_IDS_FILTER_TCP_SRC_PORT_INVALID_SHORT
Language=English
"%1". 
Intrusion detected.
.

MessageId = 663
SymbolicName = SN_EV_IDS_FILTER_TCP_DEST_PORT_INVALID
Language=English
Intrusion: %1.
Intruder: %2.
Risk Level: %3.
Source IP address: %4.
Destination IP address: %5.
TCP Source Port: %6.
TCP Destination Port: %7. Invalid.
.
MessageId = 664
SymbolicName = SN_EV_IDS_FILTER_TCP_DEST_PORT_INVALID_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<body OnLoad="IDSEventOnLoad()">
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Intrusion: %1.<br>
			Intruder: <span class="BTLink" id="BTLink" onclick="LaunchBackTrace('%2')" title ="Trace this connection">%2</span>.<br>
			Risk Level: %3.<br>
			Source IP address: <span class="BTLink" id="BTLink" onclick="LaunchBackTrace('%4')" title ="Trace this connection">%4</span>.<br>
			Destination IP address: <span class="BTLink" id="BTLink" onclick="LaunchBackTrace('%5')" title ="Trace this connection">%5</span>.<br>
			TCP Source Port: %6.<br>
			TCP Destination Port: %7. This is an invalid port number.<br>
		</td>
	</tr>
	<tr id="BTDescriptionRow">
		<td colspan=2 class="NisEvtEventValue">
			Click the address to trace the attacker.
		</td>
	</tr>
</table>
</body>
.
MessageId = 33431
SymbolicName = SN_EV_IDS_FILTER_TCP_DEST_PORT_INVALID_SHORT
Language=English
"%1". 
Intrusion detected.
.

MessageId = 665
SymbolicName = SN_EV_IDS_FILTER_TCP_FLAGS_INVALID
Language=English
Intrusion: %1.
Intruder: %2.
Risk Level: %3.
Source IP address: %4.
Destination IP address: %5.
TCP Source Port: %6.
TCP Destination Port: %7.
TCP Flags invalid: %8.
.
MessageId = 666
SymbolicName = SN_EV_IDS_FILTER_TCP_FLAGS_INVALID_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<body OnLoad="IDSEventOnLoad()">
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Intrusion: %1.<br>
			Intruder: <span class="BTLink" id="BTLink" onclick="LaunchBackTrace('%2')" title ="Trace this connection">%2</span>.<br>
			Risk Level: %3.<br>
			Source IP address: <span class="BTLink" id="BTLink" onclick="LaunchBackTrace('%4')" title ="Trace this connection">%4</span>.<br>
			Destination IP address: <span class="BTLink" id="BTLink" onclick="LaunchBackTrace('%5')" title ="Trace this connection">%5</span>.<br>
			TCP Source Port: %6.<br>
			TCP Destination Port: %7.<br>
			TCP Flags invalid: %8.<br>
		</td>
	</tr>
	<tr id="BTDescriptionRow">
		<td colspan=2 class="NisEvtEventValue">
			Click the address to trace the attacker.
		</td>
	</tr>
</table>
</body>
.
MessageId = 33433
SymbolicName = SN_EV_IDS_FILTER_TCP_FLAGS_INVALID_SHORT
Language=English
"%1". 
Intrusion detected.
.

MessageId = 667
SymbolicName = SN_EV_IDS_FILTER_TCP_OPTIONS_INVALID
Language=English
Intrusion: %1.
Intruder: %2.
Risk Level: %3.
Source IP address: %4.
Destination IP address: %5.
TCP Source Port: %6.
TCP Destination Port: %7.
Invalid TCP Option: %8.
.
MessageId = 668
SymbolicName = SN_EV_IDS_FILTER_TCP_OPTIONS_INVALID_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<body OnLoad="IDSEventOnLoad()">
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Intrusion: %1.<br>
			Intruder: <span class="BTLink" id="BTLink" onclick="LaunchBackTrace('%2')" title ="Trace this connection">%2</span>.<br>
			Risk Level: %3.<br>
			Source IP address: <span class="BTLink" id="BTLink" onclick="LaunchBackTrace('%4')" title ="Trace this connection">%4</span>.<br>
			Destination IP address: <span class="BTLink" id="BTLink" onclick="LaunchBackTrace('%5')" title ="Trace this connection">%5</span>.<br>
			TCP Source Port: %6.<br>
			TCP Destination Port: %7.<br>
			Invalid TCP Option: %8.<br>
		</td>
	</tr>
	<tr id="BTDescriptionRow">
		<td colspan=2 class="NisEvtEventValue">
			Click the address to trace the attacker.
		</td>
	</tr>
</table>
</body>
.
MessageId = 33435
SymbolicName = SN_EV_IDS_FILTER_TCP_OPTIONS_INVALID_SHORT
Language=English
"%1". 
Intrusion detected.
.
MessageId = 669
SymbolicName = SN_EV_IDS_FILTER_UDP_SRC_PORT_INVALID
Language=English
Intrusion: %1.
Intruder: %2.
Risk Level: %3.
Source IP address: %4.
Destination IP address: %5.
UDP Source Port: %6. Invalid.
UDP Destination Port: %7.
.
MessageId = 670
SymbolicName = SN_EV_IDS_FILTER_UDP_SRC_PORT_INVALID_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<body OnLoad="IDSEventOnLoad()">
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Intrusion: %1.<br>
			Intruder: <span class="BTLink" id="BTLink" onclick="LaunchBackTrace('%2')" title ="Trace this connection">%2</span>.<br>
			Risk Level: %3.<br>
			Source IP address: <span class="BTLink" id="BTLink" onclick="LaunchBackTrace('%4')" title ="Trace this connection">%4</span>.<br>
			Destination IP address: <span class="BTLink" id="BTLink" onclick="LaunchBackTrace('%5')" title ="Trace this connection">%5</span>.<br>
			UDP Source Port: %6. This port number is invalid.<br>
			UDP Destination Port: %7.<br>
		</td>
	</tr>
	<tr id="BTDescriptionRow">
		<td colspan=2 class="NisEvtEventValue">
			Click the address to trace the attacker.
		</td>
	</tr>
</table>
</body>
.
MessageId = 33437
SymbolicName = SN_EV_IDS_FILTER_UDP_SRC_PORT_INVALID_SHORT
Language=English
"%1". 
Intrusion detected.
.

MessageId = 671
SymbolicName = SN_EV_IDS_FILTER_UDP_DEST_PORT_INVALID
Language=English
Intrusion: %1.
Intruder: %2.
Risk Level: %3.
Source IP address: %4.
Destination IP address: %5.
UDP Source Port: %6.
UDP Destination Port: %7. Invalid.
.
MessageId = 672
SymbolicName = SN_EV_IDS_FILTER_UDP_DEST_PORT_INVALID_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<body OnLoad="IDSEventOnLoad()">
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Intrusion: %1.<br>
			Intruder: <span class="BTLink" id="BTLink" onclick="LaunchBackTrace('%2')" title ="Trace this connection">%2</span>.<br>
			Risk Level: %3.<br>
			Source IP address: <span class="BTLink" id="BTLink" onclick="LaunchBackTrace('%4')" title ="Trace this connection">%4</span>.<br>
			Destination IP address: <span class="BTLink" id="BTLink" onclick="LaunchBackTrace('%5')" title ="Trace this connection">%5</span>.<br>
			UDP Source Port: %6.<br>
			UDP Destination Port: %7. This port number is invalid.<br>
		</td>
	</tr>
	<tr id="BTDescriptionRow">
		<td colspan=2 class="NisEvtEventValue">
			Click the address to trace the attacker.
		</td>
	</tr>
</table>
</body>
.
MessageId = 33439
SymbolicName = SN_EV_IDS_FILTER_UDP_DEST_PORT_INVALID_SHORT
Language=English
"%1".
Intrusion detected.
.

MessageId = 673
SymbolicName = SN_EV_IDS_FILTER_ICMP_CODE_INVALID
Language=English
Intrusion: %1.
Intruder: %2.
Risk Level: %3.
Source IP address: %4.
Destination IP address: %5.
ICMP Type: %6.
ICMP Code: %7. Invalid.
.
MessageId = 674
SymbolicName = SN_EV_IDS_FILTER_ICMP_CODE_INVALID_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<body OnLoad="IDSEventOnLoad()">
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Intrusion: %1.<br>
			Intruder: <span class="BTLink" id="BTLink" onclick="LaunchBackTrace('%2')" title ="Trace this connection">%2</span>.<br>
			Risk Level: %3.<br>
			Source IP address: <span class="BTLink" id="BTLink" onclick="LaunchBackTrace('%4')" title ="Trace this connection">%4</span>.<br>
			Destination IP address: <span class="BTLink" id="BTLink" onclick="LaunchBackTrace('%5')" title ="Trace this connection">%5</span>.<br>
			ICMP Type: %6.<br>
			ICMP Code: %7. This ICMP Code is invalid.<br>
		</td>
	</tr>
	<tr id="BTDescriptionRow">
		<td colspan=2 class="NisEvtEventValue">
			Click the address to trace the attacker.
		</td>
	</tr>
</table>
</body>
.
MessageId = 33441
SymbolicName = SN_EV_IDS_FILTER_ICMP_CODE_INVALID_SHORT
Language=English
"%1". 
Intrusion detected.
.

MessageId = 675
SymbolicName = SN_EV_IDS_FILTER_ICMP_TYPE_INVALID
Language=English
Intrusion: %1.
Intruder: %2.
Risk Level: %3.
Source IP address: %4.
Destination IP address: %5.
ICMP Code: %7.
ICMP Type: %6. Invalid.
.
MessageId = 676
SymbolicName = SN_EV_IDS_FILTER_ICMP_TYPE_INVALID_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<body OnLoad="IDSEventOnLoad()">
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Intrusion: %1.<br>
			Intruder: <span class="BTLink" id="BTLink" onclick="LaunchBackTrace('%2')" title ="Trace this connection">%2</span>.<br>
			Risk Level: %3.<br>
			Source IP address: <span class="BTLink" id="BTLink" onclick="LaunchBackTrace('%4')" title ="Trace this connection">%4</span>.<br>
			Destination IP address: <span class="BTLink" id="BTLink" onclick="LaunchBackTrace('%5')" title ="Trace this connection">%5</span>.<br>
			ICMP Code: %7.<br>
			ICMP Type: %6. This ICMP Type is invalid.<br>
		</td>
	</tr>
	<tr id="BTDescriptionRow">
		<td colspan=2 class="NisEvtEventValue">
			Click the address to trace the attacker.
		</td>
	</tr>
</table>
</body>
.
MessageId = 33443
SymbolicName = SN_EV_IDS_FILTER_ICMP_TYPE_INVALID_SHORT
Language=English
"%1". 
Intrusion detected.
.

MessageId = 677
SymbolicName = SN_EV_IDS_FILTER_TCP_FLAGS_INVALID_NMAP_NULL
Language=English
Intrusion: %1.
Intruder: %2.
Risk Level: %3.
Source IP address: %4.
Destination IP address: %5.
TCP Source Port: %6.
TCP Destination Port: %7.
TCP Header Flags: %8. Invalid.
.
MessageId = 678
SymbolicName = SN_EV_IDS_FILTER_TCP_FLAGS_INVALID_NMAP_NULL_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<body OnLoad="IDSEventOnLoad()">
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Intrusion: %1.<br>
			Intruder: <span class="BTLink" id="BTLink" onclick="LaunchBackTrace('%2')" title ="Trace this connection">%2</span>.<br>
			Risk Level: %3.<br>
			Source IP address: <span class="BTLink" id="BTLink" onclick="LaunchBackTrace('%4')" title ="Trace this connection">%4</span>.<br>
			Destination IP address: <span class="BTLink" id="BTLink" onclick="LaunchBackTrace('%5')" title ="Trace this connection">%5</span>.<br>
			TCP Source Port: %6.<br>
			TCP Destination Port: %7.<br>
			TCP Header Flags: %8. These TCP Flags are invalid and the packet is characteristic of an NMap Null Scan.<br>
		</td>
	</tr>
	<tr id="BTDescriptionRow">
		<td colspan=2 class="NisEvtEventValue">
			Click the address to trace the attacker.
		</td>
	</tr>
</table>
</body>
.
MessageId = 33445
SymbolicName = SN_EV_IDS_FILTER_TCP_FLAGS_INVALID_NMAP_NULL_SHORT
Language=English
"%1". 
Intrusion detected.
.

MessageId = 679
SymbolicName = SN_EV_IDS_FILTER_TCP_FLAGS_INVALID_NMAP_XMAS
Language=English
Intrusion: %1.
Intruder: %2.
Risk Level: %3.
Source IP address: %4.
Destination IP address: %5.
TCP Source Port: %6.
TCP Destination Port: %7.
TCP Header Flags: %8. Invalid.
.
MessageId = 680
SymbolicName = SN_EV_IDS_FILTER_TCP_FLAGS_INVALID_NMAP_XMAS_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<body OnLoad="IDSEventOnLoad()">
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Intrusion: %1.<br>
			Intruder: <span class="BTLink" id="BTLink" onclick="LaunchBackTrace('%2')" title ="Trace this connection">%2</span>.<br>
			Risk Level: %3.<br>
			Source IP address: <span class="BTLink" id="BTLink" onclick="LaunchBackTrace('%4')" title ="Trace this connection">%4</span>.<br>
			Destination IP address: <span class="BTLink" id="BTLink" onclick="LaunchBackTrace('%5')" title ="Trace this connection">%5</span>.<br>
			TCP Source Port: %6.<br>
			TCP Destination Port: %7.<br>
			TCP Header Flags: %8. These TCP Flags are invalid and the packet is characteristic of an NMap Xmas Scan.<br>
		</td>
	</tr>
	<tr id="BTDescriptionRow">
		<td colspan=2 class="NisEvtEventValue">
			Click the address to trace the attacker.
		</td>
	</tr>
</table>
</body>
.
MessageId = 33447
SymbolicName = SN_EV_IDS_FILTER_TCP_FLAGS_INVALID_NMAP_XMAS_SHORT
Language=English
"%1". 
Intrusion detected.
.

MessageId = 681
SymbolicName = SN_EV_IDS_FILTER_PORTSCAN
Language=English
Intrusion: %1.
Attacker: %2.
Risk Level: %3.
At least %4 ports were probed.
.
MessageId = 682
SymbolicName = SN_EV_IDS_FILTER_PORTSCAN_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<body OnLoad="IDSEventOnLoad()">
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Intrusion: %1.<br>
			Attacker: <span class="BTLink" id="BTLink" onclick="LaunchBackTrace('%2')" title ="Trace this connection">%2</span>.<br>
			Risk Level: %3.<br>
			At least %4 ports were probed.<br>
		</td>
	</tr>
	<tr id="BTDescriptionRow">
		<td colspan=2 class="NisEvtEventValue">
			Click the address to trace the attacker.
		</td>
	</tr>
</table>
</body>
.
MessageId = 33449
SymbolicName = SN_EV_IDS_FILTER_PORTSCAN_SHORT
Language=English
"%1". 
Intrusion detected.
.


;//---------------------------------------------------------------------------
;//    N N T P	M e s s a g e s
;//---------------------------------------------------------------------------
;//
MessageId = 599
SymbolicName = SN_EV_NNTP_GROUP_BLOCKED
Language=English
NNTP request to connect to newsgroup %1 was blocked.
.
MessageId = 600
SymbolicName = SN_EV_NNTP_GROUP_BLOCKED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			NNTP request to connect to newsgroup %1 was blocked.
		</td>
	</tr>
</table>
.

MessageId = 601
SymbolicName = SN_EV_NNTP_GROUP_REMOVED
Language=English
NNTP server response contained newsgroup: %1.  Newsgroup was removed from server response.
.
MessageId = 602
SymbolicName = SN_EV_NNTP_GROUP_REMOVED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			NNTP server response contained newsgroup: %1.  Newsgroup was removed from server response.
		</td>
	</tr>
</table>
.

MessageId = 603
SymbolicName = SN_EV_NNTP_POST_BLOCKED
Language=English
NNTP POST request to newsgroup: %1 was blocked.
.
MessageId = 604
SymbolicName = SN_EV_NNTP_POST_BLOCKED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			NNTP POST request to newsgroup: %1 was blocked.
		</td>
	</tr>
</table>
.

;//---------------------------------------------------------------------------
;//    S t a t i s t i c s   E v e n t   M e s s a g e
;//---------------------------------------------------------------------------
;//
MessageId = 605
SymbolicName = SN_EV_STATS_CONNECTION
Language=English
Connection:  %1: %2  %3  %4: %5,  %6 bytes sent,  %7 bytes received,  %8 elapsed time.
.
MessageId = 606
SymbolicName = SN_EV_STATS_CONNECTION_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Connection:  %1: %2.<br>
			%3  %4: %5.<br>
			%6 bytes sent.<br>
			%7 bytes received.<br>
			%8 elapsed time.<br>
		</td>
	</tr>
</table>
.

MessageId = 607
SymbolicName = SN_EV_STATS_REDIRECTED_CONNECTION
Language=English
Redirected Connection:  %1: %2  %3  %4: %5,  %6 bytes sent,  %7 bytes received,  %8 elapsed time.
.
MessageId = 608
SymbolicName = SN_EV_STATS_REDIRECTED_CONNECTION_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Redirected Connection:  %1: %2.<br>
			%3  %4: %5.<br>
			%6 bytes sent.<br>
			%7 bytes received.<br>
			%8 elapsed time.<br>
		</td>
	</tr>
</table>
.

;//---------------------------------------------------------------------------
;//    C o n f i g u r a t i o n   C h a n g e d / U p d a t e d   M e s s a g e s
;//---------------------------------------------------------------------------
;//
MessageId = 609
SymbolicName = SN_EV_CONFIG_SETTINGS_PKG_UPDATED
Language=English
Settings package "%1" posted by admin on %2 installed successfully.
Package server: (%3,%4).
Package path: %5.
.
MessageId = 610
SymbolicName = SN_EV_CONFIG_SETTINGS_PKG_UPDATED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Settings package "%1" posted by admin on %2 installed successfully.<br>
			Package server: (%3,%4).<br>
			Package path: %5.<br>
		</td>
	</tr>
</table>
.

MessageId = 611
SymbolicName = SN_EV_CONFIG_SETTINGS_PKG_DOWNLOAD_FAILED
Language=English
Settings package "%1" posted by admin on %2 could not be downloaded.
Package server: (%3,%4).
Package path: %5.
.
MessageId = 612
SymbolicName = SN_EV_CONFIG_SETTINGS_PKG_DOWNLOAD_FAILED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Settings package "%1" posted by admin on %2 could not be downloaded.<br>
			Package server: (%3,%4).<br>
			Package path: %5.<br>
		</td>
	</tr>
</table>
.

MessageId = 613
SymbolicName = SN_EV_CONFIG_SETTINGS_PKG_VALIDATION_FAILED
Language=English
Settings package "%1" posted by admin on %2 failed validation.
Package server: (%3,%4).
Package path: %5.
.
MessageId = 614
SymbolicName = SN_EV_CONFIG_SETTINGS_PKG_VALIDATION_FAILED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Settings package "%1" posted by admin on %2 failed validation.<br>
			Package server: (%3,%4).<br>
			Package path: %5.<br>
		</td>
	</tr>
</table>
.

MessageId = 615
SymbolicName = SN_EV_CONFIG_SETTINGS_PKG_INSTALL_FAILED
Language=English
Settings package "%1" posted by admin on %2 failed to install.
Package server: (%3,%4).
Package path: %5.
.
MessageId = 616
SymbolicName = SN_EV_CONFIG_SETTINGS_PKG_INSTALL_FAILED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Settings package "%1" posted by admin on %2 failed to install.<br>
			Package server: (%3,%4).<br>
			Package path: %5.<br>
		</td>
	</tr>
</table>
.

MessageId = 617
SymbolicName = SN_EV_CONFIG_SETTINGS_PKG_FIREWALL_REFRESH_FAILED
Language=English
Internet Worm Protection failed to refresh after installing settings package "%1" posted by admin on %2.
Package server: (%3,%4).
Package path: %5.
.
MessageId = 618
SymbolicName = SN_EV_CONFIG_SETTINGS_PKG_FIREWALL_REFRESH_FAILED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Internet Worm Protection failed to refresh after installing settings package "%1" posted by admin on %2.<br>
			Package server: (%3,%4).<br>
			Package path: %5.<br>
		</td>
	</tr>
</table>
.

MessageId = 619
SymbolicName = SN_EV_CONFIG_RULE_PKG_UPDATED
Language=English
Rule package "%1" posted by admin on %2 installed successfully.
Package server: (%3,%4).
Package path: %5.
.
MessageId = 620
SymbolicName = SN_EV_CONFIG_RULE_PKG_UPDATED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Rule package "%1" posted by admin on %2 installed successfully.<br>
			Package server: (%3,%4).<br>
			Package path: %5.<br>
		</td>
	</tr>
</table>
.

MessageId = 621
SymbolicName = SN_EV_CONFIG_RULE_PKG_DOWNLOAD_FAILED
Language=English
Rule package "%1" posted by admin on %2 could not be downloaded.
Package server: (%3,%4).
Package path: %5.
.
MessageId = 622
SymbolicName = SN_EV_CONFIG_RULE_PKG_DOWNLOAD_FAILED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Rule package "%1" posted by admin on %2 could not be downloaded.<br>
			Package server: (%3,%4).<br>
			Package path: %5.<br>
		</td>
	</tr>
</table>
.

MessageId = 623
SymbolicName = SN_EV_CONFIG_RULE_PKG_VALIDATION_FAILED
Language=English
Rule package "%1" posted by admin on %2 failed validation.
Package server: (%3,%4).
Package path: %5.
.
MessageId = 624
SymbolicName = SN_EV_CONFIG_RULE_PKG_VALIDATION_FAILED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Rule package "%1" posted by admin on %2 failed validation.<br>
			Package server: (%3,%4).<br>
			Package path: %5.<br>
		</td>
	</tr>
</table>
.

MessageId = 625
SymbolicName = SN_EV_CONFIG_RULE_PKG_INSTALL_FAILED
Language=English
Rule package "%1" posted by admin on %2 failed to install.
Package server: (%3,%4).
Package path: %5.
.
MessageId = 626
SymbolicName = SN_EV_CONFIG_RULE_PKG_INSTALL_FAILED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Rule package "%1" posted by admin on %2 failed to install.<br>
			Package server: (%3,%4).<br>
			Package path: %5.<br>
		</td>
	</tr>
</table>
.

MessageId = 627
SymbolicName = SN_EV_CONFIG_RULE_PKG_FIREWALL_REFRESH_FAILED
Language=English
Internet Worm Protection failed to refresh after installing rule package "%1" posted by admin on %2.
Package server: (%3,%4).
Package path: %5.
.
MessageId = 628
SymbolicName = SN_EV_CONFIG_RULE_PKG_FIREWALL_REFRESH_FAILED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Internet Worm Protection failed to refresh after installing rule package "%1" posted by admin on %2.<br>
			Package server: (%3,%4).<br>
			Package path: %5.<br>
		</td>
	</tr>
</table>
.

MessageId = 629
SymbolicName = SN_EV_CONFIG_APP_UPDATED
Language=English
Application updated from version %1 to version %2.
.
MessageId = 630
SymbolicName = SN_EV_CONFIG_APP_UPDATED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Application updated from version %1 to version %2.
		</td>
	</tr>
</table>
.

MessageId = 631
SymbolicName = SN_EV_CONFIG_CHANGE_FIREWALL
Language=English
Internet Worm Protection setting "%1" changed.
Old Value: %2.
New Value: %3.
.
MessageId = 632
SymbolicName = SN_EV_CONFIG_CHANGE_FIREWALL_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Internet Worm Protection setting "%1" changed.<br>
			Old Value: %2.<br>
			New Value: %3.<br>
		</td>
	</tr>
</table>
.

MessageId = 633
SymbolicName = SN_EV_CONFIG_CHANGE_ADD_RESTRICTED
Language=English
Addresses added to the block list.
Address count: %1.
.
MessageId = 634
SymbolicName = SN_EV_CONFIG_CHANGE_ADD_RESTRICTED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Addresses added to the block list.<br>
			Address count: %1.<br>
		</td>
	</tr>
</table>
.

MessageId = 635
SymbolicName = SN_EV_CONFIG_CHANGE_REMOVE_RESTRICTED
Language=English
Addresses removed from block list.
First address: %1.
.
MessageId = 636
SymbolicName = SN_EV_CONFIG_CHANGE_REMOVE_RESTRICTED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Addresses removed from block list.<br>
			First address: %1.<br>
		</td>
	</tr>
</table>
.

MessageId = 637
SymbolicName = SN_EV_CONFIG_CHANGE_USER_LOGGED_IN
Language=English
User logged in.
.
MessageId = 638
SymbolicName = SN_EV_CONFIG_CHANGE_USER_LOGGED_IN_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			User logged in.
		</td>
	</tr>
</table>
.

MessageId = 639
SymbolicName = SN_EV_CONFIG_CHANGE_USER_LOGGED_OUT
Language=English
No user is logged in.
.
MessageId = 640
SymbolicName = SN_EV_CONFIG_CHANGE_USER_LOGGED_OUT_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			No user is logged in.
		</td>
	</tr>
</table>
.

MessageId = 641
SymbolicName = SN_EV_CONFIG_CHANGE_APP_CATEGORY_CHANGE
Language=English
Application categories have changed.
.
MessageId = 642
SymbolicName = SN_EV_CONFIG_CHANGE_APP_CATEGORY_CHANGE_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Application categories have changed.
		</td>
	</tr>
</table>
.

MessageId = 643
SymbolicName = SN_EV_CONFIG_CHANGE_STATISTICS_CLEARED
Language=English
Statistics cleared
Some (or all) statistics categories were cleared.
.
MessageId = 644
SymbolicName = SN_EV_CONFIG_CHANGE_STATISTICS_CLEARED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Statistics cleared<br>
			Some (or all) statistics categories were cleared.<br>
		</td>
	</tr>
</table>
.

MessageId = 645
SymbolicName = SN_EV_CONFIG_CHANGE_IDS
Language=English
IDS setting "%1" changed.
Old Value: %2.
New Value: %3.
.
MessageId = 646
SymbolicName = SN_EV_CONFIG_CHANGE_IDS_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			IDS setting "%1" changed.<br>
			Old Value: %2.<br>
			New Value: %3.<br>
		</td>
	</tr>
</table>
.

MessageId = 647
SymbolicName = SN_EV_CONFIG_CHANGE_NDIS
Language=English
NDIS setting "%1" changed.
Old Value: %2.
New Value: %3.
.
MessageId = 648
SymbolicName = SN_EV_CONFIG_CHANGE_NDIS_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			NDIS setting "%1" changed.<br>
			Old Value: %2.<br>
			New Value: %3.<br>
		</td>
	</tr>
</table>
.

MessageId = 649
SymbolicName = SN_EV_CONFIG_CHANGE_TDI
Language=English
TDI setting "%1" changed.
Old Value: %2.
New Value: %3.
.
MessageId = 650
SymbolicName = SN_EV_CONFIG_CHANGE_TDI_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			TDI setting "%1" changed.<br>
			Old Value: %2.<br>
			New Value: %3.<br>
		</td>
	</tr>
</table>
.
MessageId = 683
SymbolicName = SN_EV_IP_STORAGE_PATH_CORRUPTED
Language=English
The configuration can not be loaded or saved. Please reinstall.
.
MessageId = 684
SymbolicName = SN_EV_IP_STORAGE_PATH_CORRUPTED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			The configuration can not be loaded or saved. Please reinstall.
		</td>
	</tr>
</table>
.
MessageId = 685
SymbolicName = SN_EV_TDI_INITIALIZATION_FAILED
Language=English
Driver Verification Failed. Please reinstall.
.
MessageId = 686
SymbolicName = SN_EV_TDI_INITIALIZATION_FAILED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Driver Verification Failed. Please reinstall.
		</td>
	</tr>
</table>
.
MessageId = 687
SymbolicName = SN_EV_IP_FILTER_ENABLED
Language=English
Internet Worm Protection has been enabled.
.
MessageId = 688
SymbolicName = SN_EV_IP_FILTER_ENABLED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Internet Worm Protection has been enabled.
		</td>
	</tr>
</table>
.
MessageId = 689
SymbolicName = SN_EV_IP_FILTER_DISABLED
Language=English
Internet Worm Protection has been disabled.
.
MessageId = 690
SymbolicName = SN_EV_IP_FILTER_DISABLED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Internet Worm Protection has been disabled.
		</td>
	</tr>
</table>
.
MessageId = 691
SymbolicName = SN_EV_TDI_STARTUP_MODE_NETWORK
Language=English
Startup Mode has been set to Network.
.
MessageId = 692
SymbolicName = SN_EV_TDI_STARTUP_MODE_NETWORK_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Startup Mode has been set to Network.
		</td>
	</tr>
</table>
.
MessageId = 693
SymbolicName = SN_EV_TDI_STARTUP_MODE_AUTO
Language=English
Startup Mode has been set to Automatic.
.
MessageId = 694
SymbolicName = SN_EV_TDI_STARTUP_MODE_AUTO_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Startup Mode has been set to Automatic.
		</td>
	</tr>
</table>
.
MessageId = 695
SymbolicName = SN_EV_TDI_STARTUP_MODE_MANUAL
Language=English
Startup Mode has been set to Manual.
.
MessageId = 696
SymbolicName = SN_EV_TDI_STARTUP_MODE_MANUAL_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Startup Mode has been set to Manual.
		</td>
	</tr>
</table>
.
MessageId = 697
SymbolicName = SN_EV_TDI_INTERNET_SECURITY_ON
Language=English
Internet Worm Protection has been turned on.
.
MessageId = 698
SymbolicName = SN_EV_TDI_INTERNET_SECURITY_ON_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Internet Worm Protection has been turned on.
		</td>
	</tr>
</table>
.
MessageId = 699
SymbolicName = SN_EV_TDI_INTERNET_SECURITY_OFF
Language=English
Internet Worm Protection has been turned off.
.
MessageId = 700
SymbolicName = SN_EV_TDI_INTERNET_SECURITY_OFF_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Internet Worm Protection has been turned off.
		</td>
	</tr>
</table>
.
MessageId = 701
SymbolicName = SN_EV_IP_FILTER_RULE_CREATED_ON_LISTEN
Language=English
%1 "%2" communications.
Local address is %3(%4).
Process name is "%5".
.
MessageId = 702
SymbolicName = SN_EV_IP_FILTER_RULE_CREATED_ON_LISTEN_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			%1 "%2" communications.<br>
			Local address is %3(%4).<br>
			Process name is "%5".<br>
		</td>
	</tr>
</table>
.
MessageId = 703
SymbolicName = SN_EV_IP_FILTER_RULE_CREATED_ON_LISTEN_FOR_ANY_ADAPTER
Language=English
%1 "%2" communications.
Local address: All local network adapters(%4).
Process name is "%5".
.
MessageId = 704
SymbolicName = SN_EV_IP_FILTER_RULE_CREATED_ON_LISTEN_FOR_ANY_ADAPTER_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			%1 "%2" communications.<br>
			Local address: All local network adapters(%4).<br>
			Process name is "%5".<br>
		</td>
	</tr>
</table>
.
MessageId = 705
SymbolicName = SN_EV_ATRACK_RULE_MATCHED_LISTEN_ANY_ADAPTER
Language=English
Rule %1 matched
Local address: All local network adapters(%3).
.
MessageId = 706
SymbolicName = SN_EV_ATRACK_RULE_MATCHED_LISTEN_ANY_ADAPTER_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Rule %1 matched<br>
			Local address: All local network adapters(%3).<br>
		</td>
	</tr>
</table>
.
MessageId = 33473
SymbolicName = SN_EV_ATRACK_RULE_MATCHED_LISTEN_ANY_ADAPTER_SHORT
Language=English
Rule %1 matched.
.
MessageId = 707
SymbolicName = SN_EV_IP_FILTER_ALLOW_RULE_PRESENT_FOR_LISTEN_ANY_ADAPTER
Language=English
Rule "%1" permitted communication.
Local address: All local network adapters(%3).
Process name: "%4".
.
MessageId = 708
SymbolicName = SN_EV_IP_FILTER_ALLOW_RULE_PRESENT_FOR_LISTEN_ANY_ADAPTER_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Rule "%1" permitted communication.<br>
			Local address: All local network adapters(%3).<br>
			Process name is "%4".<br>
		</td>
	</tr>
</table>
.

MessageId = 709
SymbolicName = SN_EV_IP_FILTER_REJECT_RULE_PRESENT_FOR_LISTEN_ANY_ADAPTER
Language=English
Rule "%1" blocked communication.
Local address: All local network adapters(%3).
Process name is "%4".
.
MessageId = 710
SymbolicName = SN_EV_IP_FILTER_REJECT_RULE_PRESENT_FOR_LISTEN_ANY_ADAPTER_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Rule "%1" blocked communication.<br>
			Local address: All local network adapters(%3).<br>
			Process name is "%4".<br>
		</td>
	</tr>
</table>
.

MessageId = 711
SymbolicName = SN_EV_IP_FILTER_IGNORE_RULE_PRESENT_FOR_LISTEN_ANY_ADAPTER
Language=English
Rule "%1" ignored communication.
Local address: All local network adapters(%3).
Process name is "%4".
.
MessageId = 712
SymbolicName = SN_EV_IP_FILTER_IGNORE_RULE_PRESENT_FOR_LISTEN_ANY_ADAPTER_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Rule "%1" ignored communication.<br>
			Local address: All local network adapters(%3).<br>
			Process name is "%4".<br>
		</td>
	</tr>
</table>
.

MessageId = 714
SymbolicName = SN_EV_IAMSERV_IP_ADDRESS_GONE
Language=English
IP address %1 has disappeared and is no longer being protected.
.
MessageId = 715
SymbolicName = SN_EV_IAMSERV_IP_ADDRESS_GONE_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			IP address %1 has disappeared and is no longer being protected.
		</td>
	</tr>
</table>
.
MessageId = 716
SymbolicName = SN_EV_IAMSERV_IP_ADDRESS_GONE_SHORT
Language=English
IP address %1 has disappeared.
.


MessageId = 717
SymbolicName = SN_EV_IP_FILTER_BLACKED_OUT
Language=English
%1 is blocked for Internet access at this time.
Category: %2.
Session: %3.
.
MessageId = 718
SymbolicName = SN_EV_IP_FILTER_BLACKED_OUT_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
		%1 is blocked for Internet access at this time.
		Category: %2.
		Session: %3.
		</td>
	</tr>
</table>
.
MessageId = 719
SymbolicName = SN_EV_IP_FILTER_BLACKED_OUT_SHORT
Language=English
%1 is blocked for Internet access at this time.
.


MessageId = 720
SymbolicName = SN_EV_IP_FILTER_TIME_LIMIT_REACHED
Language=English
You have reached the maximum time allotted to use %1.
Category: %2.
Session: %3.
.
MessageId = 721
SymbolicName = SN_EV_IP_FILTER_TIME_LIMIT_REACHED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
		You have reached the maximum time allotted to use %1.
		Category: %2.
		Session: %3.
		</td>
	</tr>
</table>
.
MessageId = 722
SymbolicName = SN_EV_IP_FILTER_TIME_LIMIT_REACHED_SHORT
Language=English
%1 is blocked for Internet access at this time.
.
MessageId = 723
SymbolicName = SN_EV_IP_FILTER_BACKUP_RULES_RESTORED
Language=English
Missing or damaged Internet Worm Protection rules. Backup rules restored.
.
MessageId = 724
SymbolicName = SN_EV_IP_FILTER_BACKUP_RULES_RESTORED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Missing or damaged Internet Worm Protection rules. Backup rules restored.
		</td>
	</tr>
</table>
.
MessageId = 33491
SymbolicName = SN_EV_IP_FILTER_BACKUP_RULES_RESTORED_SHORT
Language=English
Backup rules restored.
.
MessageId = 725
SymbolicName = SN_EV_IP_FILTER_FAILED_TO_LOAD_RULES
Language=English
Missing or damaged Internet Worm Protection rules. Unable to load backup or default rules.
The Internet Worm Protection will be disabled.
.
MessageId = 726
SymbolicName = SN_EV_IP_FILTER_FAILED_TO_LOAD_RULES_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Missing or damaged Internet Worm Protection rules. Unable to load backup or default rules.
			The Internet Worm Protection will be disabled.
		</td>
	</tr>
</table>
.
MessageId = 33493
SymbolicName = SN_EV_IP_FILTER_FAILED_TO_LOAD_RULES_SHORT
Language=English
Load Rules Failed. Internet Worm Protection Disabled.
.
MessageId = 727
SymbolicName = SN_EV_IP_FILTER_PORTBLOCK_BLOCKED
Language=English
Port Blocking blocked %1(%2).
.
MessageId = 728
SymbolicName = SN_EV_IP_FILTER_PORTBLOCK_BLOCKED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Port Blocking blocked %1(%2).
		</td>
	</tr>
</table>
.
MessageId = 33494
SymbolicName = SN_EV_IP_FILTER_PORTBLOCK_BLOCKED_SHORT
Language=English
Port Blocking blocked %1(%2).
.
MessageId = 729
SymbolicName = SN_EV_IP_FILTER_PORTBLOCK_ALLOWED
Language=English
Port Blocking allowed %1(%2).
.
MessageId = 730
SymbolicName = SN_EV_IP_FILTER_PORTBLOCK_ALLOWED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Port Blocking allowed %1(%2).
		</td>
	</tr>
</table>
.
MessageId = 33495
SymbolicName = SN_EV_IP_FILTER_PORTBLOCK_ALLOWED_SHORT
Language=English
Port Blocking allowed %1(%2).
.

MessageID = 731
SymbolicName = SN_EV_IP_FILTER_SYMC_APP_ALLOWED
Language=English
The following Symantec application was implicitly allowed to communicate:
			Application: %1
			Protocol: %2
			Local IP address,port: %3,%4
			Remote IP address,port: %5,%6
.
MessageID = 732
SymbolicName = SN_EV_IP_FILTER_SYMC_APP_ALLOWED_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			The following Symantec application was implicitly allowed to communicate:<br>
			Application: %1<br>
			Protocol: %2<br>
			Local IP address,port: %3,%4<br>
			Remote IP address,port: %5,%6<br>
		</td>
	</tr>
</table>
.
MessageID = 733
SymbolicName = SN_EV_IP_FILTER_SYMC_APP_ALLOWED_LISTEN
Language=English
The following Symantec application was implicitly allowed to listen:
			Application: %1
			Local IP address,port: %3,%4
.
MessageID = 734
SymbolicName = SN_EV_IP_FILTER_SYMC_APP_ALLOWED_LISTEN_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			The following Symantec application was implicitly allowed to listen:<br>
			Application: %1<br>
			Local IP address,port: %3,%4<br>
		</td>
	</tr>
</table>
.
MessageID = 735
SymbolicName = SN_EV_IP_FILTER_SYMC_APP_ALLOWED_ACCEPT
Language=English
The following Symantec application was implicitly allowed to accept an incoming TCP connection:
			Application: %1
			Local IP address,port: %2,%3
			Remote IP address,port: %4,%5
.
MessageID = 736
SymbolicName = SN_EV_IP_FILTER_SYMC_APP_ALLOWED_ACCEPT_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			The following Symantec application was implicitly allowed to accept an incoming TCP connection:<br>
			Application: %1<br>
			Local IP address,port: %2,%3<br>
			Remote IP address,port: %4,%5<br>
		</td>
	</tr>
</table>
.
MessageID = 737
SymbolicName = SN_EV_IP_FILTER_SYMC_APP_ALLOWED_CONNECT
Language=English
The following Symantec application was implicitly allowed outbound TCP connection:
			Application: %1
			Local IP address,port: %2,%3
			Remote IP address,port: %4,%5
.
MessageID = 738
SymbolicName = SN_EV_IP_FILTER_SYMC_APP_ALLOWED_CONNECT_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			The following Symantec application was implicitly allowed outbound TCP connection:<br>
			Application: %1<br>
			Local IP address,port: %2,%3<br>
			Remote IP address,port: %4,%5<br>
		</td>
	</tr>
</table>
.
MessageId = 739
SymbolicName = SN_EV_IDS_SETTINGS_FILE_CORRUPT
Language=English
The Internet Worm Protection configuration is corrupt. Default settings have been restored.
.
MessageId = 740
SymbolicName = SN_EV_IDS_SETTINGS_FILE_CORRUPT_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			The Internet Worm Protection configuration is corrupt. Default settings have been restored.
		</td>
	</tr>
</table>
.

MessageId = 741
SymbolicName = SN_EV_IDS_DEFS_ENGINE_VERSION_INFO
Language=English
Internet Worm Protection Signature File Version: %1. Internet Worm Protection Engine Version: %2.
.
MessageId = 742
SymbolicName = SN_EV_IDS_DEFS_ENGINE_VERSION_INFO_HTML
Language=English
<script src="%%s" language="JavaScript"></script>
<table class="NisEvtEventTable">
	<tr valign=top>
		<td nowrap class="NisEvtEventProperty">Details:</td>
		<td class="NisEvtEventValue">
			Internet Worm Protection Signature File Version: %1.<br>
			Internet Worm Protection Engine Version: %2.<br>
		</td>
	</tr>
</table>
.

;//---------------------------------------------------------------------------
;// Next Message ID:  755
;//---------------------------------------------------------------------------
