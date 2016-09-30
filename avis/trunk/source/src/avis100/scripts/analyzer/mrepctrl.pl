####################################################################
#                                                                  #
# Program name:  mrepctrl.pl                                      #
#                                                                  #
# Description:   Control macro-virus replication                   #
#                                                                  #
#                                                                  #
# Statement:     Licensed Materials - Property of IBM              #
#                (c) Copyright IBM Corp. 1998-2000                 #
#                                                                  #
# Author:        Dave Chess, Jean-Michel Boulay                    #
#                                                                  #
#                U.S. Government Users Restricted Rights - use,    #
#                duplication or disclosure restricted by GSA ADP   #
#                Schedule Contract with IBM Corp.                  #
#                                                                  #
# Change history (see VSS or CMVC or whatever)                     #
#                                                                  #
#                                                                  #
####################################################################
#                                                                  #
# Function:                                                        #
#  When called with the Standard Dataflow Switches, schedules the  #
#  replications for this cookie-number, and decides when enough    #
#  have been done to proceed to manalysis.  Errors and             #
#  progress messages and such are written to $unc."mrepc.txt".     #
#                                                                  #
####################################################################

#use strict;
use Getopt::Long();            # For parameter parsing
use File::Path;
use Cwd;

use DataFlow();                # For talking to Dataflow
use AVIS::Macro::Macrodb();    # For finding out about the past
use AVIS::Macro::Macrocrc();   # For finding out about macros in files
use AVIS::FSTools();           # Random utility routines
use AVIS::Local();             # Various system globals
use AVIS::Crumbs();            # Interface to the Crumbs DB
use AVIS::Logger();            # For logging
use Mcv;

use AVIS::Macro::Types::Delta;
use AVIS::Macro::Types::Form;
use AVIS::Macro::Types::Target;
use AVIS::Macro::Types::List;
use AVIS::Macro::Types::Event;
use AVIS::Macro::Types::Goat;
use AVIS::Macro::FileProperties;
use AVIS::Macro::Implementations;


my $started = 1;       # For END to test, to exit if perl -c

#
# Some overall test and debug and strategy config for this script
#
$simple_test_only = 0;      # Set to 1 to do nothing much at all

#
# Other globals
#
$mrepc_done = 0;            # Must be set to 1 before normal exit; see END
$paranoid = 0;              # Insist on at least two of each form?
$min_rep_per_form = 2;         # Minimum number of samples per form required

#
# Maximum number of replication trials before aborting
#
$MAX_TRIALS = 10;

#number of previous runs
$prvmrep = 0;


#
# Get the parameters that were passed to us from Dataflow
#
Getopt::Long::GetOptions("Cookie=s" => \$Cookie,
                         "ProcessId=i" => \$ProcessID,
                         "Parameter=s" => \$Parameter,
                         "UNC=s" => \$unc);

# Do just the simple null-test, if requested
if ($simple_test_only) {
  # Just wait a random time, and schedule a random successor
  srand (time() ^ ($$ + ($$ << 15)));
  sleep int(rand 20) + 1;
  $outqueue = ((int(rand 2) == 0 ) ? $AVIS::Local::isqnMrep : $AVIS::Local::isqnManalysis);
  SpecifyStopping($Cookie, $ProcessID, $unc, $outqueue, "1 of 1");
  DoNext ($Cookie, $ProcessID, $unc, $outqueue, $Parameter);
  WriteSignatureForDataFlow ($ProcessID, $unc);
  return 1;
}

# Otherwise we're expected to actually do some work.

# Fix up the UNC, just in case
$unc = AVIS::FSTools::Canonize($unc);


# Get a logging object
$logger = new AVIS::Logger($unc);
$logger->screenEcho(1);      # Make it noisy
$logger->logString("Starting: $0");
$logger->logString("Cookie: $Cookie, ProcessID: $ProcessID, Parameter: $Parameter, UNC: $unc");

# Chdir to a local directory (necessary to use Macrocrc)
$initialdir = cwd();
$workdir = AVIS::FSTools::MkLocalWorkDir($Cookie, $ProcessID, $unc);
if (!chdir($workdir)) {
   $errmsg = "Unable to chdir to $workdir: $!";
   $logger->logString($errmsg);
   die "$errmsg Stopped ";  
}


#Read the crumbs database
my %Crumbs = AVIS::Crumbs::read($unc, $Cookie);

#
# Get a database-access object, to find out what's going on
#
$dbobject = new AVIS::Macro::Macrodb($Cookie,$unc,$ProcessID,$Parameter);
$dberr = $dbobject->LastError();
if ("" ne $dberr) {
  $errmsg = "Error opening database: $dberr";
  $logger->logString($errmsg);
  die "$errmsg Stopped ";
}
# Set the original file name
$dbobject->OriginalFileName($Crumbs{CaptureName});

#
# Get a Macrocrc object, for listing and caching and stuff
#
$mobject = new AVIS::Macro::Macrocrc();
$tooldir = AVIS::FSTools::Canonize($AVIS::Local::isdnBin,$AVIS::Local::isdnROOT);
$mobject->ToolDir($tooldir);
$mobject->UNC($unc);

