#!perl -w
####################################################################
#                                                                  #
# Program name:  DataFlow.pm                                       #
#                                                                  #
# Description:   Module with interface and helper functions        #
#                for the Dataflow scaffolding                      #
#                                                                  #
# Statement:     Licensed Materials - Property of IBM              #
#                (c) Copyright IBM Corp. 1998-1999                 #
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
####################################################################
package DataFlow;
require Exporter;
use vars qw(@ISA @EXPORT $VERSION);
use MQSeries;
use File::CounterFile;

#use strict 'vars';
$VERSION = "0.1";
@ISA = qw(Exporter);
@EXPORT = qw(UnpackPara PackPara Para2Opts DoNext SpecifyStopping Execute);

my $QMgr = ""; # Queue manager = default
my $verbose = 1;

#######################################################################
# UnpackPara -- unpack the parameter string into a hash.
sub UnpackPara ($) {
  my $msg = shift or die "One parameter required";
  ($msg =~ m/^(\w+):\s(.+)/mg);
}

#######################################################################
# PackPara -- pack hash into a parameter string. This is a string of
# lines, where each line is of the format: "key: value\n".
sub PackPara (@) {
  my %array = @_;
  my $myoutpar = "";
  foreach (keys %array) {
    $myoutpar .= "$_: $array{$_}\n";
  }
  $myoutpar;
}

#######################################################################
# Para2Opts -- pack parameter hash into a POSIX argument string. 
# E.g. "--Cookie 1234 --ProcessID 12"
sub Para2Opts (@) {
  my %array = @_;
  my $myoutpar = "";
  foreach (keys %array) {
    $myoutpar .= "--$_ $array{$_} ";
  }
  $myoutpar;
}

#######################################################################
# SpecifyStopping -- specify which stopping condition to use
# Parameters:
#   $Cookie:    The issue's cookie number. The called routine should
#               just pass this through to the next DoNext.
#   $ProcessID: The process identifier with in this issue. This value
#               gets incremented by this routine. The called routine
#               should just pass this through to the next DoNext().
#   $unc:       This is the persistence directory for this issue. It
#               shouldn't change once it has been set. However, the
#               called routine should make no assumptions about this.
#   $outqueue:  This is the name of the service the requestor will
#               be sending commands.
#   $condition: This tells the dataflow system what stopping conditions
#               to apply to the next batch of DoNext's
#
# This function is not implemented in the current scaffolding!
#
sub SpecifyStopping ($$$$$) {
  my ($Cookie, $ProcessID, $unc, $outqueue, $condition) = @_;

}

#######################################################################
# DoNext -- specify which service to do after the current one.
# Parameters:
#   $Cookie:    The issue's cookie number. The called routine should
#               just pass this through to the next DoNext.
#   $ProcessID: The process identifier with in this issue. This value
#               gets incremented by this routine. The called routine
#               should just pass this through to the next DoNext().
#   $unc:       This is the persistence directory for this issue. It
#               shouldn't change once it has been set. However, the
#               called routine should make no assumptions about this.
#   $outqueue:  This is the name of the service to go to next.
#   $ExtraParameter: This is a pass-through from the caller to the
#               callee and is arbitrary.
#
sub DoNext ($$$$$) {
  my ($Cookie, $ProcessID, $unc, $outqueue, $ExtraParameter) = @_;
  my ($Hconn, $CompCode, $Reason, $Hobj, $Options, 
      $tempMsg, %pmo, %ObjDesc, %Parameter, %msgDesc, $rc);

  print "DoNext($Cookie, $ProcessID, $unc, $outqueue, $ExtraParameter)\n" 
    if $verbose;

  $c = new File::CounterFile "$unc/ProcessID.cnt", "0";
  $pid = $c->inc;

  # connect to server ###########################

  $CompCode = 0;
  $Reason = 0;

  print "Connecting to the queue manager '$QMgr'\n";
  $Hconn = MQCONN($QMgr, $CompCode, $Reason);
  print "$Hconn = MQCONN ('$QMgr', $CompCode, $Reason)\n" if ($verbose);

  if ($CompCode == MQCC_FAILED) {
    warn "Failed to connect to queue manager '$QMgr'";
    return 0;
  }

  # use PUT1 to put message to queue ###########

  $Parameter{Cookie} = $Cookie;
  $Parameter{ProcessID} = $pid;
  $Parameter{Parameter} = $ExtraParameter;
  $Parameter{unc} = $unc;
  $rc = 1;
  do  {
    $Options = MQOO_OUTPUT | MQOO_SET;
    %ObjDesc = (
		ObjectType => MQOT_Q,
		ObjectName => $outqueue,
		ObjectQMgrName => ""
	       );

    $tempMsg = PackPara(%Parameter);
    $CompCode = 0;
    $Reason = 0;
    MQPUT1($Hconn,\%ObjDesc,\%msgDesc,\%pmo,$tempMsg,$CompCode,$Reason);
    if ($CompCode == MQCC_FAILED) {
	if ($Reason == MQRC_Q_FULL) {
	    warn "Error: The queue was full\n";
	} elsif ($Reason == MQRC_MSG_TOO_BIG_FOR_Q) {
	    # this should never happen
	    MQDISC($Hconn, $CompCode, $Reason);
	    die "Error: The message was too big\n";
	} elsif ($Reason == MQRC_UNKNOWN_OBJECT_NAME) {
	    warn "Unknown object name";
	    $rc = 0;
	    last;
	} else {
	    print "Error: Completion code: $CompCode, Reason code $Reason\n";
	    $rc = 0;
	    last;
	} 
    }
  } while ($CompCode == MQCC_FAILED);

  # disconnect from queue #######################
  $CompCode = 0;
  $Reason = 0;

  print "Calling MQDISC...\n" if ($verbose);
  MQDISC($Hconn,$CompCode,$Reason);
  $rc;
}

1;



















