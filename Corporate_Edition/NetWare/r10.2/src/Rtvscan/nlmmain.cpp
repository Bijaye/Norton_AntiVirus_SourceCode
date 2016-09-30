// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#define IncDebugParser
#include "PSCAN.H"
#include "Status.h"
#include "slash.h"
#include "SymSaferStrings.h"

#ifndef LINUX
#include "VEFuncs.h"
#endif

#ifdef NLM
extern "C"
{
#include "nwdir.h"
}
#include "nlm/nit/nwmsg.h"
#include "nlm/nwfinfo.h"
#include "nwfileio.h"
#include "nlm/nwnspace.h"
#include "nwextatt.h"

#endif // NLM

#ifdef LINUX

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
extern void StartConsoleScreen();

// The NDK function EnterCritSec() will suspend all other threads within and NLM and then
// let the current thread execute on its own. ExitCritSec() resumes the other threads.
// As the Linux port will run in user mode, there is no need for such drastic
// measures. Instead, we will use the static mutex to allow one thread at a time to
// execute within the critical section. Threads executing other parts will continue to
// execute. <bertil Wed Mar 17  5.42 pm 2004>

INIT_LOCK();

#define EnterCritSec() LOCK()
#define ExitCritSec() UNLOCK()
#endif

static void CloseNLMStuff();

BOOL StrNEqual(char *s1,char *s2,int len);

extern "C" int MaxVolumes = 0;          // gdf 06/13/00
extern "C" char ExcludedVols       [VOLUMES][32] = {0};          // gdf 01/20/2003  fix defect 1-LLE9R
extern "C" char ExcludedVolsScratch[VOLUMES][32] = {0};   // gdf 01/20/2003  fix defect 1-LLE9R
extern "C" int  ExVolHandle = 0;    // gdf 02/08/2003  fix defect 1-LLE9R
extern "C" int MainThread=0;
extern "C" char ConsoleRunning = 0;
extern "C" DWORD EventHandle = 0;
#ifndef LINUX
extern "C" int MainTgID = 0;
#endif
extern "C" char NewVersionString[128] = {0};

extern "C" int CLibVersion=0;
extern "C" int NWversion=0;
extern "C" int NWsubVersion=0;
/*
int MaxVolumes;          // gdf 06/13/00
char ExcludedVols[VOLUMES][32];          // gdf 01/20/2003  fix defect 1-LLE9R
char ExcludedVolsScratch[VOLUMES][32];   // gdf 01/20/2003  fix defect 1-LLE9R
int  ExVolHandle = 0;    // gdf 02/08/2003  fix defect 1-LLE9R
int MainThread=0;
char ConsoleRunning = 0;
DWORD EventHandle;
int MainTgID;
char NewVersionString[128];

int CLibVersion=0;
int NWversion=0;
int NWsubVersion=0;
*/

//char Nothing=0;

char HaveUI = 0;
char LocaleIDString[5];
char FileServerName[50];        // server where rtvscan is running
char VolumeName[50];        // volume where rtvscan is located
char ProgressCharDone=177;
char ProgressCharNotDone=176;

BOOL gZero=0;

int MainScreenHan=0;

//HINSTANCE hInstLang = 0;

#define MIN_RUN_TIME 10 // number of seconds the NLM must run before we will allow it to unload

extern "C" char DebugLogFile[IMAX_PATH];
extern "C" char gszDebug[DEBUG_STR_LEN];
extern "C" BOOL doRegDebug;
extern "C" BOOL ExitingNormally;


#ifdef LINUX
 // exsiting global flag for rtvscan main debug flag check
#define RTV_DEBUG_STR_LEN          100
char gszRTVDebug[RTV_DEBUG_STR_LEN]="";
BOOL gRTVdoRegDebug = TRUE;
#else
char gszRTVDebug[DEBUG_STR_LEN]="";
BOOL gRTVdoRegDebug;
#endif
extern char DebugLogFile[IMAX_PATH];
extern BOOL ExitingNormally;

BOOL CheckForNoLoad(int time);
void UnloadSavServer( void );   // gdf CW conversion

//bnm following prototype should be removed when we upgrade to the latest NDK
//removing dead code, this call was also causing issues with slfn , UseAccurateCaseForPaths API eliminates the need
//for this API for now...
/*
extern DWORD NWParsePath
(
   char * path,
        char          server1[50],
        NWCONN_HANDLE conn,
        char          volName[17],
        char          dirPath[256]
);
*/
//to tell following APIs not to uppercase when parsing
extern void  UseAccurateCaseForPaths( int yesno );      /* (default/original is FALSE) */


#ifdef MemCheck
void StartMemLog(void);
#endif // MemCheck
#ifdef SemaCheck
void StartSemaLog(void);
#endif // SemaCheck
/****************************************************************************/
DWORD FillDriveList(char *list)
{
#ifdef LINUX
//todo: do we need any data at this point? probably not for ap
    return 0;
#else
    int i;
    char str[IMAX_PATH];
    char vol[64];
// gdf need mods here

    StrCopy(list,"NW!");

//      num = GetNumberOfVolumes();   // gdf 06/18/00
//      for (i=0;i<num;i++) {         // gdf 06/18/00
    for ( i=0; i<MaxVolumes; i++ )     // gdf 06/18/00
    {
        memset(vol, 0, sizeof(vol)); //gdf 06/18/00
//              GetVolumeName(i,vol); //gdf 06/18/00
        if ( GetVolumeName(i, vol) == ESUCCESS )  // gdf 06/18/00
        {
            // gdf 06/18/00
            if ( vol[0] != NULL )  // gdf 06/18/00
            {
                // gdf 06/18/00
                sssnprintf(str,sizeof(str),"%s:%c;",vol,'F');
                strcat(list,str);
            }  // gdf 06/18/00
        }// gdf 06/18/00
    }
    list[NumBytes(list)-1] = 0;
    return 0;
#endif
}
/*********************************************************************/
DWORD GetCurrentDirectory(DWORD size,LPSTR path)
{

    getcwd(path,size);

    return 0;
}
/*********************************************************************/
BOOL CopyFile(LPCTSTR ExistingFile,LPCTSTR NewFile,BOOL FailIfExists)
{

    int sHan,dHan;
    int ret=FALSE;

    sHan = open(ExistingFile,O_BINARY|O_RDONLY,0);
    if ( sHan != INVALID_HANDLE_VALUE )
    {
        dHan = open(NewFile,O_WRONLY|O_TRUNC|O_BINARY|(FailIfExists?O_EXCL:0)|O_CREAT,S_IREAD|S_IWRITE);
        if ( dHan != INVALID_HANDLE_VALUE )
        {
            LONG size = lseek(sHan,0,SEEK_END);
            LONG out=0;
            lseek(sHan,0,SEEK_SET);
            ret = FileServerFileCopy(sHan,dHan,0,0,size,&out);
            if ( !ret )
                ret = TRUE;
            close(dHan);
        }
        close(sHan);
    }

    return(BOOL)ret;
}
/*********************************************************************/
BOOL MoveFile(LPCTSTR  lpszExisting,LPCTSTR  lpszNew)
{

    int ret;

    if ( CopyFile(lpszExisting,lpszNew,TRUE) )
    {
        MakeWriteable (lpszExisting,0xfffffffc);
        ret = DeletePurgeFile(lpszExisting);
        if ( !ret )
            return TRUE;
        dprintf("Unlink failed on move of %s  (%d)\n",lpszExisting,errno);
        DeletePurgeFile(lpszNew);
    }

    return FALSE;
}
/************************************************************************************/
BOOL GetSid(PSID pSid)
{

    pSid->ConnectionID = 0;
    StrCopy(pSid->UserName,"NetWare Server");

    return TRUE;
}
/************************************************************************************/
DWORD SendMessageTo(PSID sid,char *line)
{

    WORD list[10] = {0};
    BYTE rList[10] = {0};

    if ( sid->ConnectionID == 0xffffffff )
    {
        // need to find CinnectionID

        int i,max;
        char name[IMAX_PATH];
        char user[NAME_SIZE];
        char computer[NAME_SIZE];
        WORD type;
        long id;
        BYTE logTime[7];

        GetNames(sid,user,computer,NULL);
        max = GetMaximumNumberOfStations();

        for ( i=0;i<max;i++ )
        {
            ThreadSwitch();
			if ( GetConnectionInformation((WORD)i,name,&type,&id,logTime) == 0 && name[0] )
            {
                if ( !StrComp(name,user) )
                {
                    list[0] = (WORD)i;
                    break;
                }
            }
        }

        if ( i == max )
            return ERROR_FALSE_SID;
    }
    else
        list[0] = (WORD)sid->ConnectionID;

    while ( NumBytes(line) > 55 )
    {
        char *q,*w;
        q = w = line+55;
        while ( *q != ' ' )
        {
            q = PrevChar(line,q);
            if ( !q || q == line )
            {
                q = w;
                break;
            }
        }
        *q = 0;
        SendBroadcastMessage(line,list,rList,1);
        NTxSleep(250); // if we don't do this the next call will fail
        line = q+1;
    }
    SendBroadcastMessage(line,list,rList,1);

    return rList[0];
}
/****************************************************************************************/

