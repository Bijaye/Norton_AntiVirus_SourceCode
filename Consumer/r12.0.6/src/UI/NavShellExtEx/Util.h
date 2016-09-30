#pragma once

#include "synchelper.h"
#include <atlstr.h>

// /////////////////////////////////////////////////////////////////////////////////////////////////
// Common macros
// //////////////////////
#ifndef ASSERT
    #include <crtdbg.h>
    #define ASSERT(f) _ASSERTE(f)
#endif // ASSERT

// /////////////////////////////////////////////////////////////////////////////////////////////////
// Array dimension
// //////////////////////
#define CCDIMOF(Array) (sizeof(Array) / sizeof(Array[0]))

namespace UTIL
{
// /////////////////////////////////////////////////////////////////////////////////////////////////
// TraceX functions declaration
// //////////////////////
// The implementation of the TraceX functions must be provided by the 
// user. See the IMPLEMENT_CCTRACE() macro.
// /////////////////////////////////////////////////////////////////////////////////////////////////

void TraceInformation(LPCTSTR szFormat, ...) throw();
void TraceWarning(LPCTSTR szFormat, ...) throw();
void TraceError(LPCTSTR szFormat, ...) throw();


// /////////////////////////////////////////////////////////////////////////////////////////////////
// CCTRACEx macros
// //////////////////////

#define CCTRACEI ::TraceInformation
#define CCTRACEW ::TraceWarning
#define CCTRACEE ::TraceError


// /////////////////////////////////////////////////////////////////////////////////////////////////
// TRACE declaration and implementation macro
// //////////////////////
// Use IMPLEMENT_CCTRACE() to implement the TraceX functions as required by 
// the CCTRACEx macros.
// The DebugOutput parameter must be a global object that is a or derives 
// from the CDebugOutput class.
// /////////////////////////////////////////////////////////////////////////////////////////////////

#define IMPLEMENT_CCTRACE(DebugOutput) \
    void ::TraceInformation(LPCTSTR szFormat, ...) throw() \
    { \
        va_list args; \
        va_start(args, szFormat); \
        DebugOutput.DebugOutputInformation(szFormat, args); \
        va_end(args); \
    } \
    void ::TraceWarning(LPCTSTR szFormat, ...) throw() \
    { \
        va_list args; \
        va_start(args, szFormat); \
        DebugOutput.DebugOutputWarning(szFormat, args); \
        va_end(args); \
    } \
    void ::TraceError(LPCTSTR szFormat, ...) throw() \
    { \
        va_list args; \
        va_start(args, szFormat); \
        DebugOutput.DebugOutputError(szFormat, args); \
        va_end(args); \
    } 

/////////////////////////////////////////////////////////////////////////////////////////////////////
// Base
#ifdef _DEBUG

#define CCCATCH_BASE(ExceptionInfo) \
    ExceptionInfo.SetFile(_T(__FILE__)); \
    ExceptionInfo.SetLine(__LINE__); \
    //CCTRACEE(_T("CCCATCH : %s\n"), ExceptionInfo.GetFullDescription());

#else // _DEBUG

#define CCCATCH_BASE(ExceptionInfo) \
    //CCTRACEE(_T("CCCATCH : %s\n"), ExceptionInfo.GetFullDescription());

#endif // _DEBUG

// ATL
#if defined(__ATLEXCEPT_H__) && (_ATL_VER >= 0x0700)
    #define CCCATCH_ATL(ExceptionInfo) \
        catch (::ATL::CAtlException& e) \
        { \
            ExceptionInfo.SetType(CExceptionInfo::eATLException); \
            ExceptionInfo.SetName(_T("CAtlException")); \
            TCHAR _szDescription[16]; \
            wsprintf(_szDescription, _T("0x%08X"), e.m_hr); \
            ExceptionInfo.SetDescription(_szDescription); \
            CCCATCH_BASE(ExceptionInfo) \
        }
#else // __ATLEXCEPT_H__
    #define CCCATCH_ATL(ExceptionInfo)
#endif // __ATLEXCEPT_H__

// CCCATCHMEM
#define CCCATCHMEM(ExceptionInfo) \
    CCCATCH_ALLOC(ExceptionInfo) \
    CCCATCH_ATL(ExceptionInfo)

// STL bad_alloc
#ifdef _NEW_
    #define CCCATCH_ALLOC(ExceptionInfo) \
    catch (::std::bad_alloc& e) \
        { \
            ExceptionInfo.SetType(CExceptionInfo::eSTLBadAllocException); \
            ExceptionInfo.SetName(_T("std::bad_alloc")); \
            ExceptionInfo.SetDescription(e.what()); \
            CCCATCH_BASE(ExceptionInfo) \
        } 
#else // _NEW_
    #define CCCATCH_ALLOC(ExceptionInfo)
#endif // _NEW_



/*
class	CMessageLock
{
public:
	CMessageLock() {};
	~CMessageLock(){};
	DWORD LockWithMessagePump(DWORD dwCount,
                              const HANDLE* pHandles,
                              BOOL bWaitAll,
                              DWORD dwMilliseconds,
                              BOOL bAlertable);

	BOOL	GetWaitTime(DWORD dwStartTime, 
                              DWORD dwTotalTime, 
                              DWORD& dwWaitTime);

	DWORD GetTickCountDif(DWORD dwTimeStart, DWORD dwTimeStop);
};
*/
// /////////////////////////////////////////////////////////////////////////////////////////////////
// Class Declaration : CMessageLock
// //////////////////////
// Description : MsgWaitForMultipleObjectsEx lock with message pump
// /////////////////////////////////////////////////////////////////////////////////////////////////
class CMessageLock
{
// /////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor, Destructor, and Initialization 
// //////////////////////
public:
    CMessageLock(BOOL bPumpMessages, BOOL bExitOnQuit) throw();
    virtual ~CMessageLock() throw();

// /////////////////////////////////////////////////////////////////////////////////////////////////
// Prevent canonical behaviour
// //////////////////////
private:
    CMessageLock() throw();
    CMessageLock(const CMessageLock&) throw();
    CMessageLock& operator =(const CMessageLock&) throw();

// /////////////////////////////////////////////////////////////////////////////////////////////////
// Methods
// //////////////////////
public:
    DWORD Lock(DWORD dwCount,
               const HANDLE* pHandles,
               BOOL bWaitAll,
               DWORD dwMilliseconds,
               BOOL bAlertable) throw();
    DWORD Lock(HANDLE hHandle, DWORD dwMilliseconds, BOOL bAlertable) throw();
    DWORD Lock(HANDLE hHandle, DWORD dwMilliseconds) throw();

// /////////////////////////////////////////////////////////////////////////////////////////////////
// Internals
// //////////////////////
protected:
    DWORD LockWithMessagePump(DWORD dwCount,
                              const HANDLE *pHandles,
                              BOOL bWaitAll,
                              DWORD dwMilliseconds,
                              BOOL bAlertable) throw();

protected:
    BOOL m_bPumpMessages;
    BOOL m_bExitOnQuit;

protected:
    static const long m_nMaxMessages;
};

// /////////////////////////////////////////////////////////////////////////////////////////////////
// Function Name : CreateInstance<TYPE>()
// Description   : Creates a local or remote TYPE interface
// //////////////////////
template <class TYPE>
HRESULT CreateInstance(TYPE*& pType, 
                       REFCLSID clsId, 
                       LPCTSTR szServer, 
                       IUnknown *pOuter, 
                       DWORD dwContext) throw()
{
    // Server
    if (szServer == NULL ||
        lstrlen(szServer) == 0)
    {
        szServer = NULL;
    }
    COSERVERINFO coServerInfo;
    coServerInfo.dwReserved1 = 0;
    coServerInfo.dwReserved2 = 0;
    coServerInfo.pAuthInfo = NULL;
    CT2W szWServer(szServer);
    coServerInfo.pwszName = (szServer == NULL ? NULL : szWServer);

    // IUnknown interface
    MULTI_QI multiQi[1];
    multiQi[0].pIID = &IID_IUnknown;
    multiQi[0].pItf = NULL;
    multiQi[0].hr = S_OK;
    
    // Create object
    HRESULT hRes = E_FAIL;
    hRes = ::CoCreateInstanceEx(clsId, 
                                pOuter, 
                                dwContext,
                                (szServer == NULL ? NULL : &coServerInfo),
                                CCDIMOF(multiQi),
                                multiQi);
    if (FAILED(hRes))
    {
        //CCTRACEE(_T("CreateInstance() : CoCreateInstanceEx() != S_OK, 0x%08X\n"), hRes);
    }
    else
    {
        hRes = multiQi[0].pItf->QueryInterface(__uuidof(TYPE), (void**)&pType);
        multiQi[0].pItf->Release();
        if (FAILED(hRes))
        {
            //CCTRACEE(_T("CreateInstance() : multiQi[0].pItf->QueryInterface() != S_OK, 0x%08X\n"), hRes);
        }
    }
    return hRes;
}

// /////////////////////////////////////////////////////////////////////////////////////////////////
// Class Declaration : CSyncObject
// //////////////////////
// Description : Base synchronizer object
// /////////////////////////////////////////////////////////////////////////////////////////////////
class CSyncObject
{
// /////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor, Destructor, and Initialization 
// //////////////////////
public:
    CSyncObject() throw();
    virtual ~CSyncObject() throw();

// /////////////////////////////////////////////////////////////////////////////////////////////////
// Prevent canonical behaviour
// //////////////////////
private:
    CSyncObject(const CSyncObject&) throw();
    CSyncObject& operator =(const CSyncObject&) throw();

// /////////////////////////////////////////////////////////////////////////////////////////////////
// Pure Virtual Functions
// //////////////////////
public:
    virtual BOOL Lock(DWORD dwMilliseconds, BOOL bPumpMessages) throw() = 0;
    virtual BOOL Unlock() throw() = 0;
    virtual HANDLE GetHandle() const throw() = 0;

// /////////////////////////////////////////////////////////////////////////////////////////////////
// Static helper methods
// //////////////////////
public:
    // Returns TRUE if result is WAIT_OBJECT_0 + x or WAIT_ABANDONED_0 + x
    static BOOL IsAcquiredResult(DWORD dwResult) throw();
    // Returns the index of the acquired object, 
    // e.g. WAIT_OBJECT_0 - dwResult or WAIT_ABANDONED_0 - dwResult
    static size_t GetAcquiredIndex(DWORD dwResult) throw();

// /////////////////////////////////////////////////////////////////////////////////////////////////
// Internals
// //////////////////////
protected:
    //static BOOL CreateGlobalName(LPCTSTR szName, LPTSTR szGlobalName, DWORD dwCharCount) throw();

protected:
    static const LPCTSTR m_szGlobal;
};


// /////////////////////////////////////////////////////////////////////////////////////////////////
// Class Declaration : CCriticalSection
// //////////////////////
// Description : CRITICAL_SECTION Wrapper class
// /////////////////////////////////////////////////////////////////////////////////////////////////
class CCriticalSection : public CSyncObject
{
// /////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor, Destructor, and Initialization 
// //////////////////////
public:
    CCriticalSection() throw();
    virtual ~CCriticalSection() throw();

// /////////////////////////////////////////////////////////////////////////////////////////////////
// Prevent canonical behaviour
// //////////////////////
private:
    CCriticalSection(const CCriticalSection&) throw();
    CCriticalSection& operator =(const CCriticalSection&) throw();

// /////////////////////////////////////////////////////////////////////////////////////////////////
// Operators
// //////////////////////
public:
    operator CRITICAL_SECTION*() throw();
    operator CRITICAL_SECTION() throw();

// /////////////////////////////////////////////////////////////////////////////////////////////////
// Methods
// //////////////////////
public:
    BOOL Create() throw();
    void Destroy() throw();
    BOOL Lock() throw();
    BOOL TryLock() throw();

// /////////////////////////////////////////////////////////////////////////////////////////////////
// Properties
// //////////////////////
public:
    BOOL IsCriticalSection() const throw();

// /////////////////////////////////////////////////////////////////////////////////////////////////
// CSyncObject overrides
// //////////////////////
public:
    virtual BOOL Lock(DWORD dwMilliseconds, BOOL bPumpMessages) throw();
    virtual BOOL Unlock() throw();
    virtual HANDLE GetHandle() const throw();

// /////////////////////////////////////////////////////////////////////////////////////////////////
// Internals
// //////////////////////
protected:
    CRITICAL_SECTION m_CritSect;
    BOOL m_bCreated;
};


// /////////////////////////////////////////////////////////////////////////////////////////////////
// Class Declaration : CSemaphore
// //////////////////////
// Description : Semaphore wrapper class
// /////////////////////////////////////////////////////////////////////////////////////////////////
class CSemaphore : public CSyncObject
{
// /////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor, Destructor, and Initialization 
// //////////////////////
public:
    CSemaphore() throw();
    CSemaphore(LPSECURITY_ATTRIBUTES pSA,
               LONG nInitialCount, 
               LONG nMaxCount,
               LPCTSTR szName, 
               BOOL bGlobal) throw();
    virtual ~CSemaphore() throw();
    
// /////////////////////////////////////////////////////////////////////////////////////////////////
// Prevent canonical behaviour
// //////////////////////
private:
    CSemaphore(const CSemaphore&) throw();
    CSemaphore& operator =(const CSemaphore&) throw();
    
// /////////////////////////////////////////////////////////////////////////////////////////////////
// Operators
// //////////////////////
public:
    operator HANDLE() const throw();

// /////////////////////////////////////////////////////////////////////////////////////////////////
// Methods
// //////////////////////
public:
    BOOL Create(LPSECURITY_ATTRIBUTES pSA,
                LONG nInitialCount, 
                LONG nMaxCount,
                LPCTSTR szName, 
                BOOL bGlobal) throw();
    BOOL Open(DWORD dwDesiredAccess, BOOL bInheritHandle, LPCTSTR szName, BOOL bGlobal) throw();
    void Destroy() throw();
    BOOL Unlock(LONG nCount, LPLONG pPrevCount) throw();

// /////////////////////////////////////////////////////////////////////////////////////////////////
// Properties
// //////////////////////
public:
    BOOL IsSemaphore() const throw();

// /////////////////////////////////////////////////////////////////////////////////////////////////
// CSyncObject overrides
// //////////////////////
public:
    virtual BOOL Lock(DWORD dwMilliseconds, BOOL bPumpMessages) throw();
    virtual BOOL Unlock() throw();
    virtual HANDLE GetHandle() const throw();

// /////////////////////////////////////////////////////////////////////////////////////////////////
// Internals
// //////////////////////
protected:
    HANDLE m_hSemaphore;
};


// /////////////////////////////////////////////////////////////////////////////////////////////////
// Class Declaration : CMutex
// //////////////////////
// Description : Mutex wrapper class
// /////////////////////////////////////////////////////////////////////////////////////////////////
class CMutex : public CSyncObject
{
// /////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor, Destructor, and Initialization 
// //////////////////////
public:
    CMutex() throw();
    CMutex(LPSECURITY_ATTRIBUTES pSA,
           BOOL bInitiallyOwn, 
           LPCTSTR szName,
           BOOL bGlobal) throw();
    virtual ~CMutex() throw();
    
// /////////////////////////////////////////////////////////////////////////////////////////////////
// Prevent canonical behaviour
// //////////////////////
private:
    CMutex(const CMutex&) throw();
    CMutex& operator =(const CMutex&) throw();

// /////////////////////////////////////////////////////////////////////////////////////////////////
// Operators
// //////////////////////
public:
    operator HANDLE() const throw();

// /////////////////////////////////////////////////////////////////////////////////////////////////
// Methods
// //////////////////////
public:
    BOOL Create(LPSECURITY_ATTRIBUTES pSA, 
                BOOL bInitiallyOwn, 
                LPCTSTR szName,
                BOOL bGlobal) throw();
    BOOL Open(DWORD dwDesiredAccess, BOOL bInheritHandle, LPCTSTR szName, BOOL bGlobal) throw();
    void Destroy() throw();

// /////////////////////////////////////////////////////////////////////////////////////////////////
// Properties
// //////////////////////
public:
    BOOL IsMutex() const throw();
 
// /////////////////////////////////////////////////////////////////////////////////////////////////
// CSyncObject overrides
// //////////////////////
public:
    virtual BOOL Lock(DWORD dwMilliseconds, BOOL bPumpMessages) throw();
    virtual BOOL Unlock() throw();
    virtual HANDLE GetHandle() const throw();

// /////////////////////////////////////////////////////////////////////////////////////////////////
// Internals
// //////////////////////
protected:
    HANDLE m_hMutex;
};


// /////////////////////////////////////////////////////////////////////////////////////////////////
// Class Declaration : CEvent
// //////////////////////
// Description : Event wrapper class
// /////////////////////////////////////////////////////////////////////////////////////////////////
class CEvent : public CSyncObject
{
// /////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor, Destructor, and Initialization 
// //////////////////////
public:
    CEvent() throw();
    CEvent(LPSECURITY_ATTRIBUTES pSA,
           BOOL bManualReset,
           BOOL bInitiallyOwn, 
           LPCTSTR szName, 
           BOOL bGlobal) throw();
    virtual ~CEvent() throw();

// /////////////////////////////////////////////////////////////////////////////////////////////////
// Prevent canonical behaviour
// //////////////////////
private:
    CEvent(const CEvent&) throw();
    CEvent& operator =(const CEvent&) throw();

// /////////////////////////////////////////////////////////////////////////////////////////////////
// Operators
// //////////////////////
public:
    operator HANDLE() const throw();

// /////////////////////////////////////////////////////////////////////////////////////////////////
// Methods
// //////////////////////
public:
    BOOL Create(LPSECURITY_ATTRIBUTES pSA,
                BOOL bManualReset,
                BOOL bInitiallyOwn, 
                LPCTSTR szName, 
                BOOL bGlobal) throw();
    BOOL Open(DWORD dwDesiredAccess, BOOL bInheritHandle, LPCTSTR szName, BOOL bGlobal) throw();
    void Destroy() throw();
    BOOL SetState(BOOL bSet) throw();
    BOOL GetState() const throw();
    BOOL SetEvent() throw();
    BOOL PulseEvent() throw();
    BOOL ResetEvent() throw();

// /////////////////////////////////////////////////////////////////////////////////////////////////
// Properties
// //////////////////////
public:
    BOOL IsEvent() const throw();

// /////////////////////////////////////////////////////////////////////////////////////////////////
// CSyncObject overrides
// //////////////////////
public:
    virtual BOOL Lock(DWORD dwMilliseconds, BOOL bPumpMessages) throw();
    virtual BOOL Unlock() throw();
    virtual HANDLE GetHandle() const throw();

// /////////////////////////////////////////////////////////////////////////////////////////////////
// Internals
// //////////////////////
protected:
    HANDLE m_hEvent;
};


// /////////////////////////////////////////////////////////////////////////////////////////////////
// Class Declaration : CTickCount
// //////////////////////
// Description : GetTickCount() timer
// /////////////////////////////////////////////////////////////////////////////////////////////////
class CTickCount
{
// /////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor, Destructor, and Initialization 
// //////////////////////
public:
    CTickCount() throw();
    CTickCount(const CTickCount &TickCount) throw();
    virtual ~CTickCount() throw();

// /////////////////////////////////////////////////////////////////////////////////////////////////
// Operators
// //////////////////////
public:
    CTickCount& operator =(const CTickCount &TickCount) throw();

// /////////////////////////////////////////////////////////////////////////////////////////////////
// Methods
// //////////////////////
public:
    // Return the current GetTickCount() value and starts the timer
    DWORD Start() throw();
    // Return the current GetTickCount() value and stops the timer
    DWORD Stop() throw();

// /////////////////////////////////////////////////////////////////////////////////////////////////
// Properties
// //////////////////////
public:
    // Return start time
    DWORD GetStartTime() const throw();
    // Return stop time
    DWORD GetStopTime() const throw();
    // Return the time difference in milliseconds between Start() and Stop()
    DWORD GetElapsedTime() const throw();

// /////////////////////////////////////////////////////////////////////////////////////////////////
// Static Methods
// //////////////////////
public:
    // Return the difference in milliseconds between two GetTickCount() calls, the return value 
    // will account for one rollover event.
    static DWORD GetElapsedTime(DWORD dwStartTime, DWORD dwStopTime) throw();
    // Calculate remaining time usefull for WaitForXXX() functions.
    // A wait time of 0 and INFINITE is never modified and will always return true.
    // Return false if the total time period expired. 
    // Return true if time remains, and the remaining time is indicated in dwRemainingTime.
    static bool GetRemainingTime(DWORD dwStartTime, 
                                 DWORD dwStopTime, 
                                 DWORD dwTotalTime, 
                                 DWORD &dwRemainingTime) throw();

// /////////////////////////////////////////////////////////////////////////////////////////////////
// Internals
// //////////////////////
public:
    DWORD m_dwStartTime;
    DWORD m_dwStopTime;
};


// /////////////////////////////////////////////////////////////////////////////////////////////////
// Class Declaration : CExceptionInfo
// //////////////////////
// Description : Exception information container
// /////////////////////////////////////////////////////////////////////////////////////////////////
class CExceptionInfo
{
// /////////////////////////////////////////////////////////////////////////////////////////////////
// Structures, classes, typedefs, enums
// //////////////////////
public:
    enum ExceptionType
    {
        eNoException, 
        eUnknownException, 
        eMFCInternetException, 
        eMFCArchiveException, 
        eMFCFileException, 
        eMFCMemoryException, 
        eMFCNotSupportedException, 
        eMFCOleException, 
        eMFCOleDispatchException,
        eMFCResourceException, 
        eMFCUserException, 
        eMFCException, 
        eMFCDBException, 
        eMFCDaoException, 
        eMFCInvalidArgException,
        eSTLException, 
        eSTLBadException, 
        eSTLBadCastException, 
        eSTLBadTypeIdException, 
        eSTLBadAllocException,
        eCOMErrorException,
        eATLException,
        eCharException, 
        eUnsignedCharException, 
        eShortException, 
        eUnsignedShortException, 
        eIntException, 
        eUnsignedIntException, 
        eLongException, 
        eUnsignedLongException, 
        eStringException,
        eWideStringException
    };

// /////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor, Destructor, and Initialization 
// //////////////////////
public:
    CExceptionInfo(LPCTSTR szFunction) throw();
    virtual ~CExceptionInfo() throw();

// /////////////////////////////////////////////////////////////////////////////////////////////////
// Prevent canonical behaviour
// //////////////////////
private:
    CExceptionInfo() throw();
    CExceptionInfo(const CExceptionInfo&) throw();
    CExceptionInfo& operator =(const CExceptionInfo&) throw();

// /////////////////////////////////////////////////////////////////////////////////////////////////
// Operators
// //////////////////////
public:
    // TODO: Deprecate and replace with IsException()
    operator BOOL() const throw();

// /////////////////////////////////////////////////////////////////////////////////////////////////
// Methods
// //////////////////////
public:    

