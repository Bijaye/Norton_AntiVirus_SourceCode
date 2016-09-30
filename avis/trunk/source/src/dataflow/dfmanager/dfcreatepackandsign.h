UINT CleanAfterProgramCrash( LPVOID pParam );

#ifdef CRASH_RECOVER_MAIN
int crashRecoverStatus;
#else
extern int crashRecoverStatus;
#endif

#define CRASH_RECOVER_STARTED   1
#define CRASH_RECOVER_SIGNALLED_TO_STOP     2
#define CRASH_RECOVER_STOPPED   3
