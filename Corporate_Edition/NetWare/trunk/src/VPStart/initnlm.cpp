// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#include "pscan.h"
#include "finetime.h"
#include "nlmui.h"
#include "status.h"
#include "password.h"
#include "licensehelper.h"
#include "nlm/nit/nwbindry.h"
#include "nlm/nit/nwdir.h"
#include "nlm/nwstring.h"
#include "nlm/nwfileio.h"
#include "SymSaferRegistry.h"
#include "SymSaferStrings.h"

extern "C" void PrintStuff(char *heading,char *string);  //gdf CW conversion
#include "slash.h"
void DisableAutoexec(void);  //gdf CW conversion
void DisableSystemLoginScript();   //gdf CW conversion
void DisableProfileLoginScript();   //gdf CW conversion
void DisableAMSDelete();    //gdf CW conversion

#define INVALID_FILE_HANDLE_VALUE (-1)

#ifndef FILE_ATTRIBUTE_DIRECTORY
#define FILE_ATTRIBUTE_DIRECTORY    _A_SUBDIR
#endif // FILE_ATTRIBUTE_DIRECTORY

#ifndef IsDir
#define IsDir(x) ((x) & FILE_ATTRIBUTE_DIRECTORY) // Do I care about _A_SYSTEM
#endif // IsDir

#define TEST_FILE  "sys:\\~~XYX!!0.!~5"

#define ABORT_KEY "~"
extern "C" FILE *TraceFile = NULL;  // installation trace log file  gdf sts 337273 03/09/01
typedef struct _PathNode {
	struct _PathNode *next;

	BOOL isDir;
	char path[IMAX_PATH];
} PathNode;

extern "C" PathNode *DelCurr=NULL;
extern "C" PathNode *DelHead=NULL;

#define INSTALL_LOG "install.log"

extern "C" DWORD LastError = 0;

extern "C" BOOL AuthCancelled=FALSE;
extern "C" BOOL BadDefAuth=FALSE;
extern "C" BOOL UsingDS=FALSE;

extern "C" LONG cfPort=0;
extern "C" PCB *cfPCB=NULL;

extern "C" char WWWRoot[IMAX_PATH]     = "";
extern "C" char LDVPWWWRoot[IMAX_PATH] = "";

extern "C" int Install=1;
extern "C" char Installing=FALSE;

extern "C" char AuthUser[256] = "";
extern "C" char AuthPass[256] = "";
extern "C" char Container[128]="";
extern "C" char ServerDN[MAX_PATH] = ""; // the distinguished name of the server

extern "C" char *AutoBuff=NULL;
extern "C" char *VPBuff=NULL;
extern "C" char *CurrentDefFile=NULL;

extern "C" char *fpos=NULL;
extern "C" char *fbuf=NULL;
extern "C" int fbytes=0;
extern "C" int fsize=0;

extern "C" int ConsoleHandle = 0;
extern "C" CBA_Addr CopyAddress = {0};

extern "C" char CopyStatusValue[64] = "";
extern "C" char CopyErrorValue[64]  = "";
extern "C" char CopyCmdValue[64]    = "";

extern "C" NWDSContextHandle hContext=-1;
extern "C" Buf_T NWFAR *inBuf=NULL;
extern "C" Buf_T NWFAR *outBuf=NULL;
extern "C" char DSNew = 1;
extern "C" int DSret=0;
extern "C" int LineNum=0;

extern "C" BOOL dsInitialized=FALSE;

extern "C" int Authenticated=0;
extern "C" int ConnectionNum=0;

void NewSection(char* section);

extern "C" FILE *rfile=NULL; // install log file

DWORD UpdateUpgradeStatus(DWORD status);
DWORD UpdateUpgradeError(DWORD error, char* ErrMsg);
DWORD CheckCopyServerCommand();

extern "C" DWORD UpdateFiles=0;

extern "C" DWORD ErrorCount;
extern "C" FILE_SERV_INFO g_fsinf;
extern "C" char RegMode;
extern "C" char *UpdateInf[];
extern "C" char HomeDir[IMAX_PATH];
extern "C" BOOL UpdateProg;

typedef NWDSCCODE (*t_NWDSAuthenticate)(NWCONN_HANDLE,NWDS_FLAGS,NWDS_SESSION_KEY*);
typedef NWDSCCODE (*t_NWDSAllocBuf)(int,NWDS_BUFFER **);
typedef NWDSCCODE (*t_NWDSCreateContextHandle)(NWDSContextHandle*);
typedef NWDSCCODE (*t_NWDSGetContext)(NWDSContextHandle,int,void*);
typedef NWDSCCODE (*t_NWDSSetContext)(NWDSContextHandle,int,void*);
typedef NWDSCCODE (*t_NWDSLogin)(NWDSContextHandle,NWDS_FLAGS,char*,char*,NWDS_VALIDITY);
typedef NWDSCCODE (*t_NWDSLogout)(NWDSContextHandle);
typedef NWDSCCODE (*t_NWDSFreeBuf)(NWDS_BUFFER *);
typedef NWDSCCODE (*t_NWDSFreeContext)(NWDSContextHandle);
typedef NWDSCCODE (*t_NWDSInitBuf)(NWDSContextHandle,NWDS_OPERATION,NWDS_BUFFER *);
typedef NWDSCCODE (*t_NWDSPutAttrName)(NWDSContextHandle,NWDS_BUFFER *,char *);
typedef NWDSCCODE (*t_NWDSPutAttrVal)(NWDSContextHandle,NWDS_BUFFER *,NWSYNTAX_ID,void *);
typedef NWDSCCODE (*t_NWDSRemoveObject)(NWDSContextHandle,char *);
typedef NWDSCCODE (*t_NWDSAddObject)(NWDSContextHandle,char *,NWDS_ITERATION,NWFLAGS,NWDS_BUFFER *);
typedef NWDSCCODE (*t_NWDSGenerateObjectKeyPair)(NWDSContextHandle,char *,char *,NWDS_FLAGS);
typedef NWDSCCODE (*t_NWDSPutChange)(NWDSContextHandle,NWDS_BUFFER *,NWCHANGE_TYPE,char *);
typedef NWDSCCODE (*t_NWDSModifyObject)(NWDSContextHandle,char *,NWDS_ITERATION*,NWFLAGS,NWDS_BUFFER *);
#ifdef NLM   // gdf CW conversion 
typedef NWDSCCODE (*t_NWDSMapNameToID)(NWDSContextHandle,NWCONN_HANDLE,char*,nuint32*);
#else
typedef NWDSCCODE (*t_NWDSMapNameToID)(NWDSContextHandle,NWCONN_HANDLE,char*,uint32*);
#endif   // gdf CW conversion 
typedef NWDSCCODE (*t_NWDSOpenStream)(NWDSContextHandle,char *,char *,NWDS_FLAGS,NWDS_LOGIN_FILE *);
typedef NWDSCCODE (*t_NWDSGetServerDN)(NWDSContextHandle,NWCONN_HANDLE,char*);

static t_NWDSOpenStream _NWDSOpenStream;
static t_NWDSAuthenticate _NWDSAuthenticate;
static t_NWDSMapNameToID _NWDSMapNameToID;
static t_NWDSAllocBuf _NWDSAllocBuf;
static t_NWDSCreateContextHandle _NWDSCreateContextHandle;
static t_NWDSGetContext _NWDSGetContext;
static t_NWDSSetContext _NWDSSetContext;
static t_NWDSLogout _NWDSLogout;
static t_NWDSLogin _NWDSLogin;
static t_NWDSFreeBuf _NWDSFreeBuf = NULL;
static t_NWDSFreeContext _NWDSFreeContext;
static t_NWDSInitBuf _NWDSInitBuf;
static t_NWDSPutAttrName _NWDSPutAttrName;
static t_NWDSPutAttrVal _NWDSPutAttrVal;
static t_NWDSRemoveObject _NWDSRemoveObject;
static t_NWDSAddObject _NWDSAddObject;
static t_NWDSGenerateObjectKeyPair _NWDSGenerateObjectKeyPair;
static t_NWDSPutChange _NWDSPutChange;
static t_NWDSModifyObject _NWDSModifyObject;
static t_NWDSGetServerDN _NWDSGetServerDN;

int DSPrelims (void);
DWORD UnloadOldNLMs(NUTInfo *nut);
DWORD ConfigureWebAdmin(char *SRMpath);
int DisplayErrorMessage (DWORD Err,int LineNum,char *oline,int state);

/************************************************************************************/
VOID eprintf(DWORD Err, char *format,...) {

	va_list marker;
	char line[1024];

	va_start(marker, format);
	
    DWORD dwFineTime = GetFineLinearTimeEx( ONEDAY_MAX_RES );

    DWORD dwSeconds = dwFineTime / ONEDAY_MAXRES_TICKS_PER_SECOND;
    DWORD dwMicros  = OneDayMaxResToMicroSeconds( dwFineTime % ONEDAY_MAXRES_TICKS_PER_SECOND );
    DWORD dwHours, dwMins, dwSecs;

    SecondsToHoursMinsSecs( dwSeconds, dwHours, dwMins, dwSecs );

    sssnprintf(line, sizeof(line), "%02d:%02d:%02d.%06d[_%ld %ld_]|", dwHours, dwMins, dwSecs, dwMicros, NTxGetCurrentThreadId( ), GetThreadGroupID()  );

	ssvsnprintfappend(line,sizeof(line),format,marker);
	UpdateUpgradeError(Err, line);

	va_end(marker);
}
/************************************************************************************/
VOID iprintf(char *format,...) {

	va_list marker;
	char line[1024];

	memset(line, 0, sizeof(line));
	va_start(marker, format);
	
    DWORD dwFineTime = GetFineLinearTimeEx( ONEDAY_MAX_RES );

    DWORD dwSeconds = dwFineTime / ONEDAY_MAXRES_TICKS_PER_SECOND;
    DWORD dwMicros  = OneDayMaxResToMicroSeconds( dwFineTime % ONEDAY_MAXRES_TICKS_PER_SECOND );
    DWORD dwHours, dwMins, dwSecs;

    SecondsToHoursMinsSecs( dwSeconds, dwHours, dwMins, dwSecs );

    sssnprintf(line, sizeof(line), "%02d:%02d:%02d.%06d[_%ld %ld_]|", dwHours, dwMins, dwSecs, dwMicros, NTxGetCurrentThreadId( ), GetThreadGroupID()  );

	ssvsnprintfappend(line,sizeof(line),format,marker);
	PrintToInstallPortal(line);

	if (rfile) fprintf(rfile,"%s",line);

	if(TraceFile)											// gdf sts 337273 03/09/01
	{   
		strcat(line, "\n");  
		fprintf(TraceFile,"%s",line); fflush(TraceFile);
	}  
	dprintf ("%s",line);
	va_end(marker);
}
/*******************************************************************************************/
DWORD CreateBinderyUser(char *name,char *pass,char super) {

	DSret=DeleteBinderyObject(name, OT_USER);

	if (!Install)
		return ERROR_SUCCESS;

	DSret=CreateBinderyObject(name,OT_USER,BF_STATIC, BS_ANY_READ | BS_SUPER_WRITE );
	switch (DSret) {
		case ERR_OBJECT_ALREADY_EXISTS:	case ESUCCESS:break;		
		default: return DSERR(DSret);
	}
	DSret=CreateProperty( name, OT_USER, "GROUPS_I'M_IN", BF_STATIC | BF_SET,BS_ANY_READ | BS_ANY_WRITE );
	switch (DSret) {
		case ERR_OBJECT_ALREADY_EXISTS:	case ESUCCESS:break;		
		default: return DSERR(DSret);
	}
	DSret=CreateProperty( name, OT_USER, "SECURITY_EQUALS", BF_STATIC | BF_SET,BS_ANY_READ | BS_ANY_WRITE );
	switch (DSret) {
		case ERR_OBJECT_ALREADY_EXISTS:	case ESUCCESS:break;		
		default: return DSERR(DSret);
	}
	DSret=ChangeBinderyObjectPassword( name, OT_USER, "", pass );
	switch (DSret) {
		case ERR_OBJECT_ALREADY_EXISTS:	case ESUCCESS:break;		
		default: return DSERR(DSret);
	}

	if (super) {
		DSret=AddBinderyObjectToSet(name,OT_USER,"SECURITY_EQUALS", "SUPERVISOR", OT_USER);
		switch (DSret) {
			case ERR_OBJECT_ALREADY_EXISTS:	case ESUCCESS:break;		
			default: return DSERR(DSret);
		}
	}
	return ERROR_SUCCESS;
}
/*******************************************************************************************/
DWORD CreateBinderyGroup(char *name,char *members) {

	char *q=NULL;

//	DSret=DeleteBinderyObject(name, OT_USER_GROUP);

	if (!Install)
    {
    	DSret=DeleteBinderyObject(name, OT_USER_GROUP);
		return ERROR_SUCCESS;
    }

	DSret=CreateBinderyObject(name,OT_USER_GROUP,BF_STATIC, BS_ANY_READ | BS_SUPER_WRITE );
	switch (DSret) {
		case ERR_OBJECT_ALREADY_EXISTS:	case ESUCCESS:break;		
		default: return DSERR(DSret);
	}
	DSret=CreateProperty( name, OT_USER_GROUP, "GROUP_MEMBERS", BF_STATIC | BF_SET,BS_ANY_READ | BS_ANY_WRITE );
	switch (DSret) {
		case ERR_OBJECT_ALREADY_EXISTS:	case ESUCCESS:break;		
        case ERR_PROPERTY_ALREADY_EXISTS: break;
        default: return DSERR(DSret);
	}

	if (members && *members) {
		q = strtok(members,",");
		while(q) {
			DSret=AddBinderyObjectToSet(q,OT_USER,"SECURITY_EQUALS", name, OT_USER_GROUP);
			switch (DSret) {
				case ERR_OBJECT_ALREADY_EXISTS:	case ESUCCESS:break;		
                case ERR_MEMBER_ALREADY_EXISTS: break;
				default: return DSERR(DSret);
			}
			DSret=AddBinderyObjectToSet(q,OT_USER,"GROUPS_I'M_IN", name, OT_USER_GROUP);
			switch (DSret) {
				case ERR_OBJECT_ALREADY_EXISTS:	case ESUCCESS:break;		
                case ERR_MEMBER_ALREADY_EXISTS: break;
				default: return DSERR(DSret);
			}
			DSret=AddBinderyObjectToSet(name,OT_USER_GROUP,"GROUP_MEMBERS", q, OT_USER);
			switch (DSret) {
				case ERR_OBJECT_ALREADY_EXISTS:	case ESUCCESS:break;		
                case ERR_MEMBER_ALREADY_EXISTS: break;
				default: return DSERR(DSret);
			}
			q = strtok(NULL,",");
			}
		}
	return ERROR_SUCCESS;
}
/*******************************************************************************************/
DWORD AddBinderyGroupTrustee(char *name,char *dir,WORD prims) {

	long ID=0;

	if (dir == NULL || *dir == 0)
		return EI_NO_TRUSTEE_DIR;

	DSret=GetBinderyObjectID(name,OT_USER_GROUP,&ID);
	if (DSret == ESUCCESS && ID) {
		if (Install)
			DSret=AddTrustee(dir,ID,prims);
		else
			DSret=DeleteTrustee(dir,ID);
		}

	return DSERR(DSret);
}
/*******************************************************************************************/
WORD ConvertPrims(char *q) {

	WORD prims = 0;

	while (*q) {
		switch(*q) {
			case 'R' : prims |= 1<<0; break;
			case 'W' : prims |= 1<<1; break;
			case 'C' : prims |= 1<<3; break;
			case 'F' : prims |= 1<<6; break;
			case 'M' : prims |= 1<<7; break;
			case 'D' : prims |= 1<<4; break;
			case 'S' : prims |= 1<<8; break;
			case 'A' : prims |= 1<<5; break;
			}
		q=NextChar(q);
		}
	return prims; // Added by Evan
}
/*******************************************************************************************/
DWORD CreateDSUser(char *name,char *pass,char super) {

	char cname[256];

	REF(super);

	memset (cname,0,sizeof(cname));

	if (!dsInitialized) return EI_DS_NO_INIT;
	if (NumBytes (Container) < 1) {
		LogInstallMessage("Trying to Create DS User without a valid container\n");
		return EI_NO_CONTAINER;
	}

	sssnprintf (cname,sizeof(cname),"CN=%s.%s",name,Container);

	DSret=_NWDSInitBuf(hContext, DSV_ADD_ENTRY, inBuf);
	DSret=_NWDSPutAttrName(hContext, inBuf, A_OBJECT_CLASS);
	DSret=_NWDSPutAttrVal(hContext, inBuf, SYN_CLASS_NAME, const_cast<char*>(C_USER));
	DSret=_NWDSPutAttrName(hContext, inBuf, A_SURNAME);
	DSret=_NWDSPutAttrVal(hContext, inBuf, SYN_CI_STRING, const_cast<char*>("Symantec AntiVirus User"));

	//delete the user so we are guaranteed to be coming into a clean state
	DSret=_NWDSRemoveObject(hContext, cname);

	if (!Install)
		return DSERR(DSret);

	//create the actual user
	DSret=_NWDSAddObject(hContext, cname, NULL, 0, inBuf);
	DSret=_NWDSGenerateObjectKeyPair(hContext, cname, pass, NULL);

	return ERROR_SUCCESS;
}
/*******************************************************************************************/

