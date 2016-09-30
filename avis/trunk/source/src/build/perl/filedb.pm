# Perl Package for Creating a File Search Database
# IBM Research
# Robert B. King
# March 16, 1997 (last modified on May 13, 1997)
#
#
#

package FileDB;

use directory ();


#----------------------------------------------------------------------
# 
# create_file_db()
#
# This function loads the platform database which includes searching for
# the makefiles that will need to be called.
#
#----------------------------------------------------------------------

sub create_file_db
{
    # Parse the parameter list in the usual way
    local($ctrldir, $ctrlpattern, $filesuffix, $searchdirs, $restrpattern) = @_;

    # Read the List of Files in the Platform-Specific Directory and store
    # the names in an array.
    opendir(DIR, $ctrldir) or die "Can't open $ctrldir, $!, stopped";
    local(@ctrlFiles) = readdir(DIR);
    closedir(DIR);

    # Loop Through List of Platforms, build datastructure of makefiles
    local(@file_db)=();

    foreach (@ctrlFiles)
    {
        next unless -f "$ctrldir\\$_";       # Only Process Files
        next unless (/$ctrlpattern/i);       # Ignore Invalid File Extensions
        local($control) = $1;

        # Check to see if there is a restricted build environment list
        if (defined $restrpattern)
        {
            # Check to see if this platform should be restricted
            if (($$restrpattern{TYPE} == 0) &&          ## Exclude
                (defined $$restrpattern{$control}) &&   ## Defined
                ($$restrpattern{$control} == 0))        ## Exclude It!
            {
                next;
            }
            
            # Check to see if this platform should be included
            if (($$restrpattern{TYPE} == 1) &&          ## Include Only
                (not defined $$restrpattern{$control})) ## Not Defined
            {
                next;
            }
        }

        # Check to see if this platform has already been defined
        if (defined $file_db{$control})
        {
            next;
        }

        local(@files)=();
        foreach (@$searchdirs)
        {
            @files = (@files, Directory::directories("$_", "($control)$filesuffix"));
        }

        $file_db{$control} = [ sort @files ];    # Sort the files in alphabetical order
    }

    return %file_db;
}


#----------------------------------------------------------------------
# 
# print_file_db()
#
# This function prints the platform database.
#
#----------------------------------------------------------------------

sub print_file_db
{
    # Parse the parameter list in the usual way
    local(%file_db) = @_;

    foreach $plats (keys %file_db)
    {
        print "Control = $plats\n";
        foreach (@{$file_db{$plats}})
        {
            print "File = $_\n";
        }
    }
}

1;
