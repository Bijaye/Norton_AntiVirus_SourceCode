#ifndef __OBJLIST_H_

#define __OBJLIST_H_

#ifndef _WINDOWS_
    #include    <windows.h>
#endif

#ifndef _WINPERF_
    #include    <winperf.h>
#endif

#ifndef _PERFSNAP_H_
    #include    "perfsnap.h"
#endif

class CPerfObject;


class CPerfObjectList
{
public:
    CPerfObjectList (CPerfSnapshot * const pPerfSnapshot, CPerfTitleDatabase * const pPerfTitleDatabase);
    
    ~CPerfObjectList (void) { };
    
    // functions that return CPerfObject pointers.  Caller is responsible for deleting
    // the CPerfObject * when done with it.
 
    CPerfObject *GetFirstPerfObject ( void );
    CPerfObject *GetNextPerfObject  ( void );
    CPerfObject *GetPerfObject      ( PTSTR const pszObjListName );
    
protected:
    CPerfSnapshot           *m_pPerfSnapshot;
    CPerfTitleDatabase      *m_pPerfCounterTitles;
    unsigned                m_currentObjectListIndex;
    PPERF_OBJECT_TYPE       m_pCurrObjectType;
    
};

typedef CPerfObjectList *PCPerfObjectList;

#endif