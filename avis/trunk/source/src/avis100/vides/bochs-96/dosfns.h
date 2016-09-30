/* dosfns.h -- list of audited dos functions */

#define ASCIZ_MAXLEN 64

enum iartype_e {
  DOSINT,             /* DOS INT 21h */
  DISKINT,            /* BIOS INT 13h */
  MEM,                /* memory event */
  CMOSINT             /* CMOS int 1Ah */
};

#define argNIL	0
#define	argAL	0x0001
#define	argBL	0x0002
#define	argCL	0x0004
#define argDL	0x0008
#define	argCF	0x0010
#define	argAX	0x0020
#define	argBX	0x0040
#define argCX	0x0080
#define argDX	0x0100
#define argSI	0x0200
#define argDI	0x0400
#define argDS	0x0800
#define argES	0x1000
#define argSTR1	0x2000
#define argSTR2	0x4000

enum fnargs_e {
fnargs_nil,
fnargs_all,
fnargs_al,
fnargs_al_bx_cx_dx,
fnargs_al_cl_asciz,
fnargs_al_cx,
fnargs_al_cx_dx,
fnargs_al_dl,
fnargs_al_dx,
fnargs_al_asciz,
fnargs_al_ds_dx,
fnargs_ax,
fnargs_ax_bx_cx,
fnargs_ax_bx_cx_dx,
fnargs_bl,
fnargs_bx,
fnargs_bx_cx_dx,
fnargs_bx_cx_dx_asciz_in_si,
fnargs_bx_cx_ds_dx,
fnargs_bx_dx,
fnargs_bx_es,
fnargs_bx_cx,
fnargs_cl_asciz,
fnargs_cl_2_asciz,
fnargs_cx_dx,
fnargs_cx_fcb,
fnargs_dl,
fnargs_dl_asciz_in_si,
fnargs_dx,
fnargs_dx_si,
fnargs_dx_ds,
fnargs_es,
fnargs_fcb,
fnargs_xfcb,
fnargs_asciz,
fnargs_cf,
fnargs_cf_ax,
fnargs_cf_ax_bx,
fnargs_cf_ax_cx,
fnargs_cf_ax_dx,
fnargs_cf_cx,
fnargs_cf_cx_dx,
fnargs_ax_es_di,
fnargs_disk_io,
fnargs_ax_cx_dx,
fnargs_cf_ax_cx_dx,
fnargs_cx
};

