#if !defined (__SIGFILE_H__)
#define __SIGFILE_H__ 

#include <string>

class SignatureCache;

class MTrace;
class TraceClient;

//
//: Representation of and container for a cached definition package file
//: 
//
// The SignatureFile class is used by the SignatureCache object to
// maintain the actual content buffer and checksum information for a
// particular definition package.  The SignatureFile class is
// responsible for finding the definition package and loading it on
// construction, and flushing the content buffer on destruction.
//
// For more information see the SignatureCache class
//

class SignatureFile
{
  friend SignatureCache;
protected:

  //
  //: Object constructor
  //

  SignatureFile(MTrace& trace_server_,
                const std::string& root_path_,
                const std::string& sequence_);

  //
  //: Destructor
  //

  ~SignatureFile();

  //
  //: Retrieve the MD5 checksum for the signature file content
  //
  

  const std::string& get_checksum       () const;

  //
  //: Retrieve the content length for this definition package
  //
  
        long         get_content_length () const;

  //
  //: Grab a pointer to the content block for this signature file
  //
  // Note: It's possible to call get_content, then cast away constness 
  // from the pointer, and delete the content block.  While it would
  // be a strange thing to ever want to do, it would invalidate the
  // object.
  //

  const char*        get_content        () const;

  //
  //: Get the value of the "Content-Type" field for an http
  //: transmission of this content stream.
  //

  const std::string& get_content_type   () const;
  
private:

  //
  //: Allocate an internal buffer and read the full file contents into 
  //: memory.
  //

  void slurp(const std::string& filename_);

  //
  //: Generate an MD5 hash off of the content pointer.
  //

  std::string calculate_md5(const char* content_,
                            long length_) const;

  //
  //: Given the sequence number, and the root path for the definition
  //: packages on this server, generate the fully-qualified pathname
  //: (including extension) for this definition package file.
  //

  std::string derive_file_name(const std::string& root_path_ ,
                               const std::string& sequence_  );
  
  //
  //: Fully-qualified filename of this package
  //

  std::string _filename;

  // 
  //: Padded signature sequence number
  //
  
  std::string _sequence;

  //
  //: Generated MD5 checksum field
  //

  std::string _checksum;

  //
  //: Content-Type value for this definition package
  //

  std::string _content_type;

  //
  //: content buffer pointer
  //

  char*       _content;

  //
  //: Byte length of the content file buffer
  //

  long        _content_length;

  //
  //: Standard Tracing object.
  //

  TraceClient* _tracer;
};
#endif // __SIGFILE_H__
