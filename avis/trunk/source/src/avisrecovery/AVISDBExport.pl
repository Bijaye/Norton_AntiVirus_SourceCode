#---------------------------------------------------------------------------------------
# avisdbexport.pl
# The procedure backups AVIS database tables: each table in a separate file. The backup
# is done with an EXPORT command that is part of the DB2 standard command line processor. 
# The files are stored in the weekday\tables (Mon, Tue, Wed, Thu, Fri, Sat, Sun)
# subdirectory of the backup directory.
#
#----------------------------------------------------------------------------------------     
# History: 02/21/2000  S.N. Krasikov - created 
#          03/21/2000  S.N. Krasikov added table "statustobereported" 
#          04/11/2000  S.N. Krasikov deleted the "attribute" table
#          04/25/2000  S.N. Krasikov changed the user's id to "db2admin" #----------------------------------------------------------------------------------------

    use File::Copy;

    # NOTE: 	Please go through the customization block
    #		and change the directories and files' names
    #		and paths where it is needed.  
    #---------------------------------------------
    #   customization block begins
    #---------------------------------------------
    # db2 cmd 
    # ------------------------ 
        $db2exe="d:\\sqllib\\bin\\db2cmd.exe"; 
    # ------------------------ 
    # SQl & BAT files location  
    # ------------------------ 
        $dir="d:\\sqlbat\\";
    # ------------------------ 
    # main backup directory  
    # ------------------------ 
        $mainbackupdir="\\\\Rushmore\\acbackup";
    # ------------------------ 
    # database name  
    # ------------------------ 
        $database ="avisdata";
    # ------------------------ 
    # zip file name  
    # ------------------------ 
        $ziptail="avisdatabase";
    # ------------------------ 
    # user's name  
    # ------------------------ 
        $user="db2admin";
    # ------------------------ 
    # user's password  
    # ------------------------ 
        $pw="hicl";  
    # ------------------------ 
    # output file  
    # ------------------------ 
        $outputfile="avisdbexport.log";  
    # ------------------------ 
    # zip program  
    # ------------------------ 
	$zippath  = "d:\\tools\\zip.exe";       
    #---------------------------------------------
    #   customization block ends
    #---------------------------------------------

        $result = $dir =~ /\\$/;
        if (!$result) {
            $dir = $dir . "\\";
        }
        $sqlpath= $dir . "avisdbexport.sql";
        $batpath=$dir . "avisdbexport.bat";
          
        # analysisstate table 
        # 
        $analysisstateinfoTable="analysisstateinfo";
	$analysisstateinfoFile=$analysisstateinfoTable . ".ixf"; 

        # submittors table 
        #
        $submittorsTable="submittors";
	$submittorsFile=$submittorsTable . ".ixf"; 

        # sigstobeexported table 
        #
        $sigstobeexportedTable="sigstobeexported";
	$sigstobeexportedFile=$sigstobeexportedTable . ".ixf"; 

        # servers table 
        #
        $serversTable="servers";
	$serversFile=$serversTable . ".ixf"; 

        # remotemustbeunique table 
        #
        $remotemustbeuniqueTable="remotemustbeunique";
	$remotemustbeuniqueFile=$remotemustbeuniqueTable . ".ixf"; 

        # signatures table 
        #
        $signaturesTable="signatures";
	$signaturesFile=$signaturesTable . ".ixf"; 

        # globals table 
        #
        $globalsTable="globals";
	$globalsFile=$globalsTable . ".ixf"; 

        # analysisresults table 
        #
        $analysisresultsTable="analysisresults";
	$analysisresultsFile=$analysisresultsTable . ".ixf"; 

        # analysisrequest table 
        #
        $analysisrequestTable="analysisrequest";
	$analysisrequestFile=$analysisrequestTable . ".ixf"; 

        # attributekeys table 
        #
        $attributekeysTable="attributekeys";
	$attributekeysFile=$attributekeysTable . ".ixf"; 

        # attributes table 
        #
        #$attributesTable="attributes";
	#$attributesFile=$attributesTable . ".ixf"; 

        # statustobereported table 
        #
        $statustobereportedTable="statustobereported";
	$statustobereportedFile=$statustobereportedTable . ".ixf"; 

        # check for sql & bat dir
        if (!(-d $dir))
        {
            if (!mkdir ("$dir", 0)) {
	        print ("Unable to create directory $dir\n");
                exit;
            }
        }

        # check for bachup dir