#
# Find out if we've scheduled any replications yet, if any results
# are back.
#

$reptable = $dbobject->ReplicationTable();
$dberr = $dbobject->LastError();
if ("" ne $dberr) {
  $errmsg = "Error reading replication table: $dberr";
  $logger->logString($errmsg);
  die "$errmsg  Stopped ";
}
#get the number of previous operations (replications & conversions)
$prvmrep = scalar @$reptable;

if (!$prvmrep) {
  #nothing has been tried yet
  $original_sample = $dbobject->OriginalFileName();
  if (!AVIS::Macro::FileProperties::has_macros($original_sample, $mobject)) {
     $errmsg = "No macro found in $original_sample. Deferring.";
     $logger->logString($errmsg);
     die "$errmsg Stopped ";
  }
  if (!AVIS::Macro::FileProperties::IsViable($original_sample, $unc)) {
     my $msg = AVIS::Macro::FileProperties::NonViabilityDiagnostic($original_sample, $unc);
     $errmsg = "$original_sample is not viable ($msg). Deferring.";
     $logger->logString($errmsg);
     die "$errmsg Stopped ";
  }
}
else {  
  # Combine the individual goats into a single set of
  # replicants that are actually probably infected.
  @newtargetids = ();
  @newconverts = ();
  analyze_last_runs($reptable, \@newtargetids, \@newconverts);
}

$reps = $dbobject->Replications();
$dberr = $dbobject->LastError();
if ("" ne $dberr) {
  $errmsg = "Error reading replications: $dberr";
  $logger->logString($errmsg);
  die "$errmsg  Stopped ";
}

$forms = $dbobject->Forms();
$targets = $dbobject->Targets();

 if (0!=($rcount = schedule_replications($reptable,$reps,$mobject,[@newtargetids],[@newconverts]))) {
  $outqueue = $AVIS::Local::isqnMrep;
  DataFlow::SpecifyStopping($Cookie, $ProcessID, $unc, $outqueue, "$rcount of $rcount");
  for ($i=0;$i<$rcount;$i++) {
    DataFlow::DoNext($Cookie, $ProcessID, $unc, $outqueue, $prvmrep + $i);
  }
  DataFlow::WriteSignatureForDataFlow ($ProcessID, $unc);
} elsif (0!=($rcount = schedule_conversions($reptable,$reps,$forms,$mobject))) {
  $outqueue = $AVIS::Local::isqnMrep;
  DataFlow::SpecifyStopping($Cookie, $ProcessID, $unc, $outqueue, "$rcount of $rcount");
  for ($i=0;$i<$rcount;$i++) {
    DataFlow::DoNext($Cookie, $ProcessID, $unc, $outqueue, $prvmrep + $i);
  }
  DataFlow::WriteSignatureForDataFlow ($ProcessID, $unc);
} elsif (adequate_results($reptable,$reps,$mobject)) {
  $outqueue = $AVIS::Local::isqnManalysis;
  DataFlow::SpecifyStopping($Cookie, $ProcessID, $unc, $outqueue, "1 of 1");
  DataFlow::DoNext($Cookie, $ProcessID, $unc, $outqueue, $Parameter);
  DataFlow::WriteSignatureForDataFlow ($ProcessID, $unc);
} else {
  $outqueue = $AVIS::Local::isqnDefer;
  DataFlow::SpecifyStopping($Cookie, $ProcessID, $unc, $outqueue, "1 of 1");
  DataFlow::DoNext($Cookie, $ProcessID, $unc, $outqueue, $Parameter);
  DataFlow::WriteSignatureForDataFlow ($ProcessID, $unc);
}

#
# And that's really about all...
#
$mrepc_done = 1;
exit;




sub analyze_last_runs($$$) {
  my ($reptable, $newtargetidrf, $newconvertsrf) = @_;

  my @lastreplications = ();
  my @lastconversions = ();
  
  #get the list of events not yet analyzed
  my $runid = 0;
  foreach $event (@$reptable) {
    if ($event->NeedsPostProcessing()) {
      #mark as already analyzed
      $event->MarkAsPostProcessed();
      if ($event->IsReplication()) {
        push @lastreplications, $runid;
      }
      elsif ($event->IsConversion()) {
        push @lastconversions, $runid;
      }
      else {
        $logger->logString("Unknown operation type for run number $runid. Deferring");
        die;
      }
  
      #check whether the run should be rescheduled
      my $SWStatus = $dbobject->GetSWStatus($runid);
      if ((lc($SWStatus) eq "killed") && ($event->IsRepeatableOnFailure())) {
        $event->Reschedule();
      }
    }    
    ++$runid;
  }

  #update status for latest runs
  $dbobject->ReplicationTable($reptable);
  
  analyze_last_replications([@lastreplications], $newtargetidrf);
  analyze_last_conversions([@lastconversions], $newconvertsrf);
}


