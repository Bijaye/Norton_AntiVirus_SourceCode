// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#include "pscan.h"
#include "nlmui.h"
#include "password.h"
#include "ScanStatus.h"
#include "SymSaferStrings.h"

void ToggleVerbose(void);
void TogglePassword(void);
extern "C" void RegDebug (void/*BOOL set*/);

void VBinStatusBar(void);   // gdf CW conversion
void WarnAboutAdminMenu(void);  // gdf CW conversion
void ExplainDBDumps(int FormatDone,char *File);  // gdf CW conversion
void DrawDebugOptions(void);   // gdf CW conversion
void ConstructCommandBar(void);  // gdf CW conversion


extern "C" int MRS=0;
extern "C" CScanStatus *UIScanList[NS]={0};
extern "C" char gszDebug[DEBUG_STR_LEN]="";
extern "C" BOOL doRegDebug = TRUE;

#ifdef DEBUG_MALLOC
 void *BLOCK=NULL;
#endif

extern "C" BOOL MenuOpen=FALSE;
extern "C" BOOL usePassword=TRUE;
extern BOOL gRTVdoRegDebug;

LONG headerPort=0;	// portal for displaying information at the top of the screen
PCB *headerPCB=NULL;
LONG histPort=0;	// portal for scan history list
extern "C" PCB *histPCB=NULL;
LONG helpPort=0;	// portal for displaying keyboard help
PCB *helpPCB=NULL;
LONG vbinPort=0;	// portal for displaying keyboard help
PCB *vbinPCB=NULL;
LONG funcPort=0;	// portal for displaying funtion key defs at the bottom of the screen
PCB *funcPCB=NULL;
LONG schPort=0;		// portal for scheduled scan list
PCB *schPCB=NULL;
LONG rtsPort=0;		// portal for realtime protection status
PCB *rtsPCB=NULL;
LONG fssPort=0;		// portal for server scan status
PCB *fssPCB=NULL;
LONG dbgPort=0;		// portal for displaying information with the debug menu
PCB *dbgPCB=NULL;

time_t CurrentTime=0;

MessageInfo menuMessages;
NUTInfo *nutH=NULL;
LONG UIallocTag=0;
LONG NLMHandle=0;
int ScreenHan=0;

int RTSStatColumn=0,RTSLastFileColumn=0,RTSLastVirusColumn=0;
int FSSStatColumn=0,FSSCurrentFileColumn=0,FSSLastVirusColumn=0,VBINColumn=0;

int SecondsTillScrSave=0;
time_t lastKeyTime=0;

BYTE *Msg[nMSG]; // the Message

#ifdef EnableScreenSaver
	HKEY hScrKey=0; 	// Key for the screen saver info
#endif
HKEY hManScanKey=0;	// Key for the manual scan info
int StatusDisp=dRTSCAN,prevDisp=0; // Flags for whether the real time scan status or file server scan status is displayed

BOOL ScanIsRunningNow=FALSE; // flag is true when a scan is currently running

#ifdef EnableScreenSaver
	BOOL RunScreenSave=TRUE;
#endif
int CurrentScan=-1;
DWORD NumberOfScans=0; // The number of scans that have run

char CommandBar[128];
char UserName[MAX_PASSWORD],ServerName[NAME_SIZE],DomainName[NAME_SIZE];
FILE_SERV_INFO ServerInfo;

int MainThreadGroup=-1;
BOOL MessagesInitialized=FALSE;

ScheduledScan *SchScanList = NULL;//[20];

char Spinner[]="-\\|/\0";
int SpinLoc=0;

int Highlight=0;
int lastNScans=-1;

INIT_LOCK();

typedef struct vbentry {
	struct vbentry *next;
	char LongName[IMAX_PATH];
	char LogLine[MAX_LOG_LINE_SIZE];
	DWORD Flags;
	DWORD RecordID;               // assigned
	BYTE InternalStateData[512];  // assigned
	char StorageName[NAME_SIZE];
	DWORD StorageInstanceID;
	char StorageKey[IMAX_PATH];
} VBEntry;
VBEntry* VBinList=NULL;

int TopVBLine=0;

int NumVBin=0;

#define VBSize 17

int DebugMenu(void *handle);
static BOOL WarnAboutFileDump();
//static int DisplayRunningScan(PSCAN_STATUS s,int l);
//static int DisplayLastInfection(PSCAN_STATUS s,int l);

static int DisplayRunningScan(CScanStatus *s,int l);
static int DisplayLastInfection(CScanStatus *s,int l);

   
/************************************************************************************/
void ClearVBinList(void) {

	VBEntry *temp,*ptr=VBinList;
	VBinList=NULL;

	while(ptr) {
		temp=ptr;
		ptr=ptr->next;
		free(temp);
		temp=NULL;
	}
}
/************************************************************************************/
DWORD GatherVBinInfo(void) {
	VBEntry *NewEntry=NULL, *curr=VBinList;
	VBININFO Info;
	char path[IMAX_PATH];
	WIN32_FIND_DATA fd;
	HANDLE han;
	char *q;
	DWORD id;
	DWORD cc = ERROR_NO_MORE;

	TopVBLine=0;
	NumVBin=0;

	memset(&Info,0,sizeof(VBININFO));
	Info.Size = sizeof(VBININFO);

//	sssnprintf(path,sizeof(path),"%s\\VBin\\*.VBN",HomeDir);
	sssnprintf(path,sizeof(path),"%s\\%s\\*.VBN",HomeDir, QUARANTINE_DIR); //EA - 08/05 for Netware NLM product the Quarantined files will be in dir "QUAR"
	han = FindFirstFile(path,&fd);

	if (han != INVALID_HANDLE_VALUE) {
		do  {

			ThreadSwitchWithDelay();

			q = StrRChar(fd.cFileName,'.');
			if (q) {
				*q = 0;

				sscanf(fd.cFileName,"%08X",&id);
				cc = VBin_GetInfo(id,&Info);
				if(cc==ERROR_SUCCESS) {
					NewEntry=(VBEntry*)malloc(sizeof(VBEntry));
					if(!NewEntry) continue;

					memset(NewEntry,0,sizeof(VBEntry));

					memcpy(NewEntry->LongName,Info.Description,IMAX_PATH);
					memcpy(NewEntry->LogLine,Info.LogLine,MAX_LOG_LINE_SIZE);
					NewEntry->Flags=Info.Flags;
					NewEntry->RecordID=Info.RecordID;
					memcpy(NewEntry->InternalStateData,Info.InternalStateData,512);
					memcpy(NewEntry->StorageName,Info.StorageName,NAME_SIZE);
					NewEntry->StorageInstanceID=Info.StorageInstanceID;
					memcpy(NewEntry->StorageKey,Info.StorageKey,IMAX_PATH);

					if(!VBinList) {
						VBinList=NewEntry;
						curr=NewEntry;
					}
					else {
						while(curr->next)
							curr=curr->next;

						curr->next=NewEntry;
						curr=NewEntry;
					}
					NumVBin++;
				}
			}
		} while (FindNextFile(han,&fd));
		FindClose(han);
		return ERROR_SUCCESS;
	}

	return cc;
}
/************************************************************************************/
int VBListAction( LONG keyPressed, LIST **element, LONG *itenNumber, void *listParm ) {

	VBEntry *entry=NULL;
	int l=0;//,len;
	char *q;//buf[16];
	PEVENTBLOCK pEvent=NULL;
	LONG entPort=0;
	PCB *entPCB=NULL;
	char path[IMAX_PATH];

	if(keyPressed==M_ESCAPE)
		return 1;

	entry=(VBEntry*)((*element)->otherInfo);

	memcpy(path,entry->LongName,IMAX_PATH);

	q=NWLstrrchr(path,'\\');
	if(!q) { // no slashes
		q=NWLstrchr(path,':'); // look for the volume name
		if(q) {
			q++;
			*q=0; // found a colon, chop off rest of string
		}
		q=NWLstrchr(entry->LongName,':'); // since we chopped the rest of the string, use the original to find the filename
		if(q)
			q++;
		else
			q=path;// there aren't any colons either
	}
	else {
		*q=0;
		q++; // move the pointer past the slash
	}

	memset (CommandBar,0,sizeof(CommandBar));

	sssnprintf (CommandBar,sizeof(CommandBar)," %s%s",LS (IDS_CMD_RETURN_TO_LIST),LS(IDS_BLANK_LINE));

	STATUS_BAR_S(CommandBar);


	MyCreatePortal (&entPort,&entPCB,3,2,10,76,40,80,SAVE,
                    reinterpret_cast<unsigned char*>(q),VINTENSE,SINGLE,VINTENSE,nutH);

	if(CreateEventFromLogLine( &pEvent,entry->LogLine)==ERROR_SUCCESS) {

		NWSDisplayTextInPortal (l,1,reinterpret_cast<unsigned char*>(LS(IDS_VIRUS_FOUND)),VNORMAL,entPCB);
		l=NWSDisplayTextInPortal (l,VBINColumn,reinterpret_cast<unsigned char*>(ctime((time_t*)&pEvent->Time)),VINTENSE,entPCB);

		NWSDisplayTextInPortal (l,1,reinterpret_cast<unsigned char*>(LS(IDS_VIRUS_NAME)),VNORMAL,entPCB);
		l=NWSDisplayTextInPortal (l,VBINColumn,reinterpret_cast<unsigned char*>(pEvent->VirusName),VINTENSE,entPCB);

		DestroyCopyOfEvent(pEvent);
	}
	l=NWSDisplayTextInPortal (l,1,reinterpret_cast<unsigned char*>(LS(IDS_ORIGINAL_PATH)),VNORMAL,entPCB);
	l=NWSDisplayTextInPortal (l,5,reinterpret_cast<unsigned char*>(path),VINTENSE,entPCB);

/*	l=NWSDisplayTextInPortal (l,1,,VNORMAL,entPCB);
	l=NWSDisplayTextInPortal (l,1,itoa(entry->RecordID,buf,16),VNORMAL,entPCB);
	l=NWSDisplayTextInPortal (l,1,entry->InternalStateData,VNORMAL,entPCB);
//	l=NWSDisplayTextInPortal (l,1,entry->StorageName,VNORMAL,entPCB);
//	l=NWSDisplayTextInPortal (l,1,itoa(entry->StorageInstanceID,buf,16),VNORMAL,entPCB);
	l=NWSDisplayTextInPortal (l,1,entry->StorageKey,VNORMAL,entPCB);
*/
	while(!NWSKeyStatus(nutH) && SystemRunning)
		NTxSleep(100);

	NWSDestroyPortal(entPort,nutH);

	while(NWSKeyStatus(nutH))
		getch();

	VBinStatusBar();

	return -1;
}
/************************************************************************************/
void VBinStatusBar(void) {

	memset (CommandBar,0,sizeof(CommandBar));

	sssnprintf (CommandBar,sizeof(CommandBar)," %s%s",LS (IDS_CMD_VIEW_DETAILS),"   "); // "Alt+f10=Quit"

	while (NumBytes (CommandBar) < (79-NumBytes (LS(IDS_CMD_RETURN))))
		StrCat (CommandBar," ");
	StrCat (CommandBar, LS(IDS_CMD_RETURN)); // add "esc=return"
	StrCat (CommandBar,"     ");
	STATUS_BAR_S(CommandBar);
}
/************************************************************************************/
void DisplayVirusBin(BOOL clear) {

	int l=0;//s=0,
	VBEntry *curr=VBinList;
	int width=strlen(LS(IDS_FILES_IN_VBIN))+2;
	int len;
	DWORD cc=ERROR_SUCCESS;

	if ( !nutH ) return;

	NWSStartWait(10,40,nutH);
	{
		cc=GatherVBinInfo();
		if(cc==ERROR_SUCCESS)
		{

			NWSDestroyList(nutH);

			NWSInitList( nutH, NULL );
			for( l=0,curr=VBinList ; curr && l!=-1; curr=curr->next )
			{
				len=strlen(curr->LongName)+6;
				if(len>78) {
					char NewName[IMAX_PATH];
					char *p;
					width=78;
					memcpy(NewName,curr->LongName,IMAX_PATH);
					p=NWLstrchr(NewName,'\\');

					while( p && (len=strlen(p)+9)>78)
						p=NWLstrchr(p+1,'\\');

					if(p) { // if we found sufficient backslashes to shorten the name
						*(p-1)='.';
						*(p-2)='.';
						*(p-3)='.';
						p-=3;
					}
					else { // else if we couldn't shorten the string for some reason
						p=NWLstrrchr(NewName,'\\'); // just strip the path
						if(p)
							p++;
						else
							p=NewName; // there must not be any slashes
					}

					NWSAppendToList( reinterpret_cast<unsigned char*>(p), curr, nutH );
				}
				else {
					width = width>len ? width : len;

					NWSAppendToList( reinterpret_cast<unsigned char*>(curr->LongName), curr, nutH );
				}
			}

			VBinStatusBar();
		}
	}
	NWSEndWait(nutH);

	if(cc==ERROR_SUCCESS)
	{
		LIST *pSelected=nutH->head; // we'll highlight the first item in the list

		NWSSetDynamicMessage(DYNAMIC_MESSAGE_ONE,reinterpret_cast<unsigned char*>(LS(IDS_FILES_IN_VBIN)),&nutH->messages);
		NWSList( DYNAMIC_MESSAGE_ONE, 13, 40, (NumVBin<17 ? NumVBin : 17), width, M_ESCAPE|M_SELECT, &pSelected, nutH, NULL, VBListAction, NULL );
	}
	else
	{
		NWSSetDynamicMessage(DYNAMIC_MESSAGE_ONE,reinterpret_cast<unsigned char*>(LS(IDS_VBIN_EMPTY)),&nutH->messages);
		NWSAlert(10,40,nutH,DYNAMIC_MESSAGE_ONE);
	}

	NWSStartWait(10,40,nutH);
	{
		NWSDestroyList(nutH);

		ClearVBinList();
	}
	NWSEndWait(nutH);
}
/************************************************************************************/
int AdminAction (int option, void* stuff);

