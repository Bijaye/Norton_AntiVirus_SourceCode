   This directory contains the source files needed for the
   RTVScan Linux port.

   Files whose names start with a lower case prefix, like
   `ndk', contain functions that are exported for general
   use.  Their header files are found in the `include'
   directory.

   Note that the `include' directory contains a number of
   header files that need to override the corresponding header
   files in other parts of the include path in the
   Makefiles.  Therefore, it must be placed first in this path.

   Files with a capitalized name contain functions that are
   for internal use only. The corresponding header files are
   found in the same directory as the source file and are not
   intended for general use.
