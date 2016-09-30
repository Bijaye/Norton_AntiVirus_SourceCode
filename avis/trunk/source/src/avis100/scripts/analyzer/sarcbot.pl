####################################################################
#                                                                  #
# Program name:  sarcbot.pl                                        #
#                                                                  #
# Description:   simulation queue for the definition compiler.     #
#                                                                  #
#                                                                  #
# Statement:     Licensed Materials - Property of IBM              #
#                                                                  #
# Author:        Morton Swimmer                                    #
#                                                                  #
#                U.S. Government Users Restricted Rights - use,    #
#                duplication or disclosure restricted by GSA ADP   #
#                Schedule Contract with IBM Corp.                  #
#                                                                  #
#                                                                  #
####################################################################
#                                                                  #
# Function:                                                        #
#                                                                  #
#                                                                  #
#                                                                  #
####################################################################

use Getopt::Long;
use DataFlow;

srand (time() ^ ($$ + ($$ << 15)));

GetOptions("Cookie=i" => \$Cookie,
	  "ProcessId=i" => \$ProcessID,
	  "Parameter=s" => \$Parameter,
	  "UNC=s" => \$unc);

print "Cookie: $Cookie, ProcessId: $ProcessId, Parameter: $Parameter, UNC: $unc\n";

sleep int(rand 20) + 1;

# 'decide' where we want to go today (tm)
$outqueue = ((int(rand 2) == 0 ) ? "Q.TEST" : "Q.DEFER");

# Specify what stopping condition to use for multiple DoNext calls. In
# this case, we do not actual do more than one thing next, so we
# specify a one out of one stopping condition. (In fact, nothing else
# is supported in the scaffolding anyway.) $Cookie, $ProcessID and
# $unc MUST be passed through unmodified.
SpecifyStopping($Cookie, $ProcessID, $unc, $outqueue, "1 of 1");

# Tell the dataflow what to do next, ie. which queue to send stuff
# to. We are free to set the $Parameter, but $Cookie, $ProcessID and
# $unc MUST be passed through (although we can use the values.)
DoNext ($Cookie, $ProcessID, $unc, $outqueue, $Parameter);

1;






