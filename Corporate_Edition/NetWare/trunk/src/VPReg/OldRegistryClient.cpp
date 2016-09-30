// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation. All rights reserved.


#include <string.h>
#include <sys/msg.h>

#include "winBaseTypes.h"
#include "RegistryIPC.h"

// Override the default behavior, and don't #define the registry calls
#define REGISTRY_CLIENT_IPC_EXPLICIT
#include "RegistryClient.h"

// extern "C" {
// #include "dprintf.h"
// #include "dprintf_2.h"
// #undef dprintf
// }

#include "REGISTRY.H"

// extern DEBUGFLAGTYPE debug;


static int msqid = -1;

void ipcRegOpenChannel()
{
    if ( msqid == -1 ) {
        key_t key = 0;
        key = ftok("/etc/hosts", 'b');
        msqid = msgget(key, 0666);
    }
}

void ipcRegCloseChannel()
{
}

static void pack_dword( char*& ptr, DWORD val )
{
    *ptr++ = (val & 0x000000ff) >> 0;
    *ptr++ = (val & 0x0000ff00) >> 8;
    *ptr++ = (val & 0x00ff0000) >> 16;
    *ptr++ = (val & 0xff000000) >> 24;
}

static DWORD unpack_dword(const unsigned char*& ptr)
{
    DWORD dwRet = 0;
    dwRet |= *ptr++;
    dwRet |= ((*ptr++) << 8);
    dwRet |= ((*ptr++) << 16);
    dwRet |= ((*ptr++) << 24);
    return dwRet;
}

static void pack_string( char*& ptr, const char* str )
{
    DWORD len = str ? (DWORD)strlen(str) : 0;
    pack_dword( ptr, len );
    if ( len > 0 ) {
        memcpy( ptr, str, len );
        ptr += len;
    }
    *ptr++ = 0;
}

static const char* unpack_string(const unsigned char*& ptr)
{
    DWORD len = unpack_dword(ptr);
    const char* ret = (const char*)ptr;
    ptr += len + 1;
    return ret;
}

DWORD ipcRegOpenKey(HKEY base, LPCSTR key, PHKEY out)
{
    DWORD dwRet = ERROR_REGISTRY_IO_FAILED;
    ipcRegOpenChannel();
    if ( msqid != -1 ) {
        vpRegMsgbuf buf;
        vpRegMsgbuf resp;
        buf.mtype = REGIPC_CLIENT;
        buf.opcode = REGIPC_OP_OPEN_KEY;
        buf.size = sizeof(buf);
        {
            char* ptr = buf.payload;

            pack_dword( ptr, (DWORD)base );
            pack_string( ptr, key );
        }

        int ret = msgsnd( msqid, (struct msgbuf *)&buf, sizeof(buf), 0 );
        if ( ret != -1 ) {
            // read server response
            ret = msgrcv(msqid, (struct msgbuf *)&resp, sizeof(resp), REGIPC_SERVER, 0);
            if ( ret != -1 ) {
                const unsigned char *ptr = (const unsigned char*)resp.payload;

                dwRet = unpack_dword(ptr);

                if ( dwRet == ERROR_SUCCESS ) {
                    DWORD newKey = unpack_dword(ptr);
                    if ( out ) {
                        *out = (HKEY)newKey;
                    }
                }
            }
        }
    }
    return dwRet;
}

DWORD ipcRegOpenKeyEx(HKEY hKey, LPCSTR lpSubKey, DWORD ulOptions, DWORD samDesired, HKEY *phkResult)
{
    DWORD dwRet = ERROR_REGISTRY_IO_FAILED;
    ipcRegOpenChannel();
    if ( msqid != -1 ) {
        vpRegMsgbuf buf;
        vpRegMsgbuf resp;
        buf.mtype = REGIPC_CLIENT;
        buf.opcode = REGIPC_OP_OPEN_KEY_EX;
        buf.size = sizeof(buf);
        {
            char* ptr = buf.payload;

            pack_dword( ptr, (DWORD)hKey );
            pack_dword( ptr, ulOptions );
            pack_dword( ptr, samDesired );
            pack_string( ptr, lpSubKey );
        }

        int ret = msgsnd( msqid, (struct msgbuf *)&buf, sizeof(buf), 0 );
        if ( ret != -1 ) {
            // read server response
            ret = msgrcv(msqid, (struct msgbuf *)&resp, sizeof(resp), REGIPC_SERVER, 0);
            if ( ret != -1 ) {
                const unsigned char *ptr = (const unsigned char*)resp.payload;

                dwRet = unpack_dword(ptr);

                if ( dwRet == ERROR_SUCCESS ) {
                    DWORD newKey = unpack_dword(ptr);
                    if ( phkResult ) {
                        *phkResult = (HKEY)newKey;
                    }
                }
            }
        }
    }
    return dwRet;
}

