#
# Change history:
#  1998/12/04 - First version using Data::Dumper.
#  1998/12/09 - Make unc slash-stuff more robust and correct.
#  1998/12/10 - Figure out original file name if none recorded.
#  1998/12/15 - Have Replications() record in a $Parameter-relative
#               files, and return the union of all records.
#  1999/02/09 - Use FSTools for Canonize().
#  1999/02/26 - Document JM's separate Goat() and Replicants() functions
#

package AVIS::Macro::Macrodb;

#
# A small piece of OO-Perl that knows how get and set various bits of
# data that are communicated between pieces of BlueIce/Macro.
#
# The current implementation just uses small flat ASCII files; if we
# decide to use the DB2 database (or carrier pigeons) instead, only
# this module should have to change.
#

# Class Methods:
#
#   $dbobject = new Macrodb($Cookie,$unc,$ProcessID,$Parameter);
#
#   creates a new instance of the database-accessor, for the given
#   cookie and unc and so on.  Do $dbobject->LastError() after the
#   "new", to make sure it worked.
#
# Instance Methods:
#
#   $err = $dboject->LastError();
#
#   returns a message if the last operation failed, or "" if it
#   worked.
#
#   $vname = $dbobject->VirusName();
#
#   returns the virus-name string associated with this cookie etc.
#   Virus-name strings had better not contain double-quotes.
#
#   $dbobject->VirusName($newname);
#
#   sets the virus-name string associated with this cookie etc, and
#   returns the value of the string.
#
#   $formarrayref = $dbobject->Forms();
#
#   returns an arrayref representing the forms of this virus that were
#   found during replication.  Each element of the array is a hashref.
#   Each hash has an "Fname" value giving the name of one file that
#   the form was found in, an "App" value giving the application-name
#   string for this form ("WD7", etc), an "Mnames" value that references
#   an array of macro-name strings, and optionally a "Crcvals" value
#   that gives the NAV CRCs of the corresponding macros.
#
#   $dbobject->Forms($formarrayref);
#
#   enters the given array of forms (see last entry) into the database
#   for this cookie etc.
#
#   $arrayref = $dbobject->ReplicationTable();
#
#   returns an reference to an array of list references.  Each list
#   contains an application-name, an language-code, and a strategy,
#   as in ( "WD7", "EN", "standard" ).
#
#   $dbobject->ReplicationTable($arrayref);
#
#   enters the given array of lists (see last entry) into the database
#   for this cookie etc.
#
#   $arrayref = $dbobject->Goats();
#
#   returns an reference to an array of hash references.  Each hash
#   has at least a Before field and an After field, each of which
#   contain a filename.  This form of the call gathers all Goats
#   information that was set under any parameter.
#
#   $dbobject->Goats($arrayref);
#
#   enters the given array of hashes (see last entry) into the database
#   for this cookie and parameter.
#
#   $dbobject->CleanupGoatList()
#
#   destroys any Goats() arrays stored for this cookie under any
#   parameter.
#
#   $arrayref = $dbobject->Replications();
#
#   returns an reference to an array of hash references.  Each hash
#   has at least a Before field and an After field, each of which
#   contain a filename.
#
#   $dbobject->Replications($arrayref);
#
#   enters the given array of hashes (see last entry) into the database
#   for this cookie etc.  The Replications array differs from the
#   Goats array in that there is just one for all runs (all
#   parameters), and only files that were actually modified (in
#   viral-looking ways) should be included in the list.
#
#   $name = $dbobject->OriginalFileName();
#
#   returns a string containing the name of the original infected sample
#   associated with this cookie etc.  (If it has not been explicitly
#   set, as described next, it is assumed to be the name of the first
#   file in $unc/$AVIS::Local::isdnSample.)
#
#   $dbobject->OriginalFileName($name);
#
#   enters the given string (see last entry) into the database for this
#   cookie etc.
#

use Data::Dumper();
use AVIS::FSTools();
use AVIS::Local;

use AVIS::Macro::Types::List;

