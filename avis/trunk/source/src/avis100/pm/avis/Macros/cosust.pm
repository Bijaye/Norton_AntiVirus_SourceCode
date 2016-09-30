
package AVIS::Cosust;
$VERSION = "0.02";

=head1 NAME

Cosust.pm - Find the fixed-length common substrings of a set of strings

=head1 AUTHOR

 David M. Chess

=head1 SYNOPSIS

    use AVIS::Cosust;

    # Create an object to do things with
    $m = new Cosust($input_array_ref);

    # Optionally, canonicalize the strings
    die if not $m->canonicalizeStrings($canonicalization_style_name);

    $output_array_ref = $m->findSubstrings($length);

    print "The common $length long substrings are:\n";
    foreach (@$output_array_ref) {
      print "$_ \n";
    }

=head1 HISTORY

  1999/12/09 - First draft, 0.01
  1999/12/13 - canonicalizeStrings, 0.02

=head1 BUGS

None currently known; see Limitations

=head1 LIMITATIONS

The algorithm is certainly not the fastest possible!
Only substrings of a given fixed length can be found.
If you want all (maximal) common substrings, or the longest one,
or anything like that, we can't help you.

=cut

use strict;

1;

=head1 Creating a new object

=head2 new($array_reference)

returns a new object for finding common substrings.
It must be passed a reference to an array the elements of which
are the strings to find substrings inside of.

=cut

sub new {
  my $class = shift;
  my $ref = shift;
  my $self = {};
  bless $self, $class;

  $self->{strings} = $ref;

  return $self;
}

=head1 Using the object

=head2 findSubstrings($length)

returns a reference to an array containing all of the $length-long
substrings of the object's set of strings.

=head2 findSubstrings($length,$exclude)

returns a reference to an array containing all of the $length-long
substrings of the object's set of strings, that do not contain the
string in $exclude.

=cut

sub findSubstrings {
  my $self = shift;
  my $length = shift;
  my $exclude = shift() || "";
  my @answer = ();

  # Find the shortest string
  my $shortestindex = 0;
  my $shortestlength = length($self->{strings}[0]);
  for (my $i=1;$i<@{$self->{strings}};$i++) {
    if (length($self->{strings}[$i]) < $shortestlength) {
      $shortestindex = $i;
      $shortestlength = length($self->{strings}[$i]);
    }
  }
  my $rootstring = $self->{strings}[$shortestindex];

  # Look at every single substring of it
  my %seen = ();
  my $couldbe;
  my $target;
  for (my $i=0;$i<=length($rootstring)-$length;$i++) {
    $couldbe = 1;
    $target = substr($rootstring,$i,$length);
    next if ($exclude && (-1 != index($target,$exclude)));
    next if $seen{$target};
    $seen{$target} = 1;
    foreach (@{$self->{strings}}) {
      if (-1 == index($_,$target)) {
        $couldbe = 0;
        next;
      }
    }
    push @answer, $target if $couldbe;
  }

  return \@answer;
}

=head2 canonicalizeStrings($style)

diddles with the set of strings according to the given style, to
for instance avoid looking at comments and stuff when extracting
common substrings.  Returns true for success, false for failure.

Supported styles:

"VBA5" quasi-removes '-style comments, by removing
everything between a ' and the next line-end.
The contract here is that no substring not containing a ' will appear
after the canonicalization, unless it was there before it.

=cut

sub canonicalizeStrings {
  my $self = shift;
  my $style = shift;

  if ($style eq "VBA5") {
    return $self->_canonicalizeStringsVba5();
  } else {
    return undef;
  }

}

sub _canonicalizeStringsVba5 {
  my $self = shift;

  for (@{$self->{strings}}) {
    s/'[^\n]*\n/'\n/g;
    s/'[^\n]*$/'/g;
  }

  return 1;
}


=head1 Copyright

Program property of IBM.
(c) Copyright IBM, 1999, all rights reserved.

=cut

