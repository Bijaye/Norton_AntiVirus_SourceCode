####################################################################
#                                                                  #
# Package name:  AVIS::Macro::Setup                                #
#                                                                  #
# Description:                                                     #
#                                                                  #
#                                                                  #
# Statement:     Licensed Materials - Property of IBM              #
#                (c) Copyright IBM Corp. 1998-1999                 #
#                                                                  #
# Author:        Jean-Michel Boulay, Morton Swimmer                #
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
# The function of this code is to prepare the run of RC by         #
# selecting the environment and the modifications to apply to it:  #
# -image(s)                                                        #
# -virtual drives                                                  #
# -goats                                                           #
# -files (executables, DLLs and so on) needed in the image         #
#                                                                  #
####################################################################
package AVIS::Macro::Setup;


require Exporter;
@ISA = qw(Exporter);
@EXPORT = qw();
@EXPORT_OK = qw();

$VERSION = 2.0;


use strict;
use AVIS::Emulator::Setup::Drives;
use AVIS::Emulator::Setup::VirtualDrive;

#########################################################
#
# Global variables
#

my $lasterror = '';
my $filestopdir = '/data/Avis/Macro/Files/Config/Applications';

my @SBCSLanguages = ('Brazilian', 'Dutch', 'English', 'French', 'German', 'Italian', 'Spanish');
my @DBCSLanguages = ('Chinese', 'Japanese', 'Taiwanese');

my %variables = ();


########################################################
#
# Error messages
#

sub SetLastError($) {
  return ($lasterror = shift);
}

sub GetLastError() {
  return $lasterror;
}


#########################################################
#
# Image stuff
#

sub SelectImage($$$$$) {
my ($application, $release, $nls, $build, $emulator) = @_;
  
  if ($emulator eq 'SoftWindows95') {
    return 'W95usO95usO97us.hdf' if ($application eq 'word' and $release eq '7' and grep /^$nls$/, @SBCSLanguages);
    return 'W95jpO95jp.hdf'      if ($application eq 'word' and $release eq '7' and grep /^$nls$/, @DBCSLanguages);
    return 'W95usO95usO97us.hdf' if ($application eq 'word' and $release eq '8' and grep /^$nls$/, @SBCSLanguages);
    return 'W95usO95usO97us.hdf' if ($application eq 'excel' and $release eq '7' and $nls eq 'English');
    return 'W95usO95usO97us.hdf' if ($application eq 'excel' and $release eq '8' and $nls eq 'English');
    return 'W95usO97us.hdf' if ($application eq 'powerpoint' and $release eq '8' and $nls eq 'English');
    
    my $msg = "[$application; $release; $nls; $build] is not a supported environment on [$emulator].";
    warn $msg; SetLastError($msg);
  }
  else {
    my $msg = "[$emulator] is not a supported emulator.";
    warn $msg; SetLastError($msg);
  }
  return undef;
}

sub SelectDriveSet($$$$$$) {
my ($application, $release, $nls, $build, $emulator, $driveset) = @_;  
  
  if ($emulator eq 'SoftWindows95') {
    my $osimage = SelectImage($application, $release, $nls, $build, $emulator);
    
    if (not defined $osimage) {
      my $msg = "[$application; $release; $nls; $build] is not a supported environment on [$emulator].";
      warn $msg; SetLastError($msg);
      return undef;
    }
    if ($application eq 'word' and $release eq '8' and grep /^$nls$/, @SBCSLanguages) {
      my $vcdrive = new AVIS::Emulator::Setup::VirtualDrive('flatimage',
                                                             $osimage,
                                                             'C', 
                                                             $AVIS::Emulator::Setup::VirtualDrive::osdrive|$AVIS::Emulator::Setup::VirtualDrive::appdrive|$AVIS::Emulator::Setup::VirtualDrive::filedrive
                                                             );
      $driveset->Add($vcdrive);
    }
    else {
      my $vcdrive = new AVIS::Emulator::Setup::VirtualDrive('flatimage',
                                                             $osimage,
                                                             'C', 
                                                             $AVIS::Emulator::Setup::VirtualDrive::osdrive|$AVIS::Emulator::Setup::VirtualDrive::appdrive
                                                             );
      $driveset->Add($vcdrive);
      my $vndrive = new AVIS::Emulator::Setup::VirtualDrive('fsadrive',
                                                             undef,
                                                             'N', 
                                                             $AVIS::Emulator::Setup::VirtualDrive::filedrive                                                             
                                                             );
      $driveset->Add($vndrive);
    }                    
    return $driveset;
  }
  else {
    my $msg = "[$emulator] is not a supported emulator.";
    warn $msg; SetLastError($msg);
  }
  return undef;
}


