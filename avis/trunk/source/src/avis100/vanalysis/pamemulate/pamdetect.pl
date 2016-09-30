#!/usr/bin/perl
######################################################################
#                                                                    #
# Program name:  PAMdetect.pl                                        #
#                                                                    #
# Description:   binary virus detection generation via PAM emulation #
#                                                                    #
# Statement:     Licensed Materials - Property of IBM                #
#                (c) Copyright IBM Corp. 1998-1999                   #
#                                                                    #
# Author:        Andy Raybould                                       #
#                                                                    #
#                U.S. Government Users Restricted Rights - use,      #
#                duplication or disclosure restricted by GSA ADP     # 
#                Schedule Contract with IBM Corp.                    #
#                                                                    #
#                                                                    #
######################################################################
use Cwd;
use AVIS::Local;

sub run  ($);
sub PD   ($);
sub dir  ($);
sub name ($);
sub stem ($);
sub type ($);
sub max  ($$);
sub min  ($$);

sub PAMdetect (\@$);
sub emulateEachFile (\@);
sub getSignatures(\%\%);
sub findCandidatesOfType(\%\%$);
sub printCandidates(\%\%$);
sub resultsAreOK(\%\%);
sub emitCandidates (\%\%$);

sub typeCount (\%$);
sub ORopcodes($$);


$::VERBOSE = 0;
$::LOGFILE = '';

$PAMemulate = PD "$isdnBinDir/PAMemulate.exe";

%typePattern = (COM => '.om', EXE => '.xe', ALL => '.*');

#------------------------------------------------------------------------------

$::HOME = cwd();
run 'copy /B '.PD "$isdnBinDir/master.dat".' .';
run 'copy /B '.PD "$isdnBinDir/build.set0".' .';

my ($root, $outFile, $FP, @infected) =
   ('',    '',       '*', ()       );

for (my $a = 0; $a < @ARGV; $a++) {
   my $arg = $ARGV[$a];
   if    ($arg =~ m/-out/i)     {$outFile   = $ARGV[++$a];}
   elsif ($arg =~ m/-pattern/i) {$FP        = $ARGV[++$a];}
   elsif ($arg =~ m/-root/i)    {$root      = $ARGV[++$a];}
   elsif ($arg =~ m/-log/i)     {$::LOGFILE = $ARGV[++$a];}
   elsif ($arg =~ m/-v/i)       {$::VERBOSE = 1;}
   else                         {push @infected, $arg;}
   die "FAIL: bad argument list @ARGV\n" if $a == @ARGV;
}
if ($::LOGFILE) {
   open LOG, ">$::LOGFILE" or print ("WARN: Unable to open PAMdetect log file $::LOGFILE\n"), $::LOGFILE = '';
}

unless (scalar @infected) {
   die "FAIL: root $root not found\n" unless -d $root;
   @infected = glob ("$root\\$FP.?om $root\\$FP.?xe");
}
my $rc = PAMdetect (@infected,$outFile);
close LOG if ($::LOGFILE);
exit $rc;

#==============================================================================

sub PAMdetect (\@$) {
   my ($pInfected,$outFile) = @_;

   my %signatures;
   my %candidates;
   my %samples = emulateEachFile(@$pInfected);
  
   foreach my $pSample (values %samples) {
      getSignatures (%$pSample,%signatures);
   }

   foreach my $type ('COM','EXE') {   # remove 'ALL' - will be aggregated in vanalysis??
      #!!my $n = typeCount(%samples,$type);
      $candidates{$type} = findCandidatesOfType(%signatures,%samples,$type);
      printCandidates(%signatures,%samples,$type);
   }

   return 8 if !resultsAreOK (%candidates, %samples);

   return emitCandidates (%candidates, %samples, $outFile);
}



sub emulateEachFile (\@) {
   my ($pInfected) = @_;
   my %sample;
   
   foreach my $file (@$pInfected) {
      print "$file\n";
      die "FAIL: sample $file not found or is empty\n" unless -s $file;
      die "FAIL: invalid file name: $file\n" unless $file =~ m<([^\\\.]+)\..(om|xe)$>i;
      
      my $rc =  emulate($file);
      
      $sample{$file} = {path => $file, rc => $rc, sigs => {}};
   }
   return %sample;
}



