// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/ppapi.h_v   1.2   03 Feb 1998 14:21:56   DCHI  $
//
// Description:
//  Core PowerPoint 97 access implementation header.
//
//************************************************************************
// $Log:   S:/INCLUDE/VCS/ppapi.h_v  $
// 
//    Rev 1.3   19 Jan 2001 14:21:56   ELAI
// Added support for non compressed embedded OLE streams.
//
//    Rev 1.2   03 Feb 1998 14:21:56   DCHI
// Added embedded object support prototypes.
// 
//    Rev 1.1   26 Nov 1997 15:04:56   DCHI
// Added PPGetVBAInfoAtomOffset() prototype
// and added dwUserEditOffset parameter to PPGetOLEStgOffset().
// 
//    Rev 1.0   24 Nov 1997 17:36:52   DCHI
// Initial revision.
//
//************************************************************************

#ifndef _PPAPI_H_

#define _PPAPI_H_

#include "SymOLESS.h"

#ifdef PP_ENUM

typedef enum tagEPPREC
{
	ePPREC_Unknown						= 0,
	ePPREC_SubContainerCompleted		= 1,
	ePPREC_IRRAtom						= 2,
	ePPREC_PSS							= 3,
	ePPREC_SubContainerException		= 4,
	ePPREC_ClientSignal1				= 6,
	ePPREC_ClientSignal2				= 7,
	ePPREC_PowerPointStateInfoAtom		= 10,
	ePPREC_Document						= 1000,
	ePPREC_DocumentAtom					= 1001,
	ePPREC_EndDocument					= 1002,
	ePPREC_SlidePersist					= 1003,
	ePPREC_SlideBase					= 1004,
	ePPREC_SlideBaseAtom				= 1005,
	ePPREC_Slide						= 1006,
	ePPREC_SlideAtom					= 1007,
	ePPREC_Notes						= 1008,
	ePPREC_NotesAtom					= 1009,
	ePPREC_Environment					= 1010,
	ePPREC_SlidePersistAtom				= 1011,
	ePPREC_Scheme						= 1012,
	ePPREC_SchemeAtom					= 1013,
	ePPREC_DocViewInfo					= 1014,
	ePPREC_SslideLayoutAtom				= 1015,
	ePPREC_MainMaster					= 1016,
	ePPREC_SSSlideInfoAtom				= 1017,
	ePPREC_SlideViewInfo				= 1018,
	ePPREC_GuideAtom					= 1019,
	ePPREC_ViewInfo						= 1020,
	ePPREC_ViewInfoAtom					= 1021,
	ePPREC_SlideViewInfoAtom			= 1022,
	ePPREC_VBAInfo						= 1023,
	ePPREC_VBAInfoAtom					= 1024,
	ePPREC_SSDocInfoAtom				= 1025,
	ePPREC_Summary						= 1026,
	ePPREC_Texture						= 1027,
	ePPREC_VBASlideInfo					= 1028,
	ePPREC_VBASlideInfoAtom				= 1029,
	ePPREC_DocRoutingSlip				= 1030,
	ePPREC_OutlineViewInfo				= 1031,
	ePPREC_SorterViewInfo				= 1032,
	ePPREC_ExObjList					= 1033,
	ePPREC_ExObjListAtom				= 1034,
	ePPREC_PPDrawingGroup				= 1035,
	ePPREC_PPDrawing					= 1036,
	ePPREC_NamedShows					= 1040,
	ePPREC_NamedShow					= 1041,
	ePPREC_NamedShowSlides				= 1042,
	ePPREC_List							= 2000,
	ePPREC_FontCollection				= 2005,
	ePPREC_ListPlaceholder				= 2017,
	ePPREC_BookmarkCollection			= 2019,
	ePPREC_SoundCollection				= 2020,
	ePPREC_SoundCollAtom				= 2021,
	ePPREC_Sound						= 2022,
	ePPREC_SoundData					= 2023,
	ePPREC_BookmarkSeedAtom				= 2025,
	ePPREC_GuideList					= 2026,
	ePPREC_RunArray						= 2028,
	ePPREC_RunArrayAtom					= 2029,
	ePPREC_ArrayElementAtom				= 2030,
	ePPREC_Int4ArrayAtom				= 2031,
	ePPREC_ColorSchemeAtom				= 2032,
	ePPREC_OEShape						= 3008,
	ePPREC_ExObjRefAtom					= 3009,
	ePPREC_OEPlaceholderAtom			= 3011,
	ePPREC_GrColor						= 3020,
	ePPREC_GrectAtom					= 3025,
	ePPREC_GratioAtom					= 3031,
	ePPREC_Gscaling						= 3032,
	ePPREC_GpointAtom					= 3034,
	ePPREC_OEShapeAtom					= 3035,
	ePPREC_OutlineTextRefAtom			= 3998,
	ePPREC_TextHeaderAtom				= 3999,
	ePPREC_TextCharsAtom				= 4000,
	ePPREC_StyleTextPropAtom			= 4001,
	ePPREC_BaseTextPropAtom				= 4002,
	ePPREC_TxMasterStyleAtom			= 4003,
	ePPREC_TxCFStyleAtom				= 4004,
	ePPREC_TxPFStyleAtom				= 4005,
	ePPREC_TextRulerAtom				= 4006,
	ePPREC_TextBookmarkAtom				= 4007,
	ePPREC_TextBytesAtom				= 4008,
	ePPREC_TxSIStyleAtom				= 4009,
	ePPREC_TextSpecInfoAtom				= 4010,
	ePPREC_DefaultRulerAtom				= 4011,
	ePPREC_FontEntityAtom				= 4023,
	ePPREC_FontEmbedData				= 4024,
	ePPREC_TypeFace						= 4025,
	ePPREC_CString						= 4026,
	ePPREC_ExternalObject				= 4027,
	ePPREC_MetaFile						= 4033,
	ePPREC_ExOleObj						= 4034,
	ePPREC_ExOleObjAtom					= 4035,
	ePPREC_ExPlainLinkAtom				= 4036,
	ePPREC_CorePict						= 4037,
	ePPREC_CorePictAtom					= 4038,
	ePPREC_ExPlainAtom					= 4039,
	ePPREC_SrKinsoku					= 4040,
	ePPREC_Handout						= 4041,
	ePPREC_ExEmbed						= 4044,
	ePPREC_ExEmbedAtom					= 4045,
	ePPREC_ExLink						= 4046,
	ePPREC_ExLinkAtom_old				= 4047,
	ePPREC_BookmarkEntityAtom			= 4048,
	ePPREC_ExLinkAtom					= 4049,
	ePPREC_SrKinsokuAtom				= 4050,
	ePPREC_ExHyperlinkAtom				= 4051,
	ePPREC_ExPlain						= 4053,
	ePPREC_ExPlainLink					= 4054,
	ePPREC_ExHyperlink					= 4055,
	ePPREC_SlideNumberMCAtom			= 4056,
	ePPREC_HeadersFooters				= 4057,
	ePPREC_HeadersFootersAtom			= 4058,
	ePPREC_RecolorEntryAtom				= 4062,
	ePPREC_TxInteractiveInfoAtom		= 4063,
	ePPREC_EmFormatAtom					= 4065,
	ePPREC_CharFormatAtom				= 4066,
	ePPREC_ParaFormatAtom				= 4067,
	ePPREC_MasterText					= 4068,
	ePPREC_RecolorInfoAtom				= 4071,
	ePPREC_ExQuickTime					= 4073,
	ePPREC_ExQuickTimeMovie				= 4074,
	ePPREC_ExQuickTimeMovieData			= 4075,
	ePPREC_ExSubscription				= 4076,
	ePPREC_ExSubscriptionSection		= 4077,
	ePPREC_ExControl					= 4078,
	ePPREC_ExControlAtom				= 4091,
	ePPREC_SlideListWithText			= 4080,
	ePPREC_AnimationInfoAtom			= 4081,
	ePPREC_InteractiveInfo				= 4082,
	ePPREC_InteractiveInfoAtom			= 4083,
	ePPREC_SlideList					= 4084,
	ePPREC_UserEditAtom					= 4085,
	ePPREC_CurrentUserAtom				= 4086,
	ePPREC_DateTimeMCAtom				= 4087,
	ePPREC_GenericDateMCAtom			= 4088,
	ePPREC_HeaderMCAtom					= 4089,
	ePPREC_FooterMCAtom					= 4090,
	ePPREC_ExMediaAtom					= 4100,
	ePPREC_ExVideo						= 4101,
	ePPREC_ExAviMovie					= 4102,
	ePPREC_ExMCIMovie					= 4103,
	ePPREC_ExMIDIAudio					= 4109,
	ePPREC_ExCDAudio					= 4110,
	ePPREC_ExWAVAudioEmbedded			= 4111,
	ePPREC_ExWAVAudioLink				= 4112,
	ePPREC_ExOleObjStg					= 4113,
	ePPREC_ExCDAudioAtom				= 4114,
	ePPREC_ExWAVAudioEmbeddedAtom		= 4115,
	ePPREC_AnimationInfo				= 4116,
	ePPREC_RTFDateTimeMCAtom			= 4117,
	ePPREC_ProgTags						= 5000,
	ePPREC_ProgStringTag				= 5001,
	ePPREC_ProgBinaryTag				= 5002,
	ePPREC_BinaryTagData				= 5003,
	ePPREC_PrintOptions					= 6000,
	ePPREC_PersistPtrFullBlock			= 6001,
	ePPREC_PersistPtrIncrementalBlock	= 6002,
	ePPREC_RulerIndentAtom				= 10000,
	ePPREC_GscalingAtom					= 10001,
	ePPREC_GrColorAtom					= 10002,
	ePPREC_GLPointAtom					= 10003,
	ePPREC_GlineAtom					= 10004,
} EPPREC_T, FAR *LPEPPREC;

