###################################################################
#                                                                  #
# Program name:  pm/AVIS/FSTools/FSTools.pm                        #
#                                                                  #
# Package name:  AVIS::FSTools                                     #
#                                                                  #
# Description:   Tools for manipulating files and directories.     #
#                                                                  #
# Statement:     Licensed Materials - Property of IBM              #
#                (c) Copyright IBM Corp. 1998-1999                 #
#                                                                  #
# Author:        Morton Swimmer                                    #
#                                                                  #
#                U.S. Government Users Restricted Rights - use,    #
#                duplication or disclosure restricted by GSA ADP   #
#                Schedule Contract with IBM Corp.                  #
#                                                                  #
####################################################################

# pod style header:

=head1 NAME

Package: AVIS::FSTools

Filename: pm/AVIS/FSTools/FSTools.pm

Tools for manipulating files and directories.

=head1 STATEMENT

Licensed Materials - Property of IBM

(c) Copyright IBM Corp. 1998-1999 

U.S. Government Users Restricted Rights - use,    
duplication or disclosure restricted by GSA ADP   
Schedule Contract with IBM Corp.                  

=head1 AUTHOR

Morton Swimmer, et al.

=cut
    
package AVIS::FSTools;
require Exporter;
use AVIS::Local;
use Text::CSV_XS;
use IO::File;
use File::Copy; 
use File::Path; 
use File::Basename ();
use Cwd;
use Time::Local;
use Time::localtime;
    
@ISA = qw(Exporter);
@EXPORT = qw(MkLocalWorkDir Wipe CopyToDir
	     ReadVRun WriteVRun 
	     DateStr2Long Date2Jap);

@EXPORT_OK = (@EXPORT, qw(Canonize GetFullPath diroffile filepath objectfullpath));
    
use strict;
    
1; # OK to load module

=head1 DESCRIPTION

=head2 MkLocalWorkDir

AVIS::FSTools::MkLocalWorkDir($Cookie, $ProcessID, $unc)

Makes a unique work directory on the local workstation for this
process to use. The name of the directory is returned. The process
is responsible for cleaning the directory after use.

=cut

sub MkLocalWorkDir ($$$) {
  my ($Cookie, $ProcessID, $unc) = @_;
  my $WorkPath = "$isdnWorkRoot/$Cookie.$ProcessID";
  if (!mkpath("$WorkPath", 1, 0777)) {
   if (-e $WorkPath) {
     warn("Warning MkLocalWorkDir: $WorkPath already exists");
   }
   else {
     warn("Warning MkLocalWorkDir: $WorkPath $!");
   }
  }
  return $WorkPath;
}
    
=head2 Wipe

Wipe($path1, $path2, ...)
    
Routine to wipe recursively a directory or a set of files.

=cut

sub Wipe {
  foreach my $entry (@_) {
    if (-f $entry) { unlink $entry or return 0; }
    elsif (-d $entry) {
      opendir THEDIR, $entry or return 0;
      my @subs = map "$entry\\$_", grep !/^\.\.?$/, readdir THEDIR;
      closedir THEDIR;
      Wipe(@subs) or return 0;
      rmdir $entry or return 0;
    } else {
      return 0;
    }
  }
  return 1;
}
    
=head2 CopyToDir

CopyToDir(...)

Copy a set of files to a directory.
returns 1 on success.

=cut

sub CopyToDir {
  my @glob = @_;
  my $dir = pop @glob;
  my ($base,$file);
    
  $dir =~ s/[\\\/]+$//;
  return 0 unless -d $dir;
  foreach $file (@glob) {
    next unless -f $file;
    $base = $file;
    $base = File::BaseName::basename($file);
    &copy ($file, "$dir/$base") or return 0;
  }
  return 1;
}
    
=head2 FirstLine

FirstLine($file)

Returns the first line of a given file (not including the CR at the end).
If file does not exist, returns blank line.

=cut

sub FirstLine {
  my $line;
  open (F,$_[0]);
  chomp($line = <F>);
  close (F);
  return $line;
}

=head2 ReadVRun

ReadVRun($unc, $cookie, $num, $index, $type, $date, $strat, $stat,
    $sample)


Read the binary virus runs file into the arrays provided as references
    and returns.

=cut

sub ReadVRun($$$$$$$$$) {
  my ($unc, $cookie, $num, $index, $type, $date, $strat, $stat, $sample) = @_;
  my ($i, $fh, $columns, $csv, $filename);
  
  $filename = "$unc/$isfnVRun";
  $csv = Text::CSV_XS->new({'eol'  => "\012" });
  $$num = 0;
  $fh = new IO::File;
  if ($fh->open("<$filename")) {
    $i = 0; 
    $$num = 0;
    while () {
      $columns = $csv->getline($fh);
      last unless defined $columns;
      last if (scalar @$columns == 0);
      
      if (scalar @$columns == 6) {
    	($$index[$$num], 
    	 $$type[$$num], 
    	 $$date[$$num], 
    	 $$strat[$$num], 
    	 $$stat[$$num],
    	 $$sample[$$num]
    	) = @$columns;
    	#print "$$index[$$num]: $$type[$$num], $$date[$$num], $$strat[$$num], $$stat[$$num]\n";
    	$$num++;
      } else {
    	print "Incorrect number of columns in $filename on line $i\n";
      }
      $i++;
    }
    $fh->close;
  } else {
#    print "File $filename not found assuming empty file\n";
    return 0;
  }
  1;
}
	     
