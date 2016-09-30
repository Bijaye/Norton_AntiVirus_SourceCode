/////////////////////////////////////////////////////////////////////
// TARGETS.H - Created - 10/23/96
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/INCLUDE/VCS/at_tgt.h_v   1.3   22 May 1997 14:27:02   DSACKIN  $
//
// Description:
//    Header file for all of the possible target types derived from
//    a base CAlertTarget class.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/INCLUDE/VCS/at_tgt.h_v  $
// 
//    Rev 1.3   22 May 1997 14:27:02   DSACKIN
// No change.
// 
//    Rev 1.2   14 May 1997 15:31:58   GDZIECI
// Minor changes in the CSNMPTarget class.
// 
//    Rev 1.1   09 May 1997 19:13:26   GDZIECI
// Added CSNMPTarget class for SNMP trap targets.
// 
//    Rev 1.0   24 Apr 1997 20:49:36   JBRENNA
// Initial revision.
// 
//    Rev 1.0   13 Mar 1997 15:10:42   RFULLER
// Initial revision
// 
//    Rev 1.7   18 Jan 1997 06:44:08   DSACKIN
// Added TARGET_SNMP
//
//    Rev 1.6   16 Jan 1997 00:25:00   ksackin
// Added a RemoveNextTarget() function to compliment the GetNextTarget()
// function.  This new function will actually remove the CAlertTarget from the
// list so that you can be responsable for deleting that object, and not relying
// on the CAlertOptions class to do that for you.  This will allow you to add
// this object to another object list if you so desire.
//
//    Rev 1.5   20 Dec 1996 17:51:52   ksackin
// Added a resource type variable to the CNetworkMessage target type.  This
// variable will determine how we are going to send messages to the Net
// Messenger API's.
//
//    Rev 1.4   12 Dec 1996 12:56:40   TSmith
// Added declaration for class CActivityLogTarget.
//
//    Rev 1.3   21 Nov 1996 16:31:58   ksackin
// Changed CAlertTarget base class to be able to save base class options before
// a call to the Pack functions of the derived classes.
//
//    Rev 1.2   18 Nov 1996 15:38:08   DSACKIN
// Added protocol selection to target forwarding.
//
//    Rev 1.1   12 Nov 1996 22:51:44   KSACKIN
// Changed class definitions to use the MFC_EXT_CLASS macro, which will export
// all functions in that class.  Also converted to an MFC Extension DLL.
//
//    Rev 1.0   08 Nov 1996 11:36:12   DKEEFER
// Initial revision
//
//    Rev 1.1   07 Nov 1996 01:07:00   KSACKIN
// Fixed up some export items with the correct export statements.  Also added
// some unicode support.
//
//    Rev 1.0   29 Oct 1996 16:41:16   KSACKIN
// Initial revision.
//
//    Rev 1.0   29 Oct 1996 15:37:12   KSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#if !defined(__TARGETS_H)
#define __TARGETS_H


#include <afx.h>
#include <tchar.h>
#include <winsock.h>
#include "AT_Count.h"

/////////////////////////////////////////////////////////////////////
//  #defines

                                        // Alert Target Types.
#define TARGET_EMAIL            0x00000001
#define TARGET_PAGER            0x00000002
#define TARGET_MESSAGE_LOCAL    0x00000004
#define TARGET_MESSAGE_NETWORK  0x00000008
#define TARGET_FORWARD          0x00000010
#define TARGET_NLM              0x00000020
#define TARGET_EVENTLOG         0x00000040
#define TARGET_ACTIVITY_LOG     0x00000080
#define TARGET_SNMP             0x00000100

                                        // All possible target types.
#define TARGET_ALL              0xffffffff



/////////////////////////////////////////////////////////////////////
// Forward Declarations.

class AFX_EXT_CLASS CAlertTarget;


/////////////////////////////////////////////////////////////////////
// Function Prototypes

                                        // Parameter Packing Functions.
