//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/xlrec.h_v   1.4   12 Mar 1998 10:42:12   DCHI  $
//
// Description:
//  Contains Excel 5.0/95/97 record types.
//
//************************************************************************
// $Log:   S:/INCLUDE/VCS/xlrec.h_v  $
// 
//    Rev 1.4   12 Mar 1998 10:42:12   DCHI
// Various additional functionality for new Excel engine.
// 
//    Rev 1.3   11 Feb 1998 16:13:52   DCHI
// Added formula token definitions and BOUNDSHEET and ARRAY structures.
// 
//    Rev 1.2   28 Jan 1998 14:05:08   DCHI
// Added NAME record 0x18.
// 
//    Rev 1.1   28 Jan 1998 13:30:00   DCHI
// Added additional record definitions.
// 
//    Rev 1.0   05 Dec 1997 13:13:10   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _XLREC_H_

#define _XLREC_H_

#ifdef XL_ENUM

typedef enum tagEXLREC
{
    eXLREC_DIMENSIONS           = 0x00,   // DIMENSIONS: Cell Table Size
    eXLREC_BLANK                = 0x01,   // BLANK: Cell Value, Blank Cell
    eXLREC_NUMBER               = 0x03,   // NUMBER: Cell Value, Floating-Point Number
    eXLREC_LABEL                = 0x04,   // LABEL: Cell Value, String Constant
    eXLREC_BOOLERR              = 0x05,   // BOOLERR: Cell Value, Boolean or Error
    eXLREC_FORMULA              = 0x06,   // FORMULA: Cell Formula
    eXLREC_STRING               = 0x07,   // STRING: String Value of a Formula
    eXLREC_ROW                  = 0x08,   // ROW: Describes a Row
    eXLREC_EOF                  = 0x0A,   // EOF: End of File
    eXLREC_INDEX                = 0x0B,   // INDEX: Index Record
    eXLREC_CALCCOUNT            = 0x0C,   // CALCCOUNT: Iteration Count
    eXLREC_CALCMODE             = 0x0D,   // CALCMODE: Calculation Mode
    eXLREC_PRECISION            = 0x0E,   // PRECISION: Precision
    eXLREC_REFMODE              = 0x0F,   // REFMODE: Reference Mode
    eXLREC_DELTA                = 0x10,   // DELTA: Iteration Increment
    eXLREC_ITERATION            = 0x11,   // ITERATION: Iteration Mode
    eXLREC_PROTECT              = 0x12,   // PROTECT: Protection Flag
    eXLREC_PASSWORD             = 0x13,   // PASSWORD: Protection Password
    eXLREC_HEADER               = 0x14,   // HEADER: Print Header on Each Page
    eXLREC_FOOTER               = 0x15,   // FOOTER: Print Footer on Each Page
    eXLREC_EXTERNCOUNT          = 0x16,   // EXTERNCOUNT: Number of External References
    eXLREC_EXTERNSHEET          = 0x17,   // EXTERNSHEET: External Reference
    eXLREC_NAME                 = 0x18,  // NAME: Defined Name
    eXLREC_WINDOWPROTECT        = 0x19,   // WINDOWPROTECT: Windows Are Protected
    eXLREC_VERTICALPAGEBREAKS   = 0x1A,   // VERTICALPAGEBREAKS: Explicit Column Page Breaks
    eXLREC_HORIZONTALPAGEBREAKS = 0x1B,   // HORIZONTALPAGEBREAKS: Explicit Row Page Breaks
    eXLREC_NOTE                 = 0x1C,   // NOTE: Comment Associated with a Cell
    eXLREC_SELECTION            = 0x1D,   // SELECTION: Current Selection
    eXLREC_FORMAT               = 0x1E,   // FORMAT: Number Format
    eXLREC_ARRAY                = 0x21,   // ARRAY: Array-Entered Formula
    eXLREC_1904                 = 0x22,   // 1904: 1904 Date System
    eXLREC_EXTERNNAME           = 0x23,   // EXTERNNAME: Externally Referenced Name
    eXLREC_DEFAULTROWHEIGHT     = 0x25,   // DEFAULTROWHEIGHT: Default Row Height
    eXLREC_LEFTMARGIN           = 0x26,   // LEFTMARGIN: Left Margin Measurement
    eXLREC_RIGHTMARGIN          = 0x27,   // RIGHTMARGIN: Right Margin Measurement
    eXLREC_TOPMARGIN            = 0x28,   // TOPMARGIN: Top Margin Measurement
    eXLREC_BOTTOMMARGIN         = 0x29,   // BOTTOMMARGIN: Bottom Margin Measurement
    eXLREC_PRINTHEADERS         = 0x2A,   // PRINTHEADERS: Print Row/Column Labels
    eXLREC_PRINTGRIDLINES       = 0x2B,   // PRINTGRIDLINES: Print Gridlines Flag
    eXLREC_FILEPASS             = 0x2F,   // FILEPASS: File Is Password-Protected
    eXLREC_FONT                 = 0x31,   // FONT: Font Description
    eXLREC_TABLE                = 0x36,   // TABLE: Data Table
    eXLREC_CONTINUE             = 0x3C,   // CONTINUE: Continues Long Records
    eXLREC_WINDOW1              = 0x3D,   // WINDOW1: Window Information
    eXLREC_WINDOW2              = 0x3E,   // WINDOW2: Sheet Window Information
    eXLREC_BACKUP               = 0x40,   // BACKUP: Save Backup Version of the File
    eXLREC_PANE                 = 0x41,   // PANE: Number of Panes and Their Position
//    eXLREC_CODENAME             = 0x42,   // CODENAME: VBE Object Name
    eXLREC_CODEPAGE             = 0x42,   // CODEPAGE: Default Code Page
    eXLREC_PLS                  = 0x4D,   // PLS: Environment-Specific Print Record
    eXLREC_DCON                 = 0x50,   // DCON: Data Consolidation Information
    eXLREC_DCONREF              = 0x51,   // DCONREF: Data Consolidation References
    eXLREC_DCONNAME             = 0x52,   // DCONNAME: Data Consolidation Named References
    eXLREC_DEFCOLWIDTH          = 0x55,   // DEFCOLWIDTH: Default Width for Columns
    eXLREC_XCT                  = 0x59,   // XCT: CRN Record Count
    eXLREC_CRN                  = 0x5A,   // CRN: Nonresident Operands
    eXLREC_FILESHARING          = 0x5B,   // FILESHARING: File-Sharing Information
    eXLREC_WRITEACCESS          = 0x5C,   // WRITEACCESS: Write Access User Name
    eXLREC_OBJ                  = 0x5D,   // OBJ: Describes a Graphic Object
    eXLREC_UNCALCED             = 0x5E,   // UNCALCED: Recalculation Status
    eXLREC_SAVERECALC           = 0x5F,   // SAVERECALC: Recalculate Before Save
    eXLREC_TEMPLATE             = 0x60,   // TEMPLATE: Workbook Is a Template
    eXLREC_OBJPROTECT           = 0x63,   // OBJPROTECT: Objects Are Protected
    eXLREC_COLINFO              = 0x7D,   // COLINFO: Column Formatting Information
    eXLREC_RK                   = 0x7E,   // RK: Cell Value, RK Number
    eXLREC_IMDATA               = 0x7F,   // IMDATA: Image Data
    eXLREC_GUTS                 = 0x80,   // GUTS: Size of Row and Column Gutters
    eXLREC_WSBOOL               = 0x81,   // WSBOOL: Additional Workspace Information
    eXLREC_GRIDSET              = 0x82,   // GRIDSET: State Change of Gridlines Option
    eXLREC_HCENTER              = 0x83,   // HCENTER: Center Between Horizontal Margins
    eXLREC_VCENTER              = 0x84,   // VCENTER: Center Between Vertical Margins
    eXLREC_BOUNDSHEET           = 0x85,   // BOUNDSHEET: Sheet Information
    eXLREC_WRITEPROT            = 0x86,   // WRITEPROT: Workbook Is Write-Protected
    eXLREC_ADDIN                = 0x87,   // ADDIN: Workbook Is an Add-in Macro
    eXLREC_EDG                  = 0x88,   // EDG: Edition Globals
    eXLREC_PUB                  = 0x89,   // PUB: Publisher
    eXLREC_COUNTRY              = 0x8C,   // COUNTRY: Default Country and WIN.INI Country
    eXLREC_HIDEOBJ              = 0x8D,   // HIDEOBJ: Object Display Options
    eXLREC_SORT                 = 0x90,   // SORT: Sorting Options
    eXLREC_SUB                  = 0x91,   // SUB: Subscriber
    eXLREC_PALETTE              = 0x92,   // PALETTE: Color Palette Definition
    eXLREC_STYLE                = 0x93,   // STYLE: Style Information
    eXLREC_LHRECORD             = 0x94,   // LHRECORD: .WK? File Conversion Information
    eXLREC_LHNGRAPH             = 0x95,   // LHNGRAPH: Named Graph Information
    eXLREC_SOUND                = 0x96,   // SOUND: Sound Note
    eXLREC_LPR                  = 0x98,   // LPR: Sheet Was Printed Using LINE.PRINT(
    eXLREC_STANDARDWIDTH        = 0x99,   // STANDARDWIDTH: Standard Column Width
    eXLREC_FNGROUPNAME          = 0x9A,   // FNGROUPNAME: Function Group Name
    eXLREC_FILTERMODE           = 0x9B,   // FILTERMODE: Sheet Contains Filtered List
    eXLREC_FNGROUPCOUNT         = 0x9C,   // FNGROUPCOUNT: Built-in Function Group Count
    eXLREC_AUTOFILTERINFO       = 0x9D,   // AUTOFILTERINFO: Drop-Down Arrow Count
    eXLREC_AUTOFILTER           = 0x9E,   // AUTOFILTER: AutoFilter Data
    eXLREC_SCL                  = 0xA0,   // SCL: Window Zoom Magnification
    eXLREC_SETUP                = 0xA1,   // SETUP: Page Setup
    eXLREC_COORDLIST            = 0xA9,   // COORDLIST: Polygon Object Vertex Coordinates
    eXLREC_GCW                  = 0xAB,   // GCW: Global Column-Width Flags
    eXLREC_SCENMAN              = 0xAE,   // SCENMAN: Scenario Output Data
    eXLREC_SCENARIO             = 0xAF,   // SCENARIO: Scenario Data
    eXLREC_SXVIEW               = 0xB0,   // SXVIEW: View Definition
    eXLREC_SXVD                 = 0xB1,   // SXVD: View Fields
    eXLREC_SXVI                 = 0xB2,   // SXVI: View Item
    eXLREC_SXIVD                = 0xB4,   // SXIVD: Row/Column Field IDs
    eXLREC_SXLI                 = 0xB5,   // SXLI: Line Item Array
    eXLREC_SXPI                 = 0xB6,   // SXPI: Page Item
    eXLREC_DOCROUTE             = 0xB8,   // DOCROUTE: Routing Slip Information
    eXLREC_RECIPNAME            = 0xB9,   // RECIPNAME: Recipient Name
    eXLREC_SHRFMLA              = 0xBC,   // SHRFMLA: Shared Formula
    eXLREC_MULRK                = 0xBD,   // MULRK: Multiple RK Cells
    eXLREC_MULBLANK             = 0xBE,   // MULBLANK: Multiple Blank Cells
    eXLREC_MMS                  = 0xC1,   // MMS: ADDMENU/DELMENU Record Group Count
    eXLREC_ADDMENU              = 0xC2,   // ADDMENU: Menu Addition
    eXLREC_DELMENU              = 0xC3,   // DELMENU: Menu Deletion
    eXLREC_SXDI                 = 0xC5,   // SXDI: Data Item
    eXLREC_SXDB                 = 0xC6,   // SXDB: PivotTable Cache Data
    eXLREC_SXSTRING             = 0xCD,   // SXSTRING: String
    eXLREC_SXTBL                = 0xD0,   // SXTBL: Multiple Consolidation Source Info
    eXLREC_SXTBRGIITM           = 0xD1,   // SXTBRGIITM: Page Item Name Count
    eXLREC_SXTBPG               = 0xD2,   // SXTBPG: Page Item Indexes
    eXLREC_OBPROJ               = 0xD3,   // OBPROJ: Visual Basic Project
    eXLREC_SXIDSTM              = 0xD5,   // SXIDSTM: Stream ID
    eXLREC_RSTRING              = 0xD6,   // RSTRING: Cell with Character Formatting
    eXLREC_DBCELL               = 0xD7,   // DBCELL: Stream Offsets
    eXLREC_BOOKBOOL             = 0xDA,   // BOOKBOOL: Workbook Option Flag
//    eXLREC_PARAMQRY             = 0xDC,   // PARAMQRY: Query Parameters
    eXLREC_SXEXT                = 0xDC,   // SXEXT: External Source Information
    eXLREC_SCENPROTECT          = 0xDD,   // SCENPROTECT: Scenario Protection
    eXLREC_OLESIZE              = 0xDE,   // OLESIZE: Size of OLE Object
    eXLREC_UDDESC               = 0xDF,   // UDDESC: Description String for Chart Autoformat
    eXLREC_XF                   = 0xE0,   // XF: Extended Format
    eXLREC_INTERFACEHDR         = 0xE1,   // INTERFACEHDR: Beginning of User Interface Records
    eXLREC_INTERFACEEND         = 0xE2,   // INTERFACEEND: End of User Interface Records
    eXLREC_SXVS                 = 0xE3,   // SXVS: View Source
    eXLREC_TABIDCONF            = 0xEA,   // TABIDCONF: Sheet Tab ID of Conflict History
    eXLREC_MSODRAWINGGROUP      = 0xEB,   // MSODRAWINGGROUP: Microsoft Office Drawing Group
    eXLREC_MSODRAWING           = 0xEC,   // MSODRAWING: Microsoft Office Drawing
    eXLREC_MSODRAWINGSELECTION  = 0xED,   // MSODRAWINGSELECTION: Microsoft Office Drawing Selection
    eXLREC_SXRULE               = 0xF0,   // SXRULE: PivotTable Rule Data
    eXLREC_SXEX                 = 0xF1,   // SXEX: PivotTable View Extended Information
    eXLREC_SXFILT               = 0xF2,   // SXFILT: PivotTable Rule Filter
    eXLREC_SXNAME               = 0xF6,   // SXNAME: PivotTable Name
    eXLREC_SXSELECT             = 0xF7,   // SXSELECT: PivotTable Selection Information
    eXLREC_SXPAIR               = 0xF8,   // SXPAIR: PivotTable Name Pair
    eXLREC_SXFMLA               = 0xF9,   // SXFMLA: PivotTable Parsed Expression
    eXLREC_SXFORMAT             = 0xFB,   // SXFORMAT: PivotTable Format Record
    eXLREC_SST                  = 0xFC,   // SST: Shared String Table
    eXLREC_LABELSST             = 0xFD,   // LABELSST: Cell Value, String Constant/SST
    eXLREC_EXTSST               = 0xFF,   // EXTSST: Extended Shared String Table
    eXLREC_SXVDEX               = 0x100,  // SXVDEX: Extended PivotTable View Fields
    eXLREC_SXFORMULA            = 0x103,  // SXFORMULA: PivotTable Formula Record
    eXLREC_SXDBEX               = 0x122,  // SXDBEX: PivotTable Cache Data
    eXLREC_TABID                = 0x13D,  // TABID: Sheet Tab Index Array
    eXLREC_USESELFS             = 0x160,  // USESELFS: Natural Language Formulas Flag
    eXLREC_DSF                  = 0x161,  // DSF: Double Stream File
    eXLREC_XL5MODIFY            = 0x162,  // XL5MODIFY: Flag for DSF
    eXLREC_FILESHARING2         = 0x1A5,  // FILESHARING2: File-Sharing Information for Shared Lists
    eXLREC_USERBVIEW            = 0x1A9,  // USERBVIEW: Workbook Custom View Settings
    eXLREC_USERSVIEWBEGIN       = 0x1AA,  // USERSVIEWBEGIN: Custom View Settings
    eXLREC_USERSVIEWEND         = 0x1AB,  // USERSVIEWEND: End of Custom View Records
    eXLREC_QSI                  = 0x1AD,  // QSI: External Data Range
    eXLREC_SUPBOOK              = 0x1AE,  // SUPBOOK: Supporting Workbook
    eXLREC_PROT4REV             = 0x1AF,  // PROT4REV: Shared Workbook Protection Flag
    eXLREC_CONDFMT              = 0x1B0,  // CONDFMT: Conditional Formatting Range Information
    eXLREC_CF                   = 0x1B1,  // CF: Conditional Formatting Conditions
    eXLREC_DVAL                 = 0x1B2,  // DVAL: Data Validation Information
    eXLREC_DCONBIN              = 0x1B5,  // DCONBIN: Data Consolidation Information
    eXLREC_TXO                  = 0x1B6,  // TXO: Text Object
    eXLREC_REFRESHALL           = 0x1B7,  // REFRESHALL: Refresh Flag
    eXLREC_HLINK                = 0x1B8,  // HLINK: Hyperlink
    eXLREC_SXFDBTYPE            = 0x1BB,  // SXFDBTYPE: SQL Datatype Identifier
    eXLREC_PROT4REVPASS         = 0x1BC,  // PROT4REVPASS: Shared Workbook Protection Password
    eXLREC_PFERRIEXLEXPLOIT     = 0x1BD,  // NoSecurityChecksAreNeeded
    eXLREC_DV                   = 0x1BE,  // DV: Data Validation Criteria
    eXLREC_DIMENSIONS2          = 0x200,  // DIMENSIONS: Cell Table Size
    eXLREC_BLANK2               = 0x201,  // BLANK: Cell Value, Blank Cell
    eXLREC_NUMBER2              = 0x203,  // NUMBER: Cell Value, Floating-Point Number
    eXLREC_LABEL2               = 0x204,  // LABEL: Cell Value, String Constant
    eXLREC_BOOLERR2             = 0x205,  // BOOLERR: Cell Value, Boolean or Error
    eXLREC_STRING2              = 0x207,  // STRING: String Value of a Formula
    eXLREC_ROW2                 = 0x208,  // ROW: Describes a Row
    eXLREC_INDEX2               = 0x20B,  // INDEX: Index Record
    eXLREC_NAME2                = 0x218,  // NAME: Defined Name
    eXLREC_ARRAY2               = 0x221,  // ARRAY: Array-Entered Formula
    eXLREC_EXTERNNAME2          = 0x223,  // EXTERNNAME: Externally Referenced Name
    eXLREC_DEFAULTROWHEIGHT2    = 0x225,  // DEFAULTROWHEIGHT: Default Row Height
    eXLREC_FONT2                = 0x231,  // FONT: Font Description
    eXLREC_TABLE2               = 0x236,  // TABLE: Data Table
    eXLREC_WINDOW22             = 0x23E,  // WINDOW2: Sheet Window Information
    eXLREC_RK2                  = 0x27E   // RK: Cell Value, RK Number
    eXLREC_STYLE2               = 0x293,  // STYLE: Style Information
    eXLREC_FORMULA2             = 0x406,  // FORMULA: Cell Formula
    eXLREC_FORMAT2              = 0x41E,  // FORMAT: Number Forma
    eXLREC_SHRFMLA2             = 0x4BC,  // SHRFMLA: Shared Formula
    eXLREC_BOF                  = 0x809,  // BOF: Beginning of File
    eXLREC_UNITS                = 0x1001, // UNITS: Chart Units
    eXLREC_CHART                = 0x1002, // CHART: Location and Overall Chart Dimensions
    eXLREC_SERIES               = 0x1003, // SERIES: Series Definition
    eXLREC_DATAFORMAT           = 0x1006, // DATAFORMAT: Series and Data Point Numbers
    eXLREC_LINEFORMAT           = 0x1007, // LINEFORMAT: Style of a Line or Border
    eXLREC_MARKERFORMAT         = 0x1009, // MARKERFORMAT: Style of a Line Marker
    eXLREC_AREAFORMAT           = 0x100A, // AREAFORMAT: Colors and Patterns for an Area
    eXLREC_PIEFORMAT            = 0x100B, // PIEFORMAT: Position of the Pie Slice
    eXLREC_ATTACHEDLABEL        = 0x100C, // ATTACHEDLABEL: Series Data/Value Labels
    eXLREC_SERIESTEXT           = 0x100D, // SERIESTEXT: Legend/Category/Value Text
    eXLREC_CHARTFORMAT          = 0x1014, // CHARTFORMAT: Parent Record for Chart Group
    eXLREC_LEGEND               = 0x1015, // LEGEND: Legend Type and Position
    eXLREC_SERIESLIST           = 0x1016, // SERIESLIST: Specifies the Series in an Overlay Chart
    eXLREC_BAR                  = 0x1017, // BAR: Chart Group is a Bar or Column Chart Group
    eXLREC_LINE                 = 0x1018, // LINE: Chart Group Is a Line Chart Group
    eXLREC_PIE                  = 0x1019, // PIE: Chart Group Is a Pie Chart Group
    eXLREC_AREA                 = 0x101A, // AREA: Chart Group Is an Area Chart Group
    eXLREC_SCATTER              = 0x101B, // SCATTER: Chart Group Is a Scatter Chart Group
    eXLREC_CHARTLINE            = 0x101C, // CHARTLINE: Drop/Hi-Lo/Series Lines on a Line Chart
    eXLREC_AXIS                 = 0x101D, // AXIS: Axis Type
    eXLREC_TICK                 = 0x101E, // TICK: Tick Marks and Labels Format
    eXLREC_VALUERANGE           = 0x101F, // VALUERANGE: Defines Value Axis Scale
    eXLREC_CATSERRANGE          = 0x1020, // CATSERRANGE: Defines a Category or Series Axis
    eXLREC_AXISLINEFORMAT       = 0x1021, // AXISLINEFORMAT: Defines a Line That Spans an Axis
    eXLREC_CHARTFORMATLINK      = 0x1022, // CHARTFORMATLINK: Not Used
    eXLREC_DEFAULTTEXT          = 0x1024, // DEFAULTTEXT: Default Data Label Text Properties
    eXLREC_TEXT                 = 0x1025, // TEXT: Defines Display of Text Fields
    eXLREC_FONTX                = 0x1026, // FONTX: Font Index
    eXLREC_OBJECTLINK           = 0x1027, // OBJECTLINK: Attaches Text to Chart or to Chart Item
    eXLREC_FRAME                = 0x1032, // FRAME: Defines Border Shape Around Displayed Text
    eXLREC_BEGIN                = 0x1033, // BEGIN: Defines the Beginning of an Object
    eXLREC_END                  = 0x1034, // END: Defines the End of an Object
    eXLREC_PLOTAREA             = 0x1035, // PLOTAREA: Frame Belongs to Plot Area
    eXLREC_3D                   = 0x103A, // 3D: Chart Group Is a 3-D Chart Group
    eXLREC_PICF                 = 0x103C, // PICF: Picture Format
    eXLREC_DROPBAR              = 0x103D, // DROPBAR: Defines Drop Bars
    eXLREC_RADAR                = 0x103E, // RADAR: Chart Group Is a Radar Chart Group
    eXLREC_SURFACE              = 0x103F, // SURFACE: Chart Group Is a Surface Chart Group
    eXLREC_RADARAREA            = 0x1040, // RADARAREA: Chart Group Is a Radar Area Chart Group
    eXLREC_AXISPARENT           = 0x1041, // AXISPARENT: Axis Size and Location
    eXLREC_LEGENDXN             = 0x1043, // LEGENDXN: Legend Exception
    eXLREC_SHTPROPS             = 0x1044, // SHTPROPS: Sheet Properties
    eXLREC_SERTOCRT             = 0x1045, // SERTOCRT: Series Chart-Group Index
    eXLREC_AXESUSED             = 0x1046, // AXESUSED: Number of Axes Sets
    eXLREC_SBASEREF             = 0x1048, // SBASEREF: PivotTable Reference
    eXLREC_SERPARENT            = 0x104A, // SERPARENT: Trendline or ErrorBar Series Index
    eXLREC_SERAUXTREND          = 0x104B, // SERAUXTREND: Series Trendline
    eXLREC_IFMT                 = 0x104E, // IFMT: Number-Format Index
    eXLREC_POS                  = 0x104F, // POS: Position Information
    eXLREC_ALRUNS               = 0x1050, // ALRUNS: Text Formatting
    eXLREC_AI                   = 0x1051, // AI: Linked Data
    eXLREC_SERAUXERRBAR         = 0x105B, // SERAUXERRBAR: Series ErrorBar
    eXLREC_SERFMT               = 0x105D, // SERFMT: Series Format
    eXLREC_FBI                  = 0x1060, // FBI: Font Basis
    eXLREC_BOPPOP               = 0x1061, // BOPPOP: Bar of Pie/Pie of Pie Chart Options
    eXLREC_AXCEXT               = 0x1062, // AXCEXT: Axis Options
    eXLREC_DAT                  = 0x1063, // DAT: Data Table Options
    eXLREC_PLOTGROWTH           = 0x1064, // PLOTGROWTH: Font Scale Factors
    eXLREC_SIINDEX              = 0x1065, // SIINDEX: Series Index
    eXLREC_GELFRAME             = 0x1066, // GELFRAME: Fill Data
    eXLREC_BOPPOPCUSTOM         = 0x1067, // BOPPOPCUSTOM: Custom Bar of Pie/Pie of Pie Chart Options
} EXLREC_T, FAR *LPEXLREC;

