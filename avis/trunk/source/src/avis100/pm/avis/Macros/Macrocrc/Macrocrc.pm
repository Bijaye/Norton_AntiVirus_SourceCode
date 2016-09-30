####################################################################
#                                                                  #
# Program name:  pm/AVIS/Macros/FileProperties.pm                  #
#                                                                  #
# Package name:  AVIS::Macro::FileProperties                       #
#                                                                  #
# Description:   Determine properties of macro files               #
#                                                                  #
# Statement:     Licensed Materials - Property of IBM              #
#                (c) 1998-2000                                     #
#                                                                  #
# Authors:       David M Chess, Jean-Michel Boulay                 #
#                                                                  #
# And also:      U.S. Government Users Restricted Rights - use,    #
#                duplication or disclosure restricted by GSA ADP   #
#                Schedule Contract with IBM Corp.                  #
#                                                                  #
#                                                                  #
####################################################################
#
# Change history:
#  1998/12/01 - First version.
#  1998/12/02 - Add GetMacroList.
#  1998/12/03 - Have GetMacroList exploit cache better.
#  1998/12/04 - UNC relativization stuff.
#  1998/12/09 - Make UNC slash-stuff more robust via Canonize.
#  1998/12/10 - GetMacroList only call one VBA5 lister.  Also
#               detect empty VBA5 modules.
#  1999/02/09 - Use FSTools for Canonize().
#  1999/02/17 - Add macro-length calls.
#  1999/02/24 - Make an empty UNC cause absolute filenames
#  1999/02/26 - Even better caching.  (Would really be best to
#               cache to a file, to avoid re-reading ever at all?)
#  Rest: see CMVC

package AVIS::Macro::Macrocrc;

#
# A small piece of OO-Perl that knows how to get the CRCs of macros
# from files; also supports a cache, for speed.
#
# Class Methods:
#
#    $whatever = new AVIS::Macro::Macrocrc;
#
#      Returns an empty Macrocrc object.
#
# Instance Methods:
#
#    $whatever->ToolDir($tooldir);
#
#      tells the object where to find the tools that it needs (the
#      binary executables that extract macros, calculate CRCs, etc),
#      and returns the new setting.  If passed no argument it will
#      just return the current setting without changing it.  Note that
#      the directory-name passed in is *not* UNC-relative.
#
#    $whatever->UNC($UNC);
#
#      tells the object the UNC for this run: a string that needs to
#      be prepended to any filename that are UNC-relative.  Special case:
#      if the UNC is equal to "", all filenames will be passed to the
#      operating system as is.
#
#    $whatever->GetMacroCRC(filename,appname,macroname);
#
#      returns the CRC (as an 8-byte ASCII-hex string) of the macro
#      for the given application (WD7, WD8, XL95, XL97) with the
#      given macro name.  Returns undef if there ain't one.  The
#      filename given must be UNC-relative.
#
#    $whatever->SetMacroCRC(filename,appname,macroname,crcstring);
#
#      stores the corresponding data into the objects internal
#      cache, for use by later GetMacroCRC() calls.  This is
#      useful when the information about macro CRCs has been obtained
#      from some other source, and we don't want to have to reread
#      it later.  $filename must be UNC-relative.
#
#    $whatever->GetMacroLen(filename,appname,macroname);
#
#      returns the length (as a decimal number) of the macro
#      for the given application (WD7, WD8, XL95, XL97) with the
#      given macro name.  Returns undef if there ain't one.  The
#      filename given must be UNC-relative.
#
#    $whatever->SetMacroLen(filename,appname,macroname,length);
#
#      stores the corresponding data into the objects internal
#      cache, for use by later GetMacroLen() calls.  This is
#      useful when the information about macro lengths has been obtained
#      from some other source, and we don't want to have to reread
#      it later.  $filename must be UNC-relative.
#
#    $arrayref = $whatever->GetMacroList(filename)
#
#      returns the usual information, but in a somewhat different
#      form: this time, it's a ref to an array of hashrefs.  Each hash has
#      fields called App, Macroname, CRC, and Len, with the obvious values.
#      Filename is of course UNC-relative.
#

use Cwd;
use File::Path;
use AVIS::FSTools();

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
# The "private" instance variables
  $self->{_MacrosInFiles} = {};
