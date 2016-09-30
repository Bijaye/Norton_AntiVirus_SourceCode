####################################################################
#                                                                  #
# Program name:  pm/AVIS/VMtools/VMTools.pm                        #
#                                                                  #
# Package name:  AVIS::VMtools                                     #
#                                                                  #
# Description:   MSDOS file access managment for AVIS              #
#                                                                  #
# Statement:     Licensed Materials - Property of IBM              #
#                (c) 1998-1999                                     #
#                                                                  #
# Author:        Stefan Tode                                       #
#                based on AVIS::Logger                             #
#                                                                  #
# And also:      U.S. Government Users Restricted Rights - use,    #
#                duplication or disclosure restricted by GSA ADP   #
#                Schedule Contract with IBM Corp.                  #
#                                                                  #
#                                                                  #
####################################################################
#                                                                  #
# Function:                                                        #
#                                                                  #
# Access MSDOS/Fat16/32 formated diskimages                        #
# used mtools program mcopy.exe mdir.exe must be in the path       #
#		                                                             #
####################################################################

package AVIS::VMTools;

use AVIS::FSTools();    # For Canonize() etc
use FileHandle;         # To get a simple storable filehandle for local files
use Fcntl ":flock";     # To get nice flock symbols
use Cwd;

#
# The usual oo-perl new() routine
#
sub new {
  my $class = shift;      # the class to bless into
  my $self = {};          # the anonymous hash to return
  bless $self, $class;
  $self->_empty(@_);      # initialize the variables
# and we're done
  return $self;
}

#
# Set all variables pristine; done at new() and DESTROY() times.
#
sub _empty {
  my $self = shift;
  my $unc = undef;          # So I'm paranoid!
  $unc = shift if @_;       # the argument we were given, if any
# Simple instance variables
  $self->{localFile} = "";
  $self->{orgMtoolsSrc} = "";
  $self->{subcode} = "";
  $self->{unc} = (defined $unc) ? $unc : "" ;
# The "private" instance variables
  $self->{_localHandle} = undef;
  return;
}

#
# When a local file is set, make sure it can be opened.
#
sub localFile {
  my $self = shift;

  # undocumented zero-argument query function
  return $self->{localFile} if not @_;   # Query function

  # otherwise take the argument
  my $newvalue = shift;

  # return at once if it's already in effect
  return $newvalue if $newvalue eq $self->{localFile};

  # otherwise, if a handle is already open, close it
  if (defined $self->{_localHandle}) {
    $self->{_localHandle}->close();
    $self->{_localHandle} = undef;
    $self->{localFile} = "";
  }

  # if the argument is "", we're done
  if ($newvalue eq "") {
    $self->{_localHandle} = undef;
    $self->{localFile} = "";
    return 1;    # always works!
  }

  # otherwise try to open a new handle
  $self->{_localHandle} = new FileHandle(">> $newvalue");
  $self->{_localHandle}->autoflush(1);

  # error if fails.
  if (not defined $self->{_localHandle}) {
    warn "Error opening local file [$newvalue].\n";
    $self->{localFile} = "";
    return 0;
  }

  # non-error if works
  $self->{localFile} = $newvalue;

  return 1;

}

#
# The routine that adds a drive
#
# $msdos->VAddDrive("C","C:\\tools\\bochs\\harddrv.IMG",$drive_options);

sub VAddDrive {
  my $self = shift;
  my $driveletter = shift;
  my $imgname = shift;
  my $other = shift;
  my $string;
  $self->{orgMtoolsSrc} = $ENV{MTOOLSRC};  # save the environment variable
  $ENV{MTOOLSRC} = $self->{localFile};     # set to the current configuration file

  $string = $self->_genMtoolsLine($driveletter,$imgname,$other);
  print $string if $self->{screenEcho};
  return 0 unless ($self->_logToGlobal($string));
  return 0 unless ($self->_logToLocal($string));
  $ENV{MTOOLSRC} = $self->{orgMtoolsSrc}; #
  return 1;
}

