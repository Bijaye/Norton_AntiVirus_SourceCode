// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#include "pscan.h"
#include <nwfshook.h>  // gdf CW conversion
#include <nwdir.h>

#include <advanced.h> // ksr - for EVENT_CLEAR_CONNECTION
// Prototypes
int ReMapClearConnection( void );

void GetServerMajorVersion(void);  // gdf CW conversion
void CheckActionTable(void);  // gdf CW conversion
static void ClearConnectionCallBack(DWORD connectionId, DWORD param2);  // gdf CW conversion

//LONG WarnClearConnection( void (*OutPutFunc)(void *fmt,...), LONG parameter );
// void ClearConnectionCallBack( LONG parameter );  // gdf CW conversion

extern "C" void UnMapClearConnection( void );
static DWORD ConnEventHandle = 0;
extern "C" extern "C" int MaxVolumes;          // gdf 06/14/00  value is set in nlmmain.c
extern "C" BOOL MountDismountInProgress = FALSE;    // gdf 01/31/2003 fix defect 1-LLE9R
extern "C" char MountDismountVolume[17] = {0};     // gdf 01/31/2003 fix defect 1-LLE9R

extern "C" char running=0;
extern "C" DWORD UIDS = 1;
extern "C" PFILE_ACTION ActionHead=NULL;
extern "C" PMEM MemHead = NULL;
extern "C" PMEM AllMems = NULL;
extern "C" SYS_PRAMS SystemPrams = {0};
extern "C" DWORD CClru = 100;
extern "C" char CheckVolumes=1;
extern "C" char CallState=0;
extern "C" char NoCallAfterFail=0;
extern "C" VolumeTBL VolumeTable[VOLUMES] = {0};
extern "C" int Process=0;
extern "C" int EventsInQueue=0;
extern "C" long MainTgID;
extern "C" long AllocTag=0;

extern "C" long volumeSemHandle = 0;

extern "C" long MountEvent=0;
extern "C" long DismountEvent = 0;

extern "C" STORAGEITEM *StorageList; //crt port

// ksr	1/4/02 - NetWare 6 support
extern "C" LONG	NetWareVersion = 0;
typedef int (*P_ZISNSSVOLUME)(char* path); //bnm at plugfest: added to check for volume type
int	(*_zIsNSSVolume)(char* path); //bnm at plugfest: added to check for volume type
int *_SleepNotAllowedUseCount = 0;

BYTE majorServerVersion;
BYTE minorServerVersion; //bnm plugfest fix for version 5.60 check

//bnm: this will do until we change volume table update from polling to event driven
int unInitVolFlag = 9;  //0=traditional, 1=NSS, ..., 9=volume table not updated for this entry.

DWORD SleepingSystemThreads=0;
USERTHREAD *UserThreadHead = NULL;
USERTHREAD *SystemThreadHead = NULL;
PFILE_ACTION WaitQueue=NULL;
LONG openPreCallBackHandle = 0,openPostCallBackHandle = 0,closePostCallBackHandle = 0;
LONG renamePreCallBackHandle = 0,createPreCallBackHandle = 0,createPostCallBackHandle = 0;
LONG erasePreCallBackHandle = 0,genErasePreCallBackHandle = 0,delDirPreCallBackHandle = 0;
LONG genericRenamePreCallBackHandle = 0,genericOpenPreCallBackHandle = 0,genericOpenPostCallBackHandle = 0;

CCA CleanCacheArray[CLEANCACHESIZE];
DWORD SystemThreadsOwned=0;

extern "C" void *Alloc(LONG,LONG);
extern "C" void Free (void *);

typedef LONG (*PADDFSMONITORHOOK)(LONG callBackNumber, void *callBackFunc,LONG *callBackHandle);
typedef LONG (*PREMOVEFSMONITORHOOK)(LONG callBackNumber, LONG callBackHandle);

PADDFSMONITORHOOK    AddFSMonitorHook    = NULL;
PREMOVEFSMONITORHOOK RemoveFSMonitorHook = NULL;

#define NWAddFSMonitorHook AddFSMonitorHook
#define NWRemoveFSMonitorHook RemoveFSMonitorHook

/**************************************************************************************/
DWORD GetUID(void) {

	DWORD ret;

	if ((ret = UIDS++) >=0x00ffffff)
		UIDS = 1;

	return ret;
}
/**************************************************************************************/
void InitAction(PFILE_ACTION Action,PFILE_ACTION Base) {

	int han = Action->SemHan;
	ULONG t;

	memset(Action,0,sizeof(FILE_ACTION));

	if (Base) {
		*Action = *Base;
		t = Action->UID>>24;
		t++;
		Action->UID = (Action->UID&0x00ffffff) | (t<<24);
		}
	else
		Action->UID = GetUID();

	Action->SemHan = han;
	Action->Address = (DWORD)Action;
	Action->Magic = FA_MAGIC;
	Action->Time = GetCurrentTime();
	Action->LastMagic = FA_MAGIC;

}
/**************************************************************************************/
void RemoveAndFreeMem(PMEM mem) {

	PMEM cur=AllMems,last=NULL;

	while(cur) {
		if (cur == mem) {
			if (last)
				last->AllNext = cur->AllNext;
			else
				AllMems = cur->AllNext;

			SystemPrams.Allocs--;
			NTxSemaClose(cur->Action.SemHan);
			Free(cur);
			break;
			}
		last = cur;
		cur = cur->AllNext;
		}
}
/**************************************************************************************/
void FreeMemList(void) {

	PMEM cur,old;

	cur = MemHead;
	MemHead = NULL;

	while (cur) {
		old = cur;
		cur = cur->Next;
		RemoveAndFreeMem(old);
		}

	if (SystemPrams.Allocs) {
		while(AllMems) {
			_printf(LS(IDS_ACTION_BLK_STUCK),AllMems->Action.Name);
			RemoveAndFreeMem(AllMems); // value of AllMems will be different on return.
			}
		}

	if (SystemPrams.Allocs) {
		_printf(LS(IDS_SOMETHING_WRONG));
		}

}
/******************************************************************************************/
DWORD VerifyAction(PFILE_ACTION Action) {

	PMEM cur = AllMems;

	while(cur) {
		if (Action == &cur->Action)
			break;
		cur = cur->AllNext;
		}

	if (cur)
		if (Action->Magic == FA_MAGIC && Action->LastMagic == FA_MAGIC)
			return TRUE;


	return FALSE;
}
/**************************************************************************************/
PFILE_ACTION GetAction(PFILE_ACTION BaseAction) {

	PFILE_ACTION Action=NULL;
	PMEM mem;

	if (MemHead) {
		Action = &MemHead->Action;
		MemHead = MemHead->Next;
		SystemPrams.ActionsInUse++;
		}
	else {
		mem = (PMEM)Alloc(sizeof(MEM),AllocTag);
		if (mem) {
			memset(mem,0,sizeof(mem));
			Action = &mem->Action;
			Action->SemHan = reinterpret_cast<int>(NTxSemaOpen(0, NULL));
			SystemPrams.Allocs++;
			SystemPrams.ActionsInUse++;
			mem->AllNext = AllMems;
			AllMems = mem;
			}
		else
			Action = NULL;
		}


	if (Action) {
		InitAction(Action,BaseAction);
		}

	return Action;
}
/**************************************************************************************/
VOID GiveAction(PFILE_ACTION Action) {

	PMEM mem;

	if (!VerifyAction(Action)) {
		ConsolePrintf( "RTVSCAN: Bad Action Block Returned.\n");
		SystemPrams.Faults++;
		return;
		}

	if (NTxSemaExamine(Action->SemHan) != 0) {
		ConsolePrintf( "RTVSCAN: Non-Zero Semaphore.\n");
		SystemPrams.Faults++;
		while (NTxSemaExamine(Action->SemHan) > 0)
			NTxSemaWait(Action->SemHan);
		while (NTxSemaExamine(Action->SemHan) < 0)
			NTxSemaSignal(Action->SemHan);
		}


	Action->Magic = 0;
	mem = CONTAINING_RECORD(Action,MEM,Action);

	mem->Next = MemHead;
	MemHead = mem;
	SystemPrams.ActionsInUse--;

	return;
}



//***************************************************************************
// Compute a 2 32-bit CRC's from a supplied table for a string
//
// Description: calculate 2 32-bit crc from a supplied table for a string
//
// Return Value:
//
// See Also:
//
//***************************************************************************
// 02/32/1994 JMILLARD Function Created.
//***************************************************************************

    /* number of bits in CRC: don't change it. */
#define W 32

    /* this the number of bits per char: don't change it. */
#define B 8


void ComputeDoubleCRC32ForString (
	 	WCHAR *lpData,

		LPDWORD lpdwCrc1,
		DWORD dwSeed1,
       	DWORD dwCrcTable1[],

		LPDWORD lpdwCrc2,
		DWORD dwSeed2,
        DWORD dwCrcTable2[] )
{
    register DWORD  dwCRC1 = dwSeed1;
    register DWORD  dwCRC2 = dwSeed2;

	if (sizeof (*lpData) == 1)
	{
		// important - must be unsigned
	    register unsigned char *cp   = (unsigned char *) lpData;

	    while( *cp != 0 )
		{
	        dwCRC1 = (dwCRC1<<B) ^ dwCrcTable1[(dwCRC1>>(W-B)) ^ *cp];
	        dwCRC2 = (dwCRC2<<B) ^ dwCrcTable2[(dwCRC2>>(W-B)) ^ *cp];

			cp += 1;
		}
	}
	else	// actually running with Unicode
	{
		// important - must be unsigned
	    register unsigned short int *cp   = (unsigned short int *) lpData;

	    while( *cp != 0 )
		{
			// if unicode then do both halves of the 16-bit character

	        dwCRC1 = (dwCRC1<<B) ^ dwCrcTable1[(dwCRC1>>(W-B)) ^ (*cp & 0xff)];
	        dwCRC1 = (dwCRC1<<B) ^ dwCrcTable1[(dwCRC1>>(W-B)) ^ (*cp >> 8)];

	        dwCRC2 = (dwCRC2<<B) ^ dwCrcTable2[(dwCRC2>>(W-B)) ^ (*cp & 0xff)];
	        dwCRC2 = (dwCRC2<<B) ^ dwCrcTable2[(dwCRC2>>(W-B)) ^ (*cp >> 8)];

			cp += 1;
		}
	}


	*lpdwCrc1 = dwCRC1;
	*lpdwCrc2 = dwCRC2;

}

// table to compute 32-bit CRC for primitive polynomial 0x1589b4545

