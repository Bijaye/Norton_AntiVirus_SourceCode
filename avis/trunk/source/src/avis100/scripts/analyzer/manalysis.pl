####################################################################
#                                                                  #
# Program name:  manalysis.pl                                      #
#                                                                  #
# Description:   Produce definitions for macro viruses.            #
#                                                                  #
#                                                                  #
# Statement:     Licensed Materials - Property of IBM              #
#                                                                  #
# Author:        Dave Chess                                        #
#                                                                  #
#                U.S. Government Users Restricted Rights - use,    #
#                duplication or disclosure restricted by GSA ADP   #
#                Schedule Contract with IBM Corp.                  #
#                                                                  #
# Change history: See VSS or CMVC or whatever.                     #
#                                                                  #
####################################################################
#                                                                  #
# Function:                                                        #
#  When called with the Standard Dataflow Switches, processes the  #
#  forms associated with this cookie-number in The Database, and   #
#  writes new NAV defs to $unc\navdefs.  Errors and                #
#  progress messages and such are written to a Logger.             #
#                                                                  #
####################################################################

use Getopt::Long();            # For parameter parsing
use DataFlow();                # For talking to Dataflow
use AVIS::Macro::Macrodb();    # For finding out about the past
use AVIS::Macro::Mkdefs();     # For making the future
use AVIS::Macro::Qut();        # For quick unit test
use AVIS::Macro::Types::List;  # For accessing the list of forms created by mrepctrl
use AVIS::FSTools();           # Random annoying utilities
use AVIS::Local();             # System globals
use AVIS::Crumbs();            # Interface to the Crumbs DB
use AVIS::Logger();            # A logging object
use File::Path();              # For mkpath()

use strict;

my $started = 1;            # Just so the END block can exit if -c

#
# Some overall test and debug and strategy config for this script
#
my $simple_test_only = 0;      # Set to 1 to do nothing much at all
my $use_existing_nav_defs = 0; # Set to 1 to read in and parse the existing
                               #   NAV defs, to avoid producing redundant ones.
                               # NOTE: code currently commented out.
my $ignore_macro_names = $AVIS::Local::ignoreMacroNames;
my $use_notfullset = 0;
my $old_protocol = 0;

my $success_queue = $old_protocol ? $AVIS::Local::isqnReturn :
                                    $AVIS::Local::isqnSARCExit;

#
# Other globals
#
my $mana_done = 0;             # Must be set to 1 before normal exit; see END
my ($errmsg,$dberr,$thisform,$thisdef,$thiscat,$thisrep,$success);
my ($outstring,$logstring,$outqueue);

#
# Get the parameters that were passed to us from Dataflow
#
my ($Cookie,$ProcessID,$Parameter,$unc);
Getopt::Long::GetOptions("Cookie=s" => \$Cookie,
                         "ProcessId=i" => \$ProcessID,
                         "Parameter=s" => \$Parameter,
                         "UNC=s" => \$unc);

# Do just the simple null-test, if requested

if ($simple_test_only) {

  # Just wait a random time, and schedule a random successor
  srand (time() ^ ($$ + ($$ << 15)));
  sleep int(rand 20) + 1;
  $outqueue = ((int(rand 2) == 0 ) ?
    $success_queue : $AVIS::Local::isqnDefer);
  DataFlow::SpecifyStopping($Cookie, $ProcessID, $unc, $outqueue, 1);
  DataFlow::DoNext ($Cookie, $ProcessID, $unc, $outqueue, $Parameter);
  DataFlow::WriteSignatureForDataFlow($ProcessID,$unc);
  return 1;

}

# Make the passed UNC canonical, just in case
$unc = AVIS::FSTools::Canonize($unc);

# Get a logging object to chat into
my $logger = new AVIS::Logger($unc);
$logger->screenEcho(1);

# Otherwise we're expected to actually do some work.

# Make sure our output directory is there.
my $navdefout = AVIS::FSTools::Canonize("navdefs",$unc);
if (-e $navdefout) {
  if (not -d $navdefout) {
    $errmsg = "$navdefout exists, but isn't a directory!!";
    $logger->logString($errmsg);
    die "$errmsg  Stopped ";    # kick out to END routine
  }
} else {
  if (not File::Path::mkpath($navdefout)) {
    $errmsg = "Error making $navdefout: $!.";
    $logger->logString($errmsg);
    die "$errmsg  Stopped ";
  }
}
$logger->logString("Starting: $0");
$logger->logString("manalysis: Cookie= $Cookie, ProcessID= $ProcessID, Parameter= $Parameter, UNC= $unc");

#
# Get a database-access object, and read the virus-name and list
# of forms from it.
#
my $dbobject = new AVIS::Macro::Macrodb($Cookie,$unc,$ProcessID,$Parameter);
$dberr = $dbobject->LastError();
if ("" ne $dberr) {
  $errmsg = "Error opening database: $dberr";
  $logger->logString($errmsg);
  die "$errmsg  Stopped ";
}
my $virusname = $dbobject->VirusName;
my $formlist = $dbobject->Forms;
my @forms = $formlist->ItemList();
my $formarray = \@forms;
my $orgfname = $dbobject->OriginalFileName;

#
# Get a def-making object, set it up, and give it the name and forms
#
my $defobject = new AVIS::Macro::Mkdefs();
$defobject->IgnoreMacroNames($ignore_macro_names);
$defobject->UseNotFullSet($use_notfullset);
$defobject->StatusCallback(\&LogCallback);
my $tooldir = AVIS::FSTools::Canonize($AVIS::Local::isdnBin,$AVIS::Local::isdnROOT);
$defobject->ToolDir($tooldir);
$defobject->UNC($unc);
$defobject->VirusName($virusname);
$defobject->SetForms($formarray);

