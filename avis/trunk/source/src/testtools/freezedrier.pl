#!/usr/bin/perl -w

=head1 NAME

freezedrier.pl -- Create NAV Central Quarantine 'freezedried' samples

=head1 STATEMENT

(c) Copyright IBM Corp. 2000

U.S. Government Users Restricted Rights - use,     
duplication or disclosure restricted by GSA ADP    
Schedule Contract with IBM Corp.                   

=head1 AUTHOR

Ian Whalley, E<lt>inw@us.ibm.comE<gt>, 2000-04-03

=head1 REQUIREMENTS

Any recent Perl for Win32.

=head1 SYNOPSIS

B<freezedrier.pl> [--quarantine=E<lt>dirE<gt>]
                  [--reason=E<lt>strE<gt>]
                  [--infected=E<lt>yes<gt>|E<lt>noE<gt>]
                  E<lt>list of files and directoriesE<gt>


=head1 DESCRIPTION

B<freezedrier.pl> allows the user to take a set of files
(or a directory hierarchy) and create central quarantine
('freezedried') sample files.  These samples can be
(indeed, by default, they are) dropped directly
into the central quarantine directory on a working
Quarantine Server machine, whereupon IcePack, QConsole,
et al will treat them as if they are naturally
captured files.

By default, the freezedried samples created are
dropped into c:\quarantine.  This path can be
overridden by use of the B<--quarantine> option.

The B<--infected> option is used either as
B<--infected=yes> or B<--infected=no>.  If it is omitted, or
is set to an invalid value, 'yes' is assumed.  This
option affects the X-Scan... attributes written into the
newly-freezedried sample, and can be useful to test false-positive
cases, or cases where infected files have been captured
manually.

=head1 BUGS

The X-Date family of attributes are 'hardwired' to a given date.
We need to work out how to write our own dates into these
attributes -- the problem stems from the fact that I don't
understand how dates are written to freezedried files.

The X-Platform-GUID and X-Sample-UUID attributes are also
hardwired.  I cannot work out where SYMC get these numbers
from, so I have just wired them up.  This probably means
that (without fixing this :-)), you cannot use
B<freezedrier.pl> to create samples and expect the
definitions that come back from the Immune System to
make it to the client.  I believe that DefCast needs the
platform GUID to work correctly.

=cut

# -----

use strict;

# -----

use IO::Handle;
use Getopt::Long;
use MD5;
use Fcntl;
use Socket;
use Win32;
use Win32::Registry;

# -----

# This is the default path to the quarantine directory.  This will
# be used if the user does not specify a --quarantine=<dir> option.
my $defaultQpath  = "C:\\Program Files\\Symantec\\Quarantine\\Server\\Submissions";

# This is the default sample submission reason.  It can be
# overridden if the user specifies --reason=<string>.
my $defaultReason = "automatic";

# This is the default information that will be used if the file is
# allegedly 'infected'.
my $infectVirusName = "inw's Really Deadly Virus";
my $infectVirusID   = 57757;
my $infectSigSeq    = 0;
my $infectSigVer    = "12345678.901";

# And this is the default information that will be used if the file is
# allegedly 'clean'.
my $cleanVirusName = "";
my $cleanVirusID   = 0;
my $cleanSigSeq    = 0;
my $cleanSigVer    = "12345678.901";

# The attributes in this list are added to the sample immediately before
# the freezedried version is written to the file.  They will override
# any other attributes that have been created up to that point, if names
# clash.  Note that dates are represented as buffers -- I can't work
# out how these values are translated into the on-disk version of
# the attributes.  They don't matter anyway, so I have hard-coded them.
my $useTheseAttributes =
{
  "X-Sample-Changes"            => 1,                                        # MANDATORY.
  "X-Sample-Status"             => 0,                                        # MANDATORY.
  "X-Sample-Type"               => "file",                                   # MANDATORY.
  "X-Sample-Priority"           => 500,                                      # MANDATORY.
  "X-Sample-UUID"               => "{C2E4C331-0989-11D4-BF3C-0004ACECC57A}", # MANDATORY.
  "X-Scan-Result"               => "completed",                              # MANDATORY.
  "X-Date-Accessed"             => "\x06\x13\xD0\07\x0E\x0F\x2E",            # MANDATORY.
  "X-Date-Modified"             => "\x06\x13\xD0\07\x0E\x0F\x2E",            # MANDATORY.
  "X-Date-Created"              => "\x06\x13\xD0\07\x0E\x0F\x2E",            # MANDATORY.
  "X-Date-Quarantined"          => "\x06\x13\xD0\07\x0E\x0F\x2E",            # MANDATORY.
  "X-Platform-GUID"             => "{15BA8700-05A2-11D4-BF37-0004ACECC57A}", # MANDATORY.
  "X-Platform-Scanner"          => "freezedrier.pl",                         # MANDATORY.
};

