/////////////////////////////////////////////////////////////////////
// AT_OPTS.H - Created - 10/21/96
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/INCLUDE/VCS/at_opts.h_v   1.3   16 Jul 1997 21:59:10   KSACKIN  $
//
// Description:
//    This file will declare the class CAlertOptions and define the
//    functions needed.  This class will be responsible for loading,
//    saving, and iterating through options for the NAV Alerter.
//
//    The way you would use this class would be to declare an
//    instance of this class, and then call the LoadOptions member
//    function with the file name of your alert options.
//
//    When this is done, you will be able to iterate through the
//    events by making subsequent calls to GetNextEvent();
//
//
//           ÚÄÄÄÄÄCAlertOptionsÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
//           ³                                                     ³
//           ³  ÚÄ CAlertTargetListÄÄÄÄÄÄÄÄÄÄÄÄ¿   LoadOptions     ³
//           ³  ³                              ³   AddTarget       ³
//           ³  ³ÚÄÄ CObListÄÄÄÄÄÄÄÄÄ¿         ³   RemoveTarget    ³
//           ³  ³³                   ³ Reset   ³                   ³
//           ³  ³³ ÚÄCAlertTargetÄ¿  ³ GetNext ³                   ³
//           ³  ³³ ³      CObjects³  ³         ³   ResetIterator   ³
//           ³  ³³ ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ  ³         ³   GetNextTarget   ³
//           ³  ³³                 ³         ³                   ³
//           ³  ³³ ÚÄCAlertTargetÄ¿  ³         ³                   ³
//           ³  ³³ ³              ³  ³         ³                   ³
//           ³  ³³ ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ  ³         ³                   ³
//           ³  ³³                 ³         ³                   ³
//           ³  ³³      NULL         ³         ³                   ³
//           ³  ³ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ         ³                   ³
//           ³  ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ                   ³
//           ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
//
//    Ex.
//
//    CAlertOptions     cAlertOptions;
//    CAlertTarget *    lpcAlertTarget;
//
//        cAlertOtions.LoadOptions( "Options.txt" );
//
//        while ( lpcAlertTarget = cAlertOptions.GetNextTarget( ID_ALL ) )
//            {
//            DoSomethingWithTarget( lpcAlertTarget );
//            }
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/INCLUDE/VCS/at_opts.h_v  $
// 
//    Rev 1.3   16 Jul 1997 21:59:10   KSACKIN
// Updated our ProfileString saving options functions to accept a CStdioFile
// to write the options to.  We are now going to write all options directly to
// the file instead of mixing WritePrivateProfile strings and writing to the
// file as well.
//
//    Rev 1.2   22 May 1997 14:26:44   DSACKIN
// Converted back to TCHAR.
//
//    Rev 1.1   07 May 1997 16:21:10   GDZIECI
//
//
//    Rev 1.1   06 May 1997 18:05:14   GDZIECI
// Added members and access functions to support options to receive ATP, IPX
// and IP based alerts.
//
//    Rev 1.0   18 Apr 1997 13:57:32   DSACKIN
// Ported from Tesla
//
//    Rev 1.0   13 Mar 1997 15:11:56   RFULLER
// Initial revision
//
//    Rev 1.8   16 Jan 1997 00:23:44   ksackin
// Added a RemoveNextTarget() function to compliment the GetNextTarget()
// function.  This new function will actually remove the CAlertTarget from the
// list so that you can be responsable for deleting that object, and not relying
// on the CAlertOptions class to do that for you.  This will allow you to add
// this object to another object list if you so desire.
//
//    Rev 1.7   07 Jan 1997 13:45:32   ksackin
// added two new member variables with access functions to the CAlertOptions
// class.
//
//    Rev 1.6   05 Dec 1996 01:50:12   DSACKIN
// Moved the SMTP functionality from inline functions to at_smtp.cpp
//
//    Rev 1.5   04 Dec 1996 23:13:54   DSACKIN
// Changed all instances of LPCTSTR to LPCWSTR for use with non-UNICODE apps.
//
//    Rev 1.4   03 Dec 1996 14:34:52   ksackin
// Added member function for setting and getting the AP and Scanner Remote Alert
// Enabled BOOL member variables.  Also added the variables.
//
//    Rev 1.3   25 Nov 1996 21:53:54   ksackin
// Added a flag of m_bRemoteAlertsEnabled to disable all remote alerts.
//
//    Rev 1.2   23 Nov 1996 21:41:14   ksackin
// Project got confused and started to think that this was a local include
// instead of a global include.  Very strange...  Deleted file from project and
// added it back to the global include area.
//
//    Rev 1.4   23 Nov 1996 21:23:12   ksackin
// Added a function prototype for FixupINIFileName()
//
//    Rev 1.3   23 Nov 1996 20:44:32   ksackin
// Changed declaration of the m_szOptionsFileName to a CString.
//
//    Rev 1.2   23 Nov 1996 19:52:18   ksackin
// Added functionality to save and load SMTP Configuration options.  For now,
// the implementation of this is a member string that gets saved and loaded
// when LoadOptions() or SaveOptions() is called.  Also added access functions
// to get and set this configuration.
//
//    Rev 1.1   15 Nov 1996 02:33:50   KSACKIN
// Added a ResetPosition() member function to the CAlertOptions class for
// resetting the position of the current iterator, as well as added
// functionality for loading the default file name for the options file based
// upon the directory that your module resides in.  This was previously broken
// because it simply tried to load the file name, and we were in another
// directory.
//
//    Rev 1.0   14 Nov 1996 01:21:34   KSACKIN
// Initial revision.
//
//    Rev 1.1   12 Nov 1996 22:51:40   KSACKIN
// Changed class definitions to use the MFC_EXT_CLASS macro, which will export
// all functions in that class.  Also converted to an MFC Extension DLL.
//
//    Rev 1.0   08 Nov 1996 11:36:04   DKEEFER
// Initial revision
//
//    Rev 1.2   07 Nov 1996 16:44:54   KSACKIN
// Changed an include of Windows.h to Afx.h
//
//    Rev 1.1   07 Nov 1996 01:05:38   KSACKIN
// Fixed some exported functions, as well as ensured UNICODE support.  Also
// removed a dependancy of the file alertstr.h.  This file should only be
// included by those modules in this project that need the string resources.
// This should not be required to be in the global include area.
//
//    Rev 1.0   29 Oct 1996 16:40:56   KSACKIN
// Initial revision.
//
//    Rev 1.0   29 Oct 1996 15:37:16   KSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#if !defined(__ALERTOPS_H)
#define __ALERTOPS_H


