/**********************************************************************
** File:  symarc.stb
**
** Shared .RC file .C* file symbols
***********************************************************************
** $Header:   S:/SYMALOG/VCS/symarc.h_v   1.11   13 Jun 1996 14:28:22   PVENKAT  $
** 06/28/93 WRL is William R. Lee II (BLEE)
**            - Added IDS_MSGACTIONTMPL to supercede any input template
**              and use local version that expects message and and
**              action field types (along with the usual date/time).
**********************************************************************/
#define IDS_COMMDLG_TITLE   4
#define IDS_COMMDLG_FILTER  5

#define IDS_DEFPRINTFILE    6           /* Default print file name   */
#define IDS_PRINTTITLE     10           /* Print title (dlg and hardcpy) */

#define IDS_VIRLISTCHANGE  20
#define IDS_ERROR          21
#define IDS_INOCHANGE      22
#define IDS_KNOWNVIRUS     23
#define IDS_UNKNOWNVIRUS   24
#define IDS_LOADUNLOAD     25
#define IDS_STARTEND       26
#define IDS_STATUS         27
#define IDS_VIRUSLIKE      28
#define IDS_PT_NLM         29
#define IDS_PT_TSR         30
#define IDS_PT_WINDOWS     31
#define IDS_INGOING        32
#define IDS_OUTGOING       33
#define IDS_PT_WINVXD      34

#define IDS_ZAPFILE        35

#define IDS_MSGACTIONTMPL  36           /* Local template string     */
#define IDS_BADFILE        37

#define IDS_NAV_NAME       38


/**********************************************************************
** Dialog Box and Control ID numbers
**********************************************************************/
#define IDD_PRINT          200
#define IDI_PR_PRINTER        201
#define IDI_PR_FILE           202
#define IDI_PR_USER           203
#define IDI_PR_FILENAME       204

#define IDD_ADJUST_LOG     300
#define IDI_CLEAR_ALL         301
#define IDI_CLEAR_SOME        302
#define IDI_KEEP_SIZE         303
#define IDI_KEEP_ENTRIES      304
#define IDI_KEEP_DAYS         305
#define IDI_KB                306
#define IDI_ENTRIES           307
#define IDI_DAYS              308

#define IDD_OVERWRITE_APPEND  400
#define IDI_ZAP_TEXT1            401
#define IDI_ZAP_TEXT2            402
#define IDI_OVERWRITE            403
#define IDI_APPEND               404

