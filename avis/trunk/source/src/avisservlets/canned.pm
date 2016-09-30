
package canned;
use dbiface;
require Exporter;
@ISA = qw (Exporter);

@EXPORT = qw (query_customer_samples
              query_customer_detail
              query_open_samples
              query_admin_info
              query_current_signatures
              query_sample_detail
              query_aged_samples
              get_content_type);


# This is one of those fun little routines that's profoundly suboptimal
sub _n_days_ago_db2
  {
    my ($n) = @_;
    my $current_timestamp = time();
    my $correct_day = $current_timestamp - (3600 * 24 * $n);
    my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime($correct_day);
    $year += 1900;
    $mon += 1;
    $wday += 1;
    return "$year-$mon-$mday 00:00:00";
  }

# These return the following delimiters in a simple list
# Beginnning of Content
# End of Content
# Beginning of table
# End of table
# Beginning of record
# End of record
# Beginning of data (field)
# End of data (field)

sub _get_html_delims
  {
    return ("<html><body>","</body></html>",
            "<table border=1>","</table>",
            "<tr>","</tr>",
            "<td>","</td>");
  }
sub _get_csv_delims
  {
    return ("","",
            "","",
            "","\n",
            "\"","\",");
  }
sub _get_tab_delims
  {
    return ("","",
            "","",
            "","\n",
            "\"","\"\t");
  }
sub get_delimiters
  {
    my ($response_format) = @_;
    my @delims = ();
    if ($response_format eq 'csv')
      {
        @delims = _get_csv_delims();
      }
    elsif ($response_format eq 'tab')
      {
        @delims = _get_tab_delims();
      }
    elsif($response_format eq 'html')
      {
        @delims = _get_html_delims();
      }
    return @delims;
  }

sub get_content_type
  {
    my ($response_format) = @_;
    my $content_type = "text/plain";
    if ($response_format eq 'csv')
      {
        $content_type = "text/csv";
      }
    elsif($response_format eq 'tab')
      {
        $content_type = "text/tab";
      }
    elsif($response_format eq 'html')
      {
        $content_type = "text/html";
      }
    return $content_type;
  }

# These actually get a little scary.

#####################################################################
# query_customer_samples is by far the most complex and ugly of these
# queries.  It involves at least two separate SQL queries, and a host
# of transforms on the results
#####################################################################
sub query_customer_samples  
  {
    my ($response_format,%parm_hash) = @_;
    my ($boc,$eoc,$bot,$eot,$bor,$eor,$bod,$eod) = get_delimiters($response_format);
    
    my $sql = "select ";
    $sql .= " distinct(attributes.name),count(distinct(attributes.localcookie)) ";
    $sql .= "from ";
    $sql .= "  attributes, ";
    $sql .= "  attributekeys ";
    $sql .= "where ";
    $sql .= "  attributekeys.attributekey = ";
    $sql .= "    (select attributekey from attributekeys where ";
    $sql .= " text = 'X-Customer-Contact-Email') ";

    my $content =  "$boc$bot";
    $content .= "$bor$bod" . "Customer$eod$eor";

    my $db_handle = dbiface::run_query("AVISDATA",$sql);
    if ($db_handle)
      {
        while ($db_handle->FetchRow())
          {
            $content .= "$bor";
            my ($customer) = $db_handle->Data();
            $content .= "$bod$customer$eod";
            $content .= "$eor";
          }
        $db_handle->Close();
      }
    $content .= "$eot$eoc";
    
    return $content;
  }
#####################################################################
#####################################################################
sub query_customer_detail  
  {
    my ($response_format,%parm_hash) = @_;
    my ($boc,$eoc,$bot,$eot,$bor,$eor,$bod,$eod) = get_delimiters($response_format);
  }