BOOL PackParameter(
    CString *lpcPackedString,
    LPCTSTR cszParameterToAdd
    );

                                        // Parameter Packing Functions.
BOOL PackNumericParameter(
    CString *lpcPackedString,
    DWORD dwNumericParameterToAdd
    );

                                        // Extract a single parameter from the
                                        // from of a packed parameter list.
BOOL ExtractParameter(
    CString *lpcPackedString,
    CString *lpcParameter
    );

                                        // Extract a DWORD parameter.
BOOL ExtractDWORDParameter(
    CString *lpcPackedString,
    DWORD   *lpdwParameter
    );

/////////////////////////////////////////////////////////////////////
//  CAlertTargetList class   (derived from CObList)

class AFX_EXT_CLASS CAlertTargetList : public CObList
{
                                        // Allow for dynamic creation of objects
                                        // that can be identified with IsKindOf()
    DECLARE_DYNCREATE(CAlertTargetList)

public:
                                        // Constructor and Destructor.
    CAlertTargetList( void );
    ~CAlertTargetList( void );

                                        // Function to Add an Alert Target
    int AddTarget( CAlertTarget *lpcTarget );

                                        // This function will remove a target
                                        // that you have a handle to.  Perhaps
                                        // more remove functions will come with
                                        // better searching.  This function will
                                        // also delete the memory for this
                                        // object.
    int RemoveTarget( CAlertTarget *lpcTarget );

                                        // This function will return how many
                                        // of that target type are actually in
                                        // the list.  This function takes a
                                        // combination of target type flags for
                                        // multiple type queries.
    DWORD GetTargetCount( int nTargetType = TARGET_ALL );

                                        // Retrieve the next target from the
                                        // target list.  This will use the
                                        // m_rPosition variable as the iterator
                                        // position.
    CAlertTarget *GetNextTarget( int nEventType = TARGET_ALL );

                                        // Physically remove the next target of
                                        // the type you specify from the list.
                                        // After this, the caller of this
                                        // function will be responsable for
                                        // deleting the memory allocated to
                                        // this object.
    CAlertTarget *RemoveNextTarget( int nEventType = TARGET_ALL );

                                        // Function to reset the current
                                        // position pointed to by the iterator.
    void ResetPosition( void );

                                        // Destroys ALL entries in the
                                        // m_cAlertTargetList member valiable's
                                        // list. CAUTION! Deletes all items
                                        // from list.
    void DestroyAll( void );

private:
    CTargetCounterList m_cTargetCounterList;
    POSITION    m_rPosition;
};


/////////////////////////////////////////////////////////////////////
//  CAlertTarget class   (derived from CObject)

class AFX_EXT_CLASS CAlertTarget : public CObject
{
                                        // Allow for dynamic creation of objects
                                        // that can be identified with IsKindOf()
    DECLARE_DYNCREATE(CAlertTarget)

public:
                                        // Constructor and Destructor.
    CAlertTarget( void );
    ~CAlertTarget( void );

                                        // Function to retrieve this target's
                                        // priority.
    int GetPriority( void ) { return m_nPriority; };

                                        // Function to set this target's
                                        // priority.
    void SetPriority( int nNewPriority );

                                        // Function to retrieve this target's
                                        // Active state.
    int GetActive( void ) { return m_bActive; };

                                        // Function to set this target's
                                        // Active State.
    void SetActive( BOOL nNewActiveState );

                                        // These function will take care of
                                        // the features common to all alert
                                        // targets such as priority.
                                        // These will also physically
                                        // extract the parameters from the
                                        // string sent before returning.
    BOOL ConvertCommonToPacked( CString *lpcPackedString = NULL );
    BOOL ConvertCommonFromPacked( CString *lcszPackedString = NULL );

