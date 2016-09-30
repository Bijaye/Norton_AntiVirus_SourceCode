#for Netbios
#a few remarks about Aix connections:
#-only root can mount drives as of version number 1.6 
#-if the -d option is not used when root tnclogins, auto has to login
# each time it wants to access the mounted drive
package AVIS::Macro::Connection;

require Exporter;
@ISA = qw(Exporter);
@EXPORT = qw($err_ok $err_command_failed $err_login_failed $err_logout_failed $err_mount_failed $err_unmount_failed $err_initialization $err_server_not_found $err_already_logged_in);
@EXPORT_OK = @EXPORT;


#use strict;
use File::Path;


my $mountroot = "/NTshares";
my $acinstalldir = "/usr/tn";
my $defaultuser = "guest";
my $defaultpassword = "";
my $verbose = 0;

 $err_command_failed = -1;
 $err_ok = 0;
 $err_error = 1;
 $err_login_failed = 2;
 $err_logout_failed = 3;
 $err_mount_failed = 4;
 $err_unmount_failed = 5;
 $err_initialization = 6;
 $err_server_not_found = 7;
 $err_default_connection_exists = 8;
 $err_already_mounted = 9;
 $err_already_logged_in = 10;

1;

#Methods

sub new($$) {
    my $class = shift;
    my $server = shift;
    my $volume = shift;

    my $self = {};
    $self->{Server} = $server;
    $self->{Volume} = $volume;
    $self->{Mountpoint} = GetMountPoint($server, $volume);
    $self->{LastError} = "";
    $self->{Permanent} = 0;
    $self->{Username} = $defaultuser;
    $self->{Password} = $defaultpassword;
    
    return bless $self, $class;
}


sub Username {
  my $self = shift;
  if (@_) {
    $self->{Username} = shift;  
  }
  else {
    return $self->{Username};
  }  
}


sub Password {
  my $self = shift;
  if (@_) {
    $self->{Password} = shift;  
  }
  else {
    return $self->{Password};
  }  
}


sub TranslateUnc($$) {
    my $self = shift;
    my $unc = shift;
    
    my $trunc;
    my ($server, $volume, $relunc) = UnpackUnc($unc);
    if (!defined $server) {
	   return undef;
    }
    if ((uc($server) ne $self->{Server})||(uc($volume) ne $self->{Volume})) {
	   warn "unc ($unc) does not map to $server:$volume";
	   return undef;
    }
    $trunc = $self->{Mountpoint};
    if ($relunc ne "") {
	   $trunc .= "/$relunc";
    }
    return $trunc;
}


sub Connect($) {
    my $self = shift;

    return $err_initialization if (LoadClientDrivers() != $err_ok);      
    if (!IsNBRunning()) {
      return $err_initialization if (StartNB() != $err_ok);
    }
    if (IsMounted($self->{Server}, $self->{Volume})) {
      warn $self->{Server}.":".$self->{Volume}." is already mounted";
      return $err_already_mounted;
    }
#    my @volumemountpoints = GetMountedVolumes($self->{Server});
#    foreach (@volumemountpoints) {
#      return $err_UNMOUNT_FAILED if (Unmount($_) != $err_OK);          
#    }
#    if ($self->LoggedonUsers) {
#      Logout($self->{Server}, 0);    
#    }
    my $loginrc = Login($self->{Server}, $self->{Username}, $self->{Password}, 1);
    if ($loginrc == $err_default_connection_exists) {
       return $err_logout_failed if Logout($self->{Server}, 1);
       return $err_login_failed if (Login($self->{Server}, $self->{Username}, $self->{Password}, 1) != $err_ok);
    }
    elsif ($loginrc != $err_ok) {
       return $err_login_failed;
    }
    return $err_mount_failed if (Mount($self->{Server}, $self->{Volume}) != $err_ok);
  
  return $err_ok;
}

sub Disconnect($) {
  my $self = shift;
    
    return $err_unmount_failed if (Unmount($self->{Mountpoint}) != $err_ok);
    #should check whether other resources are used on the server first
    return $err_logout_failed if (Logout($self->{Server}, 1) != $err_ok);
  
  return $err_ok;
}

sub Refresh($) {
    my $self = shift;

    $self->Disconnect();
    $self->Connect();
}

sub ServerLogin() {
  my $self = shift;
  my $default = (@_ ? shift : 0);
  
  return Login($self->{Server}, $self->{Username}, $self->{Password}, $default);
}

sub ServerLogout($) {
  my $self = shift; 
  my $default = (@_ ? shift : 0);
  
  return Logout($self->{Server}, $default);
}

sub MountVolume($) {
  my $self = shift;
  
  return Mount($self->{Server}, $self->{Volume});
}

sub UnmountVolume($) {
  my $self = shift; 
  
  return Unmount($self->{MountPoint});
}


