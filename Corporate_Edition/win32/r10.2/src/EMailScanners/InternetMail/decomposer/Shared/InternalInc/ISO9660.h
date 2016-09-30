///////////////////////////////////////////////////////////////////////////////
// Copyright 2002 Symantec, Corp.  All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////
//
// Structure definitions for ISO 9660 data format.
//
///////////////////////////////////////////////////////////////////////////////

#define MINIMUM_LOGICAL_SECTOR_SIZE		2048
#define FIRST_SYSTEM_AREA_SECTOR		0
#define FIRST_DATA_AREA_SECTOR			16
#define FIRST_DATA_AREA_OFFSET			(FIRST_DATA_AREA_SECTOR * MINIMUM_LOGICAL_SECTOR_SIZE)

#define RANGE(from, to) (to - from + 1)

// Definitions for VOLUME_DESC.Type
#define VOLUME_TYPE_BOOT			0
#define VOLUME_TYPE_PRIMARY			1
#define VOLUME_TYPE_SUPPLEMENTARY	2
#define VOLUME_TYPE_PARTITION		3
#define VOLUME_TYPE_TERMINATOR		255

// Definitions for VOLUME_DESC.ID
#define VOLUME_STANDARD_ID			"\x43\x44\x30\x30\x31"  // CD001
#define VOLUME_HIGH_SIERRA_ID		"\x43\x44\x52\x4F\x4D"  // CDROM

#define FILE_FLAG_EXISTENCE			0x01
#define FILE_FLAG_DIRECTORY			0x02
#define FILE_FLAG_ASSOCIATED_FILE	0x04
#define FILE_FLAG_RECORD			0x08
#define FILE_FLAG_PROTECTION		0x10
#define FILE_FLAG_RESERVED1			0x20
#define FILE_FLAG_RESERVED2			0x40
#define FILE_FLAG_MULTI_EXTENT		0x80


#if defined(_WINDOWS)
#pragma pack(push, 1)
#elif defined(IBMCXX)
#pragma option align=packed
#else
#pragma pack(1)
#endif

typedef struct tagVolumeDescriptor
{
	BYTE	Type				[RANGE(1, 1)];		// Byte
	BYTE	ID					[RANGE(2, 6)];
	BYTE	Version				[RANGE(7, 7)];		// Byte
	BYTE	Data				[RANGE(8, 2048)];
#if defined(__GNUC__)
} __attribute__ ((packed)) VOLUME_DESC;
#else
} VOLUME_DESC;
#endif


typedef struct tagBootRecord
{
	BYTE	Type				[RANGE(1, 1)];		// Byte
	BYTE	ID					[RANGE(2, 6)];
	BYTE	Version				[RANGE(7, 7)];		// Byte
	BYTE	BootSystemID		[RANGE(8, 39)];		// a-characters
	BYTE	BootID				[RANGE(40, 71)];	// a-characters
	BYTE	BootData			[RANGE(72, 2048)];
#if defined(__GNUC__)
} __attribute__ ((packed)) BOOT_RECORD;
#else
} BOOT_RECORD;
#endif


typedef struct tagPrimaryDescriptor
{
	BYTE	Type				[RANGE(1, 1)];		// Byte
	BYTE	ID					[RANGE(2, 6)];
	BYTE	Version				[RANGE(7, 7)];		// Byte
	BYTE	Unused1				[RANGE(8, 8)];
	BYTE	SystemID			[RANGE(9, 40)];		// a-characters
	BYTE	VolumeID			[RANGE(41, 72)];	// d-characters
	BYTE	Unused2				[RANGE(73, 80)];
	BYTE	SpaceSize			[RANGE(81, 88)];	// DWord
	BYTE	Unused3				[RANGE(89, 120)];
	BYTE	SetSize				[RANGE(121, 124)];	// DWordBE
	BYTE	SequenceNumber		[RANGE(125, 128)];	// DWordBE
	BYTE	LogicalBlockSize	[RANGE(129, 132)];	// DWordBE
	BYTE	PathTableSize		[RANGE(133, 140)];	// DWord
	BYTE	TypeLPathTable		[RANGE(141, 144)];	// DWordLE
	BYTE	OptTypeLPathTable	[RANGE(145, 148)];	// DWordLE
	BYTE	TypeMPathTable		[RANGE(149, 152)];	// DWordBE
	BYTE	OptTypeMPathTable	[RANGE(153, 156)];	// DWordBE
	BYTE	RootDirectoryRecord [RANGE(157, 190)];
	BYTE	VolumeSetID			[RANGE(191, 318)];	// d-characters
	BYTE	PublisherID			[RANGE(319, 446)];	// a-characters
	BYTE	DataPreparerID		[RANGE(447, 574)];	// a-characters
	BYTE	ApplicationID		[RANGE(575, 702)];	// a-characters
	BYTE	CopyrightFileID		[RANGE(703, 739)];	// d-characters
	BYTE	AbstractFileID		[RANGE(740, 776)];	// d-characters
	BYTE	BiblioFileID		[RANGE(777, 813)];	// d-characters
	BYTE	CreationTime		[RANGE(814, 830)];	// date-time
	BYTE	ModificationTime	[RANGE(831, 847)];	// date-time
	BYTE	ExpirationTime		[RANGE(848, 864)];	// date-time
	BYTE	EffectiveTime		[RANGE(865, 881)];	// date-time
	BYTE	FileStructureVersion[RANGE(882, 882)];	// Byte
	BYTE	Unused4				[RANGE(883, 883)];
	BYTE	ApplicationUse		[RANGE(884, 1395)];
	BYTE	Unused5				[RANGE(1396, 2048)];
#if defined(__GNUC__)
} __attribute__ ((packed)) PRIMARY_DESC;
#else
} PRIMARY_DESC;
#endif


