// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#ifndef _ILDVPDISTRIBUTE_H
#define _ILDVPDISTRIBUTE_H

#define LEGACY_FILE					"Legacy.inf"
#define SERVER_FILE					"Server.inf"
#define CONSOLE_FILE				"Console.inf"
#define CLIENT32_FILE				"Client32.inf"
#define AMS_FILE					"Vpams.inf"
#define CLIENT16_FILE				"Client16.inf"
#define CLTSKW_FILE					"Cltskw.inf"
#define DISTRIBUTE_FILE				"Ldvpdist.dll"
#define TRANSMAN_FILE				"Transman.dll"
#define VAR_FILE					"vpdata.var"
#define CLIENT32_DIR				"\\Clients\\Win32"

// Mail Client Flags
#define MAIL_EXCHANGE		0x0001
#define MAIL_NOTES			0x0002
#define MAIL_CCMAIL			0x0004

#define SERVER_TYPE			0x1000
#define ADMIN_TYPE			0x2000
#define CLIENT_TYPE			0x3000
#define COMPON_TYPE			0x4000
#define CLEANUP_TYPE		0x5000
#define FINISH_TYPE			0x6000

// Class dependent flags
// Must leave first 4 bits for copy flags
#define VAL_WINNT_ONLY			0x00000010 // used in class and command flags
#define VAL_WIN95_ONLY			0x00000020 // used in class and command flags
#define VAL_NW3_ONLY			0x00000040 // used in class and command flags
#define VAL_NW4_ONLY			0x00000080 // used in class and command flags
#define VAL_NW5_ONLY			0x00000100 // used in class and command flags
#define WIN98_DISPLAY			0x00000200
#define REBOOT_NEEDED			0x00000400
#define RUN_SILENTLY			0x00000800
#define PERFORM_INSTALL			0x00001000
#define PERFORM_REINSTALL		0x00002000
#define PERFORM_UPDATE			0x00004000
#define PERFORM_UNINSTALL		0x00008000
#define DEST_REMOTE				0x00010000
#define SRC_REMOTE				0x00020000
#define ALL_LOCAL				0x00040000
#define PARTIAL_SCRIPT			0x00080000
#define NO_TRANSPORT_SERVICE	0x00100000
#define REMOTE_CLIENT			0x00200000
#define CLIENT_HELP				0x00400000
#define UPDATE_NOW				0x00800000
#define EXTERNAL_FILE			0x01000000

// flags for already installed servers -- used with class flags
#define NDS_TYPE				0x02000000
#define ADMIN_CDIMAGE			0x04000000
#define AUTO_START				0x08000000
#define CLIENT_FILES			0x10000000
#define ADMIN_FILES				0x20000000
#define AMS_SERVER_FILES		0x40000000
#define AMS_ADMIN_FILES			0x80000000


// MASKS depend on class flags
#define COPYFLAG_MASK(x)		(x & 0x0000000f)
#define OSVER_MASK(x)			(x & 0x000803f0)
#define SNAPIN_FLAGS(x)			(x & 0x0000f1f0) // includes PERFORM action and OS flags


// Useful Macros
#define HIWORD_ONLY(x)			(x & 0xffff0000)
#define EQUAL(x,y)				(!((DWORD)((x) & (y)) ^ (DWORD)(y)))

// Special Variables
#define LANG_ID					"%LANG%"
#define LANG_DIR				"%LANG_DIR%"
#define OS_VER					"%OS%"
#define SRC_PATH				"%SRC_PATH%"
#define DEST_PATH				"%DEST_PATH%"
#define REMOTE_PATH				"%REMOTE_PATH%"
#define SYSTEM_ROOT				"%SYS_ROOT%"
#define WIN_PATH				"%WIN_PATH%"
#define SYS_PATH				"%SYS_PATH%"
#define STARTUP_TYPE			"%STARTUP_TYPE%"
#define LICENSE					"%LICENSE%"
#define PERFORM_SECTION			"%PERFORM_SECTION%"
#define COMPUTER_NAME			"%COMPUTER_NAME%"
#define ADDRESS_0				"%COPYSRV_ADDRESS_0%"
#define ADDRESS_1				"%COPYSRV_ADDRESS_1%"
#define SRV_ADDRESS_1			"%SERVER_ADDRESS_1%"
#define AMS_TOKEN				"%INCLUDE_AMS2%"
#define ADMIN_TOKEN				"%INCLUDE_ADMIN%"
#define CLIENT_TOKEN			"%INCLUDE_CLIENT%"
#define DONGLE_TOKEN			"%DONGLE_TOKEN%"
#define ADMIN_SETUP				"%ADMIN_SETUP%"
#define ADMIN_VERSION			"%ADMIN_VERSION%"


DWORD ReleaseInterfaces();

HRESULT AddComputer(LPCTSTR sName, long dwFlags);
HRESULT AddProduct(long nId, LPCTSTR sCmdFile, LPCTSTR sSrcPath, LPCTSTR sPatchFile, long dwFlags);
HRESULT AddProductVarString(LPCTSTR sVarName, LPCTSTR sVarData);
HRESULT AddProductVarValue(LPCTSTR sVarName, long dwVarData);
HRESULT RemoveProduct(long nId);
HRESULT RemoveComputer(LPCTSTR sName);
HRESULT UpdateComputers(long dwFlags, LPCTSTR sLangID = "");

#endif