#########################################################
#
# things depending on the image
#
#

sub GetAppRootDir($$) {
my ($application, $release) = @_;

  return 'c:\\MSOffice' if ($application eq 'word' and $release eq '7');
  return 'c:\\Program Files\\Microsoft Office' if ($application eq 'word' and $release eq '8');
  return 'c:\\MSOffice' if ($application eq 'excel' and $release eq '7');
  return 'c:\\Program Files\\Microsoft Office' if ($application eq 'excel' and $release eq '8');
  return 'c:\\Program Files\\Microsoft Office' if ($application eq 'powerpoint' and $release eq '8');

  return undef;
}

sub GetAppExecutableDir($$) {
my ($application, $release) = @_;

  my $approot = GetAppRootDir($application, $release);
  return $approot.'\\Winword' if ($application eq 'word' and $release eq '7');
  return $approot.'\\Office' if ($application eq 'word' and $release eq '8');
  return $approot.'\\Excel' if ($application eq 'excel' and $release eq '7');
  return $approot.'\\Office' if ($application eq 'excel' and $release eq '8');
  return $approot.'\\Office' if ($application eq 'powerpoint' and $release eq '8');

  return undef;
}

sub GetAppExecutablePath($$) {
my ($application, $release) = @_;

  my $appdir = GetAppExecutableDir($application, $release);
  return $appdir.'\\Winword.exe' if ($application eq 'word' and $release eq '7');
  return $appdir.'\\Winword.exe' if ($application eq 'word' and $release eq '8');
  return $appdir.'\\Excel.exe' if ($application eq 'excel' and $release eq '7');
  return $appdir.'\\Excel.exe' if ($application eq 'excel' and $release eq '8');
  return $appdir.'\\Powerpoint.exe' if ($application eq 'powerpoint' and $release eq '8');

  return undef;
}

sub GetAppTemplateDir($$) {
my ($application, $release) = @_;
  
  my $approot = GetAppRootDir($application, $release);  
  return $approot.'\\Templates' if ($application eq 'word' and $release eq '7');
  return $approot.'\\Templates' if ($application eq 'word' and $release eq '8');
  return $approot.'\\Templates' if ($application eq 'excel' and $release eq '7');
  return $approot.'\\Templates' if ($application eq 'excel' and $release eq '8');
  return $approot.'\\Templates' if ($application eq 'powerpoint' and $release eq '8');

  return undef;
}

sub GetAppStartupDir($$) {
my ($application, $release) = @_;

  my $approot = GetAppRootDir($application, $release);
  return $approot.'\\Startup' if ($application eq 'word' and $release eq '7');
  return $approot.'\\Startup' if ($application eq 'word' and $release eq '8');
  return $approot.'\\Excel\\Xlstart' if ($application eq 'excel' and $release eq '7');
  return $approot.'\\Office\\Xlstart' if ($application eq 'excel' and $release eq '8');
  return $approot.'\\Startup' if ($application eq 'powerpoint' and $release eq '8');

  return undef;
}

