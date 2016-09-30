=head1 NAME

Package: AVIS::Jockey
  
Filename: pm/AVIS/Jockey/Jockey.pm

Disk-image jockey

=head1 STATEMENT

Licensed Materials - Property of IBM

(c) Copyright IBM Corp. 1999-2000

U.S. Government Users Restricted Rights - use,    
duplication or disclosure restricted by GSA ADP   
Schedule Contract with IBM Corp.                  

=head1 AUTHORS

Morton Swimmer, Jean-Michel Boulay

=head1 HISTORY

=head1 BUGS

=cut


package Jockey;

require Exporter;
@ISA = qw(Exporter);
@EXPORT = qw( 
	     fetch 
	     release
	     lock_read_descriptors 
	     unlock_descriptors 
	     lock_write_descriptors
	     BuildImages
	     $isfnImageStatus @isvalMasterImages $isfnMopPID
	     $isvalImageCopies $isdnImagesDir $isdnBackupImagesDir 
	     $isvalImageExt);
@EXPORT_OK = @EXPORT;

use Data::Dumper;
use English;
use File::Copy;

use Fcntl qw(:DEFAULT :flock);

my $debug = 0;

use strict 'subs';

# These should eventually wind up in AVIS::Local
$isfnImageStatus = "images.dd";
@isvalMasterImages = ("W95usO95usO97us", "W95jpO95jp", "W95usO97us");
$isvalImageCopies = 2;
$isdnImagesDir = "/home/auto/Jockey/Images";
$isdnBackupImagesDir = "/data/Avis/Macro/Files/Config/Images";
$isvalImageExt = "hdf";
$isfnMopPID = "moppid.dd";
# ...

# ...

1;

=head1 DESCRIPTION

=head2 fetch

Fetch access to a clean image

=cut


sub is_file_flocked($) {
  my ($file) = @_;
  local *FH;

  if (sysopen(FH, $file, O_RDWR)) {
    my $locktrial = eval { flock(FH, LOCK_EX | LOCK_NB); };
    close FH;
    return 0 if not defined $locktrial;
    return $locktrial ? 0 : 1;
  } else {
    return 0;
  }  
}


sub fetch($) {
  my ($prototype) = @_;
  _log( localtime()." $$ fetch\n");
  my $descriptor;
  my @descriptors;
  my $tryagain = 1;
  while ($tryagain) {
    @descriptors = lock_read_descriptors();
    print STDERR scalar @descriptors, "\n";
    foreach $descriptor (@descriptors) {
      next unless ($$descriptor{'Prototype'} eq $prototype);
      if ($$descriptor{Status} eq "clean") {
         next if (is_file_flocked($$descriptor{'Path'})); #ignore image if it is flocked
         $$descriptor{'Status'} = "inuse";
      	$$descriptor{'PID'} = $PROCESS_ID;
      	lock_write_descriptors(@descriptors);
      	unlock_descriptors();
      	return $$descriptor{'Path'};
      }
    }
    unlock_descriptors();
    print localtime()." Waiting for an image of type $prototype to become free\n";
    sleep 1;
  }
}

=head2 release

Clean an image. 

This spawns a background process that cleans the image and then
releases the image when it is clean.

=cut

sub release($) {
  my ($path) = @_;
  _log (localtime()." $$ release\n");
  
  #check whether the image is locked
  if (is_file_flocked($path)) {
    _log(localtime()." release failed for $path: image is still locked by a process");
    return 0;
  }  
  
  my @descriptors = lock_read_descriptors();
  my $descriptor;
  my $tryagain = 1;

  foreach $descriptor (@descriptors) {
    next unless ($$descriptor{'Path'} eq $path);
    unless ($$descriptor{Status} eq "cleaning") {
      $$descriptor{'Status'} = "dirty";
      undef $$descriptor{'PID'};
      lock_write_descriptors(@descriptors);
      unlock_descriptors();
      return 1;
    } else {
      return 0;
    }
  }

  _log(localtime()." release couldn't find image $path to release");
  return 0;
}

=head2 lock_read_descriptors

Read in the descriptor file but keep open and locked

=cut

sub lock_read_descriptors() {
  my ($idescr, $d, $rc);
  _log (localtime()." $$ lock_read_descriptors\n") if $debug;
  filelock("$isdnImagesDir/$isfnImageStatus");
  open(IN, "$isdnImagesDir/$isfnImageStatus") 
      or _log (localtime()." $$ Open failed: $!"), return ();
  seek(IN, 0, 0);
  $rc = read IN, $d, 32000;
  close IN;
  eval $d;
  return @$idescr;
}

