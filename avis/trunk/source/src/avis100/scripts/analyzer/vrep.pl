####################################################################
#                                                                  #
# Program name:  vrep.pl                                           #
#                                                                  #
# Description:   simulation queue server for binary virus          #
#                replication.                                      #
#                                                                  #
# Statement:     Licensed Materials - Property of IBM              #
#                (c) 1998-1999 IBM Corp.                           #
#                                                                  #
# Author:        Morton Swimmer, Stefan Tode                       #
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
use AVIS::Vides;
use AVIS::ParamStr;
use AVIS::Logger;
use Data::Dumper;
#use AVIS::Crumbs;

my $vrep_done = 0;             # Must be set to 1 before normal exit; see END
my %isvarStatus = ();  # status for vrepctrl
#my $report_only = 1;
my $report_only = 0;
my $vdebug = 1;  # to show some informations during the run
my $verbosepath = 0;
GetOptions("Cookie=s" => \$Cookie,
	  "ProcessId=i" => \$ProcessID,
	  "Parameter=s" => \$Parameter,
	  "UNC=s" => \$unc);
my $vreplogger = new AVIS::Logger("");

# Our directory to reposite data to will be
# $unc/$isdnRun/$Parameter which we must create

if (!(-e "$unc")) {
  #  log some error message, if it doesn't exist
  vrep_error("Error $unc doesn't exist, $!","FATAL");
}
print "\$Goats: $isdnVGoats\n";
print "Cleaning directory $unc/$isdnRun/$Parameter \n";

File::Path::rmtree("$unc/$isdnRun/$Parameter",$verbosepath,1);
if ($verbosepath) {
  print "trying to mkdir $unc/$isdnRun/$Parameter\n";
  print "trying to mkdir $unc/$isdnRun/$Parameter/$isdnInfected\n";
  print "trying to mkdir $unc/$isdnRun/$Parameter/$isdnSectors\n";
}

mkpath (["$unc/$isdnRun/$Parameter",
	 "$unc/$isdnRun/$Parameter/$isdnInfected",
	 "$unc/$isdnRun/$Parameter/$isdnSectors"], $verbosepath, 0777);

# now setup the logfile
$vreplogger->localFile("$unc/$isdnRun/$Parameter/vrep.log");
#$vreplogger->screenEcho(1);
$vreplogger->logString("Cookie: $Cookie, ProcessID: $ProcessID, Parameter: $Parameter, UNC: $unc");

my $isdnVOutput = "$unc/$isdnRun/$Parameter";
my ($runnum, @runindex, @runtype, @rundate, @runstrat, @runstat, @runsample);
my ($samplefile, $resultdir, $temppath);

ReadVRun($unc, $Cookie, \$runnum, \@runindex, \@runtype, \@rundate, \@runstrat, \@runstat, \@runsample);
$vreplogger->logString("Read in all samples:");

for ($i = 0; ($i < $runnum) && ($runindex[$i] != $Parameter); $i++) {}

if ($runindex[$i] == $Parameter) {
  $UseThisSample = $runsample[$i];
  $Type = $runtype[$i];
  $Date = $rundate[$i];
  $Strategy = $runstrat[$i];
}

# now setup the ReplicationParameter hash

my (%ReplicationParameter) = ();
if ($runindex[$i] == $Parameter) {
  $ReplicationParameter{Parameter}     = $Parameter;
  $ReplicationParameter{RunSample}     = $runsample[$i];
  $ReplicationParameter{RunType}       = $runtype[$i];
  $ReplicationParameter{RunDate}       = $rundate[$i];
  $ReplicationParameter{RunStrategy}   = $runstrat[$i];
  $ReplicationParameter{ReturnExitMsg} = "N/A";
  $ReplicationParameter{Cookie}        = $Cookie;
}

$vreplogger->logString("Running VIDES2 using");
$vreplogger->logString("Parameter: $ReplicationParameter{Parameter}");
$vreplogger->logString("Sample:    $ReplicationParameter{RunSample}");
$vreplogger->logString("Cookie:    $ReplicationParameter{Cookie}");
$vreplogger->logString("RunType:   $ReplicationParameter{RunType}");
$vreplogger->logString("Strategy:  $ReplicationParameter{RunStrategy}");
$vreplogger->logString("RunDate:   $ReplicationParameter{RunDate}");

