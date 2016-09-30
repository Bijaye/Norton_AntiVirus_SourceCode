# Perl Package for Logging Information
# IBM Research
# Robert B. King
# March 27, 1997
#
#

package LogInfo;

#----------------------------------------------------------------------
# This function is used to open the log file.
#----------------------------------------------------------------------

sub open_log
{
    # Parse the parameter list in the usual way and store the individual
    # parameters in local variables.

    local($logfile,$append) = @_;

    # Open the logfile

    if (defined $append)
    {
        open LOG, ">>$logfile" or die "Can't open for appending log file $logfile, $!, stopped";
    }
    else
    {
        open LOG, ">$logfile" or die "Can't open and zero log file $logfile, $!, stopped";
    }

    # Make the logfile unbuffered
    my $oldfh = select LOG; $| = 1; select $oldfh;
}

#----------------------------------------------------------------------
# This function closes the log file.
#----------------------------------------------------------------------

sub close_log
{
    close LOG;
}


#----------------------------------------------------------------------
# This function is used to log information to the file.
#----------------------------------------------------------------------

sub to_log
{
    # Parse the parameter list in the usual way and store the individual
    # parameters in local variables.

    local($params) = join '', @_;

    # Loop through string, printing as each \n is encountered
    local($pos)=0;
    local($len)=-1;
    while (($len = index($params, "\n", $pos)) > -1)
    {
        print LOG scalar(localtime) . ': ' . substr($params, $pos, 1+$len);
	$pos = ++$len;
    }

    # Print the remainder of the string
    print LOG scalar(localtime) . ': ' . substr($params, $pos) . "\n";
}

1;