    // ////////////////////////////////////////////////////////////////////////////////
    // Name        : Reset()
    // Description : Resets all the properties affected by an exception.
    //               Use this method when reusing the object inside loops.  
    // //////////////////////
    void Reset() throw();

// /////////////////////////////////////////////////////////////////////////////////////////////////
// Properties
// //////////////////////
public:
    LPCTSTR GetFunction() const throw();
    void SetType(ExceptionType eType) throw();
    ExceptionType GetType() const throw();
    void SetFile(LPCTSTR szFile) throw();
    LPCTSTR GetFile() const throw();
    void SetLine(long nLine) throw();
    long GetLine() const throw();
    void SetName(LPCTSTR szName) throw();
    LPCTSTR GetName() const throw();
    void SetDescription(LPCSTR szDescription) throw();
    void SetDescription(LPCWSTR szDescription) throw();
    LPCTSTR GetDescription() const throw();
    LPCTSTR GetFullDescription() const throw();
    bool IsException() const throw();

// /////////////////////////////////////////////////////////////////////////////////////////////////
// Internals
// //////////////////////
protected:
    ExceptionType m_eType;
    LPCTSTR m_szFunction;
    CString m_sName;
    CString m_sDescription;
    CString m_sFile;
    long m_nLine;
    mutable CString m_sFullDescription;
};

} // UTIL