// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#include "stdafx.h"

#include "vpcommon.h"
#include "TransmanBase.h"
#include "SymSaferStrings.h"

#include "savhelper.h"

// these are helper functions to deal with the nuts and bolts of remote communication
// and managing changing parents.

DWORD GetRemoteRegValue( const TCHAR *szServer, TCHAR *szKeyName, TCHAR *szValueName, LPBYTE Value, DWORD dwValueSize, int nRetries )
{
    BYTE  sendBuf[MAX_PACKET_DATA];

    DWORD dwItemsSent;
    DWORD dwRet = ERROR_WRITE_FAULT;

    for ( int i=0; i<nRetries; i++ )     // runs once even with 0 retries, which is the intent
    {
        memset(sendBuf, 0, MAX_PACKET_DATA);

        // build the registry key to read

        sssnprintf( (char *) sendBuf, sizeof(sendBuf), "%s", szValueName );

        // send == root key name,list of values  buf=[name0name0name00]
        // do get values reply. data of values (trunc if not fit) status = number of successful gets
        // reply buf=[WWdataWWdataWWdata]  WW=word len of data

        dwItemsSent = 1;
        dwRet = pfSendCOM_GET_VALUES((char *)(const char *)szServer, szKeyName, (char *) sendBuf, 0, &dwItemsSent);
        if ( dwRet == ERROR_SUCCESS )
        {
            Value[0] = 0;       // default return is empty value in case of error

            // return the value returned

            WORD wBytes = * (WORD *) sendBuf;
            BYTE *lpValue = (unsigned char *) sendBuf+2;

            if ( wBytes > 0 )
            {
                memcpy( Value, lpValue, wBytes );
                if ( wBytes < dwValueSize )
                {
                    // null terminate only if there is extra room
                    Value[wBytes] = 0;
                }
            }

            break;
        }

        // presume a transient problem, wait a bit, try again

        Sleep( 1000 );
    }

    // on time out I just return the actual error - there is no indicator that I tried more than once

    return dwRet;
}

DWORD SetRemoteRegValue( const TCHAR *szServer, TCHAR *szKeyName, TCHAR *szValueName, DWORD dwValueType, LPBYTE Value, DWORD dwValueSize, int nRetries )
{
    BYTE  packet[MAX_PACKET_DATA];

    DWORD dwItemsSent;
    DWORD used;
    DWORD dwRet = ERROR_WRITE_FAULT;

    for ( int i=0; i<nRetries; i++ )     // runs once even with 0 retries, which is the intent
    {
        dwItemsSent = 1;
        used = 0;

        memset(packet,0,MAX_PACKET_DATA);

        pfAddValueToBlock(packet,szValueName,dwValueType,(WORD)dwValueSize,(unsigned char *)Value,&used);

        dwRet = pfSendCOM_SET_VALUES((char *)szServer,szKeyName,1,packet,used);
        if ( dwRet == ERROR_SUCCESS )
        {
            break;
        }

        // presume a transient problem, wait a bit, try again

        Sleep( 1000 );
    }

    // on time out I just return the actual error - there is no indicator that I tried more than once

    return dwRet;
}

DWORD GetMachineAddressCacheValue( const TCHAR *szMachine, TCHAR *szAddrCacheEntry, TCHAR *szValueName, BYTE* Value, DWORD &dwSize, int nRetries )
{
    TCHAR sendBuf[MAX_PACKET_DATA];
    TCHAR szRootKey[512];

    DWORD dwItemsSent;
    DWORD dwRet = ERROR_WRITE_FAULT;

    for ( int i=0; i<nRetries; i++ )     // runs once even with 0 retries, which is the intent
    {
        memset(sendBuf, 0, MAX_PACKET_DATA); 

        _tcscpy( szRootKey, _T("AddressCache\\"));
        _tcscat( szRootKey, szAddrCacheEntry );
        _tcscat( szRootKey, _T("\\") );

        _tcscpy( sendBuf, szValueName );

        // send == root key name,list of values  buf=[name0name0name00]
        // do get values reply. data of values (trunc if not fit) status = number of successful gets
        // reply buf=[WWdataWWdataWWdata]  WW=word len of data

        dwItemsSent = 1;
        dwSize = 0;

        dwRet = pfSendCOM_GET_VALUES( (char *)szMachine, szRootKey, sendBuf, 0, &dwItemsSent);

        // return the value I got

        if ( ERROR_SUCCESS == dwRet )
        {
            dwSize = sendBuf[1] * 16 + sendBuf[0];

            memcpy( Value, sendBuf + 2, dwSize );

            break;
        }

        // presume a transient problem, wait a bit, try again

        Sleep( 1000 );
    }

    return dwRet;

}