#ifdef ThreadCheck
///////////////////////////////////////////
#define MAX_THREAD_TRACK 64
///////////////////////////////////////////
typedef struct
{
    LONG taskID;
    int threadID;
    char name[24];
} th;
th ThreadTracker[MAX_THREAD_TRACK];
///////////////////////////////////////////
void ResetThreadTracking()
{
    memset( ThreadTracker, 0, sizeof(th)*MAX_THREAD_TRACK );
}
///////////////////////////////////////////
void ThreadReport(BOOL shutdown)
{
    int i,j=0;

// ksr - 6/18/02 - memory leak fix, sts # 381005
#ifdef ThreadCheck
    if ( !TrackThreads )
        return;
#endif

    if ( shutdown && !debug )
        debug = DEBUGLOG|DEBUGSAFELOG;

#define msg "The following threads are being tracked:\n"
    if ( shutdown )
    {
        LogLine(msg,TRUE);
    }
    else
    {
        dprintf(msg);
    }
#undef msg

    for ( i=0 ; i<MAX_THREAD_TRACK ; i++ )
    {
        if ( ThreadTracker[i].taskID )
        {
            if ( shutdown )
            {
                char line[128]={0};
                if ( !j )
                {
                    time_t t = time(NULL);
                    LogLine( "**************************\r\nThread Report\r\n", FALSE );
                    LogLine( ctime(&t), FALSE );
                    j++;
                }

                sssnprintf(line,sizeof(line),"%2d 0x%08x %5d (%s)\r\n",i,ThreadTracker[i].threadID,ThreadTracker[i].taskID,ThreadTracker[i].name);
                LogLine(line,TRUE);
            }
            else
            {
                dprintf("%2d 0x%08x %5d (%s)\n",i,ThreadTracker[i].threadID,ThreadTracker[i].taskID,ThreadTracker[i].name);
            }
        }
    }

    if ( shutdown )
    {
        debug = 0;
        if ( j )
            LogLine("**************************\r\n",TRUE);
    }
}
///////////////////////////////////////////
void ThreadStarting( char *name, LONG taskID)
{

    int i;

    if ( 
// ksr - 6/18/02 - memory leak fix, sts # 381005
#ifdef ThreadCheck
       !TrackThreads || 
#endif
       !taskID ) return;

    for ( i=0 ; i<MAX_THREAD_TRACK ; i++ )
    {
        if ( !ThreadTracker[i].taskID )
        {
            dprintf( "%d: Thread Starting: %x(%s) %d\n", i, taskID, name);
#ifdef LINUX_DEMO
printf( "%d: Thread Starting: %x(%s) %d\n", i, taskID, name);
#endif
            strncpy( ThreadTracker[i].name, name, 23 );
            ThreadTracker[i].taskID = taskID;
            ThreadTracker[i].threadID = NTxGetCurrentThreadId();
            return;
        }
    }
    dprintf("Couldn't store thread %x(%s) in ThreadTracker array!\n", taskID, name);
}
///////////////////////////////////////////
void ThreadEnding( LONG taskID )
{

    int i;

    if ( 
// ksr - 6/18/02 - memory leak fix, sts # 381005
#ifdef ThreadCheck
       !TrackThreads || 
#endif
       !taskID ) return;

    for ( i=0 ; i<MAX_THREAD_TRACK ; i++ )
    {
        if ( ThreadTracker[i].taskID == taskID )
        {
            dprintf( "%d: Thread Ending: %x(%s) 0x%08x\n", i, taskID, ThreadTracker[i].name, ThreadTracker[i].threadID);
#ifdef LINUX_DEMO
printf( "%d: Thread Ending: %x(%s) 0x%08x\n", i, taskID, ThreadTracker[i].name, ThreadTracker[i].threadID);
#endif
            memset( &ThreadTracker[i], 0, sizeof(th) );
            return;
        }
    }
    dprintf("Couldn't find thread %x in ThreadTracker array!\n", taskID );
}
///////////////////////////////////////////
#endif // ThreadCheck

