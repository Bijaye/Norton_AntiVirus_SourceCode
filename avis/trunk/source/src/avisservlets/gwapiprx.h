// gwapiprx.h - Prototype for the GwapiProxy class

#if !defined (__GWAPIPROXY_H__)
#define __GWAPIPROXY_H__

class TraceClient;
class MTrace;

//
//: Interface proxy to the Lotus Go server.
//
//
//  This is a straight API encapsulation job.
//  The first cut of this is to 1:1 the api, pulling the "handle"
//  out into the class.
//
//  GwapiProxy contains the simple pass-through methods (with some type
//  normalization) for the GWAPI interface.
//
//  There are a number of methods that use strings (unsigned char*)
//  and their lengths as parameters, but do not modify either.
//  For those, I'm building overloads that take only the const char*
//  and will do the substitutions.
//  
//  Please note, that as with other value-adds, I will be adding them 
//  on an as-needed basis.  So if the set is not complete, 
//  that will most likely be why.
//

class GwapiProxy
{

public:

  //
  //: query the valid state of the object instance
  //

  virtual bool validObject() const;

  //
  //: Return codes from the Gwapi methods
  //
  // These two enums are very handy to avoid including 'htapi.h' in every
  // file here. (Which would defeat the purpose of encapuslting the API
  // rather handily, don't you think?)
  //

  enum HTTPD_Codes
  {
    _HTTPD_UNSUPPORTED         = -1,
    _HTTPD_SUCCESS             = 0,
    _HTTPD_FAILURE             = 1,
    _HTTPD_INTERNAL_ERROR      = 2,
    _HTTPD_PARAMETER_ERROR     = 3,
    _HTTPD_STATE_CHECK         = 4,
    _HTTPD_READ_ONLY           = 5,
    _HTTPD_BUFFER_TOO_SMALL    = 6,
    _HTTPD_AUTHENTICATE_FAILED = 7,
    _HTTPD_EOF                 = 8,
    _HTTPD_ABORT_REQUEST       = 9,
    _HTTPD_REQUEST_SERVICED    = 10,
    _HTTPD_RESPONSE_ALREADY_COMPLETED   = 11
  };

  //
  //: Full set of HTTP protocol response codes
  //

  enum HTTP_Codes
  {
    /* ICS specific return codes */
    _HTTP_REQUEST_SERVICED_LOG = ( -1 ),
    _HTTP_NOACTION             =  0,
    
    /* 1xx Informational */
    _HTTP_CONTINUE            = 100,
    _HTTP_SWITCHING_PROTOCOLS = 101,
    
    /* 2xx Successful */
    _HTTP_OK                  = 200,
    _HTTP_CREATED             = 201,
    _HTTP_ACCEPTED            = 202,
    _HTTP_NON_AUTHORITATIVE   = 203,
    _HTTP_NO_CONTENT          = 204,
    _HTTP_RESET_CONTENT       = 205,
    _HTTP_PARTIAL_CONENT      = 206,
    
    /* 3xx Redirection */
    _HTTP_MULTIPLE_CHOICES    = 300,
    _HTTP_MOVED_PERMANENTLY   = 301,
    _HTTP_MOVED_TEMPORARILY   = 302,
    _HTTP_SEE_OTHER           = 303,
    _HTTP_NOT_MODIFIED        = 304,
    _HTTP_USE_PROXY           = 305,

    /* 4xx Client Error */
    _HTTP_BAD_REQUEST         = 400,
    _HTTP_UNAUTHORIZED        = 401,
    _HTTP_FORBIDDEN           = 403,
    _HTTP_NOT_FOUND           = 404,
    _HTTP_METHOD_NOT_ALLOWED  = 405,
    _HTTP_NOT_ACCEPTABLE      = 406,
    _HTTP_PROXY_UNAUTHORIZED  = 407,
    _HTTP_REQUEST_TIMEOUT     = 408,
    _HTTP_CONFLICT            = 409,
    _HTTP_GONE                = 410,
    _HTTP_LENGTH_REQUIRED     = 411,
    _HTTP_PRECONDITION_FAILED = 412,
    _HTTP_ENTITY_TOO_LARGE    = 413,
    _HTTP_URI_TOO_LONG        = 414,
    _HTTP_BAD_MEDIA_TYPE      = 415,
    
    /* 5xx Server Error */
    _HTTP_SERVER_ERROR        = 500,
    _HTTP_NOT_IMPLEMENTED     = 501,
    _HTTP_BAD_GATEWAY         = 502,
    _HTTP_SERVICE_UNAVAILABLE = 503,
    _HTTP_GATEWAY_TIMEOUT     = 504,
    _HTTP_BAD_VERSION         = 505
  };

  //
  //: Must be constructed with a handle. (retrieved from the Go Server)
  //

