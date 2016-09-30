#
# Change history:
#  1998/11/13 - First version finished.
#  1998/11/17 - Make sure supersets are listed before subsets, by
#               just ordering the forms by macro-count.  Of course this
#               doesn't help if a new virus is a superset of an old one;
#               the SARCBOT def-absorbing process will have to do that.
#  1998/12/01 - Split out macros-in-files stuff into Macrocrc.pm.
#  1998/12/04 - Clean up UNC-relativization stuff, related cleanup.
#  1998/12/10 - Use a << here and there, for readability.
#  1999/01/25 - Add "Category" field to the return hash from MakeNavDefs(),
#               for the new DefPak output format.
#  1999/01/26 - Put in special DefPak <VirusName> and <VirusID> and etc tags;
#               optionally use NAME_ANY_NAME to ignore macro names.
#  1999/02/04 - Optionally use NotFullSet on all defs.
#  1999/02/05 - Correct use of <VirusID> and <VirusIDLabel> tags.
#  1999/02/17 - Add structure for macro lengths and such, compute VirusSize.
#  1999/02/18 - Use new Cleans class to NotFullSet known clean macros.
#  1999/02/24 - Include CRC in ID of NamedCRCSigs, to avoid clashes.
#  1999/03/04 - Clean up non-ASCII chars in macro-names, to avoid misbuilds.
#

package AVIS::Macro::Mkdefs;

