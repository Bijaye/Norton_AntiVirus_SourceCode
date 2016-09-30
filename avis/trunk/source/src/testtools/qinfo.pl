#!/usr/bin/perl -w

=head1 NAME

qinfo.pl -- Dump information about Quarantine samples.

=head1 STATEMENT

(c) Copyright IBM Corp. 2000

U.S. Government Users Restricted Rights - use,     
duplication or disclosure restricted by GSA ADP    
Schedule Contract with IBM Corp.                   

=head1 AUTHOR

Ian Whalley, E<lt>inw@us.ibm.comE<gt>, 2000-03-15

=head1 REQUIREMENTS

Any recent Perl for Win32.

=head1 SYNOPSIS

B<qinfo.pl> [--release] [--realname] E<lt>list of files and directoriesE<gt>

=head1 DESCRIPTION

B<qinfo.pl> allows the user to see a listing of information
about a freezedried quarantine sample -- most notably, perhaps,
a list of attributes.  It does I<not> use the quarantine API
to obtain this information -- it simply needs to see the freezedried
file.

If B<--release> is specified, B<qinfo.pl> will attempt to extract
the 'original file' (that which was captured) from the freezedried
sample.  It will place this file into a file in the same directory
as the freezedried sample, with '.content' appended to the name.

=head1 BUGS

It's pretty inflexible right now.  A more generic version would
be nice, so the user can ask to view the value of just one attribute.
Attribution addition/modification features would also be really nice.

=cut

# -----

use strict;

use IO::Handle;
use Getopt::Long;

# -----

#
# This list is used by decodeAttributeType()
# to produce a printable version of the numeric
# attribute type.
#
my @typeList =
(
  "UNKNOWN",   # 0
  "STRING",    # 1
  "NUMERIC",   # 2
  "DATE",      # 3
);

my $strAttrType = 1;
my $u14AttrType = 2;
my $datAttrType = 3;

my $restoreSample;
my $useRealName;

# -----

#
# Allows the caller to read descrambled bytes from the quarantine
# file.  Parameters as for read(), except that the buffer is a
# reference.  Returns the number of bytes read.
#
sub readScrambled($$$)
{
  my ($ih,$buffPtr,$len) = @_;
  my $tmpLenRead      = 0;
  my $lenRead         = 0;
  my $char;
  my $buff;

  $$buffPtr = "";

  while ($lenRead != $len)
  {
    $tmpLenRead = read($ih, $char, 1);
    (! $tmpLenRead) && last;
    $char = chr((ord($char)) ^ 0xFF);
    $$buffPtr .= $char;
    $lenRead += $tmpLenRead;
  }

  return $lenRead;
}

# -----

#
# Allows the caller to read a U32 from the file.
#
# Returns: the number on success, undef on error.
sub readU32($)
{
  my ($ih) = @_;
  my $buff;
  my $num;
  my $lenRead;

  # A U32 is, by definition, four bytes.
  $lenRead = readScrambled($ih, \$buff, 4);
  ($lenRead != 4) && return undef;

  # Convert it to a number.
  ($num) = unpack('L', $buff);

  # JD.
  return $num;
}

# -----

#
# Allows the caller to read the next attribute name
# from the file.  It must be under the file pointer :-).
#
# Returns: name on success, undef on error.
#
sub readAttributeName($)
{
  my ($ih) = @_;
  my $buff;
  my $name;
  my $lenRead;

  # Attribute names are 64 characters in length.
  $lenRead = readScrambled($ih, \$buff, 64);
  ($lenRead != 64) && return undef;

  # They are zero-padded, so throw that stuff away.
  ($name) = unpack('a*', $buff);

  # JD.
  return $name;
}

# -----

#
# Allows the caller to read the type of current
# attribute.  The filepointer must, again, be in
# the correct place.
#
# Returns: numeric attribute type on success, undef on error.
#
sub readAttributeType($)
{
  my ($ih) = @_;
  my $buff;
  my $type;
  my $lenRead;

  # Attribute names are 64 characters in length.
  $lenRead = readScrambled($ih, \$buff, 2);
  ($lenRead != 2) && return undef;

  # This is an unsigned short (2 bytes).
  ($type) = unpack('S', $buff);

  # JD.
  return $type;
}

