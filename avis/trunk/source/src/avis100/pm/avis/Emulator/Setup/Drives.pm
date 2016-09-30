####################################################################
#                                                                  #
# Package name:  AVIS::Emulator::Setup::Drives                     #
#                                                                  #
# Description:   Base class for virtual drives adapters            #
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
package AVIS::Emulator::Setup::Drives;


use strict;

use Cwd;
use File::Path;

use AVIS::Emulator::Setup::VirtualDrive;
use AVIS::Emulator::Drives::Adapters;


my %data = (
   'lasterror' => undef,
   'list' => [],
   'root' => undef,
   'adapters' => undef
);


sub new {
my $arg = shift;
my $class = ref($arg)||$arg;

  my $root = (@_) ? shift : cwd();
  my $self = { %data };
  $self->{'root'} = $root;  

  return bless $self, $class;
}


##################################################
#
# Common Interface
#

sub SetLastError($$) {
  my $self = shift;
  return ($self->{'lasterror'} = shift);
}

sub GetLastError($) {
  my $self = shift;
  return $self->{'lasterror'};
}


sub Add($$) {
  my $self = shift;  
  my $virtualdrive = shift;
  
  if (ref($virtualdrive) ne 'AVIS::Emulator::Setup::VirtualDrive') {
    my $msg = "Add failed: $virtualdrive is not an AVIS::Emulator::Setup::VirtualDrive object";
    warn $msg; $self->SetLastError($msg);
    return 0;
  }

  $virtualdrive->Path($self->{'root'}."/vdrive".$virtualdrive->Letter);
  push @{$self->{'list'}}, $virtualdrive;
  
  return 1;
}


sub List($) {
  my $self = shift;
  return @{$self->{'list'}};
}

sub GetFlaggedDriveLetter($$) {
  my ($self, $flag) = @_;
  foreach ($self->List) {
    return $_->Letter if $_->Attributes & $flag;
  }
  return undef;
}

sub GetAppDriveLetter($) {
  my $self = shift;
  return GetFlaggedDriveLetter($self, $AVIS::Emulator::Setup::VirtualDrive::appdrive);
}

sub GetOSdriveletter($) {
  my $self = shift;
  return GetFlaggedDriveLetter($self, $AVIS::Emulator::Setup::VirtualDrive::osdrive);
}

sub GetFileDriveLetter($) {
  my $self = shift;
  return GetFlaggedDriveLetter($self, $AVIS::Emulator::Setup::VirtualDrive::filedrive);
}

sub GetDriveSubtype($$) {
  my ($self, $letter) = @_;
  foreach (@{$self->{'list'}}) {
    return $_->Subtype if $_->Letter eq uc($letter);
  }
  return undef;
}


sub ExtractAdapters($) {
my $self = shift;

  my $adapters = new AVIS::Emulator::Drives::Adapters;
  my $virtualdrive;
  foreach $virtualdrive ($self->List) {
    if (not defined $virtualdrive->Adapter) {
      $virtualdrive->ExtractAdapter();
    }
    $adapters->Add($virtualdrive->Adapter);
  }
  $self->Adapters($adapters);
  return $adapters;
}


sub Adapters {
my $self = shift;

  if (@_) {
    return ($self->{'adapters'} = shift);
  }
  else {
    return $self->{'adapters'};
  }
}


sub Cleanup($) {
  my $self = shift;  

  foreach ($self->List) {
    $_->Cleanup();
  }
}


1;
