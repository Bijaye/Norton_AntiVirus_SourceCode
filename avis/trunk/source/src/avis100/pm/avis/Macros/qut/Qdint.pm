#
# Change history:
#  1999/01/22 - NOP scaffolding version
#  1999/01/26 - Added undoIntegration() method
#  1999/01/27 - Made doIntegration() real
#  1999/02/05 - Add handling of <VirusIDLabel>
#  1999/02/08 - Force "QA Status" to PASSED, for build
#  1999/02/09 - Use FSTools for Canonize()
#  1999/02/26 - Make the save directory if it's not there
#

package AVIS::Macro::Qdint;

#
# A small piece of OO-Perl that knows how to do a quick-and-dirty
# integration of a def-delta file into a NAV local build tree.
#
# Class Methods:
#
#    $whatever = new AVIS::Macro::Qdint($deltaFile,$navroot);
#
#      Returns a Qdint object, ready to do a quick integration of the
#      deltas in $deltaFile, into the NAV local build with the given
#      root.
#
# Instance Methods:
#
#    $success = $whatever->doIntegration();
#
#      Actually does the above integration, and returns true if it
#      worked, and false otherwise.
#
#    $success = $whatever->undoIntegration($changed_files);
#
#      Undoes the effects of the last doIntegration(), using the filenames
#      in the array referenced by $changed_files (if any) as a hint.
#      Note that do/undo's may *not* be nested.  Not if you want it all
#      to work right, anyway.
#
#    $logstring = $whatever->logString();
#
#      Returns a human-readable string to be logged, containing any
#      interesting information about the results of the last doIntegration()
#      or undoIntegration().  The string may contain embedded newlines.
#
#    $changed_files = $whatever->changedFiles();
#
#      Returns a reference to an array of names of files that were
#      changed by the last doIntegration().
#
#    $VirusID = $whatever->virusID();
#
#      Returns the four-hex-digit ID that was used for the new
#      definition.  (This may eventually have to be generalized to
#      a list of IDs, or something like that.)
#
#    $VirusID = $whatever->virusName();
#
#      Returns the string that was used for the virus name in the new
#      definition.  (This may eventually have to be generalized also.)
#

use AVIS::FSTools();           # Random annoying utilities
use File::Copy();              # File copying.  Hey!
use File::Path();
use Data::Dumper();

#
# The usual oo-perl new() routine
#
sub new {
  my $class = shift;      # the class to bless into
  my $dname = shift;      # the filename with the delta to apply
  my $nroot = shift;      # the root of the nav local build
  my $self = {};          # the anonymous hash to return
  bless $self, $class;
# Simple instance variables
  $self->{logString} = "No integration has been done.";
  $self->{changedFiles} = "";
  $self->{virusID} = "6488";                # STUB!
  $self->{virusName} = "Test_Virus";        # STUB!
# The "private" instance variables
  $self->{_deltaFile} = $dname;
  $self->{_navroot} = $nroot;
  $self->{_savedir} = AVIS::FSTools::Canonize("qutsave",$nroot);
# and we're done
  return $self;
}

#
# The main routine that actually applies the deltas
#
sub doIntegration {
  my $self = shift;     # Who we are
  my $ddata;            # Data from the delta file
  my $dname = $self->{_deltaFile};
  my $nroot = $self->{_navroot};
  my ($deltaset, $thisdelta, $thisfile, $thatfile, $thisjunk, $badness);
  my $savedir = $self->{_savedir};
  my ($fname, $success);
  local *FH;

  $self->{logString} = "Qdint->doIntegration terminated early.";
  $self->{changedFiles} = [];

  # Make sure the save directory exists
  if (not -d $savedir) {
    eval { File::Path::mkpath($savedir); };
    if ($@) {
      $self->{logString} = "Unable to make $savedir: $@";
      return 0;
    }
  }

  # Clean it out
  my $target = AVIS::FSTools::Canonize("*",$savedir);
  my @debris = <"$target">;   # These quotes are important!!
  if ($debris[0] ne $target) {
    foreach $thisjunk ( @debris ) {
      unlink $thisjunk;
    }
  }

  # Read in the delta file
  unless (open FH,"<$dname") {
    $self->{logString} = "Failed to open $dname.";
    return 0;
  }
  $ddata = join "",<FH>;
  close FH;

  # Get the list of additions and files to add them to
  $ddata = $self->_substituteTags($ddata);
  $deltaset = $self->_makeDeltaSet($ddata);
  unless ($deltaset) {return 0;}

  # Record, and make a backup copy of, each file to be touched
  $fno = 1;
  %maphash = ();
  foreach $thisfile ( keys %$deltaset ) {
    push @{$self->{changedFiles}}, $thisfile;
    $thatfile = AVIS::FSTools::Canonize($fno,$savedir);
    unless (File::Copy::copy($thisfile,$thatfile)) {
      $badness = $!;
      chomp $badness;
      $self->{logString} = "Error ($badness) copying ($thisfile) to ($thatfile).";
      return 0;
    }
    $maphash{$thatfile} = $thisfile;
    $fno++;
  }

  # Use Data::Dumper to save the hash of name mappings
  $fname = AVIS::FSTools::Canonize("index",$savedir);
  open FH, ">$fname" or
    $self->{logString} = "Error opening <$fname>: $!.",
    return 0;
  $Data::Dumper::Useqq = 1;
  $Data::Dumper::Indent = 0;
  print FH Data::Dumper::Dumper(\%maphash);
  close FH;

  # Now actually apply the deltas
  foreach $thisfile ( keys %$deltaset ) {
    $thisdelta = $deltaset->{$thisfile};
    $success = $self->_insertString($thisfile,$thisdelta);
    unless ($success) {return 0;}
  }

  $self->{logString} = "Qdint->doIntegration succeeded.";
  return 1;
}

