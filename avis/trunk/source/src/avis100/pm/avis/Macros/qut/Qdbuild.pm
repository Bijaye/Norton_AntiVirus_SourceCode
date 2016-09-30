package AVIS::Macro::Qdbuild;

use Cwd;
use File::Copy;
use File::Compare;

sub new {
        my ($class) = shift;
        my ($self) = {};
        bless($self,$class);

        $self->{LOGSTRING} = undef;
        $self->{NEWDEFS} = undef;

        if (@_) {
           $self->{NAVROOT} = shift;
           if (@_) {
               $self->{CHANGELIST} = shift;
           } else {
               $self->{CHANGELIST} = undef;
           }
        }  else {
             $self->{NAVROOT} = undef;
        }# endif
        $self->{newdefdir}="newdef";  ###directory to put new definitions
        $self->{LOGOBJECT} = 0;		   ###optional logging object
        return ($self);
} #end new


##############

sub navroot {
        my $self = shift;
        if (@_) {
           $self->{NAVROOT} = shift;
        }
        return $self->{NAVROOT};
}  #end navroot

###############

sub changelist {
        my $self = shift;
        if (@_) {
           $self->{CHANGELIST} = shift;
        }
        return $self->{CHANGELIST};
}

################

sub logString {
        my $self = shift;
        if (@_) {
           $self->{LOGSTRING} = shift;
        }
        return $self->{LOGSTRING};
}
################

sub logObject {
        my $self = shift;
        if (@_) {
           $self->{LOGOBJECT} = shift;
        }
        return $self->{LOGOBJECT};
}

################

sub newDefs {
        my $self = shift;
        if (@_) {
           $self->{NEWDEFS} = shift;
        }
        return $self->{NEWDEFS};
}

#################


sub doBuild {

        my $self = shift;
        my $logger = $self->{LOGOBJECT};

	     $logger->logString("Entering doBuild...") if $logger;

        if (defined $self->{NAVROOT}) {
                 $blddir = $self->navroot();
        }  else {
             $self->logString("No root directory specified\n");
             return(0);
        }

        if (defined $self->{CHANGELIST}) {
                my $reflist = $self->changelist();
                @filelist = @$reflist;
        } else {
            $self->logString("Changelist not specified - full build not implemented\n");
            return(0);
        }
        $MACFLAG=0;
        $BINFLAG=0;
        foreach $file (@filelist) {
                 if ($file =~ /wdnamsig/i || $file =~ /xlnamsig/i) {
                        $MACFLAG=1;
                } elsif ($file =~ /autoverv/i) {
                        $BINFLAG=1;
                }
        } #end foreach
        my $Curdir=Cwd::cwd();

        $self->setup();  #######setup the environment variables

        if ($MACFLAG==1) {  ##macro build
                unless (chdir "$blddir\\NAVEX") {   ####hardcoded dir name NAVEX
                        $self->logString("Unable to change to the directory $blddir\\navex \n");
                        return(0)
                }
                $self->PJ("dx");  #####  hardcoded platform dx
                if (-e "virscan6.dat") {
                        unless (unlink "virscan6.dat") {
                                $self->logString("Unable to delete old virscan6.dat\n");
                                return(0);
                        }
                }
		foreach (<*.err>) {
			unlink;  #####erase any error files from the previous build
		}
                system ("nmake /I /f macbld.mak");
                chdir "$Curdir";
                if (-e "$blddir\\navex\\virscan6.dat") {
                        return (0==$self->dopackage("$blddir\\navex\\virscan6.dat"));
                } else {
		    if (-e "$blddir\\navex\\make.err") {
				unless (open (MAKERR, "<$blddir\\navex\\make.err")) {
				   $MakeErrString = "Can't open the make.err file\n";
				} else {
				   	$MakeErrString = <MAKERR>;
					close (MAKERR);
				} #end unless
		    } #endif
                    $self->logString("virscan6.dat not built - here are the contents of make.err \n $MakeErrString \n");
                    return(0);
                }
        }

        if ($BINFLAG==1) {
                unless (chdir "$blddir\\PAM") {
                        $self->logString("Unable to change to PAM directory \n");
                        return(0);
                }
                $self->PJ("dx");
                if (-e "$blddir\\PAM\\autoverv.dat") {
                        unless (unlink "$blddir\\PAM\\autoverv.dat") {
                                $self->logString("Unable to delete autoverv.dat\n");
                                return (0);
                        }
                }
                system("nmake /fautoverv.mak");
                if (-e "$blddir\\pam\\autoverv.dat") {
                        if($retcode = $self->dovirscan2()==0) {
                                return(0==$self->dopackage("$blddir\\packages\\virscan2.dat"));
                        } else {
                            return(0==$retcode);
                        }
                } else {
                        $self->logString("autoverv.dat not built - please check error file\n");
                        return(0);
                }
        } ###end if

        $self->logString("Don't have make file to deal with this changelist, sorry \n");
        return(0);

} ###end dobuild

