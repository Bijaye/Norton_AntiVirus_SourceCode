//
// IBM AntiVirus Immune System
//
// File Name: ZlibTool.h
// Author:    Milosz Muszynski
//
// This class represents a connection between AVIS compression and
// decompression users and the ZLIB compression engine
// This class provides an easier to use unterface for ZLIB functions
// 
// The idea for this class should be credited to Mark Nelson and
// his ZlibEngine class
//
// $Log: $
//

#ifndef _ZLIBTOOL_H_
#define _ZLIBTOOL_H_

#include "AVIS.h"
#include "Zlib.h"

//
//
// class ZlibTool - internal
//
//


class ZlibTool : public z_stream
{
public:
	 ZlibTool();
	 virtual ~ZlibTool();

	 enum { DefaultCompressionLevel = 6 };

     bool Inflate        ( const uchar* in, uint& inSize, uchar* out, uint& outSize );
     bool InflateDone    ( uchar* out, uint& outSize);

     bool Deflate        ( const uchar* in, uint& inSize, uchar* out, uint& outSize, int level = DefaultCompressionLevel );
     bool DeflateDone    ( uchar* out, uint& outSize);

	 int  Error          (){ return _err; }

protected:
	 void Init           ();

	 bool _first;
	 bool _done; // CKF added
	 int  _err;
};


#endif //#ifndef _ZLIBTOOL_H_