DWORD SendFileToRemoteMachine( LPCTSTR szMachine, char *szLocalPath, char *szRemotePath, int nRetries )
{
    DWORD dwRet = ERROR_WRITE_FAULT;

    for ( int i=0; i<nRetries; i++ )     // runs once even with 0 retries, which is the intent
    {
        dwRet = pfMasterFileCopy( NULL, szLocalPath, (char *)szMachine, szRemotePath, COPY_SAVE_DATE | COPY_ALWAYS );
        if ( dwRet == ERROR_SUCCESS )
        {
            break;
        }

        // presume a transient problem, wait a bit, try again

        Sleep( 1000 );
    }

    // on time out I just return the actual error - there is no indicator that I tried more than once

    return dwRet;
}


DWORD BuildGRCFile( FILE *fGRCFile, const char *szParent, BOOL bNewGUID, LPBYTE GUID )
{
    DWORD dwRet;

    BYTE FossilGRCTime[8] = { 0, 0, 0, 0x47, 0x47, 0x47, 0x00, 0x00};

    // build a skeleton GRC file to set the parent and get the
    // full GRC file from the new parent

    _ftprintf( fGRCFile, "[KEYS]\n" );
    _ftprintf( fGRCFile, "!KEY!=$REGROOT$\n" );
    _ftprintf( fGRCFile, "AlertDirectory=S\\%s\\VPALERT$\n", szParent );
    _ftprintf( fGRCFile, "RemoteHomeDirectory=S\\%s\\VPHOME\n", szParent );
    _ftprintf( fGRCFile, "Parent=S%s\n", szParent );
    _ftprintf( fGRCFile, "FullGRCUpdateCounter=D1\n" );   // this is unused, but customarily set

    // this is a very old time, so I get new from parent
    _ftprintf( fGRCFile, "GRCUpdateTime=B%04x%02x%02x%02x%02x%02x%02x%02x%02x\n", 
               sizeof(FossilGRCTime),
               FossilGRCTime[0],
               FossilGRCTime[1],
               FossilGRCTime[2],
               FossilGRCTime[3],
               FossilGRCTime[4],
               FossilGRCTime[5],
               FossilGRCTime[6],
               FossilGRCTime[7] );

    if ( bNewGUID )
    {
        // give the machine a new GUID - that way the previous parent can't control me
        // does this cause any other confusion? only when moving the client back and forth
        // between the same machines - then the client entries multiply, but if I clean up
        // the old parent then that isn't an issue

        dwRet = CoCreateGuid( (_GUID *) GUID );
        if ( dwRet == ERROR_SUCCESS )
        {
            _ftprintf( fGRCFile, "GUID=B%04x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\n", 
                       sizeof(_GUID),
                       GUID[0],
                       GUID[1],
                       GUID[2],
                       GUID[3],
                       GUID[4],
                       GUID[5],
                       GUID[6],
                       GUID[7],
                       GUID[8],
                       GUID[9],
                       GUID[10],
                       GUID[11],
                       GUID[12],
                       GUID[13],
                       GUID[14],
                       GUID[15] );
        }
    }

    // now set up the address cache entry just in case that he can't find the new parent server

    {
        LONG lResult;

        HKEY hAddressCacheKey = NULL;
        HKEY hClientKey = NULL;

        TCHAR szAddrCache[512];

        _tcscpy( szAddrCache, _T("SOFTWARE\\Intel\\LANDesk\\VirusProtect6\\CurrentVersion\\AddressCache\\") );
        _tcscat( szAddrCache, szParent );
        _tcscat( szAddrCache, _T("\\") );

        lResult = RegOpenKeyEx( HKEY_LOCAL_MACHINE, szAddrCache, NULL, KEY_ALL_ACCESS, &hAddressCacheKey );
        if ( ERROR_SUCCESS == lResult )
        {

            DWORD dwGUIDSize = sizeof( GUID );

            _ftprintf( fGRCFile, "!KEY!=$REGROOT$\\AddressCache\n" );
            _ftprintf( fGRCFile, "!KEY!=$REGROOT$\\AddressCache\\%s\n", szParent );
            _ftprintf( fGRCFile, "good=D1\n" );

            // now add whatever address and protocol entries there are

            for ( int i=0; ; i++ )
            {
                TCHAR szValueName[512];
                DWORD dwValueNameSize = sizeof( szValueName );

                BYTE Value[1024];
                DWORD dwValueSize = sizeof( Value );

                DWORD dwType;

                lResult = RegEnumValue( hAddressCacheKey, i, szValueName, &dwValueNameSize, NULL, &dwType, Value, &dwValueSize );
                if ( ERROR_SUCCESS == lResult )
                {
                    if ( !stricmp( szValueName, "Protocol" ) ||
                         !stricmp( szValueName, "Address_0" ) ||
                         !stricmp( szValueName, "Address_1" ) )
                    {
                        switch ( dwType )
                        {
                            case REG_SZ:
                                {
                                    _ftprintf( fGRCFile, "%s=S%s\n", szValueName, Value );
                                    break;
                                }
                            case REG_DWORD:
                                {
                                    _ftprintf( fGRCFile, "%s=D%u\n", szValueName, *(DWORD *)&Value );
                                    break;
                                }
                            case REG_BINARY:
                                {
                                    _ftprintf( fGRCFile, "%s=B%04x", szValueName, dwValueSize );
                                    for ( DWORD j=0; j<dwValueSize; j++ )
                                    {
                                        _ftprintf( fGRCFile, "%02x", Value[j] );
                                    }

                                    _ftprintf( fGRCFile, "\n" );

                                    break;
                                }
                        }
                    }
                }
                else
                {
                    break;
                }
            }

            RegCloseKey( hAddressCacheKey );
        }
    }

    return dwRet;
}

