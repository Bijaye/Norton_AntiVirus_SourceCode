// typer.h : Data-type identification functions
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 by Symantec Corporation.  All rights reserved.

typedef struct tagTypeInfo
{
	DWORD		dwType;			// Primary data type (DATA_TYPE_xxx)
	DWORD		dwSubType;		// Data sub-type (DATA_SUBTYPE_xxx)
	DWORD		dwInfo[6];		// Additional type-specific information.
} TYPEINFO;


// Typer APIs
class COldTypeID
{
public:
	size_t	DetectDataType(unsigned char *pBuffer, size_t nBufferSize, size_t nFileSize, TYPEINFO *pTypeInfo, size_t nTypes);
	bool	DetectDataTypeByExt(char *pszExt, unsigned char *pBuffer, size_t nBufferSize, size_t nFileSize, TYPEINFO *pTypeInfo, size_t nTypes);

private:
	int		TestDataType(DWORD dwType);

	unsigned char	*m_pBuffer;
	size_t			m_nBufferSize;
	size_t			m_nFileSize;
	TYPEINFO		*m_pTypeInfo;
	size_t			m_nTypes;
};


// Data type IDs
// *** NOTE: Very important!!!  Do NOT re-order these values.  Only add new ones to the end
//           of the list.
enum DATA_TYPE_IDS
{
	DATA_TYPE_UNKNOWN = 0,
	DATA_TYPE_ACE,			// ACE                                      (.ace)
	DATA_TYPE_AMG,			// ArcManager                               (.amg)
	DATA_TYPE_ANI,			// Microsoft Windows Animated cursor        (.ani)
	DATA_TYPE_ARC,			// PKARC                                    (.arc)
	DATA_TYPE_ARJ,			// ARJ                                      (.arj)
	DATA_TYPE_AVI,			// Microsoft Audio Video Interleaved movie  (.avi)
	DATA_TYPE_BAG,			// BAG                                      (.bag)
	DATA_TYPE_BMP,			// Windows bitmap                           (.bmp)
	DATA_TYPE_CAB,			// Microsoft Cabinet                        (.cab)
	DATA_TYPE_EXE,			// DOS/Windows executable                   (.exe, .dll)
	DATA_TYPE_GHO,			// Symantec Ghost                           (.gho)
	DATA_TYPE_GIF,			// Graphics Interchange Format              (.gif)
	DATA_TYPE_GZIP,			// GZIP                                     (.gz, .gzip)
	DATA_TYPE_HQX,			// BinHex                                   (.hqx)
	DATA_TYPE_JPEG,			// JPEG                                     (.jpeg)
	DATA_TYPE_LHA,			// LHA/LZH                                  (.lha, .lzh)
	DATA_TYPE_LZ,			// Microsoft LZ                             (.lz)
	DATA_TYPE_MPEG,			// MPEG                                     (.mpeg, .mp2, .mp3)
	DATA_TYPE_OLESS,		// Microsoft OLE Structured Storage         (.doc, .xls, .ppt, .shs)
	DATA_TYPE_PDF,			// Portable Document Format                 (.pdf)
	DATA_TYPE_RAR,			// RAR                                      (.rar)
	DATA_TYPE_RTF,			// Rich Text Format                         (.rtf)
	DATA_TYPE_TAR,			// Tape Archive                             (.tar)
	DATA_TYPE_TGA,			// Targa                                    (.tga)
	DATA_TYPE_TIFF,			// TIFF                                     (.tif, .tiff)
	DATA_TYPE_TNEF,			// Transport Neutral Encapsulation Format   (.tnef, .dat)
	DATA_TYPE_UUE,			// UUE                                      (.uue)
	DATA_TYPE_WAV,			// Redbook audio                            (.wav)
	DATA_TYPE_ZIP,			// ZIP                                      (.zip)
	DATA_TYPE_ZOO,			// ZOO                                      (.zoo)
	DATA_TYPE_TEXT,			// ASCII text                               (.txt)
	DATA_TYPE_OLE1,			// OLE 1.0 package object                   (.pkg)
	DATA_TYPE_HTTP,			// HyperText Transfer Protocol              (.http)
	DATA_TYPE_HTML,			// HyperText Markup Language                (.htm, .html)
	DATA_TYPE_669,			// 669 audio                                (.669)
	DATA_TYPE_AIFF,			// Audio Interchange File Format            (.aif, .aiff)
	DATA_TYPE_AMD,			// AMUSIC Tracker                           (.amd)
	DATA_TYPE_AMF,			// Advanced Module Format                   (.amf)
	DATA_TYPE_AMM,			// Audio Manager Module                     (.amm)
	DATA_TYPE_AMS,			// Extreme Tracker / Velvet Studio          (.ams)
	DATA_TYPE_AU,			// Unix audio                               (.au)
	DATA_TYPE_BPM,			// B's Pro Tracker                          (.bpm)
	DATA_TYPE_FAR,			// Farandole Composer Format                (.far)
	DATA_TYPE_GDM,			// Bells, whistles, and sound boards module (.gdm)
	DATA_TYPE_IT,			// Impulse Tracker                          (.it)
	DATA_TYPE_MIDI,			// Standard MIDI                            (.mid, .midi)
	DATA_TYPE_MOD,			// MOD audio                                (.mod)
	DATA_TYPE_MTM,			// Master Tracker Module                    (.mtm)
	DATA_TYPE_MED,			// Med/OctaMed MMD0/MMD1                    (.med)
	DATA_TYPE_OGG,			// Ogg Vorbis open-source audio format      (.ogg)
	DATA_TYPE_PNG,			// Portable Network Graphics                (.png)
	DATA_TYPE_QT,			// QuickTime Movie                          (.qt, .mov, .movie)
	DATA_TYPE_RMI,			// RIFF MIDI                                (.rmi)
	DATA_TYPE_SHN,			// Shorten audio compression format         (.shn)
	DATA_TYPE_STM,			// Scream Tracker 1.0                       (.stm)
	DATA_TYPE_STX,			// Scream Tracker Music Interface Kit       (.stx)
	DATA_TYPE_S3M,			// Scream Tracker 3.01                      (.s3m)
	DATA_TYPE_SWF,			// ShockWave Flash                          (.swf)
	DATA_TYPE_ULT,			// Mysterious Ultra Tracker                 (.ult)
	DATA_TYPE_UNI,			// Uni Tracker                              (.uni)
	DATA_TYPE_UNIC,			// Unic Tracker v1/v2                       (.uni)
	DATA_TYPE_VIVO,			// VivoActive video                         (.viv, .vivo)
	DATA_TYPE_VQF,			// VQF audio                                (.vqf)
	DATA_TYPE_WMA,			// Windows Media Audio                      (.wma)
	DATA_TYPE_XM,			// FastTracker II                           (.xm)
	LAST_DATA_TYPE			// NOTE: Always keep this as the last value.
};


