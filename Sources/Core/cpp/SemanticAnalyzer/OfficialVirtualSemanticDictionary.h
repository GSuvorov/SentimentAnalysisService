#pragma once

namespace SS
{
namespace Semantic
{
namespace VirtualDictionaries
{
	
	class COfficialVirtualSyntaxDictionary 
	{
	public:
		/// конструктор
		COfficialVirtualSyntaxDictionary(void);
		/// деструктор
		virtual ~COfficialVirtualSyntaxDictionary(void);
		bool GetByFormPrepositionSyntaxCategories(std::wstring* pwsWord, list<SS::Core::Features::Types::SyntaxCategories> * plSyntaxCategories);
		bool IsPrepositionSyntaxExist(std::wstring* pwsWord );

	private:
		//structures
		struct SSyntaxCategories
		{
			std::vector<SS::Core::Features::Types::SyntaxCategories> m_SyntaxCategories;
			UINT m_uiCount;
			SSyntaxCategories(
				SS::Core::Features::Types::SyntaxCategories  oType1 = SS::Core::Features::Values::SyntaxCategories.Undefined
				,SS::Core::Features::Types::SyntaxCategories oType2 = SS::Core::Features::Values::SyntaxCategories.Undefined
				,SS::Core::Features::Types::SyntaxCategories oType3 = SS::Core::Features::Values::SyntaxCategories.Undefined
				,SS::Core::Features::Types::SyntaxCategories oType4 = SS::Core::Features::Values::SyntaxCategories.Undefined
				,SS::Core::Features::Types::SyntaxCategories oType5 = SS::Core::Features::Values::SyntaxCategories.Undefined)
			{
				m_uiCount=0;
				m_SyntaxCategories.reserve(5);
				if ( !oType1.IsUndefined() ){  m_SyntaxCategories.push_back(oType1); m_uiCount++; }
				else return;
				if ( !oType2.IsUndefined() ){  m_SyntaxCategories.push_back(oType2); m_uiCount++; }
				else return;
				if ( !oType3.IsUndefined() ){  m_SyntaxCategories.push_back(oType3); m_uiCount++; }
				else return;
				if ( !oType4.IsUndefined() ){  m_SyntaxCategories.push_back(oType4); m_uiCount++; }
				else return;
				if ( !oType5.IsUndefined() ){  m_SyntaxCategories.push_back(oType5); m_uiCount++; }
			};
		};
		//typedefs 
		typedef std::map<std::wstring, COfficialVirtualSyntaxDictionary::SSyntaxCategories> TSyntaxCategoriesTable;
		//attributes
		TSyntaxCategoriesTable m_PrepositionSyntaxTable;
		TSyntaxCategoriesTable m_ConjactionSyntaxTable;
	};

}
}
}