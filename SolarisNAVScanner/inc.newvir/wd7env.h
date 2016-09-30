//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/wd7env.h_v   1.2   15 Jul 1997 17:03:46   DCHI  $
//
// Description:
//      Header file for Word 6.0/7.0/95 emulation environment functions.
//
//************************************************************************
// $Log:   S:/INCLUDE/VCS/wd7env.h_v  $
// 
//    Rev 1.2   15 Jul 1997 17:03:46   DCHI
// Changed size_t sSize to WORD wSize in DIALOG_T structure.
// 
//    Rev 1.1   14 Jul 1997 18:48:16   DCHI
// Added ifdef changing enums to defines to reduce debug symbol usage.
// 
//    Rev 1.0   30 Jun 1997 16:17:38   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _WD7ENV_H_

#define _WD7ENV_H_

#include "gsm.h"

/////////////////////////////////////////////////////////////////
// WordBasic commands
/////////////////////////////////////////////////////////////////

#ifdef WD7_ENUM

typedef enum tagEWDCMD
{
    eWDCMD_Help                              = 0x0001,
    eWDCMD_HelpTool                          = 0x0002,
    eWDCMD_HelpUsingHelp                     = 0x0003,
    eWDCMD_HelpActiveWindow                  = 0x0004,
    eWDCMD_HelpKeyboard                      = 0x0005,
    eWDCMD_HelpIndex                         = 0x0006,
    eWDCMD_HelpQuickPreview                  = 0x0007,
    eWDCMD_HelpExamplesAndDemos              = 0x0008,
    eWDCMD_HelpAbout                         = 0x0009,
    eWDCMD_HelpWordPerfectHelp               = 0x000A,
    eWDCMD_GrowFont                          = 0x000B,
    eWDCMD_ShrinkFont                        = 0x000C,
    eWDCMD_Overtype                          = 0x000D,
    eWDCMD_ExtendSelection                   = 0x000E,
    eWDCMD_Spike                             = 0x000F,
    eWDCMD_InsertSpike                       = 0x0010,
    eWDCMD_ChangeCase                        = 0x0011,
    eWDCMD_MoveText                          = 0x0012,
    eWDCMD_CopyText                          = 0x0013,
    eWDCMD_InsertAutoText                    = 0x0014,
    eWDCMD_OtherPane                         = 0x0015,
    eWDCMD_NextWindow                        = 0x0016,
    eWDCMD_PrevWindow                        = 0x0017,
    eWDCMD_RepeatFind                        = 0x0018,
    eWDCMD_NextField                         = 0x0019,
    eWDCMD_PrevField                         = 0x001A,
    eWDCMD_ColumnSelect                      = 0x001B,
    eWDCMD_DeleteWord                        = 0x001C,
    eWDCMD_DeleteBackWord                    = 0x001D,
    eWDCMD_EditClear                         = 0x001E,
    eWDCMD_InsertFieldChars                  = 0x001F,
    eWDCMD_UpdateFields                      = 0x0020,
    eWDCMD_UnlinkFields                      = 0x0021,
    eWDCMD_ToggleFieldDisplay                = 0x0022,
    eWDCMD_LockFields                        = 0x0023,
    eWDCMD_UnlockFields                      = 0x0024,
    eWDCMD_UpdateSource                      = 0x0025,
    eWDCMD_Indent                            = 0x0026,
    eWDCMD_UnIndent                          = 0x0027,
    eWDCMD_HangingIndent                     = 0x0028,
    eWDCMD_UnHang                            = 0x0029,
    eWDCMD_Font                              = 0x002A,
    eWDCMD_FontSizeSelect                    = 0x002B,
    eWDCMD_WW2_RulerMode                     = 0x002C,
    eWDCMD_Bold                              = 0x002D,
    eWDCMD_Italic                            = 0x002E,
    eWDCMD_SmallCaps                         = 0x002F,
    eWDCMD_AllCaps                           = 0x0030,
    eWDCMD_Strikethrough                     = 0x0031,
    eWDCMD_Hidden                            = 0x0032,
    eWDCMD_Underline                         = 0x0033,
    eWDCMD_DoubleUnderline                   = 0x0034,
    eWDCMD_WordUnderline                     = 0x0035,
    eWDCMD_Superscript                       = 0x0036,
    eWDCMD_Subscript                         = 0x0037,
    eWDCMD_ResetChar                         = 0x0038,
    eWDCMD_CharColor                         = 0x0039,
    eWDCMD_LeftPara                          = 0x003A,
    eWDCMD_CenterPara                        = 0x003B,
    eWDCMD_RightPara                         = 0x003C,
    eWDCMD_JustifyPara                       = 0x003D,
    eWDCMD_SpacePara1                        = 0x003E,
    eWDCMD_SpacePara15                       = 0x003F,
    eWDCMD_SpacePara2                        = 0x0040,
    eWDCMD_CloseUpPara                       = 0x0041,
    eWDCMD_OpenUpPara                        = 0x0042,
    eWDCMD_ResetPara                         = 0x0043,
    eWDCMD_EditRepeat                        = 0x0044,
    eWDCMD_GoBack                            = 0x0045,
    eWDCMD_SaveTemplate                      = 0x0046,
    eWDCMD_OK                                = 0x0047,
    eWDCMD_Cancel                            = 0x0048,
    eWDCMD_CopyFormat                        = 0x0049,
    eWDCMD_PrevPage                          = 0x004A,
    eWDCMD_NextPage                          = 0x004B,
    eWDCMD_NextObject                        = 0x004C,
    eWDCMD_PrevObject                        = 0x004D,
    eWDCMD_DocumentStatistics                = 0x004E,
    eWDCMD_FileNew                           = 0x004F,
    eWDCMD_FileOpen                          = 0x0050,
    eWDCMD_MailMergeOpenDataSource           = 0x0051,
    eWDCMD_MailMergeOpenHeaderSource         = 0x0052,
    eWDCMD_FileSave                          = 0x0053,
    eWDCMD_FileSaveAs                        = 0x0054,
    eWDCMD_FileSaveAll                       = 0x0055,
    eWDCMD_FileSummaryInfo                   = 0x0056,
    eWDCMD_FileTemplates                     = 0x0057,
    eWDCMD_FilePrint                         = 0x0058,
    eWDCMD_FilePrintPreview                  = 0x0059,
    eWDCMD_WW2_PrintMerge                    = 0x005A,
    eWDCMD_WW2_PrintMergeCheck               = 0x005B,
    eWDCMD_WW2_PrintMergeToDoc               = 0x005C,
    eWDCMD_WW2_PrintMergeToPrinter           = 0x005D,
    eWDCMD_WW2_PrintMergeSelection           = 0x005E,
    eWDCMD_WW2_PrintMergeHelper              = 0x005F,
    eWDCMD_MailMergeReset                    = 0x0060,
    eWDCMD_FilePrintSetup                    = 0x0061,
    eWDCMD_FileExit                          = 0x0062,
    eWDCMD_FileFind                          = 0x0063,
    eWDCMD_FormatAddrFonts                   = 0x0067,
    eWDCMD_MailMergeEditDataSource           = 0x0068,
    eWDCMD_WW2_PrintMergeCreateDataSource    = 0x0069,
    eWDCMD_WW2_PrintMergeCreateHeaderSource  = 0x006A,
    eWDCMD_EditUndo                          = 0x006B,
    eWDCMD_EditCut                           = 0x006C,
    eWDCMD_EditCopy                          = 0x006D,
    eWDCMD_EditPaste                         = 0x006E,
    eWDCMD_EditPasteSpecial                  = 0x006F,
    eWDCMD_EditFind                          = 0x0070,
    eWDCMD_EditFindFont                      = 0x0071,
    eWDCMD_EditFindPara                      = 0x0072,
    eWDCMD_EditFindStyle                     = 0x0073,
    eWDCMD_EditFindClearFormatting           = 0x0074,
    eWDCMD_EditReplace                       = 0x0075,
    eWDCMD_EditReplaceFont                   = 0x0076,
    eWDCMD_EditReplacePara                   = 0x0077,
    eWDCMD_EditReplaceStyle                  = 0x0078,
    eWDCMD_EditReplaceClearFormatting        = 0x0079,
    eWDCMD_EditGoTo                          = 0x007A,
    eWDCMD_EditAutoText                      = 0x007B,
    eWDCMD_EditLinks                         = 0x007C,
    eWDCMD_EditObject                        = 0x007D,
    eWDCMD_ActivateObject                    = 0x007E,
    eWDCMD_TextToTable                       = 0x007F,
    eWDCMD_TableToText                       = 0x0080,
    eWDCMD_TableInsertTable                  = 0x0081,
    eWDCMD_TableInsertCells                  = 0x0082,
    eWDCMD_TableInsertRow                    = 0x0083,
    eWDCMD_TableInsertColumn                 = 0x0084,
    eWDCMD_TableDeleteCells                  = 0x0085,
    eWDCMD_TableDeleteRow                    = 0x0086,
    eWDCMD_TableDeleteColumn                 = 0x0087,
    eWDCMD_TableMergeCells                   = 0x0088,
    eWDCMD_TableSplitCells                   = 0x0089,
    eWDCMD_TableSplit                        = 0x008A,
    eWDCMD_TableSelectTable                  = 0x008B,
    eWDCMD_TableSelectRow                    = 0x008C,
    eWDCMD_TableSelectColumn                 = 0x008D,
    eWDCMD_TableRowHeight                    = 0x008E,
    eWDCMD_TableColumnWidth                  = 0x008F,
    eWDCMD_TableGridlines                    = 0x0090,
    eWDCMD_ViewNormal                        = 0x0091,
    eWDCMD_ViewOutline                       = 0x0092,
    eWDCMD_ViewPage                          = 0x0093,
    eWDCMD_WW2_ViewZoom                      = 0x0094,
    eWDCMD_ViewDraft                         = 0x0095,
    eWDCMD_ViewFieldCodes                    = 0x0096,
    eWDCMD_Style                             = 0x0097,
    eWDCMD_ToolsCustomize                    = 0x0098,
    eWDCMD_ViewRuler                         = 0x0099,
    eWDCMD_ViewStatusBar                     = 0x009A,
    eWDCMD_NormalViewHeaderArea              = 0x009B,
    eWDCMD_ViewFootnoteArea                  = 0x009C,
    eWDCMD_ViewAnnotations                   = 0x009D,
    eWDCMD_InsertFrame                       = 0x009E,
    eWDCMD_InsertBreak                       = 0x009F,
    eWDCMD_WW2_InsertFootnote                = 0x00A0,
    eWDCMD_InsertAnnotation                  = 0x00A1,
    eWDCMD_InsertSymbol                      = 0x00A2,
    eWDCMD_InsertPicture                     = 0x00A3,
    eWDCMD_InsertFile                        = 0x00A4,
    eWDCMD_InsertDateTime                    = 0x00A5,
    eWDCMD_InsertField                       = 0x00A6,
    eWDCMD_InsertMergeField                  = 0x00A7,
    eWDCMD_EditBookmark                      = 0x00A8,
    eWDCMD_MarkIndexEntry                    = 0x00A9,
    eWDCMD_InsertIndex                       = 0x00AA,
    eWDCMD_InsertTableOfContents             = 0x00AB,
    eWDCMD_InsertObject                      = 0x00AC,
    eWDCMD_ToolsCreateEnvelope               = 0x00AD,
    eWDCMD_FormatFont                        = 0x00AE,
    eWDCMD_FormatParagraph                   = 0x00AF,
    eWDCMD_FormatSectionLayout               = 0x00B0,
    eWDCMD_FormatColumns                     = 0x00B1,
    eWDCMD_FilePageSetup                     = 0x00B2,
    eWDCMD_FormatTabs                        = 0x00B3,
    eWDCMD_FormatStyle                       = 0x00B4,
    eWDCMD_FormatDefineStyleFont             = 0x00B5,
    eWDCMD_FormatDefineStylePara             = 0x00B6,
    eWDCMD_FormatDefineStyleTabs             = 0x00B7,
    eWDCMD_FormatDefineStyleFrame            = 0x00B8,
    eWDCMD_FormatDefineStyleBorders          = 0x00B9,
    eWDCMD_FormatDefineStyleLang             = 0x00BA,
    eWDCMD_FormatPicture                     = 0x00BB,
    eWDCMD_ToolsLanguage                     = 0x00BC,
    eWDCMD_FormatBordersAndShading           = 0x00BD,
    eWDCMD_FormatFrame                       = 0x00BE,
    eWDCMD_ToolsSpelling                     = 0x00BF,
    eWDCMD_ToolsSpellSelection               = 0x00C0,
    eWDCMD_ToolsGrammar                      = 0x00C1,
    eWDCMD_ToolsThesaurus                    = 0x00C2,
    eWDCMD_ToolsHyphenation                  = 0x00C3,
    eWDCMD_ToolsBulletsNumbers               = 0x00C4,
    eWDCMD_ToolsRevisions                    = 0x00C5,
    eWDCMD_ToolsCompareVersions              = 0x00C6,
    eWDCMD_TableSort                         = 0x00C7,
    eWDCMD_ToolsCalculate                    = 0x00C8,
    eWDCMD_ToolsRepaginate                   = 0x00C9,
    eWDCMD_ToolsOptions                      = 0x00CA,
    eWDCMD_ToolsOptionsGeneral               = 0x00CB,
    eWDCMD_ToolsOptionsView                  = 0x00CC,
    eWDCMD_ToolsAdvancedSettings             = 0x00CE,
    eWDCMD_ToolsOptionsPrint                 = 0x00D0,
    eWDCMD_ToolsOptionsSave                  = 0x00D1,
    eWDCMD_WW2_ToolsOptionsToolbar           = 0x00D2,
    eWDCMD_ToolsOptionsSpelling              = 0x00D3,
    eWDCMD_ToolsOptionsGrammar               = 0x00D4,
    eWDCMD_ToolsOptionsUserInfo              = 0x00D5,
    eWDCMD_ToolsRecordMacroToggle            = 0x00D6,
    eWDCMD_ToolsMacro                        = 0x00D7,
    eWDCMD_PauseRecorder                     = 0x00D8,
    eWDCMD_WindowNewWindow                   = 0x00D9,
    eWDCMD_WindowArrangeAll                  = 0x00DA,
    eWDCMD_MailMergeEditMainDocument         = 0x00DB,
    eWDCMD_WindowList                        = 0x00DC,
    eWDCMD_FormatRetAddrFonts                = 0x00DD,
    eWDCMD_Organizer                         = 0x00DE,
    eWDCMD_WW2_TableColumnWidth              = 0x00DF,
    eWDCMD_ToolsOptionsEdit                  = 0x00E0,
    eWDCMD_ToolsOptionsFileLocations         = 0x00E1,
    eWDCMD_RecordNextCommand                 = 0x00E2,
    eWDCMD_ToolsAutoCorrectSmartQuotes       = 0x00E3,
    eWDCMD_ToolsWordCount                    = 0x00E4,
    eWDCMD_DocSplit                          = 0x00E5,
    eWDCMD_DocSize                           = 0x00E6,
    eWDCMD_DocMove                           = 0x00E7,
    eWDCMD_DocMaximize                       = 0x00E8,
    eWDCMD_DocRestore                        = 0x00E9,
    eWDCMD_DocClose                          = 0x00EA,
    eWDCMD_ControlRun                        = 0x00EB,
    eWDCMD_ShrinkSelection                   = 0x00EC,
    eWDCMD_EditSelectAll                     = 0x00ED,
    eWDCMD_IconBarMode                       = 0x00EE,
    eWDCMD_InsertPageField                   = 0x00EF,
    eWDCMD_InsertDateField                   = 0x00F0,
    eWDCMD_InsertTimeField                   = 0x00F1,
    eWDCMD_FormatHeaderFooterLink            = 0x00F2,
    eWDCMD_ClosePane                         = 0x00F3,
    eWDCMD_OutlinePromote                    = 0x00F4,
    eWDCMD_OutlineDemote                     = 0x00F5,
    eWDCMD_OutlineMoveUp                     = 0x00F6,
    eWDCMD_OutlineMoveDown                   = 0x00F7,
    eWDCMD_NormalStyle                       = 0x00F8,
    eWDCMD_OutlineExpand                     = 0x00F9,
    eWDCMD_OutlineCollapse                   = 0x00FA,
    eWDCMD_ShowHeading1                      = 0x00FB,
    eWDCMD_ShowHeading2                      = 0x00FC,
    eWDCMD_ShowHeading3                      = 0x00FD,
    eWDCMD_ShowHeading4                      = 0x00FE,
    eWDCMD_ShowHeading5                      = 0x00FF,
    eWDCMD_ShowHeading6                      = 0x0100,
    eWDCMD_ShowHeading7                      = 0x0101,
    eWDCMD_ShowHeading8                      = 0x0102,
    eWDCMD_ShowHeading9                      = 0x0103,
    eWDCMD_ShowAllHeadings                   = 0x0104,
    eWDCMD_OutlineShowFirstLine              = 0x0105,
    eWDCMD_OutlineShowFormat                 = 0x0106,
    eWDCMD_ShowVars                          = 0x0107,
    eWDCMD_StepOver                          = 0x0108,
    eWDCMD_StepIn                            = 0x0109,
    eWDCMD_ContinueMacro                     = 0x010A,
    eWDCMD_TraceMacro                        = 0x010B,
    eWDCMD_NextCell                          = 0x010E,
    eWDCMD_PrevCell                          = 0x010F,
    eWDCMD_StartOfRow                        = 0x0110,
    eWDCMD_EndOfRow                          = 0x0111,
    eWDCMD_StartOfColumn                     = 0x0112,
    eWDCMD_EndOfColumn                       = 0x0113,
    eWDCMD_ShowAll                           = 0x0114,
    eWDCMD_InsertPageBreak                   = 0x0115,
    eWDCMD_InsertColumnBreak                 = 0x0116,
    eWDCMD_AppMinimize                       = 0x0117,
    eWDCMD_AppMaximize                       = 0x0118,
    eWDCMD_AppRestore                        = 0x0119,
    eWDCMD_DoFieldClick                      = 0x011A,
    eWDCMD_FileClose                         = 0x011B,
    eWDCMD_InsertDrawing                     = 0x011C,
    eWDCMD_InsertChart                       = 0x011D,
    eWDCMD_SelectCurFont                     = 0x011E,
    eWDCMD_SelectCurAlignment                = 0x011F,
    eWDCMD_SelectCurSpacing                  = 0x0120,
    eWDCMD_SelectCurIndent                   = 0x0121,
    eWDCMD_SelectCurTabs                     = 0x0122,
    eWDCMD_SelectCurColor                    = 0x0123,
    eWDCMD_RemoveFrames                      = 0x0124,
    eWDCMD_MenuMode                          = 0x0125,
    eWDCMD_InsertPageNumbers                 = 0x0126,
    eWDCMD_WW2_ChangeRulerMode               = 0x0127,
    eWDCMD_EditPicture                       = 0x0128,
    eWDCMD_UserDialog                        = 0x0129,
    eWDCMD_FormatPageNumber                  = 0x012A,
    eWDCMD_WW2_FootnoteOptions               = 0x012B,
    eWDCMD_CopyFile                          = 0x012C,
    eWDCMD_FileNewDefault                    = 0x012D,
    eWDCMD_FilePrintDefault                  = 0x012E,
    eWDCMD_ViewZoomWholePage                 = 0x012F,
    eWDCMD_ViewZoomPageWidth                 = 0x0130,
    eWDCMD_ViewZoom100                       = 0x0131,
    eWDCMD_TogglePortrait                    = 0x0132,
    eWDCMD_ToolsBulletListDefault            = 0x0133,
    eWDCMD_ToggleScribbleMode                = 0x0134,
    eWDCMD_ToolsNumberListDefault            = 0x0135,
    eWDCMD_FileAOCEAddMailer                 = 0x0137,
    eWDCMD_FileAOCEDeleteMailer              = 0x0138,
    eWDCMD_FileAOCEExpandMailer              = 0x0139,
    eWDCMD_FileAOCESendMail                  = 0x013B,
    eWDCMD_FileAOCEReplyMail                 = 0x013C,
    eWDCMD_FileAOCEReplyAllMail              = 0x013D,
    eWDCMD_FileAOCEForwardMail               = 0x013E,
    eWDCMD_FileAOCENextLetter                = 0x013F,
    eWDCMD_DocMinimize                       = 0x0140,
    eWDCMD_FormatAutoFormatBegin             = 0x0141,
    eWDCMD_FormatChangeCase                  = 0x0142,
    eWDCMD_ViewToolbars                      = 0x0143,
    eWDCMD_TableInsertGeneral                = 0x0144,
    eWDCMD_TableDeleteGeneral                = 0x0145,
    eWDCMD_WW2_TableRowHeight                = 0x0146,
    eWDCMD_TableToOrFromText                 = 0x0147,
    eWDCMD_EditRedo                          = 0x0149,
    eWDCMD_EditRedoOrRepeat                  = 0x014A,
    eWDCMD_ViewEndnoteArea                   = 0x0152,
    eWDCMD_MailMergeDataForm                 = 0x0154,
    eWDCMD_InsertDatabase                    = 0x0155,
    eWDCMD_WW2_InsertTableOfContents         = 0x0158,
    eWDCMD_WW2_ToolsHyphenation              = 0x0159,
    eWDCMD_FormatFrameOrFramePicture         = 0x015A,
    eWDCMD_WW2_ToolsOptionsPrint             = 0x015B,
    eWDCMD_TableFormula                      = 0x015C,
    eWDCMD_TextFormField                     = 0x015D,
    eWDCMD_CheckBoxFormField                 = 0x015E,
    eWDCMD_DropDownFormField                 = 0x015F,
    eWDCMD_FormFieldOptions                  = 0x0161,
    eWDCMD_ProtectForm                       = 0x0162,
    eWDCMD_InsertCaption                     = 0x0165,
    eWDCMD_InsertCaptionNumbering            = 0x0166,
    eWDCMD_InsertAutoCaption                 = 0x0167,
    eWDCMD_HelpPSSHelp                       = 0x0168,
    eWDCMD_DrawTextbox                       = 0x016B,
    eWDCMD_ToolsOptionsAutoFormat            = 0x016D,
    eWDCMD_DemoteToBodyText                  = 0x016E,
    eWDCMD_InsertCrossReference              = 0x016F,
    eWDCMD_InsertFootnoteNow                 = 0x0170,
    eWDCMD_InsertEndnoteNow                  = 0x0171,
    eWDCMD_InsertFootnote                    = 0x0172,
    eWDCMD_NoteOptions                       = 0x0175,
    eWDCMD_WW2_FormatCharacter               = 0x0176,
    eWDCMD_DrawLine                          = 0x0178,
    eWDCMD_DrawRectangle                     = 0x0179,
    eWDCMD_ToolsAutoCorrect                  = 0x017A,
    eWDCMD_ToolsAutoCorrectReplaceText       = 0x017C,
    eWDCMD_ToolsAutoCorrectInitialCaps       = 0x017D,
    eWDCMD_ToolsAutoCorrectSentenceCaps      = 0x017F,
    eWDCMD_ToolsAutoCorrectDays              = 0x0180,
    eWDCMD_FormatAutoFormat                  = 0x0181,
    eWDCMD_ToolsOptionsRevisions             = 0x0182,
    eWDCMD_WW2_ToolsOptionsGeneral           = 0x0183,
    eWDCMD_ResetNoteSepOrNotice              = 0x0184,
    eWDCMD_FormatBullet                      = 0x0185,
    eWDCMD_FormatNumber                      = 0x0186,
    eWDCMD_FormatMultilevel                  = 0x0187,
    eWDCMD_ConvertObject                     = 0x0188,
    eWDCMD_TableSortAToZ                     = 0x0189,
    eWDCMD_TableSortZToA                     = 0x018A,
    eWDCMD_FormatBulletsAndNumbering         = 0x018D,
    eWDCMD_FormatNumberDefault               = 0x018E,
    eWDCMD_FormatBulletDefault               = 0x018F,
    eWDCMD_InsertAddCaption                  = 0x0192,
    eWDCMD_GoToNextPage                      = 0x0194,
    eWDCMD_GoToPreviousPage                  = 0x0195,
    eWDCMD_GoToNextSection                   = 0x0196,
    eWDCMD_GoToPreviousSection               = 0x0197,
    eWDCMD_GoToNextFootnote                  = 0x0198,
    eWDCMD_GoToPreviousFootnote              = 0x0199,
    eWDCMD_GoToNextEndnote                   = 0x019A,
    eWDCMD_GoToPreviousEndnote               = 0x019B,
    eWDCMD_GoToNextAnnotation                = 0x019C,
    eWDCMD_GoToPreviousAnnotation            = 0x019D,
    eWDCMD_WW2_FormatDefineStyleChar         = 0x019E,
    eWDCMD_WW2_EditFindChar                  = 0x019F,
    eWDCMD_WW2_EditReplaceChar               = 0x01A0,
    eWDCMD_AppMove                           = 0x01A2,
    eWDCMD_AppSize                           = 0x01A3,
    eWDCMD_Connect                           = 0x01A4,
    eWDCMD_WW2_EditFind                      = 0x01A5,
    eWDCMD_WW2_EditReplace                   = 0x01A6,
    eWDCMD_EditFindLang                      = 0x01AC,
    eWDCMD_EditReplaceLang                   = 0x01AD,
    eWDCMD_MailMergeViewData                 = 0x01AF,
    eWDCMD_ToolsCustomizeKeyboard            = 0x01B0,
    eWDCMD_ToolsCustomizeMenus               = 0x01B1,
    eWDCMD_WW2_ToolsOptionsKeyboard          = 0x01B2,
    eWDCMD_ToolsMergeRevisions               = 0x01B3,
    eWDCMD_ClosePreview                      = 0x01B5,
    eWDCMD_SkipNumbering                     = 0x01B6,
    eWDCMD_EditConvertAllFootnotes           = 0x01B7,
    eWDCMD_EditConvertAllEndnotes            = 0x01B8,
    eWDCMD_EditSwapAllNotes                  = 0x01B9,
    eWDCMD_MarkTableOfContentsEntry          = 0x01BA,
    eWDCMD_FileMacPageSetupGX                = 0x01BC,
    eWDCMD_FilePrintOneCopy                  = 0x01BD,
    eWDCMD_EditFindTabs                      = 0x01BE,
    eWDCMD_EditFindBorder                    = 0x01BF,
    eWDCMD_EditFindFrame                     = 0x01C0,
    eWDCMD_BorderOutside                     = 0x01C1,
    eWDCMD_BorderNone                        = 0x01C2,
    eWDCMD_BorderLineStyle                   = 0x01C3,
    eWDCMD_ShadingPattern                    = 0x01C4,
    eWDCMD_DrawEllipse                       = 0x01C6,
    eWDCMD_DrawArc                           = 0x01C7,
    eWDCMD_EditReplaceTabs                   = 0x01C8,
    eWDCMD_EditReplaceBorder                 = 0x01C9,
    eWDCMD_EditReplaceFrame                  = 0x01CA,
    eWDCMD_ShowClipboard                     = 0x01CB,
    eWDCMD_EditConvertNotes                  = 0x01CE,
    eWDCMD_MarkCitation                      = 0x01CF,
    eWDCMD_WW2_ToolsRevisionsMark            = 0x01D0,
    eWDCMD_DrawGroup                         = 0x01D1,
    eWDCMD_DrawBringToFront                  = 0x01D2,
    eWDCMD_DrawSendToBack                    = 0x01D3,
    eWDCMD_DrawSendBehindText                = 0x01D4,
    eWDCMD_DrawBringInFrontOfText            = 0x01D5,
    eWDCMD_InsertTableOfAuthorities          = 0x01D7,
    eWDCMD_InsertTableOfFigures              = 0x01D8,
    eWDCMD_InsertIndexAndTables              = 0x01D9,
    eWDCMD_MailMergeNextRecord               = 0x01DE,
    eWDCMD_MailMergePrevRecord               = 0x01DF,
    eWDCMD_MailMergeFirstRecord              = 0x01E0,
    eWDCMD_MailMergeLastRecord               = 0x01E1,
    eWDCMD_MailMergeGoToRecord               = 0x01E2,
    eWDCMD_InsertFormField                   = 0x01E3,
    eWDCMD_ViewHeader                        = 0x01E4,
    eWDCMD_DrawUngroup                       = 0x01E5,
    eWDCMD_PasteFormat                       = 0x01E6,
    eWDCMD_WW2_ToolsOptionsMenus             = 0x01E7,
    eWDCMD_FormatDropCap                     = 0x01E8,
    eWDCMD_ToolsCreateLabels                 = 0x01E9,
    eWDCMD_ViewMasterDocument                = 0x01EA,
    eWDCMD_CreateSubdocument                 = 0x01EB,
    eWDCMD_ViewFootnoteSeparator             = 0x01ED,
    eWDCMD_ViewFootnoteContSeparator         = 0x01EE,
    eWDCMD_ViewFootnoteContNotice            = 0x01EF,
    eWDCMD_ViewEndnoteSeparator              = 0x01F0,
    eWDCMD_ViewEndnoteContSeparator          = 0x01F1,
    eWDCMD_ViewEndnoteContNotice             = 0x01F2,
    eWDCMD_WW2_ToolsOptionsView              = 0x01F3,
    eWDCMD_DrawBringForward                  = 0x01F4,
    eWDCMD_DrawSendBackward                  = 0x01F5,
    eWDCMD_ViewFootnotes                     = 0x01F6,
    eWDCMD_ToolsProtectDocument              = 0x01F7,
    eWDCMD_ToolsShrinkToFit                  = 0x01F8,
    eWDCMD_FormatStyleGallery                = 0x01F9,
    eWDCMD_ToolsReviewRevisions              = 0x01FA,
    eWDCMD_HelpSearch                        = 0x01FE,
    eWDCMD_HelpWordPerfectHelpOptions        = 0x01FF,
    eWDCMD_MailMergeConvertChevrons          = 0x0200,
    eWDCMD_GrowFontOnePoint                  = 0x0201,
    eWDCMD_ShrinkFontOnePoint                = 0x0202,
    eWDCMD_Magnifier                         = 0x0203,
    eWDCMD_FilePrintPreviewFullScreen        = 0x0204,
    eWDCMD_InsertSound                       = 0x0207,
    eWDCMD_ToolsProtectUnprotectDocument     = 0x0208,
    eWDCMD_ToolsUnprotectDocument            = 0x0209,
    eWDCMD_RemoveBulletsNumbers              = 0x020A,
    eWDCMD_FileCloseOrCloseAll               = 0x020B,
    eWDCMD_FileCloseAll                      = 0x020C,
    eWDCMD_ToolsOptionsCompatibility         = 0x020D,
    eWDCMD_CopyButtonImage                   = 0x020E,
    eWDCMD_PasteButtonImage                  = 0x020F,
    eWDCMD_ResetButtonImage                  = 0x0210,
    eWDCMD_Columns                           = 0x0212,
    eWDCMD_Condensed                         = 0x0213,
    eWDCMD_Expanded                          = 0x0214,
    eWDCMD_FontSize                          = 0x0215,
    eWDCMD_Lowered                           = 0x0216,
    eWDCMD_Raised                            = 0x0217,
    eWDCMD_FileOpenFile                      = 0x0218,
    eWDCMD_DrawRoundRectangle                = 0x0219,
    eWDCMD_DrawFreeformPolygon               = 0x021A,
    eWDCMD_SelectDrawingObjects              = 0x0221,
    eWDCMD_Shading                           = 0x0222,
    eWDCMD_Borders                           = 0x0223,
    eWDCMD_Color                             = 0x0224,
    eWDCMD_DialogEditor                      = 0x0228,
    eWDCMD_MacroREM                          = 0x0229,
    eWDCMD_StartMacro                        = 0x022A,
    eWDCMD_Symbol                            = 0x022B,
    eWDCMD_DrawToggleLayer                   = 0x022C,
    eWDCMD_ToolsCustomizeKeyboardShortcut    = 0x022D,
    eWDCMD_ToolsCustomizeAddMenuShortcut     = 0x022E,
    eWDCMD_DrawFlipHorizontal                = 0x022F,
    eWDCMD_DrawFlipVertical                  = 0x0230,
    eWDCMD_DrawRotateRight                   = 0x0231,
    eWDCMD_DrawRotateLeft                    = 0x0232,
    eWDCMD_TableAutoFormat                   = 0x0233,
    eWDCMD_FormatDrawingObject               = 0x0235,
    eWDCMD_InsertExcelTable                  = 0x0237,
    eWDCMD_MailMergeListWordFields           = 0x0238,
    eWDCMD_MailMergeFindRecord               = 0x0239,
    eWDCMD_NormalFontSpacing                 = 0x023B,
    eWDCMD_NormalFontPosition                = 0x023C,
    eWDCMD_ViewZoom200                       = 0x023D,
    eWDCMD_ViewZoom75                        = 0x023E,
    eWDCMD_DrawDisassemblePicture            = 0x023F,
    eWDCMD_ViewZoom                          = 0x0241,
    eWDCMD_ToolsProtectSection               = 0x0242,
    eWDCMD_FontSubstitution                  = 0x0245,
    eWDCMD_ToggleFull                        = 0x0246,
    eWDCMD_InsertSubdocument                 = 0x0247,
    eWDCMD_MergeSubdocument                  = 0x0248,
    eWDCMD_SplitSubdocument                  = 0x0249,
    eWDCMD_NewToolbar                        = 0x024A,
    eWDCMD_ToggleMainTextLayer               = 0x024B,
    eWDCMD_ShowPrevHeaderFooter              = 0x024C,
    eWDCMD_ShowNextHeaderFooter              = 0x024D,
    eWDCMD_GoToHeaderFooter                  = 0x024E,
    eWDCMD_PromoteList                       = 0x024F,
    eWDCMD_DemoteList                        = 0x0250,
    eWDCMD_ApplyHeading1                     = 0x0251,
    eWDCMD_ApplyHeading2                     = 0x0252,
    eWDCMD_ApplyHeading3                     = 0x0253,
    eWDCMD_ApplyListBullet                   = 0x0254,
    eWDCMD_GotoAnnotationScope               = 0x0255,
    eWDCMD_TableHeadings                     = 0x0256,
    eWDCMD_OpenSubdocument                   = 0x0257,
    eWDCMD_LockDocument                      = 0x0258,
    eWDCMD_ToolsCustomizeRemoveMenuShortcut  = 0x0259,
    eWDCMD_FormatDefineStyleNumbers          = 0x025A,
    eWDCMD_FormatHeadingNumbering            = 0x025B,
    eWDCMD_ViewBorderToolbar                 = 0x025C,
    eWDCMD_ViewDrawingToolbar                = 0x025D,
    eWDCMD_FormatHeadingNumber               = 0x025E,
    eWDCMD_ToolsEnvelopesAndLabels           = 0x025F,
    eWDCMD_DrawReshape                       = 0x0260,
    eWDCMD_MailMergeAskToConvertChevrons     = 0x0261,
    eWDCMD_FormatCallout                     = 0x0262,
    eWDCMD_DrawCallout                       = 0x0263,
    eWDCMD_TableFormatCell                   = 0x0264,
    eWDCMD_FileSendMail                      = 0x0265,
    eWDCMD_EditButtonImage                   = 0x0266,
    eWDCMD_ToolsCustomizeMenuBar             = 0x0267,
    eWDCMD_AutoMarkIndexEntries              = 0x0268,
    eWDCMD_InsertEnSpace                     = 0x026A,
    eWDCMD_InsertEmSpace                     = 0x026B,
    eWDCMD_DottedUnderline                   = 0x026C,
    eWDCMD_ParaKeepLinesTogether             = 0x026D,
    eWDCMD_ParaKeepWithNext                  = 0x026E,
    eWDCMD_ParaPageBreakBefore               = 0x026F,
    eWDCMD_FileRoutingSlip                   = 0x0270,
    eWDCMD_EditTOACategory                   = 0x0271,
    eWDCMD_TableUpdateAutoFormat             = 0x0272,
    eWDCMD_ChooseButtonImage                 = 0x0273,
    eWDCMD_ParaWidowOrphanControl            = 0x0274,
    eWDCMD_ToolsAddRecordDefault             = 0x0275,
    eWDCMD_ToolsRemoveRecordDefault          = 0x0276,
    eWDCMD_ToolsManageFields                 = 0x0277,
    eWDCMD_ViewToggleMasterDocument          = 0x0278,
    eWDCMD_DrawSnapToGrid                    = 0x0279,
    eWDCMD_DrawAlign                         = 0x027A,
    eWDCMD_HelpTipOfTheDay                   = 0x027B,
    eWDCMD_FormShading                       = 0x027C,
    eWDCMD_RemoveSubdocument                 = 0x027F,
    eWDCMD_CloseViewHeaderFooter             = 0x0280,
    eWDCMD_TableAutoSum                      = 0x0281,
    eWDCMD_MailMergeCreateDataSource         = 0x0282,
    eWDCMD_MailMergeCreateHeaderSource       = 0x0283,
    eWDCMD_StopMacroRunning                  = 0x0285,
    eWDCMD_DrawInsertWordPicture             = 0x0288,
    eWDCMD_IncreaseIndent                    = 0x0289,
    eWDCMD_DecreaseIndent                    = 0x028A,
    eWDCMD_SymbolFont                        = 0x028B,
    eWDCMD_ToggleHeaderFooterLink            = 0x028C,
    eWDCMD_AutoText                          = 0x028D,
    eWDCMD_ViewFooter                        = 0x028E,
    eWDCMD_MicrosoftMail                     = 0x0290,
    eWDCMD_MicrosoftExcel                    = 0x0291,
    eWDCMD_MicrosoftAccess                   = 0x0292,
    eWDCMD_MicrosoftSchedule                 = 0x0293,
    eWDCMD_MicrosoftFoxPro                   = 0x0294,
    eWDCMD_MicrosoftPowerPoint               = 0x0295,
    eWDCMD_MicrosoftPublisher                = 0x0296,
    eWDCMD_MicrosoftProject                  = 0x0297,
    eWDCMD_ListMacros                        = 0x0298,
    eWDCMD_ScreenRefresh                     = 0x0299,
    eWDCMD_ToolsRecordMacroStart             = 0x029A,
    eWDCMD_ToolsRecordMacroStop              = 0x029B,
    eWDCMD_StopMacro                         = 0x029C,
    eWDCMD_ToggleMacroRun                    = 0x029D,
    eWDCMD_DrawNudgeUp                       = 0x029E,
    eWDCMD_DrawNudgeDown                     = 0x029F,
    eWDCMD_DrawNudgeLeft                     = 0x02A0,
    eWDCMD_DrawNudgeRight                    = 0x02A1,
    eWDCMD_WW2_ToolsMacro                    = 0x02A2,
    eWDCMD_MailMergeEditHeaderSource         = 0x02A3,
    eWDCMD_MailMerge                         = 0x02A4,
    eWDCMD_MailMergeCheck                    = 0x02A5,
    eWDCMD_MailMergeToDoc                    = 0x02A6,
    eWDCMD_MailMergeToPrinter                = 0x02A7,
    eWDCMD_MailMergeHelper                   = 0x02A8,
    eWDCMD_MailMergeQueryOptions             = 0x02A9,
    eWDCMD_InsertWordArt                     = 0x02AA,
    eWDCMD_InsertEquation                    = 0x02AB,
    eWDCMD_RunPrintManager                   = 0x02AC,
    eWDCMD_FileMacPageSetup                  = 0x02AD,
    eWDCMD_FileConfirmConversions            = 0x02AF,
    eWDCMD_HelpContents                      = 0x02B0,
    eWDCMD_WW2_InsertSymbol                  = 0x02B5,
    eWDCMD_FileClosePicture                  = 0x02B6,
    eWDCMD_WW2_InsertIndex                   = 0x02B7,
    eWDCMD_DrawResetWordPicture              = 0x02B8,
    eWDCMD_WW2_FormatBordersAndShading       = 0x02B9,
    eWDCMD_OpenOrCloseUpPara                 = 0x02BA,
    eWDCMD_DrawNudgeUpPixel                  = 0x02BC,
    eWDCMD_DrawNudgeDownPixel                = 0x02BD,
    eWDCMD_DrawNudgeLeftPixel                = 0x02BE,
    eWDCMD_DrawNudgeRightPixel               = 0x02BF,
    eWDCMD_ToolsHyphenationManual            = 0x02C0,
    eWDCMD_ClearFormField                    = 0x02C2,
    eWDCMD_InsertSectionBreak                = 0x02C3,
    eWDCMD_DrawUnselect                      = 0x02C4,
    eWDCMD_DrawSelectNext                    = 0x02C5,
    eWDCMD_DrawSelectPrevious                = 0x02C6,
    eWDCMD_MicrosoftSystemInfo               = 0x02C7,
    eWDCMD_ToolsCustomizeToolbar             = 0x02CC,
    eWDCMD_ListCommands                      = 0x02D3,
    eWDCMD_EditCreatePublisher               = 0x02DC,
    eWDCMD_EditSubscribeTo                   = 0x02DD,
    eWDCMD_EditPubOrSubOptions               = 0x02DE,
    eWDCMD_EditPublishOptions                = 0x02DF,
    eWDCMD_EditSubscribeOptions              = 0x02E0,
    eWDCMD_FileMacCustomPageSetupGX          = 0x02E1,
    eWDCMD_Highlight                         = 0x02E6,
    eWDCMD_FixSpellingChange                 = 0x02E8,
    eWDCMD_FileProperties                    = 0x02EE,
    eWDCMD_EditCopyAsPicture                 = 0x02EF,
    eWDCMD_InsertAddress                     = 0x02F6,
    eWDCMD_NextMisspelling                   = 0x02F7,
    eWDCMD_FilePost                          = 0x02F8,
    eWDCMD_ToolsAutoCorrectExceptions        = 0x02FA,
    eWDCMD_MailHideMessageHeader             = 0x02FB,
    eWDCMD_MailMessageProperties             = 0x02FC,
    eWDCMD_ToolsAutoCorrectCapsLockOff       = 0x02FF,
    eWDCMD_MailMessageReply                  = 0x0300,
    eWDCMD_MailMessageReplyAll               = 0x0301,
    eWDCMD_MailMessageMove                   = 0x0302,
    eWDCMD_MailMessageDelete                 = 0x0303,
    eWDCMD_MailMessagePrevious               = 0x0304,
    eWDCMD_MailMessageNext                   = 0x0305,
    eWDCMD_MailCheckNames                    = 0x0306,
    eWDCMD_MailSelectNames                   = 0x0307,
    eWDCMD_MailMessageForward                = 0x0308,
    eWDCMD_ToolsSpellingRecheckDocument      = 0x0309,
    eWDCMD_MailMergeUseAddressBook           = 0x030B,
    eWDCMD_EditFindHighlight                 = 0x030C,
    eWDCMD_EditReplaceHighlight              = 0x030D,
    eWDCMD_EditFindNotHighlight              = 0x030E,
    eWDCMD_EditReplaceNotHighlight           = 0x030F,
    eWDCMD_2Help                             = 0x031A,
    eWDCMD_HelpMSN                           = 0x031B,
    eWDCMD_CharLeft                          = 0x0FA0,
    eWDCMD_CharRight                         = 0x0FA1,
    eWDCMD_WordLeft                          = 0x0FA2,
    eWDCMD_WordRight                         = 0x0FA3,
    eWDCMD_SentLeft                          = 0x0FA4,
    eWDCMD_SentRight                         = 0x0FA5,
    eWDCMD_ParaUp                            = 0x0FA6,
    eWDCMD_ParaDown                          = 0x0FA7,
    eWDCMD_LineUp                            = 0x0FA8,
    eWDCMD_LineDown                          = 0x0FA9,
    eWDCMD_PageUp                            = 0x0FAA,
    eWDCMD_PageDown                          = 0x0FAB,
    eWDCMD_StartOfLine                       = 0x0FAC,
    eWDCMD_EndOfLine                         = 0x0FAD,
    eWDCMD_StartOfWindow                     = 0x0FAE,
    eWDCMD_EndOfWindow                       = 0x0FAF,
    eWDCMD_StartOfDocument                   = 0x0FB0,
    eWDCMD_EndOfDocument                     = 0x0FB1,
    eWDCMD_CharLeftExtend                    = 0x0FB2,
    eWDCMD_CharRightExtend                   = 0x0FB3,
    eWDCMD_WordLeftExtend                    = 0x0FB4,
    eWDCMD_WordRightExtend                   = 0x0FB5,
    eWDCMD_SentLeftExtend                    = 0x0FB6,
    eWDCMD_SentRightExtend                   = 0x0FB7,
    eWDCMD_ParaUpExtend                      = 0x0FB8,
    eWDCMD_ParaDownExtend                    = 0x0FB9,
    eWDCMD_LineUpExtend                      = 0x0FBA,
    eWDCMD_LineDownExtend                    = 0x0FBB,
    eWDCMD_PageUpExtend                      = 0x0FBC,
    eWDCMD_PageDownExtend                    = 0x0FBD,
    eWDCMD_StartOfLineExtend                 = 0x0FBE,
    eWDCMD_EndOfLineExtend                   = 0x0FBF,
    eWDCMD_StartOfWindowExtend               = 0x0FC0,
    eWDCMD_EndOfWindowExtend                 = 0x0FC1,
    eWDCMD_StartOfDocExtend                  = 0x0FC2,
    eWDCMD_EndOfDocExtend                    = 0x0FC3,
    eWDCMD_File1                             = 0x0FC5,
    eWDCMD_File2                             = 0x0FC6,
    eWDCMD_File3                             = 0x0FC7,
    eWDCMD_File4                             = 0x0FC8,
    eWDCMD_File5                             = 0x0FC9,
    eWDCMD_File6                             = 0x0FCA,
    eWDCMD_File7                             = 0x0FCB,
    eWDCMD_File8                             = 0x0FCC,
    eWDCMD_File9                             = 0x0FCD,
    eWDCMD_MailMergeInsertAsk                = 0x0FCF,
    eWDCMD_MailMergeInsertFillIn             = 0x0FD0,
    eWDCMD_MailMergeInsertIf                 = 0x0FD1,
    eWDCMD_MailMergeInsertMergeRec           = 0x0FD2,
    eWDCMD_MailMergeInsertMergeSeq           = 0x0FD3,
    eWDCMD_MailMergeInsertNext               = 0x0FD4,
    eWDCMD_MailMergeInsertNextIf             = 0x0FD5,
    eWDCMD_MailMergeInsertSet                = 0x0FD6,
    eWDCMD_MailMergeInsertSkipIf             = 0x0FD7,
    eWDCMD_BorderTop                         = 0x0FDE,
    eWDCMD_BorderLeft                        = 0x0FDF,
    eWDCMD_BorderBottom                      = 0x0FE0,
    eWDCMD_BorderRight                       = 0x0FE1,
    eWDCMD_BorderInside                      = 0x0FE2,
    eWDCMD_TipWizard                         = 0x0FE3,
    eWDCMD_ShowMe                            = 0x0FE4,
    eWDCMD_AutomaticChange                   = 0x0FE6,
    eWDCMD_WW2_FileTemplates                 = 0x17A6,
    eWDCMD_FormatFillColor                   = 0x1B8B,
    eWDCMD_FormatLineColor                   = 0x1B8C,
    eWDCMD_FormatLineStyle                   = 0x1B8D,
    eWDCMD_Abs                               = 0x8000,
    eWDCMD_Sgn                               = 0x8001,
    eWDCMD_Int                               = 0x8002,
    eWDCMD_Len                               = 0x8003,
    eWDCMD_Asc                               = 0x8004,
    eWDCMD_ChrS                              = 0x8005,
    eWDCMD_Val                               = 0x8006,
    eWDCMD_StrS                              = 0x8007,
    eWDCMD_LeftS                             = 0x8008,
    eWDCMD_RightS                            = 0x8009,
    eWDCMD_MidS                              = 0x800A,
    eWDCMD_StringS                           = 0x800B,
    eWDCMD_DateS                             = 0x800C,
    eWDCMD_TimeS                             = 0x800D,
    eWDCMD_Rnd                               = 0x800E,
    eWDCMD_InStr                             = 0x800F,
    eWDCMD_2ShowAll                          = 0x8010,
    eWDCMD_2ColumnSelect                     = 0x8011,
    eWDCMD_Insert                            = 0x8012,
    eWDCMD_InsertPara                        = 0x8013,
    eWDCMD_WW1_InsertPara                    = 0x8014,
    eWDCMD_SelectionS                        = 0x8015,
    eWDCMD_GetBookmarkS                      = 0x8016,
    eWDCMD_CmpBookmarks                      = 0x8017,
    eWDCMD_CopyBookmark                      = 0x8018,
    eWDCMD_SetStartOfBookmark                = 0x8019,
    eWDCMD_SetEndOfBookmark                  = 0x801A,
    eWDCMD_ExistingBookmark                  = 0x801B,
    eWDCMD_EmptyBookmark                     = 0x801C,
    eWDCMD_CountBookmarks                    = 0x801D,
    eWDCMD_CountMergeFields                  = 0x801E,
    eWDCMD_BookmarkNameS                     = 0x801F,
    eWDCMD_MergeFieldNameS                   = 0x8020,
    eWDCMD_CountStyles                       = 0x8021,
    eWDCMD_StyleNameS                        = 0x8022,
    eWDCMD_IsDocumentDirty                   = 0x8023,
    eWDCMD_SetDocumentDirty                  = 0x8024,
    eWDCMD_FileNameS                         = 0x8025,
    eWDCMD_CountFiles                        = 0x8026,
    eWDCMD_GetAutoTextS                      = 0x8027,
    eWDCMD_CountAutoTextEntries              = 0x8028,
    eWDCMD_AutoTextNameS                     = 0x8029,
    eWDCMD_SetAutoText                       = 0x802A,
    eWDCMD_MsgBox                            = 0x802B,
    eWDCMD_Beep                              = 0x802C,
    eWDCMD_Shell                             = 0x802D,
    eWDCMD_2ResetChar                        = 0x802E,
    eWDCMD_2ResetPara                        = 0x802F,
    eWDCMD_TabType                           = 0x8030,
    eWDCMD_TabLeaderS                        = 0x8031,
    eWDCMD_2DocMove                          = 0x8032,
    eWDCMD_2DocSize                          = 0x8033,
    eWDCMD_VLine                             = 0x8034,
    eWDCMD_HLine                             = 0x8035,
    eWDCMD_VPage                             = 0x8036,
    eWDCMD_HPage                             = 0x8037,
    eWDCMD_VScroll                           = 0x8038,
    eWDCMD_HScroll                           = 0x8039,
    eWDCMD_CountWindows                      = 0x803A,
    eWDCMD_WindowNameS                       = 0x803B,
    eWDCMD_WindowPane                        = 0x803C,
    eWDCMD_2DocSplit                         = 0x803D,
    eWDCMD_Window                            = 0x803E,
    eWDCMD_2AppSize                          = 0x803F,
    eWDCMD_2AppMove                          = 0x8040,
    eWDCMD_2AppMinimize                      = 0x8041,
    eWDCMD_2AppMaximize                      = 0x8042,
    eWDCMD_2AppRestore                       = 0x8043,
    eWDCMD_2DocMaximize                      = 0x8044,
    eWDCMD_GetProfileStringS                 = 0x8045,
    eWDCMD_SetProfileString                  = 0x8046,
    eWDCMD_2CharColor                        = 0x8047,
    eWDCMD_2Bold                             = 0x8048,
    eWDCMD_2Italic                           = 0x8049,
    eWDCMD_2SmallCaps                        = 0x804A,
    eWDCMD_2AllCaps                          = 0x804B,
    eWDCMD_2Strikethrough                    = 0x804C,
    eWDCMD_2Hidden                           = 0x804D,
    eWDCMD_2Underline                        = 0x804E,
    eWDCMD_2DoubleUnderline                  = 0x804F,
    eWDCMD_2WordUnderline                    = 0x8050,
    eWDCMD_2Superscript                      = 0x8051,
    eWDCMD_2Subscript                        = 0x8052,
    eWDCMD_2CenterPara                       = 0x8053,
    eWDCMD_2LeftPara                         = 0x8054,
    eWDCMD_2RightPara                        = 0x8055,
    eWDCMD_2JustifyPara                      = 0x8056,
    eWDCMD_2SpacePara1                       = 0x8057,
    eWDCMD_2SpacePara15                      = 0x8058,
    eWDCMD_2SpacePara2                       = 0x8059,
    eWDCMD_2OpenUpPara                       = 0x805A,
    eWDCMD_2CloseUpPara                      = 0x805B,
    eWDCMD_DDEInitiate                       = 0x805C,
    eWDCMD_DDETerminate                      = 0x805D,
    eWDCMD_DDETerminateAll                   = 0x805E,
    eWDCMD_DDEExecute                        = 0x805F,
    eWDCMD_DDEPoke                           = 0x8060,
    eWDCMD_DDERequestS                       = 0x8061,
    eWDCMD_Activate                          = 0x8062,
    eWDCMD_AppActivate                       = 0x8063,
    eWDCMD_SendKeys                          = 0x8064,
    eWDCMD_StyleDescS                        = 0x8065,
    eWDCMD_2ParaKeepLinesTogether            = 0x8066,
    eWDCMD_2ParaKeepWithNext                 = 0x8067,
    eWDCMD_2ParaPageBreakBefore              = 0x8069,
    eWDCMD_2ParaWidowOrphanControl           = 0x806A,
    eWDCMD_2ViewFootnotes                    = 0x806B,
    eWDCMD_2ViewAnnotations                  = 0x806C,
    eWDCMD_2ViewFieldCodes                   = 0x806D,
    eWDCMD_2ViewDraft                        = 0x806E,
    eWDCMD_2ViewStatusBar                    = 0x806F,
    eWDCMD_2ViewRuler                        = 0x8070,
    eWDCMD_ViewRibbon                        = 0x8071,
    eWDCMD_ViewToolbar                       = 0x8072,
    eWDCMD_2ViewPage                         = 0x8073,
    eWDCMD_2ViewOutline                      = 0x8074,
    eWDCMD_2ViewNormal                       = 0x8075,
    eWDCMD_ViewMenus                         = 0x8076,
    eWDCMD_2TableGridlines                   = 0x8077,
    eWDCMD_2OutlineShowFirstLine             = 0x8078,
    eWDCMD_2Overtype                         = 0x8079,
    eWDCMD_FontS                             = 0x807A,
    eWDCMD_CountFonts                        = 0x807B,
    eWDCMD_2Font                             = 0x807C,
    eWDCMD_2FontSize                         = 0x807D,
    eWDCMD_LanguageS                         = 0x807E,
    eWDCMD_CountLanguages                    = 0x807F,
    eWDCMD_Language                          = 0x8080,
    eWDCMD_WW6_EditClear                     = 0x8081,
    eWDCMD_FileList                          = 0x8082,
    eWDCMD_2File1                            = 0x8083,
    eWDCMD_2File2                            = 0x8084,
    eWDCMD_2File3                            = 0x8085,
    eWDCMD_2File4                            = 0x8086,
    eWDCMD_2File5                            = 0x8087,
    eWDCMD_2File6                            = 0x8088,
    eWDCMD_2File7                            = 0x8089,
    eWDCMD_2File8                            = 0x808A,
    eWDCMD_2File9                            = 0x808B,
    eWDCMD_wdToolsGetSpelling                = 0x8090,
    eWDCMD_wdToolsGetSynonyms                = 0x8091,
    eWDCMD_2NextPage                         = 0x8094,
    eWDCMD_2PrevPage                         = 0x8095,
    eWDCMD_2NextObject                       = 0x8096,
    eWDCMD_2PrevObject                       = 0x8097,
    eWDCMD_2ExtendSelection                  = 0x8098,
    eWDCMD_ExtendMode                        = 0x8099,
    eWDCMD_SelType                           = 0x809A,
    eWDCMD_OutlineLevel                      = 0x809B,
    eWDCMD_NextTab                           = 0x809C,
    eWDCMD_PrevTab                           = 0x809D,
    eWDCMD_DisableInput                      = 0x809E,
    eWDCMD_2DocClose                         = 0x809F,
    eWDCMD_2FileClose                        = 0x80A0,
    eWDCMD_FilesS                            = 0x80A1,
    eWDCMD_2FileExit                         = 0x80A2,
    eWDCMD_2FileSaveAll                      = 0x80A3,
    eWDCMD_2FilePrintPreview                 = 0x80A4,
    eWDCMD_FilePrintPreviewPages             = 0x80A6,
    eWDCMD_InputS                            = 0x80A7,
    eWDCMD_Seek                              = 0x80A8,
    eWDCMD_Eof                               = 0x80A9,
    eWDCMD_Lof                               = 0x80AA,
    eWDCMD_Kill                              = 0x80AB,
    eWDCMD_ChDir                             = 0x80AC,
    eWDCMD_MkDir                             = 0x80AD,
    eWDCMD_RmDir                             = 0x80AE,
    eWDCMD_UCaseS                            = 0x80AF,
    eWDCMD_LCaseS                            = 0x80B0,
    eWDCMD_InputBoxS                         = 0x80B1,
    eWDCMD_WW2_RenameMenu                    = 0x80B2,
    eWDCMD_OnTime                            = 0x80B3,
    eWDCMD_2ChangeCase                       = 0x80B4,
    eWDCMD_AppInfoS                          = 0x80B5,
    eWDCMD_SelInfo                           = 0x80B6,
    eWDCMD_CountMacros                       = 0x80B7,
    eWDCMD_MacroNameS                        = 0x80B8,
    eWDCMD_CountFoundFiles                   = 0x80B9,
    eWDCMD_FoundFileNameS                    = 0x80BA,
    eWDCMD_WW2_CountMenuItems                = 0x80BB,
    eWDCMD_WW2_MenuMacroS                    = 0x80BC,
    eWDCMD_WW2_MenuTextS                     = 0x80BD,
    eWDCMD_MacroDescS                        = 0x80BE,
    eWDCMD_CountKeys                         = 0x80BF,
    eWDCMD_KeyCode                           = 0x80C0,
    eWDCMD_KeyMacroS                         = 0x80C1,
    eWDCMD_MacroCopy                         = 0x80C2,
    eWDCMD_IsExecuteOnly                     = 0x80C3,
    eWDCMD_CommandValid                      = 0x80C4,
    eWDCMD_WW2_GetToolButton                 = 0x80C5,
    eWDCMD_WW2_GetToolMacroS                 = 0x80C6,
    eWDCMD_OKButton                          = 0x80C7,
    eWDCMD_CancelButton                      = 0x80C8,
    eWDCMD_Text                              = 0x80C9,
    eWDCMD_GroupBox                          = 0x80CA,
    eWDCMD_OptionButton                      = 0x80CB,
    eWDCMD_PushButton                        = 0x80CC,
    eWDCMD_2NextField                        = 0x80CD,
    eWDCMD_2PrevField                        = 0x80CE,
    eWDCMD_2NextCell                         = 0x80CF,
    eWDCMD_2PrevCell                         = 0x80D0,
    eWDCMD_2StartOfRow                       = 0x80D1,
    eWDCMD_2EndOfRow                         = 0x80D2,
    eWDCMD_2StartOfColumn                    = 0x80D3,
    eWDCMD_2EndOfColumn                      = 0x80D4,
    eWDCMD_ExitWindows                       = 0x80D5,
    eWDCMD_DisableAutoMacros                 = 0x80D6,
    eWDCMD_EditFindFound                     = 0x80D7,
    eWDCMD_CheckBox                          = 0x80D8,
    eWDCMD_TextBox                           = 0x80D9,
    eWDCMD_ListBox                           = 0x80DA,
    eWDCMD_OptionGroup                       = 0x80DB,
    eWDCMD_ComboBox                          = 0x80DC,
    eWDCMD_2ToolsCalculate                   = 0x80DD,
    eWDCMD_2WindowList                       = 0x80DE,
    eWDCMD_Window1                           = 0x80DF,
    eWDCMD_Window2                           = 0x80E0,
    eWDCMD_Window3                           = 0x80E1,
    eWDCMD_Window4                           = 0x80E2,
    eWDCMD_Window5                           = 0x80E3,
    eWDCMD_Window6                           = 0x80E4,
    eWDCMD_Window7                           = 0x80E5,
    eWDCMD_Window8                           = 0x80E6,
    eWDCMD_Window9                           = 0x80E7,
    eWDCMD_CountDirectories                  = 0x80E8,
    eWDCMD_GetDirectoryS                     = 0x80E9,
    eWDCMD_LTrimS                            = 0x80EA,
    eWDCMD_RTrimS                            = 0x80EB,
    eWDCMD_EnvironS                          = 0x80EE,
    eWDCMD_WaitCursor                        = 0x80EF,
    eWDCMD_DateSerial                        = 0x80F0,
    eWDCMD_DateValue                         = 0x80F1,
    eWDCMD_Day                               = 0x80F2,
    eWDCMD_Days360                           = 0x80F3,
    eWDCMD_Hour                              = 0x80F4,
    eWDCMD_Minute                            = 0x80F5,
    eWDCMD_Month                             = 0x80F6,
    eWDCMD_Now                               = 0x80F7,
    eWDCMD_Weekday                           = 0x80F8,
    eWDCMD_Year                              = 0x80F9,
    eWDCMD_DocWindowHeight                   = 0x80FA,
    eWDCMD_DocWindowWidth                    = 0x80FB,
    eWDCMD_DOSToWinS                         = 0x80FC,
    eWDCMD_WinToDOSS                         = 0x80FD,
    eWDCMD_TimeSerial                        = 0x80FE,
    eWDCMD_Second                            = 0x80FF,
    eWDCMD_TimeValue                         = 0x8100,
    eWDCMD_Today                             = 0x8101,
    eWDCMD_ShowAnnotationBy                  = 0x8102,
    eWDCMD_SetAttr                           = 0x8103,
    eWDCMD_2DocMinimize                      = 0x8105,
    eWDCMD_GetSystemInfo                     = 0x8106,
    eWDCMD_AppClose                          = 0x8107,
    eWDCMD_AppCount                          = 0x8108,
    eWDCMD_AppGetNames                       = 0x8109,
    eWDCMD_AppHide                           = 0x810A,
    eWDCMD_AppIsRunning                      = 0x810B,
    eWDCMD_GetSystemInfoS                    = 0x810C,
    eWDCMD_GetPrivateProfileStringS          = 0x810D,
    eWDCMD_SetPrivateProfileString           = 0x810E,
    eWDCMD_GetAttr                           = 0x810F,
    eWDCMD_AppSendMessage                    = 0x8110,
    eWDCMD_ScreenUpdating                    = 0x8111,
    eWDCMD_AppWindowPosTop                   = 0x8112,
    eWDCMD_2Style                            = 0x8113,
    eWDCMD_MailMergeDataSourceS              = 0x8114,
    eWDCMD_MailMergeState                    = 0x8115,
    eWDCMD_SelectCurWord                     = 0x8116,
    eWDCMD_SelectCurSentence                 = 0x8117,
    eWDCMD_IsTemplateDirty                   = 0x8118,
    eWDCMD_SetTemplateDirty                  = 0x8119,
    eWDCMD_2ToolsAutoCorrectSmartQuotes      = 0x811A,
    eWDCMD_DlgEnable                         = 0x811B,
    eWDCMD_DlgUpdateFilePreview              = 0x811C,
    eWDCMD_DlgVisible                        = 0x811D,
    eWDCMD_DlgValue                          = 0x811E,
    eWDCMD_DlgTextS                          = 0x811F,
    eWDCMD_DlgFocusS                         = 0x8120,
    eWDCMD_AppShow                           = 0x8121,
    eWDCMD_DlgListBoxArray                   = 0x8122,
    eWDCMD_DlgControlId                      = 0x8123,
    eWDCMD_2ViewEndnoteArea                  = 0x8124,
    eWDCMD_Picture                           = 0x8125,
    eWDCMD_DlgSetPicture                     = 0x8126,
    eWDCMD_WW2_ChangeCase                    = 0x8127,
    eWDCMD_MailMergeMainDocumentType         = 0x8128,
    eWDCMD_WW2_FilesS                        = 0x8131,
    eWDCMD_CountToolsGrammarStatistics       = 0x8132,
    eWDCMD_2DottedUnderline                  = 0x8133,
    eWDCMD_ToolsGrammarStatisticsArray       = 0x8134,
    eWDCMD_FilePreview                       = 0x8135,
    eWDCMD_DlgFilePreviewS                   = 0x8136,
    eWDCMD_DlgText                           = 0x8137,
    eWDCMD_DlgFocus                          = 0x8138,
    eWDCMD_2MailMergeGoToRecord              = 0x8139,
    eWDCMD_2BorderLineStyle                  = 0x813B,
    eWDCMD_2ShadingPattern                   = 0x813C,
    eWDCMD_MenuItemTextS                     = 0x813D,
    eWDCMD_MenuItemMacroS                    = 0x813E,
    eWDCMD_CountMenus                        = 0x813F,
    eWDCMD_MenuTextS                         = 0x8140,
    eWDCMD_CountMenuItems                    = 0x8141,
    eWDCMD_AppWindowPosLeft                  = 0x8142,
    eWDCMD_AppWindowHeight                   = 0x8143,
    eWDCMD_AppWindowWidth                    = 0x8144,
    eWDCMD_DocWindowPosTop                   = 0x8145,
    eWDCMD_DocWindowPosLeft                  = 0x8146,
    eWDCMD_Stop                              = 0x8147,
    eWDCMD_DropListBox                       = 0x8148,
    eWDCMD_RenameMenu                        = 0x8149,
    eWDCMD_2FileCloseAll                     = 0x814A,
    eWDCMD_SortArray                         = 0x814B,
    eWDCMD_SetDocumentVar                    = 0x814C,
    eWDCMD_GetDocumentVarS                   = 0x814D,
    eWDCMD_AnnotationRefFromSelS             = 0x814E,
    eWDCMD_GetFormResultS                    = 0x814F,
    eWDCMD_SetFormResult                     = 0x8150,
    eWDCMD_EnableFormField                   = 0x8151,
    eWDCMD_IsMacro                           = 0x8152,
    eWDCMD_FileNameFromWindowS               = 0x8153,
    eWDCMD_MacroNameFromWindowS              = 0x8154,
    eWDCMD_GetFieldDataS                     = 0x8155,
    eWDCMD_PutFieldData                      = 0x8156,
    eWDCMD_2MailMergeConvertChevrons         = 0x8157,
    eWDCMD_2MailMergeAskToConvertChevrons    = 0x8158,
    eWDCMD_2AutoMarkIndexEntries             = 0x815A,
    eWDCMD_MoveToolbar                       = 0x815B,
    eWDCMD_SizeToolbar                       = 0x815C,
    eWDCMD_DrawSetRange                      = 0x815D,
    eWDCMD_MountVolume                       = 0x815E,
    eWDCMD_DrawClearRange                    = 0x815F,
    eWDCMD_DrawCount                         = 0x8160,
    eWDCMD_DrawSelect                        = 0x8161,
    eWDCMD_DrawExtendSelect                  = 0x8162,
    eWDCMD_DrawSetInsertToTextbox            = 0x8163,
    eWDCMD_DrawSetInsertToAnchor             = 0x8164,
    eWDCMD_DrawGetType                       = 0x8165,
    eWDCMD_DrawCountPolyPoints               = 0x8166,
    eWDCMD_DrawGetPolyPoints                 = 0x8167,
    eWDCMD_DrawSetPolyPoints                 = 0x8168,
    eWDCMD_DrawGetCalloutTextbox             = 0x8169,
    eWDCMD_DrawSetCalloutTextbox             = 0x816A,
    eWDCMD_2Magnifier                        = 0x816B,
    eWDCMD_MacScript                         = 0x816C,
    eWDCMD_MacScriptS                        = 0x816D,
    eWDCMD_MacIDS                            = 0x816E,
    eWDCMD_GetSelStartPos                    = 0x816F,
    eWDCMD_GetSelEndPos                      = 0x8170,
    eWDCMD_SetSelRange                       = 0x8171,
    eWDCMD_GetTextS                          = 0x8172,
    eWDCMD_MoveButton                        = 0x8173,
    eWDCMD_DeleteButton                      = 0x8174,
    eWDCMD_AddButton                         = 0x8175,
    eWDCMD_wdAddCommand                      = 0x8176,
    eWDCMD_DeleteAddIn                       = 0x8177,
    eWDCMD_AddAddIn                          = 0x8178,
    eWDCMD_GetAddInNameS                     = 0x8179,
    eWDCMD_2FormatBulletDefault              = 0x817A,
    eWDCMD_2FormatNumberDefault              = 0x817B,
    eWDCMD_2ResetButtonImage                 = 0x817C,
    eWDCMD_DlgFilePreview                    = 0x817D,
    eWDCMD_2SkipNumbering                    = 0x817E,
    eWDCMD_wdGetInst                         = 0x817F,
    eWDCMD_GetAddInId                        = 0x8180,
    eWDCMD_CountAddIns                       = 0x8181,
    eWDCMD_ClearAddIns                       = 0x8182,
    eWDCMD_AddInState                        = 0x8183,
    eWDCMD_ToolsRevisionType                 = 0x8184,
    eWDCMD_ToolsRevisionAuthorS              = 0x8185,
    eWDCMD_ToolsRevisionDate                 = 0x8186,
    eWDCMD_2MailMergeViewData                = 0x8187,
    eWDCMD_AddDropDownItem                   = 0x8188,
    eWDCMD_RemoveDropDownItem                = 0x8189,
    eWDCMD_ToolsRevisionDateS                = 0x818A,
    eWDCMD_2TableHeadings                    = 0x818B,
    eWDCMD_DefaultDirS                       = 0x818C,
    eWDCMD_FileNameInfoS                     = 0x818D,
    eWDCMD_MacroFileNameS                    = 0x818E,
    eWDCMD_2ViewHeader                       = 0x818F,
    eWDCMD_2ViewFooter                       = 0x8190,
    eWDCMD_2PasteButtonImage                 = 0x8191,
    eWDCMD_2CopyButtonImage                  = 0x8192,
    eWDCMD_2EditButtonImage                  = 0x8194,
    eWDCMD_CountToolbars                     = 0x8195,
    eWDCMD_ToolbarNameS                      = 0x8196,
    eWDCMD_ChDefaultDir                      = 0x8198,
    eWDCMD_2EditUndo                         = 0x8199,
    eWDCMD_2EditRedo                         = 0x819A,
    eWDCMD_2ViewMasterDocument               = 0x819B,
    eWDCMD_2ToolsAutoCorrectReplaceText      = 0x819C,
    eWDCMD_2ToolsAutoCorrectInitialCaps      = 0x819D,
    eWDCMD_2ToolsAutoCorrectSentenceCaps     = 0x819E,
    eWDCMD_2ToolsAutoCorrectDays             = 0x819F,
    eWDCMD_GetAutoCorrectS                   = 0x81A0,
    eWDCMD_2ViewFootnoteArea                 = 0x81A1,
    eWDCMD_FileQuit                          = 0x81A2,
    eWDCMD_ConverterLookup                   = 0x81A3,
    eWDCMD_2FileConfirmConversions           = 0x81A4,
    eWDCMD_GetMergeFieldS                    = 0x81A5,
    eWDCMD_ConverterS                        = 0x81A6,
    eWDCMD_MailMergeFoundRecord              = 0x81A7,
    eWDCMD_CountDocumentVars                 = 0x81A8,
    eWDCMD_GetDocumentVarNameS               = 0x81A9,
    eWDCMD_wdPrint                           = 0x81AA,
    eWDCMD_CleanStringS                      = 0x81AB,
    eWDCMD_PathFromWinPathS                  = 0x81AC,
    eWDCMD_PathFromMacPathS                  = 0x81AD,
    eWDCMD_2LockDocument                     = 0x81D0,
    eWDCMD_GoToNextSubdocument               = 0x81D1,
    eWDCMD_GoToPreviousSubdocument           = 0x81D2,
    eWDCMD_SelectionFileNameS                = 0x81D3,
    eWDCMD_2SymbolFont                       = 0x81D4,
    eWDCMD_RemoveAllDropDownItems            = 0x81D5,
    eWDCMD_2FormShading                      = 0x81D6,
    eWDCMD_GetFormResult                     = 0x81D7,
    eWDCMD_ToolbarState                      = 0x81D8,
    eWDCMD_CountToolbarButtons               = 0x81D9,
    eWDCMD_ToolbarButtonMacroS               = 0x81DA,
    eWDCMD_WW2_Insert                        = 0x81DB,
    eWDCMD_AtEndOfDocument                   = 0x81DC,
    eWDCMD_AtStartOfDocument                 = 0x81DD,
    eWDCMD_WW2_KeyCode                       = 0x81DE,
    eWDCMD_FieldSeparatorS                   = 0x81E0,
    eWDCMD_wdGetHwnd                         = 0x81E1,
    eWDCMD_WW2CallingConvention              = 0x81E2,
    eWDCMD_wdSetTimer                        = 0x81E3,
    eWDCMD_AOCEAddRecipient                  = 0x81E4,
    eWDCMD_AOCECountRecipients               = 0x81E5,
    eWDCMD_AOCEGetRecipientS                 = 0x81E6,
    eWDCMD_AOCEGetSenderS                    = 0x81E7,
    eWDCMD_AOCEGetSubjectS                   = 0x81E9,
    eWDCMD_AOCESetSubject                    = 0x81EA,
    eWDCMD_AOCESendMail                      = 0x81EB,
    eWDCMD_AOCEAuthenticateUser              = 0x81EC,
    eWDCMD_AOCEClearMailerField              = 0x81ED,
    eWDCMD_wdOpenWindow                      = 0x81EE,
    eWDCMD_wdCloseWindow                     = 0x81EF,
    eWDCMD_wdCaptureKeyDown                  = 0x81F0,
    eWDCMD_wdReleaseKeyDown                  = 0x81F1,
    eWDCMD_Shadow                            = 0x81F2,
    eWDCMD_Outline                           = 0x81F3,
    eWDCMD_FileTypeS                         = 0x81F4,
    eWDCMD_FileCreatorS                      = 0x81F5,
    eWDCMD_SetFileCreatorAndType             = 0x81F6,
    eWDCMD_DlgStoreValues                    = 0x81F7,
    eWDCMD_DlgLoadValues                     = 0x81F8,
    eWDCMD_DocumentHasMisspellings           = 0x81F9,
    eWDCMD_GetAddressS                       = 0x81FA,
    eWDCMD_CountDocumentProperties           = 0x81FB,
    eWDCMD_GetDocumentPropertyS              = 0x81FC,
    eWDCMD_GetDocumentProperty               = 0x81FD,
    eWDCMD_SetDocumentProperty               = 0x81FE,
    eWDCMD_SetDocumentPropertyLink           = 0x81FF,
    eWDCMD_DeleteDocumentProperty            = 0x8200,
    eWDCMD_DocumentPropertyNameS             = 0x8201,
    eWDCMD_IsDocumentPropertyReadOnly        = 0x8202,
    eWDCMD_IsCustomDocumentProperty          = 0x8203,
    eWDCMD_DocumentPropertyExists            = 0x8204,
    eWDCMD_DocumentPropertyType              = 0x8205,
    eWDCMD_2FilePost                         = 0x8206,
    eWDCMD_AddAddress                        = 0x8208,
    eWDCMD_2ToolsAutoCorrectCapsLockOff      = 0x8209,
    eWDCMD_CountAutoCorrectExceptions        = 0x820A,
    eWDCMD_GetAutoCorrectExceptionS          = 0x820B,
    eWDCMD_IsAutoCorrectException            = 0x820C,
    eWDCMD_SpellChecked                      = 0x820E,
    eWDCMD_2CharLeft                         = 0xC000,
    eWDCMD_2CharRight                        = 0xC001,
    eWDCMD_2WordLeft                         = 0xC002,
    eWDCMD_2WordRight                        = 0xC003,
    eWDCMD_2SentLeft                         = 0xC004,
    eWDCMD_2SentRight                        = 0xC005,
    eWDCMD_2ParaUp                           = 0xC006,
    eWDCMD_2ParaDown                         = 0xC007,
    eWDCMD_2LineUp                           = 0xC008,
    eWDCMD_2LineDown                         = 0xC009,
    eWDCMD_2PageUp                           = 0xC00A,
    eWDCMD_2PageDown                         = 0xC00B,
    eWDCMD_2StartOfLine                      = 0xC00C,
    eWDCMD_2EndOfLine                        = 0xC00D,
    eWDCMD_2StartOfWindow                    = 0xC00E,
    eWDCMD_2EndOfWindow                      = 0xC00F,
    eWDCMD_2StartOfDocument                  = 0xC010,
    eWDCMD_2EndOfDocument                    = 0xC011,
    eWDCMD_2EditClear                        = 0xC012,
    eWDCMD_2BorderOutside                    = 0xC018,
    eWDCMD_2BorderNone                       = 0xC019,
    eWDCMD_2BorderTop                        = 0xC024,
    eWDCMD_2BorderLeft                       = 0xC025,
    eWDCMD_2BorderBottom                     = 0xC026,
    eWDCMD_2BorderRight                      = 0xC027,
    eWDCMD_2BorderInside                     = 0xC028,
    eWDCMD_HighlightColor                    = 0xC029,
    eWDCMD_DocumentProtection                = 0xC02B,
    eWDCMD_BatchMode                         = 0xE008,

// Far East Only Commands

    eWDCMD_FormatTextFlow                    = 0x0234,
    eWDCMD_ToolsOptionsTypography            = 0x02E3,
    eWDCMD_BytePos                           = 0x81AE,
    eWDCMD_ChangeByte                        = 0x7E42,
    eWDCMD_ChangeKana                        = 0x81BF,
    eWDCMD_DistributePara                    = 0x81BC,
    eWDCMD_DrawVerticalTextbox               = 0x02E2,
    eWDCMD_EditUpdateIMEDic                  = 0x027E,
    eWDCMD_HanCharSetS                       = 0x81AF,
    eWDCMD_HelpIchitaroHelp                  = 0x02D8,
    eWDCMD_IMEControl                        = 0x81B8,
    eWDCMD_IndentChar                        = 0x02CF,
    eWDCMD_IndentFirstChar                   = 0x02D1,
    eWDCMD_InputBS                           = 0x81B0,
    eWDCMD_InStrB                            = 0x7E4F,
    eWDCMD_LeftBS                            = 0x81B2,
    eWDCMD_LenB                              = 0x81B3,
    eWDCMD_MidBS                             = 0x81B4,
    eWDCMD_RightBS                           = 0x81B5,
    eWDCMD_RomanCharSetS                     = 0x81B6,
    eWDCMD_StrConvS                          = 0x81B7,
    eWDCMD_UnIndentChar                      = 0x02D0,
    eWDCMD_UnIndentFirstChar                 = 0x02D2,
    eWDCMD_ViewGridlines                     = 0x81BD,
} EWDCMD_T, FAR *LPEWDCMD;

