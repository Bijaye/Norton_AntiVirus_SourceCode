#!perl -w
####################################################################
#                                                                  #
# Program name:  ParamStr.pm                                       #
#                                                                  #
# Package name:  AVIS::ParamStr.pm                                 #
#                                                                  #
# Description:                                                     #
#                                                                  #
#                                                                  #
# Statement:     Licensed Materials - Property of IBM              #
#                (c) Copyright IBM Corp. 1998-1999                 #
#                                                                  #
# Author:        Morton Swimmer                                    #
#                                                                  #
#                                                                  #
#                U.S. Government Users Restricted Rights - use,    #
#                duplication or disclosure restricted by GSA ADP   #
#                Schedule Contract with IBM Corp.                  #
#                                                                  #
# Change history:                                                  #
#                                                                  #
#                                                                  #
####################################################################
#                                                                  #
# Function:                                                        #
#                                                                  #
#                                                                  #
#                                                                  #
#                                                                  #
####################################################################

package AVIS::ParamStr;

require Exporter;
use vars qw(@ISA @EXPORT $VERSION);

use strict;

@ISA = qw(Exporter);
@EXPORT = qw(UnpackPara PackPara Para2Opts);

#######################################################################
# UnpackPara -- unpack the parameter string into a hash.
sub UnpackPara ($) {
  my $msg = shift or die "One parameter required";
  ($msg =~ m/^(\w+):\s(.+)/mg);
}

#######################################################################
# PackPara -- pack hash into a parameter string. This is a string of
# lines, where each line is of the format: "key: value\n".
sub PackPara (@) {
  my %array = @_;
  my $myoutpar = "";
  foreach (keys %array) {
    $myoutpar .= "$_: $array{$_}\n";
  }
  $myoutpar;
}

#######################################################################
# Para2Opts -- pack parameter hash into a POSIX argument string. 
# E.g. "--Cookie 1234 --ProcessID 12"
sub Para2Opts (@) {
  my %array = @_;
  my $myoutpar = "";
  foreach (keys %array) {
    $myoutpar .= "--$_ $array{$_} ";
  }
  $myoutpar;
}

1;




















