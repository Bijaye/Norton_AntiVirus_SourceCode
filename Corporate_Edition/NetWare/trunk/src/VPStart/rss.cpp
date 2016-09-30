// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#include "pscan.h"
#include "nlmui.h"
#include "status.h"
#include "nlm/nwfinfo.h"
#include "slash.h"
#include "SymSaferRegistry.h"
#include "SymSaferStrings.h"

#define EX_NO_OPEN_SERV 1

#define rssPrintf dprintf
#define dPrintf dprintf 
#define LOG if(log_file) fprintf

#define KLUDGE_FILE		"templist.dat"
#define TEMP_DIR		"TempInst"

#define PAUSE(x) 
//{rssPrintf(x);getch();}

extern "C" char line[300] = "";
extern "C" char RSSDir[255] = "";
extern "C" char ProgramDir[];

DWORD LoadRegistryFunctions(int compatibility);
DWORD UnloadRegistryFunctions(void);
extern "C" BOOL RegFunctionsLoaded;
void CopyTempFiles(char* path);  // gdf CW conversion
void CopyOneFile(char *src);   // gdf CW conversion


extern "C" FILE* log_file=NULL;

#define COPY_FILES		0
#define VERIFY_SIZES	1
#define DELETE_DIRS		2

extern "C" CBA_Addr CSAddr[2] = {0};
extern "C" CBA_Addr MyAddr[2] = {0};

extern "C" char StatusValue[60] = "";
extern "C" char ErrorValue [60] = "";
extern "C" char CommandValue[60] = "";
extern "C" HKEY hRemote=0;
extern "C" HKEY hError=0;
VOID eprintf(DWORD Err, char *format,...);

extern "C" DWORD ErrorCount;

static int CheckTempFileSizes(char *path);
static int CycleDirs(char *path,int verify);
static int CheckFileExists();
static int CheckFileSize( char* file, long size );

#ifndef FILE_ATTRIBUTE_DIRECTORY
#define FILE_ATTRIBUTE_DIRECTORY    _A_SUBDIR
#endif // FILE_ATTRIBUTE_DIRECTORY

#ifndef IsDir
#define IsDir(x) ((x) & FILE_ATTRIBUTE_DIRECTORY) // Do I care about _A_SYSTEM
#endif // IsDir


/**********************************************************************************************************/
// these are from VPREMOVE:
char modules[20][10]={"snmphndl","nlmxhndl","bcsthndl","iaostg","iaosnmp","iao","hndlrsvc","amslib","amsdb","msgsrv","msgsys",":END:"};
/***********************************************************************/
#pragma argsused

extern "C" int CSprot=0;
static char g_sServer[50] = {0};