/************************************************************************************************/
//#define MAX_THREADS 10	in pscan.h
extern "C" DWORD scanThreadIDArray[MAX_THREADS] = {0};
extern "C" int theadIDIndex=0;
extern "C" BOOL  dec_bScanExclude=0; // gdf 11/12/2003  1-1XEB4p
/*
DWORD scanThreadIDArray[MAX_THREADS];
int theadIDIndex=0;
BOOL  dec_bScanExclude=0; // gdf 11/12/2003  1-1XEB4p
*/
/************************************************************************************************/
typedef struct
{
    void (*fun)(void *);
    void *p;
    char name[64];
} THREADS;
/************************************************************************************************/
void _st(void *nothing)
{


    LONG    origTaskID,     newTaskID;
    THREADS *t = (THREADS *)nothing;
    void *p = t->p;
    void (*fun)(void *) = t->fun;

    char tName[64];
    strcpy(tName,t->name);

    RenameThread(NTxGetCurrentThreadId(),t->name);

//all scan threads start with this label. All threads start here, we only want the scan threads.
//there will be ten of them.
	if(!strncmp("RTV Checker",t->name,11))
	{
		scanThreadIDArray[theadIDIndex]=NTxGetCurrentThreadId();
		theadIDIndex++;
	}
    free(t);

    ThreadsInUse++;
    dprintf ("Thread %s starting\n",tName);
#ifdef LINUX_DEMO
printf ("%u: Thread %s starting\n",GetCurrentThreadId(),tName);
#endif
    newTaskID = AllocateBlockOfTasks(1);
    origTaskID = SetCurrentTask(newTaskID);

#ifdef ThreadCheck
    ThreadStarting(tName,newTaskID);
#endif // ThreadCheck

    fun(p);

#ifdef ThreadCheck
    ThreadEnding(newTaskID);
#endif // ThreadCheck

    SetCurrentTask(origTaskID);
    ReturnBlockOfTasks(newTaskID, 1);
    dprintf ("Thread %s terminating\n",tName);
#ifdef LINUX_DEMO
printf ("%u: Thread %s terminating\n",GetCurrentThreadId(),tName);
#endif
    ThreadsInUse--;
}
/************************************************************************************************/
DWORD rtvBeginThread(void (*fun)(void*),void *p,char *name)
{

    int rtvtgid;
    THREADS *t;
    t=(THREADS*)malloc(sizeof(THREADS));
//    t=malloc(sizeof(THREADS));

    if ( t )
    {
        t->fun = fun;
        t->p = p;
        strncpy(t->name,name,sizeof(t->name));
        rtvtgid = BeginThreadGroup(_st,NULL,STACK_SIZE,(void*)t);
        if ( rtvtgid != EFAILURE )
        {
            dprintf("Thread %s created\n",name);
#ifdef LINUX_DEMO
printf("%u: Thread %s created\n",GetCurrentThreadId(),name);
#endif
        }
        else
        {
            dprintf("Thread %s was not created (create fail)\n",name);
            // I would rather lose mem that take the change of freeing it twice.
        }
        return(rtvtgid==EFAILURE) ? -1 : rtvtgid;
    }
    dprintf("Thread %s was not created (malloc fail)\n",name);
    errno = ENOMEM;
    return -1;
}
/***************************************************************************/
DWORD StartNTSService(void)
{
#ifndef LINUX
    HANDLE han;
    char libpath[IMAX_PATH];

    sssnprintf(libpath,sizeof(libpath),NW_SYSTEM_DIR"\\%s","PDS.NLM");
    han = _LoadLibrary(libpath,true);

    if ( han == NULL )
        return P_NO_VIRUS_ENGINE;
#endif
    return 0;
}
/***********************************************************************************************/
BOOL Check8Dot3 (char *path)
{

    int
    len=NumBytes(path);
    char
    *s=NULL,
    *r=NULL,
    *q=path;

    if ( len>12 )
    {           // if the section we just looked at is len>12 we have a long name
//              dprintf ("%s is NOT in 8.3 format(1)\n",path);
        return FALSE;
    }

    r=StrChar (q,'.');
    if ( r )
    {                        // we have a dot
        *r='\0';                // split the string into parts
        r++;
        s=StrChar (r,'.');
        if ( s )
        {// we have two dots in the section so we are a long name
//                      dprintf ("%s is NOT in 8.3 format(2)\n",path);
            return FALSE;
        }

        if ( NumBytes (q)>8 || NumBytes(r)>3 )
        {  // if the first part len > 8 or second part len >3 we have a long name
//                      dprintf ("%s is NOT in 8.3 format(3)\n",path);
            return FALSE;
        }
    }
    else
    {                                          // there is not dot so
        if ( len>8 )
        {                    // if len > 8 we have a long name
//                      dprintf ("%s is NOT in 8.3 format(4)\n",path);
            return FALSE;
        }
    }

//dprintf ("%s is in 8.3 format\n",path);
// if we get here then the path is in 8.3 format
    return TRUE;
}
/***********************************************************************************************/
BOOL isNSName(char* path,char nameSpace)
{

    int
    ns,
    ccode;
    char
    server[48],
    volume[16],
    dir[IMAX_PATH],
    buff[IMAX_PATH];
    char
    *r,
    *q,
    *end;

    ns=ccode=0;
    q=end=NULL;
    memset (buff,0,IMAX_PATH);

    if ( !path ) return TRUE;
    if ( NumBytes (path)<1 ) return TRUE;
//dprintf ("isNSName(%s,%d)\n",path,nameSpace);
    ParsePath (path,server,volume,dir);
//dprintf ("ParsePath=(%s,%s,%s,%s)\n",path,server,volume,dir);
    q=dir;
    if ( *q=='/' || *q=='\\' )
        q++;

    // first look for non-short characters
    if ( StrChar (q,' ') || StrChar (q,'[') || StrChar (q,']') || StrChar (q,',') || StrChar (q,'=') || StrChar (q,';') || StrChar (q,'+') )
        return((nameSpace==0) ? FALSE : TRUE);         // path has long components so if nameSpace is 0 return false else return true

	end=&dir[NumBytes(dir)];
    while( q && *q && q<=end )
    { // start at the beginning of the path and get a pointer to the end of it
        for ( r=q ; q && *q && q<=end && *q!='/' && *q!='\\' ; q=NextChar(q) ); // look for the next section

        if ( q )
        {
            char* w = NextChar(q);
            *q=0;    // separate the next section
            q=w;
        }
        if ( Check8Dot3 (r)==FALSE )      // check to see if section is in 8.3 format
            return((nameSpace==0) ? FALSE : TRUE); // if not in 8.3 format return false
    }

//dprintf ("Path has only short components\n");
    // now we know that there are no long components in the path
    // since we only call to check on the shortness of a name we should return true

    return((nameSpace==0) ? TRUE : FALSE);         // so if nameSpace is 0 return true else return false
}
/***********************************************************************************************/
#ifdef NLM //file name conversion is not required on Linux
#include <nwfile.h>
#include <niterror.h>
BOOL ConvertFileName(char *output,char *input,int len,char inputNS,char outputNS)
{
/*
NWGetDirBaseFromPath (input,inputNS,&volNum,&NSdirBase,&DOSdirBase)
FEMapHandleToVolumeAndDirectory(handle,&volNum,&dirNum)
FEConvertDirectoryNumber (inputNS,volNum,dirNum,outputNS,&destDirNum)
FEMapVolumeAndDirectoryToPathForNS(volNum,destDirNum,outputNS,output,&pathCount)
FEMapVolumeAndDirectoryToPath(volNum,destDirNum,output,&pathCount)
*/
    int
    oldVolNum,
    volNum,
    oldNameSpace,
    newNameSpace,
    ccode;
    char
    server[48],
    volume[16],
    path[IMAX_PATH],
    shortName[IMAX_PATH],
    longPath[IMAX_PATH];
    char
    *q,
    *r,
    *end,
    delim;
    BOOL
    ResetVolNum = FALSE,
    volFound = FALSE,
    ret = TRUE;

/*
        int           ccode1;
        NWCONN_HANDLE conn;
        char          server1[50];
        char          volName[17];
        char          dirPath[256];
*/

    if ( debug & DEBUGVERBOSE )
        dprintf( "CONVERTFILENAME: \"%s\" from NS %d to %d into %lu byte buffer.\n", input, inputNS, outputNS, len );

    // Validate params.
    if ( NumBytes(input) > sizeof(path) - 1 )
    {
        dprintf( "CONVERTFILENAME: Input path is too long to convert.\n" );
        ret = FALSE;
    }
    else if ( inputNS == outputNS )
    {
        // don't convert if we don't need to
        if ( debug & DEBUGVERBOSE )
            dprintf( "CONVERTFILENAME: Files are already in the same namespace.\n" );

        if ( NumBytes(input) > len - 1 )
        {
            // We can't copy the caller's input path into his output buffer,
            // even though the names are in the same namespace.  Don't give the caller
            // the impression that we are putting data in his out buffer - fail.
            dprintf( "CONVERTFILENAME: Output buffer too small for same namespace name.\n" );
            ret = FALSE;
        }
        else
        {
            StrNCopy( output, input, len );
            output[ len - 1 ] = 0;
        }
    }
    else
    {
/*
                //bnm this newer API will give us the path without uppercasing it. 
                // Make sure that when we put this in, we unit test with SLFN paths.
                // i.e. paths over 256 chars in length.
                ccode1 = NWParsePath( input,     // > pointer to path
                                                          server1,   // < pointer to server name 48 char optional
                                                          &conn,     // < pointer to conn handle
                                                          volName,   // < pointer to volume name 17 char optional
                                                          dirPath ); // < pointer to directory  256 char optional
                if(ccode1){
                        dprintf("CONVERTFILENAME: NWParsePath failed %X\n",ccode);
                }
*/
        // Parse the caller's path for the volume name.  The docs for the NW APIs say
        // that this function will fail if input is longer than 255 - but that doesn't seem to be
        // the case.
        if ( ParsePath( input, server, volume, path ) == ERROR_SUCCESS &&
             GetVolumeNumber( volume, &volNum ) == ERROR_SUCCESS )
        {
            // Set the current working volume.
            oldVolNum = FESetCWVnum( volNum );
            ResetVolNum = TRUE; // Undo the set later in the function
        }
        else
        {
            dprintf( "CONVERTFILENAME: Couldn't set current working volume number.\n");
        }

        // Copy the caller's path into our working buffer.
        //bnm sts#378433 and 378996 to avoid using the returned uppercased name which would result in short name conversion failure

        StrNCopy( path, input, sizeof(path) );
        path[ sizeof(path) - 1 ] = 0;

        oldNameSpace = SetCurrentNameSpace( inputNS );
        if ( debug & DEBUGVERBOSE )
            dprintf( "CONVERTFILENAME: Switching NS from %d to %d.\n", oldNameSpace, inputNS );

        // Pick the caller's path apart piece-by-piece.  We are going to convert each
        // directory and file name into the corresponding new name space name.
        memset( longPath, 0, sizeof(longPath) );

        // Skip the first slash, if it exists.
        q = path;
        if ( *q == '\\' || *q == '/' )
            q++;

        // Truncate the output buffer
        output[0] = 0;

        // Iterate to the end of the caller's path.
        end = path + NumBytes(path);
        while ( q < end )
        {
            // Iterate forward to the first delimiter (save our place in r)
			char *w; //otherwise we'll get a null string back from NWNextChar
            r = q;
            while ( *q != 0 && *q != ':' && *q != '/' && *q != '\\' )
                q = NextChar(q);

            delim = *q;                                                                     // find out what delimeter is there

//			*q = 0;
            if ( q < end )
			{
				w=q;
				q=NextChar(q);
				*w=0;
			}

            if ( delim == ':' )
            {
                if ( volFound )
                {
                    dprintf( "CONVERTFILENAME: Bad path - extra volume delimiter.\n");
                    ret = FALSE;
                    break; // bad path - ':' in the wrong place.
                }

                // we have the volume
                if ( NumBytes(r) + 1 > len - 1 ||
                     NumBytes(r) + 1 > sizeof(longPath) )
                {
                    dprintf( "CONVERTFILENAME: Target volume name is too long.\n");
                    ret = FALSE;
                    break;
                }

                sssnprintf( output, len, "%s:", r );                    // put the volume, the colon and the slash in output

                if ( *q == '/' || *q == '\\' )                   // there may be a slash after the volume
                    q = NextChar(q);                                        // move to the character after the slash

                StrCopy( longPath, output );                    // start building the longpath
                volFound = TRUE;
            }
            else
            {
                // Add the next section to the long path (if it fits)
                // This shouldn't happedn because we already did this check at the head of the fn.
                if ( NumBytes(longPath) + 1 + NumBytes(r) > sizeof(longPath) - 1 )
                {
                    dprintf( "CONVERTFILENAME: Input path is too long to convert (2).\n" );
                    ret = FALSE;
                    break;
                }

                sssnprintf( longPath, sizeof(longPath), "%s" sSLASH "%s", longPath, r );
                memset( shortName, 0, sizeof(shortName) );

                //bnm: tell NWGetNameSpaceEntryName not to uppercase when parsing file name
                UseAccurateCaseForPaths(TRUE);

                // convert the path to the new namespace

                if ( (ccode = NWGetNameSpaceEntryName( (const unsigned char *)longPath, (unsigned long)outputNS, (unsigned long)sizeof(shortName), (unsigned char *)shortName ) ) == ERROR_SUCCESS )
                {
                    // add the converted filename to output (if it fits)
                    if ( NumBytes(output) + 1 + NumBytes(shortName) > len - 1 )
                    {
                        dprintf( "CONVERTFILENAME: Target namespace name is too long: name (%s) in path [%s].\n", shortName, output );
                        ret = FALSE;
                        break;
                    }

                    sssnprintf( output, len, "%s" sSLASH "%s", output, shortName );
                }
                else
                {
                    dprintf( "CONVERTFILENAME: GetNameSpaceEntryName failed: in->[%s] error code->(%d).\n", longPath, ccode );
                    ret = FALSE;
                    break;
                }
            }
        }

        if ( ret )
            if ( debug & DEBUGVERBOSE )
                dprintf( "CONVERTFILENAME: Path converted: %s.\n", output );
    }

    if ( !ret )
    {
        // On failure, echo the caller's path into the output buffer,
        // if it fits.
        if ( NumBytes(input) < len - 1 )
        {
            StrNCopy( output, input, len );
            output[ len - 1 ] = 0;
        }
        else
        {
            memset( output, 0, len );
        }
    }

    // Put things back the way we found them.
    UseAccurateCaseForPaths(FALSE);
    newNameSpace=SetCurrentNameSpace( oldNameSpace );
    SAVASSERT(newNameSpace==inputNS);
    if ( ResetVolNum )
        FESetCWVnum( oldVolNum );

    return( ret );
}
#endif //NLM - filename conversion not required on Linux

// --------------------------------------
// ksr - 8/17/2002

// No need for this function
#if 0 

/**************************************************************************************/
// LONG GetOriginalName (
//              char *srcName,          // (in) file name to get the original name for
//              char *OriginalName, // (out) original file name
//              LONG srcNS,                     // (in) name space of the source file name
//              LONG *OriginalNS        // (out) originating name space
// );
// Gets the original name of the file in the originating name space
// OriginalName and OriginalNS can be NULL
// this function returns the originating name space
// if the function fails returns -1
LONG GetOriginalName (char *srcName,char *OriginalName,LONG srcNS, LONG *OriginalNS)
{

    LONG
    ons=-1,
    d;
    int
    oldVolNum,
    volNum,
    v;
    HANDLE
    h=-1;
    BYTE
    oldNS;
    char
    server[48],
    volume[16],
    path[IMAX_PATH];
    BOOL
    ResetNS=FALSE,
    ResetVolNum=FALSE;

    if ( !srcName ) return -1;

    if ( ParsePath (srcName,server,volume,path)==ERROR_SUCCESS )
    {
        if ( GetVolumeNumber (volume,&volNum)==ERROR_SUCCESS )
        {
            if ( FEGetCWVnum() != volNum )
            {
                oldVolNum=FESetCWVnum (volNum);
                ResetVolNum=TRUE;
            }
        }
    }
    oldNS=SetCurrentNameSpace(srcNS);
    if ( oldNS!=255 && oldNS!=srcNS ) // only reset if successfull and srcNS doesn't equal oldNS
        ResetNS=TRUE;

    dprintf ("GetOriginalName (%s,%d)\n",srcName,srcNS);
    h=open (srcName,O_RDONLY);

    if ( OriginalNS )
        *OriginalNS=-1;

    if ( h!=-1 )
    {
//              dprintf ("\thandle (%d)\n",h);
        if ( FEMapHandleToVolumeAndDirectory (h,&v,&d)==ERROR_SUCCESS )
        {
//                      dprintf ("\tv:(%d)\n\td:(0x%x)\n",v,d);
            ons=FEGetOriginatingNameSpace (v,d);
            if ( ons!=-1 )
            {
//                              dprintf ("\t**OriginalNS:(%d)**\n",ons);
                if ( !OriginalName )
                { // if OriginalName is NULL we're done
                    dprintf ("OriginalName is NULL so we're done\n");
                }
                else if ( srcNS==ons )
                {
                    StrCopy (OriginalName,srcName); // it is already in the originating namespace so just copy over
                    dprintf ("\tAlready Original Filename: %s\n",OriginalName);
                }
                else if ( ConvertFileName(OriginalName,srcName,IMAX_PATH,srcNS,ons) )
                {
                    dprintf ("\tOriginal Filename: %s\n",OriginalName);
                }
                else
                {
                    dprintf ("\tERROR Converting file name (%d)(%d)\n",errno,NetWareErrno);
                }
            }
            else
            {
                dprintf ("\tERROR Getting originating name space (%d)(%d)\n",errno,NetWareErrno);
            }
        }
        else
        {
            dprintf ("\tERROR mapping handle to directory (%d)(%d)\n",errno,NetWareErrno);
        }
        close (h);
    }
    else
    {
        dprintf ("\tERROR getting file handle (%d)(%d)\n",errno,NetWareErrno);
    }

    if ( ons==-1 && OriginalName )
        OriginalName[0]='\0';

    if ( OriginalNS )
        *OriginalNS=ons;

    if ( ResetNS ) SetCurrentNameSpace (oldNS);
    if ( ResetVolNum ) FESetCWVnum (oldVolNum);
    return ons;
}