=head2 WriteVRun

WriteVRun($unc, $cookie, $num, $index, $type, $date, $strat, $stat, $sample)

Write the binary virus runs file from the arrays provided as
references and returns. 

=cut

sub WriteVRun($$$$$$$$$) {
  my ($unc, $cookie, $num, $index, $type, $date, $strat, $stat, $sample) = @_;
  my ($i, $fh, @columns, $csv, $filename);
    
  $filename = "$unc/$isfnVRun";
  $csv = Text::CSV_XS->new({'eol' => "\012"});
  $fh = new IO::File;
  if ($fh->open(">$filename")) {
    #    print "Number of runs: $$num\n";
    for ($i = 0; $i < $$num; $i++) {
      @columns = ($$index[$i], $$type[$i], $$date[$i], $$strat[$i], $$stat[$i], $$sample[$i]);
      #print "$$index[$i], $$type[$i], $$date[$i], $$strat[$i], $$stat[$i], $$sample[$i]\n";
      #print @columns;
      $csv->print($fh, \@columns);
    }
    $fh->close;
  } else {
    print "File $filename could not be written to\n";
    return 0;
  }
  1;
}

=head2 DateStr2Long

DateStr2Long($date)

Convert a date string to UNIX time (long integer). The format of the date string is yyyymmddhhmmss (also known as Japanese format)

For example: 19970921163112 (Japanese) == 874873872 (UNIX) == Sun Sep 21, 16:31:12, 1997 (human)

If length($Date) is < 12, use current time

Returns the UNIX time as a long integer.

=cut

sub DateStr2Long($) {
  my ($Date) = @_;
  my ($LongTime);
  
  if (length($Date) >= 12) { 
    $Date .= "00" if (length($Date) == 12); # add to $Date some seconds
    # $Date.="12";             # append seconds
    # $Date="19981010101012";  # (10:10:0015)
     
    my($cent,$year,$mon,$mday,$hours,$min,$sec) = unpack "a2a2a2a2a2a2a2",$Date;
    print "$cent,$year,$mon,$mday,$hours,$min,$sec\n";
    
    $mon--; # adjust the month, cause it's in 0..11 (for timelocal)
    
    $LongTime = timelocal($sec,$min,$hours,$mday,$mon,$year);
  }
  else { # use current date
    $LongTime = 0;
  }
  return $LongTime;
}

=head2 Date2Jap

Date2Jap(;$) 

This function returns the current date and time, according to the
local system clock, in the popular japanese format.

=cut

sub Date2Jap(;$) {
  my ($when) = @_;

  $when = time unless $when;
   
  my ($sec , $min, $hour, $day, $mon, $cyear, $tm);

=pod   

Convert the current date and time into an array of familiar
integers, and construct an array of strings containing them 
in the format "yyyymmddhhmmss".

=cut
   
  $tm = localtime($when);
  $cyear = ($tm->year)+1900;
  $mon = ($tm->mon)+1;
  $day = ($tm->mday);
  $hour = $tm->hour;
  $min = $tm->min;
  $sec = $tm->sec;
  return (sprintf "%04d%02d%02d%02d%02d%02d",$cyear,$mon,$day,$hour,$min,$sec);
}

=head2 Canonize

There are two variations:

=over 4

=item *

$result = AVIS::FSTools::Canonize($fname);

Converts all of the Other Kind of slashes into This Kind of
slashes, as determined by the current operating system.  If
the last character of $fname is either kind of slash, removes it.

=item *

$result = AVIS::FSTools::Canonize($fname,$stem);

Does the above to both $fname and $stem, and then returns $stem,
followed by This Kind of slash, followed by $fname.

=back

=cut

sub Canonize {
  my $fname = shift;
  my $thisslash = "/";
  my $thatslash = "\\";

  # Choose the right kind of slashes
  if ($^O eq 'os2' || $^O eq 'msdos' || $^O eq 'MSWin32') {
    $thisslash = "\\";
    $thatslash = "/";
  }

  # First do the simple translation.  (Note that the search pattern,
  # but not the replacement string, has to be quotemeta()d.  This
  # is a Perl oddity; trust me on this.)
  $fname =~ s|\Q$thatslash\E|$thisslash|g;

  # Then lop off any trailing slash
  (substr($fname,-1) ne $thisslash) or substr($fname,-1) = "";

  # and stick on the (canonized) stem, if any
  if (@_) {
    my $stem = shift;
    $fname = Canonize($stem).$thisslash.$fname;
  }
  return $fname;
}

