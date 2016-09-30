#!/usr/bin/perl

# IIS 4.0 won't seem to let me start in the cgi-bin dir
# so we need to chdir there first thing.
BEGIN { chdir ("cgi-bin") }

use CGI;
use Perforce;
use DirHandle;
use File::Path;
use File::Copy;
use POSIX qw(strftime);

require 'ssclient.pl';

# Set if debugging
$DEBUG_IM = 1;

# Function return values
$OK = 0;
$ERROR = 1;
$FAIL = 2;
$IN_PROGRESS = 3;

# for access to the Perforce system
$p4 = new Perforce (p4user   => 'cformul',
                    p4port   => '9.2.70.30:1666',
                    p4client => 'morse_im');

$query = new CGI;
$function =  $query->param('function');

# This eval saves us from checking that the function called actually
# exists and prevents a bail out due to a runtime error.
eval { &$function() };
if ($@) {
    print $query->header (-type => 'text/html', -X_IMStatus => $ERROR);
    print "Unknown function" if $DEBUG_IM;
}


sub Lock {
    my $LOCK = 'MASTER-LOCK';
    my $TIMEOUT = 10000;

    my $result = SSLockMutex ($LOCK, $TIMEOUT);
    if ($result == $SSLOCKMUTEX_OK) {
        print $query->header (-type => 'text/html', -X_IMStatus => $OK);
        print "Lock OK" if $DEBUG_IM;
    } elsif ($result == $SSLOCKMUTEX_TIMEOUT) {
	print $query->header (-type => 'text/html', -X_IMStatus => $IN_PROGRESS);
        print "Lock Timed Out" if $DEBUG_IM;
    } else {
        print $query->header (-type => 'text/html', -X_IMStatus => $ERROR);
        print "Lock Error" if $DEBUG_IM;
    }
}

sub Unlock {
    my $LOCK = 'MASTER-LOCK';
    my $TIMEOUT = 10000;

    my $result = SSUnlockMutex ($LOCK);

    if ($result == $SSUNLOCKMUTEX_OK) {
        print $query->header (-type => 'text/html', -X_IMStatus => $OK);
        print "Unlock OK" if $DEBUG_IM;
    } else {
        print $query->header (-type => 'text/html', -X_IMStatus => $ERROR);
        print "Unlock Error" if $DEBUG_IM;
    }
}

sub MakeLocalCopy {
    my $LocalSrcPath = 'source';
    my $LocalMacroSrcPath = 'source\\navex';
    my $LocalVirscanSrcPath = 'source\\virscan.dat';
    my $LocalTrojanSrcPath = 'source\\virscan7.dat';
    my $SrcFileList = 'LastOpenedSrc.dat';

    rmtree ($LocalSrcPath);
    mkpath ([$LocalMacroSrcPath, $LocalVirscanSrcPath, $LocalTrojanSrcPath]);
    unlink ($SrcFileList);
    
    if ($p4->sync() == 0) {
        print $query->header (-type => 'text/html', -X_IMStatus => $OK);
        print "MakeLocalCopy OK" if $DEBUG_IM;
    } else {
        print $query->header (-type => 'text/html', -X_IMStatus => $ERROR);
        print "p4 sync exited with error" if $DEBUG_IM;
    }

    foreach ($p4->lastmsg()) {
        print "<BR>$_";
    }
}

sub RevertLocalCopy {
    my $LocalSrcPath = 'source';
    my $LocalMacroSrcPath = 'source\\navex';
    my $LocalVirscanSrcPath = 'source\\virscan.dat';
    my $LocalTrojanSrcPath = 'source\\virscan7.dat';
    my $SrcFileList = 'LastOpenedSrc.dat';

    rmtree ($LocalSrcPath);
    mkpath ([$LocalMacroSrcPath, $LocalVirscanSrcPath, $LocalTrojanSrcPath]);
    unlink ($SrcFileList);

    print $query->header (-type => 'text/html', -X_IMStatus => $OK);
    print "RevertLocalCopy OK" if $DEBUG_IM;
}

