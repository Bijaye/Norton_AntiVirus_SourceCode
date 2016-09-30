/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/


#ifndef _MACROS_H_
#define _MACROS_H_

//////////////////////////////////////////////////////////////////////

// 
// Indexes for columns
// 
#define RESULT_COL_FILENAME         0	// QServer and Blue Ice
#define RESULT_COL_USERNAME         1	// QServer and Blue Ice
#define RESULT_COL_MACHINENAME      2	// QServer and Blue Ice
#define RESULT_COL_DOMAINNAME       3   // QServer only
#define RESULT_COL_RECIEVED         4	// QServer only
#define RESULT_COL_SUBMITTED        5	// QServer only
#define RESULT_COL_SUBMIT_BY        6	// QServer only
#define RESULT_COL_STATUS           7	// QServer and Blue Ice
#define RESULT_COL_VIRUS_NAME       8	// QServer and Blue Ice

// Indexes for Blue Ice columns
#define RESULT_COL_COMPLEATE		3
#define RESULT_COL_AGE				4
#define RESULT_COL_STATE			5
#define RESULT_COL_SEQ_NEEDED		6
#define RESULT_COL_ERROR			9



#define MAX_RESULT_COLUMNS          9
#define MAX_BI_RESULT_COLUMNS       10
#define MAX_COLUMNS					10

// 
// Update All View hints
// 
#define VIEW_HINT_REFRESH_BEGIN			0x00000001
#define VIEW_HINT_REFRESH_END			0x00000002
#define VIEW_HINT_DELETE_ITEM			0x00000003
#define VIEW_HINT_REFRESH_ITEM			0x00000004   
#define VIEW_HINT_SERVER_ATTACH_BEGIN   0x00000005   
#define VIEW_HINT_SERVER_ATTACH_FINISH	0x00000006   
#define VIEW_HINT_FULL_REFRESH_BEGIN	0x00000007
#define VIEW_HINT_FULL_REFRESH_END		0x00000008

// 
// Name of the quarantine server service.
// 
#define QUARANTINE_SERVICE_NAME				_T("Qserver")
#define QCONSOLE_HELPFILE					_T("AvisConsole.CHM")
#define QCONSOLE_HELPTOPIC_GEN_PROP			_T("General_Properties.htm")
#define QCONSOLE_HELPTOPIC_SAMPLE_PROP		_T("Sample_General_Properties.htm")
#define QCONSOLE_HELPTOPIC_SAMP_USER_PROP	_T("Sample_User_info_Properties.htm")
#define QCONSOLE_HELPTOPIC_SAMP_VIRUS_PROP	_T("Sample_Virus_info_Properties.htm")

// 
// AP manipulation routines.
// 
void EnableAP();
void DisableAP();


//////////////////////////////////////////////////////////////////////

#endif