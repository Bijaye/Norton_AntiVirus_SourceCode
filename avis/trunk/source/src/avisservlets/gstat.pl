#!/perl/bin/perl
use dbiface;
use canned;

#------------------------------------------------------------------
# Each query requires the following three methods
#------------------------------------------------------------------
# 1) Query Selection & Composition
# 2) Data Retrieval
# 3) Result set transformation
#------------------------------------------------------------------
# This merely creates the simple introductory page with a list of
# available queries and some summary information about the current
# database state. 
#------------------------------------------------------------------
sub stat_open_samples
  {
    $sql = "select count(localcookie) from analysisrequest " . 
      "where finished is null";
    my $db_handle = dbiface::run_query("AVISDATA",$sql);
    my $count = "";
    if ($db_handle)
      {
        $db_handle->FetchRow();
        my ($count) = $db_handle->Data();
      }
    $db_handle->Close();
    return $count;
  }
sub stat_finished_samples
  {
    $sql = "select count(localcookie) from analysisrequest " . 
      "where finished is not null";
    my $db_handle = dbiface::run_query("AVISDATA",$sql);
    my $count = "";
    if ($db_handle)
      {
        $db_handle->FetchRow();
        my ($count) = $db_handle->Data();
      }
    $db_handle->Close();
    return $count;
  }
sub get_summary_information
  {
    my $content = "<br>";
    my $open_samples = stat_open_samples();
    my $finished_samples = stat_finished_samples();
    my $sql = "select NewestSeqNum,NewestBlssdSeqNum from globals where alwayszero = 0";

    my $db_handle = dbiface::run_query("AVISDATA",$sql);
    if ($db_handle)
      {
        $db_handle->FetchRow();
        my ($newest,$blessed) = $db_handle->Data();
        $content .= "<table>";
        $content .= "<tr><td>Open Samples</td><td>$open_samples</td></tr>";
        $content .= "<tr><td>Finished Samples</td><td>$finished_samples</td></tr>";
        $content .= "<tr><td>Latest Unblessed Definition Package :</td><td>" . sprintf("%08d",$newest) . "</td></tr>";
        $content .= "<tr><td>Latest Blessed Definition Package   : </td><td>" . sprintf("%08d",$blessed) . "</td></tr>\n";
        $content .= "</table>";
        $db_handle->Close();
      }
    else
      {
        $content .= "Error retrieving summary information<br>\n";
      }
    return $content;
  }
#------------------------------------------------------------------
#------------------------------------------------------------------
sub build_front_page
  {
    my $content = "";
    #--------------------------------------------------------
    # Summary information content
    #--------------------------------------------------------
    $content .= "<hr>\n";
    $content .= get_summary_information();
    $content .= "<hr>\n";
    #--------------------------------------------------------
    # Form Header
    #--------------------------------------------------------
    $content .= "<form action=\"/avis/cgi/gstat.pl\" method=\"get\">\n";
    #--------------------------------------------------------
    # 
    #--------------------------------------------------------
    # Return type selection
    #--------------------------------------------------------
    $content .= "Would you like the results in :<br>\n";
    $content .= "\t<input type=\"radio\" name=\"format\"";
    $content .= " value = \"html\"><b>HTML format</b>\n";
    $content .= "or <input type=\"radio\" name=\"format\"";
    $content .= " value = \"csv\"><b>Comma Separated format</b>\n";
    $content .= "<hr>\n";
    #--------------------------------------------------------
    #--------------------------------------------------------
    #--------------------------------------------------------
    #--------------------------------------------------------
    # Drop radio buttons with the available queries
    #--------------------------------------------------------
    my $descriptions = canned::available_queries();
    foreach $key(keys %$descriptions)
      {
        $content .="\t<input type=\"radio\" name=\"query\" value = ";
        $content .="\"$key\"><b>$$descriptions{$key}</b><br>\n";
      }
    #--------------------------------------------------------
    #--------------------------------------------------------
    $content .= "<input type=\"Submit\" value=\"Run Selected Query\"";
    $content .= "align=\"MIDDLE\">\n";
    #--------------------------------------------------------
    $content .= "</form>\n";
  }

sub fetch_results
  {
    my ($query) = @_;
    my $sql = get_sql($query);
    my $db_handle = dbiface::run_query("AVISDATA",$sql);
    my $content = results_to_table($db_handle);
    return $content;
  }
sub build_content
  {
    my ($query_string) = @_;
    my $content = "";
    if ($query_string eq "")
      {
        # Build front page
        $content = build_front_page();
      }
    else
      {
        my ($name,$value) = split /\=/,$query_string,2;
        if ($name eq "query")
          {
            $content = fetch_results($value);
          }
      }
    return $content;
  }

sub parse_cgi_parms
  {
    my ($query_string) = @_;
    # I'm just not doing enough complex cgi processing to warrant use
    # of the cgi.pm module
    my %parm_hash = ();
#    print "Parsing CGI parameters\n";

    foreach (split /\&/,$query_string)
      {
        my ($name,$value) = split /\=/;
        $parm_hash{$name} = $value;
#        print "$name = $value\n";
      }
    return %parm_hash;
  }