#
# A small piece of OO-Perl that knows how to create defs for NAV and
# IBMAV, given a set of virus "forms".
#
# Class Methods:
#
#    $whatever = new AVIS::Macro::Mkdefs;
#
#      Returns an empty Mkdefs object.
#
# Instance Methods:
#
#    $whatever->ToolDir($tooldir);
#
#      tells the object where to find the tools that it needs (the
#      binary executables that extract macros, calculate CRCs, etc),
#      and returns the new setting.  If passed no argument, it will
#      just return the current setting without changing it.  The
#      directory name is *not* UNC-relative.
#
#    $whatever->VirusName($vn);
#
#      tells the object the name of the virus it's working on, and
#      returns the string.  If passed no argument it just returns the
#      current setting without changing it.
#
#    $whatever->UNC($UNC);
#
#      tells the object the UNC for this run: a string that needs to
#      be prepended to any filenames that are UNC-relative.
#
#    $whatever->NewVirus();
#
#      makes the object forget any existing forms it's been given,
#      uncache information about macros in files, and otherwise reset
#      (but not clear its list of existing definitions; if you want
#      to do that too, just get rid of the object and new a new one).
#
#    $whatever->AddForm($filename,$app,@macronames);
#
#      tells the object about a new form of the virus, which consists
#      of the given macros as found in the given file.  Returns a FALSE
#      value in case of error.  Valid apps are WD7, WD8, XL95, and XL97.
#      The filename is UNC-relative.
#
#    $whatever->AddForm($filename,$app,\@macronames,\@crcstrings);
#
#      This call can be used if the CRCs of the macros are already known;
#      the fourth argument must reference an array of strings with the same
#      cardinality as the array of macro names.  Each string should be
#      eight characters long, with no leading "0x" or anything.  When this
#      form is used, the (UNC-relative) $filename is recorded, but not used.
#
#    $whatever->AddForm($filename,$app,\@macronames,\@crcs,\@crclens);
#
#      This call can be used if the lengths of the macros, as well as the
#      CRCs, are already known.  The last argument is a reference to an array
#      of (decimal) lengths of the corresponding macros.
#
#    $whatever->SetForms($arrayref);
#
#      will throw away any existing information about forms, and substitute
#      the information in the given array.  Each element of the array must
#      be a hash reference, containing fields called "Fname" and "App" with
#      the obvious values, and a field "Mnames" containing a reference to
#      an array of macro-names.  If CRCs for the macros in a form are
#      known, they may be provided in a field "Crcvals" containing a
#      reference to an array of crc-value strings (see above), in which
#      case the "Fname" field is ignored.  If the Crcvals field exists,
#      there may also be a "Lengths" field, containing a reference to an
#      array of decimal numbers reflecting macro lengths.  Returns false
#      in case of error.  Note that when the "Fname" field is used, it is
#      assumed to be UNC-relative.
#
#    $whatever->LoadNavDefs($defdir);
#
#      will load a set of NAV macro signatures from the files in the
#      given directory.  When producing new defs, it will re-use any
#      existing subsignatures, rather than making duplicates.  Returns
#      a false value in case of error; see GetLastError().  Note that
#      the directory name is *not* UNC-relative.
#
#    @newdefs = $whatever->MakeNavDefs();
#
#      returns a list of hash references, where the "Filename" field of
#      each hash contains the name of a def-source file, the "Content"
#      field contains a string, broken up into lines by newlines, to be
#      added to that file, and the "Category" field contains the general
#      category of the change (for the DefPak format).  In case of error,
#      returns undef; see GetLastError().
#
#    $whatever->IgnoreMacroNames(boolean);
#
#      set or reset (or query, if called with no arguments) the object's
#      macro-name-ignoring mode.  If that mode is set true, the defs
#      produced by MakeNavDefs() will ignore the names of macros, doing
#      all detection and repair by CRC instead.  This can be useful both
#      if we're afraid a virus might be more name-polymorphic than we
#      noticed, and also just in general, since macros do get renamed in
#      the real world, and it's nice to be able to handle that.
#
#    $whatever->UseNotFullSet(boolean);
#
#      set or reset (or query, if called with no arguments) the object's
#      notfullset-using mode.  If that mode is set true, the defs
#      produced by MakeNavDefs() will include the [NotFullSet] keyword
#      in the NamedCRCSigs that it produces.  This lessens the danger
#      of detecting an innocent snatched user macro as a virus, but
#      increases the danger of not correctly handling a subset of the
#      virus that is still viral, in the future.
#
#    $errormsg = $whatever->GetLastError();
#
#      returns a free-form error message if the last NakeNavDefs() or
#      LoadNavDefs() failed (otherwise returns an empty string).  It
#      would probably be a good idea to formalize these eventually, and
#      have the first token be a numeric error code or whatever.
#
#    $whatever->StatusCallback($coderef);
#
#      allows the caller to supply a reference to a callback routine
#      which will get whatever progress and status messages and such
#      that the object generates as it goes.  The callback routine will
#      be called with two arguments: a message category (like "Error"),
#      and a message (like "the building is on fire").  The return value
#      from the callback routine is ignored.  At the moment, the objects
#      don't call the callback very often at all; there is room for
#      expansion here!
#

use AVIS::Macro::Macrocrc();
use AVIS::Macro::Cleans();

#
# The usual oo-perl new() routine
#
sub new {
  my $class = shift;      # the class to bless into
  my $mname = shift;      # the argument we were given
  my $self = {};          # the anonymous hash to return
  bless $self, $class;
# Simple instance variables
  $self->{ToolDir} = "";
  $self->{UNC} = "";
  $self->{VirusName} = "Unknown";    # To be fixed up later
  $self->{VirusSize} = 1000;         # To be fixed up later
  $self->{StatusCallback} = sub {};
  $self->{IgnoreMacroNames} = 0;
  $self->{UseNotFullSet} = 0;
# The "private" instance variables
  $self->NewVirus;
  $self->{_ExistingNameSigs} = {};
  $self->{_ExistingNamedCRCSigs} = {};
  $self->{_CleanList} = {};
# and we're done
  return $self;
}

#
# Forget all existing forms and virus-name and defs and stuff (but
# not cached defs).
#
sub NewVirus {
  my $self = shift;
  $self->{_Macrocrc} = new AVIS::Macro::Macrocrc();
  $self->{_Forms} = [];
  $self->{VirusName} = "Unknown";
  $self->{_NameSigs} = "";
  $self->{_NamedCRCSigs} = "";
  $self->{_KnownVirusSigs} = "";
  $self->{_LastError} = "";
  $self->{_DefxxSRC} = "";
  $self->{_VirscanTXT} = "";
}

