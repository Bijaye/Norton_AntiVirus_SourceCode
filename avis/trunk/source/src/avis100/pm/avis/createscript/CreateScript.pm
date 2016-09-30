####################################################################
#                                                                  #
# Program name:  pm/AVIS/CreateScript/CreateScript.pm              #
#                                                                  #
# Package name:  AVIS::CreateScript                                #
#                                                                  #
# Description:   Creates command scripts for VIDES2                #
#                                                                  #
#                                                                  #
#                                                                  #
# Statement:     Licensed Materials - Property of IBM              #
#                (c) 1998-1999                                     #
#                                                                  #
# Author:        Stefan Tode                                       #
#                                                                  #
#                U.S. Government Users Restricted Rights - use,    #
#                duplication or disclosure restricted by GSA ADP   #
#                Schedule Contract with IBM Corp.                  #
#                                                                  #
#                                                                  #
####################################################################
#                                                                  #
# Function:                                                        #
#                                                                  #
# Notes:                                                           #
#                                                                  #
#                                                                  #
#                                                                  #
#                                                                  #
#                                                                  #
#                                                                  #
####################################################################

# Thoughout the script TSR designates Terminate Stay Resident
# DA designates Direct Action

package AVIS::CreateScript;
require Exporter;
@ISA = qw(Exporter);
@EXPORT = qw(CreateScript);
@EXPORT_OK = qw();

use strict;

my $cpid = "A";
srand;

1;

sub CreateScript
{
  my ($listfile, $scriptfile, $samplename) = @_;
  my (@COMs, @EXEs, @others, $i, @orderedlist);
  my ($n,$nc,$ne,$no) = (0,0,0,0);
  my ($copyletter) = "A";

  $samplename = "sample" unless $samplename;

  open(L, $listfile) or die "Can't open $listfile: $!";
  while (<L>) {
    chomp;
    $_ = uc;
    next if /^$/;
    $n++;
    if (/COM$/) {
      push @COMs, $_;
      $nc++;
    }
    elsif (/EXE$/) {
      push @EXEs, $_;
      $ne++;
    }
    else {
      push @others, $_;
      $no++;
    }
  }
  close(L);

#DEBUG
#  print "@COMs\n@EXEs\n@others\n";

  while (@COMs > 0 or @EXEs > 0 or @others > 0) {
    my $item;
    push @orderedlist, $item if ($item = pop @COMs);
    push @orderedlist, $item if ($item = pop @EXEs);
    push @orderedlist, $item if ($item = pop @others);
  }

  die "Script Error!" unless @orderedlist == $n;

#DEBUG
#  print "@orderedlist\n";

  open(S, ">".$scriptfile) or die "Can't create $scriptfile: $!";
  print S $samplename, "\n";
  print S "dir\n";
  print S "dir *.com\n";
  print S "dir *.exe\n";
  for ($i = 0; $i < $n/2; $i++) {
    print S lc ApplyMethod(shift(@orderedlist), 1);
  }
  for (; $i < $n ; $i++) {
    $copyletter++;
    print S lc ApplyMethod(shift(@orderedlist), 2);
  }
  print S "stop\n";
  close(S);
}

sub ApplyMethod
{
  my ($file,$method,$subdir) = @_;
  my $rv;
  if ($method == 1) {
    $rv = TSR_Exec($file);
  }
  elsif ($method == 2) {
    $rv = TSR_OpenClose($file);
  }
  elsif ($method == 3) {
    $rv = TSR_Create($file);
  }
  else {
    $rv = DA_Subdir($file,$subdir);
  }
  return $rv;
}

sub TSR_Exec
{
  my $file = shift;
  return $file . "\n";
}

sub TSR_OpenClose
{
  my $file = shift;
  return "opcl $file\n";
}

sub TSR_Create
{
  my $file = shift;
  return "copy $file " . GetRndName() . "\n";
}

sub DA_Subdir
{
  my ($file,$subdir) = @_;
  return "copy $file " . $subdir . "\\" . GetRndName() . "\n";
}

sub GetRndName
{
  my $extension = shift;
  $extension = "COM" unless $extension;

  my $id = substr ("THEQUICKBROWNFOXJUMPSOVERTHELAZYDOG", rand(35), 1+rand(3));
  return "D" . $id . $cpid++ . "E." . $extension;
}