#include <afx.h>
#include <tchar.h>
#include "at_tgt.h"


/////////////////////////////////////////////////////////////////////
// External global variables.

extern HINSTANCE ghInst;

//extern AFX_EXTENSION_MODULE ThisExtensionModule;

/////////////////////////////////////////////////////////////////////
// Exported Function Prototypes

                                        // Function to create a new target
                                        // object of a specific type and
                                        // return a base pointer to that
                                        // object. See targets.h for types.
AFX_API_EXPORT CAlertTarget *CreateNewTargetOfType(
    int nTargetType
    );

/////////////////////////////////////////////////////////////////////
// Internal Function Prototypes
                                        // Function to retrieve the section
                                        // name for a particular type of target.
BOOL GetSectionNameForTarget(
    int nTargetType,
    CString *lpcszSectionName
    );
                                        // Function to add braces to a section
                                        // name if needed
void AddBracesIfNeeded(
    CString *lpcszSectionName
    );
                                        // Takes a file and seeks to the
                                        // beginning of the section name passed.
BOOL SeekToSection(
    CStdioFile *lpcInputFile,
    LPCTSTR szSectionToFind
    );

                                        // Save an option to our file with a
                                        // specific ID.
BOOL SaveOptionWithID(
    UINT        nKeyNameID,
    LPCTSTR     cszParameterToSave,
    CStdioFile *lpcOptionsFile
    );

                                        // Load an option from our file with a
                                        // specific Keyname ID.
BOOL LoadOptionWithID(
    LPCTSTR  cszSectionName,
    UINT     nKeyNameID,
    LPCTSTR  cszDefaultValue,
    CString *lpcReturnString,
    LPCTSTR  cszFileName
    );

                                        // Save a BOOLEAN option to our file
                                        // with a specific ID.