DWORD CreateDSGroup(char *name,char *members) {

	char cname[256],*member=NULL;
	char cname2[256];
	NWDS_ITERATION iterationHandle;

	// ---------------------------------------
	// ksr - NetWare Certification, 10/21/2002
	// Procedure Coverage
	/*	        
	int i;
	LONG *seed, rdelay, someaddress;
	*/
	// ---------------------------------------

	memset (cname,0,sizeof(cname));
	memset (cname2,0,sizeof(cname2));

	if (NumBytes (Container) < 1) {
		LogInstallMessage("Trying to create a DS group without a valid container\n");

		return EI_NO_CONTAINER;
	}
	sssnprintf (cname,sizeof(cname),"CN=%s.%s",name,Container);
	
	if (!dsInitialized) return EI_DS_NO_INIT;

	DSret=_NWDSInitBuf(hContext, DSV_ADD_ENTRY, inBuf);
	DSret=_NWDSPutAttrName(hContext, inBuf, A_OBJECT_CLASS);
	DSret=_NWDSPutAttrVal(hContext, inBuf, SYN_CLASS_NAME, const_cast<char*>(C_GROUP));

	if (!Install) {
		DSret=_NWDSRemoveObject(hContext, cname);
		return DSERR(DSret);
	}

//  gdf 04/10/00 the call to _NWDSAddObject is producing bogus NDS groups such as
//  0_1 and 0_2 in addition to the valid groups SymantecAntiVirusAdmin
//  and SymantecAntiVirusUser when the console command
//	load sys:\nav\vpstart /install is entered simultaneously from two
//  servers in the same NDS tree.

//  debugging observations:
//  1.  _NWDSAddObject is called exactly twice on each server once for each
//  valid group and the arguments are correct.
//  2.  bogus groups are not seen if commands ar not entered
//  simultaneously.
//  3. commenting out _NWDSAddObject prevents the creation of valid and
//  bogus groups in the NDS tree.
//  4. symbol import of NWDSAddObject and its equivalence to
//  _NWDSAddObject is correct.

	// ---------------------------------------
	// ksr - NetWare Certification, 10/21/2002
	// Procedure Coverage
	/*	        
	seed = &someaddress;   // gdf 04/11/00
	srand((LONG)seed);     // gdf 04/11/00
	rdelay = 0;            // gdf 04/11/00
	
    while(rdelay <= 1000 || rdelay >= 6000)    // gdf 04/11/00
		rdelay = rand();   // gdf 04/11/00

	rdelay = rdelay*10;   // gdf 04/11/00

	for(i=0; i<rdelay; i=i+100)  // gdf 04/11/00
	{
		ThreadSwitchWithDelay();  // gdf 04/11/00
		NTxSleep(100);            // gdf 04/11/0 0
	}
	*/
	
	//NTxSleep( 1000 );  
	
	// ---------------------------------------
	// ksr - NetWare Certification, 10/31/2002
	// Time slice 
	ThreadSwitchWithDelay();		
	
	// ---------------------------------------
 	
	DSret=_NWDSAddObject(hContext, cname, NULL, 0, inBuf); // -606 = ERR_ENTRY_ALREADY_EXISTS

	if (members && *members) {
		member = strtok(members,",");
		while (member) {
			sssnprintf(cname2,sizeof(cname2),"CN=%s.%s",member,Container);
	// cname2 == user  cname == group

	// add group to users [group membership] and [security =]
			DSret=_NWDSInitBuf(hContext, DSV_MODIFY_ENTRY, inBuf);
			DSret=_NWDSPutChange(hContext, inBuf, DS_ADD_VALUE,A_SECURITY_EQUALS);
			DSret=_NWDSPutAttrVal(hContext, inBuf, SYN_DIST_NAME, cname);
			DSret=_NWDSPutChange(hContext, inBuf, DS_ADD_VALUE,A_GROUP_MEMBERSHIP);
			DSret=_NWDSPutAttrVal(hContext, inBuf, SYN_DIST_NAME, cname);
			DSret=_NWDSModifyObject(hContext, cname2, &iterationHandle, 0, inBuf);

	// add user to groups [member]
			DSret=_NWDSInitBuf(hContext, DSV_MODIFY_ENTRY, inBuf);
			DSret=_NWDSPutChange(hContext, inBuf, DS_ADD_VALUE,A_MEMBER);
			DSret=_NWDSPutAttrVal(hContext, inBuf, SYN_DIST_NAME, cname2);
			DSret=_NWDSModifyObject(hContext, cname, &iterationHandle, 0, inBuf);

			member = strtok(NULL,",");

			// ---------------------------------------
			// ksr - NetWare Certification, 10/21/2002
			ThreadSwitchWithDelay();  
			
			// ---------------------------------------
	
			}
		}

	return ERROR_SUCCESS;
}
/*******************************************************************************************/
DWORD AddDSGroupTrustee(char *name,char *dir,WORD prims) {

	char cname[256];
	NWOBJ_ID ID;

	if (dir == NULL || *dir == 0)
		return EI_NO_TRUSTEE_DIR;

	if (!dsInitialized) return EI_DS_NO_INIT;

	memset (cname,0,sizeof(cname));

	if (NumBytes (Container) < 1) {
		LogInstallMessage("Trying to Add DS group trustee without a valid container\n");

		return EI_NO_CONTAINER;
	}


	sssnprintf (cname,sizeof(cname),"CN=%s.%s",name,Container);

dprintf( "AddDSGroupTrustee(%s,%s,0x%x)\n\t%s\n", name, dir, prims, cname );

	DSret=_NWDSMapNameToID( hContext, GetConnectionNumber(), cname, &ID );

dprintf( "\t_NWDSMapNameToID returned 0x%02x ID=0x%08x\n", DSret, ID );

	if (ID) {
		if (Install)
			DSret=AddTrustee(dir,LongSwap(ID),prims);
		else
			DSret=DeleteTrustee(dir,LongSwap(ID));
		}
dprintf( "\t%sTrustee returned 0x%08x\n", (Install?"Add":"Delete"), DSret );

	return DSERR(DSret);
}
/******************************************************************************************/
char *ExpandContainerName(char *cont){ // returns a cannonical name of the form OU=subunit.OU=subunit.O=baseunit

	char Contain[130],*o,*ou,*q,*p;
	char Expanded[130];

	o=ou=q=p=NULL;
	memset (Expanded,0,sizeof(Expanded));
	memset (Contain,0,sizeof(Contain));

	strcpy (Contain,cont);

	for (p=Contain ; (q=strchr (p,'.')) ; ) {
		*q=0;
		q++;
		if ((toupper(*p)=='O') && (toupper(*(p+1))=='U') && (*(p+2)=='='))
			sssnprintf (Expanded,sizeof(Expanded),"%s%s.",Expanded,p);
		else
			sssnprintf (Expanded,sizeof(Expanded),"%sOU=%s.",Expanded,p);
		p=q;
	}
	if (!((*p=='o' || *p=='O') && (*(p+1)=='=')))
		strcat (Expanded,"O=");
	strcat (Expanded,p);

	strcpy (cont,Expanded);
	return cont;
}
/******************************************************************************************/
int GetStreamHandle(char *ou) {

	short file=-1;
	int ccode=0;

//	NWDS_ITERATION iterationHandle;
// -601(fffffda7)=ERR_NO_SUCH_ENTRY   -602=ERR_NO_SUCH_VALUE   -603=ERR_NO_SUCH_ATTRIBUTE
	if (!dsInitialized) return EI_DS_NO_INIT;
	ccode=_NWDSOpenStream(hContext,ou,A_LOGIN_SCRIPT,3,&file);
	if (ccode==ERR_NO_SUCH_ENTRY) {		// OpenStream needs the canonical name or it returns NO_SUCH_ENTRY
		ExpandContainerName(ou);
		ccode=_NWDSOpenStream(hContext,ou,A_LOGIN_SCRIPT,3,&file);
	}

	if (ccode == ERR_NO_SUCH_ATTRIBUTE || ccode == ERR_NO_SUCH_VALUE || ccode == ERR_NO_SUCH_ENTRY) {
		if ((ccode=_NWDSInitBuf(hContext, DSV_MODIFY_ENTRY, inBuf))==ERROR_SUCCESS)
			if ((ccode=_NWDSPutChange(hContext, inBuf, DS_ADD_VALUE,A_LOGIN_SCRIPT))==ERROR_SUCCESS)
				if ((ccode=_NWDSPutAttrVal(hContext, inBuf, SYN_STREAM, const_cast<char*>("")))==ERROR_SUCCESS)
					if ((ccode=_NWDSModifyObject(hContext, ou, NULL, 0, inBuf))==ERROR_SUCCESS)
						ccode=_NWDSOpenStream(hContext,ou,A_LOGIN_SCRIPT,3,&file);
		if (ccode!=ERROR_SUCCESS) {
			LogInstallMessage("Could not get stream handle to context login script: %d 0x%x\n",ccode,ccode);
			file = -1;
		}
	}

	return file;
}
/*******************************************************************************************/
DWORD DumpSystemLogin(char *buf) {

	FILE *in=NULL,*out=NULL;
	char success=FALSE;
	char s[256];
	char state=START_TAG_SEARCH;
	char added = FALSE;
	int fh;

	DisablePreviousInstallation(DISABLE_SYS_LOG);

	memset (s,0,sizeof(s));
	MakeWriteable (BinderyLoginPath"net$log.dat",0xff70fff0);
	in = fopen(BinderyLoginPath"net$log.dat","rt");
	if (!in) {
		fh=open (BinderyLoginPath"net$log.dat",O_CREAT|O_RDONLY);
		if (fh!=-1) close (fh);
		in = fopen(BinderyLoginPath"net$log.dat","rt");
	}
	if (in) {
		out = fopen(BinderyLoginPath"net$log.n1w","wt");
		if (out) {
			if (Install) {
				while (fgets(s,sizeof(s),in)) { //look for the start tag
					ThreadSwitch();
					if (state==START_TAG_SEARCH && isStartTag(s)) { // we found it
						state=FOUND_START_TAG;
//						added = TRUE;
					}
					else if (state==FOUND_START_TAG && isEndTag(s)) {
						state=FOUND_END_TAG;
						break;
					}
				}
				fclose(in);
				in = fopen(BinderyLoginPath"net$log.dat","rt");

				if (state!=FOUND_END_TAG) { // if we didn't find it at all or didn't find the end tag then add it at the beginning of the file
					fputs(buf,out);
					if (state==FOUND_START_TAG) { // we didn't find the end tag
						state=FOUND_END_TAG; // with this at 2 it will simply add the rest of the original login script on to the new one
						iprintf (LS(IDS_ERROR_BIND_LOG_SCR));
						iprintf (LS(IDS_REMOVE_BY_HAND));
					}
				}
				else state=START_TAG_SEARCH;
				
				memset (s,0,sizeof(s));
			}
			while (fgets(s,sizeof(s),in)) {
				ThreadSwitch();
				if (state==START_TAG_SEARCH && isStartTag(s)) {
					state=FOUND_START_TAG;
					if (Install)
						fputs(buf,out);
					added = TRUE;
				}
				else if (state==FOUND_START_TAG && isEndTag(s))
					state=FOUND_END_TAG;
				else if (state==START_TAG_SEARCH || state==FOUND_END_TAG)
					fputs(s,out);
				memset (s,0,sizeof(s));
			}
			success = TRUE;
			fclose(out);
		}
		fclose(in);
	}
	if (success) {
		DeletePurgeFile(BinderyLoginPath"net$log.bak");
		rename(BinderyLoginPath"net$log.dat",BinderyLoginPath"net$log.bak");
		rename(BinderyLoginPath"net$log.n1w",BinderyLoginPath"net$log.dat");
	}
	else {
		LogInstallMessage("Error modifying bindery system login script\n");
		return EI_BINDERY_LOGIN;
	}
	return ERROR_SUCCESS;
}
/*******************************************************************************************/
DWORD ParseBinderyLogin(char *line,char *q) {

	if (AutoBuff == NULL) {
		AutoBuff = (char*)malloc(BUFSIZE);
		if (AutoBuff == NULL) {
			LogInstallMessage("Out of Memory Error trying to Parse Bindery Login\n");
			return EI_MEMORY;
		}
		memset (AutoBuff,0,BUFSIZE);
		StrCopy(AutoBuff,"\n"STARTTAG"\n"DO_NOT_MODIFY"\n");
		}

	if (line == NULL) {
		sssnprintf (AutoBuff, BUFSIZE, "%s%s\n", AutoBuff, ENDTAG);
//		strcat(AutoBuff,ENDTAG);
		DumpSystemLogin(AutoBuff);
		free(AutoBuff);
		AutoBuff = NULL;
		}
	else {
		strcat(AutoBuff,line);
		if (q) {
			strcat(AutoBuff,"=");
			strcat(AutoBuff,q);
		}
		strcat(AutoBuff,"\n");
		}
	return ERROR_SUCCESS;
}
/******************************************************************************************/
char *GetNextLine(char *q,char *out) {

	ThreadSwitch();

	if (q==NULL || out==NULL) return NULL;
	if (*q==0) return NULL;

	while (*q != '\n' && *q != 0) {
//		*out++=*q++;
		*out=*q;
		out=NextChar(out);	// MB PROBLEM
		q=NextChar(q);	  //MLR (FIXED?)
		*(out-1)=*(q-1);	// this is in case it was a multi byte character so we also copy the second byte
	}
//	*out++=*q++;
	*out=*q;
	out=NextChar(out);
	q=NextChar(q);
	*(out-1)=*(q-1);	// this is in case it was a multi byte character so we also copy the second byte
	*out=0;

	return q;
}
/******************************************************************************************/
DWORD RestoreOldLoginScript(char *buff) {

	int out;
	char buf[256];

	memset (buf,0,sizeof (buf));
	out = GetStreamHandle(Container);	// Open it again so we can write over it
	if (out!=INVALID_FILE_HANDLE_VALUE) {
//		write (out,buff,NumBytes(buff)+1);
		write (out,buf,sizeof (buf));
		close (out);
	}
	
	return ERROR_SUCCESS;
}
/******************************************************************************************/
DWORD SaveLDVPSectionToFile(char *buf) {

	FILE *handle;
	char LoginScript[IMAX_PATH];
	sssnprintf (LoginScript,sizeof(LoginScript),"%s" sSLASH "VP5Login.dat",ProgramDir);
	handle = fopen (LoginScript,"wt");
	if (handle) {
		fwrite (buf,1,NumBytes(buf),handle);
		fprintf (handle,"\n\0\0\0");
		fclose (handle);
	}
	return ERROR_SUCCESS;	
}
/******************************************************************************************/
DWORD DumpProfileLogin(char *buf) {

	// get stream to ou login and add buf
	char *buff=NULL;
	int in=-1,out=-1,ccode=-1;
	char s[256];
	char state=START_TAG_SEARCH;
	char added = FALSE;
	char *q=NULL;
	int len;
	BOOL tagsFound=FALSE;
	DWORD sLen=0;

	if (NumBytes (Container) < 1) {
		LogInstallMessage("Trying to dump profile login with invalid container\n");
		if (Install)
			SaveLDVPSectionToFile (buf);
		return EI_NO_CONTAINER;
	}

	in = GetStreamHandle(Container);
	if (in==-1 && NumBytes(Container)<1) {
		UseDefaultContext();
		in = GetStreamHandle(Container);
	}
	if (in!=-1) {
		HKEY hkey;

		if (RegOpenKey(HKEY_LOCAL_MACHINE,REGHEADER,&hkey) == ERROR_SUCCESS) {
			PutStr(hkey,"Container",Container);
			RegCloseKey(hkey);
		}

		len=filelength(in)+5;
		buff = (char*)malloc(len);
		if (!buff) {
			close(in);
			if (Install)
				SaveLDVPSectionToFile (buf);
			LogInstallMessage("Out of memory trying to dump profile login\n");
			return EI_MEMORY;
		}
		if (buff) {
			memset(buff,0,len);
			q = buff;
			ccode=read(in,buff,len);	// read in the entire script to a buffer
			close(in);					// close it
			in = 0;

			if (ccode) { // create backup file for profile login script
				char ProfBackup [MAX_PATH];
				FILE *backup;
				sssnprintf (ProfBackup,sizeof(ProfBackup),"%s" sSLASH "LoginScr.bak",ProgramDir);
				backup=fopen (ProfBackup,"wt");
				if (backup){
					fwrite (buff,1,ccode,backup);
					fclose (backup);
				}
			}
			out = GetStreamHandle(Container);	// Open it again so we can write over it

			if (out!=-1) {
				memset(s,0,sizeof(s));
				q = buff;
				while ( q=GetNextLine(q,s)) {	// get a line from the buffer
					if (isStartTag(s)) {		// look for the start tag
						state=FOUND_START_TAG;				// if we find the start tag set state to 0
					}
					else if (state==FOUND_START_TAG && isEndTag(s)) {		// if we have found the start tag and we then find the end tag
						state=FOUND_END_TAG;							// set the state to 2
						break;
					}
				}
				if (state!=FOUND_END_TAG) { // if we didn't find it at all or didn't find the end tag then add it at the beginning of the file
					ccode=write(out,buf,NumBytes(buf));
					state=FOUND_END_TAG; // with this at 2 it will simply add the rest of the original login script on to the new one
					if (state==FOUND_START_TAG) { // we didn't find the end tag
						iprintf (LS(IDS_ERROR_PROF_LOG_SCR)); // print out an error message
						iprintf (LS(IDS_REMOVE_BY_HAND));
					}
				}
				else {
					state=START_TAG_SEARCH; // we did find it so reset state to 0;
					if(RemoveInProgress)
						tagsFound=TRUE;
					}

				q = buff;								// start at the beginning of the script
				while (q=GetNextLine(q,s)) {			// read in a line from the buffer
					if (state==START_TAG_SEARCH && isStartTag(s)) {	// look for the start tag if state is zero
						state=FOUND_START_TAG;						// we found it so set state to 1
						if (Install) 
							ccode=write(out,buf,NumBytes(buf));	// write out the LDVP section
						added = TRUE;
					}
					else if (state==FOUND_START_TAG && isEndTag(s))		// if we have found the start tag then look for the end tag
						state=FOUND_END_TAG;							// and the set state to 2
					else if (state==START_TAG_SEARCH || state==FOUND_END_TAG) { 	// write out the rest of the login script
						sLen=NumBytes(s);
						if(tagsFound) {
							DWORD lseekReturn;
							tagsFound=FALSE;
							if( chsize(out,sLen) != 0)
								iprintf (LS(IDS_ERROR_PROF_LOG_SCR)); // print out an error message
							lseekReturn=lseek(out,0,SEEK_SET);
							ccode=write(out,s,sLen);
						}
						else
							ccode=write(out,s,sLen);
						if (ccode != NumBytes(s)) {  // Error while writing so restore to backup
							close (out);
							out=-1;
							RestoreOldLoginScript(buff);
							if (Install)
								SaveLDVPSectionToFile (buf);
							break;
						}
					}
				}
				if (out!=-1) {
					if(RemoveInProgress&&(tagsFound)) {
						DWORD lseekReturn;
						if( chsize(out,0) != 0)
							iprintf (LS(IDS_ERROR_PROF_LOG_SCR)); // print out an error message
						lseekReturn=lseek(out,0,SEEK_SET);
						ccode=write(out,s,0);
						
					}
					memset(s,0,sizeof(s));
					write(out,s,0); // this truncates the rest of the file

					close(out);
				}
			}
			free(buff);
			buff=NULL;
		}
		if (in)
			close(in);
		DisablePreviousInstallation(DISABLE_PROF_LOG);
	}
	else {
		iprintf (LS(IDS_ERROR_OPEN_PROF_LOG));
		if (Install)
			SaveLDVPSectionToFile (buf);
		LogInstallMessage("Error opening profile login script\n");
		return EI_PROFILE_LOGIN;
	}

	return ERROR_SUCCESS;
}
/****************************************************************************************/
DWORD DumpAutoexecNcf(char *buf) {

	FILE *in=NULL,*out=NULL;
	char success=FALSE;
	char s[256];
	char state=START_TAG_SEARCH;
	char added = FALSE;
	char AutoExec[256],AutoBack[256],AutoTemp[256],Auto[20];
//	char prev=0;
	FILE_SERV_INFO fsInfo;

	DisablePreviousInstallation(DISABLE_AUTO);

	memset (&fsInfo,0,sizeof (FILE_SERV_INFO));
	GetServerInformation(65,&fsInfo);

	dprintf ("SFTLevel is %d\n",fsInfo.SFTLevel);
	if (fsInfo.SFTLevel==3)
		StrCopy (Auto,"msauto");
	else
		StrCopy (Auto,"autoexec");

	sssnprintf (AutoExec,sizeof(AutoExec),SystemPath"%s.ncf",Auto);
	sssnprintf (AutoBack,sizeof(AutoBack),SystemPath"%s.bak",Auto);
	sssnprintf (AutoTemp,sizeof(AutoTemp),SystemPath"%s.n1w",Auto);

	in = fopen(AutoExec,"rt");
	if (in) {

		out = fopen(AutoTemp,"wt");
		if (out) {
			memset (s,0,sizeof(s));
			while (fgets(s,sizeof(s),in)) {
				ThreadSwitch();
				if (state==START_TAG_SEARCH && isStartTag(s)) {	// look for the start tag
					state=FOUND_START_TAG; 						// and change the state
					if (Install)
						fputs(buf,out); 			// add the new section
					added = TRUE;
				}
				else if (state==FOUND_START_TAG && isEndTag(s))	// look for the end tag
					state=2;						// and change the state when we find it
				else if (state==START_TAG_SEARCH || state==2) {
					fputs(s,out);					// write out the line
				}
				memset (s,0,sizeof(s));
			}

			if (!added && Install) { // put the section on the end of the autoexec.ncf file
				fputs ("\n\n",out);
				fputs(buf,out);
			}
			success = TRUE;
			fclose(out);
		}
		else
			LogInstallMessage("Error opening temporary autoexec.ncf file\n");
		fclose(in);
	}
	else {
		LogInstallMessage("Error opening autoexec.ncf\n");
		return EI_OPEN_AUTOEXEC;
	}

	if (state==FOUND_START_TAG) { // We never found the end tag! We may have dropped the last part of the autoexec.ncf file
		success=FALSE;
		if (Install) {
			in = fopen(AutoExec,"at+");	// open the original autoexec.ncf file
			if (in) {
				fputs ("\n\n",in);
				fputs (buf,in);							 // now stick the new ldvp section on end of the original file
				fclose (in);
				iprintf (LS(IDS_ERROR_AUTO_NCF));
				iprintf (LS(IDS_REMOVE_BY_HAND));
			}
			else {
				LogInstallMessage("Error adding LDVP section to autoexec.ncf\n");
				return EI_ADDING_AUTOEXEC;
			}
		}
	}
	if (success) {
		DeletePurgeFile(AutoBack);
		rename(AutoExec,AutoBack);
//		RS(1);
		rename(AutoTemp,AutoExec);
	}
	else {
		LogInstallMessage("Error modifying autoexec.ncf file\n");
		return EI_MODIFY_AUTOEXEC;
	}
	return ERROR_SUCCESS;
}
/******************************************************************************************/
DWORD ParseProfileLogin(char *line,char *q) {

	int ccode;

	if ((ccode = DSPrelims())!=ERROR_SUCCESS)
		return ccode; // if DSPrelims doesn't return ERROR_SUCCESS then we just specified the container but didn't authenticate yet

	ccode=0;

	if (AutoBuff == NULL) {
		AutoBuff = (char*)malloc(BUFSIZE);
		if (AutoBuff == NULL) {
			LogInstallMessage("Out of memory trying to parse profile login\n");
			return EI_MEMORY;
		}
		memset (AutoBuff,0,BUFSIZE);
		StrCopy(AutoBuff,STARTTAG"\n"DO_NOT_MODIFY"\n");
	}

	if (line == NULL) {
		sssnprintf (AutoBuff, BUFSIZE, "%s%s\n", AutoBuff, ENDTAG);
		ccode=DumpProfileLogin(AutoBuff);
		free(AutoBuff);
		AutoBuff = NULL;
	}
	else {
		strcat(AutoBuff,line);
		if (q) {
			strcat(AutoBuff,"=");
			strcat(AutoBuff,q);
		}
		strcat(AutoBuff,"\n");
	}
	if (ccode)
		LogInstallMessage("Error dumping profile login script\n");
	return ccode;
}
/****************************************************************************************/
DWORD ParseAutoexecNCF(char *line,char *q) {

	if (AutoBuff == NULL) {
		AutoBuff = (char*)malloc(BUFSIZE);
		if (AutoBuff == NULL) {
			LogInstallMessage("out of memory trying to parse autoexec.ncf file\n");
			return EI_MEMORY;
		}
		memset (AutoBuff,0,BUFSIZE);
		sssnprintf(AutoBuff,BUFSIZE,STARTTAG"\n"DO_NOT_MODIFY"\n");
	}
	if (line == NULL) {
		strcat (AutoBuff,ENDTAG"\n");
//		strcat(AutoBuff,"\n");
		DumpAutoexecNcf(AutoBuff);
		free(AutoBuff);
		AutoBuff = NULL;
	}
	else
/*Tom*/ sssnprintfappend (AutoBuff, BUFSIZE, "%s%s%s\n", line, q?"=":"", q?q:"");

	return ERROR_SUCCESS;
}
/****************************************************************************************/
DWORD DumpVPstartNcf(char *buf) {

	FILE *out=NULL;
	int cc;

	if (!Install) {
		cc=DeletePurgeFile(SystemPath"vpstart.ncf");
		if (cc)
			LogInstallMessage("Error unlinking vpstart.ncf\n");
		return cc ? DSERR(errno) : ERROR_SUCCESS;
	}

	out = fopen(SystemPath"vpstart.ncf","wt");

	if (out) {
		cc=fputs(buf,out);
		fclose(out);
		return cc ? DSERR(errno) : ERROR_SUCCESS;
	}
	LogInstallMessage("Error opening vpstart.ncf\n");
	return EI_OPEN_VPSTART;
}
/****************************************************************************************/
DWORD ParseVPstartNcf (char *line,char *q) {

	if (VPBuff == NULL) {
		VPBuff = (char*)malloc(BUFSIZE);
		if (VPBuff == NULL) {
			LogInstallMessage("Out of memory trying to parse vpstart.ncf\n");
			return EI_MEMORY;
		}
		memset (VPBuff,0,BUFSIZE);
	}
	if (line == NULL) {
		DumpVPstartNcf(VPBuff);
		free(VPBuff);
		VPBuff = NULL;
	}
	else {
		sssnprintf (VPBuff, BUFSIZE, "%s%s%s%s\n", VPBuff, line, q?"=":"", q?q:"");
//		strcat(VPBuff,line);
//		strcat(VPBuff,"\n");
	}
	return ERROR_SUCCESS;
}
/*******************************************************************************************/
/*void DisplayConsoleScreen (void) {
	SetCurrentScreen (ConsoleHandle);
	DisplayScreen (ConsoleHandle);
}
void DisplayInstallScreen (void) {
	SetCurrentScreen (InstScreenHan);
	DisplayScreen (InstScreenHan);
}*/
/*******************************************************************************************/
void ClearNWDSFuncs(void) {

	_NWDSOpenStream=(t_NWDSOpenStream)NULL;
	_NWDSAuthenticate=(t_NWDSAuthenticate)NULL;
	_NWDSMapNameToID=(t_NWDSMapNameToID)NULL;
	_NWDSAllocBuf=(t_NWDSAllocBuf)NULL;
	_NWDSCreateContextHandle=(t_NWDSCreateContextHandle)NULL;
	_NWDSGetContext=(t_NWDSGetContext)NULL;
	_NWDSSetContext=(t_NWDSSetContext)NULL;
	_NWDSLogout=(t_NWDSLogout)NULL;
	_NWDSLogin=(t_NWDSLogin)NULL;
	_NWDSFreeBuf=(t_NWDSFreeBuf)NULL;
	_NWDSFreeContext=(t_NWDSFreeContext)NULL;
	_NWDSInitBuf=(t_NWDSInitBuf)NULL;
	_NWDSPutAttrName=(t_NWDSPutAttrName)NULL;
	_NWDSPutAttrVal=(t_NWDSPutAttrVal)NULL;
	_NWDSRemoveObject=(t_NWDSRemoveObject)NULL;
	_NWDSAddObject=(t_NWDSAddObject)NULL;
	_NWDSGenerateObjectKeyPair=(t_NWDSGenerateObjectKeyPair)NULL;
	_NWDSPutChange=(t_NWDSPutChange)NULL;
	_NWDSModifyObject=(t_NWDSModifyObject)NULL;
	_NWDSGetServerDN=(t_NWDSGetServerDN)NULL;

	UnimportSymbol(GetNLMHandle(),"NWDSAuthenticate");
	UnimportSymbol(GetNLMHandle(),"NWDSMapNameToID");
	UnimportSymbol(GetNLMHandle(),"NWDSAllocBuf");
	UnimportSymbol(GetNLMHandle(),"NWDSCreateContextHandle");
	UnimportSymbol(GetNLMHandle(),"NWDSGetContext");
	UnimportSymbol(GetNLMHandle(),"NWDSSetContext");
	UnimportSymbol(GetNLMHandle(),"NWDSLogout");
	UnimportSymbol(GetNLMHandle(),"NWDSLogin");
	UnimportSymbol(GetNLMHandle(),"NWDSFreeBuf");
	UnimportSymbol(GetNLMHandle(),"NWDSFreeBuf");
	UnimportSymbol(GetNLMHandle(),"NWDSFreeContext");
	UnimportSymbol(GetNLMHandle(),"NWDSInitBufp");
	UnimportSymbol(GetNLMHandle(),"NWDSPutAttrName");
	UnimportSymbol(GetNLMHandle(),"NWDSPutAttrVal");
	UnimportSymbol(GetNLMHandle(),"NWDSRemoveObject");
	UnimportSymbol(GetNLMHandle(),"NWDSAddObject");
	UnimportSymbol(GetNLMHandle(),"NWDSGenerateObjectKeyPair");
	UnimportSymbol(GetNLMHandle(),"NWDSPutChange");
	UnimportSymbol(GetNLMHandle(),"NWDSModifyObject");
	UnimportSymbol(GetNLMHandle(),"NWDSOpenStream");
	UnimportSymbol(GetNLMHandle(),"NWDSGetServerDN");

}
/*******************************************************************************************/
//#define DSAPI_DATE 0x3404C648 // time_t date of the correct DSAPI.NLM  earlier versions don't
								// have all the correct functions

