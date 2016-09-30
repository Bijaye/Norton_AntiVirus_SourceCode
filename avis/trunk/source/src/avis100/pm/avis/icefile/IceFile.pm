=head1 NAME

Package: AVIS::IceFile

Filename: pm/AVIS/IceFile/IceFile.pm

=head1 STATEMENT

Licensed Materials - Property of IBM

(c) Copyright IBM Corp. 1998-1999 

U.S. Government Users Restricted Rights - use,    
duplication or disclosure restricted by GSA ADP   
Schedule Contract with IBM Corp.                  

=head1 AUTHOR

Edward Pring, Morton Swimmer

=cut

package AVIS::IceFile;

# This package exports several public functions:

require Exporter;
$VERSION = 1.10;
@ISA = qw(Exporter);
@EXPORT = ();
@EXPORT_OK = qw(parse create uncreate clear names type copy wipe zip unzip token readhttp writehttp appendhttp movehttp capture uncapture md5digest getSTATUSTXT putSTATUSTXT);

# This program uses several public Perl packages, which must be installed 
# in the Perl library directory of the machine where this program executes:

use MD5;
use Win32::Mutex ();
use MIME::Base64 ();

# This package uses several private Perl packages, which must be installed
# in the current directory of the machine where this program executes:

use AVIS::IceBase ();

use strict 'vars';

# ...

my $eol = "\r\n";
my $nul = ">nul";

=head1 DESCRIPTION

Subroutines defined in this package:

=head2 md5digest

Calculate digest of the file submitted as a parameter.

=cut

sub md5digest ($) {
  my ($filename) = @_;

  my $md5 = MD5::new();
  my $md5dig;

  AVIS::IceBase::event("calculating digest of $filename\n");

  if (0) {
    my $program = AVIS::IceBase::config('md5Program');
    open MD5,"$program -b $filename|" or AVIS::IceBase::fatal("cannot run $program, $!\n");
    $md5dig = <MD5> =~ m/^(\w+)/;
    close MD5;
  } else {
    if (open(P, $filename)) {
      seek(P, 0, 0);
#      $md5->reset();
      $md5->addfile(P);
      $md5dig = $md5->hexdigest();
      close P;
    } else {
      AVIS::IceBase::fatal("cannot generate digest, $!\n");
    }
  }
  
  $md5dig = uc($md5dig);
  AVIS::IceBase::event("calculated digest $md5dig of $filename\n");
  return $md5dig;
}

=head2 parse

This function parses a filename into separate components and returns
a list containing the components.  All punctuation is retained in the
components and returned with them.

For example, if this filename is given:

   z:\pring\updates\av30ba.zip

then the list returned will be:
 
    index  component  example

    -----  ---------- -----------------

      0    server     

      1    share      

      2    drive      z:

      3    path       \pring\updates\

      4    name       av30ba

      5    extension  .zip
 
For another example, if this filename is given:

    \\avfiles\transfer\pring\updates\av30ba.zip

then the list returned will be:
 
    index  component  example

    -----  ---------- -----------------

      0    server     \\avfiles

      1    share      \transfer

      2    drive      

      3    path       \pring\updates\

      4    name       av30ba

      5    extension  .zip
 
Note that not all components are present in all filenames.  The
function always returns a list with six items in it; null values 
are returned for all missing components.

=cut

sub parse ($) {
  # The parameter for this function is a path.

  my ($filename) = @_;

  # Parse the path into components using a really ugly regular
  # expression.  Note that it is not valid to specify both "server name" 
  # and "drive letter", and it is valid to omit either or both.

  my ($server,$drive,$path,$name,$extension) = 
    $filename =~ m/^(\\\\[\w $_]*)?(\w:)?(.*\\)*([\w \$_\-\.]+?)(\.[\w\$_]*)??$/;

  # If "server name" was specified, then the first token in the path
  # is a "share name", not a directory name.

  my $share = "";
  ($share,$path) = $path =~ m/^(\\[\w $_]*)(.*)$/ if $server;

  # Reteurn a list of six items containing the components of the 
  # filename.  Null values are used for missing components.  Note
  # that all returned lists contain at least one null value, since
  # "server name" and "drive letter" are mutually exclusive.

  #print "<$filename> parsed into <$server> <$share> <$drive> <$path> <$name> <$extension>\n";
  return ($server,$share,$drive,$path,$name,$extension);
}

=head2 create

This function creates a directory if it does not already exist.
Please note that this function is recursive.

=cut

sub create ($) {
  # The parameter for this function is a directory path to be created,
  # if necessary.
    
  my ($directory) = @_;
  return 1 if $directory eq "";
  return 1 if -d "$directory";
  return 1 if -d "$directory\\";

  # If the specified directory does not already exist and includes
  # either a "drive letter" or "file server" component, then make sure
  # that component exists before trying to create the directory
  # ...........

  #??? my ($server,$share,$drive,$path,$name,$extension) = parse($directory);
  #??? if ($drive || $server ) {}

  my ($root,$stem) = $directory =~ m/(.*)\\(.+)$/;
  create($root) or return 0;

  # Try to create the last directory in the path, record our success or
  # failure, and return an indication of success or failure to our 
  # caller.

  mkdir $directory, \0777 or AVIS::IceBase::error("cannot create $directory, $!\n"), return 0;
  #AVIS::IceBase::event("created directory $directory\n");
  return 1;
}

