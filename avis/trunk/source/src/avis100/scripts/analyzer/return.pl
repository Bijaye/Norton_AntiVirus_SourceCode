=head1 NAME

Filename: Scripts/Communication/return.pl

=head1 STATEMENT

Licensed Materials - Property of IBM

(c) Copyright IBM Corp. 1999 

U.S. Government Users Restricted Rights - use,    
duplication or disclosure restricted by GSA ADP   
Schedule Contract with IBM Corp.                  

=head1 AUTHOR

Morton Swimmer

=head1 DESCRIPTION

Package up contents of the cookie for successful return.

The resulting zip file always gets the name of the cookie number.

=cut

use Getopt::Long;
use Cwd;
use File::Copy;
use File::Path ();
use scopy;

use DataFlow;
use AVIS::FSTools ();
use AVIS::Local;
use AVIS::IceBase 1.10;
use AVIS::IceMisc 1.10;
use AVIS::Crumbs ();
use AVIS::Logger();            # For logging

#my $nul = ">nul";
my $nul = "";
my ($Cookie, $ProcessID, $Parameter, $unc);
my %c;

GetOptions("Cookie=s" => \$Cookie,
	   "ProcessId=i" => \$ProcessID,
	   "Parameter=s" => \$Parameter,
	   "UNC=s" => \$unc);
use strict;

# Get a logging object
my $logger = new AVIS::Logger($unc);
$logger->screenEcho(1);      # Make it noisy
$logger->logString("Starting: $0");
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

move("$tmpDirectory/$Cookie.zip", $exporterdir)
      or AVIS::IceBase::fatal("Failed to copy zip file to exporter directory!"),
  $logger->logString("Failed to copy zip file to exporter directory!");
$logger->logString("Now exporting $Cookie to $attributes{'X-Gateway-Name'}");

AVIS::IceMisc::export($Cookie, $attributes{'X-Gateway-Name'}); 

%c = AVIS::Crumbs::read($unc, $Cookie);

finishIt($statusfile, $c{VirusName}, $Cookie, $Cookie, $attributes{'X-Gateway-Name'});

my $outqueue = $isqnEnd;

# Clean up the local directories

#AVIS::FSTools::Wipe($tmpDirectory);

File::Path::rmtree([$tmpDirectory], 1, 1);

$logger->logString("Finishing: $0");

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

  $curdir = getcwd();
  $logger->logString("We were in '$curdir'");
  my $flags = "-r";
  $cmdline = "$program $flags $archivedir\\$archivename * $nul";
  $logger->logString("Executing: '[$archivedir]> $cmdline'");
  Cwd::chdir($tmpDirectory) 
    or $logger->logString("Couldn't chdir to $archivedir");
  $logger->logString("Cwd: ".getcwd());
  $cc = system($cmdline);
  Cwd::chdir($curdir) or $logger->logString("Couldn't chdir back to $curdir");
  AVIS::IceBase::fatal("zip failed, cannot zip $directory into $archivedir/$archivename, cc=$cc, $!\n"),
  $logger->logString("zip failed, cannot zip $directory into $archivedir/$archivename, cc=$cc, $!") 
    if $cc;

}

=head2 finishIt

This function changes the state of the sample to infected and puts it into the finish queue.

=cut

sub finishIt {
  my ($statusfile,$vname,$sigid,$cookie,$gateway) = @_;
  AVIS::IceFile::movehttp($statusfile,
	AVIS::IceBase::config('finishQueue')."\\".$gateway,
	'X-Analysis-State'        => "infected",
	'X-Analysis-Virus'        => $vname,
	'X-Signatures-Identifier' => $sigid,
	'X-Date-Finished'         => HTTP::Date::time2str(),
	);
  AVIS::IceBase::event("sample $cookie enqueued for finishing\n"."-"x30),
  $logger->logString("sample $cookie enqueued for finishing");
}

=head1 HISTORY

=head1 BUGS

=head1 TO DO

=head1 SEE ALSO

F<deferred.pl>

=cut