//DWORD CheckDSVersion(HANDLE han) {

//	struct stat st;

//  stat (NW_SYSTEM_DIR sSLASH "DSAPI.NLM",&st);

//	return st.st_mtime<DSAPI_DATE ? EI_DSINIT : ERROR_SUCCESS;
//}	
/*******************************************************************************************/
WORD LoadNWDSFuncs(void) {

	HANDLE dsapiHan=0;

	dsapiHan = _LoadLibrary ("dsapi.nlm",true);
	if (!dsapiHan) 
		return EI_LOAD_DS_NLM;
	

	if ((_NWDSFreeBuf = 		(t_NWDSFreeBuf)			ImportSymbol(GetNLMHandle(),"NWDSFreeBuf")) == NULL)		{ LogInstallMessage("Error importing NWDSFreeBuf\n"); 		return 0x02|EI_DSINIT;}
	if ((_NWDSLogout = 			(t_NWDSLogout)			ImportSymbol(GetNLMHandle(),"NWDSLogout")) == NULL)			{ LogInstallMessage("Error importing NWDSLogout\n"); 		return 0x03|EI_DSINIT;}
	if ((_NWDSFreeContext = 	(t_NWDSFreeContext)		ImportSymbol(GetNLMHandle(),"NWDSFreeContext")) == NULL)	{ LogInstallMessage("Error importing NWDSFreeContext\n"); 	return 0x04|EI_DSINIT;}
	if ((_NWDSAuthenticate = 	(t_NWDSAuthenticate)	ImportSymbol(GetNLMHandle(),"NWDSAuthenticate")) == NULL)	{ LogInstallMessage("Error importing NWDSAuthenticate\n"); 	return 0x05|EI_DSINIT;}
	if ((_NWDSAllocBuf = 		(t_NWDSAllocBuf)		ImportSymbol(GetNLMHandle(),"NWDSAllocBuf")) == NULL)		{ LogInstallMessage("Error importing NWDSAllocBuf\n"); 		return 0x06|EI_DSINIT;}
	if ((_NWDSGetContext = 		(t_NWDSGetContext)		ImportSymbol(GetNLMHandle(),"NWDSGetContext")) == NULL)		{ LogInstallMessage("Error importing NWDSGetContext\n"); 	return 0x07|EI_DSINIT;}
	if ((_NWDSSetContext = 		(t_NWDSSetContext)		ImportSymbol(GetNLMHandle(),"NWDSSetContext")) == NULL)		{ LogInstallMessage("Error importing NWDSSetContext\n"); 	return 0x08|EI_DSINIT;}
	if ((_NWDSLogin = 			(t_NWDSLogin)			ImportSymbol(GetNLMHandle(),"NWDSLogin")) == NULL)			{ LogInstallMessage("Error importing NWDSLogin\n"); 		return 0x09|EI_DSINIT;}
	if ((_NWDSInitBuf = 		(t_NWDSInitBuf)			ImportSymbol(GetNLMHandle(),"NWDSInitBuf")) == NULL)		{ LogInstallMessage("Error importing NWDSInitBuf\n"); 		return 0x0a|EI_DSINIT;}
	if ((_NWDSPutAttrName = 	(t_NWDSPutAttrName)		ImportSymbol(GetNLMHandle(),"NWDSPutAttrName")) == NULL)	{ LogInstallMessage("Error importing NWDSPutAttrName\n"); 	return 0x0b|EI_DSINIT;}
	if ((_NWDSPutAttrVal = 		(t_NWDSPutAttrVal)		ImportSymbol(GetNLMHandle(),"NWDSPutAttrVal")) == NULL)		{ LogInstallMessage("Error importing NWDSPutAttrVal\n"); 	return 0x0c|EI_DSINIT;}
	if ((_NWDSRemoveObject = 	(t_NWDSRemoveObject)	ImportSymbol(GetNLMHandle(),"NWDSRemoveObject")) == NULL)	{ LogInstallMessage("Error importing NWDSRemoveObject\n"); 	return 0x0d|EI_DSINIT;}
	if ((_NWDSAddObject = 		(t_NWDSAddObject)		ImportSymbol(GetNLMHandle(),"NWDSAddObject")) == NULL)		{ LogInstallMessage("Error importing NWDSAddObject\n"); 	return 0x0e|EI_DSINIT;}
	if ((_NWDSPutChange =		(t_NWDSPutChange)		ImportSymbol(GetNLMHandle(),"NWDSPutChange")) == NULL)		{ LogInstallMessage("Error importing NWDSPutChange\n"); 	return 0x0f|EI_DSINIT;}
	if ((_NWDSModifyObject =	(t_NWDSModifyObject)	ImportSymbol(GetNLMHandle(),"NWDSModifyObject")) == NULL)	{ LogInstallMessage("Error importing NWDSModifyObject\n"); 	return 0x10|EI_DSINIT;}
	if ((_NWDSMapNameToID =		(t_NWDSMapNameToID)		ImportSymbol(GetNLMHandle(),"NWDSMapNameToID")) == NULL)	{ LogInstallMessage("Error importing NWDSMapNameToID\n"); 	return 0x11|EI_DSINIT;}
	if ((_NWDSOpenStream =		(t_NWDSOpenStream)		ImportSymbol(GetNLMHandle(),"NWDSOpenStream")) == NULL)		{ LogInstallMessage("Error importing NWDSOpenStream\n"); 	return 0x12|EI_DSINIT;}
	if ((_NWDSGetServerDN =		(t_NWDSGetServerDN)		ImportSymbol(GetNLMHandle(),"NWDSGetServerDN")) ==	NULL)	{ LogInstallMessage("Error importing NWDSGetServerDN\n"); 	return 0x13|EI_DSINIT;}
	if ((_NWDSCreateContextHandle = (t_NWDSCreateContextHandle)	ImportSymbol(GetNLMHandle(),"NWDSCreateContextHandle")) == NULL)	{ LogInstallMessage("Error importing NWDSCreateContextHandle\n");	return 0x14|EI_DSINIT;}
	if ((_NWDSGenerateObjectKeyPair = (t_NWDSGenerateObjectKeyPair)ImportSymbol(GetNLMHandle(),"NWDSGenerateObjectKeyPair")) == NULL)	{ LogInstallMessage("Error importing NWDSGetnerateObjectKeyPair\n");	return 0x15|EI_DSINIT;}

	return ERROR_SUCCESS;
}
/*******************************************************************************************/

