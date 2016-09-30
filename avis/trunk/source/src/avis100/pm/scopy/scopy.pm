=head1 NAME

Package: scopy

Filename: pm/scopy/scopy.pm

=head1 STATEMENT

Licensed Materials - Property of IBM

(c) Copyright IBM Corp. 1999 

U.S. Government Users Restricted Rights - use,    
duplication or disclosure restricted by GSA ADP   
Schedule Contract with IBM Corp.                  

=head1 AUTHOR

Morton Swimmer

=head1 DESCRIPTION

=cut

package scopy;
use Exporter;
@ISA = qw(Exporter);
@EXPORT = qw (scopy);
@EXPORT_OK = @EXPORT;

use File::Find;
use File::Copy;
use File::Path;
use strict;

#These variables need to be moved to AVIS::Local

my @isvalInclusions = qw(
			 .*\.dd
			 .*\.mna
			 replic\.lst
			 avislog.*\.log
			 appcfg\.lst
			 dfstatus\.dat
			 .*\/sample\/.*
			 .*\/navdefs\/.*
			 .*\/run\/.*\/infected\/.*
			 infected\.rps
			);
my @isvalExclusions = qw(
			 .*\/run\/.*\/infected\/.*\.log
			 .*\/dfresults\/.*
			);
my ($_scopy_to, $_scopy_from);
my $verbose = 1;

1;

=head2 xcopy($$)

Do a selective copy from the directory in the first parameter to the
directory in the second parameter.

=cut

sub scopy ($$) {
  ($_scopy_from, $_scopy_to) = @_;

  unless ((-d $_scopy_from) && (-d $_scopy_to)) {
    warn "'$_scopy_from' does not exist." unless (-d $_scopy_from);
    warn "Destination '$_scopy_to' does not exist." unless (-d $_scopy_to);
  }
  find(\&_scopy, $_scopy_from);
}

sub _scopy {
  my $includeit = 0;
  my ($inclusion, $exclusion);
  if (-f) {
    foreach $inclusion (@isvalInclusions) {
      if ($File::Find::name =~ /$inclusion/i) {
	#printf "Inclusion: %-16s : %s\n", $inclusion, $File::Find::name
	#  if $verbose;
	$includeit = 1;
	last;
      }
    }
    if ($includeit) {
      foreach $exclusion (@isvalExclusions) {
	if ($File::Find::name =~ /$exclusion/i) {
	  #printf "Exclusion: %-16s : %s\n", $inclusion, $File::Find::name 
	  #  if $verbose;
	  $includeit = 0;
	  last;
	}
      }
    }
    if ($includeit) {
      my $reldir;
      my $full = $File::Find::dir;
      $full =~ tr/\\/\//;
      my $root = $_scopy_from;
      $root =~ tr/\\/\//;
      if ($full =~ /($root)[\/\\](.*)/) {
	$reldir = $2;
      }
      print "Copying: $File::Find::name to $_scopy_to/$reldir\n" if ($verbose);
      mkpath(["$_scopy_to/$reldir"], $verbose, 0777);
      copy $_, "$_scopy_to/$reldir";
    }
  }  
}

=head1 HISTORY

=head1 BUGS

=head1 TO DO

scopy should be made to take the inclusions and exclusions as
parameters so that the inclusions and exclusions can be moved to
AVIS::Local.

=head1 SEE ALSO

F<return.pl>, F<deferred.pl>

=cut





