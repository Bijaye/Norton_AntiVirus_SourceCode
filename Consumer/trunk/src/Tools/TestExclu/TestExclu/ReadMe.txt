========================================================================
    CONSOLE APPLICATION : TestExclu (NAV Exclusion Engine 2006 Test Tool)
========================================================================
Overview:
	TestExclu is a debugger tool for NAV Exclusion Engine 2006 (N32Exclu2.dll)
allows users test this dll without installing of NAV 2006 product.

Usage:
	To use this tool with NAV, first turn off symprotect and drop TextExclu.exe 
	into NAV folder and run from there.

	TestExclu runs in 2 modes, user mode is a console application allow users
to input from a commandline.  The following are the list of the commands.
			help			Display this menu 
			automate		Execute a given input script 
			addfs			Add a file system exclusion 
			adda			Add an anomaly exclusion 
			findfs			Find a file system exclusion
			finda			Find an anomaly exclusion 
			get				Get an exclusion using id 
			isfa			Is file system exculsion? state = 0 if not 
			isa				Is anomaly exclusion? state = 0 if not 
			list			List all exclusion items 
			remove			Removes a exculsion by id 
			replace			Replace (edit) an existing exclusion 
			reload			Reloads states from ccSettings 
			save			Save state to ccSettings 
			stateflag		Set exclusion state by turn each bit ON or OFF 
			tofile			Save exclusion settings into a file 
			fromfile		Load exclusion settings from a file 
			default			Load default setting to manager 
			setauto+		Set AutoCommit on 
			setauto-		Set AutoCommit off 
			isauto			Check the state of AutoCommit 
			viewsettings		View stored exclusions in ccSetttings 
			exit			Exit program 

Automation mode usage:
	TestExclu.exe [INPUT_FILE_NAME]
format of input file:
	Place one command of the following each line into a text file.
	
	add a file system exclusion: addfs [dir_path] [time_expiry] [state] [0 or 1 for isOem]
	example:	addfs c:\windows 0 7 0
							
	add an anomaly exclusion: adda [VID] [GUID] [time_expiry] [name] [state]
	example:	adda 100 {abcd-efg-opq-rstu} 3600 newVirus 3
							
	find a file system exclusion: findfs [dir_path]
	example:	finfs c:\windows
							
	find an anomaly exclusion: finda [VID] [GUID]	
	example:	finda 101 {nbe9-adbad-ewrd-sdf}
							
	get a exclusion: get [Exclusion_ID]	
	example:	get 1
							
	is file system exclusion: isfs [dir_path] [1 or 0 for isOem] [state]
	example:	isfs c:\windows 0 0
						
	is anomaly exclusion: isa [VID][GUID] [state]
	example:	isa 101 {abc-def-gede-ef} 1
						
	list all exclusion in manager: list
		
	remove an exclusion: remove [Exclusion_ID]
	example:	remove 1
							
	replace file system exclusion: replace [Exclusion_ID] [dir_path] [time_expiry] [state]
	example:	replace 1 c:\windows\systems 4800 15
	
	replace anomaly exclusion: replace [Exclusion_ID] [VID] [GUID] [time_expiry] [name] [state]
	example:	replace 2 999 {new-guid} new_name 3
							
	reload form ccSettings: reload
		
	save state to ccSettings: save
		
	set exclusion state: stateflag [Exclusion_ID] [state] [0 or 1]
	example:	stateflag 2 1 1
							
	save into a file: tofile [file_name_path]
	example:	tofile c:\data.dat
			
	load from a file: formfile [file_name_path] [state] [type]
	example:	formfile c:\data.dat 0 0 (state=0 type=0 for all exclusions)
					
	load default to manager: default [state] [type]
	example:	default 0 0 (state=0 type=0 for all exclusions)
		
	set auto-commit on: setauto+
		
	set auto-commit off: setauto-
		
	check state of auto-commit: isauto
		
	view exclusions in ccSettings: viewsettings
	
To store output into a file:
	TestExclu.exe [INPUT_FILE_NAME] >[OUTPUT_FILE_NAME]
	example:	TestExclu.exe	input.txt >output.txt
		
		
Open Issues:
	- In automation mode, all input data must have no space in them.  
	for example "c:\prgram files" will not be taken as one string of input.
		
		
Logs:
	- Qi Guo 5/10/2005: V 2.0 complete. Added support for automations

	