#else // #ifdef PP_ENUM

#define ePPREC_Unknown						0
#define ePPREC_SubContainerCompleted		1
#define ePPREC_IRRAtom						2
#define ePPREC_PSS							3
#define ePPREC_SubContainerException		4
#define ePPREC_ClientSignal1				6
#define ePPREC_ClientSignal2				7
#define ePPREC_PowerPointStateInfoAtom		10
#define ePPREC_Document						1000
#define ePPREC_DocumentAtom					1001
#define ePPREC_EndDocument					1002
#define ePPREC_SlidePersist					1003
#define ePPREC_SlideBase					1004
#define ePPREC_SlideBaseAtom				1005
#define ePPREC_Slide						1006
#define ePPREC_SlideAtom					1007
#define ePPREC_Notes						1008
#define ePPREC_NotesAtom					1009
#define ePPREC_Environment					1010
#define ePPREC_SlidePersistAtom				1011
#define ePPREC_Scheme						1012
#define ePPREC_SchemeAtom					1013
#define ePPREC_DocViewInfo					1014
#define ePPREC_SslideLayoutAtom				1015
#define ePPREC_MainMaster					1016
#define ePPREC_SSSlideInfoAtom				1017
#define ePPREC_SlideViewInfo				1018
#define ePPREC_GuideAtom					1019
#define ePPREC_ViewInfo						1020
#define ePPREC_ViewInfoAtom					1021
#define ePPREC_SlideViewInfoAtom			1022
#define ePPREC_VBAInfo						1023
#define ePPREC_VBAInfoAtom					1024
#define ePPREC_SSDocInfoAtom				1025
#define ePPREC_Summary						1026
#define ePPREC_Texture						1027
#define ePPREC_VBASlideInfo					1028
#define ePPREC_VBASlideInfoAtom				1029
#define ePPREC_DocRoutingSlip				1030
#define ePPREC_OutlineViewInfo				1031
#define ePPREC_SorterViewInfo				1032
#define ePPREC_ExObjList					1033
#define ePPREC_ExObjListAtom				1034
#define ePPREC_PPDrawingGroup				1035
#define ePPREC_PPDrawing					1036
#define ePPREC_NamedShows					1040
#define ePPREC_NamedShow					1041
#define ePPREC_NamedShowSlides				1042
#define ePPREC_List							2000
#define ePPREC_FontCollection				2005
#define ePPREC_ListPlaceholder				2017
#define ePPREC_BookmarkCollection			2019
#define ePPREC_SoundCollection				2020
#define ePPREC_SoundCollAtom				2021
#define ePPREC_Sound						2022
#define ePPREC_SoundData					2023
#define ePPREC_BookmarkSeedAtom				2025
#define ePPREC_GuideList					2026
#define ePPREC_RunArray						2028
#define ePPREC_RunArrayAtom					2029
#define ePPREC_ArrayElementAtom				2030
#define ePPREC_Int4ArrayAtom				2031
#define ePPREC_ColorSchemeAtom				2032
#define ePPREC_OEShape						3008
#define ePPREC_ExObjRefAtom					3009
#define ePPREC_OEPlaceholderAtom			3011
#define ePPREC_GrColor						3020
#define ePPREC_GrectAtom					3025
#define ePPREC_GratioAtom					3031
#define ePPREC_Gscaling						3032
#define ePPREC_GpointAtom					3034
#define ePPREC_OEShapeAtom					3035
#define ePPREC_OutlineTextRefAtom			3998
#define ePPREC_TextHeaderAtom				3999
#define ePPREC_TextCharsAtom				4000
#define ePPREC_StyleTextPropAtom			4001
#define ePPREC_BaseTextPropAtom				4002
#define ePPREC_TxMasterStyleAtom			4003
#define ePPREC_TxCFStyleAtom				4004
#define ePPREC_TxPFStyleAtom				4005
#define ePPREC_TextRulerAtom				4006
#define ePPREC_TextBookmarkAtom				4007
#define ePPREC_TextBytesAtom				4008
#define ePPREC_TxSIStyleAtom				4009
#define ePPREC_TextSpecInfoAtom				4010
#define ePPREC_DefaultRulerAtom				4011
#define ePPREC_FontEntityAtom				4023
#define ePPREC_FontEmbedData				4024
#define ePPREC_TypeFace						4025
#define ePPREC_CString						4026
#define ePPREC_ExternalObject				4027
#define ePPREC_MetaFile						4033
#define ePPREC_ExOleObj						4034
#define ePPREC_ExOleObjAtom					4035
#define ePPREC_ExPlainLinkAtom				4036
#define ePPREC_CorePict						4037
#define ePPREC_CorePictAtom					4038
#define ePPREC_ExPlainAtom					4039
#define ePPREC_SrKinsoku					4040
#define ePPREC_Handout						4041
#define ePPREC_ExEmbed						4044
#define ePPREC_ExEmbedAtom					4045
#define ePPREC_ExLink						4046
#define ePPREC_ExLinkAtom_old				4047
#define ePPREC_BookmarkEntityAtom			4048
#define ePPREC_ExLinkAtom					4049
#define ePPREC_SrKinsokuAtom				4050
#define ePPREC_ExHyperlinkAtom				4051
#define ePPREC_ExPlain						4053
#define ePPREC_ExPlainLink					4054
#define ePPREC_ExHyperlink					4055
#define ePPREC_SlideNumberMCAtom			4056
#define ePPREC_HeadersFooters				4057
#define ePPREC_HeadersFootersAtom			4058
#define ePPREC_RecolorEntryAtom				4062
#define ePPREC_TxInteractiveInfoAtom		4063
#define ePPREC_EmFormatAtom					4065
#define ePPREC_CharFormatAtom				4066
#define ePPREC_ParaFormatAtom				4067
#define ePPREC_MasterText					4068
#define ePPREC_RecolorInfoAtom				4071
#define ePPREC_ExQuickTime					4073
#define ePPREC_ExQuickTimeMovie				4074
#define ePPREC_ExQuickTimeMovieData			4075
#define ePPREC_ExSubscription				4076
#define ePPREC_ExSubscriptionSection		4077
#define ePPREC_ExControl					4078
#define ePPREC_ExControlAtom				4091
#define ePPREC_SlideListWithText			4080
#define ePPREC_AnimationInfoAtom			4081
#define ePPREC_InteractiveInfo				4082
#define ePPREC_InteractiveInfoAtom			4083
#define ePPREC_SlideList					4084
#define ePPREC_UserEditAtom					4085
#define ePPREC_CurrentUserAtom				4086
#define ePPREC_DateTimeMCAtom				4087
#define ePPREC_GenericDateMCAtom			4088
#define ePPREC_HeaderMCAtom					4089
#define ePPREC_FooterMCAtom					4090
#define ePPREC_ExMediaAtom					4100
#define ePPREC_ExVideo						4101
#define ePPREC_ExAviMovie					4102
#define ePPREC_ExMCIMovie					4103
#define ePPREC_ExMIDIAudio					4109
#define ePPREC_ExCDAudio					4110
#define ePPREC_ExWAVAudioEmbedded			4111
#define ePPREC_ExWAVAudioLink				4112
#define ePPREC_ExOleObjStg					4113
#define ePPREC_ExCDAudioAtom				4114
#define ePPREC_ExWAVAudioEmbeddedAtom		4115
#define ePPREC_AnimationInfo				4116
#define ePPREC_RTFDateTimeMCAtom			4117
#define ePPREC_ProgTags						5000
#define ePPREC_ProgStringTag				5001
#define ePPREC_ProgBinaryTag				5002
#define ePPREC_BinaryTagData				5003
#define ePPREC_PrintOptions					6000
#define ePPREC_PersistPtrFullBlock			6001
#define ePPREC_PersistPtrIncrementalBlock	6002
#define ePPREC_RulerIndentAtom				10000
#define ePPREC_GscalingAtom					10001
#define ePPREC_GrColorAtom					10002
#define ePPREC_GLPointAtom					10003
#define ePPREC_GlineAtom					10004