##################################

sub setup {
        my ($self) = shift;
        my($blddir)=$self->navroot();
        $ENV{"BLDDIR"}=$blddir;
        if($ENV{"OLDPATH"} ne NULL)
                {
                        $ENV{"OLDPATH"}=$ENV{"PATH"};
                }

        $temp = join "\\" , $blddir,"progs";
        $ENV{"PATH"}=join ";", $temp,$ENV{"OLDPATH"};
        $ENV{"INCLUDE"} = join "\\", $blddir,"include";
        $ENV{"LIB"} = join "\\", $blddir,"lib";
        $ENV{"QUAKEINC"} = join "\\", $blddir,"NAVEX";
        $ENV{"SARCBOT"} = 1;

}  ###End Setup


####################################

sub PJ {
        my ($self) = shift;
        my ($plat)= shift;
        $plat = uc $plat;
        my ($blddir) = $self->navroot();

        $ENV {"QUAKEINC"} = ".";
        if ($ENV{"OLDINCLUDE"} ne NULL)
                {
                        $ENV{"OLDINCLUDE"}=$ENV{"INCLUDE"};
                }
        # WIN project can use the same directory as DX!
        $blddir=$ENV{"BLDDIR"};
        if ($ENV{"PLATFORM"} ne ".WIN")
                {
                        $tempinc=join "\\", $blddir,"include",$plat;
                }
        else
                {
                        $tempinc=join "\\", $blddir,"include","dx";
                }

        $ENV{"INCLUDE"} = join ";", $ENV{"OLDINCLUDE"},$tempinc;

        if ($ENV{"OLDLIB"} ne NULL)
                {
                        $ENV{"OLDLIB"}=$ENV{"LIB"};
                }

        $templib=join "\\", $blddir,"lib",$plat;
        $ENV{"LIB"} = join ";", $ENV{"OLDLIB"},$templib;

} ###End PJ

#################################

sub dovirscan2 {
        my ($self) = shift;
        my ($blddir)= $self->{NAVROOT};
        $wrkdir="$blddir\\PACKAGES";
        unless (chdir $wrkdir) {
                $self->logString("Unable to change to $wrkdir\n");
                return(1);
        }
        unless (File::Copy::copy("$blddir\\virscan\\virscan.dat",$wrkdir)) {
                my $error = $!;
                chomp $error;
                $self->logString("Error ($error) copying $blddir\\virscan\\virscan.dat to $wrkdir\n");
                return(1);
        }
        unless (File::Copy::copy ("$blddir\\pam\\virscan2.dat","$wrkdir\\virscan2.pam")) {
                my $error = $!;
                chomp $error;
                $self->logString("Error ($error) copying $blddir\\pam\\virscan2.dat to $wrkdir\\virscan2.pam\n");
                return(1);
       }

        system ("$blddir\\progs\\datgen $wrkdir\\virscan.dat");
        system ("$blddir\\progs\\builddat $blddir\\progs\\virscan2.def $wrkdir\\virscan2.dat $blddir\\progs\\version.dat");

        @todel=("boolist.dat","claimed.dat","algitw.dat","algzoo.dat","boo.dat","crc.dat","mem.dat",
                        "names.dat","namesidx.dat","virinfo.dat","virscan.a!!","virscan.alg",
                        "virscan.c!!","virscan.crc","virscan.m!!","virscan.mlg","virscan.mrc",
                        "virscan.mpg","virscan.p!!","virscan.plg","virscan.dat");

        foreach $filname (@todel)
                {

                  if (-e "$wrkdir\\$filname")
                        {
                                unlink "$wrkdir\\$filname";
                        }
                }

        system ("$blddir\\progs\\glue $wrkdir\\virscan2.dat $wrkdir\\virscan2.pam $wrkdir\\virscan2.out");
        unless (File::Copy::copy ("$wrkdir\\virscan2.out", "$wrkdir\\virscan2.dat")) {
                my $error = $!;
                chomp $error;
                $self->logString("Error ($error) copying $wrkdir\\virscan2.out to $wrkdir\\virscan2.dat\n");
                return(1);
        }

        if (-e "$wrkdir\\virscan2.pam")
                {
                        unlink "$wrkdir\\virscan2.pam";
                }
        if (-e "$wrkdir\\virscan2.out")
                {
                        unlink "$wrkdir\\virscan2.out";
                }
        return(0);
} ##End dovirscan2
###################################