static DWORD dwPrimitive32CrcTable[256] = {
    0x00000000, 0x589b4545, 0xb1368a8a, 0xe9adcfcf,
    0x3af65051, 0x626d1514, 0x8bc0dadb, 0xd35b9f9e,
    0x75eca0a2, 0x2d77e5e7, 0xc4da2a28, 0x9c416f6d,
    0x4f1af0f3, 0x1781b5b6, 0xfe2c7a79, 0xa6b73f3c,
    0xebd94144, 0xb3420401, 0x5aefcbce, 0x02748e8b,
    0xd12f1115, 0x89b45450, 0x60199b9f, 0x3882deda,
    0x9e35e1e6, 0xc6aea4a3, 0x2f036b6c, 0x77982e29,
    0xa4c3b1b7, 0xfc58f4f2, 0x15f53b3d, 0x4d6e7e78,
    0x8f29c7cd, 0xd7b28288, 0x3e1f4d47, 0x66840802,
    0xb5df979c, 0xed44d2d9, 0x04e91d16, 0x5c725853,
    0xfac5676f, 0xa25e222a, 0x4bf3ede5, 0x1368a8a0,
    0xc033373e, 0x98a8727b, 0x7105bdb4, 0x299ef8f1,
    0x64f08689, 0x3c6bc3cc, 0xd5c60c03, 0x8d5d4946,
    0x5e06d6d8, 0x069d939d, 0xef305c52, 0xb7ab1917,
    0x111c262b, 0x4987636e, 0xa02aaca1, 0xf8b1e9e4,
    0x2bea767a, 0x7371333f, 0x9adcfcf0, 0xc247b9b5,
    0x46c8cadf, 0x1e538f9a, 0xf7fe4055, 0xaf650510,
    0x7c3e9a8e, 0x24a5dfcb, 0xcd081004, 0x95935541,
    0x33246a7d, 0x6bbf2f38, 0x8212e0f7, 0xda89a5b2,
    0x09d23a2c, 0x51497f69, 0xb8e4b0a6, 0xe07ff5e3,
    0xad118b9b, 0xf58acede, 0x1c270111, 0x44bc4454,
    0x97e7dbca, 0xcf7c9e8f, 0x26d15140, 0x7e4a1405,
    0xd8fd2b39, 0x80666e7c, 0x69cba1b3, 0x3150e4f6,
    0xe20b7b68, 0xba903e2d, 0x533df1e2, 0x0ba6b4a7,
    0xc9e10d12, 0x917a4857, 0x78d78798, 0x204cc2dd,
    0xf3175d43, 0xab8c1806, 0x4221d7c9, 0x1aba928c,
    0xbc0dadb0, 0xe496e8f5, 0x0d3b273a, 0x55a0627f,
    0x86fbfde1, 0xde60b8a4, 0x37cd776b, 0x6f56322e,
    0x22384c56, 0x7aa30913, 0x930ec6dc, 0xcb958399,
    0x18ce1c07, 0x40555942, 0xa9f8968d, 0xf163d3c8,
    0x57d4ecf4, 0x0f4fa9b1, 0xe6e2667e, 0xbe79233b,
    0x6d22bca5, 0x35b9f9e0, 0xdc14362f, 0x848f736a,
    0x8d9195be, 0xd50ad0fb, 0x3ca71f34, 0x643c5a71,
    0xb767c5ef, 0xeffc80aa, 0x06514f65, 0x5eca0a20,
    0xf87d351c, 0xa0e67059, 0x494bbf96, 0x11d0fad3,
    0xc28b654d, 0x9a102008, 0x73bdefc7, 0x2b26aa82,
    0x6648d4fa, 0x3ed391bf, 0xd77e5e70, 0x8fe51b35,
    0x5cbe84ab, 0x0425c1ee, 0xed880e21, 0xb5134b64,
    0x13a47458, 0x4b3f311d, 0xa292fed2, 0xfa09bb97,
    0x29522409, 0x71c9614c, 0x9864ae83, 0xc0ffebc6,
    0x02b85273, 0x5a231736, 0xb38ed8f9, 0xeb159dbc,
    0x384e0222, 0x60d54767, 0x897888a8, 0xd1e3cded,
    0x7754f2d1, 0x2fcfb794, 0xc662785b, 0x9ef93d1e,
    0x4da2a280, 0x1539e7c5, 0xfc94280a, 0xa40f6d4f,
    0xe9611337, 0xb1fa5672, 0x585799bd, 0x00ccdcf8,
    0xd3974366, 0x8b0c0623, 0x62a1c9ec, 0x3a3a8ca9,
    0x9c8db395, 0xc416f6d0, 0x2dbb391f, 0x75207c5a,
    0xa67be3c4, 0xfee0a681, 0x174d694e, 0x4fd62c0b,
    0xcb595f61, 0x93c21a24, 0x7a6fd5eb, 0x22f490ae,
    0xf1af0f30, 0xa9344a75, 0x409985ba, 0x1802c0ff,
    0xbeb5ffc3, 0xe62eba86, 0x0f837549, 0x5718300c,
    0x8443af92, 0xdcd8ead7, 0x35752518, 0x6dee605d,
    0x20801e25, 0x781b5b60, 0x91b694af, 0xc92dd1ea,
    0x1a764e74, 0x42ed0b31, 0xab40c4fe, 0xf3db81bb,
    0x556cbe87, 0x0df7fbc2, 0xe45a340d, 0xbcc17148,
    0x6f9aeed6, 0x3701ab93, 0xdeac645c, 0x86372119,
    0x447098ac, 0x1cebdde9, 0xf5461226, 0xaddd5763,
    0x7e86c8fd, 0x261d8db8, 0xcfb04277, 0x972b0732,
    0x319c380e, 0x69077d4b, 0x80aab284, 0xd831f7c1,
    0x0b6a685f, 0x53f12d1a, 0xba5ce2d5, 0xe2c7a790,
    0xafa9d9e8, 0xf7329cad, 0x1e9f5362, 0x46041627,
    0x955f89b9, 0xcdc4ccfc, 0x24690333, 0x7cf24676,
    0xda45794a, 0x82de3c0f, 0x6b73f3c0, 0x33e8b685,
    0xe0b3291b, 0xb8286c5e, 0x5185a391, 0x091ee6d4,
    };


static DWORD dwCrcCCITT32Table[256] = {
    0x00000000, 0x04c11db7, 0x09823b6e, 0x0d4326d9,
    0x130476dc, 0x17c56b6b, 0x1a864db2, 0x1e475005,
    0x2608edb8, 0x22c9f00f, 0x2f8ad6d6, 0x2b4bcb61,
    0x350c9b64, 0x31cd86d3, 0x3c8ea00a, 0x384fbdbd,
    0x4c11db70, 0x48d0c6c7, 0x4593e01e, 0x4152fda9,
    0x5f15adac, 0x5bd4b01b, 0x569796c2, 0x52568b75,
    0x6a1936c8, 0x6ed82b7f, 0x639b0da6, 0x675a1011,
    0x791d4014, 0x7ddc5da3, 0x709f7b7a, 0x745e66cd,
    0x9823b6e0, 0x9ce2ab57, 0x91a18d8e, 0x95609039,
    0x8b27c03c, 0x8fe6dd8b, 0x82a5fb52, 0x8664e6e5,
    0xbe2b5b58, 0xbaea46ef, 0xb7a96036, 0xb3687d81,
    0xad2f2d84, 0xa9ee3033, 0xa4ad16ea, 0xa06c0b5d,
    0xd4326d90, 0xd0f37027, 0xddb056fe, 0xd9714b49,
    0xc7361b4c, 0xc3f706fb, 0xceb42022, 0xca753d95,
    0xf23a8028, 0xf6fb9d9f, 0xfbb8bb46, 0xff79a6f1,
    0xe13ef6f4, 0xe5ffeb43, 0xe8bccd9a, 0xec7dd02d,
    0x34867077, 0x30476dc0, 0x3d044b19, 0x39c556ae,
    0x278206ab, 0x23431b1c, 0x2e003dc5, 0x2ac12072,
    0x128e9dcf, 0x164f8078, 0x1b0ca6a1, 0x1fcdbb16,
    0x018aeb13, 0x054bf6a4, 0x0808d07d, 0x0cc9cdca,
    0x7897ab07, 0x7c56b6b0, 0x71159069, 0x75d48dde,
    0x6b93dddb, 0x6f52c06c, 0x6211e6b5, 0x66d0fb02,
    0x5e9f46bf, 0x5a5e5b08, 0x571d7dd1, 0x53dc6066,
    0x4d9b3063, 0x495a2dd4, 0x44190b0d, 0x40d816ba,
    0xaca5c697, 0xa864db20, 0xa527fdf9, 0xa1e6e04e,
    0xbfa1b04b, 0xbb60adfc, 0xb6238b25, 0xb2e29692,
    0x8aad2b2f, 0x8e6c3698, 0x832f1041, 0x87ee0df6,
    0x99a95df3, 0x9d684044, 0x902b669d, 0x94ea7b2a,
    0xe0b41de7, 0xe4750050, 0xe9362689, 0xedf73b3e,
    0xf3b06b3b, 0xf771768c, 0xfa325055, 0xfef34de2,
    0xc6bcf05f, 0xc27dede8, 0xcf3ecb31, 0xcbffd686,
    0xd5b88683, 0xd1799b34, 0xdc3abded, 0xd8fba05a,
    0x690ce0ee, 0x6dcdfd59, 0x608edb80, 0x644fc637,
    0x7a089632, 0x7ec98b85, 0x738aad5c, 0x774bb0eb,
    0x4f040d56, 0x4bc510e1, 0x46863638, 0x42472b8f,
    0x5c007b8a, 0x58c1663d, 0x558240e4, 0x51435d53,
    0x251d3b9e, 0x21dc2629, 0x2c9f00f0, 0x285e1d47,
    0x36194d42, 0x32d850f5, 0x3f9b762c, 0x3b5a6b9b,
    0x0315d626, 0x07d4cb91, 0x0a97ed48, 0x0e56f0ff,
    0x1011a0fa, 0x14d0bd4d, 0x19939b94, 0x1d528623,
    0xf12f560e, 0xf5ee4bb9, 0xf8ad6d60, 0xfc6c70d7,
    0xe22b20d2, 0xe6ea3d65, 0xeba91bbc, 0xef68060b,
    0xd727bbb6, 0xd3e6a601, 0xdea580d8, 0xda649d6f,
    0xc423cd6a, 0xc0e2d0dd, 0xcda1f604, 0xc960ebb3,
    0xbd3e8d7e, 0xb9ff90c9, 0xb4bcb610, 0xb07daba7,
    0xae3afba2, 0xaafbe615, 0xa7b8c0cc, 0xa379dd7b,
    0x9b3660c6, 0x9ff77d71, 0x92b45ba8, 0x9675461f,
    0x8832161a, 0x8cf30bad, 0x81b02d74, 0x857130c3,
    0x5d8a9099, 0x594b8d2e, 0x5408abf7, 0x50c9b640,
    0x4e8ee645, 0x4a4ffbf2, 0x470cdd2b, 0x43cdc09c,
    0x7b827d21, 0x7f436096, 0x7200464f, 0x76c15bf8,
    0x68860bfd, 0x6c47164a, 0x61043093, 0x65c52d24,
    0x119b4be9, 0x155a565e, 0x18197087, 0x1cd86d30,
    0x029f3d35, 0x065e2082, 0x0b1d065b, 0x0fdc1bec,
    0x3793a651, 0x3352bbe6, 0x3e119d3f, 0x3ad08088,
    0x2497d08d, 0x2056cd3a, 0x2d15ebe3, 0x29d4f654,
    0xc5a92679, 0xc1683bce, 0xcc2b1d17, 0xc8ea00a0,
    0xd6ad50a5, 0xd26c4d12, 0xdf2f6bcb, 0xdbee767c,
    0xe3a1cbc1, 0xe760d676, 0xea23f0af, 0xeee2ed18,
    0xf0a5bd1d, 0xf464a0aa, 0xf9278673, 0xfde69bc4,
    0x89b8fd09, 0x8d79e0be, 0x803ac667, 0x84fbdbd0,
    0x9abc8bd5, 0x9e7d9662, 0x933eb0bb, 0x97ffad0c,
    0xafb010b1, 0xab710d06, 0xa6322bdf, 0xa2f33668,
    0xbcb4666d, 0xb8757bda, 0xb5365d03, 0xb1f740b4
    };


/**************************************************************************************/
DWORD MakeTag(char *name, DWORD *tag1) {

	DWORD tag;

    // note - the initial seeds actually aren't all that important, but
    // I left them in

	ComputeDoubleCRC32ForString( reinterpret_cast<WCHAR*>(name), &tag, 0x749eba3c, dwCrcCCITT32Table, tag1, 0x3be8a3f7, dwPrimitive32CrcTable );

	return tag;
}
/**************************************************************************************/



DWORD FlushCleanCache(void) {

	int	i;

	dprintf("flush clean cache\n");

	dprintf("Cache hits %d misses %d\n",
		SystemPrams.Cache.Hit, SystemPrams.Cache.Miss );

    if ( debug & DEBUGVERBOSE )
    {
    	for (i=0; i<CCBLOCKS; i += 16)
    	{
    		dprintf( "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n", 
                     SystemPrams.Cache.BlockUsage[i+0 ],
                     SystemPrams.Cache.BlockUsage[i+1 ],
                     SystemPrams.Cache.BlockUsage[i+2 ],
                     SystemPrams.Cache.BlockUsage[i+3 ],
                     SystemPrams.Cache.BlockUsage[i+4 ],
                     SystemPrams.Cache.BlockUsage[i+5 ],
                     SystemPrams.Cache.BlockUsage[i+6 ],
                     SystemPrams.Cache.BlockUsage[i+7 ],
                     SystemPrams.Cache.BlockUsage[i+8 ],
                     SystemPrams.Cache.BlockUsage[i+9 ],
                     SystemPrams.Cache.BlockUsage[i+10],
                     SystemPrams.Cache.BlockUsage[i+11],
                     SystemPrams.Cache.BlockUsage[i+12],
                     SystemPrams.Cache.BlockUsage[i+13],
                     SystemPrams.Cache.BlockUsage[i+14],
                     SystemPrams.Cache.BlockUsage[i+15]);
    	}
    }

	memset(CleanCacheArray,0,sizeof(CleanCacheArray));
	SystemPrams.Cache.Flushes++;

	CClru = 100;

	return STATUS_SUCCESS;
}


/**************************************************************************************/
DWORD ClearFromCleanCache(PFILE_ACTION Action) {

	DWORD tag1;
	DWORD tag = MakeTag(Action->Name,&tag1);
	int i,j;

	if (SystemPrams.Dbg&0x10) {
		dprintf("clearing %s  %8x,%8x\n",Action->Name,tag,tag1);
	}

	for (j=0,i=(tag % CCBLOCKS)*(CLEANCACHESIZE/CCBLOCKS);j<CLEANCACHESIZE/CCBLOCKS;j++) {
		if (	CleanCacheArray[i+j].tag == tag &&
				CleanCacheArray[i+j].tag1 == tag1 &&
				CleanCacheArray[i+j].lru
			) {
			CleanCacheArray[i+j].lru = 0;
			return STATUS_SUCCESS;
			}
		}

	return STATUS_UNSUCCESSFUL;
}


