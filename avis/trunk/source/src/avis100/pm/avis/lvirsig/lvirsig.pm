####################################################################
#                                                                  #
# Program name:  lvirsig.pm                                        #
#                                                                  #
# Package name:  AVIS::lvirsig                                     #
#                                                                  #
# Description:   This package manipulates signature files          #
#                                                                  #
#                                                                  #
# Statement:     Licensed Materials - Property of IBM              #
#                (c) Copyright IBM Corp. 1998-1999                 #
#                                                                  #
# Author:                                                          #
#                                                                  #
#                U.S. Government Users Restricted Rights - use,    #
#                duplication or disclosure restricted by GSA ADP   #
#                Schedule Contract with IBM Corp.                  #
#                                                                  #
# Change history:                                                  #
#                                                                  #
####################################################################
#                                                                  #
# Function:                                                        #
# It uses OO Perl to create a database in memory reflecting the    #
# signatures of an 'lvirsig.lst' type signature file               #
#                                                                  #
# Apart from the lines of the signature file mapped to memory, the #
# signature database also contains various arrays and hashes with  #
# infos on the virus names and links (vervcodes) These internal    #
# variables are maintained to allow the SigVerv module to perform a#
# consistency check when addendas are appended to the master sig   #
# and verv files.                                                  #
#                                                                  #
# The methods for internal use only have a leading underscore '_'  #
#                                                                  #
# The object maintains a 'dirty' bit to speed unmapping of databases #
# that were not modified since their mapping. Should you modify      #
# internal object fields manually, you have the possibility (and the #
# responsibility) to call explicitly the Flush method, because the   #
# 'dirty' bit won't be updated in this case.                         #
#                                                                  #
####################################################################

package AVIS:lvirsig;
require Exporter;
@ISA = qw(Exporter);
@EXPORT = qw(Map Unmap SetAllNames GetLinks AddLink new Discard
GetAttributes SetAttributes Append SetAllLinks Flush SelectFirst SelectNext GetSig
EraseAllLinks GetName Nameslist);
@EXPORT_OK = qw(DebugDump);

use AVIScore::SigVerv; # Need it for formatting/checking links
use AVIScore::Verv; # Need it for link ordering
use strict;

my $verbose = 0;

1; # OK to load the package anyway, no ressource needed

# This is the map function to be used as a constructor
# It maps a file from disk to memory and returns a signature database object
sub Map {
  my ($obj,$sigfile) = @_;
  my $self = bless {};

  warn(" *** $0: Signature File does not exist!\n"), return 0 unless -f $sigfile;
  $self->{filename} = $sigfile;
  open(SIG, $sigfile) or warn(" *** $0: Unable to open Signature File!\n"), return 0;
  @{$self->{lines}} = <SIG>;
  close(SIG);
  $self->_InitInternals;
  $self->{dirty} = 0;
  return $self;
}

sub new {
  my ($obj,@lines) = @_;
  my $self = bless {};
  @{$self->{lines}} = @lines;
  $self->_InitInternals;
  $self->{filename} = "N/A";
  $self->{dirty} = 0;
  return $self;
}

sub _InitInternals {
  my $self = shift;
  my $state = 2;
  my ($l,$sigl,$namel,$attrl);
  my ($name,$bol,$eol,$before,$links,$after);
  my (@links,$link);
  $self->{entries} = [];
  $self->{selentry} = -1;
  $self->{numrefs} = {};
  undef $self->{fuzzymatch};
  undef $self->{selectname};
  for($l=0 ; $l<@{$self->{lines}} ; $l++) {
    $_ = $self->{lines}[$l];
    # Skip blank or comment lines
    next if (!/\S/ || /^\s*\*/);
    $state++;
    $state = 0 if $state == 3;
    if ($state == 0) {
      # This line is supposed to contain a pattern
      $sigl = $l;
      warn " *** $0: Signature is strange at line $l [after $name]!\n" unless /^\s*?[0-9A-F\s?%]+/i;
    }
    elsif ($state == 1) {
      # This line is supposed to contain a virus name and possibly some vervcodes
      $namel = $l;
      if (/</) {
        # There are some links, split to (beginning of line, end of line)
        ($bol,$eol) = /(.*?)<(.*)/;
        warn " *** $0: Missing angle bracket at line $l [after $name]\n" unless $eol =~ />/;
      }
      else {
        # There are no links
        $bol = $_;
        $eol = "";
      }
      # Remove any leading %s and spaces from the beginning of line
      $bol =~ s/^\s*?%s\s*//;
      if ($bol =~ /%s/) {
        # There is a trailing %s
        ($name,$before) = $bol =~ /(.*?)\s*%s(.*)/;
      }
      else {
        # No trailing %s
        ($name) = $bol =~ /(.*?)\s*$/;
        $before = "";
      }
      ($links,$after) = $eol =~ /(.*?)>(.*)/;
      @links = split /\s+/, uc $links;
    }
    elsif ($state == 2) {
      # This line is supposed to contain attributes
      # This is the last line of the entry
      $attrl = $l;
      # Add one entry
      push @{$self->{entries}},
	{"sigl"   => $sigl,
	 "namel"  => $namel,
	 "attrl"  => $attrl,
	 "name"   => $name,
	 "before" => $before,
	 "after"  => $after,
	 "links"  => [ @links ]};
      # Counts the number of refs to the links
      foreach $link (@links) {
	$self->{numrefs}{$link}++;
      }
    }
  } # end for $l
  warn " *** $0: Bad format!" unless $state == 2;
}

