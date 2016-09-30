# Perl Package for Manipulating Build Environment Variables
# IBM Research
# Robert B. King
# March 16, 1997 (last modified on March 21, 1997)
#
#
#

package BldEnv;

#
# This function prints the current environment variables settings
#

sub print_environment
{
    # Parse the parameter list in the usual way and store the individual
    # parameters in local variables.
    local($fh) = @_;

    if (not defined $fh)
    {
        $fh = \*STDOUT;
    }

    print $fh "\nEnvironment variables:\n";

    foreach (sort(keys(%ENV)))
    {
        print $fh "   $_=$ENV{$_}\n";
    }
}

#----------------------------------------------------------------------
# 
# read_environment(filename)
#
# This function reads in an environment list from the specified file
# in the specified directory.
#
# 1) A fully specified filename.
#
# Returns a pairwise list of the entries.
#
#----------------------------------------------------------------------

sub read_environment
{
    # Parse the parameter list in the usual way
    local($filename) = @_;

    # Read the specified file and store the names it contains in an
    # array.
    open(INPUT, "<$filename")
	or die "Can't open $filename, $!, stopped";
    local(@variables) = <INPUT>;
    close(INPUT);

    local(@environment)=();

    # For each line in the file
    foreach (@variables)
    {
        next if /^#/;   # Ignore Comments
    
        # Parse the line
        if (/^[ \n\t\r]*(\w+)[ \t]+(.+)[ \r\t\n]*$/)
        {
	    push @environment, [ $1, $2];
	}
    }

    return @environment;
}


#----------------------------------------------------------------------
# 
# set_environment(array, FILEHANDLE)
#
# This function will do the correct thing.
#
# The second argument expects a pointer to an array, rather than the
# array itself.
#
#----------------------------------------------------------------------

sub set_environment
{
    # Parse the parameter list in the usual way
    local($dosbat, $environment) = @_;

    # Check to see if the environment variable is defined
    if (not defined @{$environment})
    {
        local($package,$filename,$line) = caller;
        print "No Environment Array, called from $package, file $filename, line $line\n";
        return;
    }

    # For each line in the file
    for $i ( 0 .. $#{$environment} )
    {
	local($result) = eval(${$environment}[$i][1]);
        $ENV{${$environment}[$i][0]}=$result;

#       print "Setting ${$environment}[$i][0] to $result\n";

        if (defined $dosbat)
        {
            local($left)  = ${$environment}[$i][0];
   	    local($right) = ${$environment}[$i][1];
            $right=~ s/(\$ENV{)([a-zA-Z0-9]+)(})/%$2%/g;
  	    $right=eval($right);

            print $dosbat "set $left=$right\n";
	}
    }
}


#----------------------------------------------------------------------
# 
# resetenv(%envvars)
#
# This function sets the current environment variable settings based
# on the values specified in the environment variable array.
#
# The second argument expects a pointer to the associated array,
# rather than the array itself.
#
#----------------------------------------------------------------------

sub reset_environment
{
    # Parse the parameter list in the usual way
    local($dosbat, $envvars) = @_;

    # Set the environment variables
    foreach $key (keys %ENV)
    {
        if (${$envvars}{$key} eq "")
        {
            $ENV{$key}="";       # Ensure that the variable is deleted
            delete $ENV{$key};   # This does not seem to being working as designed.
            if (defined $dosbat)
	    {
		print $dosbat "set $key=\n";
	    }
        }
        elsif (${$envvars}{$key} ne $ENV{$key})
        {
            $ENV{$key}=${$envvars}{$key};
	    if (defined $dosbat)
	    {
		if ($key eq "PATH")
                {
                    print $dosbat "set $key=%SAVEPATH%\n";
		}
                else
                {
                    print $dosbat "set $key=$ENV{$key}\n";
                }
	    }
        }
    }
}

1;

