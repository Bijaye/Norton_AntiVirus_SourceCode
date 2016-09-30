####################################################################
#                                                                  #
# Program name:  pm/AVIS/CheckExtraction/CheckExtraction.pm        #
#                                                                  #
# Package name:  AVIS::CheckExtraction                             #
#                                                                  #
# Description:   Checks the file extraction for the                #
#                binary part of VIDES 2                            #
#                                                                  #
#                                                                  #
# Statement:     Licensed Materials - Property of IBM              #
#                (c) 1998-1999                                     #
#                                                                  #
# Author:        Stefan Tode                                       #
#                                                                  #
#                U.S. Government Users Restricted Rights - use,    #
#                duplication or disclosure restricted by GSA ADP   #
#                Schedule Contract with IBM Corp.                  #
#                                                                  #
#                                                                  #
####################################################################
#                                                                  #
# Function:      checks if the realtime extraction worked          #
#                                                                  #
# Notes:                                                           #
#                                                                  #
#                                                                  #
#                                                                  #
#                                                                  #
#                                                                  #
#                                                                  #
####################################################################

package AVIS::CheckExtraction;
require Exporter;
@ISA = qw(Exporter);
@EXPORT = qw(SameContent Equals);
@EXPORT_OK = qw();

use AVIS::FSTools;
use AVIS::Local;
use strict;

1;

sub SameContent
{
  my ($refdir, $chkdir, $refn) = @_;
  my $rv = 1;
  my $n = 0;

  my @tocheck = glob("$chkdir\\*");
     @tocheck = (), print("NO REPLICANTS!\n") if $tocheck[0] eq "$chkdir\\*";

  foreach my $file (@tocheck) {
    $n++;
    my $base = AVIS::FSTools::BaseName($file);
    my $reffile = "$refdir\\$base";
    if (-e $reffile) {
      unless (Equals($file, $reffile)) {
	print "Files $file and $reffile are different\n";
	if (-s $file > -s $reffile) { # Check against original here!
	  print "Real-time greater than afterwards -> Keeping real-time\n";
	}
	else {
	  print "Afterwards greater then real-time, may be multiply-infected...\n";
	  $rv = 0;
	}
      }
    }
    else {
      warn "No reference file! $reffile does not exist!\n";
      #$rv = 0;
    }
  }

  $$refn = $n if ref($refn) eq "SCALAR";
  return $rv;
}

sub Equals
{
  my ($f1, $f2) = @_;
  my $rc;
  # $isfnCompare defined in local.pm
  #  my $isfnCompare	= "$isdnBinDir/compare";


  $rc = system "$isfnCompare $f1 $f2 >nul";
  return $rc?0:1;
}