DWORD OpenHRemote(DWORD msg) {

	DWORD dwRet;

	if(!RegFunctionsLoaded)
		return msg;

	if(!hRemote) {
		if((dwRet=RegCreateKey( HKEY_LOCAL_MACHINE, REGHEADER"\\"UPGRADE_KEY, &hRemote ))!=ERROR_SUCCESS)
			return dwRet;

		GetFileServerName(0,g_sServer);
		strupr(g_sServer);
		sssnprintf(StatusValue, sizeof(StatusValue), "%s"_STATUS, g_sServer);
		sssnprintf(ErrorValue, sizeof(ErrorValue), "%s"_ERROR, g_sServer);
	}
	return ERROR_SUCCESS;
}
////////////////////////////////////////////////////////////////////////////////////////////////
DWORD ResetCommandBlock(void) 
{
	BYTE packet[MAX_PACKET_DATA];
	char value[60];
	DWORD dwVal=0,used=1,dwRet;
	sssnprintf(value, sizeof(value), "%s"_COMMAND, g_sServer);

	memset(packet,0,sizeof(packet));
	AddValueToBlock(packet,value,REG_DWORD,sizeof(DWORD),(BYTE*)&dwVal,&used);

	dwRet=SendCOM_SET_VALUES (NULL, reinterpret_cast<char*>(&CSAddr[CSprot]), SENDCOM_REMOTE_IS_SERVER | SENDCOM_REMOTE_USES_INSECURE_COMM, 
                              UPGRADE_KEY, 1, packet, used);

	if(dwRet!=ERROR_SUCCESS)
    {
		CSprot = CSprot ? 0 : 1;
		dwRet=SendCOM_SET_VALUES (NULL, reinterpret_cast<char*>(&CSAddr[CSprot]), SENDCOM_REMOTE_IS_SERVER | SENDCOM_REMOTE_USES_INSECURE_COMM, 
                                  UPGRADE_KEY, 1, packet, used);
	}

	return dwRet;
}
////////////////////////////////////////////////////////////////////////////////////////////////
DWORD SendErrorsToCopyServer() {
	
	DWORD dwRet=ERROR_SUCCESS;
	BYTE packet[MAX_PACKET_DATA];
	DWORD used=0;
	DWORD i;
	char Value[64];
	BYTE data[MAX_PACKET_DATA-128];
	DWORD cbVal,type,cbData;

	if((dwRet=OpenHRemote(-1))!=ERROR_SUCCESS)
		return dwRet;

	if( !hError ) {
		if((dwRet=RegCreateKey( hRemote, "Errors", &hError )) != ERROR_SUCCESS)
			return dwRet;
	}

	memset(packet,0,sizeof(packet));

	for( i=0 ; RegEnumValue( hError,i,Value,&cbVal,NULL,&type,data,&cbData)==ERROR_SUCCESS ; i++,cbData=sizeof(data),cbVal=sizeof(Value),type=REG_SZ ) {
		used=0;
		AddValueToBlock( packet, Value, REG_SZ, strlen(reinterpret_cast<char*>(data)), (BYTE*)data, &used);

		dwRet=SendCOM_SET_VALUES( NULL, reinterpret_cast<char*>(&CSAddr[CSprot]), SENDCOM_REMOTE_IS_SERVER | SENDCOM_REMOTE_USES_INSECURE_COMM, 
                                  TEMP_ERROR_KEY, 1, packet, used );

		if(dwRet!=ERROR_SUCCESS) {
			CSprot = CSprot ? 0 : 1;
			dwRet=SendCOM_SET_VALUES( NULL, reinterpret_cast<char*>(&CSAddr[CSprot]), SENDCOM_REMOTE_IS_SERVER | SENDCOM_REMOTE_USES_INSECURE_COMM, 
                                      TEMP_ERROR_KEY, 1, packet, used );
		}
		if(dwRet!=ERROR_SUCCESS) 
			return dwRet;
	}
	return ERROR_SUCCESS;
}
////////////////////////////////////////////////////////////////////////////////////////////////
DWORD CheckCopyServerCommand() {

	DWORD dwRet=ERROR_SUCCESS;
	BYTE packet[MAX_PACKET_DATA];
	BYTE *b;
	DWORD used=1;
	static int paused;
	WORD size;


	memset(packet,0,sizeof(packet));

	if(!strlen(g_sServer)) {
		GetFileServerName(0,g_sServer);
		strupr(g_sServer);
	}
	sssnprintf(reinterpret_cast<char*>(packet), sizeof(packet), "%s"_COMMAND, g_sServer);

	dwRet=SendCOM_GET_VALUES( NULL, reinterpret_cast<char*>(&CSAddr[CSprot]), SENDCOM_REMOTE_IS_SERVER | SENDCOM_REMOTE_USES_INSECURE_COMM, 
                              UPGRADE_KEY, reinterpret_cast<char*>(packet), sizeof (packet), &used );

	if(dwRet!=ERROR_SUCCESS) {
		CSprot = CSprot ? 0 : 1;
		dwRet=SendCOM_GET_VALUES( NULL, reinterpret_cast<char*>(&CSAddr[CSprot]), SENDCOM_REMOTE_IS_SERVER | SENDCOM_REMOTE_USES_INSECURE_COMM, 
                                  UPGRADE_KEY, reinterpret_cast<char*>(packet), sizeof (packet), &used );
	}
	if(dwRet!=ERROR_SUCCESS) 
		return dwRet;

	b = GetValueFromBlock(packet,0,&size);

	switch((*(DWORD*)b) & 0xffff)
	{
	case CMD_PAUSE:
		paused++;
		if(paused==1) {
			while(paused) {
				CheckCopyServerCommand();
				ThreadSwitchWithDelay();
				NTxSleep(300);
			}
		}
		else
			paused--;
		break;
	case CMD_RESUME:
		paused=0;
		break;
	case CMD_CANCEL:
		paused=0;
		return EI_ABORT_INSTALL;
		break;
	case CMD_CLOSE:
		paused=0;
		return EI_CLOSE_INSTALL;
		break;
	case CMD_ERRORS:
		SendErrorsToCopyServer();
		ResetCommandBlock();
		break;
	}
	return ERROR_SUCCESS;
}
////////////////////////////////////////////////////////////////////////////////////////////////
DWORD UpdateUpgradeStatus(DWORD status) {

	DWORD dwRet=ERROR_SUCCESS;
	BYTE packet[MAX_PACKET_DATA];
	DWORD used=0;

	dwRet=OpenHRemote(status);
	if(dwRet!=ERROR_SUCCESS)
		return dwRet;

	status |= (ErrorCount & 0xffff);

	dwRet=RegSetValueEx( hRemote, StatusValue, 0, REG_DWORD, (BYTE*)&status, sizeof(DWORD) );

	memset(packet,0,sizeof(packet));

	AddValueToBlock( packet, StatusValue, REG_DWORD, sizeof(DWORD), (BYTE*)&status, &used);

	dwRet=SendCOM_SET_VALUES( NULL, reinterpret_cast<char*>(&CSAddr[CSprot]), SENDCOM_REMOTE_IS_SERVER | SENDCOM_REMOTE_USES_INSECURE_COMM,
                              UPGRADE_KEY, 1, packet, used );

	if(dwRet!=ERROR_SUCCESS) {
		CSprot = CSprot ? 0 : 1;
		dwRet=SendCOM_SET_VALUES( NULL, reinterpret_cast<char*>(&CSAddr[CSprot]), SENDCOM_REMOTE_IS_SERVER | SENDCOM_REMOTE_USES_INSECURE_COMM, 
                                  UPGRADE_KEY, 1, packet, used );
	}
	
	return dwRet;
}
////////////////////////////////////////////////////////////////////////////////////////////////
DWORD UpdateUpgradeError(DWORD error, char* ErrMsg) {

	DWORD dwRet=ERROR_SUCCESS;
//	DWORD used=0;
//	char ErrorVal[16];
//	BYTE packet[MAX_PACKET_DATA];

LOG(log_file,"UpdateUpgradeError (0x%08x,%s)...",error,ErrMsg);

	dwRet=OpenHRemote(error);
	if(dwRet!=ERROR_SUCCESS) 
	{
		return dwRet;
	}
	if( !hError ) {
		if((dwRet=RegCreateKey( hRemote, "Errors", &hError )) != ERROR_SUCCESS)
		{
			return dwRet;
		}
	}

//	sssnprintf( ErrorVal, sizeof(ErrorVal), "0x%08x", error );
	dwRet=RegSetValueEx( hError, ErrMsg, 0, REG_DWORD, (BYTE*)&error, sizeof(DWORD) );
	
	if(dwRet==ERROR_SUCCESS)
		ErrorCount++;

	RegSetValueEx( hRemote, ErrorValue, 0, REG_DWORD, (BYTE*)&error, sizeof(DWORD) );

//	memset(packet,0,sizeof(packet));

//	AddValueToBlock( packet, ErrorValue, REG_DWORD, sizeof(DWORD), (BYTE*)&error, &used);

//	dwRet=SendCOM_SET_VALUES( NULL, (BYTE*)&CSAddr[CSprot], SENDCOM_REMOTE_IS_SERVER, UPGRADE_KEY, 1, packet, used );

//	if(dwRet!=ERROR_SUCCESS) {
//		CSprot = CSprot ? 0 : 1;
//		dwRet=SendCOM_SET_VALUES( NULL, (BYTE*)&CSAddr[CSprot], SENDCOM_REMOTE_IS_SERVER, UPGRADE_KEY, 1, packet, used );
//	}


LOG(log_file,"%d (0x%08x)\n",ErrorCount, dwRet);

	return dwRet;
}