// WordBasic dialog fields

typedef enum tagEWDDLG
{
    eWDDLG_Name                              = 0x0001,
    eWDDLG_KeyCode                           = 0x0002,
    eWDDLG_Context                           = 0x0003,
    eWDDLG_ResetAll                          = 0x0004,
    eWDDLG_Menu                              = 0x0007,
    eWDDLG_MenuText                          = 0x0008,
    eWDDLG_APPUSERNAME                       = 0x0009,
    eWDDLG_APPORGANIZATION                   = 0x000A,
    eWDDLG_Delete                            = 0x000B,
    eWDDLG_SortBy                            = 0x000C,
    eWDDLG_SavedBy                           = 0x0012,
    eWDDLG_DateCreatedFrom                   = 0x0014,
    eWDDLG_DateCreatedTo                     = 0x0015,
    eWDDLG_DateSavedFrom                     = 0x0016,
    eWDDLG_DateSavedTo                       = 0x0017,
    eWDDLG_APPNAME                           = 0x0019,
    eWDDLG_ButtonFieldClicks                 = 0x0020,
    eWDDLG_Font                              = 0x0021,
    eWDDLG_Points                            = 0x0022,
    eWDDLG_Color                             = 0x0023,
    eWDDLG_Bold                              = 0x0024,
    eWDDLG_Italic                            = 0x0025,
    eWDDLG_Hidden                            = 0x0027,
    eWDDLG_Underline                         = 0x0028,
    eWDDLG_Outline                           = 0x0029,
    eWDDLG_Position                          = 0x002B,
    eWDDLG_Define                            = 0x002C,
    eWDDLG_Spacing                           = 0x002D,
    eWDDLG_Merge                             = 0x002E,
    eWDDLG_Printer                           = 0x002F,
    eWDDLG_ContSeparator                     = 0x0031,
    eWDDLG_ContNotice                        = 0x0032,
    eWDDLG_AutoSave                          = 0x0034,
    eWDDLG_Units                             = 0x0035,
    eWDDLG_Pagination                        = 0x0036,
    eWDDLG_SummaryPrompt                     = 0x0037,
    eWDDLG_Initials                          = 0x0039,
    eWDDLG_Tabs                              = 0x003A,
    eWDDLG_Spaces                            = 0x003B,
    eWDDLG_Paras                             = 0x003C,
    eWDDLG_Hyphens                           = 0x003D,
    eWDDLG_ShowAll                           = 0x003E,
    eWDDLG_TextBoundaries                    = 0x0041,
    eWDDLG_HScroll                           = 0x0042,
    eWDDLG_VScroll                           = 0x0043,
    eWDDLG_TableGridlines                    = 0x0044,
    eWDDLG_StyleAreaWidth                    = 0x0045,
    eWDDLG_PageWidth                         = 0x0046,
    eWDDLG_PageHeight                        = 0x0047,
    eWDDLG_DefTabs                           = 0x0048,
    eWDDLG_TopMargin                         = 0x0049,
    eWDDLG_BottomMargin                      = 0x004A,
    eWDDLG_LeftMargin                        = 0x004B,
    eWDDLG_RightMargin                       = 0x004C,
    eWDDLG_Gutter                            = 0x004D,
    eWDDLG_FacingPages                       = 0x004E,
    eWDDLG_FootnotesAt                       = 0x004F,
    eWDDLG_StartingNum                       = 0x0050,
    eWDDLG_RestartNum                        = 0x0051,
    eWDDLG_Template                          = 0x0052,
    eWDDLG_WidowControl                      = 0x0053,
    eWDDLG_APPCOPYRIGHT                      = 0x0055,
    eWDDLG_APPSERIALNUMBER                   = 0x0056,
    eWDDLG_RecentFileCount                   = 0x0059,
    eWDDLG_SmallCaps                         = 0x005D,
    eWDDLG_New                               = 0x005E,
    eWDDLG_AddToTemplate                     = 0x005F,
    eWDDLG_Password                          = 0x0060,
    eWDDLG_RecentFiles                       = 0x0061,
    eWDDLG_Title                             = 0x0062,
    eWDDLG_Subject                           = 0x0063,
    eWDDLG_Author                            = 0x0064,
    eWDDLG_Keywords                          = 0x0065,
    eWDDLG_Comments                          = 0x0066,
    eWDDLG_FileName                          = 0x0067,
    eWDDLG_Directory                         = 0x0068,
    eWDDLG_CreateDate                        = 0x0069,
    eWDDLG_LastSavedDate                     = 0x006A,
    eWDDLG_LastSavedBy                       = 0x006B,
    eWDDLG_RevisionNumber                    = 0x006C,
    eWDDLG_EditTime                          = 0x006D,
    eWDDLG_LastPrintedDate                   = 0x006E,
    eWDDLG_NumPages                          = 0x006F,
    eWDDLG_NumWords                          = 0x0070,
    eWDDLG_NumChars                          = 0x0071,
    eWDDLG_Set                               = 0x0073,
    eWDDLG_Rename                            = 0x0074,
    eWDDLG_NewName                           = 0x0075,
    eWDDLG_PrintBarCode                      = 0x0076,
    eWDDLG_SmartQuotes                       = 0x0078,
    eWDDLG_BasedOn                           = 0x007B,
    eWDDLG_NextStyle                         = 0x007C,
    eWDDLG_CountBy                           = 0x007D,
    eWDDLG_Source                            = 0x007F,
    eWDDLG_Reference                         = 0x0080,
    eWDDLG_Insert                            = 0x0085,
    eWDDLG_Destination                       = 0x0086,
    eWDDLG_Type                              = 0x0087,
    eWDDLG_NumFormat                         = 0x0088,
    eWDDLG_HeaderDistance                    = 0x0089,
    eWDDLG_FooterDistance                    = 0x008A,
    eWDDLG_FirstPage                         = 0x008B,
    eWDDLG_OddAndEvenPages                   = 0x008C,
    eWDDLG_HyphenateCaps                     = 0x008D,
    eWDDLG_Confirm                           = 0x008E,
    eWDDLG_HotZone                           = 0x008F,
    eWDDLG_HeadingSeparator                  = 0x0090,
    eWDDLG_Entry                             = 0x0091,
    eWDDLG_Range                             = 0x0092,
    eWDDLG_Field                             = 0x0094,
    eWDDLG_Link                              = 0x0095,
    eWDDLG_Add                               = 0x0098,
    eWDDLG_IgnoreAllCaps                     = 0x009A,
    eWDDLG_NewTemplate                       = 0x009B,
    eWDDLG_ReadOnly                          = 0x009F,
    eWDDLG_Alignment                         = 0x00A0,
    eWDDLG_LeftIndent                        = 0x00A1,
    eWDDLG_RightIndent                       = 0x00A2,
    eWDDLG_FirstIndent                       = 0x00A3,
    eWDDLG_Before                            = 0x00A4,
    eWDDLG_After                             = 0x00A5,
    eWDDLG_LineSpacing                       = 0x00A6,
    eWDDLG_Style                             = 0x00A7,
    eWDDLG_KeepTogether                      = 0x00A8,
    eWDDLG_KeepWithNext                      = 0x00A9,
    eWDDLG_Border                            = 0x00AA,
    eWDDLG_Shading                           = 0x00AB,
    eWDDLG_PageBreak                         = 0x00AC,
    eWDDLG_NoLineNum                         = 0x00AD,
    eWDDLG_ScaleY                            = 0x00B1,
    eWDDLG_ScaleX                            = 0x00B2,
    eWDDLG_CropTop                           = 0x00B3,
    eWDDLG_CropLeft                          = 0x00B4,
    eWDDLG_CropBottom                        = 0x00B5,
    eWDDLG_CropRight                         = 0x00B6,
    eWDDLG_Level                             = 0x00B8,
    eWDDLG_NumCopies                         = 0x00B9,
    eWDDLG_From                              = 0x00BA,
    eWDDLG_To                                = 0x00BB,
    eWDDLG_Reverse                           = 0x00BC,
    eWDDLG_Draft                             = 0x00BD,
    eWDDLG_UpdateFields                      = 0x00BE,
    eWDDLG_Summary                           = 0x00C0,
    eWDDLG_Annotations                       = 0x00C1,
    eWDDLG_ShowHidden                        = 0x00C2,
    eWDDLG_ShowCodes                         = 0x00C3,
    eWDDLG_Fields                            = 0x00C7,
    eWDDLG_NumParas                          = 0x00C8,
    eWDDLG_StartAt                           = 0x00C9,
    eWDDLG_Format                            = 0x00CB,
    eWDDLG_Search                            = 0x00CC,
    eWDDLG_Replace                           = 0x00CD,
    eWDDLG_WholeWord                         = 0x00CE,
    eWDDLG_MatchCase                         = 0x00CF,
    eWDDLG_MarkRevisions                     = 0x00D0,
    eWDDLG_RevisionBar                       = 0x00D1,
    eWDDLG_NewText                           = 0x00D2,
    eWDDLG_AcceptRevisions                   = 0x00D3,
    eWDDLG_UndoRevisions                     = 0x00D4,
    eWDDLG_CreateBackup                      = 0x00D7,
    eWDDLG_LockAnnot                         = 0x00D8,
    eWDDLG_Direction                         = 0x00D9,
    eWDDLG_Columns                           = 0x00DA,
    eWDDLG_ColumnSpacing                     = 0x00DB,
    eWDDLG_ColLine                           = 0x00DC,
    eWDDLG_SectionStart                      = 0x00DD,
    eWDDLG_LineNum                           = 0x00DF,
    eWDDLG_FromText                          = 0x00E0,
    eWDDLG_NumMode                           = 0x00E2,
    eWDDLG_VertAlign                         = 0x00E3,
    eWDDLG_Order                             = 0x00E6,
    eWDDLG_Separator                         = 0x00E7,
    eWDDLG_FieldNum                          = 0x00E8,
    eWDDLG_SortColumn                        = 0x00E9,
    eWDDLG_CaseSensitive                     = 0x00EA,
    eWDDLG_ColumnWidth                       = 0x00EC,
    eWDDLG_AlwaysSuggest                     = 0x00F0,
    eWDDLG_EnvWidth                          = 0x00F4,
    eWDDLG_EnvHeight                         = 0x00F5,
    eWDDLG_Anchors                           = 0x00F7,
    eWDDLG_UseEnvFeeder                      = 0x00F8,
    eWDDLG_PrintFIMA                         = 0x00F9,
    eWDDLG_Align                             = 0x00FB,
    eWDDLG_Leader                            = 0x00FC,
    eWDDLG_Clear                             = 0x00FD,
    eWDDLG_ClearAll                          = 0x00FE,
    eWDDLG_SuggestFromMainDictOnly           = 0x00FF,
    eWDDLG_Paragraphs                        = 0x0100,
    eWDDLG_ResetIgnoreAll                    = 0x0101,
    eWDDLG_AutomaticSpellChecking            = 0x0104,
    eWDDLG_Horizontal                        = 0x0106,
    eWDDLG_Vertical                          = 0x0108,
    eWDDLG_ShiftCells                        = 0x010D,
    eWDDLG_SpaceBetweenCols                  = 0x010F,
    eWDDLG_TopBorder                         = 0x0113,
    eWDDLG_BottomBorder                      = 0x0114,
    eWDDLG_LeftBorder                        = 0x0116,
    eWDDLG_RightBorder                       = 0x0117,
    eWDDLG_ApplyTo                           = 0x0119,
    eWDDLG_NumColumns                        = 0x011A,
    eWDDLG_NumRows                           = 0x011B,
    eWDDLG_InitialColWidth                   = 0x011C,
    eWDDLG_ConvertFrom                       = 0x011D,
    eWDDLG_ConvertTo                         = 0x011E,
    eWDDLG_UsrDlg                            = 0x011F,
    eWDDLG_UpdateLinks                       = 0x012B,
    eWDDLG_Update                            = 0x012E,
    eWDDLG_ReplaceSelection                  = 0x0130,
    eWDDLG_Text                              = 0x0131,
    eWDDLG_AutoUpdate                        = 0x0133,
    eWDDLG_Description                       = 0x0136,
    eWDDLG_Option                            = 0x0138,
    eWDDLG_Setting                           = 0x0139,
    eWDDLG_Strikeout                         = 0x013A,
    eWDDLG_AllCaps                           = 0x013B,
    eWDDLG_DataType                          = 0x013E,
    eWDDLG_PasswordDoc                       = 0x0141,
    eWDDLG_PasswordDot                       = 0x0142,
    eWDDLG_ZoomPercent                       = 0x0143,
    eWDDLG_LineSpacingRule                   = 0x0144,
    eWDDLG_NumRestart                        = 0x0145,
    eWDDLG_Orientation                       = 0x0147,
    eWDDLG_Category                          = 0x0148,
    eWDDLG_ConfirmConversions                = 0x0149,
    eWDDLG_InsForPaste                       = 0x014A,
    eWDDLG_Overtype                          = 0x014B,
    eWDDLG_StatusBar                         = 0x014C,
    eWDDLG_PicturePlaceHolders               = 0x014D,
    eWDDLG_FieldCodes                        = 0x014E,
    eWDDLG_Linebreaks                        = 0x014F,
    eWDDLG_Show                              = 0x0150,
    eWDDLG_UpdateMode                        = 0x0151,
    eWDDLG_Item                              = 0x0155,
    eWDDLG_FastSaves                         = 0x0156,
    eWDDLG_SaveInterval                      = 0x0157,
    eWDDLG_OpenSource                        = 0x0158,
    eWDDLG_UpdateNow                         = 0x0159,
    eWDDLG_KillLink                          = 0x015A,
    eWDDLG_LineColor                         = 0x0161,
    eWDDLG_TopColor                          = 0x0163,
    eWDDLG_LeftColor                         = 0x0164,
    eWDDLG_BottomColor                       = 0x0165,
    eWDDLG_RightColor                        = 0x0166,
    eWDDLG_HorizColor                        = 0x0167,
    eWDDLG_VertColor                         = 0x0168,
    eWDDLG_HorizBorder                       = 0x016F,
    eWDDLG_VertBorder                        = 0x0170,
    eWDDLG_IgnoreMixedDigits                 = 0x0171,
    eWDDLG_PositionHorz                      = 0x0175,
    eWDDLG_PositionVert                      = 0x0178,
    eWDDLG_FixedWidth                        = 0x017C,
    eWDDLG_Wrap                              = 0x017D,
    eWDDLG_DistFromText                      = 0x0180,
    eWDDLG_FixedHeight                       = 0x0182,
    eWDDLG_AutoFit                           = 0x0183,
    eWDDLG_CharNum                           = 0x0184,
    eWDDLG_FullPage                          = 0x0185,
    eWDDLG_View                              = 0x018B,
    eWDDLG_Options                           = 0x0190,
    eWDDLG_Apply                             = 0x0191,
    eWDDLG_Find                              = 0x0194,
    eWDDLG_Path                              = 0x0196,
    eWDDLG_PrevRow                           = 0x0198,
    eWDDLG_NextRow                           = 0x0199,
    eWDDLG_NextColumn                        = 0x019A,
    eWDDLG_PrevColumn                        = 0x019B,
    eWDDLG_Language                          = 0x01A4,
    eWDDLG_Foreground                        = 0x01A7,
    eWDDLG_Background                        = 0x01A8,
    eWDDLG_SearchPath                        = 0x01A9,
    eWDDLG_CustomDict1                       = 0x01AB,
    eWDDLG_CustomDict2                       = 0x01AC,
    eWDDLG_CustomDict3                       = 0x01AD,
    eWDDLG_CustomDict4                       = 0x01AE,
    eWDDLG_Collate                           = 0x01B1,
    eWDDLG_Shadow                            = 0x01B2,
    eWDDLG_PrintToFile                       = 0x01B3,
    eWDDLG_Button                            = 0x01B4,
    eWDDLG_Macro                             = 0x01B6,
    eWDDLG_Reset                             = 0x01B7,
    eWDDLG_Remove                            = 0x01B9,
    eWDDLG_Protect                           = 0x01BA,
    eWDDLG_PositionHorzRel                   = 0x01BC,
    eWDDLG_MoveWithText                      = 0x01BD,
    eWDDLG_DistVertFromText                  = 0x01BE,
    eWDDLG_PositionVertRel                   = 0x01C0,
    eWDDLG_Punctuation                       = 0x01C5,
    eWDDLG_LinkToFile                        = 0x01C6,
    eWDDLG_WidthRule                         = 0x01C7,
    eWDDLG_HeightRule                        = 0x01C8,
    eWDDLG_Default                           = 0x01C9,
    eWDDLG_PaperSize                         = 0x01CB,
    eWDDLG_OtherPages                        = 0x01CC,
    eWDDLG_ApplyPropsTo                      = 0x01CF,
    eWDDLG_FormatOutline                     = 0x01D0,
    eWDDLG_FormatNumber                      = 0x01D1,
    eWDDLG_Application                       = 0x01D3,
    eWDDLG_StartNewCol                       = 0x01D4,
    eWDDLG_ApplyColsTo                       = 0x01D5,
    eWDDLG_Store                             = 0x01D7,
    eWDDLG_Class                             = 0x01DA,
    eWDDLG_Locked                            = 0x01DB,
    eWDDLG_EnvReturn                         = 0x01DC,
    eWDDLG_EnvAddress                        = 0x01DD,
    eWDDLG_Hide                              = 0x01DE,
    eWDDLG_Toolbar                           = 0x01DF,
    eWDDLG_ReplaceAll                        = 0x01E0,
    eWDDLG_Hang                              = 0x01E1,
    eWDDLG_ShowStatistics                    = 0x01E2,
    eWDDLG_ReplaceOne                        = 0x01E3,
    eWDDLG_QueryOptions                      = 0x01E4,
    eWDDLG_DataSource                        = 0x01E7,
    eWDDLG_MainDoc                           = 0x01E8,
    eWDDLG_MergeField                        = 0x01E9,
    eWDDLG_WordField                         = 0x01EA,
    eWDDLG_Address                           = 0x01EB,
    eWDDLG_Indent                            = 0x01ED,
    eWDDLG_DragAndDrop                       = 0x01EF,
    eWDDLG_DateTimePic                       = 0x01F0,
    eWDDLG_CustomZoomPercent                 = 0x01F1,
    eWDDLG_ResetTool                         = 0x01F2,
    eWDDLG_SetSize                           = 0x01F3,
    eWDDLG_SelectedFile                      = 0x01F4,
    eWDDLG_Run                               = 0x01F5,
    eWDDLG_Edit                              = 0x01F6,
    eWDDLG_HeaderSource                      = 0x01F8,
    eWDDLG_DfltTrueType                      = 0x01F9,
    eWDDLG_Tool                              = 0x01FA,
    eWDDLG_HeaderRecord                      = 0x01FB,
    eWDDLG_Prompt                            = 0x01FD,
    eWDDLG_RemoveFrame                       = 0x01FE,
    eWDDLG_EnvPaperSize                      = 0x01FF,
    eWDDLG_EnvOmitReturn                     = 0x0200,
    eWDDLG_InsertAsText                      = 0x0202,
    eWDDLG_EnvFeederInstalled                = 0x0203,
    eWDDLG_PrintEnvLabel                     = 0x0204,
    eWDDLG_AddToDocument                     = 0x0205,
    eWDDLG_RulerStyle                        = 0x0206,
    eWDDLG_MergeRecords                      = 0x0215,
    eWDDLG_Suppression                       = 0x0216,
    eWDDLG_Created                           = 0x0217,
    eWDDLG_LastSaved                         = 0x0218,
    eWDDLG_Revision                          = 0x0219,
    eWDDLG_Time                              = 0x021A,
    eWDDLG_Printed                           = 0x021B,
    eWDDLG_Pages                             = 0x021C,
    eWDDLG_Words                             = 0x021D,
    eWDDLG_Characters                        = 0x021E,
    eWDDLG_MergeField1                       = 0x021F,
    eWDDLG_MergeField2                       = 0x0220,
    eWDDLG_MergeField3                       = 0x0221,
    eWDDLG_MergeField4                       = 0x0222,
    eWDDLG_MergeField5                       = 0x0223,
    eWDDLG_MergeField6                       = 0x0224,
    eWDDLG_CompOp1                           = 0x0225,
    eWDDLG_CompOp2                           = 0x0226,
    eWDDLG_CompOp3                           = 0x0227,
    eWDDLG_CompOp4                           = 0x0228,
    eWDDLG_CompOp5                           = 0x0229,
    eWDDLG_ComparedTo1                       = 0x022A,
    eWDDLG_ComparedTo2                       = 0x022B,
    eWDDLG_ComparedTo3                       = 0x022C,
    eWDDLG_ComparedTo4                       = 0x022D,
    eWDDLG_ComparedTo5                       = 0x022E,
    eWDDLG_ComparedTo6                       = 0x022F,
    eWDDLG_CompOp6                           = 0x0230,
    eWDDLG_RemoveAttachments                 = 0x0231,
    eWDDLG_WPHelp                            = 0x0232,
    eWDDLG_WPDocNavKeys                      = 0x0233,
    eWDDLG_SetDesc                           = 0x0234,
    eWDDLG_SizeX                             = 0x0235,
    eWDDLG_SizeY                             = 0x0236,
    eWDDLG_AndOr1                            = 0x0237,
    eWDDLG_AndOr2                            = 0x0238,
    eWDDLG_AndOr3                            = 0x0239,
    eWDDLG_AndOr4                            = 0x023A,
    eWDDLG_AndOr5                            = 0x023B,
    eWDDLG_CountFootnotes                    = 0x023D,
    eWDDLG_DontSortHdr                       = 0x0248,
    eWDDLG_FieldNum2                         = 0x0249,
    eWDDLG_Type2                             = 0x024A,
    eWDDLG_FieldNum3                         = 0x024B,
    eWDDLG_Type3                             = 0x024C,
    eWDDLG_WPCommand                         = 0x0250,
    eWDDLG_Window                            = 0x0251,
    eWDDLG_FindNext                          = 0x0252,
    eWDDLG_AddToMru                          = 0x0255,
    eWDDLG_Lines                             = 0x0256,
    eWDDLG_SQLStatement                      = 0x0257,
    eWDDLG_Connection                        = 0x0258,
    eWDDLG_ColumnNo                          = 0x025E,
    eWDDLG_EvenlySpaced                      = 0x025F,
    eWDDLG_Label                             = 0x0260,
    eWDDLG_NoteType                          = 0x0262,
    eWDDLG_FootNumberAs                      = 0x0263,
    eWDDLG_FootStartingNum                   = 0x0264,
    eWDDLG_FootRestartNum                    = 0x0265,
    eWDDLG_EndnotesAt                        = 0x0266,
    eWDDLG_EndNumberAs                       = 0x0267,
    eWDDLG_EndStartingNum                    = 0x0268,
    eWDDLG_EndRestartNum                     = 0x0269,
    eWDDLG_With                              = 0x0272,
    eWDDLG_FieldName                         = 0x0274,
    eWDDLG_CustomDict5                       = 0x0275,
    eWDDLG_CustomDict6                       = 0x0276,
    eWDDLG_CustomDict7                       = 0x0277,
    eWDDLG_CustomDict8                       = 0x0278,
    eWDDLG_CustomDict9                       = 0x0279,
    eWDDLG_CustomDict10                      = 0x027A,
    eWDDLG_Object                            = 0x027B,
    eWDDLG_AutoCaption                       = 0x027C,
    eWDDLG_ErrorBeeps                        = 0x027E,
    eWDDLG_Symbol                            = 0x0281,
    eWDDLG_PrToFileName                      = 0x0282,
    eWDDLG_AppendPrFile                      = 0x0283,
    eWDDLG_Drive                             = 0x0284,
    eWDDLG_Goto                              = 0x0285,
    eWDDLG_Copy                              = 0x0287,
    eWDDLG_KeyCode2                          = 0x0288,
    eWDDLG_Caption                           = 0x028E,
    eWDDLG_TableId                           = 0x028F,
    eWDDLG_AddedStyles                       = 0x0290,
    eWDDLG_SmartCutPaste                     = 0x0291,
    eWDDLG_InsertedTextMark                  = 0x0292,
    eWDDLG_InsertedTextColor                 = 0x0293,
    eWDDLG_DeletedTextMark                   = 0x0294,
    eWDDLG_DeletedTextColor                  = 0x0295,
    eWDDLG_RevisedLinesMark                  = 0x0296,
    eWDDLG_RevisedLinesColor                 = 0x0297,
    eWDDLG_AddBelow                          = 0x0299,
    eWDDLG_Section                           = 0x029C,
    eWDDLG_ExtractAddress                    = 0x029D,
    eWDDLG_FindPrevious                      = 0x029E,
    eWDDLG_HideMarks                         = 0x029F,
    eWDDLG_ViewRevisions                     = 0x02A1,
    eWDDLG_PrintRevisions                    = 0x02A2,
    eWDDLG_CheckSpelling                     = 0x02A3,
    eWDDLG_Effects3d                         = 0x02A4,
    eWDDLG_Order2                            = 0x02A5,
    eWDDLG_Order3                            = 0x02A6,
    eWDDLG_CheckErrors                       = 0x02A8,
    eWDDLG_MenuType                          = 0x02AC,
    eWDDLG_DraftFont                         = 0x02AD,
    eWDDLG_WrapToWindow                      = 0x02AF,
    eWDDLG_Drawings                          = 0x02B0,
    eWDDLG_Formula                           = 0x02B3,
    eWDDLG_DropHeight                        = 0x02B4,
    eWDDLG_LabelIndex                        = 0x02B5,
    eWDDLG_LabelTray                         = 0x02B6,
    eWDDLG_LabelListIndex                    = 0x02B7,
    eWDDLG_LabelHeight                       = 0x02B8,
    eWDDLG_LabelWidth                        = 0x02B9,
    eWDDLG_LabelTopMargin                    = 0x02BA,
    eWDDLG_LabelSideMargin                   = 0x02BB,
    eWDDLG_LabelHorPitch                     = 0x02BC,
    eWDDLG_LabelVertPitch                    = 0x02BD,
    eWDDLG_LabelAcross                       = 0x02BE,
    eWDDLG_LabelDown                         = 0x02BF,
    eWDDLG_NumLines                          = 0x02C0,
    eWDDLG_VRuler                            = 0x02C2,
    eWDDLG_AllowRowSplit                     = 0x02C4,
    eWDDLG_Superscript                       = 0x02C6,
    eWDDLG_Subscript                         = 0x02C7,
    eWDDLG_WritePassword                     = 0x02C8,
    eWDDLG_RecommendReadOnly                 = 0x02C9,
    eWDDLG_DocumentPassword                  = 0x02CA,
    eWDDLG_Endnotes                          = 0x02CB,
    eWDDLG_Preset                            = 0x02CC,
    eWDDLG_ListType                          = 0x02CD,
    eWDDLG_Revert                            = 0x02CE,
    eWDDLG_MouseSimulation                   = 0x02CF,
    eWDDLG_DemoGuidance                      = 0x02D0,
    eWDDLG_DemoSpeed                         = 0x02D2,
    eWDDLG_CommandKeyHelp                    = 0x02D3,
    eWDDLG_DocNavKeys                        = 0x02D4,
    eWDDLG_HelpText                          = 0x02D5,
    eWDDLG_InsertAs                          = 0x02D6,
    eWDDLG_AcceptAll                         = 0x02D8,
    eWDDLG_RejectAll                         = 0x02D9,
    eWDDLG_Formatting                        = 0x02DC,
    eWDDLG_InitialCaps                       = 0x02DE,
    eWDDLG_SentenceCaps                      = 0x02DF,
    eWDDLG_Days                              = 0x02E0,
    eWDDLG_ReplaceText                       = 0x02E1,
    eWDDLG_MSQuery                           = 0x02E2,
    eWDDLG_Product                           = 0x02E4,
    eWDDLG_WritePasswordDoc                  = 0x02E7,
    eWDDLG_WritePasswordDot                  = 0x02E8,
    eWDDLG_LabelRow                          = 0x02E9,
    eWDDLG_LabelColumn                       = 0x02EA,
    eWDDLG_CommandValue                      = 0x02ED,
    eWDDLG_LimitConsecutiveHyphens           = 0x02EE,
    eWDDLG_RetAddrFromLeft                   = 0x02EF,
    eWDDLG_RetAddrFromTop                    = 0x02F0,
    eWDDLG_SoundsLike                        = 0x02F1,
    eWDDLG_KerningMin                        = 0x02F2,
    eWDDLG_PatternMatch                      = 0x02F3,
    eWDDLG_ToolTips                          = 0x02F4,
    eWDDLG_Mark                              = 0x02F5,
    eWDDLG_MarkAll                           = 0x02F6,
    eWDDLG_ShortCitation                     = 0x02F7,
    eWDDLG_LongCitation                      = 0x02F8,
    eWDDLG_AutoWordSelection                 = 0x02F9,
    eWDDLG_Passim                            = 0x02FA,
    eWDDLG_KeepFormatting                    = 0x02FB,
    eWDDLG_ColorButtons                      = 0x02FC,
    eWDDLG_LargeButtons                      = 0x02FD,
    eWDDLG_HeadingRows                       = 0x02FE,
    eWDDLG_LastRow                           = 0x02FF,
    eWDDLG_FirstColumn                       = 0x0300,
    eWDDLG_LastColumn                        = 0x0301,
    eWDDLG_Borders                           = 0x0302,
    eWDDLG_SnapToGrid                        = 0x0303,
    eWDDLG_XOrigin                           = 0x0304,
    eWDDLG_YOrigin                           = 0x0305,
    eWDDLG_XGrid                             = 0x0306,
    eWDDLG_YGrid                             = 0x0307,
    eWDDLG_EmbedFonts                        = 0x0308,
    eWDDLG_RelativeTo                        = 0x0309,
    eWDDLG_Width                             = 0x030A,
    eWDDLG_Height                            = 0x030B,
    eWDDLG_Drop                              = 0x030E,
    eWDDLG_Gap                               = 0x030F,
    eWDDLG_Angle                             = 0x0310,
    eWDDLG_CrossReference                    = 0x0314,
    eWDDLG_RightAlignPageNumbers             = 0x0315,
    eWDDLG_SendMailAttach                    = 0x0316,
    eWDDLG_RejectRevisions                   = 0x0317,
    eWDDLG_Kerning                           = 0x0318,
    eWDDLG_Exit                              = 0x0319,
    eWDDLG_Enable                            = 0x031A,
    eWDDLG_OwnHelp                           = 0x031B,
    eWDDLG_OwnStat                           = 0x031C,
    eWDDLG_StatText                          = 0x031D,
    eWDDLG_FormsData                         = 0x031E,
    eWDDLG_DefaultTray                       = 0x031F,
    eWDDLG_BookMarks                         = 0x0320,
    eWDDLG_IncludeFields                     = 0x0322,
    eWDDLG_LinkToSource                      = 0x0323,
    eWDDLG_AutoHyphenation                   = 0x0324,
    eWDDLG_TitleAutoText                     = 0x0326,
    eWDDLG_LinkStyles                        = 0x0327,
    eWDDLG_EntryAutoText                     = 0x0328,
    eWDDLG_ChapterNumber                     = 0x0329,
    eWDDLG_Message                           = 0x032A,
    eWDDLG_AllAtOnce                         = 0x032D,
    eWDDLG_ReturnWhenDone                    = 0x032E,
    eWDDLG_TrackStatus                       = 0x032F,
    eWDDLG_FillColor                         = 0x0330,
    eWDDLG_FillPattern                       = 0x0331,
    eWDDLG_FillPatternColor                  = 0x0332,
    eWDDLG_LineStyle                         = 0x0334,
    eWDDLG_LineWeight                        = 0x0335,
    eWDDLG_ArrowLength                       = 0x0336,
    eWDDLG_ArrowWidth                        = 0x0337,
    eWDDLG_ArrowStyle                        = 0x0338,
    eWDDLG_Wizard                            = 0x0339,
    eWDDLG_RoundCorners                      = 0x033A,
    eWDDLG_FineShading                       = 0x033B,
    eWDDLG_RTFInClipboard                    = 0x033C,
    eWDDLG_SavePictureInDoc                  = 0x033E,
    eWDDLG_RouteDocument                     = 0x033F,
    eWDDLG_AddrFromLeft                      = 0x0340,
    eWDDLG_AddrFromTop                       = 0x0341,
    eWDDLG_LabelDotMatrix                    = 0x0342,
    eWDDLG_LabelAutoText                     = 0x0343,
    eWDDLG_LabelText                         = 0x0344,
    eWDDLG_SingleLabel                       = 0x0345,
    eWDDLG_DifferentFirstPage                = 0x0346,
    eWDDLG_DontHyphen                        = 0x0347,
    eWDDLG_NextCitation                      = 0x0348,
    eWDDLG_TextType                          = 0x0349,
    eWDDLG_ShortMenuNames                    = 0x034A,
    eWDDLG_UnavailableFont                   = 0x034B,
    eWDDLG_SubstituteFont                    = 0x034C,
    eWDDLG_MailSubject                       = 0x034D,
    eWDDLG_MailAddress                       = 0x034E,
    eWDDLG_MailAsAttachment                  = 0x034F,
    eWDDLG_MailMerge                         = 0x0350,
    eWDDLG_LockAnchor                        = 0x0351,
    eWDDLG_TextWidth                         = 0x0353,
    eWDDLG_TextDefault                       = 0x0354,
    eWDDLG_TextFormat                        = 0x0355,
    eWDDLG_CheckSize                         = 0x0356,
    eWDDLG_CheckWidth                        = 0x0357,
    eWDDLG_CheckDefault                      = 0x0358,
    eWDDLG_PreserveStyles                    = 0x0359,
    eWDDLG_ApplyStylesHeadings               = 0x035A,
    eWDDLG_ApplyStylesLists                  = 0x035B,
    eWDDLG_ApplyStylesOtherParas             = 0x035D,
    eWDDLG_AdjustParaMarks                   = 0x035E,
    eWDDLG_AdjustTabsSpaces                  = 0x035F,
    eWDDLG_ReplaceQuotes                     = 0x0360,
    eWDDLG_ReplaceSymbols                    = 0x0361,
    eWDDLG_ReplaceBullets                    = 0x0362,
    eWDDLG_CrossReferenceAutoText            = 0x0363,
    eWDDLG_LongCitationAutoText              = 0x0364,
    eWDDLG_SearchName                        = 0x0366,
    eWDDLG_TrueAutoText                      = 0x0367,
    eWDDLG_TrueText                          = 0x0368,
    eWDDLG_FalseAutoText                     = 0x0369,
    eWDDLG_FalseText                         = 0x036A,
    eWDDLG_ValueText                         = 0x036B,
    eWDDLG_ValueAutoText                     = 0x036C,
    eWDDLG_Comparison                        = 0x036D,
    eWDDLG_CompareTo                         = 0x036E,
    eWDDLG_HelpType                          = 0x036F,
    eWDDLG_BlueScreen                        = 0x0370,
    eWDDLG_FullScreen                        = 0x0371,
    eWDDLG_AskOnce                           = 0x0372,
    eWDDLG_DefaultBookmarkText               = 0x0373,
    eWDDLG_MarkEntry                         = 0x0374,
    eWDDLG_MarkCitation                      = 0x0375,
    eWDDLG_AutoMark                          = 0x0376,
    eWDDLG_ListBy                            = 0x0377,
    eWDDLG_SubDir                            = 0x0378,
    eWDDLG_DefaultFillInText                 = 0x0379,
    eWDDLG_ClearRecipients                   = 0x037B,
    eWDDLG_AddRecipient                      = 0x037C,
    eWDDLG_OldRecipient                      = 0x037D,
    eWDDLG_AddSlip                           = 0x037E,
    eWDDLG_ResetSlip                         = 0x037F,
    eWDDLG_ClearSlip                         = 0x0380,
    eWDDLG_ReferenceType                     = 0x0381,
    eWDDLG_ReferenceKind                     = 0x0382,
    eWDDLG_ReferenceItem                     = 0x0383,
    eWDDLG_Length                            = 0x0384,
    eWDDLG_AutoAttach                        = 0x0385,
    eWDDLG_Accent                            = 0x0386,
    eWDDLG_AdjustEmptyParas                  = 0x0387,
    eWDDLG_HorizontalPos                     = 0x0388,
    eWDDLG_HorizontalFrom                    = 0x0389,
    eWDDLG_VerticalPos                       = 0x038A,
    eWDDLG_VerticalFrom                      = 0x038B,
    eWDDLG_InternalMargin                    = 0x038E,
    eWDDLG_Tab                               = 0x038F,
    eWDDLG_NoTabHangIndent                   = 0x0390,
    eWDDLG_NoSpaceRaiseLower                 = 0x0391,
    eWDDLG_PrintColBlack                     = 0x0393,
    eWDDLG_WrapTrailSpaces                   = 0x0394,
    eWDDLG_CategoryName                      = 0x0395,
    eWDDLG_Preview                           = 0x0396,
    eWDDLG_NoColumnBalance                   = 0x0397,
    eWDDLG_DrawingObjects                    = 0x0398,
    eWDDLG_ConvMailMergeEsc                  = 0x0399,
    eWDDLG_Strikethrough                     = 0x039A,
    eWDDLG_Face                              = 0x039B,
    eWDDLG_FieldShading                      = 0x039C,
    eWDDLG_NativePictureFormat               = 0x039D,
    eWDDLG_FileSize                          = 0x039E,
    eWDDLG_AllowAccentedUppercase            = 0x039F,
    eWDDLG_PictureEditor                     = 0x03A0,
    eWDDLG_InsertAsField                     = 0x03A1,
    eWDDLG_LineType                          = 0x03A2,
    eWDDLG_HyphenationZone                   = 0x03A3,
    eWDDLG_DisplayIcon                       = 0x03A4,
    eWDDLG_ShowMarks                         = 0x03A5,
    eWDDLG_ActivateAs                        = 0x03A6,
    eWDDLG_IconFilename                      = 0x03A8,
    eWDDLG_IconNumber                        = 0x03A9,
    eWDDLG_Verb                              = 0x03AA,
    eWDDLG_TwoPages                          = 0x03AB,
    eWDDLG_GlobalDotPrompt                   = 0x03AC,
    eWDDLG_SuppressSpBfAfterPgBrk            = 0x03AD,
    eWDDLG_SuppressTopSpacing                = 0x03AE,
    eWDDLG_OrigWordTableRules                = 0x03AF,
    eWDDLG_TransparentMetafiles              = 0x03B1,
    eWDDLG_NoReset                           = 0x03B2,
    eWDDLG_HideAutoFit                       = 0x03B3,
    eWDDLG_Space                             = 0x03B5,
    eWDDLG_Include                           = 0x03B6,
    eWDDLG_ShowBreaksInFrames                = 0x03B7,
    eWDDLG_SwapBordersFacingPages            = 0x03B8,
    eWDDLG_SQLStatement1                     = 0x03B9,
    eWDDLG_LeaveBackslashAlone               = 0x03CD,
    eWDDLG_AddAll                            = 0x03CE,
    eWDDLG_SaveAsAOCELetter                  = 0x03DB,
    eWDDLG_FractionalWidths                  = 0x03E0,
    eWDDLG_PSOverText                        = 0x03E1,
    eWDDLG_ExpandShiftReturn                 = 0x03E9,
    eWDDLG_DontULTrailSpace                  = 0x03EA,
    eWDDLG_DontBalanceSbDbWidth              = 0x03EB,
    eWDDLG_SuppressTopSpacingMac5            = 0x03F1,
    eWDDLG_HighlightColor                    = 0x03F3,
    eWDDLG_Highlight                         = 0x03F4,
    eWDDLG_SpacingInWholePoints              = 0x03F6,
    eWDDLG_HideSpellingErrors                = 0x03F8,
    eWDDLG_RecheckDocument                   = 0x03F9,
    eWDDLG_PrintBodyTextBeforeHeader         = 0x03FB,
    eWDDLG_NoLeading                         = 0x03FD,
    eWDDLG_ShowFolders                       = 0x03FF,
    eWDDLG_OutputPrinter                     = 0x0400,
    eWDDLG_ShowOptionsFor                    = 0x0401,
    eWDDLG_ApplyBorders                      = 0x0402,
    eWDDLG_ApplyBulletedLists                = 0x0404,
    eWDDLG_ApplyNumberedLists                = 0x0405,
    eWDDLG_ReplaceOrdinals                   = 0x0407,
    eWDDLG_ReplaceFractions                  = 0x0408,
    eWDDLG_TipWizardActive                   = 0x0409,
    eWDDLG_MWSmallCaps                       = 0x040F,
    eWDDLG_NoSpaceForUL                      = 0x0411,
    eWDDLG_NoExtraLineSpacing                = 0x0419,
    eWDDLG_DoNotSetAsSysDefault              = 0x041A,
    eWDDLG_CapsLock                          = 0x041B,
    eWDDLG_AutoAdd                           = 0x041C,
    eWDDLG_TruncateFontHeight                = 0x041E,
    eWDDLG_ToolTipsKey                       = 0x041F,
    eWDDLG_FindAllWordForms                  = 0x0422,
    eWDDLG_SubFontBySize                     = 0x0423,
    eWDDLG_AddressBookType                   = 0x0424,
    eWDDLG_TabIndent                         = 0x0452,
} EWDDLG_T, FAR *LPEWDDLG;

