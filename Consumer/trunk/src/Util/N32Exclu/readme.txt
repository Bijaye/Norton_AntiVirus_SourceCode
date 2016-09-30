NAV Exclusions Library v0.1.0

 STATUS: This module is currently CODE COMPLETE. All functionality
has been coded and appears to work in the expected cases. The code is messy
and definitely not optimized, however, the interfaces have stabilized.
You may rely on the interfaces not incorporating any major changes between
now and 1.0.0 (final/stable).
 
 This module encapsulates 'exclusions' - user preferences that indicate
what events the user would prefer to have NAV ignore. There are two
types of exclusions: filesystem and "Anomaly" (VID+GUID). Filesystem
exclusions operate on path strings; Anomaly exclusions operate on
an OR'd tuple of VIDs and GUIDs - if either the supplied VID or the supplied
GUID match, then the anomaly exclusion matches.

 The external interface is now considered to be 'alpha'; the functions
you see there _probably_ won't change in form, much, although there may
be additions as new externally-addressable functionality becomes apparent.

 DEPENDENCIES: Requires NavToolbox, CCLib, CCSettings.
 
Version History
---------------
0.1.0-r1    30 Mar 2005     Ian Dundore
    * CODE COMPLETE!
    * Fixed CESM deserialization; CEF was improperly assigning IDs.
    * Serialization/deserialization persists data properly, as far as I
        can observe.
    * External interface change: saveState now returns an ExResult instead of
        a boolean.
    * Multithreading protection added.
    * Now fires the SYM_REFRESH_NAV_EXCLUSIONS event when saveState succeeds.
    
0.0.3-r1    29 Mar 2005     Ian Dundore
    * Milestone Three: CEM pretty solid aside from save/reload & multithreading
    * Fixed up CEM more; logic now in reloadState.
    * All functions now at least written, if not working.
    * CESM serialization probably OK, but need to verify.
    * CESM deserialization does something, still need to verify.
    * Changed external interface: reloadState returns an ExResult instead
        of a boolean. This change will also be made to saveState tomorrow.
    * Changed base exclusion object: 'Copy' constructor now takes KVC pointer
        instead of CXExclusion reference.
        
0.0.2-r2    28 Mar 2005     Ian Dundore
    * Fixed up CEM a bit to use CESM in a more efficient manner.
    * NEW REQUIREMENT: Threads using Exclusions must remember to
      call CoInitialize(...) and CoUninitialize(...) before/after
      using Exclusions, because CESM requires CC Settings, which
      requires COM.
    * CESM serializes _something_ when asked to save exclusions.
      Not sure if it's real data yet.
    * CESM appears to delete things properly now.
    * Deserializing isn't quite ready yet.
      
0.0.2-r1    25 Mar 2005     Ian Dundore
    * Milestone Two: Exclusion Sets are solid.
    * Completed basic work on CEM.
    * Started work on CESM, but it's hairy. Doesn't work yet.
    * Fixed another leak; getExclusion()/replaceExclusion()
      work properly now.
    * Removed SymInterface bindings for CExclusionFactory. CEF is no longer
      externally-addressable.
    * Added exclusion enumeration support to CEM.
    * Added User Data handling to IExclusion/implementations.
        
0.0.1-r2    24 Mar 2005     Ian Dundore
    * Sets are working out well; I think all their functionality is implemented.
      However, they may need debugging. Couple weird logic errors that I caught
      late today indicates that they need a trip through The Wringer.
    * More improvements on basic exclusion objects. Added accessor for
      exclusionstate (a real necessity for isExcluded tests).
    * Discarded 'instancing' stubs. Idea sounded better on paper.
    * Adding, removing and matching exclusions via the mgr object works...
    * ANSI/OEM->Unicode conversion is in and works
    * Fixed nasty memory leak in CEF's creation functions.    
    
0.0.1-r1    23 Mar 2005     Ian Dundore
    * First milestone complete; basic stuff looks solid.
    * Construction on the Sets is mostly done. May need touchups. Needs testing.
    * Skeleton of CExclusionManager is in!
    * Added GUID generation and assignment to CEF when making new exclusions.
    
0.0.0-r6    22 Mar 2005     Ian Dundore
    * Finished up FS matching and FS-related functions.
    * CFSE/CAE are now fairly complete and solid; 'mostly' tested
    * Started work on the sets (declaration only so far)
    * Fixed CExclusionFactory, made it work nice with ISymBase.

0.0.0-r4    "               "
    * Re-rebuild. Now using cc::IStrings again, this time as
        arguments (reference-to-pointer) when used for output.
    
0.0.0-r3    "               "
    * Rebuild - convert from cc::IStrings to LPWSTRs/LPCWSTRs.
    * Not supported by ccLib::CValueCollection!
    
0.0.0-r2    "               "
    * Rebuild - attempting to use cc::IStrings properly.
       
0.0.0-r1    18 Mar 2005     Ian Dundore
    * First upload to perforce.
    * Anomaly exclusions work. Mostly.
    * Interfaces for exclusion objects (mostly) finalized.
    * Filesystem exclusions b0rked.
    * Factory completely broken.
