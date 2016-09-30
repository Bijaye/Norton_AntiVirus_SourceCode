#ifndef HTAPI_H
#define HTAPI_H

/*
 * Return values from user-written functions
 * Refer to the Web Programming Guide and HTTP specification for details.
 */

/* ICS specific return codes */
#define HTTP_REQUEST_SERVICED_LOG -1
#define HTTP_NOACTION              0

/* 1xx Informational */
#define HTTP_CONTINUE            100
#define HTTP_SWITCHING_PROTOCOLS 101

/* 2xx Successful */
#define HTTP_OK                  200
#define HTTP_CREATED             201
#define HTTP_ACCEPTED            202
#define HTTP_NON_AUTHORITATIVE   203
#define HTTP_NO_CONTENT          204
#define HTTP_RESET_CONTENT       205
#define HTTP_PARTIAL_CONENT      206

/* 3xx Redirection */
#define HTTP_MULTIPLE_CHOICES    300
#define HTTP_MOVED_PERMANENTLY   301
#define HTTP_MOVED_TEMPORARILY   302
#define HTTP_SEE_OTHER           303
#define HTTP_NOT_MODIFIED        304
#define HTTP_USE_PROXY           305

/* 4xx Client Error */
#define HTTP_BAD_REQUEST         400
#define HTTP_UNAUTHORIZED        401
#define HTTP_FORBIDDEN           403
#define HTTP_NOT_FOUND           404
#define HTTP_METHOD_NOT_ALLOWED  405
#define HTTP_NOT_ACCEPTABLE      406
#define HTTP_PROXY_UNAUTHORIZED  407
#define HTTP_REQUEST_TIMEOUT     408
#define HTTP_CONFLICT            409
#define HTTP_GONE                410
#define HTTP_LENGTH_REQUIRED     411
#define HTTP_PRECONDITION_FAILED 412
#define HTTP_ENTITY_TOO_LARGE    413
#define HTTP_URI_TOO_LONG        414
#define HTTP_BAD_MEDIA_TYPE      415

/* 5xx Server Error */
#define HTTP_SERVER_ERROR        500
#define HTTP_NOT_IMPLEMENTED     501
#define HTTP_BAD_GATEWAY         502
#define HTTP_SERVICE_UNAVAILABLE 503
#define HTTP_GATEWAY_TIMEOUT     504
#define HTTP_BAD_VERSION         505

/* Return values from ICS provided functions */
#define HTTPD_UNSUPPORTED        -1
#define HTTPD_SUCCESS             0
#define HTTPD_FAILURE             1
#define HTTPD_INTERNAL_ERROR      2
#define HTTPD_PARAMETER_ERROR     3
#define HTTPD_STATE_CHECK         4
#define HTTPD_READ_ONLY           5
#define HTTPD_BUFFER_TOO_SMALL    6
#define HTTPD_AUTHENTICATE_FAILED 7
#define HTTPD_EOF                 8
#define HTTPD_ABORT_REQUEST       9
#define HTTPD_REQUEST_SERVICED    10
#define HTTPD_RESPONSE_ALREADY_COMPLETED   11

/* prototypes for ICS provided functions */

#if defined(__OS2__)
#define HTTPD_LINKAGE _System
#elif defined(WIN32)
#define HTTPD_LINKAGE __stdcall
#else
#define HTTPD_LINKAGE
#endif

/* Long name to short name mapping */
#define HTTPD_authenticate   HTAUTHEN
#define HTTPD_extract        HTXTRACT
#define HTTPD_set            HTSET
#define HTTPD_file           HTFILE
#define HTTPD_exec           HTEXEC
#define HTTPD_read           HTREAD
#define HTTPD_write          HTWRITE
#define HTTPD_log_error      HTLOGE
#define HTTPD_log_trace      HTLOGT
#define HTTPD_restart        HTREST
#define HTTPD_proxy          HTPROXY
#define HTTPD_attributes     HTATTRIB
#define HTTPD_reverse_translate      HTREVTRA
#define HTTPD_translate      HTTRANS
#define HTTPD_supply_label   HTSUPLBL

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* authenticate a userid/password                */
/* valid only in PreExit, Authentication, and Authorization steps */
void
HTTPD_LINKAGE
HTTPD_authenticate(
             unsigned char *handle,  /* i; handle (NULL right now) */
             long *return_code);     /* o; return code */

/* extract the value of a variable associated with this request */
/* the available variables are the same as CGI                  */
/* valid in all steps, though not all variables will be         */
void
HTTPD_LINKAGE
HTTPD_extract(
    unsigned char *handle,       /* i; handle (NULL right now) */
    unsigned char *name,         /* i; name of the value to extract */
    unsigned long *name_length,  /* i; length of the name */
    unsigned char *value,        /* o; buffer in which to place the value */
    unsigned long *value_length, /* i/o; size of buffer/length of value */
             long *return_code); /* o; return code */

/* set the value of a variable associated with this request     */
/* the available variables are the same as CGI                  */
/* - note that one can create variables with this function      */
/* - if created variables are prefixed by "HTTP_", they will    */
/*   be sent as headers in the response, without the "HTTP_"    */
/*   prefix                                                     */
/* valid in all steps, though not all variables are             */
void
HTTPD_LINKAGE
HTTPD_set(
    unsigned char *handle,       /* i; handle (NULL right now) */
    unsigned char *name,         /* i; name of the value to set */
    unsigned long *name_length,  /* i; length of the name */
    unsigned char *value,        /* o; buffer which contains the value */
    unsigned long *value_length, /* i; length of value */
             long *return_code); /* o; return code */

