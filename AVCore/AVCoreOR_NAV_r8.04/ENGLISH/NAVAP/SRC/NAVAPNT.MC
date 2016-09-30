;// Copyright 1996 - 1998 Symantec, Peter Norton Product Group
;//**************************************************************************
;//
;// Description:
;//     This file contains all messages that will be logged to NT's event
;//     log by NAVAP.
;//
;//     NAVAP logs three types of messages - error, warning, and
;//     informational.  The range of event codes for errors is 1 through
;//     999, warnings is 1001 through 1999, and informational is 2001
;//     through 2999.
;//
;//     The file NAVAPNT.CPP contains translation tables which convert the
;//     platform independent MSG_ enumerators to message IDs specific to
;//     SYM_NTK defined in this file.  The file UISTRS.CPP in APUI contains
;//     tables which convert the platform independent MSG_ enumerators
;//     to the SYM_VXD specific message strings defined in the same file.
;//
;//     Please keep all messages synchronized!
;//
;//**************************************************************************

MessageIdTypedef=NTSTATUS

SeverityNames=(
        Informational=0x1:STATUS_SEVERITY_INFORMATIONAL
        Warning=0x2:STATUS_SEVERITY_WARNING
        Error=0x3:STATUS_SEVERITY_ERROR
        )

FacilityNames=(
        NAVAP=0x7:FACILITY_NAVAP_ERROR_CODE
        )

MessageId=0x0001 Facility=NAVAP Severity=Error SymbolicName=NTSTATUS_NAVAP_0001
Language=English
Reserved error code.
.

MessageId=0x0002 Facility=NAVAP Severity=Error SymbolicName=NTSTATUS_NAVAP_Unable_To_Create_Device
Language=English
Unable to create device object (\Device\NAVAP).
.

MessageId=0x0003 Facility=NAVAP Severity=Error SymbolicName=NTSTATUS_NAVAP_Unable_To_Create_Symbolic_Link
Language=English
Unable to create symbolic link (\DosDevices\NAVAP).
.

MessageId=0x0004 Facility=NAVAP Severity=Error SymbolicName=NTSTATUS_NAVAP_Core_Init_Failed
Language=English
Unable to initialize internal data.
.

MessageId=0x0005 Facility=NAVAP Severity=Error SymbolicName=NTSTATUS_NAVAP_SYMEvent_Not_Loaded
Language=English
SYMEVENT.SYS is not loaded.
.

MessageId=0x0006 Facility=NAVAP Severity=Error SymbolicName=NTSTATUS_NAVAP_SYMEvent_Incompatible_Version
Language=English
Incompatible version of SYMEVENT.SYS is loaded.
.

MessageId=0x0007 Facility=NAVAP Severity=Error SymbolicName=NTSTATUS_NAVAP_Cant_Save_Persistent_Data
Language=English
Unable to save persistent data.
.

MessageId=0x0008 Facility=NAVAP Severity=Error SymbolicName=NTSTATUS_NAVAP_Cant_Allocate_PEL_ThreadVM_Data
Language=English
Unable to allocate thread data.
.

MessageId=0x0009 Facility=NAVAP Severity=Error SymbolicName=NTSTATUS_NAVAP_DriverCommInit_Error
Language=English
Unable to initialize user mode communication.
.

MessageId=0x000A Facility=NAVAP Severity=Error SymbolicName=NTSTATUS_NAVAP_SID_Length_Increased
Language=English
Change of the SID length requires adjustment of internal data structures. Try reloading SYMEVENT.SYS or restarting the system.
.

MessageId=0x000B Facility=NAVAP Severity=Error SymbolicName=NTSTATUS_NAVAP_Cant_Allocate_Open_File_Data
Language=English
Unable to allocate open file data.
.

MessageId=0x000C Facility=NAVAP Severity=Error SymbolicName=NTSTATUS_NAVAP_Cant_Initialize_File_Caching
Language=English
Unable to initialize file caching.
.

MessageId=0x000D Facility=NAVAP Severity=Error SymbolicName=NTSTATUS_NAVAP_Unable_To_Build_NAVAP_SYS_Name
Language=English
Unable to locate NAVAP.SYS.
.

MessageId=0x000E Facility=NAVAP Severity=Error SymbolicName=NTSTATUS_NAVAP_Invalid_Event_Handles
Language=English
Unable to install event hooks.
.