sub SubmitDefinition {
    my $HeaderFile = 'autovid.h';
    my $TrojanFile = 'trojsigs.src';
    my $date = POSIX::strftime("%b %d %Y", localtime());
    my $LocalMacroSrcPath = 'source\\navex';
    my $LocalVirscanSrcPath = 'source\\virscan.dat';
    my $LocalTrojanSrcPath = 'source\\virscan7.dat';
    my $MacroSrcPath = 'H:\\Source\\navex\\src';
    my $VirscanSrcPath = 'H:\\Source\\virscan.dat\\src';
    my $TrojanSrcPath = 'H:\\Source\\virscan7.dat\\src';
    my $SrcFileList = 'LastOpenedSrc.dat';
    my ($IDSP_Type, $DefID, $DefFile, $MacroFile, $Variant, $ID, $VID, $VirusName);

    my $state = 0;
    my $IDSP = $query->param('IDSP');

    unless ($IDSP) {
        print $query->header (-type => 'text/html', -X_IMStatus => $FAIL);
        print "Error: Invalid or incomplete IDSP" if $DEBUG_IM;
        return;
    }

    while (<$IDSP>) {
        # stange things are happening when I send over the IDSP
        # for some reason a CR (0x0d) is being inserted before the CR/LF (0x0d/0x0a)
        # this should fix it.
        chop;
        my $c = chop;
        $_ .= $c if (ord ($c) != 0x0d);
        $_ .= "\n";

	if ($state == 0) {
            next if /^\|/;
            if (/\[(MACRO|MALWARE)DEFSTART\]/) {
                $IDSP_Type = $1;
                $state = 1;
                next;
            }
        }

        if ($state == 1) {
            next if /^\|/;
            if (/\[MACRODEFEND\]/ || /\[MALWAREDEFEND\]/) {
                $state = 3;
                last;
            }

            if (/\[VIRSCANSTART\]/) {
                last unless (defined $DefFile && defined $VirusName);
                $state = 2;

                copy ("$VirscanSrcPath\\$DefFile", "$LocalVirscanSrcPath\\$DefFile");
		system ("attrib -r $LocalVirscanSrcPath\\$DefFile"); 

                unless (open DAT, ">> $LocalVirscanSrcPath\\$DefFile") {
                    print $query->header (-type => 'text/html', -X_IMStatus => $ERROR);
                    print "Unable to open $LocalVirscanSrcPath\\$DefFile" if $DEBUG_IM;
                    return;
                }

                next;
            }

            if (/\[MACROSTART\]/) {
                last unless (defined $DefFile && defined $VirusName);
                $state = 2;

                copy ("$MacroSrcPath\\$MacroFile", "$LocalMacroSrcPath\\$MacroFile");
		system ("attrib -r $LocalMacroSrcPath\\$MacroFile"); 

                unless (open DAT, ">> $LocalMacroSrcPath\\$MacroFile") {
                    print $query->header (-type => 'text/html', -X_IMStatus => $ERROR);
                    print "Unable to open $LocalMacroSrcPath\\$MacroFile" if $DEBUG_IM;
                    return;
                }

                next;
            }

            if (/\[TROJANSTART\]/) {
                $state = 2;

                copy ("$TrojanSrcPath\\$TrojanFile", "$LocalTrojanSrcPath\\$TrojanFile");
		system ("attrib -r $LocalTrojanSrcPath\\$TrojanFile"); 

                unless (open DAT, ">> $LocalTrojanSrcPath\\$TrojanFile") {
                    print $query->header (-type => 'text/html', -X_IMStatus => $ERROR);
                    print "Unable to open $LocalVirscanSrcPath\\$DefFile" if $DEBUG_IM;
                    return;
                }

                next;
            }

            if (/DEFTYPE = (\w+)/) {
                next if ($1 eq "MALWARE");

                ($DefID, $Variant, $ID) = GetDefVarID();
                unless (defined $DefID && defined $Variant && defined $ID) {
                    print $query->header (-type => 'text/html', -X_IMStatus => $ERROR);
                    print "Error getting Def ID Info" if $DEBUG_IM;
                    return;
                }

                $DefFile = sprintf ("def%02x.src", $DefID);
                $VID = sprintf ("%02x%02x", $DefID, $ID);

	      SWITCH: {
		  if ($1 eq 'WD7') { $VirusName = "WM.Automat.$Variant";
                                      $MacroFile = 'wd_auto.wdf';
                                      last SWITCH; }
		  if ($1 eq 'WD8') { $VirusName = "W97M.Automat.$Variant";
                                      $MacroFile = 'wd_auto.wdf';
                                      last SWITCH; }
		  if ($1 eq 'XL95') { $VirusName = "XM.Automat.$Variant";
                                      $MacroFile = 'xl_auto.wdf';
                                      last SWITCH; }
		  if ($1 eq 'XL97') { $VirusName = "X97M.Automat.$Variant";
                                      $MacroFile = 'xl_auto.wdf';
                                      last SWITCH; }
	      }
	    }

            next;
        }

        if ($state == 2) {
            next if /^\|/;
            $state = 1, next if (/\[VIRSCANEND\]/ || /\[MACROEND\]/ || /\[TROJANEND\]/);
            s/(\[Build_Status\]\s*=\s*)UNTESTED/$1PASSED/;
            s/(\[QAStatus\]\s*)NO/$1PASSED/;
            s/<GUID>/$VID\_$Variant/;
            s/<VirusName>/$VirusName/;
            s/<Date>/$date/;
            s/<VirusID>/$VID/;
            s/<VirusIDLabel>/VID_AUTOMAT$Variant/;
            print DAT "$_";

	}
    }

    unless ($state == 3) {
        print $query->header (-type => 'text/html', -X_IMStatus => $FAIL);
        print "Error: Invalid or incomplete IDSP" if $DEBUG_IM;
        return;
    }

    if ($IDSP_Type eq 'MACRO') {
        copy ("$MacroSrcPath\\$HeaderFile", "$LocalMacroSrcPath\\$HeaderFile");
        system ("attrib -r $LocalMacroSrcPath\\$HeaderFile"); 
        unless (open DAT, ">> $LocalMacroSrcPath\\$HeaderFile") {
            print $query->header (-type => 'text/html', -X_IMStatus => $ERROR);
            print "Unable to open $LocalMacroSrcPath\\$HeaderFile" if $DEBUG_IM;
            return;
        }

        print DAT "#define VID_AUTOMAT$Variant     0x$VID\n";
        close DAT;
    }
 
    unless (open SRCLIST, "> $SrcFileList") {
        print $query->header (-type => 'text/html', -X_IMStatus => $ERROR);
        print "Unable to open $SrcFileList" if $DEBUG_IM;
        return;
    }

    print SRCLIST "$IDSP_Type\n";
    if ($IDSP_Type eq 'MACRO') {
        print SRCLIST "$DefFile $MacroFile $HeaderFile\n";
    } else {
        print SRCLIST "$TrojanFile\n";
    }
    close SRCLIST;

    print $query->header (-type => 'text/html', -X_IMStatus => $OK);
    print "SubmitDefinition OK" if $DEBUG_IM;
}