#else

typedef WORD tagEWDCMD;

#define eWDCMD_Help                              0x0001
#define eWDCMD_HelpTool                          0x0002
#define eWDCMD_HelpUsingHelp                     0x0003
#define eWDCMD_HelpActiveWindow                  0x0004
#define eWDCMD_HelpKeyboard                      0x0005
#define eWDCMD_HelpIndex                         0x0006
#define eWDCMD_HelpQuickPreview                  0x0007
#define eWDCMD_HelpExamplesAndDemos              0x0008
#define eWDCMD_HelpAbout                         0x0009
#define eWDCMD_HelpWordPerfectHelp               0x000A
#define eWDCMD_GrowFont                          0x000B
#define eWDCMD_ShrinkFont                        0x000C
#define eWDCMD_Overtype                          0x000D
#define eWDCMD_ExtendSelection                   0x000E
#define eWDCMD_Spike                             0x000F
#define eWDCMD_InsertSpike                       0x0010
#define eWDCMD_ChangeCase                        0x0011
#define eWDCMD_MoveText                          0x0012
#define eWDCMD_CopyText                          0x0013
#define eWDCMD_InsertAutoText                    0x0014
#define eWDCMD_OtherPane                         0x0015
#define eWDCMD_NextWindow                        0x0016
#define eWDCMD_PrevWindow                        0x0017
#define eWDCMD_RepeatFind                        0x0018
#define eWDCMD_NextField                         0x0019
#define eWDCMD_PrevField                         0x001A
#define eWDCMD_ColumnSelect                      0x001B
#define eWDCMD_DeleteWord                        0x001C
#define eWDCMD_DeleteBackWord                    0x001D
#define eWDCMD_EditClear                         0x001E
#define eWDCMD_InsertFieldChars                  0x001F
#define eWDCMD_UpdateFields                      0x0020
#define eWDCMD_UnlinkFields                      0x0021
#define eWDCMD_ToggleFieldDisplay                0x0022
#define eWDCMD_LockFields                        0x0023
#define eWDCMD_UnlockFields                      0x0024
#define eWDCMD_UpdateSource                      0x0025
#define eWDCMD_Indent                            0x0026
#define eWDCMD_UnIndent                          0x0027
#define eWDCMD_HangingIndent                     0x0028
#define eWDCMD_UnHang                            0x0029
#define eWDCMD_Font                              0x002A
#define eWDCMD_FontSizeSelect                    0x002B
#define eWDCMD_WW2_RulerMode                     0x002C
#define eWDCMD_Bold                              0x002D
#define eWDCMD_Italic                            0x002E
#define eWDCMD_SmallCaps                         0x002F
#define eWDCMD_AllCaps                           0x0030
#define eWDCMD_Strikethrough                     0x0031
#define eWDCMD_Hidden                            0x0032
#define eWDCMD_Underline                         0x0033
#define eWDCMD_DoubleUnderline                   0x0034
#define eWDCMD_WordUnderline                     0x0035
#define eWDCMD_Superscript                       0x0036
#define eWDCMD_Subscript                         0x0037
#define eWDCMD_ResetChar                         0x0038
#define eWDCMD_CharColor                         0x0039
#define eWDCMD_LeftPara                          0x003A
#define eWDCMD_CenterPara                        0x003B
#define eWDCMD_RightPara                         0x003C
#define eWDCMD_JustifyPara                       0x003D
#define eWDCMD_SpacePara1                        0x003E
#define eWDCMD_SpacePara15                       0x003F
#define eWDCMD_SpacePara2                        0x0040
#define eWDCMD_CloseUpPara                       0x0041
#define eWDCMD_OpenUpPara                        0x0042
#define eWDCMD_ResetPara                         0x0043
#define eWDCMD_EditRepeat                        0x0044
#define eWDCMD_GoBack                            0x0045
#define eWDCMD_SaveTemplate                      0x0046
#define eWDCMD_OK                                0x0047
#define eWDCMD_Cancel                            0x0048
#define eWDCMD_CopyFormat                        0x0049
#define eWDCMD_PrevPage                          0x004A
#define eWDCMD_NextPage                          0x004B
#define eWDCMD_NextObject                        0x004C
#define eWDCMD_PrevObject                        0x004D
#define eWDCMD_DocumentStatistics                0x004E
#define eWDCMD_FileNew                           0x004F
#define eWDCMD_FileOpen                          0x0050
#define eWDCMD_MailMergeOpenDataSource           0x0051
#define eWDCMD_MailMergeOpenHeaderSource         0x0052
#define eWDCMD_FileSave                          0x0053
#define eWDCMD_FileSaveAs                        0x0054
#define eWDCMD_FileSaveAll                       0x0055
#define eWDCMD_FileSummaryInfo                   0x0056
#define eWDCMD_FileTemplates                     0x0057
#define eWDCMD_FilePrint                         0x0058
#define eWDCMD_FilePrintPreview                  0x0059
#define eWDCMD_WW2_PrintMerge                    0x005A
#define eWDCMD_WW2_PrintMergeCheck               0x005B
#define eWDCMD_WW2_PrintMergeToDoc               0x005C
#define eWDCMD_WW2_PrintMergeToPrinter           0x005D
#define eWDCMD_WW2_PrintMergeSelection           0x005E
#define eWDCMD_WW2_PrintMergeHelper              0x005F
#define eWDCMD_MailMergeReset                    0x0060
#define eWDCMD_FilePrintSetup                    0x0061
#define eWDCMD_FileExit                          0x0062
#define eWDCMD_FileFind                          0x0063
#define eWDCMD_FormatAddrFonts                   0x0067
#define eWDCMD_MailMergeEditDataSource           0x0068
#define eWDCMD_WW2_PrintMergeCreateDataSource    0x0069
#define eWDCMD_WW2_PrintMergeCreateHeaderSource  0x006A
#define eWDCMD_EditUndo                          0x006B
#define eWDCMD_EditCut                           0x006C
#define eWDCMD_EditCopy                          0x006D
#define eWDCMD_EditPaste                         0x006E
#define eWDCMD_EditPasteSpecial                  0x006F
#define eWDCMD_EditFind                          0x0070
#define eWDCMD_EditFindFont                      0x0071
#define eWDCMD_EditFindPara                      0x0072
#define eWDCMD_EditFindStyle                     0x0073
#define eWDCMD_EditFindClearFormatting           0x0074
#define eWDCMD_EditReplace                       0x0075
#define eWDCMD_EditReplaceFont                   0x0076
#define eWDCMD_EditReplacePara                   0x0077
#define eWDCMD_EditReplaceStyle                  0x0078
#define eWDCMD_EditReplaceClearFormatting        0x0079
#define eWDCMD_EditGoTo                          0x007A
#define eWDCMD_EditAutoText                      0x007B
#define eWDCMD_EditLinks                         0x007C
#define eWDCMD_EditObject                        0x007D
#define eWDCMD_ActivateObject                    0x007E
#define eWDCMD_TextToTable                       0x007F
#define eWDCMD_TableToText                       0x0080
#define eWDCMD_TableInsertTable                  0x0081
#define eWDCMD_TableInsertCells                  0x0082
#define eWDCMD_TableInsertRow                    0x0083
#define eWDCMD_TableInsertColumn                 0x0084
#define eWDCMD_TableDeleteCells                  0x0085
#define eWDCMD_TableDeleteRow                    0x0086
#define eWDCMD_TableDeleteColumn                 0x0087
#define eWDCMD_TableMergeCells                   0x0088
#define eWDCMD_TableSplitCells                   0x0089
#define eWDCMD_TableSplit                        0x008A
#define eWDCMD_TableSelectTable                  0x008B
#define eWDCMD_TableSelectRow                    0x008C
#define eWDCMD_TableSelectColumn                 0x008D
#define eWDCMD_TableRowHeight                    0x008E
#define eWDCMD_TableColumnWidth                  0x008F
#define eWDCMD_TableGridlines                    0x0090
#define eWDCMD_ViewNormal                        0x0091
#define eWDCMD_ViewOutline                       0x0092
#define eWDCMD_ViewPage                          0x0093
#define eWDCMD_WW2_ViewZoom                      0x0094
#define eWDCMD_ViewDraft                         0x0095
#define eWDCMD_ViewFieldCodes                    0x0096
#define eWDCMD_Style                             0x0097
#define eWDCMD_ToolsCustomize                    0x0098
#define eWDCMD_ViewRuler                         0x0099
#define eWDCMD_ViewStatusBar                     0x009A
#define eWDCMD_NormalViewHeaderArea              0x009B
#define eWDCMD_ViewFootnoteArea                  0x009C
#define eWDCMD_ViewAnnotations                   0x009D
#define eWDCMD_InsertFrame                       0x009E
#define eWDCMD_InsertBreak                       0x009F
#define eWDCMD_WW2_InsertFootnote                0x00A0
#define eWDCMD_InsertAnnotation                  0x00A1
#define eWDCMD_InsertSymbol                      0x00A2
#define eWDCMD_InsertPicture                     0x00A3
#define eWDCMD_InsertFile                        0x00A4
#define eWDCMD_InsertDateTime                    0x00A5
#define eWDCMD_InsertField                       0x00A6
#define eWDCMD_InsertMergeField                  0x00A7
#define eWDCMD_EditBookmark                      0x00A8
#define eWDCMD_MarkIndexEntry                    0x00A9
#define eWDCMD_InsertIndex                       0x00AA
#define eWDCMD_InsertTableOfContents             0x00AB
#define eWDCMD_InsertObject                      0x00AC
#define eWDCMD_ToolsCreateEnvelope               0x00AD
#define eWDCMD_FormatFont                        0x00AE
#define eWDCMD_FormatParagraph                   0x00AF
#define eWDCMD_FormatSectionLayout               0x00B0
#define eWDCMD_FormatColumns                     0x00B1
#define eWDCMD_FilePageSetup                     0x00B2
#define eWDCMD_FormatTabs                        0x00B3
#define eWDCMD_FormatStyle                       0x00B4
#define eWDCMD_FormatDefineStyleFont             0x00B5
#define eWDCMD_FormatDefineStylePara             0x00B6
#define eWDCMD_FormatDefineStyleTabs             0x00B7
#define eWDCMD_FormatDefineStyleFrame            0x00B8
#define eWDCMD_FormatDefineStyleBorders          0x00B9
#define eWDCMD_FormatDefineStyleLang             0x00BA
#define eWDCMD_FormatPicture                     0x00BB
#define eWDCMD_ToolsLanguage                     0x00BC
#define eWDCMD_FormatBordersAndShading           0x00BD
#define eWDCMD_FormatFrame                       0x00BE
#define eWDCMD_ToolsSpelling                     0x00BF
#define eWDCMD_ToolsSpellSelection               0x00C0
#define eWDCMD_ToolsGrammar                      0x00C1
#define eWDCMD_ToolsThesaurus                    0x00C2
#define eWDCMD_ToolsHyphenation                  0x00C3
#define eWDCMD_ToolsBulletsNumbers               0x00C4
#define eWDCMD_ToolsRevisions                    0x00C5
#define eWDCMD_ToolsCompareVersions              0x00C6
#define eWDCMD_TableSort                         0x00C7
#define eWDCMD_ToolsCalculate                    0x00C8
#define eWDCMD_ToolsRepaginate                   0x00C9
#define eWDCMD_ToolsOptions                      0x00CA
#define eWDCMD_ToolsOptionsGeneral               0x00CB
#define eWDCMD_ToolsOptionsView                  0x00CC
#define eWDCMD_ToolsAdvancedSettings             0x00CE
#define eWDCMD_ToolsOptionsPrint                 0x00D0
#define eWDCMD_ToolsOptionsSave                  0x00D1
#define eWDCMD_WW2_ToolsOptionsToolbar           0x00D2
#define eWDCMD_ToolsOptionsSpelling              0x00D3
#define eWDCMD_ToolsOptionsGrammar               0x00D4
#define eWDCMD_ToolsOptionsUserInfo              0x00D5
#define eWDCMD_ToolsRecordMacroToggle            0x00D6
#define eWDCMD_ToolsMacro                        0x00D7
#define eWDCMD_PauseRecorder                     0x00D8
#define eWDCMD_WindowNewWindow                   0x00D9
#define eWDCMD_WindowArrangeAll                  0x00DA
#define eWDCMD_MailMergeEditMainDocument         0x00DB
#define eWDCMD_WindowList                        0x00DC
#define eWDCMD_FormatRetAddrFonts                0x00DD
#define eWDCMD_Organizer                         0x00DE
#define eWDCMD_WW2_TableColumnWidth              0x00DF
#define eWDCMD_ToolsOptionsEdit                  0x00E0
#define eWDCMD_ToolsOptionsFileLocations         0x00E1
#define eWDCMD_RecordNextCommand                 0x00E2
#define eWDCMD_ToolsAutoCorrectSmartQuotes       0x00E3
#define eWDCMD_ToolsWordCount                    0x00E4
#define eWDCMD_DocSplit                          0x00E5
#define eWDCMD_DocSize                           0x00E6
#define eWDCMD_DocMove                           0x00E7
#define eWDCMD_DocMaximize                       0x00E8
#define eWDCMD_DocRestore                        0x00E9
#define eWDCMD_DocClose                          0x00EA
#define eWDCMD_ControlRun                        0x00EB
#define eWDCMD_ShrinkSelection                   0x00EC
#define eWDCMD_EditSelectAll                     0x00ED
#define eWDCMD_IconBarMode                       0x00EE
#define eWDCMD_InsertPageField                   0x00EF
#define eWDCMD_InsertDateField                   0x00F0
#define eWDCMD_InsertTimeField                   0x00F1
#define eWDCMD_FormatHeaderFooterLink            0x00F2
#define eWDCMD_ClosePane                         0x00F3
#define eWDCMD_OutlinePromote                    0x00F4
#define eWDCMD_OutlineDemote                     0x00F5
#define eWDCMD_OutlineMoveUp                     0x00F6
#define eWDCMD_OutlineMoveDown                   0x00F7
#define eWDCMD_NormalStyle                       0x00F8
#define eWDCMD_OutlineExpand                     0x00F9
#define eWDCMD_OutlineCollapse                   0x00FA
#define eWDCMD_ShowHeading1                      0x00FB
#define eWDCMD_ShowHeading2                      0x00FC
#define eWDCMD_ShowHeading3                      0x00FD
#define eWDCMD_ShowHeading4                      0x00FE
#define eWDCMD_ShowHeading5                      0x00FF
#define eWDCMD_ShowHeading6                      0x0100
#define eWDCMD_ShowHeading7                      0x0101
#define eWDCMD_ShowHeading8                      0x0102
#define eWDCMD_ShowHeading9                      0x0103
#define eWDCMD_ShowAllHeadings                   0x0104
#define eWDCMD_OutlineShowFirstLine              0x0105
#define eWDCMD_OutlineShowFormat                 0x0106
#define eWDCMD_ShowVars                          0x0107
#define eWDCMD_StepOver                          0x0108
#define eWDCMD_StepIn                            0x0109
#define eWDCMD_ContinueMacro                     0x010A
#define eWDCMD_TraceMacro                        0x010B
#define eWDCMD_NextCell                          0x010E
#define eWDCMD_PrevCell                          0x010F
#define eWDCMD_StartOfRow                        0x0110
#define eWDCMD_EndOfRow                          0x0111
#define eWDCMD_StartOfColumn                     0x0112
#define eWDCMD_EndOfColumn                       0x0113
#define eWDCMD_ShowAll                           0x0114
#define eWDCMD_InsertPageBreak                   0x0115
#define eWDCMD_InsertColumnBreak                 0x0116
#define eWDCMD_AppMinimize                       0x0117
#define eWDCMD_AppMaximize                       0x0118
#define eWDCMD_AppRestore                        0x0119
#define eWDCMD_DoFieldClick                      0x011A
#define eWDCMD_FileClose                         0x011B
#define eWDCMD_InsertDrawing                     0x011C
#define eWDCMD_InsertChart                       0x011D
#define eWDCMD_SelectCurFont                     0x011E
#define eWDCMD_SelectCurAlignment                0x011F
#define eWDCMD_SelectCurSpacing                  0x0120
#define eWDCMD_SelectCurIndent                   0x0121
#define eWDCMD_SelectCurTabs                     0x0122
#define eWDCMD_SelectCurColor                    0x0123
#define eWDCMD_RemoveFrames                      0x0124
#define eWDCMD_MenuMode                          0x0125
#define eWDCMD_InsertPageNumbers                 0x0126
#define eWDCMD_WW2_ChangeRulerMode               0x0127
#define eWDCMD_EditPicture                       0x0128
#define eWDCMD_UserDialog                        0x0129
#define eWDCMD_FormatPageNumber                  0x012A
#define eWDCMD_WW2_FootnoteOptions               0x012B
#define eWDCMD_CopyFile                          0x012C
#define eWDCMD_FileNewDefault                    0x012D
#define eWDCMD_FilePrintDefault                  0x012E
#define eWDCMD_ViewZoomWholePage                 0x012F
#define eWDCMD_ViewZoomPageWidth                 0x0130
#define eWDCMD_ViewZoom100                       0x0131
#define eWDCMD_TogglePortrait                    0x0132
#define eWDCMD_ToolsBulletListDefault            0x0133
#define eWDCMD_ToggleScribbleMode                0x0134
#define eWDCMD_ToolsNumberListDefault            0x0135
#define eWDCMD_FileAOCEAddMailer                 0x0137
#define eWDCMD_FileAOCEDeleteMailer              0x0138
#define eWDCMD_FileAOCEExpandMailer              0x0139
#define eWDCMD_FileAOCESendMail                  0x013B
#define eWDCMD_FileAOCEReplyMail                 0x013C
#define eWDCMD_FileAOCEReplyAllMail              0x013D
#define eWDCMD_FileAOCEForwardMail               0x013E
#define eWDCMD_FileAOCENextLetter                0x013F
#define eWDCMD_DocMinimize                       0x0140
#define eWDCMD_FormatAutoFormatBegin             0x0141
#define eWDCMD_FormatChangeCase                  0x0142
#define eWDCMD_ViewToolbars                      0x0143
#define eWDCMD_TableInsertGeneral                0x0144
#define eWDCMD_TableDeleteGeneral                0x0145
#define eWDCMD_WW2_TableRowHeight                0x0146
#define eWDCMD_TableToOrFromText                 0x0147
#define eWDCMD_EditRedo                          0x0149
#define eWDCMD_EditRedoOrRepeat                  0x014A
#define eWDCMD_ViewEndnoteArea                   0x0152
#define eWDCMD_MailMergeDataForm                 0x0154
#define eWDCMD_InsertDatabase                    0x0155
#define eWDCMD_WW2_InsertTableOfContents         0x0158
#define eWDCMD_WW2_ToolsHyphenation              0x0159
#define eWDCMD_FormatFrameOrFramePicture         0x015A
#define eWDCMD_WW2_ToolsOptionsPrint             0x015B
#define eWDCMD_TableFormula                      0x015C
#define eWDCMD_TextFormField                     0x015D
#define eWDCMD_CheckBoxFormField                 0x015E
#define eWDCMD_DropDownFormField                 0x015F
#define eWDCMD_FormFieldOptions                  0x0161
#define eWDCMD_ProtectForm                       0x0162
#define eWDCMD_InsertCaption                     0x0165
#define eWDCMD_InsertCaptionNumbering            0x0166
#define eWDCMD_InsertAutoCaption                 0x0167
#define eWDCMD_HelpPSSHelp                       0x0168
#define eWDCMD_DrawTextbox                       0x016B
#define eWDCMD_ToolsOptionsAutoFormat            0x016D
#define eWDCMD_DemoteToBodyText                  0x016E
#define eWDCMD_InsertCrossReference              0x016F
#define eWDCMD_InsertFootnoteNow                 0x0170
#define eWDCMD_InsertEndnoteNow                  0x0171
#define eWDCMD_InsertFootnote                    0x0172
#define eWDCMD_NoteOptions                       0x0175
#define eWDCMD_WW2_FormatCharacter               0x0176
#define eWDCMD_DrawLine                          0x0178
#define eWDCMD_DrawRectangle                     0x0179
#define eWDCMD_ToolsAutoCorrect                  0x017A
#define eWDCMD_ToolsAutoCorrectReplaceText       0x017C
#define eWDCMD_ToolsAutoCorrectInitialCaps       0x017D
#define eWDCMD_ToolsAutoCorrectSentenceCaps      0x017F
#define eWDCMD_ToolsAutoCorrectDays              0x0180
#define eWDCMD_FormatAutoFormat                  0x0181
#define eWDCMD_ToolsOptionsRevisions             0x0182
#define eWDCMD_WW2_ToolsOptionsGeneral           0x0183
#define eWDCMD_ResetNoteSepOrNotice              0x0184
#define eWDCMD_FormatBullet                      0x0185
#define eWDCMD_FormatNumber                      0x0186
#define eWDCMD_FormatMultilevel                  0x0187
#define eWDCMD_ConvertObject                     0x0188
#define eWDCMD_TableSortAToZ                     0x0189
#define eWDCMD_TableSortZToA                     0x018A
#define eWDCMD_FormatBulletsAndNumbering         0x018D
#define eWDCMD_FormatNumberDefault               0x018E
#define eWDCMD_FormatBulletDefault               0x018F
#define eWDCMD_InsertAddCaption                  0x0192
#define eWDCMD_GoToNextPage                      0x0194
#define eWDCMD_GoToPreviousPage                  0x0195
#define eWDCMD_GoToNextSection                   0x0196
#define eWDCMD_GoToPreviousSection               0x0197
#define eWDCMD_GoToNextFootnote                  0x0198
#define eWDCMD_GoToPreviousFootnote              0x0199
#define eWDCMD_GoToNextEndnote                   0x019A
#define eWDCMD_GoToPreviousEndnote               0x019B
#define eWDCMD_GoToNextAnnotation                0x019C
#define eWDCMD_GoToPreviousAnnotation            0x019D
#define eWDCMD_WW2_FormatDefineStyleChar         0x019E
#define eWDCMD_WW2_EditFindChar                  0x019F
#define eWDCMD_WW2_EditReplaceChar               0x01A0
#define eWDCMD_AppMove                           0x01A2
#define eWDCMD_AppSize                           0x01A3
#define eWDCMD_Connect                           0x01A4
#define eWDCMD_WW2_EditFind                      0x01A5
#define eWDCMD_WW2_EditReplace                   0x01A6
#define eWDCMD_EditFindLang                      0x01AC
#define eWDCMD_EditReplaceLang                   0x01AD
#define eWDCMD_MailMergeViewData                 0x01AF
#define eWDCMD_ToolsCustomizeKeyboard            0x01B0
#define eWDCMD_ToolsCustomizeMenus               0x01B1
#define eWDCMD_WW2_ToolsOptionsKeyboard          0x01B2
#define eWDCMD_ToolsMergeRevisions               0x01B3
#define eWDCMD_ClosePreview                      0x01B5
#define eWDCMD_SkipNumbering                     0x01B6
#define eWDCMD_EditConvertAllFootnotes           0x01B7
#define eWDCMD_EditConvertAllEndnotes            0x01B8
#define eWDCMD_EditSwapAllNotes                  0x01B9
#define eWDCMD_MarkTableOfContentsEntry          0x01BA
#define eWDCMD_FileMacPageSetupGX                0x01BC
#define eWDCMD_FilePrintOneCopy                  0x01BD
#define eWDCMD_EditFindTabs                      0x01BE
#define eWDCMD_EditFindBorder                    0x01BF
#define eWDCMD_EditFindFrame                     0x01C0
#define eWDCMD_BorderOutside                     0x01C1
#define eWDCMD_BorderNone                        0x01C2
#define eWDCMD_BorderLineStyle                   0x01C3
#define eWDCMD_ShadingPattern                    0x01C4
#define eWDCMD_DrawEllipse                       0x01C6
#define eWDCMD_DrawArc                           0x01C7
#define eWDCMD_EditReplaceTabs                   0x01C8
#define eWDCMD_EditReplaceBorder                 0x01C9
#define eWDCMD_EditReplaceFrame                  0x01CA
#define eWDCMD_ShowClipboard                     0x01CB
#define eWDCMD_EditConvertNotes                  0x01CE
#define eWDCMD_MarkCitation                      0x01CF
#define eWDCMD_WW2_ToolsRevisionsMark            0x01D0
#define eWDCMD_DrawGroup                         0x01D1
#define eWDCMD_DrawBringToFront                  0x01D2
#define eWDCMD_DrawSendToBack                    0x01D3
#define eWDCMD_DrawSendBehindText                0x01D4
#define eWDCMD_DrawBringInFrontOfText            0x01D5
#define eWDCMD_InsertTableOfAuthorities          0x01D7
#define eWDCMD_InsertTableOfFigures              0x01D8
#define eWDCMD_InsertIndexAndTables              0x01D9
#define eWDCMD_MailMergeNextRecord               0x01DE
#define eWDCMD_MailMergePrevRecord               0x01DF
#define eWDCMD_MailMergeFirstRecord              0x01E0
#define eWDCMD_MailMergeLastRecord               0x01E1
#define eWDCMD_MailMergeGoToRecord               0x01E2
#define eWDCMD_InsertFormField                   0x01E3
#define eWDCMD_ViewHeader                        0x01E4
#define eWDCMD_DrawUngroup                       0x01E5
#define eWDCMD_PasteFormat                       0x01E6
#define eWDCMD_WW2_ToolsOptionsMenus             0x01E7
#define eWDCMD_FormatDropCap                     0x01E8
#define eWDCMD_ToolsCreateLabels                 0x01E9
#define eWDCMD_ViewMasterDocument                0x01EA
#define eWDCMD_CreateSubdocument                 0x01EB
#define eWDCMD_ViewFootnoteSeparator             0x01ED
#define eWDCMD_ViewFootnoteContSeparator         0x01EE
#define eWDCMD_ViewFootnoteContNotice            0x01EF
#define eWDCMD_ViewEndnoteSeparator              0x01F0
#define eWDCMD_ViewEndnoteContSeparator          0x01F1
#define eWDCMD_ViewEndnoteContNotice             0x01F2
#define eWDCMD_WW2_ToolsOptionsView              0x01F3
#define eWDCMD_DrawBringForward                  0x01F4
#define eWDCMD_DrawSendBackward                  0x01F5
#define eWDCMD_ViewFootnotes                     0x01F6
#define eWDCMD_ToolsProtectDocument              0x01F7
#define eWDCMD_ToolsShrinkToFit                  0x01F8
#define eWDCMD_FormatStyleGallery                0x01F9
#define eWDCMD_ToolsReviewRevisions              0x01FA
#define eWDCMD_HelpSearch                        0x01FE
#define eWDCMD_HelpWordPerfectHelpOptions        0x01FF
#define eWDCMD_MailMergeConvertChevrons          0x0200
#define eWDCMD_GrowFontOnePoint                  0x0201
#define eWDCMD_ShrinkFontOnePoint                0x0202
#define eWDCMD_Magnifier                         0x0203
#define eWDCMD_FilePrintPreviewFullScreen        0x0204
#define eWDCMD_InsertSound                       0x0207
#define eWDCMD_ToolsProtectUnprotectDocument     0x0208
#define eWDCMD_ToolsUnprotectDocument            0x0209
#define eWDCMD_RemoveBulletsNumbers              0x020A
#define eWDCMD_FileCloseOrCloseAll               0x020B
#define eWDCMD_FileCloseAll                      0x020C
#define eWDCMD_ToolsOptionsCompatibility         0x020D
#define eWDCMD_CopyButtonImage                   0x020E
#define eWDCMD_PasteButtonImage                  0x020F
#define eWDCMD_ResetButtonImage                  0x0210
#define eWDCMD_Columns                           0x0212
#define eWDCMD_Condensed                         0x0213
#define eWDCMD_Expanded                          0x0214
#define eWDCMD_FontSize                          0x0215
#define eWDCMD_Lowered                           0x0216
#define eWDCMD_Raised                            0x0217
#define eWDCMD_FileOpenFile                      0x0218
#define eWDCMD_DrawRoundRectangle                0x0219
#define eWDCMD_DrawFreeformPolygon               0x021A
#define eWDCMD_SelectDrawingObjects              0x0221
#define eWDCMD_Shading                           0x0222
#define eWDCMD_Borders                           0x0223
#define eWDCMD_Color                             0x0224
#define eWDCMD_DialogEditor                      0x0228
#define eWDCMD_MacroREM                          0x0229
#define eWDCMD_StartMacro                        0x022A
#define eWDCMD_Symbol                            0x022B
#define eWDCMD_DrawToggleLayer                   0x022C
#define eWDCMD_ToolsCustomizeKeyboardShortcut    0x022D
#define eWDCMD_ToolsCustomizeAddMenuShortcut     0x022E
#define eWDCMD_DrawFlipHorizontal                0x022F
#define eWDCMD_DrawFlipVertical                  0x0230
#define eWDCMD_DrawRotateRight                   0x0231
#define eWDCMD_DrawRotateLeft                    0x0232
#define eWDCMD_TableAutoFormat                   0x0233
#define eWDCMD_FormatDrawingObject               0x0235
#define eWDCMD_InsertExcelTable                  0x0237
#define eWDCMD_MailMergeListWordFields           0x0238
#define eWDCMD_MailMergeFindRecord               0x0239
#define eWDCMD_NormalFontSpacing                 0x023B
#define eWDCMD_NormalFontPosition                0x023C
#define eWDCMD_ViewZoom200                       0x023D
#define eWDCMD_ViewZoom75                        0x023E
#define eWDCMD_DrawDisassemblePicture            0x023F
#define eWDCMD_ViewZoom                          0x0241
#define eWDCMD_ToolsProtectSection               0x0242
#define eWDCMD_FontSubstitution                  0x0245
#define eWDCMD_ToggleFull                        0x0246
#define eWDCMD_InsertSubdocument                 0x0247
#define eWDCMD_MergeSubdocument                  0x0248
#define eWDCMD_SplitSubdocument                  0x0249
#define eWDCMD_NewToolbar                        0x024A
#define eWDCMD_ToggleMainTextLayer               0x024B
#define eWDCMD_ShowPrevHeaderFooter              0x024C
#define eWDCMD_ShowNextHeaderFooter              0x024D
#define eWDCMD_GoToHeaderFooter                  0x024E
#define eWDCMD_PromoteList                       0x024F
#define eWDCMD_DemoteList                        0x0250
#define eWDCMD_ApplyHeading1                     0x0251
#define eWDCMD_ApplyHeading2                     0x0252
#define eWDCMD_ApplyHeading3                     0x0253
#define eWDCMD_ApplyListBullet                   0x0254
#define eWDCMD_GotoAnnotationScope               0x0255
#define eWDCMD_TableHeadings                     0x0256
#define eWDCMD_OpenSubdocument                   0x0257
#define eWDCMD_LockDocument                      0x0258
#define eWDCMD_ToolsCustomizeRemoveMenuShortcut  0x0259
#define eWDCMD_FormatDefineStyleNumbers          0x025A
#define eWDCMD_FormatHeadingNumbering            0x025B
#define eWDCMD_ViewBorderToolbar                 0x025C
#define eWDCMD_ViewDrawingToolbar                0x025D
#define eWDCMD_FormatHeadingNumber               0x025E
#define eWDCMD_ToolsEnvelopesAndLabels           0x025F
#define eWDCMD_DrawReshape                       0x0260
#define eWDCMD_MailMergeAskToConvertChevrons     0x0261
#define eWDCMD_FormatCallout                     0x0262
#define eWDCMD_DrawCallout                       0x0263
#define eWDCMD_TableFormatCell                   0x0264
#define eWDCMD_FileSendMail                      0x0265
#define eWDCMD_EditButtonImage                   0x0266
#define eWDCMD_ToolsCustomizeMenuBar             0x0267
#define eWDCMD_AutoMarkIndexEntries              0x0268
#define eWDCMD_InsertEnSpace                     0x026A
#define eWDCMD_InsertEmSpace                     0x026B
#define eWDCMD_DottedUnderline                   0x026C
#define eWDCMD_ParaKeepLinesTogether             0x026D
#define eWDCMD_ParaKeepWithNext                  0x026E
#define eWDCMD_ParaPageBreakBefore               0x026F
#define eWDCMD_FileRoutingSlip                   0x0270
#define eWDCMD_EditTOACategory                   0x0271
#define eWDCMD_TableUpdateAutoFormat             0x0272
#define eWDCMD_ChooseButtonImage                 0x0273
#define eWDCMD_ParaWidowOrphanControl            0x0274
#define eWDCMD_ToolsAddRecordDefault             0x0275
#define eWDCMD_ToolsRemoveRecordDefault          0x0276
#define eWDCMD_ToolsManageFields                 0x0277
#define eWDCMD_ViewToggleMasterDocument          0x0278
#define eWDCMD_DrawSnapToGrid                    0x0279
#define eWDCMD_DrawAlign                         0x027A
#define eWDCMD_HelpTipOfTheDay                   0x027B
#define eWDCMD_FormShading                       0x027C
#define eWDCMD_RemoveSubdocument                 0x027F
#define eWDCMD_CloseViewHeaderFooter             0x0280
#define eWDCMD_TableAutoSum                      0x0281
#define eWDCMD_MailMergeCreateDataSource         0x0282
#define eWDCMD_MailMergeCreateHeaderSource       0x0283
#define eWDCMD_StopMacroRunning                  0x0285
#define eWDCMD_DrawInsertWordPicture             0x0288
#define eWDCMD_IncreaseIndent                    0x0289
#define eWDCMD_DecreaseIndent                    0x028A
#define eWDCMD_SymbolFont                        0x028B
#define eWDCMD_ToggleHeaderFooterLink            0x028C
#define eWDCMD_AutoText                          0x028D
#define eWDCMD_ViewFooter                        0x028E
#define eWDCMD_MicrosoftMail                     0x0290
#define eWDCMD_MicrosoftExcel                    0x0291
#define eWDCMD_MicrosoftAccess                   0x0292
#define eWDCMD_MicrosoftSchedule                 0x0293
#define eWDCMD_MicrosoftFoxPro                   0x0294
#define eWDCMD_MicrosoftPowerPoint               0x0295
#define eWDCMD_MicrosoftPublisher                0x0296
#define eWDCMD_MicrosoftProject                  0x0297
#define eWDCMD_ListMacros                        0x0298
#define eWDCMD_ScreenRefresh                     0x0299
#define eWDCMD_ToolsRecordMacroStart             0x029A
#define eWDCMD_ToolsRecordMacroStop              0x029B
#define eWDCMD_StopMacro                         0x029C
#define eWDCMD_ToggleMacroRun                    0x029D
#define eWDCMD_DrawNudgeUp                       0x029E
#define eWDCMD_DrawNudgeDown                     0x029F
#define eWDCMD_DrawNudgeLeft                     0x02A0
#define eWDCMD_DrawNudgeRight                    0x02A1
#define eWDCMD_WW2_ToolsMacro                    0x02A2
#define eWDCMD_MailMergeEditHeaderSource         0x02A3
#define eWDCMD_MailMerge                         0x02A4
#define eWDCMD_MailMergeCheck                    0x02A5
#define eWDCMD_MailMergeToDoc                    0x02A6
#define eWDCMD_MailMergeToPrinter                0x02A7
#define eWDCMD_MailMergeHelper                   0x02A8
#define eWDCMD_MailMergeQueryOptions             0x02A9
#define eWDCMD_InsertWordArt                     0x02AA
#define eWDCMD_InsertEquation                    0x02AB
#define eWDCMD_RunPrintManager                   0x02AC
#define eWDCMD_FileMacPageSetup                  0x02AD
#define eWDCMD_FileConfirmConversions            0x02AF
#define eWDCMD_HelpContents                      0x02B0
#define eWDCMD_WW2_InsertSymbol                  0x02B5
#define eWDCMD_FileClosePicture                  0x02B6
#define eWDCMD_WW2_InsertIndex                   0x02B7
#define eWDCMD_DrawResetWordPicture              0x02B8
#define eWDCMD_WW2_FormatBordersAndShading       0x02B9
#define eWDCMD_OpenOrCloseUpPara                 0x02BA
#define eWDCMD_DrawNudgeUpPixel                  0x02BC
#define eWDCMD_DrawNudgeDownPixel                0x02BD
#define eWDCMD_DrawNudgeLeftPixel                0x02BE
#define eWDCMD_DrawNudgeRightPixel               0x02BF
#define eWDCMD_ToolsHyphenationManual            0x02C0
#define eWDCMD_ClearFormField                    0x02C2
#define eWDCMD_InsertSectionBreak                0x02C3
#define eWDCMD_DrawUnselect                      0x02C4
#define eWDCMD_DrawSelectNext                    0x02C5
#define eWDCMD_DrawSelectPrevious                0x02C6
#define eWDCMD_MicrosoftSystemInfo               0x02C7
#define eWDCMD_ToolsCustomizeToolbar             0x02CC
#define eWDCMD_ListCommands                      0x02D3
#define eWDCMD_EditCreatePublisher               0x02DC
#define eWDCMD_EditSubscribeTo                   0x02DD
#define eWDCMD_EditPubOrSubOptions               0x02DE
#define eWDCMD_EditPublishOptions                0x02DF
#define eWDCMD_EditSubscribeOptions              0x02E0
#define eWDCMD_FileMacCustomPageSetupGX          0x02E1
#define eWDCMD_Highlight                         0x02E6
#define eWDCMD_FixSpellingChange                 0x02E8
#define eWDCMD_FileProperties                    0x02EE
#define eWDCMD_EditCopyAsPicture                 0x02EF
#define eWDCMD_InsertAddress                     0x02F6
#define eWDCMD_NextMisspelling                   0x02F7
#define eWDCMD_FilePost                          0x02F8
#define eWDCMD_ToolsAutoCorrectExceptions        0x02FA
#define eWDCMD_MailHideMessageHeader             0x02FB
#define eWDCMD_MailMessageProperties             0x02FC
#define eWDCMD_ToolsAutoCorrectCapsLockOff       0x02FF
#define eWDCMD_MailMessageReply                  0x0300
#define eWDCMD_MailMessageReplyAll               0x0301
#define eWDCMD_MailMessageMove                   0x0302
#define eWDCMD_MailMessageDelete                 0x0303
#define eWDCMD_MailMessagePrevious               0x0304
#define eWDCMD_MailMessageNext                   0x0305
#define eWDCMD_MailCheckNames                    0x0306
#define eWDCMD_MailSelectNames                   0x0307
#define eWDCMD_MailMessageForward                0x0308
#define eWDCMD_ToolsSpellingRecheckDocument      0x0309
#define eWDCMD_MailMergeUseAddressBook           0x030B
#define eWDCMD_EditFindHighlight                 0x030C
#define eWDCMD_EditReplaceHighlight              0x030D
#define eWDCMD_EditFindNotHighlight              0x030E
#define eWDCMD_EditReplaceNotHighlight           0x030F
#define eWDCMD_2Help                             0x031A
#define eWDCMD_HelpMSN                           0x031B
#define eWDCMD_CharLeft                          0x0FA0
#define eWDCMD_CharRight                         0x0FA1
#define eWDCMD_WordLeft                          0x0FA2
#define eWDCMD_WordRight                         0x0FA3
#define eWDCMD_SentLeft                          0x0FA4
#define eWDCMD_SentRight                         0x0FA5
#define eWDCMD_ParaUp                            0x0FA6
#define eWDCMD_ParaDown                          0x0FA7
#define eWDCMD_LineUp                            0x0FA8
#define eWDCMD_LineDown                          0x0FA9
#define eWDCMD_PageUp                            0x0FAA
#define eWDCMD_PageDown                          0x0FAB
#define eWDCMD_StartOfLine                       0x0FAC
#define eWDCMD_EndOfLine                         0x0FAD
#define eWDCMD_StartOfWindow                     0x0FAE
#define eWDCMD_EndOfWindow                       0x0FAF
#define eWDCMD_StartOfDocument                   0x0FB0
#define eWDCMD_EndOfDocument                     0x0FB1
#define eWDCMD_CharLeftExtend                    0x0FB2
#define eWDCMD_CharRightExtend                   0x0FB3
#define eWDCMD_WordLeftExtend                    0x0FB4
#define eWDCMD_WordRightExtend                   0x0FB5
#define eWDCMD_SentLeftExtend                    0x0FB6
#define eWDCMD_SentRightExtend                   0x0FB7
#define eWDCMD_ParaUpExtend                      0x0FB8
#define eWDCMD_ParaDownExtend                    0x0FB9
#define eWDCMD_LineUpExtend                      0x0FBA
#define eWDCMD_LineDownExtend                    0x0FBB
#define eWDCMD_PageUpExtend                      0x0FBC
#define eWDCMD_PageDownExtend                    0x0FBD
#define eWDCMD_StartOfLineExtend                 0x0FBE
#define eWDCMD_EndOfLineExtend                   0x0FBF
#define eWDCMD_StartOfWindowExtend               0x0FC0
#define eWDCMD_EndOfWindowExtend                 0x0FC1
#define eWDCMD_StartOfDocExtend                  0x0FC2
#define eWDCMD_EndOfDocExtend                    0x0FC3
#define eWDCMD_File1                             0x0FC5
#define eWDCMD_File2                             0x0FC6
#define eWDCMD_File3                             0x0FC7
#define eWDCMD_File4                             0x0FC8
#define eWDCMD_File5                             0x0FC9
#define eWDCMD_File6                             0x0FCA
#define eWDCMD_File7                             0x0FCB
#define eWDCMD_File8                             0x0FCC
#define eWDCMD_File9                             0x0FCD
#define eWDCMD_MailMergeInsertAsk                0x0FCF
#define eWDCMD_MailMergeInsertFillIn             0x0FD0
#define eWDCMD_MailMergeInsertIf                 0x0FD1
#define eWDCMD_MailMergeInsertMergeRec           0x0FD2
#define eWDCMD_MailMergeInsertMergeSeq           0x0FD3
#define eWDCMD_MailMergeInsertNext               0x0FD4
#define eWDCMD_MailMergeInsertNextIf             0x0FD5
#define eWDCMD_MailMergeInsertSet                0x0FD6
#define eWDCMD_MailMergeInsertSkipIf             0x0FD7
#define eWDCMD_BorderTop                         0x0FDE
#define eWDCMD_BorderLeft                        0x0FDF
#define eWDCMD_BorderBottom                      0x0FE0
#define eWDCMD_BorderRight                       0x0FE1
#define eWDCMD_BorderInside                      0x0FE2
#define eWDCMD_TipWizard                         0x0FE3
#define eWDCMD_ShowMe                            0x0FE4
#define eWDCMD_AutomaticChange                   0x0FE6
#define eWDCMD_WW2_FileTemplates                 0x17A6
#define eWDCMD_FormatFillColor                   0x1B8B
#define eWDCMD_FormatLineColor                   0x1B8C
#define eWDCMD_FormatLineStyle                   0x1B8D
#define eWDCMD_Abs                               0x8000
#define eWDCMD_Sgn                               0x8001
#define eWDCMD_Int                               0x8002
#define eWDCMD_Len                               0x8003
#define eWDCMD_Asc                               0x8004
#define eWDCMD_ChrS                              0x8005
#define eWDCMD_Val                               0x8006
#define eWDCMD_StrS                              0x8007
#define eWDCMD_LeftS                             0x8008
#define eWDCMD_RightS                            0x8009
#define eWDCMD_MidS                              0x800A
#define eWDCMD_StringS                           0x800B
#define eWDCMD_DateS                             0x800C
#define eWDCMD_TimeS                             0x800D
#define eWDCMD_Rnd                               0x800E
#define eWDCMD_InStr                             0x800F
#define eWDCMD_2ShowAll                          0x8010
#define eWDCMD_2ColumnSelect                     0x8011
#define eWDCMD_Insert                            0x8012
#define eWDCMD_InsertPara                        0x8013
#define eWDCMD_WW1_InsertPara                    0x8014
#define eWDCMD_SelectionS                        0x8015
#define eWDCMD_GetBookmarkS                      0x8016
#define eWDCMD_CmpBookmarks                      0x8017
#define eWDCMD_CopyBookmark                      0x8018
#define eWDCMD_SetStartOfBookmark                0x8019
#define eWDCMD_SetEndOfBookmark                  0x801A
#define eWDCMD_ExistingBookmark                  0x801B
#define eWDCMD_EmptyBookmark                     0x801C
#define eWDCMD_CountBookmarks                    0x801D
#define eWDCMD_CountMergeFields                  0x801E
#define eWDCMD_BookmarkNameS                     0x801F
#define eWDCMD_MergeFieldNameS                   0x8020
#define eWDCMD_CountStyles                       0x8021
#define eWDCMD_StyleNameS                        0x8022
#define eWDCMD_IsDocumentDirty                   0x8023
#define eWDCMD_SetDocumentDirty                  0x8024
#define eWDCMD_FileNameS                         0x8025
#define eWDCMD_CountFiles                        0x8026
#define eWDCMD_GetAutoTextS                      0x8027
#define eWDCMD_CountAutoTextEntries              0x8028
#define eWDCMD_AutoTextNameS                     0x8029
#define eWDCMD_SetAutoText                       0x802A
#define eWDCMD_MsgBox                            0x802B
#define eWDCMD_Beep                              0x802C
#define eWDCMD_Shell                             0x802D
#define eWDCMD_2ResetChar                        0x802E
#define eWDCMD_2ResetPara                        0x802F
#define eWDCMD_TabType                           0x8030
#define eWDCMD_TabLeaderS                        0x8031
#define eWDCMD_2DocMove                          0x8032
#define eWDCMD_2DocSize                          0x8033
#define eWDCMD_VLine                             0x8034
#define eWDCMD_HLine                             0x8035
#define eWDCMD_VPage                             0x8036
#define eWDCMD_HPage                             0x8037
#define eWDCMD_VScroll                           0x8038
#define eWDCMD_HScroll                           0x8039
#define eWDCMD_CountWindows                      0x803A
#define eWDCMD_WindowNameS                       0x803B
#define eWDCMD_WindowPane                        0x803C
#define eWDCMD_2DocSplit                         0x803D
#define eWDCMD_Window                            0x803E
#define eWDCMD_2AppSize                          0x803F
#define eWDCMD_2AppMove                          0x8040
#define eWDCMD_2AppMinimize                      0x8041
#define eWDCMD_2AppMaximize                      0x8042
#define eWDCMD_2AppRestore                       0x8043
#define eWDCMD_2DocMaximize                      0x8044
#define eWDCMD_GetProfileStringS                 0x8045
#define eWDCMD_SetProfileString                  0x8046
#define eWDCMD_2CharColor                        0x8047
#define eWDCMD_2Bold                             0x8048
#define eWDCMD_2Italic                           0x8049
#define eWDCMD_2SmallCaps                        0x804A
#define eWDCMD_2AllCaps                          0x804B
#define eWDCMD_2Strikethrough                    0x804C
#define eWDCMD_2Hidden                           0x804D
#define eWDCMD_2Underline                        0x804E
#define eWDCMD_2DoubleUnderline                  0x804F
#define eWDCMD_2WordUnderline                    0x8050
#define eWDCMD_2Superscript                      0x8051
#define eWDCMD_2Subscript                        0x8052
#define eWDCMD_2CenterPara                       0x8053
#define eWDCMD_2LeftPara                         0x8054
#define eWDCMD_2RightPara                        0x8055
#define eWDCMD_2JustifyPara                      0x8056
#define eWDCMD_2SpacePara1                       0x8057
#define eWDCMD_2SpacePara15                      0x8058
#define eWDCMD_2SpacePara2                       0x8059
#define eWDCMD_2OpenUpPara                       0x805A
#define eWDCMD_2CloseUpPara                      0x805B
#define eWDCMD_DDEInitiate                       0x805C
#define eWDCMD_DDETerminate                      0x805D
#define eWDCMD_DDETerminateAll                   0x805E
#define eWDCMD_DDEExecute                        0x805F
#define eWDCMD_DDEPoke                           0x8060
#define eWDCMD_DDERequestS                       0x8061
#define eWDCMD_Activate                          0x8062
#define eWDCMD_AppActivate                       0x8063
#define eWDCMD_SendKeys                          0x8064
#define eWDCMD_StyleDescS                        0x8065
#define eWDCMD_2ParaKeepLinesTogether            0x8066
#define eWDCMD_2ParaKeepWithNext                 0x8067
#define eWDCMD_2ParaPageBreakBefore              0x8069
#define eWDCMD_2ParaWidowOrphanControl           0x806A
#define eWDCMD_2ViewFootnotes                    0x806B
#define eWDCMD_2ViewAnnotations                  0x806C
#define eWDCMD_2ViewFieldCodes                   0x806D
#define eWDCMD_2ViewDraft                        0x806E
#define eWDCMD_2ViewStatusBar                    0x806F
#define eWDCMD_2ViewRuler                        0x8070
#define eWDCMD_ViewRibbon                        0x8071
#define eWDCMD_ViewToolbar                       0x8072
#define eWDCMD_2ViewPage                         0x8073
#define eWDCMD_2ViewOutline                      0x8074
#define eWDCMD_2ViewNormal                       0x8075
#define eWDCMD_ViewMenus                         0x8076
#define eWDCMD_2TableGridlines                   0x8077
#define eWDCMD_2OutlineShowFirstLine             0x8078
#define eWDCMD_2Overtype                         0x8079
#define eWDCMD_FontS                             0x807A
#define eWDCMD_CountFonts                        0x807B
#define eWDCMD_2Font                             0x807C
#define eWDCMD_2FontSize                         0x807D
#define eWDCMD_LanguageS                         0x807E
#define eWDCMD_CountLanguages                    0x807F
#define eWDCMD_Language                          0x8080
#define eWDCMD_WW6_EditClear                     0x8081
#define eWDCMD_FileList                          0x8082
#define eWDCMD_2File1                            0x8083
#define eWDCMD_2File2                            0x8084
#define eWDCMD_2File3                            0x8085
#define eWDCMD_2File4                            0x8086
#define eWDCMD_2File5                            0x8087
#define eWDCMD_2File6                            0x8088
#define eWDCMD_2File7                            0x8089
#define eWDCMD_2File8                            0x808A
#define eWDCMD_2File9                            0x808B
#define eWDCMD_wdToolsGetSpelling                0x8090
#define eWDCMD_wdToolsGetSynonyms                0x8091
#define eWDCMD_2NextPage                         0x8094
#define eWDCMD_2PrevPage                         0x8095
#define eWDCMD_2NextObject                       0x8096
#define eWDCMD_2PrevObject                       0x8097
#define eWDCMD_2ExtendSelection                  0x8098
#define eWDCMD_ExtendMode                        0x8099
#define eWDCMD_SelType                           0x809A
#define eWDCMD_OutlineLevel                      0x809B
#define eWDCMD_NextTab                           0x809C
#define eWDCMD_PrevTab                           0x809D
#define eWDCMD_DisableInput                      0x809E
#define eWDCMD_2DocClose                         0x809F
#define eWDCMD_2FileClose                        0x80A0
#define eWDCMD_FilesS                            0x80A1
#define eWDCMD_2FileExit                         0x80A2
#define eWDCMD_2FileSaveAll                      0x80A3
#define eWDCMD_2FilePrintPreview                 0x80A4
#define eWDCMD_FilePrintPreviewPages             0x80A6
#define eWDCMD_InputS                            0x80A7
#define eWDCMD_Seek                              0x80A8
#define eWDCMD_Eof                               0x80A9
#define eWDCMD_Lof                               0x80AA
#define eWDCMD_Kill                              0x80AB
#define eWDCMD_ChDir                             0x80AC
#define eWDCMD_MkDir                             0x80AD
#define eWDCMD_RmDir                             0x80AE
#define eWDCMD_UCaseS                            0x80AF
#define eWDCMD_LCaseS                            0x80B0
#define eWDCMD_InputBoxS                         0x80B1
#define eWDCMD_WW2_RenameMenu                    0x80B2
#define eWDCMD_OnTime                            0x80B3
#define eWDCMD_2ChangeCase                       0x80B4
#define eWDCMD_AppInfoS                          0x80B5
#define eWDCMD_SelInfo                           0x80B6
#define eWDCMD_CountMacros                       0x80B7
#define eWDCMD_MacroNameS                        0x80B8
#define eWDCMD_CountFoundFiles                   0x80B9
#define eWDCMD_FoundFileNameS                    0x80BA
#define eWDCMD_WW2_CountMenuItems                0x80BB
#define eWDCMD_WW2_MenuMacroS                    0x80BC
#define eWDCMD_WW2_MenuTextS                     0x80BD
#define eWDCMD_MacroDescS                        0x80BE
#define eWDCMD_CountKeys                         0x80BF
#define eWDCMD_KeyCode                           0x80C0
#define eWDCMD_KeyMacroS                         0x80C1
#define eWDCMD_MacroCopy                         0x80C2
#define eWDCMD_IsExecuteOnly                     0x80C3
#define eWDCMD_CommandValid                      0x80C4
#define eWDCMD_WW2_GetToolButton                 0x80C5
#define eWDCMD_WW2_GetToolMacroS                 0x80C6
#define eWDCMD_OKButton                          0x80C7
#define eWDCMD_CancelButton                      0x80C8
#define eWDCMD_Text                              0x80C9
#define eWDCMD_GroupBox                          0x80CA
#define eWDCMD_OptionButton                      0x80CB
#define eWDCMD_PushButton                        0x80CC
#define eWDCMD_2NextField                        0x80CD
#define eWDCMD_2PrevField                        0x80CE
#define eWDCMD_2NextCell                         0x80CF
#define eWDCMD_2PrevCell                         0x80D0
#define eWDCMD_2StartOfRow                       0x80D1
#define eWDCMD_2EndOfRow                         0x80D2
#define eWDCMD_2StartOfColumn                    0x80D3
#define eWDCMD_2EndOfColumn                      0x80D4
#define eWDCMD_ExitWindows                       0x80D5
#define eWDCMD_DisableAutoMacros                 0x80D6
#define eWDCMD_EditFindFound                     0x80D7
#define eWDCMD_CheckBox                          0x80D8
#define eWDCMD_TextBox                           0x80D9
#define eWDCMD_ListBox                           0x80DA
#define eWDCMD_OptionGroup                       0x80DB
#define eWDCMD_ComboBox                          0x80DC
#define eWDCMD_2ToolsCalculate                   0x80DD
#define eWDCMD_2WindowList                       0x80DE
#define eWDCMD_Window1                           0x80DF
#define eWDCMD_Window2                           0x80E0
#define eWDCMD_Window3                           0x80E1
#define eWDCMD_Window4                           0x80E2
#define eWDCMD_Window5                           0x80E3
#define eWDCMD_Window6                           0x80E4
#define eWDCMD_Window7                           0x80E5
#define eWDCMD_Window8                           0x80E6
#define eWDCMD_Window9                           0x80E7
#define eWDCMD_CountDirectories                  0x80E8
#define eWDCMD_GetDirectoryS                     0x80E9
#define eWDCMD_LTrimS                            0x80EA
#define eWDCMD_RTrimS                            0x80EB
#define eWDCMD_EnvironS                          0x80EE
#define eWDCMD_WaitCursor                        0x80EF
#define eWDCMD_DateSerial                        0x80F0
#define eWDCMD_DateValue                         0x80F1
#define eWDCMD_Day                               0x80F2
#define eWDCMD_Days360                           0x80F3
#define eWDCMD_Hour                              0x80F4
#define eWDCMD_Minute                            0x80F5
#define eWDCMD_Month                             0x80F6
#define eWDCMD_Now                               0x80F7
#define eWDCMD_Weekday                           0x80F8
#define eWDCMD_Year                              0x80F9
#define eWDCMD_DocWindowHeight                   0x80FA
#define eWDCMD_DocWindowWidth                    0x80FB
#define eWDCMD_DOSToWinS                         0x80FC
#define eWDCMD_WinToDOSS                         0x80FD
#define eWDCMD_TimeSerial                        0x80FE
#define eWDCMD_Second                            0x80FF
#define eWDCMD_TimeValue                         0x8100
#define eWDCMD_Today                             0x8101
#define eWDCMD_ShowAnnotationBy                  0x8102
#define eWDCMD_SetAttr                           0x8103
#define eWDCMD_2DocMinimize                      0x8105
#define eWDCMD_GetSystemInfo                     0x8106
#define eWDCMD_AppClose                          0x8107
#define eWDCMD_AppCount                          0x8108
#define eWDCMD_AppGetNames                       0x8109
#define eWDCMD_AppHide                           0x810A
#define eWDCMD_AppIsRunning                      0x810B
#define eWDCMD_GetSystemInfoS                    0x810C
#define eWDCMD_GetPrivateProfileStringS          0x810D
#define eWDCMD_SetPrivateProfileString           0x810E
#define eWDCMD_GetAttr                           0x810F
#define eWDCMD_AppSendMessage                    0x8110
#define eWDCMD_ScreenUpdating                    0x8111
#define eWDCMD_AppWindowPosTop                   0x8112
#define eWDCMD_2Style                            0x8113
#define eWDCMD_MailMergeDataSourceS              0x8114
#define eWDCMD_MailMergeState                    0x8115
#define eWDCMD_SelectCurWord                     0x8116
#define eWDCMD_SelectCurSentence                 0x8117
#define eWDCMD_IsTemplateDirty                   0x8118
#define eWDCMD_SetTemplateDirty                  0x8119
#define eWDCMD_2ToolsAutoCorrectSmartQuotes      0x811A
#define eWDCMD_DlgEnable                         0x811B
#define eWDCMD_DlgUpdateFilePreview              0x811C
#define eWDCMD_DlgVisible                        0x811D
#define eWDCMD_DlgValue                          0x811E
#define eWDCMD_DlgTextS                          0x811F
#define eWDCMD_DlgFocusS                         0x8120
#define eWDCMD_AppShow                           0x8121
#define eWDCMD_DlgListBoxArray                   0x8122
#define eWDCMD_DlgControlId                      0x8123
#define eWDCMD_2ViewEndnoteArea                  0x8124
#define eWDCMD_Picture                           0x8125
#define eWDCMD_DlgSetPicture                     0x8126
#define eWDCMD_WW2_ChangeCase                    0x8127
#define eWDCMD_MailMergeMainDocumentType         0x8128
#define eWDCMD_WW2_FilesS                        0x8131
#define eWDCMD_CountToolsGrammarStatistics       0x8132
#define eWDCMD_2DottedUnderline                  0x8133
#define eWDCMD_ToolsGrammarStatisticsArray       0x8134
#define eWDCMD_FilePreview                       0x8135
#define eWDCMD_DlgFilePreviewS                   0x8136
#define eWDCMD_DlgText                           0x8137
#define eWDCMD_DlgFocus                          0x8138
#define eWDCMD_2MailMergeGoToRecord              0x8139
#define eWDCMD_2BorderLineStyle                  0x813B
#define eWDCMD_2ShadingPattern                   0x813C
#define eWDCMD_MenuItemTextS                     0x813D
#define eWDCMD_MenuItemMacroS                    0x813E
#define eWDCMD_CountMenus                        0x813F
#define eWDCMD_MenuTextS                         0x8140
#define eWDCMD_CountMenuItems                    0x8141
#define eWDCMD_AppWindowPosLeft                  0x8142
#define eWDCMD_AppWindowHeight                   0x8143
#define eWDCMD_AppWindowWidth                    0x8144
#define eWDCMD_DocWindowPosTop                   0x8145
#define eWDCMD_DocWindowPosLeft                  0x8146
#define eWDCMD_Stop                              0x8147
#define eWDCMD_DropListBox                       0x8148
#define eWDCMD_RenameMenu                        0x8149
#define eWDCMD_2FileCloseAll                     0x814A
#define eWDCMD_SortArray                         0x814B
#define eWDCMD_SetDocumentVar                    0x814C
#define eWDCMD_GetDocumentVarS                   0x814D
#define eWDCMD_AnnotationRefFromSelS             0x814E
#define eWDCMD_GetFormResultS                    0x814F
#define eWDCMD_SetFormResult                     0x8150
#define eWDCMD_EnableFormField                   0x8151
#define eWDCMD_IsMacro                           0x8152
#define eWDCMD_FileNameFromWindowS               0x8153
#define eWDCMD_MacroNameFromWindowS              0x8154
#define eWDCMD_GetFieldDataS                     0x8155
#define eWDCMD_PutFieldData                      0x8156
#define eWDCMD_2MailMergeConvertChevrons         0x8157
#define eWDCMD_2MailMergeAskToConvertChevrons    0x8158
#define eWDCMD_2AutoMarkIndexEntries             0x815A
#define eWDCMD_MoveToolbar                       0x815B
#define eWDCMD_SizeToolbar                       0x815C
#define eWDCMD_DrawSetRange                      0x815D
#define eWDCMD_MountVolume                       0x815E
#define eWDCMD_DrawClearRange                    0x815F
#define eWDCMD_DrawCount                         0x8160
#define eWDCMD_DrawSelect                        0x8161
#define eWDCMD_DrawExtendSelect                  0x8162
#define eWDCMD_DrawSetInsertToTextbox            0x8163
#define eWDCMD_DrawSetInsertToAnchor             0x8164
#define eWDCMD_DrawGetType                       0x8165
#define eWDCMD_DrawCountPolyPoints               0x8166
#define eWDCMD_DrawGetPolyPoints                 0x8167
#define eWDCMD_DrawSetPolyPoints                 0x8168
#define eWDCMD_DrawGetCalloutTextbox             0x8169
#define eWDCMD_DrawSetCalloutTextbox             0x816A
#define eWDCMD_2Magnifier                        0x816B
#define eWDCMD_MacScript                         0x816C
#define eWDCMD_MacScriptS                        0x816D
#define eWDCMD_MacIDS                            0x816E
#define eWDCMD_GetSelStartPos                    0x816F
#define eWDCMD_GetSelEndPos                      0x8170
#define eWDCMD_SetSelRange                       0x8171
#define eWDCMD_GetTextS                          0x8172
#define eWDCMD_MoveButton                        0x8173
#define eWDCMD_DeleteButton                      0x8174
#define eWDCMD_AddButton                         0x8175
#define eWDCMD_wdAddCommand                      0x8176
#define eWDCMD_DeleteAddIn                       0x8177
#define eWDCMD_AddAddIn                          0x8178
#define eWDCMD_GetAddInNameS                     0x8179
#define eWDCMD_2FormatBulletDefault              0x817A
#define eWDCMD_2FormatNumberDefault              0x817B
#define eWDCMD_2ResetButtonImage                 0x817C
#define eWDCMD_DlgFilePreview                    0x817D
#define eWDCMD_2SkipNumbering                    0x817E
#define eWDCMD_wdGetInst                         0x817F
#define eWDCMD_GetAddInId                        0x8180
#define eWDCMD_CountAddIns                       0x8181
#define eWDCMD_ClearAddIns                       0x8182
#define eWDCMD_AddInState                        0x8183
#define eWDCMD_ToolsRevisionType                 0x8184
#define eWDCMD_ToolsRevisionAuthorS              0x8185
#define eWDCMD_ToolsRevisionDate                 0x8186
#define eWDCMD_2MailMergeViewData                0x8187
#define eWDCMD_AddDropDownItem                   0x8188
#define eWDCMD_RemoveDropDownItem                0x8189
#define eWDCMD_ToolsRevisionDateS                0x818A
#define eWDCMD_2TableHeadings                    0x818B
#define eWDCMD_DefaultDirS                       0x818C
#define eWDCMD_FileNameInfoS                     0x818D
#define eWDCMD_MacroFileNameS                    0x818E
#define eWDCMD_2ViewHeader                       0x818F
#define eWDCMD_2ViewFooter                       0x8190
#define eWDCMD_2PasteButtonImage                 0x8191
#define eWDCMD_2CopyButtonImage                  0x8192
#define eWDCMD_2EditButtonImage                  0x8194
#define eWDCMD_CountToolbars                     0x8195
#define eWDCMD_ToolbarNameS                      0x8196
#define eWDCMD_ChDefaultDir                      0x8198
#define eWDCMD_2EditUndo                         0x8199
#define eWDCMD_2EditRedo                         0x819A
#define eWDCMD_2ViewMasterDocument               0x819B
#define eWDCMD_2ToolsAutoCorrectReplaceText      0x819C
#define eWDCMD_2ToolsAutoCorrectInitialCaps      0x819D
#define eWDCMD_2ToolsAutoCorrectSentenceCaps     0x819E
#define eWDCMD_2ToolsAutoCorrectDays             0x819F
#define eWDCMD_GetAutoCorrectS                   0x81A0
#define eWDCMD_2ViewFootnoteArea                 0x81A1
#define eWDCMD_FileQuit                          0x81A2
#define eWDCMD_ConverterLookup                   0x81A3
#define eWDCMD_2FileConfirmConversions           0x81A4
#define eWDCMD_GetMergeFieldS                    0x81A5
#define eWDCMD_ConverterS                        0x81A6
#define eWDCMD_MailMergeFoundRecord              0x81A7
#define eWDCMD_CountDocumentVars                 0x81A8
#define eWDCMD_GetDocumentVarNameS               0x81A9
#define eWDCMD_wdPrint                           0x81AA
#define eWDCMD_CleanStringS                      0x81AB
#define eWDCMD_PathFromWinPathS                  0x81AC
#define eWDCMD_PathFromMacPathS                  0x81AD
#define eWDCMD_2LockDocument                     0x81D0
#define eWDCMD_GoToNextSubdocument               0x81D1
#define eWDCMD_GoToPreviousSubdocument           0x81D2
#define eWDCMD_SelectionFileNameS                0x81D3
#define eWDCMD_2SymbolFont                       0x81D4
#define eWDCMD_RemoveAllDropDownItems            0x81D5
#define eWDCMD_2FormShading                      0x81D6
#define eWDCMD_GetFormResult                     0x81D7
#define eWDCMD_ToolbarState                      0x81D8
#define eWDCMD_CountToolbarButtons               0x81D9
#define eWDCMD_ToolbarButtonMacroS               0x81DA
#define eWDCMD_WW2_Insert                        0x81DB
#define eWDCMD_AtEndOfDocument                   0x81DC
#define eWDCMD_AtStartOfDocument                 0x81DD
#define eWDCMD_WW2_KeyCode                       0x81DE
#define eWDCMD_FieldSeparatorS                   0x81E0
#define eWDCMD_wdGetHwnd                         0x81E1
#define eWDCMD_WW2CallingConvention              0x81E2
#define eWDCMD_wdSetTimer                        0x81E3
#define eWDCMD_AOCEAddRecipient                  0x81E4
#define eWDCMD_AOCECountRecipients               0x81E5
#define eWDCMD_AOCEGetRecipientS                 0x81E6
#define eWDCMD_AOCEGetSenderS                    0x81E7
#define eWDCMD_AOCEGetSubjectS                   0x81E9
#define eWDCMD_AOCESetSubject                    0x81EA
#define eWDCMD_AOCESendMail                      0x81EB
#define eWDCMD_AOCEAuthenticateUser              0x81EC
#define eWDCMD_AOCEClearMailerField              0x81ED
#define eWDCMD_wdOpenWindow                      0x81EE
#define eWDCMD_wdCloseWindow                     0x81EF
#define eWDCMD_wdCaptureKeyDown                  0x81F0
#define eWDCMD_wdReleaseKeyDown                  0x81F1
#define eWDCMD_Shadow                            0x81F2
#define eWDCMD_Outline                           0x81F3
#define eWDCMD_FileTypeS                         0x81F4
#define eWDCMD_FileCreatorS                      0x81F5
#define eWDCMD_SetFileCreatorAndType             0x81F6
#define eWDCMD_DlgStoreValues                    0x81F7
#define eWDCMD_DlgLoadValues                     0x81F8
#define eWDCMD_DocumentHasMisspellings           0x81F9
#define eWDCMD_GetAddressS                       0x81FA
#define eWDCMD_CountDocumentProperties           0x81FB
#define eWDCMD_GetDocumentPropertyS              0x81FC
#define eWDCMD_GetDocumentProperty               0x81FD
#define eWDCMD_SetDocumentProperty               0x81FE
#define eWDCMD_SetDocumentPropertyLink           0x81FF
#define eWDCMD_DeleteDocumentProperty            0x8200
#define eWDCMD_DocumentPropertyNameS             0x8201
#define eWDCMD_IsDocumentPropertyReadOnly        0x8202
#define eWDCMD_IsCustomDocumentProperty          0x8203
#define eWDCMD_DocumentPropertyExists            0x8204
#define eWDCMD_DocumentPropertyType              0x8205
#define eWDCMD_2FilePost                         0x8206
#define eWDCMD_AddAddress                        0x8208
#define eWDCMD_2ToolsAutoCorrectCapsLockOff      0x8209
#define eWDCMD_CountAutoCorrectExceptions        0x820A
#define eWDCMD_GetAutoCorrectExceptionS          0x820B
#define eWDCMD_IsAutoCorrectException            0x820C
#define eWDCMD_SpellChecked                      0x820E
#define eWDCMD_2CharLeft                         0xC000
#define eWDCMD_2CharRight                        0xC001
#define eWDCMD_2WordLeft                         0xC002
#define eWDCMD_2WordRight                        0xC003
#define eWDCMD_2SentLeft                         0xC004
#define eWDCMD_2SentRight                        0xC005
#define eWDCMD_2ParaUp                           0xC006
#define eWDCMD_2ParaDown                         0xC007
#define eWDCMD_2LineUp                           0xC008
#define eWDCMD_2LineDown                         0xC009
#define eWDCMD_2PageUp                           0xC00A
#define eWDCMD_2PageDown                         0xC00B
#define eWDCMD_2StartOfLine                      0xC00C
#define eWDCMD_2EndOfLine                        0xC00D
#define eWDCMD_2StartOfWindow                    0xC00E
#define eWDCMD_2EndOfWindow                      0xC00F
#define eWDCMD_2StartOfDocument                  0xC010
#define eWDCMD_2EndOfDocument                    0xC011
#define eWDCMD_2EditClear                        0xC012
#define eWDCMD_2BorderOutside                    0xC018
#define eWDCMD_2BorderNone                       0xC019
#define eWDCMD_2BorderTop                        0xC024
#define eWDCMD_2BorderLeft                       0xC025
#define eWDCMD_2BorderBottom                     0xC026
#define eWDCMD_2BorderRight                      0xC027
#define eWDCMD_2BorderInside                     0xC028
#define eWDCMD_HighlightColor                    0xC029
#define eWDCMD_DocumentProtection                0xC02B
#define eWDCMD_BatchMode                         0xE008