# -----

# This is the magic number that appears at the start of
# freezedried files.  Don't change it :-).
my $magicNumber = "\x49\x46\x53\x51\x01\x00\x00\x00";

# Freezedried samples have one byte per attribute which I
# do not understand.  In examples from NAV CE 7.something
# for WinNT, it alternates between two numbers.  These
# two numbers are not always the same.  Therefore, I chose
# my own string, which helps serve as an additional
# freezedrier identifier.
my $mysteriousRotatingSequence = "inw";
my $posInMRS = 0;

# This hash should list a type of every attribute we will
# write to the freezedried file.  Any attributes for
# which a type mapping is not found will not be written
# to the file (but the user will get a warning).
my $nameToTypeMapping =
{
  "X-Checksum-Method"           => "string",
  "X-Date-Accessed"             => "date",
  "X-Date-Modified"             => "date",
  "X-Date-Created"              => "date",
  "X-Date-Quarantined"          => "date",
  "X-Platform-Address"          => "string",
  "X-Platform-Computer"         => "string",
  "X-Platform-Domain"           => "string",
  "X-Platform-GUID"             => "string",
  "X-Platform-Host"             => "string",
  "X-Platform-Language"         => "string",
  "X-Platform-Owner"            => "string",
  "X-Platform-Processor"        => "string",
  "X-Platform-Scanner"          => "string",
  "X-Platform-System"           => "string",
  "X-Platform-User"             => "string",
  "X-Sample-Changes"            => "numeric",
  "X-Sample-Checksum"           => "string",
  "X-Sample-Extension",         => "string",
  "X-Sample-File"               => "string",
  "X-Sample-FileID"             => "numeric",
  "X-Sample-Priority"           => "numeric",
  "X-Sample-Reason"             => "string",
  "X-Sample-Size"               => "numeric",
  "X-Sample-Status"             => "numeric",
  "X-Sample-Type"               => "string",
  "X-Sample-UUID"               => "string",
  "X-Scan-Result"               => "string",
  "X-Scan-Signatures-Sequence"  => "numeric",
  "X-Scan-Signatures-Version"   => "string",
  "X-Scan-Virus-Identifier"     => "numeric",
  "X-Scan-Virus-Name"           => "string",
};

# This hash maps the string type (in the list above)
# to the numeric type that must be written to the file.
my $typeToNumMapping =
{
  "string"  => 1,
  "numeric" => 2,
  "date"    => 3,
};

# -----

my %platformAttributes;
my %options;

# -----

#
# Allows the caller to retrieve a section of the registry in hash format.
#
# getRegistryHive() is a cleaned-up version of sampler.pl's registry().
#
sub getRegistryHive($$)
{
  my ($hive, $path) = @_;
  my %keys          = ();
  my %values        = ();
  my $object;
  my $registry;

  ($hive eq "MACHINE") && ($registry = \$main::HKEY_LOCAL_MACHINE);
  ($hive eq "USER")    && ($registry = \$main::HKEY_CURRENT_USER);

  ($$registry)->Open($path,$object);
  $object->GetValues(\%keys);
  $object->Close;

  # Create a hash containing the keys and values read from the
  # specified registry cell, and return it.

  foreach (keys %keys) 
  { 
    my $keyRef = $keys{$_};
    $values{$$keyRef[0]} = $$keyRef[2]; 
  }

  return \%values;
}

# -----