sub GetDefVarID {
    $DataFile = 'NextDefVarID.dat';

    return undef unless (open FP, $DataFile);
    my ($def, $var, $id) = <FP> =~ /(\d+) ([A-Z]+) (\d+)/;
    close FP;

    my $newvar = $var;
    $newvar++;
    my $newid = $id+1;
    my $newdef = $def;
    if ($newid > 0xff) {
        $newid = 0;
        $newdef++;
    }

    return undef unless (open FP, "> $DataFile");
    print FP "$newdef $newvar $newid\n";
    close FP;

    return ($def, $var, $id);
}
    
        
sub BuildLocalCopy {
    $BUILDFLAG = 'im.building';
    $STARTFLAG = 'start.build';
    $ERRORFLAG = 'error.build';
    $FAILFLAG  = 'fail.build';
    $SequenceFile = "sequence\\defattrs.dat";
    $sleep = 5;

    $check = $query->param('query');
    $buildtype  = $query->param('buildtype');

    if ($check eq 'true') {
        if (-e $BUILDFLAG) {
            print $query->header (-type => 'text/html', 
                                  -X_IMStatus => $IN_PROGRESS);
            print "BuildLocalCopy In Progress" if $DEBUG_IM;
        } elsif (-e $FAILFLAG) {
            print $query->header (-type => 'text/html', 
                                  -X_IMStatus => $FAIL);
            print "Last build failed" if $DEBUG_IM;
        } else {
            print $query->header (-type => 'text/html', 
                                  -X_IMStatus => $OK);
            print "BuildLocalCopy Done" if $DEBUG_IM;
        }

        return;
    }

    if (-e $BUILDFLAG || -e $STARTFLAG) {
        print $query->header (-type => 'text/html', -X_IMStatus => $ERROR);
        print "Error: already in progress" if $DEBUG_IM;
        return;
    }

    if ($buildtype =~ /full/i) {
        $seq = $query->param('sequencenum');
        $dvn = $query->param('dailyversionnum');
        unless ($seq) {
            print $query->header (-type => 'text/html', -X_IMStatus => $ERROR);
            print "Error: Did not get sequence number" if $DEBUG_IM;
            return;
        }

        unless ($dvn) {
            print $query->header (-type => 'text/html', -X_IMStatus => $ERROR);
            print "Error: Did not get daily version number" if $DEBUG_IM;
            return;
        }

        mkpath ("builds/$seq");
        system ("echo $seq > builds/current.sqn");
        system ("echo $dvn > sequence/version.dat");

        $buf = pack ("L", $seq);
        unless (open SF, "> $SequenceFile") {
            print $query->header (-type => 'text/html', -X_IMStatus => $ERROR);
            print "Error: Cannot save sequence number" if $DEBUG_IM;
            return;
        }
  
        print SF $buf;
        close SF;
    }

    system ("echo $buildtype > $STARTFLAG");
    sleep ($sleep + 3);

    if (-e $STARTFLAG || -e $ERRORFLAG) {
        print $query->header (-type => 'text/html', -X_IMStatus => $ERROR);
        print "Error: BuildDats.pl does not seem to be running" if $DEBUG_IM;
        unlink ($STARTFLAG, $ERRORFLAG);
        return;
    }

    print $query->header (-type => 'text/html', -X_IMStatus => $IN_PROGRESS);
    print "BuildLocalCopy In Progress" if $DEBUG_IM;
}


