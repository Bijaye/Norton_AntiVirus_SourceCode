// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#include "regedit.h"
#include "nts.h"
#include "symnts.h"
#include "winThreads.h"
#include "SymSaferRegistry.h"
#include "SymSaferStrings.h"

#ifdef NLM
#include "FineTimeImplementation.c"
#endif

/***************************************************************************************/
#include "acta.c"

void ClearStrings (void);   // gdf CW conversion
void PrintSubkeys (void);   // gdf CW conversion
void PrintValues(void);     // gdf CW conversion
void SetStatus (char *msg);   // gdf CW conversion
void SetError (char *msg);    // gdf CW conversion
void StopRegistryEditor (void);    // gdf CW conversion
void EditRegistry(void *nothing);  // gdf CW conversion

extern "C" BOOL StrEqual(char *s1,char *s2); // returns TRUE if the strings are the same

static void UnloadRegistry ();
inline void UnloadRegistryArg (int nSig) {
	(void)nSig;
	UnloadRegistry();
}

#define ERROR_CLOSE_ROOT 1000
#define ERROR_VAL_NAME   1001
#define ERROR_BAD_TYPE  1002

int RegEditThreadGroup=0;
HKEY parentKey[KEYPATHLEN] = {0};   // Array for open key path handles
int prevMsgLen=0;                   // length of previously printed message on status line
int openKey=0;                      // Index of the current open key
BOOL RegEditRunning=FALSE;          // so only one copy of RegEdit will run at a time
BOOL RegEditThreadActive=FALSE;
char keyPath[KEYPATHLEN][KEYLEN] = {0};// Array for open key path names
char status[80] = "";               // The current status message
char RegistryRunning = 1;
extern int RegTGID=0;

typedef char KEY_ARRAY[][KEYLEN];   // array for current key subkey names - dynamically allocated
KEY_ARRAY *subKeys = NULL;          // this took a while to figure how to spell - a pointer to a
                                    // multidimensional array of incomplete type

// for listing the subkeys
LONG keyPort=0;
PCB *keyPCB=NULL;
int keyLine=0;
int numKeys=0;
#define keyWinHeight    17
#define keyWinWidth     25

// for listing the values
LONG valPort=0;
PCB *valPCB=NULL;
int valLine=0;
int numVals=0;
#define valWinHeight    17
#define valWinWidth     55

LONG rfuncPort=0;   // for printing valid kb commands
PCB *rfuncPCB=NULL;
LONG rstatPort=0;   // for displaying last message
PCB *rstatPCB=NULL;
LONG pathPort=0;    // for diplaying the key path
PCB *pathPCB=NULL;

int regScreenHan=0;
NUTInfo *regNutH=NULL;
LONG regAllocTag=0;
LONG regNLMHandle=0;

#define ERROR(e)    DisplayError(e)
#define STATUS(s)   SetStatus(LS(s))

BOOL regMenu=FALSE;

extern "C" BOOL vpRegChanged; // this is in vpreg.nlm

char RegProgramDir[255] = "";    // directory path where vpregedt.nlm was run from (including volume)
DWORD RegThreadCount=0;
int regNWversion=0;
char **RegStringTable = _CompiledStringTable;
char StartRegistryEditor = 0;

#ifdef LINUX
extern void* RegThreadID;
#else
int RegThreadID;
#endif

VOID Real_dprintf(const char *format,...);

#define dprintf     if (debug) Real_dprintf

DWORD debug=0;

