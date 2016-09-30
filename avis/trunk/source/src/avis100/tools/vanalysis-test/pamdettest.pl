$pgm = "PAMdetTestSample.com";

for ($l = 3000; $l < 6000; $l+=100) {
   $inZone = 0;
   $zoneStart = $zoneEnd = 0;
   for ($r = -1024; !$zoneEnd; $r+=1) {
      if (isFound($l,$r)) {
         if (!$inZone) {
            $inZone = 1;
            $zoneStart = $r;
         }
      }else {
         if ($inZone) {
            $inZone = 0;
            $zoneEnd = $r;
         }
      }
      $startSign = $zoneStart ? ($zoneStart < 0 ? '-' : '+') : ' ';
      $endSign = $zoneEnd ? ($zoneEnd < 0 ? '-' : '+') : ' ';
      printf("%5lx\t%4lx $startSign%3lx\t%4lx $endSign%3lx\t%3lx\r", 
             $l, $l+$zoneStart, abs($zoneStart), $l+$zoneEnd, abs($zoneEnd), $zoneEnd-$zoneStart);
   }
   print "\n";
}
unlink $pgm;



sub isFound {
   my ($l, $r) = @_;
   
   unlink $pgm;
   return 1 if `PAMdetTest $pgm $l $r`;   # sig overlaps code
   
   @list = `F:\\NAVdetect\\tools\\deftest $pgm`;
   $list[1] =~ /(clean|infected)/;
   printf((' 'x60)."%5ld $1   \r",$r);
   return $1 eq 'infected';
}