sub Vdir {
   # don't use recursive switch if you enter the filename with vdir
   #$msdos->Vdir("C:\\system\\system32\\d32e.exe","");

   my $self = shift;
   #my $drive = shift;
   my $src = shift;
   my $options = shift;
   $self->{orgMtoolsSrc} = $ENV{MTOOLSRC};
   $ENV{MTOOLSRC} = $self->{localFile};
   
   my $mycmd1 = "mdir $options";
   #my $mycmd2 = "$drive:\\$src";
   my $mycmd2 = "$src";
   $mycmd2 =~ tr|a-z|A-Z|;
   #$mycmd2 =~ s|/|\\|g;
   my $mycmd = "$mycmd1 $mycmd2";
   #print "doing: $mycmd\n";
   my $rc = system($mycmd);  
   $ENV{MTOOLSRC} = $self->{orgMtoolsSrc};

   return ($rc);
}

sub Vdel {
   # $msdos->Vdel("c:\\system\\system32\\d32_e.exe");   
   # bug in vdel removed
   # (((it removes the last character of the file to delete)))
   my $self = shift;
   my $src = shift;
   $self->{orgMtoolsSrc} = $ENV{MTOOLSRC};
   $ENV{MTOOLSRC} = $self->{localFile};
   
   my $mycmd1 = "mdel";
   my $mycmd2 = "$src"; #."e";  # mdel.c seems to cut one letter in the end, so append one
   $mycmd2 =~ tr|a-z|A-Z|;              # convert destination to uppercase (no longfilename)
   #$mycmd2 =~ s|/|\\|g;
   my $mycmd = "$mycmd1 $mycmd2";
   #print "doing: $mycmd\n";
   my $rc = system($mycmd);  
   $ENV{MTOOLSRC} = $self->{orgMtoolsSrc};

   return ($rc);
}

sub Vread {
   # copies files from an specified image to the specified directory
   #
   # $msdos->Vread("A:\\*","D:\\TEMP","-/");
   # copy recursive all files from the image C beginning from the root
   # $msdos->Vread("C:\\*","D:\\TEMP\\C","-/"); # don't use "-b" switch

   my $self       = shift;
   my $srcfile    = shift;
   my $dstfile    = shift;
   my $CmdOptions = shift;
   my $mycmd2;
   
   $self->{orgMtoolsSrc} = $ENV{MTOOLSRC};
   my $homedir = cwd;                                 # save the current directory
   my $newdir = $dstfile;
   my $rc = chdir File::Basename::dirname($newdir);   # change to destination directory
   if ($rc == 1){                                     # if we could change to the destination
	   $newdir =~ s/^[A-Za-z]://;                      # remove the drive letter
	   $ENV{MTOOLSRC} = $self->{localFile};
	   $newdir =~ s|/|\\|g;
	   my $mycmd1 = "mcopy $CmdOptions";
      $mycmd2 = "$srcfile $newdir"; #if ($srcfile =~ m/^[A-Za-z]:\\/);  # drive letter included   
      my $mycmd = "$mycmd1 $mycmd2";
	   #$mycmd =~ s|/|\\|g;
	   #print "doing: $mycmd\n";
	   my $rc = system($mycmd);                        # execute command
	   chdir $homedir;                                 # change back to last directory
      	   $ENV{MTOOLSRC} = $self->{orgMtoolsSrc};
   }  
   return ($rc);
}


