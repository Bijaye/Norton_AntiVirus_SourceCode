use ExtUtils::MakeMaker;
# See lib/ExtUtils/MakeMaker.pm for details of how to influence
# the contents of the Makefile that is written.
WriteMakefile(
    'NAME'	=> 'Mcv',
    'VERSION_FROM' => 'Mcv.pm', # finds $VERSION
    'LIBS'	=> [],   # e.g., '-lm' 
    'DEFINE'	=> '-DWS_32 -DT_WINNT -DMCV',     # e.g., '-DHAVE_SOMETHING' 
    'INC'	=> '',     # e.g., '-I/usr/include/other' 
    'OBJECT'	=> 'll.o DocProp.o mcv.o',
    #to link with dfvcnt and macvcnt
    'LDLOADLIBS' => 'msvcrt.lib oldnames.lib kernel32.lib comdlg32.lib winspool.lib gdi32.lib advapi32.lib user32.lib shell32.lib netapi32.lib ole32.lib oleaut32.lib uuid.lib wsock32.lib mpr.lib winmm.lib version.lib  odbc32.lib odbccp32.lib dfvcnt.lib macvcnt.lib'
);
