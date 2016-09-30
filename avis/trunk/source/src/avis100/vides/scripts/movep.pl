# static analyzer for vrep.pl compares some files
# not ready yet
use AVIS::Local;
use AVIS::FSTools;

my $isvarVLOG=0;
my $isfnVLOG="thelog.log";
my $dir1="D:/any_unc/4/post";
my $dir2="D:/any_unc/4/real";
my $goatbase = "MSDOS622EA";
my $dirbase = "D:/auto/worktemp/c1/p4713/extracted";
$dir1 = "$dirbase/$goatbase.real-time";
$dir2 = "$dirbase/$goatbase.afterwards";
my $dir3="D:/auto/data/vresources/goats/MSDOS622EA";

open ( LOGMSG, ">> $isfnVLOG" ) || die "Couldn't open log file: $isfnVLOG!\n";
if (LOGMSG) { $isvarVLOG = 1;
               logdate("Begin:");
}
##logmsg("Hello1\n");
##logmsg("Hello2\n",1);
###@d1 = get_files($dir1);  # collect files from after dir

###@d2 = get_files($dir2);  # collect files from realtime-extraction dir

##logmsg("== total files in $dir1 are $#d1\n",1);
##logmsg("== total files in $dir2 are $#d2\n",1);

check_real_post();

if ($isvarVLOG) { 
    logdate("End:");
    $isvarVLOG = 0;
    close(LOGMSG);
}


sub fnd_dups() {
}

sub check_real_post() {
# find equal files and remove them from post
# 
# 
 my $countfiles, $n;
 #$n = SameContent($dir1,$dir2,\$countfiles);
 @d1 = get_files($dir1); # collect files from real-time extraction dir
 @d2 = get_files($dir2); # collect files from afterwards dir
 foreach my $file (@d1) {

# $n = ProcessFiles($dir1,$dir2,$dir3,"D:/any_unc/real/bc.com",\$countfiles);
  $n = ProcessFiles($dir1,$dir2,$dir3,$file,\$countfiles);
 }
 #logmsg("count: \$n $n \$countfiles $countfiles\n",1);
 logmsg("================next part\n",1);
 foreach my $file (@d2) {
    $n = ProcessFiles($dir1,$dir2,$dir3,$file,\$countfiles);
 }


}

sub check_real_org() {
}

sub check_post_org() {
# analyze left files in the post extraction directory
# generates SA flag on the samples.lst
#
# try to remove them
# try to move them to $resultdir
#
# foreach file in $postdir
#  do cmp with $orgdir
#     if ($postdirfile equ $orgdirfile) {
#        SA = 0; ES = 0; Erase($postdirfile);
#     }
#     else {
#         SA = 1; ES = 0; move file to $resultdir;
#         update entry;
#
#     } 
}

sub update_sample_lst() {
#
}

sub SameContent
{
  my ($refdir, $chkdir, $refn) = @_;
  my $rv = 1;
  my $n = 0;
  my $base2;

  my @tocheck = glob("$chkdir\\*");
     @tocheck = (), print("NO REPLICANTS!\n") if $tocheck[0] eq "$chkdir\\*";
  my $cf = $#tocheck +1;
  logmsg("count files in $chkdir: $cf\n",1);
  foreach my $file (@tocheck) {
    $n++;
    my $base = AVIS::FSTools::BaseName($file);
    if ($base =~ m/\....\.(\d)+/) { # match something like bc.com.2
        $base2 = $base;
        $base  =~ s/\.(\d+)//;      # remove .2 at the end
    }
    my $reffile = "$refdir\\$base";
    if (-e $reffile) {
      unless (Equals($file, $reffile)) {
	    print "Files $file and $reffile are different\n";
       logmsg("$base $file $reffile\n",1);
	     if (-s $file > -s $reffile) { # Check against original here!
	        print "Real-time greater than afterwards -> Keeping real-time\n";
	     }
	     else {
	        print "Afterwards greater then real-time, may be multiply-infected...\n";
	        $rv = 0;
	     }
      }
    }
    else {
         warn "No reference file! $reffile does not exist!\n";
         #$rv = 0;
    }
    
  }

  $$refn = $n if ref($refn) eq "SCALAR";
  return $rv;
}



