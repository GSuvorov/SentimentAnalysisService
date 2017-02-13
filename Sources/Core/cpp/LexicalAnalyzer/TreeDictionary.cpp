#include "stdafx.h"
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <share.h>
#include "TreeDictionary.h"


namespace SS
{
	namespace LexicalAnalyzer
	{
		namespace TreeDictionary
		{

			CTreeDictionary::CTreeDictionary()
			{
				m_pUnitOperator = NULL;
			}

			CTreeDictionary::CTreeDictionary(UnitPropertiesOperator* pUnitOperator)
			{
				m_pUnitOperator = pUnitOperator;
			}

            void CTreeDictionary::AddWord(wstring word, string info)
			{	
				wchar_t wch = word[word.size() - 1];
				unsigned int j = 0;
				for(j = 0; j < m_Slots.size(); ++j)
				{
					if (m_Slots[j] == wch)
						break;	
				}
				if (j == m_Slots.size())
				{
					m_Slots.push_back(wch);
					CTreeDictionary* dict = new CTreeDictionary;
					m_ChildForSlots.push_back(dict);
				}

				if (word.size()==1)
                // закончили
				{
					m_Info = info;
				}
                else
				{
					wstring subword = word.substr(0, word.size()-1);
					m_ChildForSlots[j]->AddWord(subword, info);
				}
			}

			void CTreeDictionary::FindWord(const wstring &word, vector<string>& find_types, bool isFullWordFind)
			{	
				//wstring wordHi = _wcsupr((wchar_t*)word.c_str());	
                if (word.empty())
                    return;
				wchar_t wch = word[word.size()-1];
				unsigned int j = 0;
				for(j = 0; j < m_Slots.size(); ++j)
				{
					if (m_Slots[j] == wch)
						break;	
				}
				if (j == m_Slots.size())
					return;

				if (m_Info.size()>0 && (!isFullWordFind || word.size()==1))
					find_types.push_back(m_Info);	
                else if (word.size() > 1)
				{
					wstring subword = word.substr(0, word.size() - 1);
					m_ChildForSlots[j]->FindWord(subword, find_types, isFullWordFind);
				}
			}

            void CTreeDictionary::AddNewLine(const wstring &line)
            {
                int i = 0;
                int length = line.length();
                wstring word = L"";
                for(i = 0; i < length; ++i)
				{
					if ((line[i] == ' ') || (line[i] == '\t') || (line[i] == '\r') || (line[i] == '\n'))
						break;
					if ((int)line[i] == 0xFEFF)
                        continue;
					word += line[i];
				}

                string info = "";
				for(; i < length; ++i)
					if ((line[i] != ' ') && (line[i] != '\t') && (line[i] != '\r') && (line[i] != '\n'))
						info += line[i];

				if (info.size() == 0)
                    info = "undef";
				if (word.size() == 0)
					return;

				AddWord(word, info);	

				if (m_pUnitOperator)
				{
					// тогда нужно сгенерить все словоформы и тоже добавить
					set<wstring> lstWordForms;
					m_pUnitOperator->GetAllWordForms(word, lstWordForms);
					for(set<wstring>::iterator itStr = lstWordForms.begin();
                        itStr != lstWordForms.end();
                        ++itStr)
					{
						AddWord(*itStr, info);
					}
				}
            }

            void CTreeDictionary::LoadDict(const vector<wstring> &dictionary)
            {
                for(vector<wstring>::const_iterator it = dictionary.begin(); it != dictionary.end(); ++it)
                    AddNewLine(*it);
            }

			void CTreeDictionary::LoadDict(const wchar_t* str)
			{
				wchar_t bufw[300];	
				UINT32 iAccessMode, iShareMode;
				iAccessMode = GENERIC_READ;
				iShareMode = FILE_SHARE_READ;
				FILE *file = 0;
				_wfopen_s(&file, str, L"rb");
                if(!file)
                    return;

				wchar_t* res = fgetws(bufw, 300, file);
				while(res)
				{						
					int len = wcslen(bufw);
					/*if (len < 2)
                    {
                        res = fgetws(bufw, 300, file);
                        continue;
                    }*/
					if (bufw[0]==L'/' && bufw[1]==L'/')
					{
						res = fgetws(bufw,300, file);
                        continue;
					}

                    AddNewLine(bufw);

					res = fgetws(bufw, 300, file);
				}
			}
		}
	}
}