# Perl Script to Perform IBM AntiVirus Product Build
# IBM Research
# Originally written by Edward Pring
# Modified for Arrays and Package Created by Robert B. King
# March 16, 1997 (last modified on March 14, 1997)
#
#
#

package Directory;


# @files = &directories($ARGV[0], "($ARGV[1]).mak\$");
# 
# foreach (@files)
# {
#     print "Match = $_\n";
# }
# 
# exit 0;


#----------------------------------------------------------------------
# This function recursively descends a directory tree and calls 
# another function for files that match a specific pattern.
# The function parameters are:
# 
# 1) the root of the directory tree to be searched.  The directory may 
#    be specified as a relative or absolute pathname with or without a 
#    drive letter, or as a UNC pathname.
# 
# 2) the filename pattern to search for.  The pattern must be specified
#    as a regular expression.
# 
# The function first checks all of the files in each directory before 
# recursively checking each sub-directory.
#----------------------------------------------------------------------

sub directories
{
    local(@matches,@recursive);

    # Parse the parameter list in the usual way and store the individual
    # parameters in local variables.

    local($directory,$pattern) = @_;

    # Read the specified directory and store the names it contains in 
    # an array.
    
    opendir(DIR, $directory) or die "Can't open $directory, $!, stopped";
    local(@names) = readdir(DIR);
    closedir(DIR);

    # For each name in the directory that is a file, call the specified
    # function only if the name matches the specified pattern.

    foreach (@names) 
    { 
	next unless -f "$directory\\$_";
        next unless /$pattern/i;

        @matches=(@matches, "$directory\\$_");
    }

    # For each name in the directory that is a sub-directory (except the
    # directories "." and ".."), call this function recursively.							    
    foreach (@names)
    {
	next unless -d "$directory\\$_";
	next if $_ eq ".";
	next if $_ eq "..";
	@recursive=directories("$directory\\$_",$pattern);
        @matches=(@matches, @recursive);
    }

    return @matches;
}

1;
