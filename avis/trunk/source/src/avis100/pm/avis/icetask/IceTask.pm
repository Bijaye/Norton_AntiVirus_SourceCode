=head1 NAME

Package: AVIS::IceTask

Filename: pm/AVIS/IceTask/IceTask.pm

=head1 STATEMENT

Licensed Materials - Property of IBM

(c) Copyright IBM Corp. 1998-1999 

U.S. Government Users Restricted Rights - use,    
duplication or disclosure restricted by GSA ADP   
Schedule Contract with IBM Corp.                  

=head1 AUTHOR

Edward Pring, Morton Swimmer

=head1 DESCRIPTION

=cut

package AVIS::IceTask;

# This package exports several public functions:

require Exporter;
$VERSION = 1.10;
@ISA = qw(Exporter);
@EXPORT = ();
@EXPORT_OK = qw(begin idle end snooze reset);

# This package uses several public Perl packages, which must be installed 
# in the Perl library directory of the machine where this program executes:

use Win32::Event ();
use Win32::ChangeNotify 1.00 ();
use Win32::Mutex 1.00 ();
use strict;

# This package uses several private Perl packages, which must be installed
# in the current directory of the machine where this program executes:

use AVIS::IceBase ();

#----------------------------------------------------------------------
# Global variables 
#----------------------------------------------------------------------

my $taskMutex = "";
my$idleMutex = "";
my @snoozeDirectories = ();
my @snoozeObjects = ();

=head2 begin

Construct a unique name for this task's mutex.

=cut

sub begin () {
  my $name = join " ", $0, @ARGV;
  my ($key) = $name =~ /([^\\]+)$/;
  my $delay = 30;

  $taskMutex = Win32::Mutex->new(0,lc("task $key"));
  AVIS::IceBase::fatal("failed, cannot create mutex \"task $key\"\n")
    unless $taskMutex;

  my $rc = $taskMutex->wait(0);
  AVIS::IceBase::fatal("failed, another instance of $name already active\n")
    unless $rc;

  $idleMutex = Win32::Mutex->new(0,lc("idle $key"));
  AVIS::IceBase::fatal("failed, cannot create mutex \"idle $key\"\n")
    unless $idleMutex;
}


=head2 idle

=cut

sub idle (;$) {
  # Construct a unique name for this task's mutex.

  my ($name) = @_;
  $name = join " ", $0, @ARGV unless $name;
  my ($key) = $name =~ /([^\\]+)$/;

  #print "opening mutex \"$key\"\n";
  my $mutex = Win32::Mutex->open(lc("idle $key"));
  return undef unless $mutex;

  #print "waiting mutex \"$key\"\n";
  my $rc = $mutex->wait(0);
  #print "got mutex \"$key\", rc=$rc\n";
  #print "releasing mutex \"$key\"\n";
  $mutex->release;
  undef $mutex;
  #print "released mutex \"$key\"\n";

  return $rc==0 ? 1 : 0;
}

=head2 end

=cut

sub end () {
  undef $taskMutex;
  undef $idleMutex;
}

=head2 snooze

This function creates a list of ChangeNotify objects for a 
specified list of directories. It then waits until one of them
changes, or until a timer expires.  It returns the name of the
directory which changed, or null if the timer expired.

=cut

sub snooze (;$@) {
  my ($timeout,@directories) = @_;
  my $directory;    
  #print "snooze: timeout=$timeout, directories=@directories\n";

  if (@snoozeDirectories ne @directories) { 
    #print "closing ".scalar @snoozeObjects. " old snooze objects\n";
    my $object;
    foreach $object (@snoozeObjects) { $object->close; } 
    @snoozeObjects = ();
    @snoozeDirectories = ();

    foreach $directory (@directories) {
      AVIS::IceBase::fatal("cannot find directory $directory\n") 
	unless -d $directory;
      my $object = Win32::ChangeNotify->new($directory,0,"FILE_NAME LAST_WRITE");
      AVIS::IceBase::fatal("ChangeNotify failed for directory $directory\n") 
	unless $object;
      push @snoozeDirectories, $directory;
      push @snoozeObjects, $object;
      #print "monitoring snooze directory $directory\n";
    }
  } 

  return "" unless $timeout;
    
  #print "snoozing for $timeout seconds\n";
  $idleMutex->wait(0);
  my $rc = scalar @snoozeObjects 
    ? Win32::Event::wait_any(@snoozeObjects,$timeout*1000)
      : sleep($timeout)=="sleep";
  #print "snoozing complete, rc=$rc\n";
  AVIS::IceBase::fatal("ChangeNotify failed for directory $directory\n") 
    unless defined $rc;
  $idleMutex->release;
  return "" unless $rc>0;

  $snoozeObjects[$rc-1]->reset;
  return $snoozeDirectories[$rc-1];
}

=head2 reset

=cut

sub reset () {
  my $i;
  for ($i=0; $i < scalar @snoozeObjects; $i++) { 
    $snoozeObjects[$i]->reset; 
  }
}

######################################################################

1;

=head1 HISTORY

=head1 BUGS

=head1 SEE ALSO

list of other relevant modules or information sources

=cut

