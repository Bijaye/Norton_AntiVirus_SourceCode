//
// IBM AntiVirus Immune System
//
// File Name: Encoder.cpp
// Author:    Milosz Muszynski
//
// This class represents an interface for 
// a variety of encoding tools available
//
// This class is intended for an external use
// it is a pure abstract class intended to be a base class
// for classes that provide real functionality
// 
// $Log: $
//


#include "stdafx.h"
#include <string>
#include <memory>

#include "AVISFile.h"
#include "Encoder.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Encoder::Encoder()
{
}

Encoder::~Encoder()
{
}