#endif // #ifdef PP_ENUM

#if defined(SYM_WIN32) || defined(SYM_NTK) || defined(SYM_VXD) || defined(SYM_400)
#include "pshpack1.h"
#elif defined(__MACINTOSH__) || defined(SYM_AIX) 
#pragma options align=packed
#else
#pragma pack(1)
#endif

typedef struct tagPP_REC_HDR
{
	WORD	wVerInstance;
	WORD	wType;
	DWORD	dwLen;
} PP_REC_HDR_T, FAR *LPPP_REC_HDR;

typedef struct tagPP_REC_CurrentUserAtom
{
	DWORD	dwSize;
	DWORD	dwMagic;
	DWORD	dwCurrentEditOffset;
	WORD	wUserNameLen;
	WORD	wFileVersion;
	WORD	wMajorVersion;
	WORD	wMinorVersion;
} PP_REC_CurrentUserAtom_T, FAR *LPPP_REC_CurrentUserAtom;

typedef struct tagPP_REC_UserEditAtom
{
	DWORD	dwLastSlideID;
	DWORD	dwVersion;
	DWORD	dwLastEditOffset;
	DWORD	dwPersistDirOffset;
	DWORD	dwDocumentRef;
	DWORD	dwMaxPersistWritten;
	WORD	wLastViewType;
} PP_REC_UserEditAtom_T, FAR *LPPP_REC_UserEditAtom;