// Three possible identification results
enum ID_RESULTS
{
	ID_NONE = 0,
	ID_PARTIAL,
	ID_COMPLETE
};


// Command bytes for the state machine
enum STATE_COMMANDS
{
	STATE_CMD_NOP = 0,
	STATE_CMD_256_BITS,
	STATE_CMD_SINGLETON,
	STATE_CMD_ANY_VALUE
};


// Definitions for format of data in knowndatatypes.h.
typedef struct tagStateHdr
{
	DWORD	  dwTypeCount;
	DWORD	  dwFirstCommandIndex;
} STATEHDR, *PSTATEHDR;

typedef struct tagStateChainHdr
{
	DWORD	  dwDataType;
	DWORD	  dwDepth;
} STATECHAINHDR, *PSTATECHAINHDR;

typedef struct tagStateChain
{
	DWORD			dwIDLevel;
	DWORD			dwState;
} STATECHAIN, *PSTATECHAIN;


typedef struct tagTypeNode
{
	DWORD			dwType;
	DWORD			dwOffset;
	int				iIDLevel;
	int				iState;
	int				iUniqueState;
	int				iCommand;
	unsigned char	uchBits[32];
	tagTypeNode		*pUnique;
	tagTypeNode		*pNext;
} TYPENODE, *PTYPENODE;