/************************************************************************************/
void DisplayError(DWORD err)
{
    DWORD errMsg=0;
    char DefaultMsg[80];

    switch ( err )
    {
        case ERROR_SUCCESS:         errMsg=IDS_E_SUCCESS;       break;
        case ERROR_CLOSE_ROOT:      errMsg=IDS_E_CLOSE_ROOT;    break;
        case ERROR_VAL_NAME:        errMsg=IDS_E_VAL_NAME;      break;
        case ERROR_BAD_TYPE:        errMsg=IDS_E_BAD_TYPE;      break;
        case ERROR_NO_PATH:         errMsg=IDS_E_NO_PATH;       break;
        case ERROR_NO_MEMORY:       errMsg=IDS_E_NO_MEM;        break;
        case ERROR_SEMAPHORE_IN_USE:errMsg=IDS_E_SEMA_IN_USE;   break;
        case ERROR_BAD_DATABASE:    errMsg=IDS_E_BAD_DATA;      break;
        case ERROR_BAD_KEY:         errMsg=IDS_E_BAD_KEY;       break;
        case ERROR_NO_ROOM:         errMsg=IDS_E_NO_ROOM;       break;
        case ERROR_KEY_HAS_CHILDREN:errMsg=IDS_E_KEY_CHILDREN;  break;
        case ERROR_EMPTY:
        case ERROR_NO_DATABASE:
        default:    
            sssnprintf(DefaultMsg,sizeof(DefaultMsg),"Unknown Error: 0x%x (%d)",err,err);
            SetError(DefaultMsg);
            return;
    }
    SetError(LS(errMsg));
}
/************************************************************************************/
void regSetupNut (void)
{

    WORD ccode=0;
    dprintf ("reg Setup Nut\n");

    regNLMHandle = GetNLMHandle();
	regAllocTag = AllocateResourceTag (regNLMHandle,
									   reinterpret_cast<unsigned char*>(const_cast<char*>(REGEDIT_NAME" Alloc Tag")),
									   AllocSignature);

	ccode=NWSInitializeNut (MSGNUM(IDS_REG_TITLE),MSGNUM(IDS_REG_VER),SMALL_HEADER,
							NUT_REVISION_LEVEL,reinterpret_cast<unsigned char**>(RegStringTable),(BYTE *)(regNWversion == 4 ? -1 : NULL),regScreenHan,
							regAllocTag,&regNutH);

    if ( ccode!=ERROR_SUCCESS )
    {
        RingTheBell();
        dprintf ("%d.x Nut Initialization Failure:%d  "REGEDIT_NAME" could not start\n",regNWversion,ccode);
        RegEditRunning=FALSE; // this way we will not start running
        return;
    }
//  MyCreatePortal (*PORTAL,    *PCB        R   C   H   W   VH  VW      SAVE        HEADER  ATTRIBUTE   BORDER      ATTRIBUTE   NUTHANDLE
    MyCreatePortal (&pathPort,  &pathPCB,   1,  0,  1,  80, 1,  256,    NO_SAVE,    NULL,   VREVERSE,   NOBORDER,   VREVERSE,   regNutH);
    MyCreatePortal (&rstatPort, &rstatPCB,  23, 0,  1,  80, 1,  80,     NO_SAVE,    NULL,   VNORMAL,    NOBORDER,   VNORMAL,    regNutH);
    MyCreatePortal (&rfuncPort, &rfuncPCB,  24, 0,  1,  80, 1,  80,     NO_SAVE,    NULL,   VNORMAL,    NOBORDER,   VNORMAL,    regNutH);

    // changed these portals to be 0x7fffffff high instead of 128 - this is a virtual height, so
    // making it large doesn't seem to hurt anything, and then doesn't limit the number of
    // displayed keys

	MyCreatePortal (&keyPort,   &keyPCB,    2,  1,              keyWinHeight+4, keyWinWidth-1,0x7fffffff,256,NO_SAVE,reinterpret_cast<unsigned char*>(LS(IDS_SUBKEYS)),VNORMAL,DOUBLE,VNORMAL,regNutH);
	MyCreatePortal (&valPort,   &valPCB,    2,  keyWinWidth,    keyWinHeight+4, valWinWidth-1,0x7fffffff,256,NO_SAVE,reinterpret_cast<unsigned char*>(LS(IDS_VALUES)),VNORMAL,DOUBLE,VNORMAL,regNutH);
}
/************************************************************************************/
int regMenuAction (int option, void* stuff)
{

    stuff=stuff;

    if ( option==-1 ) return CMD_ESCAPE;
    return option;      // we examine this and act on the key pressed in CheckCom
}
/*****************************************************************************/
int regCommandMenu(void)
{

    int retval=0;

    dprintf ("reg Command Menu\n");

    regMenu=TRUE;
    NWSInitMenu (regNutH);

    NWSAppendToMenu (MSGNUM(IDS_ADD_KEY),NUM_ADD_KEY,regNutH);
    NWSAppendToMenu (MSGNUM(IDS_ADD_VAL),NUM_ADD_VALUE,regNutH);
    NWSAppendToMenu (MSGNUM(IDS_DEL_KEY),NUM_DELETE_KEY,regNutH);
    NWSAppendToMenu (MSGNUM(IDS_DEL_VAL),NUM_DELETE_VALUE,regNutH);
    NWSAppendToMenu (MSGNUM(IDS_OPEN_KEY),NUM_OPEN_SUBKEY,regNutH);
    NWSAppendToMenu (MSGNUM(IDS_CLOSE_KEY),NUM_CLOSE_KEY,regNutH);
    NWSAppendToMenu (MSGNUM(IDS_EDIT_DATA),NUM_EDIT_DATA,regNutH);
    NWSAppendToMenu (MSGNUM(IDS_REG_EXIT),NUM_QUIT_PROG,regNutH);

    retval=NWSMenu (MSGNUM(IDS_TITLE_CMD),17,12,NULL,regMenuAction,regNutH,(void*)regNutH);
    dprintf ("Done with NWSMenu\n");
    regMenu=FALSE;
    if ( !RegEditRunning ) return NUM_QUIT_PROG;
    NWSDestroyMenu(regNutH);

    dprintf ("EXITING: reg Command Menu\n");
    if ( /*!SystemRunning ||*/ !RegistryRunning || !RegEditRunning ) return NUM_QUIT_PROG;

    return retval;
}
/*****************************************************************************/
void PrintCurrKey (void)
{ // Prints the path of the current key on the screen

    DWORD k=0;
    char pathstr[512]={0},*pathptr=NULL,pathstr2[512]={0};

    strcpy (pathstr,LS(IDS_KEY_COLON));
    if ( !keyPath[1][0] )  // We are at the root
        strcat (pathstr,"\\ ");

    for ( k=1 ; keyPath[k][0] ; k++ )
    {   // add the full key path to the path string
        strcat (pathstr,"\\");
        strcat (pathstr,keyPath[k]);
    }
    StrCopy (pathstr2,pathstr);

    for ( pathptr=pathstr,k=0 ; NumBytes (pathstr2)>78 ; k++ )
    {  // if we are longer than can fit on the screen
       // truncate the front until we fit
        StrCopy (pathstr2,".."); // indicate that there are more keys at the front by putting ".." at the front

        while ( *pathptr == '\\' )        // skip past the current '\'
            pathptr=NextChar(pathptr);

        while ( *pathptr != '\\' )        // look for the next '\' to skip the next subkey
            pathptr=NextChar(pathptr);

//      pathptr += NumBytes (keyPath[k++])+1;

        strcat (pathstr2,pathptr);      // add the rest of the key path to the final path string
    }

	NWSDisplayTextInPortal (0,2,reinterpret_cast<unsigned char*>(LS(IDS_BLANK_LINE)),VREVERSE,pathPCB);
	NWSDisplayTextInPortal (0,0,reinterpret_cast<unsigned char*>(pathstr2),VREVERSE,pathPCB);
}
/************************************************************************************/
void regPortals()
{ // display the portals on the screen

    ClearStrings();

//dprintf ("display keys\n");
    NWSDrawPortalBorder (keyPCB);
    NWSClearPortal (keyPCB);
    PrintSubkeys();

//dprintf ("display values\n");
    NWSDrawPortalBorder (valPCB);
    NWSClearPortal (valPCB);
    PrintValues();

//dprintf ("display path\n");
    NWSClearPortal (pathPCB);
    PrintCurrKey();

//dprintf ("display status\n");
    NWSClearPortal (rstatPCB);
    NWSDisplayTextInPortal (0,0,reinterpret_cast<unsigned char*>(const_cast<char*>(">")),VREVERSE,rstatPCB);
    NWSDisplayTextInPortal (0,79,reinterpret_cast<unsigned char*>(const_cast<char*>("<")),VREVERSE,rstatPCB);
    NWSDisplayTextInPortal (0,1,reinterpret_cast<unsigned char*>(status),VNORMAL,rstatPCB);

//dprintf ("display functions\n");
    NWSClearPortal (rfuncPCB);
    NWSDisplayTextInPortal (0,0,reinterpret_cast<unsigned char*>(LS(IDS_REG_CMD_BAR)),VREVERSE,rfuncPCB);
//dprintf ("done displaying\n");
}
/************************************************************************************/
void ComAddKey ()
{

    char newKey[KEYLEN];
    DWORD ret=0;

    memset (newKey,0,KEYLEN);

    if ( (ret=NWSEditString (5,40,1,70,MSGNUM(IDS_ENTER_KEYNAME),MSGNUM(IDS_NAME),reinterpret_cast<unsigned char*>(newKey),KEYLEN,EF_ANY,regNutH,0,0,0)) != E_ESCAPE )
    {

        if ( NumBytes(newKey)<1 )
        {
            ERROR(ERROR_BAD_KEY);
            return;
        }
        if ( (ret=RegCreateKey (CURR,newKey,&parentKey[openKey+1])) == ERROR_SUCCESS )
        {
            openKey++;          // we have created a new key but we don't want it open yet
            RegCloseKey (CURR); // so close it
            CURR=0;             // clear out the entry in the parent key array
            openKey--;          // and return to the parent key
            STATUS(IDS_KEY_CREATED);
        }
        else
            ERROR(ret);
    }
    else
        STATUS(IDS_ADD_KEY_CANCELLED);
}
/************************************************************************************/
int StrDwoAction (LONG keyPressed,LIST **element,LONG *itemNum,void *listParm)
{
    // called from the first list in ComAddValue.  Used when user selects string/dword/binary type

    itemNum=itemNum;
    listParm=listParm;

    if ( keyPressed==M_ESCAPE )
        return CMD_ESCAPE;
    else
        return *((char*)(*element)->otherInfo);
}
/************************************************************************************/
void ComAddValue()
{

    char com=0,newVal[VALLEN],data[DATALEN],str='S',dwo='D',bin='B',*ptr=NULL;
    DWORD ret=0;

    memset (newVal,0,VALLEN);
    memset (data,0,DATALEN);

    // get the name of the new value
    if ( (ret=NWSEditString (5,40,1,40,MSGNUM(IDS_ENTER_VALNAME),MSGNUM(IDS_NAME),reinterpret_cast<unsigned char*>(newVal),VALLEN-1,EF_ANY,regNutH,0,0,0)) != E_ESCAPE )
    {
        if ( NumBytes(newVal)<1 )
        {
            ERROR(ERROR_VAL_NAME);  // user didn't enter a name or pressed escape
            return;
        }

        regMenu=TRUE;
        NWSInitList(regNutH,0); // for selecting the type of the new value
        NWSAppendToList(reinterpret_cast<unsigned char*>(LS(IDS_STRING)),&str,regNutH);
        NWSAppendToList(reinterpret_cast<unsigned char*>(LS(IDS_DWORD)),&dwo,regNutH);
        NWSAppendToList(reinterpret_cast<unsigned char*>(LS(IDS_BINARY)),&bin,regNutH);
        com = NWSList(MSGNUM(IDS_SELECT_TYPE),12,40,3,20,M_ESCAPE|M_SELECT|M_NO_SORT,NULL,regNutH,NULL,StrDwoAction,0);
        regMenu=FALSE;
        dprintf ("Done with NWSList\n");
        if ( !RegEditRunning ) return;

        NWSDestroyList (regNutH);

        if ( com == CMD_ESCAPE )
        { // user pressed esc so return without adding the value
            STATUS(IDS_ADD_VAL_CANCELLED);
            return;
        }

        if ( com == 'S' )
        {   // STRING
            // get the string from the user
            if ( (ret=NWSEditString (5,40,1,70,MSGNUM(IDS_ENTER_DATA),MSGNUM(IDS_DATA_STRING),reinterpret_cast<unsigned char*>(data),DATALEN-1,EF_ANY,regNutH,0,0,0)) != E_ESCAPE )
            {
                if ( (ret=RegSetValueEx (CURR,newVal,NULL,REG_SZ,reinterpret_cast<unsigned char*>(data),NumBytes(data)+1)) == ERROR_SUCCESS )
                    STATUS(IDS_DATA_ADDED);
                else
                    ERROR(ret);
            }
            else STATUS(IDS_DATA_NOT_ADDED);
        }
        else if ( com == 'D' )
        {  // DWORD
            DWORD Val=0;
            char Dstring[100];
            memset (Dstring,0,100);
            // get the string representation of the value of the dword
            if ( NWSEditString (5,40,1,70,MSGNUM(IDS_ENTER_DATA),MSGNUM(IDS_DWORD_VALUE),reinterpret_cast<unsigned char*>(Dstring),100,EF_SET,regNutH,0,0,const_cast<char*>("abcdefABCDEF0..9xX")) != E_ESCAPE )
            {
                if ( Dstring[0]=='0' ) // dword is in hex
                    Val = strtoul (Dstring,&ptr,16);
                else    // assume dword is decimal
                    Val = strtoul (Dstring,&ptr,10);
                if ( (ret=RegSetValueEx (CURR,newVal,NULL,REG_DWORD,(BYTE*)&Val,(DWORD)sizeof(Val))) == ERROR_SUCCESS )
                    STATUS(IDS_DWORD_ADDED);
                else
                    ERROR(ret);
            }
            else STATUS(IDS_DATA_NOT_ADDED);
        }
        else if ( com=='B' )
        {    // BINARY
            DWORD i=0;
            BYTE bindat[1024]={0};
            char Dstring[3000]={0},*tok=NULL;

            // get the string of binary numbers.  Assume the numbers are hex and separated by whitespace
            if ( NWSEditString (5,40,1,70,MSGNUM(IDS_ENTER_DATA),MSGNUM(IDS_BINARY_VALUE),reinterpret_cast<unsigned char*>(Dstring),3000,EF_SET,regNutH,0,0,const_cast<char*>("0..9A..Fa..f ")) != E_ESCAPE )
            {
                tok=strtok (Dstring," \n\t");
                if ( tok )
                    bindat[0]=strtoul (tok,&ptr,16);    // fill up the data array
                for ( i=1; (tok=strtok (NULL," \n\t"))!=NULL ; i++ )
                    bindat[i]=strtoul (tok,&ptr,16);

                if ( (ret=RegSetValueEx (CURR,newVal,NULL,REG_BINARY,bindat,i)) == ERROR_SUCCESS )
                    STATUS(IDS_BINARY_ADDED);
                else
                    ERROR(ret);
            }
            else STATUS(IDS_DATA_NOT_ADDED);
        }
        else
            ERROR(ERROR_BAD_TYPE);
    }
}
/************************************************************************************/
int OpenKeyAction (LONG keyPressed,LIST **element,LONG *itemNum,void *listParm)
{
// called from the list in ComOpen
    DWORD ret=0;

    itemNum=itemNum;
    listParm=listParm;

    if ( keyPressed==M_ESCAPE )
    {
        STATUS(IDS_OPEN_CANCELLED);
        return CMD_ESCAPE;
    }

    if ( (char*)((*element)->otherInfo) )
        dprintf ("%s\n",(char*)((*element)->otherInfo));

    if ( (ret=RegOpenKey (CURR,(char*)((*element)->otherInfo),&parentKey[openKey+1]))==ERROR_SUCCESS )
    {
        openKey++;
        StrCopy (keyPath[openKey],(char*)((*element)->otherInfo));
        STATUS(IDS_KEY_OPENED);
    }
    else
        ERROR(ret);

    return ret;
}
/************************************************************************************/
void ComOpen ()
{

    int k=0,n=0;

    dprintf ("Com Open\n");
    regMenu=TRUE;
    NWSInitList(regNutH,0); // create the list

    for ( k=0 ; k<numKeys ; k++ ) // add all the current subkeys to the list
        if ( (*subKeys)[k][0] )
        {
            NWSAppendToList(reinterpret_cast<unsigned char*>((*subKeys)[k]),&((*subKeys)[k]),regNutH);
            n++;
        }

    if ( n )
        NWSList(MSGNUM(IDS_SELECT_OPEN),(keyWinHeight/2)+1,13,keyWinHeight,20,M_ESCAPE|M_SELECT|M_NO_SORT,NULL,regNutH,NULL,OpenKeyAction,0);
    else
        SetError (LS(IDS_NO_SUBKEYS_OPEN)); // there are no subkeys in the current key
    dprintf ("Done with NWSList\n");
    regMenu=FALSE;

    if ( !RegEditRunning ) return;
    keyLine=valLine=0;

    NWSDestroyList (regNutH);
}
/************************************************************************************/
void ComClose ()
{

    DWORD ret=0;

    dprintf ("Com Close\n");

    if ( CURR==HKEY_LOCAL_MACHINE )
    {
        ERROR(ERROR_CLOSE_ROOT);
        return;
    }
    ret=RegCloseKey (CURR);

    CURR = 0;               // clear the parentKey array entry
    keyPath[openKey][0] = 0;// clear the keyPath array entry
    openKey--;              // move current to the parent
    STATUS(IDS_KEY_CLOSED);

    if ( ret != ERROR_SUCCESS )
        ERROR(ret);

    keyLine=valLine=0;
}
/************************************************************************************/
int DeleteKeyAction (LONG keyPressed,LIST **element,LONG *itemNum,void *listParm)
{  // Called from the list in ComDeleteKey

    DWORD ret=0;

    itemNum=itemNum;
    listParm=listParm;

    if ( keyPressed==M_ESCAPE )
    {
        STATUS(IDS_DELETE_CANCELLED);
        return CMD_ESCAPE;
    }
    if ( ((char*)(*element)->otherInfo) )
        dprintf ("Delete Key Action: %s\n",((char*)(*element)->otherInfo));

    // The key name to be deleted is passed in as otherInfo so call RegDeleteKey
    if ( (ret=RegDeleteKey (CURR,(char*)((*element)->otherInfo)))==ERROR_SUCCESS )
        STATUS(IDS_SUBKEY_DELETED);
    else
        ERROR(ret);

    return ret;
}
/************************************************************************************/
void ComDeleteKey ()
{

    int k=0,n=0;

    dprintf ("Com Delete Key\n");
    regMenu=TRUE;
    NWSInitList(regNutH,0);

    for ( k=0 ; k<numKeys ; k++ )
        if ( (*subKeys)[k][0] )
        {
            NWSAppendToList(reinterpret_cast<unsigned char*>((*subKeys)[k]),&((*subKeys)[k]),regNutH); // add the subkeys to the list
            n++;
        }

    if ( n )
        NWSList(MSGNUM(IDS_SELECT_DEL_KEY),(keyWinHeight/2)+1,13,keyWinHeight,20,M_ESCAPE|M_SELECT|M_NO_SORT,NULL,regNutH,NULL,DeleteKeyAction,0);
//      NWSList(MSGNUM(IDS_SELECT_DEL_KEY),8,14,15,24,M_ESCAPE|M_SELECT,NULL,regNutH,NULL,DeleteKeyAction,0);
    else
        SetError (LS(IDS_NO_SUBKEYS_DEL));
    dprintf ("Done with NWSList\n");
    regMenu=FALSE;
    if ( !RegEditRunning ) return;

    NWSDestroyList (regNutH);
}
/************************************************************************************/
int DeleteValAction (LONG keyPressed,LIST **element,LONG *itemNum,void *listParm)
{ // called from the list in ComDeleteValue

    DWORD ret=0;

    itemNum=itemNum;
    listParm=listParm;

    if ( keyPressed==M_ESCAPE )
    {
        STATUS(IDS_DELETE_CANCELLED);
        return CMD_ESCAPE;
    }

    if ( (Value*)(*element)->otherInfo )
        dprintf ("Delete Val Action: %s\n",((Value*)(*element)->otherInfo));

    if ( (ret=RegDeleteValue (CURR,(char*)((*element)->otherInfo)))==ERROR_SUCCESS )
        STATUS(IDS_VALUE_DELETED);
    else
        ERROR(ret);

    return ret;
}
/************************************************************************************/
void ComDeleteValue ()
{

    int v=0,n=0,i=0;
    DWORD type=0,sz=0,dsz=0;
    char valstr[VALLEN+DATALEN]={0},buf[10]={0};
    BYTE data[DATALEN]={0};
    Value *pVal=NULL,*head=NULL,*curr=NULL;

    dprintf ("Com Delete Value\n");

    dprintf ("Delete Value\n");
    regMenu=TRUE;
    NWSInitList(regNutH,0);

//dprintf ("1");    // enumerate all the values.  RegEnumValue returns SUCCESS until there are no more values
    for ( dsz=DATALEN,sz=VALLEN,v=0 ; (RegEnumValue(CURR,v,valstr,&sz,NULL,&type,data,&dsz)==ERROR_SUCCESS) ; v++,dsz=DATALEN,sz=VALLEN )
    {
//dprintf ("2");
        pVal = (Value*) malloc(sizeof(Value)); // allocate memory for it
        if ( !pVal ) break;

        memset(pVal,0,sizeof(Value));

        valstr[sz]=0;
        strcpy( pVal->name, valstr );           // fill in the fields

        if ( !head || !curr )                    // insert it into the linked list
            head = pVal;
        else
            curr->next = pVal;

        curr = pVal;

        memset (valstr,0,VALLEN+DATALEN);

        if ( dsz )
        {                  // the value has data associated with it
            data[dsz]=0;
            switch ( type )
            {          // extract the data into valstr
                case REG_SZ:
                    sssnprintf (valstr,sizeof(valstr),"%s:%s",pVal->name,data);
                    break;
                case REG_DWORD:
                    sssnprintf (valstr,sizeof(valstr),"%s: 0x%08x  %d",pVal->name,*(DWORD*)data,*(DWORD*)data);
                    break;
                case REG_BINARY:
                    sssnprintf (valstr,sizeof(valstr),"%s:",pVal->name);
                    for ( i=0 ; i<dsz && NumBytes(valstr)<(valWinWidth-8) ; i++ )
                    {
                        sssnprintf (buf,sizeof(buf)," %02x",data[i]);
                        strcat (valstr,buf);
                    }
                    if ( i<dsz )
                        strcat (valstr,"...");
                    break;
            }
            valstr[78]=0;               
        }
//dprintf ("3:(%s)-(%s)\n",valstr,pVal->name);
        NWSAppendToList (reinterpret_cast<unsigned char*>(valstr),pVal->name,regNutH); // add value to the list
//dprintf ("4");
        n++;
    }
//dprintf ("5");

    if ( n )
        NWSList(MSGNUM(IDS_SELECT_VAL_DEL),(valWinHeight/2)+1,52,keyWinHeight,50,M_ESCAPE|M_SELECT|M_NO_SORT,NULL,regNutH,NULL,DeleteValAction,0);
    else
        SetError (LS(IDS_NO_VALUES_DEL));
    dprintf ("Done with NWSList\n");
    regMenu=FALSE;
    if ( RegEditRunning )
        NWSDestroyList (regNutH);

    for ( curr = head ; curr ; head = curr ) // free all the memory we allocated
    {
        curr = curr->next;
        free( head );
    }
}
/************************************************************************************/
int EditAction (LONG keyPressed,LIST **element,LONG *itemNum,void *listParm)
{ // called from the list in ComEdit
    char data[DATALEN],Header[255];
    DWORD ret=0,size=DATALEN;

    itemNum=itemNum;
    listParm=listParm;

    memset (data,0,DATALEN);
    memset (Header,0,255);

    if ( keyPressed == M_ESCAPE )
        return CMD_ESCAPE;

    dprintf ("Edit Action: %s\n",((Value*)(*element)->otherInfo)->name);

    memset(data,0,DATALEN);

    if ( ret=SymSaferRegQueryValueEx (CURR,((Value*)(*element)->otherInfo)->name,NULL,NULL,reinterpret_cast<unsigned char*>(data),&size)!=ERROR_SUCCESS )
        ERROR(ret);

    if ( size )
        data[size]=0;

    sssnprintf (Header,sizeof(Header),"%s%s",LS(IDS_VALUE_NAME),((Value*)(*element)->otherInfo)->name);

    NWSSetDynamicMessage (DYNAMIC_MESSAGE_TEN,reinterpret_cast<unsigned char*>(Header),&regNutH->messages);

    switch ( ((Value*)(*element)->otherInfo)->type )
    {
        case REG_SZ:
            if ( (ret=NWSEditString (5,40,1,70,DYNAMIC_MESSAGE_TEN,MSGNUM(IDS_DATA),reinterpret_cast<unsigned char*>(data),DATALEN-1,EF_ANY,regNutH,0,0,0)) != E_ESCAPE )
            {
                if ( (ret=RegSetValueEx (CURR,((Value*)(*element)->otherInfo)->name,NULL,REG_SZ,reinterpret_cast<unsigned char*>(data),NumBytes(data)+1)) == ERROR_SUCCESS )
                    STATUS(IDS_DATA_CHANGED);
                else
                    ERROR(ret);
            }
            else STATUS(IDS_DATA_NOT_CHANGED);
            break;

        case REG_DWORD:
            {
                DWORD Val=0;            
                char Dstring[100],*c=NULL;
                memset (Dstring,0,100);

                ULtoA (*(DWORD*)data,Dstring,10);
                if ( NWSEditString (5,40,1,70,DYNAMIC_MESSAGE_TEN,MSGNUM(IDS_DWORD_VALUE),reinterpret_cast<unsigned char*>(Dstring),100,EF_SET,regNutH,0,0,const_cast<char*>("abcdefABCDEF0..9xX")) != E_ESCAPE )
                {
                    if ( Dstring[0]=='0' )
                        Val = strtoul (Dstring,&c,16);
                    else
                        Val = strtoul (Dstring,&c,10);
                    if ( (ret=RegSetValueEx (CURR,((Value*)(*element)->otherInfo)->name,NULL,REG_DWORD,(BYTE*)&Val,sizeof(Val))) == ERROR_SUCCESS )
                        STATUS(IDS_DATA_CHANGED);
                    else
                        ERROR(ret);
                }
                else STATUS(IDS_DATA_NOT_CHANGED);
            }
            break;
        case REG_BINARY: {
                DWORD i=0;
                BYTE bindat[1024]={0};
                char Dstring[3000]={0},*tok=NULL,buf[10]={0},*ptr=NULL;

                for ( i=0 ; i<size && i<DATALEN ; i++ )
                { // write the current binary data into a temporary buffer so we can edit it
                    if ( data[i] <= 0xf )
                        strcat (Dstring,"0");
                    strcat (Dstring,ULtoA (data[i],buf,16));
                    strcat (Dstring," ");
                }

                if ( NWSEditString (5,40,1,70,DYNAMIC_MESSAGE_TEN,MSGNUM(IDS_BINARY_VALUE),reinterpret_cast<unsigned char*>(Dstring),3000,EF_SET,regNutH,0,0,const_cast<char*>("0..9A..Fa..f ")) != E_ESCAPE )
                {
                    tok=strtok (Dstring," \n\t");
                    if ( tok )
                        bindat[0]=strtoul (tok,&ptr,16);
                    for ( i=1; (tok=strtok (NULL," \n\t"))!=NULL ; i++ )
                        bindat[i]=strtoul (tok,&ptr,16);

                    if ( (ret=RegSetValueEx (CURR,((Value*)(*element)->otherInfo)->name,NULL,REG_BINARY,bindat,i)) == ERROR_SUCCESS )
                        STATUS(IDS_DATA_CHANGED);
                    else
                        ERROR(ret);
                }
                else STATUS(IDS_DATA_NOT_CHANGED);
            }
            break;
    }
    return 1;
}
/************************************************************************************/
void ComEdit ()
{

    int v=0,n=0,i=0;
    DWORD sz=0,dsz=0;
    char valstr[VALLEN+DATALEN]={0},buf[10]={0};
    BYTE data[DATALEN]={0};
    Value *pVal=NULL,*head=NULL,*curr=NULL;
    DWORD type;

    dprintf ("Com Edit\n");
    regMenu=TRUE;
    NWSInitList(regNutH,0);

//dprintf ("1");    // enumerate all the values.  RegEnumValue returns SUCCESS until there are no more values
    for ( dsz=DATALEN,sz=VALLEN,v=0 ; ; v++,dsz=DATALEN,sz=VALLEN )
    {

        if ( RegEnumValue(CURR,v,valstr,&sz,NULL,&type,data,&dsz)!=ERROR_SUCCESS )
            break;

        pVal = (Value*) malloc(sizeof(Value)); // allocate memory for it
        if ( !pVal ) break;

        memset(pVal,0,sizeof(Value));

        valstr[sz]=0;
        strcpy( pVal->name, valstr );           // fill in the fields
        pVal->type = type;
        pVal->next = NULL;

        if ( !head || !curr )                    // insert it into the linked list
            head = pVal;
        else
            curr->next = pVal;

        curr = pVal;

//dprintf ("2");
        memset (valstr,0,sizeof(valstr));
        if ( dsz )
        {                  // the value has data associated with it
            data[dsz]=0;
            switch ( pVal->type )
            {            // extract the data into valstr
                case REG_SZ:
                    sssnprintf (valstr,sizeof(valstr),"%s:%s",pVal->name,data);
                    break;
                case REG_DWORD:
                    sssnprintf (valstr,sizeof(valstr),"%s: 0x%08x  %d",pVal->name,*(DWORD*)data,*(DWORD*)data);
                    break;
                case REG_BINARY:
                    sssnprintf (valstr,sizeof(valstr),"%s:",pVal->name);
                    for ( i=0 ; i<dsz && NumBytes(valstr)<(valWinWidth-8) ; i++ )
                    {
                        sssnprintf (buf,sizeof(buf)," %02x",data[i]);
                        strcat (valstr,buf);
                    }
                    if ( i<dsz )
                        strcat (valstr,"...");
                    break;
            }
            valstr[78]=0; // truncate it to screen width
        }
//dprintf ("3");
        NWSAppendToList(reinterpret_cast<unsigned char*>(valstr),pVal,regNutH);
        n++;
//dprintf ("4");
    }
//dprintf ("5");

    NWSList(MSGNUM(IDS_SELECT_VAL_ED),(valWinHeight/2)+1,52,keyWinHeight,50,M_ESCAPE|M_SELECT|M_NO_SORT,NULL,regNutH,NULL,EditAction,0);

    dprintf ("Done with NWSList\n");
    regMenu=FALSE;

    if ( RegEditRunning )
        NWSDestroyList (regNutH);

    for ( curr = head ; curr ; head = curr ) // free all the memory we allocated
    {
        curr = curr->next;
        free( head );
    }
}
/************************************************************************************/
void ClearStrings (void)
{   // frees the key array

    if ( subKeys )
    {
        free( subKeys );
        subKeys = NULL;
    }
}

