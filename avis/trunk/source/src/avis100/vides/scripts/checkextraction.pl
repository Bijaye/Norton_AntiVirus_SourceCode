use CheckExtraction;

die "Usage: $0 <dir1> <dir2>\n" unless $ARGV[0] && $ARGV[1];

if (SameContent($ARGV[0], $ARGV[1])) {
  print "Proper subset\n";
}
else {
  print "Tssss... not good!\n";
}