/***********************************************************************/

void UnloadModule(char *name) {

	char line[64];
	int  cnt=1500;

rssPrintf ("unload %s-->",name);
LOG(log_file,"unload %s-->",name);
	if (!FindNLMHandle(name)) {
		rssPrintf ("UNLOADED\n");
LOG(log_file,"UNLOADED\n");
		return;
	}
	sssnprintf(line,sizeof(line),"UNLOAD %s",name);
	system(line);

	do {
		NTxSleep(100);
	} while (FindNLMHandle(name) && cnt-- > 0);
	rssPrintf ("unloaded\n");
LOG(log_file,"unloaded\n");
}

/****************************************************************************************/

static DWORD GetCBAAddr(const char* sServer, SENDCOM_HINTS hints, CBA_Addr *addr)
{
	HKEY hkey=0;
	char addrKey[128],Prot[32];
	DWORD dwRet=0,dwSize=0;
	
	sssnprintf( addrKey, sizeof(addrKey), REGHEADER"\\AddressCache\\%s", sServer );

	dwRet=RegCreateKey(HKEY_LOCAL_MACHINE, addrKey, &hkey );
	if( dwRet == ERROR_SUCCESS )
	{
		dwSize=sizeof(Prot);
		dwRet=SymSaferRegQueryValueEx( hkey, "Protocol", NULL, NULL, reinterpret_cast<unsigned char*>(Prot), &dwSize );
		if( dwRet == ERROR_SUCCESS )
		{		
			dwSize=sizeof(CBA_Addr);
			dwRet=SymSaferRegQueryValueEx( hkey, Prot, NULL, NULL, (BYTE*)addr, &dwSize );
		}
		RegCloseKey( hkey );
	}

	if( dwRet!=ERROR_SUCCESS ) // if we couldn't read the address out of the registry, 
	{
		unsigned short prot = CBA_PROTOCOL_IPX;
		CBA_NETADDR rawAddress;

		memset(&rawAddress,0,sizeof(CBA_NETADDR));
		
		if(ResolveAddress( sServer, &rawAddress, &prot )) {
			int i;

			NTSBuildAddr(&rawAddress,addr,0,(unsigned char)prot);// this address may not work!
			for(i=0 ; i<32 ; i++)
				dprintf("%02x ",*((BYTE*)addr+i));

			dprintf("\n");
			VerifyAnAddress(reinterpret_cast<char*>(addr), hints);
		}
	}

	return dwRet;
}