sub Vxread {
   # xcopy like function for mtools
   # recursive copy a disk from the root to the specified directory
   # INPUT :
   #         $drive = the drive which to copy "C:" or "C:\\"
   #         $dstdir = the destination directory (where to copy the files)
   #         $CmdOptions = "-X -a" for recursive, all files copy
   #
   # RETURN: 1 is success
   #         0 if failed 
   #
   # msdos->Vxread("C:","D:\\TEMP",$CmdOptions);
   # to copy all files recuresivly use only "A:" and $CmdOptions = "-X -a"
   # msdos->Vxread("C:","D:\\TEMP","-X -a");
   # copy recursive all files from the image C beginning from the root
   # to the destination directory "D:\\TEMP\\C"
   #
   # NOTICE:
   # Existing files doesn't get overwritten
   # creates a temporary file ($isfnFileList) in the $dstdir directory
   # creates a subdirectory under the destination directory (the name is the $drive)
   #

   my $self       = shift;
   my $drive      = shift;
   my $dstdir     = shift;
   my $CmdOptions = shift;  # use for recursive and all files "-X -a"
   my $mycmd2;
   my $file,$file2,$newdir,$cmd;
   my $copy_options = "";
   my $rc = 0;

   my $isfnFileList = "$dstdir\\filelist.999";
   $self->Vdir("$drive >$isfnFileList",$CmdOptions);   
   if (open (FILELIST, $isfnFileList)) {
      while(<FILELIST>) {
         chomp ($file = $_);
         if ($file =~ m/.+\\$/) {               # is it a dir
             $newdir = $file;
             $newdir =~ s/(.*)(:)(.*)/$1$3/;    # remove the ":" in C:/path\\
             $newdir =~ s/(.+)\\$/$1/;          # remove the "\\"
             $cmd = "$dstdir\\$newdir"; # append path
             $cmd =~ s/[\\\/]/\\/g;             # change to "\\"
             File::Path::mkpath($cmd,1,0777);   # make directory
         } 
         else {
             $file =~ s/[\\\/]/\\/g;            # change to "\\"
             $file2 = $file;
             $file2 =~ s/(.*)(:)(.*)/$1$3/;     # remove the ":" in C:\\path\\filename
             $file2 = $dstdir . "\\" . $file2; # append filename
             $file2 =~ s/[\\\/]/\\/g;                  # change to "\\"
             if (!-e $file2) {
                $self->Vread("$file","$file2",$copy_options); # don't use "-b" switch
             }
             else {
                warn("vmtools: Couldn't overwrite file $file2\n");
             }
         } 

       }
       close FILELIST;
       unlink $isfnFileList;
       $rc = 1;
    }
    else { 
       $rc = 0;
    }

   return ($rc);
}


sub Vwrite {
   # copies Files into the image
   # msdos->Vwrite("D:\\TEMP\\SYS.COM","A:\\DOS\\SYS.COM",$CmdOptions);
   #
   my $self       = shift;
   #my $drive      = shift;
   my $srcfile    = shift;
   my $dstfile    = shift;
   my $CmdOptions = shift;
   
   $self->{orgMtoolsSrc} = $ENV{MTOOLSRC};
   $dstfile =~ tr|a-z|A-Z|;                           # convert destination to uppercase (no longfilename)
   my $homedir = cwd;                                 # save the current directory
   my $rc = chdir File::Basename::dirname($srcfile);  # change to destination directory
   my $src = File::Basename::basename($srcfile);
   if ($rc == 1){                                     # if we could change to the destination
      $ENV{MTOOLSRC} = $self->{localFile};     
      my $mycmd1 = "mcopy $CmdOptions";
#      my $mycmd2 = "$srcfile $drive:\\$dstfile";
      
      ##my $mycmd2 = "$src $drive:\\$dstfile";
      # if ($dstdrive !~ m/^[A-Za-z]:\\/); # drive missing
      my $mycmd2 = "$src $dstfile";
      # if ($dstdrive =~ m/^[A-Za-z]:\\/);  # drive letter included
      my $mycmd = "$mycmd1 $mycmd2";
      #$mycmd =~ s|/|\\|g;
      #print "mtools input: $srcfile,$dstfile\n";
      #print "doing: $mycmd\n";
	   my $rc = system($mycmd);                        # execute command
	   chdir $homedir;                                 # change back to last directory
      $ENV{MTOOLSRC} = $self->{orgMtoolsSrc};
   }
   return ($rc);

}