#
# Given a filename, appname,  and a list of macro names, adds them
# to the list of virus forms.  Does not check for validity of appname,
# check if the file exists, or anything like that.  If the third
# argument is an array-reference, It is the list of macro names, and
# and the next is a list of the CRCs of those macros, and the next, if
# present, is a list of the lengths of those macros.
#
sub AddForm {
  my $self = shift;
  my $fname = shift;
  my $app = shift;
  my @mnames = @_;
  my $macrocrc = $self->{_Macrocrc};
  my ($newform, $i);
  if (ref $mnames[0]) {
    @crcvals = @{$mnames[1]};
    @mnames = @{$mnames[0]};
    scalar @mnames == scalar @crcvals or return 0;
    for ($i = 0; $i < @mnames; $i++) {
      $macrocrc->SetMacroCRC($fname,$app,uc $mnames[$i],$crcvals[$i]);
    }
    if (ref $mnames[2]) {
      @maclens = @{$mnames[2]} if ref $mnames[2];
      scalar @mnames == scalar @maclens or return 0;
      for ($i = 0; $i < @mnames; $i++) {
        $macrocrc->SetMacroLen($fname,$app,uc $mnames[$i],$maclens[$i]);
      }
    }
  }
  $newform = { Fname => $fname, App => $app, Mnames => \@mnames };
  push @{$self->{_Forms}}, $newform;
  return 1;
}

#
# Set all the forms at once.  See documentation above for details.
#
sub SetForms {
  my $self = shift;
  my $newforms = shift;
  my $macrocrc = $self->{_Macrocrc};
  my ($thisform,$fname,$app);
  my (@mnames, @crcvals, @maclens, $i);

# If any forms have CRCs supplied (and lengths if any), fill in the quiche
  foreach $thisform (@$newforms) {
    if (ref $thisform->{Crcvals}) {
      @mnames = @{$thisform->{Mnames}};
      @crcvals = @{$thisform->{Crcvals}};
      scalar @mnames == scalar @crcvals or return 0;
      $fname = $thisform->{Fname};
      $app = $thisform->{App};
      for ($i = 0; $i < @mnames; $i++) {
        $macrocrc->SetMacroCRC($fname,$app,uc $mnames[$i],$crcvals[$i]);
      }
      if (ref $thisform->{Lengths}) {
        @maclens = @{$thisform->{Lengths}};
        scalar @mnames == scalar @maclens or return 0;
        for ($i = 0; $i < @mnames; $i++) {
          $macrocrc->SetMacroLen($fname,$app,uc $mnames[$i],$maclens[$i]);
        }
      }
    }
  }

# Adopt it wholesale, if that all worked
  $self->{_Forms} = $newforms;

  return 1;
}

#
# Returns a set of NAVS defs as described in the docs.
#
sub MakeNavDefs {
  my $self = shift;
  my @forms;
  my $thisform;
  my $clobject = new AVIS::Macro::Cleans($self->{UNC},$self->{ToolDir});

  $self->{_CleanList} = $clobject->getCleanList();
  if (not defined $self->{_CleanList}) {
    my $err = $clobject->LastError();
    $err = "List of known-clean macros could not be read ($err)!";
    $self->{_LastError} = $err;
    &{$self->{StatusCallback}}("Warning",$err);
    $self->{UseNotFullSet} = 1;
  }

  $self->_PrepareForNav();
  @forms = @{$self->{_Forms}};
  my ($isize, $thissize);
  undef $isize;
  eval {
    foreach $thisform (@forms) {
      $self->_DefsFromForm($thisform);
      $thissize = $self->_LengthFromForm($thisform);
      $isize = $thissize if not defined $isize;
      $isize = $thissize if $thissize<$isize;
    }
  };
  if ($@) {
    $self->{_LastError} = $@;
    &{$self->{StatusCallback}}("Error","$@");
    return ();
  }
  $self->VirusSize($isize);
  $self->{_LastError} = "";
  $self->_NonsigVirusInfo();
  return (
    { Category => "MACRO",
      Filename => "xxNAMSIG.WDF",
      Content => $self->{_NameSigs} },
    { Category => "MACRO",
      Filename => "xxSIGS.WDF",
      Content => $self->{_NamedCRCSigs} . "\n" .
                 $self->{_KnownVirusSigs} },
    { Category => "VIRSCAN",
      Filename => "DEFxx.SRC",
      Content => $self->{_DefxxSRC} },
    { Category => "TXT",
      Filename => "VIRSCAN.TXT",
      Content => $self->{_VirscanTXT} }
  );
}