=head1 DEPRECATED FUNCTIONS

Please do not use these functions anymore.

=head2 GetFullPath

GetFullPath($path)

B<WARNING>: Use of AVIS::FSTools::GetFullPath is deprecated! This
function will be dropped somewhen in the future.

=cut

sub GetFullPath {
  my ($path) = @_;
  my ($currentp, $fullp);
    
  warn "Use of AVIS::FSTools::GetFullPath is deprecated!";
    
  $currentp = &cwd();
  chdir("$path");
  $fullp = &cwd();
  chdir("$currentp");
    
  $fullp =~ s@\\@/@g;
        
  return $fullp;
}

=head2 BaseName

BaseName($path)

B<WARNING>: Use of AVIS::FSTools::BaseName is deprecated. Use
File::BaseName::basename instead! This function will be dropped
somewhen in the future.

Returns the basename of a file.  It suppresses everything before
(including) the last slash or backslash in the argument.

=cut

sub BaseName {
  my ($path) = @_;
  my $base = $path;
  warn "Use of AVIS::FSTools::BaseName is deprecated. Use File::BaseName::basename instead!";

  $base =~ s/.*[\\\/]//;
  return $base;
}


=head2 mkdirsafe

mkdirsafe($dir)

B<WARNING>: Use of AVIS::FSTools::mkdirsafe is deprecated. Use
File::Path::mkpath instead! This function will be dropped somewhen in
the future.  

Creates a directory in a 'safe' way

=cut

sub mkdirsafe {
  my ($newdir) = @_;
  warn "Use of AVIS::FSTools::mkdirsafe is deprecated. Use File::Path::mkpath instead!";
  warn "$newdir already exists\n" if -e $newdir;
  return if -d $newdir;
  mkdir $newdir, 0777 or warn " *** $0: Unable to create $newdir!\n";
}

=head2 diroffile

diroffile($file)

B<WARNING>: Use of AVIS::FSTools::diroffile is deprecated. Use
File::Basename::dirname instead! This function will be dropped
somewhen in the future.

Returns the directory part of the file path given in argument
(keeps everything up to (exclusive) the last slash or backslash)

=cut

sub diroffile {
  my ($file) = @_;
  my ($dir) = $file =~ /(.*)[\\\/]/;
  warn "Use of AVIS::FSTools::diroffile is deprecated. Use File::Basename::dirname instead!";
  return $dir;
}

=head2 filepath

filepath($file)

B<WARNING>: Use of AVIS::FSTools::filepath is deprecated! This function
will be dropped somewhen in the future.

=cut
    
sub filepath {
  my ($file) = @_;
  my $dir = diroffile($file);
  my $base = basename($file);
  my $curdir = &cwd;

  warn "Use of AVIS::FSTools::filepath is deprecated!";

  chdir $dir or warn " *** $0: Unable to chdir: $!!\n";
  my $fulldir = &cwd;
  chdir $curdir or warn " *** $0: Unable to chdir: $!!\n";
  my $path = "$fulldir\\$base";
  $path =~ s|/|\\|g; # Slash to backslash
  return $path;
}

=head2 objectfullpath

objectfullpath(object)

B<WARNING>: Use of AVIS::FSTools::objectfullpath is deprecated! This
function will be dropped somewhen in the future.

This function returns the full path of the object, file or directory,
given in argument.

=cut

sub objectfullpath {
  my $object = shift;
  my $retval;

  warn "Use of AVIS::FSTools::objectfullpath is deprecated!";

  # Keep current directory on entry
  my $curdir = cwd();
  
  $object =~ s|\\|/|g;
  
  chop($object) while ($object =~ m|//$|);
  
  # Then distinguish between files, directories, and objects
  # that don't exist (yet)
  if (-f $object) {
    # Object is a file
    #print "file\n";
    if ($object =~ m|/|) {
      my ($dir, $base) = $object =~ m|(.*/)([^/]+)|;
      chdir $dir or die "Can't chdir: $!";
      $retval = cwd() . "/" . $base;
    }
    else {
      $retval = $curdir . "/" . $object;
    }
  }
  elsif (-d $object) {
    #print "dir\n";
    chdir $object or die "Can't chdir: $!";
    $retval = cwd();
  }
  else {
    #print "not exist\n";
    if ($object =~ m[^(/|[a-zA-Z]:)]) { # Nice smiley
      #print "abs\n";
      $retval = $object;
    }
    else {
      #print "rel\n";
      $retval = $curdir . "/" . $object;
    }
  }
  # Restore current directory
  chdir $curdir or die "Can't chdir: $!";
  
  
    
  chop($retval) while ($retval =~ m|//$|);
  
  $retval =~ s|/|\\|g;
  
  return $retval;
}




