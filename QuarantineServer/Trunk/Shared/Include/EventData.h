// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#ifndef _EVENTDATA_H
#define	_EVENTDATA_H
#if 0
    FillSampleAlertMsgData()

    Sample Alert Message Format:
    Message Title
    User
    Machine name
    Machine address
    Platform
    File Name
    Virus Name
    Reason
    Age of Sample
    Error message
    Default/help message




	    FillGeneralAlertMsgData()

    General Alert Message Format:
    Message Title
    Machine name
    Machine address
    Error message
    Default/help message
#endif

#define NO_SAMPLEEVENT_PARAMS	14
#define NO_GENERALEVENT_PARAMS	6
#define MAX_AMS_EVENTNAME		256
	typedef struct {
	TCHAR name[MAX_AMS_EVENTNAME];
//	DWORD nameID;
	BYTE type;
	BYTE bField;
	} QSAPARM;

typedef struct {
	TCHAR name[MAX_AMS_EVENTNAME];
//	DWORD nameID;
	int count;
	QSAPARM prams[NO_SAMPLEEVENT_PARAMS];
	} QSAALERT;

    BOOL StartAMSEx( void);
    void SendAlert( LPCTSTR szRemoteAMSMachine, LPCTSTR szProductName );
	DWORD UninstallAMS(void) ;

//    BOOL ConfigureAMS( LPCTSTR szRemoteAMSMachine, LPCTSTR szProductName );
//    BOOL ViewAMSLog( LPCTSTR szRemoteAMSMachine, LPCTSTR szProductName );
      //if( !GetRegistryValue( _T("System\\CurrentControlSet\\Control\\ComputerName\\ComputerName"), _T("ComputerName") , sValue ) )     
#endif