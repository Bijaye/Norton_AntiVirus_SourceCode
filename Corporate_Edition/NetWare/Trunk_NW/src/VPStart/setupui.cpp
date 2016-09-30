// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#include "pscan.h"
#include "nlmui.h"
#include "slash.h"
#include "SymSaferStrings.h"

LONG msgPort=0;
PCB *msgPCB=NULL;

LONG PrintPort=0;
PCB *PrintPCB=NULL;

//LONG hlpPort=0;
//PCB *hlpPCB=NULL;

NUTInfo *InstNut=NULL;
LONG InstAllocTag=0;
LONG InstNLMHandle=0;
int InstScreenHan=0;
int PrintRow=0;

char InstallPassword[ENCODED_PASS];
extern "C" FILE *rfile; // install log file
extern "C" DWORD ErrorCount;

#define MyDestroyPortal(p,n)  {if (p && n) { NWSDestroyPortal (p,n); p=0;}}
extern "C" void PrintStuff(char *heading,char *string);

#define CLEAR_PORTAL(p)\
	NWSDrawPortalBorder (p);\
	NWSClearPortal (p);

#define CREATE_AUTHENTICATE_PORTAL	\
	{MyCreatePortal (&infoPort,&infoPCB,(LONG)8,(LONG)20,(LONG)15,(LONG)39,(LONG)128,(LONG)80,(LONG)SAVE,(BYTE*)NULL,(LONG)VINTENSE,SINGLE,(LONG)VINTENSE,(NUTInfo*)InstNut);\
	CLEAR_PORTAL(infoPCB);}
