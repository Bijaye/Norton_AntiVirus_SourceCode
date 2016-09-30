Using NTaskMgr during install and uninstall to handle scheduling tasks.
-----------------------------------------------------------------------

Abbreviations used here are NPS (Norton Program Scheduler) and MTS
(Microsoft Task Scheduler, included with Win 98).

NTaskMgr is controlled entirely by command line switches.  The switches
are:

/d  Delete all NAV Scan or NAV LiveUpdate tasks from MTS.  This does *NOT*
    delete tasks from NPS, since the NAV uninstall takes care of removing
    NPS and its tasks.
    If this switch is present, all other switches are ignored.
    Note that NTaskMgr only deletes tasks that were added to MTS by
    apps that use the CTaskAPI library, namely NAVLU32 and NTaskMgr.
    If the user creates any NAV Scan or NAV LU events manually, they
    will not be deleted.

/n  Add a NAV Scan task to NPS.  The task added is currently hard-coded to
    run at 8 PM every Friday, and scan "/L" (all local hard drives).

/w  Use this switch with /n to add the scan task to MTS instead of NPS.

/c  Convert all existing NPS tasks to MTS tasks.  This may be present
    along with /n (or /n /w), in which case the conversion takes place
    before the weekly scan task is added.

/s  Silent mode.  Normally when the /c, /d, or /n actions cannot complete,
    NTaskMgr displays an error message.  /s supresses error messages.

/m  Not an un/install switch, but a message event switch.  NTaskMgr
    displays message boxes for "Display message" events that are converted
    fro NPS to MTS.  This switch indicates that the next command line
    parameter is the text to display in the message box.  Enclose the
    text in quotes if it has any spaces.

/t  This is also a message box switch.  It indicates that the next
    parameter is the text to display in the caption of the message box.
    If this switch is not present but /m is, then the message box has
    default caption text, taken from an NTaskMgr string resource.  If
    this switch is present but /m is not, then this switch is ignored.


Notes:

Any non-switch parameters (except those immediately following /m and /t)
are ignored.  Also, a /m or /t at the end of the command line (with no
following text parameter) is ignored.

/s does not supress a message box when the /m switch is present.

If /m is specified along with /d, /n, /n /w, or /c, then the message is
displayed before the other switches' actions are carried out.

Message boxes always contain the INFORMATION icon.


Examples:

    NTaskMgr /m "Danger Will Robinson!" /t Robot

This displays a message box with the indicated caption and message text.

    NTaskMgr /n

Adds a task to NPS to launch NAV every Friday night at 8 PM.

    NTaskMgr /n /w /c

Converts all existing NPS tasks to MTS tasks, and then adds a task to
launch NAV every Friday night at 8 PM.

