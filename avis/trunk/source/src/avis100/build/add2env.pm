#!perl
# add2env.pm
# $Header: /Build/add2env.pm 1     7/17/98 7:37p Stefan $
#
# modifies current win32 environment by modifying the registry
# creates or appends new values to existing environment variables
# call with Add2LocalEnv
# adds or appends values to new or existing environment variables
# Add2LocalEnv(variable,content);
# Add2LocalEnv("PATH","O:\\PERL\\BIN"); # adds to the PATH variable
# Add2LocalEnv("PERL5LIB","O:\\PERL\\LIB");
#
# V1.07c 3:20 PM 7/16/98
# uses WIN32::Registry
# field seperator is ";"
# 
#
#$History: add2env.pm $
#
#*****************  Version 1  *****************
#User: Stefan       Date: 7/17/98    Time: 7:37p
#Created in $/Build
#modify environment varibales in win32 registry, create or append data
#
#
#*****************  Version 1  *****************
#User: Tode	      Date: 7/15/98    Time: 02:00p
#Created in $/Build
#
#


package add2env;
require Exporter;
@ISA = qw(Exporter);
@EXPORT = qw(add2path exactmatch Add2LocalPath Add2LocalEnv);
@EXPORT_OK = @EXPORT;

use Win32::Registry;
use strict;

1; # OK to load module


sub Add2LocalEnv {
# call with Add2LocalEnv
# adds or appends values to new or existing environment variables
# Add2LocalEnv(variable,content);
# Add2LocalEnv("PERL5LIB","O:\\PERL\\LIB");
# Add2LocalEnv("PATH","O:\\PERL\\BIN"); # adds to the PATH variable

my($envvar,$newcontent) = @_;
my ($CurCtrlKey,$retval,$subkey,$pc_org_var,$retpath,$no_append,$SysEnv,$retsubval);

$retpath = "";
$no_append = 0;

$CurCtrlKey = "SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment";
$subkey = $envvar;   # which key to retrieve

$main::HKEY_LOCAL_MACHINE->Open($CurCtrlKey, $SysEnv) || die "Error on Open: $!";

if (!($SysEnv->QueryValueEx($subkey, REG_SZ, $retval))) { 
   # if its not here, we get here,
   # create Environment variable and set the value of it
   $retval = $newcontent;
   $SysEnv->SetValueEx($subkey, 0, REG_EXPAND_SZ, $retval) || warn "$!"; # add/update it
}

$pc_org_var = $retval;

# compare both strings, 
# if 0 update variable
# else keep variable / no changes

$no_append = add2env::add2path($pc_org_var,$newcontent);

if ($no_append == 0) { # put newpath in front of old
    $retval = "$newcontent;$pc_org_var";
    $SysEnv->SetValueEx($subkey, 0, REG_EXPAND_SZ, $retval) || warn "$!";
    $retsubval = 1;  # it worked
}
else {  # elsif($no_append == 1) { # no change
    $retval = $pc_org_var;
    $retsubval = 0;  # it didn't work
}
$SysEnv->Close();

#print "New $envvar=$retval\n";
return($retsubval);
}

sub Add2LocalPath {
# Add2LocalPath(newpath);
# Append to local PATH
#
my($newperlpath) = @_;
my ($CurCtrlKey,$subkey,$pc_org_path,$retpath,$no_append,$SysEnv);

$retpath = "";
$no_append = 0;

$CurCtrlKey = "SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment";
$subkey = "PATH";   # which key to retrieve

$main::HKEY_LOCAL_MACHINE->Open($CurCtrlKey, $SysEnv) || die "Error on Open: $!";
$SysEnv->QueryValueEx($subkey, REG_SZ, $pc_org_path) || warn "$!"; # get subkeys

#print "Org: PATH=$pc_org_path\n";


$no_append = add2env::add2path($pc_org_path,$newperlpath);

if ($no_append == 0) { # put newpath in front of old PREPEND
    $retpath = "$newperlpath;$pc_org_path";
    $SysEnv->SetValueEx($subkey, 0, REG_EXPAND_SZ, $retpath) || warn "$!";
}
else {  # elsif($no_append == 1) { # no change
    $retpath = $pc_org_path;
}
$SysEnv->Close();

#print "New: $subkey=$retpath\n";

}

sub add2path {
   
   my ($orgpath,$newpath) = @_;    
   my ($i,$p0,$p1,$splitchar,@list);
   my ($no_update) = 0;
   
   
   $orgpath =~ tr/a-z/A-Z/;   # translate to uppercase (only temporary)
   $newpath =~ tr/a-z/A-Z/;
   # INPUT
   # add2path("C:\\DOS","D:\\PERL")
   # parameter one is original path
   # parameter two is new path (the addon)
   # OUTPUT
   # returns new path (String)
   
   
   $splitchar = ';';      # fieldseperator for pathvariables
   @list = split($splitchar,$orgpath); # split string
   $no_update = 0;
   
   # now for each string in @list, check if $newpath is in there
   for ($i=0; $i<=$#list; $i++){ 
     $p0 = exactmatch($newpath,$list[$i],$i); 
     $p1 = exactmatch($list[$i],$newpath,$i);
     if ($p0 == 1 || $p1 == 1) { # if there was an exactmatch 
#        print "exact match ";
#        print "p0: $p0 p1: $p1\n";
        $no_update = 1;
     }
   }
   ##$retpath = "$newpath;$orgpath" if $no_update == 0; # put newpath in front of old
   ##$retpath = $orgpath if $no_update == 1;            # return only oldpath
   ##return ($retpath);
   
   return ($no_update);
}



sub exactmatch {
   # INPUT
   # exactmatch($string1,$string2,$number)
   # compares $string1 and $string2
   # OUTPUT
   # returns 1 if exactmatch of INPUT strings
   # returns -1 if no exactmatch
   my ( $fstr, $lookfor ,$direction ) = @_;
   my ( $pos) = -1;
   my ( $retval ) = $pos;  # return value
   my ( $s1 , $s2);

   while(($pos = index($fstr, $lookfor, $pos)) > -1) {
      $s1 = length($fstr);
      $s2 = length($lookfor);
      #print "Found at $pos $direction s1 $s1 s2 $s2\n";
      ####$retval=$pos;
      if ($s1 == $s2 && $pos == 0) {
         # if the stringlength is equal ans the matching position
         # is in the beginning, return 1 
         # else -1 (no exactmatch)
         $retval = 1;                                        
      }
      $pos++;
   }
   return ($retval);
}

1;