sub GetStartupFilePath($) {
  my $driveset = shift;
  my $image = $driveset->GetDriveSubtype('c');

  return 'c:\\session.bat' if $image eq 'W95usO95usO97us.hdf';
  return 'c:\\session.bat' if $image eq 'W95jpO95jp.hdf';
  return 'c:\\windows\\start menu\\programs\\startup' if $image eq 'W95usO97us.hdf';

  return undef;
}

#################################################
#
# stuff to copy to the image
#

sub GetAppFilesToCopy($$$$) {
my ($application, $release, $nls, $build) = @_;

my %dlllangcodes = ('Brazilian' => 'ptb',
                     'Dutch'   => 'nld',
                     'English' => 'enu',
                     'French'  => 'fra',
                     'German'  => 'deu',
                     'Italian' => 'ita',
                     'Spanish' => 'esp'
                      );

  my $filestocopy = [];
  my $srcdir;
  $srcdir = "$filestopdir/Word/$release.0/B1/nls/$nls/Files" if ($application eq 'word');
  $srcdir = "$filestopdir/Excel/$release.0/B1/nls/$nls/Files" if ($application eq 'excel');
  my $appdir = GetAppExecutableDir($application, $release);
  my @filenames;
  if (($application eq 'word') && ($release eq '7')) {
    @filenames = ('Winword.exe', 'Wwintl32.dll', 'wb70en32.tlb');
  }
  elsif (($application eq 'word') && ($release eq '8')) {
    my $dlllangcode = $dlllangcodes{$nls};
    @filenames = ('Winword.exe', 'Wwintl32.dll', 'Mso97.dll', "Mso7$dlllangcode.dll");
  }
  elsif (($application eq 'excel') && ($release eq '7') && ($nls eq 'English')) {
    @filenames = ('Excel.exe', 'Xlintl32.dll', 'Mso95.dll', 'Xl5en32.olb');
  }
  elsif (($application eq 'excel') && ($release eq '8') && ($nls eq 'English')) {
    my $dlllangcode = $dlllangcodes{$nls};
    @filenames = ('Excel.exe', 'Xlintl32.dll', 'Mso97.dll', "Mso7$dlllangcode.dll");
  }
  elsif (($application eq 'powerpoint') && ($release eq '8') && ($nls eq 'English')) {
    my $dlllangcode = $dlllangcodes{$nls};
    @filenames = ();
  }
  else {
    #file a complaint
    warn 'horrible error';
    return undef;
  }
  
  foreach (@filenames) {
    my ($srcfile, $destfile) = ($srcdir."/$_", "$appdir\\$_");
    push @$filestocopy, { 'src' => $srcfile, 'dest' => $destfile};
  }
  return $filestocopy;
}


#################################################################
#
# goats and stuff
#

sub GetGoatDir($$$$) {
my ($application, $release, $nls, $build) = @_;

  return "$filestopdir/Word/$release.0/B1/Goats/Western" if ($application eq 'word' and grep /^$nls$/, @SBCSLanguages);
  return "$filestopdir/Word/$release.0/B1/Goats/Chinese" if ($application eq 'word' and $release eq '7' and $nls eq 'Chinese');
  return "$filestopdir/Word/$release.0/B1/Goats/Japanese" if ($application eq 'word' and $release eq '7' and $nls eq 'Japanese');
  return "$filestopdir/Word/$release.0/B1/Goats/Taiwanese" if ($application eq 'word' and $release eq '7' and $nls eq 'Taiwanese');
  return "$filestopdir/Excel/$release.0/B1/Goats/Western" if ($application eq 'excel' and $nls eq 'English');
  return "$filestopdir/Powerpoint/$release.0/B1/Goats/Western" if ($application eq 'powerpoint' and $nls eq 'English');
  return undef;
}

sub GetGoatList($$$$) {
my ($application, $release, $nls, $build) = @_;
  
  return "$filestopdir/Word/$release.0/B1/goats.lst" if $application eq 'word';
  return "$filestopdir/Excel/$release.0/B1/goats.lst" if $application eq 'excel';
  return "$filestopdir/Powerpoint/$release.0/B1/goats.lst" if $application eq 'powerpoint';
  return undef;
}