=head2 uncreate

Remove all subdirectories, and this one.

The parameter for this function is a directory path to be uncreated,
if necessary.

=cut

sub uncreate ($) {
  my ($directory) = @_;
  return 1 if $directory eq "";
  return 1 unless -d "$directory\\";

  clear($directory) or return 0;

  rmdir $directory or AVIS::IceBase::error("cannot remove directory $directory, $!\n");
  return 1;    
}

=head2 clear

This function erases all files in a specified directory, and
recursively erases all files in all sub-directories.  All
sub-directories are removed, but the specified directory itself is
retained.  Conversely, if the specified directory doesn t exist yet,
it is created.

=cut

sub clear ($) {
  # The parameter for this function is a directory path to be
  # cleared of all files and sub-directories.

  my ($directory) = @_;
  
  # Create the directory, if necessary.

  create($directory) or return 0;

  # Read the specified directory and save its names in an array.
    
  opendir(DIR, $directory) or AVIS::IceBase::fatal("cannot open $directory, $!\n");
  my(@names) = readdir(DIR);
  closedir(DIR);

  # Erase each file in this directory.  Recursively erase all 
  # files in each sub-directory, and then remove each sub-directory.

  my @list = (); 
  my $name;
  foreach $name (@names) { 
    next if $name eq "." or $name eq "..";
    my $path = $directory . "\\" . $name;
    #AVIS::IceBase::event("erasing $path\n");
    unlink $path or AVIS::IceBase::error("cannot remove file $path, $!\n") if -f $path;
    clear($path) if -d $path;
    rmdir $path or AVIS::IceBase::error("cannot remove directory $path, $!\n") if -d $path;
  }

  # Return a successful completion code.

  return 1;
}

=head2 names