// important - a very good optimization would be to check the last place you were in
// the cache - AutoProtect on the client opens every file 3 or 4 times, so there is a good
// chance that the last place is the right place - in any case the savings are great.

/**************************************************************************************/
DWORD CheckCleanCache(PFILE_ACTION Action)  {

	DWORD tag1;
	DWORD tag = MakeTag(Action->Name,&tag1);
	int i,j;

	if (!SystemPrams.DoCache)
		return STATUS_UNSUCCESSFUL;

	if (CClru < 99) {
		FlushCleanCache();
		return STATUS_UNSUCCESSFUL;
		}

	if (SystemPrams.Dbg&0x10) {
		dprintf("looking for %s  %8x,%8x\n",Action->Name,tag,tag1);
	}

	for (j=0,i=(tag % CCBLOCKS)*(CLEANCACHESIZE/CCBLOCKS);j<CLEANCACHESIZE/CCBLOCKS;j++) {
		if (	CleanCacheArray[i+j].tag == tag &&
				CleanCacheArray[i+j].tag1 == tag1 &&
				CleanCacheArray[i+j].lru
			) {
			CleanCacheArray[i+j].lru = CClru++;
			if (SystemPrams.Dbg&1) {
				dprintf("Cache says %s is clean\n",Action->Name);
				}
			Action->Flags |= FA_FILE_IN_CACHE;
			SystemPrams.Cache.Hit++;
			return STATUS_SUCCESS;
			}
		}

	SystemPrams.Cache.Miss++;
	return STATUS_UNSUCCESSFUL;
}
/**************************************************************************************/
DWORD AddToCleanCache(PFILE_ACTION Action) {

	int i,j;
	DWORD lru = 0xffffffff;
	int loc = -1;
	DWORD tag1;
	DWORD tag = MakeTag(Action->Name,&tag1);
	int block = tag % CCBLOCKS;

	if (CClru < 99) {
		FlushCleanCache();
		return STATUS_UNSUCCESSFUL;
		}

	if (SystemPrams.Dbg&0x10) {
		dprintf("CachePool %u\n",block);
	}

	for (j=0,i=block*(CLEANCACHESIZE/CCBLOCKS);j<CLEANCACHESIZE/CCBLOCKS;j++) {
		if (	CleanCacheArray[i+j].tag == tag &&
				CleanCacheArray[i+j].tag1 == tag1 &&
				CleanCacheArray[i+j].lru
			) {
			if (SystemPrams.Dbg&0x10) {
				dprintf("Already Clean Cache %s\n",Action->Name);
			}
			Action->Flags |= FA_FILE_IN_CACHE;
			return STATUS_SUCCESS;
			}
		else {
			if (CleanCacheArray[i+j].lru < lru) {
				lru = CleanCacheArray[i+j].lru;
				loc = i+j;
				}
			}
		}

	if (SystemPrams.Dbg&0x10) {
		dprintf("adding %s to cache @ %d,%d %8x,%8x\n",Action->Name,block,loc,tag,tag1);
	}

	SystemPrams.Cache.BlockUsage[block]++;
	CleanCacheArray[loc].lru = CClru++;
	CleanCacheArray[loc].tag = tag;
	CleanCacheArray[loc].tag1 = tag1;

	return STATUS_SUCCESS;
}



/**************************************************************************************/
void CompleteAction(PFILE_ACTION Action) {

	char del=0;

	if (Action->Status == ALLOW_VIRUS_ACCESS || Action->Status == DENY_ACCESS) {
		if (Action->Status == ALLOW_VIRUS_ACCESS)
			Action->Status = 0;
		}
	else if (Action->Status)  {// CAN_NOT_SCAN || BAD_VIRUSNAME || SKIP_INFECTION
			Action->Status = 0;
		}
	else {	// else it == 0 and it's clean.
		if (SystemPrams.DoCache && !(Action->Flags&FA_FILE_IN_CACHE))
			AddToCleanCache(Action);
		}

	if (running)
		Action->Flags |= FA_ACTION_COMPLETE;

	if (NTxSemaExamine(Action->SemHan) < 0)
		NTxSemaSignal(Action->SemHan);

	if (Action->Flags&FA_DELETE_WHEN_COMPLETE)
		del = 1;

	SystemPrams.FilesScaned++;

	if (SystemPrams.Dbg&8) {
		dprintf("Scan Conplete, Status:%08X:%s File:%s UID %u\n",Action->Status,del?"del":"",Action->Name,Action->UID);
	}

	if (del)
		GiveAction(Action);
}
/**************************************************************************************/
void AbandonAction(PFILE_ACTION Action) {


	if (Action->Flags&FA_OWNED_BY_USER) {
		Action->Flags |= FA_DELETE_WHEN_COMPLETE;
		}
	else if (Action->Flags&FA_OWNED_BY_QUEUE) {
		Action->Flags |= FA_DELETE_WHEN_COMPLETE;
		// do we want to yank it from the queue or not????????????
		// I thing we want to to stay because it just might get scaned latter
		// but we will free the system thread now
		if (NTxSemaExamine(Action->SemHan) < 0)
			NTxSemaSignal(Action->SemHan);
		}
	else {
		SystemPrams.FileNotScaned++;
		SystemPrams.FilesSlippedPastUs++;
		GiveAction(Action);
		}

}
/**************************************************************************************/
DWORD GiveActionToUser(PFILE_ACTION Action) {

	DWORD cc;

	if (SystemPrams.Dbg&4) {
		dprintf("Give To Use File %s UID %u\n",Action->Name,Action->UID);
	}

	if (	!(Action->Flags&FA_GET_TRAP_DATA) &&
			!(Action->Flags&FA_USE_TRAP_DATA) &&
			!(Action->Flags&FA_CHECK_IS) &&
			SystemPrams.DoCache &&
			CheckCleanCache(Action) == STATUS_SUCCESS) {
		cc = STATUS_SUCCESS;
		Action->Status = STATUS_SUCCESS;
		CompleteAction(Action);
		}
	else if (SystemPrams.ScanInKernal) {
		// cc = ScanFile(Action);
		cc = STATUS_SUCCESS;
		Action->Status = STATUS_SUCCESS;
		CompleteAction(Action);
		}
	else
		cc = WakeUpUserThread(Action);

	return cc;
}
/******************************************************************************************/
void AddToActionTable(PFILE_ACTION Action,DWORD connectionID,DWORD task,void *ref,DWORD queueType) {

	Action->ConnectionID = connectionID;
	Action->Task = task;
	Action->Ref = ref;
	Action->QueueType = queueType;

	Action->Next = ActionHead;
	ActionHead = Action;
	Action->Flags |= FA_IN_TABLE;

	if (queueType == INOPENED)
		SystemPrams.OpensWaitingForClose++;
	else if (queueType == INSYSOPEN)
		SystemPrams.OpensStartedNotEnded++;

}
/**************************************************************************************/
PFILE_ACTION SearchActionTable(DWORD connectionID,DWORD task,void *ref,DWORD queueType) {

	PFILE_ACTION cur,last=NULL;

	cur = ActionHead;
	while (cur) {
		if (	cur->Ref == ref && cur->ConnectionID == connectionID &&
				cur->Task == task && cur->QueueType == queueType) {
			if (last == NULL)
				ActionHead = cur->Next;
			else
				last->Next = cur->Next;
			cur->Flags &= ~FA_IN_TABLE;
			if (queueType == INOPENED)
				SystemPrams.OpensWaitingForClose--;
			else if (queueType == INSYSOPEN)
				SystemPrams.OpensStartedNotEnded--;
			break;
			}
		last = cur;
		cur = cur->Next;
		}

	return cur;
}
/**************************************************************************************/
/*
char isClient(char *name) {


	char *q,*q1;
	char tmp[256];
	int ret = 0;
//	int len = NumBytes(name);
	int i,j;

	q1 = name;

	q1 = StrRChar(name,'\\');  // we will never have a '/' ResolvePath takes care of that
	if (!q1)
		return ret;

	q = StrRChar(q1,'.');

	if (q) {
		q=NextChar(q);		//MLR Fixed
		StrNCopy(tmp,q,31);
		tmp[31] = 0;
		strupr(tmp);
		if (!StrComp(tmp,"CV~")) {
			q = StrChar(name,':');
			if (q) {
				if (name[i=q-name+1] == '\\') {
					memcpy(tmp,name,i=q1-name);
					tmp[i] = 0;
					}
				else {
					memcpy(tmp,name,i);
					tmp[i] = '\\';
					memcpy(tmp+i+1,name+i,j=q1-name-i);
					tmp[j+i+1] = 0;
					}
				strupr(tmp);
				if (!StrComp(tmp,SystemPrams.ClientDirectory)) {
					ret = 1;
					}
				}
			}
		}

	return ret;
}
*/
/**************************************************************************************/
char isExtSysWanted(char *ext,char *ExtList) {

	char pLen = 0, rLen = 0, *p1, *p2, *q, *r1, *r2;
	char Ext[32];

	StrNCopy(Ext, ext, sizeof(Ext)-1);
	Ext[sizeof(Ext)-1] = 0;
	StrUpper(Ext);

	for (p1 = p2 = ExtList; p1; p1 = (*q ? NextChar(q) : NULL)) {
		q = StrChar(p1, ',');
		if (!q)
			q = p1 + NumBytes(p1);

		for (r1 = r2 = Ext; ;
			p2 = NextChar(p1), pLen = p2 - p1, p1 = p2,
			r2 = NextChar(r1), rLen = r2 - r1, r1 = r2) {

			if (p1 >= q) {
				if (!(*r1))
					return 1;

				break;
			}

			if (!(*r1))
				break;		// End if Ext - no match here

			if (*p1 == '?')
				continue;	// i.e., '?' matches ANY char, even wchars

			if (pLen != rLen || *p1 != *r1)
				break;		// Dosen't match

		//	This assumes ONLY 1 or 2 byte characters!
			if (pLen > 1 && *(p1 - 1) != *(r1 - 1))
				break;		// PrevChar dosen't match
		}
	}

	return 0;
}
/**************************************************************************************/
char isFileWanted(PFILE_ACTION Action) {

	int i;
	char ret=0;
	char *name = Action->Name;
	char *q;

	i = Action->DriveVolume;

// gdf 06/17/00:  We shoule consider changing this to use the
// MaxVolumes value as a test.  The drive list value is missleading as
// it does not contain a "drive list" but is simply set to a value of
// "1"  in each byte based on the max supported volumes value.
//  example:  	if (Action->DriveVolume < MaxVolumes) {

	if (SystemPrams.DriveList[i]) {
		if (SystemPrams.ExtList[0] == 0)
			ret = TRUE;
		else {
			q = StrRChar(name,'\\');  // we will never have a '/' ResolvePath takes care of that
			if (!q)
				q = name;
			q = StrRChar(q,'.');
			if (q && isExtSysWanted(q+1,SystemPrams.ExtList))
				ret = TRUE;
			}
		}

	if (!ret)
		SystemPrams.FileNotScaned++;
	return ret;
}
/**************************************************************************************/
PFILE_ACTION GetWaitingAction(void) {

	PFILE_ACTION Action = WaitQueue;

	if (Action) {
		WaitQueue = Action->Next;
		Action->Flags &= ~FA_OWNED_BY_QUEUE;
		SystemPrams.WaitingActions--;
		}

	return Action;
}
/**************************************************************************************/
void FreeAllActions(void) {

	PFILE_ACTION cur,old;
	int i;
	PMEM acur;

	cur = ActionHead;
	ActionHead = NULL;

	while (cur) {  // free all action blocks that have been opened but not closed
		old = cur;
		cur = cur->Next;
		if (SystemPrams.Dbg&10) {
			dprintf("Action Terminated %s\n",old->Name);
		}
		old->Flags &= ~FA_IN_TABLE;
		if (old->QueueType == INOPENED)
			SystemPrams.OpensWaitingForClose--;
		else if (old->QueueType == INSYSOPEN)
			SystemPrams.OpensStartedNotEnded--;
		AbandonAction(old);
		}

	NTxSleep(250);

	while (cur=GetWaitingAction()) {  // release all system therads waiting FOR a user therad
		if (NTxSemaExamine(cur->SemHan) < 0)
			NTxSemaSignal(cur->SemHan);
		else
			AbandonAction(cur);
		}

	acur = AllMems;
	while(acur) {    // release all system therads waiting ON a user thread
		cur = &acur->Action;
		if (cur->Flags&FA_OWNED_BY_USER) {
			if (NTxSemaExamine(cur->SemHan) < 0)
				NTxSemaSignal(cur->SemHan);
			else
				AbandonAction(cur);
			}
		acur = acur->AllNext;
		}


	for (i=0;SleepingSystemThreads&&i<20*60*2;i++)  // wait for all system threads to stop waiting.
		NTxSleep(50);

	if (i == 20*60*2) {
		_printf(LS(IDS_RTVSCAN_CRIT_ERR));
//		BREAK();
		}

	NTxSleep(50);

	if (SystemPrams.ActionsInUse)	{ // some still left.  User must be hung
		_printf("System Shutdown while Scan in Progress: %u.\n",i);
		// Oh Romeo, Oh Romeo, What do I do.
		// just ignore FreeMemList will free and if action ever comes back VerifyAction will fail.
		}

	FreeMemList();
}