#
# Allows the caller to get a reference to a populated hash
# of X-Platform attributes.
#
# getPlatformAttributes() is a cleaned-up version of sampler.pl's platform().
# Actually, it has been changed a little to make the attributes look as close
# as possible to those created by NAV CE 7.x for WinNT.  This explains
# the weird double spaces and so on and so forth.
#
sub getPlatformAttributes()
{
  # If we already got the information, use that.
  (defined %platformAttributes) && (return \%platformAttributes);

  # Otherwise, clear the global hash and the populate it.
  %platformAttributes = ();

  # Hardware information.
  if (Win32::IsWin95)
  {
    my $hw = getRegistryHive("MACHINE","HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0");

	  $platformAttributes{'X-Platform-Processor'} = "$hw->{'VendorIdentifier'} $hw->{'Identifier'}";
  }
  elsif (Win32::IsWinNT)
  {
	  my $hw = getRegistryHive("MACHINE","HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0");

	  $platformAttributes{'X-Platform-Processor'} = "$hw->{'VendorIdentifier'} $hw->{'Identifier'}";
  }

  # Operating system information.
  if (Win32::IsWin95)
  {
	  my $sw = getRegistryHive("MACHINE","SOFTWARE\\Microsoft\\Windows\\CurrentVersion");

	  $platformAttributes{'X-Platform-System'} = "$sw->{'Version'}  Build $sw->{'VersionNumber'}";
	  $platformAttributes{'X-Platform-Owner'} = "$sw->{'RegisteredOwner'}  $sw->{'RegisteredOrganization'}"; 
  }
  elsif (Win32::IsWinNT)
  {
    my $sw = getRegistryHive("MACHINE","SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion");

    $platformAttributes{'X-Platform-System'} = "Windows NT  $sw->{'CurrentVersion'} Build $sw->{'CurrentBuildNumber'}";
    $platformAttributes{'X-Platform-Owner'} = "$sw->{'RegisteredOwner'}  $sw->{'RegisteredOrganization'}"; 
  }

  # Language information.
  if (Win32::IsWin95)
  {
    my $nls = getRegistryHive("MACHINE","SYSTEM\\CurrentControlSet\\Control\\NLS\\Locale");
    my $code = $nls->{""};

    ($code) && ($platformAttributes{'X-Platform-Language'} = $nls->{$code});
  }
  elsif (Win32::IsWinNT)
  {
    my $nls = getRegistryHive("USER","Control Panel\\International");

    $platformAttributes{'X-Platform-Language'} = "$nls->{'sLanguage'} ($nls->{'sCountry'})";
  }
    
  # Windows networking information.
  if (Win32::IsWin95)
  {
    my $nw = getRegistryHive("MACHINE","Network\\Logon");
    my $nb = getRegistryHive("MACHINE","SYSTEM\\CurrentControlSet\\Services\\VxD\\VNETSUP");

    ($nb->{'ComputerName'}) && ($platformAttributes{'X-Platform-Computer'} = $nb->{'ComputerName'});
    ($nb->{'Workgroup'})    && ($platformAttributes{'X-Platform-Domain'}   = $nb->{'Workgroup'});
    ($nw->{'username'})     && ($platformAttributes{'X-Platform-User'}     = $nw->{'username'});
  }
  elsif (Win32::IsWinNT)
  {
    my $wl = getRegistryHive("MACHINE","SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon");
    my $cn = getRegistryHive("MACHINE","SYSTEM\\CurrentControlSet\\Control\\ComputerName\\ComputerName");

    ($cn->{'ComputerName'})      && ($platformAttributes{'X-Platform-Computer'} = $cn->{'ComputerName'});
    ($wl->{'DefaultDomainName'}) && ($platformAttributes{'X-Platform-Domain'}   = $wl->{'DefaultDomainName'});
    ($wl->{'DefaultUserName'})   && ($platformAttributes{'X-Platform-User'}     = $wl->{'DefaultUserName'});
  }

  # IP information.
  my ($hostname, $aliases, $addrtype, $length, @addrs) = gethostbyname("");
  if (scalar @addrs)
  {
    $platformAttributes{'X-Platform-Host'}    = "$hostname";
    $platformAttributes{'X-Platform-Address'} = join(" ", map(inet_ntoa($_), @addrs));
  }

  # JD.
  return \%platformAttributes;
}

# -----

#
# Retrieves the MD5 CRC for the file's content (which has
# already been read in) and returns it in Immune System-complient
# format.
#
sub getMD5($)
{
  my ($fileContent) = @_;

  return lc(MD5->hexhash($fileContent));
}

# -----

#
# Originally, this function loaded the default attributes from a file.
# I decided this was silly, and now they are embedded in the script.
# So, this function doesn't do much any more.
#
sub getDefaultAttributes()
{
  # JD.
  return $useTheseAttributes;
}