  GwapiProxy( const unsigned char * handle , MTrace& trace_server_);

  //
  //: Virtual destructor
  //

  virtual ~GwapiProxy();

  //
  //: Handle (passed in on Construction from Go Server)
  //

  unsigned char* _handle;


  //
  //: type normalized passthrough to the lotus go webserver api. (GWAPI)
  //

   long extract           ( unsigned char* name_,
                           unsigned char* value_,
                           unsigned long* value_length_);

  //
  //: type normalized passthrough to the lotus go webserver api. (GWAPI)
  //

  long set               ( unsigned char* name_, 
                           unsigned char* value_);

  //
  //: type normalized passthrough to the lotus go webserver api. (GWAPI)
  //

  long file              ( unsigned char* name_);

  //
  //: type normalized passthrough to the lotus go webserver api. (GWAPI)
  //

  long exec              ( unsigned char *name_);

  //
  //: type normalized passthrough to the lotus go webserver api. (GWAPI)
  //

  long write             ( unsigned char *value_);

  //
  //: type normalized passthrough to the lotus go webserver api. (GWAPI)
  //

  long log_error         ( unsigned char* value_);

  //
  //: type normalized passthrough to the lotus go webserver api. (GWAPI)
  //

  long log_trace         ( unsigned char* value_);

  //
  //: type normalized passthrough to the lotus go webserver api. (GWAPI)
  //

  long proxy             ( unsigned char *url_name_,
                           unsigned char *request_body_);

  //
  //: type normalized passthrough to the lotus go webserver api. (GWAPI)
  //

  long attributes        ( unsigned char* name_,
                           unsigned char* value_,
                           unsigned long& value_length_);
  

  //
  //: type normalized passthrough to the lotus go webserver api. (GWAPI)
  //
  // Parameters:
  //
  // -     unsigned char *name : Input, name of the file
  //
  // -     unsigned char *value: Output, buffer which contains the
  //       attributes
  //
  // -     unsigned long *value_length: Input/Output, size of the
  //       buffer/length of the attributes
  //
  
  long attributes        ( unsigned char *name,         
                           unsigned char *value,        
                           unsigned long *value_length);

  //
  //: type normalized passthrough to the lotus go webserver api. (GWAPI)
  //

  long reverse_translate ( unsigned char *name_,        
                           unsigned char *value_,       
                           unsigned long *value_length_);
  

  //
  //: type normalized passthrough to the lotus go webserver api. (GWAPI)
  //

  long supply_label      ( unsigned char *value_);



  //
  //: Direct passthrough to the lotus go webserer api.
  //
  // authenticate a userid/password                
  // valid only in PreExit, Authentication, and Authorization steps
  //
  
  long authenticate();

  //
  //: Direct passthrough to the lotus go webserer api.
  //
  // extract the value of a variable associated with this request 
  // the available variables are the same as CGI                  
  // valid in all steps, though not all variables will be.
  //
  // Parameters:
  //
  // -    unsigned char *name: Input, name of the value to extract
  // 
  // -    unsigned long *name_length: Input, length of the name
  //
  // -    unsigned char *value: Output, buffer in which to place the
  // value
  //
  // -    unsigned long *value_length: Input/Output, size of
  // buffer/length of value
  //

  long extract(unsigned char *name,         
               unsigned long *name_length,  
               unsigned char *value,        
               unsigned long *value_length);
  


  //
  // set the value of a variable associated with this request    
  // the available variables are the same as CGI                 
  // - note that one can create variables with this function     
  // - if created variables are prefixed by "HTTP_", they will   
  //   be sent as headers in the response, without the "HTTP_"    
  //   prefix                                                     
  //   valid in all steps, though not all variables are.
  //
  //
  // Parameters
  //
  //   - unsigned char *name,         /* i; name of the value to set */
  //
  //   - unsigned long *name_length,  /* i; length of the name */
  //
  //   - unsigned char *value,        /* o; buffer which contains the value */
  //
  //   - unsigned long *value_length); /* i; length of value */
  //
  
  long set(unsigned char *name,         
           unsigned long *name_length,  
           unsigned char *value,        
           unsigned long *value_length);

  //
  // send a file to satisfy this request
  // valid only in PreExit, Service, NameTrans, Error and Datafilter
  // steps
  //
  // Parameters:
  //
  // - unsigned char *name: i; name of file to send
  //
  // - unsigned long *name_length: i; length of the name

  long file(unsigned char *name,
            unsigned long *name_length);
  
  
  // execute a script to satisfy this request 
  // valid only in PreExit, Service, NameTrans and Error steps  
  //
  // Parameters:
  //
  // - unsigned char *name: i; name of script to execute
  //
  // - unsigned long *name_length: i; length of the name
  //
  
