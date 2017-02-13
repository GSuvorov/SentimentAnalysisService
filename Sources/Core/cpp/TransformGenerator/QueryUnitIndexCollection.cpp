#include "StdAfx.h"
#include ".\QueryUnitIndexCollection.h"

namespace SS
{
namespace TransformGenerator
{

/////////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction

CQueryUnitIndexCollection::CQueryUnitIndexCollection(IBlackBoxQueryStorage* pBlackBoxQueryStorage, 
													 IQueryUnitIndexCollection* pColl) :
		m_pBlackBoxQueryStorage(pBlackBoxQueryStorage),
		m_pColl(pColl)
{

}

CQueryUnitIndexCollection::~CQueryUnitIndexCollection(void)
{

}

/////////////////////////////////////////////////////////////////////////////////////
// CQueryUnitIndexCollection interface

bool CQueryUnitIndexCollection::Create(EIndexType type, float flWeight)
{
	SS_TRY
	{
		m_pColl = m_pBlackBoxQueryStorage->CreateQueryUnitIndexCollection();
		if(!m_pColl)
			return false;

		m_pColl->SetIndexCollectionType(type);
		m_pColl->SetWeight(flWeight);

		return true;
	}
	SS_CATCH(L"")

	Release();
	return false;
}

IQueryUnitIndexCollection* CQueryUnitIndexCollection::Clone()
{
	IQueryUnitIndexCollection* pColl = NULL;
	SS_TRY
	{
		pColl = m_pBlackBoxQueryStorage->CreateQueryUnitIndexCollection();
		if(!pColl)
			return NULL;

		pColl->SetIndexCollectionType(m_pColl->GetIndexCollectionType());
		for(IQueryUnitIndex* pIndex = m_pColl->GetFirstQueryUnitIndex(); pIndex; pIndex = pIndex->GetNextQueryUnitIndex())
		{
			IQueryUnitIndex* pNewIndex = m_pBlackBoxQueryStorage->CreateQueryUnitIndex();
			pNewIndex->SetIndex(pIndex->GetIndex());
			pColl->AddQueryUnitIndex(pNewIndex);

			short shIDForm;
			for(bool b = pIndex->GetFirstIDForm(&shIDForm); b; b = pIndex->GetNextIDForm(&shIDForm))
				pNewIndex->AddIDForm(shIDForm);
		}

		return pColl;
	}
	SS_CATCH(L"")

	if(pColl)
		pColl->Release();

	return NULL;
}

CQueryUnitIndexCollection::operator IQueryUnitIndexCollection*()
{
	return m_pColl;
}

void CQueryUnitIndexCollection::Release()
{
	if(m_pColl)
	{
		m_pColl->Release();
		m_pColl = NULL;
	}
}

bool CQueryUnitIndexCollection::IsEmpty()
{
	return !m_pColl || m_pColl->IsEmpty();
}

}
}
