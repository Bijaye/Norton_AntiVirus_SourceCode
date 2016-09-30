####################################################################
#                                                                  #
# Package name:  AVIS::Macro::Drives                                #
#                                                                  #
# Description:   Process Control                                   #
#                                                                  #
#                                                                  #
# Statement:     Licensed Materials - Property of IBM              #
#                (c) Copyright IBM Corp. 1998-199                  #
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
# provides common methods to manipulate SoftWindows drives (images #
# and FSA drives)                                                  #
#                                                                  #
####################################################################
package AVIS::Macro::Drives;

use File::Basename;
use File::Copy;
use File::Path;
use Cwd;

use AVIS::Macro::Image;
use Jockey;

require Exporter;
@ISA = qw(Exporter);
@EXPORT = qw(IMAGE FSADRIVE);
@EXPORT_OK = qw(Add CopyTo CopyFrom MkDir Exists);

#
# Create a new one.
#
sub new () {
  my $class = shift;      # the class to bless into
  my $self = {};          # the anonymous hash to return
  bless $self, $class;

# The "private" instance variables
  $self->{_List} = {};

# and we're done
  return $self;
}


sub IMAGE {
    return 1;
}
sub FSADRIVE {
    return 2;
}
sub UPPERCASE {
    return 3;
}
sub LOWERCASE {
    return 4;
}


sub Add ($$$$) {
  my $self = shift;      # the class to bless into
  my $letter = uc(shift);
  my $type = shift;
  my $path = shift;
  my $case = undef;

# sanity check
  if (!($letter =~ m/^\w$/)) {
      warn "Invalid value for Letter field: $letter";
      return 0;
  }
  if (!($type != &IMAGE) && !($type != &FSADRIVE)) {
      warn "Invalid value for Type field: $letter";
      return 0;
  }

  my $drive = {};
  $drive->{_Type} = $type;
  $drive->{_Path} = $path;
  $drive->{_Letter} = $letter;

  if ($type == &FSADRIVE) {
    if (@_) {
      $case = shift;
    }
    else {
      $case = &LOWERCASE;
    }
    $drive->{_Case} = $case;
  }

  $self->{_List}->{$letter} = $drive;

# and we're done
  return 1;
}


sub CopyTo () {
    my $drives = shift;
    my $orig = shift;
    my $dest = shift;
    my $isdestdir = shift if (@_);

    my $drive = $drives->GetDriveForPath($dest);
    my $ret;

    if ($drive->{_Type} == &IMAGE) {
	my $img = new AVIS::Macro::Image($drive->{_Path}, $driveletter);
        $ret = $img->Copy($orig, $dest);
    }
    elsif ($drive->{_Type} == &FSADRIVE) {
      #assumption: no wildcard (one file at a time)
      my $realdest = $drives->ConvertPath($dest);
      $realdest =~ s/\/$//;
      if ($orig =~ m/\*/) {
	  my @files = glob($orig);
          if (!-e $realdest) {
		mkpath($realdest, 0, 0755);
	  }
	  foreach $origfile (@files) {
	    my ($filename, $path, $suffix) = fileparse($origfile);
	    $destfile = $drives->ConvertPath($dest."/".$filename);
            my $thisret = !copy($origfile, $destfile);
            if ($thisret) {warn "Unable to copy $origfile to $destfile";}
	    $ret += $thisret;
	  }
          return $ret;
      }
      else {
	if ($isdestdir) {
	    if (!-e $realdest) {
		mkpath($realdest, 0, 0755)||warn "Unable to create $realdest: $!";
	    }
	}
        if (-d $realdest) {
            #only the destination directory is specified
	    my ($filename, $path, $suffix) = fileparse($orig, '\..*');
	    $realdest = $drives->ConvertPath($dest."/".$filename.$suffix);
        }
        else {
	    my ($filename, $path, $suffix) = fileparse($realdest, '\..*');
            if (!-e $path) {
		mkpath($path, 0, 0755);
	    }
	}
        $ret = !copy($orig, $realdest);
        if ($ret) {warn "Unable to copy $orig to $realdest: $!";}
      }
    }
    else {
	warn "Invalid type for drive $driveletter: ".$drive->{_Type};
        $ret = 1;
    }
    return $ret;
}


sub CopyFrom ($$$) {
    my ($drives, $orig, $dest) = @_;
    my $drive = $drives->GetDriveForPath($orig);
    my $ret;

    if ($drive->{_Type} == &IMAGE) {
	my $img = new AVIS::Macro::Image($drive->{_Path}, $driveletter);
        $ret = $img->Copy($orig, $dest);
    }
    elsif ($drive->{_Type} == &FSADRIVE) {
      my $realorig = $drives->ConvertPath($orig);
      $realorig =~ s/\/$//;
      if ($realorig =~ m/\*/) {
	  my @files = glob($realorig);
	  foreach $origfile (@files) {
	    my ($filename, $path, $suffix) = fileparse($origfile);
            my $destfile = $dest."/".$filename;
            my $thisret = !copy($origfile, $destfile);
            if ($thisret) {warn "Unable to copy $origfile to $destfile: $!";}
	    $ret += $thisret;
	  }
          return $ret;
      }
      else {
        if (-d $dest) {
          #only the destination directory is specified
	  my ($filename, $path) = fileparse($orig);
          $dest = $dest."/".$filename;
        }
        $ret = !copy($realorig, $dest);
        if ($ret) {warn "Unable to copy $realorig to $dest";}
      }
    }
    else {
	warn "Invalid type for drive $driveletter: ".$drive->{_Type};
        $ret = 1;
    }
    return $ret;
}


