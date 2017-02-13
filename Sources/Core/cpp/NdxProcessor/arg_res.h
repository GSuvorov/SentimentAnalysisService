#pragma once

#include ".\arg.h"
#include ".\node_containers.h"

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

	class CIndexStorage;
}
}
}
}
}

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

///аргумент формулы - узел формулы, обеспечивает рекурсивный процесс вычисления значения формулы
class CResultArgument : public SS::Core::NdxSE::NdxProcessor::Containers::CTreeNode<CArgumentEx*>, public CConsoleClient
{
public:
	typedef vector<unsigned int> THistoryBuffer;

	CResultArgument(unsigned int uiArgumentID = SS::Core::NdxSE::NdxProcessor::EMPTY_VALUE, float fKoeff = 1.0);
	virtual ~CResultArgument(void);

	///добавляет ребенка данному узлу
	CResultArgument* AddChild(CResultArgument* pChildArgument);
	///возвращает родительский узел
	CResultArgument* GetParent(void){return ((CResultArgument*)(SS::Core::NdxSE::NdxProcessor::Containers::CTreeNode<CArgumentEx*>::GetParent()));};
	///возвращает текущего ребенка
	CResultArgument* GetCurrentChild(void){return ((CResultArgument*)(SS::Core::NdxSE::NdxProcessor::Containers::CTreeNode<CArgumentEx*>::GetCurrentChild()));};
	///возвращает ребенка по идентификатору
	CResultArgument* GetChild(unsigned int uiChildId);
	///возвращает буфер истории создания результата
	THistoryBuffer* GetHistoryBuffer(void){return &m_HistoryBuffer;};

	///применяет аргумент к входному
	void Calculate(CArgument* pArgument){TO_CONSOLE(L"Do nothing");};
	///обрезает детей на данной глубине
	void CutByDepth(unsigned int uiDepth);
	///возвращает флаг обработанного результата
	bool IsProcessed(void){return m_bProcessed;};
	///устанавливает флаг обработанного результата
	void SetProcessed(void){m_bProcessed=true;};
	///вычисляет базовый идентификатор по истории результата
	unsigned int CalculateBaseIDFromResultHistory(Storages::CIndexStorage* pIndexStorage);
	


	///сериализация в строку
	void ToString(wstring& szValue, wstring szOffset);
	///отображает содержимое объекта класса
	void View(void);

protected:
	///добавляет ребенка данному узлу
	SS::Core::NdxSE::NdxProcessor::Containers::CTreeNode<CArgumentEx*>* 
		AddChild(unsigned int uiChildID, SS::Core::NdxSE::NdxProcessor::Containers::CTreeNode<CArgumentEx*>* pChildNode){TO_CONSOLE(L"Proxed"); return NULL;};
	///буфер истории создания результата
	THistoryBuffer m_HistoryBuffer;
	///флаг обработанного результата
	bool m_bProcessed;
};

//--------------------------------------------------------------------//

}
}
}
}
}