####################################################################
#                                                                  #
# Program name:  mrep.pl                                           #
#                                                                  #
# Description:   Replicate macro viruses on AIX                    #
#                This is a shell that does the protocol stuff      #
#                with DataFlow et all, and then calls the guts     #
#                                                                  #
#                                                                  #
# Statement:     Licensed Materials - Property of IBM              #
#                1998-1999                                         #
#                                                                  #
# Author:        Jean-Michel Boulay, Morton Swimmer, Dave Chess    #
#                                                                  #
#                U.S. Government Users Restricted Rights - use,    #
#                duplication or disclosure restricted by GSA ADP   #
#                Schedule Contract with IBM Corp.                  #
#                                                                  #
#                                                                  #
####################################################################

use strict;

use File::Copy;
use File::Path;
use File::Find;
use Getopt::Long();
use Sys::Hostname;

use DataFlow;
use AVIS::Macro::Replication 2.0;
use AVIS::Macro::Macrodb();
use AVIS::Macro::Command;
use AVIS::Macro::FTP;
use AVIS::Macro::Netbios;

use AVIS::Local;
use AVIS::FSTools;
use AVIS::Logger();
use AVIS::Macro::Types::Event;
use AVIS::Macro::Types::Goat;

use vars qw($SWstatus);

my $started = 1;

my $locallogdir = $ENV{HOME}."/Avis/Macro/Log";
my $locallogname = "mrep.log";

#
# Some overall test and debug and strategy config for this script
#
my $simple_test_only = 0;      # Set to 1 to do nothing much at all

#
# Other globals
#
my $mrep_done = 0;             # Must be set to 1 before normal exit; see END
my $verbose = 0;
my $err_inaccessible_unc = 1;

$SWstatus = "";

#
# Get the parameters that were passed to us from Dataflow
#
my ($Cookie, $ProcessID, $Parameter, $unc);
Getopt::Long::GetOptions("Cookie=s" => \$Cookie,
                         "ProcessId=i" => \$ProcessID,
                         "Parameter=s" => \$Parameter,
                         "UNC=s" => \$unc);

print "Cookie: $Cookie, ProcessID: $ProcessID, Parameter: $Parameter, UNC: $unc\n";


# Convert the unc
# Make the passed UNC canonical, just in case
$unc = AVIS::FSTools::Canonize($unc);


# Get a local logging object in case of a network problem
if (!-e $locallogdir) {
    mkpath ($locallogdir, 0, 0755);
}
my $locallogger = new AVIS::Logger();
$locallogger->localFile($locallogdir."/".$locallogname);
$locallogger->screenEcho(1);        # Make it noisy

my ($remoteunc, $localunc, $workunc);
#set to 1 if direct access to files on server is possible
#set to 0 if we have to access files locally and import/export copies
my $directuncaccess = 0;


my $serverconnection = new AVIS::Macro::FTP($unc);
#my $serverconnection = new AVIS::Macro::Netbios($unc);

my %oldfiles;

$directuncaccess = $serverconnection->AllowsDirectAccess();
if (!$serverconnection->InitializeConnection()) {
  $err_inaccessible_unc = 1;
  my $errmsg = "Cookie: $Cookie, ProcessID: $ProcessID, Parameter: $Parameter, UNC: $unc\n";
  $errmsg .= "Unable to connect to ".$serverconnection->{Server}." via FTP: ".$serverconnection->{LastError}."\n";
  $locallogger->logString($errmsg);
  die "Dying...";
}
if (!$serverconnection->IsAccessible()) {
  $err_inaccessible_unc = 1;
  my $errmsg = "Cookie: $Cookie, ProcessID: $ProcessID, Parameter: $Parameter, UNC: $unc\n";
  $errmsg .= "Unable to find $unc on $serverconnection->{Server}: $serverconnection->{LastError}\n";
  $locallogger->logString($errmsg);
  die "Dying...";
}

$err_inaccessible_unc = 0;


# Otherwise we're expected to actually do some work.


my $GivenParameter = $Parameter;
#truncate the parameter if necessary
$Parameter =~ s/^(.+?)\s.*/$1/;


#local directory for the run
$localunc = AVIS::FSTools::MkLocalWorkDir($Cookie, $ProcessID, $unc);
rmtree($localunc, 0, 0);

if (!$directuncaccess) {
  #copy unc contents from the remote directory
  $serverconnection->CopyDirContents($unc, $localunc, 1);
  #get the list of files initially present in the local unc directory
  %oldfiles = ();
  File::Find::find(sub {$oldfiles{$File::Find::name} = -M $File::Find::name;}, $localunc);
  $workunc = $localunc;
  $remoteunc = $unc;
}
else {
  $remoteunc = $unc; #might need to be translated
  $workunc = $serverconnection->TranslateUnc($remoteunc);
}


# Do just the simple null-test, if requested
if ($simple_test_only) {
  # Just wait a random time, and then be finished
  srand (time() ^ ($$ + ($$ << 15)));
  sleep int(rand 20) + 1;
  $mrep_done = 1;
  return 0;
}