#endif // 0 

// --------------------------------------
#ifdef NLM
/***********************************************************************************************/
DWORD SetEA(char *name,char *key,BYTE *data,DWORD len)
{

    DWORD cc=ERROR_OPEN_FAIL;
    int eaHandle;
    LONG    accessFlags = 0x00000000;
    char buf[1024];
    DWORD size = (((len/128)*128)+128);

    if ( size > sizeof(buf) )
        return ERROR_BAD_PARAM;

    EnterCritSec();

    eaHandle = OpenEA(name,0);
    if ( eaHandle != -1 )
    {
        memset(buf,0,size);
        memcpy(buf,data,len);
        cc = WriteEA(eaHandle,(const char *)key,(const char *)data,size,accessFlags);
        if ( cc == INVALID_HANDLE_VALUE )
        {
            dprintf("Write EA failed on file %s\n",name);
        }
        else if ( cc == size )
            cc = ERROR_SUCCESS;
        else
            cc = ERROR_BAD_EA;

        CloseEA(eaHandle);
    }

    ExitCritSec();

    return cc;
}
/*******************************************************************************************************/
DWORD GetEA(char *name,char *key,BYTE *data,DWORD len)
{


    DWORD cc=ERROR_OPEN_FAIL;
    int eaHandle;
    LONG    accessFlags = 0x00000000;
    char buf[1024];
    DWORD size = (((len/128)*128)+128);

    if ( size > sizeof(buf) )
        return ERROR_BAD_PARAM;

    EnterCritSec();

    eaHandle = OpenEA(name,0);
    if ( eaHandle != -1 )
    {

        cc = ReadEA(eaHandle,key,buf,size,&accessFlags);
        if ( cc == INVALID_HANDLE_VALUE )
        {
            dprintf("Read EA failed on file %s\n",name);
        }
        else if ( cc == size )
        {
            memcpy(data,buf,len);
            cc = ERROR_SUCCESS;
        }
        else
            cc = ERROR_BAD_EA;

        CloseEA(eaHandle);
    }

    ExitCritSec();

    return cc;
}
#endif //NLM
/*****************************************************************************************
DWORD ConvertToLongName(char *output,char *input) {

        return NWGetNameSpaceEntryName(input,4,SYSFILELEN,output);
}
*******************************************************************************************/
BOOL isFileCompressed(int attr)
{

    return(attr&_A_FILE_MIGRATED) || (attr&_A_FILE_COMPRESSED);

}

/*******************************************************************************************/
//
// created jmillard 09Oct2001
// returns whether the file is currently offline or not
//
/*******************************************************************************************/

BOOL isFileOffline(WIN32_FIND_DATA *lpFindData) 
{
    // this flag is accurate and sufficient on NetWare

    return lpFindData->dwFileAttributes & _A_FILE_MIGRATED;
} 

/****************************************************************************************/
BOOL AskIfAbort(void)
{

    int i;

    printf("\n\nDebug Error:\nSymantec AntiVirus did not load properly during previous startup.\nDo you want to continue loading Virus Protect?\n\nPress 'C' to continue, 'S' to stop.\n\nVirus Protect will load in :   ");
    for ( i=0;i<30;i++ )
    {
        printf("\b\b%2u",30-i);
        NTxSleep(1000);
        if ( kbhit() )
        {
            char c = toupper(getch());
            if ( c == 'C' )
            {
                printf("\n\nVirus Protect will now load.\n");
                return FALSE;
            }
            if ( c == 'S' )
            {
                printf("\n\nStopping ... \n");
                return TRUE;
            }
        }
    }

    return FALSE;
}
/**************************************************************************************/
char *MakeNewThreadName(char *add,char *current,char *out,size_t outbytes)
{


    if ( debug&DEBUGPRINT )
    {
        char *q;
        q = StrChar(current,' ');
        if ( q == NULL )
            q = current;
        sssnprintf(out,outbytes,"VE:%s",q);
        return out;
    }
    else
        return add;
}

