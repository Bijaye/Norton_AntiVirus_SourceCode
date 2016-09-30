#!c:/perl/bin/perl

use strict;
use SDGS;
use BOSupport;

my $retry_cycle = 1; # seconds

# Parse ARGV

my $parm_hash = BOSupport::parse_parms(@ARGV);

# Formal Input parameter declarations

my $import_unc        = $parm_hash->{'UNC'};
my $import_parameter  = $parm_hash->{'Parameter'};
my $import_cookie     = $parm_hash->{'Cookie'};
my $import_process_id = $parm_hash->{'ProcessId'};

# Export Parameter declarations (with null initialization)
my ($next_service) = split /\s\s/,$import_parameter;
my $export_parameter = "";

############################################################
# Local Logic
############################################################

my $status = $SDGS::SDGS_FAIL;

#while ($status == $SDGS::SDGS_FAIL)
{   
### No need to be in a loop for unlocking.  Both Success and Failure will have the same 
### next service set.  So, commenting the while loop and the if conditions.  ---Raju & Senthil

    $status = SDGS::Unlock();
#    if ($status == $SDGS::SDGS_FAIL)
#    {
#        sleep($retry_cycle);
#
#    }
#    if ($status == $SDGS::SDGS_OK)
#    {
#          $next_service = $import_parameter;
#    }  
   
}

BOSupport::dataflow_completion($import_cookie,
                               $import_process_id,
                               $import_unc,
                               $next_service,
                               $export_parameter,
                               "");
