#include <sys/types.h>
#include <sys/ioctl.h>
#include <linux/unistd.h>

#include "PSCAN.H"
#include "symapl.h"
#include "symap_cfg.h"

#include "scancontrol.h"
#include "TrayIPC.h"

SYS_PRAMS SystemPrams;
extern "C" char SystemRunning;

// Declaration of global tray IPC manager
TrayIPCManager g_trayIPC;

// The global configuration options structure
extern CONFIG_OPTIONS gtConfigOptions;

// forward decls
static void SetupAction(PFILE_ACTION pAction);

extern "C" BOOL GetSidForUid( PSID pSid, const uid_t uid );



/**************************************************************************************/
/*
 * In this the Linux version, there is no lower set of FILE_ACTION structs
 * as on Netware.  Instead, any needed state is kept in the upper
 * FILE_ACTION struct (address passed in from caller) -- which currently
 * is just a buffer for the filename, and the FileID.
 */
DWORD DeviceIoControl(HANDLE hDevice,/*DWORD*/ ULONG Command,void *BufIn,DWORD SizeIn,void *BuffOut,DWORD SizeOut,DWORD *ReturnSize,DWORD *IdontKnow) {
    DWORD Status = static_cast<DWORD>(-1);
    PSYS_PRAMS USystemPrams;
    PFILE_ACTION Action;
    int f, rc;
    long csize;        // CFC cache size
    LONG ap_flags = 0;    // AP flag setting
    symap_getact_msg_t GetActMsg;    // message from module: file to scan
    symap_setrsp_msg_t SetRspMsg;    // message to module: scan response

    REF(IdontKnow);
    REF(f);

    if ( hDevice == INVALID_HANDLE_VALUE ) {
        dprintfTag0(DEBUGAUTOPROTECT, "AP: DeviceIoControl given invalid handle value\n");
    } else if ( hDevice == ((HANDLE)-2) ) {
        dprintfTag0(DEBUGAUTOPROTECT, "AP: DeviceIoControl given -2 handle value\n");
    }

    switch (Command) {
        case IOCTL_GET_ACTION:
            // ensure the buffer will be large enough to accept
            // the returned structure
            if (SizeOut < sizeof(FILE_ACTION))
                return EFAULT;

            // we will fill in the caller's FILE_ACTION structure
            // but first we need to set it into a starting state
            Action = (PFILE_ACTION)BuffOut;
            SetupAction(Action);

            // set up ioctl request (point to filename buffer)
            memset(&GetActMsg, 0, sizeof(GetActMsg));
            GetActMsg.namebuf = &Action->Name[0];
            GetActMsg.maxnamelen = sizeof(Action->Name);

            // this call will sleep till there is something
            // to scan (rc == 0) OR we need to exit (rc < 0
            // && errno == ESHUTDOWN).  anything else is an
            // actual error
            rc = symap_ctl(reinterpret_cast<int>(hDevice), SYMAP_IOCGETACT, &GetActMsg);

            if (rc < 0)
            {
                if (errno == ESHUTDOWN)
                {
                    // need to exit
                    dprintfTag0(DEBUGAUTOPROTECT, "AP: GETACT ioctl returned ESHUTDOWN\n");
                    // XXX would be nice if there was somethign
                    // we could return for Status that would
                    // indicate this is a clean exit rather
                    // than some error. alas...
                }
                else
                {
                    // report error
                    dprintfTag2(DEBUGAUTOPROTECT,
                        "AP: GETACT ioctl returned %d (%s)\n",
                        errno, strerror(errno));
                }
            }
            else
            {
                // grab the File ID; terminate the name string
                Action->FileID = GetActMsg.fid;
                if (GetActMsg.namelen < GetActMsg.maxnamelen)
                    Action->Name[GetActMsg.namelen] = '\0';
                else
                    Action->Name[GetActMsg.maxnamelen-1] = '\0';

                // Determine if we have non-root user
                if ( GetActMsg.uid != 0 )
                {
                    PSID pSid = reinterpret_cast<PSID>(&Action->TokenUser.Sid);
                    if ( GetSidForUid( pSid, GetActMsg.uid ) ) {
                        char user[NAME_SIZE];
                        char computer[NAME_SIZE];
                        GetNames( pSid, user, computer, NULL);
                        dprintfTag3( DEBUGAUTOPROTECT, "AP: Matched userid %d to user %s:%s\n", GetActMsg.uid, user, computer );
                    } else {
                        dprintfTag1( DEBUGAUTOPROTECT, "AP: Failed to get user record for user %d.\n", GetActMsg.uid );
                    }
                }

                // Obtain access type mask from message, fill in action flags
                switch (GetActMsg.atype & SYMAP_ATYP__MASK)
                {
                case SYMAP_ATYP_OPEN:
                    Action->Flags = FA_READ | FA_BEFORE_OPEN;
                    break;
                case SYMAP_ATYP_CLOSE:
                    Action->Flags = FA_READ | FA_AFTER_OPEN;
                    break;
                case SYMAP_ATYP_EXEC:
                    Action->Flags = FA_EXEC | FA_BEFORE_OPEN;
                    break;
                case SYMAP_ATYP_RENAME:
                    Action->Flags = FA_RENAME;
                    break;
                default:
                    dprintfTag1( DEBUGAUTOPROTECT,
                                 "AP: GETACT received unexpected access value %d\n",
                                 GetActMsg.atype );
                    break;
                }

                if (GetActMsg.atype & SYMAP_ATYP_FL_WRITE)
                {
                    Action->Flags |= FA_WRITE;
                }

                if (GetActMsg.atype & SYMAP_ATYP_FL_MANDLOCK)
                {
                    Action->Flags |= FA_MANDLOCK;
                }
                
                Action->Flags |= FA_FILE_NEEDS_SCAN;

                if (ReturnSize)
                    *ReturnSize = sizeof(FILE_ACTION);
                Status = STATUS_SUCCESS;
                dprintfTag0(DEBUGAUTOPROTECT, "AP: GETACT successful\n");
            }
            
            break;

        case IOCTL_COMPLETE_ACTION:
            Action = (PFILE_ACTION)BufIn;

            // construct the response struct
            SetRspMsg.fid = Action->FileID;

            // code to map Action->Status into SymAP result
            // code is derived from CompleteAction() in PSCAN_N.C
            switch (Action->Status)
            {
                case DENY_ACCESS:    // infected, deny
                    SetRspMsg.ret = SYMAP_RSP_DENY;
                    break;

                case ALLOW_VIRUS_ACCESS: // dirty, allow
                default:
                // cases implied for SKIP_INFECTION,
                // CAN_NOT_SCAN, BAD_VIRUS_NAME (see
                // PSCAN_N CompleteAction())
                    // these cases all require us to allow
                    // the access but not cache as "clean"
                    SetRspMsg.ret = SYMAP_RSP_ALLOW_NAME;
                    break;

                case 0:    // clean, allow (no name for this -- really!)
                    SetRspMsg.ret = SYMAP_RSP_ALLOW;
                    break;
            }

            // pass it down
            rc = symap_ctl(reinterpret_cast<int>(hDevice), SYMAP_IOCSETRSP, &SetRspMsg);

            if (rc < 0)
            {
                // report error talking to AP
                dprintfTag2(DEBUGAUTOPROTECT,
                    "AP: SETRSP ioctl returned %d (%s)\n",
                    errno, strerror(errno));
            }
            else
            {
                Status = STATUS_SUCCESS;
                dprintfTag0(DEBUGAUTOPROTECT, "AP: SETRSP successful\n");
            }
            break;


        case IOCTL_SET_SYS_PRAMS:
            // set up SymAP-style params word
            // XXX The PSCAN_N.C version will accept a "short"
            // SYS_PRAMS struct, but this isn't used in
            // the application and isn't going to be supported
            // here.  The entire struct should be passed.
            if (SizeIn != sizeof(SYS_PRAMS))
            {
                // this is a programming error (assertion?)
                break;
            }

            USystemPrams = (PSYS_PRAMS)BufIn;
            ap_flags = 0;

            // -- map access type flags
            if (USystemPrams->DoWrites)
                ap_flags |= SYMAP_FL_AP_WR;
            if (USystemPrams->DoReads)
                ap_flags |= SYMAP_FL_AP_RD;
            if (USystemPrams->DoExecs)
                ap_flags |= SYMAP_FL_AP_EX;

            // -- map medium/volume type flags
            if (USystemPrams->DoFloppy)
                ap_flags |= SYMAP_FL_AP_D_FL;    // floppy
            if (USystemPrams->DoNetwork)
                ap_flags |= SYMAP_FL_AP_D_NE;    // remote
            if (USystemPrams->DoHardDisk)
                ap_flags |= SYMAP_FL_AP_D_FX;    // fixed
            if (USystemPrams->DoCDRom)
                ap_flags |= SYMAP_FL_AP_D_RM;    // removable

            // -- map AP options
            if (USystemPrams->HoldOnClose)
                ap_flags |= SYMAP_FL_AP_HC;    // hold close
            // if (USystemPrams->DoTrap)    XXX    // ???
            // if (USystemPrams->DoIS)    XXX    // ???

#ifdef DEBUG
            // for debug builds only, set the no wait flag if so configured
            if (gtConfigOptions.bDebugScanNoWait)
                ap_flags |= SYMAP_FL_AP_NW;
#endif

            // stuff this down to the SymAP module
            rc = symap_ctl_dir(reinterpret_cast<int>(hDevice), SYMAP_IOCSETFL, ap_flags);

            // check return (which returns prior flag settings
            // or <0 if error)
            if (rc >= 0)
                Status = STATUS_SUCCESS;
            else
            {
                // report error talking to AP
                dprintfTag2(DEBUGAUTOPROTECT,
                    "AP: SETFL ioctl returned %d (%s)\n",
                    errno, strerror(errno));
            }

            // now set the CFC size
            // - it's OK to do this even if the size is
            // unchanged, since that will be a NOP,
            // cheaper than inquiring the size first
            switch (USystemPrams->DoCache)
            {
            case FILE_CACHE_STATE_DISABLE:
                csize = SYMAP_CFC_SIZE_DISABLED;
                break;
            case FILE_CACHE_STATE_DEFAULT:
                csize = SYMAP_CFC_SIZE_DEFAULT;
                break;
            case FILE_CACHE_STATE_CUSTOM:
                csize = gtConfigOptions.dwFileCacheEntries;
                break;
            default:
                dprintfTag1( DEBUGAUTOPROTECT,
                             "AP: CFC state unexpected value %d\n",
                             USystemPrams->DoCache );
                // Use default value in this case
                csize = SYMAP_CFC_SIZE_DEFAULT;
            }
            
            rc = symap_ctl_dir(reinterpret_cast<int>(hDevice), SYMAP_IOCCACHE_SET, csize);

            // check return (which returns prior CFC size setting
            // or <0 if error)
            if (rc < 0)
            {
                // report error talking to AP
                dprintfTag2(DEBUGAUTOPROTECT,
                    "AP: CACHE_SET ioctl returned %d (%s)\n",
                    errno, strerror(errno));

                // force status back to bad since both have
                // to succeed
                if (Status == STATUS_SUCCESS)
                    Status = 0xffffffff;
            }

            if (Status == STATUS_SUCCESS)
            {
                dprintfTag0(DEBUGAUTOPROTECT,
                    "AP: SETFL and CACHE_SET successful\n");
            }

            // status has now been set to SUCCESS if both
            // ioctls succeeded
            break;

        case IOCTL_GET_SYS_PRAMS:
            // not sure whether this will be used on Linux,
            // or whether it would be a good idea to use it,
            // but here it is just in case.

            if (SizeOut < sizeof(SYS_PRAMS))
            {
                // programming error (assertion?)
                break;
            }

            USystemPrams = (PSYS_PRAMS)BuffOut;
            memset(USystemPrams, 0, sizeof(SYS_PRAMS));

            // pull the current flags from the driver
            ap_flags = symap_ctl(reinterpret_cast<int>(hDevice), SYMAP_IOCGETFL, NULL);

            if (ap_flags < 0)
            {
                // error retrieving flags
                dprintfTag2(DEBUGAUTOPROTECT,
                    "AP: GETFL ioctl returned %d (%s)\n",
                    errno, strerror(errno));

                break;
            }

            // XXX probably should also be getting CFC size
            // to return somewhere


            USystemPrams->MySize = sizeof(SYS_PRAMS);

            // -- map access type flags
            USystemPrams->DoWrites = (ap_flags & SYMAP_FL_AP_WR) ?
                1 : 0;
            USystemPrams->DoReads = (ap_flags & SYMAP_FL_AP_RD) ?
                1 : 0;
            USystemPrams->DoExecs = (ap_flags & SYMAP_FL_AP_EX) ?
                1 : 0;

            // -- map medium/volume type flags
            USystemPrams->DoFloppy = (ap_flags & SYMAP_FL_AP_D_FL) ?
                1 : 0;    // floppy
            USystemPrams->DoNetwork = (ap_flags & SYMAP_FL_AP_D_NE) ?
                1 : 0;    // remote
            USystemPrams->DoHardDisk = (ap_flags & SYMAP_FL_AP_D_FX) ?
                1 : 0;    // fixed
            USystemPrams->DoCDRom = (ap_flags & SYMAP_FL_AP_D_RM) ?
                1 : 0;    // removable

            // -- map AP options
            USystemPrams->HoldOnClose = (ap_flags & SYMAP_FL_AP_HC) ?
                1 : 0;    // hold close
            // USystemPrams->DoTrap    XXX    // ???
            // USystemPrams->DoIS    XXX    // ???

            if (ReturnSize)
                *ReturnSize = sizeof(SYS_PRAMS);

            Status = STATUS_SUCCESS;
            break;

        case IOCTL_INQUIRE_SYSTEM: 
            // LRL 10/18/04 - this appears to be unused
            if (ReturnSize)
                *ReturnSize = 0;
            Status = STATUS_SUCCESS;
            break;


        case IOCTL_START_CAPTURE:
            // tell AP module to start sending us file scan
            // requests
            rc = symap_ctl_dir(reinterpret_cast<int>(hDevice), SYMAP_IOCSETAP, SYMAP_AP_ENABLE);

            if (rc == 0)
            {
                dprintfTag0(DEBUGAUTOPROTECT,
                    "AP: SETAP(ENABLE) successful\n");
                Status = STATUS_SUCCESS;
            }
            else
            {
                // report error talking to AP
                dprintfTag2(DEBUGAUTOPROTECT,
                    "AP: SETAP(ENABLE) ioctl returned %d (%s)\n",
                    errno, strerror(errno));
            }
            break;

        case IOCTL_END_CAPTURE:
            // tell AP module to stop sending us file scan
            // requests; scanner threads in progress will be
            // allowed to finish but any/all user processes will
            // be released immediately; idle scanners will
            // not notice anything unusual
            rc = symap_ctl_dir(reinterpret_cast<int>(hDevice), SYMAP_IOCSETAP, SYMAP_AP_DISABLE);

            if (rc == 0)
            {
                dprintfTag0(DEBUGAUTOPROTECT,
                    "AP: SETAP(DISABLE) successful\n");
                Status = STATUS_SUCCESS;
            }
            else
            {
                // report error talking to AP
                dprintfTag2(DEBUGAUTOPROTECT,
                    "AP: SETAP(DISABLE) ioctl returned %d (%s)\n",
                    errno, strerror(errno));
            }
            break;

        case IOCTL_INTEL_SHUTDOWN:
            // tell AP module that we want to exit.  this
            // behaves like AP_DISABLE above except that
            // idle scanner threads will return with ESHUTDOWN
            // error code.  this mode is undone with the
            // AP_ENABLE or AP_DISABLE ioctls.
            rc = symap_ctl_dir(reinterpret_cast<int>(hDevice), SYMAP_IOCSETAP, SYMAP_AP_UNLOAD);

            if (rc == 0)
            {
                dprintfTag0(DEBUGAUTOPROTECT,
                    "AP: SETAP(UNLOAD) successful\n");
                Status = STATUS_SUCCESS;
            }
            else
            {
                // report error talking to AP
                dprintfTag2(DEBUGAUTOPROTECT,
                    "AP: SETAP(UNLOAD) ioctl returned %d (%s)\n",
                    errno, strerror(errno));
            }
            break;

        case IOCTL_SET_EP:
            break;
        }

    return Status==STATUS_SUCCESS?TRUE:FALSE;
}