//-----------------------------------------------------------------------------
/*
	Registered event handler for volume mount
*/
static void VolMountCallback(LONG volNum, LONG param2)
{
    REF (param2);

	char	volName[17];
	int		cc;
	int tgid;
	SystemThreadsOwned++;
	tgid = SetThreadGroupID(MainTgID);

	if (SystemRunning) {
		if (CheckVolumes) {
			// Get the volume name
			*volName=0;
			cc= GetVolumeName(volNum, volName);
			if (!cc && *volName) {

				NTxSemaWait(volumeSemHandle);
		   		strncpy(VolumeTable[volNum].name,volName,17);
				MountDismountInProgress = 1;               // gdf 01/31/2003 fix defect 1-LLE9R
				strncpy(MountDismountVolume, volName,17);     // gdf 01/31/2003 fix defect 1-LLE9R
		        if(_zIsNSSVolume)
					VolumeTable[volNum].NSSVol = _zIsNSSVolume(VolumeTable[volNum].name);
		        if( strcmp( "_ADMIN", volName ) == 0 )
					VolumeTable[volNum].isVirtualVolume = TRUE;
				NTxSemaSignal(volumeSemHandle);
		 	    FreeNoScanDirs(&StorageList->NoScanDir,TRUE); //crt port
				if (StorageList->Info->hRTSConfigKey)
				{
					// Load the user/admin-defined list of directory exclusions
					LoadUsersNoScanDirs(StorageList->Info->hRTSConfigKey,&StorageList->NoScanDir); //crt port
					// Append the product specific list of directory exclusions (ex. Exchange Server).
					LoadProductNoScanDirs(&StorageList->NoScanDir);
				}
				MountDismountInProgress = 0;       // gdf 01/31/2003 fix defect 1-LLE9R
				memset(MountDismountVolume, 0,17);     // gdf 01/31/2003 fix defect 1-LLE9R

			}
		}

	}

	SystemThreadsOwned--;
	SetThreadGroupID(tgid);
}

//*******************************************************************************************
/*
	Registered event handler for volume dismount
*/
static void VolDismountCallback(LONG volNum, LONG param2)
{
    REF(param2);
	int tgid;
	SystemThreadsOwned++;
	tgid = SetThreadGroupID(MainTgID);
	if (SystemRunning) {
		if (CheckVolumes) {

			NTxSemaWait(volumeSemHandle);
			MountDismountInProgress = 2;                             // gdf 01/31/2003 fix defect 1-LLE9R
			strncpy(MountDismountVolume, VolumeTable[volNum].name,17);  // gdf 01/31/2003 fix defect 1-LLE9R
			/* Remove the resource from the resource list */
			VolumeTable[volNum].NSSVol=unInitVolFlag;
			VolumeTable[volNum].name[0]='\0';
			VolumeTable[volNum].isVirtualVolume = FALSE;
			NTxSemaSignal(volumeSemHandle);
	 	    FreeNoScanDirs(&StorageList->NoScanDir,TRUE); //crt port
			if (StorageList->Info->hRTSConfigKey)
			{
				// Load the user/admin-defined list of directory exclusions
				LoadUsersNoScanDirs(StorageList->Info->hRTSConfigKey,&StorageList->NoScanDir); //crt port
				// Append the product specific list of directory exclusions (ex. Exchange Server).
				LoadProductNoScanDirs(&StorageList->NoScanDir);
			}
			MountDismountInProgress = 0;        // gdf 01/31/2003 fix defect 1-LLE9R
			memset(MountDismountVolume, 0, 17);     // gdf 01/31/2003 fix defect 1-LLE9R

		}
	}
	SystemThreadsOwned--;
	SetThreadGroupID(tgid);
}
/**************************************************************************************/
DWORD StopSystem(void) {

	if (running) {
		running = 0;
		Process  = 0;
		NTxSleep(500);

	   if ( MountEvent != 0 )
	      if ( UnregisterForEvent( MountEvent ) != NULL )
	         ConsolePrintf( "\nPSCAN_N.C: Failure to unregister "
	                        "EVENT_ANY_VOL_MOUNT event!\n" );

	   if ( DismountEvent != 0 )
	      if ( UnregisterForEvent( DismountEvent ) != NULL )
	         ConsolePrintf( "\nPSCAN_N.C: Failure to unregister "
	                        "EVENT_ANY_VOL_DISMOUNT event!\n" );

		UnMapOpenFile();
		UnMapCloseFile();
		UnMapClearConnection();
		UnMapRenameEntry();
		UnMapCreateAndOpenFile();
		UnMapEraseFile();
		UnimportSymbol(GetNLMHandle(), "NWRemoveFSMonitorHook");
		UnimportSymbol(GetNLMHandle(),"SleepNotAllowedUseCount");

		NTxSleep(500);

		FreeAllActions();

		NTxSemaClose(volumeSemHandle);

		while (SystemThreadsOwned)
			ThreadSwitchWithDelay();

		if (SystemPrams.DoCache)
			FlushCleanCache();
		}
	return STATUS_SUCCESS;
}
/**************************************************************************************/
DWORD StartSystem(void) {

	if (!running) {


		/* Register for volume mount and dismount events */
		MountEvent = RegisterForEvent(EVENT_ANY_VOL_MOUNT, VolMountCallback, NULL);
		DismountEvent = RegisterForEvent(EVENT_ANY_VOL_DISMOUNT, VolDismountCallback, NULL);
		if ((MountEvent == -1) || (DismountEvent == -1))
		{
			return -1;
		}

		AddFSMonitorHook = (PADDFSMONITORHOOK)ImportSymbol(GetNLMHandle(), "NWAddFSMonitorHook");
		if (!AddFSMonitorHook) {
			dprintf("Cannot import FS Hooks");
//			RebootServer=TRUE;
			return P_HOOK_ERROR;
		}

		RemoveFSMonitorHook = (PREMOVEFSMONITORHOOK)ImportSymbol(GetNLMHandle(), "NWRemoveFSMonitorHook");
		if (!RemoveFSMonitorHook) {
			dprintf("Cannot import Remove FS Hooks");
//			RebootServer=TRUE;
			UnimportSymbol(GetNLMHandle(), "NWAddFSMonitorHook");
			return P_HOOK_ERROR;
		}

		if (ReMapOpenFile() != 0) {
			return P_OPEN_HOOK_ERROR;
		}
		if (ReMapCloseFile() != 0) {
			UnMapOpenFile();
			return P_CLOSE_HOOK_ERROR;
		}
		//	ksr -	ClearConnection
		if (ReMapClearConnection() != 0) {
			UnMapOpenFile();
			UnMapCloseFile();
			return P_HOOK_ERROR;  //replace generic error code with hook specific code
		}
		if (ReMapRenameEntry() != 0) {
			UnMapOpenFile();
			UnMapCloseFile();
			UnMapClearConnection();
			return P_RENAME_HOOK_ERROR;
			}
		if (ReMapCreateAndOpenFile() != 0) {
			UnMapOpenFile();
			UnMapCloseFile();
			UnMapClearConnection();
			UnMapRenameEntry();
			return P_COPEN_HOOK_ERROR;
		}
		if (ReMapEraseFile() != 0) {
			UnMapOpenFile();
			UnMapCloseFile();
			UnMapClearConnection();
			UnMapRenameEntry();
			UnMapCreateAndOpenFile();
			return P_ERASE_HOOK_ERROR;
		}

		UnimportSymbol(GetNLMHandle(), "NWAddFSMonitorHook");
		running = 1;
		}

	return STATUS_SUCCESS;
}
/**************************************************************************************/
DWORD WaitAndProcessAction(PFILE_ACTION Action,char block) {

	DWORD ret = STATUS_SUCCESS;

	if (GiveActionToUser(Action)==0) {
		char timedout = FALSE;
		if (!(Action->Flags&FA_ACTION_COMPLETE) && running && block) {
			if (SystemPrams.Dbg&0x20) {
				dprintf("Waiting for scan on %s\n",Action->Name);
			}
			SleepingSystemThreads++;
			NTxSemaTimedWait(Action->SemHan,TIME_OUT);
			SleepingSystemThreads--;
			if (!running) {
				AbandonAction(Action);
				return STATUS_SUCCESS;
				}
			}

		if (!(Action->Flags&FA_ACTION_COMPLETE)) {
			if (SystemPrams.Dbg&1) {
				dprintf("TIMEOUT Waiting for scan on %s\n",Action->Name);
			}
			if (block)
				SystemPrams.TimedOut++;
			ret = STATUS_SUCCESS;
			timedout = TRUE;
			}
		else {
			if (SystemPrams.Dbg&0x20) {
				dprintf("COMPLETE Waiting for scan on %s\n",Action->Name);
			}
			ret = Action->Status;
			}
		if (timedout)
			AbandonAction(Action);
		else
			GiveAction(Action);
		}
	else
		AbandonAction(Action);

	return ret;
}
/******************************************************************************************/
DWORD DoRename (DWORD ConnectionID,DWORD NameSpace,DWORD Volume,DWORD DirBase,char *PathString,DWORD PathComponentCount) {

	PFILE_ACTION Action;
	char myBigFileName[1024]; 	//bnm This is the biggest file name
								//we can handle before truncating to 260.
								//next version we'll: increase the filename size we can handle without
								//truncating and replace some Netware APIs in buildpath function.

	Action = GetAction(NULL);

	if (!Action) {
		SystemPrams.FileNotScaned++;
		return STATUS_SUCCESS;
		}

	//bnm sending in the 1k array to get all the name. will truncate to 260 if needed.
	//this is to prevent Action->Name to overflow and corrupt Action->semHandle
	//sts# 408795
	memset(&myBigFileName,0,1024);
	if (BuildPath(DirBase,PathString,PathComponentCount,Volume,myBigFileName,NameSpace) == 0) 
	{
		//fileSizeLength = strlen(myBigFileName);//for debug only

		if(strlen(myBigFileName)>SYSFILELEN)
		{
			dprintf("%s too large - truncating to 260 - length=%d\n",myBigFileName,strlen(myBigFileName));
		}
		memset(&Action->Name,0,SYSFILELEN);
		strncpy(Action->Name,myBigFileName,SYSFILELEN);
		Action->Name[SYSFILELEN-1] = '\0';
	}
	else
	{
		GiveAction(Action);
		SystemPrams.FileNotScaned++;
		return STATUS_SUCCESS;
	}

	Action->TokenUser.pSid = (PSID)&Action->TokenUser.Sid;
	Action->TokenUser.pSid->ConnectionID = ConnectionID;
	Action->DriveVolume = Volume;
	Action->NameSpace = NameSpace;


	if (SystemPrams.Dbg&1)
		dprintf("rename %s UID=%u\n",Action->Name,Action->UID);

	Action->Flags |= FA_CHECK_IS;
	Action->Flags |= FA_RENAME;
	if (isFileWanted(Action)) { //EA - 08/30 For the file to get scanned the flag FA_FILE_NEEDS_SCAN needs to be set hence we check if it is in wanted ext list and set the flag
		Action->Flags |= FA_FILE_NEEDS_SCAN;//EA 08/30
		}//EA08/30
	return WaitAndProcessAction(Action,TRUE);

}
/*******************************************************************************/
	// added for CNA insurance 10/16/96 by DDD
	// the SYS:\LOGIN\BOOTCONF.SYS file is being locked open by lprotect.
	// when this file is locked open, the server will abend when lprotect is
	// unloaded. If this file is BOOTCONF.*, skip it as a temp fix.
	// the funny char casting is a quick way of doing string compares.
	// I can check 4 chars at a time by casting them to a DWORD!
	// I know that this will fail on a mixed case file (BootConf),
	// but I assume the file will come in in one case.
