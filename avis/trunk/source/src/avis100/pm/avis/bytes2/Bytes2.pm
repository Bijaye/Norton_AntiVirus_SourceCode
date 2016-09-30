####################################################################
#                                                                  #
# Program name:  pm/AVIS/Bytes2/Bytes2.pm                          #
#                                                                  #
# Package name:  AVIS/Bytes2                                       #
#                                                                  #
# Description:   Manipulation of Bytes2 files                      #
#                                                                  #
# Statement:     Licensed Materials - Property of IBM              #
#                (c) 1998                                          #
#                                                                  #
# Author:        Morton Swimmer                                    #
#                                                                  #
#                U.S. Government Users Restricted Rights - use,    #
#                duplication or disclosure restricted by GSA ADP   #
#                Schedule Contract with IBM Corp.                  #
#                                                                  #
#                                                                  #
####################################################################
#                                                                  #
# Function:                                                        #
#                                                                  #
# This package handles bytes2 files It offers methods to           # 
# memory-map, manipulate and unmap such files.                     #
#                                                                  #
#                                                                  #
####################################################################
#
#
# NB: I always quote the "crypt" and "length" hash keys because they
# clash with predefined Perl functions

package AVIS::Bytes2;
require Exporter;
@ISA = qw(Exporter);
@EXPORT = qw(map unmap hexstr mergecloseblocks count);
@EXPORT_OK = @EXPORT;

use strict;

1; # OK to load module

# This methods maps the bytes2 to a structure in memory
sub map {
  my ($obj,$file,$verbose) = @_;
  my $self = bless {};
  my $b = 0; # This is the index of the current block
  print "Mapping $file to memory...\n" if $verbose;
  open(F,$file) or warn " *** $0: Unable to open bytes2 file!";
  $self->{filename} = $file;
  while (<F>) {
    next unless /\S/; # Skip blank lines
    if (/<VIRUS>/i) {
      $self->{virus} = $_;
    }
    elsif (/<DIRECTORY>/i) {
      $self->{directory} = $_;
    }
    elsif (/<MARKER>\s*(\S+)/i) {
      $self->{blocks}[$b]{marker} = uc $1;
    }
    elsif (/<OFFSET>\s*([\+|\-]?\d+)/i) {
      $self->{blocks}[$b]{offset} = $1;
    }
    elsif (/<LENGTH>\s*(\d+)/i) {
      $self->{blocks}[$b]{"length"} = $1;
    }
    elsif (/<CRYPT>\s*(\S+)/i) {
      $self->{blocks}[$b]{"crypt"} = uc $1;
    }
    elsif (/<CODE>\s*(\S+)/i) {
      $self->{blocks}[$b]{code} = uc $1;
    }
    elsif (/<BYTES>\s*([\da-fA-F]+)/i) {
      $self->{blocks}[$b]{bytes} = uc $1;
      $b++; # Start next block
    }
  }
  close(F);
  # All fields of all blocks should have values
  foreach (@{$self->{blocks}}) {
    warn " *** $0: Undefined marker value in bytes2 block!" 
      unless defined $_->{marker};
    warn " *** $0: Undefined offset value in bytes2 block!" 
      unless defined $_->{offset};
    warn " *** $0: Undefined length value in bytes2 block!" 
      unless defined $_->{"length"};
    warn " *** $0: Undefined crypt value in bytes2 block!"  
      unless defined $_->{"crypt"};
    warn " *** $0: Undefined code value in bytes2 block!"   
      unless defined $_->{code};
    warn " *** $0: Undefined bytes value in bytes2 block!"  
      unless defined $_->{bytes};
  }
  print "found ", scalar @{$self->{blocks}}, " bytes2 blocks\n" if $verbose;
  return $self;
}

# This method unmaps the bytes2 file. It is important to call unmap
# explicitly since the object has no destructor returns true on
# success
sub unmap {
  my $self = shift;
  my $tmp = $self->{filename}.".tmpb2";
  rename $self->{filename}, $tmp;
  unless (open(F,">".$self->{filename})) {
    warn " *** $0: Unable to unmap bytes2 file!";
    rename $tmp, $self->{filename};
    return 0;
  }
  print F $self->{virus};
  print F $self->{directory};
  foreach (@{$self->{blocks}}) {
    print F "<MARKER> ", $_->{marker}  , "\n";
    print F "<OFFSET> ", $_->{offset}  , "\n";
    print F "<LENGTH> ", $_->{"length"}, "\n";
    print F "<CRYPT> " , $_->{"crypt"} , "\n";
    print F "<CODE> "  , $_->{code}    , "\n";
    print F "<BYTES> " , $_->{bytes}   , "\n\n";
  }
  close(F);
  unlink $tmp;
  return 1;
}

# Returns an array containing the bytes in the successive bytes2
# blocks separated by nulls
sub hexstr {
  my $self = shift;
  my $hex;
  foreach (@{$self->{blocks}}) {
    $hex .= pack("H*", $_->{bytes})."\0";
  }
  return $hex;
}

# Merges successive blocks distant of less than the sig length and
# having the same marker, crypt and code attributes
sub mergecloseblocks {
  my ($self,$siglen,$verbose) = @_;
  my ($b,$padding);
  my $bs = $self->{blocks}; # Just a shorcut for clarity (well, a bit
                            # more clarity...)
  for ($b=1 ; $b<@$bs ; $b++) {
    $padding = $$bs[$b]->{offset} - ($$bs[$b-1]->{offset}+$$bs[$b-1]->{"length"});
    if ($$bs[$b-1]->{marker}  eq $$bs[$b]->{marker}  &&
	$$bs[$b-1]->{"crypt"} eq $$bs[$b]->{"crypt"} &&
	$$bs[$b-1]->{code}    eq $$bs[$b]->{code}    &&
	$padding < $siglen &&
	$$bs[$b-1]->{"length"} > 1) { # One byte only won't do for a
                                      # sig start :( 
      # Correct the length
      $$bs[$b-1]->{"length"} += $padding + $$bs[$b]->{"length"};
      # Append padding and block bytes
      $$bs[$b-1]->{bytes} .= ("??" x $padding) . $$bs[$b]->{bytes};
      # Erase the copied block
      splice @$bs, $b, 1;
      print "Merging blocks ", $b-1, " and ", $b, " (with ", $padding, " bytes padding)\n" if $verbose;
      # Don't forget to keep the same $b !
      $b--;
    }
  }
}

# Counts the bytes corresponding to the given properties.  Properties
# are passed through a hash whose keys are supposed to be marker,
# crypt and/or code.
sub count {
  my ($self,$props) = @_;
  my $count = 0;
  # Make the keys case-insensitive (Yes, I modify the hash. No, I
  # won't test for all case-sensitive letter combinations of the words
  # marker, crypt and code...)
  foreach (keys %{$props}) { $$props{+lc} = $$props{$_} } # Don't
                                                          # forget the
                                                          # "+", we
                                                          # want the
                                                          # function!
  # Now walk the blocks chain and see which ones fit
  foreach (@{$self->{blocks}}) {
    my $markerfits = (!defined $$props{marker}  or $_->{marker}  =~ /$$props{marker}/i);
    my $cryptfits  = (!defined $$props{"crypt"} or $_->{"crypt"} =~ /$$props{"crypt"}/i);
    my $codefits   = (!defined $$props{code}    or $_->{code}    =~ /$$props{code}/i);
    $count += $_->{"length"} if ($markerfits and $cryptfits and $codefits);
  }
  return $count;
}










