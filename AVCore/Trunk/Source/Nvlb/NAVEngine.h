// NAVEngine.h: interface for the CNAVEngine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NAVENGINE_H__3FFB7AC1_0D1B_11D3_8B45_00104B9DFF82__INCLUDED_)
#define AFX_NAVENGINE_H__3FFB7AC1_0D1B_11D3_8B45_00104B9DFF82__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "navapi.h"

///////////////////////////////////////////////////////////
// Info flags
///////////////////////////////////////////////////////////
#define	FILES			0x00000001
#define	EXE				0x00000002
#define	COM				0x00000004
#define	COMMAND			0x00000008
#define	SYS				0x00000010
#define	MBR				0x00000020
#define	FLOPPY_BOOT     0x00000040
#define	HD_BOOT			0x00000080
#define	WILD			0x00000100
#define	MEMORY_RES		0x00000200
#define	SIZE_STEALTH	0x00000400
#define	FULL_STEALTH	0x00000800
#define	TRIGGER			0x00001000
#define	ENCRYPTING		0x00002000
#define	POLYMORPHIC		0x00004000
#define	MULTIPART		0x00008000


///////////////////////////////////////////////////////////
// Control Flags
///////////////////////////////////////////////////////////
#define	MACRO_VIRUS		0x00000001
#define	WINDOWS_VIRUS	0x00000002
#define	AGENT_VIRUS		0x00000004
#define	SUBMITABLE		0x00000008
#define	HEURISTIC		0x00000010
#define	MAC_VIRUS		0x00000020



///////////////////////////////////////////////////////////
// Query Flags
///////////////////////////////////////////////////////////
#define	REPAIRED		0x00000001
#define REPAIR_FAILED	0x00000002
#define REPAIRABLE		0x00000004
#define DELETED			0x00000008
#define DELETABLE		0x00000010
#define DELETE_FAILED	0x00000020
#define MEMORY_DETECT	0x00000040
#define FILE_ITEM		0x00000080
#define BOOT_ITEM		0x00000100
#define PART_ITEM		0x00000200
#define NO_ACCESS		0x00000400
#define DISEMBODIED		0x00000800
#define DEF_DELETED		0x00001000
#define INFO_VALID		0x00002000


const DWORD	dwaInfoItems[][2] = 
{
	FILES,			NAV_VI_BOOL_INFECT_FILES,
	EXE,			NAV_VI_BOOL_INFECT_EXE,	
	COM,			NAV_VI_BOOL_INFECT_COM,		
	COMMAND,		NAV_VI_BOOL_INFECT_COMCOM,
	SYS,			NAV_VI_BOOL_INFECT_SYS,		
	MBR,			NAV_VI_BOOL_INFECT_MBR,		
	FLOPPY_BOOT,	NAV_VI_BOOL_INFECT_FLOPPY_BOOT,
	HD_BOOT,		NAV_VI_BOOL_INFECT_HD_BOOT,		
	WILD,			NAV_VI_BOOL_INFECT_WILD,		
	MEMORY_RES,		NAV_VI_BOOL_MEMORY_RES,	
	SIZE_STEALTH,	NAV_VI_BOOL_SIZE_STEALTH,	
	FULL_STEALTH,	NAV_VI_BOOL_FULL_STEALTH,
	TRIGGER,		NAV_VI_BOOL_TRIGGER,		
	ENCRYPTING,		NAV_VI_BOOL_ENCRYPTING,	
	POLYMORPHIC,	NAV_VI_BOOL_POLYMORPHIC,	
	MULTIPART,		NAV_VI_BOOL_MULTIPART	
};


const DWORD	dwaControlItems[][2] = 
{
	MACRO_VIRUS,	NAV_VI_BOOL_MACRO_VIRUS,
	WINDOWS_VIRUS,	NAV_VI_BOOL_WINDOWS_VIRUS,
	AGENT_VIRUS,	NAV_VI_BOOL_AGENT_VIRUS,
	SUBMITABLE,		NAV_VI_BOOL_SUBMITABLE,
	HEURISTIC,		NAV_VI_BOOL_HEURISTIC,
	MAC_VIRUS,		NAV_VI_BOOL_MAC_VIRUS
};