sub get_files($;) {

  my ($dir, $refn_file_array) = @_;

  my @tocheck = glob("$dir\\*");
#     @tocheck = (), print("NO REPLICANTS!\n") if $tocheck[0] eq "$chkdir\\*";
  my $cf = $#tocheck +1;
  
  logmsg("count files in $dir: $cf\n",0);
  $cf--;
  my $lastelement = $tocheck->[0];

  logmsg("last: $lastelement\n",0);

  foreach my $file (@tocheck) {
    $n++;                           # count files
    my $base = AVIS::FSTools::BaseName($file);
    if ($base =~ m/\....\.(\d)+/) { # match something like bc.com.2
        $base2 = $base;
        $base2 =~ s/\.(\d+)//;      # remove .2 at the end
    }
    if ($base) {
        logmsg("$n $dir $base2 $base\n",0);
        $base2 = "";
    }
    else {
        logmsg("$n $dir $base\n",0);
    }
  }
  
  return @tocheck;
}

sub ProcessFiles
{
  # $refdir is the post-extraction directory
  # $chkdir is the realtime-extraction directory
  # $orgdir is the goat directory
  my ($chkdir,$isdnPostdir,  $isdnOrigdir, $filename, $refn) = @_;
  my $rv = 1;
  my $n = 0;
  my $base2;
  my $uneq_file2ref = 0;
  my ($isvarFile_fsize,$isvarPost_fsize,$isvarOrig_fsize);


  my @tocheck = glob("$chkdir/*");
     @tocheck = (), print("NO REPLICANTS!\n") if $tocheck[0] eq "$chkdir/*";
  my $cf = $#tocheck +1;
  my ($isvarFile_exist) = 0;
  
  $isvarFile_exist = 1 if (-e $filename);
  
  #logmsg("PF: $refdir, $chkdir, $orgdir, $filename, $refn\n",1);
  #logmsg("count files in $chkdir: $cf\n",1);
#  foreach my $file (@tocheck) {
    $filename =~ s|\\|/|g;
    $file = $filename;
    #$filename = "$chkdir/$file"; 
{    $n++;                   

    my $base = AVIS::FSTools::BaseName($filename);
    if ($base =~ m/\....\.(\d)+/) { # match something like bc.com.2
        $base2 = $base;
        $base  =~ s/\.(\d+)//;      # remove .2 at the end
    }
    my $isfnPostfile = "$isdnPostdir/$base";
    my $isfnOrigfile = "$isdnOrigdir/$base";
    
    my ($isvarPost_exist,$isvarOrig_exist,$eq_fr,$eq_fo,$eq_ro) = (0,0,0,0,0);
    $isvarFile_exist = 1 if (-e $filename);
    $isvarPost_exist = 1 if (-e $isfnPostfile); # post reference file exists
    $isvarOrig_exist = 1 if (-e $isfnOrigfile); # org goat reference file exists
    if ($isvarPost_exist && $isvarFile_exist && ($file ne $isfnPostfile)) {
        $eq_fr   = 1 if (Equals($file, $isfnPostfile)); # is realtime working, should be 1
    }
    if ($isvarOrig_exist && $isvarFile_exist && ($file ne $isfnOrigfile)) {
        $eq_fo   = 1 if (Equals($file, $isfnOrigfile)); # Static Analyse:
    }
    if ($isvarOrig_exist && $isvarPost_exist && ($isfnPostfile ne $isfnOrigfile)) {
        $eq_ro   = 1 if (Equals($isfnPostfile, $isfnOrigfile)); # is there a diff between post and org
    }
    $isvarFile_fsize = 0;
    $isvarPost_fsize = 0;
    $isvarOrig_fsize = 0;
    
    $isvarFile_fsize = ((stat($file))    [7])      if($isvarFile_exist);  # get filesize of realtime-extraction file
    $isvarPost_fsize = ((stat($isfnPostfile)) [7]) if($isvarPost_exist);  # get filesize
    $isvarOrig_fsize = ((stat($isfnOrigfile)) [7]) if($isvarOrig_exist);  # get filesize
    
    # get virus size
    $isvarDiff_fsize = $isvarFile_fsize - $isvarOrig_fsize;
    
    #printf("$filename\t$rfexist,$ofexist|fp:%s,fo:%s,po:%s ",$eq_fr?"eq":"df",$eq_fo?"eq":"df",$eq_ro?"eq":"df",1);
    #logmsg("$f_fsize $f_rsize $f_osize $dif_fsize\n",1);
    if ($eq_fo eq 0 || $eq_ro eq 0) { # if sample is different from original
     logmsg("$filename\n",1);
     logmsg("$isvarFile_exist,$isvarPost_exist,$isvarOrig_exist|fp:$eq_fr,fo:$eq_fo,po:$eq_ro| ",1);
     logmsg("$isvarFile_fsize,$isvarPost_fsize,$isvarOrig_fsize,$isvarDiff_fsize\n",1);
     
     #logmsg("$filename\n$rfexist,$ofexist|fp:$eq_fr,fo:$eq_fo,po:$eq_ro ",1);
     #logmsg("$f_fsize, $f_rsize, $f_osize, $dif_fsize\n",1);
    }
#    if (-e $reffile) {
#      unless (Equals($file, $reffile)) {
#	    print "Files $file and $reffile are different\n";
#       logmsg("$base $file $reffile are different\n",1);
#       $uneq_file2ref = 1;

#	     if (-s $file > -s $reffile) { # Check against original here!
#	        print "Real-time greater than afterwards -> Keeping real-time\n";
#	     }
#	     else {
#	        print "Afterwards greater then real-time, may be multiply-infected...\n";
#	        $rv = 0;
#	     }
#      }
#    }
#    else {
#         warn "No reference file! $reffile does not exist!\n";
#         #$rv = 0;
#    }


    
  }

#  printf "=real and post dif %d\n",$uneq_file2ref;
  $$refn = $n if ref($refn) eq "SCALAR";
  return $rv;
}



