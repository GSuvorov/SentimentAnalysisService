#pragma once

#include "..\..\ASSInterface\IAMConverter.h"
using namespace SS::Interface::Core::AMConverter;

#include "..\..\FeatureLibrary\MorphoFeature.h"
using namespace SS::Core::Features;
using namespace SS::Interface::Core::ResourceManagers;
using namespace SS::Core::CommonServices;

#include "..\..\ASSInterface\ICoSeDi.h"
using namespace SS::Interface::Core::CoSeDi;

#include "..\..\ASSInterface\TCoSeDiDataTypes.h"
using namespace SS::CoSeDi::DataTypes;

#include "NumericParser.h"


namespace SS  
{
namespace Dictionary  
{
namespace NumberCombinatory
{

/////////////////////////////////////////////////////////////////////////////////////
// CNumberCombinatory

class CNumberCombinatory : 
		public INumberCombinatory,
		public CNumericParser
{
public:
	CNumberCombinatory();
	~CNumberCombinatory();

// CBaseCoreClass
public:
	virtual void SetLoadManager(ILoadManager* pLoadManager);

// IBase
public:
	virtual HRESULT QueryInterface(REFIID refIID, void** pBase);
	virtual ULONG Release();

// INumberCombinatory
public:
   virtual void GetCombinationsInfo(TIDSourceVector* pvIDSources, std::list<SCombinationNewInfo>* plCombinationInfo);

// CNumericParser
protected:
	virtual int GetCurrentLexem(int iPosition, YYSTYPE* pValue);
	virtual void AddToOutput(void* pParam, int iFirst, int iLast, YYSTYPE Result);

private:
	void FillIDSourceTable();
	UINT GetSourceID(LPCTSTR sz, CMorphoFeature* pMorphoFeature);
	bool IsOrdinal(int iPos);

private:
	struct SLexNumeric
	{
		UINT uiValue;
		LPCTSTR sz;
		int iLexemType;
		bool bOrdinal;
	};

	struct SNumeric
	{
		SNumeric(UINT ui, int i, bool b) : uiValue(ui), iLexemType(i), bOrdinal(b) { }
		UINT uiValue;
		int iLexemType;
		bool bOrdinal;
	};

private:
	static const SLexNumeric s_rNumeric[];

	typedef std::map<UINT, SNumeric> TIDSourceTable;
	TIDSourceTable m_mIDSources;

	IMorphologyAnalyser* m_pMorphologyAnalyser;
	IAMConverterMorpho* m_pConv;

	/// input vector
	TIDSourceVector* m_pvIDSources;
};

}
}
}
