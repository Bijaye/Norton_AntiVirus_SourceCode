// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation. All rights reserved.


#include <string.h>
#include <sys/msg.h>
#include <pthread.h>
#include <set>
#include <vector>

#include "winBaseTypes.h"
//#include "RegistryIPC.h"

// Override the default behavior, and don't #define the registry calls
#define REGISTRY_CLIENT_IPC_EXPLICIT
#include "RegistryClient.h"

// extern "C" {
// #include "dprintf.h"
// #include "dprintf_2.h"
// #undef dprintf
// }

#include "REGISTRY.H"

// these SysV semaphore headers must be included after REGISTRY.H to avoid
// overloading the GETVAL symbol, which is also used in vpcommon.h
#include <sys/ipc.h>
#include <sys/sem.h>

#include "ipcmessage.h"
#include "socketcomm.h"
#include "symregipc.h"

#include <new>

const char* emptyString = "";

// extern DEBUGFLAGTYPE debug;

//#define BRUTE_FORCE_DEBUG 1

using namespace SymRegIPC;

class SocketPool
{
public:

    SocketPool()
    {
        pthread_mutex_init(&m_sockPoolMutex, NULL);
        int sock = OpenIpc();
        if(-1 != sock)
            m_sockVect.push_back(sock);
    }

    ~SocketPool()
    {
        Lock();
        while(!m_sockVect.empty())
        {
            CloseIpc(m_sockVect.back());
            m_sockVect.pop_back();
        }
        Unlock();
        pthread_mutex_destroy(&m_sockPoolMutex);
    }

    int GetSocket()
    {
        int sock = -1;

        Lock();
        if(!m_sockVect.empty())
        {
            sock = m_sockVect.back();
            m_sockVect.pop_back();
        }
        Unlock();

        if(-1 == sock)
            sock = OpenIpc();

        return sock;
    }

    void ReturnSocket(int sock)
    {
        if(sock >= 0)
        {
            Lock();
            m_sockVect.push_back(sock);
            Unlock();
        }
    }

private:

    std::vector<int> m_sockVect;
    pthread_mutex_t m_sockPoolMutex;

    int OpenIpc()
    {
        int sock = -1;
        timeval timeup;
        timeup.tv_usec = 0;
        timeup.tv_sec = SymRegIPC::ipc_timeout_sec;
        sock = SocketComm::Connect(SymRegIPC_SocketName, 0, &timeup,
                                   use_abstract);
        return (sock >= 0) ? sock : -1;
    }

    void CloseIpc(int sock)
    {
        if(-1 != sock)
            SocketComm::CloseSocket(sock);
    }

    void Lock() { pthread_mutex_lock(&m_sockPoolMutex); }
    void Unlock() { pthread_mutex_unlock(&m_sockPoolMutex); }
};

// IPC connection pool.
SocketPool* sockPool = NULL;

typedef std::set<unsigned long> HKeySet;
HKEY CreateHandle(unsigned long regKey);
void DeleteHandle(HKEY handle);
bool IsValidHKey(HKEY hkey);

static DWORD ipcTransactReg(RegMsg& req, RegMsg& resp);
static DWORD ipcSendMsg(int sock, IpcMessage& msg);
static DWORD ipcReadMsg(int sock, IpcMessage& msg);


/**
 * Object to handle creation and destruction of predefeind keys.  Predefined
 * keys must be created specially as they are not requested from the server.
 */
class PredefinedKey
{
public:
    PredefinedKey( HKEY hkey, HKeySet& validator ) :
        m_hkey(INVALID_HANDLE_VALUE),
        m_handles(validator)
    {
        if ( hkey != INVALID_HANDLE_VALUE && hkey != NULL )
        {
#ifdef BRUTE_FORCE_DEBUG
            printf("creating predefined key\n");
#endif // BRUTE_FORCE_DEBUG

            m_hkey = hkey;

            // Put predefined key value in the handle set.
            unsigned long regKey = reinterpret_cast<unsigned long>(hkey);

            m_handles.insert(regKey);
        }
    }

    ~PredefinedKey()
    {
        DeleteHandle(m_hkey);
    }

    HKEY GetHkey() { return m_hkey; }

private:

    HKEY m_hkey;
    HKeySet& m_handles;
};

/**
 * Singleton conatiner for key set, predefined keys, and mutex for access to
 * key set.
 */
