####################################################################
#                                                                  #
# Package name:  AVIS::Macro::Image                                #
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
#                                                                  #
#                                                                  #
#                                                                  #
####################################################################
package AVIS::Macro::Image;

use Carp;

1;


#data:
#-file: path of the image on the system
#-cfgfile: the mtools do not take information about the image from the command line
# this information has to be in a configuration file, with that kind of sections:
# drive c:
#         file="~jmb/WIN95-jmb.hdf"
#         partition=1
# The mtools look for these settings in /usr/local/etc/mtools.conf, .mtoolsrc in $HOME,
# and finally in the file pointed to by $MTOOLSRC if this variable exists.

my %fields = (
	      file    => undef,
	      cfgfile => undef
	      );
#storing settings in .mtoolsrc is a bad idea if 
#several replication processes run at
#the same time using different images, so we're using $MTOOLSRC.


sub new {
  my $that = shift;
  my $image = shift;
  my $drive = shift||"c";
  my $partition = shift||1;

  my $class = ref($that)||$that;
  my $cfgfile = GetFreeCfgFileName();
  if (!defined $cfgfile) {
    return undef;
  }
  else {
    $fields{file} = $image;
    $fields{cfgfile} = $cfgfile;
    if (!open (CFG, ">".$cfgfile)) {
      warn "open(CFG, ">".$cfgfile) failed: ".$!;
      return undef;
    }
    else {
      print CFG "drive $drive:\n";
      print CFG "\tfile=\"$image\"\n";
      print CFG "\tpartition=$partition\n";
      close(CFG);
      my $self = {%fields};
      bless $self, $class;
      return $self;
    }
  }
}


#returns a name in .mtoolsrcxxx where xxx can be any number
#such as .mtoolsrcxxx does not exist.
#used to be sure we are not going to overwrite somebody else's settings file.
sub GetFreeCfgFileName {
  my $dir = shift||$ENV{HOME};
  my $attempt = 1;
  if (!opendir(DIR, $dir)) {
    warn "opendir($dir) failed: ".$!;
    return undef;
  }
  else {
    my $pat = ".mtoolsrc";
    my @entries = readdir DIR;
    while ($matching = grep /^\.mtoolsrc$attempt$/, @entries) {
      ++$attempt;
    }
    return $dir."/.mtoolsrc".$attempt;
  }
}



sub Copy {
  my $this = shift;
  
  #mcopy seems to prefer to see slashes in msdos pathnames instead of backslashes...
  #(it seems to assume that the msdos path refers to a directory if it contains some '\'
  (my $from = shift) =~ s/\\/\//g;
  (my $to = shift) =~ s/\\/\//g;;
  my $recursive = shift||0;
  my $text = shift;
  my $switches = "-oOnv";
  if ($recursive) {$switches .= "/";}
  
  #reset $MTOOLSRC to the config file associated to the image
  $ENV{MTOOLSRC} = $this->{'cfgfile'};
  
  #remove a possible read-only flag if the destination file exists to force the overwriting of read-only files on the image
  #the quotes around $to anf $from are necessary or else everything's going to fail for
  #pathnames that contain spaces such as c:/program files
  if (!(system "mdir -wf \"$to\" 2>/dev/null >/dev/null")) {
    my $ret = system "mattrib -r \"$to\" >/dev/null";
    if ($ret) {warn "command failed: mattrib -r \"$to\"; return code: $ret";}
  }
  
  #some quotes are needed around dos pathnames that contain:
  #-wildcards (else the shell tries to interpolate)
  #-spaces, eg c:\program files\microsoft office (or mcopy thinks it's dealing with several arguments)
  foreach $item ($from, $to) {
    #check whether the path is a DOS one
    if ($item =~ m/^\w:/) {
      #check whether there are characters justifying the use of quotes
      if (($item =~ m/\s/)||($item =~ m/\*/)) {
	$item = "\"$item\"";
      }
    }
  }
  
  my $command = "mcopy $switches $from $to >/dev/null";
  my $ret = (0xff00 & system $command) / 256;
  if ($ret) {warn "system $command failed; return code: $ret";}
  return $ret;
}


sub Dir {
  my $this = shift;
  (my $dir = shift) =~ s/\\/\//g;
  $ENV{MTOOLSRC} = $this->{'cfgfile'};
  my $ret = (0xff00 & system "mdir \"$dir\"") / 256; 
  if ($ret) {warn "system mdir $dir failed; return: $ret";}
  return $ret;
}


sub Mkdir {
  my $this = shift;
  (my $dir = shift) =~ s/\\/\//g;
  $ENV{MTOOLSRC} = $this->{'cfgfile'};
  
  my ($ret, $parentdir);
  ($parentdir = $dir) =~ s/(.*)\/.*/$1/;
  if (!($parentdir =~ m/^\w:$/)) {
    if (!($this->Exists($parentdir))) {
      $this->Mkdir($parentdir);
    }
  }
  $ret = (0xff00 & system "mmd \"$dir\"") / 256; 
  if ($ret) {warn "system mmd $dir failed; return: $ret";}
  return $ret;
}


#for existence tests
sub Exists {
  my $this = shift;
  (my $entry = shift) =~ s/\\/\//g;
  $ENV{MTOOLSRC} = $this->{'cfgfile'};
  my $ret = (0xff00 & system "mdir \"$entry\" 2>/dev/null >/dev/null");
  return !$ret;
}

sub DESTROY {
  my $this = shift;
  #erase the configuration file
  unlink($this->{'cfgfile'})||warn "unable to unlink ".$this->{'cfgfile'}.": ".$!;
}