sub CommitLocalCopy {
    my $LocalMacroSrcPath = 'source\\navex';
    my $LocalVirscanSrcPath = 'source\\virscan.dat';
    my $LocalTrojanSrcPath = 'source\\virscan7.dat';
    my $MacroSrcPath = 'H:\\Source\\navex\\src';
    my $VirscanSrcPath = 'H:\\Source\\virscan.dat\\src';
    my $TrojanSrcPath = 'H:\\Source\\virscan7.dat\\src';
    my $SrcFileList = 'LastOpenedSrc.dat';

    unless (open SRCLIST, $SrcFileList) {
        print $query->header (-type => 'text/html', -X_IMStatus => $ERROR);
        print "Unable to open $SrcFileList" if $DEBUG_IM;
        return;
    }

    chomp (my $IDSP_Type = <SRCLIST>);
    if ($IDSP_Type eq 'MACRO') {
        my ($DefFile, $MacroFile, $HeaderFile) = <SRCLIST> =~ /(.*) (.*) (.*)/;

        if ($p4->edit (file => ["$VirscanSrcPath\\$DefFile",
                                "$MacroSrcPath\\$MacroFile",
                                "$MacroSrcPath\\$HeaderFile"]) != 0) {
            print $query->header (-type => 'text/html', -X_IMStatus => $ERROR);
            print "p4 edit exited with error" if $DEBUG_IM;
            return;
        }

        if ($p4->lock () != 0) {
            print $query->header (-type => 'text/html', -X_IMStatus => $ERROR);
            print "p4 lock exited with error" if $DEBUG_IM;
            return;
        }

        copy ("$LocalVirscanSrcPath\\$DefFile", "$VirscanSrcPath\\$DefFile");
        copy ("$LocalMacroSrcPath\\$MacroFile", "$MacroSrcPath\\$MacroFile");
        copy ("$LocalMacroSrcPath\\$HeaderFile", "$MacroSrcPath\\$HeaderFile");
    } else {
        chomp (my $TrojanFile = <SRCLIST>);
        if ($p4->edit (file => "$TrojanSrcPath\\$TrojanFile") != 0) {
            print $query->header (-type => 'text/html', -X_IMStatus => $ERROR);
            print "p4 edit exited with error" if $DEBUG_IM;
            return;
        }

        if ($p4->lock () != 0) {
            print $query->header (-type => 'text/html', -X_IMStatus => $ERROR);
            print "p4 lock exited with error" if $DEBUG_IM;
            return;
        }

        copy ("$LocalTrojanSrcPath\\$TrojanFile", "$TrojanSrcPath\\$TrojanFile");
    }
    close SRCLIST;

    if ($p4->submit (description => "Bacta Automation") != 0) {
        print $query->header (-type => 'text/html', -X_IMStatus => $ERROR);
        print "p4 submit exited with and error" if $DEBUG_IM;
        return;
    }

    print $query->header (-type => 'text/html', -X_IMStatus => $OK);
    print "CommitLocalCopy OK" if $DEBUG_IM;

    foreach ($p4->lastmsg()) {
        print "<BR>$_";
    }
}