// Far East Only Commands

#define eWDCMD_FormatTextFlow                    0x0234
#define eWDCMD_ToolsOptionsTypography            0x02E3
#define eWDCMD_BytePos                           0x81AE
#define eWDCMD_ChangeByte                        0x7E42
#define eWDCMD_ChangeKana                        0x81BF
#define eWDCMD_DistributePara                    0x81BC
#define eWDCMD_DrawVerticalTextbox               0x02E2
#define eWDCMD_EditUpdateIMEDic                  0x027E
#define eWDCMD_HanCharSetS                       0x81AF
#define eWDCMD_HelpIchitaroHelp                  0x02D8
#define eWDCMD_IMEControl                        0x81B8
#define eWDCMD_IndentChar                        0x02CF
#define eWDCMD_IndentFirstChar                   0x02D1
#define eWDCMD_InputBS                           0x81B0
#define eWDCMD_InStrB                            0x7E4F
#define eWDCMD_LeftBS                            0x81B2
#define eWDCMD_LenB                              0x81B3
#define eWDCMD_MidBS                             0x81B4
#define eWDCMD_RightBS                           0x81B5
#define eWDCMD_RomanCharSetS                     0x81B6
#define eWDCMD_StrConvS                          0x81B7
#define eWDCMD_UnIndentChar                      0x02D0
#define eWDCMD_UnIndentFirstChar                 0x02D2
#define eWDCMD_ViewGridlines                     0x81BD