int AdminMenu(void *handle) {

	int retval=0;
	BOOL ret=FALSE;
	LONG palette;

	REF(handle);

	if (!nutH) return 0;

	dprintf ("Admin Menu\n");

	WarnAboutAdminMenu();

	palette=NWSGetScreenPalette(nutH);
	NWSSetScreenPalette (WARNING_PALETTE,nutH);
	while (nutH && SystemRunning) {

		ThreadSwitchWithDelay();
		STATUS_BAR (IDS_CMD_ADMIN);
		MenuOpen=TRUE;
		NWSInitMenu (nutH);

		NWSAppendToMenu (MSGNUM(IDS_DEBUG_MENU),1,nutH);
		NWSAppendToMenu (MSGNUM(IDS_PASSWORDS),2,nutH);
		NWSAppendToMenu (MSGNUM(IDS_FORM_DB_DUMP),4,nutH);
		NWSAppendToMenu (MSGNUM(IDS_RAW_DB_DUMP),5,nutH);
#ifdef EnableScreenSaver
		if (NWversion >= 4 && languageID == 4) {
			if (RunScreenSave)
				NWSAppendToMenu (MSGNUM(IDS_SCR_SAVE_OFF),6,nutH);
			else
				NWSAppendToMenu (MSGNUM(IDS_SCR_SAVE_ON),6,nutH);
		}
#endif
		if (SystemRunning)
			retval=NWSMenu (MSGNUM(IDS_ADMIN_TITLE),9,40,NULL,AdminAction,nutH,(void*)nutH);

		if (!SystemRunning) retval=KEY_ESCAPE;

		NWSDestroyMenu(nutH);
		MenuOpen=FALSE;

		switch (retval) {
			case KEY_ESCAPE:
				NWSSetScreenPalette (palette,nutH);
				goto Return;
			case 1:
				DebugMenu(NULL);
				break;
			case 2:
				SetPasswords();
				break;
			case 4:
				if (WarnAboutFileDump()) {
					char dumpDir[255];
					strcpy(dumpDir,ProgramDir);
					FileDumpDatabase(HKEY_LOCAL_MACHINE,dumpDir,sizeof(dumpDir));
					ExplainDBDumps(2,dumpDir);
				}
				else
					ExplainDBDumps(0,NULL);
				break;
			case 5:
				{
					char dumpDir[255];
					strcpy(dumpDir,ProgramDir);
					ret=RawDumpDatabase(dumpDir,sizeof(dumpDir));
					if (ret)
						ExplainDBDumps(1,dumpDir);
					else
						ExplainDBDumps(0,NULL);
				}
				break;
#ifdef EnableScreenSaver
			case 6:
				ToggleScreenSaver();
				NWSSetScreenPalette (palette,nutH);
				goto Return;
#endif
		}
	}
Return:
	dprintf ("Exiting Admin Menu:%d\n",retval);
	return retval;
}
/************************************************************************************/
int AdminAction (int option, void* stuff) {

	REF(stuff);

	if (option == -1 || !SystemRunning) return KEY_ESCAPE;
	else return option;
}
/************************************************************************************/
void WarnAboutAdminMenu() {

	LONG warnPort=0,palette;
	PCB *warnPCB=NULL;
	char AdminWarn[1024];
	int R,height;
	LONG type;
	BYTE key;

	dprintf ("Warn About Admin Menu\n");

	sssnprintf (AdminWarn,sizeof(AdminWarn),"%s  %s  %s %s",LS(IDS_ADMIN_WARN1),LS(IDS_ADMIN_WARN2),LS(IDS_ADMIN_WARN3),LS(IDS_ADMIN_WARN4));
	height=(NumBytes(AdminWarn)/48) + 6;
	if (!nutH) return;
	palette=NWSGetScreenPalette(nutH);
	NWSSetScreenPalette (WARNING_PALETTE,nutH);
	if (MyCreatePortal (&warnPort,&warnPCB,4,15,height,50,20,80,SAVE,NULL,0,SINGLE,VRBLINK,nutH)) {
		R=NWSDisplayTextJustifiedInPortal (JCENTER,1,24,46,reinterpret_cast<unsigned char *>(AdminWarn),VREVERSE,warnPCB);

		NWSDisplayTextJustifiedInPortal (JCENTER,height-3,24,strlen(LS(IDS_HIT_KEY)),reinterpret_cast<unsigned char *>(LS(IDS_HIT_KEY)),VREVERSE,warnPCB);

		NWSFillPortalZoneAttribute (0,0,height,50,VREVERSE,warnPCB);
		while (!NWSKeyStatus(nutH) && SystemRunning && CheckIfScreenDisplayed(ScreenHan,FALSE))
			NTxSleep (200);

		if (NWSKeyStatus(nutH)) NWSGetKey(&type,&key,nutH);

		NWSDestroyPortal (warnPort,nutH);
	}
	NWSSetScreenPalette (palette,nutH);
}
/************************************************************************************/
static BOOL WarnAboutFileDump() {

	LONG warnPort=0,palette;
	PCB *warnPCB=NULL;
	char message[1024];
	int R=0,height;

	dprintf ("Warn About File Dump\n");

	sssnprintf (message,sizeof(message),"%s %s",LS(IDS_DUMP_WARN1),LS(IDS_DUMP_WARN2));

	height=(NumBytes(message)/48) + 4;
	if (!nutH) return 0;
	palette=NWSGetScreenPalette(nutH);
	NWSSetScreenPalette (WARNING_PALETTE,nutH);
	if (MyCreatePortal (&warnPort,&warnPCB,4,15,height,50,20,80,SAVE,NULL,0,SINGLE,VNORMAL,nutH)) {

		NWSDisplayTextInPortal (0,1,reinterpret_cast<unsigned char*>(message),VNORMAL,warnPCB);

		R=NWSConfirm (MSGNUM(IDS_CONTINUE_YN),15,40,1,NULL,nutH,NULL);

		NWSDestroyPortal (warnPort,nutH);
	}
	NWSSetScreenPalette (palette,nutH);
	return R;
}
/************************************************************************************/
void ExplainDBDumps(int FormatDone,char *File) {

//	char RawFileName[128],DumpFileName[128];
	LONG dumpPort=0;
	PCB *dumpPCB=NULL;
	int R=0,height;
	LONG type;
	BYTE key;

	dprintf ("Explain DB Dumps\n");

	if (FormatDone==0) height=7;
	else height=8;

	if (!nutH) return;

	if (MyCreatePortal (&dumpPort,&dumpPCB,4,15,height,50,20,80,SAVE,
                        reinterpret_cast<unsigned char*>(LS(IDS_DB_DUMP_DONE)),VNORMAL,SINGLE,VNORMAL,nutH)) {

		switch (FormatDone) {
			case 0:
				R=NWSDisplayTextInPortal (R,1,reinterpret_cast<unsigned char*>(LS(IDS_NO_DUMP)),VNORMAL,dumpPCB);
				break;
			case 1:
//				sssnprintf (RawFileName,sizeof(RawFileName),RAW_REG_DUMP,ProgramDir,0);
				R=NWSDisplayTextInPortal (R,1,reinterpret_cast<unsigned char*>(LS(IDS_RAW_DUMP_FILE)),VNORMAL,dumpPCB);
				R=NWSDisplayTextInPortal (R,5,reinterpret_cast<unsigned char*>(File),VNORMAL,dumpPCB);
				break;
			case 2:
//				sssnprintf (DumpFileName,sizeof(DumpFileName),REG_DUMP,ProgramDir,0);
				R=NWSDisplayTextInPortal (R,1,reinterpret_cast<unsigned char*>(LS(IDS_FORM_DUMP_FILE)),VNORMAL,dumpPCB);
				R=NWSDisplayTextInPortal (R,5,reinterpret_cast<unsigned char*>(File),VNORMAL,dumpPCB);
				break;
		}

		R++;
		R=NWSDisplayTextJustifiedInPortal (JCENTER,R,24,strlen(LS(IDS_HIT_KEY)),
                                           reinterpret_cast<unsigned char *>(LS(IDS_HIT_KEY)),VINTENSE,dumpPCB);

		while (!NWSKeyStatus(nutH) && SystemRunning && CheckIfScreenDisplayed(ScreenHan,FALSE))
			NTxSleep (200);

		if (NWSKeyStatus(nutH)) NWSGetKey(&type,&key,nutH);

		NWSDestroyPortal (dumpPort,nutH);
	}
}
/************************************************************************************/
int DebugMenu(void *handle) {

	int retval;
	LONG palette;

	REF(handle);

	if (!nutH) return 0;

	dprintf ("Debug Menu\n");

	palette=NWSGetScreenPalette(nutH);
	NWSSetScreenPalette (WARNING_PALETTE,nutH);
	if (!MyCreatePortal (&dbgPort,&dbgPCB,6,30,8,25,10,25,NO_SAVE,
                         reinterpret_cast<unsigned char*>(LS(IDS_CURR_CFG)),VNORMAL,SINGLE,VNORMAL,nutH))
		return 0;

	DrawDebugOptions();

	MenuOpen=TRUE;
	NWSInitMenu (nutH);

	NWSAppendToMenu (MSGNUM(IDS_TOG_DEB),1,nutH);
	NWSAppendToMenu (MSGNUM(IDS_TOG_VER),2,nutH);
	NWSAppendToMenu (MSGNUM(IDS_TOG_LOG),3,nutH);

#ifdef DEBUG
	NWSAppendToMenu (MSGNUM(IDS_TOG_PASS),4,nutH);
#endif
#ifdef MemCheck
	if (TrackMallocs)
		NWSAppendToMenu (MSGNUM(IDS_REP_MEM),5,nutH);
#endif // MemCheck

	retval=NWSMenu (MSGNUM(IDS_DEBUG_CMD),10,15,0,DebugAction,nutH,(void*)nutH);
	NWSDestroyMenu(nutH);
	MenuOpen=FALSE;
	NWSDestroyPortal(dbgPort,nutH);

	NWSSetScreenPalette (palette,nutH);

	DisplayScanHistory (TRUE);

	return retval;
}
/************************************************************************************/
int DebugAction (int option, void* stuff) { // Action function for the Debug Menu

	REF(stuff);

	if (!SystemRunning) return KEY_ESCAPE;
	switch (option) {
		case -1:
			return KEY_ESCAPE;
			break;
		case 1:
			gRTVdoRegDebug = FALSE;
			ToggleDebug();
			break;
		case 2:
			gRTVdoRegDebug = FALSE;
			ToggleVerbose();
			break;
		case 3:
			gRTVdoRegDebug = FALSE;
			ToggleLogging();
			break;
#ifdef DEBUG
		case 4:
			TogglePassword();
			break;
#endif
#if defined MemCheck || defined SemaCheck || defined Thread
		case 5:
#ifdef MemCheck
			if (TrackMallocs)
				ReportMemoryUsage(FALSE);
#endif // MemCheck
#ifdef SemaCheck
			if (TrackSemaphores)
				ReportSemaUsage(FALSE);
#endif // SemaCheck
#ifdef ThreadCheck
			if (TrackThreads)
				ThreadReport(FALSE);
#endif // SemaCheck

			break;
#endif // MemCheck || SemaCheck
	}
	DrawDebugOptions();

	return -1;
}
/************************************************************************************/
void DrawDebugOptions() {

	int R=0;
	char *verboseMsg,*debugMsg,*logMsg;
#ifdef DEBUG
	char *passMsg;
#endif

	if (!nutH || !dbgPCB) return;

	if (debug&DEBUGPRINT)
		debugMsg=LS(IDS_DEBUG_ON);
	else
		debugMsg=LS(IDS_DEBUG_OFF);

	if (debug&DEBUGVERBOSE)
		verboseMsg=LS(IDS_VERBOSE_ON);
	else
		verboseMsg=LS(IDS_VERBOSE_OFF);

	if (debug&DEBUGLOG)
		logMsg=LS(IDS_LOGGING_ON);
	else
		logMsg=LS(IDS_LOGGING_OFF);

#ifdef DEBUG
	if (usePassword)
		passMsg=LS(IDS_PASSWORD_ON);
	else
		passMsg=LS(IDS_PASSWORD_OFF);
#endif

	NWSClearPortal (dbgPCB);
	NWSDrawPortalBorder (dbgPCB);

	NWSDisplayTextInPortal (R++,1,reinterpret_cast<unsigned char*>(debugMsg),VNORMAL,dbgPCB);
	NWSDisplayTextInPortal (R++,1,reinterpret_cast<unsigned char*>(logMsg),VNORMAL,dbgPCB);
#ifdef DEBUG
	NWSDisplayTextInPortal (R++,1,reinterpret_cast<unsigned char*>(passMsg),VNORMAL,dbgPCB);
#endif
	NWSDisplayTextInPortal (R++,1,reinterpret_cast<unsigned char*>(verboseMsg),VNORMAL,dbgPCB);
	NWSDisplayTextInPortal (R++, 1,reinterpret_cast<unsigned char*>(gszDebug), VNORMAL,dbgPCB);
}
/************************************************************************************/
void KillAlternatePortals() {

	if (!nutH) return;

	StatusDisp=0;
	if (schPCB) {
		dprintf ("Kill Schedule Portal...");
		NWSDestroyPortal (schPort,nutH);
		schPCB=NULL;
		schPort=0;
//		NWSDestroyPortal (nscPort,nutH);
//		nscPCB=NULL;
//		nscPort=0;
		dprintf ("done\n");
	}
	if (helpPCB) {
		dprintf ("Kill Help Portal...");
		NWSDestroyPortal (helpPort,nutH);
		helpPCB=NULL;
		helpPort=0;
		dprintf ("done\n");
	}
	if (vbinPCB) {
		dprintf ("Kill Virus Bin Portal...");
		NWSDestroyPortal (vbinPort,nutH);
		vbinPCB=NULL;
		vbinPort=0;
		dprintf ("done\n");
		ClearVBinList();
	}
}
/************************************************************************************/
void DisplayHelp (BOOL clear) {

	int r=0,R,H;
	LONG palette;

	if (!nutH) return;

	dprintf ("Display Help\n");

	palette=NWSGetScreenPalette(nutH);
	NWSSetScreenPalette (HELP_PALETTE,nutH);

	if (debug&DEBUGPRINT) {
		H=21;		// use the whole screen to display the help
		R=3;
	}
	else {
		H=12;		// only use the screen that we need to display the help
		R=7;
	}
	if (helpPCB==NULL) {
		dprintf ("Display Help Screen\n");
		MyCreatePortal (&helpPort,&helpPCB,R,5,H,70,40,80,SAVE,reinterpret_cast<unsigned char*>(LS(IDS_HELP_TITLE)),VREVERSE,SINGLE,VREVERSE,nutH);
	}
	if (!helpPCB) {
		NWSSetScreenPalette (palette,nutH);
		dprintf ("couldn't create help portal\n");
		DisplayScanHistory(TRUE);
		return;
	}
	if (prevDisp!=StatusDisp || clear) {
		NWSClearPortal (helpPCB);
		NWSDrawPortalBorder (helpPCB);
		NWSFillPortalZoneAttribute (0,0,25,70,VREVERSE,helpPCB);

		prevDisp = StatusDisp;
	}

	DISP (r++,1,	reinterpret_cast<unsigned char*>(LS(IDS_HELP_F1)),	VREVERSE,helpPCB);// help
	DISP (r++,1,	reinterpret_cast<unsigned char*>(LS(IDS_HELP_F2)),	VREVERSE,helpPCB);// scheduled scans
	DISP (r++,1,	reinterpret_cast<unsigned char*>(LS(IDS_HELP_F3)),	VREVERSE,helpPCB);// virus bin
	DISP (r++,1,	reinterpret_cast<unsigned char*>(LS(IDS_HELP_F5)),	VREVERSE,helpPCB);// manual scan
	if (debug&DEBUGPRINT) {
		DISP (r++,1,reinterpret_cast<unsigned char*>(LS(IDS_HELP_F6)),	VREVERSE,helpPCB);// Admin menu
#ifdef EnableScreenSaver
		DISP (r++,1,reinterpret_cast<unsigned char*>(LS(IDS_HELP_F9)),	VREVERSE,helpPCB);// screen saver
#endif
#ifdef DEBUG
		DISP (r++,1,reinterpret_cast<unsigned char*>(LS(IDS_HELP_F8)),	VREVERSE,helpPCB);// Admin menu (no password)
		DISP (r++,1,reinterpret_cast<unsigned char*>(LS(IDS_HELP_F10)),	VREVERSE,helpPCB);// Exit RTVSCAN (no password)
		DISP (r++,1,reinterpret_cast<unsigned char*>(LS(IDS_HELP_AF3)),	VREVERSE,helpPCB);// Debug menu (no password)
		DISP (r++,1,reinterpret_cast<unsigned char*>(LS(IDS_HELP_AF4)),	VREVERSE,helpPCB);// Dump Registry database
#endif
	}
	DISP (r++,1,	reinterpret_cast<unsigned char*>(LS(IDS_HELP_AF10)),	VREVERSE,helpPCB);// Exit RTVSCAN
	DISP (r++,1,	reinterpret_cast<unsigned char*>(LS(IDS_HELP_ARROW)),	VREVERSE,helpPCB);// scroll scan history
	DISP (r++,1,	reinterpret_cast<unsigned char*>(LS(IDS_HELP_TAB)),	VREVERSE,helpPCB);// Cycle through scan history
	DISP (r++,1,	reinterpret_cast<unsigned char*>(LS(IDS_HELP_HOME)),	VREVERSE,helpPCB);// move to top of scan history
	DISP (r++,1,	reinterpret_cast<unsigned char*>(LS(IDS_HELP_END)),	VREVERSE,helpPCB);// move to bottom of scan history
	NWSFillPortalZoneAttribute (0,0,H,70,VREVERSE,helpPCB);
	NWSSetScreenPalette (palette,nutH);
}
extern "C" CONFIG_OPTIONS gtConfigOptions;
/************************************************************************************/
void DisplayRTSStatus (BOOL clear) { // Display Real Time Scan Status

	char buf[50],Direction[50];
	char uName[128],cName[128];
	int r=0;
	static int nScanned;
	static int vFound;
	static int OnOff;
	time_t t;

	if (!nutH || !rtsPCB) return;

	if (vFound!=pStatBlock->VirusesFound || nScanned!=pStatBlock->TotalScaned || prevDisp!=StatusDisp || clear) {
		NWSClearPortal (rtsPCB);
		NWSDrawPortalBorder (rtsPCB);

		nScanned = pStatBlock->TotalScaned;
		vFound = pStatBlock->VirusesFound;
		prevDisp = StatusDisp;
		t=time(NULL);
	}

	// figure out the direction of the RT scan
	if (gtConfigOptions.dwWrites && gtConfigOptions.dwReads)
		StrCopy (Direction,LS(IDS_DIR_IN_OUT));	// in and out
	else if (gtConfigOptions.dwReads && !gtConfigOptions.dwWrites)
		StrCopy (Direction,LS(IDS_DIR_OUT));		// only in
	else if (gtConfigOptions.dwWrites && !gtConfigOptions.dwReads)
		StrCopy (Direction,LS(IDS_DIR_IN));		// only out
	else if (!gtConfigOptions.dwReads && !gtConfigOptions.dwWrites)
		StrCopy (Direction,LS(IDS_DIR_NONE));		// neither

	if( OnOff != gtConfigOptions.dwOnOff )
	{
		RtsClearLine(r);
		OnOff = gtConfigOptions.dwOnOff;
	}

	if(!gtConfigOptions.dwOnOff) {
		RTS_DISPLAY (r++, 	1,				reinterpret_cast<unsigned char*>(LS(IDS_REALTIME_DISABLED)));
	}
	else {
		RTS_DISPLAY (r, 	1,				reinterpret_cast<unsigned char*>(LS(IDS_SCAN_DIRECTION)));
		RTS_DISPLAYI(r++,	RTSStatColumn,	reinterpret_cast<unsigned char*>(Direction));
	}
	RTS_DISPLAY (r, 	1,				reinterpret_cast<unsigned char*>(LS(IDS_TOTAL_SCANNED)));
	RTS_DISPLAYI(r++,	RTSStatColumn,	reinterpret_cast<unsigned char*>(itoa(pStatBlock->TotalScaned,buf,10)));

	RTS_DISPLAY (r, 	1,				reinterpret_cast<unsigned char*>(LS(IDS_INFECTED_FOUND)));
	RTS_DISPLAYI(r++,	RTSStatColumn,	reinterpret_cast<unsigned char*>(itoa(pStatBlock->VirusesFound,buf,10)));

	r++;

	RTS_DISPLAYR (r++,1,reinterpret_cast<unsigned char*>(LS(IDS_LAST_SCANNED)));

// Last File Scanned
	if (pStatBlock->LastScaned[0]) { // only display if a file has been scanned

		GetNames ((PSID)(&pStatBlock->LastUser),uName,cName,NULL); // get the username
		RTS_DISPLAY (r,		2,					reinterpret_cast<unsigned char*>(LS(IDS_USER_NAME)));
		RTS_DISPLAYI(r++,	RTSLastFileColumn,	reinterpret_cast<unsigned char*>(uName));

		RTS_DISPLAY (r, 	2,					reinterpret_cast<unsigned char*>(LS(IDS_FILE)));

		if(strlen(pStatBlock->LastScaned) >= 150)  // gdf 05/05/00
		{   // gdf 05/05/00
			char longtemp[150], longtemp1[150];   // gdf 05/05/00
			memset(longtemp, 0, sizeof(longtemp));  // gdf 05/05/00
			memset(longtemp1, 0, sizeof(longtemp));  // gdf 05/05/00
			strncpy(longtemp, pStatBlock->LastScaned, 70);  // gdf 05/05/00
			strcat(longtemp, ".....");  // gdf 05/05/00
			strcat(longtemp1, pStatBlock->LastScaned + strlen(pStatBlock->LastScaned) - 70);  // gdf 05/05/00
			RTS_DISPLAYI(r++,	RTSLastFileColumn,	reinterpret_cast<unsigned char*>(longtemp));   // gdf 05/05/00
			RTS_DISPLAYI(r++,	RTSLastFileColumn,	reinterpret_cast<unsigned char*>(longtemp1));  // gdf 05/24/00
		}   // gdf 05/05/00
		else   // gdf 05/05/00
		{  // gdf 05/24/00
			RTS_DISPLAYI(r++,	RTSLastFileColumn,	reinterpret_cast<unsigned char*>(pStatBlock->LastScaned));
			r++;  // gdf 05/24/00
		}  // gdf 05/24/00


		r+=2; // skip lines in case the file name wraps around

// Last Virus Found
		RTS_DISPLAYR (r++,1,reinterpret_cast<unsigned char*>(LS(IDS_LAST_INFECTION)));

		if (pStatBlock->VirusesFound) {// only display if a virus has been found
			t=VTimeToTime (pStatBlock->LastVirusFoundTime);
			RTS_DISPLAY (r,		2,					reinterpret_cast<unsigned char*>(LS(IDS_DATE_TIME)));
			RTS_DISPLAYI(r++,	RTSLastVirusColumn,	reinterpret_cast<unsigned char*>(_ctime (&t)));

			RTS_DISPLAY (r,		2,					reinterpret_cast<unsigned char*>(LS(IDS_ACTION_TAKEN)));
			RTS_DISPLAYI(r++,	RTSLastVirusColumn,	reinterpret_cast<unsigned char*>(ActionMessage(pStatBlock->LastVirusFoundAction)));

			RTS_DISPLAY (r,		2,					reinterpret_cast<unsigned char*>(LS(IDS_VIRUS_NAME)));
			RTS_DISPLAYI(r++,	RTSLastVirusColumn,	reinterpret_cast<unsigned char*>(pStatBlock->LastVirusFoundName));

			RTS_DISPLAY (r,		2,					reinterpret_cast<unsigned char*>(LS(IDS_INFECTED_FILE)));
			if(strlen(pStatBlock->LastVirusFoundFile) >= 180) // gdf 05/05/00
			{  // gdf 05/05/00
				char longtemp[180];   // gdf 05/05/00
				memset(longtemp, 0, sizeof(longtemp));  // gdf 05/05/00
				strncpy(longtemp, pStatBlock->LastVirusFoundFile, 80);  // gdf 05/05/00
				strcat(longtemp, ".....");   // gdf 05/05/00
				strcat(longtemp, pStatBlock->LastVirusFoundFile + strlen(pStatBlock->LastVirusFoundFile) - 80);  // gdf 05/05/00
				RTS_DISPLAYI(r++, RTSLastVirusColumn, reinterpret_cast<unsigned char*>(longtemp));  // gdf 05/05/00
			}  // gdf 05/05/00
			else  // gdf 05/05/00
				RTS_DISPLAYI(r++, RTSLastVirusColumn, reinterpret_cast<unsigned char*>(pStatBlock->LastVirusFoundFile));
		}
		else {
			r++; // skip a line to display message
			RTS_DISPLAYJ (JCENTER,r,(StatPortWidth-2)/2,LS(IDS_NO_RTS_INFECT));
		}
	}
	else {
		r++;
		RTS_DISPLAYJ (JCENTER,r,(StatPortWidth-2)/2,LS(IDS_NO_RTS_SCANNED));
	}
}
/************************************************************************************/
/*void DumpPscanStatus(PSCAN_STATUS scan) {

	FILE *fp=fopen("pscan.txt","at");
	if(!fp) return;

	fprintf(fp,"ADDRESS=%x\n",scan);
	fprintf(fp,"Size=%d\n",scan->Size);
	fprintf(fp,"next=%x\n",scan->next);
	fprintf(fp,"CurrentFile=%s\n",scan->CurrentFile);
	fprintf(fp,"ExtList=%s\n",scan->ExtList);
	fprintf(fp,"HaveInfections=%d\n",scan->HaveInfections);
	fprintf(fp,"LastReadNode=%d\n",scan->LastReadNode);
	fprintf(fp,"NextNode=%d\n",scan->NextNode);
	fprintf(fp,"han=%d\n",scan->han);
	fprintf(fp,"hkey=%d\n",scan->hkey);
	fprintf(fp,"Status=%d\n",scan->Status);
	fprintf(fp,"time=%d\n",scan->time);

//	VTIME StartTime;
//	fprintf(fp,"=%d\n",scan->);

	fprintf(fp,"Dirs=%d\n",scan->Dirs);
	fprintf(fp,"Files=%d\n",scan->Files);
	fprintf(fp,"Scanned=%d\n",scan->Scanned);
	fprintf(fp,"Infected=%d\n",scan->Infected);
	fprintf(fp,"Viruses=%d\n",scan->Viruses);
	fprintf(fp,"NotOpen=%d\n",scan->NotOpen);
	fprintf(fp,"ScanType=%d\n",scan->ScanType);

//	SCANCBVIRUS cbVirus;
//	fprintf(fp,"=%d\n",scan->);
//	SCANCBPROG cbProgress;
//	fprintf(fp,"=%d\n",scan->);

	fprintf(fp,"InfHan=%d\n",scan->InfHan);
	fprintf(fp,"logger=%d\n",scan->logger);
	fprintf(fp,"inuse=%d\n",scan->inuse);

//	FREECB pFree;
//	fprintf(fp,"=%d\n",scan->);

	fprintf(fp,"CurDir=%d\n",scan->CurDir);
	fprintf(fp,"MaxDir=%d\n",scan->MaxDir);
	fprintf(fp,"FileCounts=%d\n",*scan->FileCounts);
	fprintf(fp,"LastVirusName=%s\n",scan->LastVirusName);
	fprintf(fp,"LastInfectedFileName=%s\n",scan->LastInfectedFileName);

//	void *FileState;
//	fprintf(fp,"=%d\n",scan->);

	fprintf(fp,"DrivesUsed=%d\n",scan->DrivesUsed);
	fprintf(fp,"WriteCheck=%d\n",scan->WriteCheck);
	fprintf(fp,"ScanID=%d\n",scan->ScanID);
	fprintf(fp,"GroupID=%d\n",scan->GroupID);

//	void *context;
//	fprintf(fp,"=%d\n",scan->);
//	PNOSCANDIR NoScanDir;
//	fprintf(fp,"=%d\n",scan->);

	fprintf(fp,"ZipDepth=%d\n",scan->ZipDepth);
	fprintf(fp,"Flags=%d\n",scan->Flags);
	fprintf(fp,"Computer=%s\n",scan->Computer);

	fprintf(fp,"**************************************\n\n");

	fclose(fp);
}*/
/************************************************************************************/
DWORD VTime2TMstruct(VTIME cur,struct tm *TM)
{
	TM->tm_year = cur.year + 70;
	TM->tm_sec = cur.sec;
	TM->tm_min = cur.min;
	TM->tm_hour = cur.hour;
	TM->tm_mday = cur.day;
	TM->tm_mon = cur.month;

	return ERROR_SUCCESS;
}
/************************************************************************************/
void DisplayFSScanStatus (BOOL clear, CScanStatus *VisibleScan){ // Display File Server Scan Status

	int l;
	char buf[50] = "";
//	time_t t;
	struct tm TM = { 0 };
	time_t tmptime;
	static int lastScan,lastStat,lastSc,lastVi;

	if (!nutH || !fssPCB) return;

	if (prevDisp != StatusDisp ||
		lastScan != VisibleScan->han ||
		lastStat != VisibleScan->Status  ||
//		lastVi != VisibleScan->Viruses ||
		lastVi != VisibleScan->GetVirusesCount() ||		  
//		lastSc != VisibleScan->Scanned || clear) {
        lastSc != VisibleScan->GetScannedFilesCount() || clear) {		  

//		DumpPscanStatus(VisibleScan);

		NWSDrawPortalBorder (fssPCB);
		NWSClearPortal (fssPCB);

		lastScan = VisibleScan->han;
		lastStat = VisibleScan->Status;
		prevDisp = StatusDisp;
		lastSc = VisibleScan->GetScannedFilesCount();
		lastVi = VisibleScan->GetVirusesCount();
		
	}

	if (NumberOfScans==0) { // if no scans have run yet we can't display any -> say so and return
		FSS_DISPLAYJ (JCENTER,1,(StatPortWidth-2)/2,LS(IDS_FS_NO_SCANS));
		return;
	}

	if (VisibleScan==NULL) {	// if we somehow got onto an empty scan
		FSS_DISPLAY (1,5,reinterpret_cast<unsigned char*>(LS(IDS_FS_EMPTY_SCAN)));	// we should never get here but if we do
		return;										// print a message and return
	}

// now we can assume that we're actually pointing at a valid scan so we can display it

	l=0;
	switch (VisibleScan->Status) {
		case S_STARTING: 		StrCopy (buf,LS(IDS_S_STARTING)); break;
		case S_STARTED: 		StrCopy (buf,LS(IDS_S_STARTED)); break;
		case S_STOPPING:		StrCopy (buf,LS(IDS_S_STOPPING)); break;
		case S_SCANNING_DIRS:	StrCopy (buf,LS(IDS_S_SCANNING_DIRS)); break;
		case S_SCANNING_BOOT:	StrCopy (buf,LS(IDS_S_SCANNING_BOOT)); break;
		case S_SCANNING_MEM:	StrCopy (buf,LS(IDS_S_SCANNING_MEM)); break;
		case S_SCANNING_FILES:	StrCopy (buf,LS(IDS_S_SCANNING_FILES)); break;
		case S_ABORTED:			StrCopy (buf,LS(IDS_S_INTERRUPTED)); break;
		case S_DELAYED:			StrCopy (buf,LS(IDS_S_DELAYED)); break;
		case S_NEVER_RUN:		StrCopy (buf,LS(IDS_S_NEVER_RUN)); break;
		case S_DONE:			StrCopy (buf,LS(IDS_S_DONE)); break;
	}
	FSS_DISPLAY (l,		1,	reinterpret_cast<unsigned char*>(LS(IDS_FS_SCAN_STAT)));
	FSS_DISPLAYI(l++,	FSSStatColumn,	reinterpret_cast<unsigned char*>(buf));

	FSS_DISPLAY (l,		1,	reinterpret_cast<unsigned char*>(LS(IDS_SCAN_NUMBER)));
	FSS_DISPLAYI(l++,	FSSStatColumn,	reinterpret_cast<unsigned char*>(itoa(VisibleScan->han,buf,10)));

	VTime2TMstruct( VisibleScan->StartTime, &TM );

	tmptime = mktime(&TM);
	TM = *localtime(&tmptime);
	if(TM.tm_mday)
		strftime( buf, sizeof(buf), "%x %X", &TM ); // DDD MMM dd hh:mm:ss YYYY

	FSS_DISPLAY (l,		1,	reinterpret_cast<unsigned char*>(LS(IDS_FS_SCAN_DATE)));
	FSS_DISPLAYI(l++,	FSSStatColumn,	reinterpret_cast<unsigned char*>(buf));

	ThreadSwitchWithDelay();

	if (ScanIsRunning (VisibleScan->Status)) 	// if the scan is still running then
		l=DisplayRunningScan(VisibleScan,l); 	// draw the scan with progress bar, etc
	else {										// else the scan is done so just print number of files scanned and number of infected files
//		sssnprintf (buf,sizeof(buf),"%d",VisibleScan->Scanned);
		FSS_DISPLAY (l,		1,	reinterpret_cast<unsigned char*>(LS(IDS_FS_FILES_SCANNED)));
		FSS_DISPLAYI(l++,	FSSStatColumn,	reinterpret_cast<unsigned char*>(ULtoA(VisibleScan->GetScannedFilesCount(),buf,10)));

//		sssnprintf (buf,sizeof(buf),"%d",VisibleScan->Infected);
 		FSS_DISPLAY (l,		1,	reinterpret_cast<unsigned char*>(LS(IDS_FS_INF_FILES)));
		FSS_DISPLAYI(l++,	FSSStatColumn,	reinterpret_cast<unsigned char*>(ULtoA(VisibleScan->GetInfectedFilesCount(),buf,10)));
	}
	ThreadSwitchWithDelay();
	l=DisplayLastInfection (VisibleScan,l);
}
/************************************************************************************/
void DisplaySpinner(int r,int n) {

	char str[2];

	str[0] = Spinner[SpinLoc];
	str[1] = 0;

	NWSDisplayTextInPortal (r,0,reinterpret_cast<unsigned char*>(str),VNORMAL,histPCB);
}
/************************************************************************************/
void DisplayScanHistory(BOOL clear) {

	char str[20] = "";
	int i,n,l;
//	time_t t;
	struct tm TM = { 0 };
	time_t tmptime;

	if (StatusDisp==dSCSCAN || StatusDisp==dHELP) return;
	if (!nutH || !histPCB) return;

	memset (str,0,sizeof(str));
	sssnprintf (str,sizeof(str),"%-14s",LS(IDS_REALTIME));

LOCK();
	if (clear) NWSClearPortal (histPCB);
	NWSDrawPortalBorder (histPCB);

	if (Highlight==0) {
		StatusDisp=dRTSCAN;
		NWSDisplayTextInPortal (0,1,reinterpret_cast<unsigned char*>(str),VREVERSE,histPCB);
		DisplayRTSStatus(clear);
	}
	else {
		StatusDisp=dFSSCAN;
		NWSDisplayTextInPortal (0,1,reinterpret_cast<unsigned char*>(str),VINTENSE,histPCB);
	}

	if (!NumberOfScans) {
		UNLOCK();
		return;
	}
	for (n=MRS+NS, l=1 ; n>MRS ; l++,n--) { // <<<<<< Changed
		CScanStatus *block;

		i=n%NS;
		if ((block = UI_SCAN_LIST(i))==NULL) continue;
		if (!block->han) continue;
		block->AddRef();

		VTime2TMstruct( block->StartTime, &TM );
		tmptime = mktime(&TM);
		TM = *localtime(&tmptime);
		strftime( str, sizeof(str), "%m/%d %X", &TM ); // mm/dd hh:mm:ss

		NWSDisplayTextInPortal (l,0,reinterpret_cast<unsigned char*>(const_cast<char*>("              ")),VNORMAL,histPCB);
		if (ScanIsRunning (block->Status)) DisplaySpinner (l,block->han);
		else NWSDisplayTextInPortal (l,0,reinterpret_cast<unsigned char*>(const_cast<char*>(" ")),VNORMAL,histPCB);
		if (l==Highlight) {
			CurrentScan=i;
			NWSDisplayTextInPortal (l,1,reinterpret_cast<unsigned char*>(str),VREVERSE,histPCB);
			DisplayFSScanStatus (clear,block); // display the highlighted scan
		}
		else {
			NWSDisplayTextInPortal (l,1,reinterpret_cast<unsigned char*>(str),VNORMAL,histPCB);
		}
		block->Release();
		}
UNLOCK();
	ThreadSwitchWithDelay();
}
/************************************************************************************/