BOOL CheckSpecialNames(char *name) {

	char
		*tmpPtr;

	if (!strncmp(name,"SYS:\\_NETWARE\\",13))   // we do not want to scan objects in Directory Services
		return TRUE;

	// ksr - 1/22/02	Defect # 378748  _ADMIN volume
	// Do not scan the virtual volume - realtime mode
	if( !strncmp( name, "_ADMIN:\\", 7 ) )
	{
		dprintf( "Skip Auto-Protect scanning: %s\n", name );
		return TRUE;
	}
	else
	{
		dprintf( "Auto-Protect scanning: %s \n", name );
	}

	tmpPtr = StrRChar(name, '.');
	if (tmpPtr != NULL) {
		tmpPtr -= 8;
		// see if *tmpPtr == "BOOTCONF" or "bootconf"
		if (*(unsigned long *)tmpPtr == *(unsigned long *)("boot") &&
			 *(((unsigned long *)tmpPtr) + 1) == *(unsigned long *)("conf") ||
			 *(unsigned long *)tmpPtr == *(unsigned long *)("BOOT") &&
			 *(((unsigned long *)tmpPtr) + 1) == *(unsigned long *)("CONF"))
			// this is a BOOTCONF file, so skip it...
			return TRUE;
		}

	return FALSE;
}
/******************************************************************************************/
#define CRIT_ENTER {SystemThreadsOwned++;	tgid = SetThreadGroupID(MainTgID);}
#define CRIT_RETURN(x) {SystemThreadsOwned--; SetThreadGroupID(tgid); return x;}
int MyOpenFileFunctionStart(DWORD connectionID,DWORD task,void *ref,
	DWORD requestedAccessRights,DWORD nameSpace,DWORD volume,DWORD dirBase,
	char *pathString,DWORD pathComponentCount,DWORD attributeMatchBits) {

	DWORD ret=STATUS_SUCCESS;
	PFILE_ACTION Action;
	char read,write,exec,del;
	int tgid;
	DWORD val;
	char trap = 0;
	char client=0;

	char myBigFileName[1024]; 	//bnm This is the biggest file name
								//we can handle before truncating to 260.
								//next version we'll: increase the filename size we can handle without
								//truncating and replace some Netware APIs in buildpath function.


	if (!running || !Process)
		return STATUS_SUCCESS;

	write = !!(requestedAccessRights&(ACCESS_OPENW));
	read  = !!(requestedAccessRights&(ACCESS_OPENR));
	exec  = !!(requestedAccessRights&(ACCESS_EXEC));
	del   = !!(requestedAccessRights&(DELETE_FILE));

	if (!(SystemPrams.Dbg&0x80)) {
		if (!read && !write && !exec && !del) {
			return STATUS_SUCCESS;
			}
		}

	CRIT_ENTER;

	Action = GetAction(NULL);

	// -----------------------------
	// ksr - Certification, 7/21/2002

	if( (DWORD)Action == ERROR_MEMORY )
		CRIT_RETURN( ERROR_MEMORY );
	// -----------------------------

	if (!Action) {
		SystemPrams.FileNotScaned++;
		CRIT_RETURN (STATUS_SUCCESS);
		}

	//bnm sending in the 1k array to get all the name. will truncate to 260 if needed.
	//this is to prevent Action->Name to overflow and corrupt Action->semHandle
	//sts# 408795
	memset(&myBigFileName,0,1024);
	if (BuildPath(dirBase,pathString,pathComponentCount,volume,myBigFileName,nameSpace) == 0)
	{

		//fileSizeLength = strlen(myBigFileName);//for debug only

		if(strlen(myBigFileName)>SYSFILELEN)
		{
			dprintf("%s too large - truncating to 260 - length=%d\n",myBigFileName,strlen(myBigFileName));
		}
		memset(&Action->Name,0,SYSFILELEN);
		strncpy(Action->Name,myBigFileName,SYSFILELEN);
		Action->Name[SYSFILELEN-1] = '\0';
	}
	//end fix for sts#408795
	else
	{
		GiveAction(Action);
		SystemPrams.FileNotScaned++;
		CRIT_RETURN (STATUS_SUCCESS);
	}


	if (CheckSpecialNames(Action->Name)) {  // don't bother scanning the directory.
		GiveAction(Action);
		SystemPrams.FileNotScaned++;
		CRIT_RETURN (STATUS_SUCCESS);
		}

	Action->TokenUser.pSid = (PSID)&Action->TokenUser.Sid;
	Action->TokenUser.pSid->ConnectionID = connectionID;
	Action->DriveVolume = volume;
	Action->NameSpace = nameSpace;

	if (SystemPrams.DoIS) {
		val = SystemPrams.ISMask;
		if (	(write && (val&DENYWRITE)) ||
				(read  && (val&DENYREAD)) ||
				(del   && (val&DENYDELETE))
				) {
			Action->Flags |= FA_CHECK_IS;
			trap = 1;
			}
		}

	if (isFileWanted(Action)) {
		Action->Flags |= FA_FILE_NEEDS_SCAN;
		}
	else {
		if (!trap) {
			GiveAction(Action);
			CRIT_RETURN (STATUS_SUCCESS);
			}
		}


	if (SystemPrams.Dbg&1) {
		dprintf("%s%s%s%s%s open %s RAR:%08X AMB:%08X UID=%u\n",
			trap?"I":"",write?"W":" ",read?"R":" ",exec?"E":" ",del?"D":" ",
			Action->Name,requestedAccessRights,attributeMatchBits,Action->UID);
	}

	Action->Flags |= (read?FA_READ:0) | (exec?FA_EXEC:0) | (del?FA_DELETE:0) | (write?FA_WRITE:0);

	if ((write && SystemPrams.DoWrites)) {
		PFILE_ACTION NewAction;

		NewAction = GetAction(Action);
		if (NewAction) {
			NewAction->Flags |= FA_WRITE;
			AddToActionTable(NewAction,connectionID,task,(void *)ref,INSYSOPEN);
			if (SystemPrams.DoTrap && !client) {
				trap = 1;
				Action->Flags |= FA_GET_TRAP_DATA;
				NewAction->Flags |= FA_USE_TRAP_DATA;
				}
			}
		else
			SystemPrams.FileNotScaned++;
		}

	if (!client && (trap || (read&&SystemPrams.DoReads) || (exec&&SystemPrams.DoExecs))) {

		Action->Flags |= FA_BEFORE_OPEN;

		ret = WaitAndProcessAction(Action,TRUE);
		}
	else
		GiveAction(Action);

	CRIT_RETURN (ret); // return non-0 will not open file and return to user with error
}
#undef CRIT_ENTER
#undef CRIT_RETURN
/**************************************************************************************/
#define CRIT_ENTER SystemThreadsOwned++;
#define CRIT_RETURN(x) {SystemThreadsOwned--;  return x;}
int MyOpenFileFunctionEnd(DWORD connectionID,DWORD task,void *ref,int Han,int ccode) {

	PFILE_ACTION Action;
	OpenFileCallBackStruct *_ofcbs = (OpenFileCallBackStruct*)ref;	// NetWare 6 support
	CRIT_ENTER;

	Action = SearchActionTable(connectionID,task,ref,INSYSOPEN);
	if (!Action)
		CRIT_RETURN (0);

	if (!running || !Process) {
		GiveAction(Action);
		CRIT_RETURN (0);
		}

	if (!Han || ccode) {
		if (SystemPrams.Dbg&0x10) {
			ConsolePrintf("RTVSCAN: Open of %s failed with %08X\n",Action->Name,ccode);
		}
		GiveAction(Action);
		CRIT_RETURN (0);
		}

	if (SystemPrams.Dbg&0x02)
		ConsolePrintf("RTVSCAN: Open of %s succeded with han %08X\n",Action->Name,Han);
	Action->han = reinterpret_cast<HANDLE>(Han);

	// NetWare 6 support
	Action->NSSFlag=VolumeTable[_ofcbs->volume].NSSVol; //bnm added at Plugfest for nw6.0 support

//bnm: this will do until we change volume table update from polling to event driven
	if ( (NetWareVersion==6) && (Action->NSSFlag==unInitVolFlag) ) {
		GiveAction(Action);
		CRIT_RETURN (0);
		}

	AddToActionTable(Action,connectionID,task,(void *)Han,INOPENED);

	CRIT_RETURN (ccode);  // you should ALWAYS return the same value you received
}
#undef CRIT_ENTER
#undef CRIT_RETURN
/**************************************************************************************/
#define CRIT_ENTER SystemThreadsOwned++;	tgid = SetThreadGroupID(MainTgID);
#define CRIT_RETURN(x) {SystemThreadsOwned--; SetThreadGroupID(tgid); return x;}
int MyCloseFileFunctionEnd(DWORD connectionID,DWORD task,int Han,int ccode )
// ksr	- 2/11/02	SleepNotAllowed fix, Novell visit
{
						 													// NetWare 6 support
	PFILE_ACTION Action;
	int tgid;

	CRIT_ENTER;

	Action = SearchActionTable(connectionID,task,(void *)Han,INOPENED);
	if (!Action)
		CRIT_RETURN (0);

// NetWare 6 support

//bnm and gdf at plugFest: this nasty piece of code is done because:
// 1-we cannot call EndSleepNotAllowed when the file we are scanning is on NetWare 6 with NSS
// 2-we cannot use the zIsNSSVolume api that tells us wether we are on nss in CloseFile callback
// 3-we cannot search Action table without taking the Action item out in the calling function.
//Therefore, we had to:
// 1- determine the volume type in driver start	and save in in volume table
// 2- save volume type in Action item when we are in Post OpenFile
// 3- Check here for NW 6, and if not NSS then call EndSleepNotAllowed

//Design should be changed so we do not have to call EndSleepNotAllowed at all

	if( (NetWareVersion==4) || (NetWareVersion==5) || (NetWareVersion==6 && Action->NSSFlag==0) ) // 0=traditional
	{
		if(_SleepNotAllowedUseCount == NULL) //nw4 , or future nss versions
			EndSleepNotAllowed();  // novell says this is OK
		else if( *_SleepNotAllowedUseCount != 0 ) //nw5, nw6 traditional
			EndSleepNotAllowed();  // novell says this is OK
	}

	if (SystemPrams.Dbg&1) {
		dprintf("Close Done %s han = %08X\n",Action->Name,Han);
		}

	if (!running)
		GiveAction(Action);
	else {
		if (SystemPrams.DoCache)
			ClearFromCleanCache(Action);

		Action->Flags |= FA_AFTER_OPEN;

		if (SystemPrams.HoldOnClose) {
			ccode = WaitAndProcessAction(Action,TRUE);
			}
		else {
			Action->Flags |= FA_DELETE_WHEN_COMPLETE;
			if (GiveActionToUser(Action) != 0) {
				AbandonAction(Action);
				}
			}
		}

	// ksr	- 2/11/02	SleepNotAllowed fix, Novell visit
	if( (NetWareVersion==4) || (NetWareVersion==5) || (NetWareVersion==6 && Action->NSSFlag==0) ) // 0=traditional
		StartSleepNotAllowed();  // novell says this is OK

	CRIT_RETURN (ccode);
}
#undef CRIT_ENTER
#undef CRIT_RETURN
/********************************************************************************************/
#define CRIT_ENTER {SystemThreadsOwned++;	tgid = SetThreadGroupID(MainTgID);}
#define CRIT_RETURN(x) {SystemThreadsOwned--; SetThreadGroupID(tgid); return x;}
int MyRenameEntryFunction(DWORD ConnectionID,DWORD NameSpace,
									DWORD srcVolume,DWORD srcDirBase,char *srcPathString,DWORD srcPathComponentCount,
									DWORD dstVolume,DWORD dstDirBase,char *dstPathString,DWORD dstPathComponentCount) {


	DWORD ret=STATUS_SUCCESS;
	int tgid;

	if (!running || !Process || !SystemPrams.DoIS || !((SystemPrams.ISMask)&DENYRENAME))
		return STATUS_SUCCESS;

	CRIT_ENTER;

	ret = DoRename(ConnectionID,NameSpace,srcVolume,srcDirBase,srcPathString,srcPathComponentCount);
	if (ret == STATUS_SUCCESS)
		ret = DoRename(ConnectionID,NameSpace,dstVolume,dstDirBase,dstPathString,dstPathComponentCount);

	CRIT_RETURN (ret); // return non-0 will not open file and return to user with error

}
/**************************************************************************************/
PFILE_ACTION BlockUserThreadAndReturnAction(void) {

	int tid = (int)NTxGetCurrentThreadId();
	USERTHREAD UserThread,*cur=UserThreadHead;

	if (!tid) {
		NTxSleep(500);
		return NULL;
		}

	memset(&UserThread,0,sizeof(USERTHREAD));

	if (!cur) {
		UserThreadHead = &UserThread;
		}
	else {
		while(cur->Next)
			cur = cur->Next;
		cur->Next = &UserThread;
		}

	UserThread.tid = tid;

	SuspendThread(tid);
	return UserThread.Action;
}
//============================================================================
void  EmptyUserThreadQueue(void)
{
	USERTHREAD *cur = UserThreadHead;

	UserThreadHead = NULL;

	while (cur) {
		ResumeThread(cur->tid);
		cur = cur->Next;
		}
}
//============================================================================
DWORD AddActionToWaitQueue(PFILE_ACTION Action) {

	PFILE_ACTION cur=WaitQueue;

	if (SystemPrams.WaitingActions > BUSY_QUEUE)
		return 0xffffffff;

	Action->Next = NULL;

	if (!cur) {
		WaitQueue = Action;
		}
	else {
		while(cur->Next)
			cur = cur->Next;
		cur->Next = Action;
		}

	Action->Flags |= FA_OWNED_BY_QUEUE;
	SystemPrams.WaitingActions++;

	return STATUS_SUCCESS;
}
/********************************************************************************************/
DWORD WakeUpUserThread(PFILE_ACTION Action) {


	USERTHREAD *cur = UserThreadHead;

	if (!cur)
		return AddActionToWaitQueue(Action);

	UserThreadHead = cur->Next;

	cur->Action = Action;
	ResumeThread(cur->tid);

	return STATUS_SUCCESS;
}
//============================================================================
// gdf CW conversion changed DWORD to ULONG as DWORD is an int in nlm.h
DWORD DeviceIoControl(HANDLE hDevice,/*DWORD*/ ULONG Command,void *BufIn,DWORD SizeIn,void *BuffOut,DWORD SizeOut,DWORD *ReturnSize,DWORD *IdontKnow) {
	DWORD Status=0xffffffff;
	PSYS_PRAMS USystemPrams;
	PFILE_ACTION Action,UAction;
	int f;

	REF(hDevice);
	REF(IdontKnow);

	switch (Command) {
		case IOCTL_GET_ACTION:
			Action = GetWaitingAction();
			if (Action) {
				Action->Flags |= FA_OWNED_BY_USER;
				memcpy(BuffOut,Action,sizeof(FILE_ACTION));
				if (ReturnSize)
					*ReturnSize = sizeof(FILE_ACTION);
				Status = STATUS_SUCCESS;
				}
			else {
				Action = BlockUserThreadAndReturnAction();
				if (Action) {
					Action->Flags |= FA_OWNED_BY_USER;
					memcpy(BuffOut,Action,sizeof(FILE_ACTION));
					if (ReturnSize)
						*ReturnSize = sizeof(FILE_ACTION);
					}
				else
					if (ReturnSize)
						*ReturnSize = 0;
				Status = STATUS_SUCCESS;
				}
			break;

		case IOCTL_COMPLETE_ACTION:
			UAction = (PFILE_ACTION)BufIn;
			Action = (PFILE_ACTION)UAction->Address;
			if (VerifyAction(Action)) {
				Action->Status = UAction->Status;
				Action->Flags &= ~FA_OWNED_BY_USER;
				CompleteAction(Action);
				}
			Status = STATUS_SUCCESS;
			break;

		case IOCTL_SET_SYS_PRAMS:
			if (SystemPrams.DoCache)
				FlushCleanCache();
			USystemPrams = (PSYS_PRAMS)BufIn;
			memcpy(&SystemPrams, USystemPrams, min(offsetof(SYS_PRAMS,RDONLY),SizeIn));
			strupr(SystemPrams.ExtList);
			strupr(SystemPrams.ClientDirectory);
			Status = STATUS_SUCCESS;
			break;

		case IOCTL_GET_SYS_PRAMS:
			USystemPrams = (PSYS_PRAMS)BuffOut;
			memcpy(USystemPrams,&SystemPrams, f=min(sizeof(SYS_PRAMS),SizeOut));
			if (ReturnSize)
				*ReturnSize = f;
			Status = STATUS_SUCCESS;
			break;

		case IOCTL_INQUIRE_SYSTEM:
			if (ReturnSize)
				*ReturnSize = 0;
			Status = STATUS_SUCCESS;
			break;


		case IOCTL_START_CAPTURE:
			if (!Process) {
				Process = 1;
				Status = STATUS_SUCCESS;
				}
			break;

		case IOCTL_END_CAPTURE:
			if (Process) {
				Process = 0;
				Status = STATUS_SUCCESS;
				}
			break;

		case IOCTL_INTEL_SHUTDOWN:
			EmptyUserThreadQueue();
			Status = STATUS_SUCCESS;
			break;

		case IOCTL_SET_EP:
			break;
		}

	return Status==STATUS_SUCCESS?TRUE:FALSE;
}
/********************************************************************************************/
void CheckVolumeTable() {
	int i, cc;
	char	tmpName[17];


//	FILE_SERV_INFO fsInfo;  // gdf 06/14/00
//	int MaxVolumes = 0;     // gdf 06/14/00

//	memset (&fsInfo, 0, sizeof(FILE_SERV_INFO));  // gdf 04/07/00
//	GetServerInformation(sizeof (FILE_SERV_INFO),&fsInfo);  // gdf 04/07/00
//	MaxVolumes = fsInfo.maxVolumesSupported;    // gdf 04/07/00

//  gdf 04/07/00 The use of "VOLUMES" (a constant)to determine the max
//  number of volumes on a NetWare server has been deleted since the
//  max number is not necessarly a constant.  The actual value of max
//  in NW5.x servers varies depending on the presence of NSS volumes or
//	volume clusters. The GetServerInformation API is used to determine max volumes.

//	for (i = 0; i < VOLUMES; i++) {   // gdf 04/07/00

	NTxSemaWait(volumeSemHandle);
	for (i = 0; i < MaxVolumes; i++)
	{    // gdf 04/07/00
		*tmpName = 0;

		//bnm: this will do until we change volume table update from polling to event driven
		VolumeTable[i].NSSVol = unInitVolFlag;	 // gdf 07/19/01 added at plugfest
		// ksr - 1/22/02	 Defect # 378748  virtual volume flag for _ADMIN
		VolumeTable[i].isVirtualVolume = FALSE;

		cc = GetVolumeName(i,tmpName);

		// ksr	- 2/11/02	SleepNotAllowed fix, Novell visit
		//ThreadSwitchWithDelay();

		if (!cc && *tmpName)
    	{

    		strncpy(VolumeTable[i].name,tmpName,17);

	        if(_zIsNSSVolume)	 // gdf 07/19/01 added at plugfest
				VolumeTable[i].NSSVol = _zIsNSSVolume(VolumeTable[i].name);	 // gdf 07/19/01 added at plugfest

			// ksr - 1/22/02	Defect # 378748
	        if( strcmp( "_ADMIN", tmpName ) == 0 )
				VolumeTable[i].isVirtualVolume = TRUE;

		}
	}
	NTxSemaSignal(volumeSemHandle);
}
//============================================================================
void ActChk(void *nothing) {

	int i;

	REF(nothing);

	while (SystemRunning) {
		CheckActionTable();
		if (SystemPrams.Dbg&0x10) {
			_printf("WA:%u OWFC:%u OSNE: %u FNS:%u FS:%u A:%u AIU:%u TO:%u FLPU:%u\n",
				SystemPrams.WaitingActions,
				SystemPrams.OpensWaitingForClose,
				SystemPrams.OpensStartedNotEnded,
				SystemPrams.FileNotScaned,
				SystemPrams.FilesScaned,
				SystemPrams.Allocs,
				SystemPrams.ActionsInUse,
				SystemPrams.TimedOut,
				SystemPrams.FilesSlippedPastUs,
				SystemPrams.Faults,
				SystemPrams.ClientCount);
			}
		for (i=0;i<30&&SystemRunning;i++)
			NTxSleep(1000);
		}

}
/******************************************************************/
DWORD DriverStart(void)
{
	LONG cc;

	GetServerMajorVersion();



#ifdef  SYM_JAPAN
	ConsolePrintf("PSCAN: For Netware-J ONLY\n");
#endif

	AllocTag = AllocateResourceTag(GetNLMHandle(),
                                   reinterpret_cast<unsigned char*>(const_cast<char*>("Intel Pre-Scan Stacks")),
                                   AllocSignature);
	if (AllocTag == 0) {
		_printf(LS(IDS_ADD_SERVER_MEM));
		return (-1);
		}

	volumeSemHandle = (long)NTxSemaOpen(1, NULL);

//  gdf added at plugfest 07/18/01 import the function to check for NSS Volumes

	// NetWare 6 support
    _zIsNSSVolume = (P_ZISNSSVOLUME)ImportSymbol(GetNLMHandle(),"zIsNSSVolume");
	_SleepNotAllowedUseCount= (int*)ImportSymbol(GetNLMHandle(),"SleepNotAllowedUseCount");

	CheckVolumeTable();

	// tell CLIB that I'll be handling my own context setting...
	{
		typedef void (*PCONTEXTSPECIFIERADDR)(int threadID, int contextSpecifier);
		PCONTEXTSPECIFIERADDR contextSpecifierAddr = NULL;

		contextSpecifierAddr = (PCONTEXTSPECIFIERADDR)ImportSymbol(GetNLMHandle(), "SetThreadContextSpecifier");
		if (contextSpecifierAddr != NULL)
			contextSpecifierAddr((int)NTxGetCurrentThreadId(), NO_CONTEXT);
	}

	memset(&SystemPrams,0,sizeof(SYS_PRAMS));

	SystemPrams.ScanInKernal = 0;
	SystemPrams.DoWrites = 1;
	SystemPrams.DoReads = 1;
	SystemPrams.DoExecs = 1;
	SystemPrams.DoFloppy = 1;
	SystemPrams.DoNetwork = 0;
	SystemPrams.DoHardDisk = 1;
	SystemPrams.DoCDRom = 1;
	SystemPrams.HoldOnClose = 1;
#ifdef DEBUG
	SystemPrams.Dbg = 0xf;
#else
	SystemPrams.Dbg = 0;
#endif
	StrCopy(SystemPrams.ExtList,"EXE,COM,BIN,OVL,DLL,SYS,DOC,DOT");
	SystemPrams.MySize = sizeof(SYS_PRAMS);

	FlushCleanCache();

	cc = StartSystem();
	if (cc)
		return cc;

	// check to see what behavior this OS has and act right.
	CallState = 1;
	Process = 1;
	fopen("SYS:\\nothing","rt");
	if (CallState == 2)
		NoCallAfterFail = TRUE;
	CallState = 0;

	Process = 0;

	MyBeginThread(ActChk,0,"RTV ActChk");

	return(STATUS_SUCCESS);
}

