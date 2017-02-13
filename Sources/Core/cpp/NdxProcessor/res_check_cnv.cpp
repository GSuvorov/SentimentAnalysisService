#include "StdAfx.h"
#include ".\res_check_cnv.h"
#include ".\arg_res_int.h"
#include ".\arg_res_hdr.h"

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

CResultsCheckerWithConvertion::CResultsCheckerWithConvertion(unsigned int uiValidResultsDepth, 
		unsigned int uiFinalResultsMaxNumber, CSearchResultFiller* pSearchResultFiller)
:CResultsChecker(uiValidResultsDepth), 
m_uiFinalResultsMaxNumber(uiFinalResultsMaxNumber),
m_uiFinalResultsCurrentNumber(0),
m_pSearchResultFiller(pSearchResultFiller)
{
}

CResultsCheckerWithConvertion::~CResultsCheckerWithConvertion(void)
{
}

bool CResultsCheckerWithConvertion::CheckResult(CResultArgument* pArgument, CArgument** ppCheckedArgument)
{
	if(ISNULL(pArgument)) return false;
	if(ISNULL(ppCheckedArgument)) return false;

	//проверка значения аргумента:
	//на пустоту 
	//на последовательность
	//на превышение глубины

	//проверяем на счетчик результатов
	if(m_uiFinalResultsCurrentNumber>=m_uiFinalResultsMaxNumber){
		//если превысили лимит результатов говорим об этом
		TO_CONSOLE(L"Results limit exceeded");
		ToConsole(L"Results Number", m_uiFinalResultsCurrentNumber);
		
		//прекращаем генерацию
		(*ppCheckedArgument)=NULL;
		return false;
	}

	if(pArgument->GetMyself()->GetValue()->IsEmpty()){
		//если значение результата пустое он не прошел проверку
		(*ppCheckedArgument)=NULL;
		return true;
	}

	if(pArgument->GetDepth()==m_uiValidResultsDepth && 
		!pArgument->GetMyself()->GetValue()->IsEmpty()){
		//если результат находится на предельной глубине
		//и его значение не пустое, нужно его конвертировать
		if(m_pSearchResultFiller){
			//если можем сразу конвертируем аргумент в линейный результаты

			//создаем временный буфер полной истории
			CResultArgument::THistoryBuffer TempHistoryBuffer(*(pArgument->GetHistoryBuffer()));
			TempHistoryBuffer.push_back(0);
			//запускаем перевод вычисленного результата в линейную координату
			m_pSearchResultFiller->StartAddCoordinate((&TempHistoryBuffer.front()), (unsigned int)TempHistoryBuffer.size());
			
			//получаем веса орт значения результата
			CArgumentEx::TValueOrtsWeightsCollection* pValueOrtsWeightsCollection=pArgument->GetMyself()->GetValueOrtsWeights();
			
			//проходим по ортам значения аргумента и вычисляем линейный результат
			SS::Core::NdxSE::NdxProcessor::Containers::CCompositeValue* pValue=pArgument->GetMyself()->GetValue();
			if(pValue->MoveFirstOrt()){
				unsigned int uiCurrentOrt=0;
				do{
					uiCurrentOrt=pValue->GetCurrentOrt();
					//переводим в линейный результат и добавляем к коллекции
					m_pSearchResultFiller->AddCoordinate(uiCurrentOrt, 
						pValueOrtsWeightsCollection->at(uiCurrentOrt));

					//обновляем текущее количество результатов
					//даже если здесь счетчик превысит предельное число результатов
					//ничего не делаем что при необходимости корректно продолжить вычисления
					//хотя, если нужно точно, можно просто прервать вычисления прямо здесь
					++m_uiFinalResultsCurrentNumber;
				}while(pValue->MoveNextOrt());
			}
		}
	
		//но проверку такой результат не пройдет
		(*ppCheckedArgument)=NULL;
		return true;
	}
	
	//результат прошел проверку, возвращаем его
	(*ppCheckedArgument)=pArgument->GetMyself();
	return true;
}

void CResultsCheckerWithConvertion::View(void)
{
	ToConsole(L"//---CResultsCheckerWithConvertion:");
	
	ToConsole(L"ValidResultsDepth", m_uiValidResultsDepth+1);
	ToConsole(L"FinalResultsNumber", m_uiFinalResultsCurrentNumber);
	ToConsole(L"Max FinalResultsNumber", m_uiFinalResultsMaxNumber);

	ToConsole(L"//---End-of-CResultsCheckerWithConvertion");
}

//--------------------------------------------------------------------//

CResultsCheckerWithIntervalConvertion::CResultsCheckerWithIntervalConvertion(unsigned int uiValidResultsDepth, 
		unsigned int uiFinalResultsMaxNumber, CSearchResultFiller* pSearchResultFiller)