# This method displays the names and links of the signature db and counts various things
sub DebugDump {
  my ($self,$verbose) = @_;
  my $numentries = scalar @{$self->{entries}};

  if ($verbose eq "verbose") {
    my ($e,$link);
    print "The signature database contains the following names:\n";
    foreach $e (@{$self->{entries}}) {
      print $e->{name}, " at line ", $e->{namel}, "\n";
    }
    print "\nThe signature database contains the following links:\n";
    foreach $link (sort keys %{$self->{numrefs}}) {
      print "$link (", $self->{numrefs}{$link}, " references)\n";
    }
  } # end if verbose

  print "Total number of signatures: $numentries\n" if $verbose;
  return $numentries;
}

# This method flushes the database from memory to disk
# returns true (1) for success, false (0) otherwise
# IMPORTANT:
# Use it if you want to manipulate the object database internals without going through the
# methods, because then the 'dirty' bit of the object will not reflect changes that you
# have made manually, and thus may cause a loss of data
# A better solution is to always use the methods, of course...
sub Flush {
  my $self = shift;
  return 1 if $self->{filename} eq "N/A";

  my $sigtmpfilename = $self->{filename}.".tmp";
  rename $self->{filename}, $sigtmpfilename or return 0;
  unless (open(NEWSIG, ">".$self->{filename})) {
    rename $sigtmpfilename, $self->{filename};
    return 0;
    }
  foreach (@{$self->{lines}}) {
    print NEWSIG $_;
  }
  close(NEWSIG);
  unlink $sigtmpfilename;
  $self->{dirty} = 0;
  return 1;
}

# Object method to unmap the signature database back to the original file on disk
# Simply flushes the db
sub Unmap {
  my $self = shift;
  return ($self->{dirty})?($self->Flush):1;
}

# Destructor
# This is useful in case someone forgets to unmap an important file
# Simply flushes the db
sub DESTROY {
  my $self = shift;
  print "[destructor invoked by perl for object $self]\n" if $verbose;
  $self->Flush if $self->{dirty};
}

sub Discard {
  my $self = shift;
  $self->{dirty} = 0;
  return 1;
}

# This method substitutes the name given in argument to the ones in all the signature file entries
# To be used with addenda files that contain only signatures for a single virus
sub SetAllNames {
  my ($self,$signame) = @_;
  my $e;
  warn " *** $0: Invalid virus name!\n" unless $signame;
  foreach $e (@{$self->{entries}}) {
    $self->{lines}[$e->{namel}] =
	"%s ".$signame." %s".$e->{before}." <".(join " ", @{$e->{links}}).">".$e->{after}."\n";
    $e->{name} = $signame;
  }
  $self->{dirty} = 1;
}

# This method sets all the links (the codes for verv) to the string given in argument
# This is to be used with an addenda file containing entries for a single virus
# (obsolete)
sub SetAllLinks {
  my ($self,$link) = @_;
  $link = SigVerv::FormatLink($link);
  warn " *** $0: Invalid link!\n" unless $link;
  my $e;
  foreach $e (@{$self->{entries}}) {
    $self->{lines}[$e->{namel}] =
	"%s ".$e->{name}." %s".$e->{before}." <$link>".$e->{after}."\n";
    $e->{links} = [ $link ];
  }
  $self->{numrefs} = { $link => scalar(@{$self->{entries}}) };
  $self->{dirty} = 1;
}