# -----

#
# Allows the caller to obtain a printable version of the
# numeric attribute type.  Uses the array typeList defined
# earlier.  In the event of getting something it doesn't
# recognise, it will return 'UNKNOWN'.
#
sub decodeAttributeType($)
{
  my ($numericType) = @_;

  (defined $typeList[$numericType]) && return $typeList[$numericType];

  return "UNKNOWN";
}

# -----

#
# Allows the caller to obtain a printable version of the
# attribute value.
#
sub getPrintableAttrVal($$)
{
  my ($attrType,$attrVal) = @_;

  # First, the easy string case.
  if ($attrType == $strAttrType)
  {
    # Strip trailing nulls.
    $attrVal =~ s/\0+$//;
    return $attrVal;
  }

  # Now, the u14 case.
  if ($attrType == $u14AttrType)
  {
    my $val;
    ($val) = unpack('L', $attrVal);
    return $val;
  }

  # I don't know how to parse these out, yet.
  if ($attrType = $datAttrType)
  {
    return "[DATE VALUE]";
  }

  # Some weird new type, presumably.  Give up.
  return $attrVal;
}

# -----

sub processOneFile($)
{
  my ($item) = @_;
  my $ih;
  my $buff;
  my $lengthOfCapturedFile;
  my $numOfAttrs;
  my $numOfAttrsFound;
  my $attrName;
  my $startOfAttrFilePos;
  my $endOfAttrFilePos;
  my $fileNameToEmitTo;

  # Get a new handle.
  $ih = IO::Handle->new();
  if (! defined $ih)
  {
    print "Couldn't get filehandle, you loose.\n";
    return 0;
  }

  # Attempt to open the quarantine file.
  if (! open($ih, $item))
  {
    print "Couldn't open '$item', skipping.\n";
    return 0;
  }

  # Go into binary mode (stupid bloody WinDOS operating systems).
  binmode($ih);

  # Get the magic number, and see if it's right.
  readScrambled($ih, \$buff, 8);
  if (! defined $buff)
  {
    print "'$item' is truncated at the magic number field.\n";
    close($ih);
    return 0;
  }
  if ($buff ne "IFSQ\1\0\0\0")
  {
    print "'$item' has the wrong magic number ['$buff'].\n";
    close($ih);
    return 0;
  }

  # Next four bytes are the length of the original file.
  $lengthOfCapturedFile = readU32($ih);
  if (! defined $lengthOfCapturedFile)
  {
    print "'$item' looked correct, but is truncated at the length field.\n";
    close($ih);
    return 0;
  }

  # This is the number of attributes which follow the file data.
  $numOfAttrs = readU32($ih);

  if ($restoreSample)
  {
    my $numRead;
    
    # Inhale the original file content.
    $numRead = readScrambled($ih, \$buff, $lengthOfCapturedFile);

    # Did we get enough content?
    if ($numRead != $lengthOfCapturedFile)
    {
      print "Couldn't read all of original sample file, you loose.\n";
      close($ih);
      return 0;
    }

    # If we are _not_ using the original name of the sample to restore
    # into (grammar alert!), then cook the name up now whilst we remember.
    (! $useRealName) && ($fileNameToEmitTo = $item . ".content");
  }
  else # (! $restoreSample)
  {
    print "\nSample $item:\n";

    # Now move over the original content of the file.
    if (! seek($ih, $lengthOfCapturedFile, 1))
    {
      print "Failed to seek within '$item'.\n";
      close($ih);
      return 0;
    }
  }

  # Just before processing the attributes, make a note of the
  # file position.
  $startOfAttrFilePos = tell($ih);

  # Reset the number found counter.
  $numOfAttrsFound = 0;

  # Now walk through the attributes.
  INNER: while (($attrName = readAttributeName($ih)) && ($numOfAttrsFound < $numOfAttrs))
  {
    my $whatsThis;
    my $attrType;
    my $attrLength;
    my $attrVal;

    # Really don't know what this is yet.
    readScrambled($ih, \$whatsThis, 1);
    if (! defined $whatsThis)
    {
      print " '$item' truncated in whatsThis field.\n";
      close($ih);
      return 0;
    }

    $attrLength = readU32($ih);
    if (! defined $attrLength)
    {
      print " '$item' truncated in attrLength field.\n";
      close($ih);
      return 0;
    }

    $attrType = readU32($ih);
    if (! defined $attrType)
    {
      print " '$item' truncated in attrType field.\n";
      close($ih);
      return 0;
    }

    readScrambled($ih, \$attrVal, $attrLength);
    if (! defined $attrVal)
    {
      print " '$item' truncated in attrVal field.\n";
      close($ih);
      return 0;
    }

    # Strip trailing nulls.
    $attrName =~ s/\0+$//;

    if ($restoreSample)
    {
      (($useRealName) && ($attrName =~ /^X-Sample-File$/)) && ($fileNameToEmitTo = $attrVal);
    }
    else
    {
      printf("   $attrName: " . getPrintableAttrVal($attrType,$attrVal) . "\n");
    }

    # We found another attribute.
    $numOfAttrsFound++;
  }

  ($numOfAttrsFound != $numOfAttrs) && (print "[Only found $numOfAttrsFound attributes (expected $numOfAttrs)]\n");

  # Done with the file.
  close($ih);

  # Are we resoring the sample?
  if ($restoreSample)
  {
    my $oh;

    if (! defined $fileNameToEmitTo)
    {
      print "Unable to file X-Sample-File: attribute, making up output name.\n";
      $fileNameToEmitTo = $item . ".content";
    }

    # Ensure that the output name doesn't have a directory component.
    $fileNameToEmitTo =~ s/^.*[\\\/]([^\\\/]+)$/$1/g;

    # Get a new filehandle.
    $oh = IO::Handle->new();
    if (! defined $oh)
    {
      print "Couldn't get filehandle, you loose.\n";
      return 0;
    }

    if (! open($oh, ">$fileNameToEmitTo"))
    {
      print "Couldn't open output file '$fileNameToEmitTo', you loose.\n";
      return 0;
    }

    # Remember to put it into binary mode.
    binmode($oh);

    # Emit the descrambled file into the output file.
    print $oh $buff;

    # Close the output file.
    close($oh);

    print "$item -> $fileNameToEmitTo\n";
  }

  return 1;
}