// Formula tokens

typedef enum tagEXLFM
{
    eXLFM_EXP                   = 0x01, // Array formula or shared formula
    eXLFM_TBL                   = 0x02, // Data table
    eXLFM_ADD                   = 0x03, // Addition
    eXLFM_SUB                   = 0x04, // Subtraction
    eXLFM_MUL                   = 0x05, // Multiplication
    eXLFM_DIV                   = 0x06, // Division
    eXLFM_POWER                 = 0x07, // Exponentiation
    eXLFM_CONCAT                = 0x08, // Concatenation
    eXLFM_LT                    = 0x09, // Less than
    eXLFM_LE                    = 0x0A, // Less than or equal
    eXLFM_EQ                    = 0x0B, // Equal
    eXLFM_GE                    = 0x0C, // Greater than or equal
    eXLFM_GT                    = 0x0D, // Greater than
    eXLFM_NE                    = 0x0E, // Not equal
    eXLFM_ISECT                 = 0x0F, // Intersection
    eXLFM_UNION                 = 0x10, // Union
    eXLFM_RANGE                 = 0x11, // Range
    eXLFM_UPLUS                 = 0x12, // Unary plus
    eXLFM_UMINUS                = 0x13, // Unary minus
    eXLFM_PERCENT               = 0x14, // Percent sign
    eXLFM_PAREN                 = 0x15, // Parenthesis
    eXLFM_MISSARG               = 0x16, // Missing argument
    eXLFM_STR                   = 0x17, // String constant
    eXLFM_EXTEND                = 0x18, // Extended ptg
    eXLFM_ATTR                  = 0x19, // Special attribute
    eXLFM_SHEET                 = 0x1A, // Deleted
    eXLFM_ENDSHEET              = 0x1B, // Deleted
    eXLFM_ERR                   = 0x1C, // Error value
    eXLFM_BOOL                  = 0x1D, // Boolean
    eXLFM_INT                   = 0x1E, // Integer
    eXLFM_NUM                   = 0x1F, // Number
    eXLFM_ARRAY                 = 0x20, // Array constant
    eXLFM_FUNC                  = 0x21, // Function, fixed number of arguments
    eXLFM_FUNCVAR               = 0x22, // Function, variable number of arguments
    eXLFM_NAME                  = 0x23, // Name
    eXLFM_REF                   = 0x24, // Cell reference
    eXLFM_AREA                  = 0x25, // Area reference
    eXLFM_MEMAREA               = 0x26, // Constant reference subexpression
    eXLFM_MEMERR                = 0x27, // Erroneous constant reference subexpression
    eXLFM_MEMNOMEM              = 0x28, // Incomplete constant reference subexpression
    eXLFM_MEMFUNC               = 0x29, // Variable reference subexpression
    eXLFM_REFERR                = 0x2A, // Deleted cell reference
    eXLFM_AREAERR               = 0x2B, // Deleted area reference
    eXLFM_REFN                  = 0x2C, // Cell reference within a shared formula
    eXLFM_AREAN                 = 0x2D, // Area reference within a shared formula
    eXLFM_MEMAREAN              = 0x2E, // Reference subexpression within a name
    eXLFM_MEMNOMEMN             = 0x2F, // Incomplete reference subexpression within a name
    eXLFM_FUNCCE                = 0x38, // FuncCE
    eXLFM_NAMEX                 = 0x39, // Name or external name
    eXLFM_REF3D                 = 0x3A, // 3-D cell reference
    eXLFM_AREA3D                = 0x3B, // 3-D area reference
    eXLFM_REFERR3D              = 0x3C, // Deleted 3-D cell reference
    eXLFM_AREAERR3D             = 0x3D, // Deleted 3-D area reference
    eXLFM_ARRAYV                = 0x40,
    eXLFM_FUNCV                 = 0x41,
    eXLFM_FUNCVARV              = 0x42,
    eXLFM_NAMEV                 = 0x43,
    eXLFM_REFV                  = 0x44,
    eXLFM_AREAV                 = 0x45,
    eXLFM_MEMAREAV              = 0x46,
    eXLFM_MEMERRV               = 0x47,
    eXLFM_MEMNOMEMV             = 0x48,
    eXLFM_MEMFUNCV              = 0x49,
    eXLFM_REFERRV               = 0x4A,
    eXLFM_AREAERRV              = 0x4B,
    eXLFM_REFNV                 = 0x4C,
    eXLFM_AREANV                = 0x4D,
    eXLFM_MEMAREANV             = 0x4E,
    eXLFM_MEMNOMEMNV            = 0x4F,
    eXLFM_FUNCCEV               = 0x58,
    eXLFM_NAMEXV                = 0x59,
    eXLFM_REF3DV                = 0x5A,
    eXLFM_AREA3DV               = 0x5B,
    eXLFM_REFERR3DV             = 0x5C,
    eXLFM_AREAERR3DV            = 0x5D,
    eXLFM_ARRAYA                = 0x60,
    eXLFM_FUNCA                 = 0x61,
    eXLFM_FUNCVARA              = 0x62,
    eXLFM_NAMEA                 = 0x63,
    eXLFM_REFA                  = 0x64,
    eXLFM_AREAA                 = 0x65,
    eXLFM_MEMAREAA              = 0x66,
    eXLFM_MEMERRA               = 0x67,
    eXLFM_MEMNOMEMA             = 0x68,
    eXLFM_MEMFUNCA              = 0x69,
    eXLFM_REFERRA               = 0x6A,
    eXLFM_AREAERRA              = 0x6B,
    eXLFM_REFNA                 = 0x6C,
    eXLFM_AREANA                = 0x6D,
    eXLFM_MEMAREANA             = 0x6E,
    eXLFM_MEMNOMEMNA            = 0x6F,
    eXLFM_FUNCCEA               = 0x78,
    eXLFM_NAMEXA                = 0x79,
    eXLFM_REF3DA                = 0x7A,
    eXLFM_AREA3DA               = 0x7B,
    eXLFM_REFERR3DA             = 0x7C,
    eXLFM_AREAERR3DA            = 0x7D
} EXLFM_T, FAR *LPEXLFM;