# and we're done
  return $self;
}

#
# Load all the macros for the given app into the given file in the quiche.
#
sub _GetFileMacros {
  my $self = shift;
  my $fname = shift;
  my $appname = shift;
  if ($appname eq "WD7") {
    $self->_GetWD7FileMacros($fname);
  } elsif ($appname eq "WD8") {
    $self->_GetWD8FileMacros($fname);
  } elsif ($appname eq "XL97") {
    $self->_GetXL97FileMacros($fname);
  } elsif ($appname eq "XL95") {
    $self->_GetXL95FileMacros($fname);
  } elsif ($appname eq "XL4") {
    $self->_GetXL4FileMacros($fname);
  } elsif ($appname eq "PP97") {
    $self->_GetPP97FileMacros($fname);
  } elsif ($appname eq "AC97") {
    $self->_GetAC97FileMacros($fname);
  } else {
    die "Unknown appname <$appname> :";
  }
}

#
# Load all the WD7 macros into the given file into the macros-in-files
# hash/cache.
#
sub _GetWD7FileMacros {
  my $self = shift;
  my $fname = shift;
  my $hashref = $self->{_MacrosInFiles};

  # See if we have a cache-hit
  return if exists $hashref->{$fname}{WD7};

  # Otherwise do the expensive query
  my $program = AVIS::FSTools::Canonize("wd7crc",$self->ToolDir);
  my $fullfname = $fname;
  $fullfname = Win32::GetShortPathName(AVIS::FSTools::Canonize($fname,$self->{UNC}))
    if $self->{UNC} ne "";
  my $command = "$program $fullfname";
  my $results = `$command`;
  $hashref->{$fname}{WD7} = {};  # in case there are none
  while ( $results =~ m/^Macro.*?:\s(\S*).*Size:\s*(\S*).*CRC32:\s0x(\S*)\s*$/gim ) {
    $hashref->{$fname}{WD7}{uc $1}{CRC} = $3;
    $hashref->{$fname}{WD7}{uc $1}{Len} = $2;
  }
}

#
# Load all the WD8 macros into the given file into the macros-in-files
# hash/cache (just passes along to _GetVBA5FileMacros()).
#
sub _GetWD8FileMacros {
  my $self = shift;
  my $fname = shift;
  return $self->_GetVBA5FileMacros($fname,"WD8");
}

#
# Similarly for "XL97".
#
sub _GetXL97FileMacros {
  my $self = shift;
  my $fname = shift;
  return $self->_GetVBA5FileMacros($fname,"XL97");
}

#
# Similarly for "XL97".
#
sub _GetPP97FileMacros {
  my $self = shift;
  my $fname = shift;
  return $self->_GetVBA5FileMacros($fname,"PP97");
}

#
# Load all the vba5 macros into the given file into the macros-in-files
# hash/cache.
#
sub _GetVBA5FileMacros {
  my $self = shift;
  my $fname = shift;
  my $app = shift;
  my $hashref = $self->{_MacrosInFiles};

  # See if we have a cache-hit
  return if exists $hashref->{$fname}{$app};

  # Otherwise do the expensive query
  my $program = AVIS::FSTools::Canonize("vba5ext",$self->ToolDir);
  my $fullfname = $fname;
  $fullfname = Win32::GetShortPathName(AVIS::FSTools::Canonize($fname,$self->{UNC}))
    if $self->{UNC} ne "";
  my $command = "$program \-s $fullfname";
  my $results = `$command`;
  $hashref->{$fname}{"XL97"} = {};   # in case there are none
  $hashref->{$fname}{"WD8"} = {};   # in case there are none
  $hashref->{$fname}{"PP97"} = {};   # in case there are none  
  if (substr($results,0,5) eq "Excel") {$app = "XL97";}
  if (substr($results,0,4) eq "Word") {$app = "WD8";}
  if (substr($results,0,10) eq "PowerPoint") {$app = "PP97";}
  while ( $results =~ m/^Module.*?:\sCRC32\(0x(\S*?)\,(.*?)\).*?Name\((.+?)\)\s*$/gim ) {
    $hashref->{$fname}{$app}{uc $3}{CRC} = $1 if 0 + $2;
    $hashref->{$fname}{$app}{uc $3}{Len} = $2 if 0 + $2;
  }
}

