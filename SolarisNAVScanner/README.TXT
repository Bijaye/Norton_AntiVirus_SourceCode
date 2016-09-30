OVERVIEW
--------

This file describes how the Unix (Solaris) port of the NAV scanning engine 
is organized.

The final product consists of the following files:

  File           Description
  -------------- ------------------------------------------------------------
  libnavapi.a    NAV API static library
  libnavex15.so  NAVEX 1.5 dynamic library (part of virus definitions) 
  libnaveng.so   NAVENG 1.5 dynamic library (part of virus definitions) 

These are built from several intermediate libraries.  A few small test 
programs are also build from the same set of source code files.

These files must be combined with the non-executable "virscan" virus 
definition files before the test programs will work.  The virscan
files must be "in sync" with the executable virus definition files
or problems will result.


DIRECTORIES
-----------

In the Santa Monica Build System, the scanning engine is assembled from
multiple projects in the NEWVIR, AVCORE, and AVCOMP environments.  
The Build System doesn't exist for Unix, and is really unnecessary.
All that is needed is to copy the source code from the Build System 
projects into a Unix directory tree.  Simple shell scripts can then 
be used to invoke the 'make' command on the Unix makefile in each 
directory.

The Unix project directories and their corresponding Build System ENV/PROJ 
project names are:

  Directory  ENV     PROJ     Purpose (What it Makes)
  ---------- ------- -------- -----------------------------------------------
  avenge     avcore  avenge   libaveng.a
  cmdline    newvir  cmdline  cmdline
  crc32      newvir  crc32    libcrc32.a
  df         newvir  df       libdf.a
* emulator   avcore  emulator libpam.a
  engine15   newvir  engine15 libnaveng.so, engtest
  heur       newvir  heur     libheur.a
  inc.avcore avcore  include  (nothing)
  inc.newvir newvir  include  (nothing)
  inc.core   core    include  (nothing)
  macroheu   newvir  macroheu libmacroheu.lib
  navapi     avcore  navapi   libnavapi.a, apitest
$ navex15    newvir  navex15  libnavex15.so
  olessapi   newvir  olessapi libolessapi.a, libwd7api.a, libo97api.a, 
                              libx15api.a, libacsapi.a, libppapi.a 
  symkunix   core    symkrnl  libsymkunix.a

 * Note: The emulator project is made obsolete by the navapi project,
   which has its own copy of the PAM source code.  However, it was ported
   to Solaris anyway.

 $ Note: In the Build System, the source code for navex15 is actually
   copied from the newvir/navex project.

Each project directory should contain ONLY the following:
  - Source code files from the Santa Monica version control system.
  - A unix.mak file (to be checked into the version control system later).
  
The unix.mak files are standardized.  They each include a file named
unixmake.inc with common macros, build rules, and build targets. 
The standard build targets are named debug, release, and clean.
The default target is debug.

Output files are stored in the project directories.  However, the makefiles 
also automatically copy the final output files to the following directories:

  Directory  Purpose
  ---------- ----------------------------------------------------------------
  bin.sd     Receives Solaris debug output files
  bin.sr     Receives Solaris release output files

The appropriate bin directory is put in the library path during make
operations, so projects can access intermediate libraries created by other 
projects.

In addition to the project and bin directories, the following directories
are used for development and testing:

  Directory  Purpose
  ---------- ----------------------------------------------------------------
  defs       Virus definition files; must correspond to current source code.
  dumbvirs   Read-only copies of some sample dummy viruses.
  test       Testing area; uses temp. copies of the defs and dumbvirs files.
  diffs      Perforce diffs between ported code and original 28Jan1999 code.

The parent directory contains all of the directories listed above,
plus the following files:

  File          Purpose
  ------------- -------------------------------------------------------------
  README.TXT    This file
  MakeAll       Script to make all projects
  CleanAll      Script to clean all projects
  mk            Script to invoke make on unix.mak and save output
  tolower       Script to rename all files in current directory to lowercase
  unixmak.inc   Include file for unix.mak files
  gcc-man.txt   man entry for gcc
  gdb-man.txt   man entry for gdb
  gdb-menu.txt  help menus from gdb


PROJECT HEIRARCHY
-----------------

This section is a brief description of the project dependencies.

The NAVAPI library has its own copy of the PAM (EMULATOR) source files, 
and also links to the AVENGE library.  NAVAPI is the only project that uses
Symantec Core Library functions, which are in the SYMKUNIX library.

NAVAPI also dynamically loads the NAVEX15 and NAVENG shared libraries.

