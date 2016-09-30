// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2006 Symantec Corporation. All rights reserved.

#include "pscan.h"
#include "vpstrutils.h"

#include "DarwinResCommon.h"

CResourceLoader g_ResLoader(&_ModuleRes, _T("PScanRes.dll"));
CResourceLoader g_ResActa(&_ModuleRes, _T("ActaRes.dll"));
HINSTANCE g_hInstRes = NULL;

HINSTANCE hInstance,hInstLang;
char SystemRunning = TRUE;
int inited=0;
//tSendVirusAlert *callback[10] = {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
//tReturnPong *callback2[10] = {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
HKEY hMainKey = 0;
HKEY hCommonKey = 0;
char HomeDir[IMAX_PATH];
char PattPath[IMAX_PATH];

typedef struct _HAN {
	DWORD magic;
	tSendVirusAlert *Virus;
	tReturnPong *Pong;
	struct _HAN *Next;
	} HAN, *PHAN;

PHAN                 Head = NULL;
static char          NullUseArray[100];
bool				 LoadedByInstall=false;
extern "C" tGetSendInfoCallback  g_fnSendComGetSendInfoCallback  = NULL;
extern "C" void                 *g_pSendComGetSendInfoClientData = NULL;

/**************************************************************************************/
void AddHanToList(PHAN han) {

	han->Next = Head;
	Head = han;

}
/**************************************************************************************/
void RemHanFromList(PHAN han) {

	PHAN cur=Head,last=NULL;

	while (cur) {
		if (cur == han) {
			if (last == NULL)
				Head = cur->Next;
			else
				last->Next = cur->Next;
			return;
			}
		last = cur;
		cur = cur->Next;
		}

	return;
}
/***************************************************************************************
 * 06/14/06 - JHoa - Defect 1-5NTR1B - Only need to run this fix during an install. 
 * Checking for install executables. Related to changelist 1946856.
 **************************************************************************************/
void CheckLoadedByInstall()
{
	TCHAR szFilePath[IMAX_PATH] = {};
	TCHAR *lpFileName = NULL;
	LoadedByInstall = false;

	if( 0 < GetModuleFileName(NULL,szFilePath,IMAX_PATH) )
	{
		// Get the filename
		lpFileName = _tcsrchr( szFilePath, '\\' );
		if( lpFileName && (lpFileName+1) )
		{
			// compare with install files that could of loaded transman, setup.exe and msiexec.exe
			// incremented pointer past the backslash found by _tcsrchr
			if( _tcsicmp( "setup.exe", lpFileName+1 ) == 0 || _tcsicmp( "msiexec.exe", lpFileName+1 ) == 0 )
				LoadedByInstall = true;
		}
	}
}
/**************************************************************************************/
BOOL WINAPI DllMain(HINSTANCE hinstDLL,DWORD fdwReason,LPVOID  lpvReserved) {

	REF(lpvReserved);
 //	HKEY hkey;

	if (hInstance == 0)
		hInstLang = hInstance = hinstDLL;

	switch (fdwReason) {
		case DLL_PROCESS_ATTACH:
			InitDLL(hInstance);
//   		RegOpenKey(HKEY_LOCAL_MACHINE,"Software\\Microsoft\\Windows\\CurrentVersion
  // write in random number for encription key
			memset(NullUseArray,0,sizeof(NullUseArray));

/*			{
			char path[1024];
			GetModuleFileName(hinstDLL,path,1024);
			MessageBox(NULL,path,"",0);
			}
*/

			CheckLoadedByInstall();
			break;

		case DLL_PROCESS_DETACH:
			DeinitDLL(hInstance);
			break;
		}

	return TRUE;
}
/**************************************************************************************/
DWORD DeinitTransman(DWORD ihan) {

	PHAN han = (PHAN)ihan;

	dprintfTag1( DEBUGCOMMPACKET, "Transman deiniting %08X\n", han );

	if (han == NULL)
		return (DWORD)-1;

	if ((DWORD)han < 100) {
		if (NullUseArray[(DWORD)han] == 0)
			return (DWORD)-1;
		}
	else {
		if (han->magic != 0x4f2e648a)
			return (DWORD)-1;
		}

	inited--;

	dprintfTag1( DEBUGCOMMPACKET, "Transman inited = %u\n", inited );

	if ((DWORD)han > 100) {
		if (heapcheck() == _HEAPOK) {
			RemHanFromList(han);
			han->magic = 0;
			free(han);
			}
		else
			MessageBox(NULL,"BAD HEAP","TRANSMAN.DLL",MB_OK|MB_ICONSTOP);
		}
	else {
		if((DWORD)han < 100) NullUseArray[(DWORD)han] = 0;
	}

	if (inited <= 0)
    {
        StopNTS();

		// Closing the open registry keys to reduce handle leaks.
		hCommonKey = GetCommonKey();
		if(hCommonKey)
			RegCloseKey(hCommonKey);

		if(hMainKey)
			RegCloseKey(hMainKey);

        g_fnSendComGetSendInfoCallback  = NULL;
		g_pSendComGetSendInfoClientData = NULL;
    }

	return 0;
}