# -----

#
# This function combines the default attributes (from the configuration
# file) with the attributes already constructed for the sample, and
# returns the combined hash ref.
#
sub addDefaultAttrs($)
{
  my ($attrs) = @_;
  my $newAttrs;
  my $key;

  # Sanity check.
  (! defined $attrs) && return undef;

  # Get the default attributes from the configuration file.
  $newAttrs = getDefaultAttributes();

  # Loop across the default attributes, copying into this sample's
  # attributes.
  foreach $key (keys %{$newAttrs})
  {
    $attrs->{$key} = $newAttrs->{$key};
  }

  # JD.
  return $attrs;
}

# -----

#
# Allows the caller to retrieve the name of, and an open
# handle to, a quarantine file.
#
sub getNameInQuarantine()
{
  my $qDir = $options{'quarantine'};
  my $fh;
  my $name;
  my $i;
  my $gotOne;
  
  # Sanity check.
  (! defined $qDir) && return (undef, undef);

  # Flip the slashes to Unix-style slashes.
  $qDir =~ s#\\#/#g;

  # Get a filehandle.
  $fh = IO::Handle->new();
  (! defined $fh) && return (undef, undef);

  # Loop for a lot of times :-).
  $gotOne = 0;
  for ($i = 0; $i < 100; $i++)
  {
    $name = getLikelyLookingQuarantineName();
    $name = $qDir . "/" . $name;

    # Attempt to open the file (it must not already exist).
    if (sysopen($fh, $name, O_WRONLY | O_EXCL | O_CREAT))
    {
      # Pop it into binary mode before we forget.
      binmode($fh);
      $gotOne = 1;
      last;
    }
  }

  # If we got a file, then return the information.
  ($gotOne) && return ($name,$fh);

  # Otherwise, return failure.
  return (undef,undef);
}

# -----

#
# Constructs the filename component of a quarantine name, in SYMC
# normal form.  Returns this filename as a string.  It does
# not include the directory name.
#
sub getLikelyLookingQuarantineName()
{
  my $name;
  my $i;
  my @chars;

  # Loop eight times (the names are eight characters long),
  # picking a number from 0 to 15 (0 to F) each time, and
  # constructing an array of the numbers.
  for ($i = 0; $i < 8; $i++)
  {
    push @chars, int(rand 15);
  }

  # Finally, emit the numbers in the correct format into a string,
  # and return it.
  return sprintf("%X%X%X%X%X%X%X%X", @chars);
}

# -----

#
# This function takes the name of a quarantine sample, and
# returns the decimal version of the filename component.
# For some reason, this is the value of an attribute that
# NAV CE puts into the freezedried sample, and we have to
# emulate this odd behaviour.
#
sub convertToDecimal($)
{
  my ($hexString) = @_;
  my $num;

  # Sanity check.
  (! defined $hexString) && (return undef);

  # [We know the slashes are already in the Unix style.]

  # Remove everything up to (and including) the last slash.
  $hexString =~ s#^.*/##;

  # And return the decimal version of the number.
  return hex($hexString);
}

# -----

#
# This function is the main writer.  Everything comes
# through here for scrambling and writing to the file.
# It returns the number of bytes written.
#
sub scrambleAndWriteBuffer($$$)
{
  my ($fh, $buffPtr, $len) = @_;
  my $lenWritten           = 0;
  my $buffer               = "";
  my $char;

  # Loop across the input buffer, one character at a time.
  while ($lenWritten != $len)
  {
    # Pull out the appropriat character.
    $char = substr($$buffPtr, $lenWritten, 1);

    # Ph33r 0ur crypt0 pr0w3ss.
    $buffer .= chr((ord($char)) ^ 0xFF);

    $lenWritten++;
  }

  # Finished scrambling.  Write the new buffer out.
  return syswrite($fh, $buffer, $lenWritten);
}

# -----

#
# This function writes numbers (which are unsigned 32-bit
# quantities, in our case) to the freezedried file.
# It returns 0 on failure, !0 on success.
#
sub scrambleAndWriteU32($$)
{
  my ($fh, $num) = @_;
  my $numStr;

  # Sanity check.
  ((! defined $fh) || (! defined $num)) && return 0;

  $numStr = pack('L', $num);

  return scrambleAndWriteBuffer($fh, \$numStr, 4);
}

# -----