sub GetLocalVirusDataFiles {
    my $path = 'builds';
    my $defs = 'sarcx86.exe';

    $type = $query->param('type');
    $sequence = $query->param('sequencenumber');

    if ($type =~ /newest/i) {
        ($current) = `type $path\\current.sqn` =~ /(\d+)/;
        $path .= "\\$current" if ($current > $sequence);
    } elsif ($type =~ /blessed/i) {
        ($current) = `type $path\\blessed\\current.sqn` =~ /(\d+)/;
        $path .= "\\blessed" if ($current > $sequence);
    } else {
        $path .= "\\$type";
    }

    unless (-e "$path\\$defs") {
        print $query->header (-type => 'text/html', -X_IMStatus => $FAIL,
                                                    -X_sequencenum => $current);
        print "$path\\$defs does not exist" if $DEBUG_IM;
        return;
    }

    unless (open UPDATER, "$path\\$defs") {
        print $query->header (-type => 'text/html', -X_IMStatus => $ERROR);
        print "Unable to open $path\\$defs" if $DEBUG_IM;
        return;
    }

    print "X-imstatus: $OK\n",
          "X-sequencenum: $current\n", 
          "Content-Disposition: attachment; filename=\"$defs\"\n",
          "Content-Encoding: binary\n",
          "Content-Type: application/zip\n\n";

    # NT unlike UNIX distinguishes between text and binary files, so
    # we need to tell it to handle this file as binary.
    binmode UPDATER;
    while (<UPDATER>) {
        print;
    }
    close UPDATER;
}


sub GetSequenceNumber {
    my $SequenceFile = 'builds\\blessed\\current.sqn';

    unless (-e $SequenceFile) {
        print $query->header (-type => 'text/html', -X_IMStatus => $OK, -X_sequencenum => 0);
        print "No sequence number found returning 0<BR>",
              "GetSequenceNumber OK" if $DEBUG_IM;
        return;
    }

    my ($sequence) = `type $SequenceFile` =~ /(\d+)/;

    print $query->header (-type => 'text/html', -X_IMStatus => $OK, -X_sequencenum => $sequence),
          "$sequence\n";
    print "GetSequenceNumber OK" if $DEBUG_IM;
}


sub GetNextSequenceNumber {
    my $SequenceFile = 'H:\\Source\\virs1234.dat\\src\\defattrs.dat';
    my $LOCK = 'SEQUENCE-LOCK';
    my $TIMEOUT = 10000;

    #
    # Lock access to the sequence files
    #
    my $result = SSLockMutex ($LOCK, $TIMEOUT);

    unless ($result == $SSLOCKMUTEX_OK) {
        print $query->header (-type => 'text/html', -X_IMStatus => $IN_PROGRESS);
        print "Lock Error" if $DEBUG_IM;
    }

    #
    # Get the latest versions off the Perforce server
    #
    $p4->sync (file => $SequenceFile);

    unless (open SF, $SequenceFile) {
        print $query->header (-type => 'text/html', -X_IMStatus => $ERROR);
        print "Unable to open $SequenceFile" if $DEBUG_IM;
        SSUnlockMutex ($LOCK);
        return;
    }

    binmode SF;
    read (SF, $buf, 4);
    my ($id) = unpack("L",$buf);
    close SF;

    $buf = pack ("L", ++$id);
    
    # update version.dat with sequence number
    if ($p4->edit (file => $SequenceFile) != 0) {
        print $query->header (-type => 'text/html', -X_IMStatus => $ERROR);
        print "p4 edit exited with error" if $DEBUG_IM;
        SSUnlockMutex ($LOCK);
        return;
    }

    unless (open SF, "> $SequenceFile") {
        print $query->header (-type => 'text/html', -X_IMStatus => $ERROR);
        print "Unable to open $SequenceFile" if $DEBUG_IM;
        SSUnlockMutex ($LOCK);
        return;
    }
  
    binmode SF;
    print SF $buf;
    close SF;

    if ($p4->submit (description => "IM - updating DefAttrs.dat") != 0) {
        print $query->header (-type => 'text/html', -X_IMStatus => $ERROR);
        print "p4 submit exited with error" if $DEBUG_IM;
        SSUnlockMutex ($LOCK);
        return;
    }

    $result = SSUnlockMutex ($LOCK);

    unless ($result == $SSUNLOCKMUTEX_OK) {
        print $query->header (-type => 'text/html', -X_IMStatus => $ERROR);
        print "Unlock Error" if $DEBUG_IM;
    }

    print $query->header (-type => 'text/html', -X_IMStatus => $OK, -X_sequencenum => $id),
          "$id\n";
    print "GetNextSequenceNumber OK" if $DEBUG_IM;
}