typedef struct tagPP_REC_VBAInfoAtom
{
	DWORD	dwExOleObjStgRef;
	DWORD	dwFlags;
	DWORD	dwUnknown;
} PP_REC_VBAInfoAtom_T, FAR *LPPP_REC_VBAInfoAtom;

typedef struct tagPP_REC_ExOleObjAtom
{
	DWORD	dwDrawAspect;
	DWORD	dwType;
	DWORD	dwObjID;
	DWORD	dwSubType;
	DWORD	dwObjStgDataRef;
	BYTE	byIsBlank;
} PP_REC_ExOleObjAtom_T, FAR *LPPP_REC_ExOleObjAtom;

#define PP_REC_ACTION_NONE				0
#define PP_REC_ACTION_MACRO				1
#define PP_REC_ACTION_RUN_PROGRAM		2
#define PP_REC_ACTION_JUMP				3
#define PP_REC_ACTION_HYPERLINK			4
#define PP_REC_ACTION_OLE				5
#define PP_REC_ACTION_MEDIA				6
#define PP_REC_ACTION_CUSTOM_SHOW		7

#define PP_REC_JUMP_NONE				0
#define PP_REC_JUMP_NEXT_SLIDE			1
#define PP_REC_JUMP_PREVIOUS_SLIDE		2
#define PP_REC_JUMP_FIRST_SLIDE			3
#define PP_REC_JUMP_LAST_SLIDE			4
#define PP_REC_JUMP_LAST_VIEWED_SLIDE	5
#define PP_REC_JUMP_END_SHOW			6

