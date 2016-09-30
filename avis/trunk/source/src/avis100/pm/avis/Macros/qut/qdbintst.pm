#########################################################################
#                                                                                                       #
# Package name: Qdbintst.pm                                                                     #
#                                                                                                       #
# Description:   unit test the definitions produced by analysis center.                         #
#                                                                                                       #
#                                                                                                       #
# Statement:     Licensed Materials - Property of IBM                                   #
#                                                                                                       #
# Author:       Raju Pavuluri                                                                   #
#                                                                                                       #
#                U.S. Government Users Restricted Rights - use,                         #
#                duplication or disclosure restricted by GSA ADP                        #
#                Schedule Contract with IBM Corp.                                               #
#                                                                                                       #
# Change history:                                                                               #
#   1999/09/01 - First version complete                                                         #
#                                                               #
#########################################################################

package AVIS::Macro::Qdbintst;

sub new {
	my ($class) = shift;
	my ($self) = {};
	bless($self,$class);

	$self->{NEWDEFPATH}=undef;
	$self->{REFTOINFARR}=undef;
	$self->{VIRUSNAME}=undef;
	$self->{VIRUSID}=undef;
	$self->{LOGFILE}="test.log";
	$self->{LOGSTRING}=undef;
	$self->{LOGOBJECT}=0;
	$self->{TRYREPAIR}=0;
	if (@_) {
	     $self->{NEWDEFPATH}=shift;
	     if (@_) {
		$self->{REFTOINFARR}=shift;
		if (@_) {
		   $self->{VIRUSID}=shift;
		   if (@_) {
		      $self->{VIRUSNAME}=shift;
		      if (@_) {
			 $self->{LOGFILE}=shift;
		      } else {
			   return($self);
		      }
		   } else {
			return($self);
		   }
		} else {
		     return($self);
		}
	     } else  {
		  return($self);
	     }
	} else {
	     return($self);
	}
	return($self);
}
##############
sub newdefpath {
	my $self = shift;
	if (@_) {
	   $self->{NEWDEFPATH} = shift;
	}
	return $self->{NEWDEFPATH};
}
#################
sub reftoinfarr {
	my $self = shift;
	if (@_) {
	   $self->{REFTOINFARR} = shift;
	}
	return $self->{REFTOINFARR};
}
##################
sub logString {
	my $self = shift;
	if (@_) {
	   $self->{LOGSTRING} = shift;
	}
	return $self->{LOGSTRING};
}
##################
sub logObject {
	my $self = shift;
	if (@_) {
	   $self->{LOGOBJECT} = shift;
	}
	return $self->{LOGOBJECT};
}
##################
sub tryRepair {
	my $self = shift;
	if (@_) {
	   $self->{TRYREPAIR} = shift;
	}
	return $self->{TRYREPAIR};
}
##################
sub virusName {
	my $self = shift;
	if (@_) {
	   $self->{VIRUSNAME} = shift;
	}
	return $self->{VIRUSNAME};
}
#####################
sub virusID {
	my $self = shift;
	if (@_) {
	   $self->{VIRUSID} = shift;
	}
	return $self->{VIRUSID};
}
########################
sub PD ($);
########################
sub doTest {

  my ($self) = shift;
  my ($newdefpath) = $self->{NEWDEFPATH};
  my ($refarray) = $self->{REFTOINFARR};
  my ($bluelog) = $self->{LOGFILE};
  my $logger = $self->{LOGOBJECT};
  my $tryrepair = $self->{TRYREPAIR};
  my $str = "";
  my $timeCap = 5;  ###Maximum time allowed for detection or repair

  $logger->logString("Starting doTest...") if $logger;

  use navapi;
  use Cwd();

  @insamples = @$refarray;
  $numsamples = @insamples;

  unless (open (MYLOG,">$bluelog")) {
	$self->logString("Unable to create log file\n");
	return(0);
  }

  if ($numsamples == 0) {
	print MYLOG "The sample array is empty\n";
	$self->logString("The input sample array is empty\n");
	close (MYLOG);
	return(0);
  }
  $CurDir = Cwd::cwd();
  system("mkdir testtmp");
  unless (chdir "$CurDir\\testtmp"){
	print MYLOG "Unable to change to directory - testtmp\n";
	$self->logString("Unable to change to directory - testtmp\n");
	return(0);
  }

  @result = &setup_navapi($newdefpath,*MYLOG);
  if ($result[0]==0) {
	$HNAVEngine = $result[1];
  } else {
	print MYLOG "setup_navapi failed\n";
	$self->logString("Error initialising navapi\n");
	close (MYLOG);
	return(0);
  } #endif
  print MYLOG "Now testing the samples...\n";
  $ErrorFlag=0;

  ##special treatment for the original sample!!!
  $originalSample = (shift @insamples)->{infected};
  @testresult = &test_the_sample($HNAVEngine,$originalSample,*MYLOG,0,"nocompare");  ##do not try to compare
  print MYLOG "@testresult\n";
  if ($testresult[0] != 0) { ##Error occured - original sample not detected
	$ErrorFlag=1;
	print MYLOG "$testresult[1]\n";
	$self->logString("Originalsample not detected - ".$testresult[1]); 
  }elsif ($testresult[2] >$timeCap) {  ## check to see if the detection time is OK.
	$str = sprintf ("too much time taken for original sample detection - %.4f sec\n", $testresult[2]);
	$self->logString($self->{LOGSTRING}.$str);
	print MYLOG "too much time taken for original sample detection - $testresult[2] sec\n";
	$ErrorFlag = 1;
 } elsif ($testresult[3] >$timeCap) {## check to see if the repair time is OK.
			$str = sprintf ("too much time taken for original sample repair - %.4f sec\n", $testresult[3]);
			$self->logString($self->{LOGSTRING}.$str);
			print MYLOG "too much time taken for original sample repair - $testresult[3] sec\n";
			$ErrorFlag = 1;
 }else { ##continue testing
	$str = sprintf ("time taken for original sample detection is %.4f sec and time taken for repair is %4f sec\n", $testresult[2],$testresult[3]);
        print MYLOG "$str\n";
	#$self->logString($self->{LOGSTRING}.$str);
	 foreach (@insamples) {
		my ($infected, $goat) = (PD $$_{infected}, PD $$_{goat});
		next if $infected =~ m</sectors/>i or $goat =~ m</none$>i;
		print MYLOG "Testing $samplefile\n";
		@testresult = &test_the_sample($HNAVEngine,$infected,*MYLOG,$tryrepair,$goat);
		if ($testresult[0] != 0) { ##Error occured
			$ErrorFlag=1;
			print MYLOG "$testresult[1]\n";
			$self->logString($self->{LOGSTRING}.$testresult[1]); ##Add to the existing error string
		} elsif ($testresult[2] >$timeCap) { ## check to see if the detection time is OK.
			$str = sprintf ("too much time taken for $infected detection - %.4f sec\n", $testresult[2]);
			$self->logString($self->{LOGSTRING}.$str);
			print MYLOG "too much time taken for $infected detection - $testresult[2] sec\n";
			$ErrorFlag = 1;
		} elsif ($testresult[3] >$timeCap) {## check to see if the repair time is OK.
			$str = sprintf ("too much time taken for $infected repair - %.4f sec\n", $testresult[3]);
			$self->logString($self->{LOGSTRING}.$str);
			print MYLOG "too much time taken for $infected detection - $testresult[3] sec\n";
			$ErrorFlag = 1;
		} else { ##everything is OK.
			$str = sprintf ("time taken for $infected detection is %.4f sec and time taken for repair is %.4f sec\n",$testresult[2],$testresult[3]);
			 print MYLOG "$str\n";
			#$self->logString($self->{LOGSTRING}.$str);
		};             
	 } # end foreach
  }##endif

    $CloseResult=navapi::NAVEngineClose($HNAVEngine);
    print MYLOG "Engine Close result: $CloseResult\n";
 chdir "..";
 system ("rmdir testtmp");
 if ($ErrorFlag == 0) {  ##Test Successful
	print MYLOG "..Test successful - definitions passed\n";
	close (MYLOG);
	return(1);
  } else {
	print MYLOG "..Errors occured during testing\n";
	close (MYLOG);
	return(0);
 } ##end if

} # end dotest