#
# Load all the XL95 macros into the given file into the macros-in-files
# hash/cache.
#
sub _GetXL95FileMacros {
  my $self = shift;
  my $fname = shift;
  my $hashref = $self->{_MacrosInFiles};

  # See if we have a cache-hit
  return if exists $hashref->{$fname}{"XL95"};

  # Otherwise do the expensive query
  my $program = AVIS::FSTools::Canonize("xl5ext",$self->ToolDir);
  my $fullfname = $fname;
  $fullfname = Win32::GetShortPathName(AVIS::FSTools::Canonize($fname,$self->{UNC}))
    if $self->{UNC} ne "";
  my $command = "$program -s $fullfname";
  my $results = `$command`;
  $hashref->{$fname}{"XL95"} = {};   # In case there are none
  while ( $results =~ m/^Module.*?:\sCRC32\(0x(\S*?)\,(.*?)\).*?Stream\((.+?)\,\S*\s*$/gim ) {
    $hashref->{$fname}{"XL95"}{uc $3}{CRC} = $1 if 0 + $2;
    $hashref->{$fname}{"XL95"}{uc $3}{Len} = $2 if 0 + $2;
  }
}

#
# Load all the XL4 macros into the given file into the macros-in-files
# hash/cache.
#
sub _GetXL4FileMacros {
  my $self = shift;
  my $fname = shift;
  my $hashref = $self->{_MacrosInFiles};

  # See if we have a cache-hit
  return if exists $hashref->{$fname}{"XL4"};

  # Otherwise do the expensive query
  my $program = AVIS::FSTools::Canonize("xl4ext",$self->ToolDir);
  my $fullfname = $fname;
  $fullfname = Win32::GetShortPathName(AVIS::FSTools::Canonize($fname,$self->{UNC}))
    if $self->{UNC} ne "";
  my $command = "$program $fullfname";
  
  if (mkdir("xl4ext", 0777)) {
    my $curdir = cwd();
    chdir("xl4ext");
    unlink("xl4.idx") if -e "xl4.idx";
    my $results = `$command`;  
    $hashref->{$fname}{"XL4"} = {};   # In case there are none
    if (open(XL4LISTING, "xl4.idx")) {
      while(<XL4LISTING>) {
        chop();
        if (m/^Module.*?:\sCRC32\(0x(\S*?)\,(.*?)\).*?Name\((.*)\)\s*$/gim ) {
          $hashref->{$fname}{"XL4"}{uc $3}{CRC} = $1 if 0 + $2;
          $hashref->{$fname}{"XL4"}{uc $3}{Len} = $2 if 0 + $2;
        }
      }
      close(XL4LISTING);
    }
    #remode files created by xl4ext
    chdir($curdir)||warn "unable to chdir to $curdir: $!";
    rmtree("xl4ext", 0, 1)||warn "unable to remove xl4ext: $!";
  }
  else {
    warn "failed to create xl4ext directory: $!";
  }
}


#
# Load all the AC97 macros into the given file into the macros-in-files
# hash/cache.
#
sub _GetAC97FileMacros {
  my $self = shift;
  my $fname = shift;
  my $hashref = $self->{_MacrosInFiles};

  # See if we have a cache-hit
  return if exists $hashref->{$fname}{"AC97"};

  # Otherwise do the expensive query
  my $program = AVIS::FSTools::Canonize("a97ext",$self->ToolDir);
  my $fullfname = $fname;
  $fullfname = Win32::GetShortPathName(AVIS::FSTools::Canonize($fname,$self->{UNC}))
    if $self->{UNC} ne "";
  my $command = "$program $fullfname";
  
  if (mkdir("a97ext", 0777)) {
    my $curdir = cwd();
    chdir("a97ext");
    unlink("a97.idx") if -e "a97.idx";
    my $results = `$command`;  
    $hashref->{$fname}{"AC97"} = {};   # In case there are none
    if (open(A97LISTING, "a97.idx")) {
      my $version;
      while(<A97LISTING>) {
        if (m/^File\s+Page:Index\s+Size\s+(\d{2})\s+Module name\s*\n$/gim ) {
          $version = $1;
        }
        if (m/^m\d+\.mod\s+\S+:\S+\s+\S+\s(.*)\s*\n$/gim ) {
          my $name = $1;
          $_ = <A97LISTING>;
          if (m/^\s+CRC\(0x(\S+?)\)\s+Size\((\d+)\)\s*\n$/gim ) {
            $hashref->{$fname}{"AC$version"}{uc $name}{CRC} = $1 if 0 + $2;
            $hashref->{$fname}{"AC$version"}{uc $name}{Len} = $2 if 0 + $2;
          }
        }
      }
      close(A97LISTING);
    }
    #remode files created by xl4ext
    chdir($curdir);
    rmtree("a97ext", 0, 1);
  }
  else {  
    warn "failed to create a97ext directory: $!";
  }
}


#
# Given a filename, an appname (WD7, WD8, etc), and the name of a
# macro for that app in that file, return the CRC of that macro.
#
sub GetMacroCRC {
  my $self = shift;
  my $fname = shift;
  my $appname = shift;
  my $mname = uc shift;
  my $hashref = $self->{_MacrosInFiles};
  my $answer;
  return $hashref->{$fname}{$appname}{$mname}{CRC} if exists $hashref->{$fname}{$appname}{$mname}{CRC};
  $self->_GetFileMacros($fname,$appname);
  return $hashref->{$fname}{$appname}{$mname}{CRC} if exists $hashref->{$fname}{$appname}{$mname}{CRC};
  return undef;
}

#
# Given a filename, an appname (WD7, WD8, etc), and the name of a
# macro for that app in that file, return the length of that macro.
#
sub GetMacroLen {
  my $self = shift;
  my $fname = shift;
  my $appname = shift;
  my $mname = uc shift;
  my $hashref = $self->{_MacrosInFiles};
  my $answer;
  return $hashref->{$fname}{$appname}{$mname}{Len} if exists $hashref->{$fname}{$appname}{$mname}{Len};
  $self->_GetFileMacros($fname,$appname);
  return $hashref->{$fname}{$appname}{$mname}{Len} if exists $hashref->{$fname}{$appname}{$mname}{Len};
  return undef;
}

#
# Given a filename, return an array of hashrefs, each hash having fields
# called App, Macroname, and CRC, corresponding to the known macros in
# that file.
#
sub GetMacroList {
  my $self = shift;
  my $fname = shift;
  my $hashref;
  my @answer;
  my ($k1,$k2,$v);

  $self->_GetFileMacros($fname,"WD7");
  $self->_GetFileMacros($fname,"XL95");
  $self->_GetFileMacros($fname,"XL4");
  $self->_GetFileMacros($fname,"WD8");
# $self->_GetFileMacros($fname,"XL97");
# $self->_GetFileMacros($fname,"PP97");
  $self->_GetFileMacros($fname,"AC97");
  $hashref = $self->{_MacrosInFiles}{$fname};
  return [] if not defined $hashref;
  foreach $k1 ( keys %$hashref  ) {   # Apps
    foreach $k2 ( keys %{$hashref->{$k1}} ) {  # macro names
      $v = $hashref->{$k1}{$k2}{CRC};
      $w = $hashref->{$k1}{$k2}{Len};
      push @answer, { App => $k1, Macroname => $k2, CRC => $v, Len => $w };
    }
  }
  return \@answer;
}

#
# Given a filename, an appname (WD7, WD8, etc), the name of a
# macro, and a CRC, fill in that stuff in the cache.
#
sub SetMacroCRC {
  my $self = shift;
  my $fname = shift;
  my $appname = shift;
  my $mname = uc shift;
  my $crcval = shift;
  my $hashref = $self->{_MacrosInFiles};
  $hashref->{$fname}{$appname}{$mname}{CRC} = $crcval;
}

#
# Given a filename, an appname (WD7, WD8, etc), the name of a
# macro, and a length,fill in that stuff in the cache.
#
sub SetMacroLen {
  my $self = shift;
  my $fname = shift;
  my $appname = shift;
  my $mname = uc shift;
  my $lenval = shift;
  my $hashref = $self->{_MacrosInFiles};
  $hashref->{$fname}{$appname}{$mname}{Len} = $lenval;
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

1;