/* send a file to satisfy this request */
/* valid only in PreExit, Service, NameTrans, Error and Datafilter steps  */
void
HTTPD_LINKAGE
HTTPD_file(
    unsigned char *handle,       /* i; handle (NULL right now) */
    unsigned char *name,         /* i; name of file to send */
    unsigned long *name_length,  /* i; length of the name */
             long *return_code); /* o; return code */

/* execute a script to satisfy this request */
/* valid only in PreExit, Service, NameTrans and Error steps  */
void
HTTPD_LINKAGE
HTTPD_exec(
    unsigned char *handle,       /* i; handle (NULL right now) */
    unsigned char *name,         /* i; name of script to execute */
    unsigned long *name_length,  /* i; length of the name */
             long *return_code); /* o; return code */

/* read the body of the client's request - use set/extract for headers */
/* Keep reading until HTTPD_EOF is returned; 4k is a good buffer size  */
/* valid only in the PreExit, Service, and Datafilter steps  */
void
HTTPD_LINKAGE
HTTPD_read(
    unsigned char *handle,       /* i; handle (NULL right now) */
    unsigned char *value,        /* i; buffer in which to place the value */
    unsigned long *value_length, /* i/o; size of buffer/length of header */
             long *return_code); /* o; return code */

/* write the body of the response - use set/extract for headers */
/* valid only in PreExit, Service, NameTrans, Error and Datafilter steps  */
void
HTTPD_LINKAGE
HTTPD_write(
    unsigned char *handle,       /* i; handle (NULL right now) */
    unsigned char *value,        /* i; data to send */
    unsigned long *value_length, /* i; length of the data */
             long *return_code); /* o; return code */

/* write a string to the server's error log */
/* valid in all steps */
void
HTTPD_LINKAGE
HTTPD_log_error(
    unsigned char *handle,       /* i; handle (NULL right now) */
    unsigned char *value,        /* i; data to write */
    unsigned long *value_length, /* i; length of the data */
             long *return_code); /* o; return code */

/* write a string to the server's trace log */
/* valid in all steps */
void
HTTPD_LINKAGE
HTTPD_log_trace(
    unsigned char *handle,       /* i; handle (NULL right now) */
    unsigned char *value,        /* i; data to write */
    unsigned long *value_length, /* i; length of the data */
             long *return_code); /* o; return code */


/* restart the server after all active requests have been processed */
/* valid in all steps except ServerInit and ServerTerm */
void
HTTPD_LINKAGE
HTTPD_restart(
             long *return_code); /* o; return code */

/* make a proxy request */
/* valid in PreExit and Service steps  */
/* Note: This is a completion function; i.e, the response is complete after this */
void
HTTPD_LINKAGE
HTTPD_proxy(
    unsigned char *handle,       /* i; handle (NULL right now) */
    unsigned char *url_name,     /* i; url to which the proxy request is made */
    unsigned long *name_length,  /* i; length of the url */
    unsigned char *request_body, /* i; body of the request */
    unsigned long *body_length,  /* i; length of the body */
             long *return_code); /* o; return code */

/* get the attributes of a file */
/* valid in all steps */
void
HTTPD_LINKAGE
HTTPD_attributes(
    unsigned char *handle,       /* i; handle (NULL right now) */
    unsigned char *name,         /* i; name of the file */
    unsigned long *name_length,  /* i; length of the name */
    unsigned char *value,        /* o; buffer which contains the attributes */
    unsigned long *value_length, /* i/o; size of buffer/length of attributes */
             long *return_code); /* o; return code */

/* reverse translate a file system object to a URL */
/* valid in all steps */
void
HTTPD_LINKAGE
HTTPD_reverse_translate(
    unsigned char *handle,       /* i; handle (NULL right now) */
    unsigned char *name,         /* i; name of the file system object */
    unsigned long *name_length,  /* i; length of the name */
    unsigned char *value,        /* o; buffer which contains the URL */
    unsigned long *value_length, /* i/o; size of buffer/length of URL */
             long *return_code); /* o; return code */

/* translate a URL */
/* valid in all steps */
void
HTTPD_LINKAGE
HTTPD_translate(
    unsigned char *handle,       /* i; handle (NULL right now) */
    unsigned char *name,         /* i; name of the URL */
    unsigned long *name_length,  /* i; length of the name */
    unsigned char *url_value,    /* o; buffer which contains the translated URL */
    unsigned long *url_value_length, /* i/o; size of buffer/length of translated URL */
    unsigned char *path_trans,   /* o; buffer which contains PATH_TRANSLATED */
    unsigned long *path_trans_length, /* i/o; size of buffer/length of PATH_TRANSLATED */
    unsigned char *query_string, /* o; buffer which contains QUERY_STRING */
    unsigned long *query_string_length, /* i/o; size of buffer/length of QUERY_STRING */
             long *return_code); /* o; return code */


void
HTTPD_LINKAGE
HTTPD_supply_label(
    unsigned char *handle,       /* i; handle */
    unsigned char *value,        /* i; PICS label being supplied */
    unsigned long *value_size,   /* i; length of the PICS label */
             long *return_code); /* o; return code */


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* HTAPI_H */
