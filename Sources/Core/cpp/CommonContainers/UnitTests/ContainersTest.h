#pragma once 

#ifdef _SS_UNITTESTS

#include "../IndexationResult.h"
#include "../SearchResult.h"
#include "../QueryResultEx.h"
#include "../../ASSInterface\IBlackBoxItems.h"
#include "..\..\ASSInterface\IBlackBox.h"
#include "../../ASSInterface/IBlackBoxQuery.h"
#include <string>
#include <vector>
#include <fstream>

namespace SS
{
	namespace UnitTests
	{
		namespace CommonContainers
		{
			using namespace SS::Interface::Core::CommonContainers;
			using namespace SS::Core::CommonContainers;

			using SS::Interface::Core::BlackBox::IText;
			using SS::Interface::Core::BlackBox::IParagraph;
			using SS::Interface::Core::BlackBox::ISentence;
			using SS::Interface::Core::BlackBox::IUnit;
			using SS::Interface::Core::BlackBox::IIndex;
			using SS::Interface::Core::BlackBox::IDictionaryIndex;
			using SS::Interface::Core::BlackBox::IFeature;

			using SS::Interface::Core::BlackBox::Query::IQuery;
			using SS::Interface::Core::BlackBox::Query::ITransformation;
			using SS::Interface::Core::BlackBox::Query::IGroup;
			using SS::Interface::Core::BlackBox::Query::IQueryUnit;
			using SS::Interface::Core::BlackBox::Query::IQueryUnitIndexCollection;
			using SS::Interface::Core::BlackBox::Query::IQueryUnitIndex;

			using SS::Interface::Core::BlackBox::IBlackBoxTextStorage;
			using SS::Interface::Core::BlackBox::IBlackBoxQueryStorage;
			using SS::Interface::Core::ResourceManagers::ILoadManager;

			class CContainersTest 
			{
			private:
				std::ofstream								m_fout;
				std::wofstream								m_wfout;
				std::string									m_strText;
				CIndexationResult*							m_pIndexationResult;
				CSearchResult*								m_pSearchResult;
				CQueryResultEx*								m_pQueryResultEx;
				IText*										m_pTextIn;
				IText*										m_pTextOut;
				IQuery*										m_pQueryIn;
				IQuery*										m_pQueryOut;
				IBlackBoxTextStorage*						m_pBlackBoxTextStorage;
				IBlackBoxQueryStorage*						m_pBlackBoxQueryStorage;
				ILoadManager*								m_pLoadManager;
				//собственные методы
				void BuildText();
				void BuildQueryIn();
				void BuildQueryOut();
				void SetString();
				void FillUnits(std::vector<IUnit*> & vUnits, IText* pText);
				void FillIndex(IUnit* pUnit, std::vector<unsigned int> & vIndex);
				void FillQueryIndex(IQuery* pQuery, std::vector<unsigned int> & vIndex, std::vector<std::wstring> & vWords);
				bool CompareIText();
				bool CompareIQuery();
				void InitConnection();
				void OnConsole(IQueryIndex* pQueryIndex);
				//¬озвращает true если данные в юнитах одинаковые
				bool CompareUnitsData(IUnit* pFirstUnit, IUnit* pSecondUnit);
							
			public:
				//конструкторы
				CContainersTest(ILoadManager* pLoadManager);
				~CContainersTest();
				//другие методы
				void TestIndexationResult();					
				void TestSearchResult();
				void TestQueryResult();
				void TestServiceIndex();
				void TestTextFeature();
				void TestTextBlockXML();
				void TestQueryIndexAttributes();
				void TestSearchResultCommon();
				void TestBlockCut();
				//“ест работы контейнеров при поиске
				void TotalTestSearch();
				//“ест работы контейнеров при индексации
				void TotalTestIndexation();
				//“ест проверки создани€ текст блоков
				void TestAddTextBlock();
			};
	
		}
	}
}

#endif //_SS_UNITTESTS