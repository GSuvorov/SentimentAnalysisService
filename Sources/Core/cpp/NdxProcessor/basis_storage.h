#pragma once

namespace SS
{
namespace Core
{
namespace NdxSE
{
namespace NdxProcessor
{
namespace Storages
{

//--------------------------------------------------------------------//

///базовое хранилище
class CBasisStorage
{
public:
	CBasisStorage(void):m_WorkingMode(ewmExtracting){};
	virtual ~CBasisStorage(void){};

	///режимы работы хранилищ
	enum EWorkingMode
	{
		///режим индексации-добавления
		ewmIndexing,
		///режим извлечения-поиска
		ewmExtracting,
	};

	///возвращает режим работы хранилища
	EWorkingMode GetWorkingMode(void){return m_WorkingMode;};
	///устанавливает режим работы хранилища
	void SetWorkingMode(EWorkingMode WorkingMode){m_WorkingMode=WorkingMode;};
	///осуществляет подготовку хранилища индексацию
	virtual void PrepareIndexing(void) = 0;
	///выполняет действия хранилища по окончании индексации
	virtual void CompleteIndexing(void) = 0;
	///выполняет действия хранилища по окончании индексации документа
	virtual void CompleteDocumentIndexing(void) = 0;

protected:
	///режим работы хранилища
	EWorkingMode m_WorkingMode;
};

//--------------------------------------------------------------------//

}
}
}
}
}