typedef struct tagSupplementaryDescriptor
{
	BYTE	Type				[RANGE(1, 1)];		// Byte
	BYTE	ID					[RANGE(2, 6)];
	BYTE	Version				[RANGE(7, 7)];		// Byte
	BYTE	Flags				[RANGE(8, 8)];		// Byte (bit-field)
	BYTE	SystemID			[RANGE(9, 40)];		// a-characters
	BYTE	VolumeID			[RANGE(41, 72)];	// d-characters
	BYTE	Unused2				[RANGE(73, 80)];
	BYTE	SpaceSize			[RANGE(81, 88)];	// DWord
	BYTE	EscapeSequences		[RANGE(89, 120)];
	BYTE	SetSize				[RANGE(121, 124)];	// DWordBE
	BYTE	SequenceNumber		[RANGE(125, 128)];	// DWordBE
	BYTE	LogicalBlockSize	[RANGE(129, 132)];	// DWordBE
	BYTE	PathTableSize		[RANGE(133, 140)];	// DWord
	BYTE	TypeLPathTable		[RANGE(141, 144)];	// DWordLE
	BYTE	OptTypeLPathTable	[RANGE(145, 148)];	// DWordLE
	BYTE	TypeMPathTable		[RANGE(149, 152)];	// DWordBE
	BYTE	OptTypeMPathTable	[RANGE(153, 156)];	// DWordBE
	BYTE	RootDirectoryRecord [RANGE(157, 190)];
	BYTE	VolumeSetID			[RANGE(191, 318)];	// d-characters
	BYTE	PublisherID			[RANGE(319, 446)];	// a-characters
	BYTE	DataPreparerID		[RANGE(447, 574)];	// a-characters
	BYTE	ApplicationID		[RANGE(575, 702)];	// a-characters
	BYTE	CopyrightFileID		[RANGE(703, 739)];	// d-characters
	BYTE	AbstractFileID		[RANGE(740, 776)];	// d-characters
	BYTE	BiblioFileID		[RANGE(777, 813)];	// d-characters
	BYTE	CreationTime		[RANGE(814, 830)];	// date-time
	BYTE	ModificationTime	[RANGE(831, 847)];	// date-time
	BYTE	ExpirationTime		[RANGE(848, 864)];	// date-time
	BYTE	EffectiveTime		[RANGE(865, 881)];	// date-time
	BYTE	FileStructureVersion[RANGE(882, 882)];	// Byte
	BYTE	Unused4				[RANGE(883, 883)];
	BYTE	ApplicationUse		[RANGE(884, 1395)];
	BYTE	Unused5				[RANGE(1396, 2048)];
#if defined(__GNUC__)
} __attribute__ ((packed)) SUPP_DESC;
#else
} SUPP_DESC;
#endif


typedef struct tagPartitionDescriptor
{
	BYTE	Type				[RANGE(1, 1)];		// Byte
	BYTE	ID					[RANGE(2, 6)];
	BYTE	Version				[RANGE(7, 7)];		// Byte
	BYTE	Unused1				[RANGE(8, 8)];
	BYTE	SystemID			[RANGE(9, 40)];		// a-characters
	BYTE	PartitionID			[RANGE(41, 72)];	// d-characters
	BYTE	PartitionLocation	[RANGE(73, 80)];	// DWord
	BYTE	PartitionSize		[RANGE(81, 88)];	// DWord
	BYTE	Unused2				[RANGE(89, 2048)];
#if defined(__GNUC__)
} __attribute__ ((packed)) PARTITION_DESC;
#else
} PARTITION_DESC;
#endif


