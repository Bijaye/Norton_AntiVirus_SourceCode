use navapi;
package navapi;

######Initialise the call back io functions
$i=navapi::CallBackInit();

#########Definition files

@k=NAVEngineInit("d:\\symantec","d:\\symantec",$i,0,0);
print ("@k\n");
$HNavEngine=$k[0];
print ($HNavEngine);
@j=NAVGetVirusDBInfo($HNavEngine);
#@j=NAVGetNavapiVersionString(10);
print ("\n");
print ("@j\n");
$lpdb=$j[1];
bless $lpdb, "VIRUSDBINFOPtr";
print ($lpdb);
print ("\n");
$M=VIRUSDBINFO_dwVersion_get($lpdb);
print ($M);

####### deal with a virus


print ("\n");
print("scanning file\n");
@m=NAVScanFile($HNavEngine,"c:\\augie\\normal.dot","dot",1);
print ("printng results\n");
print ("@m\n");

@l=NAVGetVirusInfo($m[1],$NAV_VI_VIRUS_NAME,100);
print("@l\n");
@o=NAVFreeVirusHandle($m[1]);
@p=NAVGetVirusDefCount($HNavEngine);
print("@p\n");
print ("@o\n");
@x=NAVScanBoot($HNavEngine,"C");
print("@x\n");
@y=NAVScanMemory($HNavEngine);
print("@y\n");
$n=NAVEngineClose($HNavEngine);
print ($n);
