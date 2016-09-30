# Perl Package for Getting a Recent List of Files (with levels)
# IBM Research
# Robert B. King
# March 27, 1997
# Last Modified on April 9, 1997
#
#

package RecentList;


#----------------------------------------------------------------------
# This function generates a list of levels that are greater than the
# third argument if it is defined.
#
# Directory Syntax:
#   ""    assumes the current directory
#   "\\"  assumes the root directory of the current drive
#
#----------------------------------------------------------------------

sub get_list
{
    local(@tozip)=();

    # Parse the parameter list in the usual way and store the individual
    # parameters in local variables.
    local($directory,$pattern,$lastbuilt) = @_;

    # Read the specified directory and store the names it contains in
    # an array.
    if (not opendir(DIR, "${directory}."))
    {
        return(undef);
    }
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

        if ((not defined $lastbuilt) or ($lastbuilt eq ""))
        {
            return $1
        }
        if ($1 > $lastbuilt)
        {
            unshift @tozip, $1;
        }
        else
        {
            return @tozip;
        }
    }

    return @tozip;
}

1;
