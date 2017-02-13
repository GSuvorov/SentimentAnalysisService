#include "StdAfx.h"
#include ".\coordinate_counter.h"
#include ".\console.h"
#include ".\const.h"

namespace SS
{
namespace Core
{
namespace NdxSE
{
namespace NdxProcessor
{
namespace Indexing
{

using namespace SS::Interface::Core::NdxSE::NdxDataStorage;

//--------------------------------------------------------------------//

CCoordinateCounter::CCoordinateCounter(void)
{
	Reset();
}

void CCoordinateCounter::Reset(void)
{
	memset(m_arCoordinates, EMPTY_VALUE, sizeof(unsigned int)*m_uiCoordinatesTypesNumber);
}

unsigned int CCoordinateCounter::GetCoordinate(SNdxLevelInfo::EIndexCoordinateType CoordinateType)
{
	if(CoordinateType<m_uiCoordinatesTypesNumber)
	{
		return m_arCoordinates[CoordinateType];
	}else{
		TO_CONSOLE(L"Unknown coordinate type...");
		return 0;	
	}
}

void CCoordinateCounter::Increment(SNdxLevelInfo::EIndexCoordinateType CoordinateType, unsigned int uiStep)
{
	if(CoordinateType>=m_uiCoordinatesTypesNumber)
	{
		TO_CONSOLE(L"Unknown coordinate type...");
		return;	
	}
	
	//увеличиваем координату
	m_arCoordinates[CoordinateType]+=uiStep;
	//сбрасываем подчиненные
	ResetSubCoordinates(CoordinateType);
}

int CCoordinateCounter::Update(SNdxLevelInfo::EIndexCoordinateType CoordinateType, unsigned int uiValue)
{
	if(CoordinateType>=m_uiCoordinatesTypesNumber)
	{
		TO_CONSOLE(L"Unknown coordinate type...");
		return 0;	
	}
	
	if(m_arCoordinates[CoordinateType]==uiValue)
	{
		return 0;
	}else{
		int iDif=uiValue-m_arCoordinates[CoordinateType];
		m_arCoordinates[CoordinateType]=uiValue;
		//сбрасываем подчиненные
		ResetSubCoordinates(CoordinateType);
		return iDif;
	}
	
}

void CCoordinateCounter::ResetCoordinate(SNdxLevelInfo::EIndexCoordinateType CoordinateType)
{
	if(CoordinateType<m_uiCoordinatesTypesNumber)
	{
		m_arCoordinates[CoordinateType]=EMPTY_VALUE;
		ResetSubCoordinates(CoordinateType);
	}else{
		TO_CONSOLE(L"Unknown coordinate type...");
	}
}

void CCoordinateCounter::ResetSubCoordinates(SNdxLevelInfo::EIndexCoordinateType CoordinateType)
{
	switch(CoordinateType)
	{
	case SNdxLevelInfo::ictSentenceAbsNumber:
		ResetCoordinate(SNdxLevelInfo::ictWordNumber);
		break;
	case SNdxLevelInfo::ictTextAbsNumber:
		ResetCoordinate(SNdxLevelInfo::ictParagraphNumber);
		ResetCoordinate(SNdxLevelInfo::ictSentenceInTextNumber);
		ResetCoordinate(SNdxLevelInfo::ictWordInTextNumber);
		break;
	case SNdxLevelInfo::ictParagraphNumber:
		ResetCoordinate(SNdxLevelInfo::ictSentenceNumber);
		break;
	case SNdxLevelInfo::ictSentenceNumber:
		ResetCoordinate(SNdxLevelInfo::ictWordNumber);
		break;
	case SNdxLevelInfo::ictSentenceInTextNumber:
		ResetCoordinate(SNdxLevelInfo::ictWordNumber);
		break;
	case SNdxLevelInfo::ictWordNumber:
		break;
	case SNdxLevelInfo::ictWordInTextNumber:
		break;
	default:
		TO_CONSOLE(L"Unknown coordinate type...");
		break;
	}
}

//--------------------------------------------------------------------//

}
}
}
}
}