/*****************************************************************************/
void PrintSubkeys (void)
{ // Enums the subkeys of a key and prints them on the screen

    DWORD k=0,l=0;

    int   i;

    char Key[KEYLEN];

    // first count the keys

    for ( numKeys=0 ; (RegEnumKey(CURR,numKeys,Key,KEYLEN)==ERROR_SUCCESS) ; numKeys++ )
    {
        // need to be server friendly when the list gets big!
        if ( 0 == numKeys % 100 )
            ThreadSwitchWithDelay();
    }

    ClearStrings( );        // free the old array

    // now allocate the new array

    if ( numKeys > 0 )
    {
        // get as much memory as I can, settling for less if necessary

        while ( (subKeys = (KEY_ARRAY *)malloc( numKeys * KEYLEN )) == NULL )
        {
            numKeys -= 1;
            if ( numKeys == 0 )
            {
                break;
            }
        }
    }

    if ( numKeys > 0 )
    {
        // there are some keys (and memory)

        memset( subKeys, 0, KEYLEN * numKeys );

        // i < numKeys is a safety check in case new keys showed up - unlikely but possible - malloc does 
        // relinquish control sometimes - this is more likely when malloc'ing large blocks, or in case
        // I didn't get enough memory in the first place (also unlikely) - if the number
        // of keys goes down, then there is no harm - I just update numKeys with the smaller number

        for ( i=0; i < numKeys && (RegEnumKey(CURR,i,(*subKeys)[i],KEYLEN)==ERROR_SUCCESS); i++ )
        {
            // need to be server friendly when the list gets big!
            if ( 0 == i % 100 )
                ThreadSwitchWithDelay();
        }

        numKeys = i;

        // note - this used to rely on the last entry being zero - this is no longer true, so the reasonable
        // safety check k<numKeys was added - this keeps it inside the legal keys

        // this used to break if you got down to the 128 key limit and scrolled down
        // the end - then it would wander off into the aether.

        for ( k=keyLine,l=0 ; l<keyWinHeight && k<numKeys && (*subKeys)[k][0] ; k++ )
        {   // fill the subKeys array
            if ( NumBytes ((*subKeys)[k])>keyWinWidth )
                if ( (NumBytes((*subKeys)[k]) / keyWinWidth) + l > keyWinHeight )
                    return;

            l=NWSDisplayTextInPortal (l,1,reinterpret_cast<unsigned char*>((*subKeys)[k]),VINTENSE,keyPCB);
        }
    }

    if ( !k )                                                 // There aren't any subkeys in this key
        NWSDisplayTextInPortal (1,4,reinterpret_cast<unsigned char*>(LS(IDS_NO_SUBKEYS)),VREVERSE,keyPCB);
}