#else // #ifdef XL_ENUM

#define eXLREC_DIMENSIONS           0x00   // DIMENSIONS: Cell Table Size
#define eXLREC_BLANK                0x01   // BLANK: Cell Value, Blank Cell
#define eXLREC_NUMBER               0x03   // NUMBER: Cell Value, Floating-Point Number
#define eXLREC_LABEL                0x04   // LABEL: Cell Value, String Constant
#define eXLREC_BOOLERR              0x05   // BOOLERR: Cell Value, Boolean or Error
#define eXLREC_FORMULA              0x06   // FORMULA: Cell Formula
#define eXLREC_STRING               0x07   // STRING: String Value of a Formula
#define eXLREC_ROW                  0x08   // ROW: Describes a Row
#define eXLREC_EOF                  0x0A   // EOF: End of File
#define eXLREC_INDEX                0x0B   // INDEX: Index Record
#define eXLREC_CALCCOUNT            0x0C   // CALCCOUNT: Iteration Count
#define eXLREC_CALCMODE             0x0D   // CALCMODE: Calculation Mode
#define eXLREC_PRECISION            0x0E   // PRECISION: Precision
#define eXLREC_REFMODE              0x0F   // REFMODE: Reference Mode
#define eXLREC_DELTA                0x10   // DELTA: Iteration Increment
#define eXLREC_ITERATION            0x11   // ITERATION: Iteration Mode
#define eXLREC_PROTECT              0x12   // PROTECT: Protection Flag
#define eXLREC_PASSWORD             0x13   // PASSWORD: Protection Password
#define eXLREC_HEADER               0x14   // HEADER: Print Header on Each Page
#define eXLREC_FOOTER               0x15   // FOOTER: Print Footer on Each Page
#define eXLREC_EXTERNCOUNT          0x16   // EXTERNCOUNT: Number of External References
#define eXLREC_EXTERNSHEET          0x17   // EXTERNSHEET: External Reference
#define eXLREC_NAME                 0x18   // NAME: Defined Name
#define eXLREC_WINDOWPROTECT        0x19   // WINDOWPROTECT: Windows Are Protected
#define eXLREC_VERTICALPAGEBREAKS   0x1A   // VERTICALPAGEBREAKS: Explicit Column Page Breaks
#define eXLREC_HORIZONTALPAGEBREAKS 0x1B   // HORIZONTALPAGEBREAKS: Explicit Row Page Breaks
#define eXLREC_NOTE                 0x1C   // NOTE: Comment Associated with a Cell
#define eXLREC_SELECTION            0x1D   // SELECTION: Current Selection
#define eXLREC_FORMAT               0x1E   // FORMAT: Number Format
#define eXLREC_ARRAY                0x21   // ARRAY: Array-Entered Formula
#define eXLREC_1904                 0x22   // 1904: 1904 Date System
#define eXLREC_EXTERNNAME           0x23   // EXTERNNAME: Externally Referenced Name
#define eXLREC_DEFAULTROWHEIGHT     0x25   // DEFAULTROWHEIGHT: Default Row Height
#define eXLREC_LEFTMARGIN           0x26   // LEFTMARGIN: Left Margin Measurement
#define eXLREC_RIGHTMARGIN          0x27   // RIGHTMARGIN: Right Margin Measurement
#define eXLREC_TOPMARGIN            0x28   // TOPMARGIN: Top Margin Measurement
#define eXLREC_BOTTOMMARGIN         0x29   // BOTTOMMARGIN: Bottom Margin Measurement
#define eXLREC_PRINTHEADERS         0x2A   // PRINTHEADERS: Print Row/Column Labels
#define eXLREC_PRINTGRIDLINES       0x2B   // PRINTGRIDLINES: Print Gridlines Flag
#define eXLREC_FILEPASS             0x2F   // FILEPASS: File Is Password-Protected
#define eXLREC_FONT                 0x31   // FONT: Font Description
#define eXLREC_TABLE                0x36   // TABLE: Data Table
#define eXLREC_CONTINUE             0x3C   // CONTINUE: Continues Long Records
#define eXLREC_WINDOW1              0x3D   // WINDOW1: Window Information
#define eXLREC_WINDOW2              0x3E   // WINDOW2: Sheet Window Information
#define eXLREC_BACKUP               0x40   // BACKUP: Save Backup Version of the File
#define eXLREC_PANE                 0x41   // PANE: Number of Panes and Their Position
//#define eXLREC_CODENAME             0x42   // CODENAME: VBE Object Name
#define eXLREC_CODEPAGE             0x42   // CODEPAGE: Default Code Page
#define eXLREC_PLS                  0x4D   // PLS: Environment-Specific Print Record
#define eXLREC_DCON                 0x50   // DCON: Data Consolidation Information
#define eXLREC_DCONREF              0x51   // DCONREF: Data Consolidation References
#define eXLREC_DCONNAME             0x52   // DCONNAME: Data Consolidation Named References
#define eXLREC_DEFCOLWIDTH          0x55   // DEFCOLWIDTH: Default Width for Columns
#define eXLREC_XCT                  0x59   // XCT: CRN Record Count
#define eXLREC_CRN                  0x5A   // CRN: Nonresident Operands
#define eXLREC_FILESHARING          0x5B   // FILESHARING: File-Sharing Information
#define eXLREC_WRITEACCESS          0x5C   // WRITEACCESS: Write Access User Name
#define eXLREC_OBJ                  0x5D   // OBJ: Describes a Graphic Object
#define eXLREC_UNCALCED             0x5E   // UNCALCED: Recalculation Status
#define eXLREC_SAVERECALC           0x5F   // SAVERECALC: Recalculate Before Save
#define eXLREC_TEMPLATE             0x60   // TEMPLATE: Workbook Is a Template
#define eXLREC_OBJPROTECT           0x63   // OBJPROTECT: Objects Are Protected
#define eXLREC_COLINFO              0x7D   // COLINFO: Column Formatting Information
#define eXLREC_RK                   0x7E   // RK: Cell Value, RK Number
#define eXLREC_IMDATA               0x7F   // IMDATA: Image Data
#define eXLREC_GUTS                 0x80   // GUTS: Size of Row and Column Gutters
#define eXLREC_WSBOOL               0x81   // WSBOOL: Additional Workspace Information
#define eXLREC_GRIDSET              0x82   // GRIDSET: State Change of Gridlines Option
#define eXLREC_HCENTER              0x83   // HCENTER: Center Between Horizontal Margins
#define eXLREC_VCENTER              0x84   // VCENTER: Center Between Vertical Margins
#define eXLREC_BOUNDSHEET           0x85   // BOUNDSHEET: Sheet Information
#define eXLREC_WRITEPROT            0x86   // WRITEPROT: Workbook Is Write-Protected
#define eXLREC_ADDIN                0x87   // ADDIN: Workbook Is an Add-in Macro
#define eXLREC_EDG                  0x88   // EDG: Edition Globals
#define eXLREC_PUB                  0x89   // PUB: Publisher
#define eXLREC_COUNTRY              0x8C   // COUNTRY: Default Country and WIN.INI Country
#define eXLREC_HIDEOBJ              0x8D   // HIDEOBJ: Object Display Options
#define eXLREC_SORT                 0x90   // SORT: Sorting Options
#define eXLREC_SUB                  0x91   // SUB: Subscriber
#define eXLREC_PALETTE              0x92   // PALETTE: Color Palette Definition
#define eXLREC_STYLE                0x93   // STYLE: Style Information
#define eXLREC_LHRECORD             0x94   // LHRECORD: .WK? File Conversion Information
#define eXLREC_LHNGRAPH             0x95   // LHNGRAPH: Named Graph Information
#define eXLREC_SOUND                0x96   // SOUND: Sound Note
#define eXLREC_LPR                  0x98   // LPR: Sheet Was Printed Using LINE.PRINT(
#define eXLREC_STANDARDWIDTH        0x99   // STANDARDWIDTH: Standard Column Width
#define eXLREC_FNGROUPNAME          0x9A   // FNGROUPNAME: Function Group Name
#define eXLREC_FILTERMODE           0x9B   // FILTERMODE: Sheet Contains Filtered List
#define eXLREC_FNGROUPCOUNT         0x9C   // FNGROUPCOUNT: Built-in Function Group Count
#define eXLREC_AUTOFILTERINFO       0x9D   // AUTOFILTERINFO: Drop-Down Arrow Count
#define eXLREC_AUTOFILTER           0x9E   // AUTOFILTER: AutoFilter Data
#define eXLREC_SCL                  0xA0   // SCL: Window Zoom Magnification
#define eXLREC_SETUP                0xA1   // SETUP: Page Setup
#define eXLREC_COORDLIST            0xA9   // COORDLIST: Polygon Object Vertex Coordinates
#define eXLREC_GCW                  0xAB   // GCW: Global Column-Width Flags
#define eXLREC_SCENMAN              0xAE   // SCENMAN: Scenario Output Data
#define eXLREC_SCENARIO             0xAF   // SCENARIO: Scenario Data
#define eXLREC_SXVIEW               0xB0   // SXVIEW: View Definition
#define eXLREC_SXVD                 0xB1   // SXVD: View Fields
#define eXLREC_SXVI                 0xB2   // SXVI: View Item
#define eXLREC_SXIVD                0xB4   // SXIVD: Row/Column Field IDs
#define eXLREC_SXLI                 0xB5   // SXLI: Line Item Array
#define eXLREC_SXPI                 0xB6   // SXPI: Page Item
#define eXLREC_DOCROUTE             0xB8   // DOCROUTE: Routing Slip Information
#define eXLREC_RECIPNAME            0xB9   // RECIPNAME: Recipient Name
#define eXLREC_SHRFMLA              0xBC   // SHRFMLA: Shared Formula
#define eXLREC_MULRK                0xBD   // MULRK: Multiple RK Cells
#define eXLREC_MULBLANK             0xBE   // MULBLANK: Multiple Blank Cells
#define eXLREC_MMS                  0xC1   // MMS: ADDMENU/DELMENU Record Group Count
#define eXLREC_ADDMENU              0xC2   // ADDMENU: Menu Addition
#define eXLREC_DELMENU              0xC3   // DELMENU: Menu Deletion
#define eXLREC_SXDI                 0xC5   // SXDI: Data Item
#define eXLREC_SXDB                 0xC6   // SXDB: PivotTable Cache Data
#define eXLREC_SXSTRING             0xCD   // SXSTRING: String
#define eXLREC_SXTBL                0xD0   // SXTBL: Multiple Consolidation Source Info
#define eXLREC_SXTBRGIITM           0xD1   // SXTBRGIITM: Page Item Name Count
#define eXLREC_SXTBPG               0xD2   // SXTBPG: Page Item Indexes
#define eXLREC_OBPROJ               0xD3   // OBPROJ: Visual Basic Project
#define eXLREC_SXIDSTM              0xD5   // SXIDSTM: Stream ID
#define eXLREC_RSTRING              0xD6   // RSTRING: Cell with Character Formatting
#define eXLREC_DBCELL               0xD7   // DBCELL: Stream Offsets
#define eXLREC_BOOKBOOL             0xDA   // BOOKBOOL: Workbook Option Flag
//#define eXLREC_PARAMQRY             0xDC   // PARAMQRY: Query Parameters
#define eXLREC_SXEXT                0xDC   // SXEXT: External Source Information
#define eXLREC_SCENPROTECT          0xDD   // SCENPROTECT: Scenario Protection
#define eXLREC_OLESIZE              0xDE   // OLESIZE: Size of OLE Object
#define eXLREC_UDDESC               0xDF   // UDDESC: Description String for Chart Autoformat
#define eXLREC_XF                   0xE0   // XF: Extended Format
#define eXLREC_INTERFACEHDR         0xE1   // INTERFACEHDR: Beginning of User Interface Records
#define eXLREC_INTERFACEEND         0xE2   // INTERFACEEND: End of User Interface Records
#define eXLREC_SXVS                 0xE3   // SXVS: View Source
#define eXLREC_TABIDCONF            0xEA   // TABIDCONF: Sheet Tab ID of Conflict History
#define eXLREC_MSODRAWINGGROUP      0xEB   // MSODRAWINGGROUP: Microsoft Office Drawing Group
#define eXLREC_MSODRAWING           0xEC   // MSODRAWING: Microsoft Office Drawing
#define eXLREC_MSODRAWINGSELECTION  0xED   // MSODRAWINGSELECTION: Microsoft Office Drawing Selection
#define eXLREC_SXRULE               0xF0   // SXRULE: PivotTable Rule Data
#define eXLREC_SXEX                 0xF1   // SXEX: PivotTable View Extended Information
#define eXLREC_SXFILT               0xF2   // SXFILT: PivotTable Rule Filter
#define eXLREC_SXNAME               0xF6   // SXNAME: PivotTable Name
#define eXLREC_SXSELECT             0xF7   // SXSELECT: PivotTable Selection Information
#define eXLREC_SXPAIR               0xF8   // SXPAIR: PivotTable Name Pair
#define eXLREC_SXFMLA               0xF9   // SXFMLA: PivotTable Parsed Expression
#define eXLREC_SXFORMAT             0xFB   // SXFORMAT: PivotTable Format Record
#define eXLREC_SST                  0xFC   // SST: Shared String Table
#define eXLREC_LABELSST             0xFD   // LABELSST: Cell Value, String Constant/SST
#define eXLREC_EXTSST               0xFF   // EXTSST: Extended Shared String Table
#define eXLREC_SXVDEX               0x100  // SXVDEX: Extended PivotTable View Fields
#define eXLREC_SXFORMULA            0x103  // SXFORMULA: PivotTable Formula Record
#define eXLREC_SXDBEX               0x122  // SXDBEX: PivotTable Cache Data
#define eXLREC_TABID                0x13D  // TABID: Sheet Tab Index Array
#define eXLREC_USESELFS             0x160  // USESELFS: Natural Language Formulas Flag
#define eXLREC_DSF                  0x161  // DSF: Double Stream File
#define eXLREC_XL5MODIFY            0x162  // XL5MODIFY: Flag for DSF
#define eXLREC_FILESHARING2         0x1A5  // FILESHARING2: File-Sharing Information for Shared Lists
#define eXLREC_USERBVIEW            0x1A9  // USERBVIEW: Workbook Custom View Settings
#define eXLREC_USERSVIEWBEGIN       0x1AA  // USERSVIEWBEGIN: Custom View Settings
#define eXLREC_USERSVIEWEND         0x1AB  // USERSVIEWEND: End of Custom View Records
#define eXLREC_QSI                  0x1AD  // QSI: External Data Range
#define eXLREC_SUPBOOK              0x1AE  // SUPBOOK: Supporting Workbook
#define eXLREC_PROT4REV             0x1AF  // PROT4REV: Shared Workbook Protection Flag
#define eXLREC_CONDFMT              0x1B0  // CONDFMT: Conditional Formatting Range Information
#define eXLREC_CF                   0x1B1  // CF: Conditional Formatting Conditions
#define eXLREC_DVAL                 0x1B2  // DVAL: Data Validation Information
#define eXLREC_DCONBIN              0x1B5  // DCONBIN: Data Consolidation Information
#define eXLREC_TXO                  0x1B6  // TXO: Text Object
#define eXLREC_REFRESHALL           0x1B7  // REFRESHALL: Refresh Flag
#define eXLREC_HLINK                0x1B8  // HLINK: Hyperlink
#define eXLREC_SXFDBTYPE            0x1BB  // SXFDBTYPE: SQL Datatype Identifier
#define eXLREC_PROT4REVPASS         0x1BC  // PROT4REVPASS: Shared Workbook Protection Password
#define eXLREC_PFERRIEXLEXPLOIT     0x1BD  // NoSecurityChecksAreNeeded
#define eXLREC_DV                   0x1BE  // DV: Data Validation Criteria
#define eXLREC_DIMENSIONS2          0x200  // DIMENSIONS: Cell Table Size
#define eXLREC_BLANK2               0x201  // BLANK: Cell Value, Blank Cell
#define eXLREC_NUMBER2              0x203  // NUMBER: Cell Value, Floating-Point Number
#define eXLREC_LABEL2               0x204  // LABEL: Cell Value, String Constant
#define eXLREC_BOOLERR2             0x205  // BOOLERR: Cell Value, Boolean or Error
#define eXLREC_STRING2              0x207  // STRING: String Value of a Formula
#define eXLREC_ROW2                 0x208  // ROW: Describes a Row
#define eXLREC_INDEX2               0x20B  // INDEX: Index Record
#define eXLREC_NAME2                0x218  // NAME: Defined Name
#define eXLREC_ARRAY2               0x221  // ARRAY: Array-Entered Formula
#define eXLREC_EXTERNNAME2          0x223  // EXTERNNAME: Externally Referenced Name
#define eXLREC_DEFAULTROWHEIGHT2    0x225  // DEFAULTROWHEIGHT: Default Row Height
#define eXLREC_FONT2                0x231  // FONT: Font Description
#define eXLREC_TABLE2               0x236  // TABLE: Data Table
#define eXLREC_WINDOW22             0x23E  // WINDOW2: Sheet Window Information
#define eXLREC_RK2                  0x27E  // RK: Cell Value, RK Number
#define eXLREC_STYLE2               0x293  // STYLE: Style Information
#define eXLREC_FORMULA2             0x406  // FORMULA: Cell Formula
#define eXLREC_FORMAT2              0x41E  // FORMAT: Number Forma
#define eXLREC_SHRFMLA2             0x4BC  // SHRFMLA: Shared Formula
#define eXLREC_BOF                  0x809  // BOF: Beginning of File
#define eXLREC_UNITS                0x1001 // UNITS: Chart Units
#define eXLREC_CHART                0x1002 // CHART: Location and Overall Chart Dimensions
#define eXLREC_SERIES               0x1003 // SERIES: Series Definition
#define eXLREC_DATAFORMAT           0x1006 // DATAFORMAT: Series and Data Point Numbers
#define eXLREC_LINEFORMAT           0x1007 // LINEFORMAT: Style of a Line or Border
#define eXLREC_MARKERFORMAT         0x1009 // MARKERFORMAT: Style of a Line Marker
#define eXLREC_AREAFORMAT           0x100A // AREAFORMAT: Colors and Patterns for an Area
#define eXLREC_PIEFORMAT            0x100B // PIEFORMAT: Position of the Pie Slice
#define eXLREC_ATTACHEDLABEL        0x100C // ATTACHEDLABEL: Series Data/Value Labels
#define eXLREC_SERIESTEXT           0x100D // SERIESTEXT: Legend/Category/Value Text
#define eXLREC_CHARTFORMAT          0x1014 // CHARTFORMAT: Parent Record for Chart Group
#define eXLREC_LEGEND               0x1015 // LEGEND: Legend Type and Position
#define eXLREC_SERIESLIST           0x1016 // SERIESLIST: Specifies the Series in an Overlay Chart
#define eXLREC_BAR                  0x1017 // BAR: Chart Group is a Bar or Column Chart Group
#define eXLREC_LINE                 0x1018 // LINE: Chart Group Is a Line Chart Group
#define eXLREC_PIE                  0x1019 // PIE: Chart Group Is a Pie Chart Group
#define eXLREC_AREA                 0x101A // AREA: Chart Group Is an Area Chart Group
#define eXLREC_SCATTER              0x101B // SCATTER: Chart Group Is a Scatter Chart Group
#define eXLREC_CHARTLINE            0x101C // CHARTLINE: Drop/Hi-Lo/Series Lines on a Line Chart
#define eXLREC_AXIS                 0x101D // AXIS: Axis Type
#define eXLREC_TICK                 0x101E // TICK: Tick Marks and Labels Format
#define eXLREC_VALUERANGE           0x101F // VALUERANGE: Defines Value Axis Scale
#define eXLREC_CATSERRANGE          0x1020 // CATSERRANGE: Defines a Category or Series Axis
#define eXLREC_AXISLINEFORMAT       0x1021 // AXISLINEFORMAT: Defines a Line That Spans an Axis
#define eXLREC_CHARTFORMATLINK      0x1022 // CHARTFORMATLINK: Not Used
#define eXLREC_DEFAULTTEXT          0x1024 // DEFAULTTEXT: Default Data Label Text Properties
#define eXLREC_TEXT                 0x1025 // TEXT: Defines Display of Text Fields
#define eXLREC_FONTX                0x1026 // FONTX: Font Index
#define eXLREC_OBJECTLINK           0x1027 // OBJECTLINK: Attaches Text to Chart or to Chart Item
#define eXLREC_FRAME                0x1032 // FRAME: Defines Border Shape Around Displayed Text
#define eXLREC_BEGIN                0x1033 // BEGIN: Defines the Beginning of an Object
#define eXLREC_END                  0x1034 // END: Defines the End of an Object
#define eXLREC_PLOTAREA             0x1035 // PLOTAREA: Frame Belongs to Plot Area
#define eXLREC_3D                   0x103A // 3D: Chart Group Is a 3-D Chart Group
#define eXLREC_PICF                 0x103C // PICF: Picture Format
#define eXLREC_DROPBAR              0x103D // DROPBAR: Defines Drop Bars
#define eXLREC_RADAR                0x103E // RADAR: Chart Group Is a Radar Chart Group
#define eXLREC_SURFACE              0x103F // SURFACE: Chart Group Is a Surface Chart Group
#define eXLREC_RADARAREA            0x1040 // RADARAREA: Chart Group Is a Radar Area Chart Group
#define eXLREC_AXISPARENT           0x1041 // AXISPARENT: Axis Size and Location
#define eXLREC_LEGENDXN             0x1043 // LEGENDXN: Legend Exception
#define eXLREC_SHTPROPS             0x1044 // SHTPROPS: Sheet Properties
#define eXLREC_SERTOCRT             0x1045 // SERTOCRT: Series Chart-Group Index
#define eXLREC_AXESUSED             0x1046 // AXESUSED: Number of Axes Sets
#define eXLREC_SBASEREF             0x1048 // SBASEREF: PivotTable Reference
#define eXLREC_SERPARENT            0x104A // SERPARENT: Trendline or ErrorBar Series Index
#define eXLREC_SERAUXTREND          0x104B // SERAUXTREND: Series Trendline
#define eXLREC_IFMT                 0x104E // IFMT: Number-Format Index
#define eXLREC_POS                  0x104F // POS: Position Information
#define eXLREC_ALRUNS               0x1050 // ALRUNS: Text Formatting
#define eXLREC_AI                   0x1051 // AI: Linked Data
#define eXLREC_SERAUXERRBAR         0x105B // SERAUXERRBAR: Series ErrorBar
#define eXLREC_SERFMT               0x105D // SERFMT: Series Format
#define eXLREC_FBI                  0x1060 // FBI: Font Basis
#define eXLREC_BOPPOP               0x1061 // BOPPOP: Bar of Pie/Pie of Pie Chart Options
#define eXLREC_AXCEXT               0x1062 // AXCEXT: Axis Options
#define eXLREC_DAT                  0x1063 // DAT: Data Table Options
#define eXLREC_PLOTGROWTH           0x1064 // PLOTGROWTH: Font Scale Factors
#define eXLREC_SIINDEX              0x1065 // SIINDEX: Series Index
#define eXLREC_GELFRAME             0x1066 // GELFRAME: Fill Data
#define eXLREC_BOPPOPCUSTOM         0x1067 // BOPPOPCUSTOM: Custom Bar of Pie/Pie of Pie Chart Options