/*********************************************************************************************/
#ifndef LINUX
extern "C"
{
int GetCPUUtilization(void)
{
    extern long NumberOfPollingLoops;
    extern long MaximumNumberOfPollingLoops;
    long u,m;

    m = max(MaximumNumberOfPollingLoops,1);
    u = NumberOfPollingLoops * 100 + (m >> 1);
    u /= m;
    return(max(min(100,100 - (int)u),0));
}
}
#endif
/*****************************************************************************/
void UpdateServerFiles ()
{

    DWORD UpdateStatus=0;
    HKEY hkey;

    if ( RegOpenKey(hMainKey,UPGRADE_KEY,&hkey) == ERROR_SUCCESS )
    {
        UpdateStatus=GetVal(hkey,FileServerName,0);
        if ( UpdateStatus==STS_NWCOPY_NORMAL )
            CopyUpdatedFiles(FALSE);
        if ( UpdateStatus==STS_NWCOPY_FORCE )
            CopyUpdatedFiles(TRUE);
        PutVal (hkey,FileServerName,STS_NWCOPY_NOCOPY);
        RegCloseKey(hkey);
    }
}
/*****************************************************************************/
HKEY NlmPChkey = NULL;
extern "C" void RegDebug (void/*BOOL set*/);
void RegDebug (void/*BOOL set*/)
{

    if ( gRTVdoRegDebug )
    {
        if ( NlmPChkey == NULL )
            RegOpenKey(HKEY_LOCAL_MACHINE,REGHEADER"\\ProductControl",&NlmPChkey);

        GetStr(NlmPChkey,"Debug",gszRTVDebug,sizeof(gszRTVDebug),"");
        InitializeDebugLogging(gszRTVDebug);
    }
}
/*****************************************************************************/
DWORD CopyUpdatedFiles(BOOL force)
{

    FILE
    *inf=NULL;
    char
    InfPath[MAX_PATH],
    line[1024];
    BOOL
    CopyingFiles=FALSE;
    DWORD
    dwRet=0;

    dprintf ("Copying Updated Files %s\n",force?"FORCED":"NORMAL");

    sssnprintf (InfPath,sizeof(InfPath),"%s" sSLASH "Update.inf",ProgramDir);

    inf = fopen (InfPath,"rt");
    if ( !inf ) return ERROR_GENERAL;

    while ( fgets (line, 1024, inf) )
    {

        if ( line[0]=='[' ) CopyingFiles=FALSE;
        if ( !strnicmp (line,"[UpdateNetwareFiles]",20) )
        {
            CopyingFiles=TRUE;
            continue;
        }
        if ( CopyingFiles )
            dwRet+=CopyNewFile(line,force);
        if ( feof (inf) ) break;
        memset (line,0,1024);
    }
    fclose (inf);

    return dwRet;
}
/*******************************************************************************************/
DWORD SetString(LONG wID,char *NewString)
{

    if ( TheStringTable == NULL )
        return ERROR_GENERAL;

    if ( wID < STR_APP_NAME || wID >= IDS_LAST_MESSAGE )
        wID = 998;

    TheStringTable[wID - 998] = NewString;
    return ERROR_SUCCESS;
}
/****************************************************************************************/
DWORD NullPatFun(void)
{
    return VENOTSUPPORTED;
}
/************************************************************************************/
DWORD UnLoadPattFunctions(void)
{
#ifndef LINUX
    HANDLE han;
    char libname[IMAX_PATH];
    char libpath[IMAX_PATH];
    char line[IMAX_PATH],str[4];

    GetStr(hMainKey,"VirusEngine",libname,sizeof(libname),"");

    if ( libname[0] == 0 )
        return P_NO_VIRUS_ENGINE;

    sssnprintf(libpath,sizeof(libpath),"%s" sSLASH "%s",HomeDir,libname);
    han = _LoadLibrary(libpath,false);

    if ( han == NULL )
        return P_NO_VIRUS_ENGINE;


    memcpy(str,libname,3);
    str[3] = 0;
    strlwr(str);
    sssnprintf(line,sizeof(line),"%s_%s",str,"VEInit");UnimportSymbol(GetNLMHandle(),line); VEInit = (tVEInit )NullPatFun;
    sssnprintf(line,sizeof(line),"%s_%s",str,"VEDeInit");UnimportSymbol(GetNLMHandle(),line); VEDeInit = (tVEDeInit)NullPatFun;
    sssnprintf(line,sizeof(line),"%s_%s",str,"VEScanFile");UnimportSymbol(GetNLMHandle(),line); VEScanFile = (tVEScanFile)NullPatFun;
    sssnprintf(line,sizeof(line),"%s_%s",str,"VEGetVirusBehaviorHeader");UnimportSymbol(GetNLMHandle(),line); VEGetVirusBehaviorHeader = (tVEGetVirusBehaviorHeader)NullPatFun;
    sssnprintf(line,sizeof(line),"%s_%s",str,"VEScanVirusBehaviorHeader");UnimportSymbol(GetNLMHandle(),line); VEScanVirusBehaviorHeader = (tVEScanVirusBehaviorHeader)NullPatFun;
    sssnprintf(line,sizeof(line),"%s_%s",str,"VECleanFile");UnimportSymbol(GetNLMHandle(),line); VECleanFile = (tVECleanFile)NullPatFun;
    sssnprintf(line,sizeof(line),"%s_%s",str,"VEGetInfo");UnimportSymbol(GetNLMHandle(),line); VEGetInfo = (tVEGetInfo)NullPatFun;
    sssnprintf(line,sizeof(line),"%s_%s",str,"VEGetCriticalDiskData");UnimportSymbol(GetNLMHandle(),line); VEGetCriticalDiskData = (tVEGetCriticalDiskData)NullPatFun;
    sssnprintf(line,sizeof(line),"%s_%s",str,"VESetCriticalDiskData");UnimportSymbol(GetNLMHandle(),line); VESetCriticalDiskData = (tVESetCriticalDiskData)NullPatFun;
    sssnprintf(line,sizeof(line),"%s_%s",str,"VEScanCriticalDiskData");UnimportSymbol(GetNLMHandle(),line); VEScanCriticalDiskData = (tVEScanCriticalDiskData)NullPatFun;
    sssnprintf(line,sizeof(line),"%s_%s",str,"VECleanCriticalDiskData");UnimportSymbol(GetNLMHandle(),line); VECleanCriticalDiskData = (tVECleanCriticalDiskData)NullPatFun;
    sssnprintf(line,sizeof(line),"%s_%s",str,"VEScanMemory");UnimportSymbol(GetNLMHandle(),line); VEScanMemory = (tVEScanMemory)NullPatFun;
    sssnprintf(line,sizeof(line),"%s_%s",str,"VELoadPatternFile");UnimportSymbol(GetNLMHandle(),line); VELoadPatternFile = (tVELoadPatternFile)NullPatFun;
    sssnprintf(line,sizeof(line),"%s_%s",str,"VEFreePatternFiles");UnimportSymbol(GetNLMHandle(),line); VEFreePatternFiles = (tVEFreePatternFiles)NullPatFun;
    sssnprintf(line,sizeof(line),"%s_%s",str,"VELoadSignature");UnimportSymbol(GetNLMHandle(),line); VELoadSignature = (tVELoadSignature)NullPatFun;
    sssnprintf(line,sizeof(line),"%s_%s",str,"VEUnloadSignature");UnimportSymbol(GetNLMHandle(),line); VEUnloadSignature = (tVEUnloadSignature)NullPatFun;
    sssnprintf(line,sizeof(line),"%s_%s",str,"VEEnableSignature");UnimportSymbol(GetNLMHandle(),line); VEEnableSignature = (tVEEnableSignature)NullPatFun;
    sssnprintf(line,sizeof(line),"%s_%s",str,"VEEnumSignatures");UnimportSymbol(GetNLMHandle(),line); VEEnumSignatures = (tVEEnumSignatures)NullPatFun;
    sssnprintf(line,sizeof(line),"%s_%s",str,"VEGetVirusInfo");UnimportSymbol(GetNLMHandle(),line); VEGetVirusInfo = (tVEGetVirusInfo)NullPatFun;
    sssnprintf(line,sizeof(line),"%s_%s",str,"VESetPattManParameters");UnimportSymbol(GetNLMHandle(),line); VESetPattManParameters = (tVESetPattManParameters)NullPatFun;
    sssnprintf(line,sizeof(line),"%s_%s",str,"VEGetSignatureVirusCount");UnimportSymbol(GetNLMHandle(),line); VEGetSignatureVirusCount = (tVEGetSignatureVirusCount)NullPatFun;

    _FreeLibrary(libpath);
#endif
    return 0;
}
/****************************************************************************************/
DWORD LoadPattFunctions(void)
{
#ifndef LINUX
    HANDLE han;
    char libname[IMAX_PATH];
    char libpath[IMAX_PATH];
    char line[IMAX_PATH],str[4];

    GetStr(hMainKey,"VirusEngine",libname,sizeof(libname),"");

    if ( libname[0] == 0 )
    {
        dprintf ("libname[0]==0\n");
        return P_NO_VIRUS_ENGINE;
    }

    sssnprintf(libpath,sizeof(libpath),"%s" sSLASH "%s",HomeDir,libname);
    han = _LoadLibrary(libpath,true);

    if ( han == NULL )
    {
        dprintf ("han==NULL\n");
        return P_NO_VIRUS_ENGINE;
    }

    memcpy(str,libname,3);
    str[3] = 0;
    strlwr(str);
    sssnprintf(line,sizeof(line),"%s_%s",str,"VEInit");if ( (VEInit = (tVEInit)ImportSymbol(GetNLMHandle(),line)) == NULL ) return P_INVALID_VIRUS_ENGINE;
    sssnprintf(line,sizeof(line),"%s_%s",str,"VEDeInit");if ( (VEDeInit  = (tVEDeInit )ImportSymbol(GetNLMHandle(),line)) == NULL ) return P_INVALID_VIRUS_ENGINE;
    sssnprintf(line,sizeof(line),"%s_%s",str,"VEScanFile");if ( (VEScanFile  = (tVEScanFile )ImportSymbol(GetNLMHandle(),line)) == NULL ) return P_INVALID_VIRUS_ENGINE;
    sssnprintf(line,sizeof(line),"%s_%s",str,"VEGetVirusBehaviorHeader");if ( (VEGetVirusBehaviorHeader  = (tVEGetVirusBehaviorHeader )ImportSymbol(GetNLMHandle(),line)) == NULL ) return P_INVALID_VIRUS_ENGINE;
    sssnprintf(line,sizeof(line),"%s_%s",str,"VEScanVirusBehaviorHeader");if ( (VEScanVirusBehaviorHeader  = (tVEScanVirusBehaviorHeader )ImportSymbol(GetNLMHandle(),line)) == NULL ) return P_INVALID_VIRUS_ENGINE;
    sssnprintf(line,sizeof(line),"%s_%s",str,"VECleanFile");if ( (VECleanFile  = (tVECleanFile )ImportSymbol(GetNLMHandle(),line)) == NULL ) return P_INVALID_VIRUS_ENGINE;
    sssnprintf(line,sizeof(line),"%s_%s",str,"VEGetInfo");if ( (VEGetInfo  = (tVEGetInfo )ImportSymbol(GetNLMHandle(),line)) == NULL ) return P_INVALID_VIRUS_ENGINE;
    sssnprintf(line,sizeof(line),"%s_%s",str,"VEGetCriticalDiskData");if ( (VEGetCriticalDiskData  = (tVEGetCriticalDiskData )ImportSymbol(GetNLMHandle(),line)) == NULL ) return P_INVALID_VIRUS_ENGINE;
    sssnprintf(line,sizeof(line),"%s_%s",str,"VESetCriticalDiskData");if ( (VESetCriticalDiskData  = (tVESetCriticalDiskData )ImportSymbol(GetNLMHandle(),line)) == NULL ) return P_INVALID_VIRUS_ENGINE;
    sssnprintf(line,sizeof(line),"%s_%s",str,"VEScanCriticalDiskData");if ( (VEScanCriticalDiskData  = (tVEScanCriticalDiskData )ImportSymbol(GetNLMHandle(),line)) == NULL ) return P_INVALID_VIRUS_ENGINE;
    sssnprintf(line,sizeof(line),"%s_%s",str,"VECleanCriticalDiskData");if ( (VECleanCriticalDiskData  = (tVECleanCriticalDiskData )ImportSymbol(GetNLMHandle(),line)) == NULL ) return P_INVALID_VIRUS_ENGINE;
    sssnprintf(line,sizeof(line),"%s_%s",str,"VEScanMemory");if ( (VEScanMemory  = (tVEScanMemory )ImportSymbol(GetNLMHandle(),line)) == NULL ) return P_INVALID_VIRUS_ENGINE;
    sssnprintf(line,sizeof(line),"%s_%s",str,"VELoadPatternFile");if ( (VELoadPatternFile  = (tVELoadPatternFile )ImportSymbol(GetNLMHandle(),line)) == NULL ) return P_INVALID_VIRUS_ENGINE;
    sssnprintf(line,sizeof(line),"%s_%s",str,"VEFreePatternFiles");if ( (VEFreePatternFiles  = (tVEFreePatternFiles )ImportSymbol(GetNLMHandle(),line)) == NULL ) return P_INVALID_VIRUS_ENGINE;
    sssnprintf(line,sizeof(line),"%s_%s",str,"VELoadSignature");if ( (VELoadSignature  = (tVELoadSignature )ImportSymbol(GetNLMHandle(),line)) == NULL ) return P_INVALID_VIRUS_ENGINE;
    sssnprintf(line,sizeof(line),"%s_%s",str,"VEUnloadSignature");if ( (VEUnloadSignature  = (tVEUnloadSignature )ImportSymbol(GetNLMHandle(),line)) == NULL ) return P_INVALID_VIRUS_ENGINE;
    sssnprintf(line,sizeof(line),"%s_%s",str,"VEEnableSignature");if ( (VEEnableSignature  = (tVEEnableSignature )ImportSymbol(GetNLMHandle(),line)) == NULL ) return P_INVALID_VIRUS_ENGINE;
    sssnprintf(line,sizeof(line),"%s_%s",str,"VEEnumSignatures");if ( (VEEnumSignatures  = (tVEEnumSignatures )ImportSymbol(GetNLMHandle(),line)) == NULL ) return P_INVALID_VIRUS_ENGINE;
    sssnprintf(line,sizeof(line),"%s_%s",str,"VEGetVirusInfo");if ( (VEGetVirusInfo  = (tVEGetVirusInfo )ImportSymbol(GetNLMHandle(),line)) == NULL ) return P_INVALID_VIRUS_ENGINE;
    sssnprintf(line,sizeof(line),"%s_%s",str,"VESetPattManParameters");if ( (VESetPattManParameters  = (tVESetPattManParameters )ImportSymbol(GetNLMHandle(),line)) == NULL ) return P_INVALID_VIRUS_ENGINE;
    sssnprintf(line,sizeof(line),"%s_%s",str,"VEGetSignatureVirusCount");if ( (VEGetSignatureVirusCount  = (tVEGetSignatureVirusCount )ImportSymbol(GetNLMHandle(),line)) == NULL ) return P_INVALID_VIRUS_ENGINE;
#endif
    return 0;
}

