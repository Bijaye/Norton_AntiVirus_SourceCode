
use ExtUtils::MakeMaker;
$Verbose = 1;
WriteMakefile(
	      NAME      => 'EntTrac::algutil',
	      SKIP      => [qw(all static )],
	      clean     => {'FILES' => 'algutil$(LIB_EXT)'},
	     );

sub MY::top_targets {
'
all :: static

static ::       algutil$(LIB_EXT)

algutil$(LIB_EXT): $(O_FILES)
	$(AR) /OUT:algutil$(LIB_EXT) $(O_FILES)
';
}

