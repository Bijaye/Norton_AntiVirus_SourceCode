use File::Path;
use File::Copy;
use AVIS::Vides;
use AVIS::FSTools;

sub Copy ($$);

die "src-root virus-dir goat-dir UNC\n" unless scalar @ARGV == 4;
my ($srcRoot,$virus,$goatdir,$UNC) = @ARGV;
$virus ="$srcRoot/$virus";
my $Cookie = $ENV{'COMPUTERNAME'};

(my $dosUNC = $UNC) =~ tr[/][\\];
while (-d $dosUNC) {
   print "removing $dosUNC...\n";
   print `rd /s /q $dosUNC`;
}

die "no virus-dir\n" unless -d $virus && -d "$virus/infected";
die "no goat-root\n" unless -d $srcRoot;
die "no goat-dir\n" unless -d "$srcRoot/$goatdir";

`md $dosUNC\\sample`;
Wipe glob ("$UNC/sample/*");
Copy ("$virus/sample.???", "$UNC/sample");
map {print "original:\t $_\n"}  glob("$UNC/sample/sample.???");
die "no original sample\n" unless glob("$UNC/sample/sample.???");

mkpath "$UNC/infected";
Wipe glob ("$UNC/infected/*");
Copy ("$virus/infected/*", "$UNC/infected");
map {print "infected:\t $_\n"}  glob("$UNC/infected/*");
die "no replicants\n" unless glob("$UNC/infected/*");

mkpath "$UNC/run";
Wipe glob ("$UNC/run/*");
# Copy ("$virus/vides.results", "$UNC/run");
# die "no vides.results\n" unless glob("$UNC/run/vides.results");

#if (-s "$virus/samples.lst") {
#   Copy ("$virus/samples.lst", $UNC);
#}else { 
   ###open LST, ">$UNC/samples.lst" or die "unable to open list file $UNC/samples.lst\n";
   my @SampleList;
   foreach (glob "$UNC/infected/*") {
      s[^$UNC/][];
      ($fname = $_) =~ s[.*/][];
      die "goat $srcRoot/$goatdir/$fname not found\n" unless -s "$srcRoot/$goatdir/$fname";
      my $length = (-s "$UNC/$_") - (-s "$srcRoot/$goatdir/$fname");
      $length = 0 if $length < 0;
      my $target = 'none';
      $target = 'COM' if $fname =~ /.*\.COM$/i;
      $target = 'EXE' if $fname =~ /.*\.EXE$/i;
      ###print LST "$_,A:/$fname,$goatdir/$fname,,$length-none-none-TBD-$target,,,\n";
      my @sample;
      $sample[$iBoxPath] = '';
      $sample[$iPath] = $_;
      $sample[$iGoatName] = "$goatdir/$fname";
      $sample[$iAttributes] = "$length-none-none-TBD-$target";
      push @SampleList,\@sample;
   }
   WriteSamplesFileMkII($UNC,$Cookie,-1,\@SampleList);
   ###close LST;
#}
die "no $UNC/samples.lst file\n" unless -s "$UNC/samples.lst";


sub Copy ($$) { return map {copy $_, $_[1]} glob $_[0];}