#
# Subroutine that logs to the local file, if any
#
sub _logToLocal {
  my $self = shift;
  my $string = shift;
  return 1 if $self->{localFile} eq "";
  $self->{_localHandle}->print($string) or return 0;
  return 1;
}

#
# Take a raw string and make it nice for the log.
#
sub _genMtoolsLine {
  my $self = shift;
  my $driveletter = shift;
  my $imgname = shift;
  my $other = shift;

  return sprintf("drive %s: file=\"%s\"%s\n",$driveletter,$imgname,$other);
}

#
# Subroutine that logs to the global file, if any
#
sub _logToGlobal {
  my $self = shift;
  my $string = shift;
  my $logname = $self->_logName();
  my $tries;
  my $locked;
  my $locks_exist = 1;
  local *FH;

  return 1 if $self->{unc} eq "";

  return 0 unless open FH, ">> $logname";
  for ($tries=0;$tries<5;$tries++) {
    # try to get a lock
    $locked = eval { flock FH, LOCK_EX | LOCK_NB; };
    # if eval failed, flock probably not supported
    if (not defined $locked) {
      warn "flock not supported here; ignored";
      $locks_exist = 0;
      last;      # Continue blithely on
    }
    last if $locked;
    warn "Waiting for log file to be available...";
    sleep(5);
  }
  return 0 if $locks_exist and not $locked;
  unless (print FH $string) {
    close FH;
    flock FH, LOCK_UN if $locked;
    return 0;
  }
  close FH;
  flock FH, LOCK_UN if $locked;

  return 1;
}

#
# Mapping of a unc and subcode to a global filename
#
sub _logName {
  my $self = shift;
  my $token = $self->{subcode} eq "" ? "LOG0" : $self->{subcode};
  return $self->_logPattern($token);
}

#
# Mapping to a global filename, or globbing pattern
#
sub _logPattern {
  my $self = shift;
  my $input = shift;
  my $answer = sprintf "AVIS%s.LOG", $input;
  return AVIS::FSTools::Canonize($answer,$self->{unc});
}

#
# Routine that returns the list of log files
#
sub getLogList {
  my $self = shift;
  my $pattern = $self->_logPattern("*");
  my $answer = [<"$pattern">];   # These quotes are important!!
  return [] if ($answer->[0] eq $pattern);
  return $answer;
}


#
# Called when we go away, or explicitly.  Just close up any open
# handle we might happen to have, and unset variables.
#
sub DESTROY {
  my $self = shift;
  if (defined $self->{_localHandle}) {
    $self->{_localHandle}->close();
    $self->{_localHandle} = undef;
    unlink($ENV{MTOOLSRC});
    $ENV{MTOOLSRC} = $self->{orgMtoolsSrc};
  }
  $self->_empty();
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


__END__

=head1 NAME


Package: AVIS::MTools

Filename: pm/AVIS/MTools/MTools.pm

Tools for manipulating files and directories on MSDOS / FAT16 / FAT32 drives
AVIS::VMTools == Virus image MSDOS Tools management for AVIS.



=head1 SYNOPSIS

 # simplest use

 my $drive_options = " partition=1";  # harddisk option
 my $msdos = new AVIS::VMTools("");
 ...
 $msdos->localFile("D:\\imagescfg\\msdisk3.cfg"); # configuration file
 $msdos->VAddDrive("C","C:\\images\\harddrv.IMG",$drive_options);
 ...
 $msdos->Vdir("C:\\system","");
 $msdos->Vread("C:\\*","D:\\TEMP\\C","-/"); # read files from image
 $msdos->Vxread("C:\\","D:\\TEMP","-X -a"); # recursive read all files from image to D:\\TEMP\\C
 ...
 $msdos2->Vwrite("D:\\D32.EXE","c:\\system\\d32e.exe",""); # write into image
 $msdos->Vdel("c:\\system\\d32_e.exe"); # delete file in image
 ...
 return;               # Probably an implicit DESTROY happens here


=cut


