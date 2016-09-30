#---------------------------------------------------------------------------------------
# avisdbexport.pl
# The procedure recovers AVIS database tables: each table from a backup file. The 
# recovering is done with an IMPORT command that is part of the DB2 standard command line
# processor. The backup .zip files are stored in the <weekday>\tables
# (Mon, Tue, Wed, Thu, Fri, Sat, Sun) subdirectories of the backup directory.
#----------------------------------------------------------------------------------------     
# History: 02/21/2000  S.N. Krasikov - created 
#          03/21/2000  S.N. Krasikov added table "statustobereported" 
#          04/11/2000  S.N. Krasikov deleted table "attributes" 
#----------------------------------------------------------------------------------------

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
    # zip program  
    # ------------------------ 
	$unzippath  = "d:\\tools\\unzip.exe";       
    # ------------------------ 
    # path to backup files  
    # ------------------------ 
	$whattounzip  = "d:\\backupdb\\avisdbBackup.zip";       
    # ------------------------ 
    # SQl & BAT files location  
    # ------------------------ 
        $dir="d:\\sqlbat";
    # ------------------------ 
    # database name  
    # ------------------------ 
        $database ="avisdata";
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
        $outputfile="avisdbimport.log";  
    #---------------------------------------------
    #   customization block ends
    #---------------------------------------------

        $pathtobackup = $whattounzip;
        until ( $pathtobackup =~ /\\$/ ) {
           chop($pathtobackup);
        }

        $result = $dir =~ /\\$/;
        if (!$result) {
            $dir = $dir . "\\";
        }
        $sqlpath= $dir . "\\avisdbimport.sql";
        $batpath=$dir . "\\avisdbimport.bat";
          
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

        #remotemustbeunique table 
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
	        print ("Backup directory $dir does not exist\n");
                exit;
        }
        # create .sql script 
	open (SCRIPTFILE, ">$sqlpath");
   	print SCRIPTFILE ("connect to $database user $user using $pw\n");

        #$result = $pathtobackup =~ /\\$/;
        #if (!$result) {
        #    $pathtobackup = $pathtobackup . "\\";
        #}
        

        # attributekeys table 
        $filepath=$pathtobackup . $attributekeysFile; 
   	print SCRIPTFILE ("IMPORT FROM $filepath of ixf insert into $attributekeysTable\n");

	# AnalysisStateInfo Table
        $filepath=$pathtobackup . $analysisstateinfoFile; 
   	print SCRIPTFILE ("IMPORT FROM $filepath of ixf insert into $analysisstateinfoTable\n");

        # servers table 
        $filepath=$pathtobackup . $serversFile; 
   	print SCRIPTFILE ("IMPORT FROM $filepath of ixf insert into $serversTable\n");

        # submittors table 
        $filepath=$pathtobackup . $submittorsFile; 
   	print SCRIPTFILE ("IMPORT FROM $filepath of ixf insert into  $submittorsTable\n");

        # signatures table 
        $filepath=$pathtobackup . $signaturesFile; 
   	print SCRIPTFILE ("IMPORT FROM $filepath of ixf insert into  $signaturesTable\n");

        # sigstobeexported table 
        $filepath=$pathtobackup . $sigstobeexportedFile; 
   	print SCRIPTFILE ("IMPORT FROM $filepath of ixf insert into $sigstobeexportedTable\n");

        # analysisresults table 
        $filepath=$pathtobackup . $analysisresultsFile; 
   	print SCRIPTFILE ("IMPORT FROM $filepath of ixf insert into $analysisresultsTable\n");

        # globals table 
        $filepath=$pathtobackup . $globalsFile; 
   	print SCRIPTFILE ("IMPORT FROM $filepath of ixf insert into  $globalsTable\n");

        # analysisrequest table 
        $filepath=$pathtobackup . $analysisrequestFile; 
   	print SCRIPTFILE ("IMPORT FROM $filepath of ixf insert into $analysisrequestTable\n");

        # attributes table 
        #$filepath=$pathtobackup . $attributesFile; 
   	#print SCRIPTFILE ("IMPORT FROM $filepath of ixf insert into  $attributesTable\n");

        # remotemustbeunique table 
        $filepath=$pathtobackup . $remotemustbeuniqueFile; 
   	print SCRIPTFILE ("IMPORT FROM $filepath of ixf insert into  $remotemustbeuniqueTable\n");

        # statustobereported table 
        $filepath=$pathtobackup . $statustobereportedFile; 
   	print SCRIPTFILE ("IMPORT FROM $filepath of ixf insert into  $statustobereportedTable\n");

	print SCRIPTFILE ("DISCONNECT ALL");
        close(SCRIPTFILE);

        $pos = rindex($pathtobackup, "\\"); 
        if ($pos != -1) {
	        $logpath = substr($pathtobackup, 0, $pos);
	        $pos = rindex($logpath, "\\");
	        $logpath = substr($logpath, 0, $pos +1);
        	$logpath = $logpath . $outputfile;
        }
        else {
        	print ("Unable to create the log path $pathtobackup\n");
                exit;
        }
        
        unlink ($logpath) or warn "Can't delete $logpath, $!, continuing";

        $exdir = $pathtobackup;
        chop($exdir); 

	open (BATFILE, ">$batpath");
       	$filepath=$pathtobackup . "*.ixf"; 
	print BATFILE ("erase /q $filepath\n");
	print BATFILE ("$unzippath -j -d $exdir $whattounzip\n");
   	print BATFILE ("db2 -f $sqlpath -z $logpath\n");
   	print BATFILE ("exit\n");
       	close(BATFILE); 

        system ("$db2exe $batpath"); 

 