int	IsACE(unsigned char *pBuffer, size_t ulBufferSize, size_t ulFileSize);
int	IsByteACE(unsigned char uch, size_t ulOffset);

int	IsANI(unsigned char *pBuffer, size_t ulBufferSize, size_t ulFileSize);
int	IsByteANI(unsigned char uch, size_t ulOffset);

int	IsARC(unsigned char *pBuffer, size_t ulBufferSize, size_t ulFileSize);
int	IsByteARC(unsigned char uch, size_t ulOffset);

int	IsArcManager(unsigned char *pBuffer, size_t ulBufferSize, size_t ulFileSize);
int	IsByteArcManager(unsigned char uch, size_t ulOffset);

int	IsARJ(unsigned char *pBuffer, size_t ulBufferSize, size_t ulFileSize);
int	IsByteARJ(unsigned char uch, size_t ulOffset);

int	IsAVI(unsigned char *pBuffer, size_t ulBufferSize, size_t ulFileSize);
int	IsByteAVI(unsigned char uch, size_t ulOffset);

int	IsBAG(unsigned char *pBuffer, size_t ulBufferSize, size_t ulFileSize);
int	IsByteBAG(unsigned char uch, size_t ulOffset);

int	IsBitmap(unsigned char *pBuffer, size_t ulBufferSize, size_t ulFileSize);
int	IsByteBitmap(unsigned char uch, size_t ulOffset);

int	IsEXE(unsigned char *pBuffer, size_t ulBufferSize, size_t ulFileSize);
int	IsByteEXE(unsigned char uch, size_t ulOffset);

int	IsGHO(unsigned char *pBuffer, size_t ulBufferSize, size_t ulFileSize);
int	IsByteGHO(unsigned char uch, size_t ulOffset);

int	IsGIF(unsigned char *pBuffer, size_t ulBufferSize, size_t ulFileSize);
int	IsByteGIF(unsigned char uch, size_t ulOffset);

int	IsGZIP(unsigned char *pBuffer, size_t ulBufferSize, size_t ulFileSize);
int	IsByteGZIP(unsigned char uch, size_t ulOffset);

int	IsHQX(unsigned char *pBuffer, size_t ulBufferSize, size_t ulFileSize);
int	IsByteHQX(unsigned char uch, size_t ulOffset);

int	IsHTML(unsigned char *pBuffer, size_t ulBufferSize, size_t ulFileSize);
int	IsByteHTML(unsigned char uch, size_t ulOffset);

int	IsHTTP(unsigned char *pBuffer, size_t ulBufferSize, size_t ulFileSize);
int	IsByteHTTP(unsigned char uch, size_t ulOffset);

int	IsJPEG(unsigned char *pBuffer, size_t ulBufferSize, size_t ulFileSize);
int	IsByteJPEG(unsigned char uch, size_t ulOffset);

int	IsLHA(unsigned char *pBuffer, size_t ulBufferSize, size_t ulFileSize);
int	IsByteLHA(unsigned char uch, size_t ulOffset);

int	IsLZ(unsigned char *pBuffer, size_t ulBufferSize, size_t ulFileSize);
int	IsByteLZ(unsigned char uch, size_t ulOffset);

int	IsCAB(unsigned char *pBuffer, size_t ulBufferSize, size_t ulFileSize);
int	IsByteCAB(unsigned char uch, size_t ulOffset);