#
# This is a big function, which handles writing the attributes
# to the freezedried file.  The file pointer must be positioned
# immediately after the copy of the source file.
# Returns the number of attributes successfully written to
# the file.  If it fails catastrophically, it returns 0.
# In that case, the caller should delete the quarantined file,
# as it cannot be assumed to be valid.
#
sub scrambleAndWriteAttributes($$)
{
  my ($fh, $attrs) = @_;
  my $key;
  my $str;
  my $oddByte;
  my $num;
  my $attrType;
  my $numAttrType;
  my $attrLen;

  # Sanity check.
  ((! defined $fh) || (! defined $attrs)) && return 0;

  ATTRLOOP: foreach $key (sort keys %{$attrs})
  {
    # Get the type of the attribute (in string form).
    # If this type is not defined, or is zero-length,
    # then skip this attribute as we don't know how
    # to write it.
    $attrType = $nameToTypeMapping->{$key};
    if ((! defined $attrType) || ($attrType eq ""))
    {
      print "WARNING: I don't know what type '$key' is -- skipping [attribute].\n";
      next ATTRLOOP;
    }

    # Get the type of the attribute (in numeric form).
    $numAttrType = $typeToNumMapping->{$attrType};

    # If this is a string (or date) type attribute, then it must
    # not be zero-length.  If it is, just skip this attribute.
    if ((($attrType eq "string") || ($attrType eq "date")) && ($attrs->{$key} eq ""))
    {
      next ATTRLOOP;
    }

    # Produce a version of the key that is the correct length.
    $str = pack('a64', $key);

    # Get the weird byte.
    $oddByte = substr($mysteriousRotatingSequence, $posInMRS, 1);
    $posInMRS++;
    ($posInMRS >= length($mysteriousRotatingSequence)) && ($posInMRS = 0);

    # Add the weird byte onto the end of the string.
    $str .= $oddByte;

    # Emit the key name and the weird byte.
    (scrambleAndWriteBuffer($fh, \$str, length($str)) != length($str)) && return 0;

    if (($attrType eq "string") || ($attrType eq "date"))
    {
      $attrLen = length($attrs->{$key});
    }
    elsif ($attrType eq "numeric")
    {
      $attrLen = 4;
    }

    # Write out attribute length.
    (scrambleAndWriteU32($fh, $attrLen) == 0) && return 0;

    # Write out attribute type.
    (scrambleAndWriteU32($fh, $numAttrType) == 0) && return 0;

    if (($attrType eq "string") || ($attrType eq "date"))
    {
      my $attrVal = $attrs->{$key};

      (scrambleAndWriteBuffer($fh, \$attrVal, length($attrVal)) != length($attrVal)) && return 0;
    }
    elsif ($attrType eq "numeric")
    {
      (scrambleAndWriteU32($fh, $attrs->{$key}) == 0) && return 0;
    }

    $num++;
  }

  # JD.
  return $num;
}

# -----

#
# This actually writes out the freezedried sample.
#
sub emitFreezeDriedSample($$$$)
{
  my ($fh, $content, $length, $attrs) = @_;
  my $attrNumPos;
  my $attrNum;

  # Sanity check.
  ((! defined $fh) || (! defined $content) || (! defined $length) || (! defined $attrs)) && return 0;

  # All ready to go.

  # First the magic number.
  (scrambleAndWriteBuffer($fh, \$magicNumber, length($magicNumber)) != length($magicNumber)) && return 0;

  # Now the length.
  (scrambleAndWriteU32($fh, $length) == 0) && return 0;

  # Remember where we are.
  $attrNumPos = tell($fh);

  # Now the number of attributes (this we write as zero, and fill in later).
  (scrambleAndWriteU32($fh, 0) == 0) && return 0;

  # The content.
  (scrambleAndWriteBuffer($fh, $content, $length) != $length) && return 0;

  # The attributes.
  $attrNum = scrambleAndWriteAttributes($fh, $attrs);

  ($attrNum == 0) && return 0;

  # Now go back to the position where we have to write the number of
  # attributes, and write it.
  (seek($fh, $attrNumPos, 0) == 0) && return 0;
  (scrambleAndWriteU32($fh, $attrNum) == 0) && return 0;

  # JD.
  return 1;
}

# -----