                                        // These functions are virtual function
                                        // designed to pack and unpack options
                                        // for each target type.
                                        // These functions will be virtualized
                                        // by every derivation of this class.
                                        // Would have liked these to be pure virtual,
                                        // but the runtime type info would not allow.
    virtual BOOL ConvertToPacked( CString *lpcPackedString = NULL );
    virtual BOOL ConvertFromPacked( CString *lcszPackedString = NULL );

                                        // Return the type of this target.
                                        // This is set by the derived class.
    int GetTargetType( void ) {return m_nTargetType;};

                                        // Function to set the tareget type of this
                                        // class.  This will be called by the
                                        // derived class to set the target type
                                        // during it's constructor.
    void SetTargetType( int nNewTargetType ) {m_nTargetType = nNewTargetType;};

private:
                                        // Active state flag for this target.
   BOOL m_bActive;
                                        // Private priority member variable.
   int  m_nPriority;
                                        // This target type will be filled in
                                        // by the constructor of a derived
                                        // class.  This will default to 0.
   int  m_nTargetType;
};



/////////////////////////////////////////////////////////////////////
// CEMailTarget (derived from CAlertTarget)

class AFX_EXT_CLASS CEMailTarget : public CAlertTarget
{
                                        // Allow for dynamic creation of objects
                                        // that can be identified with IsKindOf()
    DECLARE_DYNCREATE(CEMailTarget)

public:
                                        // Constructor and Destructor.
    CEMailTarget( void );
    ~CEMailTarget( void );

                                        // Function to get the name of the
                                        // E-Mail Target.
    LPCTSTR GetName( void ) {return (LPCTSTR) m_cszName;};

                                        // Function to set the name of the
                                        // E-Mial target.
    BOOL SetName( LPCTSTR cszNewName );

                                        // Get the address of this E-Mail
                                        // recipient.
    LPCTSTR GetAddress( void ) {return (LPCTSTR) m_cszAddress;};

                                        // Set the address of this E-Mail
                                        // recipient
    BOOL SetAddress( LPCTSTR cszNewAddress );

                                        // Conversion functions to pack and
                                        // unpack data from a packed file
                                        // format. (Overriding virtual
                                        // functions from CAlertTarget)
    BOOL ConvertToPacked( CString *lpcPackedString );
    BOOL ConvertFromPacked( CString *lcszPackedString );

private:
    CString m_cszName;
    CString m_cszAddress;
};

/////////////////////////////////////////////////////////////////////
// CPagerTarget (derived from CAlertTarget)


class AFX_EXT_CLASS CPagerTarget : public CAlertTarget
{
                                        // Allow for dynamic creation of objects
                                        // that can be identified with IsKindOf()
    DECLARE_DYNCREATE(CPagerTarget)

public:
                                        // Constructor and Destructor.
    CPagerTarget( void );
    ~CPagerTarget( void );

                                        // Return phone number.
    LPCTSTR GetPhoneNumber( void ) {return (LPCTSTR) m_cszPhoneNumber;};

                                        // Sets the phone number.
    BOOL SetPhoneNumber( LPCTSTR cszNewPhoneNumber );

                                        // Return message.
    LPCTSTR GetMessage( void ) {return (LPCTSTR) m_cszMessage;};

                                        // Sets the message.
    BOOL SetMessage( LPCTSTR cszNewMessage );

                                        // Conversion functions to pack and
                                        // unpack data from a packed file
                                        // format. (Overriding virtual
                                        // functions from CAlertTarget)
    BOOL ConvertToPacked( CString *lpcPackedString );
    BOOL ConvertFromPacked( CString *lcszPackedString );

private:
    CString m_cszPhoneNumber;
    CString m_cszMessage;
};

/////////////////////////////////////////////////////////////////////
// CLocalMsgTarget (derived from CAlertTarget)


class AFX_EXT_CLASS CLocalMsgTarget : public CAlertTarget
{
                                        // Allow for dynamic creation of objects
                                        // that can be identified with IsKindOf()
    DECLARE_DYNCREATE(CLocalMsgTarget)

public:
                                        // Constructor and Destructor.
    CLocalMsgTarget( void );
    ~CLocalMsgTarget( void );

