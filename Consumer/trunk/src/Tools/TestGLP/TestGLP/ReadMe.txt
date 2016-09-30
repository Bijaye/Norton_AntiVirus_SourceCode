TestGLP.exe
Overview:
	TestGLP infects all Generic Load Points listed in CCEraser Functional Specification.
User can modify these load points to point to appropriate path for dumb viruses.  This tests tool is used
To test pre-scan capabilities using ccEraser definitions.

Usages:

	- Registry Values: A list of registry values as a part of generic load points can be chosen and modified
	with path to dumb viruses.  
	
	- Startup Folders:  Retrieves possible registry keys to specify startup folders and display in a list control
	Allows users to chose which key to modify startup folders. 
	
	- Batch Files and INI files: 
		-autoexec.nt: NT System only, currently tool allows make new file on a 9x system if file does not exist
		-winstart.bat: 9x system only, currently tool allows make new file on NT system if file does not exist
		-autoexec.bat: 9x system only, currently tool allows make new file on NT system if file does not exist
		-system.ini: 9x system will modify boot section, shell and scnsave.exe keys, NT system will modify appropriate 
					Registry values.
		-win.ini: 9x system will modify windows section, load and run keys, NT system will modify appropriate 
					Registry values.
			
			
Suggestion and Notes:
		- Need to fix Regkeys dealing with COM objects.
		- Loading definitions from xml file will be nice, avoid overhead from future modifications of ccEraser func spec.
					
Logs:

- Qi Guo 4/6/2005: Changes on ccEraser Functional Specification:
	- 6 more Reg keys, total: 38 Reg keys, 3 general places for startup folders, 5 batch and ini files.

- Qi Guo 4/1/2005: Code complete
			32 Reg keys, 3 general places for startup folders, 5 batch and ini files.
			