// Formula tokens

#define eXLFM_EXP                   0x01 // Array formula or shared formula
#define eXLFM_TBL                   0x02 // Data table
#define eXLFM_ADD                   0x03 // Addition
#define eXLFM_SUB                   0x04 // Subtraction
#define eXLFM_MUL                   0x05 // Multiplication
#define eXLFM_DIV                   0x06 // Division
#define eXLFM_POWER                 0x07 // Exponentiation
#define eXLFM_CONCAT                0x08 // Concatenation
#define eXLFM_LT                    0x09 // Less than
#define eXLFM_LE                    0x0A // Less than or equal
#define eXLFM_EQ                    0x0B // Equal
#define eXLFM_GE                    0x0C // Greater than or equal
#define eXLFM_GT                    0x0D // Greater than
#define eXLFM_NE                    0x0E // Not equal
#define eXLFM_ISECT                 0x0F // Intersection
#define eXLFM_UNION                 0x10 // Union
#define eXLFM_RANGE                 0x11 // Range
#define eXLFM_UPLUS                 0x12 // Unary plus
#define eXLFM_UMINUS                0x13 // Unary minus
#define eXLFM_PERCENT               0x14 // Percent sign
#define eXLFM_PAREN                 0x15 // Parenthesis
#define eXLFM_MISSARG               0x16 // Missing argument
#define eXLFM_STR                   0x17 // String constant
#define eXLFM_EXTEND                0x18 // Extended ptg
#define eXLFM_ATTR                  0x19 // Special attribute
#define eXLFM_SHEET                 0x1A // Deleted
#define eXLFM_ENDSHEET              0x1B // Deleted
#define eXLFM_ERR                   0x1C // Error value
#define eXLFM_BOOL                  0x1D // Boolean
#define eXLFM_INT                   0x1E // Integer
#define eXLFM_NUM                   0x1F // Number
#define eXLFM_ARRAY                 0x20 // Array constant
#define eXLFM_FUNC                  0x21 // Function, fixed number of arguments
#define eXLFM_FUNCVAR               0x22 // Function, variable number of arguments
#define eXLFM_NAME                  0x23 // Name
#define eXLFM_REF                   0x24 // Cell reference
#define eXLFM_AREA                  0x25 // Area reference
#define eXLFM_MEMAREA               0x26 // Constant reference subexpression
#define eXLFM_MEMERR                0x27 // Erroneous constant reference subexpression
#define eXLFM_MEMNOMEM              0x28 // Incomplete constant reference subexpression
#define eXLFM_MEMFUNC               0x29 // Variable reference subexpression
#define eXLFM_REFERR                0x2A // Deleted cell reference
#define eXLFM_AREAERR               0x2B // Deleted area reference
#define eXLFM_REFN                  0x2C // Cell reference within a shared formula
#define eXLFM_AREAN                 0x2D // Area reference within a shared formula
#define eXLFM_MEMAREAN              0x2E // Reference subexpression within a name
#define eXLFM_MEMNOMEMN             0x2F // Incomplete reference subexpression within a name
#define eXLFM_FUNCCE                0x38 // FuncCE
#define eXLFM_NAMEX                 0x39 // Name or external name
#define eXLFM_REF3D                 0x3A // 3-D cell reference
#define eXLFM_AREA3D                0x3B // 3-D area reference
#define eXLFM_REFERR3D              0x3C // Deleted 3-D cell reference
#define eXLFM_AREAERR3D             0x3D // Deleted 3-D area reference
#define eXLFM_ARRAYV                0x40
#define eXLFM_FUNCV                 0x41
#define eXLFM_FUNCVARV              0x42
#define eXLFM_NAMEV                 0x43
#define eXLFM_REFV                  0x44
#define eXLFM_AREAV                 0x45
#define eXLFM_MEMAREAV              0x46
#define eXLFM_MEMERRV               0x47
#define eXLFM_MEMNOMEMV             0x48
#define eXLFM_MEMFUNCV              0x49
#define eXLFM_REFERRV               0x4A
#define eXLFM_AREAERRV              0x4B
#define eXLFM_REFNV                 0x4C
#define eXLFM_AREANV                0x4D
#define eXLFM_MEMAREANV             0x4E
#define eXLFM_MEMNOMEMNV            0x4F
#define eXLFM_FUNCCEV               0x58
#define eXLFM_NAMEXV                0x59
#define eXLFM_REF3DV                0x5A
#define eXLFM_AREA3DV               0x5B
#define eXLFM_REFERR3DV             0x5C
#define eXLFM_AREAERR3DV            0x5D
#define eXLFM_ARRAYA                0x60
#define eXLFM_FUNCA                 0x61
#define eXLFM_FUNCVARA              0x62
#define eXLFM_NAMEA                 0x63
#define eXLFM_REFA                  0x64
#define eXLFM_AREAA                 0x65
#define eXLFM_MEMAREAA              0x66
#define eXLFM_MEMERRA               0x67
#define eXLFM_MEMNOMEMA             0x68
#define eXLFM_MEMFUNCA              0x69
#define eXLFM_REFERRA               0x6A
#define eXLFM_AREAERRA              0x6B
#define eXLFM_REFNA                 0x6C
#define eXLFM_AREANA                0x6D
#define eXLFM_MEMAREANA             0x6E
#define eXLFM_MEMNOMEMNA            0x6F
#define eXLFM_FUNCCEA               0x78
#define eXLFM_NAMEXA                0x79
#define eXLFM_REF3DA                0x7A
#define eXLFM_AREA3DA               0x7B
#define eXLFM_REFERR3DA             0x7C
#define eXLFM_AREAERR3DA            0x7D