BOOL SaveBOOLOptionWithID(
    UINT        nKeyNameID,
    BOOL        bValueToSave,
    CStdioFile *lpcOptionsFile
    );

                                        // Load a BOOLEAN option from our file
                                        // with a specific Keyname ID.
BOOL LoadBOOLOptionWithID(
    LPCTSTR  cszSectionName,
    UINT     nKeyNameID,
    BOOL     bDefaultValue,
    BOOL    *lpbReturnVal,
    LPCTSTR  cszFileName
    );

                                        // Function to fixup an INI File name
                                        // if it is a relative path.  This will
                                        // tack on a '.\' in front of the file
                                        // name.
void FixupINIFileName(
    CString *lpcFileNameToFixup
    );


/////////////////////////////////////////////////////////////////////
//  CAlertOptions class

class AFX_EXT_CLASS CAlertOptions
{
public:
                                        // Constructor and destructor.
    CAlertOptions( LPCTSTR cszFileName = NULL );
    ~CAlertOptions( void );

                                        // This function will open the file,
                                        // and then make subsequent calls to
                                        // LoadSection to load each target
                                        // section from the file.
    int LoadOptions( LPCTSTR cszFileName = NULL );

                                        // This function will open the file,
                                        // and then make subsequent calls to
                                        // LoadSection to load each target
                                        // section from the file.
    int SaveOptions( LPCTSTR cszFileName = NULL );

                                        // This function will be used to
                                        // iterate through the current list of
                                        // events. (Defaults to all target types)
    CAlertTarget * GetNextTarget( int nEventType = TARGET_ALL );

                                        // This function will be used to
                                        // iterate through the current list of
                                        // events. This function will physically
                                        // remove the target from the list so
                                        // that the memory will not be deleted
                                        // when the options class destructs.
    CAlertTarget * RemoveNextTarget( int nEventType = TARGET_ALL );

                                        // Resets the current iteration
                                        // position for this list of targets.
                                        // This will only need to be called if
                                        // you stop iterating in the middle of
                                        // the list.  If you complete every
                                        // iteration to the end, the list will
                                        // automatically reset itself.
    void  ResetPosition( void );

                                        // Sets the AP remote alert options
                                        // enabled state.
    void  SetAPRemoteAlertsEnabled( BOOL bAPRemoteAlertsEnabled ) {m_bAPRemoteAlertsEnabled = bAPRemoteAlertsEnabled;};

                                        // Gets the AP remote alert options
                                        // enabled state.
    BOOL GetAPRemoteAlertsEnabled( void ) {return m_bAPRemoteAlertsEnabled;};

                                        // Sets the Scanner remote alert options
                                        // enabled state.
    void  SetScannerRemoteAlertsEnabled( BOOL bScannerRemoteAlertsEnabled ) {m_bScannerRemoteAlertsEnabled = bScannerRemoteAlertsEnabled;};

                                        // Gets the Scanner remote alert options
                                        // enabled state.
    BOOL GetScannerRemoteAlertsEnabled( void ) {return m_bScannerRemoteAlertsEnabled;};

                                        // Return a constant string to the SMTP
                                        // Post Office.
    LPCTSTR GetSMTPPostOffice( void );
                                        // Return a constant string to the SMTP
                                        // Author string.
    LPCTSTR GetSMTPAuthor( void );
                                        // Return a constant string to the SMTP
                                        // Subject string.
    LPCTSTR GetSMTPSubject( void );

                                        // Function to set the current SMTP
                                        // Post Office string.
    void SetSMTPPostOffice( LPCTSTR lpcszNewPostOffice );

                                        // Function to set the current SMTP
                                        // Author string.
    void SetSMTPAuthor( LPCTSTR lpcszNewAuthor );

                                        // Function to set the current SMTP
                                        // Subject string.
    void SetSMTPSubject( LPCTSTR lpcszNewSubject );

                                        // Sets the Listen for ATP Alerts option
                                        // state.
    void  SetATPAlertsEnabled( BOOL bATPAlertsEnabled ) {m_bATPAlertsEnabled = bATPAlertsEnabled;};

