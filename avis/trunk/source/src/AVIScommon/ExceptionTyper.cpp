
#include <stdafx.h>

#include <string>
#include <comdef.h>

#include <AVISException.h>

#include "ExceptionTyper.h"

using namespace std;

AVISCOMMON_API bool ExceptionTyper(string& type)
{
	bool	rc = true;

	try
	{
		throw;
	}
	catch (AVISException& e)
	{
		type	= "AVISException [";
		type	+= e.FullString();
		type	+= "]";
	}
	catch (exception& e)
	{
		type	= "std::exception [";
		type	+= e.what();
		type	+= "]";
	}
	catch (string& e)
	{
		type	= "std::string [";
		type	+= e;
		type	+= "]";
	}
	catch (char* e)
	{
		type	= "char* [";
		type	+= e;
		type	+= "]";
	}
	catch (int e)
	{
		char	buffer[24];
		sprintf(buffer, "%d]", e);
		type	= "int [";
		type	+= buffer;
	}
	catch (float e)
	{
		char	buffer[128];
		sprintf(buffer, "%f]", e);
		type	= "float [";
		type	+= buffer;
	}
	catch (double e)
	{
		char	buffer[128];
		sprintf(buffer, "%fl]", e);
		type	= "double [";
		type	+= buffer;
	}
	catch (_com_error e)
	{
		type	= "_com_error [";
		type	+= e.ErrorMessage();
		type	+= "]";
	}
	catch (...)
	{
		type	= "Unknown type";
		rc = false;
	}

	return rc;
}