class KeyMaster
{
public:
    KeyMaster(pthread_mutex_t& mutex) :
        symRegHKLM(0),
        symRegHKCU(0),
        cookie(0),
        hkSetMutex(mutex),
        hkeySet()
    {
        sockPool = new (std::nothrow) SocketPool;
        if(!sockPool)
        {
            // If there is memory allocation failure at this point, then there
            // is not chance that anything useful will work.  So, just exit.
            exit(1);
        }
        CreateHKLM();
        CreateHKCU();
    }

    void CreateHKLM()
    {
        if(symRegHKLM) delete symRegHKLM;
        symRegHKLM = new(std::nothrow) PredefinedKey(HKEY_LOCAL_MACHINE, hkeySet);
        if(symRegHKLM)
            cookie = ipcGetSubscriberCookie();
    }

    void CreateHKCU()
    {
        if(symRegHKCU) delete symRegHKCU;
        if(HKEY_LOCAL_MACHINE != HKEY_CURRENT_USER)
            symRegHKCU = new(std::nothrow) PredefinedKey(HKEY_CURRENT_USER, hkeySet);
    }

    virtual ~KeyMaster()
    {
        delete symRegHKLM;
        symRegHKLM = 0;

        delete symRegHKCU;
        symRegHKCU = 0;

        delete sockPool;
        sockPool = NULL;
    }

    PredefinedKey* symRegHKLM;
    PredefinedKey* symRegHKCU;

    unsigned long cookie;

    pthread_mutex_t& hkSetMutex;

    // The handle set object.
    HKeySet hkeySet;

private:

    unsigned long ipcGetSubscriberCookie()
    {
        RegMsg req, resp;
        req.opcode = op_GetSubscriberCookie;
        unsigned long cookie = 0;

        unsigned char* ptr = req.payload;
        PACK_U32( ptr, getpid() );
        req.size = ptr - req.payload;

        DWORD dwRet = ipcTransactReg(req, resp);
        if(dwRet == ERROR_SUCCESS)
        {
            const unsigned char *ptr = (const unsigned char*)resp.payload;
            dwRet = UNPACK_U32(ptr);
            if(dwRet == ERROR_SUCCESS)
                cookie = UNPACK_U32(ptr);
        }

        return cookie;
    }
};

/**
 * Return reference to KeyMaster singleton.  If KeyMaster is not yet created
 * then it gets created here.
 *
 * @return reference to KeyMaster singleton.
 */
KeyMaster& getKeyMaster()
{
    static pthread_mutex_t hkSetMutex = PTHREAD_MUTEX_INITIALIZER;

    // An singleton instance.
    static KeyMaster validHKeySet(hkSetMutex);

    return validHKeySet;
}

/**
 * Write string to memory.
 *
 * A 4-byte string length is written to memory preceeding the string.  The
 * string data follows.  A zero byte is written at the end of the string.  The
 * length value does not count the zero byte.  Pointer is advanced past the end
 * of the data written.
 *
 * @param ptr   [out] Memory to write to.
 *              [in]  String to write.
 */
static void pack_string( unsigned char*& ptr, const char* str )
{
    DWORD len = str ? static_cast<DWORD>(strlen(str)) : 0;
    PACK_U32( ptr, len );
    if(len > 0)
    {
        memcpy( ptr, str, len );
        ptr += len;
    }
    *ptr++ = 0;
}

/**
 * Read a string from memory, packed with pack_string.
 *
 * Read the 4-byte length and then the string data, including the zero-byte at
 * then end of the string.  Pointer is advanced to the first byte after the
 * zero-byte.
 *
 * @param ptr       [in] Pointer to packed string data.
 *
 * @return Pointer to string.  Pointer is within mem pointed to by ptr param.
 */
static const char* unpack_string(const unsigned char*& ptr)
{
    DWORD len = UNPACK_U32(ptr);
    const char* ret = (const char*)ptr;
    ptr += len + 1;
    return ret;
}

DWORD ipcRegOpenKey(HKEY base, LPCSTR key, PHKEY out)
{
    if(!IsValidHKey(base))
        return ERROR_INVALID_HANDLE;

    DWORD dwRet = ERROR_REGISTRY_IO_FAILED;

    RegMsg req, resp;
    req.opcode = op_RegOpenKey;

    unsigned char* ptr = req.payload;
    PACK_U32( ptr, getKeyMaster().cookie );
    PACK_U32( ptr, reinterpret_cast<DWORD>(base) );
    pack_string( ptr, key );

    req.size = ptr - req.payload;

    dwRet = ipcTransactReg(req, resp);
    if(dwRet == ERROR_SUCCESS)
    {
        const unsigned char *ptr = (const unsigned char*)resp.payload;
        dwRet = UNPACK_U32(ptr);
        if(dwRet == ERROR_SUCCESS)
        {
            if(out)
            {
                // Store the new reg key.
                unsigned long regKey = UNPACK_U32(ptr);
                HKEY handle = CreateHandle(regKey);
                if(INVALID_HANDLE_VALUE != handle)
                    *out = handle;
                else
                    dwRet = ERROR_OUTOFMEMORY;
            }
        }
    }

    return dwRet;
}

