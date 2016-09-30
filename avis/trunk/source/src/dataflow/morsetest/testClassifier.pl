for ($i = 1; $i <= 100; $i++)
{
    @args = ("perl", "classifier.pl", "--UNC",
             "\\\\avn-mars\\BactaTest\\Samples\\3",
             "--Parameter", "Param1", "Param2",
             "--Cookie", "3",
             "--ProcessId", "$i");

    print ("Classifier $i\n");
    system (@args);
}