//============================================================================
VOID DriverUnload(void)
{
	StopSystem();
	return;
}
/*******************************************************************/
void GetServerMajorVersion()
{
	int
		ccode;
	GetOSVersionInfoStructure
		buf;

	ccode = SSGetOSVersionInfo((BYTE *)&buf,sizeof(GetOSVersionInfoStructure));
	if (ccode != 0)
		majorServerVersion = 3;
	else {
		majorServerVersion = buf.OSMajorVersion;
		minorServerVersion = buf.OSMinorVersion;
	}

	NetWareVersion = majorServerVersion;

	// NetWare 6 support
	//
	// NetWare version 6  (5.60 - for now, encoded as  major= 5 and minor = 60 (3c hex)
	//
	if( ( majorServerVersion >= 5 ) && ( minorServerVersion >= 0x3c ) )
	{
		NetWareVersion = 6;
	}

}

/*********************************************************************/
static void ResolvePath(char *path)
{
#ifdef  SYM_JAPAN
	char
		c,
		*dest,
		*src,
		*end,
		*array[30];
	int
		mk,
		arrayCount=0;

	dest = path;
	src = path;
	end = src + NumBytes(path);

	while (src < end)
	{
		mk = 1;
		c = *src;		//MLR Fixed
		src=NextChar(src);

		if ((c >= 0x81 && c <=0x9f) || (c >= 0xe0 && c <=0xfc)) {
			*dest++ = c;
			*dest++ = *src++;
			continue;
			}

		if (c == 0xff) {
			*dest++ = (*src++)&0x7f;
			continue;
			}

		if (c == ':')
			array[arrayCount++] = dest+1;

		if (c == '/') {
			c = '\\';
			while (*src == '/') {
				mk = 0;
				src++;
				if (arrayCount)
					dest = array[--arrayCount];
				else
					dest = array[0];
				}
			array[arrayCount++] = dest+mk;
			}

		if (mk)
			*dest++ = toupper(c);
	}

	*dest = '\0';                       /* Put the final null on! */
#else
	char
		c,
		*dest,
		*src,
		*end;
	int
		mk = 0;
	dest = path;
	src = path;
	end = src + NumBytes(path);

	while (src < end)
	{
		/*      Old code would continue on 127, mask high bit if greater than 127,
			then continue if equal to 127... Now we mask and continue if equal
			to 127  */

//		c = (*src++);
		c = (*src);		//MLR Fixed?
		src=NextChar(src);

		/*switch (c&0x7f) {
			case '*':
			case '?':
			case '/':
			case '\\':
			case '.':
				c &= 0x7f;
				break;
			}

		if (c == 0x7f || c == 0xff)
			continue;
		*/
	  if (c == '/')
			c = '\\';

		if (c != '\\') {
			*dest = c; // toupper(c);		//MLR Fixed
			dest=NextChar(dest);
			mk = 0;
			}
		else   {
			if (mk) {
				for (dest -= 2 ; *dest != '\\' ; dest=PrevChar(path,dest)) {
					if (dest < path) {
						path[0] = 0;
						return;
						}
					if (*dest == ':')
						break;
					else {	//bnm STS 41064 infinite loop. get us out of this loop when file name incorrectly has multiple "/"
						dprintf("RTVSCAN: ResolvePath: Invalid filename : %s\n" , path);
						path[0] = 0;
						return;
						}

					}
				dest=NextChar(dest);
				}       else    {
				*dest = '\\';		//MLR Fixed
				dest=NextChar(dest);
				mk = 1;
				}
			}
		}
	*dest = '\0';                       /* Put the final null on! */

#endif
}       /*      END OF  ResolvePath     */