:CResultsCheckerWithConvertion(uiValidResultsDepth, uiFinalResultsMaxNumber, pSearchResultFiller)
{
}

bool CResultsCheckerWithIntervalConvertion::CheckResult(CResultArgument* pArgument, CArgument** ppCheckedArgument)
{
	if(ISNULL(pArgument)) return false;
	if(ISNULL(ppCheckedArgument)) return false;

	//проверка значения аргумента:
	//на пустоту 
	//на последовательность
	//на превышение глубины

	//проверяем на счетчик результатов
	if(m_uiFinalResultsCurrentNumber>=m_uiFinalResultsMaxNumber){
		//если превысили лимит результатов говорим об этом
		TO_CONSOLE(L"Results limit exceeded");
		ToConsole(L"Results Number", m_uiFinalResultsCurrentNumber);
		
		//прекращаем генерацию
		(*ppCheckedArgument)=NULL;
		return false;
	}

	if(pArgument->GetMyself()->GetValue()->IsEmpty()){
		//если значение результата пустое он не прошел проверку
		(*ppCheckedArgument)=NULL;
		return true;
	}

	if(pArgument->GetDepth()==m_uiValidResultsDepth && 
		!pArgument->GetMyself()->GetValue()->IsEmpty()){
		//если результат находится на предельной глубине
		//и его значение не пустое, нужно его конвертировать
		if(m_pSearchResultFiller){
			//если можем сразу конвертируем аргумент в линейный результаты

			//создаем временный буфер полной истории
			CResultArgument::THistoryBuffer TempHistoryBuffer(*(pArgument->GetHistoryBuffer()));
			TempHistoryBuffer.push_back(0);
			//запускаем перевод вычисленного результата в линейную координату
			m_pSearchResultFiller->StartAddCoordinate((&TempHistoryBuffer.front()), (unsigned int)TempHistoryBuffer.size());
			
			//получаем веса орт значения результата
			CArgumentEx::TValueOrtsWeightsCollection* pValueOrtsWeightsCollection=pArgument->GetMyself()->GetValueOrtsWeights();
			
			//получаем коллекцию интервалов результатов
			CResultArgumentWithIntervals* pResultArgumentWithIntervals=static_cast<CResultArgumentWithIntervals*>(pArgument);
			CResultArgumentWithIntervals::TResultIntervalCollection* pResultIntervalCollection=pResultArgumentWithIntervals->GetResultIntervalCollection();
			if(!pResultIntervalCollection || pResultIntervalCollection->empty()){
				//но проверку такой результат не пройдет
				(*ppCheckedArgument)=NULL;
				return true;

			}
			CResultArgumentWithIntervals::TResultIntervalCollection::iterator itResultIntervals=pResultIntervalCollection->begin();

			//проходим по ортам значения аргумента и вычисляем линейный результат
			float fWeight=0.0;
			SS::Core::NdxSE::NdxProcessor::Containers::CCompositeValue* pValue=pArgument->GetMyself()->GetValue();
			if(pValue->MoveFirstOrt()){
				unsigned int uiCurrentOrt=0;
				do{
					uiCurrentOrt=pValue->GetCurrentOrt();
					
					if(uiCurrentOrt<pValueOrtsWeightsCollection->size()){
						fWeight=pValueOrtsWeightsCollection->at(uiCurrentOrt);
					}else{
						fWeight=0.0;
					}

					//переводим в линейный результат и добавляем к коллекции
					bool bResult=m_pSearchResultFiller->AddCoordinate(uiCurrentOrt, 
						fWeight, *itResultIntervals);
					
					//обновляем текущее количество результатов
					//даже если здесь счетчик превысит предельное число результатов
					//ничего не делаем что при необходимости корректно продолжить вычисления
					//хотя, если нужно точно, можно просто прервать вычисления прямо здесь
					++m_uiFinalResultsCurrentNumber;

					//проверяем на счетчик результатов
					if(!bResult){
						//если превысили лимит результатов говорим об этом
						TO_CONSOLE(L"Results limit by intervals exceeded");
						ToConsole(L"Results Number", m_uiFinalResultsCurrentNumber);
						
						//прекращаем генерацию
						(*ppCheckedArgument)=NULL;
						return false;
					}
				}while(pValue->MoveNextOrt() && ++itResultIntervals!=pResultIntervalCollection->end());
			}
		}
	
		//но проверку такой результат не пройдет
		(*ppCheckedArgument)=NULL;
		return true;
	}
	
	//результат прошел проверку, возвращаем его
	(*ppCheckedArgument)=pArgument->GetMyself();
	return true;
}

//--------------------------------------------------------------------//