#
# The naim routine that undoes the above
#
sub undoIntegration {
  my $self = shift;     # Who we are
  my $changed_files = shift;   # Ignored at present
  my ($fname, $thisfile, $thatfile, $badness, $data);
  my $savedir = $self->{_savedir};
  local *FH;

  $fname = AVIS::FSTools::Canonize("index",$savedir);
  open FH, "<$fname" or
    $self->{logString} = "Error opening <$fname>: $!.",
    return 0;
  $data = <FH>;
  close FH;
  eval $data;

  foreach $thatfile ( keys %$VAR1 ) {
    $thisfile = $VAR1->{$thatfile};
    unless (File::Copy::copy($thatfile,$thisfile)) {
      my $badness = $!;
      chomp $badness;
      $self->{logString} = "Error ($badness) copying ($thatfile) to ($thisfile).";
      return 0;
    }
  }

  # Clean out the save directory
  my $target = AVIS::FSTools::Canonize("*",$savedir);
  my @debris = <"$target">;   # These quotes are important!!
  if ($debris[0] ne $target) {
    foreach $thisjunk ( @debris ) {
      unlink $thisjunk;
    }
  }

  $self->{logString} = "Qdint undoIntegration worked.";
  return 1;
}

#
# Insert the string into the file using whatever method we like today
#
sub _insertString {
  my $self = shift;
  my $fname = shift;
  my $string = shift;

  return $self->_insertString_at_end($fname,$string);

}


#
# Insert the string into the file at the end
# You'd think we could just open it as >>$fname, but
# then we end up inserting the string *after the EOF mark*,
# which is futile.
#
sub _insertString_at_end {
  my $self = shift;
  my $fname = shift;
  my $string = shift;
  my $data;
  local *FH;

  unless (open FH,"<$fname") {
    $self->{logString} = "Unable to read $fname.";
    return 0;
  }
  $data = join "",<FH>;
  close FH;

  unless (open FH,">$fname") {
    $self->{logString} = "Unable to write $fname.";
    return 0;
  }
  print FH $data . $string;
  close FH;

  return 1;


}


#
# Insert the string into the file at the insertion point marked
# by the hardcoded funny string
#
sub _insertString_at_marked_point {
  my $self = shift;
  my $fname = shift;
  my $string = shift;
  my $data;
  local *FH;

  unless (open FH,"<$fname") {
    $self->{logString} = "Unable to read $fname.";
    return 0;
  }
  $data = join "",<FH>;
  close FH;

  unless ($data =~ /<QDINT INSERT HERE>/) {
    $self->{logString} = "$fname has no insertion point for Qdint.";
    return 0;
  }
  $data =~ s/<QDINT INSERT HERE>/$string/o;
  unless (open FH,">$fname") {
    $self->{logString} = "Unable to write $fname.";
    return 0;
  }
  print FH $data;
  close FH;

  return 1;

}

#
# Do whatever <tag> substitution and other hacks seem called for.
#
sub _substituteTags {
  my $self = shift;
  my $data = shift;
  my $id = $self->{virusID};
  my $vn = $self->{virusName};

  $data =~ s/\<VirusName\>/$vn/go;
  $data =~ s/\<VirusID\>/0x$id/go;
  $data =~ s/\<VirusIDLabel\>/VID_6488/go;
  $data =~ s/\<GUID\>/XX6488XX/go;
  $data =~ s/(\[QAStatus\]\s*).*$/\1PASSED/gom;

  return $data;
}

#
# Given a delta-file, return a hash keyed by filename, with the
# value being the text to add to that file.
#
sub _makeDeltaSet {
  my $self = shift;
  my $data = shift;
  my $nroot = $self->{_navroot};
  my $macrostuff;
  my $addfile = 0;

  # Find the MACRO section, which is all we test for now
  $data =~ /\[MACROSTART\](.*)\[MACROEND\]/s;
  $macrostuff = $1;
  unless ($macrostuff) {
    $self->{logString} = "No MACRO section found in delta.";
    return 0;
  }

  # Figure out which type of virus is involved
  $data =~ /\[Type\]\s*(\S*)\s*/;
  $typelist = $1;
  if ($typelist =~ /WD/) {
    if ($typelist =~ /XL/) {
      $self->{logString} = "Both WD and XL are present.  Cannot test!";
      return 0;
    }
    $addfile = "navex/wdnamsig.wdf";
  } elsif ($typelist =~ /XL/) {
    $addfile = "navex/xlnamsig.wdf";
  } else {
    $self->{logString} = "Neither WD nor XL is present.  Cannot test!";
    return 0;
  }

  return {
           AVIS::FSTools::Canonize($addfile,$nroot) => $macrostuff
         };
}

#
# Basic C-record-structure imitation for simple instance variables.
# Note that all instance variables must be initialized before this
# routine will work on them; this generally happens in new().  Also,
# variables starting with a _ cannot be accessed this way.
#
sub AUTOLOAD {
  my $self = shift;
  my $type = ref($self) || confess("$self is not an object");
  my $name = $AUTOLOAD;

  $name =~ s/.*://;                 # Extract just the name part
  return undef if $name =~ /^_.*/;  # Hide private variables
  return undef if not exists($self->{$name});
  return $self->{$name} = shift if @_;
  return $self->{$name};
}

1;    # Just in case