sub emulate ($) {
   my ($file) = @_;

   run 'copy /B   build.set0   '.dir($file).'\build.set';
   
   my $rc = run "$PAMemulate $file /NMOD 256";
   die "detection emulation of $file failed with rc $rc\n" if $rc;

   run 'copy /B   emulate.log   '.$file.'.emu';
   run 'copy /B   '.dir($file).'\build.set   '.$file.'.opset';

   return $rc;
}



sub getSignatures(\%\%) {
   my ($pSample,$pSigs) = @_;
   print "$$pSample{path} $$pSample{rc}\n";

   my ($prefetch, $EP, $offset, $usedOpcodes, $path) =
      (-1,        -1,  -1,      '',           $$pSample{path});

   foreach (`type $path.emu`) {
      chomp;
      $prefetch      = $1       if m/^GO\s+prefetch=(\d+),/;
      $EP            = hex($1)  if m/^OP.\s+1f,\s*\d+i,\s*([0-9A-F]+),/i;
      $offset        = hex($1)  if m/^OP.\s+\d+f,\s*\d+i,\s*([0-9A-F]+),/i;
      $usedOpcodes   = $1       if m/^INST\s+([01]+)/;
      
      if (m/^SIG\s+(\d+)\s+(\d+)\s+(\S+)/) {
         my $instrCount =  $1;
         my $iteration  =  $2;
         (my $sig = $3) =~ s/,//g;
         # NB: if the sig occurs at several prefetch values, the last (lowest) will be kept 
         unless (defined $$pSigs{$sig}) {
            $$pSigs{$sig} = {sig        => $sig,
                             len        => length($sig)/2,
                             files      => {} 
                            };
         }

         $$pSigs{$sig}{files}{$path} = {path       => $path,
                                        prefetch   => $prefetch,
                                        offset     => $offset,
                                        EPoffset   => $offset - $EP,
                                        opcodes    => $usedOpcodes,
                                        instrCount => $instrCount
                                       };

         $$pSample{sigs}{$sig} = \$$pSigs{$sig};       
      }
   }
   return;
}



sub findCandidatesOfType(\%\%$) {
   my ($pSignatures,$pSamples,$type) = @_;
   my @candidates;

   print "Searching Samples of type $type:\n" if $::VERBOSE;

Sig: 
   foreach my $pSig (values %$pSignatures) {
      print "    ".$$pSig{sig}."\n" if $::VERBOSE;
      my ($prefetch,$opcodes);
      my $maxCount = 0;
      foreach my $path (keys %$pSamples) {
         next unless type($path) =~ m<^$typePattern{$type}$>i;
         printf ('    %15.15s',name($path)) if $::VERBOSE;
         my $pf = $$pSig{files}{$path}{prefetch};
         $prefetch = $pf unless defined $prefetch;
         if (!defined $pf or $pf != $prefetch) {   # if !defined then sig is not in file
            if (defined $pf) {print "\t** prefetch $pf not $prefetch\n" if $::VERBOSE;}
            else             {print "\t** not found\n" if $::VERBOSE;}
            next Sig;
         }
         $opcodes = ORopcodes($opcodes, $$pSig{files}{$path}{opcodes});
         $maxCount = max($maxCount, $$pSig{files}{$path}{instrCount});
         print "\t".$$pSig{files}{$path}{offset}."\t".$$pSig{files}{$path}{instrCount}."\t".$$pSig{files}{$path}{prefetch}."\n" if $::VERBOSE;
      }
      if (defined $prefetch) {   # if prefetch was not set then there are no files of this type
         print "\n" if $::VERBOSE;
         $$pSig{$type}{opcodes} = $opcodes;
         $$pSig{$type}{instrCount} = $maxCount;
         push @candidates, $pSig;
      }
   }
   return \@candidates;
}



