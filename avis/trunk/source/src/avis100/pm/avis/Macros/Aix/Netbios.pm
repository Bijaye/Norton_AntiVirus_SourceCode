#for Netbios
#a few remarks about Aix connections:
#-only root can mount drives as of version number 1.6 
#-if the -d option is not used when root tnclogins, auto has to login
# each time it wants to access the mounted drive
package AVIS::Macro::Netbios;


require Exporter;
@ISA = qw(Exporter);
@EXPORT = qw();
@EXPORT_OK = @EXPORT;


use strict;
use Net::FTP;
use File::Path;
use File::Copy;
use File::Find;

use AVIS::Macro::Connection;

my $timeout = 1200;
my $verbose = 0;


sub new($$) {
    my ($class, $unc) = @_;

    my $self = {};
    
    my ($server, $volume, $relpath) = AVIS::Macro::Connection::UnpackUnc($unc);
    return undef if !defined($server) or !defined($volume);
    $self->{Server} = $server;
    $self->{Volume} = $volume;
    $self->{Relpath} = $relpath;
    $self->{Unc} = $unc;
    $self->{LastError} = "";
    $self->{CONNECTION} = undef;    
    $self->{Connected} = 0;
    $self->{DirectAccess} = 1;
    return bless $self, $class;
}


sub AllowsDirectAccess($) {
  my $self = shift;
  return $self->{DirectAccess};  
}


sub InitializeConnection($) {
  my $self = shift;

  return 1;
}

sub ShutdownConnection($) {
  my $self = shift;
  
  return 1;
}


#check whether the unc is accessible on the server
sub IsAccessible($) {
  my $self = shift;

  my $aconn = $self->{CONNECTION};
  my $unc = $self->{Unc};
  my $trunc = $aconn->TranslateUnc($unc);
  # check whether the UNC directory is indeed accessible
#  if (!$aconn->IsAccessible()||(!$aconn->IsActive())) {
  if (!$aconn->IsActive()) {
    sleep(60);
#    if (!$aconn->IsAccessible()||(!$aconn->IsActive())) {
    if (!$aconn->IsActive()) {
      my $errmsg .= "The connection to ".$aconn->{Volume}." on ".$aconn->{Server}." is not working. ".$aconn->{LastError}."\n";
      warn $errmsg; $self->SetLastError($errmsg);
      return 0;
    }
  }
  if (!-e $trunc) {
    my ($server, $volume, $relunc) = AVIS::Macro::Connection::UnpackUnc($unc);
    my $errmsg .= "The directory $relunc cannot be accessed on ".$aconn->{Server}.":".$aconn->{Volume}.": $!\n";
    warn $errmsg; $self->SetLastError($errmsg);
    return 0;
  }
  return 1;
}


sub SetLastError($$) {
  my ($self, $msg) = @_;
  
  $self->{LastError} = $msg;  
  return 1;
}


sub IsUnc($) {
  my $path = shift;
  $path =~ s|\\|\/|g;
  return 1 if $path =~ /^\/\//;
  return 0;
}


############################################################################################
#
# Transfer operations
#
sub CopyDirContents($$$$) {
  my ($self, $srcdir, $destdir, $recurse) = @_;
  if (IsUnc($srcdir)) {
     $self->GetDirContents($srcdir, $destdir, $recurse);
  }  
  elsif (IsUnc($destdir)) {
     $self->PutDirContents($srcdir, $destdir, $recurse);
  }
}

sub CopyFile($$$) {
  my ($self, $srcfile, $destfile) = @_;
  if (IsUnc($srcfile)) {
     $self->GetFile($srcfile, $destfile);
  }  
  elsif (IsUnc($destfile)) {
     $self->PutFile($srcfile, $destfile);
  }
}

#copies the contents of a remote directory to a local directory
sub GetDirContents ($$$$) {
  my ($self, $unc, $destroot, $recurse) = @_;
  
  my $aconn = $self->{CONNECTION};
  my $trunc =  $aconn->TranslateUnc($unc);
  return 0 if !-e $trunc;
  return 0 if !-d $trunc;
  
  my $oldprune = $File::Find::prune;
  $File::Find::prune = $recurse;
  my @entries = ();
  File::Find::find(sub {push @entries, $File::Find::name if -f $File::Find::name;}, $trunc);
  $File::Find::prune = $oldprune;
  
  mkpath($destroot, 0, 0755) if !-e $destroot;
  foreach (@entries) {
    (my $destitem = $_) =~ s/\Q$trunc\E/$destroot/;
    if (-d $_) {
      mkpath($destitem, 0, 0755) if !-e $destitem;
    }
    if (-e $_) {
      copy($_, $destitem) or warn "Unable to copy $_ to $destitem: $!";
    }
  }  
}


sub PutDirContents ($$$$) {
  my ($self, $srcroot, $unc, $recurse) = @_;
  
  my $aconn = $self->{CONNECTION};
  my $trunc =  $aconn->TranslateUnc($unc);
  return 0 if !-e $srcroot;
  return 0 if !-d $srcroot;
  
  my $oldprune = $File::Find::prune;
  $File::Find::prune = $recurse;
  my @entries = ();
  File::Find::find(sub {push @entries, $File::Find::name if -f $File::Find::name;}, $srcroot);
  $File::Find::prune = $oldprune;
  
  mkpath($trunc, 0, 0755) if !-e $trunc;
  foreach (@entries) {
    (my $destitem = $_) =~ s/\Q$srcroot\E/$trunc/;
    if (-d $_) {
      mkpath($destitem, 0, 0755) if !-e $destitem;
    }
    if (-e $_) {
      copy($_, $destitem) or warn "Unable to copy $_ to $destitem: $!";
    }
  }  
}


sub PutFile ($$$) {
  my ($self, $srcfile, $unc) = @_;

  my $aconn = $self->{CONNECTION};
  my $trunc = $aconn->TranslateUnc($unc);
  if (!-e $trunc) {
    return 0;
  }
  if (!copy($srcfile, $trunc)) {
    warn "Unable to copy $srcfile to $trunc: $!";
    return 0;
  }
  return 1;
}


sub GetFile ($$$) {
  my ($self, $unc, $destfile) = @_;

  my $aconn = $self->{CONNECTION};
  my $trunc = $aconn->TranslateUnc($unc);
  if (!-e $trunc) {
    return 0;
  }
  if (!copy($trunc, $destfile)) {
    warn "Unable to copy $trunc to $destfile: $!";
    return 0;
  }
  return 1;
}


sub TranslateUnc($$) {
  my ($self, $unc) = @_;
  return $self->{CONNECTION}->TranslateUnc($unc);
}


1;
