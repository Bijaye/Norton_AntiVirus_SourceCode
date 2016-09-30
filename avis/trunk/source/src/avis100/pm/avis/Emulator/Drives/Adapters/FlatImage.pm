####################################################################
#                                                                  #
# Package name:  AVIS::Emulator::Drives::Adapters::FlatImage       #
#                                                                  #
# Description:   Access data on flat images                        #
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
package AVIS::Emulator::Drives::Adapters::FlatImage;


#data:
#-file: path of the image on the system
#-cfgfile: the mtools do not take information about the image from the command line
# this information has to be in a configuration file, with that kind of sections:
# drive c:
#         file="~user/WIN95-user.hdf"
#         partition=1
# The mtools look for these settings, in order:
# -in the file pointed to by the $MTOOLSRC environment variable if this variable exists,
# -in $HOME/.mtoolsrc if it exists
# -in /usr/local/etc/mtools.conf if nothing else works

#since several scripts using this module and running concurrently,
# the only viable option is to create a configuration file for each
# FlatImage instance and have it pointed to by $MTOOLSRC 

require AVIS::Emulator::Drives::Adapter;
@ISA = qw(AVIS::Emulator::Drives::Adapter);

use strict;


sub new {
  my $class = shift;
    
  my $path = shift;
  my $letter = shift;
  my $partition = shift||1;

  my $self = new AVIS::Emulator::Drives::Adapter('flatimage', $letter, $path);
  return undef if not defined $self;

  my $cfgfile = GetFreeCfgFileName();
  return undef if (!defined $cfgfile);
    
  $self->{'cfgfile'} = $cfgfile;
  $self->{'partition'} = $partition;
  
  return undef if not Activate($self);
  return bless $self, $class;
}


sub Activate($) {
my $self = shift;

  if (!open (CFG, ">".$self->{'cfgfile'})) {
    my $errmsg = "Unable to open ".$self->{'cfgfile'}." for output: $!";
    warn $errmsg; $self->SetLastError($errmsg);
    return 0;
  }
  else {
    print CFG 'drive '.$self->Letter.":\n";
    print CFG "\tfile=\"".$self->Path."\"\n";
    print CFG "\tpartition=".$self->{'partition'}."\n";
    close(CFG);
    return 1;
  }
}


#returns a name in .mtoolsrcxxx where xxx can be any number
#such as .mtoolsrcxxx does not exist.
#used to be sure we are not going to overwrite somebody else's settings file.
sub GetFreeCfgFileName {
  my $dir = shift||$ENV{HOME};
  if (!-d $dir) {
    my $errmsg = "GetFreeCfgFileName failed: $dir is not a valid directory";
    warn $errmsg;
    return undef;
  }
  else {
    my $attempt = 1;
    while (-e "$dir/.mtoolsrc$attempt") { ++$attempt}
    return "$dir/.mtoolsrc$attempt";
  }
}


sub Copy {
  my $self = shift;
  
  #mcopy seems to prefer to see slashes in msdos pathnames instead of backslashes...
  #(it seems to assume that the msdos path refers to a directory if it contains some '\'
  (my $from = shift) =~ s/\\/\//g;
  (my $to = shift) =~ s/\\/\//g;;
  my $recursive = shift||0;
  my $text = shift||0;
  my $switches = "-oOnv";
  if ($recursive) {$switches .= "/";}
  
  #reset $MTOOLSRC to the config file associated to the image
  $ENV{MTOOLSRC} = $self->{'cfgfile'};
  
  #remove a possible read-only flag if the destination file exists to force the overwriting of read-only files on the image
  #the quotes around $to and $from are necessary or else everything is going to fail for
  #pathnames that contain spaces such as c:/program files
  if (!(system "mdir -wf \"$to\" 2>/dev/null >/dev/null")) {
    my $command = "mattrib -r \"$to\" >/dev/null";
    my $ret = system $command;
    if ($ret) {
      my $errmsg = "command failed: $command; return code: $ret";
      warn $errmsg; $self->SetLastError($errmsg);
    }
  }
  
  #some quotes are needed around dos pathnames that contain:
  #-wildcards (else the shell tries to interpolate)
  #-spaces, eg c:\program files\microsoft office (or mcopy thinks it's dealing with several arguments)
  foreach ($from, $to) {
    #check whether the path is a DOS one
    if ($_ =~ m/^\w:/) {
      #check whether there are characters justifying the use of quotes
      if (($_ =~ m/\s/)||($_ =~ m/\*/)) {
	     $_ = "\"$_\"";
      }
    }
  }
  
  my $command = "mcopy $switches $from $to >/dev/null";
  if (system $command) {
    my $errmsg = "[$command] failed; status: $?";
    warn $errmsg; $self->SetLastError($errmsg);
  }
  return $?;
}


sub Dir($$) {
  my $self = shift;
  (my $dir = shift) =~ s/\\/\//g;
  $ENV{MTOOLSRC} = $self->{'cfgfile'};
  my $command = "mdir \"$dir\"";
  if (system $command) {
    my $errmsg = "[$command] failed; status: $?";
    warn $errmsg;
  }
  return $?;
}


sub MkDir($$) {
  my $self = shift;
  (my $dir = shift) =~ s/\\/\//g;
  $ENV{MTOOLSRC} = $self->{'cfgfile'};
  
  my $parentdir;
  ($parentdir = $dir) =~ s/\/.*//;
  if (!($parentdir =~ m/^\w:$/) && !($self->Exists($parentdir))) {
      $self->Mkdir($parentdir);
  }
  my $command = "mmd \"$dir\"";
  if (system $command) {
    my $errmsg = "[$command] failed; status: $?";
    warn $errmsg; $self->SetLastError($errmsg);
  }
  return $?;
}


#for existence tests
sub Exists($$) {
  my $self = shift;
  (my $entry = shift) =~ s/\\/\//g;
  $ENV{MTOOLSRC} = $self->{'cfgfile'};
  system "mdir \"$entry\" 2>/dev/null >/dev/null";
  return !$?;
}


sub CreateFileOnDrive($$$) {
my ($self, $path, $contents) = @_;

	my $tmpfile = GetTempFile();
	if (open(FILE, ">$tmpfile")) {
	    print FILE $contents;
	    close FILE;
	    my $ret = $self->Copy($tmpfile, $path);
	    unlink($tmpfile);
	    return $ret;
   }
	else {
	    warn "Failed to open $tmpfile for output: $!";
	    return 1;
	}
}


sub GetTempFile() {
    my $counter = 1;
    while (-e $ENV{HOME}."/tmp$counter") {$counter++;}
    my $tmpfile = $ENV{HOME}."/tmp$counter";
    #create the file with no contents to reserve the name
    open(TMPFILE, ">$tmpfile");
    close(TMPFILE);
    return $tmpfile;
}


sub Cleanup($) {
  my $self = shift;
  #release if fetched?
  unlink($self->Path)||warn "unable to unlink ".$self->Path.": $!";
}


sub DESTROY($) {
  my $self = shift;
  #erase the configuration file
  unlink($self->{'cfgfile'})||warn "unable to unlink ".$self->{'cfgfile'}.": $!";
}



1;
