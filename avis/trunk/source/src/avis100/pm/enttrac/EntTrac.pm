package EntTrac;

use strict;
use Carp;
use vars qw($VERSION @ISA @EXPORT @EXPORT_OK $AUTOLOAD);

require Exporter;
require DynaLoader;
require AutoLoader;

@ISA = qw(Exporter DynaLoader);
# Items to export into callers namespace by default. Note: do not export
# names by default without a very good reason. Use EXPORT_OK instead.
# Do not simply export all your public functions/methods/constants.
@EXPORT = qw(
);
@EXPORT_OK = qw (
		 ReadPos	
		 TrackFile
		 Partition
);
$VERSION = '0.02';

sub AUTOLOAD {
    # This AUTOLOAD is used to 'autoload' constants from the constant()
    # XS function.  If a constant is not found then control is passed
    # to the AUTOLOAD in AutoLoader.

    my $constname;
    ($constname = $AUTOLOAD) =~ s/.*:://;
    my $val = constant($constname, @_ ? $_[0] : 0);
    if ($! != 0) {
	if ($! =~ /Invalid/) {
	    $AutoLoader::AUTOLOAD = $AUTOLOAD;
	    goto &AutoLoader::AUTOLOAD;
	}
	else {
		croak "Your vendor has not defined EntTrac macro $constname";
	}
    }
    eval "sub $AUTOLOAD { $val }";
    goto &$AUTOLOAD;
}

bootstrap EntTrac $VERSION;

# Preloaded methods go here.


# Todo: need to add all other jumps to follow and other opcodes that take
# absolute arguments that might be fixed up at run time.
sub ReadPos ($$$$){
  my ($VIRUS, $curpos, $chunksize, $hash) = @_;
  my ($buffer, @array, $i, $j, $jumploc, $opclen, $readsize);

  seek($VIRUS, $curpos, 0) or return 0;

  # read in a chunk and dump
  $readsize = read $VIRUS, $buffer, $chunksize or return 0;
  @array = unpack "C$chunksize", $buffer;

  $i = 0;
  while ($i < $readsize) {
    $opclen = EntTrac::i86OpcLen($buffer, $i);
    last if ($i + $opclen > $readsize);
    if ($array[$i] == 0xE9     #JMP d
       ) {
      #printf "%08X: %02X\n", $curpos+$i, $array[$i];
      $$hash{$curpos+$i} = $array[$i];
      # JMP takes a direct argument (16 bit)
      $jumploc = ($array[$i+2] << 8) + $array[$i+1] + 3;
      #printf "Jumploc: %04X\n", $jumploc;
      ReadPos($VIRUS, $jumploc, $chunksize, $hash);
      last;
    } elsif ($array[$i] == 0x9A  #CALL i,d
	    ) {
      # ?? should we really follow this?? Maybe just ignore the
      # arguments of all absolute arguments

      #printf "%08X: %02X\n", $curpos+$i, $array[$i];
      $$hash{$curpos+$i} = $array[$i];
      # JMP takes a direct argument (16 bit)
      $jumploc = ($array[$i+2] << 8) + $array[$i+1] + 3;
      #printf "Jumploc: %04X\n", $jumploc;
      ReadPos($VIRUS, $jumploc, $chunksize, $hash);
    } elsif (($array[$i] == 0xEB)     #JMP r
	    ) {
      #printf "%08X: %02X\n", $curpos+$i, $array[$i];
      $$hash{$curpos+$i} = $array[$i];
      # JMP takes a direct argument (16 bit)
      $jumploc = $curpos + $array[$i+1] + 2;
      #printf "Jumploc: %04X\n", $jumploc;
      ReadPos($VIRUS, $jumploc, $chunksize, $hash);
      last;
    } else {
      for ($j = 0; $j < $opclen; $j++) {
	#printf "%08X: %02X\n", $curpos+$i+$j, $array[$i+$j];
	$$hash{$curpos+$i+$j} = $array[$i+$j];
      }
    }
    $i += $opclen;
  }
  1;
}

# BUG: only works for COM file. Needs EXE support.
sub TrackFile {
  my ($file, $hashref, $chunksize) = @_;
  my ($buffer);
  my ($M, $Z, $nLast, $nPages, $nReloc, $nHeader, $nMem, $nMax, $SS, $SP, $CheSu, $IP, $CS, $Reloc, $Ovl);
  
  open VIRUS, "<$file" or warn "File '$file' could not be opened", return 0;
  binmode VIRUS;

  seek(VIRUS, 0, 0) or return 0;
  read(VIRUS, $buffer, 512) or return 0;
  ($M, $Z, $nLast, $nPages, $nReloc, $nHeader, $nMem, $nMax, $SS, $SP, $CheSu, $IP, $CS, $Reloc, $Ovl) = unpack "AASSSSSSSSSSSSS", $buffer;

#  print "$M $Z\n";
  if ((($M eq 'M') && ($Z eq 'Z')) || (($M eq 'Z') && ($Z eq 'M'))) {
    #print "Oy ve!, an EXE\n";
    #printf "CS: 0x%X, IP: 0x%X, Header size: 0x%X\n", $CS, $IP, $nHeader;
    my $entry = ($CS << 4) + $IP + ($nHeader << 4);
    #printf "Entry point: 0x%X\n", $entry;
    ReadPos(\*VIRUS, $entry, $chunksize, $hashref);
  } else {
    ReadPos(\*VIRUS, 0, $chunksize, $hashref);
  }

  close VIRUS;
}


