#pragma once

#include "..\ASSInterface\IAMConverter.h"
using namespace SS::Interface::Core::AMConverter;

#include "..\FeatureLibrary\SemanticFeature.h"
using namespace SS::Core::Features;

#include "..\FeatureLibrary\define.h"
using namespace SS::Core::Features::Types;

#include "..\ASSInterface\IBlackBoxUnit.h"
#include "..\ASSInterface\IBlackBox.h"
using namespace SS::Interface::Core::BlackBox;


namespace SS
{
namespace LexicalAnalyzer
{

enum EChangeFeature
{
	cfSetNounCommonSingle,
	cfSetGenitive,
	cfSetNegative,
	cfSetOrdering,
	cfAddOrdering,
};

class CFeatureChanger
{
public:
	CFeatureChanger(IBlackBoxTextStorage* pStorage, CMorphoFeature* pMorphoFeature, 
		IAMConverterMorpho* pMorphoConverter);

public:
	void ChangeUnitFeature(IUnit* pUnit, EChangeFeature ecf);

private:
	void ConvertFeature(IFeature* pFeature, DictionariesNumber edn);

	enum EAction
	{
		actNothing,
		actRemove,
		actAdd,
	};

	EAction ChangeFeature(IFeature* pFeature, DictionariesNumber edn, EChangeFeature ecf);
	void AddFeature(IIndex* pIndex, DictionariesNumber edn);

private:
	IBlackBoxTextStorage* m_pStorage;
	CMorphoFeature* m_pMorphoFeature;
	IAMConverterMorpho* m_pMorphoConverter;
};

}
}