#####################################################################
#####################################################################
sub sample_detail_href
  {
    my ($cookie) = @_;
    my $full_href = "<a href=\"http://";
    $full_href .= $ENV{"HTTP_HOST"};
    $full_href .= $ENV{"SCRIPT_NAME"};
    $full_href .= "?query=sampdet&cookie=$cookie\">$cookie</a>";
    return $full_href;    
  }
#####################################################################
#####################################################################
                
sub query_open_samples  
  {
    my ($response_format,%parm_hash) = @_;
    my ($boc,$eoc,$bot,$eot,$bor,$eor,$bod,$eod) = get_delimiters($response_format);
    #-------------------
    # Columns to display 
    #-------------------
    # LocalCookie
    # AnalysisState
    # Last State Change
    # Imported
    # Finished
    # Needed
    #-------------------
    # SQL
    #-------------------
    # select   
    #    AnalysisRequest.LocalCookie, 
    #    AnalysisRequest.LastStateChange, 
    #    AnalysisRequest.Finished, 
    #    AnalysisRequest.Imported, 
    #    AnalysisRequest.Imported,
    #    AnalysisStateInfo.name, 
    #    AnalysisResults.SignatureSeqNum
    # from
    #    AnalysisRequest,
    #    AnalysisResults,
    #    AnalysisStateInfo
    # where 
    #    AnalysisResults.checksum          = AnalysisRequest.checksum and
    #    AnalysisRequest.LocalCookie       = _user parameter_         and
    #    AnalysisStateInfo.AnalysisStateID = AnalysisRequest.AnalysisStateID

    # I don't particularly like doing this this way, but it looks so
    # much cleaner than the alternative 

    my $sql = "select ";
    $sql .= "  AnalysisRequest.localcookie,";
    $sql .= "  AnalysisRequest.imported,";
    #    $sql .= "  AnalysisRequest.laststatechange,";
    $sql .= "  AnalysisStateInfo.name ";
    $sql .= " from ";
    $sql .= "  analysisrequest,";
    $sql .= "  analysisstateinfo "; 
    $sql .= " where ";
    $sql .= "  finished is null ";
    $sql .= "  and ";
    $sql .= "  analysisstateinfo.analysisstateid = analysisrequest.analysisstateid";
    $sql .= " order by";
    $sql .= "  analysisrequest.localcookie";

    my $db_handle = dbiface::run_query("AVISDATA",$sql);
    
    my $content = "$boc";
    if ($db_handle)
      {
        # Process results into tabular form
        # First row is always the fields themselves
        
        # Create the content buffer, and start filling it.
        
        $content .= "$bot$bor";
        $content .= "$bod Cookie$eod";
        $content .= "$bod Submitted$eod";
        $content .= "$bod State$eod";
        $content .= $eor;

        # Now add the live data
        while ($db_handle->FetchRow())
          {
            $content .= $bor;
            
            #---------------------------------------
            # This is the spiffy new dynamic way
            #---------------------------------------
            my %record = $db_handle->DataHash();
            # The field names are:
            #   LOCALCOOKIE
            #   IMPORTED
            #   LASTSTATECHANGE
            #   NAME
            # 
            #=======================================
            # Build content for cookie 
            #=======================================
            $content .= $bod;
            if ($response_format eq 'html')
              {
                $content .= sample_detail_href($record{"LOCALCOOKIE"});
              }
            else
              {
                $content .= $record{"LOCALCOOKIE"};
              }
            $content .= $eod;
            #=======================================
            # The others are simple
            #=======================================
            
            #---------------------------------------
            # Imported date
            #---------------------------------------
            $content .= $bod;
            $content .= $record{"IMPORTED"};
            $content .= $eod;
            #---------------------------------------
            # Analysis State
            #---------------------------------------
            $content .= $bod;
            $content .= $record{"NAME"};
            $content .= $eod;
            #---------------------------------------
            #---------------------------------------
            #---------------------------------------
            #---------------------------------------
            #---------------------------------------
            
            
            
            #---------------------------------------
            # This is the old dumb static way
            #---------------------------------------
            #             my @fields = $db_handle->Data();
            #             foreach my $cell(@fields)
            #               {
            #                 $content .= $bod;
            #                 $content .= $cell;
            #                 $content .= $eod;
            #               }
            #---------------------------------------
            $content .= $eor;

          }
        $db_handle->Close();
        $content .= $eot;
      }
    else
      {
        $content .= "Oops, couldn't perform query<br>";
        $content .= "Database error: " . $dbiface::DB_ERROR . "<br>\n";
        $content .= "LocalError = " . $dbiface::local_error . "<br>\n";
      }
    $content .= "$eoc";
    return $content;
  }
