# Perl Package for Pruning Old Files/Directories
# IBM Research
# Robert B. King
# March 27, 1997
# Last Updated on May 16, 1997
#
#

use LogInfo ();
use File::Path ();

package PruneOld;

#----------------------------------------------------------------------
# This function is used to delete old files/directories that are no
# longer needed.  The caller can either specify the maximum number
# of versions to keep and/or the maximum number of days to keep.
#----------------------------------------------------------------------

sub files
{
    local($debug) = 0;

    # Parse the parameter list in the usual way and store the individual
    # parameters in local variables.

    local($directory,$pattern,$number,$age) = @_;

    # Check for debug (e.g., number is negative)

    if ($number < 0)
    {
        $debug  = 1;
	$number = -$number;
    }

    # Read the specified directory and store the names it contains in
    # an array.

    opendir(DIR, $directory) or die "Can't open $directory, $!, stopped";
    local(@names) = readdir(DIR);
    closedir(DIR);

    # Sort the directory list in reverse numeric order
    sub revnum { $b cmp $a; }
    local(@sortnames) = sort revnum @names;

    # Massage the directory name appropriately so that it can be
    # correctly prepended to the files or directories contained
    # in it.
    if ($directory eq "\\")
    {
        # If root directory of partition is specified, chop
        # it off entirely
        chop $directory;     # Chop off \
    }
    elsif (substr($directory, -2, 2) eq "\\.")
    {
        # Chop off trailing \. on directory if it is present
        chop $directory;     # Remove trailing .
        chop $directory;     # Remove trailing \        
    }

    # For each name in the directory
    foreach (@sortnames)
    {
        next unless -e "$directory\\$_";
        next unless /$pattern/i;

        if ((--$number < 0) || (($age != 0) && (-M "$directory\\$_" > $age)))
        {
	    if (-d "$directory\\$_")
            {
        	LogInfo::to_log("Pruning the directory $directory\\$_");
		File::Path::rmtree(["$directory\\$_"], 0, 1) if ($debug == 0);
	    }
	    else
	    {
        	LogInfo::to_log("Pruning the file $directory\\$_\n");
                if ($debug == 0)
                {
                    if (unlink("$directory\\$_"))
                    {
                        print STDERR "Can't delete $directory\\$_, $!, continuing\n";
		    }
		}
	    }
	}
        else
        {
	    LogInfo::to_log("Not pruning $directory\\$_");
	}
    }

    return 0;
}

1;
