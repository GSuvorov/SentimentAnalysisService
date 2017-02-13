#pragma once 

#include <string>
#include "..\ascinterface\iinterface.h"
#include "..\assinterface\iblackbox.h"

//#include "IMainAnalyse.h"
//#include "ICommon.h"

#include "IObjectDescr.h"
#include "TextEntity.h"

// {92917032-6015-4a1a-A51C-BC01BDD5FCD9}
static const GUID CLSID_DigestAnalyzer = 
{ 0x93f35223, 0xee02, 0x4d66, { 0x8f, 0x82, 0x47, 0x9d, 0x3c, 0x90, 0xeb, 0x2a } };

// {7A13F240-988E-4e03-8954-EAF304DA80DA}
static const GUID CLSID_DigestAnalyzer_RusNQ = 
{ 0x254c2a74, 0x845b, 0x4fb0, { 0x95, 0x8e, 0x9a, 0xb8, 0xb4, 0x7d, 0xd3, 0xbe } };

// {D435BC32-55B6-453b-A8F6-A680AF7DA936}
static const GUID IID_DigestAnalyzer = 
{ 0x44ff6fa4, 0x9405, 0x4189, { 0xbf, 0xa9, 0x8f, 0x5b, 0xf4, 0xc3, 0x85, 0x46 } };

namespace SS
{
	namespace Interface
	{
		namespace Core
		{
			typedef vector<SS::Core::Types::CTextEntity> EntitiesCollection;
			///Интерфейс для синтаксического анализа
			class IDigestAnalyzer : virtual public SS::Interface::IBase									
			{
			protected:
				unsigned int m_MaxEntityLength;
			public:
				///Производит первичное выделение дайджеста 
				// на вход получает структурированный текст и массив смещений по тексту, которые
				// определяют предварительный дайджест. Предложения дайджеста помечаются
				virtual void AnalyzeNextBeforeSynt(SS::Interface::Core::BlackBox::IText* text, list<int>& offsets) = 0;
				///Производит вторичное выделение дайджеста
				// после синтразбора предложений первичного дайджеста
				// разрешается анафория и кореферентность и юниты текста, относящиеся к объекту
				// дайджеста помечаются
				virtual void AnalyzeNextAfterSynt(SS::Interface::Core::BlackBox::IText* text,
					SS::Interface::Core::BlackBox::IObjectDescr* objectDescr, list<int>& offs) = 0;

				virtual wstring GetDigestXML() = 0;

                /// загрузка служебных словарей из коллекции
				virtual void LoadServiceDictionaries(vector<vector<wstring>>& dictionaries) = 0;
				/// Задание максимальной длины сущности
				void SetMaxEntityLength(const unsigned int length)
				{
					m_MaxEntityLength = length;
				}

                virtual void ReinitToneDicts(wstring& type_name) = 0;
			    virtual void AddWordToDict(wstring& type_name, int dict_code, string& word) = 0;
			    virtual void GenerateDictLing(wstring& type_name) = 0;

				/// синтаксический анализ
				virtual void SyntaxAnalyzation(const wstring &srcText, SS::Interface::Core::BlackBox::IText* text,
					const wstring& baseDate, bool languageAnalyze = false) = 0;

				virtual wstring GetEntitiesXML(const wstring& srcText, SS::Interface::Core::BlackBox::IText* text) = 0;

				virtual EntitiesCollection SelectEntities(
					const wstring& srcText,
					SS::Interface::Core::BlackBox::IText* pText,
					const wstring& baseDate) = 0;

				virtual EntitiesCollection SelectSimpleEntities(
					const wstring& srcText,
					SS::Interface::Core::BlackBox::IText* pText,
					bool isSelectOnlyCeterus) = 0;

				virtual wstring GetFactXML(
					const wstring& srcText, 
					SS::Interface::Core::BlackBox::IText* text,
					list<int>& offsets,
					const wstring& baseDate, 
					bool isTonality = false,
					bool bFullText = false,
					bool bFullAnalyze = true) = 0;

				virtual wstring GetEntityHash(const wstring& srcText, SS::Interface::Core::BlackBox::IText* text) = 0;

				virtual wstring GetSentDigestXML(SS::Interface::Core::BlackBox::IText* pText,list<int>& offsets, list<int>& themes, wstring& dictName) = 0;
			};
		}
	}
}