#
# Return the most recent thing that went wrong.
#
sub GetLastError {
  my $self = shift;
  return $self->{_LastError};
}

#
# Look in the given directory for the files that existing macro sigs
# of various kinds are in, and load them into the _Existing hashes.
# This directory is *not* UNC-relative.
#
sub LoadNavDefs {
  my ($self, $navdir) = @_;
  eval {
    $self->_LoadNameSigs($navdir);
    $self->_LoadNamedCRCSigs($navdir);
  };
  if ($@) {
    $self->{_LastError} = $@;
    &{$self->{StatusCallback}}("Error","$@");
    return 0;
  }
  $self->{_LastError} = "";
  return 1;
}

#
# Load the "NameSig"s from the given directory.
#
sub _LoadNameSigs {
  my ($self, $navdir) = @_;
  # This list probably should not be hardcoded?
  $self->_LoadNameSigFile($navdir,"WDNAMSIG.WDF");
  $self->_LoadNameSigFile($navdir,"XLNAMSIG.WDF");
}

#
# Load the "NameSig"s from the given directory and file.
#
sub _LoadNameSigFile {
  my ($self, $navdir, $fname) = @_;
  local *FH;        # Another fun PERL feature!
  my $data;
  my ($entry, $crcval, $sid, $nid);
  my $hashref = $self->{_ExistingNameSigs};
  open(FH,"<$navdir\\$fname")
    || die "$fname not found in $navdir: ";
  my $howbig = read FH, $data, 20000000;
  close FH;
  die "$fname in $navdir is just too big: " if $howbig>10000000;
  while ( $data =~ m/\n\s*\[NameSigStart\](.*?)\[NameSigEnd\]$\s*\n/gis ) {
    $entry = $1;
    if ($entry =~ /\n\s*\[Signature\]\s*SeekSet\(0\)\s*\"(\S*?)\"\s*AtEnd\(\)\s*?\n/i) {
      $mname = $1;
    } else {
      next if not $entry =~ /\n\s*\[Signature\]\s*SeekSet\(0\)\s*\"(\S*?)\"\s*?\n/i;
      $mname = $1;
    }
    next if not $entry =~ /\n\s*\[ID\]\s*(\S*)\s*\n/;
    $sid = $1;
    $hashref->{uc $mname} = $sid;
  }

}

#
# Load the "NamedCRCSig"s from the given directory.
#
sub _LoadNamedCRCSigs {
  my ($self, $navdir) = @_;
  # This list probably should not be hardcoded?
  $self->_LoadNamedCRCSigFile($navdir,"WD7SIGS.WDF");
  $self->_LoadNamedCRCSigFile($navdir,"WD7SIGS2.WDF");
  $self->_LoadNamedCRCSigFile($navdir,"WD7SIGS3.WDF");
  $self->_LoadNamedCRCSigFile($navdir,"WD7SIGS4.WDF");
  $self->_LoadNamedCRCSigFile($navdir,"WD7SIGS5.WDF");
  $self->_LoadNamedCRCSigFile($navdir,"WD7SIGS6.WDF");
  $self->_LoadNamedCRCSigFile($navdir,"WD8SIGS.WDF");
  $self->_LoadNamedCRCSigFile($navdir,"XL97SIGS.WDF");
  $self->_LoadNamedCRCSigFile($navdir,"XL95SIGS.WDF");
}

