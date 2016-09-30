#!/usr/bin/perl

use autocommon;
&force_fflush();

if (scalar @ARGV < 2) { die "*** Syntax: $0 <src directory> <dst directory>\n*** ABORTING\n" }

$codosource = $ARGV[0];
$cododest   = $ARGV[1];
$counter    = 0;

for $filename (<$codosource/*.com $codosource/*.exe>)
{
  if(-f $filename)
  {
    $counter++;
    system_check0($temppath,"$codoexe", "-f $filename -b -wow 1 -nodebug -timeout $codo_timeout","CHECK_RETURN");
    &copytodir("$filename.sections", $cododest) if ($codosource ne $cododest);
  }
}

unless ($counter) 
{ 
     print "[$0: no files processed]\n";
}