DWORD PushGRCToClient( const char *szClient, const char *szFileName, int nRetries )
{
    DWORD dwRet;

    // good to go, send the grc file - this gets this moving - the client still belongs to the old parent
    // just in case something goes wrong I don't want to orphan it - I need to make checks later to
    // verify that the client moved properly

    dwRet = SendFileToRemoteMachine( szClient, "NewParnt.dat", "$\\grc.dat", nRetries );
    if ( dwRet == ERROR_SUCCESS )
    {
        // kick the machine to pick up the file - otherwise it takes a while

        DWORD dwDoIt;
        int   dwSecondsToWait = 120;

        dwDoIt = 1;
        dwRet = SetRemoteRegValue( szClient, "ProductControl\\", "ProcessGRCNow", REG_DWORD, (LPBYTE)&dwDoIt, sizeof(dwDoIt), nRetries );
        dwRet = GetRemoteRegValue( szClient, "ProductControl\\", "ProcessGRCNow", (LPBYTE)&dwDoIt, sizeof(dwDoIt), nRetries );

        // wait for the GRC to be processed

        time_t tEndTime = time( NULL ) + dwSecondsToWait;
        while ( dwRet == ERROR_SUCCESS && dwDoIt == 1 && time( NULL) < tEndTime )
        {
            Sleep( 1000 );
            dwRet = GetRemoteRegValue( szClient, "ProductControl\\", "ProcessGRCNow", (LPBYTE) &dwDoIt, sizeof(dwDoIt), nRetries );
        }

        if ( dwDoIt == 1 )
        {
            dwRet = ERROR__TIMEOUT;
        }
    }

    return dwRet;
}