struct fnregs_s {
    enum fnargs_e arg;
    WORD regs;
} fnregs[] = 
{
	{ fnargs_nil,		argNIL},
	{ fnargs_all, 		argAX| argBX| argCX| argDX| argSI| argDI| argDS| argES| argCF},
	{ fnargs_al,  		argAL},
 { fnargs_al_dl,  	argAL | argDL},
	{ fnargs_al_bx_cx_dx, 	argAL| argBX| argCX| argDX},
	{ fnargs_al_cl_asciz, 	argAL| argCL| argSTR1},
	{ fnargs_al_cx, 	argAL| argCX},
	{ fnargs_al_cx_dx, 	argAL| argCX| argDX},
	{ fnargs_al_dx, 	argAL| argDX},
	{ fnargs_al_asciz, 	argAL| argSTR1},
	{ fnargs_al_ds_dx, 	argAL| argDX| argDS},
	{ fnargs_ax, 		argAX},
	{ fnargs_ax_bx_cx, 	argAX| argBX| argCX},
	{ fnargs_ax_bx_cx_dx, 	argAX| argBX| argCX| argDX},
	{ fnargs_bl, 		argBL},
	{ fnargs_bx,		argBX},
	{ fnargs_bx_cx_dx,	argBX| argCX| argDX},
	{ fnargs_bx_cx_dx_asciz_in_si, argBX | argCX | argDX | argSTR1},
	{ fnargs_bx_cx_ds_dx,	argBX| argCX| argDS| argDX},
	{ fnargs_bx_dx,		argBX| argDX},
	{ fnargs_bx_es,		argBX| argES},
	{ fnargs_bx_cx,		argBX| argCX},
	{ fnargs_cl_asciz,	argCL| argSTR1},
	{ fnargs_cl_2_asciz,	argCL| argSTR1| argSTR2},
	{ fnargs_cx_dx,		argCX| argDX},
	{ fnargs_cx_fcb,	argCX| argSTR1},
	{ fnargs_dl,		argDL},
	{ fnargs_dl_asciz_in_si,	argDL| argSTR1},
	{ fnargs_dx,		argDX },
	{ fnargs_dx_si,		argDX| argSI},
	{ fnargs_dx_ds,		argDS| argDX},
	{ fnargs_es,		argES},
	{ fnargs_fcb,		argSTR1},
	{ fnargs_xfcb,		argSTR1| argSTR2},
	{ fnargs_asciz,		argSTR1},
	{ fnargs_cf,		argCF},
	{ fnargs_cf_ax,		argAX| argCF},
	{ fnargs_cf_ax_bx,	argAX| argBX| argCF},
	{ fnargs_cf_ax_cx,	argAX| argCX| argCF},
	{ fnargs_cf_ax_dx,	argAX| argDX| argCF},
	{ fnargs_cf_cx,		argCX| argCF},
	{ fnargs_cf_cx_dx, 	argCX| argDX| argCF},
	{ fnargs_ax_es_di,      argAX|argES|argDI},
	{ fnargs_disk_io,       argAL|argBX|argCX|argDX|argES },
	{ fnargs_ax_cx_dx,      argAX|argCX|argDX },
	{ fnargs_cf_ax_cx_dx,   argCF|argAX|argCX|argDX },
	{ fnargs_cx,            argCX},
	{ -1, 0}
};

struct dosfns_s {
    int           num,
                  ah,
                  al;
    enum fnargs_e argtype,
                  rettype;
    float         dosver;
    char         *description;
};

struct ArgVals_s {
    enum fnargs_e type;
    int  cf;
    WORD ax,
         bx,
         cx,
         dx,
         si,
         di,
         ds,
         es;
    char str1[ASCIZ_MAXLEN],
         str2[ASCIZ_MAXLEN];
};

struct internal_audit_rec_s {
    enum iartype_e	type;
    int 		function;
    long unsigned int 	start_time,
			end_time;

    struct ArgVals_s	args,
			ret;
};

