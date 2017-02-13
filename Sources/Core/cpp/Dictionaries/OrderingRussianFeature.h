#pragma once


namespace SS
{
namespace Dictionary
{
namespace Virtual
{

struct SOrderingFeature
{
	TIDForm idForm;
	SS::Core::Features::Types::CaseType ct;
	SS::Core::Features::Types::GenderType gt;
	SS::Core::Features::Types::NumberType nt;
	SS::Core::Features::Types::AnimInAnimType at;
};

struct SConvertedFeature
{
	int iMorphoInfo;
	int iSpecialMorpho;
};

typedef const std::list<const SConvertedFeature> TConvFeatureList;


class COrderingRussianFeature
{
public:
	COrderingRussianFeature();
	~COrderingRussianFeature();

public:
	void Init(IAMConverterMorpho* pAMConverter);
	TConvFeatureList* GetConvertedFeature(TIDForm idForm, UINT uiValue);

	LPCWSTR GetSuffix(TIDForm idForm);
	TIDForm GetIDForm(LPCWSTR szSuffix);
	int GetSuffixCount();

private:
	typedef std::vector<TConvFeatureList> TFeatureVect;
	void InitVect(TFeatureVect* pVect, SOrderingFeature* rgFeatures, int iCount, IAMConverterMorpho* pAMConverter);

private:
	CMorphoFeature m_oMorphoFeature;

	TFeatureVect m_vFeatures_490;
	TFeatureVect m_vFeatures_491;
	TFeatureVect m_vFeatures_511;
	TFeatureVect m_vFeatures_510;
	TFeatureVect m_vFeatures_512;
	TFeatureVect m_vFeatures_513;
};

}
}
}
