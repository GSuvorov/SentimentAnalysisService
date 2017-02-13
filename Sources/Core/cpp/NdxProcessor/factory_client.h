#pragma once

namespace SS
{
namespace Core
{
namespace NdxSE
{
namespace NdxProcessor
{

//--------------------------------------------------------------------//

///клиент фабрики для создания объектов
template <class TFactory>
class CFactoryClient
{
public:
	CFactoryClient(TFactory* pFactory):m_pFactory(pFactory){};

	///возвращает фабрику 
	TFactory* GetFactory(void){return m_pFactory;};

private:
	///порождающая объекты фабрика
	TFactory* m_pFactory;
};

//--------------------------------------------------------------------//

}
}
}
}