struct dosfns_s dosfns[] = 
{
/*       num,   ah, al, argtype,       	rettype,      dosver, description    if al == -1, then it is ignored */
	{  1, 0x00, -1, fnargs_nil, 	fnargs_nil, 	1.0, "TERMINATE"},
	{  2, 0x0d, -1, fnargs_nil, 	fnargs_cf, 	1.0, "FLUSH"},
	{  3, 0x0e, -1, fnargs_dl, 	fnargs_al, 	1.0, "SET_DRIVE"},
	{  4, 0x0f, -1, fnargs_fcb, 	fnargs_al, 	1.0, "OPEN"},
	{  5, 0x10, -1, fnargs_fcb, 	fnargs_al, 	1.0, "CLOSE"},
	{  6, 0x11, -1, fnargs_fcb, 	fnargs_al, 	1.0, "FINDFIRST"},
	{  7, 0x12, -1, fnargs_nil, 	fnargs_al, 	1.0, "FINDNEXT"},
	{  8, 0x13, -1, fnargs_fcb, 	fnargs_al, 	1.0, "DELETE"},
	{  9, 0x14, -1, fnargs_fcb, 	fnargs_al, 	1.0, "SEQ_READ"},
	{ 10, 0x15, -1, fnargs_fcb, 	fnargs_al, 	1.0, "SEQ_WRITE"},
	{ 11, 0x16, -1, fnargs_fcb, 	fnargs_al, 	1.0, "CREATE"},
	{ 12, 0x17, -1, fnargs_xfcb, 	fnargs_al,	1.0, "RENAME"},
	{ 13, 0x19, -1, fnargs_nil, 	fnargs_al, 	1.0, "DEFAULT_DRIVE"},
	{ 95, 0x1a, -1, fnargs_dx_ds,	fnargs_nil,	1.0, "SET_DTA"},
	{ 14, 0x1b, -1, fnargs_nil, 	fnargs_al_cx_dx, 	1.0, "GET_DEFAULT_DRIVE_ALLOC"},
	{ 15, 0x1c, -1, fnargs_dl, 	fnargs_al_cx_dx, 	1.0, "GET_DRIVE_ALLOC"},
	{ 16, 0x1f, -1, fnargs_nil, 	fnargs_al, 	1.0, "GET_DRIVE_PARAMETERS"},
	{ 17, 0x21, -1, fnargs_fcb, 	fnargs_al, 	1.0, "RND_READ"},
	{ 18, 0x22, -1, fnargs_fcb, 	fnargs_al, 	1.0, "RND_WRITE"},
	{ 19, 0x23, -1, fnargs_fcb, 	fnargs_al, 	1.0, "GET_FILESIZE"},
	{ 20, 0x24, -1, fnargs_fcb, 	fnargs_nil, 	1.0, "SET_RND_RECNO"},
	{ 21, 0x25, -1, fnargs_al_ds_dx, fnargs_nil,	1.0, "SET_INT"},
	{ 22, 0x26, -1, fnargs_dx, 	fnargs_nil, 	1.0, "NEWPSP"},
	{ 23, 0x2b, -1, fnargs_cx_dx, 	fnargs_al,	1.0, "SET_SYS_DATE"},
	{ 24, 0x27, -1, fnargs_cx_fcb, 	fnargs_al_cx,	1.0, "RND_BLK_READ"},
	{ 25, 0x28, -1, fnargs_cx_fcb, 	fnargs_al_cx,	1.0, "RND_BLK_WRITE"},
	{ 26, 0x2a, -1, fnargs_nil, 	fnargs_al_cx_dx, 	1.0, "GET_SYS_DATE"},
	{ 27, 0x2c, -1, fnargs_nil, 	fnargs_cx_dx, 	1.0, "GET_SYS_TIME"},
	{ 28, 0x2d, -1, fnargs_cx_dx, 	fnargs_al,	1.0, "SET_SYS_TIME"},
	{ 29, 0x2f, -1, fnargs_nil, 	fnargs_bx_es, 	2.0, "GET_DTA"},
	{ 30, 0x30, -1, fnargs_nil, 	fnargs_ax, 	1.0, "GET_VERSION"},
	{ 31, 0x31, -1, fnargs_al_dx, 	fnargs_nil,	2.0, "TSR"}, /* never returns */
	{ 32, 0x32, -1, fnargs_dl, 	fnargs_al, 	2.0, "GET_DRIVE_PARAMETERS"},
	{ 33, 0x33,  0, fnargs_nil, 	fnargs_dl,	2.0, "GET_BREAK_STATE"},
	{ 34, 0x33,  1, fnargs_dl, 	fnargs_nil,	2.0, "SET_BREAK_STATE"},
	{ 35, 0x33,  2, fnargs_dl, 	fnargs_dl,	3.0, "SET_BREAK_STATE"},
	{ 36, 0x33,  5, fnargs_nil, 	fnargs_dl,	4.0, "GET_BOOT_DRIVE"},
	{ 37, 0x33,  6, fnargs_nil, 	fnargs_bx_dx,	5.0, "GET_TRUE_VERSION"},
	{ 38, 0x34, -1, fnargs_nil, 	fnargs_bx_es,	2.0, "GET_INDOS_ADR"},
	{ 39, 0x35, -1, fnargs_al, 	fnargs_bx_es,	2.0, "GET_INT"},
	{ 40, 0x36, -1, fnargs_dl, 	fnargs_ax_bx_cx_dx,	2.0, "GET_FREE_SPACE"},
	{ 41, 0x39, -1, fnargs_asciz, 	fnargs_cf,	2.0, "MKDIR"},
	{ 42, 0x3a, -1, fnargs_asciz, 	fnargs_cf,	2.0, "RMDIR"},
	{ 43, 0x3b, -1, fnargs_asciz, 	fnargs_cf,	2.0, "CHDIR"},
	{ 44, 0x3c, -1, fnargs_cl_asciz, fnargs_cf_ax,	2.0, "CREATE"},
	{ 45, 0x3d, -1, fnargs_al_cl_asciz, fnargs_cf_ax,	2.0, "OPEN"},
	{ 46, 0x3e, -1, fnargs_bx, 	fnargs_cf,	2.0, "CLOSE"},
	{ 47, 0x3f, -1, fnargs_bx_cx_ds_dx, fnargs_cf_ax,	2.0, "READ"},
	{ 48, 0x40, -1, fnargs_bx_cx_ds_dx, fnargs_cf_ax,	2.0, "WRITE"},
	{ 49, 0x41, -1, fnargs_cl_asciz, fnargs_cf,	2.0, "UNLINK"},
	{ 50, 0x42, -1, fnargs_al_bx_cx_dx, fnargs_cf_ax_dx,	2.0, "LSEEK"},
	{ 51, 0x43, -1, fnargs_al_cl_asciz, fnargs_cf_cx,	2.0, "GETSET_ATTRIB"},
	{ 52, 0x44, -1, fnargs_bx,	fnargs_cf,	2.0, "IOCTL"},
	{ 53, 0x45, -1, fnargs_bx,	fnargs_cf_ax,	2.0, "DUP"},
	{ 54, 0x46, -1, fnargs_bx_cx,	fnargs_cf,	2.0, "FORCEDUP"},
	{ 55, 0x47, -1, fnargs_dl,	fnargs_cf,	2.0, "CWD"},
	{ 56, 0x48, -1, fnargs_bx,	fnargs_cf_ax_bx,	2.0, "MALLOC"},
	{ 57, 0x49, -1, fnargs_es,	fnargs_cf,	2.0, "FREE"},
	{ 58, 0x4a, -1, fnargs_bx_es,	fnargs_cf_ax_bx,	2.0, "REALLOC"},
	{ 59, 0x4b,  0, fnargs_al_asciz, fnargs_nil,	2.0, "EXEC"},
	{ 60, 0x4b,  1, fnargs_al_asciz, fnargs_cf,	2.0, "LOAD"},
	{ 61, 0x4b,  3, fnargs_al_asciz, fnargs_cf,	2.0, "LOAD_OVL"},
	{ 62, 0x4c, -1, fnargs_al,	fnargs_nil,	2.0, "TERMINATE"}, /* never returns */
	{ 63, 0x4d, -1, fnargs_nil,	fnargs_ax,	2.0, "GET_ERRORLEVEL"},
	{ 64, 0x4e, -1, fnargs_al_cl_asciz, fnargs_cf,	2.0, "FINDFIRST"},
	{ 65, 0x4f, -1, fnargs_nil,	fnargs_cf,	2.0, "FINDNEXT"},
	{ 66, 0x50, -1, fnargs_bx,	fnargs_cf,	2.0, "SET_PSP"},
	{ 67, 0x51, -1, fnargs_nil,	fnargs_cf,	2.0, "GET_PSP"},
	{ 68, 0x52, -1, fnargs_nil,	fnargs_nil,	2.0, "SYSVARS"},
	{ 69, 0x53, -1, fnargs_nil,	fnargs_nil,	2.0, "XLAT_BPB2DPB"},
	{ 70, 0x54, -1, fnargs_nil,	fnargs_al,	2.0, "SET_VERIFY"},
	{ 71, 0x55, -1, fnargs_dx_si,	fnargs_cf,	2.0, "NEW_PSP"},
	{ 72, 0x56, -1, fnargs_cl_2_asciz, fnargs_cf,	2.0, "RENAME"},
	{ 73, 0x57,  0, fnargs_bx,	fnargs_cf_cx_dx,	2.0, "GET_FILE_DATE_TIME"},
        { 74, 0x57,  1, fnargs_bx_cx_dx, fnargs_cf,	2.0, "SET_FILE_DATE_TIME"},
	{ 75, 0x58,  0, fnargs_nil,	fnargs_ax,	2.0, "GET_MALLOC_STRATAGY"},
	{ 76, 0x58,  1, fnargs_bl,	fnargs_bx,	2.0, "SET_MALLOC_STRATAGY"},
	{ 77, 0x58,  2, fnargs_dl,	fnargs_al,	5.0, "GET_UMB_LINK_STATE"},
	{ 78, 0x58,  3, fnargs_bx,	fnargs_bx,	5.0, "SET_UMB_LINK_STATE"},
	{ 79, 0x59, -1, fnargs_nil,	fnargs_ax_bx_cx,	3.0, "GET_XERROR"},
	{ 80, 0x5a, -1, fnargs_cl_asciz, fnargs_cf_ax,	3.0, "CREATE_TEMP_FILE"},
	{ 81, 0x5b, -1, fnargs_cl_asciz, fnargs_cf_ax,	3.0, "CREATE"},
	{ 82, 0x5c, -1, fnargs_all,	fnargs_cf,	3.0, "FLOCK"},
	{ 83, 0x5d,  0, fnargs_nil,	fnargs_cf,	3.1, "SERVER_FUNCTION_CALL"},
	{ 84, 0x5d,  1, fnargs_nil,	fnargs_cf,	3.1, "SERVER_COMMIT"},
	{ 85, 0x5d,  2, fnargs_nil,	fnargs_cf,	3.1, "SHARE_NAME_CLOSE"},
	{ 86, 0x5d,  3, fnargs_nil,	fnargs_cf,	3.1, "SHARE_COMPUTER_CLOSEALL"},
	{ 87, 0x5d,  4, fnargs_nil,	fnargs_cf,	3.1, "SHARE_PROCESS_CLOSEALL"},
	{ 88, 0x5d,  5, fnargs_nil,	fnargs_cf,	3.1, "SHARE_GET_OPEN_LIST"},
	{ 89, 0x5d,  6, fnargs_nil,	fnargs_cf,	3.0, "GET_DOSSWAP_ADDR"},
	{ 90, 0x5f,  7, fnargs_dl,	fnargs_cf,	5.0, "ENABLE_DRIVE"},
	{ 91, 0x5f,  8, fnargs_dl,	fnargs_cf,	5.0, "DISABLE_DRIVE"},
	{ 92, 0x62, -1, fnargs_nil,	fnargs_bx,	3.0, "GET_CURRENT_PSP"},
	{ 93, 0x67, -1, fnargs_bx,	fnargs_cf,	3.3, "SET_HANDLE_COUNT"},
	{ 94, 0x68, -1, fnargs_bx,	fnargs_cf,	3.3, "FFLUSH"},
	{ 96, 0x6c,  0, fnargs_bx_cx_dx_asciz_in_si, fnargs_cf_ax_cx, 4.0, "EXTENDED_OPEN"},
 { 97, 0x02, -1, fnargs_al_dl, fnargs_nil, 1.0, "WRITECHAR"},
 { 98, 0x03, -1, fnargs_nil, fnargs_al, 1.0, "READCHAR"},
 { 99, 0x06, -1, fnargs_al_dl, fnargs_nil, 1.0, "DIRECTOUTPUT"},
 {100, 0x09, -1, fnargs_al_ds_dx, fnargs_nil, 1.0, "STRINGOUTPUT"},
 {101, 0x38, -1, fnargs_al_ds_dx, fnargs_cf, 2.0, "COUNTRYINFO"},
/* next number: 101 */
        { -1, -1, -1, fnargs_nil,	fnargs_cf, 0.0, NULL}
};

