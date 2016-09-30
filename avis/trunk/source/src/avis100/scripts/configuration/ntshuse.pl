use strict;

use Getopt::Long;

use AVIS::Local;
use AVIS::Logger;
use AVIS::Macro::Connection;


my $permanent = 0;
my $sleeptime = 10;
my $timeout = 60;

my ($server, $volume, $username, $password, $usedefault, $registerasdefault);
my ($connect, $disconnect) = (0, 0);


my $logfile = $isdnROOT."/AvisConnection.log";
#clean up the log file
unlink($logfile) if (-e $logfile);

my $logger = new AVIS::Logger();
$logger->localFile($logfile);
$logger->screenEcho(1);      # Make it noisy
my $msg = "Running $0";
foreach (@ARGV) { $msg = $msg." ".$_;}
$logger->logString($msg);


GetOptions("server=s" => \$server,
            "volume=s" => \$volume,
            "connect" => \$connect,
            "disconnect" => \$disconnect,
            "username=s" => \$username,
            "password=s" => \$password,
            "permanent" => \$permanent
);

$msg  = "Server: ".(defined($server) ? $server : "none").", ";
$msg .= "Volume: ".(defined($volume) ? $volume : "none").", ";
$msg .= "User: $username, " if defined $username;
$msg .= "Password: $password, " if defined $username;
$msg .= "Action: disconnect, " if $disconnect;
$msg .= "Action: connect, " if $connect;
$msg .= "Action: none, " if (!$connect && !$disconnect);
$msg .= "Permanent: ".($permanent ? "yes" : "no").".";
$logger->logString($msg);


if (scalar @ARGV || !defined($server) || !defined($volume) || (!$connect && !$disconnect)) {
    usage();
    exit(100);
}

if ($< != 0) {
   $logger->logString("Real user id is $< instead of 0. You need to be root to run this script, punk!.");
   exit(100);
}


my $conn = new AVIS::Macro::Connection($server, $volume);
#set the username and passwords if they've been specified on the command line; else use the defaults
if (defined($username)) {
 	$conn->Username($username);
   if (defined($password)) {
  	  $conn->Password($password);
   }
}


if ($disconnect) {
    if ($conn->Disconnect()) {
          $logger->logString("Failed to disconnect from ".$conn->{Volume}." on ".$conn->{Server});
          die("Aborting...");
    }
    $logger->logString("Disconnected from ".$conn->{Volume}." on ".$conn->{Server});
}
if ($connect) {
    my $rc;
    if ($rc = $conn->Connect()) {
       $logger->logString("Failed to connect to ".$conn->{Volume}." on ".$conn->{Server}." ($rc)");
       die("Aborting...");
    }
    $logger->logString("Connected to ".$conn->{Volume}." on ".$conn->{Server});
}

#keep running after the initial mount
if ($permanent) {
  if (!$connect) {
    $logger->logString("permanent flag can be used only when connecting");
    exit(100);
  }
  while (AVIS::Macro::Connection::IsMounted($conn->{Server}, $conn->{Volume})) {
    if ($conn->IsAccessible()) {#the connection is still active
       sleep $sleeptime;
    }
    else {  #the connection has been terminated
       $logger->logString("The volume ".$conn->{Volume}." on ".$conn->{Server}." is no longer accessible.");

       #unmount the remote drive and log off
       my $rc = $conn->Disconnect();
       if (($rc != $err_ok) && ($rc != $err_logout_failed)) {
         $logger->logString("Failed to disconnect from ".$conn->{Volume}." on ".$conn->{Server});
         die("Aborting...");       
       }       

       #loop until the contact is reestablished or a nasty error happens
       while(1) {
         my $starttime = 0;
         #wait if no netbios server service is found on the machine (it could be rebooting)
         while(($rc = AVIS::Macro::Connection::IsServerRunning($conn->{Server})) == 0) {
           sleep($sleeptime);
           if (!((time - $starttime) % $timeout)) {
              $logger->logString($conn->{Server}." has not been accessible since ".localtime($starttime));
#               die("Aborting...");                   
           }
         }
       
         if ($rc == $err_command_failed) {
           die("Aborting...");              
         }
       
         #the server service is running on the server
         #check whether it accepts connections
         if ($rc == 1) {
           #we get server_not_found if the machine is not running and login_failed if the server service is not running on the NT machine.       
           #wait until the machine is up
           while(($rc = $conn->ServerLogin()) != $err_ok) {
             sleep($sleeptime);
             if (!((time - $starttime) % $timeout)) {
               $logger->logString($conn->{Server}." has not been accessible since ".localtime($starttime));
  #               die("Aborting...");
             }
             if (($rc == $err_server_not_found)||($rc == $err_login_failed)) {}
             elsif ($rc == $err_already_logged_in) {
                 if ($conn->ServerLogout() != $err_ok) {
                   $logger->logString("Unable to log off from ".$conn->{Server});
                   die("Aborting...");
                 }           
             }
             else {
                $logger->logString("Login attempt returned $rc");
                die "Dying in horrible pain...";
             }
           }
           #we can log on. now, log out
           if ($conn->ServerLogout() != $err_ok) {
               $logger->logString("Unable to log off from ".$conn->{Server});
               die("Aborting...");
           }
           last;
         }
       }

       if ($conn->Connect()) {
          $logger->logString("Failed to connect to ".$conn->{Volume}." on ".$conn->{Server});
          die("Aborting...");
       }
       if (!$conn->IsAccessible()) {
         $logger->logString("Failed to reconnect to ".$conn->{Volume}." on ".$conn->{Server});
         die("Aborting...");
       }
       $logger->logString("Succeeded in reconnecting to ".$conn->{Volume}." on ".$conn->{Server});
    }
  }
  
  $logger->logString($conn->{Volume}." on ".$conn->{Server}." has been unmounted by another process. Exiting.");  
}


exit;


sub usage {
my $msg = "Usage: $0 -server servername -volume volumename";
$msg .= " ((-connect [-username username [-password password]] [-permanent])";
$msg .= " |(-disconnect))";
$msg .= "-connect: connects to volume on server\n";
$msg .= "-disconnect: disconnects from volume on server\n";
$msg .= "-permanent: runs in loop and reconnects to the server if the connection gets terminated\n";
$msg .= "You must be root to run this script.\n";

print STDERR $msg;
$logger->logString("Incorrect syntax. Exiting...");
}
