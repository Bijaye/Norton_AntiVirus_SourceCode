$::MAX_SIG_OFFSET = 128;
$::MAX_SIG_LENGTH =  24;
$::MIN_SIG_LENGTH =   4;
@::SIG_START_CHARS = (
   "\x2E", "\x3D", "\x50", "\x74", "\x75", "\x8B", "\x8C", "\xB8",
   "\xB9", "\xBA", "\xBB", "\xBE", "\xBF", "\xCD", "\xE8", "\xE9"
);

my ($lConst, $lVar, $encr, $seed, $dir, $codo) = @ARGV;
die "bad encryption\n" unless $encr =~ m/^[pax]$/i;
die "too short!\n" if $lConst < 32 or $lVar < 32;
srand ($seed);

my $ConstV  = randomStr($lConst);
my $lConst1 = evenIrand($lConst-16)+8;
my $lConst2 = $lConst - $lConst1;

my $lVar1 = evenIrand($lVar-16)+8;
my $lVar2 = $lVar - $lVar1;

my $ConstV2 = randomStr($lConst2);
my $EP = evenIrand($lConst1-$::MIN_SIG_LENGTH);

my $KeyPos = Irand($lVar1-2);

my $maxSigLen = min ($::MAX_SIG_LENGTH, $lConst1-$EP);
my $sigLen = $::MIN_SIG_LENGTH + Irand ($maxSigLen-$::MIN_SIG_LENGTH);
my $sig = $::SIG_START_CHARS[Irand($#::SIG_START_CHARS)].randomStr($sigLen-1);
my $maxSigStart = min($lConst1-$sigLen, $EP+$::MAX_SIG_OFFSET);
my $sigStart = $EP + Irand($maxSigStart-$EP);
substr($ConstV,$sigStart,$sigLen) = $sig;

print "const viral: \t$lConst \t($lConst1 + $lConst2)\nvar. viral: \t$lVar \t($lVar1 + $lVar2)\nentry pt: \t$EP\nKey locn: \t$KeyPos ($encr)\nsignature: \t$sigLen bytes @ EP + ".($sigStart - $EP)."\n";

foreach $file (glob("$dir\\*.exe")) {
   makeFakeVir($file, $lConst1, $lConst2, $lVar1, $lVar2, $ConstV, $EP, $KeyPos, $encr, $codo); 
}


sub makeFakeVir {
 # construct a fake infected file as follows:
 #    Infected file header
 #    HostLM
 #    Padding
 #    Iconst1 (contains EP & signature)
 #    Ivar1   (contains key)
 #    Host header
 #    Iconst2
 #    Ivar2
   
   my ($file, $lConst1, $lConst2, $lVar1, $lVar2, $ConstV, $EP, $KeyPos, $encr, $codo) = @_;
   my ($ALIGN, $lHDR) = (32, 24); 

   print "\n$file\n";
   my $lFile = -s $file;
 
 # choose a key
   my $K1 = randByte();
   my $K2 = randByte();

 # encrypt the Iconst regions (but not between the EP and the end of the 1st Iconst section, where the signature is) 
   my $eConstV1 = encrypt(substr($ConstV,0,$EP), $encr, $K1, $K2).substr($ConstV,$EP,$lConst1-$EP);
   my $eConstV2 = encrypt(substr($ConstV,$lConst1), $encr, $K1, $K2);

 # get the host file
   my $host = '';
   open IN, $file or die "cannot open $file: $!\n";
   binmode IN;
   if (read (IN, $host, $lFile) != $lFile) {die "bad host read\n";} 
   close IN;

 # encrypt that part of the host header that will be saved
   my $lHostHdr = $lHDR-6;
   my $hostHdr = encrypt(substr($host,6,$lHostHdr), $encr, $K1, $K2);
 # the host load module  
   my $hostLM = encrypt(substr($host,$lHDR), $encr, $K1, $K2);

 # create the Ivar regions  
   my $VarV1 = randomStr($lVar1);
   my $VarV2 = randomStr($lVar2);

 # create the padding needed for alignment
   my $lPad = ($ALIGN - ($lFile % $ALIGN)) % $ALIGN;
   my $padding = randomStr($lPad);

 # put the key in the 1st Ivar region
   substr($VarV1,$KeyPos,1) = chr($K1);
   substr($VarV1,$KeyPos+1,1) = chr($K2) if $encr =~ m/[AX]/;
   my $KPabs = $lFile + $lPad + $lConst1 + $KeyPos;


 # construct the Ifile's header...
   my $Ihdr = 'MZ' . (chr(0) x 22);

 #     image length 
   my $lOut = $lFile + $lPad + $lConst1 + $lVar1 + $lHostHdr + $lConst2 + $lVar2;
   my $LMoffset = (ord(substr($host,8,1)) + ord(substr($host,9,1))*0x100) * 0x10;
   my $lLMout = $lOut - $LMoffset;
   substr($Ihdr,2,1) = lo($lLMout % 512);
   substr($Ihdr,3,1) = hi($lLMout % 512);

 #     file page count
   substr($Ihdr,4,1) = lo(int(($lOut+511) / 512));
   substr($Ihdr,5,1) = hi(int(($lOut+511) / 512));
   
 #     header length remains the same
   substr($Ihdr,8,2) = substr($host,8,2);

 #     code segment begins at the start of Iconst1
   my $CSoffset = $lFile + $lPad - $LMoffset;
   substr($Ihdr,0x16,1) = lo(int($CSoffset / 0x10));
   substr($Ihdr,0x17,1) = hi(int($CSoffset / 0x10));

 #     entry point
   substr($Ihdr,0x14,1) = lo($EP);
   substr($Ihdr,0x15,1) = hi($EP);
   $EPabs = $lFile + $lPad + $EP;

 # write the Ifile

   print "\thost length $lFile pad length $lPad; infected length $lOut (should be ".($lFile+$lPad+$lConst1+$lConst2+$lVar1+$lVar2+$lHostHdr).")\n";
   print "\tLM length $lLMout @ $LMoffset; CS @ $CSoffset; EP @ $EPabs; Key @ $KPabs (T".signed($KPabs-$EPabs).",E".signed($KPabs+1-$lOut).")\n";
   print "\tkey ".($encr!~/[AX]/?"$K1" : "$K1,$K2 = ".($K1 + $K2 * 0x100))."\n" unless $encr =~ /p/i;  
   print "\tIHdr\tHostLM\tPad\tConst1\tVar1\tHhdr\tConst2\tVar2\n";
   my $o = 0;
   print "B\t".$o."\t".($o+=length($Ihdr))."\t".($o+=length($hostLM))."\t".($o+=length($padding))."\t".($o+=length($eConstV1))."\t".($o+=length($VarV1))."\t".($o+=length($hostHdr))."\t".($o+=length($eConstV2))."\t|<".($o+=length($VarV2))."\n";
   $o = -$EPabs;
   print "T\t".$o."\t".($o+=length($Ihdr))."\t".($o+=length($hostLM))."\t".($o+=length($padding))."\t".($o+=length($eConstV1))."\t".($o+=length($VarV1))."\t".($o+=length($hostHdr))."\t".($o+=length($eConstV2))."\t|<".($o+=length($VarV2))."\n";
   $o = 1-$lOut;
   print "E\t".$o."\t".($o+=length($Ihdr))."\t".($o+=length($hostLM))."\t".($o+=length($padding))."\t".($o+=length($eConstV1))."\t".($o+=length($VarV1))."\t".($o+=length($hostHdr))."\t".($o+=length($eConstV2))."\t|<".($o+=length($VarV2))."\n";

   $outFile = $Ihdr.$hostLM.$padding.$eConstV1.$VarV1.$hostHdr.$eConstV2.$VarV2;
   print "*** output string length mismatch: ".length($outFile)."\n" if length($outFile) != $lOut;
   open OUT, ">$file" or die "cannot open $file: $!\n";
   binmode OUT;
   print OUT $outFile;
   close OUT;
   die "$file length not $lOut\n" unless -s $file == $lOut;

   open SECT, ">$file.sections" or die "cannot open $file.sections: $!\n";
   print SECT "   [$LMoffset  ".($lFile + $lPad + $lConst1 - 1)."]\n";
   print SECT "   [".($lFile + $lPad + $lConst1 + $lVar1 + $lHostHdr)." ".($lFile + $lPad + $lConst1 + $lVar1 + $lHostHdr + $lConst2 - 1)."]\n";
   close SECT;
   print `copy $file.sections $codo`; 
   print `del $file.sections`; 
}


sub encrypt {
   my ($str, $encr, $K1, $K2) = @_;

   my $out = '';
   my $k = $K1;
   my $K = int ($K1 + $K2 * 0x100);

   if ($encr =~ m/p/i) {
      #!!return $str;
      foreach (unpack "C*", $str) {
         $out .= lo($_);
      }
   }elsif ($encr =~ m/a/) {
      foreach (unpack "C*", $str) {
         $out .= lo($_ + $k);
      }
   }elsif ($encr =~ m/x/) {
      foreach (unpack "C*", $str) {
         $out .= lo($_ ^ $k);
      }
   }elsif ($encr =~ m/A/) {
      foreach (unpack "S*", $str.'\0') { # pad in case $str is of odd length 
         $n = ($_ + $K) % 0x10000;
         $out .= (lo($n) . hi($n)); 
      }
   }elsif ($encr =~ m/X/) {
      foreach (unpack "S*", $str.'\0') { # ditto
         $n = ($_ ^ $K) % 0x10000;
         $out .= (lo($n) . hi($n)); 
      }
   }
   return substr($out, 0, length($str)); # remove any padding
}


sub lo {
   my ($n) = @_;
   return chr ($n % 0x100);
}


sub hi {
   my ($n) = @_;
   return chr ($n / 0x100);
}


sub signed {
   my ($n) = @_;
   return $n < 0 ? "$n" : "+$n";
}

              
sub randomStr {
   my ($l) = @_;
   my $str = '';
   while ($l--) {
      $str .= chr randByte();
   }
   return $str;
}


sub Irand {
   my ($n) = @_;
   return int(rand($n+1));
}


sub evenIrand {
   my ($n) = @_;
   return int(rand($n - ($n % 2)));
}


sub randByte {return Irand(0x100);}


sub min {return $_[0] < $_[1] ? $_[0] : $_[1];}