###repl($Parameter,\%ReplicationParameter);

$isVarDate = $ReplicationParameter{RunDate};
# $isVarDate = 0;
##### $isVarDate = "199709211631";

# Okeydokey, lets run VIDES2 using the sample in $unc/$UseThisSample
# using parameters $OtherParameter

# Rules for further processing:

# Never WRITE anywhere else than to $unc/$isdnRun/$Parameter and it's
# subdirectories. You may read anything else, though. Also, you MUST
# create this directory yourself!

# You may create a local directory using
# AVIS::FSTools::MkTempWorkDir($Cookie, $ProcessID, $unc) (which
# returns the directory name after creating it on your local system.)

$vreplogger->logString( "-> Entering vides2...");

# generate the status data
$isvarStatus{STATUSMSG}  = "Processing"; #"OK", "INTERROR", "NOREP", "CRASHED"
$isvarStatus{STARTTIME}  = Date2Jap();  # save start date in japanese format
$isvarStatus{ENDTIME}    = 0;           # end date is 0
$isvarStatus{REPLICANTS} = scalar 0;
# write the status file
PutISStatus(%isvarStatus);

$samplefile = "$unc/$UseThisSample";  # this is the path to the sample
$samplefile = AVIS::FSTools::Canonize($samplefile);

if (-e "$samplefile") {
  $vreplogger->logString("Sample $samplefile found");
  print "processing $samplefile\n";
  $resultdir = "$unc/$isdnRun/$Parameter";
  $resultdir = AVIS::FSTools::Canonize($resultdir);
  $vreplogger->logString("\$resultdir = $resultdir");

  $VIDESres  = "$resultdir/VIDES.results";

  # generating local temp directory (workdir)
  $temppath = AVIS::FSTools::MkLocalWorkDir($Cookie, $ProcessID, $unc);

  $ReplicationParameter{isdnTempPath}  = $temppath;
  $ReplicationParameter{ISDNWORKDIR}   = $temppath;
  $ReplicationParameter{ISDNRESULTDIR} = "$isdnRun/$Parameter"; # result without $unc
  $ReplicationParameter{ISDNUNC}       = "$unc";
  $ReplicationParameter{CountFiles}    = 0;   # number of infected files
  $ReplicationParameter{CountSectors}  = 0;   # number of infected sectors
  $ReplicationParameter{ISDNVLOGFILE}  = "$ReplicationParameter{ISDNUNC}/$ReplicationParameter{ISDNRESULTDIR}/$isfnVRepLog";
  $ReplicationParameter{SAMPLEFILE}    = $samplefile;
  $ReplicationParameter{VREPLOGGER}    = $vreplogger;
  print "Logging to   $ReplicationParameter{ISDNVLOGFILE}\n";

  $vreplogger->logString("temppath: $temppath, Cookie $Cookie, PID $ProcessID, UNC $unc");

  ##########################################################################
  #   Find out in which replication mode VIDES2 should run by checking
  #   the Replication Strategy.
  ##########################################################################

  unless ($report_only) {
    if ($ReplicationParameter{RunStrategy} =~ m/^FILE-.+-.+/) {
      $vreplogger->logString("Running Vides-File...");
      AVIS::Vides::ReplicateFile(\%ReplicationParameter) or warn "NOT GOOD!!!";
    } elsif ($ReplicationParameter{RunStrategy} =~ m/^BOOT-.+-.+/) {
      $vreplogger->logString("Running Vides-Boot.........");
      AVIS::Vides::ReplicateBoot(\%ReplicationParameter) or warn "NOT GOOD!!!";
    } else {
      $vreplogger->logString("Error: couldn't resolve file type, Terminating");
    }
  }

  ##########################################################################
  # Process the results
  ##########################################################################

  $vreplogger->logString("Running Vides-Summary...");

  my ($rcVS) = 0;
  $rcVS = AVIS::Vides::VidesSummary(\%ReplicationParameter);
  $vreplogger->logString("VidesSummary returned: $rcVS");

  if ($rcVS) { # if we have samples
    $isvarStatus{STATUSMSG} = "OK"; # "Processing", "OK", "INTERROR", "NOREP", "CRASHED"
  } else {       # if there are no samples generated
    $isvarStatus{STATUSMSG} = "NOREP"; # "Processing", "OK", "INTERROR", "NOREP", "CRASHED"
  }

  ##########################################################################
  # Fill %isvarStautus
  ##########################################################################

  $isvarStatus{ENDTIME}   = Date2Jap();
  $isvarStatus{RETURNMSG} = $ReplicationParameter{ReturnExitMsg}->{TerminationMsg};
  $isvarStatus{REPLICANTS}= $ReplicationParameter{CountSamples};

} else {
  ########################################################################
  # if the $samplefile was not found
  ########################################################################
  my $ErrorMsg = "Sample $samplefile not found, $!";
  $vreplogger->logString("Warning: $ErrorMsg");
  $isvarStatus{STATUSMSG} = "NOREP";
  $isvarStatus{ENDTIME}   = Date2Jap();
  $isvarStatus{REPLICANTS}= 0;
  $isvarStatus{RETURNMSG} = "$ErrorMsg";
}

