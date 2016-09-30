#!perl -w
####################################################################
#                                                                  #
# Program name:  submit.pl                                         #
#                                                                  #
# Description:   put issues into the classifier queue              #
#                                                                  #
#                                                                  #
# Statement:     Licensed Materials - Property of IBM              #
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
# Installs into queueserver/bin                                    #
#                                                                  #
####################################################################


use DataFlow;
#use AVIS::Local;
$isdnServerRoot  = "//mosel/srvroot";
$isdnData        = "data";
$isdnSample      = "sample";
use File::Path;
#use Getopt::Long;
use File::CounterFile;
use File::Copy;

#use strict;
print "$#ARGV\n";

die "no arguments" if ($#ARGV < 0);

$filename = $ARGV[0];

die "file not found" if (!(-f $filename));

$c = new File::CounterFile "$isdnServerRoot/$isdnData/counter.file", "0";

$id = $c->inc;

$cookie = sprintf("%08d", $id);

print "$cookie\n";

$cookiedir = "$isdnServerRoot/$isdnData/$cookie";
$sampledir = "$cookiedir/$isdnSample";

mkpath([$cookiedir, $sampledir], 1, 0777);

copy($filename, "$sampledir/sample.com");

#Form the complete filename.
$fileName = $cookiedir . "\\dfstart.dat";
#Check if the file already exists.
unless (-e $fileName)
{
     #Create this file
     open (FILEHANDLE, ">$fileName");
     close (FILEHANDLE);    
}   

1;