DWORD InitDS(void) {

	DWORD dckFlags=0;
	DWORD ccode=-1;

	LONG infoPort=0;
	PCB *infoPCB=NULL;

	if (dsInitialized) return ERROR_SUCCESS;

	if (rfile) 	fprintf (rfile,"InitDS\n");
//	ccode = CheckDSVersion(dsapiHan);
//	if (ccode!=ERROR_SUCCESS)
//		return ccode;

	memset (ServerDN,0,MAX_PATH);

	MyCreatePortal (&infoPort,&infoPCB,4,20,16,39,128,80,SAVE,NULL,VINTENSE,SINGLE,VINTENSE,InstNut);

	NWSDrawPortalBorder (infoPCB);
	NWSClearPortal (infoPCB);
	NWSDisplayTextInPortal (0,0,reinterpret_cast<unsigned char*>(LS(IDS_INIT_DS)),VNORMAL,infoPCB);

//	DisplayConsoleScreen ();
//	DisplayInstallScreen();

	ConnectionNum=SetCurrentConnection(-1);
	ConnectionNum=GetConnectionNumber();

	ClearNWDSFuncs();

	ccode=LoadNWDSFuncs();
	if(ccode!=ERROR_SUCCESS) {
		NWSDestroyPortal (infoPort,InstNut);
		return ccode;
	}

	inBuf=outBuf=NULL;
	ccode=_NWDSAllocBuf(DEFAULT_MESSAGE_LEN, &outBuf);
	if(ccode!=ERROR_SUCCESS) {
		LogInstallMessage("Error allocating outBuf\n");
		NWSDestroyPortal (infoPort,InstNut);
		return EI_ALLOC_BUF;
	}
	ccode=_NWDSAllocBuf(DEFAULT_MESSAGE_LEN, &inBuf);
	if(ccode!=ERROR_SUCCESS) {
		LogInstallMessage("Error allocating inBuf\n");
		NWSDestroyPortal (infoPort,InstNut);
		return EI_ALLOC_BUF;
	}

	hContext=(NWDSContextHandle)ERR_CONTEXT_CREATION;
	ccode=_NWDSCreateContextHandle(&hContext);
	if(ccode!=ERROR_SUCCESS || hContext==(NWDSContextHandle)ERR_CONTEXT_CREATION) {
		LogInstallMessage("Error creating context\n");
		NWSDestroyPortal (infoPort,InstNut);
		return EI_CREATE_CONTEXT;
	}

//	Authenticated=0;
	DSret=_NWDSGetContext(hContext, DCK_FLAGS, &dckFlags);
	dckFlags &= ~DCV_TYPELESS_NAMES;
	dckFlags |= DCV_CANONICALIZE_NAMES;
	DSret=_NWDSSetContext(hContext, DCK_FLAGS, &dckFlags);
	DSret=_NWDSSetContext(hContext, DCK_NAME_CONTEXT, const_cast<char*>("[Root]"));

	_NWDSGetServerDN (hContext,ConnectionNum,ServerDN);
	dprintf ("Distinguished name of the server = \"%s\"\n",ServerDN);

	dsInitialized=TRUE;


	NWSDestroyPortal (infoPort,InstNut);

	return ccode;
}
/*******************************************************************************************/
DWORD DeInitDS(void) {

	int ccode=0;

	if (rfile) 	fprintf (rfile,"DeInitDS\n");
	ccode=ReturnConnection (ConnectionNum);
	// -344 -> INVALID_TDS ???
	if (_NWDSLogout && Authenticated )
		if (ccode=_NWDSLogout(hContext))
			iprintf (InitMessage(IDS_E_LOGOUT),ccode);

	if (_NWDSFreeBuf) {
		if (outBuf)
			ccode=_NWDSFreeBuf(outBuf);
		if (inBuf)
			ccode=_NWDSFreeBuf(inBuf);
		inBuf=outBuf=NULL;
	}
	if (_NWDSFreeContext) {
		if (hContext!=(NWDSContextHandle)ERR_CONTEXT_CREATION && hContext!=-1)
			ccode=_NWDSFreeContext(hContext);
		hContext=-1;
	}

	ClearNWDSFuncs();

	return ERROR_SUCCESS;
}
/*******************************************************************************************/
DWORD LoginToDirectory(char *username,char *pass) {

	char cname[256]; 
	char pass2[PASS_MAX_CIPHER_TEXT_BYTES + 7]; //this is the right size to prevent mangling the user name
	char tmp[PASS_MAX_PLAIN_TEXT_BYTES+1];
	char *p=NULL;
	int ccode=0;
	int x,in;

	memset (cname,0,sizeof(cname));
	memset (pass2,0,sizeof(pass2));

	StrCopy(cname, username);

	p = pass;
	if (!strncmp(p,/**/"!CRYPT!",7))
		p += 7;
	UnMakeEP(p, strlen(p)+1, cname, PROFILE_PASS_KEY, pass2, sizeof(pass2)); // password.cpp

	if (!dsInitialized) return EI_DS_NO_INIT;
	ccode=_NWDSLogin(hContext,0,cname,pass2,0);
	if (ccode) {
		LogInstallMessage("NWDSLogin1(%s) returned %X\n",cname,ccode);
		dprintf ("Failed login for %s, (Error code %x) ",cname,ccode);
		sssnprintf (cname,sizeof(cname),"%s.%s",username,Container);
		dprintf ("trying again with %s\n",cname);
		ccode=_NWDSLogin(hContext,0,cname,pass2,0);
	}
	if (ccode) {
		LogInstallMessage("NWDSLogin2(%s) returned %X\n",cname,ccode);
//		return ccode;
	}
	ccode=_NWDSAuthenticate(ConnectionNum,0,NULL);
	if (ccode) {
		LogInstallMessage("NWDSAuthenticate(%s) returned %x\n",cname,ccode);
//		return ccode;
	}

	x = open( TEST_FILE, O_WRONLY|O_TRUNC|O_BINARY|O_CREAT,S_IWRITE|S_IREAD);
	if (x == -1) {
		LogInstallMessage("Not allowed to write to local disk\n");
		_NWDSLogout(hContext);
		SetCurrentConnection(0);
		return EI_DISK_WRITE;
		}
	close(x);
	DeletePurgeFile( TEST_FILE );

	in = GetStreamHandle(Container);
	while (in==-1) {
		BadDefAuth=TRUE;
		if (!GetContextUI (Container)) {
			LogInstallMessage("Get container canceled\n");
			_NWDSLogout(hContext);
			SetCurrentConnection(0);
			return EI_GET_CONTEXT;
			}
		in = GetStreamHandle(Container);
		if (in == -1)
			FailedAuthentication(102);
		}

	close(in);

	Authenticated=1;

	ssStrnCpy( AuthUser, cname, sizeof(AuthUser) );
	ssStrnCpy( tmp, pass2, sizeof(tmp) );
	ssStrnCpy( pass2, "!CRYPT!", sizeof(pass2) );
	MakeEP(pass2+7, sizeof(pass2) - 7, cname,PROFILE_PASS_KEY, tmp, sizeof(tmp));
	ssStrnCpy( AuthPass, pass2, sizeof(AuthPass) );
	
	return ERROR_SUCCESS;
}
/*******************************************************************************************/
DWORD AuthenticateUser() {

	char user[256],pass[256];
	int ccode=1;

	if (Authenticated || AuthCancelled)
		return ERROR_SUCCESS;

	memset (user,0,sizeof(user));
	memset (pass,0,sizeof(pass));

	BadDefAuth=FALSE;

	if (AuthUser[0]) {
		ccode = LoginToDirectory (AuthUser,AuthPass);
		if (ccode==ERROR_SUCCESS)
			return ERROR_SUCCESS;
		StrCopy (user,AuthUser);
		StrCopy (pass,AuthPass);
		BadDefAuth=TRUE;
	}

	FailedAuthentication(ccode);

	do {
		if (AuthenticateUI(user,pass))
			ccode=LoginToDirectory (user,pass);
		else {
			dprintf ("Authentication Cancelled\n");
			iprintf (InitMessage(IDS_AUTH_CANCELLED));
			AuthCancelled=TRUE;
			LogInstallMessage("Authentication cancelled\n");
			return EI_NOT_AUTHENTICATED;
		}
		if (ccode) {
			iprintf (InitMessage(IDS_AUTH_FAILED));
			LogInstallMessage("Authentication failed 0x%x\n",ccode);
			FailedAuthentication(ccode);
		}
	}
	while (ccode && !Authenticated);

	iprintf (InitMessage(IDS_AUTHENTICATED));
	return ERROR_SUCCESS;
}
/*******************************************************************************************/
int DSPrelims (void) {

	DWORD ccode=0;
	
	UsingDS=TRUE;

	if (DSNew) {
		HKEY hkey;
		DSNew = 0;

		if (RegOpenKey(HKEY_LOCAL_MACHINE,REGHEADER,&hkey) != ERROR_SUCCESS)
			return EI_OPEN_REGHEADER;

		GetStr(hkey,"ProfileUserName",AuthUser,sizeof(AuthUser),"");
		GetStr(hkey,"ProfilePassword",AuthPass,sizeof(AuthPass),"");
		GetStr(hkey,"Container",Container,sizeof(Container),"");

		if (NumChars(Container)<1) {
			HKEY hkey2;

			BadDefAuth=FALSE;
			UseDefaultContext();
			if (!GetContextUI(Container)) {
				LogInstallMessage("Error Getting Context before initDS in DS preliminaries\n");
				RegCloseKey(hkey);
				return EI_GET_CONTEXT;
			}

			if (RegOpenKey(HKEY_LOCAL_MACHINE,REGHEADER,&hkey2) == ERROR_SUCCESS) {
				PutStr(hkey2,"Container",Container);
				RegCloseKey(hkey2);
			}
		}
		if ((ccode=InitDS())!=ERROR_SUCCESS) {
			LogInstallMessage("Error (0x%x)(%d) initializing DS in DS Preliminaries\n",ccode,ccode);
			RegCloseKey(hkey);
			return ccode;
		}

		if ((ccode=AuthenticateUser())!=ERROR_SUCCESS){
			LogInstallMessage("Error (0x%x)(%d) authenticating user in DS preliminaries\n",ccode,ccode);
			RegCloseKey(hkey);
			return ccode;
		}

		PutStr(hkey,"ProfileUserName",AuthUser);
		PutStr(hkey,"ProfilePassword",AuthPass);
		RegCloseKey(hkey);
	}

	if (!Authenticated) {
		return EI_NOT_AUTHENTICATED;
	}

	return ERROR_SUCCESS; // return ERROR_SUCCESS if we are just going to continue in ParseObjLine and ParseProileLogin
}
/*******************************************************************************************/
DWORD ParseObjLine(char *line,char *q,char ds) {

	char *name,*pass="",*members="",*dir="",*prims="";
	char super=0;
	int ccode=ERROR_SUCCESS;

	if (!line||!q) return EI_NULL_PARAMETER;

	if (ds) 
		if ((ccode = DSPrelims())!=ERROR_SUCCESS)
			return ccode; // if DSPrelims doesn't return ERROR_SUCCESS then we just specified the container but didn't authenticate yet

	if (!stricmp(line,"USER")) {
		name = q;
		q = StrChar(q,',');
		if (q) {
			*q = 0;
			pass = q+1;
			q = StrChar(q+1,','); // Added ,','
			if (q) {
				*q = 0;
				if (!stricmp(q+1,"SUPERVISOR")) // changed "strnicmp" to "stricmp"
					super = 1;
				}
			}
		if (name[0]) {
			if (ds) {
				if (Authenticated)
					CreateDSUser(name,pass,super);
				else {
					LogInstallMessage("Not authenticated so can't create a DS user\n");
					return EI_NOT_AUTHENTICATED;
				}
			}
			else
				return CreateBinderyUser(name,pass,super);
		}
	}
	else if (!stricmp(line,"GROUPMEMBERS")) {
		name = q;
		q = StrChar(q,',');
		if (q) {
			*q = 0;
			members = q+1;
		}
		if (name[0]) {
			if (ds) {
				if (Authenticated)
					CreateDSGroup(name,members);
				else {
					LogInstallMessage("not authenticated so can't create DS group\n");
					return EI_NOT_AUTHENTICATED;
				}
			}
			else
				return CreateBinderyGroup(name,members);
		}
	}
//	else if (!stricmp(line,"AUTH")) {
//		name = q;
//		q = StrChar(q,',');
//		if (q) {
//			*q = 0;
//			LoginToDirectory(name,q+1);
//			}
//		}
	else if (!stricmp(line,"GROUPPRIMS")) {
		name = q;
		q = StrChar(q,',');
		if (q) {
			*q = 0;
			dir = q+1;
			q = StrChar(q+1,',');   // added ,','
			if (q) {
				*q = 0;
				prims = q+1;
			}
		}
		if (name[0]) {
			if (ds) {
				if (Authenticated)
					ccode=AddDSGroupTrustee(name,dir,ConvertPrims(prims));
				else {
					LogInstallMessage("not authenticated so can't create DS group trustee\n");
					return EI_NOT_AUTHENTICATED;
				}
			}
			else
				ccode=AddBinderyGroupTrustee(name,dir,ConvertPrims(prims));
		}
	}
	return ccode;
}
/****************************************************************************************************/
DWORD DeleteTree(char *szDir)
{
	WIN32_FIND_DATA fd;
	HANDLE	handleFind=0;
	char	szAll[IMAX_PATH];
	DWORD	
			bBad = ERROR_SUCCESS,
			bVal = ERROR_SUCCESS;

	memset (szAll,0,sizeof(szAll));
	memset (&fd,0,sizeof (WIN32_FIND_DATA));

	if (!szDir[0])
		return ERROR_SUCCESS;
	if (access (szDir,0)) {
		rmdir(szDir);
		return ERROR_SUCCESS;
	}
	sssnprintf(szAll,sizeof(szAll),"%s" sSLASH "*.*",szDir);
	DeletePurgeFile (szAll);
	handleFind = FindFirstFile(szAll, &fd);

	if (handleFind == INVALID_HANDLE_VALUE) { // the directory is empty
//		dprintf ("Trying to remove directory...");
		if (StrComp (szDir,ProgramDir)) // we can't remove the program directory
			rmdir(szDir);
//		dprintf ("done\n");
		return ERROR_SUCCESS;
	}
	while (handleFind != INVALID_HANDLE_VALUE)
	{
		char szToDelete[IMAX_PATH];
		ThreadSwitch();

		if (fd.cFileName[0] != '.')
		{
			sssnprintf(szToDelete,sizeof(szToDelete),"%s" sSLASH "%s",szDir,fd.cFileName);
			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				bVal = DeleteTree(szToDelete);
				if (bVal!=ERROR_SUCCESS)
					LogInstallMessage("failed on DeleteTree(%s):%d\n",szToDelete,bVal);
			}
			else {
//				dprintf ("Deleting file: %s\n",szToDelete);
				bVal = DeletePurgeFile(szToDelete);
				if (bVal!=ERROR_SUCCESS) {
					bVal = errno;
					MakeWriteable (szToDelete,0xff01fffc);
					bVal = DeletePurgeFile(szToDelete);
					if (bVal!=ERROR_SUCCESS) {
						LogInstallMessage("Couldn't delete: %s\tbVal: %d\terrno: %d\n",szToDelete,bVal,errno);
					}
				}
			}
			bBad=bVal;
		}

		if (!FindNextFile(handleFind, &fd))
			break;
	}

	FindClose(handleFind);

	bVal = rmdir(szDir);

	if (bVal!=ERROR_SUCCESS) {
		LogInstallMessage("failed on rmdir (%s)  errno:%d   NWerrno: %d\n",szDir,errno,NetWareErrno);
		bBad = bVal;
	}
	
	if (bBad!=ERROR_SUCCESS)
		LogInstallMessage("Error Deleting Tree\n");

	return bBad;
}
/*******************************************************************************************/
DWORD AddToDeleteList(char *path, BOOL isDir ) {

	PathNode *newNode=(PathNode*)malloc( sizeof(PathNode) );
	
	if( !newNode ) return EI_MEMORY;

	memset( newNode, 0, sizeof(PathNode) );

	newNode->isDir=isDir;
	strncpy( newNode->path, path, IMAX_PATH );

	if( !DelHead ) {
		DelHead=newNode;
		DelCurr=newNode;
		return ERROR_SUCCESS;
	}
	DelCurr->next=newNode;

	DelCurr=newNode;

	return ERROR_SUCCESS;
}
/*******************************************************************************************/
DWORD DeleteFilesNow() {

	iprintf(LS(IDS_DELETING_NOW));

	while ( DelHead ) {

		if(DelHead->isDir) {
			PrintStuff (LS(IDS_DIRECTORY),DelHead->path);
			DeleteTree(DelHead->path);
		}
		else {
			PrintStuff (LS(IDS_FILE),DelHead->path);
			PurgeFile(DelHead->path);
		}

		DelCurr=DelHead;
		DelHead=DelHead->next;
		free (DelCurr);
	}
	DelHead=NULL;
	DelCurr=NULL;
	return ERROR_SUCCESS;
}
/*******************************************************************************************/
DWORD ParseDirLine(char *line) {

	int cc;

	if (!line) return EI_NULL_PARAMETER;

dprintf ("%s Directory: %s\n",(Install? "Creating":"Removing"),line);

	PrintStuff (LS(IDS_DIRECTORY),line);
	if (Install) {
		cc=_VerifyPath(line);
		if (cc)
			LogInstallMessage("Error creating directory\n");
	}
	else {
		cc=AddToDeleteList(line,TRUE);
		if (cc)
			LogInstallMessage("Error deleting directory\n");
	}
	return cc;
}
/****************************************************************************************/
DWORD ParseRunLine(char *line) {

	DWORD retval=0;

	if (!Install)
		return ERROR_SUCCESS;
	if (!line)	return EI_NULL_PARAMETER;
	DisablePreviousInstallation(UNLOAD_OLD_NLM);
//	DisplayConsoleScreen ();
	retval = !_LoadLibrary(line,true); // misc.c or nlm.c
//	DisplayInstallScreen();
	if (retval)
		LogInstallMessage("Error loading library: %s\n",line);

	return retval;
}
/*******************************************************************************************/
BOOL GetFileStat(char *file,struct stat *filestat) {

	int fh=-1;

	fh=open (file,O_RDONLY|O_BINARY);

	if (fh==INVALID_FILE_HANDLE_VALUE) {
		LogInstallMessage("unable to open %s to get file stat\n",file);
		return FALSE;
	}

	if ((fstat(fh,filestat)!=ERROR_SUCCESS)) {
		close (fh);
		LogInstallMessage("unable to get file stat from file %s\n",file);
		return FALSE;
	}
	close (fh);
	return TRUE;
}
/*******************************************************************************************/
// returns:
//   1 if "to" is newer,
//   0 if same time,
//  -1 if "to" doesn't exist or "from" is newer
//  -2 if an error occured

int CheckNewerFileTime(char *to,char *from) {

	struct stat tostat,fromstat;

	if (!to || !from) {
		dprintf("Parameter Error, to is %s, from is %s\n", to?to:"NULL", from?from:"NULL");
		return -2;
	}

	if ( !strlen(to) || !strlen(from) ) {
		dprintf("Parameter Error, (%s) is not a valid filename\n", !strlen(to) ? "destination" : "source" );
		return -2;
	}

	if (access(from, 0)) {
		dprintf("Parameter Error, %s not fould!\n", from);
		return -2;
	}

	if (access(to, 0))
		return -1;

	if (!stat (to,&tostat) && !stat (from,&fromstat))
		return (tostat.st_mtime == fromstat.st_mtime) ? 0 : ((tostat.st_mtime > fromstat.st_mtime) ? 1 : -1) ;

	else {
		LogInstallMessage("Error Getting File Stat for file %s\n",to);
		return -2;
	}
}
/*******************************************************************************************/
DWORD ConfirmOverwrite(char *to,char *from) {

	switch (CheckNewerFileTime(to,from)) {// if the destination file is newer than the source
		case 1:
			if (AskOverwrite(to,from)) {	// ask if the user wants to overwrite
				dprintf ("Overwriting %s\n",to);
				return MyCopyFile(from,to);
			}
			else {
				dprintf ("Not Overwriting %s\n",to);
				return ERROR_SUCCESS;
			}
			break;

		case -1:
			return MyCopyFile(from,to);

		default:
			dprintf ("Not Overwriting %s, error\n",to);
			return ERROR_SUCCESS;
	}
}
/*******************************************************************************************/
DWORD ParseRequiredCopyLine(char *to,char *from) {

	char realTo[IMAX_PATH];
	char *q;

	if (!to || !from) return EI_NULL_PARAMETER;
		
	memset (realTo,0,sizeof(realTo));
	StrCopy(realTo,to);
	ThreadSwitch();
	q=strrchr (to,cSLASH);
	if (q) q++;
	else q=to;
	PrintStuff(LS(IDS_FILE),q);
	if (Install) {
		if ((CheckNewerFileTime(to,from)) == -1)
			return MyCopyFile(from,to);
		else {
			dprintf ("Source File is not everwriting\n");
		}
	}
	return ERROR_SUCCESS;
}
/*******************************************************************************************/
DWORD ParseCopyDirLine(char *to,char *from) {

	DWORD
		cc=0;
	WIN32_FIND_DATA
		fd;
	HANDLE
		han;
	char
		realFrom[IMAX_PATH],
		realTo[IMAX_PATH];
			

	if (!to || !from) return EI_NULL_PARAMETER;

	dprintf ("[%s]=[%s]\n",to,from);
		
	if (UsingDS && !Authenticated) {
		dprintf ("Can't Copy if Not Authenticated\n");
		return ERROR_SUCCESS;
	}

	dprintf ("Copying Directory: %s\n",to);
	ThreadSwitch();

	if (Install) {
		cc=_VerifyPath(to);
		if (cc) {
			LogInstallMessage("Error creating directory\n");
			return cc;
		}
	}
	else if (access (to,0)) { // if the directory has already been deleted there's nothing to do
		return ERROR_SUCCESS;
	}
	memset (&fd,0,sizeof (WIN32_FIND_DATA));
	strcat (from,sSLASH "*.*");
	if ((han=FindFirstFile(from,&fd)) !=INVALID_HANDLE_VALUE) {
		from[strlen(from)-4] = 0;
		dprintf ("[%s]\n",from);
		do {
			if( IsDir(fd.dwFileAttributes) )
				continue;

			PrintStuff(LS(IDS_FILE),fd.cFileName);
			dprintf ("%s:[%s]\n",Install?"Copying":"Deleting",fd.cFileName);

			sssnprintf (realTo,sizeof(realTo),"%s" sSLASH "%s",to,fd.cFileName);
			if (Install) {
				sssnprintf (realFrom,sizeof(realFrom),"%s" sSLASH "%s",from,fd.cFileName);
				if (access(realTo,0)==0) // if the destination file already exists
					cc=ConfirmOverwrite(realTo,realFrom);
				else
					cc=MyCopyFile(realFrom,realTo);//==FALSE?GetLastError():0;

				if (cc) 
					LogInstallMessage("Error (%d) copying file %s\n",cc,realFrom);
			}
			else{ // add the individual files to the delete list
				cc=AddToDeleteList(realTo,FALSE);
//				cc=unlink (realTo);
				if (cc) 
					LogInstallMessage("Error (%d) deleting file %s\n",cc,realTo);
			}					
		} while (FindNextFile (han,&fd));
		FindClose (han);
	}
	else {
		dprintf ("no matching files [%s]\n",from);
	}
	return cc;
}
/*******************************************************************************************/
DWORD ParseCopyLine(char *to,char *from) {

	char realTo[IMAX_PATH],*q;

	if (!to || !from) return EI_NULL_PARAMETER;
		
	if (UsingDS && !Authenticated) {
		dprintf ("Can't Copy if Not Authenticated\n");
		return ERROR_SUCCESS;
	}

	dprintf ("Copying File: %s\n",to);
	memset (realTo,0,sizeof(realTo));
	StrCopy(realTo,to);
	ThreadSwitch();
	q=strrchr (to,cSLASH);
	if (q) q++;
	else q=to;
	PrintStuff(LS(IDS_FILE),q);
	if (Install) {
//		iprintf (InitMessage(IDS_COPY_X_TO_Y),from,to);
		dprintf ("%s\n",to);
		if (access(realTo,0)==0) // if the destination file already exists
			return ConfirmOverwrite(realTo,from);
		else
			return MyCopyFile(from,realTo);//==FALSE?GetLastError():0;
	}
	else {
		dprintf (LS(IDS_DELETING_FILE),realTo);
		AddToDeleteList(realTo,FALSE);
		//unlink(realTo);
	}
	return ERROR_SUCCESS;
}
/*******************************************************************************************/
void PrintStuff(char *heading,char *string) {

	if (string) {
		if (!cfPCB)
			MyCreatePortal (&cfPort,&cfPCB,	2,	40,	4,	38,	4,	80,	SAVE,	NULL,	VNORMAL,	SINGLE,	VNORMAL,	InstNut);
		NWSClearPortal (cfPCB);
		NWSDrawPortalBorder (cfPCB);
		NWSDisplayTextInPortal (0,1,reinterpret_cast<unsigned char*>(heading),VNORMAL,cfPCB);
		NWSDisplayTextInPortal (1,5,reinterpret_cast<unsigned char*>(string),VNORMAL,cfPCB);
	}
}
/****************************************************************************************/
/****************************************************************************************/
//DWORD ParseLoginScript (char *script) { // replace the $$ strings in the login script file

