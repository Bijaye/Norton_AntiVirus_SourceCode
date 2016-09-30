#include "stdafx.h"
#include <windows.h>
#include <new>
#include <tchar.h>
#include "Util.h"

namespace UTIL
{
// /////////////////////////////////////////////////////////////////////////////////////////////////
// Static initialization
// //////////////////////
const long CMessageLock::m_nMaxMessages = 32;

// /////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor, Destructor, and Initialization 
// //////////////////////
CMessageLock::CMessageLock(BOOL bPumpMessages, BOOL bExitOnQuit) throw() :
    m_bPumpMessages(bPumpMessages),
    m_bExitOnQuit(bExitOnQuit)
{
}

CMessageLock::~CMessageLock() throw()
{
}

// /////////////////////////////////////////////////////////////////////////////////////////////////
// Methods
// //////////////////////
DWORD CMessageLock::Lock(DWORD dwCount,
                         const HANDLE* pHandles,
                         BOOL bWaitAll,
                         DWORD dwMilliseconds,
                         BOOL bAlertable) throw()
{
    if (m_bPumpMessages == FALSE)
    {
        return WaitForMultipleObjectsEx(dwCount, pHandles, bWaitAll, dwMilliseconds, bAlertable);
    }
    else
    {
        return LockWithMessagePump(dwCount, pHandles, bWaitAll, dwMilliseconds, bAlertable);
    }
}

DWORD CMessageLock::Lock(HANDLE hHandle, DWORD dwMilliseconds, BOOL bAlertable) throw()
{
    if (m_bPumpMessages == FALSE &&
        bAlertable == FALSE)
    {
        return WaitForSingleObject(hHandle, dwMilliseconds);
    }
    return Lock(1, &hHandle, FALSE, dwMilliseconds, bAlertable);
}

DWORD CMessageLock::Lock(HANDLE hHandle, DWORD dwMilliseconds) throw()
{
    if (m_bPumpMessages == FALSE)
    {
        return WaitForSingleObject(hHandle, dwMilliseconds);
    }
    return Lock(1, &hHandle, FALSE, dwMilliseconds, FALSE);
}

// /////////////////////////////////////////////////////////////////////////////////////////////////
// Internals
// //////////////////////
DWORD CMessageLock::LockWithMessagePump(DWORD dwCount,
                                        const HANDLE *pHandles,
                                        BOOL bWaitAll,
                                        DWORD dwMilliseconds,
                                        BOOL bAlertable) throw()
{
    ASSERT(dwCount > 0 && dwCount <= MAXIMUM_WAIT_OBJECTS - 1);
    ASSERT(pHandles != NULL);

    // The MWMO_INPUTAVAILABLE flag has been removed at the suggest of Microsoft since it 
    // causes problems on 9x platforms. 
    // The presence of the MWMO_INPUTAVAILABLE flag caused MsgWaitForMultipleObjectsEx() to 
    // return signaling that a message is in the queue, but PeekMessage() does not detect the 
    // message causing the code to go into a tight loop.

    // The MWMO_WAITALL flag will not return unless all events are signalled, and a message is 
    // in the queue. This behavior is not the same as that of WaitForMultipleObjectsEx().
    // We have to write custom logic to make the bWaitAll flag work correctly.

    // The message pumping technique is based on the ATL AtlWaitWithMessageLoop() function, with
    // modifications to account for waiting on multiple objects.

    // Start the clock
    DWORD dwStartTime = GetTickCount();    

    // Prepare vector of handles in case of bWaitAll
    const HANDLE *pEventHandles = pHandles;
    ::std::vector<HANDLE> handleVector;
    if (bWaitAll != FALSE)
    {
        CExceptionInfo exceptionInfo(_T("CMessageLock::LockWithMessagePump()"));
        try
        {
            // Add all the handles to a vector
            handleVector.reserve(dwCount);
            DWORD i = 0;
            for (i = 0; i < dwCount; i ++)
            {
                handleVector.push_back(pHandles[i]);
            }
        }
        CCCATCHMEM(exceptionInfo);
        if (exceptionInfo != FALSE)
        {
            return WAIT_FAILED;
        }
        pEventHandles = &handleVector.at(0);
    }

    // Keep on waiting until time runs out or event is signalled
    DWORD dwRemainingTime = dwMilliseconds;
    DWORD dwRet = 0;
    for (;;)
    {
        // Calculate correct wait time (0 and INFINITE is not modified)
        if (CTickCount::GetRemainingTime(dwStartTime, 
                                         GetTickCount(), 
                                         dwMilliseconds, 
                                         dwRemainingTime) == FALSE)
        {
            // Set timeout return code
            dwRet = WAIT_TIMEOUT;

            // Done
            break;
        }

        // Wait
        dwRet = MsgWaitForMultipleObjectsEx(dwCount,
                                            pEventHandles,
                                            dwRemainingTime,
                                            QS_ALLINPUT,
                                            (bAlertable == FALSE ? 0 : MWMO_ALERTABLE));
        // Timeout
        if (dwRet == WAIT_TIMEOUT)
        {
            // Done
            break;
        }

        // Failure
        if (dwRet == WAIT_FAILED)
        {
            //CCTRACEE(_T("CMessageLock::LockWithMessagePump() : MsgWaitForMultipleObjectsEx() == WAIT_FAILED, 0x%08X\n"), GetLastError());
            
            // Done
            break;
        }
        
        // APC
        if (dwRet == WAIT_IO_COMPLETION)
        {
            // Done
            break;
        }
        
        // Message received
        if (dwRet == WAIT_OBJECT_0 + dwCount)
        {
            // Pump messages
            BOOL bBreak = FALSE;
            MSG msg = {0};
            size_t nCount = 0;
            while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) != FALSE)
            {
                // Increment count
                nCount ++;

                // Handle WM_QUIT
                if (msg.message == WM_QUIT)
                {
                    //CCTRACEI(_T("CMessageLock::LockWithMessagePump() : msg.message == WM_QUIT\n"));

                    // Break on WM_QUIT without removing the message from the queue
                    if (m_bExitOnQuit != FALSE)
                    {
                        //CCTRACEI(_T("CMessageLock::LockWithMessagePump() : m_bExitOnQuit != FALSE\n"));
            
                        // Done
                        bBreak = TRUE;
                        break;
                    }
                }

                // UNICODE or MBCS window
                BOOL bUNICODEWindow = FALSE;
                if (msg.hwnd == NULL)
                {
                    // Use native mode for NULL windows

#ifdef _UNICODE

                    bUNICODEWindow = TRUE;

#else // _UNICODE

                    bUNICODEWindow = FALSE;

#endif // _UNICODE

                }
                else
                {
                    // Get type from window
                    bUNICODEWindow = IsWindowUnicode(msg.hwnd);
                }
                
                // We peeked, remove the message
                if (bUNICODEWindow == FALSE)
                {
                    if (GetMessageA(&msg, NULL, 0, 0) == FALSE &&
                        msg.message != WM_QUIT)
                    {
                        //CCTRACEE(_T("CMessageLock::LockWithMessagePump() : GetMessageA() == FALSE, 0x%08X\n"), GetLastError());
                    }
                }
                else
                {
                    if (GetMessageW(&msg, NULL, 0, 0) == FALSE &&
                        msg.message != WM_QUIT)
                    {
                        //CCTRACEE(_T("CMessageLock::LockWithMessagePump() : GetMessageW() == FALSE, 0x%08X\n"), GetLastError());
                    }
                }

                // Pump message
                TranslateMessage(&msg);
                if (bUNICODEWindow == FALSE)
                {
                    DispatchMessageA(&msg);
                }
                else
                {
                    DispatchMessageW(&msg);
                }

                // Test event signal state in between pumping messages
                DWORD dwRetTemp = WaitForMultipleObjects(dwCount, pEventHandles, FALSE, 0);
                if (CSyncObject::IsAcquiredResult(dwRetTemp) != FALSE)
                {
                    // Event signalled, update return code to be processed in next block
                    dwRet = dwRetTemp;
                    break;
                }

                // Make sure we do not wait longer than the wait period
                if (CTickCount::GetRemainingTime(dwStartTime, 
                                                 GetTickCount(), 
                                                 dwMilliseconds, 
                                                 dwRemainingTime) == FALSE)
                {
                    // Set timeout return code
                    dwRet = WAIT_TIMEOUT;

                    // Done
                    bBreak = TRUE;
                    break;
                }
            }

            // Did we process any messages
            if (nCount == 0)
            {
                // It has been observed that on 9x systems where message queues are exhausted this 
                // condition may occur, and as such it could be an indication of a problem.
                // MSDN:
                // This value is also returned upon the occurrence of a system event that requires 
                // the thread's action, such as foreground activation. Therefore, 
                // MsgWaitForMultipleObjectsEx can return even though no appropriate input is 
                // available and even if dwWakeMask is set to 0. If this occurs, call GetMessage or 
                // PeekMessage to process the system event before trying the call to 
                // MsgWaitForMultipleObjectsEx again.
                //CCTRACEI(_T("CMessageLock::LockWithMessagePump() : nCount == 0, 0x%08X\n"), GetLastError());
            }

            // Break if required
            if (bBreak != FALSE)
            {
                // Done
                break;
            }

            // Continue if the return code was not modified
            if (dwRet == WAIT_OBJECT_0 + dwCount)
            {
                continue;
            }
        }

