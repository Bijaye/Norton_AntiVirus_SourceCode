BaseTest.exe v 1.0  (Test tool for NAV 2006)
--------------------------------
Overview:
	BaseTest.exe is a test tool to show DLL 's preferred base address 
	and actual loaded base address. To Test DLL rebasing for potential 
	performance gain, if preferred base address and actual loaded base 
	address does not match, a collision has accours while loading this DLL.

Capabilities:
	- Display all unloaded DLL's preferred base addresses in a folder (dir)
	- Allows user to modify names of processes, and display any collisions 
	were caused by load modules under these processes.  
	(Displays all modules where preferred address does not match loaded base)
	- Run test in a loop every 1 second interval
	
TODO:
	- Command line support to run tool silently to support automation
	- Save output to a file
	- Clear output button
	- Add start stop botton to support dynamic dll loading test. //Done 
	- also need to log output for this case. 
	
Logs:
	- Qi: 6/30/2005 Added capability of run test continuesly untill user hits stop
	- Qi: 6/14/2005 Code complete V 1.0



Qi
x8529