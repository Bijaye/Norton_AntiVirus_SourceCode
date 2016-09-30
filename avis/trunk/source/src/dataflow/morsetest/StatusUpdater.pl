# StatusUpdater.pl -- 
use Getopt::Long;
use DataFlow;
use strict;

srand (time() ^ ($$ + ($$ << 15)));

my ($Cookie, $ProcessID, $Parameter, $unc);

GetOptions("Cookie=i" => \$Cookie,
	  "ProcessId=i" => \$ProcessID,
	  "Parameter=s" => \$Parameter,
	  "UNC=s" => \$unc);

print STDERR "Cookie: $Cookie, ProcessId: $ProcessID, Parameter: $Parameter, UNC: $unc\n";

sleep int(rand 20) + 1;

1;