sub GetNextDailyVersionNumber {
    my $SequenceFile = 'H:\\Source\\virs1234.dat\\src\\version.dat';
    my $DateFile = 'H:\\Source\\virs1234.dat\\src\\vdate.dat';
    my $today = POSIX::strftime("m%d", localtime());
    my $MAX_ID = 99;
    my $LOCK = 'DAILYVER-LOCK';
    my $TIMEOUT = 10000;

    #
    # Lock access to the sequence files
    #
    my $result = SSLockMutex ($LOCK, $TIMEOUT);

    unless ($result == $SSLOCKMUTEX_OK) {
        print $query->header (-type => 'text/html', -X_IMStatus => $IN_PROGRESS);
        print "Lock Error" if $DEBUG_IM;
    }

    #
    # Get the latest versions off the Perforce server
    #
    $p4->sync (file => [$SequenceFile, $DateFile]);

    unless (open SF, "< $SequenceFile") {
        print $query->header (-type => 'text/html', -X_IMStatus => $ERROR);
        print "Unable to open $SequenceFile" if $DEBUG_IM;
        SSUnlockMutex ($LOCK);
        return;
    }

    unless (open DF, "< $DateFile") {
        print $query->header (-type => 'text/html', -X_IMStatus => $ERROR);
        print "Unable to open $DateFile" if $DEBUG_IM;
        SSUnlockMutex ($LOCK);
        return;
    }

    my ($id) = <SF> =~ /(\d+)/;
    my ($date) = <DF> =~ /(\d+)/;
    close SF;
    close DF;

    if (($date == $today) && ($id >= $MAX_ID)) {
        print $query->header (-type => 'text/html', -X_IMStatus => $ERROR);
        print "Error: Max sequence number exceeded" if $DEBUG_IM;
        SSUnlockMutex ($LOCK);
        return;
    }

    $id = ($date == $today) ? $id+1 : 1;

    # update version.dat with sequence number
    if ($p4->edit (file => $SequenceFile) != 0) {
        print $query->header (-type => 'text/html', -X_IMStatus => $ERROR);
        print "p4 edit exited with error" if $DEBUG_IM;
        SSUnlockMutex ($LOCK);
        return;
    }
    system ("echo $id > $SequenceFile");
    if ($p4->submit (description => "IM - updating version.dat") != 0) {
        print $query->header (-type => 'text/html', -X_IMStatus => $ERROR);
        print "p4 submit exited with error" if $DEBUG_IM;
        SSUnlockMutex ($LOCK);
        return;
    }

    #
    # update the date file if it's changed
    #
    if ($date != $today) {
        if ($p4->edit (file => $DateFile) != 0) {
            print $query->header (-type => 'text/html', -X_IMStatus => $ERROR);
            print "p4 edit exited with error" if $DEBUG_IM;
            SSUnlockMutex ($LOCK);
            return;
        }
        system ("echo $today > $DateFile");
        if ($p4->submit (description => "IM - updating vdate.dat") != 0) {
            print $query->header (-type => 'text/html', -X_IMStatus => $ERROR);
            print "p4 submit exited with error" if $DEBUG_IM;
            SSUnlockMutex ($LOCK);
            return;
        }
    }

    $result = SSUnlockMutex ($LOCK);

    unless ($result == $SSUNLOCKMUTEX_OK) {
        print $query->header (-type => 'text/html', -X_IMStatus => $ERROR);
        print "Unlock Error" if $DEBUG_IM;
    }

    print $query->header (-type => 'text/html', -X_IMStatus => $OK, -X_dailyversionnum => $id),
          "$today $id\n";
    print "GetNextDailyVersionNumber OK" if $DEBUG_IM;
}