        // Event signalled by MsgWaitForMultipleObjectsEx() or by WaitForMultipleObjects() from
        // previous block.
        if (CSyncObject::IsAcquiredResult(dwRet) != FALSE)
        {
            // Are we waiting on all events to signal
            if (bWaitAll != FALSE)
            {
                // Remove the signalled event from the vector
				size_t nIndex = CSyncObject::GetAcquiredIndex(dwRet);
                handleVector.erase(handleVector.begin() + nIndex);

                // Only break once all events have been signalled
                if (handleVector.size() == 0)
                {
                    // Done
                    break;
                }

                // Reset vector and count
                pEventHandles = &handleVector.at(0);
                dwCount = DWORD(handleVector.size());
            }
            else
            {
                // Done
                break;
            }
        }
        else
        {
            // An unknown result...
            ASSERT(FALSE);
            break;
        }
    }
    return dwRet;
}

// /////////////////////////////////////////////////////////////////////////////////////////////////
// Class Implementation : CSyncObject
// /////////////////////////////////////////////////////////////////////////////////////////////////

// /////////////////////////////////////////////////////////////////////////////////////////////////
// Static initialization
// //////////////////////
const LPCTSTR CSyncObject::m_szGlobal = _T("Global\\");

// /////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor, Destructor, and Initialization 
// //////////////////////
CSyncObject::CSyncObject() throw()
{
}