DWORD ipcRegDeleteKey(HKEY base, LPCSTR key)
{
    DWORD dwRet = ERROR_REGISTRY_IO_FAILED;
    ipcRegOpenChannel();
    if ( msqid != -1 ) {
        vpRegMsgbuf buf;
        vpRegMsgbuf resp;
        buf.mtype = REGIPC_CLIENT;
        buf.opcode = REGIPC_OP_DELETE_KEY;
        buf.size = sizeof(buf);
        {
          char* ptr = buf.payload;

          pack_dword( ptr, (DWORD)base );
          pack_string( ptr, key );
        }

        int ret = msgsnd( msqid, (struct msgbuf *)&buf, sizeof(buf), 0 );
        if ( ret != -1 ) {
            // read server response
            ret = msgrcv(msqid, (struct msgbuf *)&resp, sizeof(resp), REGIPC_SERVER, 0);
            if ( ret != -1 ) {
                const unsigned char *ptr = (const unsigned char*)resp.payload;

                dwRet = unpack_dword(ptr);
            }
        }
    }
    return dwRet;
}

DWORD ipcRegDeleteValue(HKEY base, LPCSTR value)
{
    DWORD dwRet = ERROR_REGISTRY_IO_FAILED;
    ipcRegOpenChannel();
    if ( msqid != -1 ) {
        vpRegMsgbuf buf;
        vpRegMsgbuf resp;
        buf.mtype = REGIPC_CLIENT;
        buf.opcode = REGIPC_OP_DELETE_VALUE;
        buf.size = sizeof(buf);
        {
          char* ptr = buf.payload;

          pack_dword( ptr, (DWORD)base );
          pack_string( ptr, value );
        }

        int ret = msgsnd( msqid, (struct msgbuf *)&buf, sizeof(buf), 0 );
        if ( ret != -1 ) {
            // read server response
            ret = msgrcv(msqid, (struct msgbuf *)&resp, sizeof(resp), REGIPC_SERVER, 0);
            if ( ret != -1 ) {
                const unsigned char *ptr = (const unsigned char*)resp.payload;

                dwRet = unpack_dword(ptr);
            }
        }
    }
    return dwRet;
}

DWORD ipcRegCreateKey(HKEY base, LPCSTR key, PHKEY out)
{
    DWORD dwRet = ERROR_REGISTRY_IO_FAILED;
    ipcRegOpenChannel();
    if ( msqid != -1 ) {
        vpRegMsgbuf buf;
        vpRegMsgbuf resp;
        buf.mtype = REGIPC_CLIENT;
        buf.opcode = REGIPC_OP_CREATE_KEY;
        buf.size = sizeof(buf);
        {
            char* ptr = buf.payload;

            pack_dword( ptr, (DWORD)base );
            pack_string( ptr, key );
        }

        int ret = msgsnd( msqid, (struct msgbuf *)&buf, sizeof(buf), 0 );
        if ( ret != -1 ) {
            // read server response
            ret = msgrcv(msqid, (struct msgbuf *)&resp, sizeof(resp), REGIPC_SERVER, 0);
            if ( ret != -1 ) {
                const unsigned char *ptr = (const unsigned char*)resp.payload;

                dwRet = unpack_dword(ptr);

                if ( dwRet == ERROR_SUCCESS ) {
                    DWORD newKey = unpack_dword(ptr);
                    if ( out ) {
                        *out = (HKEY)newKey;
                    }
                }
            }
        }
    }
    return dwRet;
}