#
# Create a new one.
#
sub new {
  my $class = shift;      # the class to bless into
  my ($cookie, $unc, $ProcessID, $Parameter) = @_;  # our arguments
  my $self = {};          # the anonymous hash to return
  bless $self, $class;
# The "private" instance variables
  $self->{_LastError} = "";
  $self->{_Cookie} = $cookie;
  $self->{_UNC} = $unc;
  $self->{_ProcessID} = $ProcessID;
  $self->{_Parameter} = $Parameter;
# and we're done
  return $self;
}

#
# Return the last error, if any
#
sub LastError {
  my $self = shift;
  return $self->{_LastError};
}

#
# Record the given value in the given file (must be a ref)
# Uses Data::Dumper
#
sub _RecordValue {
  my $self = shift;
  my $value = shift;
  my $fname = shift;
  local *FH;

  open FH, ">$fname" or
    $self->{_LastError} = "Error opening <$fname>: $!.",
    return;
  $Data::Dumper::Useqq = 1;
  $Data::Dumper::Indent = 0;
  print FH Data::Dumper::Dumper($value);
  close FH;
  return $value;
}

#
# Get a reference from the given file and return it
#
sub _ReadValue {
  my $self = shift;
  my $fname = shift;
  my $VAR1;

  open FH, "<$fname" or
#   $self->{_LastError} = "Error opening <$fname>: $!.",
    return undef;
  $data = <FH>;
  close FH;
  eval $data;
  return $VAR1;
}

#
# Get/Set the virus name for this cookie etc
# Simple file-based implementation
#
sub VirusName {
  my $self = shift;
  my $unc = $self->{_UNC};
  my $fname = AVIS::FSTools::Canonize("vname.mna",$unc);
  my $vname;

  if (@_) {   # We were supplied an argument; update
    $vname = shift;
    return $self->_RecordValue($vname,$fname);
  } else {   # No argument, just read it
    return $self->_ReadValue($fname);
  }
}

