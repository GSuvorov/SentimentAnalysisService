#include "StdAfx.h"
#include ".\ndx_configurator.h"
#include ".\console.h"

namespace SS
{
namespace Core
{
namespace NdxSE
{
namespace NdxBaseManager
{

using namespace SS::Interface::Core::NdxSE::NdxDataStorage;
using namespace SS::Interface::Core::NdxSE;

//--------------------------------------------------------------------//

CNdxConfigurator::CNdxConfigurator(void)
{
}

CNdxConfigurator::~CNdxConfigurator(void)
{
}

void CNdxConfigurator::Cofigure(INdxStorage* pNdxStorage, ENdxStorageConfig NdxStorageConfig)
{
	switch(NdxStorageConfig){
	case escAbsSentence:
		ConfigLevels_0(pNdxStorage);
		break;
	case escAbsSentence_WC:
		ConfigLevels_1(pNdxStorage);
		break;
	case escText_WC:
		ConfigLevels_2(pNdxStorage);
		//ConfigLevels_4(pNdxStorage);
		//ConfigLevels_5(pNdxStorage);
		break;
	case escText_WC_ForBig:
		ConfigLevels_5(pNdxStorage);
		break;
	case escText_WC_ForSmall:
		ConfigLevels_4(pNdxStorage);
		break;
	case escText_WC_Huge:
		ConfigLevels_6(pNdxStorage);
		break;
	case escText:
		ConfigLevels_3(pNdxStorage);
		break;
	default:
		TO_CONSOLE(L"Unsupported levels configuration!");
		break;
	}
}

void CNdxConfigurator::ConfigLevels_0(INdxStorage* pNdxStorage)
{
//для индексации по абсолютному номеру предложения
//координата абс. номер предложения
//abs 1/32/3/18/4/10
//индексируем только по абс. номеру предложения
SNdxLevelInfo NdxLevelInfo;
	
	if(ISNULL(pNdxStorage)) return;

	NdxLevelInfo.m_IndexCoordinateType=SNdxLevelInfo::ictSentenceAbsNumber;
	NdxLevelInfo.m_eControlType=SNdxLevelInfo::lctUndefined;
	NdxLevelInfo.m_eControlByType=SNdxLevelInfo::lctUndefined;
	
	//----------------------------------------------
	NdxLevelInfo.m_ucLevelNumber=0;
	NdxLevelInfo.m_ucStartBit=31;
	NdxLevelInfo.m_ucBitPerLevel=18;

	pNdxStorage->AddLevelInfo(&NdxLevelInfo);
	//----------------------------------------------

	//----------------------------------------------
	NdxLevelInfo.m_ucLevelNumber++;
	NdxLevelInfo.m_ucStartBit-=NdxLevelInfo.m_ucBitPerLevel;
	NdxLevelInfo.m_ucBitPerLevel=4;

	pNdxStorage->AddLevelInfo(&NdxLevelInfo);
	//----------------------------------------------

	//----------------------------------------------
	NdxLevelInfo.m_ucLevelNumber++;
	NdxLevelInfo.m_ucStartBit-=NdxLevelInfo.m_ucBitPerLevel;
	NdxLevelInfo.m_ucBitPerLevel=10;

	pNdxStorage->AddLevelInfo(&NdxLevelInfo);
	//----------------------------------------------
}

void CNdxConfigurator::ConfigLevels_1(INdxStorage* pNdxStorage)
{
//для индексации по абсолютному номеру предложения
//координата абс. номер предложения
//abs 1/32/3/18/4/10
//word 1/8/1/8
//индексируем только по абс. номеру предложения
SNdxLevelInfo NdxLevelInfo;
	
	if(ISNULL(pNdxStorage)) return;

	NdxLevelInfo.m_IndexCoordinateType=SNdxLevelInfo::ictSentenceAbsNumber;
	NdxLevelInfo.m_eControlType=SNdxLevelInfo::lctUndefined;
	NdxLevelInfo.m_eControlByType=SNdxLevelInfo::lctUndefined;
	
	//----------------------------------------------
	NdxLevelInfo.m_ucLevelNumber=0;
	NdxLevelInfo.m_ucStartBit=31;
	NdxLevelInfo.m_ucBitPerLevel=18;

	pNdxStorage->AddLevelInfo(&NdxLevelInfo);
	//----------------------------------------------

	//----------------------------------------------
	NdxLevelInfo.m_ucLevelNumber++;
	NdxLevelInfo.m_ucStartBit-=NdxLevelInfo.m_ucBitPerLevel;
	NdxLevelInfo.m_ucBitPerLevel=4;

	pNdxStorage->AddLevelInfo(&NdxLevelInfo);
	//----------------------------------------------

	//----------------------------------------------
	NdxLevelInfo.m_ucLevelNumber++;
	NdxLevelInfo.m_ucStartBit-=NdxLevelInfo.m_ucBitPerLevel;
	NdxLevelInfo.m_ucBitPerLevel=10;
	NdxLevelInfo.m_eControlType=SNdxLevelInfo::lctWords;

	pNdxStorage->AddLevelInfo(&NdxLevelInfo);
	//----------------------------------------------
}

void CNdxConfigurator::ConfigLevels_2(INdxStorage* pNdxStorage)
{
//для индексации по номеру текста, номеру предложения и номеру слова
//номер слова входит в последний уровень предложения
//text 1/24/3/18/4/2
//word 1/32/1/32

	SNdxLevelInfo NdxLevelInfo;
	
	if(ISNULL(pNdxStorage)) return;

	NdxLevelInfo.m_IndexCoordinateType=SNdxLevelInfo::ictTextAbsNumber;
	NdxLevelInfo.m_eControlType=SNdxLevelInfo::lctUndefined;
	NdxLevelInfo.m_eControlByType=SNdxLevelInfo::lctUndefined;
	
	//----------------------------------------------
	NdxLevelInfo.m_ucLevelNumber=0;
	NdxLevelInfo.m_ucStartBit=23;
	NdxLevelInfo.m_ucBitPerLevel=18;

	pNdxStorage->AddLevelInfo(&NdxLevelInfo);
	//----------------------------------------------

	//----------------------------------------------
	NdxLevelInfo.m_ucLevelNumber++;
	NdxLevelInfo.m_ucStartBit-=NdxLevelInfo.m_ucBitPerLevel;
	NdxLevelInfo.m_ucBitPerLevel=4;

	pNdxStorage->AddLevelInfo(&NdxLevelInfo);
	//----------------------------------------------

	//----------------------------------------------
	NdxLevelInfo.m_ucLevelNumber++;
	NdxLevelInfo.m_ucStartBit-=NdxLevelInfo.m_ucBitPerLevel;
	NdxLevelInfo.m_ucBitPerLevel=2;
	NdxLevelInfo.m_eControlType=SNdxLevelInfo::lctWords;

	pNdxStorage->AddLevelInfo(&NdxLevelInfo);
	//----------------------------------------------
}

void CNdxConfigurator::ConfigLevels_3(INdxStorage* pNdxStorage)
{
//для индексации по номеру текста, номеру предложения и номеру слова
//номер слова входит в последний уровень предложения
//text 1/24/3/18/4/2

	SNdxLevelInfo NdxLevelInfo;
	
	if(ISNULL(pNdxStorage)) return;

	NdxLevelInfo.m_IndexCoordinateType=SNdxLevelInfo::ictTextAbsNumber;
	NdxLevelInfo.m_eControlType=SNdxLevelInfo::lctUndefined;
	NdxLevelInfo.m_eControlByType=SNdxLevelInfo::lctUndefined;
	
	//----------------------------------------------
	NdxLevelInfo.m_ucLevelNumber=0;
	NdxLevelInfo.m_ucStartBit=23;
	NdxLevelInfo.m_ucBitPerLevel=18;

	pNdxStorage->AddLevelInfo(&NdxLevelInfo);
	//----------------------------------------------

	//----------------------------------------------
	NdxLevelInfo.m_ucLevelNumber++;
	NdxLevelInfo.m_ucStartBit-=NdxLevelInfo.m_ucBitPerLevel;
	NdxLevelInfo.m_ucBitPerLevel=4;

	pNdxStorage->AddLevelInfo(&NdxLevelInfo);
	//----------------------------------------------

	//----------------------------------------------
	NdxLevelInfo.m_ucLevelNumber++;
	NdxLevelInfo.m_ucStartBit-=NdxLevelInfo.m_ucBitPerLevel;
	NdxLevelInfo.m_ucBitPerLevel=2;

	pNdxStorage->AddLevelInfo(&NdxLevelInfo);
	//----------------------------------------------
}

void CNdxConfigurator::ConfigLevels_4(INdxStorage* pNdxStorage)
{
//для индексации по номеру текста, номеру предложения и номеру слова
//номер слова входит в последний уровень предложения
//text 1/24/3/12/6/6
//word 1/32/1/32

	SNdxLevelInfo NdxLevelInfo;
	
	if(ISNULL(pNdxStorage)) return;

	NdxLevelInfo.m_IndexCoordinateType=SNdxLevelInfo::ictTextAbsNumber;
	NdxLevelInfo.m_eControlType=SNdxLevelInfo::lctUndefined;
	NdxLevelInfo.m_eControlByType=SNdxLevelInfo::lctUndefined;
	
	//----------------------------------------------
	NdxLevelInfo.m_ucLevelNumber=0;
	NdxLevelInfo.m_ucStartBit=23;
	NdxLevelInfo.m_ucBitPerLevel=12;

	pNdxStorage->AddLevelInfo(&NdxLevelInfo);
	//----------------------------------------------

	//----------------------------------------------
	NdxLevelInfo.m_ucLevelNumber++;
	NdxLevelInfo.m_ucStartBit-=NdxLevelInfo.m_ucBitPerLevel;
	NdxLevelInfo.m_ucBitPerLevel=6;

	pNdxStorage->AddLevelInfo(&NdxLevelInfo);
	//----------------------------------------------

	//----------------------------------------------
	NdxLevelInfo.m_ucLevelNumber++;
	NdxLevelInfo.m_ucStartBit-=NdxLevelInfo.m_ucBitPerLevel;
	NdxLevelInfo.m_ucBitPerLevel=6;
	NdxLevelInfo.m_eControlType=SNdxLevelInfo::lctWords;

	pNdxStorage->AddLevelInfo(&NdxLevelInfo);
	//----------------------------------------------

}

void CNdxConfigurator::ConfigLevels_5(INdxStorage* pNdxStorage)
{
//для индексации по номеру текста, номеру предложения и номеру слова
//номер слова входит в последний уровень предложения
//text 1/24/3/15/5/4
//word 1/32/1/32

	SNdxLevelInfo NdxLevelInfo;
	
	if(ISNULL(pNdxStorage)) return;

	NdxLevelInfo.m_IndexCoordinateType=SNdxLevelInfo::ictTextAbsNumber;
	NdxLevelInfo.m_eControlType=SNdxLevelInfo::lctUndefined;
	NdxLevelInfo.m_eControlByType=SNdxLevelInfo::lctUndefined;
	
	//----------------------------------------------
	NdxLevelInfo.m_ucLevelNumber=0;
	NdxLevelInfo.m_ucStartBit=23;
	NdxLevelInfo.m_ucBitPerLevel=16;

	pNdxStorage->AddLevelInfo(&NdxLevelInfo);
	//----------------------------------------------

	//----------------------------------------------
	NdxLevelInfo.m_ucLevelNumber++;
	NdxLevelInfo.m_ucStartBit-=NdxLevelInfo.m_ucBitPerLevel;
	NdxLevelInfo.m_ucBitPerLevel=5;

	pNdxStorage->AddLevelInfo(&NdxLevelInfo);
	//----------------------------------------------

	//----------------------------------------------
	NdxLevelInfo.m_ucLevelNumber++;
	NdxLevelInfo.m_ucStartBit-=NdxLevelInfo.m_ucBitPerLevel;
	NdxLevelInfo.m_ucBitPerLevel=3;
	NdxLevelInfo.m_eControlType=SNdxLevelInfo::lctWords;

	pNdxStorage->AddLevelInfo(&NdxLevelInfo);
	//----------------------------------------------

}

void CNdxConfigurator::ConfigLevels_6(INdxStorage* pNdxStorage)
{
//для индексации по номеру текста, номеру предложения и номеру слова
//номер слова входит в последний уровень предложения
//text 1/24/2/12/12
//word 1/32/1/32

	SNdxLevelInfo NdxLevelInfo;
	
	if(ISNULL(pNdxStorage)) return;

	NdxLevelInfo.m_IndexCoordinateType=SNdxLevelInfo::ictTextAbsNumber;
	NdxLevelInfo.m_eControlType=SNdxLevelInfo::lctUndefined;
	NdxLevelInfo.m_eControlByType=SNdxLevelInfo::lctUndefined;
	
	//----------------------------------------------
	NdxLevelInfo.m_ucLevelNumber=0;
	NdxLevelInfo.m_ucStartBit=23;
	NdxLevelInfo.m_ucBitPerLevel=18;

	pNdxStorage->AddLevelInfo(&NdxLevelInfo);
	//----------------------------------------------

	//----------------------------------------------
	NdxLevelInfo.m_ucLevelNumber++;
	NdxLevelInfo.m_ucStartBit-=NdxLevelInfo.m_ucBitPerLevel;
	NdxLevelInfo.m_ucBitPerLevel=6;
	NdxLevelInfo.m_eControlType=SNdxLevelInfo::lctWords;

	pNdxStorage->AddLevelInfo(&NdxLevelInfo);
	//----------------------------------------------

}

//--------------------------------------------------------------------//

}
}
}
}