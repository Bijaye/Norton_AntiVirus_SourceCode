#----------------------------------------------------------------------------------------
# avisgatesampleexport.pl
# The procedure backs up two files (.sample & .attributes) in one .zip file for each sample 
# stored in the Data Storage machine in the Gateway area.  
# The backup files are stored in the weekday (Mon, Tue, Wed, Thu, Fri, Sat, Sun)   
# subdirectory of the backup directory.
#----------------------------------------------------------------------------------------     
# History: 03/21/2000  S.N. Krasikov - created 
#          04/15/2000  S.N. Krasikov - deleted creation of a "day" subdirectory
#                                      for backup files.                
#          06/05/2000  S.N. Krasikov  - replaced return with exit when there are no samples available to export 
#          06/08/2000  S.N. Krasikov  - added check for successful zipping 
#----------------------------------------------------------------------------------------
#

    use File::Copy;
    use Win32::File;

    # NOTE: 	Please go through the customization block
    #		and change the directories and files' names
    #		and paths where it is needed.  
    #---------------------------------------------
    #   customization block begins
    #---------------------------------------------
    # sample storage directory  
    # ------------------------ 
        $samplestoragedir="\\\\data\\samples";
    # ------------------------ 
    # zip file name  
    # ------------------------ 
    # main backup directory  
    # ------------------------ 
        $mainbackupdir="\\\\data\\gwbackup";
    # ------------------------ 
    # zip program  
    # ------------------------ 
	$zipexe  = "d:\\tools\\zip.exe";       
    #---------------------------------------------
    #   customization block ends
    #---------------------------------------------

        # check for backup dir
        if (!(-d $mainbackupdir))
        {
#            if (!mkdir ("$mainbackupdir", 0)) {
#	        print ("Unable to create directory $mainbackupdir\n");
#                exit;
#            }
        }
        # create a "weekday" directory if it does not exist 
	#$currtime = time(); 
       	#$ctime = localtime($currtime);
        #@words = split (/ /, $ctime);
        #$result = $mainbackupdir =~ /\\$/;
        #if (!$result) {
        #    $mainbackupdir = $mainbackupdir . "\\";
        #}
        #$backupdir2=$mainbackupdir . $words[0];
        #if (!(-d $backupdir2))
        #{
        #    if (!mkdir ("$backupdir2", 0)) {
	#        print ("Unable to create directory $backupdir2\n");
        #        exit;
        #    }
        #}




        $result = $samplestoragedir =~ /\\$/;
        if (!$result) {
          $samplestoragedir = $samplestoragedir . "\\";
        }

	# Read the specified files and store the names it contains in 
    	# an array.
    
	$sampledir = $samplestoragedir . "*.sample";
    	$dirlist = $samplestoragedir . "dir.list"; 
        
    	system ("dir /b /aa $sampledir > $dirlist");

	open (FILEHANDLE, "<$dirlist");

	my @names = <FILEHANDLE>;
    	chop @names;
    	close FILEHANDLE;
    	unlink $dirlist;
    	# if the list is empty, return.
    	my $numberOfEntries;
    	$numberOfEntries = @names;  
#    	print ("Total dir entries = $numberOfEntries\n");

    	# If there are no files in the SampleStorage, exit the program now.
    	if ($numberOfEntries == 0)
    	{
        	exit;
    	}

     

	# For each name in the directory that .sample zip two files: <checksum>.sample
        # <checksum>.attributes   
    	foreach (@names)
    	{
                $filename = $_; 
	        @array = split (/\./, $filename);
                #$subdir = $samplestoragedir . "$_";
		#next unless -d $subdir;
		#next if $_ eq ".";
		#next if $_ eq "..";
		$sampleFile = $array[0] . ".sample";
		$sampleAttributesFile = $array[0] . ".attributes";                                      
                $sampleFile = $samplestoragedir . $sampleFile;
                $sampleAttributesFile = $samplestoragedir . $sampleAttributesFile;                                 
                if ((-e $sampleAttributesFile))
	        {
                        if(-s $sampleFile) {
                        	if(-s $sampleAttributesFile) {
#	         	       		Win32::File::GetAttributes($sampleAttributesFile, $Attributes) || die $!; 		        
#	 	        		if ( $Attributes == 32 ) {
						$zipname = $array[0] . ".zip";                
                				$zippath = $mainbackupdir . "\\";
                				$zippath = $zippath . $zipname;		                																														
						system("$zipexe $zippath $sampleFile $sampleAttributesFile\n");     																
					               if   (-e $zippath)  { 
			              	                           Win32::File::SetAttributes($sampleAttributesFile, 128); 		        
							Win32::File::SetAttributes($sampleFile, 128); 		        
                                                                                     }


#	                       		}
        	        	}
                	 }

	        }
        }
       	exit;