struct dosfns_s int13fns[] =
/*num, ah, al, argtype, rettype,  dosver, description */
{
  {1, 0x00, -1, fnargs_dl, fnargs_ax, 0, "DISK_RESET"},
  {2, 0x01, -1, fnargs_dl, fnargs_ax, 0, "DISK_STATUS"},
  {3, 0x02, -1, fnargs_disk_io, fnargs_cf_ax, 0, "DISK_READ"},
  {4, 0x03, -1, fnargs_disk_io, fnargs_cf_ax, 0, "DISK_WRITE"},
  {5, 0x04, -1, fnargs_disk_io, fnargs_cf_ax, 0, "DISK_VERIFY"},
  {6, 0x08, -1, fnargs_dl, fnargs_all, 0, "DISK_GET_PARAMS"},
  {7, 0x15, -1, fnargs_dl, fnargs_cf_ax_cx_dx, 0, "DISK_GET_TYPE"},
  {8, 0x18, -1, fnargs_cx_dx, fnargs_ax_es_di, 0,"DISK_SET_MEDIA_TYPE"},
  {9, 0x05, -1, fnargs_disk_io, fnargs_cf_ax, 0, "DISK_FORMAT"},
  {10, 0x16, -1, fnargs_dl, fnargs_cf_ax, 0, "DISK_FLOPPY_CHANGE"},
  {11, 0x17, -1, fnargs_al_dl, fnargs_cf_ax, 0, "DISK_OLD_SET_TYPE"},
  {12, 0x06, -1, fnargs_ax_cx_dx, fnargs_ax, 0, "DISK_FORMAT_BAD"},
  {13, 0x07, -1, fnargs_disk_io, fnargs_ax, 0, "DISK_FORMAT_FROM"},
  {14, 0x0A, -1, fnargs_disk_io, fnargs_cf_ax, 0, "DISK_READ_LONG"},
  {15, 0x0B, -1, fnargs_disk_io, fnargs_cf_ax, 0, "DISK_WRITE_LONG"},
  {16, 0x0C, -1, fnargs_cx_dx, fnargs_cf_ax, 0, "DISK_SEEK"},
  {17, 0x0D, -1, fnargs_dl, fnargs_cf_ax, 0, "DISK_RESET"},
  {18, 0x10, -1, fnargs_dl, fnargs_cf_ax, 0, "DISK_READY"},
  {19, 0x11, -1, fnargs_dl, fnargs_cf_ax, 0, "DISK_CALIBRATE"},
  {-1, -1, -1, fnargs_nil, fnargs_cf, 0, NULL}
};
  