/************************************************************************************************/
void shutdowner(void *nothing)
{

//      DWORD dwType;
//      DWORD cbData; //,cc;
//      HKEY hkey;
//      DWORD val;

    REF(nothing);

    NTxSleep(2000);
    RenameThread(NTxGetCurrentThreadId(),"RTV Shutdown");
/*
        if (RegOpenKey(HKEY_LOCAL_MACHINE, REGHEADER, &hkey) == ERROR_SUCCESS) {
                char path[IMAX_PATH];
                FILE *file;
                cbData = IMAX_PATH;
                SymSaferRegQueryValueEx(hkey, "Home Directory", NULL, &dwType,(LPVOID)path, &cbData);
                strcat(path, sSLASH "rss.cmd");
                file = fopen(path,"wt");
                if (file) {
                        fputs("PAUSE\n",file);
                        fclose(file);
                        }
                cbData = IMAX_PATH;
                SymSaferRegQueryValueEx(hkey, "Home Directory", NULL, &dwType,(LPVOID)path, &cbData);
                strcat(path, sSLASH "rss.nlm");
                RegCloseKey(hkey);
                _LoadLibrary(path,true);
                }
*/
    // --------------------------------------
    // ksr - NetWare Certification, 10/8/2002

//     unload();
    UnloadSavServer();

    // --------------------------------------

    // ---------------------------------------
	  //this thread might have been suspended when we started unloading to let
	  //the unload functions do their job. Now we should resume it to finish unloading 
	  //rtvscan. defect 1-ZRBR7
     if (MainThread) {
          ResumeThread(MainThread);
          }

    // ---------------------------------------

}
/************************************************************************************/
DWORD BeginShutdown(void)
{

    return BeginThreadGroup(shutdowner,NULL,STACK_SIZE,NULL)==0xffffffff?ERROR_BAD_THREAD_START:ERROR_SUCCESS;
}
/************************************************************************************/
void DoRemove(void)
{
#ifdef LINUX
//do we need to do anything to remove? when the rpm remove is done
#else
    char path[IMAX_PATH];

    sssnprintf(path,sizeof(path),"%s" sSLASH "VPSTART.NLM /REMOVE",HomeDir);

    RenameThread(NTxGetCurrentThreadId(),"RTV Remover");

    NTxSleep(3000);

    // --------------------------------------
    // ksr - NetWare Certification, 10/8/2002

//     unload();
    UnloadSavServer();

    // --------------------------------------

    LoadLibrary(path);

    if ( MainThread )
        ResumeThread(MainThread);
#endif
}
/*********************************************************************/
DWORD BeginRemove(void)
{

    return BeginThreadGroup((THREAD)DoRemove,NULL,STACK_SIZE,NULL)==0xffffffff?ERROR_BAD_THREAD_START:ERROR_SUCCESS;
}
/*********************************************************************/
DWORD RTShutdown(HANDLE *hDevice)
{

    if ( hDevice )
        *hDevice = 0;

    DriverUnload();
    return 0;
}
/************************************************************************************/
DWORD RTStartup(HANDLE *hDevice)
{

    if ( hDevice )
        *hDevice = 1;
    return DriverStart();
}
/**********************************************************************************/
void CloseNLMStuff(void)
{
#ifdef LINUX
//nothing needed here
#else
    DWORD ret;

    if ( !NLMRunning ) return;
    NLMRunning=0;

    if ( ConsoleRunning )
    {
#ifdef NLM        
		  CloseConsoleScreen();
#endif
    }
    ConsoleRunning = 0;
	if(ExVolHandle)   // gdf 02/08/2003 fix defect 1-LLE9R
	{   // gdf 02/08/2003 fix defect 1-LLE9R
		close(ExVolHandle);  // gdf 02/08/2003 fix defect 1-LLE9R
		ExVolHandle = 0;  // gdf 02/08/2003 fix defect 1-LLE9R
	}   // gdf 02/08/2003 fix defect 1-LLE9R

#ifdef EnableScreenSaver
    ScreenSaveRunning=0;
#endif

    ThreadSwitchWithDelay();

    if ( debug&DEBUGPRINT )
        NTxSleep(1000);

    if ( ThreadCount )
    {
        int i;
        NTxSleep(500);
        for ( i=0;i<(20*60)&&ThreadCount;i++ )
            NTxSleep(55);
    }

    dprintf("Thread Count: %d\n",ThreadCount);

// ksr - 6/18/02 - memory leak fix, sts # 381005
#ifdef ThreadCheck
    if ( TrackThreads )
    {
        _printf("We have %d unreleased Threads!",ThreadCount);
        Breakpoint(ThreadCount);
    }

    ThreadReport(FALSE);
#endif // ThreadCheck

    if ( EventHandle )
    {
        ret=UnregisterForEvent(EventHandle);
        if ( ret )
        {
            dprintf ("UnregisterForEvent returned error: (0x%x)(%d)\n",ret,ret);
        }
        else
            EventHandle=0;
    }

//      dprintf ("Done Closing NLM Stuff\n");

    if ( NlmPChkey )
    {
        RegCloseKey(NlmPChkey);
        NlmPChkey = NULL;
        gRTVdoRegDebug = FALSE;
    }

    debug = 0;
#endif
}
/**********************************************************************************/
extern "C" int UnloadCheck(void)
//int UnloadCheck(void)
{
#ifdef LINUX
//we might use this
    return(1);
#else
    int tmpTGID;
    DWORD OldScreenID,NewScreenID;

    tmpTGID = SetThreadGroupID(MainTgID);
    _printf(LS(IDS_CANNOT_UNLOAD));
    OldScreenID = GetCurrentScreen();

    if ( 1||OldScreenID )
    {
        /* if OldScreenID == 0 and we call CreateScreen we will abend */
        NewScreenID = CreateScreen("System Console",0);

        if ( OldScreenID != NewScreenID )
            (void)SetCurrentScreen(NewScreenID);
        (void)ungetch('n');
        if ( OldScreenID != NewScreenID )
            (void)SetCurrentScreen(OldScreenID);
    }

    (void)SetThreadGroupID(tmpTGID);
    return(1);
#endif
}
/**********************************************************************************/

// --------------------------------------
// ksr - NetWare Certification, 10/8/2002

//void unload()
void UnloadSavServer( void )
// --------------------------------------
{
//      StopRegistryEditor();
	RegSaveDatabase();
    DeInitPscan();
    CloseNLMStuff();
}

/************************************************************************************/
#ifndef LINUX
void CheckLDSM(void)
{

        #define LD_AUTO NW_SYSTEM_DIR sSLASH "LD_AUTO."
        #define LineLen 200

    HANDLE nts;
    char line[LineLen], *cp;
    FILE *in, *out;
    char disabeled = 0, modified = 0;

    nts = _LoadLibrary("NTS.NLM",false);
    if ( nts )
    {

        memset(line, 0, LineLen);
        strncpy(line, (char *)nts + 0x6D, *((char *)nts + 0x6C)-1);
        if ( strstr(line, "LANDesk") )
        {

            _FreeLibrary("NW4AGNT.NLM");
            _FreeLibrary("SMCTRL.NLM");
            _FreeLibrary("NTS.NLM");
            _FreeLibrary("POLLER.NLM");

            //      edit LD_AUTO.NCF
            in = fopen(LD_AUTO "NCF", "rt");
            if ( !in )
                goto Return;

            out = fopen(LD_AUTO "TMP", "wt");
            if ( !out )
            {
                fclose(in);
                goto Return;
            }

            while ( fgets(line, LineLen, in) )
            {

                cp = line;
                while ( *cp && isspace(*cp) )
                    cp ++ ;

                if ( !strnicmp(cp, "REM ", 4) )
                    fputs(line, out);

                else if ( strstr(line, "load ntsp") )
                {
                    fclose(in);
                    fclose(out);
                    DeletePurgeFile(LD_AUTO "TMP");
                    goto Return;
                }

                else if ( 
                        strstr(line, "load nts") ||
                        strstr(line, "load smctrl") ||
                        strstr(line, "load nw3agnt") ||
                        strstr(line, "load nw4agnt") ||
                        strstr(line, "load poller") )
                {
                    fprintf(out, "REM %s", line);
                    modified=1;
                }

                else
                    fputs(line, out);
            }

            fclose(in);
            fclose(out);
            DeletePurgeFile(LD_AUTO "BAK");

            if ( modified )
            {
                if ( !MoveFile(LD_AUTO "NCF", LD_AUTO "BAK") ||
                     !MoveFile(LD_AUTO "TMP", LD_AUTO "NCF") )
                {
                    if ( !access(LD_AUTO "BAK", 0) )
                        MoveFile(LD_AUTO "BAK", LD_AUTO "NCF");

                    DeletePurgeFile(LD_AUTO "NEW");
                }
                else
                    disabeled=1;
            }
            else
                DeletePurgeFile(LD_AUTO "TMP");

            Return:         _printf(LS(disabeled?IDS_LDSM_DISABLED:IDS_LDSM_UNLOADED));
            _printf(LS(IDS_LDSM_UPDATE));
        }
    }
}
#endif //LINUX
/************************************************************************************/
void ServerDown(LONG parameter)
{

    REF(parameter);

    PutVal(hMainKey,"Status",0);
}
/*************************************************************************************/
void KeepAlive(void)
{
// Netware has a bug, SUPPRIZE!!! this keeps a thread in our thread group list, thus preventing NW from
// calling exit because of a ExitThread.
    RenameThread(NTxGetCurrentThreadId(),"RTV Keeper");
    SuspendThread(NTxGetCurrentThreadId());
}
/*************************************************************************************/
DWORD CheckInstall(void)
{
   DWORD cc = RegOpenKey(HKEY_LOCAL_MACHINE,REGHEADER,&hMainKey);
    char HomeDirectory[IMAX_PATH];

    if ( cc == ERROR_SUCCESS )
    {
        GetStr(hMainKey,"Home Directory",HomeDirectory,sizeof(HomeDirectory),"");

        if ( HomeDirectory[0] == 0 )
            cc = 1;
    }

    return cc;
}
/*************************************************************************************/
#define CFNL {  if (CheckForNoLoad(0)) {ExitingNormally=FALSE;goto done;}}
///////////////////////////////////////////////////////////////////////////
// main():
//
//      This function is the starting code for RTVScan.NLM.
//      It does the following things:
//              1)  Verify that we are running on a version of Netware that we support.
//              2)  Set the current working directory to where RTVScan.NLM is located.
//              3)  Start up a suspended thread to keep this process alive.
//              4)  Initialize PSCAN
//              5)  Maintain the NAV netware console UI while RTVScan.NLM is running.
//              6)  DeInitialize PSCAN, if it hasn't been.
//
// void main(
//      int argc,
//      char *argv[] )
//
// Parameters:
//
// Returns:
//
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