#
# Called when either we don't know what $item is, or when we
# know it's a directory.  Does all that funky recursion stuff,
# and calls processOneFile() to do the actual work.
#
sub processOneItem($)
{
  my ($item) = @_;
  my $retMe  = 1;
  
  # Switch '\' with '/' (the one true slash).
  $item =~ s#\\#/#g;
  # Remove trailing '/'s.
  $item =~ s#/+$##;
  # Remove trailing whitespace (probably not necessary).
  $item =~ s/[\r\n\s]+$//;

  # Is it a file?
  if (-f $item)
  {
    (-r $item) && (return processOneFile($item));
    print "'$item' is not readable -- crisis!\n";
    return 0;
  }

  # Is it a directory?
  if (-d $item)
  {
    my $dh;
    my @gulp;

    # Get a new handle.
    $dh = IO::Handle->new();
    if (! defined $dh)
    {
      print "Couldn't get filehandle, you loose.\n";
      $retMe = 0;
    }
    else # Got the handle.
    {
      # Crack open the directory listing.
      if (! opendir($dh, $item))
      {
        print "Can't open directory '$item' -- skipping [directory].\n";
        $retMe = 0;
      }
      else # Opened the directory.
      {
        # Swallow everything, and close it again.
        @gulp = readdir($dh);
        closedir($dh);

        # Walk the things in the directory, working on the files.
        foreach (grep { ! /^(\.|\.\.)$/ } @gulp)
        {
          # Process the item.  If the return code is zero, then
          # save it for our return code.  Thus we return zero if
          # one more things failed.
          (processOneItem($item . "/" . $_) == 0) && ($retMe = 0);
        }
      }
    }

    # JD.
    return $retMe;
  }

  # We don't know what it is.
  print "'$item' is not a file or a directory -- crisis!\n";
  return 0;
}

# -----

#
# Called when we already know that the item is a file,
# and appears to exist.  Does all the work to create
# a freezedried sample from file.
#
sub processOneFile($)
{
  my ($item) = @_;
  my $fh;
  my $fileSize;
  my $fileContent;
  my $lenRead;
  my $attrs;
  my $nameInQuarantine;
  my $tmpStr;
  my $retVal;

  # Attempt to get the file size.
  $fileSize = (stat($item))[7];
  if (! defined $fileSize)
  {
    print "WARNING: Unable to determine size of '$item' -- skipping [file]\n";
    return 0;
  }

  # Get a new filehandle.
  $fh = IO::Handle->new();

  # Attempt to open the file.
  if (! open($fh, $item))
  {
    print "WARNING: Unable to open '$item' -- skipping [file]\n";
    return 0;
  }

  # Go into binary mode.
  binmode($fh);

  # Swallow the file.
  $lenRead = read($fh, $fileContent, $fileSize);
  if (($lenRead != $fileSize) || (length($fileContent) != $fileSize))
  {
    print "Read $lenRead bytes -- expected $fileSize\n";
    close($fh);
    return 0;
  }
  close($fh);

  # Got the file.  Start to think about attributes.

  # Create the attributes has by getting the platform attributes.
  $attrs = getPlatformAttributes();

  # Now the checksum attributes.
  $attrs->{'X-Checksum-Method'}  = "md5";
  $attrs->{'X-Sample-Checksum'}  = getMD5($fileContent);

  # Miscellaneous sample attributes.

  # Flip the Unix slashes back to Windows slashes.
  $item =~ s/\//\\/g;
  $attrs->{'X-Sample-File'}      = $item;
  $attrs->{'X-Sample-Size'}      = $fileSize;

  # Get the extension.
  $tmpStr = $item;

  # Extract the extension.  Only add this to the attributes hash
  # if the file does, indeed, have an extension.
  ($tmpStr =~ /^.*\.([^\.\\]+)$/) && ($attrs->{'X-Sample-Extension'} = $1);

  # Fold in the default attributes.
  $attrs = addDefaultAttrs($attrs);

  # Attempt to get the name of, and open, a file in quarantine.
  # (this is done atomically).
  ($nameInQuarantine, $fh) = getNameInQuarantine();

  # Did we get a name?  If not, we have to give up.  This shouldn't
  # ever happen...
  if (! defined $nameInQuarantine)
  {
    print "Unable to create a uniquely-named quarantine file\n";
    return 0;
  }

  # Get the FileID attribute.
  $attrs->{'X-Sample-FileID'} = convertToDecimal($nameInQuarantine);

  # Fill in the reason attribute.
  $attrs->{'X-Sample-Reason'} = $options{'reason'};

  # If the user said that this sample should be regarded
  # as 'infected', then fill in some made-up virus information.
  # Otherwise, fill in some made-up clean information.
  if ($options{'infected'})
  {
    $attrs->{'X-Scan-Signatures-Sequence'} = $infectSigSeq;
    $attrs->{'X-Scan-Signatures-Version'}  = $infectSigVer;
    $attrs->{'X-Scan-Virus-Identifier'}    = $infectVirusID;
    $attrs->{'X-Scan-Virus-Name'}          = $infectVirusName;
  }
  else
  {
    $attrs->{'X-Scan-Signatures-Sequence'} = $cleanSigSeq;
    $attrs->{'X-Scan-Signatures-Version'}  = $cleanSigVer;
    $attrs->{'X-Scan-Virus-Identifier'}    = $cleanVirusID;
    $attrs->{'X-Scan-Virus-Name'}          = $cleanVirusName;
  }

  # Phew.  Attributes are ready to go.

  # Go go go.
  $retVal = emitFreezeDriedSample($fh, \$fileContent, $fileSize, $attrs);

  # Close the file as soon as we are done.
  close($fh);

  # Print a nice message telling the user what happened.  If it went
  # wrong, delete the abortive freezedried file.
  if ($retVal)
  {
    my $printName = $nameInQuarantine;

    $printName =~ s/^.*[\\\/]([^\\\/]+)$/$1/g;
    print "$printName <- $item\n";
  }
  else
  {
    print "WARNING: Failed to correctly write freezedried copy of '$item'.\n";
    unlink($nameInQuarantine);
  }

  # JD.
  return $retVal;;
}