/*******************************************************************************************/
void NewSection(char* section) {

	if(!InstNut) return;

	iprintf(section);

	NWSFillPortalZone(0,0,1,80,' ',VREVERSE,msgPCB);
	NWSDisplayTextInPortal ((LONG)0, (LONG)1,(BYTE*)section,(LONG)VREVERSE,(PCB*)msgPCB);
}
/****************************************************************************************/
BOOL AbortInstall(char *abort) {

	LONG type=0;
	char key=0,*q;

	if (InstNut) {
		if (NWSKeyStatus(InstNut)) {
//			NWSGetKey(&type,&key,InstNut);
			NWSGetKey((LONG*)type, (BYTE*)key,(NUTInfo*)InstNut);
			q=abort;
			while (*q) {
				if (key==*q) {
					dprintf ("Abort Install\n");
					PrintToInstallPortal("Aborting Install");
					return TRUE;
				}
				switch (*q) {
					case 27: if (type==K_ESCAPE) {
						dprintf ("Abort Install\n");
						PrintToInstallPortal("Aborting Install");
						return TRUE;
					}
				}
				q++;
			}
		}
	}
	return FALSE;
}
/*******************************************************************************************/
char *InitMessage(int wId) {
	return LS(wId);
}
/*******************************************************************************************/
DWORD AskOverwrite(char*to,char*from) { // confirm overwrite of a newer file 

	char InfoMessage[512],destStr[512],sourceStr[512];
	DWORD ccode=0;
	struct stat tostat,fromstat;
	char *q;

	LONG infoPort=0;
	PCB *infoPCB=NULL;

	dprintf ("Ask Overwrite to:(%s) from:(%s)\n",to,from);
	if (rfile) fprintf(rfile,"Ask Overwrite to:(%s) from:(%s)\n",to,from);

	memset (InfoMessage,0,512);
	memset (destStr,0,512);
	memset (sourceStr,0,512);
	memset (&tostat,0,sizeof (struct stat));
	memset (&fromstat,0,sizeof (struct stat));
	
//	iprintf (LS(IDS_NEW_DEST));
//	iprintf ("File %s has a newer time than source\n",to);
	// form the string to print in the help portal
	if (GetFileStat (to,&tostat)) 
		sssnprintf (destStr,sizeof(destStr),LS(IDS_DEST_STATS),to,_ctime(&tostat.st_mtime),tostat.st_size);
	if (GetFileStat (from,&fromstat)) 
		sssnprintf (sourceStr,sizeof(sourceStr),LS(IDS_SRC_STATS),from,_ctime(&fromstat.st_mtime),fromstat.st_size);
	
	sssnprintf (InfoMessage,sizeof(InfoMessage),LS(IDS_CONFIRM_DEST_HELP),destStr,sourceStr);

	if (rfile) fprintf(rfile,"%s",InfoMessage);

	q=strrchr (to,cSLASH);
	if (q) q++;
	else q=to;
	PrintStuff(LS(IDS_CHECK_B4_OVERWRITE),q);

	MyCreatePortal (&infoPort,&infoPCB,(LONG)8,(LONG)2,(LONG)15,(LONG)76,(LONG)128,(LONG)80,(LONG)SAVE,(BYTE*)NULL,(LONG)VNORMAL,(LONG)SINGLE,VINTENSE,(NUTInfo*)InstNut);
	NWSDisplayTextInPortal ((LONG)0, (LONG)0, (BYTE*)InfoMessage,(LONG)VINTENSE,(PCB*)infoPCB);	

	ccode=NWSConfirm (MSGNUM(IDS_OVERWRITE_DEST),3,2,DEFAULT_NO,NULL,InstNut,NULL);
	if (ccode==-1) ccode=ERROR_SUCCESS;

	MyDestroyPortal (infoPort,InstNut);

	return ccode;
}
/************************************************************************************/
static int Pnum=0;
static BOOL ScrollNow=FALSE;
/************************************************************************************/
void PrintToInstallPortal(char *line) {

	if( !line || !InstNut || !PrintPCB ) return;
			

	if( !strlen(line) ) return; // if there's nothing in the string then just return.

	if( ScrollNow ) {
		int lines = strlen(line)/76;
		Pnum=20-lines;

		NWSScrollPortalZone(0,0,21,80,VNORMAL,lines+1,V_UP,PrintPCB);
		NWSFillPortalZoneAttribute(0,0,21,80,VNORMAL,PrintPCB);
	}

	Pnum = NWSDisplayTextInPortal ((LONG)Pnum, (LONG)1,(BYTE*)line, (LONG)VNORMAL, (PCB*)PrintPCB);
	if(Pnum>20){
		ScrollNow=TRUE;
		Pnum=20;
	}
//	else
//		ScrollNow=FALSE;
	NWSDrawPortalBorder (PrintPCB);
}
/************************************************************************************/
void SetupInstallUI(char Install) {

	REF(Install);

	dprintf ("SetupInstallUI\n");

	InstScreenHan = CreateScreen("Symantec AntiVirus",DONT_CHECK_CTRL_CHARS|AUTO_DESTROY_SCREEN);
	if (InstScreenHan==EFAILURE || InstScreenHan==NULL) {
		//RingTheBell();
		_printf (LS(IDS_ERR_INSTALL_SCREEN));
		return;
	}
	SetCurrentScreen (InstScreenHan);
	DisplayScreen (InstScreenHan);
	SetAutoScreenDestructionMode(TRUE);

	PrintRow=0;
	InstAllocTag=0;
	InstNLMHandle=0;
	Pnum=0;
	ScrollNow=FALSE;

	InstNLMHandle = GetNLMHandle();
	InstAllocTag = AllocateResourceTag ((LONG)InstNLMHandle, (const BYTE*)"Installation Alloc Tag", (LONG)AllocSignature);

	NWSInitializeNut (MSGNUM(IDS_MAIN_TITLE),MSGNUM(IDS_VERSION), SMALL_HEADER, NUT_REVISION_LEVEL,
		reinterpret_cast<unsigned char**>(TheStringTable), (BYTE *)(NWversion == 4 ? -1 : NULL),InstScreenHan/*MainScreenHan*/,InstAllocTag,
	    &InstNut);

//	MyCreatePortal (*Port,**Pcb,r,c,h,w,vh,vw,save,*headerText,headerAttr,borderType,borderAttr,*nuthandle) {

// main message window
	MyCreatePortal (&PrintPort,&PrintPCB,1,0,23,80,255,80,NO_SAVE,NULL,VNORMAL,SINGLE,VNORMAL,InstNut);

// line at the bottom of the screen
	MyCreatePortal (&msgPort,&msgPCB,24,0,1,80,1,80,NO_SAVE,NULL,VNORMAL,NOBORDER,VNORMAL,InstNut);

// other portal
//	MyCreatePortal (&hlpPort,&hlpPCB,8,40,16,39,128,80,NO_SAVE,NULL,VNORMAL,SINGLE,VNORMAL,InstNut);

	NWSDisplayTextInPortal ((LONG)0, (LONG)0,(BYTE*)LS(IDS_INSTALLING_LDVP),(LONG)VREVERSE, (PCB*)msgPCB);

	NWSSelectPortal (PrintPort,InstNut);
}
/************************************************************************************/
void DestroyInstallUI( DWORD waitTime, bool showPrompt )
// Destroy the install UI.  If waitTime != 0, first displays a "Please wait" message and waits the specified time (in milliseconds)
{

	if (InstNut == NULL) 
		return;

	dprintf ("Destroy Install UI\n");
    if (showPrompt)
        NWSStartWait(12,40,InstNut);

#ifdef DEBUG
    if(debug)
    {
        NWSSetDynamicMessage(DYNAMIC_MESSAGE_ONE,"Press any Key to Close this Window",&InstNut->messages);
        NWSAlert (3,40,InstNut,DYNAMIC_MESSAGE_ONE);
    }
#endif //DEBUG

    NTxSleep(waitTime);
    if (showPrompt)
        NWSEndWait(InstNut);

//	if(hlpPort)
//		MyDestroyPortal (hlpPort,InstNut);

	MyDestroyPortal (msgPort,InstNut);
	MyDestroyPortal (PrintPort,InstNut);

	msgPort=0;
	msgPCB=NULL;
	PrintPort=0;
	PrintPCB=NULL;
//	hlpPort=0;
//	hlpPCB=NULL;

	NWSRestoreNut (InstNut);

	InstNut=NULL;

	DestroyScreen (InstScreenHan);
	InstScreenHan=0;
}
/************************************************************************************/
void UseDefaultContext() {

	char *q;

	if (!dsInitialized)
		InitDS();

	if (dsInitialized && strlen(ServerDN)>0) {
		q=strchr (ServerDN,'.');

		dprintf ("Use Default Context\n");
	
		if (q) { // there should ALWAYS be a '.' in the distinguished name of the server ie CN=HAGGIS.OU=plasma.O=kronos
			q++;
			StrCopy(Container,q);
			dprintf ("Default Container=\"%s\"\n",Container);
		}
	}
}
/************************************************************************************/
BOOL GetContextUI (char *context) {

	int l=0;
	LONG infoPort=0;
	PCB *infoPCB=NULL;

	dprintf ("Get Context UI\n");

	if (!context) {
		dprintf ("Context is NULL\n");
		return FALSE;
	}
	
	CREATE_AUTHENTICATE_PORTAL;
//	MyCreatePortal (&infoPort,&infoPCB,8,20,15,39,128,80,SAVE,NULL,VNORMAL,SINGLE,VNORMAL,InstNut);
//	CLEAR_PORTAL(infoPCB);

	if (BadDefAuth) {
		l=NWSDisplayTextInPortal ((LONG)l,(LONG)0,(BYTE*)LS(IDS_ERROR_OPEN_PROF_LOG),(LONG)VNORMAL,(PCB*)infoPCB);
		l=NWSDisplayTextInPortal ((LONG)l,(LONG)0,(BYTE*)LS(IDS_BAD_CONTAINER),(LONG)VNORMAL,(PCB*)infoPCB);
		l=NWSDisplayTextInPortal ((LONG)l,(LONG)0,(BYTE*)LS(IDS_REENTER_CONTAINER),(LONG)VNORMAL,(PCB*)infoPCB);
		BadDefAuth=FALSE;
	}
	else {
		l=NWSDisplayTextInPortal ((LONG)l,(LONG)0,(BYTE*)LS(IDS_NO_CONTAINER),(LONG)VNORMAL,(PCB*)infoPCB);
		l=NWSDisplayTextInPortal ((LONG)l,(LONG)0,(BYTE*)LS(IDS_ENTER_CONTAINER),(LONG)VNORMAL,(PCB*)infoPCB);
	}

	NWSSetDynamicMessage((LONG)DYNAMIC_MESSAGE_ONE,(BYTE*)">",(MessageInfo*)&InstNut->messages);

	if (NWSEditString((LONG)4,(LONG)61,(LONG)1,(LONG)37,(LONG)MSGNUM(IDS_CONTAINER),(LONG)DYNAMIC_MESSAGE_ONE,
					  (BYTE*)context,(LONG)128,(LONG)EF_ANY,(NUTInfo*)InstNut,(LONG)NULL,(LONG)NULL,(LONG)NULL)!=E_ESCAPE) {
		CLEAR_PORTAL(infoPCB);
		if (NumBytes(context)<1) {
			UseDefaultContext();
			dprintf ("Invalid Context entered\n");
			return FALSE;
		}
		dprintf ("Context: %s\n",context);
		return TRUE;
	}

	UseDefaultContext();	

	MyDestroyPortal (infoPort,InstNut);

	dprintf ("Get context cancelled by ESC\n");
	return FALSE;
}	
/************************************************************************************/
BOOL AuthenticateUI(char *user,char*pass) { // used during the install procedure.  called from initnlm.c

	BOOL retval=FALSE;
	int l=0;
	LONG passPort=0;
	PCB *passPCB=NULL;
	char *AuthHelpMsg=NULL;
	LONG infoPort=0;
	PCB *infoPCB=NULL;

	dprintf ("Authenticate UI\n");
	memset (InstallPassword,0,ENCODED_PASS);

	CREATE_AUTHENTICATE_PORTAL;
//	MyCreatePortal (&infoPort,&infoPCB,8,20,15,39,128,80,SAVE,NULL,VNORMAL,SINGLE,VNORMAL,InstNut);
//	CLEAR_PORTAL(infoPCB);
//	NWSClearPortal (infoPCB);
	
	NWSSetDynamicMessage((LONG)DYNAMIC_MESSAGE_ONE,(BYTE*)">",(MessageInfo*)&InstNut->messages);
	
	if (BadDefAuth) {
		l=NWSDisplayTextInPortal ((LONG)l,(LONG)0,(BYTE*)LS(IDS_ERROR_AUTH_DS),(LONG)VNORMAL,(PCB*)infoPCB);
		l=NWSDisplayTextInPortal ((LONG)l,(LONG)0,(BYTE*)LS(IDS_BAD_USER_PASS),(LONG)VNORMAL,(PCB*)infoPCB);
		l=NWSDisplayTextInPortal ((LONG)l,(LONG)0,(BYTE*)LS(IDS_REENTER_USER_PASS),(LONG)VNORMAL,(PCB*)infoPCB);
		BadDefAuth=FALSE;
	}
	else {
        size_t AuthHelpMsgBytes = NumBytes(LS(IDS_AUTH_HELP1))+NumBytes(LS(IDS_AUTH_HELP2))+5;
		AuthHelpMsg = (char*)malloc(AuthHelpMsgBytes);
		if (AuthHelpMsg) {
			sssnprintf (AuthHelpMsg,AuthHelpMsgBytes,"%s%s",LS(IDS_AUTH_HELP1),LS(IDS_AUTH_HELP2));
			CLEAR_PORTAL(infoPCB);
			NWSDisplayTextInPortal ((LONG)l,(LONG)0,(BYTE*)AuthHelpMsg,(LONG)VNORMAL,(PCB*)infoPCB);
		}
		
		// -----------------------------
		// ksr - Certification, 7/21/2002 
		
		else
		{
			dprintf( "Cannot allocate memory in AuthenticateUI()\n" );
			return ERROR_MEMORY;
		}
		// -----------------------------
		
	}
	
//	NWSDisplayTextInPortal (0,0,LS(IDS_ENTER_USERNAME),VREVERSE,msgPCB);

	if (NWSEditString((LONG)4,(LONG)61,(LONG)1,(LONG)37,(LONG)MSGNUM(IDS_USERNAME),(LONG)DYNAMIC_MESSAGE_ONE,(BYTE*)user,
					  (LONG)ENCODED_PASS,EF_ANY,(NUTInfo*)InstNut,(LONG)NULL,(LONG)NULL,(LONG)NULL)!=E_ESCAPE) {

//		NWSDisplayTextInPortal (0,0,LS(IDS_ENTER_USERPASS),VREVERSE,msgPCB);

//	MyCreatePortal (LONG *Port,PCB **Pcb,LONG r,LONG c,LONG h,LONG w,LONG vh,LONG vw,LONG save,BYTE *headerText,
//					LONG headerAttr,LONG borderType,LONG borderAttr,NUTInfo *handle) {

		if (MyCreatePortal (&passPort,&passPCB,(LONG)2,(LONG)40,(LONG)5,(LONG)39,(LONG)5,(LONG)256,(LONG)SAVE,
							(BYTE*)LS(IDS_PROFILE_PASSWORD),(LONG)VINTENSE,(LONG)DOUBLE,(LONG)VNORMAL,(NUTInfo*)InstNut)) {
		
			if (ReadPassword (InstallPassword,passPCB,ENCODED_PASS,InstNut)!=-1) {
				StrCopy (pass,InstallPassword);
				retval=TRUE;
			}
			else dprintf ("Enter Password cancelled by escape\n");

			NWSDestroyPortal (passPort,InstNut);
		}
		else dprintf ("Couldn't Create Password portal\n");
	}
	else dprintf ("Enter Username cancelled by escape\n");

//	NWSDisplayTextInPortal (0,0,LS(IDS_INSTALLING_LDVP),VREVERSE,msgPCB);
	if (AuthHelpMsg) {
		free(AuthHelpMsg);
		AuthHelpMsg=NULL;
	}
	
	MyDestroyPortal (infoPort,InstNut);

	return retval;
}
/************************************************************************************/
void FailedAuthentication(DWORD ccode){

	LONG infoPort=0;
	PCB *infoPCB=NULL;

	CREATE_AUTHENTICATE_PORTAL;
//	MyCreatePortal (&infoPort,&infoPCB,8,20,15,39,128,80,SAVE,NULL,VNORMAL,SINGLE,VNORMAL,InstNut);
//	CLEAR_PORTAL(infoPCB);

	dprintf ("Failed Authentication\n");
	switch(ccode) {
		case 100: 
			NWSDisplayTextInPortal ((LONG)0,(LONG)0,(BYTE*)LS(IDS_NEED_NEW_USER),(LONG)VINTENSE,(PCB*)infoPCB); break;
		case EI_DISK_WRITE: case 101: 
			NWSDisplayTextInPortal ((LONG)0,(LONG)0,(BYTE*)LS(IDS_BAD_RIGHTS1),(LONG)VINTENSE,(PCB*)infoPCB); break;
		case EI_GET_CONTEXT: case 102: 
			NWSDisplayTextInPortal ((LONG)0,(LONG)0,(BYTE*)LS(IDS_BAD_RIGHTS2),(LONG)VINTENSE,(PCB*)infoPCB); break;
		default:	 
			NWSDisplayTextInPortal ((LONG)0,(LONG)0,(BYTE*)LS(IDS_REENTER_USERPASS),(LONG)VINTENSE,(PCB*)infoPCB); break;
		}

	NWSAlert (3,59,InstNut,MSGNUM(IDS_INVALID_USER_PASS));

	MyDestroyPortal (infoPort,InstNut);
}


