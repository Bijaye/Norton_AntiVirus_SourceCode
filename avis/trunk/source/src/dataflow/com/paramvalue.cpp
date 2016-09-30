// ParamValue.cpp: implementation of the CParamValue class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ParamValue.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CParamValue::CParamValue()
{
    m_DefaultParam.SetSize(0,10);
    m_DefaultValue.SetSize(0,10);
    m_Param.SetSize(0,10);
    m_Value.SetSize(0,10);
}

CParamValue::~CParamValue()
{
    m_DefaultParam.RemoveAll();
    m_DefaultValue.RemoveAll();
    m_Param.RemoveAll();
    m_Value.RemoveAll();
}

void CParamValue::ResetValues()
{
    m_DefaultParam.RemoveAll();
    m_DefaultValue.RemoveAll();
    m_Param.RemoveAll();
    m_Value.RemoveAll();
}

int CParamValue::ReadParamValue(FILE * fp, BOOL addDefault, BOOL updateDefault, CString delim)
{
    long oldFilePos;

    m_Param.RemoveAll();
    m_Value.RemoveAll();
    m_Param.SetSize(0,10);
    m_Value.SetSize(0,10);

    if (fp)
    {
        CString param;
        CString value;

        while (TRUE)
        {
            param.Empty();
            value.Empty();

            oldFilePos = ftell (fp);
            if (!GetNextParamValue (fp, param, value, delim))
                break;

            param.TrimLeft();
            param.TrimRight();
            value.TrimLeft();
            value.TrimRight();

            if (param.IsEmpty() || value.IsEmpty())
                continue;

            if (!param[0] || !value[0])
                continue;

            if (param[0] == ';')
                continue;

            //param.MakeUpper();

            int paramSize = m_Param.GetSize();
            int i;

            for (i = 0; i < paramSize; i++)
            {
                CString paramStr;

                paramStr = m_Param[i];

                if (!paramStr.CompareNoCase(param))
                    break;
            }

            if (i >= paramSize)
            {
                m_Param.Add(param);
                m_Value.Add(value);
            }
            else
            {
                fseek (fp, oldFilePos, SEEK_SET);
                break;
            }
        }

        if (m_Param.GetSize() && addDefault == TRUE)
        {
            int defSize = m_DefaultParam.GetSize();
            int i, j;

            for (i = 0; i < defSize; i++)
            {
                CString defParamStr;
                int paramSize = m_Param.GetSize();

                defParamStr = m_DefaultParam[i];

                for (j = 0; j < paramSize; j++)
                {
                    CString paramStr;

                    paramStr = m_Param[j];

                    if (!(defParamStr.CompareNoCase(paramStr)))
                        break;
                }
                if (j >= paramSize && !m_DefaultValue[i].IsEmpty())
                {
                    m_Param.Add(defParamStr);
                    m_Value.Add(m_DefaultValue[i]);
                }
            }
        }

        if (updateDefault == TRUE)
        {
            int paramSize = m_Param.GetSize();
            int i, j;

            for (i = 0; i < paramSize; i++)
            {
                CString paramStr;
                int defSize = m_DefaultParam.GetSize();

                paramStr = m_Param[i];

                for (j = 0; j < defSize; j++)
                {
                    CString defParamStr;

                    defParamStr = m_DefaultParam[j];

                    if (!(defParamStr.CompareNoCase(paramStr)))
                        break;
                }
                if (j >= defSize)
                {
                    m_DefaultParam.Add(paramStr);
                    m_DefaultValue.Add(m_Value[i]);
                }
                else
                {
                    m_DefaultValue.SetAt(j, m_Value[i]);
                }
            }
        }
    }
    return (m_Param.GetSize());
}

int CParamValue::GetNextParamValue(FILE * fp, CString & param, CString & value, CString & delim)
{
    char fLine[1024];
    char * ptr;

    if (fgets (fLine, sizeof (fLine), fp))
    {
        ptr = strtok (fLine, delim);
        if (ptr)
        {
            param = ptr;
            ptr = strtok (NULL, "\r\n");
            if (ptr)
                value = ptr;
        }
        return 1;
    }

    return 0;
}

BOOL CParamValue::GetValue(CString & key, CString & value)
{
    int paramSize = m_Param.GetSize();

    for (int i = 0; i < paramSize; i++)
    {
        if (!m_Param[i].CompareNoCase(key))
            break;
    }

    if (i >= paramSize || m_Value[i].IsEmpty())
    {
        value.Empty();
        return (FALSE);
    }

    value = m_Value[i];
    return (TRUE);

}

void CParamValue::AddParamValue(CString & key, CString & value)
{
    m_Param.Add (key);
    m_Value.Add (value);
}

BOOL CParamValue::WriteParamValue(FILE * fp, CString delim)
{
    int paramSize = m_Param.GetSize();
    int i;
    int rc;

    for (i = 0; i < paramSize; i++)
    {
        rc = fprintf (fp, "%s %c %s\n", (LPCTSTR) m_Param[i], delim[0], (LPCTSTR) m_Value[i]);
        if (rc < 0)
            return (FALSE);
    }
    fprintf (fp, "\n");

    return (TRUE);
}