DWORD ipcRegOpenKeyEx(HKEY hKey, LPCSTR lpSubKey, DWORD ulOptions,
                      DWORD samDesired, HKEY *phkResult)
{
    if(!IsValidHKey(hKey))
        return ERROR_INVALID_HANDLE;

    DWORD dwRet = ERROR_REGISTRY_IO_FAILED;

    RegMsg req, resp;
    req.opcode = op_RegOpenKeyEx;

    unsigned char* ptr = req.payload;
    PACK_U32( ptr, getKeyMaster().cookie );
    PACK_U32( ptr, reinterpret_cast<DWORD>(hKey) );
    PACK_U32( ptr, ulOptions );
    PACK_U32( ptr, samDesired );
    pack_string( ptr, lpSubKey );
    req.size = ptr - req.payload;

    dwRet = ipcTransactReg(req, resp);
    if(dwRet == ERROR_SUCCESS)
    {
        const unsigned char *ptr = (const unsigned char*)resp.payload;
        dwRet = UNPACK_U32(ptr);
        if ( dwRet == ERROR_SUCCESS )
        {
            if(phkResult)
            {
                // Store the new reg key.
                unsigned long regKey = UNPACK_U32(ptr);
                HKEY handle = CreateHandle(regKey);
                if(INVALID_HANDLE_VALUE != handle)
                    *phkResult = handle;
                else
                    dwRet = ERROR_OUTOFMEMORY;
            }
        }
    }

    return dwRet;
}

DWORD ipcRegDeleteKey(HKEY base, LPCSTR key)
{
    if(!IsValidHKey(base))
        return ERROR_INVALID_HANDLE;

    if(key == NULL)
        return ERROR_INVALID_PARAMETER;

    DWORD dwRet = ERROR_REGISTRY_IO_FAILED;

    RegMsg req, resp;
    req.opcode = op_RegDeleteKey;

    unsigned char* ptr = req.payload;
    PACK_U32( ptr, reinterpret_cast<DWORD>(base) );
    pack_string( ptr, key );

    req.size = ptr - req.payload;

    dwRet = ipcTransactReg(req, resp);
    if(dwRet == ERROR_SUCCESS)
    {
        const unsigned char *ptr = (const unsigned char*)resp.payload;
        dwRet = UNPACK_U32(ptr);
    }

    return dwRet;
}

DWORD ipcRegDeleteValue(HKEY base, LPCSTR value)
{
    if(!IsValidHKey(base))
        return ERROR_INVALID_HANDLE;

    if(value == NULL)
        value = emptyString;

    DWORD dwRet = ERROR_REGISTRY_IO_FAILED;

    RegMsg req, resp;
    req.opcode = op_RegDeleteValue;

    unsigned char* ptr = req.payload;
    PACK_U32( ptr, reinterpret_cast<DWORD>(base) );
    pack_string( ptr, value );

    req.size = ptr - req.payload;

    dwRet = ipcTransactReg(req, resp);
    if(dwRet == ERROR_SUCCESS)
    {
        const unsigned char *ptr = (const unsigned char*)resp.payload;
        dwRet = UNPACK_U32(ptr);
    }
    return dwRet;
}