sub printCandidates(\%\%$) {
   my ($pSignatures,$pSamples,$type) = @_;

   print LOG "Samples of type $type:\n" if $::LOGFILE;
   foreach my $pSig (values %$pSignatures) {
      print LOG "    ".$$pSig{sig}."\n" if $::LOGFILE;
      my ($prefetch,$opcodes);
      foreach my $path (keys %$pSamples) {
         next unless type($path) =~ m<^$typePattern{$type}$>i;
         print LOG sprintf ('    %15.15s',name($path)) if $::LOGFILE;
         if (defined $$pSig{files}{$path}{path}) {
            if ($$pSig{files}{$path}{path} eq $path) {;
               print LOG "\t".$$pSig{files}{$path}{offset}."\t".$$pSig{files}{$path}{instrCount}."\t".$$pSig{files}{$path}{prefetch}."\t".$$pSig{files}{$path}{opcodes}."\n" if $::LOGFILE;
            }else {
               print LOG "\t*** corrupt entry: ".$$pSig{files}{$path}{path}."\n" if $::LOGFILE;
            }
         }else {
            print LOG "\t*** not found\n" if $::LOGFILE;
         }
      }
   }
}



sub resultsAreOK (\%\%) {
   my ($pCandidates,$pSamples) = @_;
   
   if (!defined $$pCandidates{ALL}[0]) {
      foreach my $type ('COM','EXE') {
         next unless typeCount(%$pSamples,$type) > 0;
         return 0 unless defined $$pCandidates{$type}[0];
      }
   }
   return 1;
}



sub emitCandidates (\%\%$) {
   my ($pCandidates,$pSamples,$file) = @_;

   return 4 unless defined $file;

   open OUT, ">$file" or die "unable to open $file for output\n";

   foreach my $type ('COM','EXE') {
      next unless typeCount(%$pSamples,$type) > 0;
      foreach my $pSig (@{$$pCandidates{$type}}) {
         print OUT "$$pSig{sig}  $type PLAIN CODE    T 0 $$pSig{len} $$pSig{$type}{opcodes} $$pSig{$type}{instrCount}\n";
      }
   }
   close OUT;
   return 0;
}

#------------------------------------------------------------------------------

sub typeCount (\%$) {
   my ($pSamples,$type) = @_;
   my $n = 0;
   map {$n++ if type($_) =~ m<^$typePattern{$type}$>i} keys(%$pSamples);
   return $n;
}


sub ORopcodes($$) {
   my ($x,$y) = @_;
   return $y if !defined $x;
   return $x if !defined $y;
   my @x = split //,$x;
   my @y = split //,$y;
   my $result;
   for (my $i = 0; $i < max(@x,@y); $i++) {
      if    (!defined $x[$i]) {$result .= $y[$i];}
      elsif (!defined $y[$i]) {$result .= $x[$i];}
      else                    {$result .= $x[$i] || $y[$i] ? 1 : 0;}
   }
   return $result;
 }
                   
#------------------------------------------------------------------------------

sub run ($) {
   my ($cmd) = @_;
   print ">>>>>$cmd\n";
   my $rc = system ($cmd);
   print "---->$rc\n";
   return $rc;
}


sub PD ($) {
   my ($path) = @_;
   if ($^O =~ m/win/i) {$path =~ tr[/][\\];}
   else                {$path =~ tr[\\][/];}
   return $path;
}

sub dir  ($) {return ($_[0] =~ m<(.*)\\> ? $1 : '.')}

sub name ($) {return ($_[0] =~ m<.*\\(.+)> ? $1 : $_[0])}

sub stem ($) {my $name = name($_[0]); return ($name =~ m<(.*)\.> ? $1 : $name)}
      
sub type ($) {return ($_[0] =~ m<.*\.(.*)> ? $1 : '')}

sub max ($$) {return $_[0] > $_[1] ? $_[0] : $_[1];}

sub min ($$) {return $_[0] < $_[1] ? $_[0] : $_[1];}

