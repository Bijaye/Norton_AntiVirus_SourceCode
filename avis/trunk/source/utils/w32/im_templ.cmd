=== Delete the following information lines after modifying
===
=== 1. Copy IM_TEMPL.CMD to \my_subproject\m.cmd
=== 2. replace 'THIS_PROJECT_NAME' with the lowercased name of subproject.
=== 3. replace 'THIS_LIBRARY_NAME' with the name of output .LIB file (if DLL subproject)
===    'THIS_LIBRARY_NAME' may be different from DLL name.
===    Leave 'THIS_LIBRARY_NAME' empty (SET THIS_LIBRARY_NAME=) for non-DLL subprojects.
=== 4. 'UTILD' variable must exists and points to the place where the following files are:
===        IUNIMAKE.CMD
===        IBM_STUB.MAK
===        TWEAKMAK.EXE

: Syntax:
:     m depend  [..]
:     m compile [..]
:     m help    [..]
:     m library [..]
:     m exedll  [..]
:     m all     [..]
:     m clean   [..]
:
:     m [-a] debug/release

@SET   THIS_PROJECT_NAME=testdll
@SET   THIS_LIBRARY_NAME=test_dll

@call %UTILD%\iunimake.cmd %1 %2 %3 %4 %5 %6 %7 %8 %9

@SET   THIS_PROJECT_NAME=
@SET   THIS_LIBRARY_NAME=