DWORD ipcRegCreateKey(HKEY base, LPCSTR key, PHKEY out)
{
    if(!IsValidHKey(base))
        return ERROR_INVALID_HANDLE;

    // MSDN: If hKey is one of the predefined keys, lpSubKey may be NULL. In
    // that case, the handle returned by using phkResult is the same hKey
    // handle passed in to the function.
    // NOTE: This is not true for RegCreateKeyEx(), which specifies that
    // lpSubKey cannot be NULL.
    //
    // The server will also handle this properly, but this nicer to server.
    if( (!key || !*key) )
    {
        if( IS_PREDEF_HKEY(base) )
        {
            if(out)
                *out = base;
            return ERROR_SUCCESS;
        }
        else
            return ERROR_BAD_PATHNAME;
    }

    DWORD dwRet = ERROR_REGISTRY_IO_FAILED;

    RegMsg req, resp;
    req.opcode = op_RegCreateKey;

    unsigned char* ptr = req.payload;
    PACK_U32( ptr, getKeyMaster().cookie );
    PACK_U32( ptr, reinterpret_cast<DWORD>(base) );
    pack_string( ptr, key );

    req.size = ptr - req.payload;

    dwRet = ipcTransactReg(req, resp);
    if(dwRet == ERROR_SUCCESS)
    {
        const unsigned char *ptr = (const unsigned char*)resp.payload;
        dwRet = UNPACK_U32(ptr);
        if(dwRet == ERROR_SUCCESS)
        {
            if(out)
            {
                // Store the new reg key.
                unsigned long regKey = UNPACK_U32(ptr);
                HKEY handle = CreateHandle(regKey);
                if(INVALID_HANDLE_VALUE != handle)
                    *out = handle;
                else
                    dwRet = ERROR_OUTOFMEMORY;
            }
        }
    }
    return dwRet;
}

DWORD ipcRegCreateKeyEx(HKEY    hKey,
                        LPCSTR lpSubKey,
                        DWORD   Reserved,
                        LPSTR  lpClass,
                        DWORD   dwOptions,
                        DWORD   samDesired,
                        DWORD  *lpSecurityAttributes,
                        HKEY   *phkResult,
                        DWORD  *lpdwDisposition)
{
    // Validate the lpSubKey is not NULL and does not begin with backslash.
    if( (!lpSubKey || !*lpSubKey) || ('\\' == *lpSubKey) )
        return ERROR_BAD_PATHNAME;

    return ipcRegCreateKey(hKey, lpSubKey, phkResult);
}


DWORD ipcRegSetValueEx(HKEY hkey, LPCSTR value, DWORD reserved, DWORD type,
                       const BYTE* data, DWORD cbdata)
{
    if(!IsValidHKey(hkey))
        return ERROR_INVALID_HANDLE;

    if(!value)
        value = emptyString;

    DWORD dwRet = ERROR_REGISTRY_IO_FAILED;

    RegMsg req, resp;
    req.opcode = op_RegSetValueEx;

    unsigned char* ptr = req.payload;
    PACK_U32( ptr, reinterpret_cast<DWORD>(hkey) );
    PACK_U32( ptr, reserved );
    PACK_U32( ptr, type );
    PACK_U32( ptr, cbdata );
    PACK_U32( ptr, static_cast<DWORD>(data ? 1 : 0) );
    pack_string( ptr, value );
    if ( data && cbdata )
    {
        memcpy( ptr, data, cbdata );
        ptr += cbdata;
    }

    req.size = ptr - req.payload;

    dwRet = ipcTransactReg(req, resp);
    if(dwRet == ERROR_SUCCESS)
    {
        const unsigned char *ptr = (const unsigned char*)resp.payload;
        dwRet = UNPACK_U32(ptr);
    }

    return dwRet;
}

DWORD ipcRegQueryValueEx(HKEY hkey, LPCSTR value, LPDWORD reserved, LPDWORD type, LPBYTE data, LPDWORD cbdata)
{
    if(!IsValidHKey(hkey))
        return ERROR_INVALID_HANDLE;

    if(value == NULL)
        value = emptyString;

    DWORD dwRet = ERROR_REGISTRY_IO_FAILED;

    RegMsg req, resp;
    req.opcode = op_RegQueryValueEx;

    unsigned char* ptr = req.payload;
    PACK_U32( ptr, reinterpret_cast<DWORD>(hkey) );
    PACK_U32( ptr, static_cast<DWORD>(data ? 1 : 0) );
    PACK_U32( ptr, static_cast<DWORD>(cbdata ? 1 : 0) );
    PACK_U32( ptr, cbdata ? *cbdata : 0 );
    pack_string( ptr, value );

    req.size = ptr - req.payload;

    dwRet = ipcTransactReg(req, resp);
    if(dwRet == ERROR_SUCCESS)
    {
        const unsigned char *ptr = (const unsigned char*)resp.payload;
        dwRet = UNPACK_U32(ptr);
        if( dwRet == ERROR_SUCCESS || dwRet == ERROR_MORE_DATA )
        {
            DWORD originalSize = 0;
            DWORD dwType = UNPACK_U32(ptr);
            DWORD hasData = UNPACK_U32(ptr);
            DWORD dwCountReturned = UNPACK_U32(ptr);
            // ptr should now be at the actual data, if returned.

            if(type)
            {
                *type = dwType;
            }

            if(cbdata)
            {
                originalSize = *cbdata;
                *cbdata = dwCountReturned;
                if ( data && originalSize < dwCountReturned )
                {
                    dwRet = ERROR_MORE_DATA;
                }
            }

            if(data && hasData)
            {
                // User wants data filled in.
                // By spec, it is undefined what happens if the supplied
                // buffer was too small. for now we fill what we have.
                DWORD ammount = min(dwCountReturned, originalSize);
                if(ammount)
                {
                    memcpy( data, ptr, ammount );
                }
            }
        }
    }

    return dwRet;
}

