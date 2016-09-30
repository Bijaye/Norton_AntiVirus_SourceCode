####################################################################
#                                                                  #
# Program name:  pm/AVIS/Emulator/SW95.pm                          #
#                                                                  #
# Package name:  AVIS::Emulator::SW95                              #
#                                                                  #
# Description:   Runs SoftWindows95 sessions                       #
#                                                                  #
# Statement:     Licensed Materials - Property of IBM              #
#                (c) 1999-2000                                     #
#                                                                  #
# Author:        Jean-Michel Boulay                                #
#                                                                  #
# And also:      U.S. Government Users Restricted Rights - use,    #
#                duplication or disclosure restricted by GSA ADP   #
#                Schedule Contract with IBM Corp.                  #
#                                                                  #
#                                                                  #
####################################################################

package AVIS::Emulator::SW95;

use strict;
use Sys::Hostname;

use AVIS::Macro::Command;
use AVIS::Emulator::Setup::SW95;


my %data = (
  'lasterror' => undef,
  'setup'     => undef,
  'pid'       => undef,
  'started'   => 0
);


sub new {
my $arg = shift;
my $class = ref($arg)||$arg;

  my $self = { %data };
  $self->{"setup"} = new AVIS::Emulator::Setup::SW95;
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


#returns a reference to the setup of the emulator
sub Setup($) {
  my $self = shift;

  return $self->{'setup'};  
}

sub Run() {
  my $self = shift;
  my %options = (@_) ? @_ : ();
  
  $self->Setup->Dump();
  $self->Setup->FinalizeSetup();
  
  my $program = "/usr/SoftWindows/bin/SoftWindows95";
  my $cmdline = "$program -batch";
  my @configargs = $self->Setup->GetCmdlineArgs();
  foreach (@configargs) {$cmdline .= " $_";}
  
  #these lines has been added so that SoftWindows does not try to use a remote display in case the script is started using rsh
  my $hostaddr = gethostbyname(hostname());
  my ($addr1, $addr2, $addr3, $addr4) = unpack('C4', $hostaddr);
  $hostaddr = "$addr1.$addr2.$addr3.$addr4";
  $ENV{DISPLAY} = $hostaddr.":0.0";    
  
  #start SoftWindows with a timeout of 20 mn
  # if SoftWindows is killed, the image files are messed up and they can't be accessed using mtools
  my $isvalSWTout = 1200;
  
  #there should be only one instance of SoftWindows running on each AIX machine,
  #any instance already running probably survived a killed instance of mrep, is
  #probably locking an image, and should not be here anyway
  my @psoutput = grep /\s$program\s/, `ps -ef`;
  if (scalar @psoutput) {
      my $sw95s = scalar (@psoutput);
      print $sw95s, " instance", (($sw95s > 1) ? 's' : ''), " of $program ", (($sw95s > 1) ? 'are' : 'is'), " already running\n";
  }
  foreach (@psoutput) {
    chomp();
    if (/\s*(\S+)\s+(\d+)\s+(\d+)\s+(\S+)/) {
      my ($username, $pid, $ppid, $runningtime) = ($1, $2, $3, $4);
      print "An instance of $program (pid $pid) is already running on behalf of $username\n";
      if (kill 'KILL', $pid) {
        print "Successfully killed process $pid\n";
      } else {
        print "Failed to kill process $pid\n";      
      }
    }
  }
  
  my $rc = system_t($cmdline, $isvalSWTout, \($self->{'pid'}));

  #reset the display
  delete $ENV{DISPLAY};
  return $rc;
}


1;