#####################################################################
#####################################################################
sub query_admin_info  
  {
    my ($response_format,%parm_hash) = @_;
    my ($boc,$eoc,$bot,$eot,$bor,$eor,$bod,$eod) = get_delimiters($response_format);
  }
#####################################################################
#####################################################################
sub query_aged_samples
  {
    my ($response_format,%parm_hash) = @_;
    my $db2_date = _n_days_ago_db2($parm_hash{'sampleage'});

    my $sql = "select ";
    $sql .= "  AnalysisRequest.localcookie,";
    $sql .= "  AnalysisRequest.imported,";
    $sql .= "  AnalysisRequest.laststatechange,";
    $sql .= "  AnalysisStateInfo.name ";
    $sql .= " from ";
    $sql .= "  analysisrequest,";
    $sql .= "  analysisstateinfo "; 
    $sql .= " where ";
    $sql .= "  analysisrequest.imported > '$db2_date' ";
    $sql .= "  and analysisrequest.analysisstateid = analysisstateinfo.analysisstateid ";
    $sql .= " order by";
    $sql .= "  analysisrequest.imported";

    my ($boc,$eoc,$bot,$eot,$bor,$eor,$bod,$eod) = get_delimiters($response_format);
    my $db_handle = dbiface::run_query("AVISDATA",$sql);
    
    my $content = "$boc";
    if ($db_handle)
      {
        # Query information
        $content .= "Summary of Samples submitted since $db2_date<br>";
        # Process results into tabular form
        # First row is always the fields themselves
        
        # Create the content buffer, and start filling it.
        
        $content .= "$bot$bor";
        $content .= "$bod Cookie$eod";
        $content .= "$bod Submitted$eod";
        $content .= "$bod State$eod";
        $content .= $eor;

        # Now add the live data
        while ($db_handle->FetchRow())
          {
            $content .= $bor;
            
            #---------------------------------------
            # This is the spiffy new dynamic way
            #---------------------------------------
            my %record = $db_handle->DataHash();
            # The field names are:
            #   LOCALCOOKIE
            #   IMPORTED
            #   LASTSTATECHANGE
            #   NAME
            # 
            #=======================================
            # Build content for cookie 
            #=======================================
            $content .= $bod;
            if ($response_format eq 'html')
              {
                $content .= sample_detail_href($record{"LOCALCOOKIE"});
              }
            else
              {
                $content .= $record{"LOCALCOOKIE"};
              }
            $content .= $eod;
            #=======================================
            # The others are simple
            #=======================================
            
            #---------------------------------------
            # Imported date
            #---------------------------------------
            $content .= $bod;
            $content .= $record{"IMPORTED"};
            $content .= $eod;
            #---------------------------------------
            # Analysis State
            #---------------------------------------
            $content .= $bod;
            $content .= $record{"NAME"};
            $content .= $eod;
            #---------------------------------------
            #---------------------------------------
            #---------------------------------------
            #---------------------------------------
            #---------------------------------------
            
            
            
            #---------------------------------------
            # This is the old dumb static way
            #---------------------------------------
            #             my @fields = $db_handle->Data();
            #             foreach my $cell(@fields)
            #               {
            #                 $content .= $bod;
            #                 $content .= $cell;
            #                 $content .= $eod;
            #               }
            #---------------------------------------
            $content .= $eor;

            #         # Process results into tabular form
            #         # First row is always the fields themselves
            #         my @columns = $db_handle->FieldNames();
            
            #         # Create the content buffer, and start filling it.
            
            #         $content .= "$bot$bor";
            #         foreach my $column(@columns)
            #           {
            #             $content .= "$bod$column $eod";
            #           }
            #         $content .= $eor;
            
            #         # Now add the live data
            #         while ($db_handle->FetchRow())
            #           {
            #             $content .= $bor;
            #             my @fields = $db_handle->Data();
            #             foreach my $cell(@fields)
            #               {
            #                 $content .= $bod;
            #                 $content .= $cell;
            #                 $content .= $eor;
            #               }
            #             $content .= $eor;
          }
        $db_handle->Close();
        $content .= $eot;
      }
    else
      {
        $content .= "Oops, couldn't perform query<br>";
        $content .= "Database error: " . $dbiface::DB_ERROR . "<br>\n";
        $content .= "LocalError = " . $dbiface::local_error . "<br>\n";
        $content .= "SQL: '$sql'";
      }
    $content .= $eoc;
    return $content;

  }
