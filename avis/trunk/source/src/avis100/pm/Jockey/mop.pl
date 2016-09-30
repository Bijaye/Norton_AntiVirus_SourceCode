
=head1 NAME

Filename: Scripts/mop.pl

A program that cleans disk images by copying a clean backup copy to
the dirty images.

=head1 STATEMENT

Licensed Materials - Property of IBM

(c) Copyright IBM Corp. 1999-2000

U.S. Government Users Restricted Rights - use,    
duplication or disclosure restricted by GSA ADP   
Schedule Contract with IBM Corp.                  

=head1 AUTHOR

Morton Swimmer

=head1 HISTORY

MS19990303 Drafted code
MS19990308 First cleaned up and running version
MS19990315 Modified to use lock files instead of flock. Added --reset option by popular demand.
JMB20000203 No longer attempts to clean up and reallocate images which are still locked by a process 
JMB20000203 such as a SoftWindows session surviving a killed mrep session.

=head1 BUGS

If mop.pl crashes hard, it doesn't always recover properly. We may end
up with an empty images.dd file. We probably need to detect whether
the image copies are still in use or not.

Do we know if Softwindows95 locks the image and if it gets unlocked
when Softwindows95 crashes? Will this be a problem??

Mop.pl should restart itself on a daily basis.

=head1 DESCRIPTION

First the program has to establish whether a copy of itself is already
running. This way it can be started by cron on a regular basis without
the danger of multiple instances of the program.

The main function goes into an endless loop waiting for images to
become dirty. It checks this by reading in a table of image states
using Data::Dumper. If an image needing cleaning is found, we must
lock and reopen the file for writing. The image status is set to
'cleaning' before the file is written back. This is done via
subroutines in AVIS::Jockey.

'

=cut

use Jockey;
#use AVIS::Local;
use File::Copy;
use English;
use Getopt::Long;
use strict;

# Avoid restarting by first checking the PID file. If it exists, read
# the PID stored in it. Check the process table for that PID (using
# kill 0). If the process is still running, exit.

my $pid = read_pid();
my $rc;
my $reset = 0;

$rc = GetOptions("reset!" => \$reset);

if ($pid != -1) {
  #check if an instance of this program is already running
    my $rc = kill 0 => $pid;
    if ($rc) {
      if (!$reset) {
        print "Another instance of this program found. Exiting...\n";
        # An instance already exists, exit
        exit(2);
      }
      else {
        my $rc = kill 9 => $pid;
        if (!$rc) {
          print "Unable to kill previously started instance of this program. Exiting...\n";
          # Previous instance could not be killed, exit
          exit(2);
        }
      }
  }
}


my $rebuild = $reset;

$rebuild = 1 unless (-e "$isdnImagesDir/$isfnImageStatus");

if (-e "$isdnImagesDir/$isfnImageStatus") {
    my ($dev,$ino,$mode,$nlink,$uid,$gid,$rdev,$size,
	$atime,$mtime,$ctime,$blksize,$blocks) 
	= stat("$isdnImagesDir/$isfnImageStatus");
    print "$isdnImagesDir/$isfnImageStatus is $size\n";
    $rebuild = 1 if $size == 0;
}

# Rebuild the images and status files if necessary
if ($rebuild) {
    unlink "$isdnImagesDir/$isfnImageStatus";
    unlink "$isdnImagesDir/$isfnImageStatus.LOCK";
    unlink "$isdnImagesDir/mop.log";
    print localtime()." Building images...\n";
    BuildImages();
    unlink "$isdnImagesDir/$isfnMopPID"; 
}

# Ok, no other instance of this program is running, let's register
# this copy in the pid file

write_pid();

while (1) {
  print ".";
  # Read in the descriptors
  my @descriptors = lock_read_descriptors();
  if (@descriptors) {
      my $descriptor;
      my $descriptorindex = 0;
      foreach $descriptor (@descriptors) {
     #leave alone images which are locked
     if (Jockey::is_file_flocked($$descriptor{'Path'})) {
	      print "\n".localtime()." ".$$descriptor{Path}." is locked\n";
     }     
     # clean the dirty image, if we find one.
     elsif ($$descriptor{'Status'} eq "dirty") {
	      my $copyname = $$descriptor{Path};
	      print "\n".localtime()." Creating clean image $copyname from $$descriptor{Master}\n";
	      $$descriptor{'Status'} = "cleaning";
	      $$descriptor{'PID'} = $PROCESS_ID;
	      lock_write_descriptors(@descriptors);
 		   unlock_descriptors();
	      #sleep 5;
	      if (copy($$descriptor{Master}, $copyname)) {
		     print localtime()." Clean succeeded\n";
		     my @lastdescriptors = lock_read_descriptors();
           $lastdescriptors[$descriptorindex]->{'Status'} = "clean";
		     undef $lastdescriptors[$descriptorindex]->{'PID'};
		     lock_write_descriptors(@lastdescriptors);
		     # Now unlock and close
		     unlock_descriptors();
	      } else {
		     warn "Copy unsuccessful: $!";
	      }
	      last; # reread descriptors
	  } 
	  # Check for processes that have not updated the database properly,
	  # perhaps because they have crashed.
	  elsif (($$descriptor{Status} eq "inuse") 
		 || ($$descriptor{Status} eq "cleaning")) {
	      print "\n".localtime()." checking $$descriptor{'PID'} for life signs\n";
	      $rc = kill 0 => $$descriptor{'PID'};
	      unless ($rc) {
		  print localtime()." $$descriptor{'PID'} deceased: $?\n";
		  print localtime()." Changing status of $$descriptor{Path} to dirty\n";
		  $$descriptor{'Status'} = "dirty";
		  undef $$descriptor{'PID'};
		  lock_write_descriptors(@descriptors);
		  # Now unlock and close
		  unlock_descriptors();
		  last; # reread descriptors
	      }
	  }
     ++$descriptorindex;
     }
  }
  unlock_descriptors();
  sleep 1;
}
1;


sub write_pid {
  my $d;
  print "Writing Process id: $PROCESS_ID\n";
  $d = Data::Dumper->new([$PROCESS_ID], [qw(pid)]);
  $d->Purity(0);
  $d->Useqq(1);
  open OUT, ">$isdnImagesDir/$isfnMopPID" or warn "Open: $!", return 0;
  seek(OUT, 0, 0);
  print OUT $d->Dump;
  close OUT;
  return 1;
}

sub read_pid {
  my ($pid, $d, $rc);
  open(IN, "<$isdnImagesDir/$isfnMopPID") or warn "Open: $!", return -1;
  seek(IN, 0, 0);
  $rc = read IN, $d, 32000;
  close IN;
  eval $d;
  return $pid;
}

=head1 SEE ALSO

F<AVIS/Jockey.pm>

=cut