/*****************************************************************************/
void PrintValues(void)
{ // Enums the values of a key and prints them on the screen

    DWORD v=0,sz=0,dsz=0,type=0;
    int i=0;
    LONG l=0;
    char name[VALLEN]={0},data[DATALEN]={0},buf[10]={0},valstr[DATALEN+VALLEN]={0};

//dprintf ("PrintValues...");

    for ( dsz=DATALEN,sz=VALLEN,numVals=0 ; (RegEnumValue(CURR,numVals,name,&sz,NULL,&type,reinterpret_cast<unsigned char*>(data),&dsz)==ERROR_SUCCESS) ; numVals++,dsz=DATALEN,sz=VALLEN );

    for ( dsz=DATALEN,sz=VALLEN,v=valLine,l=0 ; l<valWinHeight && (RegEnumValue(CURR,v,name,&sz,NULL,&type,reinterpret_cast<unsigned char*>(data),&dsz)==ERROR_SUCCESS) ; v++,dsz=DATALEN,sz=VALLEN )
    {

        if ( dsz )
        {                                  // the value has data associated with it
            data[dsz]=0;
            switch ( type )
            {
                case REG_SZ:
                    sssnprintf (valstr,sizeof(valstr),"%s:%s",name,data);
                    break;
                case REG_DWORD:
                    sssnprintf (valstr,sizeof(valstr),"%s: 0x%08x  %d",name,*(DWORD*)data,*(DWORD*)data);
                    break;
                case REG_BINARY:
                    sssnprintf (valstr,sizeof(valstr),"%s:",name);
                    for ( i=0 ; i<dsz && NumBytes(valstr)<(valWinWidth-8) ; i++ )
                    {
                        sssnprintf (buf,sizeof(buf)," %02x",data[i]);
                        strcat (valstr,buf);
                    }
                    if ( i<dsz )
                        strcat (valstr,"...");
                    break;
            }
        }
        if ( NumBytes (valstr) > valWinWidth-2 )
            if ( (NumBytes(valstr) / valWinWidth) + l > valWinHeight )
                return;
        l=NWSDisplayTextInPortal (l,1,reinterpret_cast<unsigned char*>(valstr),VINTENSE,valPCB);
    }
    if ( !v )
    {                                               // there aren't any values associated with this key
        NWSDisplayTextInPortal (1,19,reinterpret_cast<unsigned char*>(LS(IDS_NO_VALUES)),VREVERSE,valPCB);
    }
//dprintf("done\n");
}
/*****************************************************************************/
void SetStatus (char *msg)
{ // Sets the status message so it can be printed next time

    prevMsgLen = NumBytes (status); // save the previous length of the message
    StrCopy (status,msg);
}
/*****************************************************************************/
void SetError (char *msg)
{

    SetStatus (msg);
    RingTheBell();
}
/*****************************************************************************/
void InitRegEdit (void)
{ // Initialization functions

    dprintf ("Init Reg Edit\n");
    openKey = 0;

    memset (keyPath,0,KEYLEN*KEYPATHLEN);       // Clear the keyPath array
    memset (parentKey,0,KEYPATHLEN);            // Clear the parentKey Array

    parentKey[0] = HKEY_LOCAL_MACHINE;          // Set the base key in parentKey

    StrCopy (keyPath[0],"\\");//LS(IDS_HKEY_LOCAL_MACHINE));    // set the base key in keyPath

    keyLine=valLine=0;

    STATUS(IDS_DATABASE_LOADED);
}