/****************************************************************************************/
static DWORD GetOldRtvscanVersion(CBA_Addr *addr)
{
	PONGDATA pd;

	if (SendCOM_GET_PONG_DATA(NULL, reinterpret_cast<char*>(addr), SENDCOM_REMOTE_IS_SERVER | SENDCOM_REMOTE_USES_INSECURE_COMM, &pd) != ERROR_SUCCESS)
		return 0;

	return (pd.ProductVersion&0xffff)/100; // we only want to return the major version, not minor version or build number
}

/****************************************************************************************/

DWORD IncrementRegValue( HKEY hkey, char *val )
{
	DWORD oldVal = GetVal( hkey, val, 2 ) + 1;
	return RegSetValueEx( hkey, val, 0, REG_DWORD, (BYTE*)&oldVal, sizeof(DWORD) );
}

/****************************************************************************************/

DWORD SendUnloadToRtvscan() {

	DWORD dwRet=0,i;
	char sServer[50];
	CBA_Addr addr;
	time_t t;
	int pct=0;
	DWORD Version=0;

	DWORD ShutdownType = SERVER_SHUTDOWN_AND_WAIT;

	memset( sServer, 0, sizeof(sServer) );
	memset( &addr, 0, sizeof(CBA_Addr) );

	UpdateUpgradeStatus(STS_STOPPING_SERVICE);

rssPrintf ("Trying to send server unload command\n");
LOG(log_file,"Trying to send server unload command\n");

	GetFileServerName(0,sServer);
rssPrintf ("servername: %s\n",sServer);
LOG(log_file,"servername: %s\n",sServer);

	Version=GetOldRtvscanVersion(&MyAddr[CSprot]);

rssPrintf("Version of Old rtvscan is %d\n",Version);
LOG(log_file,"Version of Old rtvscan is %d\n",Version);

	if( Version ) 
	{
		if( Version < MAINPRODUCTVERSION )
		{

rssPrintf("Uninstall the Old rtvscan\n");
LOG(log_file,"Uninstall the Old rtvscan\n");
			ShutdownType = SERVER_SERVICE_REMOVE; // we want to Uninstall old NW rtvscan

			if(!hMainKey) dwRet=RegCreateKey(HKEY_LOCAL_MACHINE,REGHEADER,&hMainKey);

rssPrintf("open hMainkey returned: 0x%x\n",dwRet);
LOG(log_file,"open hMainkey returned: 0x%x\n",dwRet);

// read some registry keys from the old rtvscan
			dwRet=CopyRemoteReg(reinterpret_cast<char*>(&MyAddr[CSprot]),SENDCOM_REMOTE_IS_SERVER | SENDCOM_REMOTE_USES_INSECURE_COMM,
                                "AddressCache","AddressCache");

			dwRet=CopyRemoteReg(reinterpret_cast<char*>(&MyAddr[CSprot]),SENDCOM_REMOTE_IS_SERVER | SENDCOM_REMOTE_USES_INSECURE_COMM,
                                "LoginOptions","LoginOptions");
rssPrintf("copy key LoginOptions returned: 0x%x\n",dwRet);
LOG(log_file,"copy key LoginOptions returned: 0x%x\n",dwRet);
//Breakpoint(1);///////////////////////////////////////////////////////////////////
			if( hMainKey )
			{
				HKEY hLKey;
				if( RegCreateKey( HKEY_LOCAL_MACHINE, REGHEADER"\\LoginOptions", &hLKey ) == ERROR_SUCCESS )
				{ // this should force the clients to update the next time they   in
					dwRet=IncrementRegValue( hLKey, "WinNTClientVersion");
					dwRet+=IncrementRegValue( hLKey, "Win95ClientVersion");
					dwRet+=IncrementRegValue( hLKey, "16BitClientVersion");
					if(dwRet)
					{
rssPrintf("Error incrementing client numbers\n");
LOG(log_file,"Error incrementing client numbers\n");
					}
				}
			}
		}
	}

	dwRet=SendCOM_REMOVE_PROGRAM( NULL, reinterpret_cast<char*>(&MyAddr[CSprot]),
                                  SENDCOM_REMOTE_IS_SERVER | SENDCOM_REMOTE_USES_INSECURE_COMM, ShutdownType );
	if( dwRet!=ERROR_SUCCESS )
	{
		dwRet=GetCBAAddr(sServer,SENDCOM_REMOTE_IS_SERVER | SENDCOM_REMOTE_USES_INSECURE_COMM, &addr);
		if(dwRet!=ERROR_SUCCESS) // we couldn't read the address of the server!!
		{
rssPrintf ("Couldn't read the CBA Address of: %s\n",sServer);
LOG(log_file,"Couldn't read the CBA Address of: %s\n",sServer);
		}

		UpdateUpgradeStatus(STS_STOPPING_SERVICE|(5<<PERCENT_SHIFT));

		dwRet=SendCOM_REMOVE_PROGRAM( NULL, reinterpret_cast<char*>(&addr),
                                      SENDCOM_REMOTE_IS_SERVER | SENDCOM_REMOTE_USES_INSECURE_COMM, ShutdownType );
		if( dwRet!=ERROR_SUCCESS )
		{
rssPrintf ("Couldn't send unload command to: %s\n",sServer);
LOG(log_file,"Couldn't send unload command to: %s\n",sServer);
			UpdateUpgradeError(STS_STOPPING_SERVICE,"Unable to unload RTVSCAN");
			return dwRet;
		}
	}
	DeletePurgeFile(NW_SYSTEM_DIR sSLASH "VPSTART.NCF"); // we don't want the old rtvscan to call this--we'll re-create it later

	pct+=10;
	UpdateUpgradeStatus(STS_STOPPING_SERVICE|(pct<<PERCENT_SHIFT));
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
	if(CheckCopyServerCommand()==EI_ABORT_INSTALL)
		return EI_ABORT_INSTALL;
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

rssPrintf ("Server unload returned %d 0x%x\n",dwRet,dwRet);
LOG(log_file,"Server unload returned %d 0x%x\n",dwRet,dwRet);

	for ( i = 0 ; i < 240 && FindNLMHandle("RTVSCAN") ; i++ ) {
		if (!i)
			iprintf("Waiting for RTVSCAN to unload");

		NTxSleep(500);
		pct = pct<85 ? ++pct : 85;
		UpdateUpgradeStatus(STS_STOPPING_SERVICE|(pct<<PERCENT_SHIFT));
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
	if(CheckCopyServerCommand()==EI_ABORT_INSTALL)
		return EI_ABORT_INSTALL;
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
	}
	if( FindNLMHandle("RTVSCAN") )
		UpdateUpgradeError(STS_STOPPING_SERVICE,"Unable to unload RTVSCAN");

	UpdateUpgradeStatus(STS_STOPPING_SERVICE|(85<<PERCENT_SHIFT));

	time (&t);
rssPrintf ("rtvscan%s unloaded at %s",(FindNLMHandle("RTVSCAN")?" NOT":""),ctime (&t));
LOG(log_file,"rtvscan%s unloaded at %s",(FindNLMHandle("RTVSCAN")?" NOT":""),ctime (&t));

	return dwRet;
}

