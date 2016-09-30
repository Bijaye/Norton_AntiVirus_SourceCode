####################################################################
#                                                                  #
# Program name:  PM/AVIS/TestBinRepair/pathEnvTools.pm             #
#                                                                  #
# Module name:   AVIS::PathEnvTools.pm                             #
#                                                                  #
# Description:   For manipulating the PATH environment variable.   #
#                                                                  #
#                                                                  #
# Statement:     Licensed Materials - Property of IBM              #
#                (c) Copyright IBM Corp. 1999                      #
#                                                                  #
# Author:        Andy Raybould                                     #
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
#   pathcatAsList (pathcat):                                       #
#      splits a semicolon-delimited path concatenation into a list.#
#                                                                  #
#   listAsPathcat (list of paths):                                 #
#      joins an array of path strings into a semicolon-delimited   #
#      path concatenation.                                         #
#                                                                  #
#   findInPathcat (pathcat, filename):                             #
#      Returns a list of the occurrences of a given file in the    #
#      directories of the given pathcat.                           #
#      e.g. findInPathcat($ENV{'PATH'},'cmd.exe').                 #
#                                                                  #
#   notInPathcat (pathcat, list of filenames):                     #
#      True if one or more of the files can't be found in the      #
#      pathcat. Returns a list of the missing files.               #
#                                                                  #
#   addScriptDirToPath ():                                         #
#      prefixes the directory of the main perl script to the path  #
#      - makes it easier to bundle scripts with required binaries. #
#                                                                  #
#   addINCtoPath ():                                               #
#      prefixes the contents of @INC to the path                   #
#      - makes it easier to bundle packages with required binaries.#
#                                                                  #
####################################################################

package AVIS::PathEnvTools;
require Exporter;

@ISA = qw(Exporter);
@EXPORT = qw(pathcatAsList        listAsPathcat
             findInPathcat        notInPathcat
             addScriptDirToPath   addINCtoPath);

@EXPORT_OK = qw();

use strict;

my $DD = ($^O =~ m/win/i ? '\\' : '/');
my $QDD = quotemeta $DD;
1;



sub pathcatAsList ($) {
   split /;/,$_[0];
}



sub listAsPathcat (@) {
   join ';', @_;
}



sub findInPathcat ($$) {
   my ($path, $file) = @_;
   grep {-f} map {"$_$DD$file"} pathcatAsList($path);
}



sub notInPathcat ($@) {
   my ($path, @files) = @_;
   grep {!findInPathcat($path,$_)} @files;
}



sub addScriptDirToPath () {
   (my $here = $0) =~ s/[^:$QDD]*$//;
   $ENV{'PATH'} = "$here.;$ENV{'PATH'}";
}



sub addINCtoPath () {
   $ENV{'PATH'} = listAsPathcat(@INC).";$ENV{'PATH'}";
}


__END__
#?? WIP

sub listFilesInDir ($) {
   grep isInClass('f',$_), listObjectsInDir(@_);
}



sub listSubdirs ($) {
   grep isInClass('d',$_), listObjectsInDir(@_);
}



sub listObjectsInDir ($) {
   my ($dir,$pattern)   =   $_[0] =~ m/(.*)([^:$QDD]+)/;
   if ($^O =~ m/win/i) {
      map {chomp;$dir.$_} `dir /b $dir.$pattern`;   # glob can't handle blanks in paths
   }else {
      grep {$_ ne $dir.$pattern} glob $dir.$pattern;
   }
}



sub isInClass ($$) {
   my ($class, $file) = @_;
   $file =~ s<$QDD></>;
   foreach (split //,$class) {
      return 1 if eval "-$_ '$file'";
   }
   return 0;
}