/********************************************************************************************/


// XXX this whole thing is probably obsolete at this level.  we'd need to make a syscall
// to pull up the data.  is it needed (in this form)?
void ActChk(void *nothing) {

	int i;

	REF(nothing);

	while (SystemRunning) {
/***  JUNK // this won't work any more
		CheckActionTable();
		if (SystemPrams.Dbg&0x10) {
			_printf("WA:%u OWFC:%u OSNE: %u FNS:%u FS:%u A:%u AIU:%u TO:%u FLPU:%u\n",
				SystemPrams.WaitingActions,
				SystemPrams.OpensWaitingForClose,
				SystemPrams.OpensStartedNotEnded,
				SystemPrams.FileNotScaned,
				SystemPrams.FilesScaned,
				SystemPrams.Allocs,
				SystemPrams.ActionsInUse,
				SystemPrams.TimedOut,
				SystemPrams.FilesSlippedPastUs,
				SystemPrams.Faults,
				SystemPrams.ClientCount);
			}
***/
		for (i=0;i<30&&SystemRunning;i++)
			NTxSleep(1000);
		}

}
/******************************************************************/
DWORD DriverStart(void)
{
	// (default SystemParms initialization has moved to SymAP init)

	// XXX is this thread needed any more (see ActChk())?
	// MyBeginThread(ActChk,0,"RTV ActChk");

	return(STATUS_SUCCESS);
}

