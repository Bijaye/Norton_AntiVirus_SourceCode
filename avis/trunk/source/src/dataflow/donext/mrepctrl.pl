# mrepctrl.pl -- Macro virus replication controller
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

#my @decision = ("Q.M.REPLICATOR", "Q.M.ANALYSIS", "Q.DEFER");
my @decision = ("MacroReplicationEngine", "MacroReplicationEngine", "MacroReplicationEngine", "MacroAnalysis",
                "MacroReplicationEngine", "MacroReplicationEngine", "MacroAnalysis",
                "MacroAnalysis", "MacroAnalysis", "MacroAnalysis", "End");

my $outqueue = $decision[int(rand 11)];

my $randomCount = int (rand 10) + 1;

if ($ProcessID == 2)
{
    if ($outqueue eq "MacroAnalysis")
    {
        $outqueue = "MacroReplicationEngine";
    }
}

if ($outqueue ne "MacroReplicationEngine")
{
    $randomCount = 1;
}

SpecifyStopping($Cookie, $ProcessID, $unc, $outqueue, "$randomCount of $randomCount");

my $DoNextChoice = int (rand 10);

if ($DoNextChoice != 9)
{
    my $i = 0;
    for ($i = 0; $i < $randomCount; $i++)
    {
        DoNext ($Cookie, $ProcessID, $unc, $outqueue, $Parameter);
    }

   WriteSignatureForDataFlow ($ProcessID, $unc);
}

1;
