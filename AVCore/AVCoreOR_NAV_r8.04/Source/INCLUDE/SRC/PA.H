// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/pa.h_v   1.2   18 Mar 1998 15:02:10   tcashin  $
//
// Description:
//  Contains CPatchApplication and CPatchApplicationFactory class
//  declarations.  These are the user's interface to the object.
//
// Contains:
//
// See Also:
//
//************************************************************************
// $Log:   S:/INCLUDE/VCS/pa.h_v  $
// 
//    Rev 1.2   18 Mar 1998 15:02:10   tcashin
// Moved from avcomp with extern "C" change.
// 
//    Rev 1.4   17 Mar 1998 18:45:36   tcashin
// Oops ... C function needs to be extern "C".
// 
//    Rev 1.3   17 Mar 1998 11:34:52   CNACHEN
// Added standard C function export.
// 
//    Rev 1.2   03 Mar 1998 17:46:36   CNACHEN
// Added export code to export from dll.
// 
//    Rev 1.1   03 Mar 1998 12:21:06   CNACHEN
// Parameter lpszPatchDirectory added to ApplyPatch to replace patchfile parameter.
// 
// 
//    Rev 1.0   27 Feb 1998 17:46:30   CNACHEN
// Initial revision.
// 
//************************************************************************

#ifndef _PA_H_

#define _PA_H_

class CPatchApplicationNotify
{
    public:

        //************************************************************************
        // Sink()
        //
        // Purpose
        //
        //      This is the sink function call for notifications
        //
        // Parameters:
        //
        //      dwPercentDone   % completed
        //      lpbAbort        The sink function should set *lpbAbort to TRUE
        //                      to abort the operation in progress
        //
        // Returns:
        //
        //      nothing
        //
        //************************************************************************
        // 9/11/97 Carey created.
        //************************************************************************

        virtual void Sink
        (
            DWORD               dwPercentDone,
            LPBOOL              lpbAbort
        ) = 0;
};

class CPatchApplication
{
    public:

        //************************************************************************
        // Release()
        //
        // Purpose
        //
        //      This releases all memory for the object and lets the object
        //  delete itself.
        //
        // Parameters:
        //
        //  None
        //
        // Returns:
        //
        //  nothing
        //
        //************************************************************************
        // 2/27/98 Carey created.
        //************************************************************************

        virtual void Release
        (
            void
        ) = 0;

        //************************************************************************
        // ApplyPatch()
        //
        // Purpose
        //
        //      This method takes a specified patch file, and a set of hub
        //  datafiles and patches to a new hub and a target set of data files.
        //  If this function fails, all directories will be reset to their
        //  original state.
        //  Note: The lpszTargetDirectory should be empty at the start of
        //        patching.
        //  Note: Only one instance of this object may be instantiated and
        //        used at a time.
        //
        // Parameters:
        //
        //  lpszPatchDirectory: Directory containing all files extracted from
        //                      the LiveUpdate patch ZIP.
        //  lpszHubDirectory: Directory containing current hub definition set
        //                    This function will replace the old hub with the
        //                    new hub if successful.
        //  lpszTargetDirectory: Directory where to place latest virus
        //                       definition set, if successful.
        //  lpcSink: Callback class
        //
        // Returns:
        //
        //  TRUE on success
        //  FALSE on failure
        //
        //************************************************************************
        // 2/27/98 Carey created.
        //************************************************************************

        virtual BOOL ApplyPatch
        (
            LPTSTR                      lpszPatchDirectory,
            LPTSTR                      lpszHubDirectory,
            LPTSTR                      lpszTargetDirectory,
            CPatchApplicationNotify *   lpcSink
        ) = 0;
};

//************************************************************************
// CreateInstance()
//
// Purpose
//
//      This is the class factory for the CPatchApplication class
//
// Parameters:
//
// Returns:
//
//      A new instance of the CPatchApplication.
//
//************************************************************************
// 9/11/97 Carey created.
//************************************************************************

#ifdef _PATCHAPP_CPP_
#define PADLLImportExport __declspec(dllexport)
#else
#define PADLLImportExport __declspec(dllimport)
#endif

class PADLLImportExport CPatchApplicationFactory
{
    public:

        static CPatchApplication * CreateInstance
        (
            void
        );
};

#ifdef __cplusplus
extern "C" {
#endif

PADLLImportExport CPatchApplication *CreatePatchApplicationInstance(void);

#ifdef __cplusplus
}
#endif

#endif // #ifndef _PA_H_