DWORD ipcRegEnumValue(HKEY hkey, DWORD index, LPSTR value, LPDWORD cbvalue,
                      LPDWORD reserved, LPDWORD type, LPBYTE data,
                      LPDWORD cbdata)
{
    if(!IsValidHKey(hkey))
        return ERROR_INVALID_HANDLE;

    if ( data && !cbdata )
        return ERROR_INVALID_PARAMETER;

    if ( reserved )
        return ERROR_INVALID_PARAMETER;

    if ( !value || !cbvalue )
        return ERROR_INVALID_PARAMETER;

    DWORD dwRet = ERROR_REGISTRY_IO_FAILED;

    RegMsg req, resp;
    req.opcode = op_RegEnumValue;

    unsigned char* ptr = req.payload;
    PACK_U32(ptr, reinterpret_cast<DWORD>(hkey));
    PACK_U32( ptr, index );
    PACK_U32( ptr, *cbvalue );
    PACK_U32( ptr, static_cast<DWORD>(data ? 1 : 0) );
    PACK_U32( ptr, static_cast<DWORD>(cbdata ? 1 : 0) );
    PACK_U32( ptr, cbdata ? *cbdata : 0 );

    req.size = ptr - req.payload;

    dwRet = ipcTransactReg(req, resp);
    if(dwRet == ERROR_SUCCESS)
    {
        const unsigned char *ptr = (const unsigned char*)resp.payload;

        dwRet = UNPACK_U32(ptr);
        DWORD dwCbValueName = UNPACK_U32(ptr);
        DWORD dwType = UNPACK_U32(ptr);
        DWORD hasData = UNPACK_U32(ptr);
        DWORD dwCbData = UNPACK_U32(ptr);
        // ptr should now be at the actual name data
        const unsigned char *pData = ptr + dwCbValueName;

        // TODO revisit this later in regards to short name and data buffers
        if ( dwRet == ERROR_SUCCESS || dwRet == ERROR_MORE_DATA )
        {
            DWORD originalSize = 0;
            memcpy( value, ptr, dwCbValueName );
            *cbvalue = dwCbValueName;
            if ( type )
            {
                *type = dwType;
            }
            if ( cbdata )
            {
                originalSize = *cbdata;
                *cbdata = dwCbData;
                if ( data && originalSize < dwCbData )
                {
                    dwRet = ERROR_MORE_DATA;
                }
            }

            if ( data && hasData )
            {
                // User wants data filled in.
                // By spec, it is undefined what happens if the supplied
                // buffer was too small.  for now we fill what we have.
                DWORD ammount = min(dwCbData, originalSize);
                if ( ammount )
                {
                    memcpy( data, pData, ammount );
                }
            }
        }
    }
    return dwRet;
}

DWORD ipcRegEnumKey(HKEY hkey, DWORD index, LPSTR key, DWORD cbkey)
{
    if(!IsValidHKey(hkey))
        return ERROR_INVALID_HANDLE;

    if(!key)
        return ERROR_INVALID_PARAMETER;

    DWORD dwRet = ERROR_REGISTRY_IO_FAILED;

    RegMsg req, resp;
    req.opcode = op_RegEnumKey;

    unsigned char* ptr = req.payload;
    PACK_U32(ptr, reinterpret_cast<DWORD>(hkey));
    PACK_U32( ptr, index );
    PACK_U32( ptr, cbkey );

    req.size = ptr - req.payload;

    dwRet = ipcTransactReg(req, resp);
    if(dwRet == ERROR_SUCCESS)
    {
        const unsigned char *ptr = (const unsigned char*)resp.payload;
        dwRet = UNPACK_U32(ptr);
        if ( dwRet == ERROR_SUCCESS )
        {
            const char* pStr = unpack_string(ptr);
            strncpy( key, pStr, cbkey );
            key[cbkey - 1] = 0;
        }
    }

    return dwRet;
}

