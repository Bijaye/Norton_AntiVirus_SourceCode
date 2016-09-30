####################################################################
#                                                                  #
# Program name:  vrepctrl.pl                                       #
#                                                                  #
# Description:   simulation queue server for the binary virus      #
#                replication controller.                           #
#                                                                  #
# Statement:     Licensed Materials - Property of IBM              #
#                (c) Copyright IBM Corp. 1998-1999                 #
#                                                                  #
# Author:        Morton Swimmer, Till Teichmann                    #
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

# missing boot viruses handeling

use Getopt::Long;
use Time::Local;
use AVIS::Local;
use AVIS::Vides;
use AVIS::FSTools;
use AVIS::Crumbs;
use AVIS::ParamStr;
use AVIS::Logger;
use DataFlow;
use EntTrac;

use strict;

my $vrepc_done = 0;            # Must be set to 1 before normal exit; see END

my ($Cookie, $ProcessID, $Parameter, $unc);

GetOptions("Cookie=s" => \$Cookie,
   "ProcessID=i" => \$ProcessID,
   "Parameter=s" => \$Parameter,
   "UNC=s" => \$unc);

my $logfile = new AVIS::Logger($unc);

$logfile->logString("Starting: $0");
$logfile->logString("Cookie: $Cookie, ProcessID: $ProcessID, Parameter: $Parameter, UNC: $unc");
$logfile->logString("$unc/$isfnVRun does not exist") unless (-e "$unc/isfnRun");

###############################################################
########################## Variables ##########################
###############################################################

# VRUN variables
my (@runindex, @runtype, @rundate, @runstrat, @runstat, @runsample,
   $runnum);

# Samples variables
#my ($sampnum, @sampPath, @sampDetermination, @sampBoxPath,
#   @sampGoatName, @sampModifiedZones, @sampExecZones,
#   @sampAttributes, @sampRunNr);
#my ($sampnumTmp, @sampPathTmp, @sampDeterminationTmp, @sampBoxPathTmp,
#   @sampGoatNameTmp,  @sampModifiedZonesTmp, @sampExecZonesTmp,
#   @sampAttributesTmp, @sampRunNrTmp);
my (@SamplesTmp, @Samples);

# other local variables
my ($line, $nofile,$outqueue,$comfiles,$exefiles,$atime,$i,$j,$donext,
   $oldnum,$origsample,%crumbs,$newlines);

###############################################################
######################## Condition flags ######################
###############################################################

# Condition flags

# Condition 1: No COM files infected
my ($condNoCOMInfections)=0;

# Condition 2: No EXE files infected
my ($condNoEXEInfections)=0;

# Condition 3: No replicants
my ($condNoReplicants)=0;

# Condition 4: No replication round has finished
my ($condNoRunSuccessful)=0;

# Condition 5: Polpymorph virus
my ($condPolymorphicVirus)=0;

# Condition 6: Not enough infected COM-files
my ($condNotEnoughCOMInfections)=0;

# Condition 7: Not enough infected EXE-files
my ($condNotEnoughEXEInfections)=0;

# Condition 8: Max. number of replications round reached
my ($condMaxRoundsReached)=0;

###############################################################
######################### READ VRUN ###########################
###############################################################

%crumbs=AVIS::Crumbs::read($unc,$Cookie);
$origsample=$crumbs{'CaptureName'};

# get the actual time and use this in every vrun-line
$atime=time;

$nofile=ReadVRun($unc, $Cookie, \$runnum, \@runindex, \@runtype, \@rundate, \@runstrat, \@runstat, \@runsample);

###############################################################
##################### create new VRUN-List ####################
###############################################################

$newlines=0;

