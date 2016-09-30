####################################################################
#                                                                  #
# Program name:  banalysis.pl                                      #
#                                                                  #
# Description:   Boot virus replication and analysis               #
#                                                                  #
# Statement:     Licensed Materials - Property of IBM              #
#                (c) 1998-1999 IBM Corp.                           #
#                                                                  #
# Author:        Morton Swimmer, Gleb Esman, Stefan Tode           #
#                                                                  #
#                U.S. Government Users Restricted Rights - use,    #
#                duplication or disclosure restricted by GSA ADP   #
#                Schedule Contract with IBM Corp.                  #
#                                                                  #
####################################################################
#                                                                  #
# Function:                                                        #
#                                                                  #
####################################################################

use Getopt::Long;
use DataFlow;
use File::Path;
use AVIS::Local;
use AVIS::FSTools;
use AVIS::Logger;

GetOptions("Cookie=s" => \$Cookie,
	  "ProcessId=i" => \$ProcessID,
	  "Parameter=s" => \$Parameter,
	  "UNC=s" => \$unc);

# Our directory to reposite data to will be 
# $unc/$isdnRun/$Parameter which we must create

if (!(-e "$unc")) {
  die "Ack, the unc does not exist!!!";
}


###########################################################################

# Do useful things here

###########################################################################


# For want of a better to queue to send our stuff to...
$outqueue = $isqnReturn; 

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
WriteSignatureForDataFlow($ProcessID, $unc);

1;
