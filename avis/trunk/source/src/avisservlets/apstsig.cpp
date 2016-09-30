// apstsig.cpp - Implementation of the AvisPostSignature class

#pragma warning (disable : 4786)
#include "apstsig.h"

// Local Headers
#include "strutil.h"
#include "gosession.h"
#include "utilexception.h"
#include "namedmutex.h"

// Standard Headers
#include <direct.h> // for _mkdir
#include <assert.h>
#define ASSERT assert

// AvisCommon headers
#include "aviscommon/stdafx.h"
#include "aviscommon/log/traceclient.h"
#include "aviscommon/log/stacktrace.h"
#include "aviscommon/datetime.h"

// AvisFile headers
#include "avisfile/avisfile.h"
#include "avisfile/md5/md5calculator.h"

// Avisdb headers
#include "avisdb/stdafx.h"
#include "avisdb/signature.h"
 
// AvisUnpack
//#include "avisunpack/unpack.h" // contains ziparchive.h & navpackage.h
//#include "avisunpack/fatalbertunpack/fatalbertunpack.h"

// VDB unpacker
#include "vdbunpacker/vdbunpacker.h"
#include "vdbunpacker/vdbpackage.h"

// Standard Library Headers
//#include <strstream>

AvisPostSignature::AvisPostSignature(GoServletHost& host_,const char* gwapihandle_)
  :GoServletClient(host_,gwapihandle_),
   _tracer(0),
   _sequence_number(0),
   _sequence_string("00000000"),
   _package_file(""),
   _package_path(""),
   _content_hash("")
  
{
  _tracer = new TraceClient(_trace_server,"AvisPostSignature");
  _tracer->debug("Created AvisPostSignature");
  // Add Required Headers
  addRequiredHeader("X-Signatures-Sequence");
}
AvisPostSignature::~AvisPostSignature()
{
  _tracer->debug("Destroying AvisPostSignature");
  if (_tracer)
    delete _tracer;
}

bool AvisPostSignature::initialize_state()
{
  StackTrace _stack(*_tracer,"initialize_state");
  bool _success = false;

  //-----------------------------------------------------------
  // Extract appropriate headers
  //-----------------------------------------------------------
  
  // Force padding of the sequence string...

  _sequence_string = _gwapi.incomingHeader("X-Signatures-Sequence");
  StringUtil::padLeft(_sequence_string,'0',8);
  

  // initialize the internal, numeric sequence_number
  // Note: This is an signed->unsigned assignment.
  // It SHOULDN'T be an issue until I can find the appropriate method
  _sequence_number = atoi(_sequence_string.c_str());

  std::string _msg("Sequence: [");
  _msg +=  _sequence_string;
  _msg += "]";
  _tracer->info(_msg);

  //-----------------------------------------------------------
  // Required headers ()
  //-----------------------------------------------------------
  _signature_checksum =
    _gwapi.incomingHeader("X-Signatures-Checksum");
  _msg = "Signature Checksum: [";
  _msg += _signature_checksum;
  _msg += "]";
  _tracer->info(_msg);
  
  _date_published     = _gwapi.incomingHeader("X-Date-Published");
  _msg = "Date Published: [";
  _msg += _date_published;
  _msg += "]";
  _tracer->info(_msg);

  // Oops, I had forgotten this code entirely.  Submissions were
  // failing on a bad "produced" column on the database insert.
  _date_produced   = _gwapi.incomingHeader("X-Date-Produced");
  _msg = "Date Produced: [";
  _msg += _date_produced;
  _msg += "]";
  _tracer->info(_msg);

  //-----------------------------------------------------------
  // Optional headers (extracted individually)
  //-----------------------------------------------------------
  try
    {
      _date_blessed = _gwapi.incomingHeader("X-Date-Blessed");
      _msg = "Date Blessed: [";
      _msg += _date_blessed;
      _msg += "]";
      _tracer->info(_msg);
    }
  catch(InvalidHttpHeader&)
    {
      // 
      _stack.msg("Submitted signatures unblessed");
    }

  // Internal attributes;
  // The path for expanded packages is remarkably similar to the
  // compressed filename ;)
  // Initialize package directory location
  _package_path  = _environment["SignatureDirectory"];
  _package_path << "\\"
                << _sequence_string;

  // Initialize package destination file name
//   if (stricmp(_gwapi.incomingHeader("Content-Type").c_str(),"application/x-NAV-FatAlbert") == 0)
//     {
//       _package_file = _package_path + ".exe";
//     }
//   else
//     {
      _package_file = _package_path + ".vdb";
//     }
  //--------------------------------------------
  // Dumping state vars to log with info status
  //--------------------------------------------
  _msg = "Package Path: [";
  _msg += _package_path;
  _msg += "]";
  _tracer->info(_msg);

  _msg = "Package File: [";
  _msg += _package_file;
  _msg += "]";

  _tracer->info(_msg);
  
  return true;
}

