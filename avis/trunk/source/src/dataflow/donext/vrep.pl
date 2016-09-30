# vrep.pl -- Binary virus replication engine
use Getopt::Long;
use DataFlow;
use strict;

srand (time() ^ ($$ + ($$ << 15)));

my ($Cookie, $ProcessID, $Parameter, $unc);

GetOptions("Cookie=i" => \$Cookie,
	  "ProcessId=i" => \$ProcessID,
	  "Parameter=s" => \$Parameter,
	  "UNC=s" => \$unc);

print "Cookie: $Cookie, ProcessId: $ProcessID, Parameter: $Parameter, UNC: $unc\n";

sleep int(rand 20) + 1;

#my $outqueue = "Q.V.REPLICATOR.CTRL";

my @decision = ("MacroReplicationController", "MacroReplicationController", "MacroReplicationController", "MacroReplicationController", "MacroReplicationController", 
                "MacroReplicationController", "MacroReplicationController", "MacroReplicationController");

my $outqueue = $decision[int(rand 8)];


SpecifyStopping($Cookie, $ProcessID, $unc, $outqueue, "1 of 1");

my $DoNextChoice = int (rand 10);

if ($DoNextChoice != 9)
{
   DoNext ($Cookie, $ProcessID, $unc, $outqueue, $Parameter);
   WriteSignatureForDataFlow ($ProcessID, $unc);
}

1;
