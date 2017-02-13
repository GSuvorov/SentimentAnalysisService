#pragma once

#include ".\es_filler.h"
#include ".\index_storage.h"
#include "..\ASCInterface\ICommonContainers.h"
#include "..\ASDInterface\INDXQuery.h"

namespace SS
{
namespace Core
{
namespace NdxSE
{
namespace NdxProcessor
{
namespace Extracting
{

//--------------------------------------------------------------------//

///наполнитель - отвечает за наполнение формул и подстановщиков аргументами создан специально под тип запроса IQueryResult
class CESFillerFromQuery : 
	public CESFillerEx<SS::Interface::Core::NdxSE::IParametrizedQuery>
{
public:
	CESFillerFromQuery(
		Containers::CContainersFactory* pCntFactory, 
		SS::Interface::Core::NdxSE::IParametrizedQuery* pPrmQueryResult);

	virtual ~CESFillerFromQuery(void);

	///заполнение формулы и подстановщика аргументами и коллекциями значений	
	void Fill(CCalcEquation* pEquation, CSubster* pSubster){};
	void Fill(CBlockEquation* pEquation, CSubster* pSubster);
	///отображает коллекцию LBaseSearchWords
	void ViewFillingSource(void);
protected:
	///добавляет группу аргументов по содержимому блока
	virtual void AddArgumentsGroup(CBlockEquation* pEquation, 
		SS::Interface::Core::NdxSE::IBlock* pBlock, bool bAndBlock);
	///добавляет слово в группу аругментов
	virtual void AddWordToArgumentsGroup(CArgumentsGroup* pArgumentsGroup, 
		SS::Interface::Core::NdxSE::IWord* pWord);
	///добавляет индекс в группу аругментов
	void AddIndexToArgumentsGroup(CArgumentsGroup* pArgumentsGroup, 
									unsigned int uiIndex, unsigned int uiEntryPoint, unsigned int uiIndexFrequency, 
									SS::Interface::Core::CommonContainers::SQueryIndexAttributes QueryIndexAttributes);

	///текущий подстановщик аргументов
	CSubster* m_pSubster;
};

//--------------------------------------------------------------------//

///наполнитель формулы, учитывает статистику индексов при наполнении
class CESFillerFromQuery_Statistic : public CESFillerFromQuery
{
public:
	CESFillerFromQuery_Statistic(Containers::CContainersFactory* pCntFactory, 
		SS::Interface::Core::NdxSE::IParametrizedQuery* pPrmQueryResult) :
			CESFillerFromQuery(pCntFactory, pPrmQueryResult),
			m_bCurrentAndBlock(false)
	{
	};

	///заполнение формулы и подстановщика аргументами и коллекциями значений	
	void Fill(CBlockEquation* pEquation, CSubster* pSubster);

protected:
	typedef list<unsigned int> TIndexCollection;

	///добавляет группу аргументов по содержимому блока
	void AddArgumentsGroup(CBlockEquation* pEquation, 
		SS::Interface::Core::NdxSE::IBlock* pBlock, bool bAndBlock);
	///добавляет слово в группу аругментов
	void AddWordToArgumentsGroup(CArgumentsGroup* pArgumentsGroup, 
		SS::Interface::Core::NdxSE::IWord* pWord);
	
	///коллекция индексов которые нужно добавить в AndBlock
	TIndexCollection m_IndexToAndBlock; 
	bool m_bCurrentAndBlock;
};

//--------------------------------------------------------------------//

}
}
}
}
}