#
# Load the "NamedCRCSig"s from the given file in the given directory.
#
sub _LoadNamedCRCSigFile {
  my ($self, $navdir, $fname) = @_;
  local *FH;        # Another fun PERL feature!
  my $data;
  my ($entry, $mname, $sid);
  my $hashref = $self->{_ExistingNamedCRCSigs};
  open(FH,"<$navdir\\$fname")
    || die "$fname not found in $navdir: ";
  my $howbig = read FH, $data, 20000000;
  close FH;
  die "$fname in $navdir is just too big: " if $howbig>10000000;
  while ( $data =~ m/\n\s*\[NamedCRCSigStart\](.*?)\[NamedCRCSigEnd\]$\s*\n/gis ) {
    $entry = $1;
    next if not $entry =~ /\n\s*\[Signature\]\s*Namesig\((\S*?)\)\s*?\n/i;
    $nid = $1;
    next if not $entry =~ /\n\s*\[ID\]\s*(\S*)\s*?\n/;
    $sid = $1;
    if ($entry =~ /\n\s*\[CRCWD7\]\s*0x(\S*)\s*?\n/) {
      $crcval = $1;
      $hashref->{WD7}{$crcval}{$nid} = $sid;
    }
    if ($entry =~ /\n\s*\[CRCXL95\]\s*0x(\S*)\s*?\n/) {
      $crcval = $1;
      $hashref->{XL95}{$crcval}{$nid} = $sid;
    }
    if ($entry =~ /\n\s*\[CRCXL97\]\s*0x(\S*)\s*?\n/) {
      $crcval = $1;
      $hashref->{XL97}{$crcval}{$nid} = $sid;
    }
  }
}

#
# Given a macro-name, returns the ID for the NameSig to use
# for that name.  This will be either NAME_ANY_NAME, if we're
# in that mode, or the actual signature that matches the name.
#
sub _NameSigFor {
  my $self = shift;
  my $mname = shift;
  return "NAME_ANY_NAME" if $self->IgnoreMacroNames();
  return $self->_RealNameSigFor($mname);
}

#
# Given a macro-name, returns the ID for the NameSig for that
# macro name (first creating one if necessary and adding it
# to the _NameSigs string)
#
sub _RealNameSigFor {
  my $self = shift;
  my $mname = uc shift;
  my $hashref = $self->{_ExistingNameSigs};
  my ($i, $c);
  my $clean_sig;
  my $clean_name = $mname;
  my ($oldstate, $newstate);
  my $xcount;

  return "NAME_ANY_NAME" if length($mname)==0;   # just in case

  $clean_name =~ s/\W/_/g;  # Make funny chars into _'s

  return $hashref->{$mname} if exists $hashref->{$mname};
  my $newname = "NAME_ONLY_".$clean_name."_<GUID>";

  # cleanse the signature of odd characters also, if necessary
  if (not $mname =~ /[^\w\b]/) {
    $clean_sig = qq/"$mname"/;
  } else {
    $oldstate = 0;
    $xcount = 0;
    $clean_sig = "";
    for ($i=0;$i<=length($mname);$i++) {
      if ($i==length($mname)) {
        $newstate = 0;
      } else {
        $c = substr($mname,$i,1);
        $newstate = ($c =~ /[\w\b]/) ? 1 : 2;
      }
      if ($oldstate==$newstate) {
        if ($newstate==1) {            # just accumulating letters
          $clean_sig .= $c;
        } elsif ($newstate==2) {       # one more funnything
          $xcount++;
        }
      } else {    # Starting a new state
        # Close the old state
        if ($oldstate==1) {        # need to close quotes
          $clean_sig .= q/" /;
        } elsif ($oldstate==2) {   # need to write a SeekCur()
          $clean_sig .= "SeekCur($xcount) ";
          $xcount=0;
        }
        # Start the new one
        if ($newstate==1) {  # need to open quotes
          $clean_sig .= qq/"$c/;
        } elsif ($newstate==2) {
          $xcount=1;    # seen one now
        }
      }
      $oldstate = $newstate;
    }  # end of for loop
  }  # end if funny chars in name

  # Now finally write the entry
  $self->{_NameSigs} .= <<"EOD";

[NameSigStart]
[Comment]       Name $mname
[ID]            $newname
[Signature]     SeekSet(0) $clean_sig AtEnd()
[NameSigEnd]
EOD
  $hashref->{$mname} = $newname;
  return $newname;
}