bool AvisPostSignature::should_i_install_package() const
{
  StackTrace _stack(*_tracer,"should_i_install_package");
  bool _yes = false;
  // For some reason, the logic I had installed to perform this check
  // is gone.  I know it's not possible that _I_ actually deleted the
  // method accidentally.  It must have been the underpants gnomes.

  // At this point, the integrity of the transaction has been
  // verified.  The relevant attributes have been pulled into the
  // local object scope, and this is a const operation. 
  
  // One of my objectives is to not allow this to throw any
  // exceptions... However, reality may intervene the way it seems to
  // be inclined to.

  //---------------------------------------------------------
  // Rule #1 - Does the package already exist?
  //           If so, then continue, otherwise return true.
  //---------------------------------------------------------

  try
    {
      Signature _sig(_sequence_number);
      if (!_sig.IsNull())
        {
      
          //---------------------------------------------------------
          // Rule #2 - Is the SUBMITTED signature package "blessed"?
          //           If so, continue, otherwise, return false.
          //---------------------------------------------------------
          if (_date_blessed != "")
            {
              //---------------------------------------------------------
              // Rule #3 - Is the EXISTING signature package "blessed"?
              //           If NOT, continue, otherwise, return false.
              //---------------------------------------------------------
          
              if (!_sig.IsBlessed())
                {
                  //-----------------------------------------------------
                  // Rule #4 - Do the checksums of both packages match?
                  //           if SO, set the Blessed Date of the 
                  //           EXISTING signature set to match that 
                  //           of the SUBMITTED package, and return 
                  //           false.
                  //-----------------------------------------------------
                  if (_sig.CheckSum() == _signature_checksum)
                    {
                      // Set the blessed date of the pre-installed package
                      DateTime _dt(_date_blessed.c_str());
                      _sig.Blessed(_dt);
                      // Return false, we're done
                    }
                  else
                    {
                      // This is reality intervening...
                      throw ServletRecoverable(
                                               "Duplicate Signature submission with unmatching checksums."
                                               ,"unpublished");
                    }
                }
            }
      
          // Submitted package is not blessed, return false, do NOT submit
          // this package
          else
            {
              _yes = false;
            }
        
        }
      else
        {
          _yes = true;
        }

    }
  catch (AVISDBException&)
    {
      // SignatureSet does not already exist in the database as is
      // evidenced by the exception that will be thrown by the
      // Signature constructor.
      // return true from this method, this is not a duplicate
      _yes = true;
    }

  return _yes;
}
int AvisPostSignature::fulfillRequest()
{
  StackTrace _stack(*_tracer,"fulfillRequest");


  // This sets the object-local state members
  if (initialize_state())
    {
      
      if (should_i_install_package())
        {

          // Given that we're actually installing the package...
          
          // Save the file first.  Because if it doesn't save
          // There's a potential for the scanner to try and
          // access signatures that are in the database
          // but not in the filesystem
          
          if (writeSignatureFile())
            {
              _stack.debug("Signature File written");
              explodeSignatureFile();
              createNewSignature();
            }
        }

    }
  // Unconditionally return 204.
  return 204;
}
bool AvisPostSignature::createNewSignature()
{

  StackTrace _stack(*_tracer,"createNewSignature");

  bool _success = false;
  //--------------------------------------------------------------------------
  // Collect all the required vars for the Signature::AddNew method
  //--------------------------------------------------------------------------
  

  // Here's the exception case:
  // If the "sequence number" is PANG or all zeros:
  // do nothing, but return success.
  // This is a test value.

  if ( ( (_sequence_string.find_first_not_of("0") == std::string::npos)  || 
         (_sequence_string == "PANG")))
    {
      _success = true;
    }
  else
    {
     

      DateTime _produced(_date_produced);
      DateTime _published(_date_published);
      DateTime _blessed(_date_blessed);
      
      //--------------------------------------------------------------------------
      // Now pull all this together and add a signature object to the database
      // (Don't forget I have to pull the content in here as well!)
      try
        {
          _stack.debug("Adding New Signature Record");

          // Note: 'uint' is a typedef of Andy Klapper's with the
          // following definition:
          // src/aviscommon/Avis.h:20:typedef unsigned int uint
          _success =  Signature::AddNew((uint)_sequence_number, _blessed,
                                        _produced  , _published,
                                        _signature_checksum
                                        );
        }
      catch(AVISDBException& e_)
        {
          _stack.error(e_.DetailedInfo());
          throw ServletRecoverable(e_.DetailedInfo().c_str(),"unpublished");
        }
    }
  return _success;
}

