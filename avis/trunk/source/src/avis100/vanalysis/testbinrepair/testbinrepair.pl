use lib ('//Shine/TestZoneShine/perl/lib/site'); #!!testing only

use AVIS::TestBinRepair;
use AVIS::PathEnvTools;


my ($repaired, $original) = @ARGV;
addScriptDirToPath();

my $status = isRepairOf($repaired, $original);
print "$repaired -> $original: ".($status ? $status : 'no')." repair\n";

exit (1)  if $status eq "inexact"; 
exit (0)  if $status eq "exact"; 
exit(-1);
