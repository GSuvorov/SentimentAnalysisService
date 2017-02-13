#pragma once

#include "..\..\ASSInterface\IBlackBox.h"
using namespace SS::Interface::Core::BlackBox;

#include "..\..\ASCInterface\defines.h"
using namespace SS::Interface::Core::ResourceManagers;

#include "..\..\ASSInterface\IAMConverter.h"
using namespace SS::Interface::Core::AMConverter;

#include "..\..\FeatureLibrary\SemanticFeature.h"
using namespace SS::Core::Features;

#include "..\..\FeatureLibrary\define.h"
using namespace SS::Core::Features::Types;

//#include "..\..\ASSInterface\TDictionaryTypes.h"
//using namespace SS::Dictionary::DATA_TYPE;

#include "..\..\ASSInterface\TDictionaryNames.h"
using namespace SS::Dictionary::DATA_TYPE::NAMES;

#include "..\..\ASCInterface\TCoommonTypes.h"
using namespace SS::Core::Types::IndexationSearch;

#include <string>
using namespace std;

#include "TypeFeatureWrapper.h"
#include "..\..\ASSInterface\TDictionaryManager.h"

using namespace SS::Dictionary::Types;


namespace SS
{
namespace Core
{
namespace BlackBox
{
namespace XML
{

class CFeatureHelper
{
public:
	CFeatureHelper();
	~CFeatureHelper();

public:
	void Init(ILoadManager* pLoadManager);

	string GetDescription(TDictionaryIndex oIndex);
	string GetDescription(IFeature* pFeature, IIndex* pParentIndex);

	bool IsTitleUnit(IUnit* pUnit);
	void SetTitleUnit(IUnit* pUnit);

private:
	CSemanticFeature* m_pSemanticFeature;
	CMorphoFeature* m_pMorphoFeature;
	CSyntaxFeatureApriori* m_pSyntaxFeature;

	IAMConverterSemantic* m_pSemanticConverter;
	IAMConverterMorpho* m_pMorphoConverter;
	IAMConverterSyntax* m_pSyntaxConverter;

	CTypeFeatureWrapper<SyntaxCategories, SyntaxCategoriesType> m_oSyntaxCategories;
};

}
}
}
}
