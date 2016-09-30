sub percent ($$);



my ($root) = @ARGV;

my ($nNotReplicated, $nDetected, $nRepaired, $nFailed) = 
   (0,               0,          0,          0       );

my ($s20, $s40, $s60, $s80, $s100, $s120, $s140, $sBig) =
   (0,    0,    0,    0,    0,     0,     0,     0    );

my $maxInsufficientReplicants = 0;

foreach (glob "$root/*") {
   tr[/][\\];
   next unless m/\.(com|exe)/i and -f;
   my @log = `type $_`;

   my ($replicated, $completed, $detected, $repaired) = 
      (1,           0,          0,         0        );
   my ($bestCOMsig, $bestEXEsig, $bestFileSig) =
      (0,           0,           0           );
   my ($bestCOMtype, $bestEXEtype, $bestFileType) =
      ('',           '',           ''           );
   my $nInsufficientReplicants = 0;
   foreach (@log) {
      $completed  = 1 if m/VANALYSIS COMPLETED/;
      $detected   = 1 if m/INFO: passed unit test/;
      $repaired   = 1 if m/INFO: passed unit test \(repaired\)/;
      $replicated = 0, $nInsufficientReplicants = $1 if m/ERR: Replicants count (%d+)/ or m/Insufficient replicants: (\d+)/;

      if (m/Sig\s+(\S+)\s+\S+\s+(\S)(\S)/) {
         if     ($2 eq 'C' && $1 < $bestCOMsig) {
                    $bestCOMsig   = $1;
                    $bestCOMtype  = $3;
         }elsif ($2 eq 'X' && $1  < $bestEXEsig) {
                    $bestEXEsig   = $1;
                    $bestEXEtype  = $3;
         }elsif ($2 eq 'F' && $1  < $bestFileSig) {
                    $bestFileSig  = $1;
                    $bestFileType = $3;
         }
      }
   }
   $maxInsufficientReplicants = max($maxInsufficientReplicants,$nInsufficientReplicants) unless $replicated;

   s/^(.*\\|)([^\\]+\.)(com|exe)\..*/$2$3   /i;
   print "$_\t";

   if     (!$completed) {
             print "*** DID NOT COMPLETE ***\n";
             next;
   }elsif (!$replicated) {
             print "bad replication ($nInsufficientReplicants)\n";
             $nNotReplicated++;
             next;
   }elsif ($detected) {
             print "detected". ($repaired ? "   repaired" : "           ");
             $nDetected++;
             $nRepaired++ if ($repaired);
   }else  {#failed
             print "FAILED\n";
             $nFailed++;
             next;
   }
   print "\tC$bestCOMtype $bestCOMsig"   if ($bestCOMsig  < 0); 
   print "\tX$bestEXEtype $bestEXEsig"   if ($bestEXEsig  < 0); 
   print "\tF$bestFileType $bestFileSig" if ($bestFileSig < 0); 
   print "\n";

   foreach ($bestCOMsig, $bestEXEsig, $bestFileSig) {
      $s20++  if ($_ <    0 and $_ >= -20);
      $s40++  if ($_ <  -20 and $_ >= -40);
      $s60++  if ($_ <  -40 and $_ >= -60);
      $s80++  if ($_ <  -60 and $_ >= -80);
      $s100++ if ($_ <  -80 and $_ >= -100);
      $s120++ if ($_ < -100 and $_ >= -120);
      $s140++ if ($_ < -120 and $_ >= -140);
      $sBig++ if ($_ < -140);
   }
}

print "\nMax. insufficient replicants: $maxInsufficientReplicants\n";

my $total = $nDetected + $nFailed + $nNotReplicated;
my $nReplicated = $total - $nNotReplicated;

print "\n   detected: $nDetected (".percent($nDetected,$total).
      "%)   repaired: $nRepaired (".percent($nRepaired,$total).
      "%)   failed: $nFailed (".percent($nFailed,$total).
      "%)   no repl: $nNotReplicated (".percent($nNotReplicated,$total)."%)\n";

print "\nOf successfully-replicating runs:";
print "\n   detected: $nDetected (".percent($nDetected,$nReplicated).
      "%)   repaired: $nRepaired (".percent($nRepaired,$nReplicated).
      "%)   failed: $nFailed (".percent($nFailed,$nReplicated)."%)\n";

print "\n--------------------------------------------------------------------\n";
print "scores\t|  -20\t|  -40\t|  -60\t|  -80\t| -100\t| -120\t| -140\t|...\n";
print "--------------------------------------------------------------------\n";
print "\t$s20\t$s40\t$s60\t$s80\t$s100\t$s120\t$s140\t$sBig  |\n";
print "--------------------------------------------------------------------\n";
while (1) {
   my $line = '';
   foreach ($s20, $s40, $s60, $s80, $s100, $s120, $s140, $sBig) {
      $line .= ($_-- > 0 ? "\t." : "\t ");
   }
   print "$line\n";
   last unless $line =~ m/\./;
}



sub percent ($$) {
   my ($n, $t) = @_;
   return "?" unless $t;
   return sprintf ("%4.2f",($n * 100) / $t);
}



sub max ($$) {
   return $_[0] > $_[1] ? $_[0] : $_[1];
}
