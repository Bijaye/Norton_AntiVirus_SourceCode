# classifier.pl -- Virus classifier and last filter scans
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

#    my $fileName = "c:\\dataflow\\classifier.log";
#    #Check if the file already exists.
#    unless (-e $fileName)
#    {
#        #Create this file
#        open (FILEHANDLE, ">$fileName");
#    }
#    else
#    {
#        #File exists. Open it for append mode.
#        open (FILEHANDLE, ">>$fileName");
#    }

#    #write the required data to the file.
#    print FILEHANDLE ("Cookie: $Cookie, ProcessId: $ProcessID, Parameter: $Parameter, UNC: $unc\n");
#    close FILEHANDLE;

sleep int(rand 20) + 1;

# 'decide' where we want to go today (tm)

#my @decision = ("Q.V.REPLICATION.CTRL", "Q.M.REPLICATION.CTRL");
my @decision = ("MacroController", "BinaryController");

my $outqueue = $decision[int(rand 2)];

# Specify what stopping condition to use for multiple DoNext calls. In
# this case, we do not actual do more than one thing next, so we
# specify a one out of one stopping condition. (In fact, nothing else
# is supported in the scaffolding anyway.) $Cookie, $ProcessID and
# $unc MUST be passed through unmodified.
SpecifyStopping($Cookie, $ProcessID, $unc, $outqueue, "1 of 1");

# Tell the dataflow what to do next, ie. which queue to send stuff
# to. We are free to set the $Parameter, but $Cookie, $ProcessID and
# $unc MUST be passed through (although we can use the values.)

my $DoNextChoice = int (rand 40);

if ($DoNextChoice != 9)
{
   DoNext ($Cookie, $ProcessID, $unc, $outqueue, $Parameter);
   WriteSignatureForDataFlow ($ProcessID, $unc);
}

1;