bool AvisPostSignature::writeSignatureFile()
{
  StackTrace _stack(*_tracer,"writeSignatureFile");
  long _contentLength = atol(_gwapi.incomingHeader("Content-Length").c_str());
  long _bytesRead = 0;

  
      
  // Pull the content and drop it on disk in the "right location"
  // Get the content length so I know how much to pull
      
  if (_contentLength)
    {
      MD5Calculator _md5;
      long _bufferFill = 0;
      
      std::string _msg("");

      FILE* _ofile = fopen(_package_file.c_str(),"wb");
      if (_ofile == NULL)
        {
          _msg ="Unable to create file: ";
          _msg += _package_file;
          throw ServletRecoverable(_msg.c_str(),"unpublished");
        }
      // Strictly speaking, this else condition is completely
      // unnecessary, since the exception is thrown in the opposing
      // case. However, to retain the semantics of "one entrypoint,
      // one exitpoint" if in readability only, I'm leaving it in.

      else
        {
          // Create a buffer of 8k.  This is an arbitrary size based
          // on nothing but the notion that... well, these files are
          // big.
          // Properly, this entire set of operations should go in the
          // GoSession class as a "save_content_stream(const char*)"
          // method.

          long _bufferSize = 8192;
          char* _buffer = new char[_bufferSize];
              
          // While we haven't read the full file,
          // And the web server doesn't think it's done
          while ( (_bytesRead < _contentLength) && (! _gwapi.eof()))
            {
                  
              // Read a buffer block
              _bufferFill = _gwapi.getContentChunk((unsigned char*)_buffer,_bufferSize);
                  
                  
              // Write to the file
              fwrite(_buffer,_bufferFill,1,_ofile);
                  
              // tick the MD5
              _md5.CRCCalculate((const uchar*)_buffer,_bufferFill);
                  
              // Increment the total bytecount
              _bytesRead += _bufferFill;
                  
            }
          fclose(_ofile);
          delete[] _buffer;
        } // if _ofile

      //------------------------------------------------
      // Tie off the MD5 & check against header value
      _md5.Done(_content_hash);
      // Log the bytes read count
      //            _msg  = "Bytes Read: ";
      //            _msg += StringUtil::ltos(_bytesRead);
      //            _tracer->msg(_msg);
      // The hash key is worth logging
      _msg = "Calculated content hash: ";
      _msg += _content_hash;
      _tracer->msg(_msg);
      //------------------------------------------------
      // Manually check the content hash against the
      // reported hash, 
      // Kill the file & throw recoverable if it's bad
      //------------------------------------------------
      StringUtil::strlwr(_content_hash);
      std::string _incoming_hash = _gwapi.incomingHeader("X-Content-Checksum");
      if (_content_hash != StringUtil::strlwr(_incoming_hash))
        {
          // Kill the file we've just created
          unlink(_package_file.c_str());
          // Throw a "Servlet Recoverable"
          _msg = "corrupted ";
          _msg += _content_hash;
          _msg += " ";
          _msg += _gwapi.incomingHeader("X-Content-Checksum");
          throw ServletRecoverable("Corrupted Content Stream",_msg.c_str());
        }
      //------------------------------------------------

    }
  return (_bytesRead == _contentLength);
}

