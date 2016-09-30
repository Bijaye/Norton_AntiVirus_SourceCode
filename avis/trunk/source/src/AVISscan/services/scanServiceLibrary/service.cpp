// service.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <string>

#include "ScanService.h"

int main(int argc, char* argv[])
{
	std::string	type;

	if (NULL == argv || NULL == argv[1])
		type = "latest";
	else
		type = argv[1];

	if (ScanService::Initialize(type))
		ScanService::Run();

	ScanService::DeInitialize();

	return 0;
}

