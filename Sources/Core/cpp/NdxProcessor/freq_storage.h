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

#pragma pack(push,1)  // Fix packing size 
///информация по частоте вхождения индекса в документ
struct SIndexTF
{
	///значение индекса
	unsigned int m_uiIndex;
	///частота вхождения индекса в документ
	unsigned int m_IndexFrequencyInText;
};
#pragma pack(pop)

//--------------------------------------------------------------------//

///хранилище частот индексов в документах
class CIndexFrequencyStorage
{
public:
	CIndexFrequencyStorage(SS::Interface::Core::NdxSE::NdxDataStorage::INdxVectorStorage* pNdxVectorStorage);
	virtual ~CIndexFrequencyStorage(void);

	///добавляет очередные частоты основ
	void WriteIndexFreqs(vector<SIndexTF>& v_tf);
	///устанваливает что текст закончился
	void SetTextComplete(void);
	///читает информацию по тексту
	void ReadIndexFreqsForText(unsigned int uiTextNumber, map<unsigned int,unsigned int>& mm_tf);

protected:
	///читает очередной буфер с частотами для текущего текста
	unsigned int ReadIndexFreqsBuffer(vector<SIndexTF>& IndexFreqsBuffer);
	
	///физическое векторное хранилище данных
	SS::Interface::Core::NdxSE::NdxDataStorage::INdxVectorStorage* m_pNdxVectorStorage;
};

//--------------------------------------------------------------------//

}
}
}
}
}