# Get the root for this run (unc/isdnRun/Parameter/)
my $runsubdir = $AVIS::Local::isdnRun."/".$Parameter;
my $remoteroot = AVIS::FSTools::Canonize($runsubdir,$remoteunc);
my $root = AVIS::FSTools::Canonize($runsubdir,$workunc);
#create the remote $root directory if needed
my $localroot = AVIS::FSTools::Canonize($runsubdir,$localunc);
rmtree($localroot, 0, 1);#remove the $root directory if it already exists
mkpath($localroot,0,0755)||warn "Unable to mkpath($localroot,1,0755): $!";   # Should be safer

if ($directuncaccess) {
  # Create result directory
  if (!-e $root) {
    mkpath($root, 0, 0777)||warn "unable to make $unc/$runsubdir: $!";
  }
}


# Get a logging object for logging
my $logger = new AVIS::Logger($localroot);
$logger->screenEcho(1);        # Make it noisy
#$logger->subcode($Parameter);  # Point to the right sublog
my $errmsg = "Starting $0 on ".hostname().".";
$logger->logString($errmsg);

#complain if truncating the parameter has been necessary
if ($Parameter ne $GivenParameter) {
  $logger->logString("The parameter string has been truncated because it contained several values: $GivenParameter");
}


# Options that Replication::Replicate() wants
my $file;
my $language = "";
my $commands;
my $app = "";
my $apprls = "";
my $appbld = "B1";    # Hardwired for now; change when "build" changes


#
# Get a database-access object, and read the Replication Table from it.
#
my $dbobject = new AVIS::Macro::Macrodb($Cookie,$workunc,$ProcessID,$Parameter);
my $dberr = $dbobject->LastError();
if ("" ne $dberr) {
  $errmsg = "Error opening database: $dberr";
  $logger->logString($errmsg);
  die "$errmsg  Stopped ";
}

my $reptable = $dbobject->ReplicationTable();
$dberr = $dbobject->LastError();
if ("" ne $dberr) {
  $errmsg = "Error reading replication table: $dberr";
  $logger->logString($errmsg);
  die "$errmsg  Stopped ";
}

# Make sure it's not too small
if (@$reptable<=$Parameter) {
  $errmsg = "Replication table does not contain <$Parameter> entry. ".@$reptable;
  warn $unc;
  $logger->logString($errmsg);
  die "$errmsg  Stopped ";
}
my $event = @$reptable[$Parameter];


# Extract the file and other information that our entry in it
$file = AVIS::FSTools::Canonize($event->SampleFile(), $workunc);

# Translate two-letter language code into English language-name
my $langcode = $event->Nls();
$language = TranslateNlsCode($langcode);
if ($language eq "") {
  $errmsg = "Language code <$langcode> not recognized.";
  $logger->logString($errmsg);
  die "$errmsg  Stopped ";
}

# Translate short app/version into app and apprls
my $appcode = $event->Application();
($app, $apprls) = TranslateAppCode($appcode);
if ($app eq "") {
  $errmsg = "Application code <$appcode> not recognized.";
  $logger->logString($errmsg);
  die "$errmsg  Stopped ";
}

$commands = $event->Commands();

#must the virus have a special path on the image
my ($samplepath, $date);
if (defined ($event->SamplePath())) {
  $samplepath = $event->SamplePath();
}
if (defined ($event->Date())) {
  $date = $event->Date();
}

# Check is the commands file is known
# if (!(($commands eq "doc2doccmds.lst")
#    || ($commands eq "docinfcmds.lst")
#    || ($commands eq "excelcmds.lst")
#    || ($commands eq "globinfcmds.lst"))) {
#  $errmsg = "Strategy code <$commands> not recognized.";
#  $logger->logString($errmsg);
#  die "$errmsg  Stopped ";
#}


# Make directories; should be done more safely
my ($tmpdir, $resultsdir, $cleandir);
$tmpdir = AVIS::FSTools::Canonize("Tmp",$localroot);
$resultsdir = AVIS::FSTools::Canonize($AVIS::Local::isdnInfected,$localroot);
$cleandir = AVIS::FSTools::Canonize("Clean",$localroot);
mkdir($tmpdir, 0755);
mkdir($resultsdir, 0755);
mkdir($cleandir, 0755);


# setup is finished, tell the user that we're continuing and do stuff
print "Starting Replication with parameters:\n";
print "\tfile=$file\n";
print "\tlanguage=$language\n";
print "\tcommands=$commands\n";
print "\tapp=$app\n\tapprls=$apprls\n";
print "\tappbld=$appbld\n";
print "\tparameter=$Parameter\n";
print "\tresultsdir=$resultsdir\n";
print "\tsamplepath=$samplepath\n" if defined $samplepath;


# Why do we do this?
chdir($tmpdir);

# Actually call the Magic Box
my @repargs = ($app, $apprls, $language, $file, $commands, $resultsdir, $logger);
push(@repargs, $samplepath) if defined $samplepath;
my $rc = AVIS::Macro::Replication::Replicate(@repargs);


