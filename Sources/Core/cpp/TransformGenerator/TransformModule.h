#pragma once

#include <vector>
#include <set>
using namespace std;

#include "..\ASCInterface\services.h"
using namespace SS::Core::CommonServices;

#include "..\ASCInterface\IInterface.h"
using namespace SS::Interface::Core::ResourceManagers;

#include "..\ASSInterface\IRev.h"
using namespace SS::Interface::Core::Rev;

#include "..\ASSInterface\IAMConverter.h"
#include "..\ASSInterface\IBlackBox.h"

using namespace SS::Interface::Core::BlackBox;
using namespace SS::Interface::Core::BlackBox::Query;
using namespace SS::Interface::Core::MainAnalyse;
using namespace SS::Interface::Core;
using namespace SS::Core::Types;
using namespace SS::Core;

#include "..\ASSInterface\ICoSeDi.h"
using namespace SS::Interface::Core::CoSeDi;

#include "..\ASSInterface\TCoSeDiDataTypes.h"
using namespace SS::CoSeDi::DataTypes;

#include "WeightTable.h"
#include "TypeChecker.h"


namespace SS
{
namespace TransformGenerator
{

class CTransformModule : public IRelevanceEvaluation
{
public:
	CTransformModule();

public:
	HRESULT QueryInterface(REFIID refIID, void** pBase);
	ULONG Release();
	
	virtual void SetLoadManager(ILoadManager* pLoadManager);
	virtual void SetAnalyseParams(IAnalyseParams* pParams);

public:
	HRESULT EvaluateQuery(IText* pText, IQuery* pQuery);

private:
	/// Выдает трансформации для предложения в зависимости от языка
	/**
		\param pSentence - предложение которое будем трансформировать
		\param pQuery [out] - результат
		\param Lang - язык запроса
	*/
	bool DoTransform(ISentence* pSentence, IQuery* pQuery);
	/// Заполняет множество допустимых синтаксических категорий
	static void InitAcceptibleCategoriesList();
	/// Обпределяет, является ли вопрос вопросом типа "Who is?"
	bool IsWhoIsQuestion(ISentence* pSent);
	bool IsNameDefinition(ISentence* pSent);

	/// Проверяет наличие синтаксической категории юнита в множестве
	bool IsUnitInSet(set<unsigned int>* pst, IUnit* pUnit);

private:
	/// Список трансформаций для английского языка
	static int s_Trans_Eng1[];
	static int s_Trans_Eng2[];
	/// Список трансформаций для русского языка
	static int s_Trans_Rus[];
	static int s_Trans_Undefined[];

	/// Множество допустимых значений синтаксических категорий
	static set<unsigned int> s_sSC;
	/// Синтаксические категории, по которым определяется вопрос типа "Who is?"
	static set<unsigned int> s_sWhoIsSC;

	/// Фабрика классов блэк бокса
	SS::Interface::Core::BlackBox::IBlackBoxQueryStorage* m_pBlackBoxQueryStorage;
	SS::Interface::Core::BlackBox::IBlackBoxTextStorage* m_pBlackBoxTextStorage;

	ISemanticAnalysis* m_pSemanticAnalysis;
	ICombinatory* m_pCombinatory;

	CWeightTable m_oWeightTable;
	CTypeChecker m_oTypeChecker;

	bool m_bLite;
};

}
}