# Comparison idea: Create such hashes for every sample. Create a
# 'partition' array (one element per sample) and initialize it so that
# all samples are in partition -1 (unchecked). Maintain a list of
# defined partitions. Start with the first sample and mark it as
# partition 0. Compare the hash with the next sample. If it is
# different, create a new partition for it, and mark it so. If it was
# the same, mark it with the first partition number. Thus go through
# the entire list creating a list of partition membership of each
# sample.

sub Partition {
  my (@filelist) = @_;

  my %hash;
  my $file;
  my %hashstring;
  my %partition;
  my $maxpartition;
  my $i;
  my $j;

  # Create such hashes for every sample.
  foreach $file (@filelist) {
    #print "Processing $file\n";
    %hash = ();
    # create the track hash. The precise chunksize value might need tuning.
    TrackFile($file, \%hash, 16);

    # Now convert the hash into a string containing only the hash
    # values. As it is sorted, if each virus infected in the same way,
    # the opcodes in this list should create identical strings, or at
    # least very similar strings if the virus infected in a very
    # similar way.

    # print "\n";
    $hashstring{$file} = "";
    foreach (sort keys %hash) {
      #printf "%08X= %02X\n", $_, $hash{$_};
      $hashstring{$file} .= sprintf "%02X", $hash{$_};
    }
    #print "$hashstring{$file}\n";
  }
  # Create a 'partition' array (one element per sample) and initialize
  # it so that all samples are in partition -1 (unchecked).

  %partition = ();

  # Start with the first sample and mark it as partition 0.
  $maxpartition = 0;
  for ($i = 0; $i <= $#filelist; $i++) {
    # skip any sample of known partition
    next if defined $partition{$filelist[$i]};
    # Create a new partition for this example
    $partition{$filelist[$i]} = $maxpartition;
    # increment the partition counter for next time
    $maxpartition++;
    #print "Using $filelist[$i] as an example for partition $partition{$filelist[$i]}\n";
    # Go through the entire list of files and see if they are equal to
    # this example. If they are, then add it to the partition.
    for($j = 1; $j <= $#filelist; $j++) {
      #??? Should we skip defined files as before??
      #print "is $hashstring{$filelist[$j]} eq $hashstring{$filelist[$i]}?\n";
      if ($hashstring{$filelist[$j]} eq $hashstring{$filelist[$i]}) {
	$partition{$filelist[$j]} = $partition{$filelist[$i]};
	#print "Adding $filelist[$j] to $partition{$filelist[$i]}\n";
      }
    }
  }
  %partition;
}

#Format of .EXE file header:
#Offset  Size    Description
# 00h  2 BYTEs   .EXE signature, either "MZ" or "ZM" (5A4Dh or 4D5Ah)
# 02h    WORD    number of bytes in last 512-byte page of executable
# 04h    WORD    total number of 512-byte pages in executable (includes any
#                partial last page)
# 06h    WORD    number of relocation entries
# 08h    WORD    header size in paragraphs
# 0Ah    WORD    minimum paragraphs of memory to allocation in addition to
#                executable's size
# 0Ch    WORD    maximum paragraphs to allocate in addition to executable's size
# 0Eh    WORD    initial SS relative to start of executable
# 10h    WORD    initial SP
# 12h    WORD    checksum (one's complement of sum of all words in executable)
# 14h    DWORD   initial CS:IP relative to start of executable
# 18h    WORD    offset within header of relocation table (40h for New EXE)
# 1Ah    WORD    overlay number (normally 0000h = main program)

# Autoload methods go after =cut, and are processed by the autosplit program.

1;
__END__
# Below is the stub of documentation for your module. You better edit it!

=head1 NAME

EntTrac - Perl extension for finding entry point differences in a given set
of DOS executable files

=head1 SYNOPSIS

  use EntTrac qw(Partition);
  @filelist = qw(file1 file2);
  %partitionlist = Partition(@filelist);
  if ($partitionlist{"file1"} == $partitionlist{"file2"}) {
    print "Files 1 and 2 are in the same partition!\n";
  }

=head1 DESCRIPTION

Use Partition with a list of file names to create a hash of partitions, 
or more precisely, the hash key references a partition number. If you
feed it a list of the same virus, it should tell you if there are
significant differences in the entry point of the virus.

Warning: Partition has no knowledge of what a virus is, so you can use
it to partition programs with unpredicable results.

=head1 AUTHOR

Morton Swimmer, swimmer@us.ibm.com

=head1 SEE ALSO

perl(1).

=cut













