/***********************************************************************/
int BuildPath(long dirnumber,char *fpath,int names,long volumenumber,char *path,long namespc) {

	char *cp;
	int a,vlen,plen,ret;

#if 0
	long
		tmpTGID;
#endif

	cp = path;

	StrCopy(path,VolumeTable[volumenumber].name);
	vlen = NumBytes(path);
	cp = path + vlen;
	*cp++ = ':';
	*cp = 0;


	if (dirnumber == 0) {
		ret = 0;
		plen = 1;
		}
	else
		ret = MapDirectoryNumberToPath(volumenumber,dirnumber,namespc,cp,MAX_STACK-18,&plen);
#if 0
	ret = MapDirectoryNumberToPath(volumenumber,0,1,cp,MAX_STACK-18,&plen);
	tmpTGID = SetThreadGroupID(MainTgID);
	ret = MapDirectoryNumberToPath(volumenumber,0,1,cp,MAX_STACK-18,&plen);
	SetThreadGroupID(tmpTGID);
#endif

	if (ret) {
		StrCopy(cp,":[NDF]");
		return(-1);
	}

	ConvertPathToNCP(cp,plen);
//	cp[0] = ':';
//	if (cp[1] != '/') {
//		memmove(cp+1,cp);
//		cp[1] = '/';
//		plen++;
//		}
	cp += plen;

	if (plen != 1) {
		*cp++ = '/';		//MLR Fixed
	}

	for (a = 0; a < names; a++)
	{
	int len;
		len = fpath[0];
		//bnm add to check file name length
		if(len+strlen(path)>1024-1)
			return(-1);
		if (0 != len)   // CAJ  This seems to happen for some OpenFile notifications comming from AppleTalk File Protocol
			memcpy(cp,fpath+1,len);   // calling this with a zero length causes NetWare to ABEND.
		cp += len;
		*cp++ = '/';		//MLR Fixed
		fpath += len + 1;
	}
	*(--cp) = 0;  /* remove last '/' */ // MLR Fixed

	ResolvePath(path);

	return(0);
}       /*      END OF  BuildPath       */

/*******************************************************************************************/
void CheckActionTable() {

	PFILE_ACTION cur,last=NULL;

startover:
	cur  = ActionHead;

	while (cur) {
		if (cur->QueueType == INSYSOPEN) {
			if ((unsigned long)(GetCurrentTime() - cur->Time) > (unsigned long)90) {
				if (last == NULL)
					ActionHead = cur->Next;
				else
					last->Next = cur->Next;
				cur->Flags &= ~FA_IN_TABLE;
				SystemPrams.OpensStartedNotEnded--;
				AbandonAction(cur);
				goto startover;
				}
			}
		last = cur;
		cur = cur->Next;
		}
}
/****************************************************************/
/****************************************************************/
/****************************************************************/
/****************************************************************/
/****************************************************************/
int OpenFileCallBackEnt(OpenFileCallBackStruct *ofcbs) {

	if (!Process)
		return 0;

	// don't monitor connection 0, except for the start-up test...
	if (ofcbs->connection == 0 && CallState == 0)
		return(0);

	// start-up test code
	if (CallState == 1 && ofcbs->connection == 0) {
		CallState = 2;
		return -1;
		}

// gdf 06/14/00 don't monitor volumes greater than 32
// gdf 06/14/00	if (ofcbs->volume >= 32)

// gdf 06/14/00 don't monitor volumes greater than Max Volumes supported
	if (ofcbs->volume >= MaxVolumes)  // gdf 06/14/00
		return(0);


	return MyOpenFileFunctionStart(ofcbs->connection,ofcbs->task,(void *)ofcbs,
								   ofcbs->requestedAccessRights,
								   ofcbs->nameSpace,ofcbs->volume,ofcbs->dirBase,
								   reinterpret_cast<char*>(ofcbs->pathString),
                                   ofcbs->pathComponentCount, 0);
}
/**************************************************************************************/
int GenericOpenCallBackEnt(GenericOpenCreateCBStruct *ofcbs) {

	if (!Process)
		return 0;

	// don't monitor connection 0, except for the start-up test...
	if (ofcbs->connection == 0 && CallState == 0)
		return(0);

	// start-up test code
	if (CallState == 1 && ofcbs->connection == 0) {
		CallState = 2;
		return -1;
		}

//  gdf 06/14/00 don't monitor volumes greater than 32
//	gdf 06/14/00 if (ofcbs->volume >= 32)

// gdf  06/14/00 don't monitor volumes greater than Max Volumes supported
	if (ofcbs->volume >= MaxVolumes)  // gdf 06/14/00
		return(0);



	return MyOpenFileFunctionStart(ofcbs->connection,ofcbs->task,(void *)ofcbs,
								   ofcbs->requestedAccessRights,
								   ofcbs->nameSpace,ofcbs->volume,ofcbs->dirBase,
								   reinterpret_cast<char*>(ofcbs->pathString),
                                   ofcbs->pathComponentCount, 0);
}
/****************************************************************************************/
int CreateAndOpenFileCallBackEnt(CreateAndOpenCallBackStruct *ofcbs)
{
	if (!Process)
		return 0;

	// don't monitor connection 0, except for the start-up test...
	if (ofcbs->connection == 0 && CallState == 0)
		return(0);

	// start-up test code
	if (CallState == 1 && ofcbs->connection == 0) {
		CallState = 2;
		return -1;
		}

//  gdf 06/14/00 don't monitor volumes greater than 32
//	gdf 06/14/00 if (ofcbs->volume >= 32)

// gdf don't monitor volumes greater than Max Volumes supported
	if (ofcbs->volume >= MaxVolumes) // gdf 06/14/00
		return(0);


	return MyOpenFileFunctionStart(ofcbs->connection,ofcbs->task,(void *)ofcbs,
								   ofcbs->requestedAccessRights,
								   ofcbs->nameSpace,ofcbs->volume,ofcbs->dirBase,
								   reinterpret_cast<char*>(ofcbs->pathString),
                                   ofcbs->pathComponentCount, 0);
}
/****************************************************************************************/
void OpenFileCallBackExt(OpenFileCallBackStruct *ofcbs,int ccode) {

	if (!Process)
		return;

	// don't monitor connection 0, except for the start-up test...
	if (ofcbs->connection == 0 && CallState == 0)
		return;

	// start-up test code
	if (CallState == 2 && ofcbs->connection == 0) {
		CallState = 3;
		return;
		}

//  gdf 06/14/00 don't monitor volumes greater than 32
//	gdf 06/14/00 if (ofcbs->volume >= 32)

// gdf  06/14/00 don't monitor volumes greater than Max Volumes supported
	if (ofcbs->volume >= MaxVolumes)  // gdf 06/14/00
		return;

	MyOpenFileFunctionEnd(ofcbs->connection,ofcbs->task,(void *)ofcbs,*ofcbs->fileHandle,ccode);
}
/****************************************************************************************/
void CreateAndOpenFileCallBackExt(CreateAndOpenCallBackStruct *ofcbs,int ccode) {

	if (!Process)
		return;

	// don't monitor connection 0, except for the start-up test...
	if (ofcbs->connection == 0 && CallState == 0)
		return;

	// start-up test code
	if (CallState == 2 && ofcbs->connection == 0) {
		CallState = 3;
		return;
		}

//  gdf 06/14/00 don't monitor volumes greater than 32
//	gdf 06/14/00 if (ofcbs->volume >= 32)

//  gdf  06/14/00 don't monitor volumes greater than Max Volumes supported
	if (ofcbs->volume >= MaxVolumes)  // gdf 06/14/00
		return;

	MyOpenFileFunctionEnd(ofcbs->connection,ofcbs->task,(void *)ofcbs,*ofcbs->fileHandle,ccode);
}
/****************************************************************************************/
void GenericOpenCallBackExt(GenericOpenCreateCBStruct *ofcbs, int ccode) {

	if (!Process)
		return;

	// don't monitor connection 0, except for the start-up test...
	if (ofcbs->connection == 0 && CallState == 0)
		return;

	// start-up test code
	if (CallState == 2 && ofcbs->connection == 0) {
		CallState = 3;
		return;
		}

//  gdf 06/14/00 don't monitor volumes greater than 32
//	gdf 06/14/00 if (ofcbs->volume >= 32)

//  gdf  06/14/00 don't monitor volumes greater than Max Volumes supported
	if (ofcbs->volume >= MaxVolumes)  // gdf 06/14/00
		return;

	MyOpenFileFunctionEnd(ofcbs->connection,ofcbs->task,(void *)ofcbs,*ofcbs->fileHandle,ccode);
}
/**********************************************************************************************/
void CloseFileCallBackExt(CloseFileCallBackStruct *cbs,int ccode)
{

	if (!Process)
		return;

	// don't monitor connection 0
	if (cbs->connection == 0 && CallState == 0)
		return;

	// ksr	- 2/11/02	SleepNotAllowed fix, Novell visit
	MyCloseFileFunctionEnd(cbs->connection,cbs->task,cbs->fileHandle,ccode);

}