sub analyze_last_replications($$) {
  my @lastreplications = @{shift()};
  my $newtargetidsrf = shift; #reference to list of ids of new targets

  return if (!@lastreplications);
  
  #get the list of the goats involved in the last runs
  my @newgoats = ();
  foreach $run (@lastreplications) {
    my @rungoats = $dbobject->Goats($run);
    push @newgoats, @rungoats;
  }

  my $forms = $dbobject->Forms();
  my $targets = $dbobject->Targets();
  my $reps = $dbobject->Replications();
  my $converts = $dbobject->Conversions();

  #find out which goats were infected
  my @newreplicants = ();
  my @uninfectedgoats = ();
  
  foreach $goat (@newgoats) {
    #extract the delta
    my $delta = new AVIS::Macro::Types::Delta($goat->BackupCopy(), $goat->UsedCopy(), $mobject);
    # if delta not empty  
    if (!$delta->IsEmpty()) {
      #discard the delta if it is related to several applications  
      if (!$delta->HasOneApp()) {
        $logger->logString("Macros from multiple applications found in".$goat->UsedCopy().". Deferring");
        die;
      }
      #extract the form from each replicant
      my $form = new AVIS::Macro::Types::Form($delta);
      my $goatform;
      #find out whether it is a new form or a known form
      if (!$forms->Contains($form)) {
        #add the form to the list
        $forms->Add($form);
        $goatform = $form;
      }
      else {
        #increment the count
        my $similarform = $forms->FindSimilarItem($form);
        $similarform->IncrementCount();
        $goatform = $similarform;
      }
      
      #find out whether the replicant represents a new target
      my $target = new AVIS::Macro::Types::Target($goatform->Id(), $goat->Type());
      if (!$targets->Contains($target)) {
        #add the target to the list
        $targets->Add($target);
        $goat->TargetId($target->Id());
        push @$newtargetidsrf, $target->Id();
      }
      else {
        #increment the count
        my $similartarget = $targets->FindSimilarItem($target);
        $similartarget->IncrementCount();
        $goat->TargetId($similartarget->Id());
      }
      push @newreplicants, $goat;
    }    
    else {
      push @uninfectedgoats, $goat;
    }
  }
  
  #get read of the uninfected goats (and their backup copies if possible)
  if (@uninfectedgoats) {
    my %backupcopies = ();
    #check whether the clean copy is used for any infected goat
    foreach $rep (@newreplicants) {
      $backupcopies{$rep->BackupCopy()} += 1;
    }
    foreach $goat (@uninfectedgoats) {
      #get rid of the goat
      my $file = AVIS::FSTools::Canonize($goat->UsedCopy(), $unc);
      if (!(unlink $file)) {
        $logger->logString("Error unlinking $file: $!");
      };  
      if (!(exists $backupcopies{$goat->BackupCopy()})) {
        if (lc($goat->BackupCopy()) eq lc($dbobject->OriginalFileName())) {
          my $errmsg = "Illicit attempt to delete the original sample.";
          $logger->logString($errmsg);
          die "$errmsg Stopped ";  
        }
        $file = AVIS::FSTools::Canonize($goat->BackupCopy(), $unc);
        if ((-e $file) && !(unlink $file)) {
          $logger->logString("Error unlinking $file: $!");
        };  
      }
    }
  }
  
  #update the parenthood statistics  
  #update the database if some new replicants have been produced
  if (@newreplicants) {
    push @$reps, @newreplicants;  
    foreach $rep (@$reps) {
       foreach $goat (@newreplicants) {
         if ($goat->Father() eq $rep->UsedCopy()) {
            #increment the children count of the father
            $rep->IncrementChildrenCount();
            #flag the child as convert if the father is a convert
            if ($rep->IsConvert()) {
              my $prefix;
              if ($rep->IsUpConvert()) {
                $goat->MarkAsUpConvert();
                $prefix = "up_";
              }
              elsif ($rep->IsDownConvert()) {
                $goat->MarkAsDownConvert();
                $prefix = "dn_";
              }
              (my $fname = $goat->UsedCopy()) =~ m/(.*)\/(.*)/;
              my $mfname = $1."/".$prefix.$2;
              rename(AVIS::FSTools::Canonize($fname, $unc), AVIS::FSTools::Canonize($mfname, $unc))||warn "unable to rename $fname to $mfname: $!";
              $mfname =~ s/\\/\//g;
              $goat->UsedCopy($mfname);
            }
         }
       }    
    }  
    foreach $convert (@$converts) {
       foreach $goat (@newreplicants) {
         if ($goat->Father() eq $convert->UsedCopy()) {
            #increment the children count of the father
            #flag the child as convert if the father is a convert
              my $prefix;
              if ($convert->IsUpConvert()) {
                $goat->MarkAsUpConvert();
                $prefix = "up_";
              }
              elsif ($convert->IsDownConvert()) {
                $goat->MarkAsDownConvert();
                $prefix = "dn_";
              }
              (my $fname = $goat->UsedCopy()) =~ m/(.*)\/(.*)/;
              my $mfname = $1."/".$prefix.$2;
              rename(AVIS::FSTools::Canonize($fname, $unc), AVIS::FSTools::Canonize($mfname, $unc))||warn "unable to rename $fname to $mfname: $!";
              $mfname =~ s/\\/\//g;
              $goat->UsedCopy($mfname);         
         }
       }    
    }  
  
    $dbobject->Replications($reps);
    $dbobject->Forms($forms);
    $dbobject->Targets($targets);
  }


  #check for unhandled cases (polymorphic, devolving...) before investing time in anything else
  if (catastrophic_form_anomaly($forms)) {
    my $errmsg = "Deferring";
    $logger->logString($errmsg);
    die;
  }
}


