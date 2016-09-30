for ($i = 1; $i <= 100; $i++)
{
    @args = ("perl", "manalysis.pl", "--UNC",
             "\\\\avn-mars\\BactaTest\\Samples\\6",
             "--Parameter", "Param1", "Param2",
             "--Cookie", "6",
             "--ProcessId", "$i");

    print ("Macro Analysis $i\n");
    system (@args);
}