int	IsOLE1(unsigned char *pBuffer, size_t ulBufferSize, size_t ulFileSize);
int	IsByteOLE1(unsigned char uch, size_t ulOffset);

int	IsPDF(unsigned char *pBuffer, size_t ulBufferSize, size_t ulFileSize);
int	IsBytePDF(unsigned char uch, size_t ulOffset);

int	IsRAR(unsigned char *pBuffer, size_t ulBufferSize, size_t ulFileSize);
int	IsByteRAR(unsigned char uch, size_t ulOffset);

int	IsRTF(unsigned char *pBuffer, size_t ulBufferSize, size_t ulFileSize);
int	IsByteRTF(unsigned char uch, size_t ulOffset);

int	IsSqueeze(unsigned char *pBuffer, size_t ulBufferSize, size_t ulFileSize);
int	IsByteSqueeze(unsigned char uch, size_t ulOffset);

int	IsOLESS(unsigned char *pBuffer, size_t ulBufferSize, size_t ulFileSize);
int	IsByteOLESS(unsigned char uch, size_t ulOffset);

int	IsTAR(unsigned char *pBuffer, size_t ulBufferSize, size_t ulFileSize);
int	IsByteTAR(unsigned char uch, size_t ulOffset);

int	IsTarga(unsigned char *pBuffer, size_t ulBufferSize, size_t ulFileSize);
int	IsByteTarga(unsigned char uch, size_t ulOffset);

int	IsText(unsigned char *pBuffer, size_t ulBufferSize, size_t ulFileSize);
int	IsByteText(unsigned char uch, size_t ulOffset);

int	IsTIFF(unsigned char *pBuffer, size_t ulBufferSize, size_t ulFileSize);
int	IsByteTIFF(unsigned char uch, size_t ulOffset);

int	IsTNEF(unsigned char *pBuffer, size_t ulBufferSize, size_t ulFileSize);
int	IsByteTNEF(unsigned char uch, size_t ulOffset);

int	IsUUE(unsigned char *pBuffer, size_t ulBufferSize, size_t ulFileSize);
int	IsByteUUE(unsigned char uch, size_t ulOffset);

int	IsWAV(unsigned char *pBuffer, size_t ulBufferSize, size_t ulFileSize);
int	IsByteWAV(unsigned char uch, size_t ulOffset);

int	IsZIP(unsigned char *pBuffer, size_t ulBufferSize, size_t ulFileSize);
int	IsByteZIP(unsigned char uch, size_t ulOffset);

int	IsZOO(unsigned char *pBuffer, size_t ulBufferSize, size_t ulFileSize);
int	IsByteZOO(unsigned char uch, size_t ulOffset);

int	Is669(unsigned char *pBuffer, size_t ulBufferSize, size_t ulFileSize);
int	IsByte669(unsigned char uch, size_t ulOffset);

int	IsAIFF(unsigned char *pBuffer, size_t ulBufferSize, size_t ulFileSize);
int	IsByteAIFF(unsigned char uch, size_t ulOffset);

int	IsAMD(unsigned char *pBuffer, size_t ulBufferSize, size_t ulFileSize);
int	IsByteAMD(unsigned char uch, size_t ulOffset);

int	IsAMF(unsigned char *pBuffer, size_t ulBufferSize, size_t ulFileSize);
int	IsByteAMF(unsigned char uch, size_t ulOffset);

int	IsAMM(unsigned char *pBuffer, size_t ulBufferSize, size_t ulFileSize);
int	IsByteAMM(unsigned char uch, size_t ulOffset);

int	IsAMS(unsigned char *pBuffer, size_t ulBufferSize, size_t ulFileSize);
int	IsByteAMS(unsigned char uch, size_t ulOffset);

int	IsAU(unsigned char *pBuffer, size_t ulBufferSize, size_t ulFileSize);
int	IsByteAU(unsigned char uch, size_t ulOffset);

