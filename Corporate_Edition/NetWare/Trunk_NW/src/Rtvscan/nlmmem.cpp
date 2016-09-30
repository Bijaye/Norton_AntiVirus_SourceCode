// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#include "pscan.h"
#include "slash.h"
#include "SymSaferStrings.h"
#define BLOCKS 3000

#if defined MemCheck || defined SemaCheck

INIT_LOCK();

int memchk(void *q,char c,int size);   // gdf CW conversion
#endif

#ifdef MemCheck
typedef struct {
	void *block;
	char loc[30];
	long size;
	} ttt;

char *SetLoc(ttt *blk, char *file, int line) {

	static char loc[30];
	char *p = strrchr(file, cSLASH);

	sssnprintf(loc, sizeof(loc), "(%s,%d)", p ? ++p : file, line);

	if (blk)
		strcpy(blk->loc,loc);

	return loc;
}

FILE *mf = NULL;
BOOL VerboseMemory = 0;
char MemLogFile[100] = "";

#define MEM_LOG_FILE "%s" sSLASH "VPDebugM.log"
#define OPEN_MEM_LOG_FILE LOCK(); if (TrackMallocs && (mf || (mf = fopen(MemLogFile, "at")))) {
#define CLOSE_MEM_LOG_FILE if (mf && (shutdown || debug&DEBUGSAFELOG)) { fflush(mf); fclose(mf); mf = NULL; } } UNLOCK();
#define MEM_LOG if(mf) fprintf(mf,

/*************************************************************************/
#endif // MemCheck

/*************************************************************************/
///////////////////////////////////////////////////////////////////////////
#ifdef SemaCheck

#undef NTxSemaOpen
#undef NTxSemaClose
#if defined POSIX
	#define NTxSemaOpen(i,p) OpenSemaphore(i)
	#define NTxSemaClose(h)  CloseSemaphore((LONG)h)
#elif defined NLM
	#define NTxSemaOpen(i,p) OpenLocalSemaphore(i)
   #define NTxSemaClose(h)  CloseLocalSemaphore((LONG)h)
#endif

ttt *semaArray[BLOCKS];
long sems=0;
FILE *sf = NULL;

char SemaLogFile[128];

#define NumSemaLogFiles 1

#if NumSemaLogFiles == 1
	#define semaLogFileID (char)('S')
#elif NumSemaLogFiles > 1 && NumSemaLogFiles < 13
	#define semaLogFileID (char)((i) + 'A' - 1)
#else
	#error NumSemaLogFiles invalid!  Must be 1 to 12
#endif // NumSemaLogFiles

#define SEMA_LOG_FILE "%s" sSLASH "VPDebug%c.log"
#define OPEN_SEMA_LOG_FILE LOCK(); if (TrackSemaphores && (sf || (sf = fopen(SemaLogFile, "at")))) {
#define CLOSE_SEMA_LOG_FILE if (sf && (shutdown || debug&DEBUGSAFELOG)) { fflush(sf); fclose(sf); sf = NULL; } } UNLOCK();
#define SEMA_LOG if(sf) fprintf(sf,

/*************************************************************************/
void StartSemaLog() {	// rearrange the SemaLogFiles so we are using file 0

	char SemaLogFile2[128];
	int i;

	for (i=NumSemaLogFiles; i>0; i--) {
		if (i==NumSemaLogFiles) {
			sssnprintf(SemaLogFile, sizeof(SemaLogFile), SEMA_LOG_FILE, ProgramDir, semaLogFileID);
			if(!access(SemaLogFile,0))	// this is the last file
				DeletePurgeFile(SemaLogFile);		// so delete it
			}
		else {
			sssnprintf(SemaLogFile2, sizeof(SemaLogFile2), SEMA_LOG_FILE, ProgramDir, semaLogFileID);
			if(!access(SemaLogFile2,0))
				rename(SemaLogFile2, SemaLogFile);

			strcpy(SemaLogFile, SemaLogFile2);
			}
		}

	OPEN_SEMA_LOG_FILE 
	time_t t = time(NULL);
	SEMA_LOG "***************************************************************\n");
	SEMA_LOG "*** Opening semaphore log file at --> %s",ctime(&t));
	SEMA_LOG "***************************************************************\n\n");
	CLOSE_SEMA_LOG_FILE
}

