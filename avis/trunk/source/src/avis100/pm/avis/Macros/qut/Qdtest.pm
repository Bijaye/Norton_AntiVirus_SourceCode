#########################################################################
#                                                                  					#
# Package name: Qdtest.pm                                      				#
#                                                                					#
# Description:   unit test the definitions produced by analysis center.            		#
#                                                                  					#
#                                                                  					#
# Statement:     Licensed Materials - Property of IBM              			#
#                                                                  					#
# Author:       Raju Pavuluri                                        				#
#                                                                  					#
#                U.S. Government Users Restricted Rights - use,    			#
#                duplication or disclosure restricted by GSA ADP   			#
#                Schedule Contract with IBM Corp.                 				#
#                                                                  					#
# Change history:                                                  				#
#   1999/07/20 - First version complete                            				#
#   								#
#########################################################################

package AVIS::Macro::Qdtest;

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
sub doTest {

  my ($self) = shift;
  my ($newdefpath) = $self->{NEWDEFPATH};
  my ($refarray) = $self->{REFTOINFARR};
  my ($bluelog) = $self->{LOGFILE};
  my $logger = $self->{LOGOBJECT};

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
  foreach $samplefile (@insamples) {
	print MYLOG "Testing $samplefile\n";
	@testresult = &test_the_sample($HNAVEngine,$samplefile,*MYLOG);
	if ($testresult[0] != 0) { ##Error occured
		$ErrorFlag=1;
		print MYLOG "$testresult[1]\n";
		$self->logString($self->{LOGSTRING}.$testresult[1]); ##Add to the existing error string
	} #endif
  } # end foreach

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
	local ($HNAVEngine,$samplefile,*LOG) = @_;
	system ("copy $samplefile .");
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
	  @result = &donav ($HNAVEngine,$scanthis,$ext,*LOG);
	  unlink("$scanthis");
	  print LOG "Finished with $samplefile\n\n";
	  return($result[0],$result[1]);
	} else {
	  print LOG "File doesn't exist or can't be copied to testtmp directory\n";
	  return(1,"$samplefile - File doesn't exist or can't be copied to temp directory\n");
	}
} # end test_the_sample

###############################

###############################
sub donav { # use navapi to scan and repair the sample
	local ($HNAVEngine,$scanthis,$ext,*LOG) = @_;
	print LOG "Scanning $scanthis with ext $ext\n";
	@scanresult=navapi::NAVScanFile($HNAVEngine,$scanthis,$ext,1);
	if ($scanresult[0]==0) { #scan is ok
	      if ($scanresult[1]==NULL) {
		  print LOG "...ERROR: No Virus found in $scanthis!!\n";
		  return(1,"No Virus Found in $scanthis\n");
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
			return (1,"$scanthis - Error getting VirusId\n");
		 }

		###Get the virus name
		@virname = navapi::NAVGetVirusInfo($scanresult[1],$navapi::NAV_VI_VIRUS_NAME,100);
		if ($virname[0]==0) {
			print LOG "$scanthis is infected with $virname[1]\n";
			#$self->virusName($virname[1]);
		} else {
			print LOG "$scanthis - Error getting virus name\n";
			navapi::NAVFreeVirusHandle($scanresult[1]);
			return (1,"$scanthis - Error getting virus name\n");
		}
		
	
		  #now try to repair it

		  if (navapi::NAVGetVirusInfo($scanresult[1],$navapi::NAV_VI_BOOL_REPAIRABLE,2)) {
			@represult=navapi::NAVRepairFile($HNAVEngine,$scanthis,$ext);
			if ($represult[0]==0) { #we think we repaired it
				print LOG "We think we repaired it\n";
				navapi::NAVFreeVirusHandle($scanresult[1]);
				@secondresult=navapi::NAVScanFile($HNAVEngine,$scanthis,$ext,1);
				if ($secondresult[1]==NULL) {
					print LOG "  No Virus found on second scan - repair successful\n";
					return(0,"$scanthis - Repair successful - infected with $virname[1] - $virid[1]\n");
					} else {
						print LOG "  Virus Still Found! Return code: $secondresult[0]\n";
						navapi::NAVFreeVirusHandle($secondresult[1]);
						return(1,"$scanthis - We think we repaired it - but virus is still found\n");
				}
			  } else {#represult not 0
				print LOG "...ERROR: Could not repair, return code: $represult[0]\n";
				navapi::NAVFreeVirusHandle($scanresult[1]);
				return(1,"$scanthis ...ERROR: Could not repair, return code: $represult[0]\n");
			}
		  } else {
			print LOG "...ERROR: We won't be able to repair, will not try\n";
			navapi::NAVFreeVirusHandle($scanresult[1]);
			return(1,"$scanthis ...ERROR: We won't be able to repair this file\n");
		  }
	     } #end if virus found

	} else {
	  print LOG "Error scanning $scanthis, scanresult= $scanresult[0]\n";
	  return(1,"Error scanning file $scanthis\n");
	} # end if bad scan
} # end donav

###################

1;