//============================================================================
VOID DriverUnload(void)
{
	return;
}
/*******************************************************************/

/***************************************************************************
 * DeviceOpen() - open a descriptor to the AP kernel module.
 * Returns a descriptor != -1 or -1 in case of error and *lpErr set; error detail is
 * logged in debug log
 */
HANDLE DeviceOpen(LPDWORD lpErr)
{
    int fd = -1;
    DWORD err = ERROR_SUCCESS;

    fd = open(SYMAP_NODENAME, O_RDWR, 0);

    if (fd != -1)
    {
        // If the AP device open succeeded, then the very first thing we do is
        // to exclude the current process. We want no intermediate operations
        // which might generate deadlocking hazards.

        int rc = symap_ctl_dir( fd, SYMAP_IOCEXCLPR, 0 );
        if (0 == rc)
        {
            dprintfTag2(DEBUGAUTOPROTECT, "AP: opened <%s> on fd=%d\n",
                        SYMAP_NODENAME, fd);
        }
        else
        {
            err = errno;

            if (0 == close( fd ))
            {
                fd = -1;
                dprintfTag3( DEBUGAUTOPROTECT,
                             "AP: failed to disable AP for process %d: %s (%d)\n",
                             getpid(), strerror( errno ), errno );
            }
            else
            {
                dprintfTag5( DEBUGAUTOPROTECT,
                             "AP: couldn't recover from failure to disable AP for process %d: %s (%d) - orig: %s (%d)\n",
                             getpid(), strerror( errno ), errno, strerror( err ), err );
            }
        }

    } else if (0 != gtConfigOptions.dwOnOff) {
        // ok, the device open has failed, but we only complain if AP is enabled

        //  have to save errno since dprintf might stomp on it
        err = static_cast<DWORD>(errno);

        // some error -- translate it
        dprintfTag3(DEBUGAUTOPROTECT, "AP: open of <%s> failed: %s (%d)\n",
                    SYMAP_NODENAME, strerror(err), err);
    }

    if ( lpErr )
        *lpErr = err;

    return reinterpret_cast<HANDLE>(fd);
}

