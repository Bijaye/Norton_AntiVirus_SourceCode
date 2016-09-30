for ($i = 1; $i <= 100; $i++)
{
    @args = ("perl", "vrep.pl", "--UNC",
             "\\\\avn-mars\\BactaTest\\Samples\\5",
             "--Parameter", "Param1", "Param2",
             "--Cookie", "5",
             "--ProcessId", "$i");

    print ("Macro Replication Engine $i\n");
    system (@args);
}
