#pragma once

#include ".\console.h"
#include ".\consoleclient.h"
#include "..\ASDInterface\INdxDataStorage.h"
#include ".\byte_align.h"

namespace SS
{
namespace Core
{
namespace NdxSE
{
namespace NdxProcessor
{
namespace Containers
{

//--------------------------------------------------------------------//

///значение - должно уметь выполянять операции с другими значениями 
class CValue : public CConsoleClient
{
public:
	CValue(void);
	virtual ~CValue(void);

	///выполняет операцию AND
	virtual void MakeAnd(CValue* pValue);
	///выполняет операцию AND со сглаживанием
	virtual void MakeSmoothAnd(CValue* pValue, unsigned int uiAdjacentSize);
	///выполняет операцию OR
	virtual void MakeOr(CValue* pValue);
	///выполняет операцию NOT
	virtual void MakeNot(CValue* pValue){TO_CONSOLE(L"Not ovveriden");};
	///проверяет на пустоту
	virtual bool IsEmpty(void){TO_CONSOLE(L"Not ovveriden"); return true;};
	///сбрасывает значение на пустое
	virtual void Reset(void){TO_CONSOLE(L"Not ovveriden");};

	///возвращает вес значения
	float GetValueWeight(void){return m_fValueWeight;};
	///устанавливает вес значения
	void SetValueWeight(float fValue){m_fValueWeight=fValue;};
	///добавляет к весу значения
	void AddToValueWeight(float fValue){m_fValueWeight+=fValue;};

protected:
	///вес значения
	float m_fValueWeight;
};

//--------------------------------------------------------------------//

///значение - состоящие из нескольких составляющих
class CCompositeValue : public CValue
{
public:
	//Обход выставленных составляющих
	///позиционируется на первой составляющей
	virtual bool MoveFirstOrt(void){TO_CONSOLE(L"Not ovveriden"); return false;};
	///позиционируется на следущей составляющей
	virtual bool MoveNextOrt(void){TO_CONSOLE(L"Not ovveriden"); return false;};
	///позиционируется на последней составляющей
	virtual bool MoveLastOrt(void){TO_CONSOLE(L"Not ovveriden"); return false;};
	///возвращает текущую составляющую
	virtual unsigned int GetCurrentOrt(void){TO_CONSOLE(L"Not ovveriden"); return 0;};

	///возвращает количество составляющих значения
	virtual unsigned int GetOrtsNumber(void){TO_CONSOLE(L"Not ovveriden"); return 0;};
	///возвращает максимально возможное количество составляющих значения
	virtual unsigned int GetOrtsMaxNumber(void){TO_CONSOLE(L"Not ovveriden"); return 0;};
	///устанавливает составляющую
	virtual bool SetOrt(unsigned int uiValue){TO_CONSOLE(L"Not ovveriden"); return false;};
	///сбрасывает составляющую
	virtual bool UnsetOrt(unsigned int uiValue){TO_CONSOLE(L"Not ovveriden"); return false;};
	///проверяет на наличие составляющей
	virtual bool HasOrt(unsigned int uiValue){TO_CONSOLE(L"Not ovveriden"); return false;};
};

//--------------------------------------------------------------------//

///буфер указателей указателей на значения
typedef std::vector<CValue*> TValuePtrsBuffer;
///контайнер - аргументов
class CValuesContainer : public CConsoleClient
{
public:
	using CConsoleClient::ToString;

	CValuesContainer(unsigned int uiEntryPoint);
	virtual ~CValuesContainer(void);
	
	///выставляет значения коллекции результата по последнему использованому значению
	virtual void SetFreshValues(CValue* pPrevValue, TValuePtrsBuffer* pValuePtrsBuffer){TO_CONSOLE(L"Not ovveriden");};
	///освобождает значения по последнему породившему значению
	virtual void FreeValues(CValue* pGenerativeValue){TO_CONSOLE(L"Not ovveriden");};

	///сериализация в строку
	void ToString(wstring& szValue);

protected:
	///точка входа для считывания коллекции значений
	unsigned int m_uiEntryPoint;
};

//--------------------------------------------------------------------//

///интерфейсный класс бинарного контайнера
class CBinaryContainer
{
public:
	virtual ~CBinaryContainer(void){};
	///возвращает бинарный буфер
	virtual unsigned char* GetBinaryBuffer(void){TO_CONSOLE(L"Not ovveriden"); return NULL;};
	///возвращает размер бинарного буфера
	virtual unsigned int GetBinaryBufferSize(void){TO_CONSOLE(L"Not ovveriden"); return 0;};
	///устанавливает новый размер бинарного буфера, буфер очищает
	virtual void InitBinaryBuffer(unsigned int uiNewSize){TO_CONSOLE(L"Not ovveriden");};
	///заполняет бинарный буфер, по исходному буферу
	virtual void FillBinaryBuffer(unsigned char* pBuffer, unsigned int uiBufferSize){TO_CONSOLE(L"Not ovveriden");};
	///консервирует содержимое бинарного буфера, буфер не должен меняться после вызова функции
	virtual void PreserveBinaryBuffer(void){TO_CONSOLE(L"Not ovveriden");};

	///пишет содержимое контайнера в файл
	virtual unsigned int Write(SS::Interface::Core::NdxSE::NdxDataStorage::INdxLevelStorage* pNdxStorage);
	///читает содержимое контайнера из файла
	virtual void Read(SS::Interface::Core::NdxSE::NdxDataStorage::INdxLevelStorage* pNdxStorage);

	///заполняется по буферу, возврщает размер считанного
	virtual unsigned int FromBinaryBuffer(unsigned char* pBuffer, unsigned int uiBufferSize){TO_CONSOLE(L"Not ovveriden");return 0;};
	///пишется в буфер
	virtual void ToBinaryBuffer(CByteAlignCompress::TBytesBuffer* pBuffer){TO_CONSOLE(L"Not ovveriden");};

	///упаковщик ByteAlign
	static CByteAlignCompress* m_pByteAlignCompress;

protected:
	///сжимает 4 байта и пишет в файл
	unsigned int WriteCompressed(SS::Interface::Core::NdxSE::NdxDataStorage::INdxLevelStorage* pNdxStorage, unsigned int uiValue);
	///читает и разжимает в 4 байта из файл
	unsigned int ReadCompressed(SS::Interface::Core::NdxSE::NdxDataStorage::INdxLevelStorage* pNdxStorage);
};

//--------------------------------------------------------------------//

///контайнер бинарного буфера
class CBinaryBufferContainer : public CBinaryContainer 
{
public:
	CBinaryBufferContainer(void);
	virtual ~CBinaryBufferContainer(void);

	///возвращает бинарный буфер
	unsigned char* GetBinaryBuffer(void){return m_pucBuffer;};
	///возвращает размер бинарного буфера
	unsigned int GetBinaryBufferSize(void){return m_uiBufferSize;};
	///устанавливает новый размер бинарного буфера, буфер очищает
	void InitBinaryBuffer(unsigned int uiNewSize);
	///заполняет бинарный буфер, по исходному буферу
	void FillBinaryBuffer(unsigned char* pBuffer, unsigned int uiBufferSize);
	///сбрасывает буфер
	void ResetBinaryBuffer(void);
	///усекает буфер с конца
	void ReduceBackBinaryBuffer(unsigned int uiReducedSize);

protected:
	///буфер с необработанными битовыми картами с позициями
	unsigned char* m_pucBuffer;
	///размер отработанных данных в буфере
	unsigned int m_uiBufferSize;
};

//--------------------------------------------------------------------//

}
}
}
}
}