                                        // Return the alert status.
    BOOL GetAlertStatus( void ) {return m_bAlertStatus;};

                                        // Sets the alert status.
    BOOL SetAlertStatus( BOOL bNewAlertStatus );

                                        // Conversion functions to pack and
                                        // unpack data from a packed file
                                        // format. (Overriding virtual
                                        // functions from CAlertTarget)
    BOOL ConvertToPacked( CString *lpcPackedString );
    BOOL ConvertFromPacked( CString *lcszPackedString );

private:
    BOOL m_bAlertStatus;
};


/////////////////////////////////////////////////////////////////////
// CNetMsgTarget (derived from CAlertTarget)


class AFX_EXT_CLASS CNetMsgTarget : public CAlertTarget
{
                                        // Allow for dynamic creation of objects
                                        // that can be identified with IsKindOf()
    DECLARE_DYNCREATE(CNetMsgTarget)

public:
                                        // Constructor and Destructor.
    CNetMsgTarget( void );
    ~CNetMsgTarget( void );

                                        // Return Address of Network
                                        // message target.
    LPCTSTR GetAddress( void ) {return (LPCTSTR) m_cszAddress;};

                                        // Set the address.
    BOOL SetAddress( LPCTSTR cszNewAddress );

                                        // Return the resource type of this
                                        // object.
    DWORD GetResourceType( void ) {return m_dwResourceType;};

                                        // Set the resource type of this
                                        // object.
    void SetResourceType( DWORD dwNewResourceType );

                                        // Conversion functions to pack and
                                        // unpack data from a packed file
                                        // format. (Overriding virtual
                                        // functions from CAlertTarget)
    BOOL ConvertToPacked( CString *lpcPackedString );
    BOOL ConvertFromPacked( CString *lcszPackedString );

private:
    CString m_cszAddress;

                                        // See NResCont.h for defines of
                                        // resource types.
    DWORD   m_dwResourceType;
};

/////////////////////////////////////////////////////////////////////
// CForwardTarget (derived from CAlertTarget)


class AFX_EXT_CLASS CForwardTarget : public CAlertTarget
{
                                        // Allow for dynamic creation of objects
                                        // that can be identified with IsKindOf()
    DECLARE_DYNCREATE(CForwardTarget)

public:
                                        // Constructor and Destructor.
    CForwardTarget( void );
    ~CForwardTarget( void );

                                        // Return Address of message
                                        // forward target.
    LPCTSTR GetAddress( void ) {return (LPCTSTR) m_cszAddress;};

                                        // Set the address.
    BOOL SetAddress( LPCTSTR cszNewAddress );

                                        // Return protocol of message
                                        // forward target.
    LPCTSTR GetProtocol( void ) {return (LPCTSTR) m_cszProtocol;};

                                        // Set the protocol.
    BOOL SetProtocol( LPCTSTR cszNewProtocol );

                                        // Conversion functions to pack and
                                        // unpack data from a packed file
                                        // format. (Overriding virtual
                                        // functions from CAlertTarget)
    BOOL ConvertToPacked( CString *lpcPackedString );
    BOOL ConvertFromPacked( CString *lcszPackedString );

private:
    CString m_cszAddress;
    CString m_cszProtocol;
};


/////////////////////////////////////////////////////////////////////
// CNLMTarget (derived from CAlertTarget)


class AFX_EXT_CLASS CNLMTarget : public CAlertTarget
{
                                        // Allow for dynamic creation of objects
                                        // that can be identified with IsKindOf()
    DECLARE_DYNCREATE(CNLMTarget)

public:
                                        // Constructor and Destructor.
    CNLMTarget( void );
    ~CNLMTarget( void );

                                        // Return Address of NLM server.
    LPCTSTR GetAddress( void ) {return (LPCTSTR) m_cszAddress;};