#endif // #ifdef XL_ENUM

#if defined(SYM_WIN32) || defined(SYM_NTK) || defined(SYM_VXD) || defined(SYM_400)
#include "pshpack1.h"
#elif defined(__MACINTOSH__) || defined(SYM_AIX)
#pragma options align=packed
#else
#pragma pack(1)
#endif

typedef struct tagXL_REC_HDR
{
    WORD        wType;
    WORD        wLen;
} XL_REC_HDR_T, FAR *LPXL_REC_HDR;

#define XL_USTR_FLAG_DOUBLE_BYTE        0x01
#define XL_USTR_FLAG_EXTENDED           0x04
#define XL_USTR_FLAG_RICH               0x08

#define XL_REC_BOUNDSHEET_MAX_NAME_LEN  31

#define XL_REC_BOUNDSHEET_STATE_MASK    0x03

#define XL_REC_BOUNDSHEET_VISIBLE       0x00
#define XL_REC_BOUNDSHEET_HIDDEN        0x01
#define XL_REC_BOUNDSHEET_VERY_HIDDEN   0x02

#define XL_REC_BOUNDSHEET_TYPE_MASK     0x0F

#define XL_REC_BOUNDSHEET_WORKSHEET     0x00
#define XL_REC_BOUNDSHEET_40_MACRO      0x01
#define XL_REC_BOUNDSHEET_CHART         0x02
#define XL_REC_BOUNDSHEET_VB_MODULE     0x06