sub analyze_last_conversions($$) {
  my @lastconversions = @{shift()};
  my $newconvertsrf = shift; #reference to list of ids of new targets
  
  return if (!@lastconversions);
  
  #get the list of potential converts produced in the last conversion attempts
  my @pconverts = ();
  my $reptable = $dbobject->ReplicationTable();
  foreach $eventid (@lastconversions) {
    my @eventpconverts = $dbobject->Goats($eventid);
    foreach $pconvert (@eventpconverts) {
      $pconvert->{TargetApp} = $reptable->[$eventid]->FinalApplication();
    }
    push @pconverts, @eventpconverts;
  }

  #find out which goats were infected
  my @failedconverts = ();
  
  foreach $pconvert (@pconverts) {
    #extract the delta
    my $delta = new AVIS::Macro::Types::Delta($pconvert->BackupCopy(), $pconvert->UsedCopy(), $mobject);
    # if delta not empty  
    if (!$delta->IsEmpty()) {
      #discard the delta if it is related to several applications  
      if (!$delta->HasOneApp()) {
        $logger->logString("Macros from multiple applications found in".$pconvert->UsedCopy().". Deferring");
        die;
      }
      #extract the form from each replicant
      my $form = new AVIS::Macro::Types::Form($delta);
      #check whether the form belongs to the wanted application
      if ($form->{App} eq $pconvert->{TargetApp}) {
        push @$newconvertsrf, $pconvert;
      }
      else {
        #complain and discard
        $logger->logString($pconvert->UsedCopy()." belongs to app ".$form->{App}." instead of ".$pconvert->{TargetApp}." Conversion failed. Discarding");
        push @failedconverts, $pconvert;
      }      
    }    
    else {
      #complain and discard
      $logger->logString($pconvert->UsedCopy()." contains no macros. Discarding");
      push @failedconverts, $pconvert;
    }
  }
  
  #delete the failed converts
  foreach $fconvert (@failedconverts) {
    #get rid of the file
    my $file = AVIS::FSTools::Canonize($fconvert->UsedCopy, $unc);
    if (!(unlink $file)) {
      $logger->logString("Error unlinking $file: $!");
    };  
  }
    
  if (@newconverts) {    
    #update the list of conversions
    my $converts = $dbobject->Conversions();
    my $reps = $dbobject->Replications();
    my $forms = $dbobject->Forms();
    my $targets = $dbobject->Targets();
    foreach $convert (@newconverts) {
      #find the father
       foreach $rep (@$reps) {
          if ($convert->Father() eq $rep->UsedCopy()) {
               #find out to which application the father belongs
               my $fathertargetid = $rep->TargetId();
               my $fathertarget = $targets->FindItemById($fathertargetid);
               my $fatherformid = $fathertarget->Formid();
               my $fatherform = $forms->FindItemById($fatherformid);
               my $orgapp = $fatherform->{App}; 
               my $cnvapp = $convert->{TargetApp};
               (my $orgrls = $orgapp) =~ s/.*?(\d+)/$1/;
               (my $cnvrls = $cnvapp) =~ s/.*?(\d+)/$1/;
                 my $prefix;
                 if ($orgrls < $cnvrls) {
                   $convert->MarkAsUpConvert();
                   $prefix = "up_";
                 }
                 elsif ($orgrls > $cnvrls) {
                   $convert->MarkAsDownConvert();
                   $prefix = "dn_";
                 }
		 else {
		   $logger->logString("Unable to figure out whether the converted sample ".$convert->UsedCopy()." is an upconvert or a downconvert");
		   next;
		 }
                 (my $fname = $convert->UsedCopy()) =~ m/(.*)\/(.*)/;
                 my $mfname = $1."/".$prefix.$2;
                 rename(AVIS::FSTools::Canonize($fname, $unc), AVIS::FSTools::Canonize($mfname, $unc))||warn "unable to rename $fname to $mfname: $!";
                 $mfname =~ s/\\/\//g;
                 $convert->UsedCopy($mfname);         
          }
      }    
      push @$converts, $convert;        
    }
    $dbobject->Conversions($converts);
  }
}


#
# Are the given replication attempts and results sufficient to pass
# the stuff along to analysis?  If so, return TRUE after entering the
# list of forms into the database.  Otherwise, return FALSE to cause
# (more) replications to be scheduled.
#
sub adequate_results($$$) {
  my $reptable = shift;
  my $reps = shift;
  my $mobject = shift;

  # If nothing's even been tried yet, return FALSE at once.
  if (@$reptable == 0) {
    return 0;
  }

  for ($i =0; $i < @$reptable; ++$i) {
    if (!-e AVIS::FSTools::Canonize("run$i.success",$unc)) {
      $logger->logString("An error occured during run $i. Check log file in ".AVIS::FSTools::Canonize($AVIS::Local::isdnRun."\\$i",$unc)." for more information");
    }
  }

  # Otherwise see if we can generate adequate forms from these replicants.
  my $forms = $dbobject->Forms();
  return plausible_forms($forms);
}