                                        // Set the address.
    BOOL SetAddress( LPCTSTR cszNewAddress );

                                        // Conversion functions to pack and
                                        // unpack data from a packed file
                                        // format. (Overriding virtual
                                        // functions from CAlertTarget)
    BOOL ConvertToPacked( CString *lpcPackedString );
    BOOL ConvertFromPacked( CString *lcszPackedString );

private:
    CString m_cszAddress;
};

/////////////////////////////////////////////////////////////////////
// CEventLogTarget (derived from CAlertTarget)


class AFX_EXT_CLASS CEventLogTarget : public CAlertTarget
{
                                        // Allow for dynamic creation of objects
                                        // that can be identified with IsKindOf()
    DECLARE_DYNCREATE(CEventLogTarget)

public:
                                        // Constructor and Destructor.
    CEventLogTarget( void );
    ~CEventLogTarget( void );

                                        // Return Address of message
                                        // forward target.
    LPCTSTR GetAddress( void ) {return (LPCTSTR) m_cszAddress;};

                                        // Set the address.
    BOOL SetAddress( LPCTSTR cszNewAddress );

                                        // Conversion functions to pack and
                                        // unpack data from a packed file
                                        // format. (Overriding virtual
                                        // functions from CAlertTarget)
    BOOL ConvertToPacked( CString *lpcPackedString );
    BOOL ConvertFromPacked( CString *lcszPackedString );

private:
    CString m_cszAddress;
};


/////////////////////////////////////////////////////////////////////
// CActivityLogTarget (derived from CAlertTarget)

class AFX_EXT_CLASS CActivityLogTarget : public CAlertTarget
{
                                        // Allow for dynamic creation of objects
                                        // that can be identified with IsKindOf()
    DECLARE_DYNCREATE(CActivityLogTarget)

public:
                                        // Constructor and Destructor.
    CActivityLogTarget( void );
    ~CActivityLogTarget( void );

                                        // Return Address of message
                                        // forward target.
    LPCTSTR GetAddress( void ) {return (LPCTSTR) m_cszAddress;};

                                        // Set the address.
    BOOL SetAddress( LPCTSTR cszNewAddress );

                                        // Conversion functions to pack and
                                        // unpack data from a packed file
                                        // format. (Overriding virtual
                                        // functions from CAlertTarget)
    BOOL ConvertToPacked( CString *lpcPackedString );
    BOOL ConvertFromPacked( CString *lcszPackedString );

private:
    CString m_cszAddress;
};

/////////////////////////////////////////////////////////////////////
// CSNMPTarget (derived from CAlertTarget)

class AFX_EXT_CLASS CSNMPTarget : public CAlertTarget
{
                                        // Allow for dynamic creation of objects
                                        // that can be identified with IsKindOf()
    DECLARE_DYNCREATE(CSNMPTarget)

public:
                                        // Constructor and Destructor.
    CSNMPTarget( void );
    ~CSNMPTarget( void );

                                        // Function to get the name of the
                                        // SNMP Target.
    LPCTSTR GetName( void ) {return (LPCTSTR) m_cszName;};

                                        // Function to set the name of the
                                        // SNMP target.
    BOOL SetName( LPCTSTR cszNewName );

                                        // Get the address of this SNMP
                                        // target.
    BOOL GetAddress( LPSOCKADDR pAddress );

                                        // Set the address of this SNMP
                                        // target
    BOOL SetAddress( LPSOCKADDR pNewAddress );

                                        // Conversion functions to pack and
                                        // unpack data from a packed file
                                        // format. (Overriding virtual
                                        // functions from CAlertTarget)
    BOOL ConvertToPacked( CString *lpcPackedString );
    BOOL ConvertFromPacked( CString *lcszPackedString );

private:
    CString     m_cszName;
    SOCKADDR    m_address;
};

#endif // if !defined(__TARGETS_H)
