#for Netbios
#a few remarks about Aix connections:
#-only root can mount drives as of version number 1.6 
#-if the -d option is not used when root tnclogins, auto has to login
# each time it wants to access the mounted drive
package AVIS::Macro::FTP;


require Exporter;
@ISA = qw(Exporter);
@EXPORT = qw();
@EXPORT_OK = @EXPORT;


use strict;
use Net::FTP;
use File::Path;

my $defaultuser = "ftp";
my $defaultpassword = "whoever\@here.there";
my $timeout = 1200;
my $verbose = 0;


sub new($$) {
    my ($class, $unc) = @_;

    my $self = {};
    
    my ($server, $virdir) = UnpackUnc($unc);
    return undef if !defined($server);
    $self->{Server} = $server;
    $self->{VirtualDir} = $virdir;
    $self->{Unc} = $unc;
    $self->{LastError} = "";
    $self->{Username} = $defaultuser;
    $self->{Password} = $defaultpassword;
    $self->{FTP} = undef;    
    $self->{Connected} = 0;
    $self->{DirectAccess} = 0;
    return bless $self, $class;
}


sub AllowsDirectAccess($) {
  my $self = shift;
  return $self->{DirectAccess};  
}


sub InitializeConnection($) {
  my $self = shift;

  my $ftp = new Net::FTP($self->{Server}, Timeout => 1200);
  if (!defined $ftp) {
    my $msg = "FTP connect to server $self->{Server} failed: $!";
    warn $msg; $self->SetLastError($msg);
    return 0;
  }
  else {
    if (!$ftp->login($self->{Username}, $self->{Password})) {
      my $msg = "FTP login to server $self->{Server} as user $self->{Username} failed: $!";
      warn $msg; $self->SetLastError($msg);
      return 0;
    }    
    $self->{FTP} = $ftp;
    $self->{Connected} = 1;
    return 1;
  }
}

sub ShutdownConnection($) {
  my $self = shift;

  my $ftp = $self->{FTP};
  $ftp->quit();
  $self->{FTP} = undef;
  $self->{Connected} = 0;
}


#check whether the unc is accessible on the server
sub IsAccessible($) {
  my $self = shift;

  my $ftp = $self->{FTP};
  if (!$ftp->cwd($self->{VirtualDir})) {
    my $msg = "Unable to cwd to ".$self->{VirtualDir}.": $!";
    warn $msg; $self->SetLastError($msg);
    return 0;
  }
  return 1;
}


sub SetLastError($$) {
  my ($self, $msg) = @_;
  
  $self->{LastError} = $msg;  
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
  
  my ($perms, $inode, $owner, $group, $length, $month, $day, $year, $name);
  my $ftp = $self->{FTP};
  my ($server, $srcroot) = UnpackUnc($unc);
  print "Copying '$srcroot' to '$destroot'\n" if $verbose;
  my @dirlist = $ftp->dir($srcroot);
  return if (!(defined @dirlist));
  $ftp->binary();
  foreach (@dirlist) {
    print "$_\n" if $verbose;
    #BUG: the split will not handle file names with spaces in them.
    ($perms, $inode, $owner, $group, $length, $month, $day, $year, $name) = split;
    if ((defined $length) && !($length eq "")) {
      if (/^d/ && $recurse) {
        if (($_ ne ".")&&($_ ne "..")) {
          print "<Directory>\n" if $verbose;
          # recurse into subdirectory
          $self->GetDirContents("$unc/$name", "$destroot/$name", $recurse);
        }
      } else {
        print "GET $srcroot/$name $destroot/$name\n" if $verbose;
        # make local directory first
        File::Path::mkpath(["$destroot"], $verbose, 0777);
        #get the file
        $ftp->get("$srcroot/$name", "$destroot/$name") or warn "GET failed ($@): $!";
      }
    }
  }
}


sub PutDirContents ($$$$) {
  my ($self, $srcroot, $unc, $recurse) = @_;
  
  my ($server, $destroot) = UnpackUnc($unc);
  my $ftp = $self->{FTP};
  my $dir = $ftp->mkdir($destroot, 1);
  print "Made ".$dir." out of $destroot\n" if $verbose;

  $ftp->binary();
  # Search the local directory for files and subdirectories to transfer
  foreach (<$srcroot/*>) {
    print "$_\n" if $verbose;
    my $destpath = $_;
    if (-d && $recurse && ($_ ne ".") && ($_ ne "..")) {
      print "<Directory>\n" if $verbose;          
      $destpath =~ s/\Q$srcroot\E/$unc/;
      #BUG: check for . and ..
      # It's a subdirectory, go recurse
      $self->PutDirContents($_, $destpath, $recurse);
    } elsif (-e ){
      # It's a file
      $destpath =~ s/\Q$srcroot\E/$destroot/;
      print "PUT $_ $destpath\n" if $verbose;
      $ftp->put($_, $destpath) or warn "PUT($_, $destpath) failed: $!";
    }
  }
}


sub PutFile ($$$) {
  my ($self, $srcfile, $unc) = @_;

  my ($server, $destfile) = UnpackUnc($unc);
  my $ftp = $self->{FTP};
  $ftp->binary();
  (my $destdir = $destfile) =~ s/(.*)\/.*/$1/;
  $ftp->mkdir($destdir, 1);
  print "PUT $srcfile $destfile\n" if $verbose;
  if (!-e $srcfile) {
    my $msg = "PUT($srcfile, $destfile) failed: $srcfile does not exist";
    warn $msg; $self->SetLastError($msg);
    return 0;
  }
  
  if (!$ftp->put($srcfile, $destfile)) {
    my $msg = "PUT($srcfile, $destfile) failed: $!";
    warn $msg; $self->SetLastError($msg);    
    return 0;
  }
  return 1;
}


sub GetFile ($$$) {
  my ($self, $unc, $destfile) = @_;

  my ($server, $srcfile) = UnpackUnc($unc);
  my $ftp = $self->{FTP};
  $ftp->binary();
  print "PUT $srcfile $destfile\n" if $verbose;
  if (!$ftp->get($srcfile, $destfile)) {
    warn "GET failed ($@): $!";
    return 0;
  }
  return 1;
}


############################################################################################
sub TranslateUnc($$) {
    my ($self, $unc) = @_;
    
    my $trunc;
    my ($server, $virdir) = UnpackUnc($unc);
    return undef if (! defined $server);
    if (lc($server) ne (lc($self->{Server}))) {
	   warn "unc ($unc) does maps to $server instead of $self->{Server}";
	   return undef;
    }
    return $virdir;
}


#extracts server name and virtual directory from unc
sub UnpackUnc($) {
    my $unc = shift;

    my ($server, $virdir);
    $unc =~ s/\\/\//g; #translate unc from DOS format to unix format
    if ($unc =~ s/^\/\/(.*?)(\/.*)//) {
	   ($server, $virdir) = ($1, $2);
    }
    else {
	   warn "unc ($unc) has a wrong format";
    }

    return ($server, $virdir);
}


1;