/***************************************************************************
 * DeviceClose() - close a descriptor to the AP kernel module.
 * Fails silently if the passed descriptor wasn't valid (but logs
 * the error)
 */
void DeviceClose(HANDLE han)
{
	int fd = reinterpret_cast<int>(han);

	if (han == INVALID_HANDLE_VALUE)
	{
		dprintfTag2(DEBUGAUTOPROTECT, "AP: closed <%s> on fd=%d\n",
			SYMAP_NODENAME, fd);
		return;
	}

	if (close(fd) == 0)
		return;

	// some error -- translate it
	dprintfTag3(DEBUGAUTOPROTECT, "AP: close of <%d> failed: %d (%s)\n",
		fd, errno, strerror(errno));

	return;
}

/***************************************************************************
 * DeviceFlushCleanFileCache() - forces a flush of the kernel-side clean
 * file cache. Returns true on success, false on failure.
 */
BOOL DeviceFlushCleanFileCache(HANDLE han)
{
    int fd = reinterpret_cast<int>(han);

    if (INVALID_HANDLE_VALUE == han)
    {
        dprintfTag1( DEBUGAUTOPROTECT,
                     "AP: DeviceFlushCleanFileCache given invalid handle value %d\n",
                     fd );
        return FALSE;
    }

    int rc = symap_ctl( fd, SYMAP_IOCCACHE_FL, (void*)NULL );
    if (rc >= 0)
    {
        dprintfTag0( DEBUGAUTOPROTECT,
                     "AP: DeviceFlushCleanFileCache succeeded\n" );
        return TRUE;

    }
    else
    {
        dprintfTag2( DEBUGAUTOPROTECT,
                     "AP: DeviceFlushCleanFileCache failed: %d (%s)\n",
                     errno, strerror( errno ) );
        return FALSE;
    }
}

