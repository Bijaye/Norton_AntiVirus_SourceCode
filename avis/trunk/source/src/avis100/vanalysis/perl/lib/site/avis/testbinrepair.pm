####################################################################
#                                                                  #
# Program name:  PM/AVIS/TestBinRepair/TestBinRepair.pm            #
#                                                                  #
# Module name:   AVIS::TestBinRepair.pm                            #
#                                                                  #
# Description:   Checks if a binary file has been repaired.        #
#                                                                  #
#                                                                  #
# Statement:     Licensed Materials - Property of IBM              #
#                (c) Copyright IBM Corp. 1999                      #
#                                                                  #
# Author:        ?                                                 #
#                                                                  #
#                U.S. Government Users Restricted Rights - use,    #
#                duplication or disclosure restricted by GSA ADP   #
#                Schedule Contract with IBM Corp.                  #
#                                                                  #
#                                                                  #
####################################################################
#                                                                  #
# Function:                                                        #
#    Checks if a binary file has been repaired. Will accept        #
#    certain inconsequential inexactness in EXE repair.            #
#                                                                  #
#                                                                  #
####################################################################

package AVIS::TestBinRepair;
require Exporter;

@ISA = qw(Exporter);
@EXPORT = qw(isRepairOf);   #?? isExactRepairOf isInexactRepairOf);
@EXPORT_OK = qw();

my $compprog          = "compare.exe";
my $writeheaderprog   = "writeheader.exe";
my $truncprog         = "truncate.exe";

#?? test that they're on the path

1;

#-----------------------------------------------------------------------------------

sub isRepairOf {
  my ($repaired, $original) = @_;
  return "exact"   if isExactRepairOf   ($repaired, $original);
  return "inexact" if isInexactRepairOf ($repaired, $original);
  return '';
}

#-----------------------------------------------------------------------------------

sub isExactRepairOf {
  my ($repaired, $original) = @_;
  my $retval = system "$compprog $original $repaired >nul";
  return $retval == 0;
}

sub isInexactRepairOf {
  my ($repaired, $original) = @_;
  my $len = -s $original;
  my $rlen = -s $repaired;
  my (	$sig1, $sig2,
	$mod512, $div512,
	$relocnum, $hsize,
	$min, $max,
	$SS, $SP,
	$checksum,
	$IP, $CS,
	$relocofs, $ovlnum
	) = _readEXEHeader ($original);
  my (	$rsig1, $rsig2,
	$rmod512, $rdiv512,
	$rrelocnum, $rhsize,
	$rmin, $rmax,
	$rSS, $rSP,
	$rchecksum,
	$rIP, $rCS,
	$rrelocofs, $rovlnum
	) = _readEXEHeader ($repaired);

#print "Signature: $sig1$sig2\n";

  my $sig   = pack ("CC", $sig1, $sig2);
  my $rsig  = pack ("CC",$rsig1,$rsig2);
  my $lenfromheader  = _EXElenfrommoddiv( $mod512,  $div512);
  my $rlenfromheader = _EXElenfrommoddiv($rmod512, $rdiv512);


#print "Sig $sig\n";

  if (($sig eq "MZ" or $sig eq "ZM") and ($rsig eq "MZ" or $rsig eq "ZM")) {

#print "Mod512 $mod512 Div512 $div512   Header Size $hsize   MinSize $min MaxSize $max\n";
#print "Reloc items # $relocnum First Reloc Item Offset $relocofs\n";
#print "SS:SP $SS:$SP   CS:IP $CS:$IP\n";
#print "Overlay number $ovlnum\n";
#print "Original - Length: $len\tHeader Length $lenfromheader\n";
#print "Repaired - Length: $rlen\tHeader Length $rlenfromheader\n";

  if (	$len == $lenfromheader &&	# No overlay in original file
	$rlenfromheader >= $len &&	# Enough bytes loaded by DOS from the repaired file
	$rlenfromheader <= $rlen &&	# Bytes indicated in header do exist
	$rlen >= $len			# Repaired file at least as long as original
	) {

	# We write the header even in the equality case to fix the checksum anyway
	# This could be a good inexact repair, we fix the checksum, the sig and truncate the repaired file

#print "Writing new header\n";

	_writeEXEHeader($repaired,
			$sig1, $sig2,
			$mod512, $div512,
			$rrelocnum, $rhsize,
			$rmin, $rmax,
			$rSS, $rSP,
			$checksum,
			$rIP, $rCS,
			$rrelocofs, $rovlnum);

#print "Truncating $repaired to $len\n"; #DEBUG

	system "$truncprog $repaired $len >nul";
	# Finally redo a comparison
        return isExactRepairOf($repaired, $original);
    }
    else {
	# Length conditions are no good
	return 0;
    }
  }
  else {
    # The file is not an EXE
    return 0;
  }
}

sub _readEXEHeader {
   my $file = shift;
   my $headerbuffer = "";
   open (FILE, $file);
   binmode FILE;
   my $n = read FILE, $headerbuffer, 28; print "TestBinRepair::_readEXEHeader read only $n bytes from $file\n" unless $n == 28;
   close (FILE);
   return unpack ("C2S13", $headerbuffer);
}

sub _writeEXEHeader {
   my ($file, @headerfields) = @_;
   my $headerbuffer;
   my $tmpheadfile = "_wehtmp$$";

   $headerbuffer = pack ("C2S13", @headerfields);
   open (TMPTMP, ">".$tmpheadfile);
   binmode TMPTMP;
   print TMPTMP $headerbuffer;
   close (TMPTMP);
   system "$writeheaderprog $tmpheadfile $file";
   unlink $tmpheadfile;
}

sub _EXElenfrommoddiv {
  my ($mod512, $div512) = @_;
  return 512 * $div512 + ($mod512 == 0 ? 0 : $mod512 - 512);
}