sub LoggedonUsers($) {  
    my $self = shift;

    my $server = $self->{Server};
    my @users = ();
    my $cmdline = $acinstalldir."/client/tncwho";
    print STDERR $cmdline, "\n";
    my @lines = `$cmdline`;
    foreach $_ (@lines) {
      if (m/^(\S*)\s+NB\s+($server)\s+(\S+)/i) {
        push @users, $1;
      }
    }
    if ($?) {
      warn "The command '$cmdline' failed. Status word: ".sprintf("%x",$?);
    }
    
  return @users;    
}



sub IsActive($) {
    my $self = shift;

    my @statattribs = stat $self->{Mountpoint};
    if (!@statattribs) {
        $self->{LastError} = "stat command failed on ".$self->{Mountpoint}.": $!";
	     warn $self->{LastError};
        return 0;
    }
    my ($dev, $ino, $mode,
     $nlink, $uid, $gid,
     $rdev, $size, $atime,
     $mtime, $ctime, $blksize,
     $blocks) = @statattribs;
    if ($dev >= 0) {
	     $self->{LastError} = $self->{Mountpoint}." is not a NFS filesystem";
        warn $self->{LastError};
        return 0;
    }
    else {
        return 1;
    }
}


sub IsAccessible($) {
    my $self = shift;

    my $mountpoint = $self->{Mountpoint};
    if (!-e $mountpoint) {return 0;}
    if (!opendir(MOUNTPOINT, $mountpoint)) {return 0;}
    my @entries = readdir(MOUNTPOINT);
    closedir(MOUNTPOINT);
    return (@entries ? 1 : 0);
}


sub IsNFS($) {
    my $self = shift;

    my @statattribs = stat $self->{Mountpoint};
    if (!@statattribs) {
        $self->{LastError} = "stat command failed on ".$self->{Mountpoint}.": $!";
	     warn $self->{LastError};
        return 0;
    }
    my ($dev, $ino, $mode,
     $nlink, $uid, $gid,
     $rdev, $size, $atime,
     $mtime, $ctime, $blksize,
     $blocks) = @statattribs;
    if ($dev >= 0) {
	     $self->{LastError} = $self->{Mountpoint}." is not a NFS filesystem";
        warn $self->{LastError};
        return 0;
    }
    else {
        return 1;
    }
}


sub IsMounted ($$) {
  my ($server, $volume) = @_;
  
  my $command = "mount";
  my @results = `$command`;
  if ($? == 0xff00) {
    warn "The command '$command' failed. Status word: ".sprintf("%x",$?);
    return $err_command_failed;
  }
  
  my $mountpoints = 0;
  foreach (@results) {
    if (m/^$server\s+$volume\s+(\S*)\s+tncfs\s+.*/i) {
      ++$mountpoints;
    }
  }
  return $mountpoints;
}


sub GetMountedVolumes ($) {
  my $server = shift;

  my @volumemountpoints = ();
  
  my $command = "mount";
  my @results = `$command`;
  if ($? == 0xff00) {
    warn "The command '$command' failed. Status word: ".sprintf("%x",$?);
    return ();
  }
  
  my $mountpoints = 0;
  foreach (@results) {
    if (m/^$server\s+(\S*)\s+(\S*)\s+tncfs\s+.*/i) {
      push @volumemountpoints, $2;
    }
  }
  return @volumemountpoints;
}


sub VolumesImportedFromServer ($) {
  my ($server) = @_;
  
  my $command = "mount";
  my @results = `$command`;
  if ($? == 0xff00) {
    warn "The command '$command' failed. Status word: ".sprintf("%x",$?);
    return $err_command_failed;
  }
  
  my $volumes = 0;
  foreach (@results) {
    if (m/^$server\s+\S*\s+\S*\s+tncfs\s+.*/) {
      ++$volumes;
    }
  }
  return $volumes;
}


sub IsNBRunning () {
  my $command = "$acinstalldir/tnstat";
  my @results = `$command`;
  if ($? == 0xff00) {
    warn "The command '$command' failed. Status word: ".sprintf("%x",$?);
    return $err_command_failed;
  }
  foreach (@results) {
    if (/NBservices are (\w+)\./i) {
      return 0 if (lc($1) eq "down");
      return 1 if (lc($1) eq "up");
    }    
  }  
  die "Unable to figure the status of the NB services";
}


sub IsServerRunning ($) {
  my $server = shift;

  my $command = "$acinstalldir/client/TNclient -n $server -r NB 2>&1 > /dev/null";
  my @results = `$command`;
  if ($? == 0x0000) {
    return 1;
  }
  foreach (@results) {
    return 0 if (/could not verify server $server/i);
  }  
  foreach (@results) {print $_;}
  warn "The command '$command' failed. Status word: ".sprintf("%x",$?);
  return $err_command_failed;
}

#Utilities