typedef struct tagXL_REC_BOUNDSHEET
{
    DWORD       dwOffset;
    BYTE        byHiddenState;
    BYTE        bySheetType;
    BYTE        byNameLen;
} XL_REC_BOUNDSHEET_T, FAR *LPXL_REC_BOUNDSHEET;

typedef struct tagXL7_REC_BOUNDSHEET
{
    DWORD       dwOffset;
    BYTE        byHiddenState;
    BYTE        bySheetType;
    BYTE        byNameLen;
} XL7_REC_BOUNDSHEET_T, FAR *LPXL7_REC_BOUNDSHEET;

typedef struct tagXL8_REC_BOUNDSHEET
{
    DWORD       dwOffset;
    BYTE        byHiddenState;
    BYTE        bySheetType;
    BYTE        byNameLen;
    BYTE        byNameFlags;
} XL8_REC_BOUNDSHEET_T, FAR *LPXL8_REC_BOUNDSHEET;

typedef struct tagXL_REC_ARRAY
{
    WORD        wFirstRow;
    WORD        wLastRow;
    BYTE        byFirstCol;
    BYTE        byLastCol;
    WORD        wFlags;
    DWORD       dwChn;
    WORD        wExprLen;
} XL_REC_ARRAY_T, FAR *LPXL_REC_ARRAY;