sub _quoted {
  my $string = shift;
  $string =~ s/(\(|\)|\+|\*|\||\\|\/|\?|\.)/\\$1/g;
  return $string;
}

# Internal use only
# Core of the SelectFirst/Next work
# The $n parameter is the entry from which to start searching
sub _Select {
  my ($self,$signame,$n) = @_;
  my $e;
  my $safename = $signame;
  if (	$self->{fuzzymatch} eq "substring"	||
	$self->{fuzzymatch} eq "word"		||
	$self->{fuzzymatch} eq "caseinsensitive"
     ) {
    $safename = _quoted($signame);
  }

  if ($self->{fuzzymatch} eq "substring") { # The substring case
    for ( ; $n<@{$self->{entries}} ; $n++) {
      $self->{selentry} = $n;
      $e = $self->{entries}[$n];
      return $e->{namel} if ($e->{name} =~ /$safename/i);
    }
    return 0;
  }
  elsif ($self->{fuzzymatch} eq "word") { # The word case
    for ( ; $n<@{$self->{entries}} ; $n++) {
      $self->{selentry} = $n;
      $e = $self->{entries}[$n];
      return $e->{namel} if ($e->{name} =~ /(^|\W)$safename(\W|$)/i);
    }
    return 0;
  }
#  elsif ($self->{fuzzymatch} eq "caseinsensitive") { # The case-insensitive case
#    for ( ; $n<@{$self->{entries}} ; $n++) {
#      $self->{selentry} = $n;
#      $e = $self->{entries}[$n];
#      return $e->{namel} if ($e->{name} =~ /^$safename$/i);
#    }
#    return 0;
#  }
  else { # The stricter case or the pattern case
    for ( ; $n<@{$self->{entries}} ; $n++) {
      $self->{selentry} = $n;
      $e = $self->{entries}[$n];
      return $e->{namel} if ($e->{name} eq $safename);
    }
    return 0;
  }
}

# Selects an entry by name, starting at the beginning of the signature database
# Returns the line number of the match on success (which happens to be true)
# and false (0) otherwise
sub SelectFirst {
  my ($self,$signame,$fuzzy) = @_;
  warn " *** $0: Invalid virus name!\n" unless $signame;
  $self->{fuzzymatch} = $fuzzy;
  $self->{selectname} = $signame;

  return $self->_Select($signame,0);
}

# Selects an entry by name, starting at the last entry found by SelectFirst/SelectNext
sub SelectNext {
  my ($self,$signame) = @_;
  my $n = $self->{selentry}+1;
  $signame = $self->{selectname} unless $signame;
  warn " *** $0: Invalid virus name!\n" unless $signame;

  return $self->_Select($signame,$n);
}

sub _CheckEntrySelected {
  my $self = shift;
  if ($self->{selentry} < 0 || $self->{selentry} >= @{$self->{entries}} ) {
    warn " *** $0: No entry selected!\n";
    return 0;
  }
  else {
    return 1;
  }
}

# Returns the links to verv in one line
# (obsolete)
sub GetLinks {
  my $self = shift;
  if ($self->_CheckEntrySelected) {
    return join " ", @{$self->{entries}[$self->{selentry}]{links}};
  }
}