if ($nofile==0) {
   # create the first vrun line
   # set parameters for first trial

   $logfile->logString("Create VRun-List");

   for ($line=0; $line<1; $line++){
      $runnum=$line+1;            # number of lines
      $runindex[$line]=$line;
      $runtype[$line]=$isvaltype[2];
      $rundate[$line]=Date2Jap($atime);
      $runstrat[$line]=$isvalstrategy[$line];
      $runstat[$line]="REQUESTED";
      $runsample[$line]=$origsample;
   }

   WriteVRun($unc, $Cookie, \$runnum, \@runindex, \@runtype, \@rundate, \@runstrat, \@runstat, \@runsample);
   $outqueue=$isqnBrep;
   $donext=1;
   $newlines=1;

   $condNoReplicants=1;
   $condNoRunSuccessful=1;
   $condNotEnoughCOMInfections=1;
   $condNotEnoughEXEInfections=1;
   $condMaxRoundsReached=0;
}

###############################################################
####################### ANALYSE STATUS ########################
###############################################################

else{
   my $run;
   my $norep;

   $logfile->logString("Read in $runnum samples");

   # no replicants
   $condNoReplicants=1;
   # no round finished correct
   $condNoRunSuccessful=1;
   $oldnum=$runnum;
   # Read every sample.lst an evaluate it
   for ($run = 0; $run < $runnum; $run++) {
      # print "Read local sample-list\n";
      if ($runstat[$run] eq "REQUESTED") {
         # check the status of the replicaton
         $runstat[$run]=checkstatus($run);
      }
      if ($runstat[$run] eq "NOREP") {
         $condNoRunSuccessful=0;
         next;
      }
      if ($runstat[$run] eq "INTERROR") {
         next;
      }
      if ($runstat[$run] eq "CRASHED") {
         next;
      }
      if ($runstat[$run] eq "OK") {
         $condNoRunSuccessful=0;
         $condNoReplicants=0;
         #@sampPathTmp="";
         #@sampBoxPathTmp="";
         #@sampGoatNameTmp="";
         #@sampDeterminationTmp="";
         #@sampAttributesTmp="";
         #@sampModifiedZonesTmp="";
         #@sampExecZonesTmp="";
         #@sampRunNrTmp="";

         #ReadSamplesFile($unc, $Cookie, $run, \$sampnumTmp,
         #   \@sampPathTmp, \@sampBoxPathTmp, \@sampGoatNameTmp,
         #   \@sampDeterminationTmp, \@sampAttributesTmp,
         #   \@sampModifiedZonesTmp, \@sampExecZonesTmp,
         #   \@sampRunNrTmp);
	 @SamplesTmp = ReadSamplesFileMkII($unc, $Cookie, $run);
	 print "vrepctrl.pl: ".($#SamplesTmp+1)." samples returned from vrep.pl\n";
         #push @sampPath,          @sampPathTmp;
         #push @sampBoxPath,       @sampBoxPathTmp;
         #push @sampGoatName,      @sampGoatNameTmp;
         #push @sampDetermination, @sampDeterminationTmp;
         #push @sampAttributes,    @sampAttributesTmp;
         #push @sampModifiedZones, @sampModifiedZonesTmp;
         #push @sampExecZones,     @sampExecZonesTmp;
         #push @sampRunNr,         @sampRunNrTmp;
	 push @Samples, @SamplesTmp;

         #for ($j=0;$j<$#sampPathTmp;$j++) {
	 for ($j=0; $j <= $#Samples; $j++) {
	   #if ($sampGoatName[$j] =~ /\.COM/) {
	   if ($Samples[$j]->[$AVIS::Vides::iGoatName] =~ /\.COM/i) {
	     $comfiles++;
	   }
	   #if ($sampGoatName[$j] =~ /\.EXE/) {
	   if ($Samples[$j]->[$AVIS::Vides::iGoatName] =~ /\.EXE/i) {
	     $exefiles++;
	   }
         }
      }
   }

   #$sampnum=$#sampGoatName+1;
   my $sampnum=$#Samples+1;

   $logfile->logString("Write Sample-List");

   #WriteSamplesFile($unc, $Cookie, -1, \$sampnum,
   #   \@sampPath, \@sampBoxPath, \@sampGoatName,
   #   \@sampDetermination, \@sampAttributes,
   #   \@sampModifiedZones, \@sampExecZones,
   #   \@sampRunNr);
   WriteSamplesFileMkII($unc, $Cookie, -1, \@Samples);

   if ($runnum>=$isvalVMaxRepProc) {
      # no replications round anymore
      $logfile->logString("Maximum replications rounds reached");
      $condMaxRoundsReached=1;
   }
   if ($condNoReplicants) {
      $logfile->logString("No replicants");
      for ($line=$runnum; $line<$runnum; $line++){
         $runnum=$line+1;            # number of lines
         $runindex[$line]=$line;
         $runtype[$line]=$isvaltype[2];
         $rundate[$line]=Date2Jap($atime);
         $runstrat[$line]=$isvalstrategy[$line];
         $runstat[$line]="REQUESTED";
         $runsample[$line]=$origsample;

      }
      $donext=0;
      $newlines=0;
      # Write the updated vrun-file
      $logfile->logString("Write new VRUN-List");
      WriteVRun($unc, $Cookie, \$runnum, \@runindex, \@runtype, \@rundate, \@runstrat, \@runstat, \@runsample);
   } else {
      $logfile->logString("Some replicants");
      #$sampnum = scalar @Samples;
      $sampnum = $#Samples + 1;


###############################################################
####################### Multipartite  ? #######################
###############################################################

      my @filelist;
      my %partitions;
      my @uniq;

      $filelist[$#Samples+1]=$runsample[$run];
      %partitions=EntTrac::Partition(@filelist);
      @uniq=keys%partitions;

      if ($#uniq==$#filelist) {
         $logfile->logString("Multipartiter Virus");
         $condPolymorphicVirus=1;
         $runnum=$run+1;
         $runindex[$run]=$run;
         $runtype[$run]=$isvaltype[2];
         $rundate[$run]=Date2Jap($atime);
         $runstrat[$run]=$isvalstrategy[5];
         $runstat[$run]="REQUESTED";
         $runsample[$run]=$origsample;

         WriteVRun($unc, $Cookie, \$runnum, \@runindex, \@runtype, \@rundate, \@runstrat, \@runstat, \@runsample);
         $outqueue=$isqnBrep;
         $donext++;
         $newlines=1;
      }
      else{

###############################################################
##################### ANALYSE SAMPLE LINE #####################
###############################################################

         my (@line0,@line1);
         my $sindex;
         my (@sLength,@sDate,@sAccess,@sInfMeth,@sTarget,@sES,@sSA);

         $logfile->logString("infected COM files: $comfiles");
         $logfile->logString("infected EXE files: $exefiles");

         if ($comfiles!=0 || $exefiles!=0) {
            my $successfile = new AVIS::Logger("");
            $successfile->localFile("$unc\\success.log");
            $successfile->logString("EXE: $exefiles  COM: $comfiles");
         }
         for ($sindex=0; $sindex < $sampnum; $sindex++){
            #@line0=split(/-/,$sampDeterminationTmp[$sindex]);
	   @line0=split(/-/,$SamplesTmp[$sindex]->[$iDetermination]);
            $sES[$run]=$line1[0];
            $sSA[$run]=$line1[1];
            if ($sES[$run] eq "ES0") {
               # print "Expertsystem sucks\n";
            }
            elsif ($sSA[$run] eq "SA0") {
               # print "Static analyse no report\n";
            }
            else {
               # print "Virus\n";
            }
            #@line1=split(/-/,$sampAttributesTmp[$sindex]);
	   @line1=split(/-/,$SamplesTmp[$sindex]->[$iAttributes]);
            $sLength[$run]=$line1[0];
            $sDate[$run]=$line1[1];
            $sAccess[$run]=$line1[2];
            $sInfMeth[$run]=$line1[3];
            if ($sInfMeth[$run] =~ /n\/a/) {
            }
            if ($sInfMeth[$run] =~ //) {
            }
            $sTarget[$run]=$line1[4];
            if ($sTarget[$run] =~ /n\/a/) {
               $sTarget[$run]="COM";
            }
            if ($sTarget[$run] =~ /COM/) {
            }
            if ($sTarget[$run] =~ /EXE/) {
            }
         }
         if (-z "$unc//$isdnRun/$Parameter/$isfnVRepLog") {
         }elsif (-e "$unc//$isdnRun/$Parameter/$isfnVRepLog") {
            $logfile->logString("open log file");
            open("IN","$unc//$isdnRun/$Parameter/$isfnVRepLog") or die "log file not found";
            $_=<IN>;
            close (IN);
            # Searching after special sentence in the log file
            if ($_ =~ / /) {
               # action
            }
         }

         $outqueue="empty";

         if ($runnum>=$isvalVMaxRepProc) {
            # no replications round anymore
            $logfile->logString("Maximum replications rounds reached");
            $condMaxRoundsReached=1;
         }
         else{
            $line=$run;

            # set standard parameters for VRUN.LST

            if ($runstrat[$line] =~ /MASSREP/) {
               if ($isvalMaxPolySample>=$exefiles+$comfiles) {
                  $outqueue=$isqnBanalysis;
               }
            }
            if ($comfiles==0) {
               $logfile->logString("No infected COM-files");
               # COM files are not infected
               $runindex[$runnum]=$runnum;
               $runtype[$runnum]=$isvaltype[2];
               $rundate[$runnum]=Date2Jap($atime);
               $runstrat[$runnum]=$isvalstrategy[0];
               $runstat[$runnum]="REQUESTED";
               $runsample[$runnum]=$origsample;
               $condNoCOMInfections=1;
               $runnum++;
               $newlines++;
               $outqueue=$isqnBrep;
            }
            if ($exefiles==0) {
               $logfile->logString("No infected EXE-files");
               # EXE files are not infected
               $condNoEXEInfections=1;
               $runindex[$runnum]=$runnum;
               $runtype[$runnum]=$isvaltype[2];
               $rundate[$runnum]=Date2Jap($atime);
               $runstrat[$runnum]=$isvalstrategy[0];
               $runstat[$runnum]="REQUESTED";
               $runsample[$runnum]=$origsample;
               $runnum++;
               $newlines++;
               $outqueue=$isqnBrep;
            }
            if ($comfiles<$isvalVReqCOM) {
               $logfile->logString("Not enough infected COM-files");
               # not enough infected COM-Files
               $condNotEnoughCOMInfections=1;
               $runindex[$runnum]=$runnum;
               $runtype[$runnum]=$isvaltype[2];
               $rundate[$runnum]=Date2Jap($atime);
               $runstrat[$runnum]=$isvalstrategy[0];
               $runstat[$runnum]="REQUESTED";
               $runsample[$runnum]=$origsample;
               $runnum++;
               $newlines++;
               $outqueue=$isqnBrep;
            }
            if ($exefiles<$isvalVReqEXE) {
               $logfile->logString("Not enough infected EXE-files");
               # not enough infected EXE-Files
               $condNotEnoughEXEInfections=1;
               $runindex[$runnum]=$runnum;
               $runtype[$runnum]=$isvaltype[2];
               $rundate[$runnum]=Date2Jap($atime);
               $runstrat[$runnum]=$isvalstrategy[0];
               $runstat[$runnum]="REQUESTED";
               $runsample[$runnum]=$origsample;
               $runnum++;
               $newlines++;
               $outqueue=$isqnBrep;
            }
            $donext++;
         }
         # Write the updated vrun-file
         $logfile->logString("Write VRun-list");
         WriteVRun($unc, $Cookie, \$runnum, \@runindex, \@runtype, \@rundate, \@runstrat, \@runstat, \@runsample);
      }
   }
}

$logfile->logString("Create DoNext lines");

# MS hack to prevent infinite looping, because I can't really figure
# out why it is doing this
if ($runnum > 3) {
    $logfile->logString("Forcing replication termination");
    $condNoRunSuccessful = 1;
    $condMaxRoundsReached = 1;
    $newlines = 0;
}
# end of hack

if ($newlines!=0) {
    SpecifyStopping($Cookie, $ProcessID, $unc, $outqueue, "$newlines of $newlines");
    for ($i=0;$i<$newlines;$i++) {
	$j=$oldnum+$i;
	$outqueue=$isqnBrep;
	DoNext ($Cookie, $ProcessID, $unc, $outqueue, $j);
    }
    $logfile->logString("New VREP-Runs: $newlines");
} else {
    SpecifyStopping($Cookie, $ProcessID, $unc, $outqueue, "1 of 1");
    if ($condNoRunSuccessful && $condMaxRoundsReached) {
	$outqueue=$isqnDefer;
	DoNext ($Cookie, $ProcessID, $unc, $outqueue, $Parameter );
    } elsif ($condNoReplicants && $condMaxRoundsReached) {
	$outqueue=$isqnDefer;
	DoNext ($Cookie, $ProcessID, $unc, $outqueue, $Parameter );
    } elsif ($condMaxRoundsReached && !$condNoReplicants) {
      $outqueue=$isqnBanalysis;
      DoNext ($Cookie, $ProcessID, $unc, $outqueue, $Parameter );
   } elsif (!$condNotEnoughCOMInfections && !$condNotEnoughEXEInfections && $condMaxRoundsReached) {
      $outqueue=$isqnBanalysis;
      DoNext ($Cookie, $ProcessID, $unc, $outqueue, $Parameter );
   } else {
      $outqueue=$isqnBanalysis;
      DoNext ($Cookie, $ProcessID, $unc, $outqueue, $Parameter );
   }
}

$logfile->logString("Conditions: [$condNoCOMInfections, $condNoEXEInfections, $condNoReplicants, $condNoRunSuccessful, $condPolymorphicVirus, $condNotEnoughCOMInfections, $condNotEnoughEXEInfections, $condMaxRoundsReached]");
$logfile->logString("Outqueue: $outqueue");
$logfile->logString("Finishing: $0");

WriteSignatureForDataFlow ($ProcessID, $unc);
$vrepc_done = 1;
exit;

###############################################################
########## analyse the status for one vrun-line ###############
###############################################################

sub checkstatus ($) {

my ($run)=@_;
my $status;
my $localstatus;

# analyse if replication for successful, failed (no replication,
# internal error, crashed)

   $localstatus="CRASHED";
   if (-e "$unc/$isdnRun/$run/$isfnVRepStat") {
      open IN,"<$unc/$isdnRun/$run/$isfnVRepStat";
      while ($status=<IN>){
         my %hstatus;
         %hstatus=UnpackPara($status);

         if (defined $hstatus{REPLICANTS} && $hstatus{REPLICANTS}==0) {
            $localstatus="NOREP";
         }
         if (defined $hstatus{STATUSMSG} && $hstatus{STATUSMSG} eq "OK") {
            $localstatus="OK";
         }
         if (defined $hstatus{STATUSMSG} && $hstatus{STATUSMSG} eq "INTERROR") {
            $localstatus="INTERROR";
         }
         if (defined $hstatus{ENDTIME} && $hstatus{ENDTIME}==0) {
            $localstatus="CRASHED";
         }
      }
      close (IN);
   }
   else{
      $logfile->logString("!!!!!!!!!!!!warning file status.txt could not be opened");
   }
   $logfile->logString("Check status: $localstatus");
   return ($localstatus);
}


END {
  if ($vrepc_done) {
#   Do any last-minute normal cleanup in here.
    $logfile->logString("vrepctrl finished normally.");
  } else {
#   Signal a terrible unexpected error, and clean up, here.
    $outqueue = $AVIS::Local::isqnDefer;    # Give up.
    DataFlow::SpecifyStopping($Cookie, $ProcessID, $unc, $outqueue, "1 of 1");
    DataFlow::DoNext ($Cookie, $ProcessID, $unc, $outqueue, $Parameter);
    DataFlow::WriteSignatureForDataFlow ($ProcessID, $unc);
    $logfile->logString("vrepctrl finished not-normally; deferring.");
  }
  $logfile->logString("Finishing: $0");
  undef $logfile;
}