#
# Given a filename, a macro-name, and the ID of a NameSig that gets that
# macro name, return the ID for a NamedCRCSig that matches it, first
# creating and adding to the _NameCRCSigs string if necessary.
#
sub _NamedCRCSigFor {
  my $self = shift;
  my $fname = shift;
  my $appname = shift;
  my $mname = uc shift;
  my $NameSigName = shift;
  my $Macrocrc = $self->{_Macrocrc};
  my $crcval;
  my $clean_name = $mname;
  my $hashref = $self->{_ExistingNamedCRCSigs};

  $Macrocrc->ToolDir($self->ToolDir);
  $Macrocrc->UNC($self->UNC);
  $crcval = $Macrocrc->GetMacroCRC($fname,$appname,$mname);
  (defined $crcval)
    or die "Macro <$mname> for app <$appname> not found in file <$fname>: ";
  return $hashref->{$appname}{$crcval}{$NameSigName}
    if exists $hashref->{$appname}{$crcval}{$NameSigName};
  $clean_name =~ s/\W/_/g;  # Make funny chars into _'s
  my $newname = "CRC_$clean_name"."_".$appname."_".$crcval."_<GUID>";
  $self->{_NamedCRCSigs} .= <<"EOD";

[NamedCRCSigStart]
[Comment]          <VirusName>: $mname
[ID]               $newname
EOD
  $self->{_NamedCRCSigs} .= "[NotFullSet]\n"
    if $self->_NotFullSet($appname,$crcval);
  $self->{_NamedCRCSigs} .= <<"EOD";
[CRC$appname]           0x$crcval
[Signature]        NameSig($NameSigName)
[NamedCRCSigEnd]
EOD
  $hashref->{$appname}{$crcval}{$NameSigName} = $newname;
  return $newname;
}

#
# Should [NotFullSet] be used for this macro?
#
sub _NotFullSet {
  my $self = shift;

  return 1 if $self->{UseNotFullSet};    # Use for everything?

  # Otherwise check the clean list
  my $appname = shift;
  my $crcval = shift;
  my $hashref = $self->{_CleanList};
  return 1 if $hashref->{$appname}{$crcval};
  return 0;
}

#
# Prepare to generate NAV defs.  At the moment, this just means
# ordering the array of forms in descending order by number of
# macros, to get supersets listed before subsets.
#
sub _PrepareForNav {
  my $self = shift;
  my @forms = sort {
      scalar(@{$b->{Mnames}}) <=> scalar(@{$a->{Mnames}}) } @{$self->{_Forms}};
  $self->{_Forms} = \@forms;
}

#
# Given a filename and the name of a macro in that file, update the
# various "Sigs" strings as necessary for that macro, return the
# name of the NamedCRCSig used for it.
#
sub _DefsFromMacro {
  my ($self, $fname, $appname, $mname) = @_;
  my $NameSigName = $self->_NameSigFor($mname);
  return $self->_NamedCRCSigFor($fname,$appname,$mname,$NameSigName);
}