/****************************************************************************************/


DWORD ConvertStrToBinData(BYTE *addr,char *line) {

	char 
		b[3],
		*s;
	DWORD 
		dwSize = strlen (line)/2,
		apos,
		lpos;

	for (lpos=apos=0; apos < dwSize; apos++) {
		b[0]=line[lpos++];
		b[1]=line[lpos++];
		b[2]=0;
		addr[apos] = (BYTE)strtol(b,&s,16);
	}

	for( apos=0 ; apos<dwSize ; apos++ )
		LOG(log_file,"%02x",addr[apos]);

	return ERROR_SUCCESS;
}

/****************************************************************************************/
static char dataline[1024];
#define CSADDR	"COPYSERVERADDRESS"
#define CSLEN	17
#define MYADDR	"SERVERADDRESS"
#define MYLEN	13
DWORD ReadCopyServerAddress(char* datafile) {

	FILE *fp;
	BYTE addr[sizeof(CBA_Addr)];
	char *q;

	memset( &addr, 0, sizeof(CBA_Addr) );

	fp=fopen(datafile,"rt");
	if(!fp) return -1;

	while ( fgets( dataline, sizeof(line), fp ) ) {
		if(!strnicmp( dataline, CSADDR, CSLEN )) {
			if(dataline[strlen(dataline)-1]=='\n') 
				dataline[strlen(dataline)-1]=0;
LOG(log_file,"found copy server address line (%s)\n",dataline);

			if(dataline[CSLEN]=='=') ///<< all these numbers
				q=&dataline[CSLEN+1];
			else
				q=&dataline[CSLEN+2];

			ConvertStrToBinData( (BYTE*)&addr, q );

			switch(dataline[CSLEN]) {
			case '=': 
			case '0':
				memcpy( &CSAddr[0], addr, sizeof(CBA_Addr) );
				break;
			case '1':
				memcpy( &CSAddr[1], addr, sizeof(CBA_Addr) );
				break;
			default:
				q=strchr(dataline,'=');
				if(!q) continue;
				q++;
				ConvertStrToBinData( (BYTE*)&addr, q );
				memcpy( &CSAddr[0], addr, sizeof(CBA_Addr) );
				break;
			}
		}
		if(!strnicmp( dataline, MYADDR, MYLEN )) {
			if(dataline[strlen(dataline)-1]=='\n') 
				dataline[strlen(dataline)-1]=0;
LOG(log_file,"found copy server address line (%s)\n",dataline);

			if(dataline[MYLEN]=='=') // if there is no 0 or 1 then look just past the =
				q=&dataline[MYLEN+1];
			else
				q=&dataline[MYLEN+2];

			ConvertStrToBinData( (BYTE*)&addr, q );

			switch(dataline[MYLEN]) { // is this SERVERADDRES0 or SERVERADDRES1 ?
			case '=':
			case '0':
				memcpy( &MyAddr[0], addr, sizeof(CBA_Addr) );
				break;
			case '1':
				memcpy( &MyAddr[1], addr, sizeof(CBA_Addr) );
				break;
			default:
				q=strchr(dataline,'=');
				if(!q) continue;
				q++;
				ConvertStrToBinData( (BYTE*)&addr, q );
				memcpy( &MyAddr[0], addr, sizeof(CBA_Addr) );
				memcpy( &MyAddr[1], addr, sizeof(CBA_Addr) );
				break;
			}
		}
	}

	return ERROR_SUCCESS;
}