=head2 lock_write_descriptors

Write the descriptor file but leave locked and opened.

=cut

sub lock_write_descriptors {
  my @idescr = @_;
  _log (localtime()." $$ lock_write_descriptors\n") if $debug;
  my $d;
  $d = Data::Dumper->new([\@idescr], [qw(idescr)]);
  $d->Purity(0);
  $d->Useqq(1);
  filelock("$isdnImagesDir/$isfnImageStatus");
  open OUT, ">$isdnImagesDir/$isfnImageStatus" 
      or _log (localtime()." $$ Open failed: $!"), return 0;
  seek(OUT, 0, 0);
  print OUT $d->Dump;
  close OUT;
  return 1;
}

=head2 unlock_descriptors

Unlock and close the file.

=cut

sub unlock_descriptors() {
  _log (localtime()." $$ unlock_descriptors\n") if $debug;
  fileunlock("$isdnImagesDir/$isfnImageStatus");
  return 1;
}

=head2 BuildImages

Build the images and the status file

=cut

sub BuildImages() {
  my @descriptors = ();
  my ($i, $j);
  my $copyname;
  my $master;
  $j = 0;
  
  #delete existing images
  my @existing_images = <$isdnImagesDir/*.$isvalImageExt>;
  my $existing_image;
  foreach $existing_image (@existing_images) {
    if (is_file_flocked($existing_image)) {
      print "$existing_image is locked.\n";
    } else {  
      print "Deleting $existing_image...\n";
      unlink($existing_image)||warn "Unable to delete $existing_image: $!";
    }
  }
  
  #create images
  foreach $master (@isvalMasterImages) {
    # create $isvalImageCopies of the $master image
    print "Creating $master copies\n";
    for($i=0; $i < $isvalImageCopies; $i++) {
      # Should we delete all images beforehand?
      $copyname = "$isdnImagesDir/$master.$i.$isvalImageExt";
      if (is_file_flocked($copyname)) {
        print "Unable to create clean image $copyname from $isdnBackupImagesDir/$master.$isvalImageExt: there is a lock on $copyname\n";
        $descriptors[$j] = { 
			    'Path' => $copyname, 
			    'Status' => "dirty", 
			    'Master' => "$isdnBackupImagesDir/$master.$isvalImageExt",
			    'Prototype' => "$master"
        };
	     $j++;
      }
      else {
        print "Creating clean image $copyname from $isdnBackupImagesDir/$master.$isvalImageExt\n";
        if (copy("$isdnBackupImagesDir/$master.$isvalImageExt", $copyname)) {
          print "Copy succeeded\n";
	       $descriptors[$j] = { 
			    'Path' => $copyname, 
			    'Status' => "clean", 
			    'Master' => "$isdnBackupImagesDir/$master.$isvalImageExt",
			    'Prototype' => "$master"
		    };
	       $j++;
        } else {
          warn "Couldn't create copy $copyname: $!";
        }
      }
    } #for
  } #foreach
  lock_write_descriptors(@descriptors);
  unlock_descriptors();

  return 1;
}

sub _log ($) {
    my $msg = shift;
    #print $msg;
    open LOG, ">>$isdnImagesDir/mop.log" or warn "Can't append to log file", return 0;
    print LOG $msg;
    close LOG;
}


sub filelock ($) {
    my $file = shift;

    unless (-e "$file.LOCK") {
   	open LOCK, ">$file.LOCK";
	   print LOCK "$$\n";
	   close LOCK;
    } else {
	    while (-e "$file.LOCK") {
	      open LOCK, "<$file.LOCK";
	      my $pid = <LOCK>;
	      chomp $pid;
	      close LOCK;
	      #print "'$$': File locked by '$pid'. Waiting...\n";
	      return 1 if ($pid eq $$);
	      last if (! kill(0, $pid)); #previous locking process is deceased
         sleep 1;
	    };
	    open LOCK, ">$file.LOCK";
	    print LOCK "$$\n";
	    close LOCK;
    }
    return 1;
}


sub fileunlock ($) {
    my $file = shift;

    unlink "$file.LOCK";
    return 1;
}

=head1 SEE ALSO

F<mop.pl>

=cut