typedef struct tagDirectoryRecord
{
	BYTE	Length				[RANGE(1, 1)];		// Byte
	BYTE	EALength			[RANGE(2, 2)];		// Byte
	BYTE	ExtentLocation		[RANGE(3, 10)];		// DWord
	BYTE	DataLength			[RANGE(11, 18)];	// DWord
	BYTE	RecordingTime		[RANGE(19, 25)];	// 7 Bytes
	BYTE	FileFlags			[RANGE(26, 26)];	// Byte (bit-field)
	BYTE	FileUnitSize		[RANGE(27, 27)];	// Byte
	BYTE	InterleaveGapSize	[RANGE(28, 28)];	// Byte
	BYTE	SequenceNumber		[RANGE(29, 32)];	// Word
	BYTE	FileIDLength		[RANGE(33, 33)];	// Byte
	BYTE	FileID				[RANGE(34, 256)];
#if defined(__GNUC__)
} __attribute__ ((packed)) DIR_RECORD;
#else
} DIR_RECORD;
#endif


typedef struct tagPathTableRecord
{
	BYTE	DirectoryIDLength	[RANGE(1, 1)];	 // Byte
	BYTE	EALength			[RANGE(2, 2)];	 // Byte
	BYTE	ExtentLocation		[RANGE(3, 6)];	 // DWordLE
	BYTE	ParentDirNumber		[RANGE(7, 8)];	 // WordLE
	BYTE	DirectoryID			[RANGE(9, 9)];
#if defined(__GNUC__)
} __attribute__ ((packed)) PATH_TABLE_RECORD;
#else
} PATH_TABLE_RECORD;
#endif


typedef struct tagExtendedAttributeRecord
{
	BYTE	OwnerID				[RANGE(1, 4)];
	BYTE	GroupID				[RANGE(5, 8)];
	BYTE	Permissions			[RANGE(9, 10)];
	BYTE	FileCreationTime	[RANGE(11, 27)];
	BYTE	FileModificationTime[RANGE(28, 44)];
	BYTE	FileExpirationTime	[RANGE(45, 61)];
	BYTE	FileEffectiveTime	[RANGE(62, 78)];
	BYTE	RecordFormat		[RANGE(79, 79)];
	BYTE	RecordAttributes	[RANGE(80, 80)];
	BYTE	RecordLength		[RANGE(81, 84)];
	BYTE	SystemID			[RANGE(85, 116)];
	BYTE	SystemUse			[RANGE(117, 180)];
	BYTE	EARecordVersion		[RANGE(181, 181)];
	BYTE	ESCSequenceLength	[RANGE(182, 182)];	// LEN_ESC
	BYTE	Unused1				[RANGE(183, 246)];
	BYTE	AppUseLength		[RANGE(247, 250)];	// LEN_AU
	BYTE	AppUse				[RANGE(251, 251)];
	// BYTE EscapeSequences     [RANGE(251 + LEN_AU, 250 + LEN_ESC + LEN_AU)];
#if defined(__GNUC__)
} __attribute__ ((packed)) EA_RECORD;
#else
} EA_RECORD;
#endif

#if defined(_WINDOWS)
#pragma pack(pop)
#elif defined(IBMCXX)
#pragma option align=reset
#else
#pragma pack()
#endif


// Structure for linked-list to manage the directory entries read from the image.
typedef struct tagDirEntry
{
	DIR_RECORD		DirRecord;
	char			szName[MAX_PATH];
	bool			bIsDir;
	tagDirEntry		*pNext;
} DIR_ENTRY;


class CImageFile
{
public:
	CImageFile();
	CImageFile(IDecContainerObjectEx *pObject);
	~CImageFile();

	DECRESULT Open(FILE *fpImageFile);
	DECRESULT FindFirst(void);
	DECRESULT FindNext(void);
	DECRESULT Extract(const char *pszFilename);
	DECRESULT Close(void);
	DECRESULT GetEntryName(char *szName, WORD wNameSize);
	DECRESULT GetEntrySize(DWORDLONG *pdwlSize);
	DECRESULT GetEntryDateTime(DWORD *pdwYear, DWORD *pdwMonth, DWORD *pdwDay,
							  DWORD *pdwHour, DWORD *pdwMinute, DWORD *pdwSecond);
	DECRESULT GetEntryAttributes(DWORD *pdwAttrs);