###########################################################
#
# Nls config file
#

sub GetNlsConfigFile($$$$) {
my ($application, $release, $nls, $build) = @_;

  return "$filestopdir/Word/$release.0/B1/nls/$nls/language.cfg" if $application eq 'word';
  return "$filestopdir/Excel/$release.0/B1/nls/$nls/language.cfg" if $application eq 'excel';
  return "$filestopdir/Powerpoint/$release.0/B1/nls/$nls/language.cfg" if $application eq 'powerpoint';
  return undef;
}


#might as well generate this one on the fly... (always the same)
sub GetGeneralFile($$$) {
my ($application, $release, $build) = @_;

  return "$filestopdir/Word/$release.0/B1/general.cfg" if $application eq 'word';
  return "$filestopdir/Excel/$release.0/B1/general.cfg" if $application eq 'excel';
  return "$filestopdir/Powerpoint/$release.0/B1/general.cfg" if $application eq 'powerpoint';
  return undef;
}


#############################################################################################################
#
# Needed to create Appcfg.cfg
#

sub GetWMParameters($$) {
my ($application, $release) = @_;

return {'NEWWP' => 78, 'OPENWP' => 79, 'CLOSEWP' => 490, 'SAVEWP' => 82, 'SAVEASWP' => 83, 'EXITWP' => 97}
   if $application eq 'word' and $release eq '7';
return {'NEWWP' => 78, 'OPENWP' => 79, 'CLOSEWP' => 230, 'SAVEWP' => 82, 'SAVEASWP' => 83, 'EXITWP' => 97}
   if $application eq 'word' and $release eq '8';
return {'NEWWP' => 119, 'OPENWP' => 1, 'CLOSEWP' => 144, 'SAVEWP' => 4, 'SAVEASWP' => 5, 'EXITWP' => 10}
   if $application eq 'excel' and $release eq '7';
return {'NEWWP' => 119, 'OPENWP' => 1, 'CLOSEWP' => 144, 'SAVEWP' => 4, 'SAVEASWP' => 5, 'EXITWP' => 10}
   if $application eq 'excel' and $release eq '8';
return {'NEWWP' => 2, 'OPENWP' => 3, 'CLOSEWP' => 4, 'SAVEWP' => 5, 'SAVEASWP' => 6, 'EXITWP' => 25, 'SLIDESHOWWP' => 56}
   if $application eq 'powerpoint' and $release eq '8';

return undef;
}

sub GetOLEClassID($$) {
my ($application, $release) = @_;

return 'Word.Basic'
   if $application eq 'word' and $release eq '7';
return 'Word.Application'
   if $application eq 'word' and $release eq '8';
return 'Excel.Application.5'
   if $application eq 'excel' and $release eq '7';
return 'Excel.Application'
   if $application eq 'excel' and $release eq '8';
return 'Powerpoint.Application'
   if $application eq 'powerpoint' and $release eq '8';

return undef;
}

sub GetAppWindowClassNames($$) {
my ($application, $release) = @_;

return {'OpenDlgClassName' => 'bosa_sdm_Microsoft Word for Windows 95', 'SaveAsDlgClassName' => 'bosa_sdm_Microsoft Word for Windows 95', 'DeskClassName' => 'OpusDesk', 'DocCLassName' => 'OpusMwd'}
   if $application eq 'word' and $release eq '7';
return {'OpenDlgClassName' => 'bosa_sdm_Microsoft Word 8.0', 'SaveAsDlgClassName' => 'bosa_sdm_Microsoft Word 8.0', 'DeskClassName' => '_WwB', 'DocCLassName' => '_WwB'}
   if $application eq 'word' and $release eq '8';
return {'OpenDlgClassName' => 'bosa_sdm_XL', 'SaveAsDlgClassName' => 'bosa_sdm_XL', 'DeskClassName' => 'XLDESK', 'DocCLassName' => 'EXCEL9'}
   if $application eq 'excel' and $release eq '7';
return {'OpenDlgClassName' => 'bosa_sdm_XL8', 'SaveAsDlgClassName' => 'bosa_sdm_XL8', 'DeskClassName' => 'XLDESK', 'DocCLassName' => 'EXCEL7'}
   if $application eq 'excel' and $release eq '8';
return {'OpenDlgClassName' => 'bosa_sdm_Mso96', 'SaveAsDlgClassName' => 'bosa_sdm_Mso96', 'DeskClassName' => 'paneClassDC', 'DocCLassName' => 'paneClassDC'}
   if $application eq 'powerpoint' and $release eq '8';

return undef;
}