typedef WORD EWDCMD_T, FAR *LPEWDCMD;

// WordBasic dialog fields

typedef WORD tagEWDDLG;

#define eWDDLG_Name                              0x0001
#define eWDDLG_KeyCode                           0x0002
#define eWDDLG_Context                           0x0003
#define eWDDLG_ResetAll                          0x0004
#define eWDDLG_Menu                              0x0007
#define eWDDLG_MenuText                          0x0008
#define eWDDLG_APPUSERNAME                       0x0009
#define eWDDLG_APPORGANIZATION                   0x000A
#define eWDDLG_Delete                            0x000B
#define eWDDLG_SortBy                            0x000C
#define eWDDLG_SavedBy                           0x0012
#define eWDDLG_DateCreatedFrom                   0x0014
#define eWDDLG_DateCreatedTo                     0x0015
#define eWDDLG_DateSavedFrom                     0x0016
#define eWDDLG_DateSavedTo                       0x0017
#define eWDDLG_APPNAME                           0x0019
#define eWDDLG_ButtonFieldClicks                 0x0020
#define eWDDLG_Font                              0x0021
#define eWDDLG_Points                            0x0022
#define eWDDLG_Color                             0x0023
#define eWDDLG_Bold                              0x0024
#define eWDDLG_Italic                            0x0025
#define eWDDLG_Hidden                            0x0027
#define eWDDLG_Underline                         0x0028
#define eWDDLG_Outline                           0x0029
#define eWDDLG_Position                          0x002B
#define eWDDLG_Define                            0x002C
#define eWDDLG_Spacing                           0x002D
#define eWDDLG_Merge                             0x002E
#define eWDDLG_Printer                           0x002F
#define eWDDLG_ContSeparator                     0x0031
#define eWDDLG_ContNotice                        0x0032
#define eWDDLG_AutoSave                          0x0034
#define eWDDLG_Units                             0x0035
#define eWDDLG_Pagination                        0x0036
#define eWDDLG_SummaryPrompt                     0x0037
#define eWDDLG_Initials                          0x0039
#define eWDDLG_Tabs                              0x003A
#define eWDDLG_Spaces                            0x003B
#define eWDDLG_Paras                             0x003C
#define eWDDLG_Hyphens                           0x003D
#define eWDDLG_ShowAll                           0x003E
#define eWDDLG_TextBoundaries                    0x0041
#define eWDDLG_HScroll                           0x0042
#define eWDDLG_VScroll                           0x0043
#define eWDDLG_TableGridlines                    0x0044
#define eWDDLG_StyleAreaWidth                    0x0045
#define eWDDLG_PageWidth                         0x0046
#define eWDDLG_PageHeight                        0x0047
#define eWDDLG_DefTabs                           0x0048
#define eWDDLG_TopMargin                         0x0049
#define eWDDLG_BottomMargin                      0x004A
#define eWDDLG_LeftMargin                        0x004B
#define eWDDLG_RightMargin                       0x004C
#define eWDDLG_Gutter                            0x004D
#define eWDDLG_FacingPages                       0x004E
#define eWDDLG_FootnotesAt                       0x004F
#define eWDDLG_StartingNum                       0x0050
#define eWDDLG_RestartNum                        0x0051
#define eWDDLG_Template                          0x0052
#define eWDDLG_WidowControl                      0x0053
#define eWDDLG_APPCOPYRIGHT                      0x0055
#define eWDDLG_APPSERIALNUMBER                   0x0056
#define eWDDLG_RecentFileCount                   0x0059
#define eWDDLG_SmallCaps                         0x005D
#define eWDDLG_New                               0x005E
#define eWDDLG_AddToTemplate                     0x005F
#define eWDDLG_Password                          0x0060
#define eWDDLG_RecentFiles                       0x0061
#define eWDDLG_Title                             0x0062
#define eWDDLG_Subject                           0x0063
#define eWDDLG_Author                            0x0064
#define eWDDLG_Keywords                          0x0065
#define eWDDLG_Comments                          0x0066
#define eWDDLG_FileName                          0x0067
#define eWDDLG_Directory                         0x0068
#define eWDDLG_CreateDate                        0x0069
#define eWDDLG_LastSavedDate                     0x006A
#define eWDDLG_LastSavedBy                       0x006B
#define eWDDLG_RevisionNumber                    0x006C
#define eWDDLG_EditTime                          0x006D
#define eWDDLG_LastPrintedDate                   0x006E
#define eWDDLG_NumPages                          0x006F
#define eWDDLG_NumWords                          0x0070
#define eWDDLG_NumChars                          0x0071
#define eWDDLG_Set                               0x0073
#define eWDDLG_Rename                            0x0074
#define eWDDLG_NewName                           0x0075
#define eWDDLG_PrintBarCode                      0x0076
#define eWDDLG_SmartQuotes                       0x0078
#define eWDDLG_BasedOn                           0x007B
#define eWDDLG_NextStyle                         0x007C
#define eWDDLG_CountBy                           0x007D
#define eWDDLG_Source                            0x007F
#define eWDDLG_Reference                         0x0080
#define eWDDLG_Insert                            0x0085
#define eWDDLG_Destination                       0x0086
#define eWDDLG_Type                              0x0087
#define eWDDLG_NumFormat                         0x0088
#define eWDDLG_HeaderDistance                    0x0089
#define eWDDLG_FooterDistance                    0x008A
#define eWDDLG_FirstPage                         0x008B
#define eWDDLG_OddAndEvenPages                   0x008C
#define eWDDLG_HyphenateCaps                     0x008D
#define eWDDLG_Confirm                           0x008E
#define eWDDLG_HotZone                           0x008F
#define eWDDLG_HeadingSeparator                  0x0090
#define eWDDLG_Entry                             0x0091
#define eWDDLG_Range                             0x0092
#define eWDDLG_Field                             0x0094
#define eWDDLG_Link                              0x0095
#define eWDDLG_Add                               0x0098
#define eWDDLG_IgnoreAllCaps                     0x009A
#define eWDDLG_NewTemplate                       0x009B
#define eWDDLG_ReadOnly                          0x009F
#define eWDDLG_Alignment                         0x00A0
#define eWDDLG_LeftIndent                        0x00A1
#define eWDDLG_RightIndent                       0x00A2
#define eWDDLG_FirstIndent                       0x00A3
#define eWDDLG_Before                            0x00A4
#define eWDDLG_After                             0x00A5
#define eWDDLG_LineSpacing                       0x00A6
#define eWDDLG_Style                             0x00A7
#define eWDDLG_KeepTogether                      0x00A8
#define eWDDLG_KeepWithNext                      0x00A9
#define eWDDLG_Border                            0x00AA
#define eWDDLG_Shading                           0x00AB
#define eWDDLG_PageBreak                         0x00AC
#define eWDDLG_NoLineNum                         0x00AD
#define eWDDLG_ScaleY                            0x00B1
#define eWDDLG_ScaleX                            0x00B2
#define eWDDLG_CropTop                           0x00B3
#define eWDDLG_CropLeft                          0x00B4
#define eWDDLG_CropBottom                        0x00B5
#define eWDDLG_CropRight                         0x00B6
#define eWDDLG_Level                             0x00B8
#define eWDDLG_NumCopies                         0x00B9
#define eWDDLG_From                              0x00BA
#define eWDDLG_To                                0x00BB
#define eWDDLG_Reverse                           0x00BC
#define eWDDLG_Draft                             0x00BD
#define eWDDLG_UpdateFields                      0x00BE
#define eWDDLG_Summary                           0x00C0
#define eWDDLG_Annotations                       0x00C1
#define eWDDLG_ShowHidden                        0x00C2
#define eWDDLG_ShowCodes                         0x00C3
#define eWDDLG_Fields                            0x00C7
#define eWDDLG_NumParas                          0x00C8
#define eWDDLG_StartAt                           0x00C9
#define eWDDLG_Format                            0x00CB
#define eWDDLG_Search                            0x00CC
#define eWDDLG_Replace                           0x00CD
#define eWDDLG_WholeWord                         0x00CE
#define eWDDLG_MatchCase                         0x00CF
#define eWDDLG_MarkRevisions                     0x00D0
#define eWDDLG_RevisionBar                       0x00D1
#define eWDDLG_NewText                           0x00D2
#define eWDDLG_AcceptRevisions                   0x00D3
#define eWDDLG_UndoRevisions                     0x00D4
#define eWDDLG_CreateBackup                      0x00D7
#define eWDDLG_LockAnnot                         0x00D8
#define eWDDLG_Direction                         0x00D9
#define eWDDLG_Columns                           0x00DA
#define eWDDLG_ColumnSpacing                     0x00DB
#define eWDDLG_ColLine                           0x00DC
#define eWDDLG_SectionStart                      0x00DD
#define eWDDLG_LineNum                           0x00DF
#define eWDDLG_FromText                          0x00E0
#define eWDDLG_NumMode                           0x00E2
#define eWDDLG_VertAlign                         0x00E3
#define eWDDLG_Order                             0x00E6
#define eWDDLG_Separator                         0x00E7
#define eWDDLG_FieldNum                          0x00E8
#define eWDDLG_SortColumn                        0x00E9
#define eWDDLG_CaseSensitive                     0x00EA
#define eWDDLG_ColumnWidth                       0x00EC
#define eWDDLG_AlwaysSuggest                     0x00F0
#define eWDDLG_EnvWidth                          0x00F4
#define eWDDLG_EnvHeight                         0x00F5
#define eWDDLG_Anchors                           0x00F7
#define eWDDLG_UseEnvFeeder                      0x00F8
#define eWDDLG_PrintFIMA                         0x00F9
#define eWDDLG_Align                             0x00FB
#define eWDDLG_Leader                            0x00FC
#define eWDDLG_Clear                             0x00FD
#define eWDDLG_ClearAll                          0x00FE
#define eWDDLG_SuggestFromMainDictOnly           0x00FF
#define eWDDLG_Paragraphs                        0x0100
#define eWDDLG_ResetIgnoreAll                    0x0101
#define eWDDLG_AutomaticSpellChecking            0x0104
#define eWDDLG_Horizontal                        0x0106
#define eWDDLG_Vertical                          0x0108
#define eWDDLG_ShiftCells                        0x010D
#define eWDDLG_SpaceBetweenCols                  0x010F
#define eWDDLG_TopBorder                         0x0113
#define eWDDLG_BottomBorder                      0x0114
#define eWDDLG_LeftBorder                        0x0116
#define eWDDLG_RightBorder                       0x0117
#define eWDDLG_ApplyTo                           0x0119
#define eWDDLG_NumColumns                        0x011A
#define eWDDLG_NumRows                           0x011B
#define eWDDLG_InitialColWidth                   0x011C
#define eWDDLG_ConvertFrom                       0x011D
#define eWDDLG_ConvertTo                         0x011E
#define eWDDLG_UsrDlg                            0x011F
#define eWDDLG_UpdateLinks                       0x012B
#define eWDDLG_Update                            0x012E
#define eWDDLG_ReplaceSelection                  0x0130
#define eWDDLG_Text                              0x0131
#define eWDDLG_AutoUpdate                        0x0133
#define eWDDLG_Description                       0x0136
#define eWDDLG_Option                            0x0138
#define eWDDLG_Setting                           0x0139
#define eWDDLG_Strikeout                         0x013A
#define eWDDLG_AllCaps                           0x013B
#define eWDDLG_DataType                          0x013E
#define eWDDLG_PasswordDoc                       0x0141
#define eWDDLG_PasswordDot                       0x0142
#define eWDDLG_ZoomPercent                       0x0143
#define eWDDLG_LineSpacingRule                   0x0144
#define eWDDLG_NumRestart                        0x0145
#define eWDDLG_Orientation                       0x0147
#define eWDDLG_Category                          0x0148
#define eWDDLG_ConfirmConversions                0x0149
#define eWDDLG_InsForPaste                       0x014A
#define eWDDLG_Overtype                          0x014B
#define eWDDLG_StatusBar                         0x014C
#define eWDDLG_PicturePlaceHolders               0x014D
#define eWDDLG_FieldCodes                        0x014E
#define eWDDLG_Linebreaks                        0x014F
#define eWDDLG_Show                              0x0150
#define eWDDLG_UpdateMode                        0x0151
#define eWDDLG_Item                              0x0155
#define eWDDLG_FastSaves                         0x0156
#define eWDDLG_SaveInterval                      0x0157
#define eWDDLG_OpenSource                        0x0158
#define eWDDLG_UpdateNow                         0x0159
#define eWDDLG_KillLink                          0x015A
#define eWDDLG_LineColor                         0x0161
#define eWDDLG_TopColor                          0x0163
#define eWDDLG_LeftColor                         0x0164
#define eWDDLG_BottomColor                       0x0165
#define eWDDLG_RightColor                        0x0166
#define eWDDLG_HorizColor                        0x0167
#define eWDDLG_VertColor                         0x0168
#define eWDDLG_HorizBorder                       0x016F
#define eWDDLG_VertBorder                        0x0170
#define eWDDLG_IgnoreMixedDigits                 0x0171
#define eWDDLG_PositionHorz                      0x0175
#define eWDDLG_PositionVert                      0x0178
#define eWDDLG_FixedWidth                        0x017C
#define eWDDLG_Wrap                              0x017D
#define eWDDLG_DistFromText                      0x0180
#define eWDDLG_FixedHeight                       0x0182
#define eWDDLG_AutoFit                           0x0183
#define eWDDLG_CharNum                           0x0184
#define eWDDLG_FullPage                          0x0185
#define eWDDLG_View                              0x018B
#define eWDDLG_Options                           0x0190
#define eWDDLG_Apply                             0x0191
#define eWDDLG_Find                              0x0194
#define eWDDLG_Path                              0x0196
#define eWDDLG_PrevRow                           0x0198
#define eWDDLG_NextRow                           0x0199
#define eWDDLG_NextColumn                        0x019A
#define eWDDLG_PrevColumn                        0x019B
#define eWDDLG_Language                          0x01A4
#define eWDDLG_Foreground                        0x01A7
#define eWDDLG_Background                        0x01A8
#define eWDDLG_SearchPath                        0x01A9
#define eWDDLG_CustomDict1                       0x01AB
#define eWDDLG_CustomDict2                       0x01AC
#define eWDDLG_CustomDict3                       0x01AD
#define eWDDLG_CustomDict4                       0x01AE
#define eWDDLG_Collate                           0x01B1
#define eWDDLG_Shadow                            0x01B2
#define eWDDLG_PrintToFile                       0x01B3
#define eWDDLG_Button                            0x01B4
#define eWDDLG_Macro                             0x01B6
#define eWDDLG_Reset                             0x01B7
#define eWDDLG_Remove                            0x01B9
#define eWDDLG_Protect                           0x01BA
#define eWDDLG_PositionHorzRel                   0x01BC
#define eWDDLG_MoveWithText                      0x01BD
#define eWDDLG_DistVertFromText                  0x01BE
#define eWDDLG_PositionVertRel                   0x01C0
#define eWDDLG_Punctuation                       0x01C5
#define eWDDLG_LinkToFile                        0x01C6
#define eWDDLG_WidthRule                         0x01C7
#define eWDDLG_HeightRule                        0x01C8
#define eWDDLG_Default                           0x01C9
#define eWDDLG_PaperSize                         0x01CB
#define eWDDLG_OtherPages                        0x01CC
#define eWDDLG_ApplyPropsTo                      0x01CF
#define eWDDLG_FormatOutline                     0x01D0
#define eWDDLG_FormatNumber                      0x01D1
#define eWDDLG_Application                       0x01D3
#define eWDDLG_StartNewCol                       0x01D4
#define eWDDLG_ApplyColsTo                       0x01D5
#define eWDDLG_Store                             0x01D7
#define eWDDLG_Class                             0x01DA
#define eWDDLG_Locked                            0x01DB
#define eWDDLG_EnvReturn                         0x01DC
#define eWDDLG_EnvAddress                        0x01DD
#define eWDDLG_Hide                              0x01DE
#define eWDDLG_Toolbar                           0x01DF
#define eWDDLG_ReplaceAll                        0x01E0
#define eWDDLG_Hang                              0x01E1
#define eWDDLG_ShowStatistics                    0x01E2
#define eWDDLG_ReplaceOne                        0x01E3
#define eWDDLG_QueryOptions                      0x01E4
#define eWDDLG_DataSource                        0x01E7
#define eWDDLG_MainDoc                           0x01E8
#define eWDDLG_MergeField                        0x01E9
#define eWDDLG_WordField                         0x01EA
#define eWDDLG_Address                           0x01EB
#define eWDDLG_Indent                            0x01ED
#define eWDDLG_DragAndDrop                       0x01EF
#define eWDDLG_DateTimePic                       0x01F0
#define eWDDLG_CustomZoomPercent                 0x01F1
#define eWDDLG_ResetTool                         0x01F2
#define eWDDLG_SetSize                           0x01F3
#define eWDDLG_SelectedFile                      0x01F4
#define eWDDLG_Run                               0x01F5
#define eWDDLG_Edit                              0x01F6
#define eWDDLG_HeaderSource                      0x01F8
#define eWDDLG_DfltTrueType                      0x01F9
#define eWDDLG_Tool                              0x01FA
#define eWDDLG_HeaderRecord                      0x01FB
#define eWDDLG_Prompt                            0x01FD
#define eWDDLG_RemoveFrame                       0x01FE
#define eWDDLG_EnvPaperSize                      0x01FF
#define eWDDLG_EnvOmitReturn                     0x0200
#define eWDDLG_InsertAsText                      0x0202
#define eWDDLG_EnvFeederInstalled                0x0203
#define eWDDLG_PrintEnvLabel                     0x0204
#define eWDDLG_AddToDocument                     0x0205
#define eWDDLG_RulerStyle                        0x0206
#define eWDDLG_MergeRecords                      0x0215
#define eWDDLG_Suppression                       0x0216
#define eWDDLG_Created                           0x0217
#define eWDDLG_LastSaved                         0x0218
#define eWDDLG_Revision                          0x0219
#define eWDDLG_Time                              0x021A
#define eWDDLG_Printed                           0x021B
#define eWDDLG_Pages                             0x021C
#define eWDDLG_Words                             0x021D
#define eWDDLG_Characters                        0x021E
#define eWDDLG_MergeField1                       0x021F
#define eWDDLG_MergeField2                       0x0220
#define eWDDLG_MergeField3                       0x0221
#define eWDDLG_MergeField4                       0x0222
#define eWDDLG_MergeField5                       0x0223
#define eWDDLG_MergeField6                       0x0224
#define eWDDLG_CompOp1                           0x0225
#define eWDDLG_CompOp2                           0x0226
#define eWDDLG_CompOp3                           0x0227
#define eWDDLG_CompOp4                           0x0228
#define eWDDLG_CompOp5                           0x0229
#define eWDDLG_ComparedTo1                       0x022A
#define eWDDLG_ComparedTo2                       0x022B
#define eWDDLG_ComparedTo3                       0x022C
#define eWDDLG_ComparedTo4                       0x022D
#define eWDDLG_ComparedTo5                       0x022E
#define eWDDLG_ComparedTo6                       0x022F
#define eWDDLG_CompOp6                           0x0230
#define eWDDLG_RemoveAttachments                 0x0231
#define eWDDLG_WPHelp                            0x0232
#define eWDDLG_WPDocNavKeys                      0x0233
#define eWDDLG_SetDesc                           0x0234
#define eWDDLG_SizeX                             0x0235
#define eWDDLG_SizeY                             0x0236
#define eWDDLG_AndOr1                            0x0237
#define eWDDLG_AndOr2                            0x0238
#define eWDDLG_AndOr3                            0x0239
#define eWDDLG_AndOr4                            0x023A
#define eWDDLG_AndOr5                            0x023B
#define eWDDLG_CountFootnotes                    0x023D
#define eWDDLG_DontSortHdr                       0x0248
#define eWDDLG_FieldNum2                         0x0249
#define eWDDLG_Type2                             0x024A
#define eWDDLG_FieldNum3                         0x024B
#define eWDDLG_Type3                             0x024C
#define eWDDLG_WPCommand                         0x0250
#define eWDDLG_Window                            0x0251
#define eWDDLG_FindNext                          0x0252
#define eWDDLG_AddToMru                          0x0255
#define eWDDLG_Lines                             0x0256
#define eWDDLG_SQLStatement                      0x0257
#define eWDDLG_Connection                        0x0258
#define eWDDLG_ColumnNo                          0x025E
#define eWDDLG_EvenlySpaced                      0x025F
#define eWDDLG_Label                             0x0260
#define eWDDLG_NoteType                          0x0262
#define eWDDLG_FootNumberAs                      0x0263
#define eWDDLG_FootStartingNum                   0x0264
#define eWDDLG_FootRestartNum                    0x0265
#define eWDDLG_EndnotesAt                        0x0266
#define eWDDLG_EndNumberAs                       0x0267
#define eWDDLG_EndStartingNum                    0x0268
#define eWDDLG_EndRestartNum                     0x0269
#define eWDDLG_With                              0x0272
#define eWDDLG_FieldName                         0x0274
#define eWDDLG_CustomDict5                       0x0275
#define eWDDLG_CustomDict6                       0x0276
#define eWDDLG_CustomDict7                       0x0277
#define eWDDLG_CustomDict8                       0x0278
#define eWDDLG_CustomDict9                       0x0279
#define eWDDLG_CustomDict10                      0x027A
#define eWDDLG_Object                            0x027B
#define eWDDLG_AutoCaption                       0x027C
#define eWDDLG_ErrorBeeps                        0x027E
#define eWDDLG_Symbol                            0x0281
#define eWDDLG_PrToFileName                      0x0282
#define eWDDLG_AppendPrFile                      0x0283
#define eWDDLG_Drive                             0x0284
#define eWDDLG_Goto                              0x0285
#define eWDDLG_Copy                              0x0287
#define eWDDLG_KeyCode2                          0x0288
#define eWDDLG_Caption                           0x028E
#define eWDDLG_TableId                           0x028F
#define eWDDLG_AddedStyles                       0x0290
#define eWDDLG_SmartCutPaste                     0x0291
#define eWDDLG_InsertedTextMark                  0x0292
#define eWDDLG_InsertedTextColor                 0x0293
#define eWDDLG_DeletedTextMark                   0x0294
#define eWDDLG_DeletedTextColor                  0x0295
#define eWDDLG_RevisedLinesMark                  0x0296
#define eWDDLG_RevisedLinesColor                 0x0297
#define eWDDLG_AddBelow                          0x0299
#define eWDDLG_Section                           0x029C
#define eWDDLG_ExtractAddress                    0x029D
#define eWDDLG_FindPrevious                      0x029E
#define eWDDLG_HideMarks                         0x029F
#define eWDDLG_ViewRevisions                     0x02A1
#define eWDDLG_PrintRevisions                    0x02A2
#define eWDDLG_CheckSpelling                     0x02A3
#define eWDDLG_Effects3d                         0x02A4
#define eWDDLG_Order2                            0x02A5
#define eWDDLG_Order3                            0x02A6
#define eWDDLG_CheckErrors                       0x02A8
#define eWDDLG_MenuType                          0x02AC
#define eWDDLG_DraftFont                         0x02AD
#define eWDDLG_WrapToWindow                      0x02AF
#define eWDDLG_Drawings                          0x02B0
#define eWDDLG_Formula                           0x02B3
#define eWDDLG_DropHeight                        0x02B4
#define eWDDLG_LabelIndex                        0x02B5
#define eWDDLG_LabelTray                         0x02B6
#define eWDDLG_LabelListIndex                    0x02B7
#define eWDDLG_LabelHeight                       0x02B8
#define eWDDLG_LabelWidth                        0x02B9
#define eWDDLG_LabelTopMargin                    0x02BA
#define eWDDLG_LabelSideMargin                   0x02BB
#define eWDDLG_LabelHorPitch                     0x02BC
#define eWDDLG_LabelVertPitch                    0x02BD
#define eWDDLG_LabelAcross                       0x02BE
#define eWDDLG_LabelDown                         0x02BF
#define eWDDLG_NumLines                          0x02C0
#define eWDDLG_VRuler                            0x02C2
#define eWDDLG_AllowRowSplit                     0x02C4
#define eWDDLG_Superscript                       0x02C6
#define eWDDLG_Subscript                         0x02C7
#define eWDDLG_WritePassword                     0x02C8
#define eWDDLG_RecommendReadOnly                 0x02C9
#define eWDDLG_DocumentPassword                  0x02CA
#define eWDDLG_Endnotes                          0x02CB
#define eWDDLG_Preset                            0x02CC
#define eWDDLG_ListType                          0x02CD
#define eWDDLG_Revert                            0x02CE
#define eWDDLG_MouseSimulation                   0x02CF
#define eWDDLG_DemoGuidance                      0x02D0
#define eWDDLG_DemoSpeed                         0x02D2
#define eWDDLG_CommandKeyHelp                    0x02D3
#define eWDDLG_DocNavKeys                        0x02D4
#define eWDDLG_HelpText                          0x02D5
#define eWDDLG_InsertAs                          0x02D6
#define eWDDLG_AcceptAll                         0x02D8
#define eWDDLG_RejectAll                         0x02D9
#define eWDDLG_Formatting                        0x02DC
#define eWDDLG_InitialCaps                       0x02DE
#define eWDDLG_SentenceCaps                      0x02DF
#define eWDDLG_Days                              0x02E0
#define eWDDLG_ReplaceText                       0x02E1
#define eWDDLG_MSQuery                           0x02E2
#define eWDDLG_Product                           0x02E4
#define eWDDLG_WritePasswordDoc                  0x02E7
#define eWDDLG_WritePasswordDot                  0x02E8
#define eWDDLG_LabelRow                          0x02E9
#define eWDDLG_LabelColumn                       0x02EA
#define eWDDLG_CommandValue                      0x02ED
#define eWDDLG_LimitConsecutiveHyphens           0x02EE
#define eWDDLG_RetAddrFromLeft                   0x02EF
#define eWDDLG_RetAddrFromTop                    0x02F0
#define eWDDLG_SoundsLike                        0x02F1
#define eWDDLG_KerningMin                        0x02F2
#define eWDDLG_PatternMatch                      0x02F3
#define eWDDLG_ToolTips                          0x02F4
#define eWDDLG_Mark                              0x02F5
#define eWDDLG_MarkAll                           0x02F6
#define eWDDLG_ShortCitation                     0x02F7
#define eWDDLG_LongCitation                      0x02F8
#define eWDDLG_AutoWordSelection                 0x02F9
#define eWDDLG_Passim                            0x02FA
#define eWDDLG_KeepFormatting                    0x02FB
#define eWDDLG_ColorButtons                      0x02FC
#define eWDDLG_LargeButtons                      0x02FD
#define eWDDLG_HeadingRows                       0x02FE
#define eWDDLG_LastRow                           0x02FF
#define eWDDLG_FirstColumn                       0x0300
#define eWDDLG_LastColumn                        0x0301
#define eWDDLG_Borders                           0x0302
#define eWDDLG_SnapToGrid                        0x0303
#define eWDDLG_XOrigin                           0x0304
#define eWDDLG_YOrigin                           0x0305
#define eWDDLG_XGrid                             0x0306
#define eWDDLG_YGrid                             0x0307
#define eWDDLG_EmbedFonts                        0x0308
#define eWDDLG_RelativeTo                        0x0309
#define eWDDLG_Width                             0x030A
#define eWDDLG_Height                            0x030B
#define eWDDLG_Drop                              0x030E
#define eWDDLG_Gap                               0x030F
#define eWDDLG_Angle                             0x0310
#define eWDDLG_CrossReference                    0x0314
#define eWDDLG_RightAlignPageNumbers             0x0315
#define eWDDLG_SendMailAttach                    0x0316
#define eWDDLG_RejectRevisions                   0x0317
#define eWDDLG_Kerning                           0x0318
#define eWDDLG_Exit                              0x0319
#define eWDDLG_Enable                            0x031A
#define eWDDLG_OwnHelp                           0x031B
#define eWDDLG_OwnStat                           0x031C
#define eWDDLG_StatText                          0x031D
#define eWDDLG_FormsData                         0x031E
#define eWDDLG_DefaultTray                       0x031F
#define eWDDLG_BookMarks                         0x0320
#define eWDDLG_IncludeFields                     0x0322
#define eWDDLG_LinkToSource                      0x0323
#define eWDDLG_AutoHyphenation                   0x0324
#define eWDDLG_TitleAutoText                     0x0326
#define eWDDLG_LinkStyles                        0x0327
#define eWDDLG_EntryAutoText                     0x0328
#define eWDDLG_ChapterNumber                     0x0329
#define eWDDLG_Message                           0x032A
#define eWDDLG_AllAtOnce                         0x032D
#define eWDDLG_ReturnWhenDone                    0x032E
#define eWDDLG_TrackStatus                       0x032F
#define eWDDLG_FillColor                         0x0330
#define eWDDLG_FillPattern                       0x0331
#define eWDDLG_FillPatternColor                  0x0332
#define eWDDLG_LineStyle                         0x0334
#define eWDDLG_LineWeight                        0x0335
#define eWDDLG_ArrowLength                       0x0336
#define eWDDLG_ArrowWidth                        0x0337
#define eWDDLG_ArrowStyle                        0x0338
#define eWDDLG_Wizard                            0x0339
#define eWDDLG_RoundCorners                      0x033A
#define eWDDLG_FineShading                       0x033B
#define eWDDLG_RTFInClipboard                    0x033C
#define eWDDLG_SavePictureInDoc                  0x033E
#define eWDDLG_RouteDocument                     0x033F
#define eWDDLG_AddrFromLeft                      0x0340
#define eWDDLG_AddrFromTop                       0x0341
#define eWDDLG_LabelDotMatrix                    0x0342
#define eWDDLG_LabelAutoText                     0x0343
#define eWDDLG_LabelText                         0x0344
#define eWDDLG_SingleLabel                       0x0345
#define eWDDLG_DifferentFirstPage                0x0346
#define eWDDLG_DontHyphen                        0x0347
#define eWDDLG_NextCitation                      0x0348
#define eWDDLG_TextType                          0x0349
#define eWDDLG_ShortMenuNames                    0x034A
#define eWDDLG_UnavailableFont                   0x034B
#define eWDDLG_SubstituteFont                    0x034C
#define eWDDLG_MailSubject                       0x034D
#define eWDDLG_MailAddress                       0x034E
#define eWDDLG_MailAsAttachment                  0x034F
#define eWDDLG_MailMerge                         0x0350
#define eWDDLG_LockAnchor                        0x0351
#define eWDDLG_TextWidth                         0x0353
#define eWDDLG_TextDefault                       0x0354
#define eWDDLG_TextFormat                        0x0355
#define eWDDLG_CheckSize                         0x0356
#define eWDDLG_CheckWidth                        0x0357
#define eWDDLG_CheckDefault                      0x0358
#define eWDDLG_PreserveStyles                    0x0359
#define eWDDLG_ApplyStylesHeadings               0x035A
#define eWDDLG_ApplyStylesLists                  0x035B
#define eWDDLG_ApplyStylesOtherParas             0x035D
#define eWDDLG_AdjustParaMarks                   0x035E
#define eWDDLG_AdjustTabsSpaces                  0x035F
#define eWDDLG_ReplaceQuotes                     0x0360
#define eWDDLG_ReplaceSymbols                    0x0361
#define eWDDLG_ReplaceBullets                    0x0362
#define eWDDLG_CrossReferenceAutoText            0x0363
#define eWDDLG_LongCitationAutoText              0x0364
#define eWDDLG_SearchName                        0x0366
#define eWDDLG_TrueAutoText                      0x0367
#define eWDDLG_TrueText                          0x0368
#define eWDDLG_FalseAutoText                     0x0369
#define eWDDLG_FalseText                         0x036A
#define eWDDLG_ValueText                         0x036B
#define eWDDLG_ValueAutoText                     0x036C
#define eWDDLG_Comparison                        0x036D
#define eWDDLG_CompareTo                         0x036E
#define eWDDLG_HelpType                          0x036F
#define eWDDLG_BlueScreen                        0x0370
#define eWDDLG_FullScreen                        0x0371
#define eWDDLG_AskOnce                           0x0372
#define eWDDLG_DefaultBookmarkText               0x0373
#define eWDDLG_MarkEntry                         0x0374
#define eWDDLG_MarkCitation                      0x0375
#define eWDDLG_AutoMark                          0x0376
#define eWDDLG_ListBy                            0x0377
#define eWDDLG_SubDir                            0x0378
#define eWDDLG_DefaultFillInText                 0x0379
#define eWDDLG_ClearRecipients                   0x037B
#define eWDDLG_AddRecipient                      0x037C
#define eWDDLG_OldRecipient                      0x037D
#define eWDDLG_AddSlip                           0x037E
#define eWDDLG_ResetSlip                         0x037F
#define eWDDLG_ClearSlip                         0x0380
#define eWDDLG_ReferenceType                     0x0381
#define eWDDLG_ReferenceKind                     0x0382
#define eWDDLG_ReferenceItem                     0x0383
#define eWDDLG_Length                            0x0384
#define eWDDLG_AutoAttach                        0x0385
#define eWDDLG_Accent                            0x0386
#define eWDDLG_AdjustEmptyParas                  0x0387
#define eWDDLG_HorizontalPos                     0x0388
#define eWDDLG_HorizontalFrom                    0x0389
#define eWDDLG_VerticalPos                       0x038A
#define eWDDLG_VerticalFrom                      0x038B
#define eWDDLG_InternalMargin                    0x038E
#define eWDDLG_Tab                               0x038F
#define eWDDLG_NoTabHangIndent                   0x0390
#define eWDDLG_NoSpaceRaiseLower                 0x0391
#define eWDDLG_PrintColBlack                     0x0393
#define eWDDLG_WrapTrailSpaces                   0x0394
#define eWDDLG_CategoryName                      0x0395
#define eWDDLG_Preview                           0x0396
#define eWDDLG_NoColumnBalance                   0x0397
#define eWDDLG_DrawingObjects                    0x0398
#define eWDDLG_ConvMailMergeEsc                  0x0399
#define eWDDLG_Strikethrough                     0x039A
#define eWDDLG_Face                              0x039B
#define eWDDLG_FieldShading                      0x039C
#define eWDDLG_NativePictureFormat               0x039D
#define eWDDLG_FileSize                          0x039E
#define eWDDLG_AllowAccentedUppercase            0x039F
#define eWDDLG_PictureEditor                     0x03A0
#define eWDDLG_InsertAsField                     0x03A1
#define eWDDLG_LineType                          0x03A2
#define eWDDLG_HyphenationZone                   0x03A3
#define eWDDLG_DisplayIcon                       0x03A4
#define eWDDLG_ShowMarks                         0x03A5
#define eWDDLG_ActivateAs                        0x03A6
#define eWDDLG_IconFilename                      0x03A8
#define eWDDLG_IconNumber                        0x03A9
#define eWDDLG_Verb                              0x03AA
#define eWDDLG_TwoPages                          0x03AB
#define eWDDLG_GlobalDotPrompt                   0x03AC
#define eWDDLG_SuppressSpBfAfterPgBrk            0x03AD
#define eWDDLG_SuppressTopSpacing                0x03AE
#define eWDDLG_OrigWordTableRules                0x03AF
#define eWDDLG_TransparentMetafiles              0x03B1
#define eWDDLG_NoReset                           0x03B2
#define eWDDLG_HideAutoFit                       0x03B3
#define eWDDLG_Space                             0x03B5
#define eWDDLG_Include                           0x03B6
#define eWDDLG_ShowBreaksInFrames                0x03B7
#define eWDDLG_SwapBordersFacingPages            0x03B8
#define eWDDLG_SQLStatement1                     0x03B9
#define eWDDLG_LeaveBackslashAlone               0x03CD
#define eWDDLG_AddAll                            0x03CE
#define eWDDLG_SaveAsAOCELetter                  0x03DB
#define eWDDLG_FractionalWidths                  0x03E0
#define eWDDLG_PSOverText                        0x03E1
#define eWDDLG_ExpandShiftReturn                 0x03E9
#define eWDDLG_DontULTrailSpace                  0x03EA
#define eWDDLG_DontBalanceSbDbWidth              0x03EB
#define eWDDLG_SuppressTopSpacingMac5            0x03F1
#define eWDDLG_HighlightColor                    0x03F3
#define eWDDLG_Highlight                         0x03F4
#define eWDDLG_SpacingInWholePoints              0x03F6
#define eWDDLG_HideSpellingErrors                0x03F8
#define eWDDLG_RecheckDocument                   0x03F9
#define eWDDLG_PrintBodyTextBeforeHeader         0x03FB
#define eWDDLG_NoLeading                         0x03FD
#define eWDDLG_ShowFolders                       0x03FF
#define eWDDLG_OutputPrinter                     0x0400
#define eWDDLG_ShowOptionsFor                    0x0401
#define eWDDLG_ApplyBorders                      0x0402
#define eWDDLG_ApplyBulletedLists                0x0404
#define eWDDLG_ApplyNumberedLists                0x0405
#define eWDDLG_ReplaceOrdinals                   0x0407
#define eWDDLG_ReplaceFractions                  0x0408
#define eWDDLG_TipWizardActive                   0x0409
#define eWDDLG_MWSmallCaps                       0x040F
#define eWDDLG_NoSpaceForUL                      0x0411
#define eWDDLG_NoExtraLineSpacing                0x0419
#define eWDDLG_DoNotSetAsSysDefault              0x041A
#define eWDDLG_CapsLock                          0x041B
#define eWDDLG_AutoAdd                           0x041C
#define eWDDLG_TruncateFontHeight                0x041E
#define eWDDLG_ToolTipsKey                       0x041F
#define eWDDLG_FindAllWordForms                  0x0422
#define eWDDLG_SubFontBySize                     0x0423
#define eWDDLG_AddressBookType                   0x0424
#define eWDDLG_TabIndent                         0x0452