CSyncObject::~CSyncObject() throw()
{
}

// /////////////////////////////////////////////////////////////////////////////////////////////////
// Static helper methods
// //////////////////////
BOOL CSyncObject::IsAcquiredResult(DWORD dwResult) throw()
{
    if ((dwResult >= WAIT_OBJECT_0 && dwResult <= (WAIT_OBJECT_0 + MAXIMUM_WAIT_OBJECTS - 1)) ||
        (dwResult >= WAIT_ABANDONED_0 && dwResult <= (WAIT_ABANDONED_0 + MAXIMUM_WAIT_OBJECTS - 1)))
    {
        return TRUE;
    }
    return FALSE;
}

size_t CSyncObject::GetAcquiredIndex(DWORD dwResult) throw()
{
    size_t nIndex = 0;
    if (dwResult >= WAIT_OBJECT_0 && 
        dwResult <= (WAIT_OBJECT_0 + MAXIMUM_WAIT_OBJECTS - 1))
    {
        nIndex = dwResult - WAIT_OBJECT_0;
    }
    else if (dwResult >= WAIT_ABANDONED_0 && 
             dwResult <= (WAIT_ABANDONED_0 + MAXIMUM_WAIT_OBJECTS - 1))
    {
        nIndex = dwResult - WAIT_ABANDONED_0;
    }
    else
    {
        // Not acquired results code
        ASSERT(FALSE);
    }
    return nIndex;
}



// /////////////////////////////////////////////////////////////////////////////////////////////////
// Class Implementation : CTickCount
// /////////////////////////////////////////////////////////////////////////////////////////////////

// /////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor, Destructor, and Initialization  Initialization
// //////////////////////
CTickCount::CTickCount() throw() :
    m_dwStartTime(0),
    m_dwStopTime(0)
{
}

CTickCount::CTickCount(const CTickCount &TickCount) throw() :
    m_dwStartTime(0),
    m_dwStopTime(0)
{
    // Call assignment operator
    *this = TickCount;
}

CTickCount::~CTickCount() throw()
{
}

// /////////////////////////////////////////////////////////////////////////////////////////////////
// Operators
// //////////////////////
CTickCount& CTickCount::operator =(const CTickCount& TickCount) throw()
{
    // Copy members
    m_dwStartTime = TickCount.m_dwStartTime;
    m_dwStopTime = TickCount.m_dwStopTime;

    return *this;
}

// /////////////////////////////////////////////////////////////////////////////////////////////////
// Methods
// //////////////////////
DWORD CTickCount::Start() throw()
{
    m_dwStartTime = GetTickCount();
    return m_dwStartTime;
}

DWORD CTickCount::Stop() throw()
{
    m_dwStopTime = GetTickCount();
    return m_dwStopTime;
}

// /////////////////////////////////////////////////////////////////////////////////////////////////
// Properties
// //////////////////////
DWORD CTickCount::GetStartTime() const throw()
{
    return m_dwStartTime;
}

DWORD CTickCount::GetStopTime() const throw()
{
    return m_dwStopTime;
}

DWORD CTickCount::GetElapsedTime() const throw()
{
    return GetElapsedTime(m_dwStartTime, m_dwStopTime);
}