sub UnpackUnc($) {
    my $unc = shift;

    my ($server, $volume, $relunc);
    $unc =~ s/\\/\//g;
    $unc =~ /^\/\/(.*?)\/(.*?)\/(.*)/;
    if ($unc =~ s/^\/\/(.*?)\//\//) {
	   $server = uc($1);
	   if ($unc =~ m/\/(.*?)\/(.*)/) {
	     ($volume, $relunc) = (uc($1), $2);
	   }
	   else {
	     $unc =~ s/\///;
	     ($volume, $relunc) = (uc($unc), "");
	   }
    }
    else {
	   warn "unc ($unc) has a wrong format";
    }

    return ($server, $volume, $relunc);
}


sub GetMountPoint($$) {
    my ($server, $resource) = @_;

    my $mountpoint = $mountroot."/".uc($server)."/".uc($resource);
    return $mountpoint;
}



#Implementation-specific functions
sub LoadClientDrivers() {
#    my $cmdline .= "strload -q -d tncdv";
#    print STDERR $cmdline, "\n";
#    system $cmdline;    
#    if ($? & 0xfeff) {#the status word is 0x0100 if the driver is already loaded
#      warn "The command '$cmdline' failed. Status word: ".sprintf("%x",$?);
#      return $err_command_failed;
#    }
    
    my $cmdline = "strload -f ".$acinstalldir."/client/tncdv.conf";
    print STDERR $cmdline, "\n";
    system $cmdline;
    if ($? & 0xfeff) {#the status word is 0x0100 if the driver is already loaded
      warn "The command '$cmdline' failed. Status word: ".sprintf("%x",$?);
      return $err_command_failed;
    }
     
    if ($? ==0) {
      $cmdline = $acinstalldir."/client/tncfs_conf -c";
      print STDERR $cmdline, "\n";
      system $cmdline;
      if ($?) {
        warn "The command '$cmdline' failed. Status word: ".sprintf("%x",$?);
        return $err_command_failed;
      }
    }
        
  return $err_ok;
}

    
sub StartNB() {
    my $cmdline = $acinstalldir."/tnstart -r NB";
    print STDERR $cmdline, "\n";
    system $cmdline;
    if ($?) {
      warn "The command '$cmdline' failed. Status word: ".sprintf("%x",$?);
      return $err_command_failed;
    }
    
  return $err_ok;
}


sub ShutdownNB() {
    my $cmdline = $acinstalldir."/tnshut -r NB";
    print STDERR $cmdline, "\n";
    system $cmdline;
    if ($?) {
      warn "The command '$cmdline' failed. Status word: ".sprintf("%x",$?);
      return $err_command_failed;
    }    
  return $err_ok;
}


sub Login($$$$) {
    my ($server, $username, $passwd, $default) = @_;

    my $xtraflag;
    if ($default) {$xtraflag = "-d ";}
    my $cmdline = $acinstalldir."/client/tnclogin ".$xtraflag."-s $server -r NB -u $username -p \"$passwd\"";
    print STDERR $cmdline, "\n";
    my @msgs = `$cmdline`;    
    if ($? == 0) {
      return $err_ok;
    }
    if ($?) {
      foreach (@msgs) {print $_;}
      warn "The command '$cmdline' failed. Status word: ".sprintf("%x",$?);
    }
    if ($? == 0x0400) {
      return $err_server_not_found;
    }
    elsif ($? == 0x0500) {
      #three cases:
      #-login rejected
      #-default connection already exists
      #-already logged on
      foreach (@msgs) {
        return $err_default_connection_exists if (/default login connection exists/i);
        return $err_already_logged_in if (/already logged on as <.*>/i);
      }
      return $err_login_failed;
    }
    else {
      return $err_command_failed;
    }
}

sub Logout($$) {
    my ($server, $default) = @_;

    my $xtraflag;
    
    if ($default) {$xtraflag = "-d ";}
    my $cmdline = $acinstalldir."/client/tnclogout ".$xtraflag."-s $server -r NB";
    print STDERR $cmdline, "\n";
    my @msgs = `$cmdline`;
    if ($?) {
      foreach (@msgs) {print $_;}
      warn "The command '$cmdline' failed. Status word: ".sprintf("%x",$?);
      return $err_command_failed;
    }
    
  return $err_ok;
}



sub Mount($$) {
    my ($server, $resource) = @_;

    my $mountpoint = GetMountPoint($server, $resource);
    if (!-e $mountpoint) {
   	mkpath($mountpoint, 0755)||warn "Unable to create $mountpoint: $!";
      return $err_command_failed;
    }
    my $cmdline = "mount -V tncfs -o NB $server:$resource $mountpoint";
    print STDERR $cmdline, "\n";
    system $cmdline;   
    if ($?) {
      warn "The command '$cmdline' failed. Status word: ".sprintf("%x",$?);
      return $err_command_failed;
    }
    
  return $err_ok;
}

sub Unmount($) {
    my ($mountpoint) = @_;

    my $cmdline = "umount $mountpoint";
    print STDERR $cmdline, "\n";
    system $cmdline;   
    if ($?) {
      warn "The command '$cmdline' failed. Status word: ".sprintf("%x",$?);
      return $err_command_failed;
    }
    
  return $err_ok;    
}