const DWORD	dwaQueryItems[][2] = 
{ 
	REPAIRED,		NAV_VI_BOOL_REPAIRED,
	REPAIR_FAILED,	NAV_VI_BOOL_REPAIR_FAILED,
	REPAIRABLE,		NAV_VI_BOOL_REPAIRABLE,
	DELETED,		NAV_VI_BOOL_DELETED,
	DELETABLE,		NAV_VI_BOOL_DELETABLE,
	DELETE_FAILED,	NAV_VI_BOOL_DELETE_FAILED,
	MEMORY_DETECT,	NAV_VI_BOOL_MEMORY_DETECT,
	FILE_ITEM,		NAV_VI_BOOL_FILE_ITEM,
	BOOT_ITEM,		NAV_VI_BOOL_BOOT_ITEM,
	PART_ITEM,		NAV_VI_BOOL_PART_ITEM,
	NO_ACCESS,		NAV_VI_BOOL_NO_ACCESS,
	DISEMBODIED,	NAV_VI_BOOL_DISEMBODIED,
	DEF_DELETED,	NAV_VI_BOOL_DEF_DELETED,
	INFO_VALID,		NAV_VI_BOOL_INFO_VALID 
};




///////////////////////////////////////////////////////////
//
// Class Name:
//
// Description:
//
///////////////////////////////////////////////////////////
class CNAVEngine  
{

	///////////////////////////////////////////////////////
	// Public Items
	///////////////////////////////////////////////////////
	public:
		CNAVEngine();
		virtual ~CNAVEngine();

		// Engine settings
		VOID	SetDefLocation( LPCSTR lpszDefLocation );
		VOID	SetINIFile( LPCSTR lpszINIFile );
		VOID	SetHeuristicsLevel( INT iHeurLevel );
		
		// Engine Intitialization
		DWORD	InitNAVEngine();
		DWORD	UnloadNAVEngine();
		
		// Scan functions
		DWORD	ScanMemory();
		DWORD	ScanFile( LPCSTR lpszFileName );
		DWORD	ScanBoot( CHAR cDriveLetter );
		DWORD	ScanMasterBoot( UINT uPhysDriveNum );

		// Repair functions
		DWORD	RepairFile( LPCSTR lpszFileName );
		DWORD	RepairBoot( CHAR cDriveLetter );
		DWORD	RepairMasterBoot( UINT uPhysDriveNum );

		// Virus information functions
		LPSTR	GetVirusName();
		LPSTR	GetVirusInfo();
		LPSTR	GetVirusAlias();
		WORD	GetVirusID();

		DWORD	GetPhysicalDriveNum();
		DWORD	GetPartitionNum();
		DWORD	GetPartitionType();

		DWORD	GetControlFlags();
		DWORD	GetInfoFlags();
		DWORD	GetQueryFlags();
		DWORD	GetVirusSize();


	///////////////////////////////////////////////////////
	// Private Items
	///////////////////////////////////////////////////////
	private:

		DWORD		SetVirusInfo();
		VOID		ClearVirusInfo();
		
		HNAVENGINE	m_hNAVEngine;
			
		CHAR		m_szDefLocation[_MAX_PATH];
		CHAR		m_szINIFile[_MAX_PATH];
		INT			m_iHeurLevel;
		
		HNAVVIRUS	m_hVirus;
		CHAR		m_szVirusName[25];
		CHAR		m_szVirusAlias[256];
		CHAR		m_szVirusInfo[256];
		WORD		m_wVirusID;
		DWORD		m_dwVirusSize;

		DWORD		m_dwInfoFlags;
		DWORD		m_dwControlFlags;
		DWORD		m_dwQueryFlags; 

		DWORD		m_dwPhysicalDriveNum;
		DWORD		m_dwPartitionNum;
		DWORD		m_dwPartitionType;

		BOOL		m_bIsInfected;


	///////////////////////////////////////////////////////
	// Protected Items
	///////////////////////////////////////////////////////
	protected:

};

#endif // !defined(AFX_NAVENGINE_H__3FFB7AC1_0D1B_11D3_8B45_00104B9DFF82__INCLUDED_)