DWORD ipcRegEnumKeyEx(HKEY hkey, DWORD index, LPSTR key, LPDWORD cbKey,
                      LPDWORD reserved, LPSTR keyClass, LPDWORD cbClass,
                      PFILETIME lastWriteTime)
{
    if ( !key || !cbKey )
        return ERROR_INVALID_PARAMETER;

    if ( reserved )
        return ERROR_INVALID_PARAMETER;

    DWORD dwRet = ERROR_REGISTRY_IO_FAILED;

    dwRet = ipcRegEnumKey(hkey, index, key, *cbKey);
    if(dwRet == ERROR_SUCCESS)
    {
        *cbKey = strlen(key);
        if(keyClass)
            *keyClass = 0;
        if(cbClass)
            *cbClass = 0;
        if(lastWriteTime)
            *lastWriteTime = 0;
    }

    return dwRet;
}

DWORD ipcRegCloseKey(HKEY hkey)
{
    // Do not close the predefined keys.  If an attempt is made to close a
    // predefined key, then return ERROR_SUCCESS -- same as Windows behavior.
    if(IS_PREDEF_HKEY(hkey))
        return ERROR_SUCCESS;

    if(!IsValidHKey(hkey))
        return ERROR_INVALID_HANDLE;

    DWORD dwRet = ERROR_REGISTRY_IO_FAILED;

    RegMsg req, resp;
    req.opcode = op_RegCloseKey;

    unsigned char* ptr = req.payload;
    PACK_U32( ptr, reinterpret_cast<DWORD>(hkey) );

    req.size = ptr - req.payload;

    dwRet = ipcTransactReg(req, resp);
    if(dwRet == ERROR_SUCCESS)
    {
        const unsigned char *ptr = (const unsigned char*)resp.payload;
        dwRet = UNPACK_U32(ptr);
    }

    DeleteHandle(hkey);

    return dwRet;
}

LONG ipcRegNotifyChangeKeyValue(
  HKEY hKey,
  BOOL bWatchSubtree,
  DWORD dwNotifyFilter,
  HANDLE hEvent,
  BOOL fAsynchronous
)
{
    if(!IsValidHKey(hKey))
        return ERROR_INVALID_HANDLE;

    (void)hEvent;

    // only supported in synchronous mode
    if (FALSE != fAsynchronous)
    {
#ifdef DEBUG
        assert(FALSE == fAsynchronous);
#endif
        return ERROR_INVALID_PARAMETER;
    }

    DWORD dwWatchSubtree = (bWatchSubtree ? 1 : 0);

    RegMsg req, resp;

    req.opcode = op_RegNotifyChangeKeyValue;

    unsigned char* ptr = req.payload;
    PACK_U32( ptr, reinterpret_cast<DWORD>(hKey) );
    PACK_U32( ptr, dwWatchSubtree );
    PACK_U32( ptr, dwNotifyFilter );

    req.size = ptr - req.payload;

    DWORD dwRet = ipcTransactReg(req, resp );
    if (ERROR_SUCCESS == dwRet)
    {
        const unsigned char* p = resp.payload;
        dwRet = UNPACK_U32(p);
        if(dwRet == ERROR_SUCCESS)
        {
            DWORD semid = UNPACK_U32(p);
            DWORD sem_num = UNPACK_U32(p);

            // construct the operation structure for what needs to be done
            // to the semaphore:
            //   - first, which index in semaphore set (sem_id) to act on
            //   - second, what to do to semaphore (0 means to wait until
            //     the semaphore's value is 0)
            //   - third, flags, of which we specify none
            sembuf operation = { static_cast<unsigned short>(sem_num),
                                 0,
                                 0 };

            // now perform the operation.
            while (-1 == semop( semid, &operation, 1 ))
            {
                // If the operation did not succeed, it may have been
                // interrupted by a signal. We only want to break out and return
                // an error to the caller if the failure was NOT caused by a
                // signal. Otherwise, get right back to waiting on the semaphore
                if (EINTR != errno)
                {
                    dwRet = ERROR_REGISTRY_IO_FAILED;
                    break;
                }
            }
        }
    }

    return dwRet;
}


//=============================================================================
// Private functions for transferring data over IPC.
//=============================================================================


