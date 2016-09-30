=head1 NAME

Filename: Scripts/Communication/deferred.pl

=head1 STATEMENT

Licensed Materials - Property of IBM

(c) Copyright IBM Corp. 1999 

U.S. Government Users Restricted Rights - use,    
duplication or disclosure restricted by GSA ADP   
Schedule Contract with IBM Corp.                  

=head1 AUTHOR

Morton Swimmer

=head1 DESCRIPTION

Package up contents of the cookie for unsuccessful return.

The resulting zip file always gets the name of the cookie number.

=cut

use Getopt::Long;
use Cwd ();
use File::Copy;
use File::Path ();
use scopy;

use DataFlow;
use AVIS::FSTools ();
use AVIS::Local;
use AVIS::IceBase 1.10;
use AVIS::IceMisc 1.10;
use AVIS::Logger();            # For logging

#my $nul = ">nul";
my $nul = "";
my ($Cookie, $ProcessID, $Parameter, $unc);

GetOptions("Cookie=s" => \$Cookie,
	   "ProcessId=i" => \$ProcessID,
	   "Parameter=s" => \$Parameter,
	   "UNC=s" => \$unc);
use strict;

# Get a logging object
my $logger = new AVIS::Logger($unc);
$logger->screenEcho(1);      # Make it noisy
$logger->logString("Starting: $0\n");
$logger->logString("Cookie: $Cookie, ProcessID: $ProcessID, Parameter: $Parameter, UNC: $unc");

AVIS::IceBase::profile("IceWS.prf");

my $tmpDirectory = AVIS::FSTools::MkLocalWorkDir($Cookie, $ProcessID, $unc);
my $archivedir = "$tmpDirectory/contents";
File::Path::mkpath([$archivedir], 1, 0777);

archive($tmpDirectory, "$Cookie.zip", $unc, $archivedir);

my $exporterdir = AVIS::IceBase::config('producedArchive'); 
my $statusfile = AVIS::IceBase::config('analyzeNormalQueue')."\\$Cookie.VSA";
my $samplefile = AVIS::IceBase::config('importedArchive')."\\$Cookie.VS";
my %attributes;

AVIS::IceFile::readhttp($samplefile,undef,\%attributes) 
  or AVIS::IceBase::fatal("cannot read sample file $samplefile\n"),
  $logger->logString("cannot read sample file $samplefile");

move("$tmpDirectory\\$Cookie.zip", $exporterdir)
      or AVIS::IceBase::fatal("Failed to move zip file to exporter directory!"),
  $logger->logString("Failed to move zip file to exporter directory!");

$logger->logString("Now exporting $Cookie to $attributes{'X-Gateway-Name'}\n");
AVIS::IceMisc::export($Cookie, $attributes{'X-Gateway-Name'}); 

deferIt($statusfile, $Cookie, $Cookie);

my $outqueue = $isqnEnd;

# Clean up the local directories

#AVIS::FSTools::Wipe($tmpDirectory);

File::Path::rmtree([$tmpDirectory], 1, 1);

$logger->logString("Finishing: $0\n");

# Do some Dataflow magic

SpecifyStopping($Cookie, $ProcessID, $unc, $outqueue, "1 of 1");

# Tell Dataflow what to do next (nothing)

DoNext ($Cookie, $ProcessID, $unc, $outqueue, $Parameter);

# Tell Dataflow that we are finished

WriteSignatureForDataFlow ($ProcessID, $unc);

1;

=head2 archive

Make an archive the directory given as a parameter to the file also
specified.

=cut

sub archive ($$$$) {
  my ($archivedir, $archivename,$directory,$tmpDirectory) = @_;
  my $program = AVIS::IceBase::config('zipProgram');
  my ($cc, $cmdline, $curdir);

  AVIS::IceBase::fatal("zip failed, cannot find program $program\n"),
  $logger->logString("zip failed, cannot find program $program") 
    unless -x $program;
  AVIS::IceBase::fatal("zip failed, cannot find directory $directory\n"),
  $logger->logString("zip failed, cannot find directory $directory") 
    unless -d $directory;
  AVIS::IceBase::fatal("zip failed, cannot find directory $tmpDirectory\n"),
  $logger->logString("zip failed, cannot find directory $tmpDirectory") 
    unless -d $tmpDirectory;
  AVIS::IceBase::fatal("zip failed, duplicate file $archivename\n"),
  $logger->logString("zip failed, duplicate file $archivename") 
    if -f "$archivedir/$archivename";

  # copy all files we want to archive to the tmpDirectory

  $tmpDirectory =~ tr/\//\\/;
  $unc =~ tr/\//\\/;

  scopy($unc, $tmpDirectory);

  $curdir = Cwd::getcwd();
  my $flags = "-r";
  $cmdline = "$program $flags $archivedir\\$archivename * $nul";
  $logger->logString("Executing: '[$archivedir]> $cmdline'");
  Cwd::chdir($tmpDirectory) 
    or $logger->logString("Couldn't chdir to $archivedir");
  $logger->logString("Cwd: ".Cwd::getcwd());
  $cc = system($cmdline);
  Cwd::chdir($curdir) 
    or $logger->logString("Couldn't chdir back to $curdir");
  AVIS::IceBase::fatal("zip failed, cannot zip $directory into $archivedir/$archivename, cc=$cc, $!\n"),
  $logger->logString("zip failed, cannot zip $directory into $archivedir/$archivename, cc=$cc, $!") if $cc;

}

=head2 deferIt

This function changes the state of the sample to deferred, but puts
the sample into the finish queue. Why? In this release of the analysis
center, we are not expected to process these samples after humans have
touched them. Of course, the real immune system works differently.

The state of the sample is changed to 'deferred' as expected.

=cut

sub deferIt {
  my ($statusfile,$sigid,$cookie) = @_;
  AVIS::IceFile::movehttp($statusfile,
#	AVIS::IceBase::config('deferredArchive'), #see note above...
	AVIS::IceBase::config('finishQueue'),
	'X-Analysis-State'        => "deferred",
	'X-Signatures-Identifier' => $sigid,
	'X-Date-Finished'         => HTTP::Date::time2str(),
	);
  AVIS::IceBase::event("analysis of sample $cookie deferred\n"."-"x30),
  $logger->logString("analysis of sample $cookie deferred");
}

=head1 HISTORY

=head1 BUGS

=head1 TO DO

=head1 SEE ALSO

F<return.pl>

=cut