##
## Load the existing NAV defs into it, if required
##
#if ($use_existing_nav_defs) {
#  my $navdefin = AVIS::FSTools::Canonize($AVIS::Local::isdnNavex,$AVIS::Local::isdnRoot);
#  if (not $defobject->LoadNavDefs($navdefin)) {
#    $errmsg = $defobject->GetLastError();
#    $logger->logString($errmsg);
#    die "$errmsg  Stopped ";
#  }
#}

#
# Have it produce the defs for us, and handle failure
#
my @navdefs = $defobject->MakeNavDefs();
if (not scalar(@navdefs)) {
  $errmsg = $defobject->GetLastError();
  $logger->logString($errmsg);
  die "$errmsg  Stopped ";
}

#
# Figure out the apps involved (for DEFTYPE)
#
my %formhash = ();
foreach $thisform ( @$formarray ) {
  $formhash{$thisform->{App}} = 1;
}
my $forms = join " ",keys %formhash;


#
# Write out the new file(s)
#

#Read the crumbs database
my %Crumbs = AVIS::Crumbs::read($unc, $Cookie);
my $vname = $Crumbs{"VirusName"};
my $defprev = $Crumbs{"DefPrev"};

$defprev = "UNKNOWN" unless $defprev;

my $outfname = AVIS::FSTools::Canonize("delta",$navdefout);
open OUTFH, ">$outfname" or
  $errmsg = "Error opening $outfname: $!.",
  $logger->logString($errmsg),
  die "$errmsg  Stopped ";
print OUTFH <<"EOD";

[MACRODEFSTART]

DEFTYPE = $forms
DEFPREV = $defprev

EOD
print OUTFH "| Suggested virus name: \"$vname\"\n\n" if $vname;

# Find out what categories exist
my %catlist = ();
foreach $thisdef (@navdefs) {
  $thiscat = $thisdef->{Category};
  $catlist{$thiscat} = 1;
}
foreach $thiscat (keys %catlist) {
  print OUTFH "\n[".$thiscat."START]\n\n";
  foreach $thisdef (@navdefs) {
    next unless $thisdef->{Category} eq $thiscat;
    $outstring = $thisdef->{Content};
    print OUTFH "$outstring\n";
  }
  print OUTFH "\n[".$thiscat."END]\n\n";
}
print OUTFH "[MACRODEFEND]\n";
close OUTFH;

$logger->logString("manalysis produced new definitions");

#
# Do a quick unit test, just to make sure (in the old world only)
#
if ($old_protocol) {
  my $infectedFiles = [];
  my $reps = $dbobject->Replications();
  # NOTE: assumes that mrepctrl.pl has already screened out files that
  # did not have any macros added to them during replication, or that
  # otherwise are probably not infected.
  foreach $thisrep ( @$reps ) {
    push @$infectedFiles, AVIS::FSTools::Canonize($thisrep->{After},$unc);
  }
  # Pass along the original sample, as well
  push @$infectedFiles, AVIS::FSTools::Canonize($orgfname,$unc);
  # Actually do the test
  my $thisQut = new AVIS::Macro::Qut($outfname,$infectedFiles);
  $thisQut->logObject($logger);
  $success = $thisQut->doTest();
  unless ($success) {
    $logstring = $thisQut->logString();
    $errmsg = "Quick unit test failed: $logstring";
    $logger->logString($errmsg);
    die "$errmsg  Stopped ";
  }
  $logger->logString("New definitions passed quick unit test");
}

#
# Do any last-minute really-really-final checks
#
if (might_be_mass_copier($formarray)) {
  $errmsg = "Virus appears to be a mass copier; deferring.";
  $logger->logString($errmsg);
  die "$errmsg  Stopped ";
}

#
# Schedule the next event for Dataflow
#
$outqueue = $success_queue;              # Take it away!
$Parameter = "$AVIS::Local::isfnDeltaPathName $AVIS::Local::isqnSerializeBuild $outfname";
DataFlow::SpecifyStopping($Cookie, $ProcessID, $unc, $outqueue, 1);
DataFlow::DoNext ($Cookie, $ProcessID, $unc, $outqueue, $Parameter);
DataFlow::WriteSignatureForDataFlow($ProcessID,$unc);

#
# And that's really about all...
#
$mana_done = 1;
exit;

#
# Usual error-capturing stuff.
#

END {
  exit if not $started;
  exit if ($simple_test_only);
  if ($mana_done) {
#   Do any last-minute normal cleanup in here.
    $logger->logString("manalysis finished normally.");
  } else {
#   Signal a terrible unexpected error, and clean up, here.
    $outqueue = $AVIS::Local::isqnDefer;    # Give up.
    DataFlow::SpecifyStopping($Cookie, $ProcessID, $unc, $outqueue, "1 of 1");
    DataFlow::DoNext ($Cookie, $ProcessID, $unc, $outqueue, $Parameter);
    DataFlow::WriteSignatureForDataFlow($ProcessID,$unc);
    $logger->logString("manalysis finished not-normally; deferring.");
  }
  $logger->logString("Finishing: $0");
  undef $logger;
}


#
# A callback routine to log chit-chat from the def-maker
#
sub LogCallback {
  my ($category, $msg) = @_;
  $logger->logString("\[$category\] $msg");
}

#
# Might this virus be a mass copier?  (Based on finding our special
# macro-name in any of the forms)
#
sub might_be_mass_copier {
  my $formarray = shift;
  my $thisform;
  my $names;
  my $goatname = $AVIS::Local::ismnMassCopyGoatMacroName;

  foreach $thisform (@$formarray) {
    $names = $thisform->{"Mnames"};
    foreach (@$names) {
      return 1 if lc($_) eq lc($goatname);
    }
  }
  return 0;
}

