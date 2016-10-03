//
// IBM AntiVirus Immune System
//
// File Name: ZExcept.cpp
// Author:    Milosz Muszynski
//
// Class ZException and its descendants implement the system of exceptions
// for the ZlibTool module 
// Class ZExceptionTranslator translates Windows system exceptions
// to exceptions belonging to the ZException class hierarchy
//
// $Log: $
//

#include <string>

class ZException
{
private:
    const int          _line;
    const std::string  _file;
    const std::string  _text;
	const unsigned int _code;
public:
    // constructors/destructors
    ZException( const std::string& text, unsigned int code, int line, const std::string& file );
    // accessors
    const std::string&    text() const { return _text; }
    int                   line() const { return _line; }
    const std::string&    file() const { return _file; }
    int                   code() const { return _code; }
};


class ZSystemException : public ZException
{
public:
    ZSystemException( const std::string& text, unsigned int code, int line, const std::string& file ) 
        : ZException( text, code, line, file ){}
};


class ZExceptionTranslator
{
private:
    bool                     _translatorSet;
    _se_translator_function	 _oldTranslatorFunction;
public:
    ZExceptionTranslator() : _translatorSet( false ){}
    ~ZExceptionTranslator();
    void init();

};