sub MkDir($$) {
    my ($drives, $dir) = @_;

    my $drive = $drives->GetDriveForPath($dir);
    my $ret;

    if ($drive->{_Type} == &IMAGE) {
	my $img = new AVIS::Macro::Image($drive->{_Path}, $driveletter);
        if ($img->Exists($dir)) {
	    $ret = 0;
	} else {
	    $ret = $img->Mkdir($dir);
	}
    }
    elsif ($drive->{_Type} == &FSADRIVE) {
      $realdir = $drives->ConvertPath($dir);
      if (-d $realdir) {
	  $ret = 0;
      } else {
	  $ret = !mkpath($realdir, 0, 0755);
      }
      if ($ret) {warn "Unable to mkdir $realdir: $!";}
    }
    else {
	warn "Invalid type for drive $driveletter: ".$drive->{_Type};
        $ret = 1;
    }
    return $ret;    
}


sub Exists($$) {
    my ($drives, $entry) = @_;

    my $drive = $drives->GetDriveForPath($entry);
    my $ret;

    if ($drive->{_Type} == &IMAGE) {
	my $img = new AVIS::Macro::Image($drive->{_Path}, $driveletter);
        $ret = $img->Exists($entry);
    }
    elsif ($drive->{_Type} == &FSADRIVE) {
      #assumption: no wildcard (one file at a time)
      $ret = -e $drives->ConvertPath($entry);
    }
    else {
	warn "Invalid type for drive $driveletter: ".$drive->{_Type};
        $ret = 0;
    }
    return $ret;    
}


sub GetDriveForPath {
    my ($drives, $path) = @_;

    my $driveletter = uc($path);
    ($driveletter =~ s/^(\w):.*/$1/)||warn "Error: $path is an invalid dos path";
    my $drive = $drives->{_List}->{$driveletter};
    if (!defined($drive)) {
	warn "Drive $driveletter is not defined";
    }
    return $drive;
}


sub ConvertPath($$) {
    my ($drives, $path) = @_;

    my $drive = $drives->GetDriveForPath($path);
    my $convertedpath = $path;

    if ($drive->{_Type} == &IMAGE) {
    }
    elsif ($drive->{_Type} == &FSADRIVE) {
      my $relativepath = $path;
      my $driveletter = $drive->{_Letter};
      $relativepath =~ s/$driveletter://i;
      $relativepath =~ s/\\/\//g;
      $convertedpath = $drive->{_Path}.(($drive->{_Case} == &UPPERCASE) ? uc($relativepath) : lc($relativepath));
    }
    return $convertedpath;
}


sub CreateFileOnDrive($$$) {
    my ($drives, $path, $contents) = @_;
    my $ret;

    my $drive = $drives->GetDriveForPath($path);
    my $convertedpath = $drives->ConvertPath($path);

    if ($drive->{_Type} == &IMAGE) {
	my $curdir = cwd();
	my $tmpfile = GetTempFile();
	if (open(FILE, ">".$tmpfile)) {
	    print FILE $contents;
	    close FILE;
	    $ret = $drives->CopyTo($tmpfile, $path);
	    unlink($tmpfile);
	}
	else {
	    warn "Failed to open $tmpfile: $!";
	    $ret = 1;
	}
    }
    elsif ($drive->{_Type} == &FSADRIVE) {
	if (open(FILE, ">".$convertedpath)) {
	    print FILE $contents;
	    close FILE;
	    $ret = 0;
	}
	else {
	    warn "Failed to open $convertedpath: $!";
	    $ret = 1;
	}
    }
}


sub GetTempFile() {
    my $counter = 1;
    while (-e $ENV{HOME}."/tmp$counter") {$counter++;}
    my $tmpfile = $ENV{HOME}."/tmp$counter";
    open(TMPFILE, ">$tmpfile");
    close(TMPFILE);
    return $tmpfile;
}


sub Cleanup($) {
  my $drivelist = shift;  

  my @letters = keys(%{$drivelist->{_List}});
  foreach $letter (@letters) {
      my $drive = $drivelist->{_List}->{$letter};
      if ($drive->{_Type} == &IMAGE) {
        #delete the images if not in debug mode
        my $imglink = readlink ($drive->{_Path});
        if (defined $imglink) {
          release ($imglink);
        }
        unlink($drive->{_Path})||warn "Could not unlink ".$drive->{_Path};
      }
      elsif ($drive->{_Type} == &FSADRIVE) {
        rmtree($drive->{_Path}, 0, 0);
      }      
  }
}


1;

