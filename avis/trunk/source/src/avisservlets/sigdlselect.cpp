#include "sigdlselect.h"

#include "dbconpool.h"
#include "dbconnhnd.h"

// Here's the stuff I don't like including at all.
#include "avisdb/dbsession.h"
#include "avisdb/sqlstmt.h"
#include "aviscommon/datetime.h"
// And the stuff I don't mind including much...
#include "aviscommon/log/mtrace.h"
#include "aviscommon/log/traceclient.h"
#include "aviscommon/log/stacktrace.h"
#include <sys/stat.h>

SignatureDownloadSelect::SignatureDownloadSelect(MTrace& trace_server_,
                                                 const std::string& package_path_,
                                                 DBConnectionPool& pool_,
                                                 const std::string& sequence_)
  :_pool(pool_),
   _requested_sequence(sequence_),
   _package_path(package_path_),
   // availability enum
   _availability(SA_AVAILABLE),
   // From the globals table
   _blessed_sequence(),
   _latest_sequence(),
   // 
   _sequence_number(),
   _published_date(),
   _produced_date(),
   _tracer(NULL)
{
  _tracer = new TraceClient(trace_server_,"SignatureDownloadSelect");
  // When this method returns, the database handle is no longer required
  run_selection();
  dump_state();
}

SignatureDownloadSelect::~SignatureDownloadSelect()
{
  // There are no dynamics ;)
  _tracer->info("Destroying SignatureDownloadSelect object");
  delete _tracer;
}

void SignatureDownloadSelect::run_selection()
{
  StackTrace _stack(*_tracer,"run_selection");
  
  // we ALWAYS need these numbers
  get_latest_and_blessed();
  // In order to do this (if it's needed)
  resolve_sequence_number();
  // Pad out left to 8 chars with zeros (for filenames, etc)
  normalize_sequence_number();

  // Now...
  // Convert the sequence numbers to numerics 
  long _blessed   = atol(_blessed_sequence.c_str());
  long _requested = atol(_requested_sequence.c_str());
  long _latest    = atol(_latest_sequence.c_str());

  _stack.debug(_blessed_sequence);
  _stack.debug(_requested_sequence);
  _stack.debug(_latest_sequence);
  if (_blessed > _requested)
    {
      _stack.info("sigs superceded");
      _availability = SA_SUPERCEDED;
    }
  else if (_requested > _latest)
    {
      _stack.info("sigs unavailable");
      _availability = SA_UNAVAILABLE;
    }
  else
    {
      if (package_file_exists() == true)
        {
          get_signature_information();
        }
      else
        {
          _stack.error("package file does not exist!");
          _availability = SA_UNAVAILABLE;
        }
    }
  

  // Release the connection object
}

void SignatureDownloadSelect::resolve_sequence_number()
{
  StackTrace _stack(*_tracer,"resolve_sequence_number");
  std::string _message;

  if (_requested_sequence == "blessed")
    {
      _message = "resolving to blessed:[";
      _message.append(_blessed_sequence);
      _message.append("]");
      _stack.info(_message);

      _requested_sequence = _blessed_sequence;
    }
  else if (_requested_sequence == "newest")
    {
      _message = "resolving to newest:[";
      _message.append(_latest_sequence);
      _message.append("]");
      _stack.info(_message);

      _requested_sequence = _latest_sequence;
    }
}
void SignatureDownloadSelect::normalize_sequence_number()
{
  StackTrace _stack(*_tracer,"normalize_sequence_number");
  int  _sequence = atoi(_requested_sequence.c_str());
  char _char_sequence[64];
  memset(_char_sequence,0,64);
  //  memset(_char_sequence,'\0',9);

  sprintf(_char_sequence,"%08d",_sequence);
  _requested_sequence = _char_sequence;
}

