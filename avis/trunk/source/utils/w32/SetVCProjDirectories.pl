#
#
#	SetVCProjDirectories.pl
#
#	Author:		Andy Klapper
#	Date:		10/20/98
#
#	This perl file changes the value of the Output_Dir and 
#	Intermediate_Dir values in a microsoft project file (xxx.dsp).
#
#	Arguments:
#		projectName[.dsp]
#		configurationName
#		Output_Dir
#		Intermediate_Dir
#
#	Required Environment Variables:
#		TMP
#		SRCD
#		DIRNAME
#
#	Example:
#		SetVCProjDirectories.pl AVISdb "Win32 Debug" g:\bogus\dout\ea g:\bogus\dobj\ea
#
#
#	Return codes;
#		0	everything went okey-dokey
#		-1	invalid number of arguements
#		-2	project file does not exist or is not writeable
#		-3	invalid output directory
#		-4	invalid intermediate directory
#		-5	unable to find requested configuration in the project file
#

$usage = "SetVCProjDirectories.pl projectName[.dsp] configurationName outputDir intermediateDir";


#	Get arguments

if (3 != $#ARGV)
{
	print "Invalid number of arguments\n";
	print "$usage";

	exit -1;
}

($projectName, $configName, $outDir, $intermediateDir) = @ARGV;

#	Verify arguments

if ($projectName =~ m/\.dsp/)
{
	$projectFile = $projectName;
	$projectName =~ s/\.dsp//;
}
else
{
	$projectFile = $projectName . ".dsp";
}

#if (not -w $projectFile)
#{
#	print STDOUT "Project file \"$projectFile\" is not writable";
#	exit -2;
#}

$fullConfigName = $projectName . " - $configName";

if (not -d $outDir)
{
	print STDOUT "Invalid output directory \"$outDir\"";
	exit -3;
}

if (not -d $intermediateDir)
{
	print STDOUT "Invalid intermediate directory \"$intermediateDir\"";
	exit -4
}

#$sourceDir=$ENV{"SRCD"} . "\\" . $ENV{"DIRNAME"};

open PROJFILE, "<$projectFile";

@lines = <PROJFILE>;

close PROJFILE;

$changed			= "false";
$inCorrectConfig	= "false";
$foundConfig		= "false";
$targetExtension	= "";

# find the configuration

foreach (@lines)
{
		# looking for !IF "$(CFG)" == "
		# or !ELSEIF "$(CFG)" == "
	if (m/!(IF|ELSEIF)\s+\"\$\(CFG\)\"\s+==\s+\"/)
	{
		# this is the correct configuration
		if (m/\"$fullConfigName\"/)
		{
			$inCorrectConfig	= "true";
			$foundConfig		= "true";
		}
		else
		{
			$inCorrectConfig	= "false";
		}
	}
	elsif (m/#\s+Begin Target/)
	{
		$inCorrectConfig = "false";
	}
	elsif (m/\(based on \"Win32 \(x86\)/)		# determine the target extension
	{
		$targetExtension = ".dll" if (m/Dynamic-Link Library/);
		$targetExtension = ".exe" if (m/Application/);
		$targetExtension = ".lib" if (m/Static Library/);
	}
	elsif ("true" eq $inCorrectConfig)
	{
		if (m/#\s+PROP( BASE)?\s+Output_Dir/)	# is this the Output_Dir setting?
		{
			($currentDir = $_) =~ s/#\s+PROP BASE\s+Output_Dir\s+\"([^\"]*)\"\s*/$1/;
			if ($currentDir ne $outDir)
			{
				$changed = "true";
				s/\"[^"]*\"/\"$outDir\"/;
			}
		}
		elsif (m/#\s+PROP( BASE)?\s+Intermediate_Dir/)	# the Intermediate_Dir ?
		{
			($currentDir = $_) =~ s/#\s+PROP BASE\s+Intermediate_Dir\s+\"([^\"]*)\"\s*/$1/;
			if ($currentDir ne $intermediateDir)
			{
				$changed = "true";
				s/\"[^"]*\"/\"$intermediateDir\"/;
			}
		}
		elsif (m/\sADD LINK32\s/ || m/\sADD LIB32\s/)	# LINK32/LIB32 line 
		{
			$changed = "true";
			if (m/\s\/out:/)		# replace /out:"xxx/yyyy" with /out:"$outDir/yyyy"
			{
				s/\/out:\"([^\/\\]*)/\/out:\"$outDir/;
			}
			else					# add the /out:"outDir/yyy" arguement
			{
				s/\/machine:I386/\/machine:I386 \/out:\"$outDir\/$projectName$targetExtension\"/;
			}
		}
	}
#	elsif (m/SOURCE=\./)
#	{
#		s/SOURCE=\./SOURCE=$sourceDir/;
#	}
}


if ("false" eq $foundConfig)
{
	print STDOUT "Unable to find configuration \"$configName\" in project \"$projectName\"";
	exit -5;
}

#$tmp = $ENV{"TMP"};

#if (not -d $tmp)
#{
#	mkdir($tmp, 0777);
#}

$tmpProjectFile= $projectName . "-tmp.dsp";

#if ("true" eq $changed)
#{
	open PROJFILE, ">$tmpProjectFile";
	print PROJFILE @lines;
#}

