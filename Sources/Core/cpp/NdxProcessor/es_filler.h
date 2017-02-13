#pragma once

#include ".\eq_calc.h"
#include ".\eq_block.h"
#include ".\subst.h"
#include ".\console.h"
#include ".\ev_factory.h"

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

class CESFiller : public CFactoryClient<Containers::CContainersFactory>
{
public:
	CESFiller(Containers::CContainersFactory* pCntFactory)	
		:CFactoryClient<Containers::CContainersFactory>(pCntFactory){};
	virtual ~CESFiller(void){};

	///заполнение формулы и подстановщика аргументами и коллекциями значений	
	virtual void Fill(CCalcEquation* pEquation, CSubster* pSubster){TO_CONSOLE(L"Not overriden!");};
	virtual void Fill(CBlockEquation* pEquation, CSubster* pSubster){TO_CONSOLE(L"Not overriden!");};
	///возвращает флаг инициализации
	virtual bool IsInited(void){TO_CONSOLE(L"Not overriden!"); return false;};
	///отображает источник из которого происходит заполнение
	virtual void ViewFillingSource(void){TO_CONSOLE(L"Not overriden!");};
};

//--------------------------------------------------------------------//

///наполнитель - отвечает за наполнение формул и подстановщиков аргументами и коллекциями значений
template <class TFillingSource>
class CESFillerEx : public CESFiller
{
public:
	typedef set<CCalcArgument*> TArgCollectionForSubster;

	CESFillerEx(Containers::CContainersFactory* pCntFactory, TFillingSource* pFillingSource)	
		:CESFiller(pCntFactory), m_pFillingSource(pFillingSource){};
	virtual ~CESFillerEx(void){};

	///заполнение формулы и подстановщика аргументами и коллекциями значений	
	void Fill(CCalcEquation* pEquation, CSubster* pSubster){TO_CONSOLE(L"Not overriden!");};
	///возвращает флаг инициализации
	bool IsInited(void){return (m_pFillingSource)?true:false;};
	///возвращает источник из которого происходит заполнение
	TFillingSource* GetFillingSource(void){return m_pFillingSource;};

protected:
	///заполняем коллекцию для подстановщика
	void AddForSubster(CCalcArgument* pArgument)
	{
		if(ISNULL(pArgument)) return;

		if(pArgument->GetLeftChild() && !pArgument->GetLeftChild()->IsArgumentIntermediate()){
			m_ArgCollectionForSubster.insert(pArgument->GetLeftChild());
		}
		if(pArgument->GetRightChild() && !pArgument->GetRightChild()->IsArgumentIntermediate()){
			m_ArgCollectionForSubster.insert(pArgument->GetRightChild());
		}
	};

	///заполнение подстановщика аргументами и коллекциями значений	
	void FillSubster(CSubster* pSubster)
	{
		if(ISNULL(pSubster)) return;

		//заполянем подстановщик значениями
		for(set<CCalcArgument*>::iterator itArg=m_ArgCollectionForSubster.begin(); itArg!=m_ArgCollectionForSubster.end(); itArg++){
			//ToConsole(L"ArgumentID", (*itArg)->GetMyself()->GetArgumentID());
			FillSubster(pSubster, *itArg);
		}
	};

	///заполнение подстановщика аргументом и коллекциями значений	
	void FillSubster(CSubster* pSubster, CCalcArgument* pSourceArgument)
	{
		if(ISNULL(pSubster)) return;
		if(ISNULL(pSourceArgument)) return;
		//if(ISNULL(m_pSourceIndexesOffsetTable)) return;
		
		if(!pSourceArgument->IsArgumentIntermediate() && pSourceArgument->GetMyself()){
			//если аргумент валидный и не промежуточный 
			unsigned int uiDocumentFrequency=0;
			unsigned int uiEntryPoint=SS::Core::NdxSE::NdxProcessor::EMPTY_VALUE;
			
			//читаем точку входа на коллекцию значений из таблицы смещений основ
			GetFactory()->GetIndexStorage()->GetIndexEntryPointTable()->GetIndexData(
				pSourceArgument->GetMyself()->GetArgumentID(), 
					&uiEntryPoint, &uiDocumentFrequency);
			
			if(uiEntryPoint==EMPTY_VALUE){
				ToConsole(L"EntryPoint is empty, for Index: ", pSourceArgument->GetMyself()->GetArgumentID());
			}

			//добавляем его коллекцию подстановщика
			pSubster->GetArgValCollection()->push_back(
				new CArgValuesPair(pSourceArgument->GetMyself(), 
					GetFactory()->CreateValuesContainer(uiEntryPoint)));
		}
	};

	///источник из которого происходит заполнение
	TFillingSource* m_pFillingSource;
	///коллекция аргументов для подстановщика
	TArgCollectionForSubster m_ArgCollectionForSubster;
};

//--------------------------------------------------------------------//

}
}
}
}
}