/************************************************************************************/
BOOL CheckCom (int option)
{

    dprintf ("Check Com: %d\n",option);

    switch ( option )
    {
        case NUM_QUIT_PROG:
            return TRUE;
        case NUM_ADD_KEY:
            ComAddKey();
            break;
        case NUM_ADD_VALUE:
            ComAddValue();
            break;
        case NUM_DELETE_KEY:
            ComDeleteKey();
            break;
        case NUM_DELETE_VALUE:
            ComDeleteValue();
            break;
        case NUM_OPEN_SUBKEY:
            ComOpen();
            break;
        case NUM_CLOSE_KEY:
            ComClose();
            break;
        case NUM_EDIT_DATA:
            ComEdit();
            break;
    }
    return FALSE;
}
/************************************************************************************/
void KeyScrollUp ()
{

    dprintf ("Key Scroll Up\n");
//  vpRegChanged=TRUE;
    if ( !numKeys ) return;
    if ( keyLine>0 )
        keyLine--;
}
void KeyScrollDown ()
{

    dprintf ("Key Scroll Down\n");
//  vpRegChanged=TRUE;
    if ( !numKeys ) return;
    keyLine++;
    if ( keyLine>=numKeys )
        keyLine=numKeys-1;
}
void ValueScrollUp ()
{

    dprintf ("Value Scroll Up\n");
//  vpRegChanged=TRUE;
    if ( !numVals ) return;
    if ( valLine>0 )
        valLine--;
}
void ValueScrollDown ()
{

    dprintf ("Value Scroll Down\n");
//  vpRegChanged=TRUE;
    if ( !numVals ) return;
    valLine++;
    if ( valLine>=numVals )
        valLine=numVals-1;
}
/************************************************************************************/
BOOL regConfirmQuit()
{

    dprintf ("reg Confirm Quit\n");
    if ( NWSConfirm (MSGNUM(IDS_EXIT_REGEDIT),12,40,1,NULL,regNutH,NULL)==1 )
        return TRUE;

    return FALSE;
}
/************************************************************************************/
BOOL HandleKeyCom ()
{

    char kv=0;
    LONG type;
    BYTE key;
    BOOL ret;
    char HomeDir[255]={"SYS:\\"};
    HKEY hkey;
    DWORD cbdata=sizeof(HomeDir);

    if ( !RegistryRunning || !RegEditRunning ) return TRUE;

    NWSGetKey (&type,&key,regNutH);
    vpRegChanged=TRUE;
    dprintf ("Handle Key Com: type:(%d) key:'%c'\n",type,key);
    switch ( type )
    {
        
        case K_F1:
            if ( RegOpenKey(HKEY_LOCAL_MACHINE, REGHEADER, &hkey)==ERROR_SUCCESS )
                SymSaferRegQueryValueEx(hkey,const_cast<char*>("Home Directory"),0,NULL,reinterpret_cast<unsigned char*>(HomeDir),&cbdata);

            ret=FileDumpDatabase(HKEY_LOCAL_MACHINE,HomeDir,sizeof(HomeDir));
            dprintf ("File database %sdumped\n",ret?"":"NOT ");
            break;

        case K_F2:
            if ( RegOpenKey(HKEY_LOCAL_MACHINE, REGHEADER, &hkey)==ERROR_SUCCESS )
                SymSaferRegQueryValueEx(hkey,const_cast<char*>("Home Directory"),0,NULL,reinterpret_cast<unsigned char*>(HomeDir),&cbdata);

            ret=RawDumpDatabase(HomeDir,sizeof(HomeDir));
            dprintf ("Raw database %sdumped\n",ret?"":"NOT ");
            break;

        case K_F5:
            if ( CheckCom(regCommandMenu()) )
                return regConfirmQuit();
            break;

        case K_F10:
            return TRUE;
#ifdef DEBUG
        case K_AF2:
            if ( MainThreadGroup!=-1 )
            {
                if ( ScreenHan!=EFAILURE && ScreenHan!=NULL )
                {
                    SetCurrentScreen(ScreenHan);
                    DisplayScreen (ScreenHan);
                }
                SetThreadGroupID (MainThreadGroup);
            }
            else RingTheBell();
            break;
#endif                  
        case K_INSERT:
            goto COM_ADD;

        case K_DELETE:
        case K_BACK:
            goto COM_DEL;
            break;

        case K_UP: // Up Arrow
            KeyScrollUp();
            break;

        case K_DOWN: // Down Arrow
            KeyScrollDown();
            break;

        case K_LEFT: // Left Arrow
            ValueScrollUp();
            break;

        case K_RIGHT: // Right Arrow
            ValueScrollDown();
            break;

        case K_PUP:
            ValueScrollUp();
            ValueScrollUp();
            ValueScrollUp();
            break;

        case K_PDOWN:
            ValueScrollDown();
            ValueScrollDown();
            ValueScrollDown();
            break;

        case 53: // K_HOME
            keyLine=valLine=0;
            break;

        case K_ESCAPE:
        case K_EXIT:
            return regConfirmQuit();
            break;

        case 0: switch ( toupper (key) )
            {
                
                case 'M':
                    if ( CheckCom(regCommandMenu()) )
                        return regConfirmQuit();
                    break;

                case 'A':
                    COM_ADD:        NWSClearPortal (rstatPCB);
                    NWSDisplayTextInPortal (0,1,reinterpret_cast<unsigned char*>(LS(IDS_ADD_KEY_VAL)),VINTENSE,rstatPCB);
                    regMenu=TRUE;
                    kv=toupper (getch());
                    regMenu=FALSE;
                    if ( kv=='K' )
                        ComAddKey();
                    else if ( kv=='V' )
                        ComAddValue();
                    else if ( kv==27 )
                        break;
                    else
                        STATUS(IDS_INVALID_KEY);
                    break;

                case 'D':
                    COM_DEL:        NWSClearPortal (rstatPCB);
                    NWSDisplayTextInPortal (0,1,reinterpret_cast<unsigned char*>(LS(IDS_DEL_KEY_VAL)),VINTENSE,rstatPCB);
                    regMenu=TRUE;
                    kv=toupper (getch());
                    regMenu=FALSE;
                    if ( kv=='K' )
                        ComDeleteKey();
                    else if ( kv=='V' )
                        ComDeleteValue();
                    else if ( kv==27 )
                        break;
                    else
                        STATUS(IDS_INVALID_KEY);
                    break;

                case 'O':
                    ComOpen();
                    break;

                case 'C':
                    ComClose();
                    break;

                case 'E':
                    ComEdit();
                    break;

                case 'Q':
                    return regConfirmQuit();
                    break;

                default:
                    RingTheBell();
            }
            break;

        default:
            RingTheBell();
    }
    return FALSE;
}
/************************************************************************************/
void regCloseConsoleScreen()
{

    RegCloseDatabase(TRUE);

    dprintf ("regCloseConsoleScreen\n");
    if ( !RegEditRunning ) return;
    RegEditRunning=FALSE;

    StopRegistryEditor();

    if ( regNutH )
    {
        dprintf ("DestroyList\n");
        NWSDestroyList (regNutH);
//      NTxSleep (200);
        NWSDestroyMenu (regNutH);
//      NTxSleep (200);
        NWSDestroyForm (regNutH);
//      NTxSleep (200);
        dprintf ("RestoreNut\n");
        NWSRestoreNut (regNutH);
        regNutH = NULL;
//      NTxSleep (200);
    }

    if ( RegEditThreadActive )
    {
        RegEditThreadActive=FALSE;
        dprintf ("decrememting ThreadCount for "REGEDIT_NAME"\n");
    }
    dprintf ("All Done\n");
}
/************************************************************************************/
void StopRegistryEditor (void)
{

    dprintf ("StopRegistryEditor\n");

    if ( regScreenHan!=EFAILURE && regScreenHan!=NULL )
    {
        SetCurrentScreen(regScreenHan);
        DisplayScreen (regScreenHan);
    }
    if ( regMenu )
    {
        dprintf ("Send escape to kill menu\n");
        regMenu=FALSE;
        ungetch(27);
        NTxSleep (1000);
    }
}
/************************************************************************************/
void regStartConsoleScreen(void)
{

    BOOL Stop=FALSE;

    dprintf ("regStartConsoleScreen \n");
    regScreenHan = CreateScreen(LS(IDS_REG_SCREEN_TITLE),DONT_CHECK_CTRL_CHARS|AUTO_DESTROY_SCREEN);
    if ( regScreenHan==EFAILURE || regScreenHan==NULL )
    {
        RingTheBell();
        dprintf ("Cannot Create Screen for "REGEDIT_NAME"\n");
        return;
    }
    SetCurrentScreen(regScreenHan);
    DisplayScreen (regScreenHan);
    SetAutoScreenDestructionMode(TRUE);

    regSetupNut();

    regPortals();

    while ( !Stop && RegistryRunning && RegEditRunning )
    {//SystemRunning &&
        int i;
        for ( i=0 ; i<6 ; i++ )
        {
            if ( NWSKeyStatus(regNutH) )
            {
                if ( HandleKeyCom() )
                    Stop=TRUE;
                break;
            }
            NTxSleep(50);
        }

        if ( !RegistryRunning || !RegEditRunning ) break;
        if ( vpRegChanged && CheckIfScreenDisplayed(regScreenHan,FALSE) )
        {
            regPortals();
            vpRegChanged=FALSE;
        }
        if ( !RegistryRunning || !RegEditRunning ) break;

        ThreadSwitchWithDelay();
    }

    ClearStrings( );        // free the key display memory

    dprintf ("Exiting "REGEDIT_NAME"\n");
}
/************************************************************************************/
DWORD TestDB(void)
{

    return 0;
}
/************************************************************************************/
void ReleaseTheHounds (void)
{

    vpRegChanged=TRUE;
    if ( RegEditRunning )
    {
        if ( regScreenHan!=EFAILURE && regScreenHan!=NULL )
        {
            SetCurrentScreen(regScreenHan);
            DisplayScreen (regScreenHan);
        }
        else RingTheBell();
        return;
    }
    else
        EditRegistry(NULL);
//      RegEditThreadGroup=BeginThreadGroup(EditRegistry,NULL,STACK_SIZE,NULL);
}
/***************************************************************************************/
            int __init_environment( void *reserved )
             {
                return 0;
             }

             int __deinit_environment( void *reserved )
             {
                return 0;
             }

