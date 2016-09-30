// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// ViewLog.h : header file
//
#if !defined (VIEW_LOG_INCLUDED)
#define VIEW_LOG_INCLUDED

#include "WizardPage.h"

//Define the Log Field values
#define LOG_FIELD_VTIME			0
#define LOG_FIELD_EVENT			1
#define LOG_FIELD_CATEGORY		2
#define LOG_FIELD_LOGGER		3
#define LOG_FIELD_COMPNAME		4
#define LOG_FIELD_USERNAME		5
#define LOG_FIELD_VIRUSNAME		6
#define LGO_FIELD_FILENAME		7
#define LOG_FIELD_FIRST_ACTION	8
#define LOG_FIELD_NEXT_ACTION	9
#define LOG_FIELD_REAL_ACTION	10
#define LOG_FIELD_VIRUS_TYPE	11

#define LOG_LINES				12

//Define the Events and Categories
#define GL_EVENT_IS_ALERT        1
#define GL_EVENT_SCAN_STOP       2
#define GL_EVENT_SCAN_START      3
#define GL_EVENT_PATTERN_UPDATE  4
#define GL_EVENT_INFECTION       5
#define GL_EVENT_FILE_NOT_OPEN   6
#define GL_EVENT_LOAD_PATTERN    7
//#define GL_STD_MESSAGE_INFO      8
//#define GL_STD_MESSAGE_ERROR     9
#define GL_EVENT_CHECKSUM        10
#define GL_EVENT_TRAP            11
#define GL_EVENT_CONFIG_CHANGE   12
#define GL_EVENT_SHUTDOWN        13
#define GL_EVENT_STARTUP         14
#define GL_EVENT_PATTERN_DOWNLOAD 16
#define GL_EVENT_TOO_MANY_VIRUSES 17

// TODO: MHB: Why are the above values which are already defined in vpcommon.h
// duplicated here? Since this class isn't actually included in the project I will
// follow suit but this needs to be looked at before including in the project.
#define GL_EVENT_SCAN_SUSPENDED       					65
#define GL_EVENT_SCAN_RESUMED       					66
#define GL_EVENT_SCAN_DURATION_INSUFFICIENT				67

#define GL_EVENT_MAX_EVENT_NUMBER 67

#define GL_CAT_INFECTION         1
#define GL_CAT_SUMMARY           2
#define GL_CAT_PATTERN           3
#define GL_CAT_SECURITY          4

//Define the actions
#define AC_BAD     10
#define AC_MOVE    1
#define AC_RENAME  2
#define AC_DEL     3
#define AC_NOTHING 4
#define AC_CLEAN   5


//Define the loggers
#define LOGGER_Scheduled          0
#define LOGGER_Manual             1
#define LOGGER_Real_Time          2
#define LOGGER_Integrity_Shield   3
#define LOGGER_Console            6
#define LOGGER_VPDOWN             7
#define LOGGER_System             8
#define LOGGER_Startup			  9
#define LOGGER_Idle				  10

#define LOGGER_LOCAL_END          100
#define LOGGER_Client             101
#define LOGGER_Forwarded          102


/////////////////////////////////////////////////////////////////////////////
// CViewLog dialog

class CViewLog : public CWizardPage
{
	DECLARE_DYNCREATE(CViewLog)
private:
	void OpenLogFile( const CString &strFilename );
	CString GetLogDir();

	CString ParseLogLine( CString &strSource );
	void GetEventString( int iValue, CString &strTemp );
	void GetCategoryString( int iValue, CString &strTemp );
	void GetLoggerString( int iValue, CString &strTemp );
	void GetActionString( int iValue, CString &strTemp );
	void GetVirusTypeString( int iValue, CString &strTemp );

// Construction
public:
	CViewLog();
	~CViewLog();

// Dialog Data
	//{{AFX_DATA(CViewLog)
	enum { IDD = IDD_VIEW_LOG };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CViewLog)
	public:
	virtual BOOL OnWizardFinish();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CViewLog)
	virtual BOOL OnInitDialog();
	afx_msg void OnOpen();
	afx_msg void OnSetfocusLog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

#endif
