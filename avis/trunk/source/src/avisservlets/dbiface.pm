package dbiface;

use Win32::ODBC;
require Exporter;

@ISA = qw (Exporter);
@EXPORT = qw(db2_time run_query results_to_table);

my $DB_ERROR = undef;
my $local_error = "";
sub db2_time
{
# if (sprintf(buffer, "%d-%02d-%02d-%02d.%02d.%02d", timeStamp.year,
#            timeStamp.month, timeStamp.day, timeStamp.hour,
#            timeStamp.minute, timeStamp.second))
    my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime;
    $year += 1900;
    return "$year-$mon-$mday-$hour.$min.$sec";
}

# This has been fairly heavily modified since it's inception.
# No longer does it take the actual SQL, but the name of the query
sub run_query
{
    my ($dsn,$sqlquery) = @_;
    # This simply executes the SQL statement against a newly opened
    # database handle and returns that handle, leaving the caller
    # responsible for calling the "Close" method.
    my @results = ();
    
    my $dbh = new Win32::ODBC($dsn);
    if ($dbh)
      {
        if ($dbh->Sql($sqlquery))
          {
            $DB_ERROR = $dbh->Error();
            $dbh->Close();
            $dbh = undef;
            #        exit(1);
            # will never get here
          }
        else
        {
          $local_error = "Win32::ODBC::Sql returned an error";
        }
      }
    else
      {
        $local_error = "Couldn't open database handle";
      }
    return $dbh;
  }

sub results_to_txt
{
    my ($db_handle) = @_;
    # Takes database handle with unprocessed results
    my $content = "";
    my @names = $db_handle->FieldNames();
    
#---------------------------------------
# debug dump
#---------------------------------------
    foreach (@names)
    {
        $content .= "$_|";
    }
    $content .= "\n";
    while ($db_handle->FetchRow())
    {
        @values = $db_handle->Data();
        foreach (@values)
        {
            $content .= "$_|";
        }
        $content .= "\n";
    }
#---------------------------------------
    return $content;
}


sub results_to_table
{
    my ($db_handle) = @_;
    # Takes database handle with unprocessed results
    my $table = "<table align=center border=1>";
    my @names = $db_handle->FieldNames();
    
#---------------------------------------
# debug dump
#---------------------------------------
    $table .= "<tr>";
    foreach (@names)
    {
        $table .= "<td><b>$_</b></td>";
    }
    $table .= "</tr>\n";
    while ($db_handle->FetchRow())
    {
        @values = $db_handle->Data();
        $table .= "\t<tr>";
        foreach (@values)
        {
            $table .= "<td>$_</td>";
        }
        $table .= "</tr>\n";
    }
#---------------------------------------
    $table .= "</table>";
    return $table;
}

sub dump_table_to_file
{
    my ($datasource,$tablename,$tablefile) = @_;
    my $db_handle = debugdb::run_query($datasource,"select * from $tablename");
    open TABLEFILE,">$tablefile";
    print TABLEFILE debugdb::results_to_txt($db_handle);
    close TABLEFILE;
    $db_handle->Close();
}

1;