# Adds a single link to the list of links for the selected signature entry
# Takes a verv db as an optional parameter
# If the parameter is present, the method performs vervcodes reordering (for subset/superset macro problem)
sub AddLink {
  my ($self,$link,$vrvdb,$needed_R,$needed_U,$ref_oldline) = @_;
  my ($list);
  my ($threat,$uncache);
  return () unless $self->_CheckEntrySelected;
  my $e = $self->{entries}[$self->{selentry}];
  if (defined($vrvdb) && ref($vrvdb) ne "Verv") {
    warn " *** $0: Bad parameter, AddLink expects Verv object!\n";
    return ();
  }
  else {
    $link = SigVerv::FormatLink($link);
    if (SigVerv::CheckLink($link)) {

      my $lastlink = pop @{$e->{links}};
      if ($lastlink eq "FAILM") {
	push @{$e->{links}}, ($link, "FAILM");
      }
      else {
	push @{$e->{links}}, $lastlink if $lastlink;
	push @{$e->{links}}, $link;
      }

      $self->{numrefs}{$link}++;
      $list = join " ", @{$e->{links}};
      # Do link reordering if needed
      if (defined($vrvdb) && @{$e->{links}} > 1) {
	($$needed_R, $$needed_U) = ($threat,$uncache) = $vrvdb->CheckOrder($list);
	if ($threat) {
	  print "[Threat detected, reordering codes]\n";
	  $list = $vrvdb->Reorder($list);
	}
	if ($uncache) {
	  print "[Emptying cache]\n";
	  unless ($list =~ /FAILM$/i) {
	    $list .= " FAILM";
	    push @{$e->{links}}, "FAILM";
	    $self->{numrefs}{FAILM}++;
	  }
	}
	if ($threat || $uncache) {
	  $e->{links} = [ split(/\s+/,$list) ];
	}
      }
      # Keep the old line and update to the new one
      $$ref_oldline = $self->{lines}[$e->{namel}] if ref($ref_oldline) eq "SCALAR";
      $self->{lines}[$e->{namel}] =
	"%s ".$e->{name}." %s".$e->{before}." <".$list.">".$e->{after}."\n";
      $self->{dirty} = 1;

      my ($delta_start, $delta_end) = ($e->{sigl}, $e->{attrl});
      while ($delta_start > 0 && $self->{lines}[$delta_start-1] =~ /^\s*\*/)
	{ $delta_start-- }
      return @{$self->{lines}}[$delta_start .. $delta_end];
    }
    else {
      # Link has bad format
      warn " *** $0: Bad link!\n";
      return ();
    }
  }
}

# This method returns the attributes for the signature in the selected entry
# (such as "Scan memory", "Pause if found", "At Offset ...", ....)
# AN ENTRY SHOULD BE SELECTED PRIOR TO A CALL TO THIS METHOD
sub GetAttributes {
  my $self = shift;
  if ($self->_CheckEntrySelected) {
    return $self->{lines}[$self->{entries}[$self->{selentry}]{attrl}];
  }
}

# This method sets the attributes for the signature in the selected entry
# AN ENTRY SHOULD BE SELECTED PRIOR TO A CALL TO THIS METHOD
sub SetAttributes {
  my ($self,$newattribs) = @_;
  warn " *** $0: Empty attribute line!\n" unless $newattribs;
  if ($self->_CheckEntrySelected) {
    $newattribs .= ($newattribs =~ /\n$/)?"":"\n";
    $self->{lines}[$self->{entries}[$self->{selentry}]{attrl}] = $newattribs;
    $self->{dirty} = 1;
  }
}

# This method appends an addenda signature database to the main one
# It updates the lines of the db in memory and also all the internal variables
sub Append {
  my ($self,$addenda) = @_;
  my ($e,$link);
  my $lineshift = @{$self->{lines}};
  if (ref($addenda) ne "Sig") {
    warn " *** $0: Bad addenda database!\n";
    return ();
  }
  else {
    push @{$self->{lines}}, @{$addenda->{lines}};
    foreach $e (@{$addenda->{entries}}) {
      push @{$self->{entries}},
	{ "sigl"   => $e->{sigl} +$lineshift,
	  "namel"  => $e->{namel}+$lineshift,
	  "attrl"  => $e->{attrl}+$lineshift,
	  "name"   => $e->{name},
	  "before" => $e->{before},
	  "after"  => $e->{after},
	  "links"  => [@{$e->{links}}]
	};
      foreach $link (@{$e->{links}}) {
	$self->{numrefs}{$link}++;
      }
    }
    $self->{dirty} = 1;
    return @{$addenda->{lines}};
  }
}

# Returns the signature for the selected entry
sub GetSig {
  my $self = shift;
  if ($self->_CheckEntrySelected) {
    return $self->{lines}[$self->{entries}[$self->{selentry}]{sigl}];
  }
}

# Returns the virus name for the selected entry
sub GetName {
  my $self = shift;
  if ($self->_CheckEntrySelected) {
    return $self->{entries}[$self->{selentry}]{name};
  }
}

# This methods suppresses all the links (vervcodes) in the signature db
# to be used on an addenda file containing entries for a single virus when
# we determine that the verification entry the IS came up with is not good
sub EraseAllLinks {
  my $self = shift;
  my $e;
  foreach $e (@{$self->{entries}}) {
    $self->{lines}[$e->{namel}] =
	"%s ".$e->{name}." %s".$e->{before}.$e->{after}."\n";
    $e->{links} = [];
  }
  # Now update internal vars
  $self->{numrefs} = {};
  $self->{dirty} = 1;
}

# Returns the list of all names used in the signature file
sub Nameslist {
  my $self = shift;
  return map $_->{name}, @{$self->{entries}};
}
