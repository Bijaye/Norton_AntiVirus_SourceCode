////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include ".\testexclulog.h"
#include "ccLibStd.h"
#include <stdio.h>
#include <vector>

TestExcluLog::TestExcluLog(void)
{
}

TestExcluLog::~TestExcluLog(void)
{
}

bool TestExcluLog::ReadFromFile(LPCTSTR strInputFile)
{

	FILE * pFile;
	long lSize;
	CString strTemp = "";


	pFile = fopen(strInputFile, "r");
	
	if (pFile == NULL)
	{
		fprintf( stderr, "can not open input file! \n");
		return false;
	}else{
		
		while (1){
			char charTemp = (char) fgetc(pFile);
		
			if (charTemp != ' ' && charTemp != '\0' && charTemp != '\n')
			{
				strTemp.AppendChar(charTemp);

			}else if (charTemp == ' ')
			{
			   m_strVector.push_back(strTemp);
			   strTemp = "";
			}
			else if (charTemp == '\n')
			{
				m_strVector.push_back(strTemp);
				strTemp = "";
				m_vVector.push_back(m_strVector);
				m_strVector.clear();
			}
			if (feof(pFile))
			{
				m_strVector.push_back(strTemp);
				strTemp = "";
				m_vVector.push_back(m_strVector);
				m_strVector.clear();
				break;
			}

		}

		for(int i = 0; i< m_vVector.size() ; i++)
		{
			m_strVector = m_vVector.at(i);
			for (int k = 0; k<m_strVector.size() ; k++)
			{
				printf("%s \n", m_strVector.at(k));
			}
		}

		fclose (pFile);
		return true;
	}

}
bool TestExcluLog::Process()
{
	for(int i = 0; i< m_vVector.size(); i++)
	{
		m_strVector = m_vVector.at(i);
		
		if (!m_strVector.at(0).Compare("addfs"))
		{
			//CString strFilePath;
			//time_t tNowTime = time(NULL) ;
			//time_t tTime;
			//IExclusion::ExclusionState iState;

			if (m_strVector.size() != 4)
				fprintf(stderr, "error on input file line %d", i);
			else
			{
				
			}

			printf("here\n");

		}

}
	return true;
}