#
# Given one form of the virus, update the various "Sigs" strings
# to cover it.
#
sub _DefsFromForm {
  my $self = shift;
  my $thisref = shift;
  my %thishash = %$thisref;
  my $thisfname = $thishash{Fname};
  my $thisapp = $thishash{App};
  my @mnames = @{$thishash{Mnames}};
  my ($thismacro, $thissig, $lastmacro);
  $self->{_KnownVirusSigs} .= <<"EOD";

[KnownVirusSigStart]
[Comment]   <VirusName>
[Type]      $thisapp
[QAStatus]  NO
[ID]        <VirusIDLabel>
EOD
  $lastmacro = pop @mnames;
  foreach $thismacro (@mnames) {
    $thissig = $self->_DefsFromMacro($thisfname,$thisapp,$thismacro);
    $self->{_KnownVirusSigs} .= "[Signature] NamedCRCSig($thissig) and\n";
  }
  $thissig = $self->_DefsFromMacro($thisfname,$thisapp,$lastmacro);
  $self->{_KnownVirusSigs} .= <<"EOD";
[Signature] NamedCRCSig($thissig)
[Repair]    UseSignature
[KnownVirusSigEnd]
EOD
}

#
# Given one form of the virus, return its length
#
sub _LengthFromForm {
  my $self = shift;
  my $thisref = shift;
  my %thishash = %$thisref;
  my $thisfname = $thishash{Fname};
  my $thisapp = $thishash{App};
  my @mnames = @{$thishash{Mnames}};
  my $thismacro;
  my $answer = 0;
  my $Macrocrc = $self->{_Macrocrc};

  $Macrocrc->ToolDir($self->ToolDir);
  $Macrocrc->UNC($self->UNC);

  foreach $thismacro (@mnames) {
    $answer += $Macrocrc->GetMacroLen($thisfname,$thisapp,$thismacro);
  }
  return $answer;
}

#
# Fill in the _DefxxSRC and _VirscanTXT fields for this virus
#
sub _NonsigVirusInfo {
  my $self = shift;
  my $now = localtime;
  my $isize = $self->VirusSize();

  $self->{_VirscanTXT} .= <<"EOD";

<VirusName>
<
>(No further information is currently available.)
# <VirusID>h
* ====================================
EOD

  $self->{_DefxxSRC} .= <<"EOD";

[CommentBegin]
Virus Name    : <VirusName>
Def Author    : BlueIce/Macro
Revision Date : $now
VID           : <VirusID>
[CommentEnd]

[DEFBEGIN]
[Build_Status]    = UNTESTED
[Virus_Type]      = ALG
[Virus_ID]        = 0x<VirusID>
[Virus_Name]      = "<VirusName>"
[Virus_CRC]       = 0x0000
[Control_Flags]   = NOT_IN_TSR MACRO
[Info_Flags]      = NONE
[Fixed_ID]        = 0x0000
[Memory_Sig]      = None
[Signature]       = 0xdf RetFalse()
[Infection_Size]  = #$isize
[Tag_Size]        = #0
[Host_Jump_Loc]   = 0x0000
[Jump_Length]     = #0
[SS_Offset]       = 0xffff
[SP_Offset]       = 0xffff
[CS_Offset]       = 0xffff
[IP_Offset]       = 0xffff
[Header_Loc]      = 0xffff
[Header_Length]   = 0xffff
[Book_Mark_Loc]   = 0x0000
[Book_Mark]       = 0x0000
[DEFEND]

;**************************************
EOD
}

#
# A debugging routine that just prints the current status.
#
sub DebugDump {
  my $self = shift;
  my $vn = $self->{VirusName};
  my @forms = @{$self->{_Forms}};
  my $forms = @forms;
  my ($thisform, $thisfname, @thislist, %thishash, $thismname);
  print "Virus: $vn ($forms forms)\n";
  foreach $thisform (@forms) {
    %thishash = %$thisform;
    $thisfname = $thishash{Fname};
    print " $thisfname \($thishash{App}\): ";
    @thislist = @{$thishash{Mnames}};
    foreach $thismname (@thislist) {
      print "$thismname ";
    }
    print "\n";
  }
  print "\n";
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

