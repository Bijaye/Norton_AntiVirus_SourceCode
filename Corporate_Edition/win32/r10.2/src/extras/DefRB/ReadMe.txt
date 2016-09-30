// The problem:
//  When the user tries to rollback defs by placing older defs in the 
//  Hawking INCOMING directory, the NAVC client UI shows that defs have
//  rolled back. However, the old engine files are still in use. The defs 
//  do not get unloaded even though NAVC is registered with Hawking with
//  the rollback set. Hawking may even attempt to delete the original defs
//  ( two files will be left behind because they are in use ). Bottom line,
//  Definition rollback through INCOMING directory does not work. At best,
//  rollback fails but we report success. At worst, the user is unprotected.
//
// The (hack) solution:
//  This console application is designed to force definition reload on 
//  NAVC ver 7.xx clients. Runs on WinNT or Win2k. Win9x is not supported
//  because it seems that the registry watch on the ProductControl key
//  is not working. This tool relies on the behavior of rtvscan that when
//  HeuristicLevel is changed ,the engine must be reloaded. Unfortunately,
//  when reload is forced, rtvscan only looks for .vdb files, it ignores the
//  shared ( Hawking ) defs. Our method here is to change HeuristicLevel, set
//  the NewPatternFile reg key ( which is watched ) forcing a def reload. 
//  (Actually this only forces the def unload. If no .vdb files are available,
//  no defs will be loaded. AP will be disabled. Running a manual scan will
//  reload the defs.) We repeat the process to put the HeuristicLevel back. Defs
//  are probably still unloaded. We set the NewPatternFile value a third time,
//  this time defs should be loaded. We are done.

// Usage: DefRLoad.exe [/silent]

/////////////////////////////////////////////////////////////////////////////