DWORD ReplaceFileVariables (char *inFile) {

	struct stat sStat;
	int sHan=-1;
	WORD OldAttr=0;
	FILE *in=NULL,*out=NULL;
	DWORD retval=0;
	time_t currtime=0;
	char success=FALSE;
	char s[256],TempFile[256],BackupFile[256],*q=NULL;

	if (!inFile) return EI_NULL_PARAMETER;

	memset (TempFile,0,sizeof(TempFile));
	memset (BackupFile,0,sizeof(BackupFile));
	memset (s,0,sizeof(s));
	memset (&sStat,0,sizeof(struct stat));
	
	in = fopen(inFile,"rt");
	if (in) {
		strcpy (BackupFile,inFile);
		q=strrchr (BackupFile,'.');
		if (q) *q=0;
		strcat (BackupFile,".bak");
//      sssnprintf (BackupFile,sizeof(BackupFile),"%s" sSLASH "%s",ProgramDir,TempFile);
		sssnprintf (TempFile,sizeof(TempFile),"%s" sSLASH "tempfile.tmp",ProgramDir);
		out = fopen(TempFile,"wt");
		if (out) {
			memset (s,0,sizeof(s));
			while (fgets(s,sizeof(s),in))
				fputs (ReplaceVariables(s),out);
			currtime=time(NULL);
			success = TRUE;
			fclose(out);
		}
		fclose(in);
	}
	if (success) {
		PurgeFile(BackupFile);
		
		sHan = open(inFile,O_RDONLY);				// make the inFile writeable
		if (sHan!=-1) {
			if (fstat(sHan,&sStat)==0) {			// so get the current attributes
				close (sHan);
				OldAttr=sStat.st_attr;				// save the old attributes
				sStat.st_attr &= 0xfffffffc;		// set them to read/write
				MySetFileInfo (inFile,&sStat);		// and write to the file
			}				
			else close (sHan);
		}
		retval=rename(inFile,BackupFile);
		if (!retval)
			retval=rename(TempFile,inFile);
		PurgeFile(TempFile);
	}

	return retval;
}
/****************************************************************************************/
void ClearInstArrays(BOOL start) {


	if ( AutoBuff )	{	free ( AutoBuff );		AutoBuff=NULL;	}
	if ( VPBuff )	{	free ( VPBuff );		VPBuff=NULL;	}
	if ( fbuf )		{	free ( fbuf );			fbuf=NULL;		}

//	AutoBuff=NULL;
//	VPBuff=NULL;
//	fbuf=NULL;
	fpos=NULL;
	fbytes=0;
	fsize=0;

	if (start) {
		cfPort=0;
		cfPCB=NULL;

		DSNew = 1;
		dsInitialized=FALSE;

		AuthCancelled=FALSE;
		BadDefAuth=FALSE;

		Authenticated=0;
		ConnectionNum=0;
		rfile=NULL;

		memset (Container,0,sizeof(Container));
		memset (AuthUser,0,sizeof(AuthUser));
		memset (AuthPass,0,sizeof(AuthPass));
		memset (WWWRoot,0,sizeof(WWWRoot));
		memset (LDVPWWWRoot,0,sizeof(LDVPWWWRoot));	
	}
}
/****************************************************************************************
char *ReadNextLine (char *buf,int size,FILE*fp) { //int handle) {

	int ch=0;
	int i=0;
//	int cc=0, fh=fileno(fp);
	
	if (buf) {
		for (i=0 ; i<size ; i++) {
			ch = fgetc(fp);
			if (ch==EOF || feof (fp)) return NULL;
			if (ch=='\n') return buf;
			buf[i] = ch;
			buf[i+1] = 0;
		}
	}
	else
		return NULL;
	return buf;
}
****************************************************************************************/
char* ReturnNextLine (char*buf,int size) {

	char *p=buf;

	REF(size);

	while (*fpos=='\n' || *fpos=='\r') {
		fpos++;
		fbytes++;
		if (*fpos=='\n') LineNum++;
	}
	if (*fpos==0 || fbytes>=fsize) return NULL;
	for (;*fpos!='\n' && *fpos!='\r' && *fpos!=0 && fbytes<fsize ; fbytes++,fpos++,buf++) {
		*buf=*fpos;
		*(buf+1)=0;
	}
//	while (*fpos=='\n' || *fpos=='\r') {
//		fpos++;
//		fbytes++;
//	}
	*buf=0;
//	dprintf ("returning \"%s\"\n",p);
	return p;
}
/****************************************************************************************/
DWORD CountUpdateFiles() {
	char 
		*TempPos=fpos;
	int 
		TempBytes=fbytes;
	char
		str[520];
	char
		*line=NULL,
		*q=NULL;

	for( UpdateFiles=0,memset (str,0,sizeof(str)) ; ReturnNextLine(str,sizeof(str)) ; memset (str,0,sizeof(str)) ) {
		if(str[0]=='[') break;

		if (AbortInstall(ABORT_KEY)) return EI_ABORT_INSTALL;

		ThreadSwitchWithDelay();

		line=PrepareLine(str);
		if (line==NULL)
			continue;

		if(line[0]=='[') break;

		q = StrChar(line,'=');
		if (q) { // ie. if there is an '=' in the line
			UpdateFiles++;
		}
	}

	fbytes=TempBytes;
	fpos=TempPos;

	return ERROR_SUCCESS;
}
/****************************************************************************************/
DWORD UnParseDefaultFile(void) {

	DWORD ret=0;

	// --------------------------------------
	// ksr - NetWare Certification, 8/30/2002
		
	// RegOpenDatabase();
	ret = RegOpenDatabase();
	if ( ret != ERROR_SUCCESS ) 
	{
		dprintf( "Error - UnParseDefaultFile()\n" );
		if( rfile ) 	
			fprintf( rfile, "\nError - UnParseDefaultFile()" );
		return ret;
	}
	// --------------------------------------

	ret = ParseDefaultFile(I_NORMAL_REMOVE);   // remove maintainance stuff

	RegCloseDatabase(TRUE);
	DeletePurgeFile(VPRegFileMask);

	chdir (NW_SYSTEM_DIR); // we have to change the CWD in order to remove the ProgramDir

	rmdir (HomeDir);

	return ret;
}
/****************************************************************************************/
void AddSomeKeys(void) {

	HKEY hkey=0;
	char realPassword[PASS_MAX_CIPHER_TEXT_BYTES];

	if (rfile) 	fprintf (rfile,"AddSomeKeys\n");
	memset (realPassword,0,sizeof(realPassword));

	RegOpenKey(HKEY_LOCAL_MACHINE, REGHEADER, &hkey);

	MakeEP(realPassword, sizeof(realPassword), PASS_KEY1, strupr(FileServerName), DEFAULT_PASSWORD , strlen(DEFAULT_PASSWORD)+1);
	PutStr(hkey,"Password",realPassword);
//	usePassword=TRUE;

	PutVal (hkey,"ProductVersion",PRODUCTVERSION|(BUILDNUMBER<<16));
	RegCloseKey (hkey);
}
/****************************************************************************************/
DWORD DisablePreviousInstallation(int which) {

	if (Install && !access(NW_SYSTEM_DIR sSLASH "LPROTECT.NLM",0)) {
		switch (which) {
			case DISABLE_AUTO:
				DisableAutoexec();
				break;
			case DISABLE_SYS_LOG:
				DisableSystemLoginScript();
				break;
			case DISABLE_PROF_LOG:
				DisableProfileLoginScript();
				break;
			case UNLOAD_OLD_NLM:
				if (UnloadOldNLMs(InstNut))
					return EI_UNLOAD_OLD_NLM;
				break;
			case DISABLE_AMS_DELETE:
				DisableAMSDelete();
				break;
			case 5://case PRINT_DISABLE_MSG:
				iprintf (LS(IDS_OLD_LDVP_DISABLED));
				iprintf (LS(IDS_UNINSTALL_OLD_LDVP));
				NTxSleep (4000);
				break;
			default:
				dprintf ("Previous installation of LANDesk 3.x or 4.x exists\n");
				iprintf (LS(IDS_DISABLE_OLD_LDVP));
		}
	}
	return ERROR_SUCCESS;
}
/****************************************************************************************/
DWORD UnloadOldNLMs(NUTInfo *nut) {

	char ab[5];
	LONG port,palette;
	PCB* pcb;
	int height=9,R=0;

	memset (ab,0,5);
	strcpy(ab,ABORT_KEY);
	ab[1]=27;

	if (nut && FindNLMHandle("LPROTECT")) {

		palette=NWSGetScreenPalette(nut);
		NWSSetScreenPalette (WARNING_PALETTE,nut);
//		NWSFillPortalZoneAttribute (0,0,5,40,VREVERSE,PCB);
		if (MyCreatePortal (&port,&pcb,	5,	15,	height,	50,	height+2,	80,	SAVE,	NULL,	VNORMAL,	SINGLE,	VNORMAL,	nut)) {
			R=NWSDisplayTextJustifiedInPortal(JCENTER,0,24,50,reinterpret_cast<unsigned char *>(LS(IDS_UNLOAD_OLD_LDVP)),VNORMAL,pcb);
			NWSDisplayTextJustifiedInPortal(JCENTER,R,24,50,reinterpret_cast<unsigned char *>(LS(IDS_UNLOAD_NOW)),VNORMAL,pcb);
		}
		else {
			iprintf (LS(IDS_UNLOAD_OLD_LDVP));
			iprintf (LS(IDS_UNLOAD_NOW));
		}
		NWSSetScreenPalette (palette,nut);
		while (FindNLMHandle("LPROTECT")) {
			if (AbortInstall(ab)) {
				NWSDestroyPortal (port,nut);
				return EI_UNLOAD_OLD_NLM;
			}
			NTxSleep(500);
		}
		NWSDestroyPortal (port,nut);

		if (FindNLMHandle("PSCAN"))
			system ("UNLOAD PSCAN");
	}
	return ERROR_SUCCESS;
}
/****************************************************************************************/
BOOL isOldStartTag(char *s) {

//	return (s ? (s ? FALSE : TRUE) : TRUE)
	return strnicmp(s,STARTTAG4,strlen(STARTTAG4)) ?
	(strnicmp(s,STARTTAG5,strlen(STARTTAG5)) ? FALSE : TRUE) : TRUE;
}
/*******************************************************************************************/
BOOL isOldEndTag(char *s) {

	return strnicmp(s,ENDTAG4,strlen(ENDTAG4)) ?
		(strnicmp(s,ENDTAG5,strlen(ENDTAG5)) ? FALSE : TRUE) : TRUE;
}
/****************************************************************************************/
BOOL AlreadyDisabled(char *s) {

	return (strnicmp(s,DISABLETAG,strlen(DISABLETAG)) ? FALSE : TRUE);
}
/****************************************************************************************/
void DisableAutoexec(){

	FILE *in=NULL,*out=NULL;
	char success=FALSE;
	char s[256];
	char state=START_TAG_SEARCH;
	char AutoExec[256],AutoBack[256],AutoTemp[256],Auto[20];
	FILE_SERV_INFO fsInfo;

	dprintf ("Disabling the vp_auto call in autoexec.ncf\n");

	memset (&fsInfo,0,sizeof (FILE_SERV_INFO));
	GetServerInformation(65,&fsInfo);

	dprintf ("SFTLevel is %d\n",fsInfo.SFTLevel);
	if (fsInfo.SFTLevel==3)
		StrCopy (Auto,"msauto");
	else
		StrCopy (Auto,"autoexec");

	sssnprintf (AutoExec,sizeof(AutoExec),SystemPath"%s.ncf",Auto);
	sssnprintf (AutoBack,sizeof(AutoBack),SystemPath"%s.bak",Auto);
	sssnprintf (AutoTemp,sizeof(AutoTemp),SystemPath"%s.n1w",Auto);

	in = fopen(AutoExec,"rt");
	if (in) {
		out = fopen(AutoTemp,"wt");

		if (out) {
			memset (s,0,sizeof(s));
			while (fgets(s,sizeof(s),in)) {
				ThreadSwitch();
				if (state==START_TAG_SEARCH && isOldStartTag(s)) {	// look for the start tag
					state=FOUND_START_TAG; 							// and change the state
					fputs(s,out);						// write out the line
				}
				else if (state==FOUND_START_TAG && isOldEndTag(s)) {	// look for the end tag
					state=FOUND_END_TAG;							// and change the state when we find it
					fputs(s,out);						// write out the line
				}
				else if (strstr (s,"VP_AUTO")) {		// if we find the VP_AUTO call in the line
					if (AlreadyDisabled(s))
						fputs(s,out);						// write out the line
					else
						fprintf (out,"%s %s",DISABLETAG,s);	// disable it
				}
				else if (state==START_TAG_SEARCH || state==FOUND_END_TAG)
					fputs(s,out);						// write out the line
				else if (state==FOUND_START_TAG)					// if we are inside the LDVP 3.x section
					fprintf (out,"%s %s",DISABLETAG,s);	// disable the line
				memset (s,0,sizeof(s));
			}
			success = TRUE;
			fclose(out);
		}
		fclose(in);
	}
	if (state==FOUND_START_TAG) { // We never found the end tag! We may have dropped the last part of the autoexec.ncf file

		iprintf (LS(IDS_OLD_LDVP_AUTO_ERR),AutoExec);
		success=FALSE;

		in = fopen(AutoExec,"rt");
		if (in) {
			out = fopen(AutoTemp,"wt");

			if (out) {
				memset (s,0,sizeof(s));
				while (fgets(s,sizeof(s),in)) { 	// this time just look for the VP_AUTO call
					ThreadSwitch();
					if (strstr (s,"VP_AUTO")) {		// if we find the VP_AUTO call in the line
						if (AlreadyDisabled(s))
							fputs(s,out);						// write out the line
						else {
							success = TRUE;
							fprintf (out,"%s %s",DISABLETAG,s);	// disable it
						}
					}
					else
						fputs(s,out);						// write out the line
				}
				fclose(out);
			}
			fclose(in);
		}
		if (success) {
			iprintf (LS(IDS_VP_AUTO_DISABLED));
			iprintf (LS(IDS_REMOVE_OLD_BY_HAND));
		}
		else {
			iprintf (LS(IDS_OLD_LDVP_NO_DISABLE));
			iprintf (LS(IDS_REMOVE_OLD_BY_HAND));
		}
	}
	if (success) {
		DeletePurgeFile(AutoBack);
		rename(AutoExec,AutoBack);
		rename(AutoTemp,AutoExec);
	}
	return;
}
/****************************************************************************************/
void DisableSystemLoginScript(){

	FILE *in=NULL,*out=NULL;
	char success=FALSE;
	char s[256],*q=NULL;
	char state=START_TAG_SEARCH;
	int fh;

	memset (s,0,sizeof(s));

	dprintf ("Disabling the 3/4.x system login script\n");

	in = fopen(BinderyLoginPath"net$log.dat","rt");
	if (!in) {
		fh=open (BinderyLoginPath"net$log.dat",O_CREAT|O_RDONLY);
		if (fh!=-1) close (fh);
		in = fopen(BinderyLoginPath"net$log.dat","rt");
	}
	if (in) {
		out = fopen(BinderyLoginPath"net$log.n1w","wt");
		if (out) {
			if (Install) {
				while (fgets(s,sizeof(s),in)) { //look for the 3.x start tag
					ThreadSwitch();
					if (state==START_TAG_SEARCH && isOldStartTag(s)) { // we found it
						state=FOUND_START_TAG;
					}
					else if (state==FOUND_START_TAG && isOldEndTag(s)) {
						state=FOUND_END_TAG;
						break;
					}
				}
				fseek (in,0,SEEK_SET);

				if (state!=FOUND_END_TAG) { 	// if we didn't find it at all or didn't find the end tag then add it at the beginning of the file
					if (state==FOUND_START_TAG) {	// we didn't find the end tag
						state=FOUND_END_TAG; 	// with this at 2 it will simply add the rest of the original login script on to the new one
						iprintf (LS(IDS_OLD_LDVP_LOGSCR_ERR));
						iprintf (LS(IDS_OLD_LDVP_LOGSCR_DISABLE));
						iprintf (LS(IDS_REMOVE_OLD_BY_HAND));
					}
				}
				else state=START_TAG_SEARCH;

				memset (s,0,sizeof(s));
			}
			while (fgets(s,sizeof(s),in)) {
				ThreadSwitch();
				if (state==START_TAG_SEARCH && isOldStartTag(s)) {
					state=FOUND_START_TAG;
					fputs(s,out);
				}
				else if (state==FOUND_START_TAG && isOldEndTag(s)) {
					state=FOUND_END_TAG;
					fputs(s,out);
					}
				else if (state!=FOUND_START_TAG && (q=strstr (s,"LANDESKVIRUSGROUP"))) { // if we find a reference to ldvp group outside of ldvp section
					*q=0;
					q+=17;
					fprintf (out,"%sDISABLED_BY_SYMANTEC_ANTI_VIRUS_GROUP%s",s,q);	// change it to a non-existant group
				}
				else if (state==START_TAG_SEARCH || state==FOUND_END_TAG)
					fputs(s,out);
				else if (state==FOUND_START_TAG) {
					if (AlreadyDisabled(s))
						fputs(s,out);
					else
						fprintf (out,"%s %s",DISABLETAG,s);
				}
				memset (s,0,sizeof(s));
			}
			success = TRUE;
			fclose(out);
		}
		fclose(in);
	}
	if (success) {
		DeletePurgeFile(BinderyLoginPath"net$log.bak");
		rename(BinderyLoginPath"net$log.dat",BinderyLoginPath"net$log.bak");
		rename(BinderyLoginPath"net$log.n1w",BinderyLoginPath"net$log.dat");
	}

	return;
}
/****************************************************************************************/
void DisableProfileLoginScript() {

	// get stream to ou login and add buf
	char *buff=NULL;
	int in=-1,out=-1,ccode=-1;
	char s[256],t[256];
	char state=START_TAG_SEARCH;
//	char added = FALSE;
	char *q=NULL,*r=NULL;

	if (NumBytes (Container) < 1) {
		LogInstallMessage("Trying to disable old LDVP profile login with invalid container\n");
		return;
	}

	in = GetStreamHandle(Container);
	if (in==-1 && NumBytes(Container)<1) {
		UseDefaultContext();
		in = GetStreamHandle(Container);
	}
	if (in!=-1) {
		HKEY hkey;

		if (RegOpenKey(HKEY_LOCAL_MACHINE,REGHEADER,&hkey) == ERROR_SUCCESS) {
			PutStr(hkey,"Container",Container);
			RegCloseKey(hkey);
		}

		buff = (char*)malloc(BUFSIZE);
		if (!buff) {
			close(in);
			LogInstallMessage("Out of memory trying to disable old LDVP profile login\n");
			return;
		}
		if (buff) {
			memset(buff,0,BUFSIZE);
			q = buff;
			ccode=read(in,buff,BUFSIZE);	// read in the entire script to a buffer
			close(in);					// close it
			in = 0;
			out = GetStreamHandle(Container);	// Open it again so we can write over it

			if (out!=-1) {
				memset(s,0,sizeof(s));
				q = buff;
				while ( q=GetNextLine(q,s)) {	// get a line from the buffer
					ThreadSwitch();
					if (isOldStartTag(s)) {		// look for the start tag
						state=FOUND_START_TAG;	// if we find the start tag set state to FOUND_START_TAG
					}
					else if (state==FOUND_START_TAG && isOldEndTag(s)) {		// if we have found the start tag and we then find the end tag
						state=FOUND_END_TAG;								// set the state to FOUND_END_TAG
						break;
					}
				}
				if (state!=FOUND_END_TAG) { 	// if we didn't find it at all or didn't find the end tag then add it at the beginning of the file
					if (state==FOUND_START_TAG) {	// we didn't find the end tag
						state=FOUND_END_TAG; 	// with this at 2 it will simply add the rest of the original login script on to the new one
						iprintf (LS(IDS_OLD_LDVP_LOGSCR_ERR));
						iprintf (LS(IDS_OLD_LDVP_LOGSCR_DISABLE));
						iprintf (LS(IDS_REMOVE_OLD_BY_HAND));
					}
				}
				else state=START_TAG_SEARCH;

				q = buff;								// start at the beginning of the script

				while (q=GetNextLine(q,s)) {			// read in a line from the buffer
					ThreadSwitch();
					if (state==START_TAG_SEARCH && isOldStartTag(s)) {
						state=FOUND_START_TAG;
						write(out,s,NumBytes(s));
					}
					else if (state==FOUND_START_TAG && isOldEndTag(s)) {
						state=FOUND_END_TAG;
						write(out,s,NumBytes(s));
					}
					else if (state!=FOUND_START_TAG && (r=strstr (s,"LANDESKVIRUSGROUP"))) { // if we find a reference to ldvp group outside of ldvp section
						*r=0;
						r+=17;
						sssnprintf (t,sizeof(t),"%sDISABLED_BY_SYMANTEC_ANTI_VIRUS_GROUP%s",s,r);	// change it to a non-existant group
						write(out,t,NumBytes(t));
					}
					else if (state==START_TAG_SEARCH || state==FOUND_END_TAG)
						write(out,s,NumBytes(s));
					else if (state==FOUND_START_TAG) {
						if (AlreadyDisabled(s))
							StrCopy (t,s);
						else
							sssnprintf (t,sizeof(t),"%s %s",DISABLETAG,s);
						write(out,t,NumBytes(t));
					}

//					if (state==START_TAG_SEARCH && isStartTag(s)) {	// look for the start tag if state is zero
//						state=FOUND_START_TAG;						// we found it so set state to 1
//						if (Install)
//							write(out,buf,NumBytes(buf));	// write out the LDVP section
//						added = TRUE;
//					}
//					else if (state==FOUND_START_TAG && isEndTag(s))		// if we have found the start tag then look for the end tag
//						state=FOUND_END_TAG;							// and the set state to 2
//					else if (state==START_TAG_SEARCH || state==FOUND_END_TAG) 		// write out the rest of the login script
//						write(out,s,NumBytes(s));

				}
				memset(s,0,sizeof(s));
				memset(t,0,sizeof(t));

				chsize(out, lseek(out, 0, SEEK_CUR));  // this truncates the rest of the file

				close(out);
			}
			free(buff);
			buff=NULL;
		}
		if (in)
			close(in);
	}
	else {
		iprintf (LS(IDS_ERROR_OPEN_PROF_LOG));
		LogInstallMessage("Error opening old LDVP profile login script\n");
		return;
	}

	return;
}
/****************************************************************************************/
void DisableAMSDelete(){

	FILE *in=NULL,*out=NULL;
	char s[256]; // DBNR: ,*q;

	dprintf ("Modifying "NW_SYSTEM_DIR sSLASH "uninstal.cfg so that the 4.0 uninstall doesn't delete amsdb.nlm\n");

	in = fopen(NW_SYSTEM_DIR sSLASH "Uninstal.cfg","rt");
	if (in) {
		out = fopen(NW_SYSTEM_DIR sSLASH "uninstal.tmp","wt");

		if (out) {
			memset (s,0,sizeof(s));
			while (fgets(s,sizeof(s),in)) {
				ThreadSwitch();
				if (!strnicmp (s,"AMSDB.NLM=",10))
					fputs ("AMSDB.NLM=VP50\n",out);
				else
					fputs(s,out);						// write out the line
				memset (s,0,sizeof(s));
			}
			fclose(out);
		}
		fclose(in);
		DeletePurgeFile(NW_SYSTEM_DIR sSLASH "Uninstal.bak");
		rename(NW_SYSTEM_DIR sSLASH "Uninstal.cfg",NW_SYSTEM_DIR sSLASH "Uninstal.bak");
		rename(NW_SYSTEM_DIR sSLASH "Uninstal.tmp",NW_SYSTEM_DIR sSLASH "Uninstal.cfg");
	}
	return;
}
/****************************************************************************************/
DWORD CopyWebFile (char *file,char *sdir,char *dir) {
	char
		src[IMAX_PATH],
		dest[IMAX_PATH];
	if (!file) return EI_NULL_PARAMETER;
	dprintf ("CopyWebFile (%s)",file);
	sssnprintf (src,sizeof(src),"%s%s" sSLASH "%s",ProgramDir,sdir,file);
	sssnprintf (dest,sizeof(dest),"%s%s" sSLASH "%s",LDVPWWWRoot,dir,file);
	return ParseCopyLine (dest,src);
}
/****************************************************************************************/
DWORD ParseWebAdminLine(char *line,char *q) {
	dprintf ("ParseWebAdminLine\n");

	if (!line) return EI_NULL_PARAMETER;

	if (!stricmp(line,"HTMLFile")) { // HTMLFiles are in the $HOME$\WebAdmin\Template directory
		return CopyWebFile(q,sSLASH WEBADMIN_PATH sSLASH "Template","");
	}
	else if (!stricmp(line,"ImageFile")) { // ImageFiles are in the $HOME$\WebAdmin\Images
		return CopyWebFile(q,sSLASH WEBADMIN_PATH sSLASH "Images","");
	}
	else if (!stricmp(line,"LDVP_WEB_ROOT")) { 
		StrCopy (LDVPWWWRoot,q);
		dprintf ("LDVP WWW Root=(%s)\n",LDVPWWWRoot);
		return ParseDirLine(q);
	}
	else if (!stricmp(line,"SRM.CFG")) {
		return ConfigureWebAdmin(q);
	}

	LogInstallMessage("I don't know what to do with: (%s=%s)\n",line,q);
	return EI_WEB_ADMIN;
}
/****************************************************************************************/
DWORD GetWWWRoot(char *path) {

	FILE
		*cfg;
	char
		*p,
		buf[256];

	if (WWWRoot[0]) { // if we already know the WWWRoot then just return SUCCESS
		StrCopy (path,WWWRoot);
		return ERROR_SUCCESS;
	}

// this is the file that the Novell Web Server uses to store static information about 
// the web server we are interested in reading the ServerRoot because that is where 
// the SRM.CFG file is and where we will copy the html and supporting files
	cfg=fopen (WEB_CONFIG,"rt"); // WEB_CONFIG is #defined in NLM.H
	if (!cfg) {
		dprintf ("GetWWWRoot: Couldn't open " WEB_CONFIG "\n");
//		BREAK();
		return EI_OPEN_WEB_CFG;
	}

	while (!feof (cfg) && fgets (buf,256,cfg)) {
		if (!strnicmp (buf,"ServerRoot",10)) { // we're just looking for the ServerRoot entry
			p=buf+11;
			StrCopy (path,p);
			while (isspace(path[strlen(path)-1]))
				path[strlen(path)-1]='\0';
//			dprintf ("Found ServerRoot: (%s)\n",path);
//			BREAK();
			fclose (cfg);
			return ERROR_SUCCESS;
		}
	}
	dprintf ("GetWWWRoot: Didn't find ServerRoot line in " WEB_CONFIG "\n");
//	BREAK();
	fclose (cfg);
	return EI_NO_SERVERROOT;
}
/****************************************************************************************/
DWORD WriteLDVPWebSection(FILE *tmp,char *WebAdminPath) {
	
	dprintf ("WRITING: " LOADABLE_MODULE_SECTION " /" LDVP_WEB_ADMIN_CGI " %s\n",WebAdminPath);
	fprintf (tmp,LDVP_WEB_SECTION " Start\n");
	fprintf (tmp,LOADABLE_MODULE_SECTION " /" LDVP_WEB_ADMIN_CGI " %s\n",WebAdminPath);
	fprintf (tmp,"Alias /" LDVP_WEB_ALIAS " %s" sSLASH WEBADMIN_PATH "\n",ProgramDir);
	fprintf (tmp,LDVP_WEB_SECTION " End\n");
	
	return ERROR_SUCCESS;
}
/****************************************************************************************/
DWORD RestartWebServer() {
	
	system ("webstop");
	system ("webstart");
	
	return ERROR_SUCCESS;
}
/****************************************************************************************/
DWORD ConfigureWebAdmin(char *SRMpath) {

	char
		line[1024],
//		*q,
		SRMPath[MAX_PATH],
		SRMFile[MAX_PATH],
		TempFile[MAX_PATH],
		WebAdminPath[MAX_PATH];
	DWORD
		dwRet=ERROR_SUCCESS;
	FILE
		*srm,
		*tmp;
	int
		state=0;
	BOOL
		done=FALSE;

dprintf ("ConfigureWebAdmin\n");
// Setup the path where the webadmin components are so we can copy them to the appropriate places
	memset (WebAdminPath,0,MAX_PATH);
	sssnprintf (WebAdminPath,sizeof(WebAdminPath),"%s" sSLASH WEBADMIN_PATH sSLASH WEBADMIN_NLM,ProgramDir);
	
// if we're installing make sure that we can access the WebAdminPath
	if (Install) {
		dwRet=access (WebAdminPath,0);
		if (dwRet!=0) {
			dprintf ("Couldn't access %s\n",WebAdminPath);
			return EI_ACCESS_WEBADMIN;
		}
	}

// if we haven't found the path for the srm.cfg file find it now
	if (!SRMpath) {
		if (GetWWWRoot(SRMPath)!=ERROR_SUCCESS)
			strcpy (SRMPath,DEFAULT_WWW_ROOT);
		strcat (SRMPath,sSLASH "CONFIG");
		SRMpath=SRMPath;
		dprintf ("SRM path=(%s)\n",SRMpath);
	}
	sssnprintf (TempFile,sizeof(TempFile),"%s" sSLASH "SRM.tmp",SRMpath);
	sssnprintf (SRMFile,sizeof(SRMFile),"%s" sSLASH SRM_FILE,SRMpath);

// Modify the srm.cfg file
	srm=fopen (SRMFile,"rt");
	if (!srm) {
		dprintf ("Couldn't open %s\n",SRMFile);
		return EI_OPEN_SRM;
	}
	tmp=fopen (TempFile,"wt");
	if (tmp) {
		dprintf ("SRM files are opened\n");
		while (fgets (line,1024,srm)) {
//			dprintf ("%s",line);
			if (feof (srm)) break;

			if (state==0 && !strncmp (line,LDVP_WEB_SECTION,strlen(LDVP_WEB_SECTION))) {
				state=1;
				continue;
			}
			
			if (state==1) {
				if (!strncmp (line,LDVP_WEB_SECTION,strlen(LDVP_WEB_SECTION))) 
					state=0;
				continue;
			}
			if (state==0)
				fputs (line,tmp);
		}
		//	write out our section at the end of the srm.cfg file
		if (Install) 
			if (WriteLDVPWebSection(tmp,WebAdminPath)==ERROR_SUCCESS)
				done=TRUE;
				
		fclose (tmp);
	}
	fclose (srm);


	if (done) {
		char SRMBackup [MAX_PATH];
		int i=0;
		do sssnprintf (SRMBackup,sizeof(SRMBackup),"%s" sSLASH "SRM_%03d.bak",SRMpath,i++);
		while (!access (SRMBackup,0) && i<0xfff);
		dprintf ("Backup: %s\n",SRMBackup);
		rename(SRMFile,SRMBackup);
		dwRet=rename(TempFile,SRMFile);
		if (dwRet) {
			dprintf ("Error (%d) renaming files\n",dwRet);
		}
		RestartWebServer();
	}
	return dwRet;
}
/***************************************************************************************
DWORD ParseComponentLine(char *Attr,char *Val)

Adds the name of the component to the registry so that when we uninstall all components
we are sure to catch it.

=======================================================================================*/
DWORD ParseComponentLine(char *Attr,char *Val) {

	HKEY
		cKey;

	if ( !Attr ) return EI_NULL_PARAMETER;

	if (!stricmp (Attr,"NOUNINSTALL")) {
		if(!Install)
			return EI_NO_UNINSTALL;
		return ERROR_SUCCESS;
	}

	if ( !Val ) return EI_NULL_PARAMETER;

	if (!stricmp (Attr,"NAME")) {
		if (RegCreateKey (HKEY_LOCAL_MACHINE,REGHEADER"\\Components",&cKey) == ERROR_SUCCESS) {
			if (Install) {
				iprintf (InitMessage(IDS_INSTALLING_COMPONENT), Val);
				if (CurrentDefFile!=NULL) {
					VTIME fTime;
					char *p=strrchr(CurrentDefFile,cSLASH);
					if(p)
						p++;
					else
						p=CurrentDefFile;
					RegSetValueEx(cKey, Val, 0, REG_SZ, reinterpret_cast<unsigned char*>(p), NumBytes(p) + 1);

					memset( &fTime, 0, sizeof(VTIME) );
					GetFileDate(CurrentDefFile,&fTime);

					RegSetValueEx(cKey,p,0,REG_BINARY,(BYTE*)&fTime,sizeof(VTIME));
				}
				else
					RegSetValueEx(cKey, Val, 0, REG_SZ, reinterpret_cast<unsigned char*>(const_cast<char*>("")), 1);
			}
			else {
				iprintf (InitMessage(IDS_REMOVING_COMPONENT), Val);
				RegDeleteValue (cKey,Val);
			}
		}
	}
	return ERROR_SUCCESS;
}
/****************************************************************************************/
extern "C" CBA_Addr CSAddr[2];
extern "C" int CSprot;

