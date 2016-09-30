####################################################################
#                                                                  #
# Package name:  AVIS::Emulator::Drives::Adapter                   #
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
package AVIS::Emulator::Drives::Adapter;


use strict;


#supported image types
my @types = ('flatimage', 'vmwimage', 'fsadrive');

my %data = (
   'lasterror' => undef,
   'letter'    => undef,    #letter associated to the drive in the DOS or Windows emulation
   'type'      => undef,    #
   'path'      => undef,    #path to the image file of directory (SoftWindows FSA drives only)
);



sub new {
my $arg = shift;
my $class = ref($arg)||$arg;

my ($type, $letter, $path) = @_;
my $partition = (@_) ? shift : 1;

my $self = { %data };  
  
  $self->{'type'}        = $type;
  $self->{'path'}        = $path;
  $self->{'letter'}      = $letter;
  
  if (not grep /^$type$/, @types) {  
    my $msg = "new($class, @_) failed: $type is not a supported image type";
    warn $msg;
    return undef;
  }
    
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


#return the path (and set it if needed)
sub Path() {
  my $self = shift;

  $self->{'path'} = shift if @_;
  return $self->{'path'};
}

sub Letter($) {
  my $self = shift;
  return $self->{'letter'};
}

sub Type($) {
  my $self = shift;
  return $self->{'type'};
}


#dummy manipulation methods
sub Copy {
  return 1; #return failure
}
sub Dir {
  return undef;
}
sub Mkdir {
  return 1; #return failure
}
#for existence tests
sub Exists {
  return undef;
}


1;