CResultsCheckerWithHIDConvertion::CResultsCheckerWithHIDConvertion(unsigned int uiValidResultsDepth, 
		unsigned int uiFinalResultsMaxNumber, CSearchResultFiller* pSearchResultFiller)
:CResultsCheckerWithConvertion(uiValidResultsDepth, uiFinalResultsMaxNumber, pSearchResultFiller)
{
}

bool CResultsCheckerWithHIDConvertion::CheckResult(CResultArgument* pArgument, CArgument** ppCheckedArgument)
{
	if(ISNULL(pArgument)) return false;
	if(ISNULL(ppCheckedArgument)) return false;

	//проверка значения аргумента:
	//на пустоту 
	//на последовательность
	//на превышение глубины

	//проверяем на счетчик результатов
	if(m_uiFinalResultsCurrentNumber>=m_uiFinalResultsMaxNumber){
		//если превысили лимит результатов говорим об этом
		TO_CONSOLE(L"Results limit exceeded");
		ToConsole(L"Results Number", m_uiFinalResultsCurrentNumber);
		
		//прекращаем генерацию
		(*ppCheckedArgument)=NULL;
		return false;
	}

	if(pArgument->GetMyself()->GetValue()->IsEmpty()){
		//если значение результата пустое он не прошел проверку
		(*ppCheckedArgument)=NULL;
		return true;
	}

	if(pArgument->GetDepth()==m_uiValidResultsDepth && 
		!pArgument->GetMyself()->GetValue()->IsEmpty()){
		//если результат находится на предельной глубине
		//и его значение не пустое, нужно его конвертировать
		if(m_pSearchResultFiller){
			//если можем сразу конвертируем аргумент в линейный результаты

			//создаем временный буфер полной истории
			CResultArgument::THistoryBuffer TempHistoryBuffer(*(pArgument->GetHistoryBuffer()));
			TempHistoryBuffer.push_back(0);
			//запускаем перевод вычисленного результата в линейную координату
			m_pSearchResultFiller->StartAddCoordinate((&TempHistoryBuffer.front()), (unsigned int)TempHistoryBuffer.size());
			
			//получаем веса орт значения результата
			CArgumentEx::TValueOrtsWeightsCollection* pValueOrtsWeightsCollection=pArgument->GetMyself()->GetValueOrtsWeights();
			
			//получаем коллекцию интервалов результатов
			CResultArgument_HID* pResultArgument_HID=static_cast<CResultArgument_HID*>(pArgument);
			CResultArgument_HID::TBlocksCollection* pResultBlocksCollection=pResultArgument_HID->GetResultBlocksCollection();
			if(!pResultBlocksCollection || pResultBlocksCollection->empty()){
				//но проверку такой результат не пройдет
				(*ppCheckedArgument)=NULL;
				return true;

			}
			CResultArgument_HID::TBlocksCollection::iterator itResultBlocks=pResultBlocksCollection->begin();

			//проходим по ортам значения аргумента и вычисляем линейный результат
			float fWeight=0.0;
			SS::Core::NdxSE::NdxProcessor::Containers::CCompositeValue* pValue=pArgument->GetMyself()->GetValue();
			if(pValue->MoveFirstOrt()){
				unsigned int uiCurrentOrt=0;
				do{
					uiCurrentOrt=pValue->GetCurrentOrt();
					
					if(uiCurrentOrt<pValueOrtsWeightsCollection->size()){
						fWeight=pValueOrtsWeightsCollection->at(uiCurrentOrt);
					}else{
						fWeight=0.0;
					}

					//переводим в линейный результат и добавляем к коллекции
					bool bResult=m_pSearchResultFiller->AddCoordinate(uiCurrentOrt, 
						fWeight, *itResultBlocks);
					
					//обновляем текущее количество результатов
					//даже если здесь счетчик превысит предельное число результатов
					//ничего не делаем что при необходимости корректно продолжить вычисления
					//хотя, если нужно точно, можно просто прервать вычисления прямо здесь
					++m_uiFinalResultsCurrentNumber;

					//проверяем на счетчик результатов
					if(!bResult){
						//если превысили лимит результатов говорим об этом
						TO_CONSOLE(L"Results limit by intervals exceeded");
						ToConsole(L"Results Number", m_uiFinalResultsCurrentNumber);
						
						//прекращаем генерацию
						(*ppCheckedArgument)=NULL;
						return false;
					}
				}while(pValue->MoveNextOrt() && ++itResultBlocks!=pResultBlocksCollection->end());
			}
		}
	
		//но проверку такой результат не пройдет
		(*ppCheckedArgument)=NULL;
		return true;
	}
	
	//результат прошел проверку, возвращаем его
	(*ppCheckedArgument)=pArgument->GetMyself();
	return true;
}

//--------------------------------------------------------------------//

}
}
}
}
}