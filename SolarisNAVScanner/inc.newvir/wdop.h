//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/WDOP.H_v   1.0   30 Jun 1997 16:17:32   DCHI  $
//
// Description:
//      Header file for WordBasic operator definitions
//
//************************************************************************
// $Log:   S:/INCLUDE/VCS/WDOP.H_v  $
// 
//    Rev 1.0   30 Jun 1997 16:17:32   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _WDOP_H_

#define _WDOP_H_

#define WDOP_NOT                      0x02    // "Not"
#define WDOP_AND                      0x03    // "And"
#define WDOP_OR                       0x04    // "Or"
#define WDOP_OPEN_PAREN               0x05    // "("
#define WDOP_CLOSE_PAREN              0x06    // ")"
#define WDOP_PLUS                     0x07    // "+"
#define WDOP_MINUS                    0x08    // "-"
#define WDOP_SLASH                    0x09    // "/"
#define WDOP_ASTERISK                 0x0A    // "*"
#define WDOP_MOD                      0x0B    // "Mod"
#define WDOP_EQUAL                    0x0C    // "="
#define WDOP_NOT_EQUAL                0x0D    // "<>"
#define WDOP_LESS_THAN                0x0E    // "<"
#define WDOP_GREATER_THAN             0x0F    // ">"
#define WDOP_LESS_THAN_EQUAL          0x10    // "<="
#define WDOP_GREATER_THAN_EQUAL       0x11    // ">="
#define WDOP_COMMA                    0x12    // ","
#define WDOP_13                       0x13    // "?0x13?"
#define WDOP_14                       0x14    // "?0x14?"
#define WDOP_15                       0x15    // "?0x15?"
#define WDOP_16                       0x16    // "?0x16?"
#define WDOP_17                       0x17    // "?0x17?"
#define WDOP_RESUME                   0x18    // "Resume"
#define WDOP_COLON                    0x19    // ":"
#define WDOP_END                      0x1A    // "End"
#define WDOP_SUB                      0x1B    // "Sub"
#define WDOP_FUNCTION                 0x1C    // "Function"
#define WDOP_IF                       0x1D    // "If"
#define WDOP_THEN                     0x1E    // "Then"
#define WDOP_ELSE_IF                  0x1F    // "ElseIf"
#define WDOP_ELSE                     0x20    // "Else"
#define WDOP_WHILE                    0x21    // "While"
#define WDOP_WEND                     0x22    // "Wend"
#define WDOP_FOR                      0x23    // "For"
#define WDOP_TO                       0x24    // "To"
#define WDOP_STEP                     0x25    // "Step"
#define WDOP_NEXT                     0x26    // "Next"
#define WDOP_27                       0x27    // "?0x27?"
#define WDOP_SEMICOLON                0x28    // ";"
#define WDOP_CALL                     0x29    // "Call"
#define WDOP_GOTO                     0x2A    // "Goto"
#define WDOP_2B                       0x2B    // "?0x2B?"
#define WDOP_ON                       0x2C    // "On"
#define WDOP_ERROR                    0x2D    // "Error"
#define WDOP_LET                      0x2E    // "Let"
#define WDOP_DIM                      0x2F    // "Dim"
#define WDOP_SHARED                   0x30    // "Shared"
#define WDOP_SELECT                   0x31    // "Select"
#define WDOP_IS                       0x32    // "Is"
#define WDOP_CASE                     0x33    // "Case"
#define WDOP_AS                       0x34    // "As"
#define WDOP_REDIM                    0x35    // "Redim"
#define WDOP_PRINT                    0x36    // "Print"
#define WDOP_INPUT                    0x37    // "Input"
#define WDOP_LINE                     0x38    // "Line"
#define WDOP_WRITE                    0x39    // "Write"
#define WDOP_NAME                     0x3A    // "Name"
#define WDOP_OUTPUT                   0x3B    // "Output"
#define WDOP_APPEND                   0x3C    // "Append"
#define WDOP_OPEN                     0x3D    // "Open"
#define WDOP_GET_CUR_VALUES           0x3E    // "GetCurValues"
#define WDOP_DIALOG                   0x3F    // "Dialog"
#define WDOP_SUPER                    0x40    // "Super"
#define WDOP_DECLARE                  0x41    // "Declare"
#define WDOP_DOUBLE                   0x42    // "Double"
#define WDOP_INTEGER                  0x43    // "Integer"
#define WDOP_LONG                     0x44    // "Long"
#define WDOP_SINGLE                   0x45    // "Single"
#define WDOP_STRING                   0x46    // "String"
#define WDOP_CDECL                    0x47    // "Cdecl"
#define WDOP_ALIAS                    0x48    // "Alias"
#define WDOP_ANY                      0x49    // "Any"
#define WDOP_TOOLS_GET_SPELLING       0x4A    // "ToolsGetSpelling"
#define WDOP_TOOLS_GET_SYNONYMS       0x4B    // "ToolsGetSynonyms"
#define WDOP_CLOSE                    0x4C    // "Close"
#define WDOP_BEGIN                    0x4D    // "Begin"
#define WDOP_LIB                      0x4E    // "Lib"
#define WDOP_READ                     0x4F    // "Read"
#define WDOP_CHECK_DIALOG             0x50    // "CheckDialog"
#define WDOP_SPACE                    0x51    // " "
#define WDOP_TAB                      0x52    // "    "
#define WDOP_53                       0x53    // "?0x53?"
#define WDOP_END_IF                   0x54    // "EndIf"
#define WDOP_55                       0x55    // "?0x55?"
#define WDOP_56                       0x56    // "?0x56?"
#define WDOP_57                       0x57    // "?0x57?"
#define WDOP_58                       0x58    // "?0x58?"
#define WDOP_59                       0x59    // "?0x59?"
#define WDOP_5A                       0x5A    // "?0x5A?"
#define WDOP_5B                       0x5B    // "?0x5B?"
#define WDOP_5C                       0x5C    // "?0x5C?"
#define WDOP_5D                       0x5D    // "?0x5D?"
#define WDOP_5E                       0x5E    // "?0x5E?"
#define WDOP_5F                       0x5F    // "?0x5F?"
#define WDOP_60                       0x60    // "?0x60?"
#define WDOP_61                       0x61    // "?0x61?"
#define WDOP_62                       0x62    // "?0x62?"
#define WDOP_63                       0x63    // "?0x63?"
#define WDOP_NEWLINE                  0x64    // "\n"
#define WDOP_LABEL                    0x65    // "LABEL"
#define WDOP_WORD_VALUE_LABEL         0x66    // "WORD_VALUE_LABEL"
#define WDOP_FUNCTION_VALUE           0x67    // "FUNCTION"
#define WDOP_DOUBLE_VALUE             0x68    // "DOUBLE_VALUE"
#define WDOP_PASCAL_STRING            0x69    // "PASCAL_STRING"
#define WDOP_QUOTED_PASCAL_STRING     0x6A    // "QUOTED_PASCAL_STRING"
#define WDOP_COMMENT_PASCAL_STRING    0x6B    // "COMMENT_PASCAL_STRING"
#define WDOP_UNSIGNED_WORD_VALUE      0x6C    // "UNSIGNED_WORD_VALUE"
#define WDOP_ASCII_CHARACTER          0x6D    // "ASCII_CHARACTER"
#define WDOP_MULTIPLE_SPACES          0x6E    // "MULTIPLE_SPACES"
#define WDOP_MULTIPLE_TABS            0x6F    // "MULTIPLE_TABS"
#define WDOP_REM                      0x70    // "REM"
#define WDOP_POUND                    0x71    // "#"
#define WDOP_BACKSLASH                0x72    // "\\"
#define WDOP_DIALOG_FIELD             0x73    // "DIALOG_FIELD"
#define WDOP_74                       0x74    // "?0x74?"   // Adds $
#define WDOP_75                       0x75    // "?0x75?"   // Adds #
#define WDOP_EXTERNAL_MACRO           0x76    // "EXTERNAL_MACRO"
#define WDOP_IDENTIFIER               0x77    // "IDENTIFIER"
#define WDOP_78                       0x78    // "?0x78?"   // .identifier?
#define WDOP_QUOTED_UPASCAL_STRING    0x79    // "QUOTED_UPASCAL_STRING"
#define WDOP_7A                       0x7A
#define WDOP_UPASCAL_STRING_7B        0x7B    // "UPASCAL_STRING_7B"
#define WDOP_UPASCAL_STRING           0x7C    // "UPASCAL_STRING"
// 7A same as 6A?
// 7D wide-char identifier?  <BYTE length> <WORD> <WORD>...

#endif // #ifndef _WDOP_H_


