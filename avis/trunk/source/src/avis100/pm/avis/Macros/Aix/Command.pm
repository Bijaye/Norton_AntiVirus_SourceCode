####################################################################
#                                                                  #
# Package name:  AVIS::Macro::Command                              #
#                                                                  #
# Description:   Process Control                                   #
#                                                                  #
#                                                                  #
# Statement:     Licensed Materials - Property of IBM              #
#                (c) Copyright IBM Corp. 1998-1999                 #
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
# Function: starts a process and kills it after a timeout          #
#                                                                  #
#                                                                  #
#                                                                  #
####################################################################

package AVIS::Macro::Command;

require Exporter;
@ISA = qw(Exporter);
@EXPORT = qw(system_t PROCESS_TIMEOUT PROCESS_KILLED PROCESS_FINISHED PROCESS_ERROR);


use strict;
use diagnostics;
use POSIX ":sys_wait_h";

my $pid;


#equivalent to a system call, but supposed to kill the child after a timeout.
#returns the process state
#still under development:
#can kill emacs but not netscape with this thing (even by using
#SIGKILL instead of SIGTERM, and there is not any error message...


#starts a process and waits for it to finish within a timeout
#returns pid (if process still running) or 0 (process finished) in $$pidrf
sub system_t($$$) {
  my ($command, $timeout, $pidrf) = @_;
  my $signal;
  my $timeslice = 1;
  my $rc = -1;
  my $prc;

  if ($timeout <= 0) {
      warn "system_t failed: $timeout is not a legal timeout value";
      return &PROCESS_ERROR;
  }
      
  #start the process
  if ($pid = $$pidrf = fork()) {
    #wait for the timeout to occur
    my $starttime = time();
    { #local signal handlers block
        local $SIG{INT} = \&CatchSig; #kill the child before dying
        local $SIG{TERM} = \&CatchSig;#kill the child before dying
        while ((time() <= $starttime + $timeout) &&(waitpid($pid, WNOHANG)) !=$pid) {
          sleep($timeslice);
        }
        $prc = waitpid($pid, WNOHANG);
        if ($prc == -1) {
	       #either the child has exited or there is an error
	       if (!($! =~ /no child processes/i)) {
	           warn $!; $rc = &PROCESS_ERROR;
	       } else {
              $$pidrf = 0; $rc = &PROCESS_FINISHED;
          }
        }
        elsif ($prc == 0) {
	       #the child does not want to leave by itself
	       warn "timeout $timeout elapsed";
          return killprc($pid);
        }
        else {
	       #should not happen
	       warn "waitpid $pid, WNOHANG returned $prc";
          $rc = &PROCESS_ERROR;
        }
    }#end signal handlers block
  }
  elsif(defined $pid) {
    #child
    exec ($command)||warn "exec failed: $!\n";
    exit;
  }
  else {
    warn "fork failed: $!";
    $rc = &PROCESS_ERROR;
  }
  return $rc;
}


#kills a process using the TERM or KILL signals
sub killprc($) {
  my $pid = shift;
  
  #try TERM first, and then KILL if the process has strong survival instincts
  my @signals = ('TERM', 'KILL');
  my $rc;

  warn "Attempting to terminate process $pid\n";  
  foreach (@signals) {
    if (kill($_, $pid)) {
        warn "signal $_ succeeded for $pid";
        return &PROCESS_KILLED if (waitpid($pid, WNOHANG) == $pid);
    } else {
        warn "signal $_ failed to reach $pid: $!";
        $rc = &PROCESS_ERROR;
    }
  }
  return $rc;
}


sub CatchSig {
    my $sig = shift;
    killprc($pid);
    die "Received a $sig signal";
}


sub PROCESS_TIMEOUT() {return 0;}
sub PROCESS_KILLED() {return 0;}
sub PROCESS_FINISHED() {return 1;}
sub PROCESS_ERROR() { return -1;}


1;