#        if (!(-d $mainbackupdir))
#        {
#	    print ("Backup directory $mainbackupdir does not exist\n");
#            exit;
#        }

        # create a "weekday" directory if it does not exist 
	$currtime = time(); 
	my ($sec, $min, $hr, $day, $month, $year, $wday, $yday, $fl) = localtime (time);
    	$year += 1900;  
        @array = ("Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec");
                   
        $zipname =  $day . $array[$month];
        $zipname = $zipname . $year;
        $zipname = $zipname . ".";
        $zipname = $zipname . $ziptail;
        $zipname = $zipname . ".zip";

       	$ctime = localtime($currtime);
        @words = split (/ /, $ctime);
        $result = $mainbackupdir =~ /\\$/;
        if (!$result) {
            $mainbackupdir = $mainbackupdir . "\\";
        }
        $backupdir2=$mainbackupdir . $words[0];
        if (!(-d $backupdir2))
        {
            if (!mkdir ("$backupdir2", 0)) {
	        print ("Unable to create directory $backupdir2\n");
                exit;
            }
        }

        # log file path
        $backupdir3=$backupdir2 . "\\";

        # create "tables" directory if it does not exist
        $backupdir4=$backupdir3 . "tables";
        if (!(-d $backupdir4))
        {
            if (!mkdir ("$backupdir4", 0)) {
	        print ("Unable to create directory $backupdir4\n");
                exit;
            }
        }

        # create .sql script
        $filepath1=$backupdir4 . "\\"; 
        $logpath=$backupdir3 . $outputfile;

	open (SCRIPTFILE, ">$sqlpath");
   	print SCRIPTFILE ("connect to $database user $user using $pw\n");

	# AnalysisStateInfo Table
        $filepath=$filepath1 . $analysisstateinfoFile; 
   	print SCRIPTFILE ("EXPORT TO $filepath of ixf select * from  $analysisstateinfoTable\n");

        # submittors table 
        $filepath=$filepath1 . $submittorsFile; 
   	print SCRIPTFILE ("EXPORT TO $filepath of ixf select * from  $submittorsTable\n");

        # sigstobeexported table 
        $filepath=$filepath1 . $sigstobeexportedFile; 
   	print SCRIPTFILE ("EXPORT TO $filepath of ixf select * from  $sigstobeexportedTable\n");

        # servers table 
        $filepath=$filepath1 . $serversFile; 
   	print SCRIPTFILE ("EXPORT TO $filepath of ixf select * from  $serversTable\n");

        # remotemustbeunique table 
        $filepath=$filepath1 . $remotemustbeuniqueFile; 
   	print SCRIPTFILE ("EXPORT TO $filepath of ixf select * from  $remotemustbeuniqueTable\n");

        # signatures table 
        $filepath=$filepath1 . $signaturesFile; 
   	print SCRIPTFILE ("EXPORT TO $filepath of ixf select * from  $signaturesTable\n");

        # globals table 
        $filepath=$filepath1 . $globalsFile; 
   	print SCRIPTFILE ("EXPORT TO $filepath of ixf select * from  $globalsTable\n");

        # analysisresults table 
        $filepath=$filepath1 . $analysisresultsFile; 
   	print SCRIPTFILE ("EXPORT TO $filepath of ixf select * from  $analysisresultsTable\n");

        # analysisrequest table 
        $filepath=$filepath1 . $analysisrequestFile; 
   	print SCRIPTFILE ("EXPORT TO $filepath of ixf select * from  $analysisrequestTable\n");

        # attributekeys table 
        $filepath=$filepath1 . $attributekeysFile; 
   	print SCRIPTFILE ("EXPORT TO $filepath of ixf select * from  $attributekeysTable\n");

        # attributes table 
        #$filepath=$filepath1 . $attributesFile; 
   	#print SCRIPTFILE ("EXPORT TO $filepath of ixf select * from  $attributesTable\n");

        # statustobereported table 
        $filepath=$filepath1 . $statustobereportedFile; 
   	print SCRIPTFILE ("EXPORT TO $filepath of ixf select * from  $statustobereportedTable\n");

	print SCRIPTFILE ("DISCONNECT ALL");
        close(SCRIPTFILE);

        # delete the old log file
        unlink ($logpath) or warn "Can't delete $logpath, $!, continuing";

        # delete the old zip file
        $filepath=$filepath1 . $zipname;
        unlink ($filepath) or warn "Can't delete $filepath, $!, continuing";
        $whattozip=$filepath1 . "*.*";  

        # create the batch file
 
	open (BATFILE, ">$batpath");
#   	print BATFILE ("ECHO OFF \n");
   	print BATFILE ("db2 -f $sqlpath -z $logpath\n");

	print BATFILE ("$zippath -j $filepath $whattozip\n");
        $filetocheck = $backupdir3 . "export.done";
	print BATFILE ("dir > $filetocheck\n" );
       	$filepath=$filepath1 . "*.ixf"; 
	print BATFILE ("erase /q $filepath\n");
	print BATFILE ("exit\n");

       	close(BATFILE); 

        # run the batch file
        system ("$db2exe  $batpath"); 
	until (-e $filetocheck)
        {
            sleep (1);
        }
        unlink ($filetocheck)
       
