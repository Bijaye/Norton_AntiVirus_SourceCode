// apstsig.h - Prototype for the AvisPostSignature class

#if !defined (__APSTSIG_H__)
#define __APSTSIG_H__

#include "goservletclient.h"


class NamedMutex;

//
//: Handle the "postSignatureSet" servlet
//
// AvisPostSignature handles submission of blessed and non-blessed
// definition packages, or signatures (these terms are used
// interchangeably throughout) to the Immune System gateway.
//

class AvisPostSignature : virtual public GoServletClient
{
public:

  //
  //: PostSignature Constructor, called with the single GoServletHost
  // object for this process, and the handle passed into the servlet
  // callback by the web server.
  //

  AvisPostSignature(GoServletHost& host_,const char* gwapihandle_);

  //
  //: Destructor
  //

  virtual ~AvisPostSignature();

protected:

  //
  // Responsible for pre-initializing object-local state variables
  // that may depend on full initialization of the parent object.
  // Semantically, this is essentially a construction operation,
  // failure of which denotes an invalid transaction object. (and is
  // so handled by the caller.)  I plan to put this in the semantic
  // stream of the parent class as a pure-virt.  But not today... ;)
  //

  virtual bool initialize_state();

  //
  // A strange function name, but there are some special
  // pre-conditions involving submission of duplicate signature sets.
  // Sometimes it's valid (i.e. to "bless" an "unblessed" signature
  // set) but most of the time, it's an error.
  //

  virtual bool should_i_install_package() const;

  //
  //: Envelope around liveProcessing business logic
  //
  // fulfillRequest is a method called by the parent class,
  // GoServletClient to actually execute the transaction
  // handler. Originally the plan was to use it as a try{} catch{}
  // block around some other function (namely liveProcessing) to trap
  // any outstanding errors before the transaction completed, to allow 
  // it to finish gracefully.  This logic now happens in the
  // GoServletClass::run() method, which serves this purpose quite
  // nicely.  However, we are past the "make no non-critical changes"
  // line in the development cycle, so this is staying in.
  //


  virtual int fulfillRequest();

  //
  // Create a new persistent signature object in the database
  // (but don't actually build the transient object)
  //

  bool createNewSignature() throw();

  //: Save the signature file off to disk
  // Preconditions:
  // The SignaturesDirectory environment var must be valid
  // and incoming headers must have been validated.  
  // this method pulls content from the input stream and
  // actively writes a file out to disk.
  // That file is named 
  // SignaturesDirectory\X-Signatures-NameSignatureExtension
  //
  bool writeSignatureFile() throw();

  //
  //: reconstitute the zip archive
  // Preconditions:
  // The SignaturesDirectory environment var must be valid
  // The X-Signatures-Name must be valid
  //

  void explodeSignatureFile() throw();

  //
  //: build the path from the internal var _package_path and make the
  //: directory itself
  //

  void createSigPath() const;

  //
  //: Unpack the posted VDB package
  // This method creates a VDBPackage object with the package file,
  // destination path, and a mutex lock (in an unlocked state) then
  // calls that object's "Unpack" method to perform the "installation"
  //

  void unpackVDB(NamedMutex& mutex_);
                        
  
private:

  //
  //: Standard TraceClient for this object.
  // See the documentation for TraceClient in the AvisCommon component
  // for more information
  //

  TraceClient* _tracer;

  //
  //: Sequence Number of the definition package being submitted,
  //  represented as an unsigned integer.
  //

  unsigned int _sequence_number;

  //
  //: Sequence Number of the definition package being submitted.
  // represented as an 8 digit 0-padded sequence number string (used
  // for filenames)
  //

  std::string _sequence_string;

  //
  //: MD5 of signature set as indicated in the header
  //  This is used to test the integrity of the HTTP content transfer.
  //

  std::string _signature_checksum;

  //
  //: Date signatures produced
  //

  std::string _date_produced;

  //
  //: Date signatures published
  //
  std::string _date_published;

  //
  // this string may be null, indicating unblessed signatures
  //
  std::string _date_blessed;

  //
  //: MD5 of the sample file as calculated coming across the HTTP
  //: transaction stream.
  //

  std::string _content_hash;

  //
  //: Definition package file location
  // This is calculated from the "SignatureDirectory" parameter in the
  // avisservlets.prf file, plus the sequence number of this
  // definition package, plus ".vdb"
  //

  std::string _package_file;

  //
  //: directory which will receive the "installed" definition package
  // This is calculated from the "SignatureDirectory" parameter in the
  // avisservlets.prf file, appended with the sequence number for this
  // package.
  //

  std::string _package_path;



  //
  //: Intentionally Unimplemented default constructor
  //

  AvisPostSignature();

  //
  //: Intentionally unimplemented const copy constructor
  //

  AvisPostSignature(const AvisPostSignature&);

  //
  //: Intentionally unimplemented assignment operator
  //

  AvisPostSignature& operator =(const AvisPostSignature&);

};
#endif // __APSTSIG_H__