/****************************************************************************************/

int UpdateRtvscan() {

	int i,j;
	time_t t;
	char cfile[255];
	DWORD dwRet=0;

///////////////////////////////////////////////////////////////////////////

	sssnprintf( cfile, sizeof(cfile), "%s" sSLASH "RSS.Log", ProgramDir );
	log_file=fopen( cfile, "wt" );

time (&t);
rssPrintf ("start at %s",ctime (&t));
LOG(log_file,"start at %s",ctime (&t));

PAUSE("1>");

///////////////////////////////////////////////////////////////////////////

	i=chdir(ProgramDir);
LOG(log_file,"chdir returned %d\n", i );

///////////////////////////////////////////////////////////////////////////

	ReadCopyServerAddress("vpdata.var");

//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
	if(CheckCopyServerCommand()==EI_ABORT_INSTALL)
		return EI_ABORT_INSTALL;
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

	LoadRegistryFunctions(TRUE);

	if(!RegFunctionsLoaded)
	{
		iprintf("Couldn't Load Registry Functions\n");
		return IDS_ERR_OPEN_REGDB;
	}

	dwRet=StartNTS(NULL);
///
rssPrintf ("StartNTS returned %d 0x%x\n",dwRet,dwRet);
LOG(log_file,"StartNTS returned %d 0x%x\n",dwRet,dwRet);
	
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
	if(CheckCopyServerCommand()==EI_ABORT_INSTALL)
		return EI_ABORT_INSTALL;
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

	SendUnloadToRtvscan();

//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
	if(CheckCopyServerCommand()==EI_ABORT_INSTALL)
		return EI_ABORT_INSTALL;
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//	Unload AMS NLMs

	iprintf("Unloading Supporting NLM's");

	for (j=0 ; modules[j] && modules[j][0] && modules[j][0]!=':' ; j++) {
		UnloadModule(modules[j]);
		NTxSleep(500);
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
		if(CheckCopyServerCommand()==EI_ABORT_INSTALL)
			return EI_ABORT_INSTALL;
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
	}

	UpdateUpgradeStatus(STS_STOPPING_SERVICE|(95<<PERCENT_SHIFT));

//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
	if(CheckCopyServerCommand()==EI_ABORT_INSTALL)
		return EI_ABORT_INSTALL;
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

	UnloadModule("ISCANNLM"); // Kludge to insure that IScanNLM is unloaded when updating to IBM's scan engine
	UnloadModule(REGEDIT_NAME);

time (&t);
rssPrintf ("Registry modules removed at %s",ctime (&t));
LOG(log_file,"Registry modules removed at %s",ctime (&t));

	UpdateUpgradeStatus(STS_STOPPING_SERVICE|(100<<PERCENT_SHIFT));

//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
	if(CheckCopyServerCommand()==EI_ABORT_INSTALL)
		return EI_ABORT_INSTALL;
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

	UpdateUpgradeStatus(STS_COPYING_FILES);

	NTxSleep(500);

dPrintf ("*****************************\n\n");
LOG(log_file,"\n*START***************************************************************************************\n");

iprintf("Checking file sizes in temporary directory.");
LOG(log_file,"Checking file sizes in temporary directory.");

	sssnprintf( cfile, sizeof(cfile), "%s" sSLASH TEMP_DIR, ProgramDir );
	if( CheckTempFileSizes( cfile ) ) {
		UpdateUpgradeStatus(STS_COPYING_FILES|(30<<PERCENT_SHIFT));
		CopyTempFiles( cfile );
	}
	UpdateUpgradeStatus(STS_COPYING_FILES|(80<<PERCENT_SHIFT));

//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
	if(CheckCopyServerCommand()==EI_ABORT_INSTALL)
		return EI_ABORT_INSTALL;
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

iprintf("Deleting temporary directory.");
rssPrintf( "Deleting temp directory (%s)\n", cfile );
LOG(log_file,"\n=START===============================================================\n");
LOG(log_file,"Deleting temp directory (%s)\n", cfile );

	CycleDirs( cfile, DELETE_DIRS );

LOG(log_file,"=END============================================================\n\n");

	UpdateUpgradeStatus(STS_COPYING_FILES|(100<<PERCENT_SHIFT));

//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
	if(CheckCopyServerCommand()==EI_ABORT_INSTALL)
		return EI_ABORT_INSTALL;
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

    StopNTS();
	UnloadRegistryFunctions();
	
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
	if(CheckCopyServerCommand()==EI_ABORT_INSTALL)
		return EI_ABORT_INSTALL;
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

	UnloadModule(REGISTRY_NAME);

///////////////////////////////////////////////////////////////////////////

dPrintf ("\n*****************************\n");
LOG(log_file,"*END**************************************************************************************\n\n");
time (&t);
rssPrintf ("done at %s",ctime (&t));
LOG(log_file,"done at %s",ctime (&t));

iprintf("Restarting RTVSCAN.");

	return 0;
}
/***********************************************************************/
void CopyTempFiles(char* path)
{
	iprintf("Copying file from temporary directory.");

rssPrintf( "Copying Files from temp directory (%s)\n", path );
LOG(log_file,"\n=START===============================================================\n");
LOG(log_file,"Copying Files from temp directory (%s)\n", path );
PAUSE("2>");

	CycleDirs( path, COPY_FILES );

LOG(log_file,"Done Copying files\n\n");
LOG(log_file,"=END================================================================\n\n");
}
/***********************************************************************/
FILE *g_fp = NULL;