/***************************************************************************************/
void main (int argc, char *argv[])
{

    int i,b=0,t=0;
    char *q,*p;
#ifdef MemCheck
    char* certPtr=NULL;
#endif



/////////////////////////////////////////////////////////////////////////////////////////
//bnm STS# 380046
    int               ccode, structSize;
    FILE_SERV_INFO   sbuf;

    // rectify argv[0] if launched from system because of secure console 

    if ( argc == 3 && !strcmp( argv[1], "/SECURE_CONSOLE" ) )
    {
        argv[0] = argv[2]; argc -=2;
    }
    if ( argc == 4 && !strcmp( argv[2], "/SECURE_CONSOLE" ) )
    {
        argv[0] = argv[3]; argc -=2;
    }

    structSize = 128;
    ccode = GetServerInformation (structSize, &sbuf);
    if ( ccode == 0 )
    {
        if ( sbuf.netwareVersion==5&&sbuf.netwareSubVersion==0x3c )
        {
            if ( sbuf.revisionLevel<1 )
            {
                ConsolePrintf("NetWare 6 Serive Pack 1 or higher required to run VPREGEDT.NLM.\n Please load the latest Novell Service Pack and re-load VPREGEDT.NLM.\n");
                abort();
            }
        }
    }
    else
    {
        ConsolePrintf ("ccode = %d\n", ccode);
        abort();
    }
////////////////////////////////////////////////////////////////////////////////////////

#ifdef MemCheck
    for ( i=0; i<3; i++ )
    {
        certPtr = (char*)malloc(64);
        if ( certPtr )
        {
            free(certPtr);
        }

        certPtr = (char*)calloc(1, 64);
        if ( certPtr )
        {
            char *temp = (char*)realloc( certPtr, 128 );
            if ( temp )
            {
                free( temp );
            }
            else
            {
                free( certPtr );
            }
        }

        certPtr = (char*)realloc( NULL, 64 );
        if ( certPtr )
        {
            free( certPtr );
        }
    }

#endif

    RegThreadCount++;
//  RenameThread(NTxGetCurrentThreadId(),REGEDIT_NAME" - Main");

    RegThreadID=NTxGetCurrentThreadId();
    RegTGID=GetThreadGroupID();

    // ksr - 2/11/02        Unload
    atexit( UnloadRegistry );
#ifdef NLM
	 AtUnload( UnloadRegistry );             // unload command
#endif

    signal(SIGTERM, UnloadRegistryArg); // run the unload in My Thread... (4.10 needs this)

    q = StrRChar(argv[0],'\\');
    p = StrRChar(argv[0],'/');
    q = (q>p) ? q : p;
    if ( q )
    {
        *q = 0;
        StrCopy (RegProgramDir,argv[0]);
    }
    else
        StrCopy (RegProgramDir,NW_SYSTEM_DIR);
    chdir (RegProgramDir);

    for ( i=1;i<argc;i++ )
    {
        strupr(argv[i]);

        if ( StrEqual(argv[i],"DEBUG") )
        {
            debug = 1;
            continue;
        }
        if ( StrEqual(argv[i],"BREAK") )
        {
            b = 1;
            continue;
        }
        if ( StrEqual(argv[i],"TEST") )
        {
            t = 1;
            continue;
        }
    }

    dprintf (REGEDIT_NAME" loaded\n");

    Breakpoint(b);

    if ( t )
    {
        TestDB();
    }

    EditRegistry(NULL);

    RegThreadCount--;

    // ksr - 2/11/02        Unload
    exit( 0 );
}
/*****************************************************************************/
void EditRegistry(void *nothing)
{ // The loop to draw the screen and do commands until quit is requested

    nothing=nothing;

    dprintf ("EditRegistry\n");
    if ( RegEditRunning )
    {
        if ( regScreenHan!=EFAILURE && regScreenHan!=NULL )
        {
            SetCurrentScreen(regScreenHan);
            DisplayScreen (regScreenHan);
        }
        else RingTheBell();
        return;
    }

    RegThreadCount++;

    RegEditRunning=TRUE;

//  RenameThread(NTxGetCurrentThreadId(),"Registry-"REGEDIT_NAME);
    RegEditThreadActive=TRUE;

    SetAutoScreenDestructionMode(DONT_CHECK_CTRL_CHARS|AUTO_DESTROY_SCREEN);

    InitRegEdit();
    ThreadSwitchWithDelay();

    regStartConsoleScreen();

    regCloseConsoleScreen();

    dprintf ("Wait while regNutH is being killed\n");
    while ( regNutH )
        NTxSleep (100);

    if ( regScreenHan!=EFAILURE && regScreenHan!=NULL )
    {
        dprintf ("DestoryScreen\n");
        DestroyScreen (regScreenHan);
        regScreenHan=0;
    }
    dprintf ("EXIT: EditRegistry\n");
    ThreadSwitchWithDelay();
    RegThreadCount--;
}
/*****************************************************************************/
void UnloadRegistry (void)
{

    int i;

//ConsolePrintf ("Registry Unloading: %d\n",RegThreadCount);

    // ksr - 2/11/02         Unload
    //dprintf (REGEDIT_NAME" Unloading\n");
    RegistryRunning=0;

    if ( RegThreadID )
        ResumeThread (RegThreadID);

    ThreadSwitchWithDelay();

    for ( i=200 ; RegThreadCount && i ; i-- )
    {
//ConsolePrintf ("Waiting for Threads to Die: %d\n\r",RegThreadCount);
        NTxSleep (100);
    }
//ConsolePrintf ("Threads Should be dead: %d\n",RegThreadCount);
    NTxSleep (100);
}
/************************************************************************************/
int RegDebugTGID=0;
void DebugScreen(void *nothing)
{

    int ScreenHan;

    REF(nothing);

    RegThreadCount++;
    ScreenHan = CreateScreen(REGEDIT_NAME" - Debug",DONT_CHECK_CTRL_CHARS|AUTO_DESTROY_SCREEN);
    if ( ScreenHan==EFAILURE || ScreenHan==NULL )
    {
        RingTheBell();
        RegThreadCount--;
        return;
    }
    SetCurrentScreen(ScreenHan);
    SetAutoScreenDestructionMode(TRUE);

    RenameThread(NTxGetCurrentThreadId(),REGEDIT_NAME" Debug");

    RegDebugTGID = GetThreadGroupID();

    while ( RegEditRunning )
        NTxSleep(500);

    RegThreadCount--;

//  DestroyScreen (ScreenHan);
//  ScreenHan=0;
}
/************************************************************************************/