#returns 0 on failure
sub CreateRCParameterFile($$$$$$) {
my ($application, $release, $nls, $build, $emulator, $filepath) = @_;

  my @filelines = ();
  
  if (not open RCPARAMFILE, ">$filepath") {
    my $msg = "Unable to open $filepath for output";
    warn $msg; SetLastError($msg);
    return 0;
  }

  my $appname;
  $appname = 'Word'  if $application eq 'word';
  $appname = 'Excel' if $application eq 'excel';
  $appname = 'PowerPoint' if $application eq 'powerpoint';
  if (not defined $appname) {
    my $msg = "[$application] is not a supported application";
    warn $msg; SetLastError($msg);
    close RCPARAMFILE;
    return 0;
  }  
  push @filelines, "APPNAME|$appname\n";

  my $appversion;
  $appversion = '95' if $application eq 'word' and $release eq '7';
  $appversion = '97' if $application eq 'word' and $release eq '8';
  $appversion = '95' if $application eq 'excel' and $release eq '7';
  $appversion = '97' if $application eq 'excel' and $release eq '8';
  $appversion = '97' if $application eq 'powerpoint' and $release eq '8';
  if (not defined $appversion) {
    my $msg = "[$application.$release] is not a supported verion of $application";
    warn $msg; SetLastError($msg);
    close RCPARAMFILE;
    return 0;
  }
  push @filelines, "APPVERSION|$appversion\n";
  
  my $oleid = GetOLEClassID($application, $release);
  if (not defined $oleid) {
    my $msg = "Unable to find OLE2 class ID for [$application.$release]";
    warn $msg; SetLastError($msg);
    close RCPARAMFILE;
    return 0;
  }
  push @filelines, "OLEID|$oleid\n";

  my $appexepath = GetAppExecutablePath($application, $release);
  if (not defined $oleid) {
    my $msg = "Unable to find application executable path [$application.$release]";
    warn $msg; SetLastError($msg);
    close RCPARAMFILE;
    return 0;
  }  
  push @filelines, "APPPATH|$appexepath\n";

  my $appwmprms = GetWMParameters($application, $release);
  if (not defined $appwmprms) {
    my $msg = "Unable to find WM parameters for [$application.$release]";
    warn $msg; SetLastError($msg);
    close RCPARAMFILE;
    return 0;
  }
  foreach (keys(%{$appwmprms})) {
    push @filelines, "$_|$appwmprms->{$_}\n";
  }

  my $appwndclsnames = GetAppWindowClassNames($application, $release);
  if (not defined $appwndclsnames) {
    my $msg = "Unable to find windows class names for [$application.$release]";
    warn $msg; SetLastError($msg);
    close RCPARAMFILE;
    return 0;
  }  
  foreach (keys(%{$appwndclsnames})) {
    push @filelines, "$_|$appwndclsnames->{$_}\n";
  }
  
  print RCPARAMFILE @filelines;
  close RCPARAMFILE;

return 1;
}

########################################################
#
# Variables
#

sub SetVariable($$) {
my ($varname, $varvalue) = @_;
return ($variables{$varname} = $varvalue);
}