typedef struct tagPP_REC_InteractiveInfoAtom
{
	DWORD	dwSoundRef;			// 0: Ref to sound or NULL
	DWORD	dwExHyperlinkID;	// 4: Unique ID to external object
	BYTE	byAction;			// 8: Action
	BYTE	byOleVerb;			// 9: OLE verb to use
	BYTE	byJump;				// A: See Jump Table
	BYTE	byFlags;			// B: Bit 1: Animated
								//    Bit 2: Stop sound
								//    Bit 3: CustomShowReturn
	BYTE	byHyperlinkType;	// C: Value from LinkTo enum
} PP_REC_InteractiveInfoAtom_T, FAR *LPPP_REC_InteractiveInfoAtom;

typedef struct tagPP_REC_TxInteractiveInfoAtom
{
	DWORD	dwBegin;			// 0: Beginning character position
	DWORD	dwEnd;				// 4: Ending character position
} PP_REC_TxInteractiveInfoAtom_T, *LPPP_REC_TxInteractiveInfoAtom;

#if defined(SYM_WIN32) || defined(SYM_NTK) || defined(SYM_VXD) || defined(SYM_400)
#include "poppack.h"
#elif defined(__MACINTOSH__) || defined(SYM_AIX) 
#pragma options align=reset
#else
#pragma pack()
#endif