static int CheckTempFileSizes(char *path)
{
	char cfile[256];
	int ret=0;
	
	memset( cfile,0,sizeof(cfile) );
	
	sssnprintf( cfile, sizeof(cfile), "%s" sSLASH "%s", path, KLUDGE_FILE );
dPrintf( "Opening file: (%s)\n", cfile );
LOG(log_file,"Opening file: (%s)\n", cfile );

	g_fp = fopen( cfile, "rt" );
	if(!g_fp)	
	{
LOG(log_file,"Unable to open (%s)\n",cfile);
		return 0;
	}
PAUSE("3>");

rssPrintf("Checking to make sure all files were copied\n");
LOG(log_file,"\n=START============================================================\n");
LOG(log_file,"Checking to make sure all files were copied\n");
//first make sure all the files in the list were copied over
	ret=CheckFileExists();
LOG(log_file,"=END====== File Copy Check ======================================================\n\n");

PAUSE("4>");
// now check the file sizes on all of them
	if(ret) 
	{
rssPrintf( "Checking the file sizes of files in the temp directory(%s)\n", path );
LOG(log_file,"\n=START============================================================\n");
LOG(log_file,"Checking the file sizes of files in the temp directory(%s)\n", path );
	ret=CycleDirs( path, VERIFY_SIZES );
	}
LOG(log_file,"=END====== File Size Check ======================================================\n\n");
	fclose( g_fp );
	g_fp=NULL;

rssPrintf( "The files are %sOK\n",ret?"":"NOT ");
LOG(log_file,"The files are %sOK\n",ret?"":"NOT ");
	return ret;
}
/***********************************************************************/
static int CheckFileExists()
{
	char *p=NULL;

	if(!g_fp) return 0;

	fseek( g_fp, 0, SEEK_SET );
	sssnprintf(line,sizeof(line),"%s" sSLASH,ProgramDir);

	while( fgets( (line+strlen(ProgramDir)+1) , sizeof(line), g_fp ) ) // read the file one line at a time
	{
		dPrintf("Line: %s",line);
LOG(log_file,"Line: %s",line);
PAUSE("5>");
		for(p=line+strlen(line) ; p>line && *p!=',' ; p-- );
		*p=0;
		if(access(line,0)) //if we can't access the file return false
		{
			rssPrintf("Couldn't find (%s)\n",line);
LOG(log_file,"Couldn't find (%s)\n",line);
			return 0;
		}
	}
LOG(log_file,"Found all files in listfile\n");
	return 1; // we were able to access all the files so return true
}
/***********************************************************************/
// if verify is 0 this copies all the files in the path 
// else it checks the size of the file against what is in the file list
static int CycleDirs(char *path,int verify)
{
	DIR *dirStruct=NULL;
	DIR *dirent=NULL;
	char TempPath[255];

dPrintf( "CycleDirs(%s,%d)\n", path, verify );
LOG(log_file,"_START______________________________________________________\n");
LOG(log_file,"CycleDirs(%s,%d)\n", path, verify );
PAUSE("6>");

	sssnprintf( TempPath, sizeof(TempPath), "%s" sSLASH "*.*", path );
	
	dirStruct = opendir( TempPath );
	if( !dirStruct ) 
	{
LOG(log_file,"Error opening dir (%s)\n",TempPath);
		return 0;
	}

	while( (dirent=readdir( dirStruct )) )
	{
		// ---------------------------------------
		// ksr - NetWare Certification, 10/31/2002
		// Time slice 
		ThreadSwitchWithDelay();

dPrintf("file(%s)\n",dirent->d_name);
LOG(log_file,"file(%s)\n",dirent->d_name);

		if( !IsDir(dirent->d_attr) ) // don't check the size of subdirectories
		{
			sssnprintf( TempPath, sizeof(TempPath), "%s" sSLASH "%s", path, dirent->d_name );
			switch( verify )
			{
			case VERIFY_SIZES:// check the file size
				if( !stricmp( dirent->d_name, KLUDGE_FILE) )
					continue;
				if( !CheckFileSize( TempPath, dirent->d_size) ) 
				{// either the file wasn't listed or the size was wrong
					rssPrintf("Either the file wasn't listed or the size was wrong\n");
LOG(log_file,"Either the file wasn't listed or the size was wrong\n");
					closedir( dirStruct );
					return 0; 
				}
				break;
			case COPY_FILES:
				if( !stricmp( dirent->d_name, KLUDGE_FILE) )
					continue;
				CopyOneFile( TempPath ); 
				break;
			case DELETE_DIRS:
				SetPurgeFlag( TempPath );
				DeletePurgeFile( TempPath );
				break;
			}
			
		}
		else // we have a subdirectory so recurse through it
		{
			dPrintf("SubDir(%s)\n",dirent->d_name);
LOG(log_file,"SubDir(%s)\n",dirent->d_name);
			sssnprintf( TempPath, sizeof(TempPath), "%s" sSLASH "%s", path, dirent->d_name );
			CycleDirs(TempPath,verify);
			if( verify==DELETE_DIRS )
			{
				SetPurgeFlag( TempPath );
				rmdir( TempPath );
			}
		}
	}
	closedir( dirStruct );

LOG(log_file,"Success cycling dir (%s)\n",path);
LOG(log_file,"^END^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");

	return 1;
}
/***********************************************************************/
static int CheckFileSize( char* file, long size )
{
	char *p;
	long len;

dPrintf( "CheckFileSize(%s,%d)\n", file, size );
LOG(log_file,"CheckFileSize(%s,%d)\n", file, size );
PAUSE("9>");
	if(!g_fp) return 0;

	fseek( g_fp, 0, SEEK_SET );

	sssnprintf(line,sizeof(line),"%s" sSLASH,ProgramDir);
	while( fgets( (line+strlen(ProgramDir)+1) , sizeof(line), g_fp ) ) // read the file one line at a time
	{
//dPrintf("line:%s",line);
//LOG(log_file,"line:%s",line);
PAUSE("7>");
		if( !strnicmp(line, file, strlen(file)) ) // if we find the filename
		{
			for(p=line+strlen(line) ; p>line && *p!=',' ; p-- ); // isolate the filesize
			if(p<=line) 
			{
LOG(log_file,"Error finding ',' in (%s)\n",line);
				return 0;
			}
			*p=0;
			p++;
			len = atol(p);
dPrintf("size:(%d)vs(%d)\n",size,len);
LOG(log_file,"size:(%d)vs(%d)\n",size,len);
			if(len==size) return 1; // if the size is the same return true
			else return 0;
		}
	}
dPrintf("Didn't find in " KLUDGE_FILE "\n");
LOG(log_file,"Didn't find in " KLUDGE_FILE "\n");
	return 0; // didn't find the file in the listfile so return false
}
/***********************************************************************/
void CopyOneFile(char *src)
{
	char dest[255];
	char *p=NULL,*q=NULL;
	DWORD len=0;
	int srcHan=-1, destHan=-1, ret=0;

dPrintf("CopyOneFile(%s)\n",src);
//LOG(log_file,"CopyOneFile(%s)\n",src);
PAUSE("8>");

	strcpy(dest, src);

	q=strstr( dest, TEMP_DIR );
	if(!q) 
	{
LOG(log_file,"Error finding '" sSLASH "' in (%s)\n",q+1);		
		return;
	}
//dPrintf("q(%s)\n", q);
//	q++;
	*q = 0;
	p=strchr( q+1, cSLASH );
	if(!p) 
	{
LOG(log_file,"Error finding '\\' in (%s)\n",q+1);
		return;
	}
//dPrintf("p(%s)\n", p);
	*p=0;
	p++;

//dPrintf("d(%s)\n",dest);
	while( *p )
		*q++ = *p++;

	*q=0;

	dPrintf("dest=(%s)\n",dest);
LOG(log_file,"dest=(%s)\n",dest);

	VerifyPath(dest); // create the path if it doesn't already exist

// open the source file 
	srcHan = open( src, O_RDONLY|O_BINARY ); 
	if( srcHan==-1)
	{
LOG(log_file,"couldn't open src(%s)\n",src);
		return;
	}
// open the destination file 
	destHan = open(dest, O_CREAT|O_TRUNC|O_WRONLY|O_BINARY, S_IWRITE|S_IREAD ); 
	if( destHan==-1 ) {
LOG(log_file,"couldn't open dest(%s)\n",dest);
		close(srcHan);
		return;
	}

	ret=FileServerFileCopy( srcHan, destHan, 0, 0, 0xffffffff, &len );

	close(srcHan);
	close(destHan);

dPrintf("FileServerFileCopy(%d,%d,%d)->%d\n",srcHan, destHan, len, ret );
//LOG(log_file,"FileServerFileCopy(%d,%d,%d)->%d\n",srcHan, destHan, len, ret );
}
/*******************************************************************************************/
/*******************************************************************************************/
/*******************************************************************************************/
