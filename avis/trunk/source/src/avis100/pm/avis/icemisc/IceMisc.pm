=head1 NAME

Package: AVIS::IceMisc

Filename: pm/AVIS/IceMisc/IceMisc.pm

Miscellaneous routines for the communications sub system.

=head1 STATEMENT

Licensed Materials - Property of IBM

(c) Copyright IBM Corp. 1998-1999 

U.S. Government Users Restricted Rights - use,    
duplication or disclosure restricted by GSA ADP   
Schedule Contract with IBM Corp.                  

=head1 AUTHOR

Edward Pring, Morton Swimmer


=head1 DESCRIPTION

=cut

package AVIS::IceMisc;

# This package exports several public functions:

require Exporter;
$VERSION = 1.10;
@ISA = qw(Exporter);
@EXPORT = qw();
@EXPORT_OK = qw(virsig checklog export);

# This package uses several public Perl packages, which must be
# installed in the Perl library directory of the machine where this
# program executes:

use Time::Local;
use strict;

# This package uses several private Perl packages, which must be
# installed in the current directory of the machine where this program
# executes:

use AVIS::IceBase ();
use AVIS::IceFile ();
use AVIS::IceHTTP ();

1;

=head2 virsig

This function reads the header of a specified VIRSIG.LST file 
and returns the signature identifier in it.

=cut

sub virsig ($) {
  # The parameter for this function is the filename of the VIRSIG.LST
  # file to be read.

  my ($filename) = @_;
  return "" unless -f $filename;

  # Read the header of the specified VIRSIG.LST file.

  my $header = "";
  open VIRSIG,"<$filename" 
    or AVIS::IceBase::fatal("cannot open $filename, $!\n");
  binmode VIRSIG;
  read VIRSIG, $header, 80 or AVIS::IceBase::fatal("cannot read $filename, $!\n");
  close VIRSIG;
    
  # Extract the signature identifier, production date, and expiration date.

  my ($identifier,$produced,$expires) = $header =~ m/^\w+ (\w+) +\(([\d\/]+)\). ([\d\/]+)/;

  # Ensure that the signature identifier has the expected format.

  AVIS::IceBase::error("invalid identifier $identifier in $filename\n"), return () 
    unless $identifier =~ m/AV[0-9]+[A-Z]+/;

  # Ensure that the production date has the expected format, and 
  # then convert it to standard timestamp format.

  my ($year,$month,$day) = ();
  ($year,$month,$day) = $produced =~ m/(\d+)\/(\d+)\/(\d+)/;
  AVIS::IceBase::error("invalid creation date $produced in $filename\n"), return () 
    unless $year && $month && $day;
  $produced = timegm(0,0,0,$day,$month-1,$year);
  #print "VIRSIG.LST: produced $month/$day/$year=$produced\n";

  # Ensure that the expiration date has the expected format, and 
  # then convert it to standard timestamp format.

  ($year,$month,$day) = $expires =~ m/(\d+)\/(\d+)\/(\d+)/;
  AVIS::IceBase::error("invalid expiration date $expires in $filename\n"), return () 
    unless $year && $month && $day;
  $expires = timegm(0,0,0,$day,$month-1,$year);
  #print "VIRSIG.LST: expires $month/$day/$year=$expires\n";

  # Return the signature identifier and timestamps from the VIRSIG.LST file.

  return ($identifier,$produced,$expires);
}

=head2 export

This function enqueues export status files for the specified signature
files.  A status file is created in the export directory for each of
the specified gateways.

The parameters for this function are the identifier of the signatures
to be exported and a string containing zero or more blank-delimited
gateway URLs.

=cut

sub export ($;$) {
  my ($identifier,$gateways) = @_;
  my $zipfile = AVIS::IceBase::config('producedArchive')."\\$identifier.ZIP";
  AVIS::IceBase::fatal("cannot find file $zipfile\n") unless -f $zipfile;

  # If no gateways are specified, then export the signatures
  # to all configured gateways.

  $gateways = AVIS::IceBase::config('gatewayNames') unless $gateways;

  # ...

  my $directory = AVIS::IceBase::config('tmpDirectory')."\\IceExport";
  AVIS::IceFile::create($directory);

  # For each specified or configured gateway, create a status file
  # in the export directory for the specified signatures.
  my $gateway;
  foreach $gateway (split " ", $gateways) {
    my $statusfile = "$directory\\$identifier.SSA";
    my $target = AVIS::IceBase::config('exportQueue')."\\".$gateway;
    AVIS::IceFile::create($target);
    AVIS::IceFile::movehttp($statusfile,
			    $target,
			    'X-Signatures-Identifier'=> $identifier,
			   );
    AVIS::IceBase::event("signatures $identifier enqueued for gateway $gateway\n");
  }

  # ...

  AVIS::IceFile::uncreate($directory);
}

=head2 checklog

This function enqueues export status files for the specified signature
files.  A status file is created in the export directory for each of
the specified gateways.

=cut

sub checklog ($) {
  my ($filename) = @_;
  return () unless -f $filename;

  # Read the log file and extract the version identifiers for the
  # build and signature files.

  my ($build,$virsig,$verv) = ();
  open LOG,"<$filename" or AVIS::IceBase::fatal("cannot open $filename, $!\n");
  while (<LOG>) {
    $build  = $1 if m/\*\*\*\* build ([\w\.]+)/i;
    $virsig = $1 if m/virsig\.lst: (\w+)/i;
    $verv   = $1 if m/verv\.vdb: (\w+)/i;
  }
  close LOG;

  # Return the values extracted from the log file.

  return ($build,$virsig,$verv);
}


######################################################################

=head1 HISTORY

MS19990225 Assimilated

=head1 BUGS

=head1 SEE ALSO

list of other relevant modules or information sources

=cut


