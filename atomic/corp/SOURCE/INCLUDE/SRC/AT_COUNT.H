/////////////////////////////////////////////////////////////////////
// AT_COUNT.H - Created - 10/27/96
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/INCLUDE/VCS/AT_COUNT.H_v   1.0   24 Apr 1997 20:49:28   JBRENNA  $
//
// Description:
//    Header file for the CTargetCounter and  CTargetCounterList
//    classes.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/INCLUDE/VCS/AT_COUNT.H_v  $
// 
//    Rev 1.0   24 Apr 1997 20:49:28   JBRENNA
// Initial revision.
// 
//    Rev 1.0   13 Mar 1997 15:10:26   RFULLER
// Initial revision
// 
//    Rev 1.1   12 Nov 1996 22:50:12   KSACKIN
// Changed class definitions to use the MFC_EXT_CLASS macro, which will export
// all functions in that class.  Also converted to an MFC Extension DLL.
// 
//    Rev 1.0   08 Nov 1996 11:36:18   DKEEFER
// Initial revision
// 
//    Rev 1.0   06 Nov 1996 13:25:12   KSACKIN
// Initial revision.
// 
//    Rev 1.0   29 Oct 1996 15:37:08   KSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#if !defined(__TGTCOUNT_H)
#define __TGTCOUNT_H


#include <afx.h>
#include <tchar.h>

/////////////////////////////////////////////////////////////////////
// CTargetCounter class. (Derived from CObject)

                                        // Class to keep track of how many of a
                                        // particular type of target are in a
                                        // list of targets.
class AFX_EXT_CLASS CTargetCounter : public CObject
{
                                        // Allow for dynamic creation of objects
                                        // that can be identified with IsKindOf()
    DECLARE_DYNCREATE(CTargetCounter)

public:
                                        // Constructor and destructor.
    CTargetCounter( int nTargetType = 0 );
    ~CTargetCounter( void );

                                        // Member access functions.
    void  SetTargetType( int nTargetType ) {m_nTargetType =  nTargetType;};

    int   GetTargetType( void )  {return m_nTargetType;};

    DWORD GetTargetCount( void ) {return m_dwTargetTypeCount;};

                                        // Increment count on this target type.
    void  IncrementCount( void ) {m_dwTargetTypeCount++;};

                                        // Decrement count on this target type.
                                        // If the count is already zero, we
                                        // will return a failure.
    BOOL  DecrementCount( void );

private:
                                        // Target type defaults to 0.
    int     m_nTargetType;
                                        // Count will be initialized to 1.
    DWORD   m_dwTargetTypeCount;
};


/////////////////////////////////////////////////////////////////////
// CTargetCounterList class. (derived  from a CObList class)

class CTargetCounterList : public CObList
{
                                        // Allow for dynamic creation of objects
                                        // that can be identified with IsKindOf()
    DECLARE_DYNCREATE(CTargetCounterList)

public:
    CTargetCounterList( void );
    ~CTargetCounterList( void );

                                        // This function will get the count of
                                        // a particular target type, or a
                                        // combination of types.
    DWORD GetTargetCount( int nTargetType );

                                        // Function to find a counter for a
                                        // particular target type.  NULL will
                                        // be returned if a counter of that
                                        // type does not exist.
    CTargetCounter *FindTargetCounterOfType( int nTargetType );

                                        // This function will increment the
                                        // count of a particular type of
                                        // target.  If that target does not
                                        // exist yet, it will create an
                                        // instance of that target type, and
                                        // add it to the list.
    void IncrementTargetCount( int nTargetType );

                                        // Decrement that target type.  If the
                                        // target does not exist, or the target
                                        // count is already zero, it will
                                        // return FALSE;
    BOOL DecrementTargetCount( int nTargetType );

                                        // Destroy all elements of this list,
                                        // and free memory associated with them.
    void DestroyAll( void );
};


#endif // if !defined(__TGTCOUNT_H)