# Enter goatname:infname pairs into $dbobject->Replications() from
# wherever the Magic Box stuffed them.
#
# Parse the list of manipulated files generated by RC
my $replist = [];
if (!open(REPLIST, $resultsdir."/replic.lst")) {
  $errmsg = "Failed to open the replicant list.";
  $logger->logString($errmsg);
#  die $errmsg;
}
else {
  my ($before, $after, $beforebis, $filepath, $type);
  mkdir ($cleandir, 0755);
  while(<REPLIST>) {
    if (!m/^#/) {#ignore commented out lines
      if (/\[(.*?)\]\|(.*?)\|(.*?)\|(.*)\n/) {
	my $goat = new AVIS::Macro::Types::Goat;
	($type, $before, $after, $filepath) = ($1, $2, $3, $4);
	my $father = $file;
   #copy the backup copies of the goats to the clean files subdirectory
        if ($before ne $file) {
           #the backup copy is a clean goat
          $before =~ m/.*\/(.*)/;
          $beforebis =  AVIS::FSTools::Canonize($1,$cleandir);
          copy($before, $beforebis)||warn "copy ($before, $beforebis) failed: $!";
        }
        else {
           #the virus sample is the backup copy
           $beforebis =  $file;        
        }
        
        
        #make the paths unc-relative
        $after =~ s/^\Q$localunc\E\///;
        $beforebis =~ s/^\Q$localunc\E\///;
        $beforebis =~ s/^\Q$unc\E\///;
        $father =~ s/^\Q$localunc\E\///;
        $father =~ s/^\Q$unc\E\///;
        
        #fill in the file attributes
        $goat->UsedCopy($after);
        $goat->BackupCopy($beforebis);
        $goat->Type($type);
        $goat->Path($filepath);
        $goat->Father($father);
        push(@$replist, $goat);
      }
    }
  }
  close(REPLIST);
  $dbobject->Goats($replist);
}


$dbobject->WriteSWStatus($SWstatus);

if ($rc && @$replist) {
  open(STATUS, ">".$workunc."/run$Parameter.success");
  close(STATUS);
}

# and we're done!
$errmsg = "$0 finished.";
$logger->logString($errmsg);

$mrep_done = 1;
exit;





sub TranslateAppCode($) {
my $appcode = shift;

  my ($app, $apprls);
  $app = "word", $apprls = "7" if $appcode eq "WD7";
  $app = "word", $apprls = "8" if $appcode eq "WD8";
  $app = "excel", $apprls = "7" if $appcode eq "XL95";
  $app = "excel", $apprls = "8" if $appcode eq "XL97";
  $app = "powerpoint", $apprls = "8" if $appcode eq "PP97";
  return ($app, $apprls);
}


sub TranslateNlsCode($) {
  my $langcode = shift;
  
  $language = "Dutch" if $langcode eq "NL";
  $language = "English" if $langcode eq "EN";
  $language = "French" if $langcode eq "FR";
  $language = "German" if $langcode eq "DE";
  $language = "Italian" if $langcode eq "IT";
  $language = "Spanish" if $langcode eq "SP";
  $language = "Brazilian" if $langcode eq "PT";
  $language = "Taiwanese" if $langcode eq "TW";
  $language = "Chinese" if $langcode eq "CH";
  $language = "Japanese" if $langcode eq "JP";
  return $language;
}


#
# Usual error-capturing stuff.
#

END {
  exit unless $started;
  exit if $simple_test_only;
  if ($mrep_done) {
#   Do any last-minute normal cleanup in here.
    if ($logger) {$logger->logString("mrep $Parameter finished normally.");}
  } else {
#   Signal a terrible unexpected error, and clean up, here.
#   mrep isn't allowed to DEFER, of course.
#   Should we erase all created replicants, to signal an error?  Or
#   just assume that the erroneousness will be obvious to mrepctrl.
    if ($logger && !$err_inaccessible_unc) {$logger->logString("mrep finished not-normally.");}
  }
  
  if (!$err_inaccessible_unc) {
    my $outqueue = $isqnMrepctrl;    # Back to papa
    DataFlow::SpecifyStopping($Cookie, $ProcessID, $workunc, $outqueue, 1);
    DataFlow::DoNext ($Cookie, $ProcessID, $workunc, $outqueue, $Parameter);
    DataFlow::WriteSignatureForDataFlow($ProcessID, $workunc);

    if (!$directuncaccess) {
      #copy now or modified files from local unc to remote unc
      my @newfiles = ();
      File::Find::find(sub {push @newfiles, $File::Find::name if -f $File::Find::name;}, $localunc);
      foreach (@newfiles) {
        #copy files that are new or recently modified to the server
        if (! defined $oldfiles{$_} or $oldfiles{$_} > -M) {
          #a check to be sure the destination directory exists would be nice, even if redundant
          (my $destfile = $_) =~ s/\Q$localunc\E/$unc/;
          $serverconnection->CopyFile($_, $destfile);
        }
      }
    }
    else {
      # Copy local files to the server
      $serverconnection->CopyDirContents($localroot, $remoteroot, 1);
    }
    $serverconnection->ShutdownConnection();
  }

  #cleanup files stored locally
  chdir($isdnROOT);
  rmtree($localunc,0,1) if defined $localunc;

  undef $logger;
}