BOOL PPFindRecord
(
	LPSS_STREAM			lpstStream,
	WORD				wType,
	DWORD				dwStartOffset,
	DWORD				dwMaxDist,
	LPDWORD				lpdwOffset,
	LPDWORD				lpdwLength
);

BOOL PPGetUserEditAtomOffset
(
	LPSS_STREAM			lpstStream,
	LPDWORD				lpdwOffset
);

BOOL PPGetReferenceOffset
(
	LPSS_STREAM				lpstStream,
	DWORD					dwRefNum,
	DWORD					dwUserEditOffset,
	LPDWORD					lpdwOffset
);

BOOL PPGetVBAInfoAtomOffset
(
	LPSS_STREAM				lpstStream,
	DWORD					dwUserEditOffset,
	LPDWORD					lpdwOffset,
	LPDWORD					lpdwLength
);

BOOL PPGetOLEStgOffset
(
	LPSS_STREAM				lpstStream,
	DWORD					dwUserEditOffset,
	LPDWORD					lpdwOffset,
	LPDWORD					lpdwLength,
	LPBOOL					lpbCompressed
);

#define PP_EMBED_TYPE_DEFAULT			0
#define PP_EMBED_TYPE_CLIPART			1
#define PP_EMBED_TYPE_WORD				2
#define PP_EMBED_TYPE_EXCEL				3
#define PP_EMBED_TYPE_GRAPH				4
#define PP_EMBED_TYPE_ORG_CHART			5
#define PP_EMBED_TYPE_EQUATION			6
#define PP_EMBED_TYPE_WORDART			7
#define PP_EMBED_TYPE_SOUND				8
#define PP_EMBED_TYPE_IMAGER			9
#define PP_EMBED_TYPE_PP_PRESENTATION	10
#define PP_EMBED_TYPE_PP_SLIDE			11
#define PP_EMBED_TYPE_PROJECT			12
#define PP_EMBED_TYPE_NOTE_IT			13
#define PP_EMBED_TYPE_EXCEL_CHART		14
#define PP_EMBED_TYPE_MEDIA_PLAYER		15

typedef struct tagPP_EMBED
{
	// User edit atom offset

	DWORD		dwUserEditOffset;

	// If dwEmbedOffset passes the following offset,
	//  there are no more ExEmbed records

	DWORD		dwMaxEmbedOffset;

	// If TRUE, there are no more ExEmbed records

	BOOL		bNoMore;

	// The embedded object's type

	DWORD		dwType;

	// The offset and length of the current ExEmbed record

	DWORD		dwNextSearchOffset;
} PP_EMBED_T, FAR *LPPP_EMBED;

BOOL PPGetFirstEmbeddedObject
(
	LPSS_STREAM				lpstStream,
	DWORD					dwUserEditOffset,
	LPPP_EMBED				lpstEmbed,
	LPDWORD					lpdwOffset,
	LPDWORD					lpdwLength,
	bool					*lpbCompressed
);