void Real_dvprintf(const char *format,va_list args) {

    char line[1024];
    int tgid=0;

    if ( debug )
    {
        if ( RegDebugTGID == 0 )
        {
            BeginThreadGroup(DebugScreen,NULL,8096,NULL);
            NTxSleep(100);
        }
        if ( RegDebugTGID )
        {
            tgid = SetThreadGroupID(RegDebugTGID);

            DWORD dwFineTime = GetFineLinearTimeEx( ONEDAY_MAX_RES );

            DWORD dwSeconds = dwFineTime / ONEDAY_MAXRES_TICKS_PER_SECOND;
            DWORD dwMicros  = OneDayMaxResToMicroSeconds( dwFineTime % ONEDAY_MAXRES_TICKS_PER_SECOND );
            DWORD dwHours, dwMins, dwSecs;

            SecondsToHoursMinsSecs( dwSeconds, dwHours, dwMins, dwSecs );

            sssnprintf(line,sizeof(line),"%02d:%02d:%02d.%06d[_%ld %ld_]|", dwHours, dwMins, dwSecs, dwMicros, NTxGetCurrentThreadId(), tgid );

            ssvsnprintfappend(line,sizeof(line),format,args);
            NWprintf("%s",line);

            SetThreadGroupID(tgid);
        }
    }
}
void Real_dprintf(const char *format,...) {
	va_list args; va_start(args,format); Real_dvprintf(format,args); va_end(args);
}

