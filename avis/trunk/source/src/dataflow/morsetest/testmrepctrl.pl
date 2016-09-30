for ($i = 1; $i <= 100; $i++)
{
    @args = ("perl", "mrepctrl.pl", "--UNC",
             "\\\\avn-mars\\BactaTest\\Samples\\4",
             "--Parameter", "Param1", "Param2",
             "--Cookie", "4",
             "--ProcessId", "$i");

    print ("Macro Replication Controller $i\n");
    system (@args);
}