#define RETRY_COPY	5

DWORD ParseUpdateCopyLine(char *line,char *q) {

	DWORD cc;
	static int count=0;
	char *p=strrchr(line,cSLASH);

//	if (rfile) fprintf(rfile,"ParseUpdateCopyLine(%s,%s)\n",line,q);
	count++;
	UpdateUpgradeStatus(STS_COPYING_FILES|(((count*100)/UpdateFiles)<<PERCENT_SHIFT));

//	PutRemoteVal((char *)&CopyAddress,UPGRADE_KEY,CopyStatusValue,count++);
//	if (GetRemoteVal((char *)&CopyAddress,UPGRADE_KEY,CopyCmdValue,0))
//		return ERROR_STOPPED_BY_USER;

	PrintStuff(LS(IDS_FILE), p?p:line );
	dprintf("Remote Copy %s\n",line);

	if( rfile ) fprintf(rfile,"Copying[%d]: {%-12s}", CSprot, p?p+1:"");

	cc = MasterFileCopy(reinterpret_cast<char*>(&CSAddr[CSprot]), SENDCOM_REMOTE_IS_SERVER, q,
                        NULL, 0, line, COPY_SAVE_DATE|COPY_NEW_FILES);

	if(cc==ERROR_COPY_OUT_OF_DATE)
		cc=ERROR_SUCCESS; // because we don't want to copy the file if a newer one exists

	if(cc!=ERROR_SUCCESS) 
	{ // retry it once again just for kicks if we got a failure the first time
		int i;
		rename(line,TEST_FILE);
		PurgeFile(line);
		for( i=RETRY_COPY ; i&&cc!=ERROR_SUCCESS ; i--)
		{
			NTxSleep(100);
			if( rfile ) fprintf(rfile,".");
			cc = MasterFileCopy (reinterpret_cast<char*>(&CSAddr[CSprot]), SENDCOM_REMOTE_IS_SERVER, q,
                                 NULL, 0, line, COPY_SAVE_DATE|COPY_NEW_FILES);
			if(cc==ERROR_COPY_OUT_OF_DATE)
				cc=ERROR_SUCCESS; // because we don't want to copy the file if a newer one exists
		}
		if(cc!=ERROR_SUCCESS)
		{
			if( rfile ) fprintf(rfile,"*");
			if( access(line,0) ) // if the file isn't there, restore the temp backup.
				rename(TEST_FILE,line);
		}
		PurgeFile(TEST_FILE);
	}

	if ( rfile ) fprintf(rfile," (0x%08x) >%s<\n", cc, cc==ERROR_SUCCESS?"SUCCESS!":"Possible problem");

//	if (cc != ERROR_SUCCESS) {
//		eprintf(STS_COPYING_FILES,"Error Copying Updated File: %s",q);
//	}

	return cc;
}
/****************************************************************************************/
int DetermineState (char *line) {

	int
		state=STATE_NONE;
	DWORD
		ccode=0;
	char *q;

	q = StrChar(line,']');
	if (q) {
		q++;
		if (WantedByDef(q) == 0)
			return state;
		}

	if(UpdateProg) UpdateUpgradeStatus( STS_STARTING_SERVICE );

	ThreadSwitchWithDelay();
	if (!stricmp(line,"[REG_REQ]")) {
		RegMode = 0;
		state=STATE_KEYS;
		if(UpdateProg) UpdateUpgradeStatus( STS_UPDATING_REGISTRY );
		NewSection(Install ? InitMessage(IDS_CREATE_DEF_CFG)	: InitMessage(IDS_REMOVING_DB));
	}
	else if (!stricmp(line,"[REG_DEF]")) {
		RegMode = 1;
		state=STATE_KEYS;
		if(UpdateProg) UpdateUpgradeStatus( STS_UPDATING_REGISTRY );
		NewSection(Install ? InitMessage(IDS_CREATE_DEF_CFG)	: InitMessage(IDS_REMOVING_DB));
	}
	else if (!stricmp(line,"[DIRECTORIES]")) {
		state=STATE_DIRECTORIES;
		if(UpdateProg) UpdateUpgradeStatus( STS_COPYING_FILES );
		NewSection(Install ? InitMessage(IDS_CREATE_DIRS)		: InitMessage(IDS_DEL_DIRS));
	}
	else if (!stricmp(line,"[BINDERYOBJECTS]")) {
		state=STATE_BINDERYOBJS;
		NewSection(Install ? InitMessage(IDS_UPDATE_BIND)		: InitMessage(IDS_CLEANUP_BIND));
	}
	else if (!stricmp(line,"[DSOBJECTS]")) {
		if ((ccode = DSPrelims())==ERROR_SUCCESS) {
			state=STATE_DSOBJECTS;
			NewSection(Install ? InitMessage(IDS_UPDATE_DS)		: InitMessage(IDS_CLEANUP_DS));
		}
		else {
			LastError = ccode;
			state = STATE_DSINIT_ERROR;
			DisplayErrorMessage (ccode,LineNum,line,STATE_DSPROFILE);
		}
	}
	else if (!stricmp(line,"[BINDERYLOGIN]")) {
		state=STATE_BINDERYLOGIN;
		NewSection(Install ? InitMessage(IDS_UPDATE_LOGIN)		: InitMessage(IDS_CLEANUP_LOGIN));
	}
	else if (!stricmp(line,"[DSPROFILE]")) {
		if ((ccode = DSPrelims())==ERROR_SUCCESS) {
			state = STATE_DSPROFILE;
			NewSection(Install ? InitMessage(IDS_UPDATE_PROFILE)	: InitMessage(IDS_CLEANUP_PROFILE));
		}
		else {
			LastError = ccode;
			state = STATE_DSINIT_ERROR;
			DisplayErrorMessage (ccode,LineNum,line,STATE_DSPROFILE);
		}
	}
	else if (!stricmp(line,"[COPYFILES]")) {
		state=STATE_COPYFILES;
		if(UpdateProg) UpdateUpgradeStatus( STS_COPYING_FILES );
		NewSection(Install ? InitMessage(IDS_COPY_FILES)		: InitMessage(IDS_DEL_FILES));
	}
	else if (!stricmp(line,"[CopyDirectories]")) {
		state=STATE_COPYDIRS;
		if(UpdateProg) UpdateUpgradeStatus( STS_COPYING_FILES );
		NewSection(Install ? InitMessage(IDS_COPY_DIR)	: InitMessage(IDS_DEL_DIR));
	}
	else if (!stricmp(line,"[AUTOEXEC.NCF]")) {
		state=STATE_AUTOEXECNCF;
		NewSection(Install ? InitMessage(IDS_UPDATE_AUTOEXEC)	: InitMessage(IDS_CLEANUP_AUTOEXEC));
	}
	else if (!stricmp(line,"[VPSTART]")) {
		state=STATE_VPSTART;
		NewSection(Install ? InitMessage(IDS_CREATE_VPSTART)	: InitMessage(IDS_DELETE_VPSTART));
	}
	else if (!stricmp(line,"[WEBADMIN]")) {
		state=STATE_WEBADMIN;
		NewSection(Install ? InitMessage(IDS_WEB_INSTALL)	: InitMessage(IDS_WEB_REMOVE));
	}
	else if (!stricmp(line,"[CopyUpdateFiles]")) {
		if( Install==I_PROGRAM_UPDATE ) { // only do this on an update
			state=STATE_UPDATECOPY;
			NewSection (InitMessage(IDS_GET_REMOTE_FILES));
		}
	}
	else if (!stricmp(line,"[REPLACEVARIABLES]")) {
		if (Install) {
			state=STATE_REPLACEVARS;
			NewSection(InitMessage(IDS_REPLACE_VARS));
		}
	}
	else if (!stricmp(line,"[COPYREQUIREDFILES]")) {
		if(UpdateProg) UpdateUpgradeStatus( STS_COPYING_FILES );
		if (Install) {
			state=STATE_COPYREQFILES;
			NewSection(InitMessage(IDS_COPY_REQUIRED));
		}
	}
	else if (!stricmp(line,"[RUN]")) {
		if (Install) {
			state=STATE_RUN;
			NewSection(InitMessage(IDS_RUNNING_PROGS));
		}
	}
	else if (!stricmp(line,"[COMPONENT]")) {
		state=STATE_COMPONENT;
	}
	else if (!stricmp(line,"[DeleteFiles]")) {
		if(!Install)
			state=STATE_DELETE_FILES;
	}
	else if (!stricmp(line,"[CopyOnInstallOnly]")) {
		if(UpdateProg) UpdateUpgradeStatus( STS_COPYING_FILES );
		if (Install) {
			state=STATE_COPYFILES;
			NewSection(InitMessage(IDS_COPY_FILES));
		}
	}

	return state;
}
/****************************************************************************************
DWORD ActOnState (int state,char *lVal,char *rVal,HKEY *k)

	Calls the correct function based on the current state

	state (in) the current state
	lVal (in) the string portion on the left side of the = or the full line if there was no =
	rVal (in) the string portion on the right side of the = or NULL is there was no =
	k (in/out) for STATE_KEYS to keep track of which key we are working with
=======================================================================================*/
DWORD ActOnState (int state,char *lVal,char *rVal,HKEY *k) {

	DWORD
		dwRet=ERROR_SUCCESS;
	static int cnt;

	cnt ++;
	switch (state) {
	case STATE_KEYS:			dwRet=ParseRegLine(lVal,rVal,k);		break;
	case STATE_DIRECTORIES:		dwRet=ParseDirLine(lVal);				break;
	case STATE_REPLACEVARS:		dwRet=ReplaceFileVariables(lVal);		break;
	case STATE_BINDERYLOGIN:	dwRet=ParseBinderyLogin(lVal,rVal);		break;
	case STATE_DSPROFILE:		dwRet=ParseProfileLogin(lVal,rVal);		break;
	case STATE_AUTOEXECNCF:		dwRet=ParseAutoexecNCF(lVal,rVal);		break;
	case STATE_RUN:				dwRet=ParseRunLine(lVal);		 		break;
	case STATE_VPSTART:			dwRet=ParseVPstartNcf(lVal,rVal);		break;
	case STATE_WEBADMIN:		dwRet=ParseWebAdminLine(lVal,rVal);		break;
	case STATE_BINDERYOBJS:		dwRet=ParseObjLine(lVal,rVal,0);		break;
	case STATE_DSOBJECTS:		dwRet=ParseObjLine(lVal,rVal,1);		break;
	case STATE_COPYREQFILES:	dwRet=ParseRequiredCopyLine(lVal,rVal);	break;
	case STATE_COPYFILES:		dwRet=ParseCopyLine(lVal,rVal);			break;
	case STATE_COPYDIRS:		dwRet=ParseCopyDirLine(lVal,rVal);		break;
	case STATE_COMPONENT: 		dwRet=ParseComponentLine(lVal,rVal);	break;
	case STATE_UPDATECOPY:		dwRet=ParseUpdateCopyLine(lVal,rVal);	break;
	case STATE_DELETE_FILES:	PurgeFile(lVal);						break;
	case STATE_NONE: break;
	}
// these lines are for testing error reporting:
//	if(!(cnt%50) && state!=STATE_KEYS && state!=STATE_NONE )
//		dwRet=ERROR_COMPUTER_NOT_FOUND + cnt/50;
	return dwRet;
}
/***************************************************************************************
UninstallAllComponents

	enumerates the values in the Components subkey and parses the associated def file
=======================================================================================*/
DWORD UninstallAllComponents(int verify) {

	HKEY
		cKey;
	int
		i;
	DWORD
		type,
		cc=-1,
		vSize=IMAX_PATH,
		dSize=IMAX_PATH;
	char
		ValName[IMAX_PATH],
		DefFileName[IMAX_PATH];

	if (rfile) 	fprintf (rfile,"UninstallAllComponents\n");

	if (RegOpenKey (HKEY_LOCAL_MACHINE,REGHEADER"\\Components",&cKey) == ERROR_SUCCESS) {
		for (i=0 ; RegEnumValue (cKey,i,ValName,&vSize,NULL,&type,reinterpret_cast<unsigned char*>(DefFileName),&dSize)==ERROR_SUCCESS ; i++) {

dprintf ("****Parsing Alternate DEF File: %s\n",DefFileName);
			if( access(DefFileName,0)==ERROR_SUCCESS ) {

				if (rfile) 	fprintf (rfile,"\n****Parsing Alternate DEF File: %s\n",DefFileName);

				cc=ParseDefinitionFile (DefFileName,verify);

				if (cc==EI_ABORT_INSTALL) break;
			}
			RegDeleteValue(cKey,ValName);
			RegDeleteValue(cKey,DefFileName);
		}
		RegCloseKey(cKey);
	}
	return cc;
}
/***************************************************************************************/
// this hasn't been tested at all but it compiles
DWORD UninstallComponent (char *component) { 

	HKEY
		cKey;
	int
		cc;
	DWORD
		size,
		type;
	char
		DefFileName[IMAX_PATH];

	cc=PrepareInstallationUI(100);
	if (rfile) 	fprintf (rfile,"UninstallComponent\n");
	
	if (RegOpenKey (HKEY_LOCAL_MACHINE,REGHEADER"\\Components",&cKey)==ERROR_SUCCESS) {
		if (SymSaferRegQueryValueEx (cKey,component,NULL,&type,reinterpret_cast<unsigned char*>(DefFileName),&size)==ERROR_SUCCESS) {
			cc=ParseDefinitionFile (DefFileName,100);
			RegDeleteValue(cKey,component);
			RegDeleteValue(cKey,DefFileName);
		}
		RegCloseKey (cKey);
	}
	cc=CloseInstallationUI(cc);
	return cc;
}
/***************************************************************************************
PrepareInstallationUI

	(in) verify - basically 0 to install 100 to uninstall 1 to install only the [KEYS] section

	Using PrepareInstallationUI and CloseInstallationUI specific def files can be parsed
======================================================================================*/
DWORD PrepareInstallationUI (int verify) {

	ConsoleHandle=CreateScreen("System Console",0);

	ClearInstArrays(TRUE);

	Install = verify<10?verify:FALSE;

	if(Install)
		rfile = fopen( INSTALL_LOG, "at" );
	else
		rfile = fopen( "SYS:\\"INSTALL_LOG, "wt" );

	if (rfile) {
		time_t t;
		time(&t);
		fprintf (rfile,	"\n**************************************************\n"
						"Parsing Def Files: (%d)[%d] at %s",verify,Install,ctime(&t));
	}

	ThreadSwitchWithDelay();
	Installing=TRUE;

	return ERROR_SUCCESS;
}
/***************************************************************************************/
int CloseInstallationUI(int dwRet) {

	dprintf ("CloseInstallation UI\n");
	if (Install) RegSaveDatabase();

//	if (AbortInstall(ABORT_KEY)) dwRet=EI_ABORT_INSTALL;

	ThreadSwitchWithDelay();

	if (DSNew==0) DeInitDS();
	else if (Authenticated && _NWDSLogout) _NWDSLogout(hContext);

	if (rfile) {
		fprintf (rfile,"CloseInstallationUI\n**************************************\n\n");

		fclose(rfile);
	}

	rfile=NULL;
	ThreadSwitchWithDelay();

	Installing=FALSE;

	return dwRet;
}
/**************************************************************************************/
int NextInf(BOOL reset)
{
	static int i=0;

	if(reset) {
		i=0;
		return -1;
	}

	if(Install)
		return i<MAX_INF ? i++ : -1;

	i++;
	return MAX_INF-i;
}
/***************************************************************************************/
DWORD ParseDefaultFile(int verify) {

	int	err=ERROR_SUCCESS;
	DWORD	cc=0;
	BOOL Abort=FALSE;
	char *CurrentInf=NULL;
	char szTempPath[IMAX_PATH]; //EA 09/18/2000 Fix for STS 345868
	int i;
	char szDelDirPath[IMAX_PATH];//EA 04/27 Added code to support msi install
	DelCurr=NULL;
	DelHead=NULL;

	dprintf ("*ParseDefaultFile*\n");

	PrepareInstallationUI(verify);

	DisablePreviousInstallation(DISABLE_AMS_DELETE);

	if (rfile) 	fprintf (rfile,"LoadVars\n");

	CurrentInf=(char*)malloc(256);
	
	// --------------------------------------
	// ksr - NetWare Certification, 8/30/2002
	if( !CurrentInf )
	{
		LogInstallMessage( "Out of memory in ParseDefaultFile() - CurrentInf\n" );
		
		if( rfile ) 	
			fprintf( rfile, "\nOut of memory in ParseDefaultFile() - CurrentInf" );
		Abort = TRUE;
		cc = ERROR_MEMORY; 		
	}
	else	//	if(CurrentInf)
	// --------------------------------------

		if(strlen(HomeDir))
			sssnprintf(CurrentInf,256,"%s" sSLASH "vpdata.var",HomeDir);
		else
			sssnprintf(CurrentInf,256,"vpdata.var");

	// --------------------------------------
	// ksr - NetWare Certification, 8/30/2002
		
	// LoadVars(CurrentInf);
	if( LoadVars( CurrentInf ) == ERROR_MEMORY )
	{
		LogInstallMessage( "Out of memory in ParseDefaultFile() - LoadVars() \n" );
		
		if( rfile ) 	
			fprintf( rfile, "\nOut of memory in ParseDefaultFile() - LoadVars()" );
			
		Abort = TRUE;
		cc = ERROR_MEMORY;		
	}
	
	// --------------------------------------

// we need to add the netware version to the define list as NWx where x is the major version (NW3, NW4, NW5...)
	for (i=0;i<MAX_DEF_LIST;i++) 
	{
		if (DefineList[i] == NULL) // find an empty slot in the define list
		{
			DefineList[i] = (char*)malloc(6); 
			if (DefineList[i])
				sssnprintf(DefineList[i],6,"NW%d",g_fsinf.netwareVersion);
	
			// --------------------------------------
			// ksr - NetWare Certification, 8/30/2002
			else
			{
				LogInstallMessage( "Out of memory in ParseDefaultFile() - DefineList\n" );
					
				if( rfile ) 	
					fprintf( rfile, "\nOut of memory in ParseDefaultFile() - DefineList" );
				
				Abort = TRUE;				
				return ERROR_MEMORY;		
			}
			// --------------------------------------

			break;
		}
	}

//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
	if(CheckCopyServerCommand()==EI_ABORT_INSTALL)
	{
		iprintf(LS(IDS_EI_ABORT_INSTALL));    // gdf sts 337273 03/07/01
		
		// --------------------------------------
		// ksr - NetWare Certification, 8/30/2002
		
		if( rfile ) 	
			fprintf( rfile, "\nCheckCopyServerCommand() - failed\n" );
		return EI_ABORT_INSTALL;
			
		// --------------------------------------
	}
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

// if we are doing a DS install then we want to make sure the correct DSAPI nlm is on the
// server.  if not then we don't want to do any sort of an install because it will cause 
// only a partial install to be done.  so here we will try to load the DS functions and
// if we don't have the correct version it will return an error.
	if(Install==I_NORMAL_INSTALL) {
		for (i=0;i<MAX_DEF_LIST;i++) {	
			if(DefineList[i] && !strcmp(DefineList[i],"NDS")) {
				cc=LoadNWDSFuncs();
				if(cc != ERROR_SUCCESS)	{
					iprintf(LS(IDS_EI_ABORT_NWDS), cc);     // gdf sts 337273 03/07/01
					_printf ("%s\n",LS(IDS_OLD_DSINIT));
					Abort=TRUE;
				}
				break;
			}
		}
	}

	if(CurrentInf)free(CurrentInf);
		
	for(i=0 ; i<MAX_INF ; i++ ) {
		if(DoInf[i]) {
			dprintf("Do %d=%s\n",i,DoInf[i]);
		}
		if(UpdateInf[i]) {
			dprintf("Update %d=%s\n",i,UpdateInf[i]);
		}
	}
	if (rfile) 	fprintf (rfile,"Get Copy Server Address\n");
	for (i=0;i<MAX_VAR_LIST;i++) {
		if (VarList[i]) {
			if(!strcmp(VarList[i]->name,"COPYSERVERADDRESS")) {
				if (rfile) 	fprintf (rfile,"%s\n",VarList[i]->var);
				CvtStrToBuf((PBYTE)&CopyAddress,VarList[i]->var);
				break;
			}
		}
	}

	sssnprintf(CopyStatusValue,sizeof(CopyStatusValue),"%s_Status",FileServerName);
	sssnprintf(CopyErrorValue,sizeof(CopyErrorValue),"%s_LastError",FileServerName);
	sssnprintf(CopyCmdValue,sizeof(CopyCmdValue),"%s_Command",FileServerName);

//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
	if(CheckCopyServerCommand()==EI_ABORT_INSTALL)
		{
			iprintf(LS(IDS_EI_ABORT_INSTALLATION));     // gdf sts 337273 03/07/01
			Abort=TRUE;cc=EI_ABORT_INSTALL;
		}
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/


	NextInf(1);// reset the inf file counter

	if (rfile) 	fprintf (rfile,"Start Parsing INF Files...\n");
	while( (i=NextInf(0)) !=-1 && !Abort) {

		if(verify==I_PROGRAM_UPDATE)
			CurrentInf=UpdateInf[i];
		else
			CurrentInf=DoInf[i];

		if (rfile) 	fprintf (rfile,"CurrentInf-%d:(%s)\n",i,CurrentInf?CurrentInf:"<empty>");

		if (CurrentInf) {
			char InfPath[IMAX_PATH];
			if(strlen(HomeDir))
				sssnprintf(InfPath,sizeof(InfPath),"%s" sSLASH "%s",HomeDir,CurrentInf);
			else
				sssnprintf(InfPath,sizeof(InfPath),"%s",CurrentInf);

			if(access(InfPath,0)!=ERROR_SUCCESS) {
				if (rfile) 	fprintf (rfile,"\nInf File %s could not be found\n",InfPath);
				dprintf ("Inf File %s could not be found\n",InfPath);
				continue;
			}
		
			if (rfile) 	fprintf (rfile,"\n=======================================\nParsing DEF File: %s\n",InfPath);
			dprintf ("****Parsing DEF File: %s\n",InfPath);
			cc=ParseDefinitionFile (InfPath,verify);
			if (cc)
				LogInstallMessage("Error 0x%x (%d) parsing definition file\n",cc,cc);

			if (cc==EI_ABORT_INSTALL) Abort=TRUE;
			if ( ( cc&0xf0000000 )==EI_DSINIT )
			{
				iprintf(LS(IDS_EI_DSINIT));     // gdf sts 337273 03/07/01
				Abort=TRUE;
			}
			if (AbortInstall(ABORT_KEY))
			{     // gdf sts 337273 03/07/01
				iprintf(LS(IDS_EI_ABORT_KEY));     // gdf sts 337273 03/07/01
				Abort=TRUE;
			}     // gdf sts 337273 03/07/01
			if (rfile) 	fprintf (rfile,"=======================================\n");
		}
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
		if(CheckCopyServerCommand()==EI_ABORT_INSTALL)			
			{
				iprintf(LS(IDS_EI_ABORT_INSTALLATION));     // gdf sts 337273 03/07/01
				Abort=TRUE;cc=EI_ABORT_INSTALL;
			}
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
	}

	if (rfile)
    {
        fprintf (rfile,"Done Parsing INF Files\n");
    }

	if( Abort )
    {
		iprintf(LS(IDS_INST_ABORT)); // gdf sts 337273 03/07/01
		if (rfile) 	fprintf (rfile,"Installation aborted: 0x%x (%d)\n",cc,cc);
	}
	else
    {
		cc=ERROR_SUCCESS;

	    if (!Install)
        {
		    UninstallAllComponents (verify);
	    }

	    if (Install)
        {
		    HKEY cKey=0;
		    AddSomeKeys();
		    if (RegCreateKey (HKEY_LOCAL_MACHINE,REGHEADER"\\Components",&cKey) == ERROR_SUCCESS)
            {
			    VTIME fTime;
			    memset( &fTime, 0, sizeof(VTIME) );
			    //EA 09/18/2000 Start of Fix for STS 345868
			    if(strlen(HomeDir)) //EA 09/18/2000
					sssnprintf(szTempPath,sizeof(szTempPath),"%s" sSLASH "vpdata.var",HomeDir); //EA 09/18/2000
			    else //EA 09/18/2000
				    sssnprintf(szTempPath,sizeof(szTempPath),"vpdata.var"); //EA 09/18/2000
			    //GetFileDate("VPDATA.VAR",&fTime); //EA 09/18/2000
			    GetFileDate(szTempPath,&fTime); //EA 09/18/2000
			    //EA 09/18/2000 End of Fix for STS 345868
			    RegSetValueEx(cKey,"VPDATA.VAR",0,REG_BINARY,(BYTE*)&fTime,sizeof(VTIME));
		    }
	    }

	    //IBARILE 1/14/03 porting CRT fix Siebel Defect #1-AYUCW
	    // if this is a new install, and a Parent server is defined, set the InheritGroupSettings
 	    // key to tell RTVScan to pull and incorporate GRCSrv.dat from the parent server
 	    if(verify==I_NORMAL_INSTALL && !UpdateProg)
        {
 		    HKEY cKey=0;
 		    DWORD dwSize = 0;
		    WORD dwInherit = 1;
		    char ParentName[IMAX_PATH];

		    dwSize=sizeof(ParentName);
		    if ( RegOpenKey(HKEY_LOCAL_MACHINE, REGHEADER, &cKey)==ERROR_SUCCESS ) {
			    if (SymSaferRegQueryValueEx(cKey,"Parent",0,NULL,reinterpret_cast<unsigned char*>(ParentName),&dwSize)==ERROR_SUCCESS) {
				    if(strlen(ParentName))
					    RegSetValueEx(cKey,"InheritGroupSettings",0,REG_DWORD,(BYTE*)&dwInherit,sizeof(DWORD));
			    }
			    RegCloseKey (cKey);
		    }
	    }
	    //IBARILE 1/14/03 end porting CRT fix

        // If this is a new install, insert the One-Time Update values to allow
        // a license enforcing build to update once before throwing up license errors.

        if( Install || UpdateProg )
        {
			BOOL ret;

			if( rfile ) fprintf( rfile, "Setting One-Time Update flag\n" );

            ret = License_SetOneTimeUpdateMode( TRUE );

			if( rfile ) fprintf( rfile, "Setting One-Time Update flag: %d\n", ret );
        }
		else
		{
			fprintf( rfile, "Setting One-Time Update flag skipped\n" );
		}
    }

	dprintf ("Done Parsing Def Files\n");

	err=CloseInstallationUI(cc);

	if (DSNew==0)
    {
        DeInitDS();
    }

	if (err<0)
    {
        _printf ("Installation Aborted\n");
    }
	else if (Install)
	{
		_printf (LS(IDS_INSTALL_COMPLETE));
		//EA 04/27/2000 Added code to support msi install where windows client dir have become win32
		//hence deleting winnt and win95 directories if they exist after the install is successfull
		memset(szDelDirPath,0,IMAX_PATH);
		sssnprintf (szDelDirPath,sizeof(szDelDirPath),"%s" sSLASH "clt-inst" sSLASH "winnt",ProgramDir);
		DeleteTree(szDelDirPath);
		memset(szDelDirPath,0,IMAX_PATH);
		sssnprintf (szDelDirPath,sizeof(szDelDirPath),"%s" sSLASH "clt-inst" sSLASH "win95",ProgramDir);
		DeleteTree(szDelDirPath);
		//EA 04/27/2000 end of code to support msi install of change in windows client dir
	}

	// --------------------------------------
	// ksr - NetWare Certification, 8/30/2002
	
	if( rfile ) 	
		fprintf( rfile, "\nAbout to FreeLists()" );
		
	// --------------------------------------

	FreeLists();

	ThreadSwitchWithDelay();

	dprintf ("returning from ParseDefaultFile\n");

	return err;
}
/****************************************************************************************
DWORD ParseDefinitionFile (char *DefFile,int verify)

	DefFile (in) name of a VPRegFileMask.DEF File to parse
	Verify (in) indicates type of parsing to perform (install, remove, restore registry, etc.)

	returns ERROR_SUCCESS if file was successfully parsed
		or an error value if a serious error was encountered

=======================================================================================*/
DWORD ParseDefinitionFile (char *DefFile,int verify) {

	char
		oline[520],
		str[520];
	char
		*line=NULL,
		*q=NULL;
	struct stat
		stats;
	HKEY
		hkey=0;
	int
		i,
		realstate=STATE_NONE,
		state=STATE_NONE;
	int
		file=INVALID_FILE_HANDLE_VALUE;
	DWORD
		dwRet=ERROR_SUCCESS;

//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
	if(CheckCopyServerCommand()==EI_ABORT_INSTALL)
		return EI_ABORT_INSTALL;
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

// make sure we can open the def file
	file=open (DefFile,O_RDONLY|O_TEXT);
	if (file==-1/*INVALID_FILE_HANDLE*/) {
		FILE *fp = fopen( DefFile, "rt" );
		if(!fp) {
			dprintf ("Couldn't Open (%s)\n",DefFile);
			if (rfile) fprintf(rfile,"Couldn't Open (%s)(%d)(%d)\n",DefFile,errno,NetWareErrno);
			return EI_OPEN_DEF_FILE;
		}
		file=fileno(fp);
	}

	memset (oline,0,sizeof(oline));
	memset (str,0,sizeof(str));
	memset (&stats,0,sizeof (struct stat));

	CurrentDefFile=DefFile;

// initialize the global variables
	ClearInstArrays(FALSE);

//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
	if(CheckCopyServerCommand()==EI_ABORT_INSTALL)
		return EI_ABORT_INSTALL;
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

// read the entire def file into fbuf
	lseek(file,SEEK_SET,0);
	fstat (file,&stats);
	fsize=stats.st_size;
	fbuf=(char*)malloc (fsize+10);
	if (!fbuf)  {
		close(file);
		_printf ("%s\n",GetErrorText (ERROR_MEMORY));
		dprintf ("Couldn't allocate memory for default file buffer\n");
		if (rfile)
			fclose (rfile);
		return EI_MEMORY;
	}
	memset (fbuf,0,fsize+10);
	fpos=fbuf;
	i=read (file,fbuf,fsize);
	close(file);

	if(!Install)
		DeletePurgeFile(DefFile);

	dprintf ("size:%d\tread:%d\n",fsize,i);
	iprintf ("%s: %s\n",LS(IDS_PARSE_INST_FILE),DefFile);

// loop through each line of the def file
	for (	LineNum=0 , memset (str,0,sizeof(str)) ;
			state!=STATE_ABORT_INSTALL && ReturnNextLine(str,sizeof(str)) ;
			memset (str,0,sizeof(str))) {
		if (AbortInstall(ABORT_KEY)) {dwRet=EI_ABORT_INSTALL;break;}

//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
		if(CheckCopyServerCommand()==EI_ABORT_INSTALL)
			{dwRet=EI_ABORT_INSTALL;break;}
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

		ThreadSwitchWithDelay();
//		LineNum++;
		dwRet = 0;

		line=PrepareLine(str);
		if (line==NULL)
			continue;

		StrCopy(oline,line);

		q = StrChar(line,'=');
		if (!q) { // ie. if there is no '=' in the line
			if (line[0]=='['){
				realstate=state=STATE_NONE;
				if (cfPCB) {// destroy the copy files portal if we don't need it any more
					NWSDestroyPortal (cfPort,InstNut); 
					cfPCB=NULL;
					cfPort=0;
				}
				switch(verify) {
				case I_NORMAL_INSTALL:// normal install or remove
				case I_NORMAL_REMOVE:
					realstate=state=DetermineState(line);
					break;
				case I_REG_REPAIR:			// if all we are doing is re-reading the registry keys
					if(!stricmp(line,"[REG_REQ]")) {
						realstate=state=STATE_KEYS;
						iprintf(Install ? InitMessage(IDS_CREATE_DEF_CFG)	: InitMessage(IDS_REMOVING_DB));
					}
					if(!stricmp(line,"[COMPONENT]")) {
						realstate=state=STATE_COMPONENT;
					}
					break;
				case I_PROGRAM_UPDATE:			// if we are doing an update
					realstate=state=DetermineState(line);
					switch(state) {
					case STATE_UPDATECOPY: // add whatever states we want the update to do here
						UpdateUpgradeStatus(STS_COPYING_FILES);
						CountUpdateFiles();						
						break;
					default:
						state=STATE_NONE;
					}
					break;
				}
			}
			if (!stricmp(line,"!END!")) {	// if we are in one of the sections with and !END! tag at the end
				dwRet=ActOnState(state,NULL,NULL,NULL);
				state=STATE_NONE;
			}
			else if (line[0] != '[')
				dwRet=ActOnState(state,line,NULL,&hkey);
		}
		else { // q is not NULL->it has an '=' in it
			char *w; //otherwise we'll get a null string back from NWNextChar
//			*q = 0;
			w=q;
			q=NextChar(q);	  //MLR FIXED
			*w=0;
			dwRet=ActOnState(state,line,q,&hkey);
		}
		if ( dwRet != ERROR_SUCCESS ) { // dwRet should be 0 unless there was some sort of error
			if ( dwRet==EI_MEMORY ) {
				dwRet=EI_MEMORY;
				_printf ("%s\n",GetErrorText (ERROR_MEMORY));
				state=STATE_ABORT_INSTALL;
			}
			if( dwRet==EI_NO_UNINSTALL ) {
				dwRet=ERROR_SUCCESS;
				if( Install ) {
					continue;
				}
				else {
					if (rfile) fprintf(rfile,"Don't uninstall this component\n");
//					state=STATE_ABORT_INSTALL;
					break;
				}
			}
			if ( ( dwRet&0xf0000000 )==EI_DSINIT ) {
				_printf ("%s\n",LS(IDS_OLD_DSINIT));
				state=STATE_ABORT_INSTALL;
			}
			DisplayErrorMessage (dwRet,LineNum,oline,realstate);
			dwRet=ERROR_SUCCESS;
		}

		if ( state==STATE_DSINIT_ERROR ) {
			if ( LastError==EI_NOT_AUTHENTICATED ) { // if they cancelled the authentication then just skip this section
				state=STATE_NONE;
				LastError=0;
			}
			else {
				dwRet=EI_DSINIT;
				_printf ("%s\n",LS(IDS_OLD_DSINIT));
				state=STATE_ABORT_INSTALL;
			}
		}			
	}
	CurrentDefFile=NULL;

// clean up memory
	if ( AutoBuff )	{	free ( AutoBuff );		AutoBuff=NULL;	}
	if ( VPBuff )	{	free ( VPBuff );		VPBuff=NULL;	}
	if ( fbuf )		{	free ( fbuf );			fbuf=NULL;		}
	if ( hkey )		{	RegCloseKey ( hkey );	hkey=NULL;		}

	return dwRet;
}
/*************************************************************************************/
char *StateStr(int state, char *buf)
{
	switch (state) {
	case STATE_KEYS:			sprintf(buf,"creating default configuration (%d)",state);	break;
	case STATE_DIRECTORIES:		sprintf(buf,"creating directories (%d)",state);	break;
	case STATE_REPLACEVARS:		sprintf(buf,"ReplaceFileVariables (%d)",state);	break;
	case STATE_BINDERYLOGIN:	sprintf(buf,"modifying bindery login scripts (%d)",state);	break;
	case STATE_DSPROFILE:		sprintf(buf,"modifying profile login scripts (%d)",state);	break;
	case STATE_AUTOEXECNCF:		sprintf(buf,"modifying autoexec.ncf (%d)",state);	break;
	case STATE_RUN:				sprintf(buf,"starting other NLMs(%d)",state);	break;
	case STATE_VPSTART:			sprintf(buf,"creating vpstart.ncf(%d)",state);	break;
	case STATE_WEBADMIN:		sprintf(buf,"ParseWebAdminLine (%d)",state);	break;
	case STATE_BINDERYOBJS:		sprintf(buf,"creating bindery objects (%d)",state);	break;
	case STATE_DSOBJECTS:		sprintf(buf,"creating directory services objects (%d)",state);	break;
	case STATE_COPYREQFILES:	sprintf(buf,"copying required files (%d)",state);	break;
	case STATE_COPYFILES:		sprintf(buf,"copying files (%d)",state);	break;
	case STATE_COPYDIRS:		sprintf(buf,"copying directories (%d)",state);	break;
	case STATE_COMPONENT: 		sprintf(buf,"ParseComponentLine (%d)",state);	break;
	case STATE_UPDATECOPY:		sprintf(buf,"copying updated files (%d)",state);	break;
	case STATE_DELETE_FILES:	sprintf(buf,"deleting files (%d)",state);	break;
	case STATE_NONE:			sprintf(buf,"STATE_NONE (%d)",state);	break;
	default:					sprintf(buf,"UNKNOWN (%d)",state);	break;
	}
	return buf;
}
/*************************************************************************************/
int DisplayErrorMessage (DWORD Err,int LineNum,char *oline,int state) {

	DWORD errMsg;

	if (Err==EI_NO_UNINSTALL) // I don't think this is a valid error so we'll never see it...
	{
		iprintf (InitMessage(IDS_EI_NO_UNINSTALL));
		return state;
	}

	if (rfile) fprintf(rfile,"*****ERROR<%d>(0x02x):",ErrorCount+1,Err);

	if ((Err&0xf0000000)==EI_DSINIT)
	{
		iprintf (InitMessage (IDS_DSINIT_ERROR),Err,Err,LineNum,oline);
		eprintf(Err,"(0x%08x) Directory Services Initialization Error in %s: line %d", Err, CurrentDefFile, LineNum );
	}
	else if ((Err&0xf0000000)==EI_DSRET)
	{
		iprintf (InitMessage (IDS_DS_ERROR),Err,Err,LineNum,oline);
		eprintf(Err,"(0x%08x) Bindery or Directory Services Error in %s: line %d", Err, CurrentDefFile, LineNum );
	}
	else {
		errMsg=0;
		switch (Err) {
			case EI_NO_TRUSTEE_DIR:		errMsg = IDS_EI_NO_TRUSTEE_DIR;		break;
			case EI_NO_CONTAINER:		errMsg = IDS_EI_NO_CONTAINER;		break;
			case EI_DS_NO_INIT:			errMsg = IDS_EI_DS_NO_INIT;			break;
			case EI_BINDERY_LOGIN:		errMsg = IDS_EI_BINDERY_LOGIN;		break;
			case EI_PROFILE_LOGIN:		errMsg = IDS_EI_PROFILE_LOGIN;		break;
			case EI_OPEN_AUTOEXEC:		errMsg = IDS_EI_OPEN_AUTOEXEC;		break;
			case EI_ADDING_AUTOEXEC:	errMsg = IDS_EI_ADDING_AUTOEXEC;	break;
			case EI_MODIFY_AUTOEXEC:	errMsg = IDS_EI_MODIFY_AUTOEXEC;	break;
			case EI_OPEN_VPSTART:		errMsg = IDS_EI_OPEN_VPSTART;		break;
			case EI_ALLOC_BUF:			errMsg = IDS_EI_ALLOC_BUF;			break;
			case EI_OPEN_REGHEADER:		errMsg = IDS_EI_OPEN_REGHEADER;		break;
			case EI_GET_CONTEXT:		errMsg = IDS_EI_GET_CONTEXT;		break;
			case EI_NOT_AUTHENTICATED:	errMsg = IDS_EI_NOT_AUTHENTICATED;	break;
			case EI_WEB_ADMIN:			errMsg = IDS_EI_WEB_ADMIN;			break;
			case EI_DISK_WRITE:			errMsg = IDS_EI_DISK_WRITE;			break;
			case EI_UNLOAD_OLD_NLM:		errMsg = IDS_EI_UNLOAD_OLD_NLM;		break;
			case EI_OPEN_WEB_CFG:		errMsg = IDS_EI_OPEN_WEB_CFG;		break;
			case EI_ACCESS_WEBADMIN:	errMsg = IDS_EI_ACCESS_WEBADMIN;	break;
			case EI_OPEN_SRM:			errMsg = IDS_EI_OPEN_SRM;			break;
			case EI_NO_SERVERROOT:		errMsg = IDS_EI_NO_SERVERROOT;		break;
			case EI_MEMORY:				errMsg = IDS_EI_MEMORY;				break;
			case EI_NULL_PARAMETER:		errMsg = IDS_EI_NULL_PARAMETER;		break;
			case EI_OPEN_DEF_FILE:		errMsg = IDS_EI_OPEN_DEF_FILE;		break;
			case EI_ABORT_INSTALL:		errMsg = IDS_EI_ABORT_INSTALL;		break;
			case EI_CREATE_CONTEXT:		errMsg = IDS_EI_CREATE_CONTEXT;		break;
			case EI_LOAD_DS_NLM:		errMsg = IDS_EI_LOAD_DS_NLM;		break;
		}
		if (errMsg) 
		{
			iprintf (InitMessage(IDS_INSTALL_ERR_MSG),InitMessage(errMsg),LineNum,oline);
			eprintf(Err,"(0x%08x) Error: %s in %s: line %d", Err, InitMessage(errMsg), CurrentDefFile, LineNum );
		}
		else
		{
			iprintf (InitMessage(IDS_INSTALL_ERROR),Err,GetErrorText(Err),LineNum,oline);
			eprintf(Err,"(0x%08x) Error in %s: line %d", Err, CurrentDefFile, LineNum );
		}
	}

	return state;
}
/*************************************************************************************/