DWORD ipcTransactReg(RegMsg& req, RegMsg& resp)
{
    DWORD dwRet = ERROR_SUCCESS;
    IpcMessage msg;
    int expectTag;

    msg.protoVer = 0;
    msg.serviceVer = registry_service_version;
    msg.serviceId = registry_service_id;
    msg.flags = 0;

    if(!msg.ReallocPayload(req.size + RegMsg::hdr_len))
        return ERROR_OUTOFMEMORY;

    // Serialize the RegMsg header into the IpcMessage payload.
    req.Serialize(msg.payload);

    // Send the request message and read the response message.

    expectTag = msg.tag = static_cast<int>(pthread_self()) * 100;

    int sock = sockPool->GetSocket();
    if(-1 == sock)
        return ERROR_REGISTRY_IO_FAILED;

    if((dwRet = ipcSendMsg(sock, msg)) == ERROR_SUCCESS)
        dwRet = ipcReadMsg(sock, msg);

    sockPool->ReturnSocket(sock);

    if(dwRet == ERROR_SUCCESS)
    {
        if(msg.tag == expectTag)
        {
            // Unpackage the response from the IpcMessage.
            if(resp.Deserialize(msg.payload, msg.payloadLen))
            {
                // Opcodes will not match if responce is error due to not
                // being able to read resuest.
                if(req.opcode != resp.opcode)
                {
#ifdef BRUTE_FORCE_DEBUG
                    const char* errMsg = "unknown error";
                    switch(resp.opcode)
                    {
                    case op_errorServiceNotAvail:
                        errMsg = "service not available";
                        break;
                    case op_errorVersionNotSupported:
                        errMsg = "service version not supported";
                        break;
                    case op_errorIllegalPayloadSize:
                        errMsg = "illegal payload size";
                        break;
                    case op_errorIllegalOpcode:
                        errMsg = "illegal opcode";
                        break;
                    case op_errorMalformedPayload:
                        errMsg = "malformed payload";
                        break;
                    case op_errorNotImplemented:
                        errMsg = "function not implemented";
                        break;
                    case op_errorNoMemory:
                        errMsg = "out of memory";
                        break;
                    }
                    printf("server could not process request: %s\n", errMsg);
#endif // BRUTE_FORCE_DEBUG

                    dwRet = ERROR_FUNCTION_FAILED;
                }
            }
            else
                dwRet = ERROR_INVALID_DATA;
        }
        else
            dwRet = ERROR_INVALID_DATA;
    }

    return dwRet;
}


DWORD ipcSendMsg(int sock, IpcMessage& msg)
{
    DWORD result = ERROR_REGISTRY_IO_FAILED;
    int rc;
    timeval timeup;
    timeup.tv_usec = 0;
    timeup.tv_sec = SymRegIPC::ipc_timeout_sec;

    int outLen;
    const unsigned char* outData;

    // Get the serialized IpcMessage data.
    outData = msg.GetSerialData(&outLen);
    if(!outData)
        return ERROR_OUTOFMEMORY;

    rc = SocketComm::SyncWrite(sock, outData, outLen, &timeup, MSG_NOSIGNAL);
    if(rc > 0)
    {
        result = ERROR_SUCCESS;
    }
    else if ( rc == SocketComm::e_timeout )
        result = ERROR_BUSY;
    else if ( (rc == SocketComm::e_close) )
        result = ERROR_ACCESS_DENIED;
    else
        result = ERROR_REGISTRY_IO_FAILED;

    return result;
}