/**************************************************************************************/
DWORD InitTransman(tSendVirusAlert *cb,tReturnPong *cb2, tGetSendInfoCallback fnGetSendInfoCallback)
{
	DWORD cc;
	PHAN han;

	if (cb || cb2)
    {
		han = (PHAN)malloc(sizeof(HAN));
		if (han == NULL)
			return 0;
	}
	else
    {
		int i;
		for (i=1;i<100;i++)
        {
			if (NullUseArray[i] == 0)
				break;
		}
		if (i == 100)
			return 0;

		han = (PHAN)i;
	}

	if (inited == 0)
    {
		dprintfTag0( DEBUGCOMMPACKET, "Transman inited first time\n" );

		if (RegCreateKey(HKEY_LOCAL_MACHINE, REGHEADER, &hMainKey) == ERROR_SUCCESS)
        {
			GetStr(hMainKey,szReg_Val_HomeDir,HomeDir,sizeof(HomeDir),".");
			vpstrncpy (PattPath,HomeDir, sizeof (PattPath));
            
            InitializeDebugLogging();
        }

		dprintfTag0( DEBUGCOMMPACKET, "Transman calling startnts\n" );
		cc = StartNTS(NULL);
        dprintfTag1( DEBUGCOMMPACKET, "Transman calling startnts: ret = %08X\n", cc );

		if (cc)
        {
			if ((DWORD)han > 100)
				free(han);
			return 0;
		}

		//Load the Acta strings:
		if (SUCCEEDED(g_ResActa.Initialize()))
		{
			dprintf("Loaded Acta resource DLL\n");
			hInstLang = g_ResActa.GetResourceInstance();
		}
		else
		{
			dprintf("Failed to load Acta resource DLL\n");
			return FALSE;
		}
			
		//Load the PScan resources:
		if (SUCCEEDED(g_ResLoader.Initialize()))
		{
			dprintf("Loaded PScan resource DLL\n");
			::g_hInstRes = g_ResLoader.GetResourceInstance();
		}
		else
		{
			dprintf("Failed to load PScan resource DLL\n");
			return FALSE;
		}
	}

	inited++;
	if ((DWORD)han > 100)
    {
		han->magic = 0x4f2e648a;
		han->Virus = cb;
		han->Pong = cb2;
		han->Next = NULL;

		dprintfTag1( DEBUGCOMMPACKET, "Transman calling add %08X\n", han );
		AddHanToList(han);
	}
	else
    {
		NullUseArray[(DWORD)han] = 1;
//		han;
	}

    // Ensure that NULL callback specifications do not overwrite a existing callback
    // setting. Note that the only way to reset the callback to NULL is via 
    // DeInitTransman() or RegisterGetSendInfoCallback().
    if (fnGetSendInfoCallback)
	{
        g_fnSendComGetSendInfoCallback  = fnGetSendInfoCallback;
		g_pSendComGetSendInfoClientData = NULL;
	}

	return (DWORD)han ;
}
/************************************************************************************/
void RegisterGetSendInfoCallback(tGetSendInfoCallback fnGetSendInfoCallback, void *client_data)
{
    g_fnSendComGetSendInfoCallback  = fnGetSendInfoCallback;
	g_pSendComGetSendInfoClientData = client_data;
}
/************************************************************************************/
void ValidateTransman(tGetSendInfoCallback fnGetSendInfoCallback)
{
	if (inited == 0)
		InitTransman(NULL, NULL, fnGetSendInfoCallback);
}
/************************************************************************************/
DWORD SendVirusAlert(PEVENTBLOCK eb) {

	PHAN cur = Head;
	while (cur) {
		if (cur->Virus)
			cur->Virus(eb);
		cur = cur->Next;
		}
	return 0;
}
/****************************************************************/
DWORD ReturnPong(const char *computer,const char *domain,const char *mom,PONGDATA *pong) {

	PHAN cur = Head;

	while (cur) {
		if (cur->Pong)
			cur->Pong(computer,domain,mom,pong);
		cur = cur->Next;
		}

	return 0;
}
/*********************************************************************************************************/
void Real_dprintfTagged(DEBUGFLAGTYPE dwTag, const char *format,...) {

	va_list marker;

	va_start(marker, format);
    Real_vdprintfCommon(dwTag, format, marker);
	va_end(marker);
}
/**********************************************************************************************/
void Real_dprintf(const char *format,...)
{
    // Calls to this function used to not have the call time prefixed in the logged line.
	va_list marker;

	va_start(marker, format);
    Real_vdprintfCommon(0, format, marker);
	va_end(marker);
}