BOOL PPGetNextEmbeddedObject
(
	LPSS_STREAM				lpstStream,
	LPPP_EMBED				lpstEmbed,
	LPDWORD					lpdwOffset,
	LPDWORD					lpdwLength,
	bool					*lpbCompressed
);

// Find records

BOOL PPFindRecSet
(
	LPSS_STREAM			lpstStream,
	DWORD				dwOffset,
	DWORD				dwMaxOffset,
	WORD				wCount,
	LPWORD				lpawTypes,
	LPDWORD				lpadwOffsets,
	LPDWORD				lpadwSizes
);

#define PP_REC_FIND_MAX_DEPTH		16

typedef struct tagPP_REC_FIND
{
	LPSS_STREAM		lpstStream;

	BOOL			bFound;

	WORD			wDepth;

	DWORD			dwOffset;
	DWORD			dwMaxOffset;
	WORD			awRecType[PP_REC_FIND_MAX_DEPTH];
	DWORD			adwRecOffset[PP_REC_FIND_MAX_DEPTH];
	DWORD			adwRecSize[PP_REC_FIND_MAX_DEPTH];
} PP_REC_FIND_T, FAR *LPPP_REC_FIND;

void PPRecFindPathInit
(
	LPPP_REC_FIND		lpstFind,
	LPSS_STREAM			lpstStream,
	DWORD				dwOffset,
	DWORD				dwSize
);

BOOL PPRecFindPath
(
	LPPP_REC_FIND		lpstFind,
	LPWORD				lpawPath
);

// Macro action

#define PP_MAX_INTERACTIVE_NAME_LEN		256

typedef struct tagPPACTION_INFO
{
	BOOL				bFound;

	DWORD				dwInteractiveOffset;
	DWORD				dwInteractiveSize;
	BYTE				abyszName[PP_MAX_INTERACTIVE_NAME_LEN];
	DWORD				dwTxInteractiveOffset;
	DWORD				dwTxInteractiveSize;
} PPACTION_INFO_T, FAR *LPPPACTION_INFO;

BOOL PPGetMacroActionInfo
(
	LPPP_REC_FIND		lpstFind,
	LPPPACTION_INFO		lpstInfo
);

BOOL PPDeleteMacroAction
(
	LPSS_STREAM			lpstStream,
	LPPPACTION_INFO		lpstInfo
);

BOOL PPGetVBAEmbedListInfo
(
	LPSS_STREAM				lpstStream,
	DWORD					dwUserEditOffset,
	LPDWORD					lpdwListOffset,
	LPDWORD					lpdwListLength,
	LPDWORD					lpdwObjListOffset,
	LPDWORD					lpdwObjListLength
);

BOOL PPGetVBAStgOffset
(
	LPSS_STREAM				lpstStream,
	DWORD					dwUserEditOffset,
	DWORD					dwListOffset,
	DWORD					dwListLength,
	LPDWORD					lpdwOffset,
	LPDWORD					lpdwLength,
	bool					*lpbCompressed
);

typedef BOOL (*LPFN_PP_EMBED_ENUM_CB)
(
	LPVOID					lpvCookie,
	DWORD					dwEmbeddedObjType
);

BOOL PPEnumEmbeddedObjectInit
(
	LPSS_STREAM				lpstStream,
	DWORD					dwObjListOffset,
	DWORD					dwObjListLength,
	LPPP_EMBED				lpstEmbed
);

BOOL PPEnumEmbeddedObjects
(
	LPSS_STREAM				lpstStream,
	DWORD					dwUserEditOffset,
	LPPP_EMBED				lpstEmbed,
	LPFN_PP_EMBED_ENUM_CB	lpfnEnumCB,
	LPVOID					lpvCookie,
	LPDWORD					lpdwOffset,
	LPDWORD					lpdwLength,
	LPBOOL					lpbCompressed
);

BOOL PPShiftPersistOffsets
(
	LPSS_STREAM				lpstStream,
	DWORD					dwUserEditOffset,
	DWORD					dwStartOffset,
	long					lShiftAmount,
	LPDWORD					lpdwUserEditOffset
);

BOOL PPUpdateRecLen
(
	LPSS_STREAM				lpstStream,
	DWORD					dwOffset,
	DWORD					dwLength
);

#endif // = _PPAPI_H_

