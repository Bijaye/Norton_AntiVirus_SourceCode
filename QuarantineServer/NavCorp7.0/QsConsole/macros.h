
#ifndef _MACROS_H_
#define _MACROS_H_

//////////////////////////////////////////////////////////////////////

// 
// Indexes for columns
// 
#define RESULT_COL_FILENAME         0
#define RESULT_COL_USERNAME         1
#define RESULT_COL_MACHINENAME      2
#define RESULT_COL_DOMAINNAME       3    
#define RESULT_COL_RECIEVED         4
#define RESULT_COL_SUBMITTED        5
#define RESULT_COL_SUBMIT_BY        6
#define RESULT_COL_STATUS           7
#define RESULT_COL_VIRUS_NAME       8

#define MAX_RESULT_COLUMNS          9


// 
// Update All View hints
// 
#define VIEW_HINT_REFRESH_BEGIN      0x00000001
#define VIEW_HINT_REFRESH_END        0x00000002
#define VIEW_HINT_DELETE_ITEM        0x00000003
#define VIEW_HINT_REFRESH_ITEM       0x00000004   

// 
// Name of the quarantine server service.
// 
#define QUARANTINE_SERVICE_NAME     _T("Qserver")
#define QCONSOLE_HELPFILE           _T("QSCON.CHM")


// 
// AP manipulation routines.
// 
void EnableAP();
void DisableAP();


//////////////////////////////////////////////////////////////////////

#endif