void AvisPostSignature::explodeSignatureFile()
{
  StackTrace _stack(*_tracer,"explodeSignatureFile");
  
  // Tell them what we're doing
  std::string _msg("Exploding Signature Package: ");
  _msg += _package_file;
  _stack.info(_msg);

  // Note: These will throw ServletRecoverable if there's a problem

  // Create the mutex for Milosz' unpacking methods. Note that this
  // will unlock automatically on object destruction (i.e. when the
  // stack unravels, handling the "exception" case)
  NamedMutex _mutex("SIGNATUREUNPACK");
  
  if (_gwapi.incomingHeader("Content-Type") == "application/x-NAV-VDB")
    {
      unpackVDB(_mutex);
    }
  else
    {
      throw ServletRecoverable("Unknown NAV-Package type (from Content-Type)","unpublished");
    }
  //--------------------------------------------------------------------------
  
}

void AvisPostSignature::createSigPath() const
{
  StackTrace _stack(*_tracer,"createSigPath");

  int _ecode = _mkdir(_package_path.c_str()); // Create the destination directory

  // We're good if _ecode == 0
  // or if the directory already exists

  if (! (( _ecode == 0) || ( (_ecode == -1) && (errno == EEXIST))))
    {
      throw BadPath(_package_path.c_str());
    }
}

                                         
void AvisPostSignature::unpackVDB(NamedMutex& mutex_)
{
  StackTrace _stack(*_tracer,"unpackVDB");
  // There's not much I can do here until Milosz' code is done
  // Which it is ;)

  VDBPackage _definition_package;
    if ( _definition_package.Unpack( _package_file,
                                     _package_path,
                                     mutex_.get_handle()
                                     ) == false)
      {
        

        VDBPackage::ErrorCode _ecode = _definition_package.Error();
        switch (_ecode)
          {
          case VDBPackage::NoError:
            {
              _stack.info("Package was unpacked successfully");
            }
            break;
          case VDBPackage::PackageNotFoundError:
            {
              throw ServletRecoverable("VDBPackage::PackageNotFoundError encountered","unpublished");
            }
            break;
          case VDBPackage::TargetDirError:
            {
              throw ServletRecoverable("VDBPackage::TargetDirError encountered","unpublished");
            }
            break;
          case VDBPackage::NoFilesFoundError:
            {
              throw ServletRecoverable("VDBPackage::NoFilesFoundError encountered","unpublished");
            }
            break;
          case VDBPackage::DiskFullError:
            {
              throw ServletRecoverable("VDBPackage::DiskFullError encountered","unpublished");
            }
            break;
          case VDBPackage::VersionError:
            {
              throw ServletRecoverable("VDBPackage::VersionError encountered","unpublished");
            }
            break;
          case VDBPackage::SystemException:
            {
              // Method from VDBPackage...
              // meaningful when Error() returns SystemException
              // virtual	unsigned long	ExceptionCode();	

              std::string _msg("VDBPackage::SystemException '");
              _msg.append(StringUtil::ltos(_definition_package.ExceptionCode()));
              _msg.append("' encountered");
              
              throw ServletRecoverable(_msg.c_str(),"unpublished");
            }
            break;
          case VDBPackage::ZipArchiveError:
            {
              throw ServletRecoverable("VDBPackage::ZipArchiveError encountered","unpublished");
            }
            break;
          case VDBPackage::MemoryError:
            {
              throw ServletRecoverable("VDBPackage::MemoryError encountered","unpublished");
            }
            break;
          case VDBPackage::ParameterError:
            {
              throw ServletRecoverable("VDBPackage::ParameterError encountered","unpublished");
            }
            break;
          case VDBPackage::ZipNoFilesFoundError:
            {
              throw ServletRecoverable("VDBPackage::ZipNoFilesFoundError encountered","unpublished");
            }
            break;
          case VDBPackage::UnexpectedEOFError:
            {
              throw ServletRecoverable("VDBPackage::UnexpectedEOFError encountered","unpublished");
            }
            break;
          case VDBPackage::ZipUnknownError:
            {
              throw ServletRecoverable("VDBPackage::ZipUnknownError encountered","unpublished");
            }
            break;
          case VDBPackage::FileRemovingError:
            {
              throw ServletRecoverable("VDBPackage::FileRemovingError encountered","unpublished");
            }
            break;
          case VDBPackage::DirPruningError:
            {
              throw ServletRecoverable("VDBPackage::DirPruningError encountered","unpublished");
            }
            break;
          default:
            throw ServletRecoverable("VDBPackage::Unpack returned unknown code!!!","unpublished");
          }
      }
}
