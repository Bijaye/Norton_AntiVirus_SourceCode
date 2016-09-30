# Perl Script to Parse Arguments for IBM AntiVirus Product Build
# Modified for Immune System Robert and Glenn
# 09/29/98
#
# IBM Research
# Robert B. King
# March 16, 1997 (last modified on October, 12 1998)
#
#
#

use Getopt::Long;

@defaultlangorder  = ('ea', 'ja', 'de', 'eb', 'fr', 'it', 'pb', 'sp');

@defaultlangtypes  = ('ea', 'ja', 'de', 'eb', 'fr', 'it', 'pb', 'sp');
$defaultbuildtypes = "rd";
$defaultsite       = "haw";

%builddescriptions =
(
    r     =>   'Product',
    rd    =>   'Product & Development',
    d     =>   'Development',
);

#----------------------------------------------------------------------
#
# validate_arguments()
#
# This function is used to retrieve and valid arguments.
#
#----------------------------------------------------------------------

sub validate_arguments
{
    local(%optctl);
    local($tempval);
    local($errcnt)  = 0;
    local($drive)   = undef;
    local($release) = undef;
    local($level)   = undef;
    local($os)      = undef;
    local(%bldenv)  = ();
    local($debug)   = 0;
    local($spin)    = 1;
    local($build)   = undef;
    local($site)    = $defaultsite;
    local(@lang)    = @defaultlangtypes;

    # Retrieve the arguments
    local($retval) = &GetOptions(\%optctl, 'drive=s', 'release=s',
				 'level=s', 'spin:i', 'buildos=s',
				 'environment=s@', 'noenvironment=s@',
				 'lang=s%', 'type=s%', 'site=s',
				 'scriptdebug!', 'develop', 'product');
    if ($retval == 0)
    {
	$errcnt++;
    }

    # Iterate through all possible values.
    foreach $_ (keys %optctl)
    {
        if ($_ eq "drive")
        {
            $drive=substr($optctl{$_}, 0, 1);

            # Verify that a valid drive letter was specified.
            if (not -d "$drive:\\.")
            {
                print STDERR "\n***** Error: Invalid Drive Letter ($drive) Specified\n";
		$errcnt++;
	    }
        }
	elsif ($_ eq "release")
	{
            $release=$optctl{$_};
	}
        elsif ($_ eq "level")
        {
            $tempval=int $optctl{$_};
            if (($tempval eq "") || ($tempval < 0) || ($tempval > 999))
	    {
		print STDERR "\n***** Error: Invalid Level Number ($optctl{$_}) Specified\n";
                print STDERR "      The valid range is 0 to 999.\n";
		$errcnt++;
            }
	    else
            {
                $level=sprintf "%03u", $tempval;
	    }
        }
        elsif ($_ eq "spin")
        {
            if (($optctl{$_} eq "") || ($optctl{$_} < 1) || ($optctl{$_} > 999))
            {
		print STDERR "\n***** Error: Invalid Spin Number ($optctl{$_}) Specified\n";
                print STDERR "      The valid range is 1 to 999.\n";
		$errcnt++;
            }
	    else
	    {
                $spin=int $optctl{$_};
            }
	}
        elsif ($_ eq "buildos")
        {
            if (($optctl{$_} eq 'dos') || ($optctl{$_} eq 'w32') ||
                ($optctl{$_} eq 'os2') || ($optctl{$_} eq 'w16'))
            {
		$os=$optctl{$_};
	    }
	    else
            {
		print STDERR "\n***** Error: Invalid Build Operating System ($optctl{$_}) Specified\n";
                print STDERR "      On an NT Computer, either \"w32\" or \"w16\" are valid\n";
                print STDERR "      On an OS/2 Computer, either \"dos\" or \"os2\" are valid\n";
		$errcnt++;
            }
        }
        elsif ($_ eq "lang")
	{
            if (defined $optctl{lang}{all})
	    {
                @lang=@defaultlangtypes;
            }
            else
            {
                @lang=();
                foreach (@defaultlangtypes)
	        {
                    if ($_ eq "ea")
                    {
			# Must build English first for all builds
                        push @lang, $_;
		    }
		    elsif (defined $optctl{lang}{$_})
		    {
		        push @lang, $_;
		    }
	        }
            }
        }
        elsif ($_ eq "develop")
	{
            if (defined $build)
            {
                if (($build eq "d") || ($build eq "rd"))
                {
 		    print STDERR "\n***** Warning: Development Build Already Specified.\n";
		}
                else
                {
		    $build = $build . "d";
		}
            }
            else
            {
                $build = "d";
            }
        }
        elsif ($_ eq "product")
	{
            if (defined $build)
            {
                if (($build eq "r") || ($build eq "rd"))
                {
 		    print STDERR "\n***** Warning: Product Build Already Specified.\n";
		}
                else
                {
		    $build = "r" . $build;
		}
            }
            else
            {
                $build = "r";
            }
        }
        elsif ($_ eq "site")
        {
            # Cannot verify that it is valid until we know the root of the build tree
            $site=$optctl{$_};
        }
	elsif ($_ eq "scriptdebug")
        {
            $debug = $optctl{$_};
	}
	elsif ($_ eq "environment")
        {
            if (defined $bldenv{TYPE})
            {
		print STDERR "\n***** Error: Environment Type Already Specified\n";
		$errcnt++;
            }
            else
            {
		$bldenv{TYPE}=1;
                foreach $tempval (@{$optctl{$_}})
                {
                    $bldenv{$tempval} = 1;
                }
            }
        }
        elsif ($_ eq "noenvironment")
        {
            if (defined $bldenv{TYPE})
            {
		print STDERR "\n***** Error: Environment Type Already Specified\n";
		$errcnt++;
            }
            else
            {
		$bldenv{TYPE}=0;
                foreach $tempval (@{$optctl{$_}})
                {
                    $bldenv{$tempval} = 0;
                }
            }
        }
    }

    # Set defaults for optional values if there were not specified by the builder.
    if (not defined $build)
    {
	$build=$defaultbuildtypes;
    }

    # Check to See if No Errors Occurred
    if ($errcnt == 0)
    {
        # Check to see if the Drive Letter, Release, and Level are
        # together valid -- e.g., verify that it is a directory.
        local($root) = "$drive:\\avis${release}.${level}";
	if (not -d $root)
	{
	    print STDERR "\n***** Error: Invalid Combination of Drive, Release, and Level Specified\n";
            print STDERR "      Resulting Combination Yielded $root\n";
            if (not defined $drive)
	    {
		print STDERR "      A Drive Letter Must be Specified\n";
	    }
            if (not defined $release)
	    {
		print STDERR "      A Release Number Must be Specified\n";
	    }
            if (not defined $level)
	    {
		print STDERR "      A Level Number Must be Specified\n";
	    }
            $errcnt++;
	}

        # Check for valid build operating system.
        elsif (not defined $os)
        {
            print STDERR "\n***** Error: Build Operating System Must be Specified\n";
	    $errcnt++;
        }

        # Check for a valid site.
	elsif (not -d "$root\\src\\build\\$site")
        {
            print STDERR "\n***** Error: Invalid Site ($site) Specified\n";
	    $errcnt++;
        }

        # Otherwise return with the appropriate arguments.
        else
        {
            print STDOUT "\nIBM AntiVirus Immune System Product Build\n\n";
            print STDOUT "Drive Letter = $drive\n";
	    print STDOUT "Build Root === $root\n";
	    print STDOUT "Build OS ===== $os\n";
	    print STDOUT "Build Site === $site\n";
	    print STDOUT "Build Spin === $spin\n";
            print STDOUT "Build Type === $builddescriptions{$build}\n";
	    print STDOUT "Script Debug = ", ($debug ? "On" : "Off"), "\n";
	    print STDOUT "Languages ==== @lang\n";

            return ($drive, $root, $spin, $os, $build, $site, $debug, \@lang,
                    (defined $bldenv{TYPE} ? \%bldenv : undef));
	
        }
    }

    # Errors have occurred, print usage.
    print "\nIBM AV Build Script for Building the Product\n\n";
    print "Arguments:\n";
    print "   BuildOS        -buildos w32          Required\n";
    print "   DriveLetter    -drive d              Required\n";
    print "   Level          -level 056            Required\n";
    print "   Release        -release 300          Required\n\n";
    print "   BuildsToDo     -develop -product     Optional\n";
    print "   BuildSite      -site haw             Optional\n";
    print "   BuildSpin      -spin 1               Optional\n\n";
    print "   Languages      -lang ea -lang ja     Optional\n";
    print "   All Languages  -lang all             Optional\n\n";
    print "Option names may be abbreviated to uniqueness.\n";
    exit 0;
}


# &validate_arguments;
# exit 0;

1;
