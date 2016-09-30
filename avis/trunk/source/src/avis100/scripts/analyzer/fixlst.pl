use lib ('//Shine/TestZoneShine/perl/lib/site'); #!!testing only

use Cwd;
use Getopt::Long;
use DataFlow;
use File::Path;
use File::Copy;
use AVIS::Vides;
use AVIS::FSTools;
use AVIS::Dirstack;
use AVIS::Local;
use AVIS::Logger;
use English;
use strict;


fixlst (@ARGV);


sub fixlst ($$$) {
   my ($UNC, $Cookie,$isdnVGoats) = @_;
   my @SampleList = ();

   foreach my $replicant (ReadSamplesFileMkII($UNC,$Cookie,-1)) {
      my ($boxPath, $infected, $badGoat, $badFlags) = ($$replicant[$iBoxPath],$$replicant[$iPath],$$replicant[$iGoatName],$$replicant[$iAttributes]);

      (my $topGoat = $badGoat) =~ s</.+><>;
      (my $goat = "$topGoat/$boxPath") =~ s<:><>;
      $goat =~ tr<\\></>;

      (my $infectedPath = "$UNC/$infected") =~ tr</><\\>;
      (my $goatPath = "$isdnVGoats/$goat") =~ tr</><\\>;
      my $len = max ((-s $infectedPath) - (-s $goatPath), 0);
      my @cmp = `fc "$infectedPath" "$goatPath"`;

      print ("$infectedPath not found\n"),    next unless -s $infectedPath;
      print ("$goatPath not found\n"), next unless -s $goatPath;
      print ("$infectedPath not infected (length change $len from $goatPath)\n"),next unless ($len > 8 or scalar @cmp > 16);

      (my $flags = $badFlags) =~ s<-?\d+-><>;

      my @sample;
      $sample[$iBoxPath]    = $boxPath;
      $sample[$iPath]       = $infected;
      $sample[$iGoatName]   = $goat;
      $sample[$iAttributes] = "$len-$flags";
      push @SampleList,\@sample;
      print "$len \t$infected \t$goat\n";
   }
   unlink "$UNC/samples.lst";
   WriteSamplesFileMkII($UNC,$Cookie,-1,\@SampleList);
}
   


sub max {return $_[0] > $_[1] ? $_[0] : $[1];}