typedef WORD EWDDLG_T, FAR *LPEWDDLG;


#endif

//////////////////////////////////////////////////////////////////////
// Data Types
//////////////////////////////////////////////////////////////////////

typedef enum tagETYPE
{
    eTYPE_UNUSED,
    eTYPE_UNKNOWN,
    eTYPE_VAR,
    eTYPE_LONG,
    eTYPE_STRING,
    eTYPE_ARRAY,
    eTYPE_DIALOG,
    eTYPE_DIALOG_FIELD
} ETYPE_T, FAR *LPETYPE;


typedef enum tagEOPERATOR
{
    eOPERATOR_BEGIN,
    eOPERATOR_END,
    eOPERATOR_WAIT_OPERAND,
    eOPERATOR_OPEN_PAREN,
    eOPERATOR_CLOSE_PAREN,
    eOPERATOR_NEG,
    eOPERATOR_MUL,
    eOPERATOR_DIV,
    eOPERATOR_MOD,
    eOPERATOR_ADD,
    eOPERATOR_SUB,
    eOPERATOR_EQ,
    eOPERATOR_NE,
    eOPERATOR_LT,
    eOPERATOR_GT,
    eOPERATOR_LE,
    eOPERATOR_GE,
    eOPERATOR_NOT,
    eOPERATOR_AND,
    eOPERATOR_OR
} EOPERATOR_T, FAR *LPEOPERATOR;


