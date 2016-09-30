package AVIS::Emulator::Setup::SW95;

use strict;

use Cwd;

use AVIS::Emulator::Setup::Drives;
use AVIS::Emulator::Setup::VirtualDrive;

my $isfnsw95config = 'swincfg';

my %data = (
   lasterror => undef,
   entries => [],
   drives => undef,
   dir => undef
);


sub new() {
my $arg = shift;
my $class = ref($arg)||$arg;

  my $self = { %data };
  $self->{'drives'} = new AVIS::Emulator::Setup::Drives;
  $self->{'dir'} = @_ ? shift : cwd();
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

sub Defaults($) {
}

sub AddEntry($$$) {
  my ($self, $entryname, $entryvalue) = @_;

  my $entry = { 'name' => $entryname, 'value' => $entryvalue};
  push @{$self->{'entries'}}, $entry;
}

sub GetEntry($$) {
  my ($self, $entryname) = @_;

  foreach (@{$self->{'entries'}}) {
    return $_ if ($_->{'name'} eq $entryname);
  }
  return undef;
}

sub ModifyEntry($$$) {
  my ($self, $entryname, $newentryvalue) = @_;

  my $entry = $self->GetEntry($entryname);
  if (! defined $entry) {
    #complain here
    return 0;
  }
  $entry->{'value'} = $newentryvalue;
  return 1;
}

sub Import($$) {
  my ($self, $file) = @_;
  
  my $rc = 1;
  if (!open CFGFILE, $file) {
    my $errmsg = "Unable to read from $file: $!";
    warn $errmsg; $self->SetLastError($errmsg);    
    return 0;
  }
  while(<CFGFILE>) {
    chomp();
    if (m/^(.*?)\s+(.*)/) { $self->AddEntry($1, $2);}
    elsif (m/^(.*?)\s*/) { $self->AddEntry($1, undef);}
  }
  close CFGFILE;
  
  return $rc;
}  

sub Dump($) {
  my $self = shift;

  my $file = $self->{'dir'}."/$isfnsw95config";
  #write the whole thing
  if (not open CFGFILE, ">$file") {
    my $msg = "Failed to write to $file: $!";
    warn $msg; $self->SetLastError($msg);
    return 0;
  }
  my $entry;
  foreach $entry (@{$self->{entries}}) {
    my $entryname = $entry->{'name'};
    my $entryvalue = $entry->{'value'};
    $entryvalue = "" unless defined $entryvalue;
    print CFGFILE $entryname, "\t", $entryvalue, "\n";
  }
  close CFGFILE;
  return 1;
}

sub GetCmdlineArgs($) {
  my $self = shift;

  return ('-config', $self->{'dir'}."/$isfnsw95config");
}

sub FinalizeSetup($) {
  my $self = shift;
    
  #for use of unix FS mounted as network drive
  my $drivelist = $self->DriveList();
  my $lines = "c:\\insignia\\fsadrive.com\n";
  $lines =~ s/\n/\r\n/g;
  $drivelist->Adapters->CreateFileOnDrive("c:\\autoexec.bat", $lines);
  $lines = "device=c:\\insignia\\host.sys\n";
  $lines =~ s/\n/\r\n/g;
  $drivelist->Adapters->CreateFileOnDrive("c:\\config.sys", $lines);
  return 1;
}


sub ModifyDriveEntry($$) {
  my $self = shift;
  my $virtualdrive = shift;

  my $entryname = 'DRIVE_'.uc($virtualdrive->Letter);
  if ($virtualdrive->Type eq 'flatimage') {
    $entryname .= '_FILE_NAME';
  }
  elsif ($virtualdrive->Type eq 'fsadrive') {
    $entryname .= '_FSA_DIRECTORY';
  }
  else {
    my $errmsg = "Unknown type for drive @_: ".$virtualdrive->Type;
    warn $errmsg; $self->SetLastError($errmsg);
    return 0;
  }
  $self->ModifyEntry($entryname, $virtualdrive->Path);
}


sub AddDrive($$) {
  my $self = shift;
  my $virtualdrive = shift;

  $self->ModifyDriveEntry($virtualdrive);      
  return $self->DriveList->Add($virtualdrive);
}


sub DriveList {
  my $self = shift;
  if (@_) {    
    my $list = shift;
    foreach ($list->List) {
      $self->ModifyDriveEntry($_);
    }
    $self->{'drives'} = $list;
  }
  return $self->{'drives'};
}


1;
