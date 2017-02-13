#pragma once
#include ".\eq_res.h"
#include ".\ev_factory.h"
#include ".\consoleclient.h"
#include ".\res_check.h"
#include ".\index_storage.h"

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
class CResultsContainerWithCorrection;
class CResultsChecker;

///контайнер результатов - хранит создаваемую формулу
class CResultsContainer : public CFactoryClient<CEvalFactory>, public CConsoleClient
{
public:
	friend CResultsContainerWithCorrection;

	CResultsContainer(CEvalFactory* pEvalFactory, unsigned int uiGenerationLimitDepth = 0);
	virtual ~CResultsContainer(void);
	
	///возвращает очередной пустой результат для вычислений
	virtual CResultArgument* GenerateResult(void);
	///проверяет результат на корректность
	virtual bool CheckResult(CResultArgument* pArgument, CArgument** ppCheckedArgument);

	///возвращает проверщик результатов
	CResultsChecker* GetResultsChecker(void){return m_pResultsChecker;};

	///отображает содержимое объекта класса
	void View(void);

protected:
	///создает дочерний результат для входного по орту
	CResultArgument* CreateChildResult(CResultArgument* pParentResult, unsigned int uiOrt);

	///выводимая формула
	CResultEquation* m_pDeriveEquation;
	///проверщик результатов
	CResultsChecker* m_pResultsChecker;
	///последний выданый результат
	CResultArgument* m_pLastResult;
	///глубина ограничивающая порождение результатов
	const unsigned int m_uiGenerationLimitDepth;
};

//--------------------------------------------------------------------//

///контайнер результатов - с корректровкой результов хранит корректирующую формулу
class CResultsContainerWithCorrection : public CResultsContainer
{
public:
	CResultsContainerWithCorrection(CEvalFactory* pEvalFactory, 
		SS::Core::NdxSE::NdxProcessor::Storages::CIndexStorage* pIndexStorage,
		unsigned int uiGenerationLimitDepth = 0, 
		unsigned int uiCorrectionLimitDepth = 0);
	virtual ~CResultsContainerWithCorrection (void);
	
	///добавляет коррекитрующий результат (результат находится в буфере и разбирает согласно структуры индексной базы)
	void AddCorrectionResult(unsigned int* pBuffer, unsigned int uiBufferSize);
	///вытаскивает из другого контайнера
	void CreateCorrectionTemplate(CResultsContainer* pResultsContainer);
	///возвращает очередной пустой результат для вычислений
	CResultArgument* GenerateResult(void);
	///проверяет результат на корректность
	bool CheckResult(CResultArgument* pArgument, CArgument** ppCheckedArgument);

protected:
	///информация по индексированию координат
	SS::Core::NdxSE::NdxProcessor::Storages::CIndexStorage* m_pIndexStorage;
	///глубина до которой действует коррекция
	const unsigned int m_uiCorrectionLimitDepth;
};

//--------------------------------------------------------------------//

///результат хранящийся в стеке результатов
class CResult : public CConsoleClient
{
public:
	typedef vector<unsigned int> TResultCollection;

	CResult(CArgument* pResultArgument);
	virtual ~CResult(void);

	///возвращает текущую коллекцию аргумент-значение 
	unsigned int GetCurrentEvalID(void);
	///переходит на очередную пару аргумент-значение
	bool MoveNextEvalID(void);

	///сериализация в строку
	void ToString(wstring& szValue, wstring szOffset);

protected:
	///сам вычисленный результат
	CArgument* const m_pResultArgument;
	///коллекция коллекций аргумент-значение по идентификаторам
	TResultCollection m_ResultCollection;
	///текущая коллекция аргумент-значение
	TResultCollection::iterator m_itCurrResultValuesCollection;
	///коллекция для инициализации итераторов
	TResultCollection m_NullCollection;
};

//--------------------------------------------------------------------//

///стек результатов
class CResultsStack
{
public:
	typedef list<CResult*> TResultsStack;
	
	CResultsStack(void);
	virtual ~CResultsStack(void);
	
	///добавляет новый результат в конец
	void PushBackResult(CResult* pResult);
	///выбрасывает результат из конца стека
	void PopBackResult(void);
	///возвращает последний результат
	CResult* GetBackResult(void);
	///проверяет на пустоту
	bool IsEmpty(void){return m_ResultsStack.empty();};
	///очищает стек
	void Clear(void);

	///отображает содержимое объекта класса
	void View(void);

protected:
	///список результатов
	TResultsStack m_ResultsStack;
};

//--------------------------------------------------------------------//

}
}
}
}
}