int	IsBPM(unsigned char *pBuffer, size_t ulBufferSize, size_t ulFileSize);
int	IsByteBPM(unsigned char uch, size_t ulOffset);

int	IsFAR(unsigned char *pBuffer, size_t ulBufferSize, size_t ulFileSize);
int	IsByteFAR(unsigned char uch, size_t ulOffset);

int	IsGDM(unsigned char *pBuffer, size_t ulBufferSize, size_t ulFileSize);
int	IsByteGDM(unsigned char uch, size_t ulOffset);

int	IsIT(unsigned char *pBuffer, size_t ulBufferSize, size_t ulFileSize);
int	IsByteIT(unsigned char uch, size_t ulOffset);

int	IsMED(unsigned char *pBuffer, size_t ulBufferSize, size_t ulFileSize);
int	IsByteMED(unsigned char uch, size_t ulOffset);

int	IsMIDI(unsigned char *pBuffer, size_t ulBufferSize, size_t ulFileSize);
int	IsByteMIDI(unsigned char uch, size_t ulOffset);

int	IsMOD(unsigned char *pBuffer, size_t ulBufferSize, size_t ulFileSize);
int	IsByteMOD(unsigned char uch, size_t ulOffset);

int	IsMTM(unsigned char *pBuffer, size_t ulBufferSize, size_t ulFileSize);
int	IsByteMTM(unsigned char uch, size_t ulOffset);

int	IsPNG(unsigned char *pBuffer, size_t ulBufferSize, size_t ulFileSize);
int	IsBytePNG(unsigned char uch, size_t ulOffset);

int	IsQT(unsigned char *pBuffer, size_t ulBufferSize, size_t ulFileSize);
int	IsByteQT(unsigned char uch, size_t ulOffset);

int	IsRMI(unsigned char *pBuffer, size_t ulBufferSize, size_t ulFileSize);
int	IsByteRMI(unsigned char uch, size_t ulOffset);

int	IsS3M(unsigned char *pBuffer, size_t ulBufferSize, size_t ulFileSize);
int	IsByteS3M(unsigned char uch, size_t ulOffset);

int	IsSTM(unsigned char *pBuffer, size_t ulBufferSize, size_t ulFileSize);
int	IsByteSTM(unsigned char uch, size_t ulOffset);

int	IsSTX(unsigned char *pBuffer, size_t ulBufferSize, size_t ulFileSize);
int	IsByteSTX(unsigned char uch, size_t ulOffset);

int	IsSWF(unsigned char *pBuffer, size_t ulBufferSize, size_t ulFileSize);
int	IsByteSWF(unsigned char uch, size_t ulOffset);

int	IsULT(unsigned char *pBuffer, size_t ulBufferSize, size_t ulFileSize);
int	IsByteULT(unsigned char uch, size_t ulOffset);

int	IsUNI(unsigned char *pBuffer, size_t ulBufferSize, size_t ulFileSize);
int	IsByteUNI(unsigned char uch, size_t ulOffset);

int	IsUNIC(unsigned char *pBuffer, size_t ulBufferSize, size_t ulFileSize);
int	IsByteUNIC(unsigned char uch, size_t ulOffset);

int	IsVIVO(unsigned char *pBuffer, size_t ulBufferSize, size_t ulFileSize);
int	IsByteVIVO(unsigned char uch, size_t ulOffset);

int	IsVQF(unsigned char *pBuffer, size_t ulBufferSize, size_t ulFileSize);
int	IsByteVQF(unsigned char uch, size_t ulOffset);

int	IsWMA(unsigned char *pBuffer, size_t ulBufferSize, size_t ulFileSize);
int	IsByteWMA(unsigned char uch, size_t ulOffset);

int	IsXM(unsigned char *pBuffer, size_t ulBufferSize, size_t ulFileSize);
int	IsByteXM(unsigned char uch, size_t ulOffset);