                                        // Gets the Listen for ATP Alerts option
                                        // state.
    BOOL GetATPAlertsEnabled( void ) {return m_bATPAlertsEnabled;};

                                        // Sets the Listen for IPX Alerts option
                                        // state.
    void  SetIPXAlertsEnabled( BOOL bIPXAlertsEnabled ) {m_bIPXAlertsEnabled = bIPXAlertsEnabled;};

                                        // Gets the Listen for IPX Alerts option
                                        // state.
    BOOL GetIPXAlertsEnabled( void ) {return m_bIPXAlertsEnabled;};

                                        // Sets the Listen for IP Alerts option
                                        // state.
    void  SetIPAlertsEnabled( BOOL bIPAlertsEnabled ) {m_bIPAlertsEnabled = bIPAlertsEnabled;};

                                        // Gets the Listen for IP Alerts option
                                        // state.
    BOOL GetIPAlertsEnabled( void ) {return m_bIPAlertsEnabled;};

                                        // This function will return how many
                                        // of that target type are actually in
                                        // the list.  This function takes a
                                        // combination of target type flags for
                                        // multiple type queries.  Defaults to
                                        // all target types.
    DWORD GetTargetCount( int nTargetType = TARGET_ALL );

                                        // Adds a target to the
                                        // CAlertTargetList in sorted order
                                        // based on priority.
    BOOL  AddTarget( CAlertTarget *lpcEvent );

                                        // Removes a target from the list, and
                                        // deallocates the memory for that
                                        // object.
    BOOL  RemoveTarget( CAlertTarget *lpcTarget );

                                        // Destroys ALL entries in the
                                        // m_cAlertTargetList member valiable's
                                        // list. CAUTION! Deletes all items
                                        // from list.
    void DestroyAll( void );

protected:

                                        // This function is designed to load
                                        // a list of particular target types
                                        // from a file.
    BOOL LoadSection( CStdioFile *lpcInputFile, int nTargetType );

                                        // Save a section to our file of a
                                        // certain target type.
    BOOL SaveSection( CStdioFile *lpcOutputFile, int nTargetType );

                                        // This function should not need to be
                                        // called from clients of this class.
                                        // If you would like to load the
                                        // default file name, simple do not
                                        // specify a file name to the
                                        // LoadOptions Member funciton.
    BOOL GetDefaultOptionsFileName( CString *lpcszDefaultFileName );

                                        // Function to load the General
                                        // alert options.
    BOOL LoadGeneralOptions();
                                        // Function to save the General
                                        // alert options.
    BOOL SaveGeneralOptions( CStdioFile *lpcOptionsFile );
                                        // Function to load the SMTP
                                        // Configuration options.
    BOOL LoadSMTPConfig();
                                        // Function to save the SMTP
                                        // Configuration options.
    BOOL SaveSMTPConfig( CStdioFile *lpcOptionsFile );
                                        // Function to load the Receive
                                        // Alerts options.
    BOOL LoadReceiveOptions();
                                        // Function to save the Receive
                                        // Alerts options.
    BOOL SaveReceiveOptions( CStdioFile *lpcOptionsFile );

private:
                                        // AP Remote Alerts Enabled flag.
    BOOL                m_bAPRemoteAlertsEnabled;

                                        // Scanner Remote Alerts Enabled flag.
    BOOL                m_bScannerRemoteAlertsEnabled;

                                        // SMTP Options.
    CString             m_cszSMTPPostOffice;
    CString             m_cszSMTPAuthor;
    CString             m_cszSMTPSubject;

                                        // Receive Options
    BOOL                m_bATPAlertsEnabled;
    BOOL                m_bIPXAlertsEnabled;
    BOOL                m_bIPAlertsEnabled;

                                        // Options file name.
    CString             m_cszOptionsFileName;

                                        // List of alert event objects sorted
                                        // by priority.  This list will be
                                        // made of many different derivations
                                        // of an event, all based on a CObject.
                                        // The list type is derived from a
                                        // CObList class.
    CAlertTargetList    m_cAlertTargetList;
};


#endif // if !defined(__ALERTOPS_H)