#undef semaLogFileID
#undef NumSemaLogFiles
/*************************************************************************/
int semaInsertInList(ttt *blk) {

	int i;

	for (i=0;i<BLOCKS;i++)
		if (semaArray[i] == 0) {
			semaArray[i] = blk;
//SEMA_LOG "Inserting Semaphore %u into semaArray\n",blk->size);
			return i;
			}
	SEMA_LOG "Couldn't Insert Semaphore %u into semaArray\n",blk->size);
	return -1;
}
/*************************************************************************/
int semaRemoveFromList(long sema) {

	int i;

	for (i=0; i<BLOCKS; i++)
		if(semaArray[i]) {
			if (semaArray[i]->size == sema) {
//				SEMA_LOG "Removing Semaphore %u %s from semaArray\n",sema,semaArray[i]->loc);
				free(semaArray[i]);
				semaArray[i] = 0;
				return i;
				}
			}

	dprintf("FreeLocalSemaphore called with a valid block that is not in SemaArray %x, not freeing!!\n",sema);
	SEMA_LOG "Couldn't Find Semaphore %x in semaArray\n",sema);
	return -1;
}
/*************************************************************************/
HANDLE myOpenLocalSemaphore(long initialValue, char *file, int line) {

	int i;
	long sema;
	ttt *blk;

	sema = 
#if defined POSIX			 
			 OpenSemaphore(initialValue);
#elif defined NLM	
          OpenLocalSemaphore(initialValue);
#endif

	OPEN_SEMA_LOG_FILE

	if (!sema) {
		SEMA_LOG "Couldn't open a semaphore from (%s,%d)\n", file, line);
		}

	else {
		blk = malloc(sizeof(ttt));
		if (blk) {
			blk->size = sema;
			blk->block = (void*)sema;
			SetLoc(blk, file, line);
			i = semaInsertInList(blk);
			SEMA_LOG "%d: Opened Semaphore %x from (%s,%d)\n",i , sema, file, line);
			}
			
		// -----------------------------
		// ksr - Certification, 7/21/2002 
		
		else
		{
			dprintf( "Cannot allocate memory in myOpenLocalSemaphore()\n" );
			return ERROR_MEMORY;
		}
		// -----------------------------
		
		sems++;
		}

	CLOSE_SEMA_LOG_FILE
	return sema;
}
/*************************************************************************/
int myCloseLocalSemaphore(HANDLE semaHandle, char *file, int line) {

	int i;

	if (!semaHandle) return 0;

	OPEN_SEMA_LOG_FILE
	i = semaRemoveFromList(semaHandle);
	SEMA_LOG "%d: Closing Semaphore %x from (%s,%d)\n", i, semaHandle, file, line);
	sems--;
	CLOSE_SEMA_LOG_FILE

	return 
#if defined POSIX			 
          CloseSemaphore(semaHandle);
#elif defined NLM	
          CloseLocalSemaphore(semaHandle);
#endif
}
/*************************************************************************/
char SemaLine[256];
void ReportSemaUsage(char shutdown) {

	int i;
	time_t t = time(NULL);

	OPEN_SEMA_LOG_FILE
	SEMA_LOG "\n******* ReportSemaUsage --> %s", ctime(&t));

	for (i = 0; i < BLOCKS; i ++ ) {
		if (semaArray[i]) {
			sssnprintf(SemaLine, sizeof(SemaLine), "%d: Semaphore %x opened in %s\n", i, semaArray[i]->size, semaArray[i]->loc);

			if (shutdown) {
//				_printf(SemaLine);
//				NTxSemaClose(semaArray[i]->size);
				free(semaArray[i]);
				semaArray[i] = 0;
				}
			else {
				dprintf(SemaLine);
				}

			SEMA_LOG "* %s", SemaLine);
			}
		}

	SEMA_LOG "****************************************************\n\n");
	
	if(shutdown) {
		t = time(NULL);
		SEMA_LOG "***************************************************************\n");
		SEMA_LOG "*** Closing semaphore log file at --> %s",ctime(&t));
		SEMA_LOG "***************************************************************\n");
		}

	CLOSE_SEMA_LOG_FILE
}
#endif // SemaCheck
///////////////////////////////////////////////////////////////////////////
/*************************************************************************/

#ifdef MemCheck

#ifdef STANDALONE
#define dprintf printf
#endif

 BOOL
  TrackMallocs = FALSE,
  TrackSemaphores = FALSE,
  TrackThreads = FALSE;

