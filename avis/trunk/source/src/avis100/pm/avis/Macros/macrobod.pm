
package AVIS::Macro::Macrobod;
$VERSION = "0.03";

=head1 NAME

Macrobod.pm - Get NAV-scannable macro bodies contained in file

=head1 AUTHOR

 David M. Chess

=head1 SYNOPSIS

    use AVIS::Macro::Macrobod;

    # Create an object to do things with
    $m = new Macrobod($path_to_utils);

    $success = $m->readFile($name_of_file);
    if ($success) {
      $body_array_ref = $m->getMacros();
      foreach (@$body_array_ref) {
        print "$_->{macroname}:\n";
        print "$_->{macrobody}\n\n";
      }
    } else {
      $error_code = $m->getErrorCode();
      $error_msg = $m->getErrorMessage();
      print STDERR "Error $error_code: $error_msg\n";
    }


=head1 HISTORY

  1999/12/07 - First draft, 0.01
  1999/12/14 - Better reusability, 0.02
  1999/12/16 - Debugging callback, 0.03

=head1 BUGS

None currently known; see Limitations

=head1 LIMITATIONS

Currently only VBA5/6 macros are supported.
Possible return codes from getErrorCode() need to be better documented.

=cut

use AVIS::FSTools;
use strict;

1;

=head1 Creating a new object

=head2 new($path_to_executables)

returns a new object for extracting macros from files.
If $path_to_executables is given, the relevant EXE files
(VBA5EXT.EXE and any others) must be there; otherwise they
must be somewhere on the normal execution path.

=cut

sub new {
  my $class = shift;
  my $path = shift;
  my $self = {};
  bless $self, $class;

  $self->{errorcode} = 0;
  $self->{errormessage} = "Nothing has ever happened";
  $self->{macros} = [];
  $self->{exepath} = $path;
  $self->{debug_callback} = undef;

  return $self;
}

=head1 Using the object

=head2 readFile($file)

processes the named file, reading in all macros that it contains.
Returns true for success, false for failure.

=cut

sub readFile {
  my $self = shift;
  my $fname = shift;

  my $program = AVIS::FSTools::Canonize("vba5ext",$self->{exepath});
  my $command = "$program -o $fname";
  my $results = `$command`;
  $self->{macros} = [];
  $self->{errorcode} = 0;
  $self->{errormessage} = "No error";
  if ($results =~ /^Failed/) {
    $self->{errorcode} = 1;
    $self->{errormessage} = $results;
    return 0;
  }

  while ( $results =~ /^\*\*\*\*\*\*\*\*([^\*]*)\*\*\*\*\*\*\*\*$/mg ) {
    push @{$self->{macros}}, {macroname => $1};
  }

  my @bodies = split /^\*\*\*\*\*\*\*\*[^\*]*\*\*\*\*\*\*\*\*$/m, $results;
  shift @bodies;

  if (scalar @bodies != scalar @{$self->{macros}}) {
    $self->{errorcode} = 2;
    $self->{errormessage} = "Error parsing output from VBA5EXT.";
    return 0;
  }

  my $i;
  for ($i=0;$i<scalar @bodies;$i++) {
    $self->{macros}->[$i]->{macrobody} = $bodies[$i];
    &{$self->{debug_callback}}("Body with stars in $fname.") if $self->{debug_callback} &&
      ($bodies[$i] =~ /\*\*\*\*\*\*\*\*/);
    &{$self->{debug_callback}}("Tiny body in $fname.") if $self->{debug_callback} &&
      (length($bodies[$i]) < 10);
  }

  return 1;
}

=head2 getMacros()

returns an array of hash references, one for each macro found in
the last file read by readFile().
Each referenced hash contains these fields:

  macroname - the macro's name
  macrobody - the body of the macro

=cut

sub getMacros {
  my $self = shift;
  return $self->{macros};
}

=head2 getErrorCode()

returns an integer corresponding to the result of the last call
to readFile.
These error codes are defined:

   0 - no error
  >0 - something bad happened

=cut

sub getErrorCode {
  my $self = shift;
  return $self->{errorcode};
}

=head2 getErrorMessage()

returns a human-readable English error message, possibly more than
one line long, reflecting the result of the last call to readFile.
=cut

sub getErrorMessage {
  my $self = shift;
  return $self->{errormessage};
}

#
# Undocumented debugging callback-thing
#
sub setCallback {
  my $self = shift;
  $self->{debug_callback} = shift;
}


=head1 Copyright

Program property of IBM.
(c) Copyright IBM, 1999, all rights reserved.

=cut