// /////////////////////////////////////////////////////////////////////////////////////////////////
// Static Methods
// //////////////////////
DWORD CTickCount::GetElapsedTime(DWORD dwStartTime, DWORD dwStopTime) throw()
{
    DWORD dwElapsedTime = 0;
    if (dwStopTime < dwStartTime)
    {
        // Time wrapped at DWORD(-)
        dwElapsedTime = DWORD(-1) - dwStartTime + dwStopTime;
    }
    else
    {
        dwElapsedTime = dwStopTime - dwStartTime;
    }
    return dwElapsedTime;
}

bool CTickCount::GetRemainingTime(DWORD dwStartTime, 
                                  DWORD dwStopTime,
                                  DWORD dwTotalTime, 
                                  DWORD &dwRemainingTime) throw()
{
    // If the wait time is 0 or INFINITE we allow the default processing
    if (dwTotalTime == INFINITE ||
        dwTotalTime == 0)
    {
        dwRemainingTime = dwTotalTime;
        return true;
    }

    // Get time difference
    DWORD dwElapsedTime = GetElapsedTime(dwStartTime, dwStopTime);
    if (dwElapsedTime >= dwTotalTime)
    {
        // Expired
        dwRemainingTime = 0;
        return false;
    }

    // Calculate new wait time
    dwRemainingTime = dwTotalTime - dwElapsedTime;
    return true;
}


// /////////////////////////////////////////////////////////////////////////////////////////////////
// Class Implementation : CExceptionInfo
// /////////////////////////////////////////////////////////////////////////////////////////////////

// /////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor, Destructor, and Initialization 
// //////////////////////
CExceptionInfo::CExceptionInfo(LPCTSTR szFunction) throw() :
    m_szFunction(szFunction),
    m_nLine(0),
    m_eType(eNoException) 
{
}

CExceptionInfo::~CExceptionInfo() throw()
{
}

// /////////////////////////////////////////////////////////////////////////////////////////////////
// Operators
// //////////////////////
CExceptionInfo::operator BOOL() const throw()
{
    return (IsException() != false);
}

// /////////////////////////////////////////////////////////////////////////////////////////////////
// Methods
// //////////////////////
void CExceptionInfo::Reset() throw()
{
    m_nLine = 0;
    m_eType = eNoException;
    m_sName.Empty();
    m_sDescription.Empty();
    m_sFullDescription.Empty();
    m_sFile.Empty();
}

// /////////////////////////////////////////////////////////////////////////////////////////////////
// Properties
// //////////////////////
bool CExceptionInfo::IsException() const throw()
{
    return (m_eType != eNoException);
}

LPCTSTR CExceptionInfo::GetFunction() const throw()
{
    return m_szFunction;
}

void CExceptionInfo::SetType(ExceptionType eType) throw()
{
    m_eType = eType;
}

CExceptionInfo::ExceptionType CExceptionInfo::GetType() const throw()
{
    return m_eType;
}

void CExceptionInfo::SetFile(LPCTSTR szFile) throw()
{
    try
    {
        m_sFile = szFile;
    }
    catch (::ATL::CAtlException&)
    {
        // Nothing to do in case of failure
    }
}

LPCTSTR CExceptionInfo::GetFile() const throw()
{
    return m_sFile;
}

void CExceptionInfo::SetLine(long nLine) throw()
{
    m_nLine = nLine;
}

long CExceptionInfo::GetLine() const throw()
{
    return m_nLine;
}

void CExceptionInfo::SetName(LPCTSTR szName) throw()
{
    try
    {
        m_sName = szName;
    }
    catch (::ATL::CAtlException&)
    {
        // Nothing to do in case of failure
    }
}

LPCTSTR CExceptionInfo::GetName() const throw()
{
    return m_sName;
}

void CExceptionInfo::SetDescription(LPCSTR szDescription) throw()
{
    try
    {
        m_sDescription = szDescription;
    }
    catch (::ATL::CAtlException&)
    {
        // Nothing to do in case of failure
    }
}

void CExceptionInfo::SetDescription(LPCWSTR szDescription) throw()
{
    try
    {
        m_sDescription = szDescription;
    }
    catch (::ATL::CAtlException&)
    {
        // Nothing to do in case of failure
    }
}

LPCTSTR CExceptionInfo::GetDescription() const throw()
{
    return m_sDescription;
}

LPCTSTR CExceptionInfo::GetFullDescription() const throw()
{
    try
    {
        m_sFullDescription.Format(_T("%s, %s, %s, %s (%ld)"),
                                  m_szFunction,
                                  LPCTSTR(m_sName),
                                  LPCTSTR(m_sDescription),
                                  LPCTSTR(m_sFile),
                                  m_nLine);
    }
    catch (::ATL::CAtlException&)
    {
        // Nothing to do in case of failure
    }
    return m_sFullDescription;
}

}