//////////////////////////////////////////////////////////////////////
// Tokens
//////////////////////////////////////////////////////////////////////

typedef enum tagETOKEN
{
    eTOKEN_SKIP,
    eTOKEN_NOT,
    eTOKEN_AND,
    eTOKEN_OR,
    eTOKEN_OPEN_PAREN,
    eTOKEN_CLOSE_PAREN,
    eTOKEN_PLUS,
    eTOKEN_MINUS,
    eTOKEN_SLASH,
    eTOKEN_ASTERISK,
    eTOKEN_MOD,
    eTOKEN_EQ,
    eTOKEN_NE,
    eTOKEN_LT,
    eTOKEN_GT,
    eTOKEN_LE,
    eTOKEN_GE,
    eTOKEN_COMMA,
    eTOKEN_RESUME,
    eTOKEN_COLON,
    eTOKEN_END,
    eTOKEN_SUB,
    eTOKEN_FUNCTION,
    eTOKEN_IF,
    eTOKEN_THEN,
    eTOKEN_ELSE_IF,
    eTOKEN_ELSE,
    eTOKEN_WHILE,
    eTOKEN_WEND,
    eTOKEN_FOR,
    eTOKEN_TO,
    eTOKEN_STEP,
    eTOKEN_NEXT,
    eTOKEN_SEMICOLON,
    eTOKEN_CALL,
    eTOKEN_GOTO,
    eTOKEN_ON,
    eTOKEN_ERROR,
    eTOKEN_LET,
    eTOKEN_DIM,
    eTOKEN_SHARED,
    eTOKEN_SELECT,
    eTOKEN_IS,
    eTOKEN_CASE,
    eTOKEN_AS,
    eTOKEN_REDIM,
    eTOKEN_PRINT,
    eTOKEN_INPUT,
    eTOKEN_LINE,
    eTOKEN_WRITE,
    eTOKEN_NAME,
    eTOKEN_OUTPUT,
    eTOKEN_APPEND,
    eTOKEN_OPEN,
    eTOKEN_GET_CUR_VALUES,
    eTOKEN_DIALOG,
    eTOKEN_SUPER,
    eTOKEN_DECLARE,
    eTOKEN_DOUBLE,
    eTOKEN_INTEGER,
    eTOKEN_LONG,
    eTOKEN_SINGLE,
    eTOKEN_STRING,
    eTOKEN_CDECL,
    eTOKEN_ALIAS,
    eTOKEN_ANY,
    eTOKEN_TOOLS_GET_SPELLING,
    eTOKEN_TOOLS_GET_SYNONYMS,
    eTOKEN_CLOSE,
    eTOKEN_BEGIN,
    eTOKEN_LIB,
    eTOKEN_READ,
    eTOKEN_CHECK_DIALOG,
    eTOKEN_END_IF,
    eTOKEN_NEWLINE,
    eTOKEN_LABEL,
    eTOKEN_WORD_LABEL,
    eTOKEN_FUNCTION_VALUE,
    eTOKEN_DOUBLE_VALUE,
    eTOKEN_IDENTIFIER,
    eTOKEN_QUOTED_STRING,
    eTOKEN_WORD_VALUE,
    eTOKEN_ASCII_CHAR,
    eTOKEN_POUND,
    eTOKEN_DIALOG_FIELD,
    eTOKEN_EXTERNAL_MACRO,
    eTOKEN_SUB_IDENTIFIER,
    eTOKEN_QUOTED_USTRING,
    eTOKEN_UIDENTIFIER
} ETOKEN_T, FAR *LPETOKEN;