sub dopackage {
        my ($self) = shift;
        if (@_) {
           $newfile = shift;
        } else {
           $newfile = undef;
        }
        my $newdir = $self->{newdefdir};
        my $blddir = $self->navroot();
        my $zipfile = "$blddir\\PACKAGES\\EIN32\\DISK2\\VIRSCAN2.ZIP";

        if (defined $newfile) {
                system ("zip -j $zipfile $newfile");
        }
        if (-d "$blddir\\$newdir") {  ##directory exists
                unless (opendir NEWDIR, "$blddir\\$newdir") {
                        $self->logString("Can't open directory $blddir\\$newdir \n");
                        return(1);
                }
                my @allfiles=grep !/^\.\.?$/, readdir NEWDIR;
                closedir NEWDIR;
                foreach $file (@allfiles) {
                        unless (unlink "$blddir\\$newdir\\$file") {
                                $self->logString("Can't delete $blddir\\$newdir\\$file\n");
                                return(1);
                        }
                } #end foreach
        } else { ####No such directory
            unless (mkdir "$blddir\\$newdir",umask) {
                $self->logString("Can't create $blddir\\$newdir \n");
                return(1);
            }
        }
        $newdefdir="$blddir\\$newdir";
        unless (File::Copy::copy("$blddir\\VIRSCAN.INF\\NAVENG.EXP", $newdefdir)) {
                $self->logString("Can't copy $blddir\\VIRSCAN.INF\\NAVENG.EXP to $newdefdir\n");
                return(1);
        }
        unless (File::Copy::copy("$blddir\\VIRSCAN.INF\\NAVENG.SYS", $newdefdir)) {
                $self->logString("Can't copy $blddir\\VIRSCAN.INF\\NAVENG.SYS to $newdefdir\n");
                return(1);
        }
        unless (File::Copy::copy("$blddir\\VIRSCAN.INF\\NAVENG32.DLL", $newdefdir)) {
                $self->logString("Can't copy $blddir\\VIRSCAN.INF\\NAVENG32.DLL to $newdefdir\n");
                return(1);
        }
        unless (File::Copy::copy("$blddir\\NAVEX\\NAVEX15.EXP", $newdefdir)) {
                $self->logString("Can't copy $blddir\\NAVEX\\NAVEX15.EXP to $newdefdir\n");
                return(1);
        }
        unless (File::Copy::copy("$blddir\\NAVEX\\NAVEX15.SYS", $newdefdir)) {
                $self->logString("Can't copy $blddir\\NAVEX\\NAVEX15.SYS to $newdefdir\n");
                return(1);
        }
        unless (File::Copy::copy("$blddir\\NAVEX\\NAVEX15.VXD", $newdefdir)) {
                $self->logString("Can't copy $blddir\\NAVEX\\NAVEX15.VXD to $newdefdir\n");
                return(1);
        }
        unless (File::Copy::copy("$blddir\\NAVEX\\NAVEX32A.DLL", $newdefdir)) {
                $self->logString("Can't copy $blddir\\NAVEX\\NAVEX32A.DLL to $newdefdir\n");
                return(1);
        }
        unless (File::Copy::copy("$blddir\\VIRSCAN.INF\\NCSACERT.TXT", $newdefdir)) {
                $self->logString("Can't copy $blddir\\VIRSCAN.INF\\NCSACERT.TXT to $newdefdir\n");
                return(1);
        }
        unless (File::Copy::copy("$blddir\\VIRSCAN.INF\\TECHNOTE.TXT", $newdefdir)) {
                $self->logString("Can't copy $blddir\\VIRSCAN.INF\\TECHNOTE.TXT to $newdefdir\n");
                return(1);
        }
        unless (File::Copy::copy("$blddir\\VIRSCAN.INF\\VIRSCAN.INF", $newdefdir)) {
                $self->logString("Can't copy $blddir\\VIRSCAN.INF\\VIRSCAN.INF to $newdefdir\n");
                return(1);
        }
        system ("unzip -o $blddir\\PACKAGES\\EIN32\\DISK2\\VIRSCAN2.ZIP -d $newdefdir");

        if (defined $newfile) {
                #####get the real name
                my $slashpos= rindex ($newfile, "\\");
                if ($slashpos >= 0 ) {
                        $realname= substr($newfile,$slashpos+1,length($newfile));
                } else {
                        $realname = $newfile;  #No path name specified - may be the file is in this directory itself!!
                }
                ######compare the file with the built one - just in case
                unless (File::Compare::compare ($newfile, "$newdefdir\\$realname")==0) {
                        $self->logString("File $newfile and $newdefdir\\$realname are different - packaging failed\n");
                        return(1);
                }
        }
        $self->newDefs($newdefdir);  ####Now set the newdef path
        return(0);  #######Things are alright
} #####end dopackage

1;

#########################
 