# -----

#
# Prints a banner.
#
sub helloItsMe()
{
  print <<EOM;
freezedrier.pl -- Create NAV Central Quarantine 'freezedried' samples
Copyright (c) IBM Corp. 2000
Ian Whalley <inw\@us.ibm.com>

EOM

  # JD.
  return 1;
}

# -----

sub showUsage()
{
  print <<EOU;
USAGE: freezedrier.pl [options] <file|dir> [<file|dir> ...]

Available options:
  --quarantine=<dir> : Emit freezedried files into <dir>.  The script
                       will not create this directory.  The default is:
      $defaultQpath
  --reason=<string>  : Value for X-Sample-Reason attribute.  Defaults
                       to '$defaultReason'.  The only value that currently
                       means anything is 'manual'.
  --infected=yes|no  : Should the sample appear to have been found to be
                       infected by the client?  Defaults to 'yes'.
EOU

  # JD.
  return 1;
}

# ----- START HERE -----

my $retMe         = 0;
my $infectedCheat = 1;

helloItsMe();

(! GetOptions(\%options,
              "reason=s",
              "infected=s",
              "quarantine=s")) && (print "\n") && exit showUsage();

# If they didn't give a reason, use the default.
(! defined $options{'reason'}) && ($options{'reason'} = "automatic");

# If they didn't give a quarantine path, use the default.
(! defined $options{'quarantine'}) && ($options{'quarantine'} = $defaultQpath);

# If they didn't give a --infected directive, then assume that they want
# the file to be treated as if it's infected.  This is also the case if
# they say 'yes'.  If they say 'no', then the sample is not infected.
# If they say anything else, fall to the default of 'yes'.
if ((! defined $options{'infected'}) || ($options{'infected'} =~ /^yes$/i))
{
  $infectedCheat = 1;
}
elsif ($options{'infected'} =~ /^no$/i)
{
  $infectedCheat = 0;
}

# Now we know...
$options{'infected'} = $infectedCheat;

# Check that the quarantine directory appears valid.
if (! -d $options{'quarantine'})
{
  showUsage();
  print "ERROR: '$options{'quarantine'}' must exist and must be a directory.\n";
  exit 1;
}

print "[Using '$options{'quarantine'}' as the quarantine directory.]\n\n";

# Did we get any path-type arguments?
((! defined @ARGV) || (scalar(@ARGV) == 0)) && exit showUsage();

# Spin the randometer.
srand(time());

# We got some arguments, so process them.
foreach (@ARGV)
{
  # Attempt to process one argument.  If it fails, change the exit code.
  (processOneItem($_) == 0) && ($retMe = 1);
}

# JD.
exit $retMe;
