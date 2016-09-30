
#include "stdafx.h"

#include "AVISException.h"
#include <iostream>

using namespace std;

void AVISException::LogException()
{
	std::cerr << ClassAsString() << " [" << TypeAsString();
	std::cerr << "] thrown, detailed info to follow." << std::endl;
	std::cerr << "[ " << DetailedInfo() << " ]" << std::endl;
}


string AVISException::FullString()
{
	string	fullString(ClassAsString());
	fullString	+= "(";
	fullString	+= TypeAsString();
	fullString	+= ")[";
	fullString	+= DetailedInfo();
	fullString	+= "]";

	return fullString;
}