  long exec(unsigned char *name, 
            unsigned long *name_length);
  

  //   read the body of the client's request - use set/extract for headers 
  //   Keep reading until HTTPD_EOF is returned; 4k is a good buffer size  
  //   valid only in the PreExit, Service, and Datafilter steps 
  //
  // Parameters:
  //
  // - unsigned char *value: i; buffer in which to place the value
  //
  // - unsigned long *value_length: i/o; size of buffer/length of
  // header
  //
  long read(unsigned char *value,        
            unsigned long *value_length);


  // write the body of the response - use set/extract for headers 
  // valid only in PreExit, Service, NameTrans, Error and Datafilter steps  
  //
  // Parameters
  //
  // - unsigned char *value: i; data to send
  //
  // - unsigned char *value_length: i; length of data to send
  //
  
  long write(unsigned char *value,        
             unsigned long *value_length);


  // write a string to the server's error log 
  // valid in all steps 
  //
  // Parameters
  //
  // - unsigned char *value: i; data to write
  //
  // - unsigned long *value_length: i; length of data
  //

  long log_error(unsigned char *value,        
                 unsigned long *value_length);
  
  
  // write a string to the server's trace log  valid in all steps
  //
  // Parameters
  //
  // - unsigned char *value: i; data to write
  //
  // - unsigned long *value_length: i; length of the data
  //

  long log_trace(unsigned char *value,        
                 unsigned long *value_length);
  
  

  /* restart the server after all active requests have been processed */
  /* valid in all steps except ServerInit and ServerTerm */

  long restart();
  
  // make a proxy request valid in PreExit and Service steps  Note:
  // This is a completion function; i.e, the response is complete
  // after this
  //
  // Parameters
  //
  // - unsigned char *url_name,     /* i; url to which the proxy request is made */
  //
  // - unsigned long *name_length,  /* i; length of the url */
  //
  // - unsigned char *request_body, /* i; body of the request */
  //
  // - unsigned long *body_length);  /* i; length of the body */
  //

  long proxy(unsigned char *url_name,    
             unsigned long *name_length, 
             unsigned char *request_body,
             unsigned long *body_length);
  
  /* get the attributes of a file */
  /* valid in all steps */
  //
  // Parameters
  //
  // - unsigned char *name,         /* i; name of the file */
  //
  // - unsigned long *name_length,  /* i; length of the name */
  //
  // - unsigned char *value,        /* o; buffer which contains the attributes */
  //
  // - unsigned long *value_length); /* i/o; size of buffer/length of
  // attributes */
  //

  long attributes(unsigned char *name,         
                  unsigned long *name_length,  
                  unsigned char *value,        
                  unsigned long *value_length);
  
  /* reverse translate a file system object to a URL */
  /* valid in all steps */
  //
  //
  // Parameters
  //
  // - unsigned char *name,         /* i; name of the file system object */
  //
  // - unsigned long *name_length,  /* i; length of the name */
  //
  // - unsigned char *value,        /* o; buffer which contains the URL */
  //
  // - unsigned long *value_length); /* i/o; size of buffer/length of
  // URL */
  //
  
  long reverse_translate(unsigned char *name,         
                         unsigned long *name_length,  
                         unsigned char *value,        
                         unsigned long *value_length);
  
  /* translate a URL */
  /* valid in all steps */

  //
  // Parameters
  //
  // - unsigned char *name,         /* i; name of the URL */
  //
  // - unsigned long *name_length,  /* i; length of the name */
  //
  // - unsigned char *url_value,    /* o; buffer which contains the translated URL */
  //
  // - unsigned long *url_value_length, /* i/o; size of buffer/length of translated URL */
  //
  // - unsigned char *path_trans,   /* o; buffer which contains PATH_TRANSLATED */
  //
  // - unsigned long *path_trans_length, /* i/o; size of buffer/length of PATH_TRANSLATED */
  //
  // - unsigned char *query_string, /* o; buffer which contains QUERY_STRING */
  //
  // - unsigned long *query_string_length); /* i/o; size of buffer/length of QUERY_STRING */

  long translate(unsigned char *name,            
                 unsigned long *name_length,     
                 unsigned char *url_value,       
                 unsigned long *url_value_length,
                 unsigned char *path_trans,    
                 unsigned long *path_trans_length,
                 unsigned char *query_string,  
                 unsigned long *query_string_length); 

  //
  // Parameters
  //
  // - unsigned char *value,        /* i; PICS label being supplied */
  //
  // - unsigned long *value_size);   /* i; length of the PICS label */
  //

  long supply_label(unsigned char *value,        
                    unsigned long *value_size);  
  



private:

  //
  //: Standard Trace Object
  //

  TraceClient* _tracer;

};



#endif // __GWAPIPROXY_H__


