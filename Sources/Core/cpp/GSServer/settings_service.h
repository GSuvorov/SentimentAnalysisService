#pragma once

#include ".\settings_operator.h"
#include ".\settings_editor.h"

namespace SS
{
namespace CommonServices
{

//----------------------------------------------------------------//

///сервис настроек
class CSettingsService :
	public CSettingsOperator, public CSettingsEditor
{
public:
	//-----------IBase
	///приведение к интерфейсу с поданным REFIID. pBase будет NULL, если данный интерфейс не поддерживается
	HRESULT QueryInterface(REFIID pIID, void** pBase); 
	///освобождение
	ULONG Release(void);
	//-----------Eof-IBase
protected:
	///возвращает загрузчик
	SS::Interface::Core::ResourceManagers::ILoadManager* GetLoader(void){return GetLoadManager();};

};

//----------------------------------------------------------------//

}
}