sub GetVariable($) {
my $varname = shift;
return ($variables{$varname}) if exists $variables{$varname};
my $msg = "Variable $varname not defined";
SetLastError($msg);
return undef;
}

sub GetImgVariables($) {
my $img = shift;

  my $vars;
  if (($img eq 'W95usO95usO97us.hdf')||($img eq 'W95jpO95jp.hdf')) {
    $vars = {'DOCSDIR' => 'c:\\docs'};
  }
  elsif ($img eq 'W95usO97us.hdf') {
    $vars = {'DOCSDIR' => 'c:\\My Documents'};
  }
  foreach (keys(%$vars)) {
    SetVariable($_, $vars->{$_});
  }
}

sub GetAppReleaseVariables($$) {
my ($app, $rls) = @_;

  my $vars = {'GLOBALDIR' => GetAppTemplateDir($app, $rls),
     'STARTUPDIR' => GetAppStartupDir($app, $rls)};
  foreach (keys(%$vars)) {
    SetVariable($_, $vars->{$_});
  }
}

sub GetNlsVariables($$) {
my ($app, $nls) = @_;

  my $vars;
  if ($app eq 'powerpoint') {
    $vars = {'GLOBALNAME' => 'Blank Presentation.pot'};  
  }
  else {
    if ($nls eq 'Japanese') {
      $vars = {'GLOBALNAME' => '•W€.dot'};
    } else {
      $vars = {'GLOBALNAME' => 'Normal.dot'};
    }
  }
  foreach (keys(%$vars)) {
    SetVariable($_, $vars->{$_});
  }
}


sub GetCmdVariables($) {
my $cmdfile = shift;

  my $vars;
  if (($cmdfile eq 'globinfcmds.lst')||($cmdfile eq 'wunkcmds.lst')) {
    $vars = {'VIRDIR' => GetVariable('DOCSDIR'), 'VIRNAME' => 'Sample.doc'};
  } elsif ($cmdfile eq 'docinfcmds.lst') {
    $vars = {'VIRDIR' => GetVariable('GLOBALDIR'), 'VIRNAME' => GetVariable('GLOBALNAME')};
  } elsif (($cmdfile eq 'excelcmds.lst')||($cmdfile eq 'xunkcmds.lst')) {
    $vars = {'VIRDIR' => GetVariable('DOCSDIR'), 'VIRNAME' => 'Sample.xls'};
  } elsif ($cmdfile eq 'startupcmds.lst') {
    $vars = {'VIRDIR' => GetVariable('STARTUPDIR'), 'VIRNAME' => 'PERSONAL.XLS'};
  } elsif (($cmdfile eq 'wupconv.lst')||($cmdfile eq 'xupconv.lst')||($cmdfile eq 'exdownconv.lst'))  {
    $vars = {'VIRDIR' => GetVariable('DOCSDIR'), 'VIRNAME' => 'z.z'};
  } elsif ($cmdfile eq 'powerpoint.lst')  {
    $vars = {'VIRDIR' => GetVariable('DOCSDIR'), 'VIRNAME' => 'Sample.ppt'};
  } elsif ($cmdfile eq 'ppglobal.lst') {
    $vars = {'VIRDIR' => GetVariable('GLOBALDIR'), 'VIRNAME' => GetVariable('GLOBALNAME')};
  }
  foreach (keys(%$vars)) {
    SetVariable($_, $vars->{$_});
  }
}


sub GetVariables($$$$$) {
my ($app, $rls, $nls, $cmdfile, $driveset) = @_;

  my $image = $driveset->GetDriveSubtype('c');
  GetImgVariables($image);
  GetAppReleaseVariables($app, $rls);
  GetNlsVariables($app, $nls);
  GetCmdVariables($cmdfile);
}


sub CreateVariableFile($) {
my $filename = shift;
  
  open(VARS, ">$filename");
  foreach (sort(keys(%variables))) {
    print VARS "$_=".GetVariable($_)."\n";
  }
  close(VARS);
  return 1;
}


1;