#
# Get/Set the Original File Name name for this cookie etc
#
sub OriginalFileName {
  my $self = shift;
  my $unc = $self->{_UNC};
  my $fname = AVIS::FSTools::Canonize("ofname.mna",$unc);
  my $data;

  if (@_) {   # We were supplied an argument; update
    $data = shift;
    return $self->_RecordValue($data,$fname);
  } else {   # No argument, just read it
    $data = $self->_ReadValue($fname);
    return $data if defined $data;
    my $pattern = AVIS::FSTools::Canonize($AVIS::Local::isdnSample,$unc);
    $pattern = AVIS::FSTools::Canonize("*.*",$pattern);
    $data = glob($pattern);
    return undef if $data eq $pattern;
    # Now we have to unc-relativize it!  A sad hack
    return undef if 1 != ($data =~ s/^\Q$unc\E//);
    return substr($data,1);  # Remove leading slash
  }
}

#
# Get/Set the form array for this cookie etc
# Simple no-database implementation
#
sub Forms {
  my $self = shift;
  my $unc = $self->{_UNC};
  my $fname = AVIS::FSTools::Canonize("forms.mna",$unc);
  my $forms;

  if (@_) {   # We were supplied an argument; update
    $forms = shift;
    return $self->_RecordValue($forms,$fname);
  } else {   # No argument, just read it
    my $results = $self->_ReadValue($fname);
    if (!defined $results) {
      $results = new AVIS::Macro::Types::List();
    }
    return $results;
  }
}

#
# Get/Set the form array for this cookie etc
# Simple no-database implementation
#
sub Targets {
  my $self = shift;
  my $unc = $self->{_UNC};
  my $fname = AVIS::FSTools::Canonize("targets.mna",$unc);
  my $forms;

  if (@_) {   # We were supplied an argument; update
    $forms = shift;
    return $self->_RecordValue($forms,$fname);
  } else {   # No argument, just read it
    my $results = $self->_ReadValue($fname);
    if (!defined $results) {
      $results = new AVIS::Macro::Types::List();
    }
    return $results;
  }
}

#
# Get/Set the reptable array for this cookie etc
# Simple no-database implementation
#
sub ReplicationTable {
  my $self = shift;
  my $unc = $self->{_UNC};
  my $fname = AVIS::FSTools::Canonize("reptab.mna",$unc);
  my $data;

  if (@_) {   # We were supplied an argument; update
    $data = shift;
    return $self->_RecordValue($data,$fname);
  } else {   # No argument, just read it (or calculate it)
    return $self->_ReadValue($fname);
  }
}

sub ConversionTable {
  my $self = shift;
  my $unc = $self->{_UNC};
  my $fname = AVIS::FSTools::Canonize("cnvtab.mna",$unc);
  my $data;

  if (@_) {   # We were supplied an argument; update
    $data = shift;
    return $self->_RecordValue($data,$fname);
  } else {   # No argument, just read it (or calculate it)
    return $self->_ReadValue($fname);
  }
}

#
# Get/Set the reptable array for this cookie etc
# Simple no-database implementation
#
sub Conversions {
  my $self = shift;
  my $unc = $self->{_UNC};
  my $fname = AVIS::FSTools::Canonize("converted.mna",$unc);
  my $data;

  if (@_) {   # We were supplied an argument; update
    $data = shift;
    return $self->_RecordValue($data,$fname);
  } else {   # No argument, just read it (or calculate it)
    my $answer = [];
    $data = $self->_ReadValue($fname);
    push @$answer, @$data;
    return $answer;
  }
}


#
# Get/Set the reps array for this cookie etc
#
sub Replications {
  my $self = shift;
  my $unc = $self->{_UNC};
  my $data;

  if (@_) {   # We were supplied an argument; update
    my $fname = AVIS::FSTools::Canonize("infected.rps",$unc);
    $data = shift;
    return $self->_RecordValue($data,$fname);
  } else {   # No argument, read and combine all data
    my $fname = AVIS::FSTools::Canonize("infected.rps",$unc);
    my $answer = [];
    $data = $self->_ReadValue($fname);
    push @$answer, @$data;
    return $answer;
  }
}


#
# Get/Set the discarded reps array for this cookie etc
#
sub DiscardedReps {
  my $self = shift;
  my $unc = $self->{_UNC};
  my $data;

  if (@_) {   # We were supplied an argument; update
    my $fname = AVIS::FSTools::Canonize("discarded.rps",$unc);
    $data = shift;
    return $self->_RecordValue($data,$fname);
  } else {   # No argument, read and combine all data
    my $fname = AVIS::FSTools::Canonize("discarded.rps",$unc);
    my $answer = [];
    $data = $self->_ReadValue($fname);
    push @$answer, @$data;
    return $answer;
  }
}


# Write or read the list of goats used in a particular run.
# Simple no-database implementation.  Note that
# when setting the array, the data is stored in a file
# keyed to the current parameter; when getting the array,
# the data from all existing files are combined into one.
sub Goats {
  my $self = shift;
  my $unc = $self->{_UNC};
  my $data;

  my $arg = shift;
  if (ref($arg) eq "ARRAY") {   # We were supplied a list; update
    my $parm = $self->{_Parameter};
    my $fname = AVIS::FSTools::Canonize("$parm.gts",$unc);
    $data = $arg;
    return $self->_RecordValue($data,$fname);
  } elsif (ref($arg) eq "") {    
    $arg += 0;
    my $fname = AVIS::FSTools::Canonize("$arg.gts",$unc);
    my $answer = [];

    $data = $self->_ReadValue($fname);
    foreach $goat (@$data) {
      $goat->{RunId} = $arg;
    }

    return @$data;
  }
}


sub WriteSWStatus {
  my $self = shift;
  my $unc = $self->{_UNC};
  my $data;

    my $parm = $self->{_Parameter};
    my $fname = AVIS::FSTools::Canonize($isdnRun."/$parm/SW.status",$unc);
    $data = shift;
    return $self->_RecordValue($data,$fname);
}

sub GetSWStatus {
  my $self = shift;
  my $unc = $self->{_UNC};
  my $data;

    my $parm = shift;
    my $fname = AVIS::FSTools::Canonize("$isdnRun/$parm/SW.status",$unc);

    return $self->_ReadValue($fname);
}



1;