void SignatureDownloadSelect::get_latest_and_blessed()
{
  StackTrace _stack(*_tracer,"get_latest_and_blessed");
  // Grab the connection object
  DBConnectionHandle _db_handle(_pool.get_database_connection());

  // This is where some really ugly crap happens
  // I'll probably abstract this at least one more layer, but for now,
  // here it is.
  std::string _sql("select * from globals where alwayszero = 0");
  // 
  
  SQLINTEGER	retLength;
  std::string	select("SELECT NewestBlssdSeqNum,NewestSeqNum FROM Globals WHERE AlwaysZero = 0");
  uint _blessed;
  uint _latest;
  
  SQLStmt		sql(_db_handle.get_session(), "NewestBlessedSignature",
                            AVISDBException::etInternal);
  if (sql.ExecDirect(select)                 &&
      sql.BindColumn(1, &_blessed, &retLength) &&
      sql.BindColumn(2, &_latest, &retLength) &&
      sql.Fetch(true, false))
    {
      char _sig[64];
      memset(_sig,0,64);
      sprintf(_sig,"%08d",_blessed);
      _blessed_sequence = _sig;
      sprintf(_sig,"%08d",_latest);
      _latest_sequence = _sig;
      std::string _message("Blessed [");
      _message.append(_blessed_sequence);
      _message.append("]");
      _stack.info(_message);
      _message = "Newest [";
      _message.append(_latest_sequence);
      _message.append("]");
      _stack.info(_message);      
    }
  
  // The result of this is that the _blessed_sequence and
  // _latest_sequence internal members are populated.

}
void SignatureDownloadSelect::get_signature_information()
{
  StackTrace _stack(*_tracer,"get_signature_information");
  // Grab the connection object
  DBConnectionHandle _db_handle(_pool.get_database_connection());
  std::string _sql("select SignatureSeqNum,Blessed,Produced,Published,Checksum ");
  _sql.append("from signatures where signatureseqnum = ");
  _sql.append(_requested_sequence);
  // This is where some really ugly crap happens
  // The 'availability' is populated, as well as the ancillary fields

  DateTime   _blessed;
  DateTime   _produced;
  DateTime   _published;
  // DateTime objects are initialized with the current time.  This
  // makes it rather difficult to test for NULL times coming back from
  // a database query (i.e. impossible.)  However, we CAN test whether 
  // or not the timestamp was changed as the result of a select.
  // Since a Signature has to be blessed very far back in the system
  // (architecturally) it will almost certainly not have a blessed
  // date/time of right now.
  DateTime   _right_now = _blessed;

  SQLINTEGER _sigNumRetLength;
  SQLINTEGER _blessedRetLength;
  SQLINTEGER _producedRetLength;
  SQLINTEGER _publishedRetLength;
  SQLINTEGER _checkSumRetLength;
  uint       _sequence;
  uint	     _checkSumMaxLength	= 34;
  char	     _charCheckSum[34];

  try
    {

      SQLStmt sql(_db_handle.get_session(), "SignatureDownloadSelect::get_signature_information",
                  AVISDBException::etInternal);

      // Pessimistic initialization
      _availability = SA_NULL;


      if (sql.ExecDirect(_sql))
        {
          sql.BindColumn(1, &_sequence,     &_sigNumRetLength);
          sql.BindColumn(2,  _blessed,      &_blessedRetLength);
          sql.BindColumn(3,  _produced,     &_producedRetLength);
          sql.BindColumn(4,  _published,    &_publishedRetLength);
          sql.BindColumn(5,  _charCheckSum,  _checkSumMaxLength, &_checkSumRetLength);
          if (sql.Fetch(true,false))
            {
              _stack.info("fetch succeeded.");
              // Yippee!! It exists, we've got it, we're done.
              // Convert the fetched items into the object member vars
              char _charseq[64];
              memset(_charseq,0,64);
              sprintf(_charseq,"%08d",_sequence);
              _sequence_number = _charseq;
              
              //Modified on Mar/28/2000.
              // Check if you have a valid dateTime value before
              // converting it to an HTTP string.

              // Test whether or not the blessed date was changed as a 
              // result of the sql select & fetch.
              if (!(_blessed == _right_now))
                {
                  //               if (!_blessed.IsNull())
                  _stack.info("Blessed date for this definition sequence is non-zero.");
                  _blessed.AsHttpString(_blessed_date);
                }
              else
                {
                  _stack.info("Blessed date for this definition sequence is zero.");
                  _blessed_date = "";		
                }
              
              _produced.AsHttpString(_produced_date);
              _published.AsHttpString(_published_date);
          
              _charCheckSum[_checkSumRetLength]	= '\0';
              _checksum = _charCheckSum;
          
              _availability = SA_AVAILABLE;
            }
          else
            {
              // If the error was SQL_NO_DATA (i.e. no such signatures)
              // Then set availability to SA_UNAVAILABLE
              if (sql.LastRetCode() == SQL_NO_DATA)
                {
                  _availability = SA_UNAVAILABLE;
                }
              else
                {
                  _stack.error("sql.Fetch failed");
                  throw AVISDBException(AVISDBException::etSQLStmt,"Fetch failure");
                }
            }
        }
      else
        {
          _stack.error("sql.ExecDirect failed");
          throw AVISDBException(AVISDBException::etSQLStmt,"ExecDirect failure");
        }
    }
  catch (AVISDBException& e_)
    {
      _stack.error(e_.DetailedInfo());
      throw e_;
    }
}

bool SignatureDownloadSelect::package_file_exists() const
{
  StackTrace _stack(*_tracer,"package_file_exists");
  bool _exists = false;
  std::string _filename(_package_path);
  _filename.append("\\");
  _filename.append(_requested_sequence);
  _filename.append(".vdb");
  
  struct _stat _stat_buffer;
  if (::_stat(_filename.c_str(),&_stat_buffer) != -1)
    {
      _exists = true;
    }
  return _exists;
}
void SignatureDownloadSelect::dump_state()
{
  StackTrace _stack(*_tracer,"dump_state");
  std::string _msg;
  _msg = "Requested Sequence [";
  _msg.append(_requested_sequence);
  _msg.append("]");
  _stack.debug(_msg);

  _msg = "availability [";
  switch (_availability)
    {
    case SA_NULL:
      _msg.append("SA_NULL]");
      break;
    case SA_AVAILABLE:
      _msg.append("SA_AVAILABLE]");
      break;
    case SA_SUPERCEDED:
      _msg.append("SA_SUPERCEDED]");
      break;
    case SA_UNAVAILABLE:
      _msg.append("SA_UNAVAILABLE]");
    };
  _stack.debug(_msg);

  _msg = "Newest Blessed Seq [";
  _msg.append(_blessed_sequence);
  _msg.append("]");
  
  _stack.debug(_msg);

  _msg = "Newest Unblessed Seq [";
  _msg.append(_latest_sequence);
  _msg.append("]");

  _stack.debug(_msg);

  _msg = "Sequence Number [";
  _msg.append(_sequence_number);
  _msg.append("]");

  _stack.debug(_msg);

  _msg = "Blessed Date [";
  _msg.append(_blessed_date);
  _msg.append("]");

  _stack.debug(_msg);

  _msg = "Published Date [";
  _msg.append(_published_date);
  _msg.append("]");

  _stack.debug(_msg);

  _msg = "Produced Date [";
  _msg.append(_produced_date);
  _msg.append("]");

  _stack.debug(_msg);

  _msg = "Checksum [";
  _msg.append(_checksum);
  _msg.append("]");

  _stack.debug(_msg);
              
}
