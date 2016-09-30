# Manager for the database of known-clean macros

=head1 NAME

Package: AVIS::Macro::Cleans

Filename: pm/AVIS/macros/mkdefs/cleans.pm

Manager for the database of known-clean macros

=head1 STATEMENT

Licensed Materials - Property of IBM

(c) Copyright IBM Corp. 1999

U.S. Government Users Restricted Rights - use,
duplication or disclosure restricted by GSA ADP
Schedule Contract with IBM Corp.

=head1 AUTHOR

David M. Chess

=head1 HISTORY

See VSS

=head1 BUGS

No attempt is made to lock the database file during write operations.
This should not be a problem, as the database will only be read,
not written, during live operations.

=head1 NOTES

The current implementation just uses small flat ASCII files; if we
decide to use a DB2 database (or carrier pigeons) instead, only
this module should have to change.  We'll also need a utility
module that uses this and macrocrc's macro-list method to register
all the macros found in a set of files as clean.

=cut

package AVIS::Macro::Cleans;

#
# A small piece of OO-Perl that knows how get and maintain a list of
# known-clean macros (by their appname and NAV CRC).
#
#

# Class Methods:
#
#
# Instance Methods:
#
#

use Data::Dumper();
use AVIS::FSTools();
use AVIS::Local();

=head1 Class Methods

=head2 new

$clobject = new Cleans($unc,$tooldir);

creates a new instance of the Cleans object, using the given unc
and tooldir as hints as to where to find/keep the database file(s).
The arguments are ignored last time I looked.  Never fails.

=cut

sub new {
  my $class = shift;      # the class to bless into
  my ($unc, $tooldir) = @_;  # our arguments
  my $self = {};          # the anonymous hash to return
  bless $self, $class;
# The "private" instance variables
  $self->{_LastError} = "";
  $self->{_UNC} = $unc;
  $self->{_tooldir} = $tooldir;
# and we're done
  return $self;
}

=head1 Instance methods

=head2 LastError

$err = $cloject->LastError();

returns a message if the last operation failed, or "" if it
worked.

=cut

sub LastError {
  my $self = shift;
  return $self->{_LastError};
}

=head2 getCleanList

$cleanref = $cloject->getCleanList();

returns a reference to a hash, where the value of the expression
   $cleanref->{$app}{$crc}
will be true if and only if a macro for that application, with
that (eight-digit hex) CRC is in the database.  Returns undef if
failure.

=cut

sub getCleanList {
  my $self = shift;
  my $fname = $self->_GetFName();
  $self->{_LastError} = "";
  return $self->_ReadValue($fname);
}

=head2 setCleanList

$success = $cloject->setCleanList($cleanref);

takes a reference to a hash, as above, and replaces the existing
list of clean macros with it.  Returns true for success, false
for failure (and see LastError).

=cut

sub setCleanList {
  my $self = shift;
  my $listref = shift;
  my $fname = $self->_GetFName();
  $self->{_LastError} = "";
  return $self->_RecordValue($listref,$fname);
}

# Private methods:

# Record the given value (must be a ref) in the given file.
# via Data::Dumper.
sub _RecordValue {
  my $self = shift;
  my $value = shift;
  my $fname = shift;
  local *FH;

  open FH, ">$fname" or
    $self->{_LastError} = "Error opening <$fname>: $!.",
    return 0;
  $Data::Dumper::Useqq = 1;
  $Data::Dumper::Indent = 0;
  print FH Data::Dumper::Dumper($value);
  close FH;
  return 1;
}

#
# Get a reference from the given file and return it
#
sub _ReadValue {
  my $self = shift;
  my $fname = shift;
  my $VAR1;

  open FH, "<$fname" or
    $self->{_LastError} = "Error opening <$fname>: $!.",
    return undef;
  $data = <FH>;
  close FH;
  eval $data;
  return $VAR1;
}


#
# What file shall I store it in?
#
sub _GetFName {
  return AVIS::FSTools::Canonize("cleans.db",$AVIS::Local::isdnDataDir);
}

1;