typedef union tagUDATA
{
    LPVOID          lpvstElement;
    LPVOID          lpvstVar;
    LPBYTE          lpbyStr;
    LPVOID          lpvstDlg;
    long            lValue;
    WORD            wDlgField;
} UDATA_T, FAR *LPUDATA;

typedef struct tagVAR
{
    ETYPE_T     eType;
    UDATA_T     uData;

    BYTE        byNameLen;
    LPBYTE      lpbyName;
} VAR_T, FAR *LPVAR;

typedef struct tagELEMENT
{
    ETYPE_T                     eType;
    UDATA_T                     uData;

    DWORD                       dwIndex;
    struct tagELEMENT FAR *     lpstNext;
} ELEMENT_T, FAR *LPELEMENT;

typedef struct tagOPERAND
{
    ETYPE_T     eType;
    UDATA_T     uData;
} OPERAND_T, FAR *LPOPERAND;

typedef struct tagPARAMETER
{
    struct tagPARAMETER FAR *   lpstNext;

    ETYPE_T                     eType;
    UDATA_T                     uData;
} PARAMETER_T, FAR *LPPARAMETER;


//////////////////////////////////////////////////////////////////////
// WordBasic dialog structures
//////////////////////////////////////////////////////////////////////

typedef struct tagDIALOG
{
    WORD        wSize;  // Including wSize and eWDDLG
    EWDCMD_T    eWDCMD;
} DIALOG_T, FAR *LPDIALOG;

typedef struct tagDLG_FILESAVEAS
{
    WORD        wSize;
    EWDCMD_T    eWDCMD;

    BYTE        byFormat;
} DLG_FILESAVEAS_T, FAR *LPDLG_FILESAVEAS;

typedef struct tagDLG_FILENEW
{
    WORD        wSize;
    EWDCMD_T    eWDCMD;

    BYTE        byNewTemplate;
} DLG_FILENEW_T, FAR *LPDLG_FILENEW;


//////////////////////////////////////////////////////////////////////
// States and substates
//////////////////////////////////////////////////////////////////////

typedef enum tagESTATE
{
    esINIT,
    esBEGIN_INVALID,
    esERROR,
    esEXPR,
    esEXPR_IDENTIFIER,
    esEXPR_BUILTIN,
    esEXPR_EXTERNAL,
    esSTATEMENT,
    esSTATEMENT_DLG_FIELD,
    esSTATEMENT_IDENTIFIER,
    esSTATEMENT_BUILTIN,
    esSTATEMENT_EXTERNAL,
    esFUNCTION_CALL,
    esSUB_CALL,
    esBUILTIN_FUNCTION_CALL,
    esBUILTIN_SUB_CALL,
    esEXTERNAL_FUNCTION_CALL,
    esEXTERNAL_SUB_CALL,
    esEXTERNAL_CLEANUP,
    esCALL_GET_PARAM,
    esGET_DIALOG_FIELD_PARAM,
    esCALL_PUSH_PARAM,
    esCALL_PASS_BY_REF,
    esCALL_FUNCTION,
    esCALL_SUB,
    esCALL_GET_ARGUMENTS,
    esCALL_BODY,
    esCALL_END,
    esON,
    esON_ERROR,
    esON_ERROR_GOTO,
    esON_ERROR_RESUME,
    esDLG_FIELD_ASSIGN,
    esASSIGN,
    esIF_COND,
    esIF_THEN,
    esTHEN_SINGLE,
    esTHEN_MULTIPLE,
    esELSE_SINGLE,
    esELSE_MULTIPLE,
    esIF_THEN_END,
    esFOR_IDENTIFIER,
    esFOR_EQUAL,
    esFOR_START,
    esFOR_END,
    esFOR_STEP,
    esFOR_BODY,
    esFOR_NEXT,
    esWHILE,
    esWHILE_COND,
    esWHILE_BODY,
    esWHILE_WEND,
    esDIM,
    esDIM_IDENTIFIER,
    esDIM_IDENTIFIER_AS,
    esGET_CUR_VALUES,
    esEXPR_DIALOG,
    esDIALOG_SUB_CALL,
    esDIALOG_FUNCTION_CALL,
    esGOTO,
    esGOTO_IDENTIFIER0,
    esGOTO_IDENTIFIER1,
    esGOTO_WORD0,
    esGOTO_WORD1,
    esGOTO_CHECK_FOR_SPECIAL,
    esBEGIN_DIALOG,
    esBEGIN_DIALOG_END,
    esIGNORE_EXPR_RESULT,
    esSELECT,
    esSELECT_CASE,
    esSELECT_BODY,
    esSELECT_END,
    esCASE,
    esCASE_EXPR,
    esCASE_TO,
    esCASE_IS,
    esCASE_IS_EXPR,
    esCASE_BODY,
    esCASE_FOUND_MATCH,
    esSKIP_CASE_LINE,
    esSKIP_LINE,
    esEND_INVALID
} ESTATE_T, FAR *LPESTATE;

typedef enum tagESUBSTATE
{
    essDO,
    essSKIP,
    essTRUE,
    essFALSE
} ESUBSTATE_T, FAR *LPESUBSTATE;

typedef struct tagCONTROL
{
    ESTATE_T        eState;
    ESUBSTATE_T     eSubState;
    LPVOID          lpvstControl;
} CONTROL_T, FAR *LPCONTROL;

typedef enum tagECONTROL_TYPE
{
    ectIDENTIFIER,
    ectBUILTIN,
    ectCALL,
    ectCALL_GET_ARGS,
    ectEXPR,
    ectFOR,
    ectWHILE,
    ectDIALOG_CALL,
    ectLABEL_IDENTIFIER,
    ectLABEL_WORD,
    ectSELECT
} ECONTROL_TYPE_T, FAR *LPECONTROL_TYPE;

typedef struct tagCONTROL_HDR
{
    ECONTROL_TYPE_T ecType;
} CONTROL_HDR_T, FAR *LPCONTROL_HDR;

typedef struct tagIDENTIFIER
{
    ECONTROL_TYPE_T ecType;
    LPBYTE          lpbyName;
    union
    {
        LPPARAMETER     lpstFirstParam;
        EWDDLG_T        eWDDLG;
    } u;
} IDENTIFIER_T, FAR *LPIDENTIFIER;

typedef struct tagBUILTIN
{
    ECONTROL_TYPE_T ecType;
    EWDCMD_T        eWDCMD;
    LPPARAMETER     lpstFirstParam;
} BUILTIN_T, FAR *LPBUILTIN;

typedef struct tagCALL
{
    ECONTROL_TYPE_T ecType;
    DWORD           dwReturnIP;
    DWORD           dwRoutineStartIP;
} CALL_T, FAR *LPCALL;

typedef struct tagCALL_GET_ARGS
{
    ECONTROL_TYPE_T ecType;
    LPPARAMETER     lpstParams;
} CALL_GET_ARGS_T, FAR *LPCALL_GET_ARGS;

typedef struct tagEXPR
{
    ECONTROL_TYPE_T ecType;
    WORD            wParenDepth;
} EXPR_T, FAR *LPEXPR;

typedef struct tagFOR
{
    ECONTROL_TYPE_T ecType;
    DWORD           dwBodyIP;
    LPVAR           lpstVar;
    long            lStart;
    long            lEnd;
    long            lStep;
} FOR_T, FAR *LPFOR;

typedef struct tagWHILE
{
    ECONTROL_TYPE_T ecType;
    DWORD           dwCondIP;
} WHILE_T, FAR *LPWHILE;

typedef struct tagDIALOG_CALL
{
    ECONTROL_TYPE_T ecType;
    LPPARAMETER     lpstFirstParam;
} DIALOG_CALL_T, FAR *LPDIALOG_CALL;

typedef struct tagLABEL_IDENTIFIER
{
    ECONTROL_TYPE_T ecType;
    LPBYTE          lpbyLabel;
    DWORD           dwStartIP;
} LABEL_IDENTIFIER_T, FAR *LPLABEL_IDENTIFIER;

typedef struct tagLABEL_WORD
{
    ECONTROL_TYPE_T ecType;
    WORD            wLabel;
    DWORD           dwStartIP;
} LABEL_WORD_T, FAR *LPLABEL_WORD;


typedef enum tagCASE_TYPE
{
    eCASE_TYPE_IS_EQ,
    eCASE_TYPE_IS_NE,
    eCASE_TYPE_IS_LT,
    eCASE_TYPE_IS_GT,
    eCASE_TYPE_IS_LE,
    eCASE_TYPE_IS_GE,
    eCASE_TYPE_TO_CHECK,
    eCASE_TYPE_TO_NO_CHECK
} CASE_TYPE_T, FAR *LPCASE_TYPE;

typedef struct tagSELECT
{
    ECONTROL_TYPE_T ecType;
    CASE_TYPE_T     eCaseType;
    ETYPE_T         eType;
    UDATA_T         uData;
} SELECT_T, FAR *LPSELECT;

typedef struct tagASSIGN
{
    ECONTROL_TYPE_T ecType;
    LPVAR           lpstVar;
} ASSIGN_T, FAR *LPASSIGN;

typedef enum EERROR
{
    eERROR_NO_ERROR,
    eERROR_GENERIC,
    eERROR_STATE_NEW,
    eERROR_STATE_PUSH,
    eERROR_STATE_POP,
    eERROR_STATE_TOP,
    eERROR_CONTROL_CREATE_IDENTIFIER,
    eERROR_CONTROL_ALLOC,
    eERROR_CONTROL_FREE,
} EERROR_T, FAR *LPEERROR;


//////////////////////////////////////////////////////////////////////
// Module environment
//////////////////////////////////////////////////////////////////////

#define MODULE_ENV_HASH_ARRAY_SIZE      32
#define MODULE_ENV_MAX_CACHED_FUNCTIONS 16
#define MODULE_ENV_MAX_CACHED_SUBS      16

typedef enum tagEIDENTIFIER
{
    eIDENTIFIER_LOCAL_VAR,
    eIDENTIFIER_GLOBAL_VAR,
    eIDENTIFIER_FUNCTION,
    eIDENTIFIER_SUB
} EIDENTIFIER_T, FAR *LPEIDENTIFIER;

#define HEU_RUN_BUF_SIZE            1024

#define MOD_ENV_MAX_HASHED_GOTOS    16

typedef struct tagMODULE_ENV
{
    struct tagMODULE_ENV FAR *  lpstPrev;

    LPSS_STREAM                 lpstStream;
    LPWD7ENCKEY                 lpstKey;

    WORD                        wMacroIndex;
    DWORD                       dwMacroOffset;
    BYTE                        byMacroEncryptByte;

    DWORD                       dwNumRunBufBytes;
    DWORD                       dwRunBufRelOffset;
    BYTE                        abyRunBuf[HEU_RUN_BUF_SIZE];

    // Global variable frame

    int                         nGlobalVarFrameStart;

    // Endianness

    BOOL                        bLittleEndian;

    // Module size

    DWORD                       dwSize;

    // Main IP

    DWORD                       dwMainIP;

    // Global variable hash

    BYTE                        abyGlobalVarHash[MODULE_ENV_HASH_ARRAY_SIZE];

    // User function hash

    BYTE                        abyFunctionHash[MODULE_ENV_HASH_ARRAY_SIZE];

    int                         nMaxCachedFunctions;
    int                         nNumCachedFunctions;
    LPDWORD                     lpdwCachedFunctionsHash;
    LPDWORD                     lpdwCachedFunctionsIP;

    // User subroutine hash

    BYTE                        abySubHash[MODULE_ENV_HASH_ARRAY_SIZE];
    int                         nMaxCachedSubs;
    int                         nNumCachedSubs;
    LPDWORD                     lpdwCachedSubsHash;
    LPDWORD                     lpdwCachedSubsIP;

    // Goto hash

    WORD                        awGotoLabelHash[MOD_ENV_MAX_HASHED_GOTOS];
    WORD                        awGotoLabelCount[MOD_ENV_MAX_HASHED_GOTOS];

} MODULE_ENV_T, FAR *LPMODULE_ENV;


//////////////////////////////////////////////////////////////////////
// Execution environment
//////////////////////////////////////////////////////////////////////

typedef struct tagVAR_ENV
{
    int             nMaxNumVars;
    int             nNumVars;
    LPVAR           lpastVars;

    int             nVarNameStoreSize;
    int             nVarNameStoreUsed;
    LPBYTE          lpbyVarNameStore;
} VAR_ENV_T, FAR *LPVAR_ENV;

typedef struct tagENV
{
    LPVOID                  lpvRootCookie;

    EERROR_T                eError;

    LPSS_STREAM             lpstStream;
    LPWD7ENCKEY             lpstKey;

    WD7_TDT_INFO_T          stTDTInfo;
    WORD                    wMacroCount;

    // Current execution location

    DWORD                   dwIP;
    DWORD                   dwNextIP;

    DWORD                   dwRoutineStartIP;

    WORD                    wModuleDepth;

    BOOL                    bRedoToken;
    ETOKEN_T                eToken;
    union
    {
        struct
        {
            BYTE            byLen;
            LPBYTE          lpby;
        } Identifier;

        struct
        {
            LPBYTE          lpby;
        } String;

        WORD                wValue;

        EWDCMD_T            eWDCMD;
        EWDDLG_T            eWDDLG;

        double              lfValue;
    } uTokenData;

    // Variable name and data storage

    VAR_ENV_T               stGlobalVar;
    VAR_ENV_T               stLocalVar;
    int                     nMaxVarFrames;
    int                     nNumVarFrames;
    LPINT                   lpanVarFrames;
    DWORD                   dwGSMVarSize;
    LPGSM_HDR               lpstGSMVarData;

    // Current module environment

    LPMODULE_ENV            lpstModEnv;

    // Control stack

    int                     nMaxControls;
    int                     nNumControls;

    LPCONTROL               lpstTopControl;
    LPCONTROL               lpastControlStack;

    DWORD                   dwGSMControlSize;
    LPGSM_HDR               lpstGSMControlData;

    // Expression evaluation stack

    int                     nMaxOperands;
    int                     nOperandsUsed;
    LPOPERAND               lpastOperands;

    int                     nOperatorsUsed;
    LPEOPERATOR             lpaeOperators;

    int                     nEvalDataStoreSize;
    int                     nEvalDataStoreUsed;
    LPBYTE                  lpbyEvalDataStore;

    // WB env

    long                    lErr;

    // Context

    BOOL                    bNormal;    // If emulating assuming normal.dot

    int                     nNormalLen;
    LPSTR                   lpszNormal;
    int                     nFNLen;
    LPSTR                   lpszFN;

    LPVOID                  lpvContextData;
} ENV_T, FAR *LPENV;

#define RedoToken(a) a->bRedoToken = TRUE

typedef void (*LPFNSTATE)
    (
        LPENV       lpstEnv
    );

extern LPFNSTATE FAR gapfnState[];


/////////////////////////////////////////////////////////////////
// Variable functions
/////////////////////////////////////////////////////////////////

BOOL VarCreate
(
    LPENV       lpstEnv
);

BOOL VarDestroy
(
    LPENV       lpstEnv
);

BOOL VarInit
(
    LPENV       lpstEnv
);

LPVAR VarAlloc
(
    LPENV       lpstEnv,
    BOOL        bGlobal,
    BOOL        bArray,
    BYTE        byNameLen,
    LPBYTE      lpbyName
);

LPVAR VarGet
(
    LPENV       lpstEnv,
    BYTE        byNameLen,
    LPBYTE      lpbyName
);

LPVAR VarArrayGet
(
    LPENV       lpstEnv,
    BYTE        byNameLen,
    LPBYTE      lpbyName
);

LPELEMENT VarArrayGetElement
(
    LPVAR       lpstVar,
    WORD        wIndex0,
    WORD        wIndex1
);

LPELEMENT VarArrayElementAlloc
(
    LPENV       lpstEnv,
    LPVAR       lpstVar,
    WORD        wIndex0,
    WORD        wIndex1
);

BOOL VarFree
(
    LPENV       lpstEnv,
    LPVAR       lpstVar
);

LPVAR VarSet
(
    LPENV       lpstEnv,
    LPVAR       lpstVar,
    ETYPE_T     eType,
    LPVOID      lpvData
);

LPVAR VarGetFirstFrameVar
(
    LPENV       lpstEnv,
    LPETYPE     lpeType,
    LPLPVOID    lplpvData
);

BOOL VarPushFrame
(
    LPENV       lpstEnv
);

BOOL VarPopFrame
(
    LPENV       lpstEnv
);

BOOL VarPushGlobalFrame
(
    LPENV       lpstEnv
);

BOOL VarPopGlobalFrame
(
    LPENV       lpstEnv
);


/////////////////////////////////////////////////////////////////
// Evaluation functions
/////////////////////////////////////////////////////////////////

BOOL EvalStackCreate
(
    LPENV       lpstEnv
);

BOOL EvalStackDestroy
(
    LPENV       lpstEnv
);

BOOL EvalStackInit
(
    LPENV       lpstEnv
);

BOOL EvalPushOperand
(
    LPENV       lpstEnv,
    ETYPE_T     eType,
    LPVOID      lpvData
);

BOOL EvalPushLongOperand
(
    LPENV       lpstEnv,
    long        l
);

BOOL EvalPushSZOperand
(
    LPENV       lpstEnv,
    LPSTR       lpsz
);

BOOL EvalQueryTopOperand
(
    LPENV       lpstEnv,
    LPETYPE     lpeType,
    LPLPVOID    lplpvData
);

BOOL EvalPopTopOperand
(
    LPENV       lpstEnv
);

BOOL EvalPushBegin
(
    LPENV       lpstEnv
);

BOOL EvalPushOperator
(
    LPENV       lpstEnv,
    EOPERATOR_T eOperator
);

BOOL EvalGetAndPopTopAsBoolean
(
    LPENV       lpstEnv,
    LPBOOL      lpbResult
);


/////////////////////////////////////////////////////////////////
// Control functions
/////////////////////////////////////////////////////////////////

BOOL ControlCreate
(
    LPENV       lpstEnv
);

BOOL ControlDestroy
(
    LPENV       lpstEnv
);

BOOL ControlInit
(
    LPENV       lpstEnv
);

BOOL ControlCallPushParameter
(
    LPENV       lpstEnv,
    ETYPE_T     eType,
    LPVOID      lpvData
);

BOOL ControlFreeParameter
(
    LPENV       lpstEnv,
    LPPARAMETER lpstParam
);

BOOL ControlFreeParameterList
(
    LPENV       lpstEnv,
    LPPARAMETER lpstParam
);

BOOL ParamGetLong
(
    LPENV       lpstEnv,
    LPPARAMETER lpstParam,
    LPLONG      lplValue
);

BOOL ParamGetString
(
    LPENV       lpstEnv,
    LPPARAMETER lpstParam,
    LPLPBYTE    lplpbyStr
);

#ifndef NDEBUG
void StateNew
(
    LPENV       lpstEnv,
    ESTATE_T    eState
);
#else
#define StateNew(lpstEnv,eNewState) \
    lpstEnv->lpstTopControl->eState = eNewState
#endif

void StatePush
(
    LPENV       lpstEnv,
    ESTATE_T    eState
);

ESTATE_T StatePop
(
    LPENV       lpstEnv
);

#ifndef NDEBUG
ESTATE_T StateTop
(
    LPENV       lpstEnv
);
#else
#define StateTop(lpstEnv) \
    (lpstEnv->lpstTopControl->eState)
#endif

#ifndef NDEBUG
void SubStateNew
(
    LPENV       lpstEnv,
    ESUBSTATE_T eSubState
);
#else
#define SubStateNew(lpstEnv,eNewSubState) \
    lpstEnv->lpstTopControl->eSubState = eNewSubState
#endif

#ifndef NDEBUG
ESUBSTATE_T SubStateTop
(
    LPENV       lpstEnv
);
#else
#define SubStateTop(lpstEnv) \
    (lpstEnv->lpstTopControl->eSubState)
#endif

#ifndef NDEBUG
void ControlSetTopControl
(
    LPENV       lpstEnv,
    LPVOID      lpvstControl
);
#else
#define ControlSetTopControl(lpstEnv,lpvstNewControl) \
    lpstEnv->lpstTopControl->lpvstControl = lpvstNewControl
#endif

void ControlDestroyTopControl
(
    LPENV           lpstEnv
);

BOOL ControlCreateControl
(
    LPENV           lpstEnv,
    ECONTROL_TYPE_T ecType
);

BOOL ControlCreateSelectControl
(
    LPENV           lpstEnv,
    ETYPE_T         eType,
    LPVOID          lpvData
);

#ifndef NDEBUG
LPVOID ControlTop
(
    LPENV           lpstEnv
);
#else
#define ControlTop(lpstEnv) \
    lpstEnv->lpstTopControl->lpvstControl
#endif


/////////////////////////////////////////////////////////////////
// Module environment
/////////////////////////////////////////////////////////////////

LPMODULE_ENV ModuleEnvCreate
(
    LPENV               lpstEnv,
    LPWD7ENCKEY         lpstKey,
    WORD                wMacroIndex,
    DWORD               dwMacroSize,
    DWORD               dwMacroOffset,
    BYTE                byMacroEncryptByte
);

BOOL ModuleEnvDestroy
(
    LPENV           lpstEnv,
    LPMODULE_ENV    lpstModEnv
);

void ModuleEnvCacheGlobalVar
(
    LPMODULE_ENV    lpstModEnv,
    BYTE            byNameLen,
    LPBYTE          lpbyName
);

BOOL ModuleEnvGotoLabelOkay
(
    LPENV           lpstEnv,
    BYTE            byLen,
    LPBYTE          lpbyLabel
);

void ModuleEnvCacheFunction
(
    LPMODULE_ENV    lpstModEnv,
    BYTE            byNameLen,
    LPBYTE          lpbyName,
    DWORD           dwIP
);

void ModuleEnvCacheSub
(
    LPMODULE_ENV    lpstModEnv,
    BYTE            byNameLen,
    LPBYTE          lpbyName,
    DWORD           dwIP
);

BOOL ModuleRead
(
    LPMODULE_ENV        lpstModEnv,
    DWORD               dwOffset,
    LPBYTE              lpbyBuf,
    DWORD               dwNumBytes
);

BOOL ModuleReadIdentifier
(
    LPMODULE_ENV        lpstModEnv,
    DWORD               dwIP,
    LPBYTE              lpbyIdentifierLen,
    LPBYTE              lpbyIdentifier
);

BOOL ModuleReadUIdentifier
(
    LPMODULE_ENV        lpstModEnv,
    DWORD               dwIP,
    LPBYTE              lpbyIdentifierLen,
    LPBYTE              lpbyIdentifier
);

BOOL ModuleScan
(
    LPMODULE_ENV    lpstModEnv
);

EIDENTIFIER_T ModuleEnvClassifyIdentifier
(
    LPENV           lpstEnv,
    BYTE            byNameLen,
    LPBYTE          lpbyName
);

EIDENTIFIER_T ModuleEnvClassifyExprIdentifier
(
    LPENV           lpstEnv,
    BYTE            byNameLen,
    LPBYTE          lpbyName
);

BOOL ModuleEnvVariableIsGlobal
(
    LPENV           lpstEnv,
    BYTE            byNameLen,
    LPBYTE          lpbyName
);

BOOL ModuleEnvGetFunctionIP
(
    LPENV           lpstEnv,
    BYTE            byNameLen,
    LPBYTE          lpbyName,
    LPDWORD         lpdwIP
);

BOOL ModuleEnvGetSubIP
(
    LPENV           lpstEnv,
    BYTE            byNameLen,
    LPBYTE          lpbyName,
    LPDWORD         lpdwIP
);


/////////////////////////////////////////////////////////////////
// Execution environment
/////////////////////////////////////////////////////////////////

LPENV WD7EnvCreate
(
    LPVOID          lpvRootCookie
);

BOOL WD7EnvDestroy
(
    LPENV           lpstEnv
);

BOOL WD7EnvInit
(
    LPENV           lpstEnv,
    LPSS_STREAM     lpstStream,
    LPWD7ENCKEY     lpstKey
);

BOOL WD7Execute
(
    LPENV               lpstEnv,
    WORD                wIndex,
    DWORD               dwMacroOffset,
    DWORD               dwMacroSize,
    BYTE                byMacroEncryptByte
);


/////////////////////////////////////////////////////////////////
// Execution environment
/////////////////////////////////////////////////////////////////

BOOL ExternalCallSetup
(
    LPENV           lpstEnv,
    LPBYTE          lpbyMacroName,
    LPBYTE          lpbyFunctionName,
    LPBYTE          lpbySubName
);

BOOL ExternalCallCleanup
(
    LPENV       lpstEnv
);

#endif // #ifndef _WD7ENV_H_