###################

sub setup_navapi {
  local ($NavDir,*LOG) = @_;
  ######Initialise the call back io functions
  $HfileIO=navapi::CallBackInit();
  #########Definition files
  @k=navapi::NAVEngineInit($NavDir,$NavDir,$HfileIO,2,0);
  unless ($k[1] == 0) {
    print *LOG, "Can not initialize NAV Engine using $NavDir; error=$k[1]\n";
    return(1,0);
  }
  else {
  return(0,$k[0]);
  }
}
###########################

##########################
sub test_the_sample {
	use navapi;
	local ($HNAVEngine,$samplefile,*LOG,$tryrepair,$goat) = @_;
	print LOG "Copying $samplefile to current directory\n";
	system ("copy $samplefile .");
	my $scanres = 0;
	my $scanstr = "";
	my $dettime = 0;
	my $reptime = 0;
	$slashpos= rindex ($samplefile, "\\");
	if ($slashpos >= 0 ) {
	  $scanthis= substr($samplefile,$slashpos+1,length($samplefile));
	} else {
	  $scanthis = $samplefile;  #No path name specified - may be the file is in this directory itself!!
	}
	$scanthis="$CurDir\\testtmp\\$scanthis";
	if (-e $scanthis) {
	  $extpos= rindex($scanthis,".");
	  if ($extpos >= 0) { # not clear if this is needed...
	    $ext = substr($scanthis,$extpos+1,3);
	  } else {
	    $ext = NULL; #default to NULL if can't find extension
	  }
	  @result = &donav ($HNAVEngine,$scanthis,$ext,*LOG,$tryrepair);
	  $dettime = $result[2];
	  $reptime = $result[3];
	  if ($result[0]==0) { ##test successful -  compare with the goat
			   if ($tryrepair && $goat ne 'nocompare') {  ## try repair is set and not original sample
				if (!isRepairOf ($scanthis, $goat)) {
					print LOG "$scanthis - badrepair\n";
					$scanres = 1;
					$scanstr = "$scanthis - badly repaired\n";
				}else { #repair succesful
					print LOG "$scanthis - good repair\n";
					$scanres = 0;
					$scanstr = "$scanthis - good repair\n";
				}#endif
			   } else { ##try repair not set - should not compare
			      $scanres = 0;
			      $scanstr = "$scanthis - test successful but not trying to compare\n";
			      print LOG "$scanthis - test successful but not trying to compare\n";
			   }#endif
	}else { ## result is non zero - test not succesful
			      ## set the ret values to those of donav function
		    $scanres = $result[0];
		    $scanstr = $result[1];
	} ##endif
	  unlink("$scanthis");
	  print LOG "Finished with $samplefile\n\n";
	  return($scanres,$scanstr,$dettime,$reptime);
	     } else {
	  print LOG "File doesn't exist or can't be copied to testtmp directory\n";
	  return(1,"$samplefile - File doesn't exist or can't be copied to temp directory\n",$dettime,$reptime);
	    } ##endif
} # end test_the_sample

