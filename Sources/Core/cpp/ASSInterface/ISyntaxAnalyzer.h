#pragma once 

#include "IBlackBoxItems.h"
#include "IMainAnalyse.h"
#include "./ICommon.h"

// {92917032-6015-4a1a-A51C-BC01BDD5FCD9}
static const GUID CLSID_SyntaxAnalyzer = 
{ 0x92917032, 0x6015, 0x4a1a, { 0xa5, 0x1c, 0xbc, 0x1, 0xbd, 0xd5, 0xfc, 0xd9 } };

// {7A13F240-988E-4e03-8954-EAF304DA80DA}
static const GUID CLSID_SyntaxAnalyzer_RusNQ = 
{ 0x7a13f240, 0x988e, 0x4e03, { 0x89, 0x54, 0xea, 0xf3, 0x4, 0xda, 0x80, 0xda } };

// {D435BC32-55B6-453b-A8F6-A680AF7DA936}
static const GUID IID_SyntaxAnalyzer = 
{ 0xd435bc32, 0x55b6, 0x453b, { 0xa8, 0xf6, 0xa6, 0x80, 0xaf, 0x7d, 0xa9, 0x36 } };

namespace SS
{
	namespace Interface
	{
		namespace Core
		{
			///Интерфейс для синтаксического анализа
			class ISyntaxAnalyzer : virtual public SS::Interface::IBase,
									virtual public SS::Interface::Core::Common::CAnalyseParams
			{ 
			public:
				///Производит синтаксический анализ текста
				virtual void AnalyzeNext(SS::Interface::Core::BlackBox::IText* text) = 0;

				///Производит синтаксический анализ предложения
				virtual void AnalyzeNext(SS::Interface::Core::BlackBox::ISentence* sentence) = 0;
			};
		}
	}
}