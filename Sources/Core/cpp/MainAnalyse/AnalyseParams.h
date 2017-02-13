#pragma once

#include "../ASSInterface/IMainAnalyse.h"

namespace SS
{
namespace MainAnalyse
{

	/// Хранит и управляет доступ параметрам процесса анализа текста, их измением
	class CAnalyseParams : public SS::Interface::Core::MainAnalyse::IAnalyseParams
	{
	public:
		CAnalyseParams();
		virtual ~CAnalyseParams(){};
	public:

		///Удаление объекта данного класса
		ULONG Release();

		///приведение к интерфейсу с поданным REFIID. pBase будет NULL, если данный интерфейс не поддерживается
		HRESULT QueryInterface(REFIID pIID, void** pBase); 

	};
}
}
