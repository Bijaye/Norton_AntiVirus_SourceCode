package AVIS::Emulator::Setup::VirtualDrive;

#use strict;
use Cwd;

use Jockey;
use AVIS::Emulator::Drives::Adapters::FlatImage;
use AVIS::Emulator::Drives::Adapters::FSAdrive;


#attributes
my $stockpiled = 1; #request a copy of the image from mop
my $anydrive   = 0;
$osdrive    = 2; #drive contains the operating system
$appdrive   = 4; #drive contains a directory in which the application is installed
$filedrive  = 8; #drive contains files needed for replication


#supported image types
my @stockpiled_types = ('flatimage', 'vmwimage');
my @types = (@stockpiled_types, 'fsadrive');


my %data = (
   'lasterror'    => undef,
   'letter'       => undef,    #letter associated to the drive in the DOS or Windows emulation
   'type'         => undef,    #
   'subtype'      => undef,    #
   'attributes'   => $anydrive,#
   'path'         => undef,    #path to the image file (or directory for SoftWindows FSA drives)
   'original'     => undef,    #
   'adapter'      => undef     #
);



sub new($$$$$) {
my $arg = shift;
my $class = ref($arg)||$arg;

my ($type, $subtype, $letter, $attributes) = @_;
my $self = { %data };  
  
  $self->{'type'}        = $type;
  $self->{'subtype'}     = $subtype;
  $self->{'letter'}      = $letter;
  $self->{'attributes'}  = $attributes;  
  
  if (not grep /^$type$/, @types) {  
    my $msg = "new($class, @_) failed: $type is not a supported image type";
    warn $msg;
    return undef;
  }
  if (grep /^$type$/, @stockpiled_types) {  
    #mark the image as stockpiled
    $self->{'attributes'} |= $stockpiled;
    #fetch it here?
  }
    
  return bless $self, $class;
}


##################################################
#
# Common Interface
#

sub SetLastError($$) {
  my $self = shift;
  return ($self->{lasterror} = shift);
}

sub GetLastError($) {
  my $self = shift;
  return $self->{lasterror};
}


#return the path (and set it if needed)
sub Path() {
  my $self = shift;

  $self->{'path'} = shift if @_;
  return $self->{'path'};
}

sub Attributes($) {
  my $self = shift;  
  return $self->{'attributes'};
}

sub Letter($) {
  my $self = shift;
  return $self->{'letter'};
}

sub Subtype($) {
  my $self = shift;
  return $self->{'subtype'};
}

sub Type($) {
  my $self = shift;
  return $self->{'type'};
}

sub Original($) {
  my ($self, $original) = @_;
  return ($self->{'original'} = $original);
}

sub Adapter {
  my $self = shift;
  
  $self->{'adapter'} = shift if @_;
  return $self->{'adapter'};  
}


sub ExtractAdapter($) {
  my $self = shift;
  
  warn "Extracting adapter for ".$self->Letter;
  if ($self->Attributes & $stockpiled) {
    $self->RequestCopy();
  }
  my $vdrive;
  if ($self->Type eq 'flatimage') {
    $vdrive = new AVIS::Emulator::Drives::Adapters::FlatImage($self->Path, $self->Letter);
  }
  elsif ($self->Type eq 'fsadrive') {
    $vdrive = new AVIS::Emulator::Drives::Adapters::FSAdrive($self->Path, $self->Letter);
  }
  else {
    die "Unknown type : ".$self->Type;
  }
  die "Failed to extract interface" if not defined $vdrive;
  $self->Adapter($vdrive);
  return $vdrive;
}


sub RequestCopy($) {
my $self = shift;

  my $driveimgtype = $self->Subtype;
  if ($driveimgtype =~ m/^(.+?)\.$isvalImageExt$/) {
    my $realdriveimg = Jockey::fetch($1);
    warn "Requesting copy of $driveimgtype [".$self->Subtype."] for ".$self->Letter;
    my $driveimg = $self->Path;
    if (((-e $driveimg) && unlink($driveimg))||(!-e $driveimg)) {
      die("symlink not available on this system") unless eval { symlink("",""); 1 };
      symlink($realdriveimg, $driveimg)||die "symlink($realdriveimg, $driveimg) failed: $!";
    }
    else {
      die "in great pain";
    }
  }
  else {
      die "subtype not defined; dying in great pain";
  }
}


sub Cleanup($) {
my $self = shift;

  if ($self->Attributes & $stockpiled) {#in case drive is stockpiled
      my $imglink = readlink($self->Path);
      #release image
      $self->Adapter->Cleanup();
      if (defined $imglink) {
        release($imglink);
      }
  } else {
      $self->Adapter->Cleanup();
  }
}


1;
