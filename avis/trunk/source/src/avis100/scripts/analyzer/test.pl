
use Getopt::Long;
use DataFlow;

srand (time() ^ ($$ + ($$ << 15)));

GetOptions("Cookie=i" => \$Cookie,
	  "ProcessId=i" => \$ProcessID,
	  "Parameter=s" => \$Parameter,
	  "UNC=s" => \$unc);

print "Cookie: $Cookie, ProcessId: $ProcessId, Parameter: $Parameter, UNC: $unc\n";

sleep int(rand 20) + 1;

$outqueue = ((int(rand 2) == 0 ) ? $isqnReturn : $isqnDefer);

DoNext ($Cookie, $ProcessID, $unc, $outqueue, $Parameter);
WriteSignatureForDataFlow ($ProcessID, $unc);


1;