#
# Given the list of replications we've asked for so far, and the
# results that we've gotten back, can any more replications be
# scheduled?  If so, schedule them and the number of them.  If not,
# return FALSE (zero).
#
sub schedule_replications($$$$$) {
  my $reptable = shift;
  my $reps = shift;
  my $mobject = shift;
  my @newtargetids = @{shift()};
  my @newconverts = @{shift()};
  
  my @macrolist;
  my @runs = ();
  my @samples;
  my %sample;
  my $repforsmp;
  my @reruns = ();
  
  #check if we've already tried something
  if (@$reptable != 0) {  
    
    # check for the maximum number of attempts
    if (@$reptable >= $MAX_TRIALS) {
       $logger->logString("Maximum number of trials reached. Giving up...");
       return 0;
    }
    
    #check for runs to reschedule due
    foreach $event (@$reptable) {
      #find the last runs
      if ($event->ApprovedForRescheduling()) {
        my $newrun = new AVIS::Macro::Types::Event($event);
        #do not reschedule this one if if fails        
        $newrun->DisableRepetitionOnFailure();
        push @reruns, $newrun;
        $event->DoNotReschedule();
      }
    }    
    push @$reptable, @reruns;

    # check if there are some replicants
    if (@$reps != 0) {
      @samples = ();
      if (@newtargetids) {
        #pickup replicants belonging to new targets
        foreach $targetid (@newtargetids) {
          foreach $goat (@$reps) {
            if ($goat->TargetId() eq $targetid) {
               #check that the file is viable (template byte set for Word 7 for example)
               if (!AVIS::Macro::FileProperties::IsViable($goat->UsedCopy(), $unc)) {
                 my $msg = AVIS::Macro::FileProperties::NonViabilityDiagnostic($goat->UsedCopy(), $unc);
                 $errmsg = $goat->Path." is not viable ($msg).";
                 $logger->logString($errmsg);
               }
               else {
                 $logger->logString("Scheduling replication from type ".$goat->{Type});
                 $sample = {};
                 $sample->{TYPE} = $goat->Type();
                 $sample->{FILE} = $goat->UsedCopy();
                 $sample->{PATH} = $goat->Path();
                 push @samples, $sample;          
                 last;
               }
            }
          }
        }
      }
      elsif (@newconverts) {
        #pickup replicants belonging to new targets
          foreach $goat (@newconverts) {
               $logger->logString("Scheduling conversion from type ".$goat->{Type});
               $sample = {};
               $sample->{TYPE} = $goat->Type();
               $sample->{FILE} = $goat->UsedCopy();
               $sample->{PATH} = $goat->Path();
               push @samples, $sample;          
               last; 
          }
      }
      elsif (!@reruns) {
        #in case some targets are underrepresented, find the event that
        #produced one of the representants and reschedule it to try to get some more
        
        if ($forms->ItemNumber() <= 2) {#let's not waste our time if the virus is polymorphic
          foreach $target ($targets->ItemList()) {
            if ($target->{Count} < $min_rep_per_form) {
               #find one replicant representing the target
               foreach $rep (@$reps) {
                 if ($rep->TargetId() eq $target->Id()) {
                   #find the run that led to the replicant
                   my $event = $reptable->[$rep->{RunId}];
                   if ($event->IsRepeatableIfNeeded()) {
                     $logger->logString("Target ".$rep->TargetId()." is underrepresented (".$target->{Count}." replicants)");
                     $logger->logString("Rescheduling the run that led to ".$rep->UsedCopy());
                   
                     #create a new event with the same parameters as the old one
                     my $newevent = new AVIS::Macro::Types::Event($event);
                     push @$reptable, $newevent;
                     $event->DisableRepetitionIfNeeded();
                     last;
                   }
                 }
               }             
            }
          }
        }
      }
    }
    # else: give up
    elsif (!@reruns) {
      $logger->logString("Nothing more to try. Giving up...");
      return 0;
    }
  }
  else {
    # Otherwise schedule the correct replications for this sample
    $sample->{FILE} = $dbobject->OriginalFileName();
    $sample->{TYPE} = AVIS::Macro::FileProperties::guess_file_type($sample->{FILE}, $unc);
    $sample->{PATH} = undef;
    @samples = ($sample);
  }

  foreach $sample (@samples) {
    
    $repforsmp = obvious_replications($sample->{FILE}, $sample->{TYPE}, $sample->{PATH}, $mobject);
    if (@$repforsmp == 0) {   # No obvious replications exist
      $logger->logString("No obvious replications exist for ".$sample->{FILE}."...");
      return 0;
    }
    else {
      push @$reptable, @$repforsmp;
    }
  }  

  $dbobject->ReplicationTable($reptable);
  return scalar @$reptable - $prvmrep;
}