sub logdate(;) {
   my ($outstring) = @_;
   $outstring .= sprintf(" %s\n",Date2Jap());
   logmsg("$outstring",1);
}


sub Equals
{
  my ($f1, $f2) = @_;
  my $rc;
  # $isfnCompare defined in local.pm
  #  my $isfnCompare	= "$isdnBinDir/compare";

#  logmsg("001 rc $rc f1: $f1 f2: $f2\n",1);
  if (-s $f1 > -s $f2){ 
        $rc = 1;
  }
  elsif (-s $f1 < -s $f2) { $rc = 1; }
  else {
    $rc = system "$isfnCompare $f1 $f2 >nul";
#    logmsg("002 rc $rc f1: $f1 f2: $f2\n",1);

  }
#  logmsg("003 rc $rc f1: $f1 f2: $f2\n",1);
  #print "Compareing $f1 $f2\n";
  #$rc = system "$isfnCompare $f1 $f2";
  return $rc?0:1;
}


sub logmsg($;) {
    # log a message to the logfile (and to the screen)
    # $message -- message to be printed
    # $verbose -- optional print $message to stdout if $verbose eq 1
    # use: &log("text to print") or &log("text to print",1)
    # notice that the logfile had to be opened before

    my( $message, $verbose ) = @_;
    
    if ($isvarVLOG eq 1) {
       print LOGMSG $message;
    }
    print $message if $verbose;

}



