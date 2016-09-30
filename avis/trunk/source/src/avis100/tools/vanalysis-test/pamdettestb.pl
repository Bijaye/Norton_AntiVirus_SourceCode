$pgm = "PAMdetTestSample.com";

###for ($l = 2500; $l < 8100; $l+=100) {
for ($l = 0x11C0; $l < 0x1211; $l+=1) {
   $zoneStart = binSearch ($l,-1024,3,'begin');
   $zoneEnd   = binSearch ($l,3,2000,'end');
      
   $startSign = signof($zoneStart);
   $endSign   = signof($zoneEnd);
   printf("%5lx\t%4lx $startSign%3lx\t%4lx $endSign%3lx\t%3lx\n", 
          $l, $l+$zoneStart, abs($zoneStart), $l+$zoneEnd, abs($zoneEnd), $zoneEnd-$zoneStart);
}
unlink $pgm;



sub binSearch {
   my ($l, $first,$last, $cond) = @_;
   die "search failed at $first, $last, $cond\n" if $last < $first;
   my $t = int (($first + $last)/2);
   $result = test($l, $t, $cond);
   if    ($result < 0) {return binSearch($l,$t+1,$last,$cond);}
   elsif ($result > 0) {return binSearch($l,$first,$t-1,$cond);}
   else                {return $t};
}



sub test {
   my ($l, $t, $cond) = @_;
   return ($cond eq 'begin' ? -1 : 1) unless isFound($l,$t);
   if ($cond eq 'begin') {return isFound($l,$t-1) ?  1 : 0;}
   else                  {return isFound($l,$t+1) ? -1 : 0;}
}



sub isFound {
   my ($l, $r) = @_;
   
   unlink $pgm;
   return 1 if `PAMdetTest $pgm $l $r`;   # sig overlaps code
   
   @list = `F:\\NAVdetect\\tools\\deftest $pgm`;
   $list[1] =~ /(clean|infected)/;
   printf((' 'x60)."%5ld $1   \r",$r);
   return $1 eq 'infected';
}



sub signof {
   my ($n) = @_;
   return $n ? ($n < 0 ? '-' : '+') : ' ';
}