# ----- START HERE -----

my $item;
my $dh;
my $didOne;

# Initialise these to 'no'.
$restoreSample = 0;
$useRealName   = 0;

# Process the command-line options.
if (! GetOptions("--release!", \$restoreSample, "--realname!", \$useRealName))
{
  exit 1;
}

# Check user sanity.
if (($useRealName) && (! $restoreSample))
{
  print "ERROR: '--realname' is only relevant with '--release'.\n";
  exit 1;
}

$dh = IO::Handle->new();
if (! defined $dh)
{
  print "Couldn't get directory handle, you loose.\n";
  exit 1;
}

$didOne = 0;
foreach $item (@ARGV)
{
  $didOne = 1;
  # Normalise.
  $item =~ s/\/+$//;
  $item =~ s/[\r\n\s]+$//;

  if (-f $item)
  {
    processOneFile($item);
  }
  elsif (-d $item)
  {
    my @gulp;

    # Crack open the directory listing.
    if (! opendir($dh, $item))
    {
      print "Can't open directory '$item' -- skipping.\n";
      next;
    }
    # Swallow everything, and close it again.
    @gulp = readdir($dh);
    closedir($dh);

    foreach (grep { ! /^(\.|\.\.)$/ } @gulp)
    {
      my $name = $item . "/" . $_;
      (-f $name) && processOneFile($item . "/" . $_);
    }
  }
  else
  {
    print "'$item' is not a file or a directory -- crisis!\n";
  }
}

if (! $didOne)
{
  print <<EOM;
USAGE: qinfo.pl [--release] <file|dir> [<file|dir> ...]

  --release  : Extracts the original file from the freezedried
               sample, and places it into a new file, which will
               be called '<freezedried file>.content', unless
               you also specify...
  --realname : Used in conjunction with --release, and causes
               the extracted file to have the name which it
               had before it was originally captured.
EOM

 exit 1;
}

# JD.
exit 0;