MessageId=0x000F Facility=NAVAP Severity=Error SymbolicName=NTSTATUS_NAVAP_Unable_To_Synchronize_Configuration
Language=English
Unable to synchronize configuration loading.
.

MessageId=0x0010 Facility=NAVAP Severity=Error SymbolicName=NTSTATUS_NAVAP_Unable_To_Locate_Configuration
Language=English
Unable to determine the location of the configuration files.
.

MessageId=0x0011 Facility=NAVAP Severity=Error SymbolicName=NTSTATUS_NAVAP_Config_Bad_Path_Specified
Language=English
Bad path to the configuration files specified.
.

MessageId=0x0012 Facility=NAVAP Severity=Error SymbolicName=NTSTATUS_NAVAP_Config_Unable_To_Read
Language=English
Unable to read the configuration files.
.

MessageId=0x0013 Facility=NAVAP Severity=Error SymbolicName=NTSTATUS_NAVAP_Config_VLScanInit_Failed
Language=English
Unable to initialize the virus scanning engine.
.

MessageId=0x0014 Facility=NAVAP Severity=Error SymbolicName=NTSTATUS_NAVAP_Config_VLRegisterVirusDB_Failed
Language=English
Unable to initialize the virus scanning engine database files.
.

MessageId=0x0015 Facility=NAVAP Severity=Error SymbolicName=NTSTATUS_NAVAP_0015
Language=English
Unable to initialize the virus scanning engine cache.
.

MessageId=0x0016 Facility=NAVAP Severity=Error SymbolicName=NTSTATUS_NAVAP_Config_NAVAPInitExclusionAPI_Failed
Language=English
Unable to initialize the exclusion database.
.

MessageId=0x0017 Facility=NAVAP Severity=Error SymbolicName=NTSTATUS_NAVAP_DriverComm_Register_Callbacks_Error
Language=English
Unable to register user mode communication callbacks.
.

MessageId=0x0018 Facility=NAVAP Severity=Error SymbolicName=NTSTATUS_NAVAP_Cant_Create_Device_List
Language=English
Unable to create list of devices.
.

MessageId=0x0019 Facility=NAVAP Severity=Error SymbolicName=NTSTATUS_NAVAP_Unable_To_Determine_Startup_Name
Language=English
Unable to determine the Auto-Protect startup name.
.

MessageId=0x001A Facility=NAVAP Severity=Error SymbolicName=NTSTATUS_NAVAP_SelfCheckError_No_CRC
Language=English
The file NAVAP.SYS has been altered. This may be due to virus activity.
.

MessageId=0x001B Facility=NAVAP Severity=Error SymbolicName=NTSTATUS_NAVAP_SelfCheckError_CRC_Changed
Language=English
The file NAVAP.SYS has been altered. This may be due to virus activity.
.

MessageId=0x001C Facility=NAVAP Severity=Error SymbolicName=NTSTATUS_NAVAP_SelfCheckError_File_IO
Language=English
I/O error while self-testing NAV Auto-Protect.
.

MessageId=0x001D Facility=NAVAP Severity=Error SymbolicName=NTSTATUS_NAVAP_SelfCheckError_Internal
Language=English
Internal error while self-testing NAV Auto-Protect.
.

MessageId=0x001E Facility=NAVAP Severity=Error SymbolicName=NTSTATUS_NAVAP_Unable_To_Locate_Scan_Directory
Language=English
Unable to determine the location of the virus definition files.
.

MessageId=0x03E9 Facility=NAVAP Severity=Warning SymbolicName=NTSTATUS_NAVAP_Out_Of_Memory_Error
Language=English
System memory is running very low. Norton AntiVirus Auto-Protect may not be able to function properly.
.

MessageId=0x03EA Facility=NAVAP Severity=Warning SymbolicName=NTSTATUS_NAVAP_Unable_To_Open_RegistryPath
Language=English
Unable to open RegistryPath.
.

MessageId=0x03EB Facility=NAVAP Severity=Warning SymbolicName=NTSTATUS_NAVAP_Unable_To_Close_RegistryPath
Language=English
Unable to close RegistryPath.
.

MessageId=0x03EC Facility=NAVAP Severity=Warning SymbolicName=NTSTATUS_NAVAP_Invalid_Stack_Swapping_Level
Language=English
Invalid stack swapping level.
.

MessageId=0x07D1 Facility=NAVAP Severity=Informational SymbolicName=NTSTATUS_NAVAP_Config_Load_Defaults_Used
Language=English
The configuration files were not found. Using default configuration.
.