/***************************************************************************************/
char *_strncpy(char *d,const char *s,int n)
{
    char *org=d;
    n--;
    while ( n&&*s )
    {
        *d++=*s++;
        n--;
    }
    *d=0;
    return org;
}       //MLR Fixed
/***************************************************************************************/
BOOL MyCreatePortal (LONG *Port,PCB **Pcb,LONG r,LONG c,LONG h,LONG w,LONG vh,LONG vw,LONG save,BYTE *headerText,LONG headerAttr,LONG borderType,LONG borderAttr,NUTInfo *handle)
{

    dprintf ("Creating Portal: %s\n",(headerText ? reinterpret_cast<char*>(headerText):"<NO_TITLE>"));
    if ( h>25 ) h=25;
    if ( w>80 ) w=80;
    *Port=NWSCreatePortal (r,c,h,w,vh,vw,save,headerText,headerAttr,borderType,borderAttr,CURSOR_OFF,DIRECT,handle);
    if ( *Port > 0xFFFFFF00 )
    {
        *Port=0;
        *Pcb=NULL;
        return FALSE;
    }
    NWSGetPCB (Pcb,*Port,handle);
    if ( borderType!=NOBORDER )
        NWSDrawPortalBorder (*Pcb);
    NWSClearPortal (*Pcb);
    return TRUE;
}
/********************************************************************************/
char *GetRealLSPointer(LONG wID)
{

    if ( RegStringTable == NULL )
        return "<<<NO MESSAGES>>>";

    if ( wID < STR_APP_NAME || wID >= IDS_LAST_MESSAGE )
        wID = 998;

    return RegStringTable[wID - 998];
}
/***************************************************************************************/
BOOL StrEqual(char *s1,char *s2)
{// returns TRUE if the strings are the same

    int i;

    if ( strlen(s1)!=strlen(s2) ) return FALSE;

    for ( i=0 ; i<strlen(s1) && i<strlen(s2) ; i++ )
        if ( s1[i]!=s2[i] )
            return FALSE;

    return TRUE;
}
/***************************************************************************************/

