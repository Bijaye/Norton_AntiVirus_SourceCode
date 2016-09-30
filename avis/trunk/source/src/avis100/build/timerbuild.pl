####This program reads the current "level.txt" file and enters into a loop; calls the build
####batch file every one hour with an updated level.

chdir "d:\\build_~1";
$bldbatch = "d:\\test_b~1\\isrebuildall.bat";
$levelfile = "d:\\build_~1\\level.txt";
if (-e "$levelfile") {
	open (LEVEL, "< $levelfile") or die "unable to open the level.txt file for reading\n";
	$CurLevel=<LEVEL>;
	close (LEVEL);
	chomp ($CurLevel);
	print "$CurLevel\n";
	
} else {
	print "unable to find $levelfile file , aborting\n";
	exit(0);
}

while (1) {

	$CurLevel=$CurLevel+1;

	if ($CurLevel <10) {
		$level = "0"."0"."$CurLevel";
	} elsif ($CurLevel <100){
		$level = "0"."$CurLevel";
	} else {
		$level = "$CurLevel";
	}

	open (LEVEL, "> $levelfile") or die "unable to open the level.txt file for writing\n";
	print LEVEL $level;
	close (LEVEL);

	######do build here
	system ("$bldbatch $level");
	sleep(3600);

}  ##end while
