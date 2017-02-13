#pragma once

#include "..\ASSInterface\IAMConverter.h"
using namespace SS::Interface::Core::AMConverter;

#include "..\FeatureLibrary\MorphoFeature.h"
using namespace SS::Core::Features;

#include "..\ASSInterface\IBlackBoxQuery.h"
using namespace SS::Interface::Core::BlackBox::Query;

#include "..\ASSInterface\IBlackBoxDictionaryIndex.h"
using namespace SS::Interface::Core::BlackBox;

#include "..\ASSInterface\TDictionaryManager.h"
using namespace SS::Dictionary::Types;

#include "..\ASCInterface\services.h"
using namespace SS::Core::CommonServices;

#include "..\FeatureLibrary\define.h"
using namespace SS::Core::Features::Types;


namespace SS
{
namespace TransformGenerator
{

class CTypeChecker : public CBaseCoreClass
{
public:
	CTypeChecker();
	~CTypeChecker();

public:
	virtual void SetLoadManager(SS::Interface::Core::ResourceManagers::ILoadManager* pLoadManager);
	bool IsSuperlativeForm(IIndex* pIndex);

private:
	CMorphoFeature* m_pMorphoFeature;
	IAMConverterMorpho* m_pMorphoConverter;
};

}
}