DWORD NukeRemoteKey( const char *CName, const char *Root, int nRetries )
{
    DWORD dwRet = ERROR_WRITE_FAULT;

    for ( int i=0; i<nRetries; i++ )     // runs once even with 0 retries, which is the intent
    {
        dwRet = pfSendCOM_DEL_KEY( (char *)CName, (char *)Root);
        if ( dwRet == ERROR_SUCCESS )
        {
            break;
        }

        // presume a transient problem, wait a bit, try again

        Sleep( 1000 );
    }

    // on time out I just return the actual error - there is no indicator that I tried more than once

    return dwRet;
}

// this moves a client from the SourceParent to the TargetParent
// it returns ERROR_SUCCESS or an error - in either case csStatus is updated with a description of
// the result

DWORD MoveClient( CBA_Addr address, const CString &csFullClientName, const CString &csClientName, const CString &csSourceParent, const CString &csTargetParent, CString &csStatus )
{
    DWORD dwRet;
    DWORD dwProductVersion;

    const int nRetries = 3;

    // verify contact with the servers in question - if I can't reach them then I don't try the move

    dwRet = GetRemoteRegValue( csTargetParent, "", "ProductVersion", (LPBYTE) &dwProductVersion, sizeof(dwProductVersion), nRetries );
    if ( dwRet == ERROR_SUCCESS )
    {
        dwRet = GetRemoteRegValue( csSourceParent, "", "ProductVersion", (LPBYTE) &dwProductVersion, sizeof(dwProductVersion), nRetries );
        if ( dwRet == ERROR_SUCCESS )
        {
            dwRet = GetRemoteRegValue( (char *)&address, "", "ProductVersion", (LPBYTE) &dwProductVersion, sizeof(dwProductVersion), nRetries );
            if ( dwRet != ERROR_SUCCESS )
            {
                csStatus.Format( "Can't reach client %s", (LPCTSTR)csClientName );
            }
        }
        else
        {
            csStatus.Format( "Can't reach source server %s", (LPCTSTR) csSourceParent );
        }
    }
    else
    {
        csStatus.Format( "Can't reach target server %s", (LPCTSTR) csTargetParent );
    }

    // all the errors above are fatal for the client update

    if ( dwRet == ERROR_SUCCESS )
    {
        // I have clients to move - set up the GRC.DAT file to convert them - I need one for each client because I
        // set a new GUID for each - the new GUID prevents the old parent server from reconverting them

        BYTE GUID[16];
        const TCHAR szGRCFile[] = "NewParnt.dat"; 

        FILE *fNewParentGRCFile = fopen( szGRCFile, "w" );
        if ( fNewParentGRCFile )
        {
            dwRet = BuildGRCFile( fNewParentGRCFile, csTargetParent, TRUE, GUID );

            fclose( fNewParentGRCFile );
        }
        else
        {
            csStatus.Format( "Error building grc.dat to move client %s to parent %s.", csClientName, csTargetParent );

            dwRet = ERROR_FILE_NOT_FOUND;
        }

        if ( dwRet == ERROR_SUCCESS )
        {

            // good to go, send the grc file - this gets this moving - the client still belongs to the old parent
            // just in case something goes wrong I don't want to orphan it - I need to make checks later to
            // verify that the client picked up the new parent properly - this
            // mostly guards against the old server sending a colliding GRC file - 
            // otherwise everything should be fine

            for ( int i=0; i<nRetries; i++ )
            {
                dwRet = PushGRCToClient( (char *)&address, "NewParnt.dat", nRetries );
                if ( dwRet == ERROR_SUCCESS )
                {
                    // woo hoo - pushing the grc worked - i should be done - double check the parent, and clean up at the old server

                    char szParent[50];

                    dwRet = GetRemoteRegValue( (char *)&address, "", "Parent", (LPBYTE)&szParent, sizeof(szParent), nRetries );
                    if ( dwRet == ERROR_SUCCESS )
                    {
                        if ( strcmp( szParent, csTargetParent ) )
                        {
                            // the GRC was processed, but the parent didn't change - a sign that I collided
                            // with another incoming GRC - 

                            // so I send it again - this really shouldn't be needed more than once since the administrator
                            // can't change things that quickly

                            dwRet = ERROR_WRITE_FAULT;      // I set this error in case I fall out of the timed loop - I didn't get an actual error
                        }
                        else
                        {
                            break;
                        }
                    }
                }
            }

            if ( dwRet == ERROR_SUCCESS )
            {
                // the client parent was good - I am pretty much done at this point

                // the client is moved - now just double check with the parent so I know I didn't
                // just orphan him to a parent that won't care for him

                // first wait to give the check-in a little time to get there - it should be pretty much
                // instantaneous, but you never know - 

                DWORD dwGood = 0;

                for ( int i=0; i<nRetries; i++ )
                {
                    Sleep( 1000 );

                    char szFullClientName[100];
                    sssnprintf( szFullClientName, sizeof(szFullClientName), "Clients\\%s_::_%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X", 
                               csClientName,
                               GUID[3], GUID[2], GUID[1], GUID[0],
                               GUID[7], GUID[6], GUID[5], GUID[4],
                               GUID[11], GUID[10], GUID[9], GUID[8],
                               GUID[15], GUID[14], GUID[13], GUID[12] );

                    dwGood = 0;
                    dwRet = GetRemoteRegValue( csTargetParent, szFullClientName, "good", (LPBYTE)&dwGood, sizeof(dwGood), nRetries );
                    if ( dwRet == ERROR_SUCCESS && dwGood == 1 )
                    {
                        break;
                    }
                }

                if ( dwRet == ERROR_SUCCESS && dwGood == 1 )
                {
                    // all good - there only remains the old parent to clean up - he has the wrong GUID
                    // so he can't create any mischief, but the old client is still there in the list

                    // cleaning this up is my last official act

                    dwRet = NukeRemoteKey(csSourceParent, "Clients\\" + csFullClientName, nRetries );
                    if ( dwRet )
                    {
                        // this is a non-fatal error - the old parent can't do anything with the client - I changed the GUID!
                        // so this will eventually fade away

                        csStatus.Format( "warning - problem removing client %s at old parent %s, error %x.", (LPCTSTR) csClientName, (LPCTSTR) csSourceParent, dwRet );
                    }
                }
                else
                {
                    if ( dwGood != 1 )
                    {
                        csStatus.Format( "client %s hasn't showed up at the new parent %s.", (LPCTSTR) csClientName, (LPCTSTR) csTargetParent );

                        // so what do I do here - the client has everything it needs - AddressCache entries, parent name, etc.
                        // I assume that it will get there eventually
                    }
                    else
                    {
                        csStatus.Format( "Error checking client %s check-in at the new parent %s, error %x.", (LPCTSTR) csClientName, (LPCTSTR) csTargetParent, dwRet );

                        // can't talk to the new parent to verify anything - the client is good to go, so leave it as it is
                        // they will get the server back up quickly - I am now telling them there is a problem with it.
                    }
                }
            }
            else
            {
                csStatus.Format( "Error setting parent %s at client %s, error %x.", (LPCTSTR) csTargetParent, (LPCTSTR) csClientName, dwRet );
            }
        }

        unlink( szGRCFile );
    }

    if ( dwRet == ERROR_SUCCESS )
    {
        csStatus.Format( "Client %s changed parent from %s to %s.", csClientName, csSourceParent, csTargetParent );
    }

    return dwRet;
}

