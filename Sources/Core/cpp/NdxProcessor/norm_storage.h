#pragma once

#include "../ASDInterface/INdxDataStorage.h"

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

///хранилище нормирующих коэффициентов документов, 
///в качестве норм хранятся размеры документов в словах
///на нулевой позиции хранитися общее количество слов в коллекции поэтому чтение осуществляется со сдвигом
class CDocNormStorage
{
public:
	CDocNormStorage(SS::Interface::Core::NdxSE::NdxDataStorage::INdxFile* pNdxDocNormStorage);

	///возвращает норму документа по идентификатору документа
	unsigned int GetDocNorm(unsigned int uiDocId);
	///добавляет норму документа
	void AddDocNorm(unsigned int uiDocNorm);

	///возвращает общее количество слов в коллекции
	unsigned int GetTotalWordsCount(void);

protected:
	///физическое табличное хранилище
	SS::Interface::Core::NdxSE::NdxDataStorage::INdxFile* m_pNdxDocNormStorage;
	///общее количество слов
	unsigned int m_uiTotalWordsCount;
};

//--------------------------------------------------------------------//

}
}
}
}
}