NAVEX15 has its own scanning code, and also links to the MACROHEU scanning
library and the six macro virus scanning libraries from the OLESSAPI project.
In addition, it links to the CRC32 and DF helper libraries.

NAVENG links to the HEUR library.


TO DO LIST
----------

The work of porting the virus scanner to Solaris is not quite complete.
Here are some tasks still to be done:

  - Check on infinite loop in navapi PAM code for certain dumb viruses
    (see below).
  - Check on Unix repair of DIR2 in avapi.cpp, currently DISABLED!
  - Check on use of navex.inf file in navex15/nvx15.cpp.
  - Check on loading of heuristic settings in general.
  - Check on range warnings in navex15.
  - Determine if hard-coded data file names should be upper or
    lower case.  Currently they are lowercase for Unix.  
    Is it better to have them uppercase (for LiveUpdate, etc.)?
  - Check on NAVEX15 MVP code (mvp.cpp) for endian issues, 
    path separator characters, etc.
  - Port a multi-threaded NAVAPI test application to Unix.

Dumb Virus testing results:

Passed:
  cascade.com
  da1800.com
  dir2.com
  eicar.com
  frere.exe
  gergana.exe
  hydra.com
  modhydra.com
  world707.com

Failed:
  bhm95rep.dot
  bhm95ur.dot
  bhm97rep.doc
  bhm97ur.doc
  diract.com
  dsce2100.com
  dscedemo.com
  laru95-1.xls
  laru95-2.xls
  mvp.dot
  npad95-1.dot
  npad95-2.dot
  npad97.dot


USING THE TEST PROGRAMS
-----------------------

Here are some quick hints on how to run the test apps:

  heurtest dumbvirs/diract.com 1
  cmdline -SUB dumbvirs
  apitest dumbvirs/diract.com
   

INCLUDE FILES
-------------

The Build System has many header files checked into the NEWVIR, AVCORE,
and CORE include directories.  However, only a small subset is needed
to build the scanning engine.  The required files are listed below:

inc.newvir:

  acsapi.h     callback.h   exclude.h    heurpage.h   o97str.h     search.h
  aolpw.h      callbk15.h   gsm.h        heurtok.h    olessapi.h   sigex.h
  avcb.h       callfake.h   hcover.h     hproto.h     olestrnm.h   ssbstype.h
  avdatfil.h   cover.h      heur2.h      msoapi.h     opcode.h     wbutil.h
  avendian.h   cpu.h        heurapi.h    n30type.h    page.h       wd7api.h
  avenge.h     crc32.h      heurcfg.h    navex.h      pamapi.h     wd7env.h
  avobject.h   ctsn.h       heurcpu.h    navex15.h    pamcache.h   wdop.h
  avtypes.h    datafile.h   heurds.h     navheur.h    ppapi.h      xl5api.h
  banalyze.h   df.h         heurid.h     o97api.h     prepair.h    xlrec.h
  behav.h      dfzip.h      heurint.h    o97env.h     profile.h

inc.avcore:

  avapi.h      avobject.h   ctsn.h       navcb.h      prepair.h    sigex.h
  avcb.h       avtypes.h    datafile.h   opcode.h     profile.h    symsync.h
  avdatfil.h   callback.h   exclude.h    page.h       proto.h      virusinf.h
  avendian.h   cover.h      ident.h      pamapi.h     repair.h
  avenge.h     cpu.h        n30type.h    pamcache.h   search.h

inc.core:

  dbcs.h       file.h       platform.h   xapi.h

In the Build System, AVCORE projects have the AVCORE and CORE include
directories on their include path, and the NEWVIR projects have the
NEWVIR and CORE include directories on their include path.  This is
the reason some files are duplicated between inc.newvir and inc.avcore.
Duplicate file names should have identical contents, but this has not
been strictly enforced.  In Unix, all project directories put both  
inc.newvir and inc.avcore in their include path, with inc.newvir taking
precedence.


VERSION CONTROL STATUS
----------------------

The porting source code is derived from code taken from the Santa Monica 
Perforce server on 28Jan1999.

 ENV     Perforce Location
 ------- -----------------------------------
 avcore  //depot/AVCore/Trunk/SOURCE/
 newvir  //depot/SARC/VIRLIB/SOURCE/
 core    //depot/CORE/TRUNCK/SOURCE/

It was originally intended that this ported code be checked back into
Perforce in the original locations.  This certainly would have made 
perfect sense, especially since there was an opportunity to do so before
any other code changes were checked in.  Sadly, this has not been allowed,
so instead the ported code has been checked into a new Perforce location.
Someone else will get the job of merging all Unix changes from the 
finalized ported code back into the original locations at a later time.