// set up a fresh FILE_ACTION struct to be returned to the scanner.
// This is loosely derived from a combination of pscan_n.c's
// InitAction(), GetAction(), and the MyXXX() event handlers
static void
SetupAction(PFILE_ACTION pAction)
{
	memset(pAction, 0, sizeof(FILE_ACTION));	// safety

	// NB: the Linux implementation is not doing the FA_MAGIC
	// testing.  It also doesn't need to track the Address field,
	// which is used on Netware across the user/driver boundary.
	pAction->Time = GetCurrentTime();
}

// Used to perform control actions on custom scans, triggered by change to
// ProductControl key. Iterates through custom scans, performs requested
// action for each
void ControlCustomScans(void)
{
    char    name[IMAX_PATH];
    HKEY    hlkey;
    char    RootName[IMAX_PATH];

    sssnprintf(RootName, sizeof(RootName), VpRegBase[HKEY_VP_USER_SCANS&0xff].Key, "");

    // At the time of writing this comment, we are looking under the following Registry Path:
    // '\\HKEY_CURRENT_USER\Software\Intel\Landesk\VirusProtect6\CurrentVersion\CustomTasks'
    // Open the handle to that path.
    if ( RegOpenKey(VpRegBase[HKEY_VP_USER_SCANS&0xff].hBase,RootName,&hlkey) == ERROR_SUCCESS )
    {
        int index=0;

        // Iterate through all the registry keys found in the path listed above.
        while ( RegEnumKey(hlkey,index++,name,sizeof(name)) == ERROR_SUCCESS )
        {
            HKEY subKey;
            if (ERROR_SUCCESS == RegOpenKey( hlkey, name, &subKey ))
            {
                bool closeKey = true;
                DWORD controlVal = GetVal( subKey, szReg_Val_CustomScanControl, SC_NOTHING );
                PutVal( subKey, szReg_Val_CustomScanControl, SC_NOTHING );
                switch (controlVal)
                {
                case SC_NOTHING:
                    break;
                case SC_START:
                    // Once we call StartServerScan, the scanning thread owns
                    // that key, so don't close it here.
                    dprintfTag1( DEBUGSCAN, "Starting custom scan %s\n", name );
                    StartServerScan( subKey, NULL );
                    closeKey = false;
                    break;
                default:
                    dprintfTag2( DEBUGSCAN, "Unknown custom scan control value for %s: %d\n", name, controlVal );
                    break;
                }

                if (closeKey)
                {
                    RegCloseKey( subKey );
                }
            }
        }
        RegCloseKey(hlkey);
    }
}