static int DisplayRunningScan(CScanStatus *s,int l) { // Changed
	char File[IMAX_PATH+1];
	char File1[IMAX_PATH+1];
	char buf[50];

    
	char *CurrentFile;
	DWORD FileSize = IMAX_PATH;
	
	if (!s || !nutH || !fssPCB) return l;
    CurrentFile = (char*)malloc(IMAX_PATH+1);

	memset (CurrentFile,0,IMAX_PATH+1);
	memset (buf,0,50);

	switch (s->Status) {
	case S_SCANNING_DIRS:
//		dfprintf (scanfp,"Scanning Dirs: ");//df:%d\tff:%d\n",s->Dirs,s->Files);
//		dfprintf (scanfp,"df:%d:%s\t",s->Dirs,buf);
		FSS_DISPLAY (l, 	1,	reinterpret_cast<unsigned char*>(LS(IDS_FS_DIRS_FOUND)));
		FSS_DISPLAYI(l++, 	FSSStatColumn,	reinterpret_cast<unsigned char*>(ULtoA(s->Dirs,buf,10)));

//		dfprintf (scanfp,"ff:%d:%s\n",s->Files,buf);
		l++;
		break;

	case S_SCANNING_FILES:
	case S_DONE:
	case S_STOPPING:
//		sssnprintf (buf,sizeof(buf),"%d",s->Files);

//		sssnprintf (buf,sizeof(buf),"%d",s->Scanned);
		FSS_DISPLAY (l,		1,	reinterpret_cast<unsigned char*>(LS(IDS_FS_FILES_SCANNED)));
		FSS_DISPLAYI(l++,	FSSStatColumn,	reinterpret_cast<unsigned char*>(ULtoA(s->GetScannedFilesCount(),buf,10)));

		
//		sssnprintf (buf,sizeof(buf),"%d",s->Infected);
		FSS_DISPLAY (l, 	1,	reinterpret_cast<unsigned char*>(LS(IDS_FS_INF_FILES)));
		FSS_DISPLAYI(l++, 	FSSStatColumn,	reinterpret_cast<unsigned char*>(ULtoA(s->GetInfectedFilesCount(),buf,10)));
		break;
	}

//bnm start: don't display progress bar
//	l++;
//	l=DrawProgressBar(s,l);
//bnm end

// Current File
	if (s->Status==S_SCANNING_FILES) { // only display the current file if the displayed scan is scanning files
		static int lastVi;
		static int lastSc;
		static int lastLen;

//		dfprintf (scanfp,"Scan is Running: ff:%d\tsc:%d\tif:%d\n",s->Files,s->Scanned,s->Infected);

		l+=2;
		if (s->GetScannedFilesCount()) {				// if we haven't actually scanned any files yet we can't display them
			s->GetCurrentFile(CurrentFile, FileSize );
			int filelen=NumChars(CurrentFile);
			if (lastVi!=s->GetVirusesCount() || lastSc!=s->GetScannedFilesCount() || lastLen != filelen)
			{	// if we haven't scanned any more files since last time we displayed don't display
				FssClearLine (l);			// clear the file line
				FssClearLine (l+1);
				lastVi = s->GetVirusesCount();
				lastSc = s->GetScannedFilesCount();
				lastLen = filelen;
			}
			FSS_DISPLAY (l,		1,	reinterpret_cast<unsigned char*>(LS(IDS_FS_CURR_FILE)));

			if (filelen) {
				if ((filelen-(StatPortWidth-FSSCurrentFileColumn))>(StatPortWidth-FSSCurrentFileColumn))
				{ // file is longer than two lines
//					char File[IMAX_PATH+1];

// gdf 05/15/00						*q=NULL,
// gdf 05/15/00						*p=NULL,
// gdf 05/15/00						Name[IMAX_PATH],

//					strcpy (File, s->CurrentFile);  // gdf 05/15/00

					memset(File, 0, IMAX_PATH+1);  // gdf 05/15/00
					memset(File1, 0, IMAX_PATH+1);  // gdf 05/15/00
					s->GetCurrentFile( CurrentFile, FileSize );
					strncpy (File, CurrentFile, 40); // gdf 05/15/00
					strcat(File, ".....");  // gdf 05/15/00
					strncpy(File1, CurrentFile + strlen(CurrentFile) - 50, 50);  // gdf 05/15/00

/*  begin gdf 05/15/00
					q=StrRChar(File,'\\'); 				// find the actual filename (this will be the 8.3 name)
					if (!q) q=StrRChar(File,'/');
					if (q) {
						*q=0;
						strcpy (Name,q+1);				// save the filename so we can use it easily
					}
					while ( q && filelen && (filelen-(StatPortWidth-FSSCurrentFileColumn))>
							                         (StatPortWidth-FSSCurrentFileColumn)) {
						p=StrRChar(File,'\\');
						if (!p) p=StrRChar(File,'/');
						if (!p) {*q='\\';break;}
						q=p; 					// save posistion just in case we somehow run out of slashes
						p++;
						*p=0; 							// null the char after the slash
						strcat (File,"...\\"); 			// tack the "..." on the end
						strcat (File,Name);				// tack the filename on the end
						filelen = NumChars(File);
						if ((filelen-(StatPortWidth-FSSCurrentFileColumn))>(StatPortWidth-FSSCurrentFileColumn))
							*q=0;
					}
end gdf 05/15/00 */


					FSS_DISPLAYI(l++,	FSSCurrentFileColumn,	reinterpret_cast<unsigned char*>(File));
					FSS_DISPLAYI(l++,	FSSCurrentFileColumn,	reinterpret_cast<unsigned char*>(File1));  // gdf 05\21\00
				}
				else
				{    // gdf 05/21/00
					s->GetCurrentFile(CurrentFile, FileSize );
					FSS_DISPLAYI(l++,	FSSCurrentFileColumn,	reinterpret_cast<unsigned char*>(CurrentFile));
					l++;   // gdf 05/21/00
				}   // gdf 05/21/00
			}
			else
				FSS_DISPLAYI(l++,	FSSCurrentFileColumn,	reinterpret_cast<unsigned char*>(const_cast<char*>("<EMPTY FILENAME>")));
		}
		else {
			l++;
			FSS_DISPLAY (l, 	1,	reinterpret_cast<unsigned char*>(LS(IDS_FS_CURR_FILE)));
			FSS_DISPLAYI(l++,	FSSCurrentFileColumn,	reinterpret_cast<unsigned char*>(LS(IDS_FS_SEARCHING_DIRS)));	// display a message if we haven't scanned any files yet
		}
	}
	else
		FssClearLine (l);

	free(CurrentFile);
	
	return l;
}
/************************************************************************************/
int DrawProgressBar (CScanStatus *s,int l) {

	char progBar[PROG_BAR_LEN+5],buf[50];
	DWORD Percent,pDone,len=PROG_BAR_LEN;//,temp,b;

	if (!s || !nutH || !fssPCB) return l;
	memset (buf,0,50);

	memset (progBar,0,PROG_BAR_LEN+5); 					// clear the string
	memset (progBar,ProgressCharNotDone,PROG_BAR_LEN);	// fill with UNDONE chars
	if (s->Status==S_SCANNING_FILES) {						// calculate the percentage done and draw a DONE bar that much with the percentage in the middle
		Percent= s->Files ? ((s->GetScannedFilesCount()*100)/s->Files) : 0x00;  // calculate percent done but don't devide by zero
		if (Percent==100) 								//  We may not register all the way done because of the math in the percent calculation so if it's close say it's done
			memset (progBar,ProgressCharDone,PROG_BAR_LEN);
		else {											// otherwite put DONE chars across as a percentage of the Bar length
			pDone = (Percent * len) / 100;
			if (pDone>=len) pDone=len-1;
			memset (progBar,ProgressCharDone,pDone);	// set DONE chars
		}
		FSS_DISPLAY (l,3,reinterpret_cast<unsigned char*>(progBar)); 						// draw the bar on the screen
		sssnprintf (buf,sizeof(buf),"%3d%%",Percent);					// write the percentage in the middle of the bar
		FSS_DISPLAYR (l,(StatPortWidth-2)/2,reinterpret_cast<unsigned char*>(buf));
	}
	else if (s->Status==S_SCANNING_DIRS) {					// this bar is just to show progress so divide num of dirs by some arbitrary number and mod with bar len
		pDone = (s->Dirs/3)%len;
		if (pDone>=len) pDone=len-1;
		memset (progBar,ProgressCharDone,pDone);		// set DONE chars
		FSS_DISPLAY (l,3,reinterpret_cast<unsigned char*>(progBar));						// display it
		l+=2;											// display a message if we are looking for files
		FSS_DISPLAYJ (JCENTER,l, (StatPortWidth-2)/2,	LS(IDS_FS_SEARCHING_DIRS));
	}
	return l;
}
/************************************************************************************/
static int DisplayLastInfection(CScanStatus *s,int l) {

	static int lastIn;
	static int lastVi;
	static int lastLen;

	if (!s || !nutH || !fssPCB) return l;

	if (!ScanIsRunning(s->Status) || s->GetScannedFilesCount()) {
		l++;
		FSS_DISPLAYR (l++,1,reinterpret_cast<unsigned char*>(LS(IDS_LAST_INFECTION)));
		if (s->GetInfectedFilesCount()) {
			int filelen = strlen(s->LastInfectedFileName);

			if (lastIn != s->GetScannedFilesCount() || lastVi != s->GetVirusesCount() || lastLen != filelen) {
				FssClearLine (l);
				FssClearLine (l+1);
				FssClearLine (l+2);
				FssClearLine (l+3);
				lastIn = s->GetScannedFilesCount();
				lastVi = s->GetVirusesCount();
				lastLen = filelen;
			}

			FSS_DISPLAY (l,		2,	reinterpret_cast<unsigned char*>(LS(IDS_VIRUS_NAME)));
			FSS_DISPLAYI(l++,	FSSLastVirusColumn,	reinterpret_cast<unsigned char*>(s->LastVirusName));
			FSS_DISPLAY (l, 	2,	reinterpret_cast<unsigned char*>(LS(IDS_INFECTED_FILE)));
			l=FSS_DISPLAYI(l++,	FSSLastVirusColumn,	reinterpret_cast<unsigned char*>(s->LastInfectedFileName));
			}
		else {
			char type[50], status[200];

			GetLoggerStr(type, sizeof (type), s->logger);
			vpsnprintf(status, sizeof(status), LS(ScanIsRunning(s->Status) ? IDS_FS_NONE_FOUND_YET : IDS_FS_NONE_FOUND), type);
			l++;
			l=FSS_DISPLAYJ (JCENTER,l,(StatPortWidth-2)/2,status);
		}
	}
	return l;
}
/************************************************************************************/
void DisplaySCScanStatus (BOOL clear){ // Display Scheduled Scan Status

	char buf[50],ScanLine[80],WeekDay[40];
	int l=0;//s=0,
	time_t NextScanTime=0;
	ScheduledScan *scan;

	if (!nutH) return;

	if (schPCB==NULL)
		MyCreatePortal (&schPort,&schPCB,3,0,21,80,40,80,SAVE,
                        reinterpret_cast<unsigned char*>(LS(IDS_ACTIVE_SCHED_SCANS)),VNORMAL,SINGLE,VINTENSE,nutH);

//	if (nscPCB==NULL)
//		MyCreatePortal (&nscPort,&nscPCB,20,0,4,80,40,80,SAVE,NULL,0,SINGLE,VINTENSE,nutH);

	if (/*!nscPCB ||*/ !schPCB) {
		DisplayScanHistory(TRUE);
		return;
	}

	if (prevDisp!=StatusDisp || clear) {

		NWSClearPortal (schPCB);
		NWSDrawPortalBorder (schPCB);

		prevDisp = StatusDisp;
	}

	NextScanTime=FindNextScan(0);				// check the scheduled scans

/*	if (NextScanTime) {
		DISP(0,	1,	LS(IDS_NEXT_SCAN),VNORMAL,nscPCB);
		DISP(0,	30,	_ctime(&NextScanTime),VINTENSE,nscPCB);

		DISP(1,	1,	LS(IDS_COUNT_DOWN),VNORMAL,nscPCB);
		DISP(1,	30,	CalcTimeDiff(NextScanTime,CurrentTime,buf,sizeof(buf)),VINTENSE,nscPCB);
	}
	else {
		DISP(0,	1,	LS(IDS_NEXT_SCAN),VNORMAL,nscPCB);
		DISP(0,	30,	LS(IDS_NO_SCH_SCAN),VINTENSE,nscPCB );
		ClearLine (1,nscPCB);
	}*/

	memset (ScanLine,0,sizeof(ScanLine));

	for (scan=SchScanList,l=0 ; l<17 && scan ; scan=scan->next) {
		ThreadSwitchWithDelay();
		if (scan->Type && scan->Enabled) {
			memset (ScanLine,' ',74);
			switch (scan->Type) {
				case 1:
					sssnprintf (ScanLine,sizeof(ScanLine),LS(IDS_SCH_DAILY),CalcTimeOfDay(scan->MinOfDay,buf,sizeof(buf)));
					break;
				case 2:
					memset (WeekDay,0,30);
					switch (scan->DayOfWeek) {
						case 0: StrCopy (WeekDay,LS(IDS_SCH_SUNDAY));		break;
						case 1: StrCopy (WeekDay,LS(IDS_SCH_MONDAY));		break;
						case 2: StrCopy (WeekDay,LS(IDS_SCH_TUESDAY));		break;
						case 3: StrCopy (WeekDay,LS(IDS_SCH_WEDNESDAY));	break;
						case 4: StrCopy (WeekDay,LS(IDS_SCH_THURSDAY));		break;
						case 5: StrCopy (WeekDay,LS(IDS_SCH_FRIDAY)); 		break;
						case 6: StrCopy (WeekDay,LS(IDS_SCH_SATURDAY));		break;
					}
					sssnprintf (ScanLine,sizeof(ScanLine),LS(IDS_SCH_WEEKLY),WeekDay,CalcTimeOfDay(scan->MinOfDay,buf,sizeof(buf)));
					break;
				case 3:
					sssnprintf (ScanLine,sizeof(ScanLine),LS(IDS_SCH_MONTHLY),scan->DayOfMonth,CalcTimeOfDay(scan->MinOfDay,buf,sizeof(buf)));
					break;
			}
			ScanLine[NumBytes(ScanLine)] = ' ';
			SCH_DISPLAY (l,1,reinterpret_cast<unsigned char*>(ScanLine));
			l++;
		}

		if (l==0)
			SCH_DISPLAYJ (JCENTER,1,(StatPortWidth-2)/2,LS(IDS_SCH_NO_ENABLED));
	}
	if (l!=lastNScans) { // we've changed number of scheduled scans
		lastNScans = l;
		ThreadSwitchWithDelay();

		DisplaySCScanStatus(TRUE);
	}
}
/************************************************************************************/
void DisplayStatusPortal() {

	CurrentTime = time (NULL);

	ThreadSwitchWithDelay();
	SpinLoc++;
	SpinLoc%=4;

	if (!SpinLoc) // only re-read the domain name every couple of seconds
		ReadDomainName();

	ConstructCommandBar ();
	STATUS_BAR_S(CommandBar);

	DisplayDates();
	switch(StatusDisp) {

	case dRTSCAN: case dFSSCAN:
		DisplayScanHistory (FALSE);
		break;

	case dSCSCAN:
	LOCK();
		DisplaySCScanStatus(FALSE);
	UNLOCK();
		NTxSleep (300);
		break;

	case dHELP:
	LOCK();
		DisplayHelp(FALSE);
	UNLOCK();
		NTxSleep (300);
		break;
	}

	ThreadSwitchWithDelay();
//	dprintf ("sd4:%d\n",StatusDisp);
}
/************************************************************************************/
void ConstructCommandBar () {

	int done,i;//,cs;
	char Spacing3[5]="   ";
	char *Spacing;

	for (done=i=0 ; !done && i<2 ; i++) {
		Spacing = &Spacing3[i]; 	// if we couldn't fit all the things on one line then
		memset (CommandBar,32,sizeof(CommandBar));	//decrease the size of the spacing and try again

		sssnprintf (CommandBar,sizeof(CommandBar)," %s%s",LS (IDS_CMD_QUIT),Spacing); // "Alt+f10=Quit"

		if (StatusDisp==dRTSCAN || StatusDisp==dFSSCAN) {
			if (ScanIsRunningNow)	// if the scan is running then display the stop scan message
				StrCat (CommandBar, LS(IDS_CMD_STOP_SCAN));	// "f5=stop scan"
			else					// otherwise desplay the start scan message
				StrCat (CommandBar, LS(IDS_CMD_START_SCAN));// "f5=start scan"
			StrCat (CommandBar,Spacing);
		}

		if (StatusDisp==dRTSCAN || StatusDisp==dFSSCAN) { // if we're viewing rts or server scan status
			StrCat (CommandBar, LS(IDS_CMD_VIEW_SCHED)); // add "f2=scheduled scans"
			StrCat (CommandBar,Spacing);
		}
		else if (StatusDisp==dSCSCAN) { // if we're displaying the scheduled scan portals
			while (NumBytes (CommandBar) < (79-NumBytes (LS(IDS_CMD_RETURN))))
				StrCat (CommandBar," ");
			StrCat (CommandBar, LS(IDS_CMD_RETURN)); // add "esc=return"
			StrCat (CommandBar,Spacing);
		}

		if (StatusDisp==dRTSCAN || StatusDisp==dFSSCAN) {
			while (NumBytes (CommandBar) < (79-NumBytes (LS(IDS_CMD_HELP))))
				StrCat (CommandBar," ");
			StrCat (CommandBar, LS(IDS_CMD_HELP)); // add "f1=help"
		}
		else if (StatusDisp==dHELP) {
			while (NumBytes (CommandBar) < (79-NumBytes (LS(IDS_CMD_EXIT_HELP))))
				StrCat (CommandBar," ");
			StrCat (CommandBar, LS(IDS_CMD_EXIT_HELP)); // "esc=exit help"
		}

		if (NumBytes (CommandBar)<80) done=1; // if we fit on one line then we're done
	}
	CommandBar[NumBytes (CommandBar)]=' '; //so the rest of the line is just spaces
	ThreadSwitchWithDelay();
}
/************************************************************************************/
void DisplayDates() {

	char timestr[50],*ct;
	int col=COL_AFTER(IDS_UP_TIME,54);

 	ct=_ctime(&CurrentTime);
	NWSDisplayTextInPortal (0,54,reinterpret_cast<unsigned char*>(ct),VREVERSE,headerPCB);

	NWSDisplayTextInPortal (1,54,reinterpret_cast<unsigned char*>(LS(IDS_UP_TIME)),VREVERSE,headerPCB);
	NWSDisplayTextInPortal (1,col,reinterpret_cast<unsigned char*>(const_cast<char*>("                  ")),VREVERSE,headerPCB); // clear to the end of the line
	NWSDisplayTextInPortal (1,col,reinterpret_cast<unsigned char*>(CalcTimeDiff (CurrentTime,VTimeToTime (pStatBlock->TimeStarted),timestr,sizeof(timestr))),VREVERSE,headerPCB);
}
/************************************************************************************/
void DrawHeader() {

	char Server[85];
	int l;

	memset (Server,0,sizeof(Server));
	memset (ServerName,0,NAME_SIZE);

	GetServerInformation (50,&ServerInfo); // get the server name
	StrNCopy (ServerName,ServerInfo.serverName,NAME_SIZE);	// put it in a string we can manipulate

	sssnprintf (Server,sizeof(Server),"  %s '%s'",LS(IDS_SERVER_NAME),ServerName);
	for (l=NumBytes (Server) ; NumBytes (Server) < 81 ; l++)
		Server[l] = ' ';

	NWSClearPortal (headerPCB);

	ReadDomainName();

	NWSDisplayTextInPortal (0,0,reinterpret_cast<unsigned char*>(Server),VREVERSE,headerPCB);
}
/************************************************************************************/
int ReturnLargestNumber(int c1,int c2, int c3, int c4) {
	return ((c1>c2) ? ( (c1>c3) ? ((c1>c4)?c1:c4) : ((c3>c4)?c3:c4) ) : ( (c2>c3) ? ((c2>c4)?c2:c4) : ((c3>c4)?c3:c4) ));
}
/************************************************************************************/
void CalculateColumnPositions() {
// This funtion calculates the column to position the various fields in the realtime
// and scheduled scan portals according to the actual length of the header strings.
// Since they are displayed in groups I calculate the column position relative to the
// length of the longest header string in the group.  --Evan 97.6.17

	int c1,c2,c3,c4,c5,c6,c7;

	c1=NumBytes(LS(IDS_SCAN_DIRECTION));
	c2=NumBytes(LS(IDS_TOTAL_SCANNED));
	c3=NumBytes(LS(IDS_INFECTED_FOUND));
	RTSStatColumn=ReturnLargestNumber (c1,c2,c3,0) + 1; // 30


	c1=NumBytes(LS(IDS_USER_NAME));
	c2=NumBytes(LS(IDS_FILE));
	RTSLastFileColumn=ReturnLargestNumber (c1,c2,0,0) + 2; //9


	c1=NumBytes(LS(IDS_DATE_TIME));
	c2=NumBytes(LS(IDS_ACTION_TAKEN));
	c3=NumBytes(LS(IDS_VIRUS_NAME));
	c4=NumBytes(LS(IDS_INFECTED_FILE));
	RTSLastVirusColumn=ReturnLargestNumber (c1,c2,c3,c4) + 2;//18

	c1=NumBytes(LS(IDS_FS_SCAN_STAT));
	c2=NumBytes(LS(IDS_SCAN_NUMBER));
	c3=NumBytes(LS(IDS_FS_SCAN_DATE));
	c4=0;
	c5=ReturnLargestNumber (c1,c2,c3,c4);

	c1=NumBytes(LS(IDS_FS_FILES_SCANNED));
	c2=NumBytes(LS(IDS_FS_INF_FILES));
	c3=NumBytes(LS(IDS_FS_DIRS_FOUND));
	c4=0;
	c6=ReturnLargestNumber (c1,c2,c3,c4);

	c1=0;
	c2=NumBytes(LS(IDS_FS_FILES_SCANNED));
	c3=NumBytes(LS(IDS_FS_INF_FILES));
	c7=ReturnLargestNumber (c1,c2,c3,0);

	FSSStatColumn=ReturnLargestNumber (c5,c6,c7,0)+1;//30

	FSSCurrentFileColumn=NumBytes(LS(IDS_FS_CURR_FILE))+1;//17

	c1=NumBytes(LS(IDS_VIRUS_NAME));
	c2=NumBytes(LS(IDS_INFECTED_FILE));
	FSSLastVirusColumn=ReturnLargestNumber (c1,c2,0,0)+2;//17

	VBINColumn= 14;
}
/************************************************************************************/
void SetupNut (void) {

	dprintf ("Setup Nut\n");
	NLMHandle = GetNLMHandle();
	UIallocTag = AllocateResourceTag (NLMHandle,
                                      reinterpret_cast<unsigned char*>(const_cast<char*>("RTVScan Alloc Tag")),
                                      AllocSignature);

	NWSInitializeNut (MSGNUM(IDS_MAIN_TITLE),MSGNUM(IDS_VERSION),SMALL_HEADER,NUT_REVISION_LEVEL,
		              reinterpret_cast<unsigned char**>(TheStringTable),
                      (BYTE *)(NWversion == 4 ? -1 : NULL),ScreenHan,UIallocTag,&nutH);

	MyCreatePortal (&funcPort,&funcPCB,24,0,1,80,1,80,NO_SAVE,NULL,VINTENSE,NOBORDER,VINTENSE,nutH);
	MyCreatePortal (&histPort,&histPCB,3,0,/*NS+*/5,80-StatPortWidth,25,20,NO_SAVE,
                    reinterpret_cast<unsigned char*>(LS(IDS_SCAN_HISTORY)),0,DOUBLE,VINTENSE,nutH);
	MyCreatePortal (&fssPort,&fssPCB,3,80-StatPortWidth,StatPortHeight,StatPortWidth,25,80,NO_SAVE,
                    reinterpret_cast<unsigned char*>(LS(IDS_FS_SCAN_TITLE)),VNORMAL,SINGLE,VINTENSE,nutH);
	MyCreatePortal (&rtsPort,&rtsPCB,3,80-StatPortWidth,StatPortHeight,StatPortWidth,25,80,NO_SAVE,
                    reinterpret_cast<unsigned char*>(LS(IDS_RT_SCAN_TITLE)),VNORMAL,SINGLE,VINTENSE,nutH);
	MyCreatePortal (&headerPort,&headerPCB,1,0,2,80,3,80,NO_SAVE,NULL,VINTENSE,NOBORDER,VINTENSE,nutH);

	ThreadSwitchWithDelay();

	helpPort=0;
	helpPCB=NULL;

	ConstructCommandBar ();
	STATUS_BAR_S(CommandBar);
	DrawHeader();

	StatusDisp = dRTSCAN;

	CalculateColumnPositions();

	DisplayScanHistory(TRUE);
}
/************************************************************************************/
void SetupUI (void) {

	dprintf ("Setup UI\n");
	if (MainScreenHan) {
		DestroyScreen (MainScreenHan);
		MainScreenHan=0;
	}

    ScreenHan = CreateScreen("Symantec AntiVirus Corporate Edition",DONT_CHECK_CTRL_CHARS|AUTO_DESTROY_SCREEN);
	if (ScreenHan==EFAILURE || ScreenHan==NULL) {
		RingTheBell();
		_printf (LS(IDS_ERR_CREATE_SCREEN));
		return;
	}
	SetCurrentScreen(ScreenHan);
	DisplayScreen (ScreenHan);
	SetAutoScreenDestructionMode(TRUE);

	lastKeyTime = CurrentTime = time (NULL);

	ScanIsRunningNow=FALSE;
#ifdef EnableScreenSaver
	ScreenSaveRunning=FALSE;
	if (NWversion == 3 || languageID != 4)
		RunScreenSave=FALSE;
#endif
	CurrentScan=-1;
	NumberOfScans=0;

	memset (UIScanList,0,sizeof(UIScanList));
	MRS=0;

	MainThreadGroup = GetThreadGroupID();

#ifdef EnableScreenSaver
	if (RegOpenKey (hMainKey,"ScreenSaver",&hScrKey)==ERROR_SUCCESS) {
		SecondsTillScrSave = GetVal (hScrKey,"Time",1200);
		RunScreenSave = GetVal (hScrKey,"Enabled",0);
	}
	else RunScreenSave=FALSE;
#endif
	StrCopy (UserName,PASS_KEY1);

	usePassword = TRUE;
#ifdef DEBUG
	if (debug&DEBUGPRINT)
		usePassword=FALSE;
#endif
}
/************************************************************************************/
void StartConsoleScreen(void) {

	DWORD cc;
	BOOL OtherScreen=TRUE;

//	scanfp = fopen ("sys:\\ScanStat.txt","wt");

	SetupUI();
	SetupNut();

dprintf ("Start Console Screen\n");
	while (nutH && SystemRunning) {

		ThreadSwitchWithDelay();
		CheckForRunningScans();
		RegDebug(/*FALSE*/);

		if (NWSKeyStatus(nutH)) {		// check for any key press
			if(GetKeyboardCommand()) {	// process the key press
				break;
			}
			lastKeyTime = time (NULL);	// reset the screen saver countdown
		}
		else
			NTxSleep(500); // don't wait if a key was pressed

			// start the screen saver running if no key pressed for a period of time
#ifdef EnableScreenSaver
		if (RunScreenSave && !ScreenSaveRunning && SecondsTillScrSave && CurrentTime > lastKeyTime+SecondsTillScrSave) {
			if (!ScreenSaveRunning)
				StartScreenSaver(ScreenHan,0);

			lastKeyTime = time (NULL);
			SecondsTillScrSave = GetVal (hScrKey,"Time",0);
		}
#endif
		if (CheckIfScreenDisplayed(ScreenHan,FALSE)) { 	// we only need to redraw the screen if the console screen is currently displayed
			if (OtherScreen) {					// if we were just displaying another screen then clear everything
				DisplayScanHistory(TRUE);
				OtherScreen=FALSE;
			}
			DisplayStatusPortal();
		}
		else {
#ifdef EnableScreenSaver
			lastKeyTime = time (NULL);	// if the screen is not being displayed reset the screen saver timer
#endif
			NTxSleep (500);				// and don't loop as fast
			OtherScreen=TRUE;
		}

        // check now and then to make sure that msgsys.nlm is patched - this check
        // is very fast - I ignore errors - if it isn't running, then I don't care,
        // and if the code isn't as expected, I have already told them

        (void) FixMsgSysNLM( );

		CurrentTime=time(NULL);
	}
#ifdef EnableScreenSaver
	RegCloseKey (hScrKey);
	ScreenSaveRunning=FALSE;
#endif
dprintf ("Starting to Unload Monitor\n");
	NWSDisplayTextInPortal (0,0,reinterpret_cast<unsigned char*>(LS(IDS_CMD_UNLOADING)),VRBLINK,funcPCB);
	cc=CurrentTime-VTimeToTime (pStatBlock->TimeStarted);
	if (cc<5) {		// if we unload too soon after starting we may have some thread problems
		dprintf ("Waiting %d seconds before unloading to avoid thread problems\n",(5-cc));
		NTxSleep ((5-cc)*1000);
	}
}
/************************************************************************************/
void CloseConsoleScreen(void) {

dprintf ("Close Console Screen\n");

	FreeScanList();

	if (nutH) {
		if (MenuOpen) {
			ungetch(27);
			NTxSleep (100);
		}
		NWSDestroyList (nutH);
		NWSDestroyMenu (nutH);
		NWSDestroyForm (nutH);
		NWSRestoreNut (nutH);
	}

	nutH=NULL;
	ScreenHan=0;
//	if (scanfp) {
//		fclose (scanfp);
//		scanfp=NULL;
//	}

	// note - we used to free the UIScanList here, but the memory that it points
	// to had already been freed, leaving us writing over freed memory. The code to
	// do this was moved to walker.c/FreeAllScan where it belonged anyway.

dprintf ("Console Screen Closed\n");
}

// clear references to the list of completed scans - the scan
// structures themselves are going to be freed and it is bad
// to leave the references to freed stuff laying around (some might
// use them - actually somebody was!)

void FreeUIScanList(void) {

	int i;

	for (i=0;i<NS;i++)
		if (UIScanList[i]) {
			UIScanList[i]->Release();
			UIScanList[i]=NULL;
		}

}


/************************************************************************************/