long mem = 0;
ttt *memArray[BLOCKS];

void StartMemLog(void) {
	sssnprintf(MemLogFile, sizeof(MemLogFile), MEM_LOG_FILE, ProgramDir);

	if (!access(MemLogFile, 0))
		DeletePurgeFile(MemLogFile);
}

// ksr - 6/18/02 - memory leak fix, sts # 381005
//#if 0 bnm debug cer

//#endif	// 0  //bnm debug cert

/*****************************************************************************/

#undef realloc
#undef malloc
#undef free
#undef calloc

/*************************************************************************/
void InsertInList(ttt *blk) {

	int i;

	for (i=0;i<BLOCKS;i++)
		if (memArray[i] == 0) {
			memArray[i] = blk;
			return;
			}

}
/*************************************************************************/
DWORD RemoveFromList(ttt *blk) {

	int i;

	for (i=0;i<BLOCKS;i++)
		if (memArray[i] == blk) {
			memArray[i] = 0;
			return 0;
			}

	dprintf("Free called with a valid block that is not in MemArray %08X, not freeing!!\n",blk->block);
	return 1;
}
/*******************************************************************************/

// ksr - 6/18/02 - memory leak fix, sts # 381005
//#if 0 bnm cert debug

void FAR *xealloc( void *old, size_t size )
{
	return MyNLMRealloc( old, size, __FILE__, __LINE__ );
}

void FAR xyzree( void *in )
{
	return MyNLMFree( in, __FILE__, __LINE__ );
}
void FAR *zalloc( size_t size )
{
	return MyNLMMalloc( size, __FILE__, __LINE__ );
}

void FAR *yalloc( size_t count, size_t size )
{
	return MyNLMCalloc(count, size, __FILE__, __LINE__);
}

void FAR *MyNLMRealloc(void *old,size_t size, char *file, int line) {

	if(old==NULL)
		return malloc(size);
	return realloc(old,size);

}

void FAR* MyNLMCalloc( int count, size_t size, char* file, int line)
{
	char* ptr = NULL;
	ptr = (char *)calloc(count, size);
	return ptr;
}
// void FAR *MyNLMMalloc(size_t size, char *file, int line) {  // gdf CW conversion
void FAR *MyNLMMalloc(int size, char *file, int line) {   // gdf CW conversion

	char *ptr;
#if 0
	ttt *blk;
#endif

//dprintf ("malloc\n");

//	 if (!TrackMallocs)
		ptr = (char *)malloc(size);
		return ptr;

#if 0

	ptr = malloc(size+132);
//	dprintf("Malloc %u bytes at %08x\n",size,ptr+68);
	if(VerboseMemory) {
		OPEN_MEM_LOG_FILE
		MEM_LOG "m[%08x](%u)(%s)\n", ptr+68, size, SetLoc(NULL, file, line));
		CLOSE_MEM_LOG_FILE
		}

	if (ptr) {
		blk = malloc(sizeof(ttt));
		*(long *)ptr = (long)blk; 
		if (blk) {
			blk->size = size;
			blk->block = ptr;
			SetLoc(blk, file, line);
			InsertInList(blk);
			}
		mem++;
		}
	else {
		_printf(LS(IDS_MEM_REQ_FAILED),size);
		return NULL;
		}

	memset(ptr+4,0x5a,size+128);
	return ptr+68;

#endif

}

//#endif	// 0		  bnm cert debug

