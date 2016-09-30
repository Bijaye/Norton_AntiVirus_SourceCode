####################################################################
#                                                                  #
# Package name:  AVIS::Emulator::Drives::Adapters::FSAdrive        #
#                                                                  #
# Description:   Access to SoftWindows95 FSA drives                #
#                                                                  #
#                                                                  #
# Statement:     Licensed Materials - Property of IBM              #
#                (c) Copyright IBM Corp. 1999                      #
#                                                                  #
# Author:        Jean-Michel Boulay                                #
#                                                                  #
#                U.S. Government Users Restricted Rights - use,    #
#                duplication or disclosure restricted by GSA ADP   #
#                Schedule Contract with IBM Corp.                  #
#                                                                  #
#                                                                  #
####################################################################
#                                                                  #
# Function:                                                        #
# provides methods to manipulate SoftWindows FSA drives            #                                       #
# derived from AVIS::Emulator::Drives::Adapter                     #
#                                                                  #
####################################################################
package AVIS::Emulator::Drives::Adapters::FSAdrive;

require AVIS::Emulator::Drives::Adapter;
@ISA = qw(AVIS::Emulator::Drives::Adapter);

use strict;

use File::Basename;
use File::Copy;
use File::Path;


sub UPPERCASE {
    return 1;
}
sub LOWERCASE {
    return 2;
}

#
# Create a new one.
#
sub new {
  my $class = shift;
    
  my $path = shift;
  my $letter = shift;
  my $case = shift||LOWERCASE();
  
  my $self = new AVIS::Emulator::Drives::Adapter('fsadrive', $letter, $path);

  return undef if not defined $self;
  if ((!-d $path) && !mkpath($path, 0, 0755)) {
    warn "Unable to create $path: $!";
    return undef;
  }
  $self->{'case'} = $case;
  
  return bless $self, $class;
}


sub Case {
my $self = shift;
  
  my $case = (@_) ? shift : $self->{'case'};
  return $case;
}


sub Copy {
my $self      = shift;
my $src       = shift;
my $dest      = shift;
my $isdestdir = shift if (@_);

  my ($ret, $srcfile, $destfile);
  my $issrcondrive  = IsDOSPath($src);
  my $isdestondrive = IsDOSPath($dest);
 
  my $realsrc   = $issrcondrive  ? $self->ConvertPath($src)  : $src;
  my $realdest  = $isdestondrive ? $self->ConvertPath($dest) : $dest;
  
  $realdest =~ s/\/$//;
  if ($realsrc =~ m/\*/) { #wildcard; destination is a directory
    my @files = glob($realsrc);
    if (!-e $realdest) {
		mkpath($realdest, 0, 0755);
    }
    foreach $srcfile (@files) {
      my ($filename, $path, $suffix) = fileparse($srcfile);
      $destfile = $isdestondrive ? $self->ConvertPath("$dest/$filename") : "$dest/$filename";
      my $thisret = !copy($srcfile, $destfile);
      if ($thisret) {
        my $errmsg = "Unable to copy $srcfile [$src] to $destfile [$dest]: $!";
        warn $errmsg; $self->SetLastError($errmsg);
      }
      $ret += $thisret;
    }
  }
  else {
	 if ($isdestdir) {
	   if (!-e $realdest) {
		  mkpath($realdest, 0, 0755)||warn "Unable to create $realdest: $!";
	   }
	 }
    if (-d $realdest) {
      #only the destination directory is specified
	   my ($filename, $path, $suffix) = fileparse($src, '\..*');
	   $realdest = $isdestondrive ? $self->ConvertPath("$dest\\$filename$suffix") : "$dest/$filename$suffix";
    }
    else {
	   my $path = dirname($realdest);
      if (!-e $path) {
	     if (!mkpath($path, 0, 0755)) {
          my $errmsg = "Unable to create directory $path: $!";
          warn $errmsg; $self->SetLastError($errmsg);
        }
	   }
	 }
    $ret = !copy($realsrc, $realdest);
    if ($ret) {
      my $errmsg = "Unable to copy $realsrc [$src] to $realdest [$dest]: $!";
      warn $errmsg; $self->SetLastError($errmsg);
    }
  }
  return $ret;
}


sub MkDir($$) {
my ($self, $dir) = @_;

  my $ret;
  my $realdir = $self->ConvertPath($dir);
  if (-d $realdir) {
    $ret = 0;
  } else {
    $ret = !mkpath($realdir, 0, 0755);
  }
  if ($ret) {
    my $errmsg = "Unable to mkdir $realdir [$dir]: $!";
    warn $errmsg; $self->SetLastError($errmsg);
  }
  return $ret;    
}


sub Exists($$) {
    my ($drives, $entry) = @_;

    my $ret;
    #assumption: no wildcard (one file at a time)
    $ret = -e $drives->ConvertPath($entry);
    return $ret;    
}


sub IsDOSPath($) {
  my $path = shift;
  return ($path =~ m/^\w:/);
}


sub ConvertPath($$) {
    my ($self, $path) = @_;

    my $convertedpath;
    (my $relativepath = $path) =~ s/\\/\//g;    
    my $driveletter = $self->Letter;
    if ($relativepath =~ s/^$driveletter://i) {
      $convertedpath = $self->Path.(($self->Case == &UPPERCASE) ? uc($relativepath) : lc($relativepath));
      $convertedpath =~ s/\/$//;#remove a possible trailing /
    }
    return $convertedpath;
}


sub CreateFileOnDrive($$$) {
my ($self, $path, $contents) = @_;
  
  my $convertedpath = $self->ConvertPath($path);
  if (open(FILE, ">$convertedpath")) {
	 print FILE $contents;
	 close FILE;
	 return 0;
  }
  else {
	 my $errmsg = "Failed to open $convertedpath [$path]: $!";
    warn $errmsg; $self->SetLastError($errmsg);
	 return 1;
  }
}


sub Cleanup($) {
my $self = shift;  

  rmtree($self->Path, 0, 0);
}


1;