DWORD ipcReadMsg(int sock, IpcMessage& msg)
{
    DWORD result = ERROR_REGISTRY_IO_FAILED;
    unsigned char hdrBuf[IpcMessage::hdr_len];
    int rc;
    timeval timeup;
    timeup.tv_usec = 0;
    timeup.tv_sec = ipc_timeout_sec;

    // Read message header data.
    rc = SocketComm::SyncRead(sock, hdrBuf, IpcMessage::hdr_len, &timeup,
                              MSG_NOSIGNAL);
    if(rc > 0)
    {
        if(msg.Create(hdrBuf, IpcMessage::hdr_len) >= 0)
        {
            // Validate outer message header.
            if(msg.payloadLen < RegMsg::max_payload_len &&
               msg.serviceId == registry_service_id)
            {
                // Read messagge payload data.
                timeup.tv_usec = 0; // we set again, in case the read modified the value
                timeup.tv_sec = ipc_timeout_sec;
                rc = SocketComm::SyncRead( sock,
                                           msg.payload, msg.payloadLen,
                                           &timeup, MSG_NOSIGNAL);
                if(rc >= 0)
                    result = ERROR_SUCCESS;
                else if(rc == SocketComm::e_timeout)
                    result = ERROR_BUSY;
                else if ( (rc == SocketComm::e_close) )
                    result = ERROR_ACCESS_DENIED;
                else
                    result = ERROR_REGISTRY_IO_FAILED;
            }
#ifdef BRUTE_FORCE_DEBUG
            else
            {
                printf("Error: invalid message header:\n");
                printf("\tprotoVer: %d\n"\
                       "\tserviceVer: %d\n"\
                       "\tserviceId: %d\n"\
                       "\tflags: %d\n"\
                       "\ttag: %d\n"\
                       "\tpayloadLen: %d\n\n",
                       msg.protoVer, msg.serviceVer, msg.serviceId, msg.flags,
                       msg.tag, msg.payloadLen);
            }
#endif // BRUTE_FORCE_DEBUG
        }
        else
            result = ERROR_OUTOFMEMORY;

    }
    else if(rc == SocketComm::e_timeout)
        result = ERROR_BUSY;
    else if ( (rc == SocketComm::e_close) )
        result = ERROR_ACCESS_DENIED;
    else
        result = ERROR_REGISTRY_IO_FAILED;

    return result;
}

/**
 * Validate and HKEY value.
 *
 * @param hkey     [in] Handle that referrs to a particular key.
 *
 * @return true if HKEY valid for this clien, otherwise false.
 */
bool IsValidHKey(HKEY hkey)
{
    bool isValid = false;
    if ( hkey != INVALID_HANDLE_VALUE && hkey != NULL )
    {
        HKeySet::iterator findIter;
        unsigned long hval = reinterpret_cast<unsigned long>(hkey);

        pthread_mutex_lock(&getKeyMaster().hkSetMutex);

        findIter = getKeyMaster().hkeySet.find(hval);
        if(findIter != getKeyMaster().hkeySet.end())
        {
            isValid = true;
        }
        else if(IS_PREDEF_HKEY(hkey))
        {
            // Predefined keys should always exist, so if one was not found in
            // the valid handle set, then create a new one.

#ifdef BRUTE_FORCE_DEBUG
            printf("recovering predefined key\n");
#endif // BRUTE_FORCE_DEBUG

            if(hkey == HKEY_LOCAL_MACHINE)
            {
                getKeyMaster().CreateHKLM();
                if(getKeyMaster().symRegHKLM)
                    isValid = true;
            }
            else
            {
                getKeyMaster().CreateHKCU();
                if(getKeyMaster().symRegHKCU)
                    isValid = true;
            }
        }

        pthread_mutex_unlock(&getKeyMaster().hkSetMutex);
    }
    return isValid;
}

/**
 * Insert a new valid handle into the handle set.
 */
HKEY CreateHandle(unsigned long regKey)
{
    HKEY handle = reinterpret_cast<HKEY>(regKey);
    if(!IS_PREDEF_HKEY(handle))
    {
        std::pair<HKeySet::iterator, bool> result;

        pthread_mutex_lock(&getKeyMaster().hkSetMutex);

        result = getKeyMaster().hkeySet.insert(regKey);

        // There is something terribly wrong if this ever happens.
        if(result.second == false)
        {
            //syslog(LOG_CRIT, "duplicate handle found");
#ifdef BRUTE_FORCE_DEBUG
            printf("!!!! duplicate handle found\n");
#endif // BRUTE_FORCE_DEBUG
        }

        pthread_mutex_unlock(&getKeyMaster().hkSetMutex);
    }

    return handle;
}

/**
 * Remove the valid handle from handle set.
 */
void DeleteHandle(HKEY handle)
{
    unsigned long hval = reinterpret_cast<unsigned long>(handle);
    if ( handle != INVALID_HANDLE_VALUE && handle != NULL )
    {
        HKeySet::iterator setIter;

        pthread_mutex_lock(&getKeyMaster().hkSetMutex);

        setIter = getKeyMaster().hkeySet.find(hval);
        if(setIter != getKeyMaster().hkeySet.end())
        {
            // Remove handle from valid handle set.
            getKeyMaster().hkeySet.erase(setIter);

#ifdef BRUTE_FORCE_DEBUG
            printf("removed handle %lu from set of valid handles", hval);
#endif // BRUTE_FORCE_DEBUG
        }

        pthread_mutex_unlock(&getKeyMaster().hkSetMutex);
    }
}