#
# Given the list of replications we've asked for so far, and the
# results that we've gotten back, can any more conversions be
# scheduled?  If so, schedule them and the number of them.  If not,
# return FALSE (zero).
#
sub schedule_conversions($$$$) {
  my ($reptable, $reps, $forms, $mobject) = @_;
  my @macrolist;
  my @runs = ();
  my @samples;
  my $repforsmp;

  my $cnvtable = $dbobject->ConversionTable();
  my @targets = $targets->ItemList();
  return 0 if !@targets;
  
  #check whether an attempt hasn't already been made for each suitable target        
  #step1: get the list of previous conversion attempts
  my %alreadytried = ();
  foreach $conversion (@$cnvtable) {
    my $key = "|".$conversion->{TARGETID}."|".$conversion->{TARGETEDAPP};
    $alreadytried{$key} = 1;
  }
  
  my $fname = $dbobject->OriginalFileName();
  my @originalsampleapps = AVIS::Macro::FileProperties::get_file_applications($fname, $mobject);

  #make sure the conversions haven't been tried before
  foreach $target (@targets) {
    my @cnvapps = suggest_target_conversions($target, [@originalsampleapps]);
    foreach $cnvapp (@cnvapps) {
    my $key = "|".$target->Id()."|".$cnvapp;
       if (!$alreadytried{$key} ==1) {         
         my $nextcnv = {};
         $nextcnv->{Target} = $target;
         $nextcnv->{App} = $cnvapp;
         push @nextcnvs, $nextcnv;
       }
    }
  }
  
  #make sure we are converting samples that replicate
  foreach $cnv (@nextcnvs) {
    my @targetreps = get_target_replicants($cnv->{Target});
    my $targetrep;    
    my $targetform = $forms->FindItemById($cnv->{Target}->{FORMID});
    my $targetapp = $targetform->{App};
    foreach $rep (@targetreps) {
      if ($rep->Children()) {
        $targetrep = $rep;
        last;
      }
    }
    if (!defined $targetrep) {
       my $errmsg = "Giving up for upconversion for target ".$cnv->{Target}->Id()." from ".$targetapp." to ".$cnv->{App};
       $logger->logString($errmsg);    
    }
    else {
       my $cnvforsmp = obvious_conversions($targetrep->{After}, $targetapp, $cnv->{App}, $cnv->{Target}->{DOCTYPE}, $mobject);
       push @$reptable, @$cnvforsmp;
       if (@$cnvforsmp == 0) {   # No obvious replications exist
         $logger->logString("No obvious replications exist for this sample...");
         return 0;
       }
       else {
          my $conversion = {};
          $conversion->{TARGETID} = $cnv->{Target}->Id();
          $conversion->{TARGETEDAPP} = $cnv->{App};
          $conversion->{ORIGINALAPP} = $targetapp;
          push @$cnvtable, $conversion;
       }
    }
  }

  $dbobject->ReplicationTable($reptable);
  $dbobject->ConversionTable($cnvtable);
  return scalar @$reptable - $prvmrep;
}


#returns the list of replicants belonging to a target
sub get_target_replicants($) {
  my $target = shift();

  my @targetreps = ();
  my $reps = $dbobject->Replications();
  foreach $rep (@$reps) {
    if ($rep->TargetId() eq $target->Id()) {
      push @targetreps, $rep;
    }
  }
  return @targetreps;
}


sub suggest_target_conversions($$) {
my $target = shift;
my @originalsampleapps = @{shift()};
my @targetedapps = ();

  if (!@originalsampleapps) {
    $logger->logString("Cannot figure out the application of the original sample. Unable to suggest any conversion.");
    return ();
  }
  if (@originalsampleapps > 1) {
    $logger->logString("The original sample is connected to several applications. Unable to suggest any conversion.");
    return ();
  }
  
  if (!($target->{DOCTYPE} eq "DOC")) {
    #the type of file linked to the target is not suitable for conversion
    #only documents are naturally converted
    return ();
  }
  
  my ($originalapp) = @originalsampleapps;
  my $forms = $dbobject->Forms();
  my $targetform = $forms->FindItemById($target->{FORMID});
  my $targetapp = $targetform->{App};
  #try conversions only for replicants related to the same application and version as the original sample
  if ($originalapp eq $targetapp) {
    if ($originalapp eq "WD7") {
      push @targetedapps, "WD8";
    }
    elsif ($originalapp eq "XL95") {
      push @targetedapps, "XL97";
    }
    elsif ($originalapp eq "XL97") {
      push @targetedapps, "XL95";
    }
    else {
      #do nothing
    }
  }
return @targetedapps;
}