typedef struct tagXL_REC_WINDOW1
{
    WORD        wXPos;
    WORD        wYPos;
    WORD        wWidth;
    WORD        wHeight;
    WORD        wOptionFlags;
    WORD        wTabCur;
    WORD        wTabFirst;
    WORD        wCountTabSel;
    WORD        wTabRatio;
} XL_REC_WINDOW1, FAR *LPXL_REC_WINDOW1;

#define XL_REC_WINDOW2_F0_DSP_FMLA      0x01
#define XL_REC_WINDOW2_F0_DSP_GRID      0x02
#define XL_REC_WINDOW2_F0_DSP_RW_COL    0x04
#define XL_REC_WINDOW2_F0_FROZEN        0x08
#define XL_REC_WINDOW2_F0_DSP_ZEROES    0x10
#define XL_REC_WINDOW2_F0_DEFAULT_HDR   0x20
#define XL_REC_WINDOW2_F0_ARABIC        0x40
#define XL_REC_WINDOW2_F0_DSP_GUTS      0x80

#define XL_REC_WINDOW2_F1_FROZEN_NO_SPL 0x01
#define XL_REC_WINDOW2_F1_SELECTED      0x02
#define XL_REC_WINDOW2_F1_PAGED         0x04

typedef struct tagXL_REC_WINDOW2
{
    BYTE        byFlags0;
    BYTE        byFlags1;
    WORD        wTopRow;
    WORD        wLeftCol;
    DWORD       dwRGBHdr;
} XL_REC_WINDOW2_T, FAR *LPXL_REC_WINDOW2;