###############################

###############################
sub donav { # use navapi to scan and repair the sample
	local ($HNAVEngine,$scanthis,$ext,*LOG,$tryrepair) = @_;
	use AVIS::Times;	#For times function
	$timetodetect = 0;
	$timetorepair = 0;
	print LOG "Scanning $scanthis with ext $ext\n";
	my @times = AVIS::Times::times();
	$before = $times[0]+$times[1];
	@scanresult=navapi::NAVScanFile($HNAVEngine,$scanthis,$ext,1);
	@times = AVIS::Times::times();
	$after = $times[0]+$times[1];
	$timetodetect = $after - $before;
	print MYLOG "time to detect is $timetodetect\n";
	if ($scanresult[0]==0) { #scan is ok
	      if ($scanresult[1]==NULL) {
		  print LOG "...ERROR: No Virus found in $scanthis!!\n";
		  return(1,"No Virus Found in $scanthis\n",$timetodetect,$timetorepair);
	      } else {
		  print LOG "  Virus Found!";

		  ###Get the virus ID
		  @virid = navapi::NAVGetVirusInfo($scanresult[1],$navapi::NAV_VI_VIRUS_ID,100);
		  if ($virid[0]==0) {
			
			print LOG "$scanthis is infected with VirusId - $virid[1]\n";
			#$self->virusID($virid[1]);
		 } else {
			print LOG "$scanthis - Error getting VirusId\n";
			navapi::NAVFreeVirusHandle($scanresult[1]);
			return (1,"$scanthis - Error getting VirusId\n",$timetodetect,$timetorepair);
		 }

		###Get the virus name
		@virname = navapi::NAVGetVirusInfo($scanresult[1],$navapi::NAV_VI_VIRUS_NAME,100);
		if ($virname[0]==0) {
			print LOG "$scanthis is infected with $virname[1]\n";
			#$self->virusName($virname[1]);
		} else {
			print LOG "$scanthis - Error getting virus name\n";
			navapi::NAVFreeVirusHandle($scanresult[1]);
			return (1,"$scanthis - Error getting virus name\n",$timetodetect,$timetorepair);
		}
		
	
		  #now try to repair it

		  if ($tryrepair) {
			@times = AVIS::Times::times();
			$beforerepair = $times[0]+$times[1];
			@represult=navapi::NAVRepairFile($HNAVEngine,$scanthis,$ext);
			@times = AVIS::Times::times();
			$afterrepair = $times[0]+$times[1];
			$timetorepair = $afterrepair - $beforerepair;
			print MYLOG "time to repair is $timetorepair\n";
			if ($represult[0]==0) { #we think we repaired it
				print LOG "We think we repaired it\n";
				navapi::NAVFreeVirusHandle($scanresult[1]);
				@secondresult=navapi::NAVScanFile($HNAVEngine,$scanthis,$ext,1);
				if ($secondresult[1]==NULL) {
					print LOG "  No Virus found on second scan - repair successful\n";
					return(0,"$scanthis - Repair successful - infected with $virname[1] - $virid[1]\n",$timetodetect,$timetorepair);
					} else {
						print LOG "  Virus Still Found! Return code: $secondresult[0]\n";
						navapi::NAVFreeVirusHandle($secondresult[1]);
						return(1,"$scanthis - We think we repaired it - but virus is still found\n",$timetodetect,$timetorepair);
				}
			  } else {#represult not 0
				print LOG "...ERROR: Could not repair, return code: $represult[0]\n";
				navapi::NAVFreeVirusHandle($scanresult[1]);
				return(1,"$scanthis ...ERROR: Could not repair, return code: $represult[0]\n",$timetodetect,$timetorepair);
			}
		  } else {
			print LOG "Repair flag not set: We won't be able to repair, will not try\n";
			navapi::NAVFreeVirusHandle($scanresult[1]);
			 print MYLOG "time to detect is $timetodetect\n";
			return(0,"$scanthis ...Repair flag not set: We won't be able to repair this file\n",$timetodetect,$timetorepair);
		  }
	     } #end if virus found

	} else {
	  print LOG "Error scanning $scanthis, scanresult= $scanresult[0]\n";
	  return(1,"Error scanning file $scanthis\n",$timetodetect,$timetorepair);
	} # end if bad scan
} # end donav