#
# Suggest strategy-names to be used in getting this file to
# spread in this app, under this language.
#
sub suggest_replication_strategies($$$) {
  my ($fname, $app, $type) = @_;

  my @strategylist = ();

  # very simple for now

  #Word samples
  if (($app eq "WD7")||($app eq "WD8")) {
    if ($type eq "DOC") {
       my $strategy = {};
       $strategy->{COMMANDS} = "globinfcmds.lst";
       $strategy->{APP} = $app;
       push @strategylist, $strategy;
#       push @strategylist, "doc2doccmds.lst";
    }
    elsif ($type eq "GLOBAL") {
       my $strategy = {};
       $strategy->{COMMANDS} = "docinfcmds.lst";
       $strategy->{APP} = $app;
       push @strategylist, $strategy;
    }
    elsif ($type eq "UNKNOWN") {#sample
       my $strategy = {};
       $strategy->{COMMANDS} = "wunkcmds.lst";
       $strategy->{APP} = $app;
       push @strategylist, $strategy;
#       push @strategylist, "doc2doccmds.lst";
    }
    else {
       $errmsg = "replication of samples of type [$type] is not suppported for $app.";
       $logger->logString($errmsg);    
    }
  }
  #Excel samples
  elsif (($app eq "XL95")||($app eq "XL4")||($app eq "XL97")) {
    if ($type eq "DOC") {
       my $strategy = {};
       $strategy->{COMMANDS} = "excelcmds.lst";
       $strategy->{APP} = ($app eq "XL4") ? "XL95" : $app;
       push @strategylist, $strategy;
    }
    elsif ($type eq "DROPPED") {
       my $strategy = {};
       $strategy->{COMMANDS} = "startupcmds.lst";
       $strategy->{APP} = ($app eq "XL4") ? "XL95" : $app;
       push @strategylist, $strategy;
    }
    elsif ($type eq "UNKNOWN") {
       my $strategy = {};
       $strategy->{COMMANDS} = "xunkcmds.lst";
       $strategy->{APP} = ($app eq "XL4") ? "XL95" : $app;
       push @strategylist, $strategy;
    }
    else {
       $errmsg = "replication of samples of type [$type] is not suppported for $app.";
       $logger->logString($errmsg);    
    }
  }
  elsif ($app eq "PP97") {
    if ($type eq "DOC") {
       my $strategy = {};
       $strategy->{COMMANDS} = "powerpoint.lst";
       $strategy->{APP} = $app;
       push @strategylist, $strategy;
#       push @strategylist, "doc2doccmds.lst";
    }
    elsif ($type eq "GLOBAL") {
       my $strategy = {};
       $strategy->{COMMANDS} = "ppglobal.lst";
       $strategy->{APP} = $app;
       push @strategylist, $strategy;
    }
    elsif ($type eq "UNKNOWN") {#sample
       my $strategy = {};
       $strategy->{COMMANDS} = "powerpoint.lst";
       $strategy->{APP} = $app;
       push @strategylist, $strategy;
#       push @strategylist, "doc2doccmds.lst";
    }
    else {
       $errmsg = "replication of samples of type [$type] is not suppported for $app.";
       $logger->logString($errmsg);    
    }
  }
  else {
        $errmsg = "$app viruses are not supported.";
        $logger->logString($errmsg);
  }

  return @strategylist;
}

#
# Suggest strategy-names to be used in getting this file to
# spread in this app, under this language.
#
sub suggest_conversion_strategies($$$$) {
  my ($fname, $app, $cnvapp, $type) = @_;

  my @strategylist = ();

  if ($type eq "DOC") {
    #Word samples
    if (($app eq "WD7") && ($cnvapp eq "WD8")){
       my $strategy = {};
       $strategy->{COMMANDS} = "wupconv.lst";
       $strategy->{APP} = $cnvapp;
       push @strategylist, $strategy;
    }
    #Excel samples
    elsif (($app eq "XL95") && ($cnvapp eq "XL97")) {
       my $strategy = {};
       $strategy->{COMMANDS} = "xupconv.lst";
       $strategy->{APP} = $cnvapp;
       push @strategylist, $strategy;
    }
    elsif (($app eq "XL97") && ($cnvapp eq "XL95")) {
       my $strategy = {};
       $strategy->{COMMANDS} = "exdownconv.lst";
       $strategy->{APP} = $app;
       push @strategylist, $strategy;
    }
  }

  return @strategylist;
}

#
# return an arrayref referencing a replication table, suitable for
# recording in the database.  Use the given filename to decide just
# how to replicate: what app, what language, etc.  If the file isn't
# suitable at all, return a reference to an empty array.
#
sub obvious_replications($$$$) {
  my ($fname, $type, $samplepath, $mobject) = @_;

  my ($app, $lang, $strategy);
  my @answer;

  # Return empty arrayref if no macros found
  my @applist = AVIS::Macro::FileProperties::get_file_applications($fname, $mobject);
  if (@applist == 0) {
    return [];
  }

  my @langlist = AVIS::Macro::FileProperties::get_file_languages($fname, $mobject, $unc);

  foreach $app ( @applist ) {
    my @strategylist = suggest_replication_strategies($fname, $app, $type);
    foreach $strategy ( @strategylist ) {
       my @tmplanglist = AVIS::Macro::Implementations::match_application_nls([@langlist], $strategy->{APP}, $logger);
       foreach $lang ( @tmplanglist ) {
         my $event = new AVIS::Macro::Types::Event;
         $event->SampleFile($fname);
         $event->Application($strategy->{APP});
         $event->Nls($lang);
         $event->Commands($strategy->{COMMANDS});
         $event->SetAsReplication();
         if ((lc($type) eq "dropped") &&(defined $samplepath)){
           $event->SamplePath($samplepath);
         }
         push @answer, $event;
       }
    }
  }

  return \@answer;

}