#####################################################################
#####################################################################
sub query_current_signatures  
  {
    my ($response_format,%parm_hash) = @_;
    my ($boc,$eoc,$bot,$eot,$bor,$eor,$bod,$eod) = get_delimiters($response_format);
  }
#####################################################################
#####################################################################
sub query_sample_detail  
  {
    my ($response_format,%parm_hash) = @_;
    my ($boc,$eoc,$bot,$eot,$bor,$eor,$bod,$eod) = get_delimiters($response_format);
    # This is a 2 phase select
    my $content = $boc;
    # First, pull Cookie, Imported date, state, and 'sequence' into a table
    my $sql = "";
    #     my $sql = "select ";
    #     $sql .= " analysisrequest.localcookie, ";
    #     $sql .= " analysisrequest.imported, ";
    #     $sql .= " analysisrequest.,";
    #     $sql .= " analysisresults.signaturesneeded ";
    #     $sql .= "from ";
    #     $sql .= " analysisrequest, ";
    #     $sql .= " analysisresults, ";
    #     $sql .= " analysisstateinfo ";
    #     $sql .= "where " ;
    #     $sql .= " analysisresults
    
      
    # Next, populate a table with NVPairs from the attributes table
    $sql = "select ";
    #    $sql .= " * ";
    $sql .= " attributekeys.text, ";
    $sql .= " attributes.value ";
    $sql .= " from ";
    $sql .= "  attributekeys, ";
    $sql .= "  attributes ";
    $sql .= " where ";
    $sql .= "  attributes.localcookie = " . $parm_hash{"cookie"};
    $sql .= " and ";
    $sql .= "  attributekeys.attributekey = attributes.attributekey ";
    $sql .= " and attributekeys.text like 'X-%' ";
    $sql .= "";

    my $db_handle = dbiface::run_query("AVISDATA",$sql);
    
    if ($db_handle)
      {
        # Process results into tabular form
        # First row is always the fields themselves
        
        # Create the content buffer, and start filling it.
        
        $content .= "$bot";        
        $content .= "$bor$bod Attribute$eod";
        $content .= "$bod Value$eod$eor$bor$eor";

        # Now add the live data
        while ($db_handle->FetchRow())
          {
            $content .= $bor;
            my @fields = $db_handle->Data();
            foreach my $cell(@fields)
              {
                $content .= $bod;
                $content .= $cell;
                $content .= $eor;
              }
            $content .= $eor;
          }
        $db_handle->Close();
        $content .= $eot;
      }
    else
      {
        $content .= "Oops, couldn't perform query<br>";
        $content .= "Database error: " . $dbiface::DB_ERROR . "<br>\n";
        $content .= "LocalError = " . $dbiface::local_error . "<br>\n";
        $content .= "SQL: '$sql'";
      }
    $content .= $eoc;
    return $content;
  };