###################
sub isRepairOf ($$) {
  my ($repaired, $original) = @_;
  return (1)  if isExactRepairOf   ($repaired, $original);
  return (1) if isInexactRepairOf ($repaired, $original);
  return (0);
}


sub isExactRepairOf ($$) {
  my ($repaired, $original) = @_;
   my $compprog = "d:\\bin\\compare.exe";
  my $retval = system "$compprog $original $repaired >nul";
  return $retval == 0;
}


sub isInexactRepairOf ($$) {
  my ($repaired, $original) = @_;
  my $len = -s $original;
  my $rlen = -s $repaired;
  my $truncprog  = "d:\\bin\\truncate.exe";
  my (  $sig1, $sig2,
	$mod512, $div512,
	$relocnum, $hsize,
	$min, $max,
	$SS, $SP,
	$checksum,
	$IP, $CS,
	$relocofs, $ovlnum
	) = _readEXEHeader ($original);
  my (  $rsig1, $rsig2,
	$rmod512, $rdiv512,
	$rrelocnum, $rhsize,
	$rmin, $rmax,
	$rSS, $rSP,
	$rchecksum,
	$rIP, $rCS,
	$rrelocofs, $rovlnum
	) = _readEXEHeader ($repaired);

  my $sig   = pack ("CC", $sig1, $sig2);
  my $rsig  = pack ("CC",$rsig1,$rsig2);
  my $lenfromheader  = _EXElenfrommoddiv( $mod512,  $div512);
  my $rlenfromheader = _EXElenfrommoddiv($rmod512, $rdiv512);

  if (($sig eq "MZ" or $sig eq "ZM") and ($rsig eq "MZ" or $rsig eq "ZM")) {

     if (  $len == $lenfromheader &&       # No overlay in original file
	   $rlenfromheader >= $len &&      # Enough bytes loaded by DOS from the repaired file
	   $rlenfromheader <= $rlen &&     # Bytes indicated in header do exist
	   $rlen >= $len                   # Repaired file at least as long as original
	) {

	# We write the header even in the equality case to fix the checksum anyway
	# This could be a good inexact repair, we fix the checksum, the sig and truncate the repaired file

	# write a new header
	_writeEXEHeader($repaired,
			$sig1, $sig2,
			$mod512, $div512,
			$rrelocnum, $rhsize,
			$rmin, $rmax,
			$rSS, $rSP,
			$checksum,
			$rIP, $rCS,
			$rrelocofs, $rovlnum);

	# truncate
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


sub _readEXEHeader ($) {
   my $file = shift;
   my $headerbuffer = "";
   open (FILE, $file);
   binmode FILE;
   my $n = read FILE, $headerbuffer, 28; print "TestBinRepair::_readEXEHeader read only $n bytes from $file\n" unless $n == 28;
   close (FILE);
   return unpack ("C2S13", $headerbuffer);
}


sub _writeEXEHeader ($@) {
   my ($file, @headerfields) = @_;
   my $headerbuffer;
   my $tmpheadfile = "_wehtmp$$";
   my $writeheaderprog      = "d:\\bin\\writeheader.exe";

   $headerbuffer = pack ("C2S13", @headerfields);
   open (TMPTMP, ">".$tmpheadfile);
   binmode TMPTMP;
   print TMPTMP $headerbuffer;
   close (TMPTMP);
   system "$writeheaderprog $tmpheadfile $file";
   unlink $tmpheadfile;
}


sub _EXElenfrommoddiv ($$) {
  my ($mod512, $div512) = @_;
  return 512 * $div512 + ($mod512 == 0 ? 0 : $mod512 - 512);
}

#==============================================================================

sub PD ($) {
   my ($path) = @_;
   if ($^O =~ m/win/i) {$path =~ tr[/][\\];}
   else                {$path =~ tr[\\][/];}
   return $path;
}
##################
sub min ($$) {return ($_[0] < $_[1] ? $_[0] : $_[1]);}

sub max ($$) {return ($_[0] > $_[1] ? $_[0] : $_[1]);}

sub minstr ($$) {return ($_[0] lt $_[1] ? $_[0] : $_[1]);}

sub maxstr ($$) {return ($_[0] gt $_[1] ? $_[0] : $_[1]);}


1;