/************************************************************************/
void DisplayMemoryUsage(void) {
	dprintf("There are %u malloc outstanding\n",mem);
	if(VerboseMemory) {
		OPEN_MEM_LOG_FILE
		MEM_LOG "There are %u malloc outstanding\n",mem);
		CLOSE_MEM_LOG_FILE
		}
}
/************************************************************************/
BOOL ReleasingMem=FALSE;
void ReportMemoryUsage(BOOL shutdown) {

	int i;
	char unfreed=0;
	char line[256];

	if (shutdown) { // just a simple synchronizing mechanism
		for( i=0 ; ReleasingMem && i<200 ; i++ ) {
			ThreadSwitchWithDelay();
			NTxSleep(100);
			}
		ReleasingMem=TRUE;
		}

	if(VerboseMemory) {
		OPEN_MEM_LOG_FILE
		MEM_LOG "=============================");
		MEM_LOG "ReportMemoryUsage(%s)",shutdown ? "True":"False");
		MEM_LOG "=============================");
		CLOSE_MEM_LOG_FILE
		}

	for (i=0;i<BLOCKS;i++) {
		if (memArray[i]) {
			sssnprintf(line, sizeof(line), "Block %08x of size %u assigned in %s\n",
				memArray[i]->block,memArray[i]->size,memArray[i]->loc);

			if (shutdown) {
				TrackMallocs = FALSE;
				OPEN_MEM_LOG_FILE
				MEM_LOG line);
				CLOSE_MEM_LOG_FILE
				_printf(line);
				free(memArray[i]->block); // we seem to be freeing some blocks twice during updates
				free(memArray[i]);
				memArray[i] = 0;
				unfreed++;
				}
			else {
				dprintf(line); // we want to print this out regardless
	
				if(VerboseMemory) {
					OPEN_MEM_LOG_FILE
					MEM_LOG line);
					CLOSE_MEM_LOG_FILE
					}
				}
			}
		}

	if(VerboseMemory) {
		OPEN_MEM_LOG_FILE
		MEM_LOG "=============================");
		CLOSE_MEM_LOG_FILE
		}
	
	if(shutdown)
		ReleasingMem=FALSE;

	if(shutdown && unfreed) {
		time_t t=time(NULL);
		OPEN_MEM_LOG_FILE
		MEM_LOG "*** %d  unfreed blocks at --> %s",unfreed,ctime(&t));
		MEM_LOG "***************************************************************\n");
		CLOSE_MEM_LOG_FILE
		}
}
/*****************************************************************************/
void checkmem(ttt *blk) {

	char *q1,*q2;
	char *ptr = (char *)blk->block;
	int size = blk->size;

	q1 = ptr + 4;
	q2 = ptr + size + 68;

	if (memchk(q1,0x5a,64)) {
		dprintf("Underflow on block %08x assigned in %s\n",ptr+68,blk->loc);
		}

	if (memchk(q2,0x5a,64)) {
		dprintf("Overflow on block %08x assigned in %s\n",ptr+68,blk->loc);
		}
}
/*******************************************/
int memchk(void *q,char c,int size) {

	int i;

	for (i=0;i<size;i++)
		if (*((char *)q + i) != c)
			return 1;

	return 0;
}
/************************************************************************/
void MyNLMFree(void FAR *in, char *file, int line) {

	char *ptr = (char *)in;
	ttt *blk = NULL;

//	dprintf ("free\n");

	if (!in) return;
	
	if (!TrackMallocs) {
		free(in);
		return;
		}

	if(VerboseMemory) {
		OPEN_MEM_LOG_FILE
		MEM_LOG "f[%08x](%s)\n",ptr,SetLoc( NULL, file, line ) );
		CLOSE_MEM_LOG_FILE
	}

//	dprintf("Freeing at %08x\n",ptr);

	ptr -= 68;
	blk = (ttt *)*(long *)ptr;

	if (blk) {
		if (ptr != blk->block) {
			dprintf("Bad Block requested to be freed in %s\n", SetLoc(NULL, file, line));
			return;
			}

		checkmem(blk);
		if (RemoveFromList(blk))
			return;

		free(blk);
		}

	mem--;
	free(ptr);
}
#endif // MemCheck

void mprintf (char *format,...) {

	va_list marker;
	char line[1024];

	va_start(marker, format);
	ssvsnprintf(line,sizeof(line),format,marker);
	LogLine(line,FALSE);
	dprintf (line);
	va_end(marker);
}

// ---------------------------------------
// ksr - NetWare Certification, 10/21/2002
// Procedure Coverage
#ifndef	NLM	        

#undef realloc
void *DebugRealloc(void *b,size_t s) {
	void *b2=realloc (b,s);
	mprintf("r:(0x%08x:0x%08x:%d)\n",b,b2,s);
	if (!b2) BREAK();
	return b2;
}
#undef malloc
void *DebugMalloc(size_t s){
	void *b=malloc(s);
	mprintf("m:(0x%08x:%d)\n",b,s);
	if (!b) BREAK();
	return b;
}
#undef free
void DebugFree(void *b){
	mprintf ("f:(0x%08x)\n",b);
	free (b);
}

#endif	// Procedure Coverage        
// ---------------------------------------
