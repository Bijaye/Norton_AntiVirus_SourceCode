####################################################################
#                                                                  #
# Program name:  pm/Readme File Parser/readme.pm                   #
#                                                                  #
# Package name:  Readme                                            #
#                                                                  #
# Description:   Tools for manipulating the readme file            #
#                                                                  #
# Statement:     Licensed Materials - Property of IBM              #
#                (c) Copyright IBM Corp. 1998-1999                 #
#                                                                  #
# Author:        Morton Swimmer, Frederic Perriot                  #
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


package Readme;
require Exporter;
@ISA = qw(Exporter);
@EXPORT = qw(Read Write);
@EXPORT_OK = @EXPORT;

use strict;

my $quiet = ($ENV{ISQUIET} eq "ON") if defined $ENV{ISQUIET};

1; # OK to load module

sub Read {
  my ($readmefile) = @_;

  # The defaults:
  my ($isvirus,$isdetected,$isverified,$isrepaired) = 0;
  my $vtype = "";
  my $vlength = 0;
  my @extralines;
    
  # Parsing the readme
  print "Parsing Readme File\n" unless $quiet;
  open (README, $readmefile) or warn " *** $0: Unable to open Readme file!\n";
  while (<README>) {
    # Fields subject to change by the consistency check
    if    (/Virus\s*:\s*(\w+)/i)	
      { $isvirus    = ($1 =~ /Y(es)?/i)?1:0; }
    elsif (/Detected\s*:\s*(\w+)/i)	
      { $isdetected = ($1 =~ /Y(es)?/i)?1:0; }
    elsif (/Verified\s*:\s*(\w+)/i)	
      { $isverified = ($1 =~ /Y(es)?/i)?1:0; }
    elsif (/Repaired\s*:\s*(\w+)/i)	
      { $isrepaired = ($1 =~ /Y(es)?/i)?1:0; }
    # The other fields just left intact
    else { push @extralines, $_; }
    # Return some additionnal informations
    if    (/Type\s*:\s*(\w+)/i)   { $vtype = $1; }
    elsif (/Length\s*:\s*(\d+)/i) { $vlength = $1; }
  }
  close (README);
  return ($isvirus,$isdetected,$isverified,$isrepaired,$vtype,$vlength,@extralines);
}

sub Write {
  my ($readmefile,$isvirus,$isdetected,$isverified,$isrepaired,@extralines) = @_;
  open (FINAL, ">$readmefile") or warn " *** $0: Unable to create Readme file!\n";
  print FINAL "Virus: ".(($isvirus)?"Yes":"No")."\n";
  print FINAL "Detected: ".(($isdetected)?"Yes":"No")."\n";
  print FINAL "Verified: ".(($isverified)?"Yes":"No")."\n";
  print FINAL "Repaired: ".(($isrepaired)?"Yes":"No")."\n";
  print FINAL @extralines;
  close (FINAL);
}