/*******************************************************************************************/
int EraseFileCallBackEnt(EraseFileCallBackStruct *ofcbs) {

	if (!Process)
		return 0;

	// don't monitor connection 0, except for the start-up test...
	if (ofcbs->connection == 0 && CallState == 0)
		return(0);

	// start-up test code
	if (CallState == 1 && ofcbs->connection == 0) {
		CallState = 2;
		return -1;
		}

//  gdf 06/14/00 don't monitor volumes greater than 32
//	gdf 06/14/00 if (ofcbs->volume >= 32)

//  gdf  06/14/00 don't monitor volumes greater than Max Volumes supported
	if (ofcbs->volume >= MaxVolumes)  // gdf 06/14/00
		return(0);


	return MyOpenFileFunctionStart(ofcbs->connection,ofcbs->task,(void *)ofcbs,
								   DELETE_FILE,
								   ofcbs->nameSpace,ofcbs->volume,ofcbs->dirBase,
								   reinterpret_cast<char*>(ofcbs->pathString),
                                   ofcbs->pathComponentCount, 0);
}
/*************************************************************************************************/
int GenericEraseFileCallBackEnt(GenericEraseFileCBStruct *ofcbs) {

	if (!Process)
		return 0;

	// don't monitor connection 0, except for the start-up test...
	if (ofcbs->connection == 0 && CallState == 0)
		return(0);

	// start-up test code
	if (CallState == 1 && ofcbs->connection == 0) {
		CallState = 2;
		return -1;
		}

//  gdf 06/14/00 don't monitor volumes greater than 32
//	gdf 06/14/00 if (ofcbs->volume >= 32)

//  gdf  06/14/00 don't monitor volumes greater than Max Volumes supported
	if (ofcbs->volume >= MaxVolumes)  // gdf 06/14/00
		return(0);


	return MyOpenFileFunctionStart(ofcbs->connection,ofcbs->task,(void *)ofcbs,
								   DELETE_FILE,
								   ofcbs->nameSpace,ofcbs->volume,ofcbs->dirBase,
								   reinterpret_cast<char*>(ofcbs->pathString),
                                   ofcbs->pathComponentCount, 0);
}
/**********************************************************************************************/
int DelDirCallBackEnt(DeleteDirCallBackStruct *ofcbs) {

	if (!Process)
		return 0;

	// don't monitor connection 0, except for the start-up test...
	if (ofcbs->connection == 0 && CallState == 0)
		return(0);

	// start-up test code
	if (CallState == 1 && ofcbs->connection == 0) {
		CallState = 2;
		return -1;
		}

//  gdf 06/14/00 don't monitor volumes greater than 32
//	gdf 06/14/00 if (ofcbs->volume >= 32)

//  gdf  06/14/00 don't monitor volumes greater than Max Volumes supported
	if (ofcbs->volume >= MaxVolumes)  // gdf 06/14/00
		return(0);


	return MyOpenFileFunctionStart(ofcbs->connection,5,(void *)ofcbs,
											DELETE_FILE,
											ofcbs->nameSpace,ofcbs->volume,ofcbs->dirBase,
											reinterpret_cast<char*>(ofcbs->pathString),ofcbs->pathComponentCount,
											0);
}
/******************************************************************************************/
/******************************************************************************************/
int RenameEntryCallBackEnt(RenameMoveEntryCallBackStruct *cbs)
{

	if (!Process)
		return 0;

	// don't monitor connection 0
	if (cbs->connection == 0 && CallState == 0)
		return(0);

//  gdf 06/14/00 don't monitor volumes greater than 32
//	gdf 06/14/00 if (ofcbs->volume >= 32)

//  gdf  06/14/00 don't monitor volumes greater than Max Volumes supported
	if (cbs->volume >= MaxVolumes)  // gdf 06/14/00
		return(0);

	return  MyRenameEntryFunction(cbs->connection,cbs->nameSpace,
				cbs->volume,cbs->dirBase,reinterpret_cast<char*>(cbs->pathString),
                cbs->pathComponentCount, cbs->volume,cbs->newDirBase,
                reinterpret_cast<char*>(cbs->newPathString),cbs->originalNewCount);

}
/****************************************************************/
int GenericRenameCallBackEnt(GenericRenameCBStruct *cbs)
{

	if (!Process)
		return 0;

	// don't monitor connection 0
	if (cbs->connection == 0 && CallState == 0)
		return(0);

//  gdf 06/14/00 don't monitor volumes greater than 32
//	if (cbs->srcVolume >= 32 || cbs->dstVolume >= 32)  // gdf 06/14/00

// gdf  06/14/00 don't monitor volumes greater than Max Volumes supported
	if (cbs->srcVolume >= MaxVolumes || cbs->dstVolume >= MaxVolumes)  // gdf 06/14/00
		return(0);


	return  MyRenameEntryFunction(cbs->connection,cbs->nameSpace,
				cbs->srcVolume,cbs->srcDirBase,reinterpret_cast<char*>(cbs->srcPathString),
                cbs->srcPathComponentCount, cbs->dstVolume,cbs->dstDirBase,
                reinterpret_cast<char*>(cbs->dstPathString),cbs->dstPathComponentCount);

}
/****************************************************************************************/
/****************************************************************************************/
/**************          mapping stuff             **************************************/
/****************************************************************************************/
/****************************************************************************************/
/****************************************************************************************/
int ReMapOpenFile(void) {
	int
		ccode;

	ccode = NWAddFSMonitorHook(FSHOOK_PRE_OPENFILE, OpenFileCallBackEnt, &openPreCallBackHandle);
	if (ccode)
		return -1;

	ccode = NWAddFSMonitorHook(FSHOOK_POST_OPENFILE, OpenFileCallBackExt, &openPostCallBackHandle);
	if (ccode) {
		NWRemoveFSMonitorHook(FSHOOK_PRE_OPENFILE, openPreCallBackHandle);
		return -1;
	}


	if (majorServerVersion >= 4)
	{
		ccode = NWAddFSMonitorHook(FSHOOK_PRE_GEN_OPEN_CREATE, GenericOpenCallBackEnt, &genericOpenPreCallBackHandle);
		if (ccode != 0)
		{
			return(0);	// if we cant hook this, its because the OS cant use it, so don't worry
		}
		ccode = NWAddFSMonitorHook(FSHOOK_POST_GEN_OPEN_CREATE, GenericOpenCallBackExt, &genericOpenPostCallBackHandle);
		if (ccode)
		{
			NWRemoveFSMonitorHook(FSHOOK_PRE_GEN_OPEN_CREATE, genericOpenPreCallBackHandle);
			genericOpenPreCallBackHandle = 0;
		}
	}
	return 0;
}
/*********************/
void UnMapOpenFile(void)
{
	NWRemoveFSMonitorHook(FSHOOK_PRE_OPENFILE, openPreCallBackHandle);
	NWRemoveFSMonitorHook(FSHOOK_POST_OPENFILE, openPostCallBackHandle);
	if (genericOpenPreCallBackHandle != 0)
	{
		NWRemoveFSMonitorHook(FSHOOK_PRE_GEN_OPEN_CREATE, genericOpenPreCallBackHandle);
		genericOpenPreCallBackHandle = 0;
	}
	if (genericOpenPostCallBackHandle != 0)
	{
		NWRemoveFSMonitorHook(FSHOOK_POST_GEN_OPEN_CREATE, genericOpenPostCallBackHandle);
		genericOpenPostCallBackHandle = 0;
	}

}       /*      END OF  UnMapOpenFile   */


/**********************/
int ReMapCloseFile(void)
{
	int
		ccode;

	ccode = NWAddFSMonitorHook(FSHOOK_POST_CLOSEFILE, CloseFileCallBackExt, &closePostCallBackHandle);
	if (ccode) {
		return -1;
	}
	return 0;

}       /*      END OF  ReMapCloseFile  */

/******************************************************************************/
void UnMapCloseFile(void)
{
	NWRemoveFSMonitorHook(FSHOOK_POST_CLOSEFILE, closePostCallBackHandle);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//
// ksr - Added EVENT_CLEAR_CONNECTION event
// 		8-6-2001
//
//-----------------------------------------------------------------------------

#undef CRIT_ENTER
#undef CRIT_RETURN
// Globals
//int RetValueFromWarnProc=0; // Let event continue by default
//LONG eventHandle; 			// The EVENT_CLEAR_CONNECTION event handle

//-----------------------------------------------------------------------------
#define CRIT_ENTER SystemThreadsOwned++;	tgid = SetThreadGroupID(MainTgID);
#define CRIT_RETURN(x) {SystemThreadsOwned--; SetThreadGroupID(tgid); return x;}

//-----------------------------------------------------------------------------

PFILE_ACTION SearchActionTableForClearConn( DWORD connectionID )
{

	PFILE_ACTION cur,last=NULL;

	cur = ActionHead;
	while (cur)
	{
		if ( cur->ConnectionID == connectionID )
		{
			if (last == NULL)
				ActionHead = cur->Next;
			else
				last->Next = cur->Next;
			cur->Flags &= ~FA_IN_TABLE;
			break;
		}
		last = cur;
		cur = cur->Next;
	}

	return cur;
}

//-----------------------------------------------------------------------------

int MyClearConnectionFunctionEnd( DWORD connectionID )
{

	PFILE_ACTION Action;
	int tgid;
	int ccode;

	CRIT_ENTER;

//	dprintf("Connection %d terminated by watchdog \n",connectionID );

	Action = SearchActionTableForClearConn( connectionID );
	if ( !Action )
		CRIT_RETURN ( 0 );

 	do
	{
		dprintf("Clear Connection Done %s \n",Action->Name );

		if ( !running )
			GiveAction( Action );
		else
		{
			if ( SystemPrams.DoCache )
				ClearFromCleanCache( Action );

			Action->Flags |= FA_AFTER_OPEN;

			if ( SystemPrams.HoldOnClose )
			{
				ccode = WaitAndProcessAction( Action, TRUE );
			}
			else
			{
				Action->Flags |= FA_DELETE_WHEN_COMPLETE;
				if ( GiveActionToUser( Action ) != 0 )
				{
					AbandonAction( Action );
				}
			}
		}
		Action = SearchActionTableForClearConn( connectionID );
	}
	while (Action);

	CRIT_RETURN ( ccode );
}

//-----------------------------------------------------------------------------

static void ClearConnectionCallBack( DWORD connectionId, DWORD param2 )
{
    REF (param2);

	if (!Process)
		return;

	// don't monitor connection 0
	if ( connectionId == 0 && CallState == 0)
		return;

	MyClearConnectionFunctionEnd( connectionId );

}

//-----------------------------------------------------------------------------

// This function gets called before the event EVENT_CLEAR_CONNECTION occurs.
// This gives me a chance to have the OS warn the operator before allowing
// the event to continue. In order to get NetWare to display the warning
// message, return a non-zero value.
/*
LONG WarnClearConnection( void (*OutPutFunc)(void *fmt,...), LONG parameter )
{
   OutPutFunc("Warning CLEARING CONNECTON ID %d...\n - Returning %u to NetWare\n",
                  parameter, RetValueFromWarnProc );
   return RetValueFromWarnProc;
}
*/
//-----------------------------------------------------------------------------

int ReMapClearConnection( void )
{
	ConnEventHandle = RegisterForEvent( EVENT_CLEAR_CONNECTION,
												ClearConnectionCallBack,
												//WarnClearConnection );
												NULL );
	if( ConnEventHandle == -1)
	{
		//dprintf("Error registering event EVENT_CLEAR_CONNECTION!\n");
		return -1;
	}
	return 0;

}

//-----------------------------------------------------------------------------

void UnMapClearConnection( void )
{
	if( ConnEventHandle != NULL)
	{
		if(UnregisterForEvent( ConnEventHandle ) != NULL )
			dprintf("Error during Unregister of event EVENT_CLEAR_CONNECTION\n");
		else
			ConnEventHandle=0;
	}
//	   return;
}

//-----------------------------------------------------------------------------
//      END OF  ClearConnection  Functions
//-----------------------------------------------------------------------------

/******************************************************************************/
int ReMapCreateAndOpenFile(void)
{
	int
		ccode;

	ccode = NWAddFSMonitorHook(FSHOOK_PRE_CREATE_OPENFILE, CreateAndOpenFileCallBackEnt, &createPreCallBackHandle);
	if (ccode)
		return -1;

	ccode = NWAddFSMonitorHook(FSHOOK_POST_CREATE_OPENFILE, CreateAndOpenFileCallBackExt, &createPostCallBackHandle);
	if (ccode) {
		NWRemoveFSMonitorHook(FSHOOK_PRE_CREATE_OPENFILE, createPreCallBackHandle);
		return -1;
	}
	return 0;
}
/*********************/
void UnMapCreateAndOpenFile(void) {
		NWRemoveFSMonitorHook(FSHOOK_PRE_CREATE_OPENFILE, createPreCallBackHandle);
		NWRemoveFSMonitorHook(FSHOOK_POST_CREATE_OPENFILE, createPostCallBackHandle);
}

/**********************/
int ReMapEraseFile(void) {

	int
		ccode;

	ccode = NWAddFSMonitorHook(FSHOOK_PRE_ERASEFILE, EraseFileCallBackEnt, &erasePreCallBackHandle);
	if (ccode)
		return -1;

	// now hook the del directory functions...
	ccode = NWAddFSMonitorHook(FSHOOK_PRE_DELETEDIR, DelDirCallBackEnt, &delDirPreCallBackHandle);
	if (ccode)
	{
		NWRemoveFSMonitorHook(FSHOOK_PRE_ERASEFILE, erasePreCallBackHandle);
		return -1;
	}

	if (majorServerVersion >= 4)
	{
		NWAddFSMonitorHook(FSHOOK_PRE_GEN_ERASEFILE, GenericEraseFileCallBackEnt, &genErasePreCallBackHandle);
	}

	return 0;
}
/*********************/
void UnMapEraseFile(void) {
		NWRemoveFSMonitorHook(FSHOOK_PRE_ERASEFILE, erasePreCallBackHandle);
		NWRemoveFSMonitorHook(FSHOOK_PRE_DELETEDIR, delDirPreCallBackHandle);
		if (majorServerVersion >= 4)
		{
			NWRemoveFSMonitorHook(FSHOOK_PRE_GEN_ERASEFILE, genErasePreCallBackHandle);
		}

}
/**********************/
int ReMapRenameEntry(void) {
	int ccode;

	ccode = NWAddFSMonitorHook(FSHOOK_PRE_RENAME_OR_MOVE, RenameEntryCallBackEnt, &renamePreCallBackHandle);
	if (ccode)
		return -1;
	/* //EA - 08/16 commented to resolve the NSS vol abend issue
	if (majorServerVersion >= 4)
	{
		ccode = NWAddFSMonitorHook(FSHOOK_PRE_GEN_RENAME, GenericRenameCallBackEnt, &genericRenamePreCallBackHandle);
		if (ccode)
		{
			NWRemoveFSMonitorHook(FSHOOK_PRE_RENAME_OR_MOVE, renamePreCallBackHandle);
			return(-1);

		}

	}
	*/ //EA - 08/16 commented to resolve the NSS vol abend issue
	return 0;

}       /*      END OF  ReMapRenameEntry        */
/*********************/
void UnMapRenameEntry(void)
{

	NWRemoveFSMonitorHook(FSHOOK_PRE_RENAME_OR_MOVE, renamePreCallBackHandle);

	// only Netware 4 and above allows hooking of the generic stuff
	/* //EA - 08/16 commented to resolve the NSS vol abend issue
	if (majorServerVersion >= 4)
	{
		NWRemoveFSMonitorHook(FSHOOK_PRE_GEN_RENAME, genericRenamePreCallBackHandle);
	}
	*/ //EA - 08/16 commented to resolve the NSS vol abend issue

}       /*      END OF  UnMapRenameEntry        */