Return a list of the names in a specified directory that match a
specified extension, (excluding those irritating "." and ".." entries.
Note that the names are not returned in any particular order.

=cut

sub names ($;$) {
  my ($directory,$extension) = @_;
  #print "names($directory,$extension) entered\n";

  my @names = ();
  opendir(DIR, $directory) or AVIS::IceBase::fatal("Can't open $directory, $!\n");
  my $name;
  while ($name = readdir DIR) { 
    next if $name eq "." || $name eq "..";
    #print "$directory contains $name\n";
    push @names, $1 if $name =~ /(.*)$extension$/i; 
  }
  closedir(DIR);

  #print "names($directory,$extension) returned <@names>\n";
  return @names;
}

=head2 type

This function determines the datatype of a file, based on key values
at well-known locations in the file s contents, if possible, or on
the extension of the filename, if necessary.

=cut

sub type ($) {
  # The parameter for this function is a filename to be typed.

  my ($filename) = @_;
  return "none" unless -f $filename;

  # Define constants for key values that appear at well-known offsets
  # in various types of files.

  my $documentCue = pack "H8", "D0CF11E0"; # at offset +0
  my $bootCue = pack "H4", "55AA"; # at offset +510
  my $programCue = "MZ"; # at offset +0
  my $programExtensions = "COM EXE DLL VXD";

  # Extract the extension portion of the path and file name.

  my ($extension) = $filename =~ /\.(\w+)$/;

  # ... read the first 512 bytes of the file ...
    
  my $content;
  open FILE,"<$filename" or AVIS::IceBase::fatal("cannot open $filename, $!\n");
  binmode FILE;
  my $size = (stat($filename))[7];
  my $length = read FILE, $content, $size or AVIS::IceBase::fatal("cannot read $filename, $!\n");
  close FILE;

  # Return the datatype of the file, based on the presence of key 
  # values in the file's contents or its extension.

  return "document" if substr($content,0,length($documentCue)) eq $documentCue;
  return "boot"     if substr($content,510,length($bootCue)) eq $bootCue && $size%512==0;
  return "program"  if substr($content,0,length($programCue)) eq $programCue && $programExtensions =~ m/$extension/i;
  return "unknown";
}

=head2 copy

copy

=cut

sub copy ($$) {
  my ($oldfilename,$newfilename) = @_;
  AVIS::IceBase::fatal("copy failed, cannot find $oldfilename\n") 
    unless -f $oldfilename;

  # ...

  return if $oldfilename eq $newfilename;
  return if (-f $newfilename
	     && (stat($oldfilename))[7] eq (stat($newfilename))[7]
	     && (stat($oldfilename))[9] eq (stat($newfilename))[9]
	    );

  unlink $newfilename if -f $newfilename;
  #print "copying $oldfilename to $newfilename\n";
  my $cc = system("copy $oldfilename $newfilename /V $nul");
  AVIS::IceBase::fatal("copy failed, cannot copy $oldfilename to $newfilename, cc=$cc, $!\n") if $cc;
}

=head2 _rename (NOT EXPORTED)

Front end to standard perl rename command.

=cut

sub _rename ($$) {
  my ($oldfilename,$newfilename) = @_;
  AVIS::IceBase::fatal("rename failed, cannot find $oldfilename\n") unless -f $oldfilename;
  
  # ...
  
  return if $oldfilename eq $newfilename;
  
  rename $oldfilename,$newfilename
    or AVIS::IceBase::fatal("rename failed, cannot rename $oldfilename to $newfilename, $!\n");
}

=head2 wipe

Front end to standard Perl s unlink. Deletes the file. The intension
was to add overwriting to the standard unlink.

=cut

sub wipe ($) {
  my ($filename) = @_;
  AVIS::IceBase::fatal("wipe failed, cannot find $filename\n") unless -f $filename;

  # ... rewrite file with binary zeroes, then erase it ...
  
  # ???
  unlink $filename;
} 

=head2 zip

zip

=cut

sub zip ($$) {
  my ($filename,$directory) = @_;
  my $program = AVIS::IceBase::config('zipProgram');

  AVIS::IceBase::fatal("zip failed, cannot find program $program\n") unless -x $program;
  AVIS::IceBase::fatal("zip failed, cannot find directory $directory\n") unless -d $directory;
  AVIS::IceBase::fatal("zip failed, duplicate file $filename\n") if -f $filename;

  my $flags = "-j -q";
  my $cc = system("$program $flags $filename $directory\\* $nul");
  AVIS::IceBase::fatal("zip failed, cannot zip $directory into $filename, cc=$cc, $!\n") if $cc;
}

=head2 unzip

unzip

=cut

sub unzip ($$) {
  my ($filename,$directory) = @_;
  my $program = AVIS::IceBase::config('unzipProgram');

  AVIS::IceBase::fatal("unzip failed, cannot find program $program\n") 
    unless -x $program;
  AVIS::IceBase::fatal("unzip failed, cannot find directory $directory\n") 
    unless -d $directory;
  AVIS::IceBase::fatal("unzip failed, cannot find file $filename\n") 
    unless -f $filename;

  my $flags = "-o -q";
  my $cc = system("$program $flags $filename -d $directory $nul");
  AVIS::IceBase::fatal("unzip failed, cannot unzip $filename into $directory, cc=$cc, $!\n") if $cc;
}

=head2 _open (NOT EXPORTED)

_open

=cut

sub _open ($$) {
  my ($filehandleRef,$filepath) = @_;

  my ($direction,$filename) = $filepath =~ /^([<>+]*)(.+)/;
  my ($read) = $direction =~ /(<)/;

  my $maximum = AVIS::IceBase::config('retryMaximum');
  my $retry;
  for ($retry=$maximum; $retry; sleep($maximum - --$retry)) {
    return 0 if $read && ! -f $filename;
    AVIS::IceBase::event("_open retrying, empty file $filename\n"), next 
      if -f $filename && (stat($filename))[7]==0;
    open $filehandleRef,$filepath and return 1;
    AVIS::IceBase::event("_open: read=$read, direction=$direction, filepath=$filepath\n");
    AVIS::IceBase::event("_open retrying, cannot open file $filename, $!\n");
  }

  return 0;
}

=head2 token

token

=cut

sub token ($;$) {
  my ($filename, $token) = @_;

  # ...

  my $name = lc("token $filename");
  my $mutex = Win32::Mutex->new(0,$token);
  AVIS::IceBase::fatal("token failed, cannot create mutex $filename\n") unless $mutex;
  my $rc = $mutex->wait(AVIS::IceBase::config('tokenTimeout')*1000);
  AVIS::IceBase::fatal("token failed, cannot claim mutex $filename, rc=$rc\n") if $rc==0;
  
  # ... 
  
  if ($token) {
    _open(\*FILE,">$filename")
      or AVIS::IceBase::fatal("token failed, cannot open $filename, $!\n");
    print FILE $token."\n" 
      or AVIS::IceBase::fatal("token failed, cannot write $filename, $!\n");
    close FILE;
  }

  # ...
  
  elsif (-f $filename) {
    my $content;
    _open(\*FILE,"<$filename")
      or AVIS::IceBase::fatal("token failed, cannot open $filename, $!\n");
    my $length = read FILE, $content, (stat($filename))[7]
      or AVIS::IceBase::fatal("token failed, cannot read $filename, $!\n");
    close FILE;
    ($token) = $content =~ m/(\S+)/;
  }

  # ...

  else {
    #AVIS::IceBase::error("token error, $filename does not exist\n");
  }

  # ...

  $mutex->release;
  undef $mutex;
  return $token;
}

=head2 readhttp

This function reads a file containing an HTTP message, and returns
its attributes in an associative array and its contents in a string.

=cut

sub readhttp ($$$) {
  # The parameters for this function are the path and name of a file,
  # and references to a string variable for the body and an 
  # associative array for the attributes.

  my ($filename,$contentRef,$attributesRef) = @_;
  return 0 unless -f $filename;

  # Try real hard to open the file. This may take a while, if 
  # another process has the file open, and it may fail, if another
  # process moves the file.  But if it succeeds, treat the file as
  # a binary file so that some helpful library routine doesn't 
  # alter the body in unhealthy ways.

  _open(\*FILE,"<$filename") or return 0;
  binmode FILE;

  # Read each line of text at the beginning of the file, parse it
  # as an HTTP header, and save the header name and value in the
  # referenced associative array.  Stop after reading the empty
  # line that separates the headers from the body.

  while (<FILE>) {
    last if $_ eq "\n" or $_ eq "\r\n";
    $$attributesRef{$1} = $2, next if /^(\S+): ([^\n\r\f]*)/;
    AVIS::IceBase::fatal("readhttp failed, cannot parse \"$_\" in $filename\n");
  }

  # Read the body at the end of the file and save it in the
  # referenced string variable, if its wanted.
    
  if ($contentRef) {
    my $length = read FILE, $$contentRef, (stat($filename))[7]
      or AVIS::IceBase::fatal("readhttp failed, cannot read $filename, $!\n");
  }

  # ...

  close FILE;
  return 1;
}

=head2 writehttp

This function writes a file containing an HTTP message consisting
of attributes taken from an associative array and contents taken 
from a string variable.

=cut

sub writehttp ($$%) {
  # The parameters for this function are the path and name of a file,
  # and references to a string variable with its contents and an 
  # associative array with its attributes.

  my ($filename,$contentRef,%attributes) = @_;
  #AVIS::IceBase::fatal("writehttp failed, existing file $filename\n") if -f $filename;

  # Try real hard to open the file. This may take a while, if 
  # another process has the file open, and it may fail, if another
  # process moves the file.  But if it succeeds, treat the file as
  # a binary file so that some helpful library routine doesn't 
  # alter the body in unhealthy ways.

  _open(\*FILE,">$filename") 
    or AVIS::IceBase::fatal("writehttp failed, cannot open $filename, $!\n");
  binmode FILE;

  # Write the attributes into the file. The variable referenced is
  # assumed to be an associative array, and each element in it
  # is formatted as an HTTP header.
  my $attribute;
  foreach $attribute (sort keys %attributes) { 
    print FILE "$attribute: $attributes{$attribute}$eol" 
      or AVIS::IceBase::fatal("writehttp failed, cannot write $filename, $!\n"); 
  }

  # Write the contents to the file, separated from the headers
  # by an empty line.

  print FILE "$eol" 
    or AVIS::IceBase::fatal("writehttp failed, cannot write $filename, $!\n");

  if ($contentRef) {
    print FILE $$contentRef 
      or AVIS::IceBase::fatal("writehttp failed, cannot write $filename, $!\n");
  }
    
  # ...

  close FILE;
  return;
}

=head2 appendhttp

appendhttp

=cut

sub appendhttp ($%) {
  # The parameters for this function are the path and name of a file,
  # and an associative array of attributes to append to the file.

  my ($filename,%newAttributes) = @_;

  # The file the new attributes are to be appended to may not exist
  # yet.  This degenerate case is handled by the writehttp() function.

  writehttp($filename,undef,%newAttributes), return unless -f $filename;

  # Try real hard to open the file. This may take a while, if 
  # another process has the file open, and it may fail, if another
  # process moves the file.  But if it succeeds, treat the file as
  # a binary file so that some helpful library routine doesn't 
  # alter the body in unhealthy ways.

  _open(\*FILE,"+<$filename") 
    or AVIS::IceBase::fatal("appendhttp failed, cannot open $filename, $!\n");
  binmode FILE;
  
  # Read each line of text at the beginning of the file, parse it
  # as an HTTP header, and save the header name and value in the
  # referenced associative array.  Stop after reading the empty
  # line that separates the headers from the body.

  my %oldAttributes = ();
  while (<FILE>) {
    last if $_ eq "\n" or $_ eq "\r\n";
    #print "read $_";
    $oldAttributes{$1} = $2, next if /^(\S+): ([^\n\r\f]*)/;
    AVIS::IceBase::fatal("appendhttp failed, cannot parse \"$_\" in $filename\n");
  }

  # Read the body of the file, if any.

  my $content = "";
  my $length = read FILE, $content, (stat($filename))[7];
  AVIS::IceBase::fatal("appendhttp failed, cannot read $filename\n")
    unless defined $length;

  # Merge the new attributes with the old attributes.  That is,
  # replace the values of any existing attributes, and add all
  # new attributes to the associative array.
  my $attribute;
  foreach $attribute (keys %newAttributes) { 
    $oldAttributes{$attribute} = $newAttributes{$attribute}; 
  }

  # Rewind the file to the beginning.

  seek FILE, 0, 0 or 
    AVIS::IceBase::fatal("appendhttp failed, cannot seek in $filename\n");
    
  # Write the merged new and old attributes into the beginning of 
  # the file.

  foreach $attribute (sort keys %oldAttributes) { 
    print FILE "$attribute: $oldAttributes{$attribute}$eol" 
      or AVIS::IceBase::fatal("appendhttp failed, cannot write $filename, $!\n"); 
  }

  # Write an empty line into the file to separate the headers
  # the body, and then write the body, if any.

  print FILE "$eol"
    or AVIS::IceBase::fatal("appendhttp failed, cannot write $filename, $!\n");
  print FILE $content
    or AVIS::IceBase::fatal("appendhttp failed, cannot write $filename, $!\n");

  # Close the file and we're done.

  close FILE;
  return;
}

=head2 movehttp

movehttp

=cut

sub movehttp ($$%) {
  # The parameters for this function are the path and name of an
  # existing file, a directory to move the file into, and an 
  # associative array of attributes to append to the file.

  my ($filename,$directory,%newAttributes) = @_;

  # Construct a full path and name for the target file we're 
  # expected to create.  If it already exists, the caller has
  # become confused.

  my ($name) = $filename =~ /\\([^\\]+)$/;
  my $target = "$directory\\$name";
  #print "movehttp: name=$name, target=$target\n";
  #AVIS::IceBase::fatal("movehttp failed, eixsting file $target\n") if -f $target;

  # The file the new attributes are to be appended to may not exist
  # yet.  This degenerate case is handled by the writehttp() function.

  writehttp($target,undef,%newAttributes), return unless -f $filename;

  # Try real hard to open the new file.  This shouldn't be hard, because
  # there shouldn't be any contention over it, but we're feeling
  # cautious today.  The file will be treated as a binary file so that
  # helpful library routines don't alter the body content.

  _open(\*NEW,">$target") 
    or AVIS::IceBase::fatal("movehttp failed, cannot open $target, $!\n");
  binmode NEW;

  # Try real hard to open the old file. This may take a while, if 
  # another process has the file open, and it may fail, if another
  # process moves the file.  But if it succeeds, treat the file as
  # a binary file so that some helpful library routine doesn't 
  # alter the body in unhealthy ways.

  _open(\*OLD,"<$filename") 
    or AVIS::IceBase::fatal("movehttp failed, cannot open $filename, $!\n");
  binmode OLD;

  # Read each line of text at the beginning of the file, parse it
  # as an HTTP header, and save the header name and value in the
  # referenced associative array.  Stop after reading the empty
  # line that separates the headers from the body.
  
  my %oldAttributes = ();
  while (<OLD>) {
    last if $_ eq "\n" or $_ eq "\r\n";
    #print "read $_";
    $oldAttributes{$1} = $2, next if /^(\S+): ([^\n\r\f]*)/;
    AVIS::IceBase::fatal("movehttp failed, cannot parse \"$_\" in $filename\n");
  }

  # Read the body of the file, if any.

  my $content = "";
  my $length = read OLD, $content, (stat($filename))[7];
  AVIS::IceBase::fatal("movehttp failed, cannot read $filename\n")
    unless defined $length;

  # Merge the new attributes with the old attributes.  That is,
  # replace the values of any existing attributes, and add all
  # new attributes to the associative array.
  my $attribute;
  foreach $attribute (keys %newAttributes) { 
    $oldAttributes{$attribute} = $newAttributes{$attribute}; 
  }

  # Write the merged new and old attributes into the new file.
  
  foreach $attribute (sort keys %oldAttributes) { 
    print NEW "$attribute: $oldAttributes{$attribute}$eol" 
      or AVIS::IceBase::fatal("movehttp failed, cannot write $target, $!\n"); 
  }

  # Write an empty line into the file to separate the headers
  # the body, and then write the body, if any.

  print NEW "$eol"
    or AVIS::IceBase::fatal("movehttp failed, cannot write $target, $!\n");
  print NEW $content
    or AVIS::IceBase::fatal("movehttp failed, cannot write $target, $!\n");
  
  # Close the files, erase the old file, and we're done.

  close NEW;
  close OLD;
  _erase($filename) or AVIS::IceBase::fatal("movehttp failed, cannot erase $filename, $!\n");
  
  return;
}


=head2 _erase (NOT EXPORTED)

_erase

=cut

sub _erase ($) {
  my ($filename) = @_;

  my $maximum = AVIS::IceBase::config('retryMaximum');
  my $retry;
  for ($retry=$maximum; $retry; sleep($maximum - --$retry)) {
    return 1 unless -f $filename;
    unlink($filename)==1 && return 1;
    AVIS::IceBase::event("_erase retrying, cannot erase $filename, $!\n");
  }

  return 0;
}

=head2 capture

This function captures a sample of a file that may contain an
unknown virus, along with attributes of the file needed to 
analyze it, and stores the result in another file.  The content
of the file may be compressed, concealed, and encoded as 
specified by tags in the profile.

=cut

sub capture ($$%) {
  # The parameters for this function are the source file to be 
  # captured, the target file to store, and additional
  # attributes to save along with the file's attributes.

  my ($source,$target,%attributes) = @_;
  AVIS::IceBase::fatal("capture failed, cannot find $source\n") unless -f $source;

  # Add the key attributes of the original file to the set
  # of attributes specified by the caller, overriding the
  # values of any duplicates.

  $attributes{'X-Sample-Source'} = $source;
  $attributes{'X-Sample-Size'}  = (stat($source))[7];
  $attributes{'X-Date-Modified'} =HTTP::Date::time2str((stat($source))[9]);
  $attributes{'X-Date-Created'} = HTTP::Date::time2str((stat($source))[10]);
  $attributes{'X-Date-Captured'} = HTTP::Date::time2str();
  
  # Add capture-related parameters specified in the profile, if any.
  
  my $category = AVIS::IceBase::config('captureCategory');
  $attributes{'X-Sample-Category'} = $category if $category;
  my $switches = AVIS::IceBase::config('captureSwitches');
  $attributes{'X-Sample-Switches'} = $switches if $switches;
  
  # Calculate the MD5 checksum of the original file and add it
  # to the set of attributes.
  
  $attributes{'X-Sample-Checksum'} = md5digest($source);
  
  # ...

  my $temporary = $source;

  # Compress the original file as specified in the profile, store
  # the result in a temporary file, and add the compression method
  # used to the attributes.
  
 compress:
  {
    my $compression = AVIS::IceBase::config('captureCompress');
    $attributes{'X-Content-Compression'} = $compression if $compression;
    if ($compression eq "")
      {
	AVIS::IceBase::event("no compression for sample $source\n");
	last compress;
      }
    if ($compression eq "zip")
      {
	AVIS::IceBase::event("zipping sample $source\n");
	my $program = AVIS::IceBase::config('zipProgram');
	my $flags = "-jqDJ";
	my $cc = system("$program $flags $source.Compressed $temporary $nul");
	AVIS::IceBase::fatal("capture failed, cannot zip $temporary, cc=$cc, $!\n") if $cc;
	AVIS::IceBase::event("zipped sample $source\n");
	$temporary = "$source.Compressed";
	last compress;
      }
    AVIS::IceBase::fatal("unknown compression method \"$compression\" specified\n");
  }
    
    # Conceal the compressed file as specified in the profile, store
    # the result in a temporary file, and add the concealment method
    # used to the attributes.
    
  conceal:
    {
      my $concealment = AVIS::IceBase::config('captureConceal');
      $attributes{'X-Content-Concealment'} = $concealment if $concealment;
      if ($concealment eq "")
	{
	  AVIS::IceBase::event("no concealment for sample $source\n");
	  last conceal;
	}
      if ($concealment eq "scramble")
	{
	  AVIS::IceBase::event("scrambling sample $source\n");
	  my $rc = _convert($temporary,"$source.Concealed","scramble");  
	  AVIS::IceBase::fatal("scramble failed, cannot scramble $temporary\n") if $rc;
	  AVIS::IceBase::event("scrambled sample $source\n");
	  $temporary = "$source.Concealed";
	  last conceal;
	}
      AVIS::IceBase::fatal("unknown concealment method \"$concealment\" specified\n");
    }
      
      # Encode the concealed file as specified in the profile, store
      # the result in a temporary file, and add the encoding method
      # used to the attributes.
      
    encode:
      {
	my $encoding = AVIS::IceBase::config('captureEncode');
	$attributes{'X-Content-Encoding'} = $encoding if $encoding;
	if ($encoding eq "")
	  {
	    AVIS::IceBase::event("no encoding for sample $source\n");
	    last encode;
	  }
	if ($encoding eq "base64")
	  {
	    AVIS::IceBase::event("BASE64-encoding sample $source\n");
	    my $rc = _convert($temporary,"$source.Encoded","base64encode");  
	    AVIS::IceBase::fatal("BASE64-encoding failed, cannot encode $temporary\n") if $rc;
	    AVIS::IceBase::event("BASE64-encoded sample $source\n");
	    $temporary = "$source.Encoded";
	    last encode;
	  }
	AVIS::IceBase::fatal("unknown encoding method \"$encoding\" specified\n");
      }
	
	# Calculate the MD5 checksum of the compressed and concealed
	# and encoded body and add it to the set of attributes.
	
	$attributes{'X-Content-Checksum'} = md5digest($temporary);
  
  # Add the datatype of the sample body to the attributes.

  $attributes{'Content-Type'} = "application/x-ibmav";
  
  # Read the contents of the temporary file into a string variable,
  # and add the size of the content to the attributes.
  
  my $content;
  my $contentSize = (stat($temporary))[7];
  open FILE,"<$temporary" or AVIS::IceBase::fatal("capture failed, cannot open $temporary\n");
  binmode FILE;
  read FILE, $content, $contentSize 
    or AVIS::IceBase::fatal("capture failed, cannot read $temporary, $!\n");
  close FILE;
  AVIS::IceBase::fatal("capture failed, length error reading $temporary\n") 
    unless length($content)==$contentSize;
  $attributes{'Content-Length'} = $contentSize;
  
  # Store the sample body and attributes in the specified target file,
  # and then manually deallocate the string variable containing the
  # content.
  
  unlink $target if -f $target;
  writehttp($target, \$content, %attributes); 
  undef $content;
  
  # Thoroughly erase the temporary files used to capture the sample.
  
  wipe("$source.Compressed") if -f "$source.Compressed";
  wipe("$source.Concealed") if -f "$source.Concealed";
  wipe("$source.Encoded") if -f "$source.Encoded";
  return 1;
}

=head2 uncapture

This function reconstructs the original file a sample was captured
from, including decoding, de-concealment, and decompression, as
specified by attributes in the sample file.

=cut

sub uncapture ($$) {
  my ($source,$target) = @_;
  AVIS::IceBase::fatal("uncapture failed, cannot find $source\n") unless -f $source;

  # Read the attributes and content of the sample file into a hash
  # and a string variable, respectively.
  
  my $content = "";
  my %attributes = ();
  readhttp($source,\$content,\%attributes) 
    or AVIS::IceBase::fatal("uncapture failed, cannot read $source\n");
  
  # If a Content-Length header is available, ensure that its value and
  # the size of the content match.
  
  my $lengthContent = length($content);
  my $lengthHeader = $attributes{'Content-Length'};
  if ($lengthHeader && $lengthHeader != $lengthContent) {
    my $rc = "content length $lengthContent does not match Content-Length header $lengthHeader";
    AVIS::IceBase::error("uncapture failed, $rc\n");
    return $rc;
  }
  
  # If the content datatype is not specified, assume the content is
  # scrambled with the IBMAV mask and encoded as BASE64.
  
  my $typeHeader = $attributes{'Content-Type'};
  if ($typeHeader eq "application/x-ibmav-scrambled") {
    $typeHeader = "application/x-ibmav";
    $attributes{'X-Content-Concealment'} = "scramble" unless $attributes{'X-Content-Concealment'};
    $attributes{'X-Content-Encoding'} = "base64" unless $attributes{'X-Content-Encoding'};
  }
  unless ($typeHeader eq "application/x-ibmav") {
    my $rc = "unknown content datatype \"$typeHeader\" specified";
    AVIS::IceBase::error("$rc\n");
    return $rc;
  }
  
  # Write the content of the sample file into a temporary file.
  
  my $temporary = "$target.Content";
  open FILE,">$temporary" 
    or AVIS::IceBase::fatal("uncapture failed, cannot open $temporary, $!\n");
  binmode FILE;
  print FILE $content
    or AVIS::IceBase::fatal("uncapture failed, cannot write $temporary, $!\n");
  close FILE;
  undef $content;
  
  # If an X-Content-Checksum header is available, calculate the MD5
  # checksum of the content of the sample file and ensure that the
  # values match.
  
  my $digestHeader = uc($attributes{'X-Content-Checksum'});
  if ($digestHeader) {
    my $digestTarget = md5digest($temporary);
    unless ($digestTarget eq $digestHeader) {
      my $rc = "content checksum $digestTarget does not match X-Content-Checksum $digestHeader";
      AVIS::IceBase::error("$rc\n");
      return $rc;
    }
  }
  
  # Decode the content of the sample file, as specified in the sample
  # attributes, and store the result in a temporary file.
  
 decode:
  {
    my $encoding = $attributes{'X-Content-Encoding'};
    if ($encoding eq "") {
      AVIS::IceBase::event("no decoding for sample $source\n");
      last decode;
    }
    if ($encoding eq "base64") {
      AVIS::IceBase::event("BASE64-decoding sample $source\n");
      my $rc = _convert($temporary,"$target.Decoded","base64decode");  
      AVIS::IceBase::error("BASE64-decoding failed, $rc\n"), return $rc if $rc;
      AVIS::IceBase::event("BASE64-decoded sample $source\n");
      $temporary = "$target.Decoded";
      last decode;
    }
    my $rc = "unknown encoding method \"$encoding\" specified";
    AVIS::IceBase::error("$rc\n");
    return $rc;
  }
    
    # De-conceal the content of the sample file, as specified in the 
    # sample attributes, and store the result in a temporary file.
    
 unconceal:
    {
    my $concealment = $attributes{'X-Content-Concealment'};
    if ($concealment eq "") {
      AVIS::IceBase::event("no unconcealing for sample $source\n");
      last unconceal;
    }
    if ($concealment eq "scramble") {
      AVIS::IceBase::event("unscrambling sample $source\n");
      my $rc = _convert($temporary,"$target.Unconcealed","scramble");  
      AVIS::IceBase::error("unscrambling failed, $rc\n"), return $rc if $rc;
      AVIS::IceBase::event("unscrambled sample $source\n");
      $temporary = "$target.Unconcealed";
      last unconceal;
    }
    my $rc = "unknown concealment method \"$concealment\" specified";
    AVIS::IceBase::error("$rc\n");
    return $rc;
  }
      
      # Decompress the content of the sample file, as specified in the 
      # sample attributes, and store the result in a temporary file.
      
    decompress:
      {
	my $compression = $attributes{'X-Content-Compression'};
	if ($compression eq "") {
	  AVIS::IceBase::event("no decompression for sample $source\n");
	  last decompress;
	}
	if ($compression eq "zip") {
	  AVIS::IceBase::event("unzipping sample $source\n");
	  my $program = AVIS::IceBase::config('unzipProgram');
	  my $drive = (AVIS::IceFile::parse($target))[2];
	  my $directory = (AVIS::IceFile::parse($target))[3];
	  my $filename = "";
	  open UNZIP,"$program -v $temporary|" or AVIS::IceBase::fatal("cannot run $program, $!\n");
	  while (<UNZIP>) { 
	    #split; 
	    #$filename = $_[7] if $_[7]; 
	    my @dir = split;
	    $filename = $dir[7] if $dir[7];
	  }
	  close UNZIP;
	  unless ($filename) {
	    my $rc = "no sample file found in compressed sample body";
	    AVIS::IceBase::error("$rc\n");
	    return $rc;
	  }
	  my $flags = "-oq";
	  my $cc = system("$program $flags $temporary -d $drive$directory $nul");
	  AVIS::IceBase::error("unzip failed, $cc\n"), return $cc if $cc;
	  AVIS::IceBase::event("unzipped file $filename in sample $source\n");
	  $temporary = "$drive$directory$filename";
	  last decompress;
	}
	my $rc = "unknown compression method \"$compression\" specified";
	AVIS::IceBase::error("$rc\n");
	return $rc;
      }
	
	# If an X-Sample-Size header is available, ensure that its value and
	# the size of the reconstructed original file match.
	
	my $sizeTarget = (stat($temporary))[7];
  my $sizeHeader = $attributes{'X-Sample-Size'};
  if ($sizeHeader && $sizeHeader != $sizeTarget) {
    my $rc = "reconstructed size $sizeTarget does not match X-Sample-Size $sizeHeader";
    AVIS::IceBase::error("uncapture failed, $rc\n");
    return $rc;
  }
  
  # If an X-Sample-Checksum header is available, calculate the MD5
  # checksum of the reconstructed file and ensure that the values match.
  
  $digestHeader = uc($attributes{'X-Sample-Checksum'});
  if ($digestHeader) {
    my $digestTarget = md5digest($temporary);
    unless ($digestTarget eq $digestHeader) {
      my $rc = "reconstructed checksum $digestTarget does not match X-Sample-Checksum $digestHeader";
      AVIS::IceBase::error("$rc\n");
      return $rc;
    }
  }
  
  # Rename the reconstructed original file to the specified target filename.
  
  _rename($temporary,$target);
  
  # Thoroughly erase the temporary files used in reconstructing the 
  # original file the sample was captured from.
  
  wipe("$target.Content") if -f "$target.Content";
  wipe("$target.Decoded") if -f "$target.Decoded";
  wipe("$target.Unconcealed") if -f "$target.Unconcealed";
  return "";
}

=head2 _convert (NOT EXPORTED)

_convert

=cut

sub _convert ($$$) {
  # ...

  my ($source,$target,$method) = @_;

  # Try real hard to open the old file. This may take a while, if 
  # another process has the file open, and it may fail, if another
  # process moves the file.  But if it succeeds, treat the file as
  # a binary file so that some helpful library routine doesn't 
  # alter the body in unhealthy ways.

  _open(\*OLD,"<$source") 
    or AVIS::IceBase::fatal("scramble failed, cannot open $source, $!\n");
  binmode OLD;
  
  # ...
  
  my $content = "";
  my $contentSize = (stat($source))[7];
  my $length = read OLD, $content, $contentSize;
  close OLD;
  unless ($length == $contentSize) {
    my $rc = "read only $length bytes of $contentSize";
    AVIS::IceBase::error("$method conversion failed, $rc\n");
    return $rc;
  }

  # ...
  
  my $converted = "";
 convert:
  {
    if ($method eq "base64encode") {
	    $converted = MIME::Base64::encode($content,$eol);
	    last convert;
	}
	if ($method eq "base64decode")  {
	    $converted = MIME::Base64::decode($content);
	    last convert;
	}
	if ($method eq "scramble") {
	    my $scrambleMask = "YAKVAMPISHUCHEGOZHEBOLE..";
	    for (my $i=0; $i<$contentSize; $i+=length($scrambleMask))
	       { $converted .= substr($content,$i,length($scrambleMask)) ^ $scrambleMask;  }
	    substr($converted,$contentSize) = "";
	    last convert;
	}
	my $rc = "unknown conversion method \"$method\" specified";
	AVIS::IceBase::error("$rc\n");
	return $rc;
    }
    
    # ...

    undef $content;

    # Try real hard to open the new file.  This shouldn't be hard, because
    # there shouldn't be any contention over it, but we're feeling
    # cautious today.  The file will be treated as a binary file so that
    # helpful library routines don't alter the body content.

    _open(\*NEW,">$target") 
	or AVIS::IceBase::fatal("scramble failed, cannot open $target, $!\n");
    binmode NEW;
    my $rc = "$method conversion failed";
    print NEW $converted or AVIS::IceBase::error("$method conversion failed, $rc\n"), return $rc;
    close NEW;

    # ...
    
    undef $converted;

    # ...

    return "";
}

=head2 getSTATUSTXT

Reads in the SARC status file and returns a hash with the values.

=cut

sub getSTATUSTXT ($) {
  my $file = shift;
  my %status = ();
  my $current;

  open FILE, "<$file" or AVIS::IceBase::event("cannot open $file"), return ();
  while (<FILE>) {
    # comment
    if (/^\s*;/) {
    }
    # keyword end
    elsif (/^\[\~(.*)\]/) {
      close FILE, return () unless ($current eq $1);
      # delete trailing line endings
      chomp $status{$current};
      undef $current;
    } 
    # keyword begin
    elsif (/^\[(.*)\]/) {
      $current = $1;
    }
    # content. In the case of 'log' multiple lines are ok.
    else {
      if (defined $current) {
	$status{$current} .= $_;
      } 
    }
  }
  close FILE;
  # Plausibility check on filename, which is all we really are
  # interested in
  my $ok = 0;
  if ((defined $status{filename}) && ($status{filename} =~ /^[\w\d\.\-\~\!]+$/)) {
    $ok = 1;
  }
  if ($ok) {
    return %status;
  } else {
    return ();
  }
}

=head2 putSTATUSTXT

Writes the SARC status file from the input hash

=cut

sub putSTATUSTXT ($%) {
  my ($file, %hash) = @_;
  my $key;
  open FILE, ">$file" or AVIS::IceBase::event("cannot open $file"), return 0;
  foreach $key (keys %hash) {
    print FILE "[$key]\n";
    print FILE "$hash{$key}\n";
    print FILE "[~$key]\n\n";
  }
  close FILE;
  return 1;
}

######################################################################

1;

=head1 HISTORY

MS19990221 - changed use of external md5sum program to perl extension
MD5. Added pod style documentation. Fixed a few minor bugs.

=head1 BUGS

parse: Should use File::Basename::fileparse instead. One day...

=head1 TO DO

Add overwriting the file to wipe.

=head1 SEE ALSO

=cut