#
# return an arrayref referencing a replication table, suitable for
# recording in the database.  Use the given filename to decide just
# how to replicate: what app, what language, etc.  If the file isn't
# suitable at all, return a reference to an empty array.
#
sub obvious_conversions($$$$$) {
  my ($fname, $app, $cnvapp, $type, $mobject) = @_;

  my ($lang, $strategy);
  my @answer;

  # Return empty arrayref if no macros found
  my @applist = AVIS::Macro::FileProperties::get_file_applications($fname, $mobject);
  if (@applist == 0) {
    return [];
  }

  my @langlist = AVIS::Macro::FileProperties::get_file_languages($fname, $mobject, $unc);
  my @strategylist = suggest_conversion_strategies($fname, $app, $cnvapp, $type);
  foreach $strategy ( @strategylist ) {
     my @filteredlanglist = AVIS::Macro::Implementations::match_application_nls([@langlist], $strategy->{APP}, $logger);
     foreach $lang ( @filteredlanglist ) {
       my $event = new AVIS::Macro::Types::Event;
       $event->SampleFile($fname);
       $event->Application($strategy->{APP});
       $event->Nls($lang);
       $event->Commands($strategy->{COMMANDS});
       $event->FinalApplication($cnvapp);
       $event->SetAsConversion();
       push @answer, $event;
     }
  }

  return \@answer;
}



#
# Given a reference to a list of CRCs (or anything else), how
# many different values are there?
#
sub count_distinct_entries($) {
  my $listref = shift;
  my %thishash = ();
  foreach ( @$listref ) {
    $thishash{$_} = 1;
  }
  return scalar keys %thishash;
}

#
# Is the list of virus forms one that we're pretty sure
# we understand well enough to make defs for?  Enough
# samples exist for each one, etc?
#
sub plausible_forms($) {
  my $forms = shift;
  my @forms = $forms->ItemList();
  my $thisform;
  my $anygood;    
  
  if (@forms == 0) { # Nothing to work with!
    $logger->logString("No virus forms found.");
    return 0;
  }

  if (catastrophic_form_anomaly($forms)) {return 0;}
  
  # Make sure we have at least two examples of each form, if we're
  # paranoid, and in any case make sure that not *every* form
  # has exactly one example.
  $anygood = 0;
  foreach $thisform ( @forms ) {
    if ($paranoid and $thisform->{Count}<$min_rep_per_form) {
      $logger->logString("Too few samples for some form.");
      return 0;
    }
    $anygood = 1 if $thisform->{Count}>1;
  }
  if (not $anygood) {
    $logger->logString("No form has more than one replicant.");
    return 0;
  }
  
  return 1;
}


#checks for things that would cause an immediate defer
sub catastrophic_form_anomaly($) {
  my $forms = shift;
  my @forms = $forms->ItemList();
  my %appcount;
  my ($thisform, $thisapp, $thiscount);
  
  # Make sure there are no more than two forms per app, eh?
  foreach $thisform ( @forms ) {
    $thisapp = $thisform->{App};
    $appcount{$thisapp} = 0 if not exists $appcount{$thisapp};
    $appcount{$thisapp} += 1;
    if ($appcount{$thisapp}>2) {
      $logger->logString("More than two $thisapp forms found.");
      $logger->logString("Virus is too complex (may be polymorphic or devolving).");
      return 1;
    }
  }

  # Make sure that each form for a given app has the same
  # number of *different* macros (i.e. no devolution)
  %appcount = ();
  foreach $thisform ( @forms ) {
    $thisapp = $thisform->{App};
    $thiscount = count_distinct_entries($thisform->{Crcvals});
    if ($appcount{$thisapp}) {
      if ($thiscount != $appcount{$thisapp}) {
        $logger->logString("Devolution occurred in $thisapp.");
        return 1;
      }
    }
    $appcount{$thisapp} = $thiscount;
  }

  return 0;
}


#
# Usual error-capturing stuff.
#

END {
  exit if not $started;
  exit if $simple_test_only;
  if ($mrepc_done) {
#   Do any last-minute normal cleanup in here.
    $logger->logString("mrepctrl finished normally.");
  } else {
#   Signal a terrible unexpected error, and clean up, here.
    $outqueue = $AVIS::Local::isqnDefer;    # Give up.
    DataFlow::SpecifyStopping($Cookie, $ProcessID, $unc, $outqueue, "1 of 1");
    DataFlow::DoNext ($Cookie, $ProcessID, $unc, $outqueue, $Parameter);
    DataFlow::WriteSignatureForDataFlow ($ProcessID, $unc);
    $logger->logString("mrepctrl finished not-normally; deferring.");
  }
  if (defined $workdir) {
    chdir($initialdir)||warn "unable to chdir to $initialdir: $!";
    rmtree($workdir, 0, 1)||warn "unable to rmtree($workdir): $!";
  }
  $logger->logString("Finishing: $0");
  undef $logger;
}