typedef struct tagXL_REC_NAME
{
    WORD        wFlags;
    BYTE        byKey;
    BYTE        byNameLen;
    WORD        wDefLen;
    WORD        wSheetIndex;
    WORD        wSheetIndex2;
    BYTE        byMenuLen;
    BYTE        byDescLen;
    BYTE        byHelpLen;
    BYTE        byStatusLen;
} XL_REC_NAME_T, FAR *LPXL_REC_NAME;

typedef struct tagXL_REC_BLANK
{
    WORD        wRow;
    WORD        wCol;
    WORD        wXFIndex;
} XL_REC_BLANK_T, FAR *LPXL_REC_BLANK;

typedef struct tagXL_RKREC
{
    WORD        wIXFE;
    DWORD       dwRK;
} XL_RKREC_T, FAR *LPXL_RKREC;


typedef struct tagXL_REC_MULRK
{
    WORD        wRow;
    WORD        wFirstCol;
} XL_REC_MULRK_T, FAR *LPXL_REC_MULRK;

typedef struct tagXL_REC_SHRFMLA
{
    WORD        wFirstRow;
    WORD        wLastRow;
    BYTE        byFirstCol;
    BYTE        byLastCol;
    WORD        wReserved;
    WORD        wExprLen;
} XL_REC_SHRFMLA_T, FAR *LPXL_REC_SHRFMLA;

#define XL_REC_BOF_WORKBOOK_GLOBALS     0x0005
#define XL_REC_BOF_VB_MODULE            0x0006
#define XL_REC_BOF_WORKSHEET            0x0010
#define XL_REC_BOF_CHART                0x0020
#define XL_REC_BOF_40_MACRO             0x0040
#define XL_REC_BOF_WORKSPACE            0x0100

typedef struct tagXL_REC_BOF
{
    WORD        wVer;
    WORD        wType;
    WORD        wBuild;
    WORD        wYear;
} XL_REC_BOF_T, FAR *LPXL_REC_BOF;

typedef struct tagXL8_XTI
{
    WORD        wSupBookIdx;
    WORD        wFirstTabIdx;
    WORD        wLastTabIdx;
} XL8_XTI_T, FAR *LPXL8_XTI;

#if defined(SYM_WIN32) || defined(SYM_NTK) || defined(SYM_VXD) || defined(SYM_400)
#include "poppack.h"
#elif defined(__MACINTOSH__) || defined(SYM_AIX) 
#pragma options align=reset
#else
#pragma pack()
#endif

#endif // #ifndef _XLREC_H_

