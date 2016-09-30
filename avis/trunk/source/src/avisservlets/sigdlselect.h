#if !defined (__SIGDLSELECT_H__)
#define __SIGDLSELECT_H__ 

#include <string>
class DBConnectionPool;
class TraceClient;
class MTrace;

//
//: Encapsulation of the database operations required for signature download transactions
//
// This class was created to avoid a great deal of unnecessary SQL
// traffic caused by using the "avisdb/signature" class and the
// associated infrastructural mess.  SignatureDownloadSelect
// encapsulates all required sql select transactions and uses the
// DBConnectionPool for it's database connections.
//
// A very desirable side effect of this class is that it keeps all SQL 
// out of the AvisGetSignature class.
//
// SignatureDownloadSelect and AvisGetSignature also serve as a
// prototype for implementing the DBConnectionPool throughout the
// system in replacement of the dubious AvisDB "logic".
//
// Note that since it's only task is to perform two SQL selects and
// provide accessors to the results of those selects, they are
// performed in the constructor.  By the time the constructor is
// complete, the database connection has already been released back
// into the connection pool.
//

class SignatureDownloadSelect
{ 
public:
  enum SIG_AVAILABILITY
  {
    SA_NULL,
    SA_AVAILABLE,
    SA_SUPERCEDED,
    SA_UNAVAILABLE
  };

  //
  //:  Object constructor.
  //
  // The arguments to the constructor include, the path to the
  // definition packages, a reference to the database connection pool
  // manager, and the value of the "X-Signatures-Sequence" attribute
  // of the incoming http transaction.
  //

  SignatureDownloadSelect(MTrace& trace_server_,
                          const std::string& package_path_,
                          DBConnectionPool& pool_,
                          const std::string& sequence_);

  //
  //: Virtual destructor
  //

  virtual ~SignatureDownloadSelect();

  //
  //: Return the availability of the requested signatures
  //
  // This method will report whether or not a requested signature
  // package has been superceded, is unavailable or is available for
  // download.
  //

  SIG_AVAILABILITY   availability()    const { return _availability; };

  //
  //: Return the sequence number of the requested signatures
  //
  // If  the client requested "newest" or "blessed" signatures, this
  // will return the actual sequence number of those signatures.  Of
  // course, if signatures were requested by number, that will be
  // returned as well.
  //

  const std::string& sequence_number() const { return _sequence_number; };

  //
  //: Return the blessed date of the requested signatures.
  //
  // (this string will be empty in the event that the  requested
  // signatures were not blessed)
  //

  const std::string& blessed_date()    const { return _blessed_date;    };

  //
  //: Return the published date of the requested signatures
  //

  const std::string& published_date()  const { return _published_date;  };

  // 
  //: Return the produced date of the requested signatures
  //

  const std::string& produced_date()   const { return _produced_date;   };

  //
  //: Return the MD5 hash of the signature package content
  //

  const std::string& checksum()        const { return _checksum;        };
  
protected:

  //
  //: Run the actual SQL statements to pull all required information
  //

  void run_selection();

  //
  //: Dump the full SQL results to the Trace object
  //

  void dump_state();

private:

  //
  //: Translate text tokens into "actual" sequence numbers
  //
  // Resolve requested sequence number to a true numeric
  // (only matters if "blessed" or "latest" was requested
  //

  void resolve_sequence_number();

  //
  //: Pad out a sequence number.
  //
  // Normalize the requested sequence 'number' 
  // (i.e. pad it left to 8 chars with'0')
  //

  void normalize_sequence_number();

  //
  //: Select values of the "Latest" and "Blessed" sequence numbers for 
  //: this gateway from the database
  //

  void get_latest_and_blessed();

  //
  //: Get signature information for the requested sequence number from 
  //: the Signatures table in the database
  //

  void get_signature_information();


  //
  //: Test for existance of the package file in the file system
  //
  // There "should" be no reason this file shouldn't exist, but that's 
  // no reason not to check.
  //

  bool package_file_exists() const;

  //
  //: The Database connection pool
  //

  DBConnectionPool& _pool;

  //
  //: user-requested sequence (may not be number)
  //
  // This is the exact value that came in on the transaction through
  // the constructor.
  //

  std::string _requested_sequence;

  //
  //: The location of the signature files in the file system
  //
  // Location of signature package files (passed in through
  // constructor)
  //

  std::string _package_path;
  
  //
  //: Availability of the requested signatures
  //

  SIG_AVAILABILITY _availability;

  //
  //: Latest blessed signatures on this gateway (as reported by the
  //: database query)
  //
  // This information is collected from the get_latest_and_blessed()
  // method
  //

  std::string _blessed_sequence;

  //
  //: Latest blessed signatures on this gateway (as reported by the
  //: database query)
  //
  // This information is collected from the get_latest_and_blessed()
  // method
  //

  std::string _latest_sequence;

  //
  //: sequence number of requested signature package
  //
  // Translated (if necessary) and normalized version of the
  // constructor argument
  //

  std::string _sequence_number;

  //
  //: Date requested signatures were blessed (empty string if they
  //: weren't)
  //
  // This information is collected in the get_signature_information
  // method
  //

  std::string _blessed_date;

  //
  //: Date requested signatures were "published"
  //
  // This information is collected in the get_signature_information
  // method
  //

  std::string _published_date;

  //
  //: Date requested signatures were "produced"
  //
  // This information is collected in the get_signature_information
  // method
  //

  std::string _produced_date;
  
  //
  //: MD5 hash of the signature content file
  //
  // This information is collected in the get_signature_information
  // method.
  //

  std::string _checksum;

  //
  //: Standard trace object
  //
  
  TraceClient* _tracer;
};
#endif // __SIGDLSELECT_H__ Sentinel
