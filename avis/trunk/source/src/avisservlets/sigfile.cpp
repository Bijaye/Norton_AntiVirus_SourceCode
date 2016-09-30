#pragma warning (disable : 4786)
#include "sigfile.h"


#include "avisfile/avisfile.h"
#include "avisfile/md5/md5calculator.h"

#include "aviscommon/log/mtrace.h"
#include "aviscommon/log/traceclient.h"
#include "aviscommon/log/stacktrace.h"
#include "aviscommon/log/utilexception.h"

#include <sys/stat.h>
#include <stdio.h>

SignatureFile::SignatureFile(MTrace& trace_server_,
                             const std::string& root_path_,
                             const std::string& sequence_)
  :_filename(""),
   _sequence(sequence_),
   _checksum(""),
   _content_type(""),
   _content(NULL),
   _content_length(0),
   _tracer(NULL)
{
  std::string _module("SignatureFile[");
  _module.append(_sequence);
  _module.append("]");
  _tracer = new TraceClient(trace_server_,_module.c_str());

  _filename = derive_file_name(root_path_,sequence_);
  slurp(_filename);
}
SignatureFile::~SignatureFile()
{
  _filename = "";
  _sequence = "";
  _checksum = "";
  _content_type = "";
  delete[] _content;
  _content_length = 0;
  delete _tracer;
  _tracer = NULL;
}


const std::string& SignatureFile::get_checksum() const
{
  return _checksum;
}

long SignatureFile::get_content_length() const
{
  return _content_length;
}

const char* SignatureFile::get_content() const
{
  return _content;
}

const std::string& SignatureFile::get_content_type() const
{
  return _content_type;
}


void SignatureFile::slurp(const std::string& filename_)
{
  StackTrace _stack(*_tracer,"slurp");
  
  FILE* _ifile = fopen(filename_.c_str(),"rb");
  if (_ifile)
    {
      _stack.debug("Signature file opened");
      fseek(_ifile,0,SEEK_END);
      _content_length = ftell(_ifile);
      // Rewind to beginning
      fseek(_ifile,0,SEEK_SET);
      // Allocate buffer
      _content = new char[_content_length];
      // Load into buffer
      fread(_content,_content_length,1,_ifile);
      fclose(_ifile);
    }
  _checksum = calculate_md5(_content,_content_length);
}

std::string SignatureFile::derive_file_name(const std::string& root_path_,
                                            const std::string& sequence_)
{
  StackTrace _stack(*_tracer,"derive_file_name");
  // This finds the file, whether it's a VDB or FatAlbert package.
  // This is where all the "is it a vdb by experimentation" code goes.
  std::string _fullpath(root_path_);
  _fullpath.append("\\");
  _fullpath.append(sequence_);
  _fullpath.append(".exe");

  struct _stat _stat_buffer;
  //  struct _stat _stat_buffer;
  
  // Check for Fat Albert
  // Note: The only valid error from _stat (-1) sets errno to NOENT,
  // indicating that the file entry specified does not exist.
  // Therefore, if _stat returns -1, the file doesn't exist.  No need
  // to perform a redundant test.
  
  std::string _msg("Looking for ");
  _msg.append(_fullpath);
  if (_stat(_fullpath.c_str(),&_stat_buffer) == -1)
    {
      // If there's no appropriate FatAlbert package, there'd better
      // be a VDB
      // Build the appropriate VDB filename
      _fullpath.replace(_fullpath.length() - 3,3,"vdb");
      _msg = "Now trying: ";
      _msg.append(_fullpath);
      _stack.debug(_msg);
      // Now test for that
      if (_stat(_fullpath.c_str(),&_stat_buffer) == -1)
        {
          _stack.error("Unable to load signature file");
          // Couldn't find either.
          _fullpath = "";
        }
      else
        {
          _content_type = "application/x-NAV-VDB";
        }
    }
  else
    {
      _content_type = "application/x-NAV-FatAlbert";
    }
  return _fullpath;  
}


std::string SignatureFile::calculate_md5(const char* content_,
                                         long length_) const
{
  StackTrace _stack(*_tracer,"calculate_md5");
  // Resorting to an external library :(
  MD5Calculator _md5er;
  _md5er.CRCCalculate((const uchar*)content_,length_);
  std::string _md5hash;
  _md5er.Done(_md5hash);
  return _md5hash;
}
