use ExtUtils::MakeMaker;
# See lib/ExtUtils/MakeMaker.pm for details of how to influence
# the contents of the Makefile that is written.
WriteMakefile(
    'NAME'	=> 'EntTrac',
    'VERSION_FROM' => 'EntTrac.pm', # finds $VERSION
#    'LIBS'	=> [''],   # e.g., '-lm' 
#    'DEFINE'	=> '',     # e.g., '-DHAVE_SOMETHING' 
#    'INC'	=> '',     # e.g., '-I/usr/include/other' 
    'MYEXTLIB'  => 'algutil/algutil$(LIB_EXT)', # opcode parsing routines
);

sub MY::postamble {
'
$(MYEXTLIB): algutil/Makefile
	cd algutil && $(MAKE) $(PASTHRU)
';
}