##########################################################################
# Write ISStatus file
##########################################################################

PutISStatus(%isvarStatus);  # write the status file

# dump content of data structures
my @isDataDmp = Data::Dumper->Dump([\%ReplicationParameter,\%isvarStatus], ["*ReplicationParameter","*isvarStatus"]);

# save @isDataDmp
PutISData(@isDataDmp);

$vreplogger->logString("@isDataDmp");
$vreplogger->logString("-> Leaving vides2...");

$outqueue = $isqnBrepctrl;

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
$vrep_done = 1;
exit;

# show or log errstring to screen
# $ErrType switches between hard termination of program
# or run further
sub vrep_error {
  my($ErrStr,$ErrType) = @_;

  if ($ErrType eq "FATAL") {
    die "$ErrStr\n";
  } elsif ($ErrType eq "NONFATAL") {
    print "$ErrStr\n";
  } else {
    warn "$ErrStr\n";
  }
}

sub repl($%) {
  my ($Parameter1,$ReplicationParameter1) = @_;

  print "Running VIDES2 using Parameter: $Parameter1\n";
  print "Sample:   $ReplicationParameter1->{Sample}\n";
  print "RunType:  $ReplicationParameter1->{RunType}\n";
  print "Strategy: $ReplicationParameter1->{RunStrategy}\n";
  print "RunDate:  $ReplicationParameter1->{RunDate}\n";
}

#---------------------------------------------------------------------
# save the current vrep status
# input hash
#---------------------------------------------------------------------
sub PutISStatus($) {
  my %status = @_;
  my $statusfile = "$isdnVOutput/$isfnVRepStat";
  open(STATUS, ">$statusfile") || die "Error opening $statusfile $!\n";
  my $outputstr = AVIS::ParamStr::PackPara(%status);
  print STATUS $outputstr;
  close(STATUS);
}

sub PutISData($) {
  my @outputstr = @_;

  my $isDatafile = "isdata.000";
  open(ISDATA, ">$isDatafile") || die "Error opening $isDatafile $!\n";
  print ISDATA @outputstr;
  close(ISDATA);
}



END {
  if ($vrep_done) {
#   Do any last-minute normal cleanup in here.
    $vreplogger->logString("vrep finished normally.");
  } else {
#   Signal a terrible unexpected error, and clean up, here.
    $outqueue = $isqnBrepctrl;  # Give up.
    DataFlow::SpecifyStopping($Cookie, $ProcessID, $unc, $outqueue, "1 of 1");
    DataFlow::DoNext ($Cookie, $ProcessID, $unc, $outqueue, $Parameter);
    DataFlow::WriteSignatureForDataFlow ($ProcessID, $unc);
    $vreplogger->logString("vrep finished not-normally; deferring.");
  }
  $vreplogger->logString("Finishing: $0");
  undef $vreplogger;
}
