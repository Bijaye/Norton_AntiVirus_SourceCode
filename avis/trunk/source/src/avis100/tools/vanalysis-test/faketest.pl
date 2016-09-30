################################################################################
# 4 args (lConst, lVar, encr, seed) results in a single pass with those values;
# a single arg is used to seed the random number generator prior to entering an
# infinite loop in which the run parameters are generated randomly.
################################################################################

my @encr = ('p','x','a','A','X');
my $errs = 0;
$ENV{'VANALYSIS_LOG'} = 'console' unless defined $ENV{'VANALYSIS_LOG'};
my $UNC = '//Elbe/ElbeD/'.$ENV{'COMPUTERNAME'}.'_UNC';


my ($lC,$lV,$e,$r) = @ARGV;
`echo === faketest $lC,$lV,$e,$r ============================= >>D:faketest.log`;

my $loop = !defined($r);
srand($lC) if $loop;

for(;;) {
   system 'pause' if -f $ENV{'TEMP'}.'/faketest.pause';

   if ($loop) {($lC,$lV,$e,$r) = (Irand(2000)+32,Irand(500)+32,$encr[Irand($#encr)],Irand(64000));}

   print "Trying $lC, $lV, $e, $r\n";
   system "perl -S -w setupfakevir.pl \\\\Elbe\\ElbeCorpus\\samples\\fake $lC $lV $e $r";

   $RC = system "perl -S -w setuptest.pl \\\\Elbe\\ElbeCorpus\\samples fake goats $UNC";
   die "RC = $RC\n" if $RC;

   $RC = system 'perl -S -w vana.pl --Parameter "vanalysis_fake.pl" --Cookie FAKE --ProcessID 0 --UNC //Elbe/ElbeD/'.$ENV{'COMPUTERNAME'}.'_UNC';

  `echo $RC\t$lC\t$lV\t$e\t$r >>D:faketest.log`;
   $errs++ if $RC;
   print "=====\n$errs\n=====\n";

   last unless $loop;
}


sub Irand {
   my ($n) = @_;
   return int(rand($n+1));
}