DWORD ipcRegSetValueEx(HKEY hkey, LPCSTR value, DWORD reserved, DWORD type, const BYTE* data, DWORD cbdata)
{
    DWORD dwRet = ERROR_REGISTRY_IO_FAILED;
    ipcRegOpenChannel();
    if ( msqid != -1 ) {
        vpRegMsgbuf buf;
        vpRegMsgbuf resp;
        buf.mtype = REGIPC_CLIENT;
        buf.opcode = REGIPC_OP_SET_VALUE_EX;
        buf.size = sizeof(buf);
        char* ptr = buf.payload;

        pack_dword( ptr, (DWORD)hkey );
        pack_dword( ptr, reserved );
        pack_dword( ptr, type );
        pack_dword( ptr, cbdata );
        pack_dword( ptr, (DWORD)(data ? 1 : 0) );
        pack_string( ptr, value );
        if ( data ) {
            memcpy( ptr, data, cbdata );
            ptr += cbdata;
        }

        int ret = msgsnd( msqid, (struct msgbuf *)&buf, sizeof(buf), 0 );

        if ( ret != -1 ) {
            // read server response
            ret = msgrcv(msqid, (struct msgbuf *)&resp, sizeof(resp), REGIPC_SERVER, 0);
            if ( ret != -1 ) {
              const unsigned char *ptr = (const unsigned char*)resp.payload;

              dwRet = unpack_dword(ptr);
            }
        }
    }
    return dwRet;
}

DWORD ipcRegQueryValueEx(HKEY hkey, LPCSTR value, LPDWORD reserved, LPDWORD type, LPBYTE data, LPDWORD cbdata)
{
    DWORD dwRet = ERROR_REGISTRY_IO_FAILED;
    ipcRegOpenChannel();
    if ( data && !cbdata ) {
        dwRet = ERROR_INVALID_PARAMETER;
    } else if ( reserved ) {
        // 'reserved' must be null
        dwRet = ERROR_INVALID_PARAMETER;
    } else if ( msqid != -1 ) {
        vpRegMsgbuf buf;
        vpRegMsgbuf resp;
        buf.mtype = REGIPC_CLIENT;
        buf.opcode = REGIPC_OP_QUERY_VALUE_EX;
        buf.size = sizeof(buf);
        char* ptr = buf.payload;

        pack_dword( ptr, (DWORD)hkey );
        pack_dword( ptr, (DWORD)(data ? 1 : 0) );
        pack_dword( ptr, (DWORD)(cbdata ? 1 : 0) );
        pack_dword( ptr, cbdata ? *cbdata : 0 );
        pack_string( ptr, value );

        int ret = msgsnd( msqid, (struct msgbuf *)&buf, sizeof(buf), 0 );

        if ( ret != -1 ) {
            // read server response
            ret = msgrcv(msqid, (struct msgbuf *)&resp, sizeof(resp), REGIPC_SERVER, 0);
            if ( ret != -1 ) {
                const unsigned char *ptr = (const unsigned char*)resp.payload;

                dwRet = unpack_dword(ptr);
                if ( dwRet == ERROR_SUCCESS || dwRet == ERROR_MORE_DATA ) {
                    DWORD originalSize = 0;
                    DWORD dwType = unpack_dword(ptr);
                    DWORD hasData = unpack_dword(ptr);
                    DWORD dwCountReturned = unpack_dword(ptr);
                    // ptr should now be at the actual data, if returned.

                    if ( type ) {
                        *type = dwType;
                    }
                    if ( cbdata ) {
                        originalSize = *cbdata;
                        *cbdata = dwCountReturned;
                        if ( data && originalSize < dwCountReturned ) {
                            dwRet = ERROR_MORE_DATA;
                        }
                    }

                    if ( data && hasData ) {
                        // User wants data filled in.
                        // By spec, it is undefined what happens if the supplied buffer was too small.
                        // for now we fill what we have.
                        DWORD ammount = min(dwCountReturned, originalSize);
                        if ( ammount ) {
                            memcpy( data, ptr, ammount );
                        }
                    }
                }
            }
        }
    }
    return dwRet;
}

