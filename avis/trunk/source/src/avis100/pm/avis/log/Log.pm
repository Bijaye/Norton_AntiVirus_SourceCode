####################################################################
#                                                                  #
# Program name:  pm/AVIS/Log/Log.pm                                #
#                                                                  #
# Package name:  AVIS::Log                                         #
#                                                                  #
# Description:   Tools for appending to log files                  #
#                                                                  #
# Statement:     Licensed Materials - Property of IBM              #
#                (c) 1998,1999                                     #
#                                                                  #
# Author:        Morton Swimmer                                    #
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

package AVIS::Log;
require Exporter;
use AVIS::Local;

@ISA = qw(Exporter);
@EXPORT = qw(Log LogStatus LogStatusOn Chrono_on Chrono_off $ChronoTime);
@EXPORT_OK = @EXPORT;

my $StartTime = 0;
my $ChronoTime = 0;

1;

sub Log ($$$){
# append $texttolog in $logfile
# if $display is ON, display $textlog also on screen
  return if (scalar(@_) < 2);
  my ($logfile, $texttolog, $display) = @_;

  open (LOG, ">>$logfile");
  print LOG "$texttolog";
  close (LOG);
  if ($display =~ m/ON/i) { #Display message on screen? (ON or OFF)
      print "$texttolog";
  }
}

sub LogStatus ($$$) {
  my ($cookie, $phase, $state) = @_;
  my $timestr = scalar localtime;
  
  Log ($isfnStatusFile, "$cookie - $timestr - $state $phase\n","Off");
  # when new code is available we can do the right thing here:
  #metacommon::logstatus($cookie,$phase) if $state =~ /start/i;
}

sub LogStatusOn ($$$$$) {
  my ($cookie, $phase, $state, $logfile, $display) = @_;
  my $timestr = scalar localtime;
  
  Log ($logfile, "$cookie - $timestr - $state $phase\n",$display);
  # when new code is available we can do the right thing here:
  #metacommon::logstatus($cookie,$phase) if $state =~ /start/i;
}

sub Chrono_on {
  $StartTime = time;
}

sub Chrono_off {
  $ChronoTime = time - $StartTime;
}