struct dosfns_s int1Afns[] =
/*num, ah, al, argtype, rettype,  dosver, description */
{
  {1, 0x00, -1, fnargs_nil, fnargs_al_cx_dx, 0, "CMOS_GET_CLOCK"},
  {2, 0x01, -1, fnargs_cx_dx, fnargs_ax, 0, "CMOS_SET_CLOCK"},
  {3, 0x02, -1, fnargs_nil, fnargs_cf_cx_dx, 0, "CMOS_GET_TIME"},
  {4, 0x03, -1, fnargs_cx_dx, fnargs_nil, 0, "CMOS_SET_TIME"},
  {5, 0x04, -1, fnargs_nil, fnargs_cf_cx_dx, 0, "CMOS_GET_DATE"},
  {6, 0x05, -1, fnargs_cx_dx, fnargs_nil, 0, "CMOS_SET_DATE"},
  {7, 0x06, -1, fnargs_cx_dx, fnargs_cf_ax, 0, "CMOS_SET_ALARM"},
  {8, 0x07, -1, fnargs_nil, fnargs_cf_ax, 0, "CMOS_SHUT_ALARM"},
  {9, 0x0A, -1, fnargs_nil, fnargs_cx, 0, "CMOS_GET_DAYS_SINCE_1980"},
  {10, 0x0B, -1, fnargs_cx, fnargs_nil, 0, "CMOS_SET_DAYS_SINCE_1980"},
  {-1, -1, -1, fnargs_nil, fnargs_cf, 0, NULL}
};

struct iartype_names_s {
    enum iartype_e type;
    char          *name;
} iartype_names[] =
{
	{ DOSINT, "DOS" },
	{ DISKINT, "BIOSDISK" },
	{ MEM, "MEMORY" },
	{ CMOSINT, "CMOS" },
	{ -1, NULL}
};

