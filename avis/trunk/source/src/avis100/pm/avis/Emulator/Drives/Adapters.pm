####################################################################
#                                                                  #
# Package name:  AVIS::Emulator::Drives::Adapters                  #
#                                                                  #
# Description:   Operation on a set of adapters                    #
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
package AVIS::Emulator::Drives::Adapters;

use strict;

#require Exporter;
#@ISA = qw(Exporter);
#@EXPORT = qw();
#@EXPORT_OK = qw(Add CopyTo CopyFrom MkDir Exists);


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



sub Add ($$) {
  my ($self, $adapter) = @_;

  $self->{_List}->{uc($adapter->Letter)} = $adapter;
  return 1;
}


sub CopyTo {
    my $drives = shift;
    my $src = shift;
    my $dest = shift;
#    my $isdestdir = shift if (@_);

    my $drive = $drives->GetDriveForPath($dest);

    return 1 if not defined $drive;
    return $drive->Copy($src, $dest);
}


sub CopyFrom ($$$) {
    my ($drives, $src, $dest) = @_;
    my $drive = $drives->GetDriveForPath($src);

    return 1 if not defined $drive;
    return $drive->Copy($src, $dest);
}


sub MkDir($$) {
    my ($drives, $dir) = @_;

    my $drive = $drives->GetDriveForPath($dir);

    return 1 if not defined $drive;
    return $drive->MkDir($dir);
}


sub Exists($$) {
    my ($drives, $entry) = @_;

    my $drive = $drives->GetDriveForPath($entry);
    return 0 if not defined $drive;
    return $drive->Exists($entry);
}


sub GetDriveForPath {
    my ($drives, $path) = @_;

    my $driveletter = uc($path);
    ($driveletter =~ s/^(\w):.*/$1/)||warn "Error: $path is an invalid dos path";
    my $drive = $drives->{_List}->{$driveletter};
    if (!defined($drive)) {
	   print STDERR "Drive $driveletter is not defined; ", keys(%{$drives->{_List}}), "\n";
    }
    return $drive;
}



sub CreateFileOnDrive($$$) {
    my ($drives, $path, $contents) = @_;
    my $ret;

    my $drive = $drives->GetDriveForPath($path);
    return 1 if not defined $drive;
    return $drive->CreateFileOnDrive($path, $contents);
}



sub Cleanup($) {
  my $drivelist = shift;  

  my @letters = keys(%{$drivelist->{_List}});
  foreach (@letters) {
    my $drive = $drivelist->{_List}->{$_};
    $drive->Cleanup();
  }
}


1;

