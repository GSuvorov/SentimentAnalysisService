#pragma once

#include ".\es_filler.h"
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

///наполнитель - отвечает за наполнение формул и подстановщиков аргументами создан специально под тип строкового запроса
class CESFillerFromString : public CESFillerEx<wstring>
{
public:
	CESFillerFromString(Containers::CContainersFactory* pCntFactory, wstring* pExpression);
	virtual ~CESFillerFromString(void);

	///заполнение формулы и подстановщика аргументами и коллекциями значений	
	void Fill(CCalcEquation* pEquation, CSubster* pSubster);
	///отображает коллекцию LBaseSearchWords
	void ViewFillingSource(void);

protected:
	///вытаскивает токен из строкового выражения начиная с заданной позиции
	unsigned int GetToken(wstring* pszExpression, unsigned int uiCurrPos, wstring* pszToken);
};

//--------------------------------------------------------------------//

}
}
}
}
}