int main(int argc,char *argv[])
{

    DWORD _debug, cc=0;
    int i=0;
    char brk=0,EnglishOnly=0;
#ifdef LINUX
    char *arg;
#endif
#ifdef NLM
    FILE_SERV_INFO fsinf;
#endif
    char *q=NULL,*p=NULL,LogStartStop[256];
    time_t t,starttime,endtime;

    char ExVolPath[IMAX_PATH];

#ifdef MemCheck
    char* certPtr=NULL;
#endif

#ifdef NLM
    // rectify argv[0] if launched from system because of secure console 

    if ( argc == 3 && !strcmp( argv[1], "/SECURE_CONSOLE" ) )
    {
        argv[0] = argv[2]; argc -=2;
    }
    if ( argc == 4 && !strcmp( argv[2], "/SECURE_CONSOLE" ) )
    {
        argv[0] = argv[3]; argc -=2;
    }
#endif
    memset (LogStartStop,0,sizeof(LogStartStop));
#ifndef LINUX
    MainTgID = GetThreadGroupID();
#endif
    //This tells the RegDebug() function to pickup debug flags in the registry.
    gRTVdoRegDebug = TRUE;

#ifdef ThreadCheck
    ResetThreadTracking();
#endif // ThreadCheck

    RenameThread(NTxGetCurrentThreadId(),"RTV Main");

#ifdef MemCheck
    for ( i=0; i<3; i++ )
    {
        certPtr = (char *)malloc(64);
        if ( certPtr )
        {
            free(certPtr);
        }

        certPtr = (char *)calloc(1, 64);
        if ( certPtr )
        {
            char *temp = (char *)realloc( certPtr, 128 );
            if ( temp )
            {
                free( temp );
            }
            else
            {
                free( certPtr );
            }
        }

        certPtr = (char *)realloc( NULL, 64 );
        if ( certPtr )
        {
            free( certPtr );
        }
    }

#endif

#ifdef NLM
    //Get version information about this netware server we are running on.
    GetServerInformation (sizeof(fsinf),&fsinf);
    NWversion=fsinf.netwareVersion;
    NWsubVersion = fsinf.netwareSubVersion;
    //Get the maximum number of volumes supported by this server.
    MaxVolumes   = fsinf.maxVolumesSupported;    // gdf 06/14/00;

    //Record the version information of this netware server
    CLibVersion=(fsinf.CLibMajorVersion*100 + fsinf.CLibMinorVersion)*100;
    if ( fsinf.CLibRevision > 0x60 )
        CLibVersion += fsinf.CLibRevision - 0x60;

    //The following lines (approximately 20 lines) of code will extract
    //the program directory of RTVScan.NLM from argv[0]
    //Example:  argv[0] = "SYS:\\SYSTEM\\NAV\\RTVSCAN.NLM"
    //then ProgramDir = "SYS:\\SYSTEM\\NAV"

    //Get a pointer to the last '\\' character
    q = StrRChar(argv[0],'\\');
    //Get a pointer to the last '/' character
    p = StrRChar(argv[0],'/');

    //Find the pointer that is closest to the end of the string and set q to it.
    q = (q>p) ? q : p;

    if ( q )
    {
        //Zero the last '\\' character in argv[0]
        *q = 0;

        //Record ProgramDir from argv[0]
        StrCopy (ProgramDir,argv[0]);

        //Extract the volumn name from argv[0]
        q=StrChar (argv[0],':');
        if ( q )
        {
            //NULL the ':' in argv[0]
            *q=0;
            //copy the just the volume name
            StrCopy (VolumeName,argv[0]);
        }
        else
            //Assume SYS as volume name, if no ':' in PragramDir
            StrCopy (VolumeName,"SYS");
    }
    else
    {
        //If a '\\' or '/' is not found in argv[0]
        //then
        //Default VolumeName is "SYS"
        StrCopy (VolumeName,"SYS");
        //Default ProgramDir is "SYS:\\SYSTEM"
        StrCopy (ProgramDir,NW_SYSTEM_DIR);
    }

#endif

    //Delete VPDebug.log, so we start with a new one each time
    sssnprintf (DebugLogFile, sizeof(DebugLogFile), "%s" sSLASH "VPDebug.log", ProgramDir);
    DeletePurgeFile (DebugLogFile);

#ifdef MemCheck
    StartMemLog();
#endif // MemCheck
#ifdef SemaCheck
    StartSemaLog();
#endif // SemaCheck

#ifdef NLM
    dprintf("CLib Version = %d\n", CLibVersion);
    //Copying server name: Assume server name is 50 chars or less
    strncpy (FileServerName,fsinf.serverName,50);

    //Locate ':' in ProgramDir string
    //Why didn't we do this check earlier?
    //ProgramDir = SYS:\\NAV
    p = StrChar (ProgramDir, ':');
    if ( p )
    {
        //add '\\' after ':' is there isn't one
        if ( *(p+1) != cSLASH )
        {
            memmove(p+2, p+1, strlen(p));
            *(p+1) = cSLASH;
        }
    }
    //exit if no ':' in ProgramDir
    else
    {
        ExitingNormally=FALSE;
        goto done;
    }

    //Change the current working directory to ProgramDir
    chdir (ProgramDir);

    //Record the current console screen.  Later we will restore it.
    SetAutoScreenDestructionMode(DONT_CHECK_CTRL_CHARS|AUTO_DESTROY_SCREEN);
    MainScreenHan=GetCurrentScreen();

    // --------------------------------------
    // ksr - NetWare Certification, 10/8/2002

//    signal(SIGTERM, unload);     // run the unload in My Thread... (4.10 needs this)

//4.x no longer supported
//    signal( SIGTERM, UnloadSavServer );     // run the unload in My Thread... (4.10 needs this)
    atexit( UnloadSavServer );
	 AtUnload( UnloadSavServer );            // unload command

    // --------------------------------------------


    //We do not want to run on a server with System Fault Tolerant level 3 and the SFTIII IO engine.
    //If so, we don't load.
    if ( fsinf.SFTLevel==3 )
    {
        //Possible configuration types:  0=Normal, 1=IOEngine(SFTIII), 2=MSEngine(SFTIII)
        if ( GetServerConfigurationType()==1 )
        {
            _printf ("Cannot load Symantec AntiVirus in the I/O engine of SFTIII servers\n");
            ExitingNormally=FALSE;
            goto done;
        }
    }

    //////////////////////////////////////
    //The following lines of code will be parsing the commandline parameter of RTVScan.NLM.
    //This for loop is for commandline parsing.
    for ( i=1;i<argc;i++ )
    {
        //CAPITALIZE all arguments
        strupr(argv[i]);

        //This rel allows other processes on the Netware server to run also.
        ThreadSwitchWithDelay();

        //For each loop i, check the first char of string argv[i]
        switch ( argv[i][0] )
        {

            //BREAK - This case means we want to stop at very Breakpoint(brk)
            case 'B':
//This check is only for DEBUG builds because when want
//to be more strict to know the exact intent of the parameter.
#ifndef DEBUG
                if ( StrEqual(argv[i],"BREAK") )
#endif // DEBUG
                {
                    brk = 1;
                    continue;
                }
                break;

                //DEBUG - This case means to take debug flags from the commandline.
            case 'D':
                //Get debug flags
                if ( StrNEqual(argv[i],"DEBUG=",6) )
                {
                    strcpy(gszRTVDebug, argv[i]+6);
                    InitializeDebugLogging(gszRTVDebug);
                    //This means RegDebug() will ignore debug flags from the registry,
                    //since we have already set our debug flags from the commandline.
                    gRTVdoRegDebug = FALSE;
                    continue;
                }
                break;

                //ENGLISH ONLY - This case means we will be using the English string table.
                //If this is not set, we will set the string table according to
                //the language of the server we're running on.
            case  'E':
                if ( StrEqual(argv[i],"ENGLISH") )
                {
                    EnglishOnly=TRUE;
                    continue;
                }
                break;

#ifdef MemCheck

                //MEMORY CHECKING ONLY - This case means we want to track every memory allocation
                //by printing to our debug out.
            case 'M':
#ifndef DEBUG
                if ( StrEqual(argv[i],"MEMCHECK") )
#endif// !DEBUG
                {
                    TrackMallocs = TRUE;
                    continue;
                }
                break;
#endif // MemCheck


                //ASSUMMING: NO VOLUME or MEMORY SCANNING - This case means
                //no memory scanning or no volume name checking.
            case 'N':
                if ( StrEqual(argv[i],"NO_VOLUME_CHECK") )
                {
                    //Setting CheckVolumes to 0 means we will not
                    //be setting the names of the volumes in the VolumeTable.
                    CheckVolumes = 0;
                    continue;
                }
                if ( StrEqual(argv[i],"NOMEMSCAN") )
                {
                    //No memory scanning
                    ScanMemory = 0;
                    continue;
                }
                break;

//              case 'R':
//                      break;

#ifdef SemaCheck

                //SEMACHECK - This case means to track semaphores
                //by outputting semaphore info to debug screen
                //else the debug log if unloading of RTVScan.NLM.
            case 'S':
#ifndef DEBUG
                if ( StrEqual(argv[i],"SEMACHECK") )
#endif // !DEBUG
                {
                    TrackSemaphores = TRUE;
                    continue;
                }
                break;
#endif
#ifdef ThreadCheck

                //THREADCHECK - This case means to track thread information
                //and provide this information in the debug screen,
                //else the debug log if during unload of RTVScan.NLM.
            case 'T':
#ifndef DEBUG
                if ( StrEqual(argv[i],"THREADCHECK") )
#endif // !DEBUG
                {
                    TrackThreads = TRUE;
                    continue;
                }
                break;
#endif // ThreadCheck
        }
        ///////////////end of switch////////////////

        _printf(LS(IDS_BAD_OPTION),argv[i]);
        exit(0);
    }
    ///////////////end of for loop////////////////
#elif LINUX
    for ( i=1;i<argc;i++ )
    {
        strupr(argv[i]);

        ThreadSwitchWithDelay();

        arg = argv[i];
        if ( *arg == '\\' || *arg == '/' )
            arg++;

        switch ( arg[0] )
        {
        //BREAK - This case means we want to stop at very Breakpoint(brk)
        case 'B':
//This check is only for DEBUG builds because when want
//to be more strict to know the exact intent of the parameter.
#ifndef DEBUG
            if ( StrEqual(argv[i],"BREAK") )
#endif // DEBUG
            {
                brk = 1;
                continue;
            }
            break;

            //DEBUG - This case means to take debug flags from the commandline.
        case 'D':
            //Get debug flags
            if ( StrNEqual(argv[i],"DEBUG=",6) )
            {
                strcpy(gszRTVDebug, argv[i]+6);
                InitializeDebugLogging(gszRTVDebug);
                //This means RegDebug() will ignore debug flags from the registry,
                //since we have already set our debug flags from the commandline.
                gRTVdoRegDebug = FALSE;
                continue;
            }
            break;


            case  'E':
                if ( StrEqual(arg,"ENGLISH") )
                {
                    EnglishOnly=TRUE;
                    continue;
                }
                break;
        }

        _printf(LS(IDS_BAD_OPTION),arg);
        goto done;
    }
    ///////////////end of for loop////////////////
#endif //LINUX

    //See if we have any debug flags in the registry
    RegDebug(/*FALSE*/);

    //This is used for debugging.
    //We would used this breakpoint to see if we
    //have properly parse the commandline parameters.
    Breakpoint(brk);

#ifdef NLM
    //This mean we want to be notified when the server is going down.
    //The server will call ServerDown().
	// typedef added to keep CW compiler happy.
	
	typedef void (*PFV)(LONG, LONG);
	EventHandle = RegisterForEvent((LONG)EVENT_DOWN_SERVER, PFV (ServerDown), (LONG)0);
#endif
    //Check that NAV is installed.
    //CheckInstall() succeeds if the Home Directory registry value is not empty.
    cc = CheckInstall();
    if ( cc != ERROR_SUCCESS )
    {
        _printf(LS(IDS_INVALID_SETUP1));
        _printf(LS(IDS_INVALID_SETUP2));
        _printf(LS(IDS_INVALID_SETUP3));
        goto done;
    }
    //Log that we are starting
    if ( debug&DEBUGLOG )
    {
        t=time(NULL);
        sssnprintf(LogStartStop, sizeof(LogStartStop), "\n\nRTVSCAN.NLM is Starting at %s", ctime(&t));
        LogLine(LogStartStop, TRUE);
    }
#ifdef NLM
    //Restore the server console UI to its original screen that we removed earlier.
    SetCurrentScreen (MainScreenHan);
    DisplayScreen (MainScreenHan);
    //Check if the user wants us to exit.
    CFNL
#endif

    //Set the proper string table for us to use.  If EnglishOnly == FALSE,
    //we will load the string table for the string table for the current OS.
    if ( OpenStringTable(EnglishOnly) != ERROR_SUCCESS )
    {
        ExitingNormally=FALSE;
        goto done;
    }

    //show our version and build number
	sssnprintf(NewVersionString,sizeof(NewVersionString),"%u.%u.%u.%u",MAINPRODUCTVERSION,SUBPRODUCTVERSION,INLINEPRODUCTVERSION,BUILDNUMBER);
//      SetString(IDS_VERSION,NewVersionString);
    printf ("%s - ",NewVersionString);

    //Show "Please wait, Symantec AntiVirus initializing\n"
    printf(LS(IDS_NLM_LOADING));

    ThreadSwitchWithDelay();

#ifdef NLM
    //Check if the user wants us to exit. If '~' is typed on the keyboard.
    CFNL
    //Make sure we are running for a netware server newer than version 3.12g
    if ( CLibVersion < 31207 /* v3.12g */ )
    {
        ExitingNormally=FALSE;
        _printf("%s\r\n  %s\r\n  %s\r\n",LS(IDS_UPDATING_CLIB1),LS(IDS_UPDATING_CLIB2),LS(IDS_UPDATING_CLIB3));

        goto done;
    }

    CheckLDSM();
#endif
    //Create a suspended thread.
    //If we keep one thread suspended in our thread group list,
    //thus preventing NW from calling exit because of a ExitThread.
    BeginThreadGroup((THREAD)KeepAlive,NULL,STACK_SIZE,NULL);

	memset(ExcludedVols, 0, VOLUMES*32);          // gdf 01/20/2003 fix defect 1-LLE9R
	memset(ExcludedVolsScratch, 0, VOLUMES*32);   // gdf 01/20/2003 fix defect 1-LLE9R
	//use sav home directory for ExVol.dat file
    sssnprintf(ExVolPath, sizeof(ExVolPath), "%s" sSLASH "%s", ProgramDir, EXVOL_DAT);
	ExVolHandle = open(ExVolPath, O_RDWR | O_CREAT | O_BINARY, 0); // gdf 02/08/2003 fix defect 1-LLE9R
	if(ExVolHandle)  // gdf 02/08/2003 fix defect 1-LLE9R
		close(ExVolHandle);  // gdf 02/08/2003 fix defect 1-LLE9R

    cc = InitPscan();
    //Record our start time
    starttime=time(NULL);

#ifdef ThreadCheck
    ThreadReport(FALSE);
#endif // ThreadCheck

    if ( cc )
    {
        _printf(LS(IDS_ERROR_LDVP_START),cc,GetErrorText(cc));

        // --------------------------------------
        // ksr - NetWare Certification, 10/8/2002

        dprintf( "InitPscan() returned = %d in main()\n", cc );  

/*  gdf 12/04/2002 fix 1-PG4HF.  Commmented out branch to "done:".
 *  Exiting in this manner bypasses a call to DeInitPscan resuting
 *  in unreleased threads causing a failure of RTVSCAN.NLM and I2_LDVP.NLM to
 *  exit.
*/
        // gdf fix 1-PG4HF      goto done;

        // --------------------------------------               
    }
    else
    {
        _printf(LS(IDS_LDVP_INSTALLED));
        ConsoleRunning = 1;
        //Update virus definition - by copying the new pattern file to the local login directory (%VPHOME%\Login)

        // --------------------------------------------
        // ksr - NetWare Certification, 8/17/2002

        // UpdateLoginPatternFile();
#ifdef NLM

        if ( UpdateLoginPatternFile() == ERROR_MEMORY )
        {
            dprintf( "\nUpdateLoginPatternFile() returned %d in main()", ERROR_MEMORY );            
            return -1; 
        }
        // --------------------------------------------
#endif
        //Check if the user wants us to exit. If '~' is typed on the keyboard.
        if ( !CheckForNoLoad(3000) )
        {
				//Bring up our netware console UI.  A return from this call means
            //we are in the process of unloading RTVScan.NLM
            StartConsoleScreen();
#ifdef LINUX_DEMO
printf("shutdown command issued\n");
#endif
            NTxSleep (500);
        }

        if ( !SystemRunning )
            //We have been unloaded. Let the unload function do the cleanup. just go to sleep here
            //This call will block because I am suspending myself.
            //The code responsible for unloading is also responsible for resuming this thread.
            SuspendThread(MainThread=NTxGetCurrentThreadId());

        if ( !SystemRunning )
            //DeInitialization has already been completed,
            //so we are done.
            goto done;
    }
    //Record our end time
    endtime = time (NULL);
    //If we have not been running for more than MIN_RUN_TIME, currently MIN_RUN_TIME = 10 (seconds)
    //then we will wait a little to give the startup process a chance to complete.
    if ( endtime-starttime < MIN_RUN_TIME )
        // because if we try to DeInit too soon we will hang waiting for threads to die.
        NTxSleep (((MIN_RUN_TIME+1)-(endtime-starttime))*1000);

#ifdef ThreadCheck
    ThreadReport(FALSE);
#endif // ThreadCheck

    DeInitPscan();

    done:

#ifdef ThreadCheck
    if ( TrackThreads )
        ThreadReport(TRUE);
#endif // ThreadCheck

    CloseStringTable();
    _debug = debug&(DEBUGLOG|DEBUGSAFELOG);
    CloseNLMStuff(); // debug is now 0
    if ( _debug&DEBUGLOG )
    {
        t=time(NULL);
        sssnprintf(LogStartStop,sizeof(LogStartStop),"RTVSCAN.NLM is Unloading at %s\n",ctime (&t));
        debug = _debug;
        LogLine(LogStartStop,TRUE);
        debug = 0;
    }

#ifdef SemaCheck
    if ( TrackSemaphores )
        ReportSemaUsage(TRUE);
#endif // SemaCheck
#ifdef MemCheck
    if ( TrackMallocs )
        ReportMemoryUsage(TRUE);
#endif // MemCheck

    exit(0);
}
/**********************************************************************************************/

BOOL CheckForNoLoad(int time)
{

    char c;

    NTxSleep (50);
    if ( kbhit() )
    {
        c=getch();
        if ( toupper(c)=='~' )
        {
            _printf ("Aborting RTVScan Load\n");
            NTxSleep (time);
            return TRUE;
        }
    }

    return FALSE;
}
/****************************************************************************/
//void StartBox(PINFECTION Inf)
void StartBox(PEVENTBLOCK eb)
{
    if ( eb->Special )
        _printf("%s%c\r\n\r\n",(char *)eb->Special,_GetVal(eb->hKey,"Beep",1)?'\a':' ');
}