DWORD ipcRegEnumValue(HKEY hkey, DWORD index, LPSTR value, LPDWORD cbvalue, LPDWORD reserved, LPDWORD type, LPBYTE data, LPDWORD cbdata)
{
    DWORD dwRet = ERROR_REGISTRY_IO_FAILED;
    ipcRegOpenChannel();
    if ( data && !cbdata ) {
        dwRet = ERROR_INVALID_PARAMETER;
    } else if ( reserved ) {
        // 'reserved' must be null
        dwRet = ERROR_INVALID_PARAMETER;
    } else if ( !value || !cbdata ) {
        dwRet = ERROR_INVALID_PARAMETER;
    } else if ( msqid != -1 ) {
        vpRegMsgbuf buf;
        vpRegMsgbuf resp;
        buf.mtype = REGIPC_CLIENT;
        buf.opcode = REGIPC_OP_ENUM_VALUE;
        buf.size = sizeof(buf);
        char* ptr = buf.payload;

        pack_dword( ptr, (DWORD)hkey );
        pack_dword( ptr, index );
        pack_dword( ptr, *cbvalue );
        pack_dword( ptr, (DWORD)(data ? 1 : 0) );
        pack_dword( ptr, (DWORD)(cbdata ? 1 : 0) );
        pack_dword( ptr, cbdata ? *cbdata : 0 );

        int ret = msgsnd( msqid, (struct msgbuf *)&buf, sizeof(buf), 0 );

        if ( ret != -1 ) {
            // read server response
            ret = msgrcv(msqid, (struct msgbuf *)&resp, sizeof(resp), REGIPC_SERVER, 0);
            if ( ret != -1 ) {
                const unsigned char *ptr = (const unsigned char*)resp.payload;

                dwRet = unpack_dword(ptr);
                DWORD dwCbValueName = unpack_dword(ptr);
                DWORD dwType = unpack_dword(ptr);
                DWORD hasData = unpack_dword(ptr);
                DWORD dwCbData = unpack_dword(ptr);
                // ptr should now be at the actual name data
                const unsigned char *pData = ptr + dwCbValueName;

                // TODO revisit this later in regards to short name and data buffers
                if ( dwRet == ERROR_SUCCESS || dwRet == ERROR_MORE_DATA ) {
                    DWORD originalSize = 0;
                    memcpy( value, ptr, dwCbValueName );
                    *cbvalue = dwCbValueName;
                    if ( type ) {
                        *type = dwType;
                    }
                    if ( cbdata ) {
                        originalSize = *cbdata;
                        *cbdata = dwCbData;
                        if ( data && originalSize < dwCbData ) {
                            dwRet = ERROR_MORE_DATA;
                        }
                    }

                    if ( data && hasData ) {
                        // User wants data filled in.
                        // By spec, it is undefined what happens if the supplied buffer was too small.
                        // for now we fill what we have.
                        DWORD ammount = min(dwCbData, originalSize);
                        if ( ammount ) {
                            memcpy( data, pData, ammount );
                        }
                    }
                }
            }
        }
    }
    return dwRet;
}

DWORD ipcRegEnumKey(HKEY hkey, DWORD index, LPSTR key, DWORD cbkey)
{
    DWORD dwRet = ERROR_REGISTRY_IO_FAILED;
    ipcRegOpenChannel();
    if ( !key ) {
        dwRet = ERROR_INVALID_PARAMETER;
    } else if ( msqid != -1 ) {
        vpRegMsgbuf buf;
        vpRegMsgbuf resp;
        buf.mtype = REGIPC_CLIENT;
        buf.opcode = REGIPC_OP_ENUM_KEY;
        buf.size = sizeof(buf);
        char* ptr = buf.payload;

        pack_dword( ptr, (DWORD)hkey );
        pack_dword( ptr, index );
        pack_dword( ptr, cbkey );

        int ret = msgsnd( msqid, (struct msgbuf *)&buf, sizeof(buf), 0 );

        if ( ret != -1 ) {
            // read server response
            ret = msgrcv(msqid, (struct msgbuf *)&resp, sizeof(resp), REGIPC_SERVER, 0);
            if ( ret != -1 ) {
                const unsigned char *ptr = (const unsigned char*)resp.payload;

                dwRet = unpack_dword(ptr);
                if ( dwRet == ERROR_SUCCESS ) {
                    const char* pStr = unpack_string(ptr);
                    strncpy( key, pStr, cbkey );
                    key[cbkey - 1] = 0;
                }
            }
        }
    }
    return dwRet;
}

DWORD ipcRegCloseKey(HKEY hkey)
{
    DWORD dwRet = ERROR_REGISTRY_IO_FAILED;
    ipcRegOpenChannel();
    if ( msqid != -1 ) {
        vpRegMsgbuf buf;
        vpRegMsgbuf resp;
        buf.mtype = REGIPC_CLIENT;
        buf.opcode = REGIPC_OP_CLOSE_KEY;
        buf.size = sizeof(buf);
        char* ptr = buf.payload;

        pack_dword( ptr, (DWORD)hkey );

        int ret = msgsnd( msqid, (struct msgbuf *)&buf, sizeof(buf), 0 );

        if ( ret != -1 ) {
            // read server response
            ret = msgrcv(msqid, (struct msgbuf *)&resp, sizeof(resp), REGIPC_SERVER, 0);
            if ( ret != -1 ) {
              const unsigned char *ptr = (const unsigned char*)resp.payload;

              dwRet = unpack_dword(ptr);
            }
        }
    }
    return dwRet;
}