sub invalid_format_response
  {
    print "Unknown response format";
  }
sub invalid_query_response
  {
    print "Unknown query name";
  }

sub validate_query
  {
    my ($query_name) = @_;
    # If this isn't quite correct, it's close.
    return grep $query_name,('custsamp','custdet','sampage','opensamp','adminfo','cursigs','sampdet');
  }

sub run_user_query
  {
    my ($query_name,$response_type,%parm_hash) = @_;
    
    my $content_type = canned::get_content_type($response_type);

    # Execute the query itself For now I'm using a simple if/elsif
    # construct to route the query resolution request.  I would like
    # to switch this (in my copious spare time, of course) to use a
    # hash to examplar construct to jack up the flexibility and
    # offload ALL of the individual query details to a separate
    # module. 
    
    my $body = "";
    
    # These methods are all going in canned.pm
    if ($query_name eq 'custsamp')
      {
        $body .= canned::query_customer_samples($response_type,%parm_hash);
      }
    elsif($query_name eq 'custdet')
      {
        $body .= canned::query_customer_detail($response_type,%parm_hash);
      }
    elsif($query_name eq 'opensamp')
      {
        $body .= canned::query_open_samples($response_type,%parm_hash);
      }
    elsif($query_name eq 'adminfo')
      {
        $body .= canned::query_admin_info($response_type,%parm_hash);
      }
    elsif($query_name eq 'cursigs')
      {
        $body .= canned::query_current_signatures($response_type,%parm_hash);
      }
    elsif($query_name eq 'sampdet')
      {
        $body .= canned::query_sample_detail($response_type,%parm_hash);
      }
    elsif($query_name eq 'sampage')
      {
        $body .= canned::query_aged_samples($response_type,%parm_hash);
      }
    else
      {
        # Invalid query.  This should have been caught earlier
      }
    # Get the content Build the Content-Length response header 
    # Print the whole schmegeggy to stdout
    
    my $content = "Content-Type: $content_type\n";
    $content .= "Content-Length: " . length($body);
    $content .= "\n\n$body";
    return $content;
  }

sub query_select_form
  {
    my $content = "<form action=\"/avis/cgi/gstat.pl\">";
    $content .= "<input type=radio name=query value=custsamp>Sample count by distinct customer<br>";
#    $content .= "<input type=radio name=query value=custdet>Customer Detail (unavailable)<br>";
    $content .= "<input type=radio name=query value=opensamp>List of Currently open Samples<br>";
#    $content .= "<input type=radio name=query value=adminfo>Administrative and load information<br>";
#    $content .= "<input type=radio name=query value=cursigs>Current Live Definition packages<br>";
    $content .= "<input type=radio name=query value=sampage>Samples less than <input type=text name=sampleage> days old<br>";
    $content .= "<input type=radio name=query value=sampdet>Detailed Information about sample number:  <input type=text name=cookie><br>";
    $content .= "<hr>\n";
    $content .= "Please select the data format:<br>\n";
    $content .= "<input type=radio name=format checked=true value=html>html (live to your browser)<br>";
    $content .= "<input type=radio name=format value=csv>Comma-delimited text file (easily imported into your spreadsheet of choice)<br>";
    $content .= "<input type=radio name=format value=tab>Tab-delimited text file (also easily imported)<br>";
    $content .= "<hr><input type=submit>\n";
    $content .= "</form>";
    return $content;
  }
sub front_page
  {
    # This is all old code
    print "Content-type: text/html\n";
    print "\n";
    print "<html><body>\n";
    print "Anti Virus Immune System Gateway Statistics<br>\n";
    print get_summary_information();
    print "<hr>";
    # Yuk.  Theres
    print query_select_form();
    print "</body></html>\n";
    
  }
sub validate_format
  {
    my ($candidate) = @_;
    return (($candidate eq 'html') || $candidate eq 'csv');
  }
sub run
  {
    if ($ENV{'QUERY_STRING'})
      {
        # Debug
        #        print "Content-Type: text/html\n\n";
        my %parm_hash = parse_cgi_parms($ENV{'QUERY_STRING'});
        my $query_name = $parm_hash{'query'};
        if (validate_query($query_name))
          {
#            print "query valid \n";
            my $response_format = $parm_hash{'format'};
            if (validate_format($response_format))
              {
#                print "Format: $response_format\n";
                # Yes, I could just send %parm_hash as a parameter, but
                # since I'm extracting the name and format for
                # validation, I'm sending them separately.
                print run_user_query($query_name,$response_format,%parm_hash);
              }
            else
              {
                invalid_format_response();
              }
            # NOP
          }
        else
          {
            print "Invalid query!\n";
            invalid_query_response();
          }
      }
    else
      {
        front_page();
      }
  }


run();
