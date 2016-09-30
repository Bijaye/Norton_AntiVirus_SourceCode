use VidesBoot;
use AVIScore::FSTools;

($srcdir, $destdir) = @ARGV;

die "No source directory" unless -d $srcdir;

$destdir = $srcdir . ".output" unless $destdir;

FSTools::wipe($destdir) unless length($destdir) < 5;
FSTools::mkdirsafe($destdir);

foreach $file (glob("$srcdir\\*")) {

  print "-" x 60, "\n";
  print "Dealing with $file\n";
  print "-" x 60, "\n";

  if ($file =~ /.*(\\.*)\./) {
    $dest = $1;
  }
  else {
    $dest = $file;
  }

  VidesBoot::Replicate($file, $destdir.$dest) or warn "NOT GOOD!!!";
}