	IDecContainerObjectEx	*m_pObject;

protected:
	FILE			*m_fpImageFile;
	bool			m_bHighSierra;
	bool			m_bRockRidge;
	bool			m_bGotPrimary;
	bool			m_bGotSupp;
	bool			m_bGotBoot;
	bool			m_bGotPartition;
	size_t			m_nBaseDirOffset;
	size_t			m_nSize;
	size_t			m_nJolietLevel;
	size_t			m_nLogicalSectorSize;
	size_t			m_nLogicalSectorSizeShift;
	size_t			m_nDataStartOffset;
	VOLUME_DESC		m_VolumeDesc;
	PRIMARY_DESC	m_PrimaryDesc;
	SUPP_DESC		m_SuppDesc;
	PARTITION_DESC	m_PartitionDesc;
	BOOT_RECORD		m_BootRecord;
	DIR_ENTRY		*m_pDirectory;
	DIR_ENTRY		*m_pCurrentEntry;

private:
	DECRESULT		LoadDirectory(DIR_RECORD *pdr);
	DECRESULT		ReadDirectoryRecord(DIR_RECORD *pdr, size_t nBaseOffset, size_t nDirSize, size_t nRecordOffset);
	DIR_ENTRY		*AddEntry(DIR_RECORD *pDirRecord, char *pszName, bool bIsDir);
};


typedef struct tagHighSierraVolumeDescriptor
{
	BYTE	Foo					[RANGE(1, 8)];
	BYTE	Type				[RANGE(9, 9)];
	BYTE	ID					[RANGE(10, 14)];
	BYTE	Version				[RANGE(15, 15)];
	BYTE	Data				[RANGE(16, 2048)];
#if defined(__GNUC__)
} __attribute__ ((packed)) HS_VOLUME_DESC;
#else
} HS_VOLUME_DESC;
#endif


typedef struct tagHighSierraPrimaryDescriptor
{
	BYTE	Foo					[RANGE(1, 8)];
	BYTE	Type				[RANGE(9, 9)];
	BYTE	ID					[RANGE(10, 14)];
	BYTE	Version				[RANGE(15, 15)];
	BYTE	Unused1				[RANGE(16, 16)];
	BYTE	SystemID			[RANGE(17, 48)];
	BYTE	VolumeID			[RANGE(49, 80)];
	BYTE	Unused2				[RANGE(81, 88)];
	BYTE	SpaceSize			[RANGE(89, 96)];
	BYTE	Unused3				[RANGE(97, 128)];
	BYTE	SetSize				[RANGE(129, 132)];
	BYTE	SequenceNumber		[RANGE(133, 136)];
	BYTE	LogicalBlockSize	[RANGE(137, 140)];
	BYTE	PathTableSize		[RANGE(141, 148)];
	BYTE	TypeLPathTable		[RANGE(149, 152)];
	BYTE	Unused4				[RANGE(153, 180)];
	BYTE	RootDirectoryRecord [RANGE(181, 214)];
#if defined(__GNUC__)
} __attribute__ ((packed)) HS_PRIMARY_DESC;
#else
} HS_PRIMARY_DESC;
#endif


// We use this to help us look up the parent inode numbers.
typedef struct tagISOPathTable
{
	unsigned char	name_len[2];	// 721
	char			extent[4];		// 731
	char			parent[2];		// 721
	char			name[1];		// *** DEBUG *** WAS: char name[0]
} ISOPATHTABLE;


// High sierra is identical to ISO except that the date is only 6 bytes and
// there is an extra reserved byte after the flags.
typedef struct tagISODirectoryRecord
{
	BYTE	Length				[RANGE(1, 1)];
	BYTE	ExtAttrLength		[RANGE(2, 2)];
	BYTE	Extent				[RANGE(3, 10)];
	BYTE	Size				[RANGE(11, 18)];
	BYTE	Date				[RANGE(19, 25)];
	BYTE	Flags				[RANGE(26, 26)];
	BYTE	FileUnitSize		[RANGE(27, 27)];
	BYTE	Interleave			[RANGE(28, 28)];
	BYTE	SequenceNumber		[RANGE(29, 32)];
	BYTE	NameLength			[RANGE(33, 33)];
	BYTE	Name				[RANGE(34, 34)];
#if defined(__GNUC__)
} __attribute__ ((packed)) ISO_DIR_RECORD;
#else
} ISO_DIR_RECORD;
#endif

//#define ISOFS_BLOCK_BITS 11
//#define ISOFS_BLOCK_SIZE 2048
//#define ISOFS_BUFFER_SIZE(INODE) ((INODE)->i_sb->s_blocksize)
//#define ISOFS_BUFFER_BITS(INODE) ((INODE)->i_sb->s_blocksize_bits)
//#define ISOFS_ZONE_BITS(INODE)   ((INODE)->i_sb->u.isofs_sb.s_log_zone_size)
//#define ISOFS_SUPER_MAGIC 0x9660


// Decomposer-specific control functions.
void ISO9660SetMaxExtractSize(DWORD dwMaxSize);
void ISO9660AbortProcess(bool bAbort);
