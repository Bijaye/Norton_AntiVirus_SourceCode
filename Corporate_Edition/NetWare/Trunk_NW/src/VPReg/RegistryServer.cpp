// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation. All rights reserved.


#include <string.h>
#include <sys/msg.h>

#include "winBaseTypes.h"
#include "RegistryServer.h"
#include "RegistryIPC.h"

#include "REGISTRY.H"

// Comment out these two lines in order to turn on debugging:
#undef dprintf
#define dprintf(x,...)

static int serverQid = -1;


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

static const char* unpack_string(const unsigned char*& ptr)
{
    DWORD len = unpack_dword(ptr);
    const char* ret = (const char*)ptr;
    ptr += len + 1;
    return ret;
}


void RegistryServerShutdown()
{
    if ( serverQid != -1 ) {
        int result = msgctl(serverQid, IPC_RMID, NULL);
        if ( result == -1 ) {
            dprintf( "REG_IPC: Error removing queue\n");
        } else {
            serverQid = -1;
        }
    }
}

void RegistryServer( void* )
{
    dprintf( "REG_IPC: Registry IPC Server up\n" );

    key_t key = ftok("/etc/hosts", 'b');
    dprintf( "REG_IPC:   the key is %ld\n", key );

    serverQid = msgget(key, 0666 | IPC_CREAT);
    if ( serverQid != -1 ) {
        struct msqid_ds ds;
        msgctl( serverQid, IPC_RMID, &ds );
        serverQid = msgget(key, 0666 | IPC_CREAT);
    }
    if ( serverQid != -1 ) {
        dprintf( "REG_IPC: Queue up   %d\n", serverQid);

        vpRegMsgbuf buf = {0};

        for(;;) {
            if (msgrcv(serverQid, (struct msgbuf *)&buf, sizeof(buf), REGIPC_SERVER, MSG_EXCEPT) == -1) {
	      if( EINTR == errno )
		continue;
	      else
                break;
            } else {
                vpRegMsgbuf resp = {0};
                dprintf( "REG_IPC: MESSAGE IN ----------------\n" );
                dprintf( "REG_IPC: Direction:%d\n", buf.mtype );
                switch ( buf.opcode ) {
                case REGIPC_OP_OPEN_KEY:
                {
                    dprintf( "REG_IPC:   RegOpenKey()\n");

                    DWORD base = 0;
                    const char* str = 0;
                    {
                        const unsigned char* ptr = (unsigned char*)buf.payload;
                        base = unpack_dword(ptr);
                        str = unpack_string(ptr);
                        dprintf( "REG_IPC:       base: %08x\n", (int)base);
                        dprintf( "REG_IPC:       key : '%s'\n", str );
                    }

                    // --------- process ------------

                    HKEY out = 0;

                    DWORD dwRet = RegOpenKey( reinterpret_cast<HKEY>(base), str, &out );

                    // --------- response ------------
                    resp.mtype = REGIPC_SERVER;
                    resp.opcode = buf.opcode;
                    resp.size = sizeof(resp);
                    {
                        char *ptr = resp.payload;

                        pack_dword(ptr, dwRet);
                        pack_dword(ptr, (DWORD)out);
                    }
                    int ret = msgsnd( serverQid, (struct msgbuf *)&resp, sizeof(resp), 0 );
                    if ( ret == -1 ) {
                        dprintf( "REG_IPC: Error sending response (%d)\n", errno );
                    } else {
                        dprintf( "REG_IPC: ------------------REPLY OUT 0x%08x\n", (int)dwRet );
                    }
                }
                break;
                case REGIPC_OP_OPEN_KEY_EX:
                {
                    dprintf( "REG_IPC:   RegOpenKeyEx()\n");

                    DWORD base = 0;
                    DWORD ulOptions = 0;
                    DWORD samDesired = 0;
                    const char* str = 0;
                    {
                        const unsigned char* ptr = (unsigned char*)buf.payload;
                        base = unpack_dword(ptr);
                        ulOptions = unpack_dword(ptr);
                        samDesired = unpack_dword(ptr);
                        str = unpack_string(ptr);
                        dprintf( "REG_IPC:       base   : %08x\n", (int)base);
                        dprintf( "REG_IPC:       options: %08x\n", (int)ulOptions);
                        dprintf( "REG_IPC:       SAM    : %08x\n", (int)samDesired);
                        dprintf( "REG_IPC:       key : '%s'\n", str );
                    }

                    // --------- process ------------

                    HKEY out = 0;

                    DWORD dwRet = RegOpenKeyEx( reinterpret_cast<HKEY>(base), str, ulOptions, samDesired, &out );

                    // --------- response ------------
                    resp.mtype = REGIPC_SERVER;
                    resp.opcode = buf.opcode;
                    resp.size = sizeof(resp);
                    {
                        char *ptr = resp.payload;

                        pack_dword(ptr, dwRet);
                        pack_dword(ptr, (DWORD)out);
                    }
                    int ret = msgsnd( serverQid, (struct msgbuf *)&resp, sizeof(resp), 0 );
                    if ( ret == -1 ) {
                        dprintf( "REG_IPC: Error sending response (%d)\n", errno );
                    } else {
                        dprintf( "REG_IPC: ------------------REPLY OUT 0x%08x\n", (int)dwRet );
                    }
                }
                break;
                case REGIPC_OP_DELETE_KEY:
                {
                    dprintf( "REG_IPC:   RegDeleteKey()\n");

                    DWORD base = 0;
                    const char* str = 0;
                    {
                      const unsigned char* ptr = (unsigned char*)buf.payload;
                      base = unpack_dword(ptr);
                      str = unpack_string(ptr);
                      dprintf( "REG_IPC:       base: %08x\n", (int)base);
                      dprintf( "REG_IPC:       key : '%s'\n", str );
                    }

                    // --------- process ------------

                    DWORD dwRet = RegDeleteKey( reinterpret_cast<HKEY>(base), str );

                    // --------- response ------------
                    resp.mtype = REGIPC_SERVER;
                    resp.opcode = buf.opcode;
                    resp.size = sizeof(resp);
                    {
                        char *ptr = resp.payload;

                        pack_dword(ptr, dwRet);
                    }
                    int ret = msgsnd( serverQid, (struct msgbuf *)&resp, sizeof(resp), 0 );
                    if ( ret == -1 ) {
                        dprintf( "REG_IPC: Error sending response (%d)\n", errno );
                    } else {
                        dprintf( "REG_IPC: ------------------REPLY OUT 0x%08x\n", (int)dwRet );
                    }
                }
                break;
                case REGIPC_OP_DELETE_VALUE:
                {
                    dprintf( "REG_IPC:   RegDeleteValue()\n");

                    DWORD base = 0;
                    const char* str = 0;
                    {
                      const unsigned char* ptr = (unsigned char*)buf.payload;
                      base = unpack_dword(ptr);
                      str = unpack_string(ptr);
                      dprintf( "REG_IPC:       base : %08x\n", (int)base);
                      dprintf( "REG_IPC:       value: '%s'\n", str );
                    }

                    // --------- process ------------

                    DWORD dwRet = RegDeleteValue( reinterpret_cast<HKEY>(base), str );

                    // --------- response ------------
                    resp.mtype = REGIPC_SERVER;
                    resp.opcode = buf.opcode;
                    resp.size = sizeof(resp);
                    {
                        char *ptr = resp.payload;

                        pack_dword(ptr, dwRet);
                    }
                    int ret = msgsnd( serverQid, (struct msgbuf *)&resp, sizeof(resp), 0 );
                    if ( ret == -1 ) {
                        dprintf( "REG_IPC: Error sending response (%d)\n", errno );
                    } else {
                        dprintf( "REG_IPC: ------------------REPLY OUT 0x%08x\n", (int)dwRet );
                    }
                }
                break;
                case REGIPC_OP_CREATE_KEY:
                {
                    dprintf( "REG_IPC:   RegCreateKey()\n");

                    DWORD base = 0;
                    const char* str = 0;
                    {
                        const unsigned char* ptr = (unsigned char*)buf.payload;
                        base = unpack_dword(ptr);
                        str = unpack_string(ptr);
                        dprintf( "REG_IPC:       base: %08x\n", (int)base);
                        dprintf( "REG_IPC:       key : '%s'\n", str );
                    }

                    // --------- process ------------

                    HKEY out = 0;
                    DWORD dwRet = RegCreateKey( reinterpret_cast<HKEY>(base), str, &out );

                    // --------- response ------------
                    resp.mtype = REGIPC_SERVER;
                    resp.opcode = buf.opcode;
                    resp.size = sizeof(resp);
                    {
                        char *ptr = resp.payload;

                        pack_dword(ptr, dwRet);
                        pack_dword(ptr, (DWORD)out);
                    }
                    int ret = msgsnd( serverQid, (struct msgbuf *)&resp, sizeof(resp), 0 );
                    if ( ret == -1 ) {
                        dprintf( "REG_IPC: Error sending response (%d)\n", errno );
                    } else {
                        dprintf( "REG_IPC: ------------------REPLY OUT 0x%08x\n", (int)dwRet );
                    }
                }
                break;
                case REGIPC_OP_SET_VALUE_EX:
                {
                    dprintf( "REG_IPC:   RegSetValueEx()\n");

                    DWORD hkey = 0;
                    DWORD reserved = 0;
                    DWORD type = 0;
                    DWORD cbdata = 0;
                    const char* value = 0;
                    const unsigned char* data = 0;

                    {
                        const unsigned char* ptr = (const unsigned char*)buf.payload;
                        hkey = unpack_dword(ptr);
                        reserved = unpack_dword(ptr);
                        type = unpack_dword(ptr);
                        cbdata = unpack_dword(ptr);
                        DWORD hasData = unpack_dword(ptr);
                        value = unpack_string(ptr);
                        if ( hasData ) {
                            data = ptr;
                        } else {
                            data = NULL;
                        }
                    }
                    dprintf( "REG_IPC:       hkey  : %08x\n", hkey);
                    dprintf( "REG_IPC:       resv  : %08x\n", reserved);
                    dprintf( "REG_IPC:       type  : %08x\n", type);
                    dprintf( "REG_IPC:       cbdata: %08x\n", cbdata);
                    dprintf( "REG_IPC:       value : '%s'\n", value);


                    // --------- process ------------

                    DWORD dwRet = RegSetValueEx( reinterpret_cast<HKEY>(hkey), value, reserved, type, data, cbdata);

                    // --------- response ------------
                    resp.mtype = REGIPC_SERVER;
                    resp.opcode = buf.opcode;
                    resp.size = sizeof(resp);
                    {
                        char *ptr = resp.payload;

                        pack_dword(ptr, dwRet);
                    }
                    int ret = msgsnd( serverQid, (struct msgbuf *)&resp, sizeof(resp), 0 );
                    if ( ret == -1 ) {
                        dprintf( "REG_IPC: Error sending response (%d)\n", errno );
                    } else {
                        dprintf( "REG_IPC: ------------------REPLY OUT 0x%08x\n", (int)dwRet );
                    }
                }
                break;
                case REGIPC_OP_QUERY_VALUE_EX:
                {
                    dprintf( "REG_IPC:   RegQueryValueEx()\n");

                    DWORD hkey = 0;
                    DWORD hasData = 0;
                    DWORD hasCbData = 0;
                    DWORD cbdata = 0;
                    const char* value = 0;

                    {
                        const unsigned char* ptr = (const unsigned char*)buf.payload;
                        hkey = unpack_dword(ptr);
                        hasData = unpack_dword(ptr);
                        hasCbData = unpack_dword(ptr);
                        cbdata = unpack_dword(ptr);
                        value = unpack_string(ptr);
                    }
                    dprintf( "REG_IPC:       hkey  : %08x\n", hkey);
                    dprintf( "REG_IPC:       cbdata: %08x\n", cbdata);
                    dprintf( "REG_IPC:       value : '%s'\n", value);


                    // --------- process ------------

                    DWORD type = 0;
                    BYTE* data = (BYTE*)(resp.payload + (4 * 4));
                    // TODO handle if we don't have the buffer space
                    DWORD dwRet = RegQueryValueEx( reinterpret_cast<HKEY>(hkey), value, NULL, &type, hasData ? data : NULL, hasCbData ? &cbdata : NULL );

                    // --------- response ------------
                    resp.mtype = REGIPC_SERVER;
                    resp.opcode = buf.opcode;
                    resp.size = sizeof(resp);
                    {
                        char *ptr = resp.payload;

                        pack_dword(ptr, dwRet);
                        pack_dword(ptr, type);
                        pack_dword(ptr, hasData);
                        pack_dword(ptr, cbdata);
                    }
                    int ret = msgsnd( serverQid, (struct msgbuf *)&resp, sizeof(resp), 0 );
                    if ( ret == -1 ) {
                        dprintf( "REG_IPC: Error sending response (%d)\n", errno );
                    } else {
                        dprintf( "REG_IPC: ------------------REPLY OUT 0x%08x\n", (int)dwRet );
                    }
                }
                break;
                case REGIPC_OP_ENUM_VALUE:
                {
                    dprintf( "REG_IPC:   RegEnumValue()\n");

                    DWORD hkey = 0;
                    DWORD index = 0;
                    DWORD cbvalue = 0;
                    DWORD hasData = 0;
                    DWORD hasCbData = 0;
                    DWORD cbdata = 0;

                    {
                        const unsigned char* ptr = (const unsigned char*)buf.payload;
                        hkey = unpack_dword(ptr);
                        index = unpack_dword(ptr);
                        cbvalue = unpack_dword(ptr);
                        hasData = unpack_dword(ptr);
                        hasCbData = unpack_dword(ptr);
                        cbdata = unpack_dword(ptr);
                    }
                    dprintf( "REG_IPC:       hkey  : %08x\n", hkey);
                    dprintf( "REG_IPC:       cbdata: %08x\n", cbdata);


                    // --------- process ------------

                    DWORD type = 0;
                    LPSTR value = resp.payload + (5 * 4);
                    dprintf( "REG_IPC:       value : '%s'\n", value);
                    BYTE* data = reinterpret_cast<BYTE*>(value + cbvalue);

                    DWORD dwRet = RegEnumValue( reinterpret_cast<HKEY>(hkey),
                                                index,
                                                value, &cbvalue,
                                                NULL,
                                                &type,
                                                hasData ? data : NULL,
                                                hasCbData ? &cbdata : NULL );

                    // --------- response ------------
                    resp.mtype = REGIPC_SERVER;
                    resp.opcode = buf.opcode;
                    resp.size = sizeof(resp);
                    {
                        char *ptr = resp.payload;

                        pack_dword(ptr, dwRet);
                        pack_dword(ptr, cbvalue);
                        pack_dword(ptr, type);
                        pack_dword(ptr, hasData);
                        pack_dword(ptr, cbdata);
                        // check that ptr == value
                    }
                    int ret = msgsnd( serverQid, (struct msgbuf *)&resp, sizeof(resp), 0 );
                    if ( ret == -1 ) {
                        dprintf( "REG_IPC: Error sending response (%d)\n", errno );
                    } else {
                        dprintf( "REG_IPC: ------------------REPLY OUT 0x%08x\n", (int)dwRet );
                    }
                }
                break;
                case REGIPC_OP_ENUM_KEY:
                {
                    dprintf( "REG_IPC:   RegEnumKey()\n");

                    DWORD key = 0;
                    DWORD index = 0;
                    DWORD cbname = 0;
                    {
                        const unsigned char* ptr = (unsigned char*)buf.payload;
                        key = unpack_dword(ptr);
                        index = unpack_dword(ptr);
                        cbname = unpack_dword(ptr);
                        dprintf( "REG_IPC:       key   : %08x\n", (int)key );
                        dprintf( "REG_IPC:       index : %d\n", (int)index );
                        dprintf( "REG_IPC:       cbname: %d\n", (int)cbname );
                    }

                    // --------- process ------------

                    LPSTR name = resp.payload + (2 * 4);
                    name[0] = 0;
                    DWORD dwRet = RegEnumKey( reinterpret_cast<HKEY>(key), index, name, cbname );

                    // --------- response ------------
                    resp.mtype = REGIPC_SERVER;
                    resp.opcode = buf.opcode;
                    resp.size = sizeof(resp);
                    {
                        char *ptr = resp.payload;

                        pack_dword(ptr, dwRet);

                        // must match pack_string:
                        pack_dword(ptr, (DWORD)strlen(name));
                        // ptr now should match 'name'
                    }
                    int ret = msgsnd( serverQid, (struct msgbuf *)&resp, sizeof(resp), 0 );
                    if ( ret == -1 ) {
                        dprintf( "REG_IPC: Error sending response (%d)\n", errno );
                    } else {
                        dprintf( "REG_IPC: ------------------REPLY OUT 0x%08x\n", (int)dwRet );
                    }
                }
                break;
                case REGIPC_OP_CLOSE_KEY:
                {
                    dprintf( "REG_IPC:   RegCloseKey()\n");
                    DWORD hkey = 0;
                    {
                        const unsigned char* ptr = (const unsigned char*)buf.payload;
                        hkey = unpack_dword(ptr);
                    }
                    dprintf( "REG_IPC:       hkey  : %08x\n", hkey);


                    // --------- process ------------

                    DWORD dwRet = RegCloseKey( reinterpret_cast<HKEY>(hkey) );

                    // --------- response ------------
                    resp.mtype = REGIPC_SERVER;
                    resp.opcode = buf.opcode;
                    resp.size = sizeof(resp);
                    {
                        char *ptr = resp.payload;

                        pack_dword(ptr, dwRet);
                    }
                    int ret = msgsnd( serverQid, (struct msgbuf *)&resp, sizeof(resp), 0 );
                    if ( ret == -1 ) {
                        dprintf( "REG_IPC: Error sending response (%d)\n", errno );
                    } else {
                        dprintf( "REG_IPC: ------------------REPLY OUT 0x%08x\n", (int)dwRet );
                    }
                }
                break;
                default:
                    dprintf( "REG_IPC:   unknown op: %d\n", buf.opcode );
                    // --------- response ------------
                    resp.mtype = REGIPC_SERVER;
                    resp.opcode = buf.opcode;
                    resp.size = sizeof(resp);
                    DWORD dwRet = ERROR_REGISTRY_IO_FAILED;
                    {
                        char *ptr = resp.payload;

                        pack_dword(ptr, dwRet);
                    }
                    int ret = msgsnd( serverQid, (struct msgbuf *)&resp, sizeof(resp), 0 );
                    if ( ret == -1 ) {
                        dprintf( "REG_IPC: Error sending response (%d)\n", errno );
                    } else {
                        dprintf( "REG_IPC: ------------------REPLY OUT 0x%08x\n", (int)dwRet );
                    }
                }
            }
        }


        dprintf( "REG_IPC: Shutting down\n");
        if ( serverQid != -1 ) {
            int result = msgctl(serverQid, IPC_RMID, NULL);
            if ( result == -1 ) {
                dprintf( "REG_IPC: Error removing queue\n");
            } else {
                serverQid = -1;
            }
        }
    } else {
        dprintf( "REG_IPC: Error opening msqid\n");
    }

  dprintf( "REG_IPC: Goodbye\n" );
}
